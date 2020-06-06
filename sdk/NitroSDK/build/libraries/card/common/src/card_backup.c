/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_backup.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro.h>


#include "../include/card_spi.h"
#include "../include/card_common.h"


/*****************************************************************************/
/* function*/

#if defined(SDK_ARM9)

/*
 * Processing to detect application that uses backup.
 * If backup is used intentionally or unintentionally, this character string is always included.
 */
#include <nitro/version_begin.h>
SDK_DEFINE_MIDDLEWARE(cardi_backup_assert, "NINTENDO", "BACKUP");
#include <nitro/version_end.h>
#define SDK_USING_BACKUP() SDK_USING_MIDDLEWARE(cardi_backup_assert)


/* Common configuration processing for stream commands*/
/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestStreamCommandCore

  Description:  Process body of command requests that transfers data.
                Called synchronously and asynchronously.

  Arguments:    p            Library's work buffer (passed by argument for efficiency)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_RequestStreamCommandCore(CARDiCommon * p)
{
    const int req_type = p->req_type;
    const int req_mode = p->req_mode;
    const int retry_count = p->req_retry;
    u32 size = sizeof(p->backup_cache_page_buf);

    SDK_USING_BACKUP();

    /* Request at the page or sector level*/
    if (req_type == CARD_REQ_ERASE_SECTOR_BACKUP)
    {
        size = CARD_GetBackupSectorSize();
    }
    do
    {
        const u32 len = (size < p->len) ? size : p->len;
        p->cmd->len = len;

        /* Stops here if there has been a cancel request*/
        if ((p->flag & CARD_STAT_CANCEL) != 0)
        {
            p->flag &= ~CARD_STAT_CANCEL;
            p->cmd->result = CARD_RESULT_CANCELED;
            break;
        }
        switch (req_mode)
        {
        case CARD_REQUEST_MODE_RECV:
            /* Invalidate the buffer if command is receive-related*/
            DC_InvalidateRange(p->backup_cache_page_buf, len);
            p->cmd->src = (u32)p->src;
            p->cmd->dst = (u32)p->backup_cache_page_buf;
            break;
        case CARD_REQUEST_MODE_SEND:
        case CARD_REQUEST_MODE_SEND_VERIFY:
            /* If command is send-related, copy the data to temporary buffer*/
            MI_CpuCopy8((const void *)p->src, p->backup_cache_page_buf, len);
            DC_FlushRange(p->backup_cache_page_buf, len);
            DC_WaitWriteBufferEmpty();
            p->cmd->src = (u32)p->backup_cache_page_buf;
            p->cmd->dst = (u32)p->dst;
            break;
        case CARD_REQUEST_MODE_SPECIAL:
            /* Buffer operations are unnecessary*/
            p->cmd->src = (u32)p->src;
            p->cmd->dst = (u32)p->dst;
            break;
        }
        /* Send request*/
        if (!CARDi_Request(p, req_type, retry_count))
        {
            break;
        }
        /* If specified, request verify again with same settings*/
        if (req_mode == CARD_REQUEST_MODE_SEND_VERIFY)
        {
            if (!CARDi_Request(p, CARD_REQ_VERIFY_BACKUP, 1))
                break;
        }
        else if (req_mode == CARD_REQUEST_MODE_RECV)
        {
            /* Copy from cache*/
            MI_CpuCopy8(p->backup_cache_page_buf, (void *)p->dst, len);
        }
        p->src += len;
        p->dst += len;
        p->len -= len;
    }
    while (p->len > 0);
    CARDi_EndTask(p, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestStreamCommand

  Description:  Issues request for command to transfer data.

  Arguments:    src        Transfer origin offset or memory address
                dst        Transfer destination offset or memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE
                req_type   Command request type
                req_retry  Max. no. of retries when command request fails
                req_mode   Command request operation mode

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARDi_RequestStreamCommand(u32 src, u32 dst, u32 len,
                                MIDmaCallback callback, void *arg, BOOL is_async,
                                CARDRequest req_type, int req_retry, CARDRequestMode req_mode)
{
    CARDiCommon *const p = &cardi_common;

    SDK_USING_BACKUP();

    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERT(CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_NOT_USE);
    SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_BACKUP,
                  "[CARD] current locking target is not backup.");

    CARDi_WaitTask(p, callback, arg);
    p->src = src;
    p->dst = dst;
    p->len = len;
    p->req_type = req_type;
    p->req_retry = req_retry;
    p->req_mode = req_mode;
    if (is_async)
    {
        CARDi_SetTask(CARDi_RequestStreamCommandCore);
        return TRUE;
    }
    else
    {
        cardi_common.cur_th = OS_GetCurrentThread();
        CARDi_RequestStreamCommandCore(p);
        return (p->cmd->result == CARD_RESULT_SUCCESS);
    }
}


/*---------------------------------------------------------------------------*
  Name:         CARD_GetCurrentBackupType

  Description:  get type of current specified backup device.

  Arguments:    None.

  Returns:      type of current backup device.
 *---------------------------------------------------------------------------*/
CARDBackupType CARD_GetCurrentBackupType(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->type;
}


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupTotalSize

  Description:  get total size of backup device.

  Arguments:    None.

  Returns:      total size of backup device.
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupTotalSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.total_size;
}


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupSectorSize

  Description:  get sector size of backup device.

  Arguments:    None.

  Returns:      sector size of backup device.
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupSectorSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.sect_size;
}


/*---------------------------------------------------------------------------*
  Name:         CARD_GetBackupPageSize

  Description:  get page size of backup device.

  Arguments:    None.

  Returns:      page size of backup device.
 *---------------------------------------------------------------------------*/
u32 CARD_GetBackupPageSize(void)
{
    SDK_ASSERT(CARD_IsAvailable());

    return cardi_common.cmd->spec.page_size;
}


/*---------------------------------------------------------------------------*
  Name:         CARD_IdentifyBackup

  Description:  identify the type of backup device on NITRO-CARD.

  Arguments:    type of backup device on NITRO-CARD.

  Returns:      if the reading test operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARD_IdentifyBackup(CARDBackupType type)
{
    CARDiCommon *const p = &cardi_common;

    /*
     * Because it will interfere with static determination of backup access introduced in 2.2PR and later,
     * it is forbidden to explicitly specify CARD_BACKUP_TYPE_NOT_USE.
     */
    SDK_USING_BACKUP();
    if (type == CARD_BACKUP_TYPE_NOT_USE)
    {
        OS_TPanic("cannot specify CARD_BACKUP_TYPE_NOT_USE.");
    }

    SDK_ASSERT(CARD_IsAvailable());
    /* After 2.1PR, validity determination is also made to be performed and bus lock became required.*/
    SDK_ASSERTMSG((CARDi_GetTargetMode() == CARD_TARGET_BACKUP),
                  "CARD_IdentifyBackup() must be called with CARD locked by CARD_LockBackup()!");

    /* Validity check of CARD access
       If access is prohibited, use OS_Panic in the function*/
    CARD_CheckEnabled();

    /*
     * Wait for the task and send the device type to ARM7.
     * This does not operate the device directly, so it was not necessary to lock in advance, but
     * after 2.1PR, validity determination was also made to be performed and the bus lock become required.
     */
    CARDi_WaitTask(p, NULL, NULL);
    CARDi_IdentifyBackupCore(type);
    cardi_common.cur_th = OS_GetCurrentThread();
    (void)CARDi_Request(p, CARD_REQ_IDENTIFY, 1);
    /*
     * Issue a read command for the first byte and get the result value.
     * If there is a contact problem, damage, or lifespan problem, returns TIMEOUT, regardless of the value.
     * (TIMEOUT can be identified using a Read-Status command regardless of the device type.)
     */
    p->cmd->src = 0;
    p->cmd->dst = (u32)p->backup_cache_page_buf;
    p->cmd->len = 1;
    (void)CARDi_Request(p, CARD_REQ_READ_BACKUP, 1);
    CARDi_EndTask(p, TRUE);
    return (p->cmd->result == CARD_RESULT_SUCCESS);
}


/*---------------------------------------------------------------------------*
  Name:         CARD_WaitBackupAsync

  Description:  wait for completion of current asynchronous backup operation.

  Arguments:    None.

  Returns:      if the newest operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARD_WaitBackupAsync(void)
{
    return CARDi_WaitAsync();
}


/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitBackupAsync

  Description:  check if asynchronous backup operation has been completed.

  Arguments:    None.

  Returns:      if operation has been completed, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARD_TryWaitBackupAsync(void)
{
    return CARDi_TryWaitAsync();
}

/*---------------------------------------------------------------------------*
  Name:         CARD_CancelBackupAsync

  Description:  request to cancel current asynchronous backup operation.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_CancelBackupAsync(void)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    cardi_common.flag |= CARD_STAT_CANCEL;
    (void)OS_RestoreInterrupts(bak_cpsr);
}



#endif /* SDK_ARM9*/


/*---------------------------------------------------------------------------*

  $Log: card_backup.c,v $
  Revision 1.27  2006/05/01 12:18:31  yosizaki
  fix about CARD_REQ_ERASE_SECTOR_BACKUP.

  Revision 1.26  04/10/2006 04:15:16  yosizaki
  small fix for CARD_REQUEST_MODE_SPECIAL.

  Revision 1.25  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.24  09/02/2005 07:35:24  yosizaki
  small fix.

  Revision 1.23  09/02/2005 07:04:38  yosizaki
  fix comment.

  Revision 1.22  09/02/2005 07:03:03  yosizaki
  add CARD_CancelBackupAsync()

  Revision 1.21  06/20/2005 04:47:08  yosizaki
  add assertion in CARD_IdentifyBackup().

  Revision 1.20  06/10/2005 10:16:39  yosizaki
  add a string which is embedded when using backup.

  Revision 1.19  05/12/2005 11:19:42  yosizaki
  optimize some functions and calling interfaces.
  add comment.

  Revision 1.18  04/05/2005 07:24:03  yosizaki
  add check in CARD_IdentifyBackup().

  Revision 1.17  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.16  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.15  12/15/2004 09:44:45  yasu
  Added CARD access enabler mechanism

  Revision 1.14  12/08/2004 12:40:26  yosizaki
  add comments.
  remove cardi_device.

  Revision 1.13  12/07/2004 11:05:37  yosizaki
  add comment.

  Revision 1.12  11/12/2004 05:46:01  yosizaki
  add CARDi_CheckValidity().

  Revision 1.11  11/02/2004 10:04:12  yosizaki
  fix comment.

  Revision 1.10  09/10/2004 00:00:16  yosizaki
  add assertion checks.

  Revision 1.9  09/02/2004 11:54:42  yosizaki
  fix CARD include directives.

  Revision 1.8  08/25/2004 23:57:49  yosizaki
  fix CARD_IdentifyBackup. (set callback NULL)

  Revision 1.7  08/23/2004 10:38:10  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.6  08/18/2004 13:42:05  yosizaki
  add FLASH 2Mbit.

  Revision 1.5  08/18/2004 12:11:37  yosizaki
  small fix (cardi_device->type)

  Revision 1.4  08/18/2004 08:19:02  yosizaki
  change CARDBackupType format.

  Revision 1.3  07/28/2004 13:18:06  yosizaki
  asynchronous mode support.

  Revision 1.2  07/23/2004 08:28:46  yosizaki
  change backup-routine to ARM7 only.

  Revision 1.1  07/19/2004 13:14:50  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
