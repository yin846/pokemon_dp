/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_request.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#if defined(SDK_ARM9)
/*****************************************************************************/
/* code for ARM9 only*/


#include <nitro.h>

#include "../include/card_common.h"
#include "../include/card_spi.h"


/******************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         CARDi_OnFifoRecv

  Description:  PXI FIFO word receive callback

  Arguments:    tag        PXI  tag (always PXI_FIFO_TAG_FS)
                data       Receive data
                err        Error bit (according to old specs)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(data)
    if ((tag == PXI_FIFO_TAG_FS) && err)
    {
        CARDiCommon *const p = &cardi_common;
        /* Receive reply from ARM7 and notify of completion*/
        SDK_ASSERT(data == CARD_REQ_ACK);
        p->flag &= ~CARD_STAT_REQ;
        OS_WakeupThreadDirect(p->cur_th);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TaskThread

  Description:  Main function for task thread

  Arguments:    arg          Not used

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_TaskThread(void *arg)
{
    CARDiCommon *const p = &cardi_common;
    (void)arg;

    for (;;)
    {
        /* Wait for the next process*/
        OSIntrMode bak_psr = OS_DisableInterrupts();
        while ((p->flag & CARD_STAT_TASK) == 0)
        {
            p->cur_th = p->thread;
            OS_SleepThread(NULL);
        }
        (void)OS_RestoreInterrupts(bak_psr);
        (*p->task_func) (p);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_Request

  Description:  Sends request from ARM9 to ARM7 and blocks completion
                If the result is not CARD_RESULT_SUCCESS, retries the specified number of times.
                (Locking of the specified bus and exclusive control of the task thread
                 is guaranteed by the function that called this function.)

  Arguments:    p            Library's work buffer (passed by argument for efficiency)
                req_type     Command request type
                retry_max    Max. number of times to retry

  Returns:      If the result is CARD_RESULT_SUCCESS, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARDi_Request(CARDiCommon * p, int req_type, int retry_count)
{
    /* Execute here if PXI not initialized*/
    if ((p->flag & CARD_STAT_INIT_CMD) == 0)
    {
        p->flag |= CARD_STAT_INIT_CMD;
        while (!PXI_IsCallbackReady(PXI_FIFO_TAG_FS, PXI_PROC_ARM7))
        {
            OS_SpinWait(100);
        }
        /* Send the first command "INIT" (recursion)*/
        (void)CARDi_Request(p, CARD_REQ_INIT, 1);
    }
    /* Flush the shared memory that has been set*/
    DC_FlushRange(p->cmd, sizeof(*p->cmd));
    DC_WaitWriteBufferEmpty();

    do
    {
        /* Send command request*/
        p->command = req_type;
        p->flag |= CARD_STAT_REQ;
        CARDi_SendPxi((u32)req_type);
        /* If there are more arguments, perform additional sends*/
        switch (req_type)
        {
        case CARD_REQ_INIT:
            CARDi_SendPxi((u32)p->cmd);
            break;
        }
        {
            /* Wait for commands to end*/
            OSIntrMode bak_psr = OS_DisableInterrupts();
            while ((p->flag & CARD_STAT_REQ) != 0)
            {
                OS_SleepThread(NULL);
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }
        DC_InvalidateRange(p->cmd, sizeof(*p->cmd));
        /* If timeout, retry the specified number of times*/
    }
    while ((p->cmd->result == CARD_RESULT_TIMEOUT) && (--retry_count > 0));

    /* Return success or failure*/
    return (p->cmd->result == CARD_RESULT_SUCCESS);
}


/******************************************************************************/
#endif /* defined(SDK_ARM9)*/


/*---------------------------------------------------------------------------*
  $Log: card_request.c,v $
  Revision 1.2  11/28/2005 01:17:26  yosizaki
  fix about cache operation.

  Revision 1.1  05/12/2005 11:15:23  yosizaki
  moved from ../common/src

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*

  [old log (build/libraries/card/common/src/card_request.c)]

  Revision 1.9  2005/02/28 05:26:02  yosizaki
  do-indent.

  Revision 1.8  02/18/2005 11:20:41  yosizaki
  fix around hidden warnings.

  Revision 1.7  11/15/2004 00:54:38  yosizaki
  move ARM7 implementations to ../arm7/src/card_command.c

  Revision 1.6  09/02/2004 11:54:42  yosizaki
  fix CARD include directives.

  Revision 1.5  08/23/2004 10:38:10  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.4  08/18/2004 08:19:02  yosizaki
  change CARDBackupType format.

  Revision 1.3  08/18/2004 03:24:14  yosizaki
  fix around PXI-INIT command.

  Revision 1.2  07/28/2004 13:18:06  yosizaki
  asynchronous mode support.

  Revision 1.1  07/23/2004 08:29:16  yosizaki
  (none)

 *---------------------------------------------------------------------------*/
