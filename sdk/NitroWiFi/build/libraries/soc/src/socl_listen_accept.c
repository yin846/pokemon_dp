/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_listen_accept.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_listen_accept.c,v $
Revision 1.7  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.6  01/13/2006 07:26:50  yasu
Adjusted SOCL function parameters.

Revision 1.5  08/24/2005 09:25:13  yasu
added SOCL_SocketIsInvalid

Revision 1.4  08/01/2005 13:23:00  yasu
added SOCL_Listen/Accept

Revision 1.3  07/30/2005 15:30:43  yasu
revisions in conjunction with command pipe separation

Revision 1.2  07/27/2005 12:20:04  yasu
made it possible to select a network byte order during data output

Revision 1.1  07/22/2005 13:55:35  yasu
added listen accept

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>
#include <nitroWiFi/soc.h>

static int  SOCL_ExecListenAcceptCommand(int s, vu16* remote_port_ptr, volatile SOCLInAddr* remote_ip_ptr);
static int  SOCLi_ListenAcceptCallBack(void* arg);

/*---------------------------------------------------------------------------*
Name: SOCL_Listen

Description: Specifies the number to establish a TCP passive connection
and prepares to receive it. Does nothing at present.
  
Arguments: s    socket
backlog         number of receivable connections
  
Returns: positive number or 0: success (accepted)
 *---------------------------------------------------------------------------*/
int SOCL_Listen(int s, int backlog)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;         // non-regulated value
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;      // not initialized
    }

    if (SOCL_SocketIsConnecting(socket))
    {
        return SOCL_EINVAL;         // begins connection process
    }

    if (!SOCL_SocketIsTCP(socket))
    {
        return SOCL_EINVAL;         // not a TCP socket
    }

    if (!SOCL_SocketIsBlock(socket))
    {
        return SOCL_EWOULDBLOCK;    // only for BLOCK implementation
    }

// does nothing at present
// If supporting asynchronous Listen/Accept, need to add processes.
    (void)backlog;
    (void)socket;

    return 0;
}

/*---------------------------------------------------------------------------*
Name: SOCL_Accept

Description: Establishes a TCP passive connection.
In conjunction with the CPS spec, performs Listen and Accept at the same time
perform Listen and Accept at the same time
  
Arguments: s    socket
remote_port     pointer to the remote port number
remote_ip       pointer to the remote IP address
  
Returns: positive number or 0: success (accepted)
negative number: error
currently supported error values are as below
  
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOCL_Accept(int s_listen, vu16* remote_port_ptr, volatile SOCLInAddr* remote_ip_ptr)
{
    SOCLSocket*     socket_listen = (SOCLSocket*)s_listen;
    int s_accept;
    s32 result;

    if (SOCL_SocketIsInvalid(socket_listen))
    {
        return SOCL_EINVAL;         // non-regulated value
    }

    if (!SOCL_SocketIsCreated(socket_listen))
    {
        return SOCL_ENETRESET;      // not initialized
    }

    if (SOCL_SocketIsConnecting(socket_listen))
    {
        return SOCL_EINVAL;         // begins connection process
    }

    if (!SOCL_SocketIsTCP(socket_listen))
    {
        return SOCL_EINVAL;         // not a TCP socket
    }

    if (!SOCL_SocketIsBlock(socket_listen))
    {
        return SOCL_EWOULDBLOCK;    // only for BLOCK implementation
    }

    s_accept = SOCL_TcpSocket();

    if (s_accept < 0)
    {
        return s_accept;

// SOCL_ENOMEM    : insufficient memory
    }

    result = SOCL_Bind(s_accept, socket_listen->local_port);

    if (result < 0)
    {
        return result;

// Return value of SOCL_Bind
//   SOCL_EINVAL;      Value outside the allowed range             -> Can't happen here
//   SOCL_ENETRESET;   Is not initialized     -> Can't happen here
//   SOCL_EALREADY;    The connection process has begun -> Can't happen here
//   SOCL_EMFILE;      Could not save the socket command packet
//   SOCL_ETIMEDOUT;   Connection timed out
    }

    result = SOCL_ListenAccept(s_accept, remote_port_ptr, remote_ip_ptr);

    if (result < 0)
    {
        return result;

// Return value of SOCL_ListenAccept
//   SOCL_EINVAL;      Value outside the allowed range             -> Can't happen here
//   SOCL_ENETRESET;   Is not initialized     -> Can't happen here
//   SOCL_EINVAL;    The connection process has begun -> Can't happen here
//   SOCL_EINVAL;       Not a TCP socket     -> Can't happen here.
//   SOCL_EWOULDBLOCK;  Not in BLOCK mode     -> Can't happen here
//   SOCL_EINVAL;       The Listen port is not specified -> Can't happen here
    }

    return s_accept;
}

/*---------------------------------------------------------------------------*
Name: SOCL_ListenAccept

Description: Establishes a TCP passive connection.
In conjunction with the CPS spec, performs Listen and Accept at the same time
used the socket specified by the argument for accept
  
Arguments: s    socket
remote_port     pointer to the remote port number
remote_ip       pointer to the remote IP address
net_byte_order  NetworkByteOrder if byte order is TRUE
  
Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
  
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOCL_ListenAccept(int s, vu16* remote_port_ptr, volatile SOCLInAddr* remote_ip_ptr)
{
    SOCLSocket*         socket = (SOCLSocket*)s;
    SOCLiCommandPacket*     command;
    s32 result;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;         // non-regulated value
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;      // not initialized
    }

    if (SOCL_SocketIsConnecting(socket))
    {
        return SOCL_EINVAL;         // begins connection process
    }

    if (!SOCL_SocketIsTCP(socket))
    {
        return SOCL_EINVAL;         // not a TCP socket
    }

    if (!SOCL_SocketIsBlock(socket))
    {
        return SOCL_EWOULDBLOCK;    // not in BLOCK mode
    }

    if (socket->local_port == 0)
    {
        return SOCL_EINVAL;         // no Listen port has been set
    }

// Throws a command to the receive thread, and launches CPS_Listen
// and CPS_SocWho inside the callback function.
// at present the NOBLOCK socket type is not supported
    command = SOCLi_CreateCommandPacket(SOCLi_ListenAcceptCallBack, socket, socket->flag_block);

    SDK_ASSERT(command);

// parameter settings for listen_accept
    command->listen_accept.local_port = socket->local_port;
    command->listen_accept.remote_port_ptr = remote_port_ptr;
    command->listen_accept.remote_ip_ptr = remote_ip_ptr;
    socket->state |= SOCL_STATUS_CONNECTING;

// runs the command
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ListenAcceptCallBack

Description: Listen & Accept process callback
  
Arguments: arg   pointer to the command block
  
Returns: value passed to SOCLi_ExecCommand* (in BLOCK mode)
 *---------------------------------------------------------------------------*/
static int SOCLi_ListenAcceptCallBack(void* arg)
{
    SOCLiCommandListenAccept*   cpacket = (SOCLiCommandListenAccept*)arg;
    SOCLSocket*     socket = (SOCLSocket*)cpacket->h.socket;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    u16         remote_port;
    CPSInAddr   remote_ip;
    CPSInAddr   local_ip;

    SDK_ASSERT(socket);
    recv_pipe = socket->recv_pipe;
    SDK_ASSERT(recv_pipe);

    OS_LockMutex(&recv_pipe->h.in_use); // BLOCKED
    {
// local port accept settings, remote port uses any setting
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocBind(%d,0,0)\n", cpacket->local_port);
#endif
        CPS_SocBind(cpacket->local_port, 0, 0);

// waits until connection process comes in from other socket
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_TcpListen()\n");
#endif
        CPS_TcpListen();

        recv_pipe->consumed = 0;

// gets connection target
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocWho()\n");
#endif
        remote_ip = CPS_SocWho(&remote_port, &local_ip);

        *cpacket->remote_port_ptr = remote_port;
        *cpacket->remote_ip_ptr = remote_ip;

        socket->state |= SOCL_STATUS_CONNECTED;
    }

    OS_UnlockMutex(&recv_pipe->h.in_use);

    return SOCL_ESUCCESS;               // Succeeded
}
