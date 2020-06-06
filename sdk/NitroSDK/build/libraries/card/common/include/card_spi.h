/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_spi.h

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: card_spi.h,v $
  Revision 1.12  04/10/2006 04:13:21  yosizaki
  add comments.

  Revision 1.11  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.10  08/04/2005 15:02:16  yosizaki
  add CARDi_InitStatusRegister().

  Revision 1.9  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.8  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.7  12/08/2004 12:38:47  yosizaki
  remove CardDeviceInfo.

  Revision 1.6  08/23/2004 10:38:23  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.5  08/18/2004 08:18:54  yosizaki
  change CARDBackupType format.

  Revision 1.4  07/28/2004 13:18:14  yosizaki
  asynchronous mode support.

  Revision 1.3  07/23/2004 08:28:07  yosizaki
  change backup-routine to ARM7 only.

  Revision 1.2  07/19/2004 13:14:23  yosizaki
  add CARDi_ProgramBackupCore.

  Revision 1.1  06/28/2004 01:53:00  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __NITRO_CARD_SPI_H__
#define __NITRO_CARD_SPI_H__


/*****************************************************************************/
/* constant*/

/* for CARDi_Comm()*/
#define	CSPI_CONTINUOUS_ON	0x0040
#define	CSPI_CONTINUOUS_OFF	0x0000

/* Commands*/
#define COMM_WRITE_STATUS	0x01   /* Write status.*/
#define COMM_PROGRAM_PAGE	0x02   /* Page program.*/
#define COMM_READ_ARRAY		0x03   /* Read.*/
#define COMM_WRITE_DISABLE	0x04   /* Disable writing (Not used)*/
#define COMM_READ_STATUS	0x05   /* Read status.*/
#define COMM_WRITE_ENABLE	0x06   /* Enable writing.*/

/* FLASH*/
#define COMM_PAGE_WRITE		0x0A
#define COMM_PAGE_ERASE		0xDB
#define COMM_SECTOR_ERASE	0xD8
#define COMM_CHIP_ERASE		0xC7
#define CARDFLASH_READ_BYTES_FAST	0x0B    /* Not used*/
#define CARDFLASH_DEEP_POWDOWN		0xB9    /* Not used*/
#define CARDFLASH_WAKEUP			0xAB    /* Not used*/

/* Status register bits*/
#define	COMM_STATUS_WIP_BIT		0x01    /* WriteInProgress (bufy)*/
#define	COMM_STATUS_WEL_BIT		0x02    /* WriteEnableLatch*/
#define	COMM_STATUS_BP0_BIT		0x04
#define	COMM_STATUS_BP1_BIT		0x08
#define	COMM_STATUS_BP2_BIT		0x10
#define COMM_STATUS_WPBEN_BIT	0x80


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitStatusRegister

  Description:  For FRAM/EEPROM, adjust status register at launch.
                * Because changes in write protection may occur when the power is turned on when using FRAMs.
                * Because invalid defaults may be present at time of delivery when using EEPROMs.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_InitStatusRegister(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_IdentifyBackupCore

  Description:  Identifies the device type.

  Arguments:    type -- the device type to be identified

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_IdentifyBackupCore(CARDBackupType type);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackupCore

  Description:  All the read commands for the device.

  Arguments:    src -- the origin's device offset
                dst -- the target memory address
                len -- read size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadBackupCore(u32 src, void *dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackupCore

  Description:  all of the program (write-protected) commands for the device

  Arguments:    dst -- the target device offset
                src -- the origin's memory address
                len -- the write size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ProgramBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackupCore

  Description:  all device write (deletion + program) commands

  Arguments:    dst -- the target device offset
                src -- the origin's memory address
                len -- the write size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_WriteBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackupCore

  Description:  all device verify (actual read + comparisons) commands

  Arguments:    dst -- the comparison target device offset
                src -- memory address of comparison source
                len -- comparison size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_VerifyBackupCore(u32 dst, const void *src, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSectorCore

  Description:  all sector deletion commands for the device

  Arguments:    dst -- the deletion target device offset
                len -- deletion size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_EraseBackupSectorCore(u32 dst, u32 len);

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseChipCore

  Description:  all chip deletion commands for the device

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_EraseChipCore(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetWriteProtectCore

  Description:  All the write-protect commands for the device.

  Arguments:    stat -- The protect flag to be set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_SetWriteProtectCore(u16 stat);


#endif  /*__NITRO_CARD_SPI_H__*/
