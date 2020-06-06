/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_shutdown.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_shutdown.c,v $
Revision 1.6  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.5  09/19/2005 15:11:37  yasu
minor adjustments to part added to closing state

Revision 1.4  08/24/2005 09:25:13  yasu
added SOCL_SocketIsInvalid

Revision 1.3  08/14/2005 10:58:35  yasu
corrected error in confirming connection flag

Revision 1.2  07/30/2005 15:30:52  yasu
revisions in conjunction with command pipe separation

Revision 1.1  07/22/2005 12:44:56  yasu
pseudo-implementation of asynchronous processing

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_ShutdownCallBack(void* arg);

/*---------------------------------------------------------------------------*
Name: SOCL_Shutdown

Description: Performs socket shutdown process.
  
Arguments: s		socket
  
Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :  
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOCL_Shutdown(int s)
{
    SOCLSocket*     socket = (SOCLSocket*)s;
    SOCLiSocketSendCommandPipe*     send_pipe;
    SOCLiCommandPacket*         command;
    s32 result;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;     // non-regulated value
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // not initialized
    }

    if (!SOCL_SocketIsConnected(socket) || SOCL_SocketIsClosing(socket))
    {
        return SOCL_ENOTCONN;   // not connected
    }

    socket->state |= SOCL_STATUS_CLOSING;

// Throws a command to the send thread, and launches CPS_Shutdown
// within the callback function. Error results when command cannot be held
// if socket type is in NOBLOCK.
    //
// Our current implementation uses this because of time issues, but we think we need
// to change to a process that forces a socket disconnect using destructor.
    send_pipe = socket->send_pipe;

    if (send_pipe && send_pipe->exe_socket)
    {
        command = SOCLi_CreateCommandPacket(SOCLi_ShutdownCallBack, send_pipe->exe_socket, socket->flag_block);
        if (NULL == command)
        {
            return SOCL_EMFILE;
        }

// runs the command
        result = SOCLi_ExecCommandPacketInSendPipe(send_pipe->exe_socket, command);
    }
    else
    {
        result = 0;
    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ShutdownCallBack

Description: Shutdown process callback.
  
Arguments: arg   pointer to the command block
  
Returns: value passed to SOCLi_ExecCommand* (in BLOCK mode)
 *---------------------------------------------------------------------------*/
static int SOCLi_ShutdownCallBack(void* arg)
{
    SOCLiCommandShutdown*   cpacket = (SOCLiCommandShutdown*)arg;
    SOCLSocket*         socket = (SOCLSocket*)cpacket->h.socket;

    if (SOCL_SocketIsTCP(socket))
    {
        CPS_TcpShutdown();
    }

    return SOCL_ESUCCESS;   // Succeeded
}
