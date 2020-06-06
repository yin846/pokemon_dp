/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_spi.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: card_spi.c,v $
  Revision 1.27  04/28/2006 07:43:27  yosizaki
  add DeviceCaps.

  Revision 1.26  04/10/2006 04:13:50  yosizaki
  add comments.
  small fix.

  Revision 1.25  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.24  11/30/2005 04:03:17  yosizaki
  fix warning.

  Revision 1.23  11/28/2005 02:10:59  yosizaki
  Fixed timeout check.

  Revision 1.22  10/13/2005 02:06:59  yosizaki
  change WriteStatus.

  Revision 1.21  09/30/2005 01:09:01  yosizaki
  change CARDi_InitStatusRegister().

  Revision 1.20  09/30/2005 00:51:25  yosizaki
  fix about sector-size.
  change spceification of FLASH 4Mb/8Mb.

  Revision 1.19  09/02/2005 07:35:55  yosizaki
  change CARDi_WaitPrevCommand() to verify NO_RESPONSE.

  Revision 1.18  08/04/2005 15:02:08  yosizaki
  add CARDi_InitStatusRegister().

  Revision 1.17  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.16  02/18/2005 11:20:29  yosizaki
  fix around hidden warnings.

  Revision 1.15  01/31/2005 07:23:26  yosizaki
  add FLASH 4Mb/8Mb.

  Revision 1.14  01/18/2005 09:35:48  yosizaki
  fix page size of EEPROM512kb.

  Revision 1.13  01/14/2005 08:49:08  yosizaki
  remove waste WriteEnable before read-command.
  fix addressing-width (from 3 to 2).

  Revision 1.12  12/13/2004 05:07:42  yosizaki
  add implement of FRAM64kb.

  Revision 1.11  12/08/2004 12:41:58  yosizaki
  remove cardi_device and change CARDi_IdentifyBackupCore.

  Revision 1.10  09/10/2004 10:33:26  yosizaki
  set CARDi_WaitPrevCommand timeout 50ms.

  Revision 1.9  09/06/2004 09:20:49  yosizaki
  fix backup access error at 1st time.

  Revision 1.8  09/02/2004 11:54:42  yosizaki
  fix CARD include directives.

  Revision 1.7  08/23/2004 10:38:10  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.6  08/18/2004 08:19:02  yosizaki
  change CARDBackupType format.

  Revision 1.5  07/28/2004 13:18:06  yosizaki
  asynchronous mode support.

  Revision 1.4  07/23/2004 08:28:42  yosizaki
  change backup-routine to ARM7 only.

  Revision 1.3  07/19/2004 13:15:42  yosizaki
  add CARDi_ProgramBackupCore.

  Revision 1.2  07/05/2004 08:37:21  yosizaki
  change CardBackupType to CARDBackupType.

  Revision 1.1  06/28/2004 01:52:48  yosizaki
  (none)

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/card_common.h"
#include "../include/card_spi.h"

/* CARD-SPI control*/


/******************************************************************************/
/* Constant*/

/* The reg_MI_MCCNT0 bits*/

#define MCCNT0_SPI_CLK_MASK	0x0003 /* Baud rate settings mask*/
#define MCCNT0_SPI_CLK_4M	0x0000 /* Baud rate 4.19MHz*/
#define MCCNT0_SPI_CLK_2M	0x0001 /* Baud rate 2.10MHz*/
#define MCCNT0_SPI_CLK_1M	0x0002 /* Baud rate 1.05MHz*/
#define MCCNT0_SPI_CLK_524K	0x0003 /* Baud rate 524kHz*/
#define MCCNT0_SPI_BUSY		0x0080 /* SPI busy flag*/
#define	MMCNT0_SEL_MASK		0x2000 /* CARD ROM / SPI selection mask*/
#define	MMCNT0_SEL_CARD		0x0000 /* CARD ROM selection*/
#define	MMCNT0_SEL_SPI		0x2000 /* CARD SPI selection*/
#define MCCNT0_INT_MASK		0x4000 /* Transfer completion interrupt mask*/
#define MCCNT0_INT_ON		0x4000 /* Enable transfer completion interrupts*/
#define MCCNT0_INT_OFF		0x0000 /* Disable transfer completion interrupts*/
#define MCCNT0_MASTER_MASK	0x8000 /* CARD master mask*/
#define MCCNT0_MASTER_ON	0x8000 /* CARD master ON*/
#define MCCNT0_MASTER_OFF	0x0000 /* CARD master OFF*/


/******************************************************************************/
/* Variables*/

typedef struct
{                                      /* SPI internal management parameter.*/
    u32     rest_comm;                 /* Total remaining number of bytes to send.*/
    u32     src;                       /* Transfer source*/
    u32     dst;                       /* Transfer destination*/
    BOOL    cmp;                       /* Comparison result*/
}
CARDiParam;

static CARDiParam cardi_param;


/******************************************************************************/
/* functions*/

static BOOL CARDi_CommandCheckBusy(void);
static void CARDi_CommArray(const void *src, void *dst, u32 n, void (*func) (CARDiParam *));
static void CARDi_CommReadCore(CARDiParam * p);
static void CARDi_CommWriteCore(CARDiParam * p);
static void CARDi_CommVerifyCore(CARDiParam * p);

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayRead

  Description:  post-read processing for read commands

  Arguments:    dst -- the read-target memory
                len -- read size

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayRead(void *dst, u32 len)
{
    CARDi_CommArray(NULL, dst, len, CARDi_CommReadCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayWrite

  Description:  post-write processing for write commands

  Arguments:    dst -- the write source memory
                len -- the write size

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayWrite(const void *src, u32 len)
{
    CARDi_CommArray(src, NULL, len, CARDi_CommWriteCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArrayVerify

  Description:  post-read comparisons for (comparison) read commands

  Arguments:    src -- the comparison source memory
                len -- comparison size

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_CommArrayVerify(const void *src, u32 len)
{
    CARDi_CommArray(src, NULL, len, CARDi_CommVerifyCore);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EnableSpi

  Description:  Enable CARD-SPI.

  Arguments:    cont -- the continuous transfer flag (CSPI_CONTINUOUS_ON or OFF)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_EnableSpi(u32 cont)
{
    /*
     * If in the future a device with a slow clock speed is introduced,
     * MCCNT0_SPI_CLK_4M is added to the property array. Change is dynamic.
     */
    const u16 ctrl = (u16)(MCCNT0_MASTER_ON |
                           MCCNT0_INT_OFF | MMCNT0_SEL_SPI | MCCNT0_SPI_CLK_4M | cont);
    reg_MI_MCCNT0 = ctrl;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitBusy

  Description:  Waits for CARD-SPI completion.

  Arguments:    None

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARDi_WaitBusy(void)
{
    while ((reg_MI_MCCNT0 & MCCNT0_SPI_BUSY) != 0)
    {
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandBegin

  Description:  Declaration for starting the issuing of commands.

  Arguments:    len -- the length of the command to be issued

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_CommandBegin(int len)
{
    cardi_param.rest_comm = (u32)len;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandEnd

  Description:  Command send completion process

  Arguments:    force_wait		forced wait time (ms)
                timeout		timeout time (ms)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_CommandEnd(u32 force_wait, u32 timeout)
{
    if (force_wait + timeout > 0)
    {
        /*
         * If there is a forced wait, only wait for that time.
         * Unlike what was first talked about, the FLASH wait was also forced.
         */
        if (force_wait > 0)
        {
            OS_Sleep(force_wait);
        }
        if (timeout > 0)
        {
            /*
             * Only the PageWrite command is "forced wait only in the first half".
             * Therefore this kind of special loop will happen.
             */
            int     rest = (int)(timeout - force_wait);
            while (!CARDi_CommandCheckBusy() && (rest > 0))
            {
                int     interval = (rest < 5) ? rest : 5;
                OS_Sleep((u32)interval);
                rest -= interval;
            }
        }
        /*
         * Other commands have waited the designated time.
         * Therefore ReadStatus is good only one time.
         */
        /* If busy at this point, then timeout*/
        if (!CARDi_CommandCheckBusy())
        {
            cardi_common.cmd->result = CARD_RESULT_TIMEOUT;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommandCheckBusy

  Description:  Determines the read status command that indicates whether the device's busy has been cleared.

  Arguments:    None

  Returns:      TRUE when not busy.
 *---------------------------------------------------------------------------*/
static BOOL CARDi_CommandCheckBusy(void)
{
    const u8 buf = COMM_READ_STATUS;
    u8      dst;
    CARDi_CommandBegin(1 + 1);
    CARDi_CommArrayWrite(&buf, 1);
    CARDi_CommArrayRead(&dst, 1);
    CARDi_CommandEnd(0, 0);
    return ((dst & COMM_STATUS_WIP_BIT) == 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitPrevCommand

  Description:  Busy check prior to the issuing of commands.
                Wait for 50ms if a busy state from the prior command exists.
                (this is impossible under normal circumstances as long as the library is installed and the card correctly connected)

  Arguments:    None.

  Returns:      TRUE if the command can be correctly issued.
 *---------------------------------------------------------------------------*/
static BOOL CARDi_WaitPrevCommand(void)
{
    CARDi_CommandEnd(0, 50);
    /* No response if a timeout occurs here*/
    if (cardi_common.cmd->result == CARD_RESULT_TIMEOUT)
    {
        cardi_common.cmd->result = CARD_RESULT_NO_RESPONSE;
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommArray

  Description:  Common processes for the issuing of commands.

  Arguments:    src -- process source memory (NULL if not used)
                dst -- process target memory (NULL if not used)
                len -- process size
                func -- process content

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommArray(const void *src, void *dst, u32 len, void (*func) (CARDiParam *))
{
    CARDiParam *const p = &cardi_param;
    p->src = (u32)src;
    p->dst = (u32)dst;
    CARDi_EnableSpi(CSPI_CONTINUOUS_ON);
    for (; len > 0; --len)
    {
        if (!--p->rest_comm)
        {
            CARDi_EnableSpi(CSPI_CONTINUOUS_OFF);
        }
        CARDi_WaitBusy();
        (*func) (p);
    }
    if (!p->rest_comm)
    {
        reg_MI_MCCNT0 = (u16)(MCCNT0_MASTER_OFF | MCCNT0_INT_OFF);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommReadCore

  Description:  single byte read process

  Arguments:    p -- command parameter

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommReadCore(CARDiParam * p)
{
    reg_MI_MCD0 = 0;
    CARDi_WaitBusy();
    MI_WriteByte((void *)p->dst, (u8)reg_MI_MCD0);
    ++p->dst;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommWriteCore

  Description:  single byte write process

  Arguments:    p -- command parameter

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommWriteCore(CARDiParam * p)
{
    vu16    tmp;
    reg_MI_MCD0 = (u16)MI_ReadByte((void *)p->src);
    ++p->src;
    CARDi_WaitBusy();
    tmp = reg_MI_MCD0;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CommVerifyCore

  Description:  single byte comparison

  Arguments:    p -- command parameter

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_CommVerifyCore(CARDiParam * p)
{
    reg_MI_MCD0 = 0;
    CARDi_WaitBusy();
    /*
     * Read, and disconnect if no match.
     * However, you must get out of continuous transmission.
     * Therefore it must be read at least one more time.
     */
    if ((u8)reg_MI_MCD0 != MI_ReadByte((void *)p->src))
    {
        p->cmp = FALSE;
        if (p->rest_comm > 1)
        {
            p->rest_comm = 1;
        }
    }
    ++p->src;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteEnable

  Description:  enables device write operations (needed before every write-based command)

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_WriteEnable(void)
{
    static const u8 arg[1] = { COMM_WRITE_ENABLE, };
    CARDi_CommandBegin(sizeof(arg));
    CARDi_CommArrayWrite(arg, sizeof(arg));
    CARDi_CommandEnd(0, 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SendSpiAddressingCommand

  Description:  Sets the address specification command settings.

  Arguments:    addr -- the address on the device where searches will take place.
                mode -- the command to be issued

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_SendSpiAddressingCommand(u32 addr, u32 mode)
{
    const u32 width = cardi_common.cmd->spec.addr_width;
    u32     addr_cmd;
    switch (width)
    {
    case 1:
        /* For 4kbit device, [A:8] becomes part of command*/
        addr_cmd = (u32)(mode | ((addr >> 5) & 0x8) | ((addr & 0xFF) << 8));
        break;
    case 2:
        addr_cmd = (u32)(mode | (addr & 0xFF00) | ((addr & 0xFF) << 16));
        break;
    case 3:
        addr_cmd = (u32)(mode |
                         ((addr >> 8) & 0xFF00) | ((addr & 0xFF00) << 8) | ((addr & 0xFF) << 24));
        break;
    default:
        SDK_ASSERT(FALSE);
        break;
    }
    CARDi_CommArrayWrite(&addr_cmd, (u32)(1 + width));
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitStatusRegister

  Description:  For FRAM/EEPROM, adjust status register at launch.
                * Because changes in write protection may occur when the power is turned on when using FRAMs.
                * Because invalid defaults may be present at time of delivery when using EEPROMs.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_InitStatusRegister(void)
{
    /*
     * Appropriately correct for devices which may have invalid initial values in their status registers at the point they are first used.
     * Appropriately correct at first use.
     */
    const u8 stat = cardi_common.cmd->spec.initial_status;
    if (stat != 0xFF)
    {
        static BOOL status_checked = FALSE;
        if (!status_checked)
        {
            u8      cmd = COMM_READ_STATUS;
            u8      dst;
            CARDi_CommandBegin(1 + 1);
            CARDi_CommArrayWrite(&cmd, 1);
            CARDi_CommArrayRead(&dst, 1);
            CARDi_CommandEnd(0, 0);
            if (dst != stat)
            {
                CARDi_SetWriteProtectCore(stat);
            }
            status_checked = TRUE;
        }
    }
}


/********************************************************************/
/*
 * Direct internal process.
 * Exclusions, requests, etc. are all already completed with this step.
 * The limits on the size are assumed to have been taken into consideration.
 */

/*---------------------------------------------------------------------------*
  Name:         CARDi_IdentifyBackupCore

  Description:  Identifies the device type.

  Arguments:    type -- the device type to be identified

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_IdentifyBackupCore(CARDBackupType type)
{
    /*
     * Saves the obtained parameter in CARDiCommandArg
     * This is completed by eliminating the table in the end.
     */
    {
        CARDiCommandArg *const p = cardi_common.cmd;

        /* First, clear all parameters and set to NOT_USE state.*/
        MI_CpuFill8(&p->spec, 0, sizeof(p->spec));
        p->type = type;
        p->spec.caps = CARD_BACKUP_CAPS_AVAILABLE;
        if (type != CARD_BACKUP_TYPE_NOT_USE)
        {
            /*
             * Device type, total capacity and vender can be obtained from type.
             * The vender numbe is 0 other than the case where the same type was adopted by several manufacturers and
             * that there is a need to make a distinction because of problems and such.
             */
            const u32 size = (u32)(1 << ((type >> CARD_BACKUP_TYPE_SIZEBIT_SHIFT) &
                                         CARD_BACKUP_TYPE_SIZEBIT_MASK));
            const int device =
                ((type >> CARD_BACKUP_TYPE_DEVICE_SHIFT) & CARD_BACKUP_TYPE_DEVICE_MASK);
//                      const int vender = ((type >> CARD_BACKUP_TYPE_VENDER_SHIFT) & CARD_BACKUP_TYPE_VENDER_MASK);

            p->spec.total_size = size;
            /* Use 0xFF if the status register does not need to be corrected. (This is usually the case.)*/
            p->spec.initial_status = 0xFF;
            if (device == CARD_BACKUP_TYPE_DEVICE_EEPROM)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x000200:        // CARD_BACKUP_TYPE_EEPROM_4KBITS
                    p->spec.page_size = 0x10;
                    p->spec.addr_width = 1;
                    p->spec.program_page = 5;
                    p->spec.initial_status = 0xF0;
                    break;
                case 0x002000:        // CARD_BACKUP_TYPE_EEPROM_64KBITS
                    p->spec.page_size = 0x0020;
                    p->spec.addr_width = 2;
                    p->spec.program_page = 5;
                    p->spec.initial_status = 0x00;
                    break;
                case 0x010000:        // CARD_BACKUP_TYPE_EEPROM_512KBITS
                    p->spec.page_size = 0x0080;
                    p->spec.addr_width = 2;
                    p->spec.program_page = 10;
                    p->spec.initial_status = 0x00;
                    break;
/*
				case 0x020000:	// CARD_BACKUP_TYPE_EEPROM_1MBITS
					p->spec.page_size = 0x0080;
					p->spec.addr_width = 3;
					p->spec.program_page = 10;
					break;
*/
                }
                p->spec.sect_size = p->spec.page_size;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
            }
            else if (device == CARD_BACKUP_TYPE_DEVICE_FLASH)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x040000:        // CARD_BACKUP_TYPE_FLASH_2MBITS
                case 0x080000:        // CARD_BACKUP_TYPE_FLASH_4MBITS
                case 0x100000:        // CARD_BACKUP_TYPE_FLASH_8MBITS
                    p->spec.write_page = 25;
                    p->spec.write_page_total = 300;
                    p->spec.erase_page = 300;
                    p->spec.erase_sector = 5000;
                    p->spec.caps |= CARD_BACKUP_CAPS_WRITE;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
                    break;
				case 0x200000:	// CARD_BACKUP_TYPE_FLASH_16MBITS
					p->spec.erase_sector = 1000;
					p->spec.erase_sector_total = 3000;
					p->spec.erase_chip = 17000;
					p->spec.erase_chip_total = 40000;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
					break;
				case 0x800000:	// CARD_BACKUP_TYPE_FLASH_64MBITS
					p->spec.erase_sector = 1000;
					p->spec.erase_sector_total = 3000;
					p->spec.erase_chip = 68000;
					p->spec.erase_chip_total = 160000;
                    p->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
					break;
                }
                p->spec.sect_size = 0x010000;
                p->spec.page_size = 0x0100;
                p->spec.addr_width = 3;
                p->spec.program_page = 5;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
                p->spec.caps |= CARD_BACKUP_CAPS_ERASE_SECTOR;
            }
            else if (device == CARD_BACKUP_TYPE_DEVICE_FRAM)
            {
                switch (size)
                {
                default:
                    goto invalid_type;
                case 0x002000:        // #CARD_BACKUP_TYPE_FRAM_64KBITS
                case 0x008000:        // #CARD_BACKUP_TYPE_FRAM_64KBITS
                    break;
                }
                p->spec.page_size = size;
                p->spec.sect_size = size;
                p->spec.addr_width = 2;
                p->spec.initial_status = 0x00;
                p->spec.caps |= CARD_BACKUP_CAPS_READ;
                p->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
                p->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
            }
            else
            {
              invalid_type:
                p->type = CARD_BACKUP_TYPE_NOT_USE;
                p->spec.total_size = 0;
                cardi_common.cmd->result = CARD_RESULT_UNSUPPORTED;
                return;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackupCore

  Description:  All the read commands for the device.

  Arguments:    src -- the origin's device offset
                dst -- the target memory address
                len -- read size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadBackupCore(u32 src, void *dst, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg * const cmd = cardi_common.cmd;
        /* The read process is done all at once, since there is no page boundary*/
        CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + len));
        CARDi_SendSpiAddressingCommand(src, COMM_READ_ARRAY);
        CARDi_CommArrayRead(dst, len);
        CARDi_CommandEnd(0, 0);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackupCore

  Description:  all of the program (write-protected) commands for the device

  Arguments:    dst -- the target device offset
                src -- the origin's memory address
                len -- the write size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ProgramBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg * const cmd = cardi_common.cmd;
        /* Divide up the process so the write does not cross page boundaries*/
        const u32 page = cmd->spec.page_size;
        while (len > 0)
        {
            const u32 mod = (u32)(dst & (page - 1));
            u32     size = page - mod;
            if (size > len)
            {
                size = len;
            }
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + size));
            CARDi_SendSpiAddressingCommand(dst, COMM_PROGRAM_PAGE);
            CARDi_CommArrayWrite(src, size);
            CARDi_CommandEnd(cmd->spec.program_page, 0);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            src = (const void *)((u32)src + size);
            dst += size;
            len -= size;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackupCore

  Description:  all device write (deletion + program) commands

  Arguments:    dst -- the target device offset
                src -- the origin's memory address
                len -- the write size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_WriteBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg * const cmd = cardi_common.cmd;
        /* Divide up the process so the write does not cross page boundaries*/
        const u32 page = cmd->spec.page_size;
        while (len > 0)
        {
            const u32 mod = (u32)(dst & (page - 1));
            u32     size = page - mod;
            if (size > len)
            {
                size = len;
            }
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + size));
            CARDi_SendSpiAddressingCommand(dst, COMM_PAGE_WRITE);
            CARDi_CommArrayWrite(src, size);
            CARDi_CommandEnd(cmd->spec.write_page,
                             cmd->spec.write_page_total);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            src = (const void *)((u32)src + size);
            dst += size;
            len -= size;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackupCore

  Description:  all device verify (actual read + comparisons) commands

  Arguments:    dst -- the comparison target device offset
                src -- memory address of comparison source
                len -- comparison size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_VerifyBackupCore(u32 dst, const void *src, u32 len)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg * const cmd = cardi_common.cmd;
        /* The read process is done all at once, since there is no page boundary*/
        cardi_param.cmp = TRUE;
        CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + len));
        CARDi_SendSpiAddressingCommand(dst, COMM_READ_ARRAY);
        CARDi_CommArrayVerify(src, len);
        CARDi_CommandEnd(0, 0);
        if ((cmd->result == CARD_RESULT_SUCCESS) && !cardi_param.cmp)
        {
            cmd->result = CARD_RESULT_FAILURE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSectorCore

  Description:  all sector deletion commands for the device

  Arguments:    dst -- the deletion target device offset
                len -- deletion size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseBackupSectorCore(u32 dst, u32 len)
{
    CARDiCommandArg * const cmd = cardi_common.cmd;
    const u32 sector = cmd->spec.sect_size;
    /* no processing takes place if the range is not aligned in sector multiples*/
    if (((dst | len) & (sector - 1)) != 0)
    {
        cmd->result = CARD_RESULT_INVALID_PARAM;
    }
    else if (CARDi_WaitPrevCommand())
    {
        /* processed in sector border units*/
        while (len > 0)
        {
            CARDi_WriteEnable();
            CARDi_CommandBegin((int)(1 + cmd->spec.addr_width + 0));
            CARDi_SendSpiAddressingCommand(dst, COMM_SECTOR_ERASE);
            CARDi_CommandEnd(cmd->spec.erase_sector, cmd->spec.erase_sector_total);
            if (cmd->result != CARD_RESULT_SUCCESS)
            {
                break;
            }
            dst += sector;
            len -= sector;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseChipCore

  Description:  all chip deletion commands for the device

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseChipCore(void)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg * const cmd = cardi_common.cmd;
        static const u8 arg[1] = { COMM_CHIP_ERASE, };
        CARDi_WriteEnable();
        CARDi_CommandBegin(sizeof(arg));
        CARDi_CommArrayWrite(arg, sizeof(arg));
        CARDi_CommandEnd(cmd->spec.erase_chip, cmd->spec.erase_chip_total);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetWriteProtectCore

  Description:  All the write-protect commands for the device.

  Arguments:    stat -- The protect flag to be set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_SetWriteProtectCore(u16 stat)
{
    if (CARDi_WaitPrevCommand())
    {
        CARDiCommandArg * const cmd = cardi_common.cmd;
        int     retry_count = 10;
        u8      arg[2];
        arg[0] = COMM_WRITE_STATUS;
        arg[1] = (u8)stat;
        do
        {
            CARDi_WriteEnable();
            CARDi_CommandBegin(1 + 1);
            CARDi_CommArrayWrite(&arg, sizeof(arg));
            CARDi_CommandEnd(5, 0);
        }
        while ((cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));
    }
}


#if	0

/********************************************************************/
/********************************************************************/
/* Device-specific commands now being studied ***********************/
/********************************************************************/
/********************************************************************/


/* ID read*/
static void CARDi_ReadIdCore(void)
{
    /*
     * These commands do not exist in EEPROM or FRAM.
     * Do they also not exist in the ST 2Mbit FLASH?
     * If the operation definitions are missing when incompatible command bytes are sent,
     * these commands always made unusable when 2Mbit, or
     * are they broken up in a manner such as CARD_BACKUP_TYPE_FLASH_2MBITS_SANYO.
     * Whichever it is, even though they are already divided up to that point,
     * would there be a scene that needs an ID?
     * If there is any reserve capacity, we plan to use it internally to determine the validity.
     */
    cardi_common.cmd->result = CARD_RESULT_UNSUPPORTED;
}

/* Command that can only be used with 2M FLASH  *******************************/

/* Erase page (FLASH)*/
static void CARDi_ErasePageCore(u32 dst)
{
    CARDi_WriteEnable();
    CARDi_CommandBegin(1 + cardi_common.cmd->spec.addr_width + 0);
    CARDi_SendSpiAddressingCommand(dst, COMM_PAGE_ERASE);
    CARDi_CommandEnd(cardi_common.cmd->spec.erase_page, 0);
}


#endif
