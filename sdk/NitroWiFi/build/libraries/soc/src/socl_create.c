/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_create.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_create.c,v $
Revision 1.14  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.13  01/13/2006 07:26:50  yasu
Adjusted SOCL function parameters.

Revision 1.12  09/27/2005 14:18:09  yasu
asynchronous support for SOC_Close

Revision 1.11  08/25/2005 08:27:38  yasu
added buffering process for UDP receiving

Revision 1.10  08/18/2005 13:18:49  yasu
added cleanup process through socket link list

Revision 1.9  08/13/2005 08:02:39  yasu
added consumed_min controls

Revision 1.8  08/10/2005 09:30:57  seiki_masashi
added SSL support

Revision 1.7  07/30/2005 22:30:14  yasu
revised so demo would work

Revision 1.6  07/30/2005 15:30:08  yasu
revisions in conjunction with command pipe separation

Revision 1.5  07/27/2005 12:20:33  yasu
sets the pointer to NULL when buffer size is 0

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

static int          SOCLi_CreateSocketCallBack(void* arg);

static SOCLSocket*  SOCLi_StartupSocket(const SOCLSocketParam* param);
static u32          SOCLi_GetSizeSocket(const SOCLSocketParam* param);
static u32          SOCLi_GetSizeCommandPipe(const SOCLSocketCommandPipeParam* param);
static u8*          SOCLi_InitSocket(SOCLSocket* socket, const SOCLSocketParam* param);
static u8*          SOCLi_InitSocketBuffer(u8* ptr, CPSSocBuf* buf, u32 size);
static u8*          SOCLi_InitCommandPipe(u8*  ptr, SOCLiSocketCommandPipe*  pipe, const SOCLSocketCommandPipeParam*  param);

/*---------------------------------------------------------------------------*
Name: SOCL_CreateSocket

Description: Creates the socket.
Uses own interface.
Possible to have detailed settings for memory use.

Arguments: param           detailed settings parameters
the defaults available in the SDK are:
for TCP: &SOCLSocketParamTCP
for UDP: &SOCLSocketParamUDP*
  
Returns: positive value: pointer to the socket
negative number: error
currently supported error values are as below
SOCL_ENOMEM    : insufficient memory
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOCL_CreateSocket(const SOCLSocketParam* param)
{
    SOCLSocket*         socket;
    SOCLiCommandPacket*     command;
    s32 result;

    SDK_ASSERT(param != NULL);

// Blocks off memory for the socket, and starts the thread that performs socket processes
// the SOCLSocket internal value can be overwritten by other threads when the thread is being created or destroyed
// only.
    socket = SOCLi_StartupSocket(param);
    if (socket == NULL)
    {
        return SOCL_ENOMEM;
    }

// Send the socket initialization command and launch the callback,
// and wait for process completion. Command memory is released internally at completion.
    command = SOCLi_CreateCommandPacket(SOCLi_CreateSocketCallBack, socket, OS_MESSAGE_BLOCK);

// send a command to Ctrl Pipe at creation and removal
// normally Recv Pipe is set to be Ctrl Pipe (exception at UDP Send)
// SOCLi_CreateSocketCallBack will always return SUCCESS
    result = SOCLi_ExecCommandPacketInCtrlPipe(socket, command);
    SDK_ASSERT(result >= 0);

    return (int)socket;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_CreateSocketCallBack

Description: Create process callback
  
Arguments: arg   pointer to the command block
  
Returns: value passed to SOCLi_ExecCommand*
 *---------------------------------------------------------------------------*/
static int SOCLi_CreateSocketCallBack(void* arg)
{
    SOCLiCommandCreateSocket*   cpacket = (SOCLiCommandCreateSocket*)arg;
    SOCLSocket*     socket = cpacket->h.socket;
    SOCLiSocketSendCommandPipe*     send_pipe;
    SOCLiSocketRecvCommandPipe*     recv_pipe;

    SDK_ASSERT(socket);
#ifdef SDK_MY_DEBUG
    OS_TPrintf("CPS_SocRegister\n");
#endif
    CPS_SocRegister(&socket->cps_socket);

    send_pipe = socket->send_pipe;
    recv_pipe = socket->recv_pipe;

    switch (socket->flag_mode)
    {
    case SOCL_FLAGMODE_TCP:
    case SOCL_FLAGMODE_SSL:
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocDup\n");
        OS_TPrintf("CPS_SocUse\n");
#endif
        CPS_SocDup(&send_pipe->h.thread);
        CPS_SocUse();
        break;

    case SOCL_FLAGMODE_UDP:
        CPS_SocUse();
        CPS_SocDatagramMode();
        CPS_SetUdpCallback(SOCLi_UdpRecvCallback);
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocUse\n");
        OS_TPrintf("CPS_SocDatagramMode\n");
        OS_TPrintf("CPS_SetUdpCallback\n");
#endif
        break;

    case SOCL_FLAGMODE_UDPSEND:
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocDatagramMode\n");
#endif
        CPS_SocDatagramMode();
        break;

    case SOCL_FLAGMODE_ICMP:
    default:
        SDK_ASSERT(0);
        break;
    }

    socket->state = SOCL_STATUS_CREATE;

    return SOCL_ESUCCESS;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_StartupSocket

Description: gets the memory for the socket
the following memory is blocked off contiguously on the memory map
  
memory area
                --------------------------------------------------------
SOCL socket memory
CPS socket buffer
receive thread buffer
send thread buffer

Arguments: param  parameter
  
Returns: a pointer to the blocked off socket data memory
 *---------------------------------------------------------------------------*/
static SOCLSocket* SOCLi_StartupSocket(const SOCLSocketParam* param)
{
    SOCLSocket*     socket;
    u32         size;
    OSIntrMode  enable;

// requests the needed memory size
    size = SOCLi_GetSizeSocket(param);

    enable = OS_DisableInterrupts();
    socket = SOCLi_Alloc(size);

    if (socket)
    {
// temporarily clears 0
        MI_CpuClear8(socket, size);

// assigns memory in order
        (void)SOCLi_InitSocket(socket, param);

// registers the link to the list
        SOCLi_SocketRegister(socket);
    }

    (void)OS_RestoreInterrupts(enable);

    return socket;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_GetSizeSocket
  
Description: returns the memory size needed by the socket
  
Arguments: param  socket generation parameter
  
Returns: memory size needed by the socket
 *---------------------------------------------------------------------------*/
static u32 SOCLi_GetSizeSocket(const SOCLSocketParam* param)
{
    u32 size;

    size = SOCLi_ROUNDUP4(sizeof(SOCLSocket));

// memory needed for receive functionality
    if (param->buffer.rcvbuf_size)
    {
        size += SOCLi_ROUNDUP4(sizeof(SOCLiSocketRecvCommandPipe));
        size += SOCLi_RoundUp4(param->buffer.rcvbuf_size);
        size += SOCLi_RoundUp4(param->buffer.linbuf_size);
        size += SOCLi_GetSizeCommandPipe(&param->recv_pipe);
    }

// memory needed for send functionality
    if (param->buffer.sndbuf_size)
    {
        size += SOCLi_ROUNDUP4(sizeof(SOCLiSocketSendCommandPipe));
        size += SOCLi_RoundUp4(param->buffer.sndbuf_size);
        size += SOCLi_RoundUp4(param->buffer.outbuf_size);
        size += SOCLi_RoundUp4(param->buffer.wrtbuf_size);
        size += SOCLi_GetSizeCommandPipe(&param->send_pipe);
    }

    return size;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_GetSizeCommandPipe
  
Description: returns the memory size needed by the command pipe
  
Arguments: param  Command Pipe generation parameters
  
Returns: memory size needed by the command pipe
 *---------------------------------------------------------------------------*/
static u32 SOCLi_GetSizeCommandPipe(const SOCLSocketCommandPipeParam* param)
{
    u32 size;

    size = SOCLi_RoundUp4(param->queue_max * sizeof(OSMessage));
    size += SOCLi_RoundUp4(param->stack_size);

    return size;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_InitSocket
  
Description: initializes the socket

Arguments: socket  target socket memory (if cleared, 0)
param   initialization parameters
  
Returns: next pointer to socket memory
 *---------------------------------------------------------------------------*/
static u8* SOCLi_InitSocket(SOCLSocket* socket, const SOCLSocketParam* param)
{
    u8*         ptr;
    CPSSoc*     cps = &socket->cps_socket;

    socket->flag_mode = param->flag_mode;
    socket->flag_block = param->flag_block;

    ptr = (u8*)socket;
    ptr = ptr + SOCLi_ROUNDUP4(sizeof(SOCLSocket));

// initialization for receive functionality
    if (param->buffer.rcvbuf_size)
    {
        SOCLiSocketRecvCommandPipe*     pipe = (SOCLiSocketRecvCommandPipe*)ptr;

// Recv Command Pipe
        socket->recv_pipe = pipe;
        pipe->consumed_min = param->buffer.rcvbuf_consume_min;
        ptr += SOCLi_ROUNDUP4(sizeof(SOCLiSocketRecvCommandPipe));
        ptr = SOCLi_InitCommandPipe(ptr, &pipe->h, &param->recv_pipe);

// Buffers
        ptr = SOCLi_InitSocketBuffer(ptr, &cps->rcvbuf, param->buffer.rcvbuf_size);
        ptr = SOCLi_InitSocketBuffer(ptr, &cps->linbuf, param->buffer.linbuf_size);

        pipe->udpdata.size_max = param->buffer.udpbuf_size;
        OS_InitThreadQueue(&pipe->udpdata.waiting);
    }

// initialization for send functionality
    if (param->buffer.sndbuf_size)
    {
        SOCLiSocketSendCommandPipe*     pipe = (SOCLiSocketSendCommandPipe*)ptr;

// Send Command Pipe
        socket->send_pipe = pipe;   // mutual links for socket and pipe
        pipe->exe_socket = socket;  //   Send Pipe effective while shared
        ptr += SOCLi_ROUNDUP4(sizeof(SOCLiSocketSendCommandPipe));
        ptr = SOCLi_InitCommandPipe(ptr, &pipe->h, &param->send_pipe);

// Buffers
        ptr = SOCLi_InitSocketBuffer(ptr, &cps->sndbuf, param->buffer.sndbuf_size);
        ptr = SOCLi_InitSocketBuffer(ptr, &cps->outbuf, param->buffer.outbuf_size);
        ptr = SOCLi_InitSocketBuffer(ptr, &pipe->buffer.area, param->buffer.wrtbuf_size);
        OS_InitThreadQueue(&pipe->buffer.waiting);
    }
    else
    {
// share the Send Pipe
        socket->send_pipe = ((SOCLSocket*)SOCLiUDPSendSocket)->send_pipe;
    }

    return ptr;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_InitSocketBuffer
  
Description: initializes the buffer within the socket

Arguments: ptr     memory blocked off for the buffer
buf     buffer construct
size    buffer size
  
Returns: pointer to the next available buffer memory
 *---------------------------------------------------------------------------*/
static u8* SOCLi_InitSocketBuffer(u8* ptr, CPSSocBuf* buf, u32 size)
{
    buf->data = size ? ptr : NULL;  // sets to NULL if 0
    buf->size = size;
    return ptr + SOCLi_RoundUp4(size);
}

/*---------------------------------------------------------------------------*
Name: SOCLi_InitCommandPipe
  
Description: initializes the command pipe

Arguments: ptr     memory blocked off for the buffer inside the command pipe
buf     buffer construct
size    buffer size
  
Returns: ptr's next available pointer
 *---------------------------------------------------------------------------*/
static u8* SOCLi_InitCommandPipe(u8* ptr, SOCLiSocketCommandPipe* pipe, const SOCLSocketCommandPipeParam* param)
{
    OSMessage*  messages;
    u8*         stack_bottom;

// memory assignment
    messages = (OSMessage*)ptr;
    stack_bottom = ptr + SOCLi_GetSizeCommandPipe(param);

// initializes each memory area
    OS_InitMessageQueue(&pipe->queue, messages, param->queue_max);

    OS_InitMutex(&pipe->in_use);

    OS_CreateThread(&pipe->thread, SOCLi_CommandPacketHandler, pipe, stack_bottom, param->stack_size, param->priority);
    OS_WakeupThreadDirect(&pipe->thread);

    return stack_bottom;
}
