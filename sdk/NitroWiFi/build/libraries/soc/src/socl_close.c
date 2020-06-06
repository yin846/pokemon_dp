/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_close.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_close.c,v $
  Revision 1.22  05/18/2006 11:35:44  yasu
  Made revisions to the interrupt prohibition interval

  Revision 1.21  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.20  10/14/2005 06:48:19  yasu
  Corrected a problem where the response queue was not cleared when a close notification was sent to the control pipe.

  Revision 1.19  10/14/2005 03:48:57  yasu
  warning measures

  Revision 1.18  10/14/2005 01:56:53  yasu
  Corrected a problem where close processing was incomplete whenever the command queue was full.

  Revision 1.17  09/29/2005 03:05:50  yasu
  Unregister/Register related bug fix

  Revision 1.16  09/28/2005 11:44:37  yasu
  cleaned up the code
  added SOCL_CalmDown()
  as we don't yet have official support for the CPS library, covered in processing in SOC

  Revision 1.15  09/28/2005 04:11:56  kitase_hirotake
  revised to wait for Write end

  Revision 1.14  09/27/2005 14:18:09  yasu
  asynchronous support for SOC_Close

  Revision 1.13  09/19/2005 15:11:37  yasu
  minor adjustments to part added to closing state

  Revision 1.12  09/19/2005 14:08:29  yasu
  added a closing state

  Revision 1.11  09/01/2005 06:42:55  yasu
  consolidated processes determining whether a socket is active

  Revision 1.10  09/01/2005 06:12:20  yasu
  consolidated processes determining whether a socket is active

  Revision 1.9  08/25/2005 08:27:38  yasu
  added buffering process for UDP receiving

  Revision 1.8  08/24/2005 09:25:13  yasu
  added SOCL_SocketIsInvalid

  Revision 1.7  08/19/2005 05:06:16  yasu
  fill in data within socket closed during DEBUG build

  Revision 1.6  08/18/2005 13:52:11  yasu
  get send pipe synchronization during close process

  Revision 1.5  08/18/2005 13:18:49  yasu
  added cleanup process through socket link list

  Revision 1.4  08/18/2005 08:53:36  yasu
  support for the fact that a clear Shutdown call is needed during Close for the CPS function

  Revision 1.3  07/30/2005 22:30:14  yasu
  revised so demo would work

  Revision 1.2  07/30/2005 15:30:52  yasu
  revisions in conjunction with command pipe separation

  Revision 1.1  07/22/2005 12:44:56  yasu
  pseudo-implementation of asynchronous processing

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_CloseCallBack(void* arg);
static void SOCLi_FreeCommandPipe(SOCLiSocketCommandPipe* pipe);

/*---------------------------------------------------------------------------*
  Name:         SOCL_IsClosed

  Description:  determines whether socket has been closed
                SOCL_Close performs asynchronous close processing.
                This function determines whether that process has completed or not.
  
  Arguments:    s		socket
  
  Returns:      TRUE  : closed
                FALSE : invalid socket or not closed
 *---------------------------------------------------------------------------*/
int SOCL_IsClosed(int s)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    // expected handle value
    // recognize as closed if the socket is not a valid element or if there is no remove entry
    if ((int)socket >= 0 && SOCL_SocketIsInvalid(socket) && !SOCL_SocketIsInTrash(socket))
    {
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         SOCL_Close

  Description:  Performs socket Close process.
  
  Arguments:    s		socket
  
  Returns:      0  : success (returns a 0 for the first Close call and at Close completion)
                negative number: error
                  currently supported error values are as below
                  SOCL_EINPROGRESS close process in progress
 *---------------------------------------------------------------------------*/
int SOCL_Close(int s)
{
    SOCLSocket*         socket = (SOCLSocket*)s;
    SOCLiCommandPacket*     command;

    // error if unexpected value
    if ((int)socket <= 0)
    {
        return SOCL_EINVAL;
    }

    // recognized as undergoing close if the socket is in the remove entry
    if (SOCL_SocketIsInTrash(socket))
    {
        return SOCL_EINPROGRESS;
    }

    // recognize as already closed if there is no entry for the socket
    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_ESUCCESS;
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // not initialized
    }

    // close in progress if socket flag is WAIT_CLOSE
    if (SOCL_SocketIsWaitingClose(socket))
    {
        return SOCL_EINPROGRESS;
    }

    socket->state |= (SOCL_STATUS_CLOSING | SOCL_STATUS_WAIT_CLOSE);

    // for TCP throws a NULL command to the send thread, and ends the send thread after all data is sent
    // The NULL command is issued by BLOCK.
    if (SOCL_SocketIsTCP(socket))
    {
        (void)SOCLi_SendCommandPacket(&socket->send_pipe->h, NULL);
    }

    // Issues a command to the control thread, and launches CPS_Close.
    // Although command transmission is handled by BLOCK, code does not wait for the close process itself to end.
    command = SOCLi_CreateCommandPacket(SOCLi_CloseCallBack, socket, SOCL_FLAGBLOCK_BLOCK);
    command->h.response = NULL; // Notification that the close process has ended is not required
    (void)SOCLi_SendCommandPacketToCtrlPipe(socket, command);

    // 0 is returned for the first close.
    return SOCL_ESUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CloseCallBack

  Description:  Close process callback.
  
  Arguments:    arg   pointer to the command block
  
  Returns:      value passed to SOCLi_ExecCommand* (in BLOCK mode)
 *---------------------------------------------------------------------------*/
static int SOCLi_CloseCallBack(void* arg)
{
    SOCLiCommandClose*  cpacket = (SOCLiCommandClose*)arg;
    SOCLSocket*         socket = (SOCLSocket*)cpacket->h.socket;
    OSIntrMode          enable;

    if (SOCL_SocketIsTCP(socket))
    {
        // wait until the data in send pipe is sent
        OS_JoinThread(&socket->send_pipe->h.thread);

        // socket close processing
        CPS_TcpShutdown();
        CPS_TcpClose();
        CPS_SocRelease();
    }

    CPS_SocUnRegister();

    socket->state &= ~(SOCL_STATUS_CONNECTING | SOCL_STATUS_CONNECTED);

    // Performs an end notification for this thread. As the queue is empty at this point a deadlock does not occur.
    (void)SOCLi_SendCommandPacket(SOCL_SocketIsUDPSend(socket) ? &socket->send_pipe->h : &socket->recv_pipe->h, NULL);

    // places the thread on the remove list
    enable = OS_DisableInterrupts();
    SOCLi_SocketUnregister(socket);
    SOCLi_SocketRegisterTrash(socket);
    (void)OS_RestoreInterrupts(enable);

    socket->state |= SOCL_STATUS_WAIT_RELEASE;

    return SOCL_ESUCCESS;   // Succeeded
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CleanupSocket

  Description:  releases the memory for the socket
                releases the memory in the following order
  
                memory area
                --------------------------------------------------------
                send thread buffer
                receive thread buffer
                CPS socket buffer
                SOCL socket memory

  Arguments:    socket          socket parameters
  
  Returns:      None
 *---------------------------------------------------------------------------*/
void SOCLi_CleanupSocket(SOCLSocket* socket)
{
    OSIntrMode  enable;

    if (socket)
    {
        socket->state = 0;

        // releases each part in order
        if (SOCL_SocketIsTCP(socket))
        {
            SOCLi_FreeCommandPipe(&socket->send_pipe->h);
            SOCLi_FreeCommandPipe(&socket->recv_pipe->h);
        }
        else if (SOCL_SocketIsUDP(socket))
        {
            // releases UDP receive data
            SOCLiSocketUdpData*     udpdata = socket->recv_pipe->udpdata.out;
            SOCLiSocketUdpData*     udpdata_next;

            while (udpdata)
            {
                udpdata_next = udpdata->next;
                SOCLi_Free(udpdata);
                udpdata = udpdata_next;
            }

            socket->recv_pipe->udpdata.size = 0;
            socket->recv_pipe->udpdata.in = NULL;
            socket->recv_pipe->udpdata.out = NULL;
            OS_WakeupThread(&socket->recv_pipe->udpdata.waiting);

            SOCLi_FreeCommandPipe(&socket->recv_pipe->h);
        }
        else if (SOCL_SocketIsUDPSend(socket))
        {
            SOCLi_FreeCommandPipe(&socket->send_pipe->h);
        }

        enable = OS_DisableInterrupts();
        SOCLi_SocketUnregister(socket);
        SOCLi_SocketUnregisterTrash(socket);

#ifdef SDK_DEBUG
        MI_CpuFill8(socket, 0xaa, sizeof(*socket));
#endif

        SOCLi_Free(socket);
        (void)OS_RestoreInterrupts(enable);
    }

    return;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_FreeCommandPipe

  Description:  releases the memory for the socket command pipe

  Arguments:    pipe  command pipe
  
  Returns:      None
 *---------------------------------------------------------------------------*/

//
// SOCKET THREAD

//
static void SOCLi_FreeCommandPipe(SOCLiSocketCommandPipe* pipe)
{
    SOCLiCommandPacket*     command;
    OSIntrMode  enable;

    if (pipe == NULL)
        return;

    SDK_ASSERT(OS_GetCurrentThread() != &pipe->thread);

    //
    // waits for the end of the socket thread
    // if already ended, nothing is performed and immediately returns
    //
    OS_JoinThread(&pipe->thread);

    //
    // laucnhes other threads will waiting on socket thread processing
    //     - sends a SOCL_ECANCELED message to threads waiting on command processing
    //     - recovers command packet
    // All related processes have been compiled together in an atomic process.
    // Be aware that a hang might result unless all processes related to the internal
    // message are NOBLOCK.
    //

    // Stops thread switching
    enable = OS_DisableInterrupts();
    (void)OS_DisableScheduler();
    
    while (OS_ReceiveMessage(&pipe->queue, (void*) &command, OS_MESSAGE_NOBLOCK))
    {
        if (command)
        {
            if (command->h.response)
            {
                (void)OS_SendMessage(command->h.response, (void*)SOCL_ECANCELED, OS_MESSAGE_NOBLOCK);
            }

            SOCLi_FreeCommandPacket(command);   // NOBLOCK
        }
    }
    
    // run rights are ceded to another launched thread with a higher priority
    // while doing the send process for the above messages
    (void)OS_EnableScheduler();
    OS_RescheduleThread();
    (void)OS_RestoreInterrupts(enable);
    
    return;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_TrashSocket

  Description:  releases the memory for sockets registered on the remove list
                regularly called from the CPS library
  
  Arguments:    None
  
  Returns:      None
 *---------------------------------------------------------------------------*/
void SOCLi_TrashSocket(void)
{
    OSIntrMode  enable = OS_DisableInterrupts();

    while (SOCLiSocketListTrash != NULL)
    {
        SOCLi_CleanupSocket(SOCLiSocketListTrash);
    }
    (void)OS_RestoreInterrupts(enable);
}
