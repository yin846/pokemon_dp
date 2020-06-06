/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - include
  File:     common.h

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_CARD_COMMON_H_)
#define NITRO_CARD_COMMON_H_


#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/mi/dma.h>
#include <nitro/os.h>


/*****************************************************************************/
/* constant*/

/* API result code*/
typedef enum
{
    CARD_RESULT_SUCCESS = 0,
    CARD_RESULT_FAILURE,
    CARD_RESULT_INVALID_PARAM,
    CARD_RESULT_UNSUPPORTED,
    CARD_RESULT_TIMEOUT,
    CARD_RESULT_ERROR,
    CARD_RESULT_NO_RESPONSE,
    CARD_RESULT_CANCELED
}
CARDResult;

/* for CARD_IdentifyBackup()*/
#define	CARD_BACKUP_TYPE_DEVICE_SHIFT	0
#define	CARD_BACKUP_TYPE_DEVICE_MASK	0xFF
#define	CARD_BACKUP_TYPE_DEVICE_EEPROM	1
#define	CARD_BACKUP_TYPE_DEVICE_FLASH	2
#define	CARD_BACKUP_TYPE_DEVICE_FRAM	3
#define	CARD_BACKUP_TYPE_SIZEBIT_SHIFT	8
#define	CARD_BACKUP_TYPE_SIZEBIT_MASK	0xFF
#define	CARD_BACKUP_TYPE_VENDER_SHIFT	16
#define	CARD_BACKUP_TYPE_VENDER_MASK	0xFF
#define	CARD_BACKUP_TYPE_DEFINE(type, size, vender)	\
	(((CARD_BACKUP_TYPE_DEVICE_ ## type) << CARD_BACKUP_TYPE_DEVICE_SHIFT) |	\
	((size) << CARD_BACKUP_TYPE_SIZEBIT_SHIFT) |	\
	((vender) << CARD_BACKUP_TYPE_VENDER_SHIFT))

/* Backup device type*/
typedef enum
{
    CARD_BACKUP_TYPE_EEPROM_4KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 9, 0),
    CARD_BACKUP_TYPE_EEPROM_64KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 13, 0),
    CARD_BACKUP_TYPE_EEPROM_512KBITS = CARD_BACKUP_TYPE_DEFINE(EEPROM, 16, 0),
    CARD_BACKUP_TYPE_FLASH_2MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 18, 0),
    CARD_BACKUP_TYPE_FLASH_4MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 19, 0),
    CARD_BACKUP_TYPE_FLASH_8MBITS = CARD_BACKUP_TYPE_DEFINE(FLASH, 20, 0),
    CARD_BACKUP_TYPE_FRAM_256KBITS = CARD_BACKUP_TYPE_DEFINE(FRAM, 15, 0),
    CARD_BACKUP_TYPE_NOT_USE = 0
}
CARDBackupType;

/* Card thread's default priority level*/
#define	CARD_THREAD_PRIORITY_DEFAULT	4


/* PXI command request (internal use) to components and ensata emulator*/
typedef enum
{
    CARD_REQ_INIT = 0,                 /* initialize (setting from ARM9)*/
    CARD_REQ_ACK,                      /* request done (acknowledge from ARM7)*/
    CARD_REQ_IDENTIFY,                 /* CARD_IdentifyBackup*/
    CARD_REQ_READ_ID,                  /* CARD_ReadRomID (TEG && ARM9)*/
    CARD_REQ_READ_ROM,                 /* CARD_ReadRom (TEG && ARM9)*/
    CARD_REQ_WRITE_ROM,                /* (reserved)*/
    CARD_REQ_READ_BACKUP,              /* CARD_ReadBackup*/
    CARD_REQ_WRITE_BACKUP,             /* CARD_WriteBackup*/
    CARD_REQ_PROGRAM_BACKUP,           /* CARD_ProgramBackup*/
    CARD_REQ_VERIFY_BACKUP,            /* CARD_VerifyBackup*/
    CARD_REQ_ERASE_PAGE_BACKUP,        /* CARD_EraseBackupPage*/
    CARD_REQ_ERASE_SECTOR_BACKUP,      /* CARD_EraseBackupSector*/
    CARD_REQ_ERASE_CHIP_BACKUP,        /* CARD_EraseBackupChip*/
    CARD_REQ_MAX
}
CARDRequest;

/* Command request operation type (internal use)*/
typedef enum
{
    CARD_REQUEST_MODE_RECV,            /* Data reception*/
    CARD_REQUEST_MODE_SEND,            /* Send data (Including single verify)*/
    CARD_REQUEST_MODE_SEND_VERIFY,     /* Send data + verify*/
    CARD_REQUEST_MODE_SPECIAL          /* special operations like sector deletion*/
}
CARDRequestMode;

/* Maximum number of retries*/
#define	CARD_RETRY_COUNT_MAX	10

// PXI protocol definition
#define CARD_PXI_COMMAND_MASK              0x0000003f   // command part
#define CARD_PXI_COMMAND_SHIFT             0
#define CARD_PXI_COMMAND_PARAM_MASK        0x01ffffc0   // parameter part
#define CARD_PXI_COMMAND_PARAM_SHIFT       6

//---- PXI command
#define CARD_PXI_COMMAND_TERMINATE         0x0001       // arm9->arm7 terminate command
#define CARD_PXI_COMMAND_PULLED_OUT        0x0011       // arm7->arm9 pulled out message


/*****************************************************************************/
/* function*/


#if	defined(__cplusplus)
extern  "C"
{
#endif


/* card-system settings ******************************************************/


/*---------------------------------------------------------------------------*
  Name:         CARD_Init

  Description:  initialize CARD library.
                this function is called in OS_Init().

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    CARD_Init(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_IsAvailable

  Description:  check if CARD library is ready

  Arguments:    None.

  Returns:      If CARD library is ready, True.
 *---------------------------------------------------------------------------*/
    BOOL    CARD_IsAvailable(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_IsEnabled

  Description:  check if CARD is accessible

  Arguments:    None.

  Returns:      Return card access permission.
 *---------------------------------------------------------------------------*/
    BOOL    CARD_IsEnabled(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_CheckEnabled

  Description:  Terminate program if CARD is not accessible

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    CARD_CheckEnabled(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_Enable

  Description:  Set card access permission mode.

  Arguments:    enable       permission mode to be set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    CARD_Enable(BOOL enable);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetThreadPriority

  Description:  get current priority of internal thread which deal with
                asynchronous CARD-ROM tasks.

  Arguments:    None.

  Returns:      priority of internal thread.
 *---------------------------------------------------------------------------*/
    u32     CARD_GetThreadPriority(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_SetThreadPriority

  Description:  set priority of internal thread which deal with
                asynchronous CARD-ROM tasks.

  Arguments:    None.

  Returns:      priority of internal thread.
 *---------------------------------------------------------------------------*/
    u32     CARD_SetThreadPriority(u32 prior);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetResultCode

  Description:  get result code of latest command.

  Arguments:    None.

  Returns:      result code of latest command.
 *---------------------------------------------------------------------------*/
    CARDResult CARD_GetResultCode(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetRomHeader

  Description:  get data buffer which contains own ROM header information.
                if wireless-downloaded program call this function,
                the data is not equal to HW_ROM_HEADER_BUF's.

  Arguments:    None.

  Returns:      pointer of CARD-ROM header information buffer.
 *---------------------------------------------------------------------------*/
    const u8 *CARD_GetRomHeader(void);


/* backup settings ***********************************************************/


/*---------------------------------------------------------------------------*
  Name:         CARD_IdentifyBackup

  Description:  identify the type of backup device on NITRO-CARD.

  Arguments:    type of backup device on NITRO-CARD.

  Returns:      if the reading test operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
    BOOL    CARD_IdentifyBackup(CARDBackupType type);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupTotalSize

  Description:  get total size of backup device.

  Arguments:    None.

  Returns:      total size of backup device.
 *---------------------------------------------------------------------------*/
    u32     CARD_GetBackupTotalSize(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupSectorSize

  Description:  get sector size of backup device.

  Arguments:    None.

  Returns:      sector size of backup device.
 *---------------------------------------------------------------------------*/
    u32     CARD_GetBackupSectorSize(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupPageSize

  Description:  get page size of backup device.

  Arguments:    None.

  Returns:      page size of backup device.
 *---------------------------------------------------------------------------*/
    u32     CARD_GetBackupPageSize(void);


/*---------------------------------------------------------------------------*
  Name:         CARD_GetCurrentBackupType

  Description:  get type of current specified backup device.

  Arguments:    None.

  Returns:      type of current backup device.
 *---------------------------------------------------------------------------*/
    CARDBackupType CARD_GetCurrentBackupType(void);


#if	defined(__cplusplus)
}                                      /* extern "C"*/
#endif


/* NITRO_CARD_COMMON_H_*/
#endif

/*---------------------------------------------------------------------------*

  $Log: common.h,v $
  Revision 1.23  04/10/2006 04:12:00  yosizaki
  add card request mode.

  Revision 1.22  04/03/2006 01:58:22  yosizaki
  add CARD_BACKUP_TYPE_FLASH_8MBITS.

  Revision 1.21  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.20  10/20/2005 05:10:56  yosizaki
  add CARD_BACKUP_TYPE_FLASH_4MBITS.

  Revision 1.19  09/02/2005 07:15:14  yosizaki
  add CARDResult codes.

  Revision 1.18  07/05/2005 00:19:39  yosizaki
  add FRAM.

  Revision 1.17  06/15/2005 09:33:27  yosizaki
  add CARD_BACKUP_TYPE_EEPROM_512KBITS.

  Revision 1.16  05/12/2005 11:21:07  yosizaki
  add some internal parameters.

  Revision 1.15  04/05/2005 07:21:55  yosizaki
  add comment about CARD_RESULT_TIMEOUT.

  Revision 1.14  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.13  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.12  12/15/2004 09:52:44 AM  yasu
  Added CARD access enabler mechanism

  Revision 1.11  12/08/2004 12:15:52  yosizaki
  remove CARD_BACKUP_TYPE_EEPROM_512KBITS.

  Revision 1.10  12/07/2004 10:52:52  yosizaki
  add CARD_BACKUP_TYPE_EEPROM_512KBITS.
  add comments.

  Revision 1.9  10/08/2004 14:31:52  yosizaki
  add CARD_GetRomHeader().

  Revision 1.8  09/27/2004 13:40:49  yada
  add some definitions

  Revision 1.7  09/09/2004 07:11:21  yosizaki
  delete CARD_BACKUP_TYPE_EEPROM_ 512k, 1M.

  Revision 1.6  08/18/2004 13:40:49  yosizaki
  add FLASH 2Mbit.

  Revision 1.5  08/18/2004 08:19:19  yosizaki
  change CARDBackupType format.

  Revision 1.4  07/28/2004 13:18:29  yosizaki
  asynchronous mode support.

  Revision 1.3  07/23/2004 08:31:58  yosizaki
  add new function.

  Revision 1.2  07/19/2004 13:10:34  yosizaki
  add task thread.

  Revision 1.1  07/05/2004 08:10:48  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
