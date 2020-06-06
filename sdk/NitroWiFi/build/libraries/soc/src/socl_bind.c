/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_bind.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_bind.c,v $
Revision 1.16  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.15  01/13/2006 07:26:50  yasu
Adjusted SOCL function parameters.

Revision 1.14  09/19/2005 14:08:29  yasu
added a closing state

Revision 1.13  09/13/2005 07:27:06  yada
applied for connect() on UDP

Revision 1.12  08/24/2005 09:25:13  yasu
added SOCL_SocketIsInvalid

Revision 1.11  08/18/2005 08:54:18  yasu
revised returned value for NOBLOCK connect

Revision 1.10  08/10/2005 09:30:35  seiki_masashi
added SSL support

Revision 1.9  08/08/2005 14:26:11  yasu
changed result value for when connection is already in NoBlock mode

Revision 1.8  08/07/2005 13:04:05  yasu
adjusted processes during NOBLOCK

Revision 1.7  08/01/2005 13:23:42  yasu
Changed Mutex release position during Bind

Revision 1.6  07/30/2005 22:30:14  yasu
revised so demo would work

Revision 1.5  07/30/2005 15:30:52  yasu
revisions in conjunction with command pipe separation

Revision 1.4  07/23/2005 14:16:49  yasu
concealed debug messages

Revision 1.3  07/22/2005 12:44:56  yasu
pseudo-implementation of asynchronous processing

Revision 1.2  07/19/2005 14:06:33  yasu
added SOCL_Read/Write

Revision 1.1  07/18/2005 13:16:28  yasu
created send/receive threads

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_BindCallBack(void* arg);
static int  SOCLi_ExecBindCommand(SOCLSocket* socket);

/*---------------------------------------------------------------------------*
Name: SOCL_Bind

Description: performs socket settings (bind) from the local port
In the CPS function, the connection target port and address are set at the same time
and then connected to, but here we only save the setting values
within the socket structure.

Arguments: s		socket
localport	the local port number

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOCL_Bind(int s, u16 local_port)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;     // non-regulated value
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // not initialized
    }

    if (SOCL_SocketIsConnecting(socket))
    {
        return SOCL_EALREADY;   // begins connection process
    }

    socket->local_port = local_port;

    return SOCL_SocketIsUDP(socket) ? SOCLi_ExecBindCommand(socket) : 0;
}

/*---------------------------------------------------------------------------*
Name: SOCL_Connect

Description: performs socket settings (bind) from the remote port
In the CPS function, the connection target port and address are set at the same time
and then connected to, but here the setting values saved within the socket structure
in SOC_Bind are used.

Arguments: s		socket
remoteport	the remote port number
remoteIP	the remote IP address

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
SOC_EINPROGRESS  : The connection is still in progress.
SOC_EINVAL       : Invalid operation.
SOC_EISCONN      : The socket is already connected.
SOC_EMFILE       : Cannot make any more socket indicators than this.
SOC_ENETRESET    : The socket is not initialized.
SOC_ENOBUFS      : Insufficient resources.
SOC_ETIMEDOUT    : TCP connect timeout.

As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOCLiResultCodeInConnecting = SOCL_EINPROGRESS; //or SOCL_EALREADY;
int SOCL_Connect(int s, u16 remote_port, SOCLInAddr remote_ip)
{
    SOCLSocket*     socket = (SOCLSocket*)s;
    int result;

    if (SOCL_SocketIsInvalid(socket) || SOCL_SocketIsClosing(socket))
    {
        return SOCL_EINVAL;                 // unexpected value or in disconnect
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;              // not initialized
    }

// for TCP
    if (SOCL_SocketIsTCP(socket))
    {
        if (SOCL_SocketIsConnected(socket)) // already connected?
        {
            return SOCL_SocketIsBlock(socket) ? SOCL_EISCONN : 0;   // already connected
        }

        if (SOCL_SocketIsConnecting(socket))
        {
            if (SOCL_SocketIsError(socket))
            {
                return socket->result;

// SOCL_ETIMEDOUT : Connection timeout.
            }
            else
            {
// already begun connecting
// Compatibility issue in determining the return value.
// Based on the application, both SOCL_EINPROGRESS and SOCL_EALREADY
// may be expected. - > support data
// made a global variable to allow user to change it
                return SOCLiResultCodeInConnecting;
            }
        }

        socket->remote_port = remote_port;
        socket->remote_ip = remote_ip;

        result = SOCLi_ExecBindCommand(socket);

        return SOCL_SocketIsBlock(socket) ? result : SOCL_EINPROGRESS;
    }
    else
    {
// For UDP, good if already connected (support for connect after bind. just keep it in mind)
        socket->remote_port = remote_port;
        socket->remote_ip = remote_ip;
        return 0;
    }
}

/*---------------------------------------------------------------------------*
Name: SOCL_ExecBindCommand

Description: performs socket settings (bind) from the remote port
In the CPS function, the connection target port and address are set at the same time
and then connected to, but here the setting values saved within the socket structure
in SOC_Bind are used.

Arguments: s		socket
remoteport	the remote port number
remoteIP	the remote IP address

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
SOCL_EMFILE    : unable to hold socket command packet
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
static int SOCLi_ExecBindCommand(SOCLSocket* socket)
{
    SOCLiCommandPacket*     command;
    s32 result;

// Throws a command to recv_pipe, and runs CPS_Bind within
// within the callback function. Error results when command cannot be held
// if socket type is in NOBLOCK.
    command = SOCLi_CreateCommandPacket(SOCLi_BindCallBack, socket, socket->flag_block);
    if (NULL == command)
    {
        return SOCL_EMFILE;
    }

// parameter settings for bind
    command->bind.local_port = socket->local_port;
    command->bind.remote_port = socket->remote_port;
    command->bind.remote_ip = socket->remote_ip;
    socket->state |= SOCL_STATUS_CONNECTING;

// runs the command
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_BindCallBack

Description: Bind/Connect process callback

Arguments: arg   pointer to the command block

Returns: value passed to SOCLi_ExecCommand* (in BLOCK mode)
 *---------------------------------------------------------------------------*/
static int SOCLi_BindCallBack(void* arg)
{
    SOCLiCommandBind*   cpacket = (SOCLiCommandBind*)arg;
    SOCLSocket*         socket = cpacket->h.socket;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    u32 retcode = 0;

    SDK_ASSERT(socket);
    recv_pipe = socket->recv_pipe;
    SDK_ASSERT(recv_pipe);

    OS_LockMutex(&recv_pipe->h.in_use); // BLOCKED
    {
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocBind(\n");
        OS_TPrintf("   local  port=%d,\n", cpacket->local_port);
        OS_TPrintf("   remote_port=%d,\n", cpacket->remote_port);
        OS_TPrintf("   remote_ip  =%3d. %3d. %3d. %3d\n", CPS_CV_IPv4(cpacket->remote_ip));
#endif
        CPS_SocBind(cpacket->local_port, cpacket->remote_port, cpacket->remote_ip);
        recv_pipe->consumed = 0;

// If TCP, performs the connect process.
// if fails, returns the timeout error value
        if (cpacket->h.flag_mode == SOCL_FLAGMODE_TCP || cpacket->h.flag_mode == SOCL_FLAGMODE_SSL)
        {
#ifdef SDK_MY_DEBUG
            OS_TPrintf("CPS_TcpConnect\n");
#endif
            retcode = CPS_TcpConnect();
#ifdef SDK_MY_DEBUG
            OS_TPrintf("CPS_TcpConnect.retcode=%d\n", retcode);
#endif
        }
    }

    OS_UnlockMutex(&recv_pipe->h.in_use);

    if (retcode)
    {
        socket->state |= SOCL_STATUS_ERROR;
        return SOCL_ETIMEDOUT;
    }

    socket->state |= SOCL_STATUS_CONNECTED;
    return SOCL_ESUCCESS;               // Succeeded
}
