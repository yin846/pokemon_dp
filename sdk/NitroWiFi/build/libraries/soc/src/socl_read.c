/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_read.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_read.c,v $
Revision 1.39  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.38  12/06/2005 11:53:13  yasu
addressed issue in TCP BLOCK Write
introduced a macro for determining BLOCK mode

Revision 1.37  09/19/2005 14:08:29  yasu
added a closing state

Revision 1.36  09/16/2005 12:13:50  yasu
activated the event counter for FinalRom as well

Revision 1.35  09/14/2005 06:00:19  yasu
SOCLi_CheckCount support

Revision 1.34  09/09/2005 07:38:57  yasu
revised the determination for block status

Revision 1.33  09/08/2005 05:35:34  yasu
moved the ability to determine whether the socket scripter was of a valid value to the SOCL

Revision 1.32  09/01/2005 06:42:55  yasu
consolidated processes determining whether a socket is active

Revision 1.31  08/25/2005 08:27:38  yasu
added buffering process for UDP receiving

Revision 1.30  08/24/2005 09:25:13  yasu
added SOCL_SocketIsInvalid

Revision 1.29  08/14/2005 10:59:34  yasu
added consumed_min controls

Revision 1.28  08/13/2005 08:20:27  yasu
added consumed_min controls

Revision 1.27  08/12/2005 09:50:23  yasu
added connection status settings when EOF is reached

Revision 1.26  08/11/2005 12:02:50  yasu
revised the sloppy part of the UDP socket connection reset process after consume

Revision 1.25  08/11/2005 10:07:15  yasu
saved the local port UDP bind results at read

Revision 1.24  08/11/2005 07:02:34  yasu
eliminated debug output

Revision 1.23  08/10/2005 12:34:53  yasu
changed buffer operations

Revision 1.22  08/10/2005 09:31:44  seiki_masashi
added SSL support

Revision 1.21  08/10/2005 04:49:13  yasu
changed the returned value during receipt

Revision 1.20  08/09/2005 12:26:33  yasu
revised the consume process

Revision 1.19  08/09/2005 03:20:14  yasu
changed the TCP/UDP consume handling

Revision 1.18  08/08/2005 14:15:18  yasu
changed the read returned value

Revision 1.17  08/07/2005 13:04:49  yasu
re-revised the read returned value

Revision 1.16  08/04/2005 04:15:56  yasu
fixed an initialization bug

Revision 1.15  08/04/2005 04:08:28  yasu
revised the UDP read returned value

Revision 1.14  08/02/2005 13:12:32  seiki_masashi
replaced the EOF determination constant with an enum value

Revision 1.13  08/02/2005 10:50:09  yasu
short-term support for NoBlock EOF determination process

Revision 1.12  08/02/2005 09:40:11  yasu
added process revisions during BLOCK READ mode

Revision 1.11  08/02/2005 08:13:12  yasu
READ EOF temporary implementation.
added process revisions during BLOCK READ mode

Revision 1.10  08/02/2005 07:00:58  yasu
support for EOF during NOBLOCK mode

Revision 1.9  08/01/2005 07:04:27  seiki_masashi
revised to return EWOULDBLOCK when there is no data at non-blocking
revised argument provided to SOCLi_ExecReadCommand during blocking

Revision 1.8  08/01/2005 06:26:36  yasu
changed the returned value when there is no data for SOCLi_ReadCallBack

Revision 1.7  08/01/2005 05:55:26  yasu
revised subtraction timing for recv_pipe->consumed

Revision 1.6  07/30/2005 22:30:14  yasu
revised so demo would work

Revision 1.5  07/30/2005 15:30:29  yasu
revisions in conjunction with command pipe separation

Revision 1.4  07/25/2005 10:15:01  yasu
added the SOC library

Revision 1.3  07/22/2005 12:44:56  yasu
pseudo-implementation of asynchronous processing

Revision 1.2  07/19/2005 14:06:33  yasu
added SOCL_Read/Write

Revision 1.1  07/15/2005 13:33:11  yasu
adjusted soc library source

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_ReadAndConsumeBuffer(SOCLSocket* , void* , int, u16* , SOCLInAddr* , s32, s32);
static int  SOCLi_ReadBuffer(SOCLSocket* , void* , int, u16* , SOCLInAddr* , s32);
static int  SOCLi_ReadUdpBuffer(SOCLSocket* , void* , int, u16* , SOCLInAddr* , s32);
static int  SOCLi_CopyCPSBuffer(SOCLSocket* , void* , int, u16* , SOCLInAddr* );
static u8*  SOCLi_ReadCPSBuffer(SOCLSocket* , s32* , u16* , u16* , SOCLInAddr* );
static int  SOCLi_ExecReadCommand(SOCLSocket* , void* , int, u16* , SOCLInAddr* );
static int  SOCLi_ReadCallBack(void* );
static int  SOCLi_ConsumeBuffer(SOCLSocket* );
static int  SOCLi_ConsumeCallBack(void* );
static int  SOCLi_ConsumeCPSBuffer(SOCLSocket* );

u32         SOCLi_CheckCount[SOCL_CHECKCOUNT_MAX] = { 0};

/*---------------------------------------------------------------------------*
Name: SOCL_Read

Description: Receives data from the socket

Arguments: socket          socket
buffer          received data
buffer_len      length of the buffer storing the received data
remote_port     originating host port
remote_ip       originating host IP

remote_port/remote_ip are only valid when UDP is not performing
Connect. Values are ignored when UDP is performing TCP or Connect

Returns: positive value: number of bytes in the received message
negative value: error
 *---------------------------------------------------------------------------*/
int SOCL_Read(int s, void* buffer, int buffer_len, u16* remote_port, SOCLInAddr* remote_ip)
{
    return SOCL_ReadFrom(s, buffer, buffer_len, remote_port, remote_ip, 0);
}

/*---------------------------------------------------------------------------*
Name: SOCL_ReadNoBlock

Description: Receives the data from the socket in NOBLOCK mode.

Arguments: socket          socket
buffer          received data
buffer_len      length of data received
remote_port     originating host port (only valid for UDP)
remote_ip     originating host IP (only valid for UDP)

Returns: positive value: number of bytes in the received message
negative value: error
 *---------------------------------------------------------------------------*/
int SOCL_ReadNoBlock(int s, void* buffer, int buffer_len, u16* remote_port, SOCLInAddr* remote_ip)
{
    return SOCL_ReadFrom(s, buffer, buffer_len, remote_port, remote_ip, SOCL_MSG_DONTWAIT);
}

/*---------------------------------------------------------------------------*
Name: SOCL_ReadBlock

Description: Receives the data from the socket in BLOCK mode.

Arguments: socket          socket
buffer          received data
buffer_len      length of data received
remote_port     originating host port (only valid for UDP)
remote_ip     originating host IP (only valid for UDP)

Returns: positive value: number of bytes in the received message
negative value: error
 *---------------------------------------------------------------------------*/
int SOCL_ReadBlock(int s, void* buffer, int buffer_len, u16* remote_port, SOCLInAddr* remote_ip)
{
    return SOCL_ReadFrom(s, buffer, buffer_len, remote_port, remote_ip, 0);
}

/*---------------------------------------------------------------------------*
Name: SOCL_ReadFrom

Description: Receives data from the socket

Arguments: socket          socket
buffer          received data
buffer_len      length of the buffer storing the received data
remote_port     originating host port
remote_ip       originating host IP
flag            received flag

remote_port/remote_ip are only valid when UDP is not performing
Connect. Values are ignored when UDP is performing TCP or Connect

Returns: positive value: number of bytes in the received message
negative value: error
 *---------------------------------------------------------------------------*/
int SOCL_ReadFrom(int s, void* buffer, int buffer_len, u16* remote_port, SOCLInAddr* remote_ip, int flags)
{
    SOCLSocket*     socket = (SOCLSocket*)s;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    int flag_block;
    int result;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;     // non-regulated value
    }

    if ((flags & SOCL_MSG_DONTWAIT) || SOCL_SocketIsNoBlock(socket))
    {
        if (SOCL_SocketIsSSL(socket))
        {
            return SOCL_EINVAL; // SSL only supports a blocking call
        }

        flag_block = SOCL_FLAGBLOCK_NOBLOCK;
    }
    else
    {
        if (OS_GetProcMode() == OS_PROCMODE_IRQ)
        {
            return SOCL_EINVAL; // blocking calls cannot be called within an interrupt handler
        }

        flag_block = SOCL_FLAGBLOCK_BLOCK;
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // not initialized
    }

    if (SOCL_SocketIsTCP(socket) && (!SOCL_SocketIsConnected(socket) || SOCL_SocketIsClosing(socket)))
    {
        return SOCL_ENOTCONN;   // not connected
    }

    SDK_ASSERT(buffer && buffer_len);
    recv_pipe = socket->recv_pipe;
    SDK_ASSERT(recv_pipe);      // initialization method is off
    if (!SOCL_FLAGISBLOCK(flag_block))
    {
        if (!OS_TryLockMutex(&recv_pipe->h.in_use))
        {
            return SOCL_EWOULDBLOCK;
        }
    }
    else
    {
        OS_LockMutex(&recv_pipe->h.in_use);
    }

    result = SOCLi_ReadAndConsumeBuffer(socket, buffer, buffer_len, remote_port, remote_ip, flag_block, flags);

    OS_UnlockMutex(&recv_pipe->h.in_use);
    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ReadAndConsumeBuffer

Description: Begins the READ and Consume processes.
However, exception controls are not performed.

Arguments: socket          socket
buffer          copy data
buffer_len      length of copy data
remote_port     originating host port (only valid for UDP)
remote_ip     originating host IP (only valid for UDP)
flag_block      flag to indicate BLOCK status

Returns: length of copied data (failed if 0)
 *---------------------------------------------------------------------------*/
static int SOCLi_ReadAndConsumeBuffer(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port,
                                      SOCLInAddr*  remote_ip, s32 flag_block, s32 flags)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    BOOL    is_noconsumed = ((flags & SOCL_MSG_PEEK) && recv_pipe);
    int     result;
    s8      flag_noconsume;

    if (is_noconsumed)
    {
// temporarily raises a noconsume flag
        flag_noconsume = recv_pipe->flag_noconsume;
        recv_pipe->flag_noconsume = TRUE;
    }

    if (SOCL_SocketIsUDP(socket))
    {
        result = SOCLi_ReadUdpBuffer(socket, buffer, buffer_len, remote_port, remote_ip, flag_block);
    }
    else
    {
        result = SOCLi_ReadBuffer(socket, buffer, buffer_len, remote_port, remote_ip, flag_block);

        if (result >= 0)
        {
            (void)SOCLi_ConsumeBuffer(socket);
        }
    }

    if (is_noconsumed)
    {
// restores the noconsume flag
        recv_pipe->flag_noconsume = flag_noconsume;
    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ReadBuffer

Description: Begins the READ process. However, exceptions controls are not performed.

Arguments: socket          socket
buffer          copy data
buffer_len      length of copy data
remote_port     originating host port (only valid for UDP)
remote_ip     originating host IP (only valid for UDP)
flag_block      flag to indicate BLOCK status
If BLOCK, the command is created and run.

Returns: length of copied data (failed if 0)
 *---------------------------------------------------------------------------*/
static int SOCLi_ReadBuffer(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port, SOCLInAddr*  remote_ip,
                            s32 flag_block)
{
    int result;

#ifdef SDK_MY_DEBUG
    OS_TPrintf("\n===\nReadBuffer.buffer                 =%d-%d %d\n", buffer, (u32) buffer + buffer_len, buffer_len);
#endif

// Because we cannot examine the CPS received buffer with SSL,
// we always issue a receive thread command.
    if (SOCL_SocketIsSSL(socket))
    {
        result = SOCLi_ExecReadCommand(socket, buffer, buffer_len, remote_port, remote_ip);
        return result;
    }

    //
// Start off issuing a NOBLOCK read regardless of BLOCK/NOBLOCK status.
// If BLOCK mode and EWOULDBLOCK has been returned, the expectation is that
// data will arrive and a READ command is issued to recv_pipe.
    //
    result = SOCLi_CopyCPSBuffer(socket, buffer, buffer_len, remote_port, remote_ip);

    if (result == SOCL_EWOULDBLOCK && SOCL_FLAGISBLOCK(flag_block))
    {
        result = SOCLi_ExecReadCommand(socket, buffer, buffer_len, remote_port, remote_ip);
    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_CopyCPSBuffer

Description: copies data from the CPS library receive buffer
in a format to attempt a NOBLOCK read

Arguments: socket          socket
buffer          copy target buffer
buffer_len      length of copy target buffer
remote_port     pointer to the recipient's port
remote_ip       pointer to the recipient's IP

Returns: positive value: size of data copied to the TCP buffer
size of data sent to the UDP buffer
0     : EOF
negative value: error
SOCL_EWOULDBLOCK: no data sent
SOCL_EINVAL     : the internal state is wrong
 *---------------------------------------------------------------------------*/
static int SOCLi_CopyCPSBuffer(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port,
                               SOCLInAddr*  remote_ip)
{
    OSIntrMode  enable;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    u8*         cpsbuffer;
    s32         cpsbuffer_len;
    s32         copy_len;
    s32         consume_len;
    u16         my_port;
    u16         his_port;
    SOCLInAddr  his_ip;
    int         result = 0;

    SDK_ASSERT(buffer && buffer_len > 0);

    //
// prevents Consume from being required during data copy
// There is a means for using mutex, but here it is handled in an interrupt.
    //
    enable = OS_DisableInterrupts();
    {
        //
// Confirms the presence of data in the receive buffer, and, if present,
// copies it. If not, and in BLOCK mode, issues a READ command
// and performs a transfer using recv_pipe.
        //
        cpsbuffer = SOCLi_ReadCPSBuffer(socket, &cpsbuffer_len, &my_port, &his_port, &his_ip);

        if (cpsbuffer)
        {
            if (cpsbuffer_len == 0)
            {
// when the pointer is set and the size is 0,
// data has not been received
                result = SOCL_EWOULDBLOCK;
            }
            else
            {
// calculates the data copy size and consume size
//   copies received data up to the size of the buffer
//   TCP removes the copied portion of data from the total data
//   received in UDP
                copy_len = MATH_MIN(buffer_len, cpsbuffer_len);
                consume_len = SOCL_SocketIsTCP(socket) ? copy_len : cpsbuffer_len;

// copy and destroy
// do not destroy when using PEEK reads
                MI_CpuCopy8(cpsbuffer, buffer, (u32) copy_len); // copy
#ifdef SDK_MY_DEBUG
                OS_TPrintf("MI_CPUCOPY    .buffer             =%d-%d %d\n", buffer, (u32) buffer + copy_len, copy_len);
                OS_TPrintf("              .copy_len           =%d\n", copy_len);
#endif
                recv_pipe = socket->recv_pipe;
                SDK_ASSERT(recv_pipe);
                if (!recv_pipe->flag_noconsume)
                {
                    recv_pipe->consumed += consume_len;         // destroy
                }

#ifdef SDK_MY_DEBUG
                OS_TPrintf("              .recv_pipe->consumed=%d\n", recv_pipe->consumed);
#endif
                result = (int)consume_len;
            }
        }
        else
        {
// when no cpsbuffer pointer is returned and the size is 0, EOF
// returns a 0
            if (cpsbuffer_len == 0)
            {
                result = 0;
            }
            else
            {
                result = SOCL_EINVAL;   // system error
            }

            socket->state &= ~(SOCL_STATUS_CONNECTING | SOCL_STATUS_CONNECTED);
        }

        if (result >= 0)
        {
            //
// copies the other party's IP/port
// confirms the local port
            //
            if (remote_port && remote_ip)
            {
                *remote_port = his_port;
                *remote_ip = his_ip;
            }

            if (socket->local_port == 0)
            {
                socket->local_port = my_port;
            }
        }
    }
    (void)OS_RestoreInterrupts(enable);

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ReadCPSBuffer

Description: determines the states of the CPS library receive buffer

Arguments: socket          socket
buffer_len      pointer to the length of the receive buffer
local_port      pointer to the port on the local host
remote_port     pointer to the recipient's port
remote_ip       pointer to the recipient's IP

Returns: value other than 0: leading address of the receive buffer
size is returned to buffer_len
if 0, in standby waiting for data to arrive
0: error or EOF
in this case, returns an error code to buffer_len
if 0, EOF, if a negative value, a system error
 *---------------------------------------------------------------------------*/
static u8* SOCLi_ReadCPSBuffer(SOCLSocket*  socket, s32*  buffer_len, u16*  local_port, u16*  remote_port,
                               SOCLInAddr*  remote_ip)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    CPSSoc*     cpsoc;
    s32         consumed;
    s32         len;

    recv_pipe = socket->recv_pipe;
    cpsoc = (CPSSoc*)OSi_GetSpecificData(&recv_pipe->h.thread, OSi_SPECIFIC_CPS);

    consumed = recv_pipe->consumed;
    len = (s32) cpsoc->rcvbufp - consumed;

    if (len >= 0)
    {
        (*local_port) = cpsoc->local_port;
        (*remote_port) = cpsoc->remote_port;
        (*remote_ip) = (SOCLInAddr) cpsoc->remote_ip;
        (*buffer_len) = len;

// EOF determination
        if (len == 0 && cpsoc->state != CPS_STT_ESTABLISHED)
        {
            return NULL;
        }
    }
    else
    {
        (*buffer_len) = -1;
        return NULL;    // error
    }

    return((u8*)cpsoc->rcvbuf.data) + consumed;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ExecReadCommand

Description: BLOCK READ issuing command

Arguments: s    socket
user_buffer       READ target
user_buffer_len   READ buffer size

Returns: positive value: number of bytes in the received message
negative value: error
 *---------------------------------------------------------------------------*/
static int SOCLi_ExecReadCommand(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port,
                                 SOCLInAddr*  remote_ip)
{
    SOCLiCommandPacket*     command;
    s32 result;

// Throws a command to recv_pipe, and runs CPS_SocConsume
// CPS_SocConsume within the callback function.
// as it uses a forced BLOCK mode, will wait for success
    command = SOCLi_CreateCommandPacket(SOCLi_ReadCallBack, socket, SOCL_FLAGBLOCK_BLOCK);
    SDK_ASSERT(command);

    command->read.buffer = buffer;
    command->read.buffer_len = buffer_len;
    command->read.remote_port = remote_port;
    command->read.remote_ip = remote_ip;

// runs the command
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ReadCallBack

Description: READ process callback.
only during BLOCK mode

Arguments: arg   pointer to the command packet

Returns: positive value: number of bytes in the received message
0: EOF
negative value: error
 *---------------------------------------------------------------------------*/
static int SOCLi_ReadCallBack(void* arg)
{
    SOCLiCommandRead*   cpacket = (SOCLiCommandRead*)arg;
    SOCLSocket*         socket = cpacket->h.socket;
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    u8*         buffer = cpacket->buffer;
    s32         buffer_len = cpacket->buffer_len;
    u16*        remote_port = cpacket->remote_port;
    SOCLInAddr*     remote_ip = cpacket->remote_ip;
    u8*         cpsbuffer;
    u32 cpsbuffer_len;
    s32 consumed;
    int result;

// Waits in BLOCK mode until the new data is added.
// Currently, to implement this functionality in the SOCL layer, polling
// is performed using OS_Sleep; the implementation was revised within
// the CPS layer to end BLOCK mode when new data arrives.
    consumed = recv_pipe->consumed;
    for (;;)
    {
        cpsbuffer = CPS_SocRead(&cpsbuffer_len);
        if (cpsbuffer == NULL || (s32) cpsbuffer_len - consumed > 0)
        {
            break;
        }

        if (SOCL_SocketIsTCP(socket) && socket->cps_socket.state != CPS_STT_ESTABLISHED)
        {
            cpsbuffer = NULL;   // EOF
            break;
        }

        OS_Sleep(10);           // wait 10msec
    }

// because we cannot examine the received buffer with SSL,
// we merely copy the received data from cpsbuffer
// as SSL is always called in BLOCK mode, the process is much simpler
    if (SOCL_SocketIsSSL(socket))
    {
        u32 len;

        if (cpsbuffer == NULL)
        {
            return 0;
        }

        len = MATH_MIN(buffer_len, cpsbuffer_len);
        MI_CpuCopy8(cpsbuffer, buffer, len);
        CPS_SocConsume(len);
        return (int)len;
    }

// the expectation is that the data has already been added to the buffer by CPS_SocRead
// this call therefore immediately ends. when cpsbuffer is 0, EOF
    if (cpsbuffer)
    {
        result = SOCLi_CopyCPSBuffer(socket, buffer, buffer_len, remote_port, remote_ip);
    }
    else
    {
        result = 0 /*EOF*/ ;
    }

// if an error or EOF, end with that return value
    if (result <= 0)
    {
        return result;
    }

    //
// if consumed is at or above its minimum length, the Consume process is performed
    //
    SDK_ASSERT(recv_pipe->consumed_min > 0);
    if (recv_pipe->consumed >= (s32) recv_pipe->consumed_min)
    {
        (void)SOCLi_ConsumeCPSBuffer(socket);
    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ConsumeBuffer

Description: performs the Consume process

Arguments: s    socket

Returns: positive value: number of bytes in the received message
negative value: error
 *---------------------------------------------------------------------------*/
static int SOCLi_ConsumeBuffer(SOCLSocket* socket)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    SOCLiCommandPacket*         command;
    s32 result;

    //
// if consume is less than its minimum length, ends without doing anything
    //
    SDK_ASSERT(recv_pipe->consumed_min > 0);
    if (recv_pipe->consumed < (s32) recv_pipe->consumed_min)
    {
        return SOCL_ESUCCESS;
    }

// Throws a command to recv_pipe, and runs CPS_SocConsume
// within the callback function. process using NOBLOCK
    command = SOCLi_CreateCommandPacket(SOCLi_ConsumeCallBack, socket, SOCL_FLAGBLOCK_NOBLOCK);
    if (NULL == command)
    {
        return SOCL_EMFILE;
    }

// runs the command
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ConsumeCallBack

Description: Consume process callback.

Arguments: arg   pointer to the command packet

Returns: number of bytes in the consumed data
 *---------------------------------------------------------------------------*/
static int SOCLi_ConsumeCallBack(void* arg)
{
    SOCLiCommandConsume*    cpacket = (SOCLiCommandConsume*)arg;

    return SOCLi_ConsumeCPSBuffer(cpacket->h.socket);
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ConsumeCPSBuffer

Description: performs the Consume process

Arguments: recv_pipe the receive pipe

Returns: number of bytes in the consumed data
 *---------------------------------------------------------------------------*/
static int SOCLi_ConsumeCPSBuffer(SOCLSocket* socket)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    OSIntrMode  enable;
    s32         consumed;

    SDK_ASSERT(recv_pipe);

// as the consumed parameters are operated, interrupts are halted
// and the parameters and controls for consumed and CPS_SocConsume are aligned
    enable = OS_DisableInterrupts();
    consumed = recv_pipe->consumed;

    if (consumed)
    {
#ifdef SDK_MY_DEBUG
        OS_TPrintf("\nCPS_SocConsume(%d)\n", consumed);
#endif

        //
// If the timing of the release of the internal buffer within CPS_SocConsume
// occurs before or after blocking, it is necessary to change the time at which recv_pipe->
// consumed is decreased. Be aware that is dependent on the implementation of CPS_SocConsume.
        //
#if 1   // If blocking after releasing the internal buffer, decrease consumed in advance (as at present).
        recv_pipe->consumed = 0;
        CPS_SocConsume((u32) consumed); // BLOCKED
#else // If blocking before releasing the internal buffer, decrease consumed
        CPS_SocConsume((u32) consumed); // BLOCKED
        recv_pipe->consumed -= consumed;
#endif

#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SocConsume.cpssoc.state      =%d\n", socket->cps_socket.state);
        OS_TPrintf("              .cpssoc.rcvbufp    =%d\n", socket->cps_socket.rcvbufp);
        OS_TPrintf("              .recv_pipe.consumed=%d\n", consumed);
#endif
    }
    (void)OS_RestoreInterrupts(enable);

    return (int)consumed;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ReadUdpBuffer

Description: Begins the READ process. However, exceptions controls are not performed.

Arguments: socket          socket
buffer          copy data
buffer_len      length of copy data
remote_port     originating host port (only valid for UDP)
remote_ip     originating host IP (only valid for UDP)
flag_block      flag to indicate BLOCK status
If BLOCK, the command is created and run.

Returns: length of copied data (failed if 0)
 *---------------------------------------------------------------------------*/
static int SOCLi_ReadUdpBuffer(SOCLSocket*  socket, void*  buffer, int buffer_len, u16*  remote_port,
                               SOCLInAddr*  remote_ip, s32 flag_block)
{
    SOCLiSocketRecvCommandPipe*     pipe = socket->recv_pipe;
    SOCLiSocketUdpData*         udpdata = pipe->udpdata.out;
    OSIntrMode  enable;
    int         result;

    enable = OS_DisableInterrupts();
    while (NULL == (udpdata = pipe->udpdata.out))
    {
        if (!SOCL_FLAGISBLOCK(flag_block))
        {
            result = SOCL_EWOULDBLOCK;
            break;
        }

        OS_SleepThread(&pipe->udpdata.waiting);

        if (SOCL_SocketIsInvalid(socket) || !SOCL_SocketIsCreated(socket))
        {
            result = SOCL_ENOTCONN;
            break;  // socket closed during sleep
        }
    }

    if (udpdata)
    {
        MI_CpuCopy8((u8*) &udpdata[1], buffer, (u32) MATH_MIN(buffer_len, udpdata->size));

        if (remote_port)
        {
            (*remote_port) = udpdata->remote_port;
        }

        if (remote_ip)
        {
            (*remote_ip) = udpdata->remote_ip;
        }

        result = udpdata->size;

        if (!pipe->flag_noconsume)
        {
            pipe->udpdata.out = udpdata->next;
            if (udpdata->next == NULL)
            {
                pipe->udpdata.in = NULL;
            }

            SOCLi_Free(udpdata);
            pipe->udpdata.size -= result;
        }
    }
    (void)OS_RestoreInterrupts(enable);

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_UdpRecvCallback

Description: callback at UDP packet receipt

Arguments: data  pointer to the data
len  the data length
soc   pointer to the CPSSoc structure

Returns:1 : destroys the buffer
 *---------------------------------------------------------------------------*/
int SOCLi_UdpRecvCallback(u8* data, u32 len, CPSSoc* soc)
{
    SOCLSocket*     socket = (SOCLSocket*)soc;
    SOCLiSocketRecvCommandPipe*     pipe = socket->recv_pipe;
    SOCLiSocketUdpData*         udpheader;
    OSIntrMode  enable = OS_DisableInterrupts();

    if (pipe->udpdata.size_max >= pipe->udpdata.size + len)
    {
// copy to the receive buffer
        udpheader = SOCLi_Alloc(sizeof(SOCLiSocketUdpData) + len);

        if (udpheader)
        {
            pipe->udpdata.size += len;
            udpheader->next = NULL;
            udpheader->size = (u16) len;
            udpheader->remote_port = soc->remote_port;
            udpheader->remote_ip = soc->remote_ip;
            MI_CpuCopy8(data, (void*) &udpheader[1], len);

// confirms the local port
            if (socket->local_port == 0)
            {
                socket->local_port = soc->local_port;
            }

// receipt preparations for the next data
            soc->remote_port = soc->remote_port_bound;
            soc->remote_ip = soc->remote_ip_bound;

            if (pipe->udpdata.in)
            {
                pipe->udpdata.in->next = udpheader;
            }

            pipe->udpdata.in = udpheader;

            if (pipe->udpdata.out == NULL)
            {
                pipe->udpdata.out = udpheader;
            }
        }
        else
        {
            SOCLi_CheckCount[SOCL_CHECKCOUNT_UDPRCVCB_1]++;
        }
    }
    else
    {
        SOCLi_CheckCount[SOCL_CHECKCOUNT_UDPRCVCB_2]++;
    }

// launch if there are waiting threads
    OS_WakeupThread(&pipe->udpdata.waiting);

    (void)OS_RestoreInterrupts(enable);

    return 1;   // data destroyed
}
