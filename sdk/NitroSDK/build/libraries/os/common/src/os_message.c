/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_message.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_message.c,v $
  Revision 1.14  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.13  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.12  11/12/2004 11:01:19  yada
  just fix comment

  Revision 1.11  03/12/2004 12:10:22  yada
  only fix comment

  Revision 1.10  03/12/2004 08:41:14  yada
  changed name from OS_PeepMessage() -> OS_ReadMessage()

  Revision 1.9  03/12/2004 02:38:51  yada
  added OS_PeepMessage()

  Revision 1.8  03/01/2004 11:56:47  yada
  Creation of function comments in English.

  Revision 1.7  02/12/2004 11:08:30  yada
  Worked on distribution of processing for ARM9/ARM7.

  Revision 1.6  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.5  01/18/2004 02:25:37  yada
  Reformed indents, etc.

  Revision 1.4  12/25/2003 07:31:26  yada
  Changes as per unified type rules

  Revision 1.3  12/11/2003 11:53:36  yada
  modified by change of name   OS_CreateThreadQueue() -> OS_InitThreadQueue()

  Revision 1.2  2003/11/30 04:19:25  yasu
  Changed the type of the OS_EnableInterrupt/DisableInterrupt return value

  Revision 1.1  11/29/2003 01:25:46  yada
  File name changed

  Revision 1.3  11/26/2003 04:18:14  yada
  (none)

  Revision 1.2  11/26/2003 01:50:27  yada
  Mount body of functions     

  Revision 1.1  11/25/2003 12:36:36  yada
  Initial version. Function names only


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include  <nitro/os.h>
#include  <nitro/os/common/message.h>

/*---------------------------------------------------------------------------*
  Name:         OS_InitMessageQueue

  Description:  initialize message queue

  Arguments:    mq          message queue
                msgArray    buffer for message queue
                msgCount    max massage size for buffer

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_InitMessageQueue(OSMessageQueue *mq, OSMessage *msgArray, s32 msgCount)
{
    OS_InitThreadQueue(&mq->queueSend);
    OS_InitThreadQueue(&mq->queueReceive);
    mq->msgArray = msgArray;
    mq->msgCount = msgCount;
    mq->firstIndex = 0;
    mq->usedCount = 0;
}

/*---------------------------------------------------------------------------*
  Name:         OS_SendMessage

  Description:  send message to message queue

  Arguments:    mq          message queue
                msg         massage which is sent
                flags       whether wait or not when queue is full

  Returns:      TRUE if send
                
 *---------------------------------------------------------------------------*/
BOOL OS_SendMessage(OSMessageQueue *mq, OSMessage msg, s32 flags)
{
    OSIntrMode enabled;
    s32     lastIndex;

    enabled = OS_DisableInterrupts();

    while (mq->msgCount <= mq->usedCount)       // check for full queue
    {
        if (!(flags & OS_MESSAGE_BLOCK))
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        else
        {
            // Yield
            OS_SleepThread(&mq->queueSend);
        }
    }

    // Store message
    lastIndex = (mq->firstIndex + mq->usedCount) % mq->msgCount;
    mq->msgArray[lastIndex] = msg;
    mq->usedCount++;

    // Wakeup receiving threads if any
    OS_WakeupThread(&mq->queueReceive);

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         OS_ReceiveMessage

  Description:  receive message from message queue

  Arguments:    mq          message queue
                msg         message which is received
                flags       whether wait or not when queue is empty

  Returns:      TRUE if receive
                
 *---------------------------------------------------------------------------*/
BOOL OS_ReceiveMessage(OSMessageQueue *mq, OSMessage *msg, s32 flags)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    while (mq->usedCount == 0)         // check for empty queue
    {
        if (!(flags & OS_MESSAGE_BLOCK))
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        else
        {
            // Yield
            OS_SleepThread(&mq->queueReceive);
        }
    }

    // Copy-out message
    if (msg != NULL)
    {
        *msg = mq->msgArray[mq->firstIndex];
    }
    mq->firstIndex = (mq->firstIndex + 1) % mq->msgCount;
    mq->usedCount--;

    // Wakeup sending threads if any
    OS_WakeupThread(&mq->queueSend);

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         OS_JamMessage

  Description:  send message to message queue, cutting in the top of the queue

  Arguments:    mq          message queue
                msg         massage which is sent
                flags       whether wait or not when queue is full

  Returns:      TRUE if send
                
 *---------------------------------------------------------------------------*/
BOOL OS_JamMessage(OSMessageQueue *mq, OSMessage msg, s32 flags)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    while (mq->msgCount <= mq->usedCount)       // check for full queue
    {
        if (!(flags & OS_MESSAGE_BLOCK))
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        else
        {
            // Yield
            OS_SleepThread(&mq->queueSend);
        }
    }

    // Store message at the front
    mq->firstIndex = (mq->firstIndex + mq->msgCount - 1) % mq->msgCount;
    mq->msgArray[mq->firstIndex] = msg;
    mq->usedCount++;

    // Wakeup receiving threads if any
    OS_WakeupThread(&mq->queueReceive);

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         OS_ReadMessage

  Description:  read message at message queue. no change in queue
                not occur to switch threads.

  Arguments:    mq          message queue
                msg         message which is received
                flags       whether wait or not when queue is empty

  Returns:      TRUE if read
                
 *---------------------------------------------------------------------------*/
BOOL OS_ReadMessage(OSMessageQueue *mq, OSMessage *msg, s32 flags)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    while (mq->usedCount == 0)         // check for empty queue
    {
        if (!(flags & OS_MESSAGE_BLOCK))
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
        else
        {
            // Yield
            OS_SleepThread(&mq->queueReceive);
        }
    }

    // Copy-out message
    if (msg != NULL)
    {
        *msg = mq->msgArray[mq->firstIndex];
    }

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}
