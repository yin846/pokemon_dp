/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_write.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_write.c,v $
Revision 1.34  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.33  01/17/2006 07:26:22  yasu
Created 2005 ->Changed 2006

Revision 1.32  01/17/2006 07:24:36  yasu
Re-added the correction to the issue in TCP BLOCK Write.
introduced a macro for determining BLOCK mode

Revision 1.31  10/20/2005 09:08:14  yasu
Corrected a problem where the write buffer was not being freed when a transmission failed.

Revision 1.30  10/18/2005 06:12:44  terui
Made changes related to increasing TCP send throughput

Revision 1.29  09/19/2005 15:11:37  yasu
minor adjustments to part added to closing state

Revision 1.28  09/19/2005 14:08:29  yasu
added a closing state

Revision 1.27  09/19/2005 04:51:49  seiki_masashi
small fix

Revision 1.26  09/17/2005 11:54:52  yasu
typo correction

Revision 1.25  09/17/2005 10:14:00  yasu
made handling at SocWrite errors more strict

Revision 1.24  09/13/2005 07:27:06  yada
applied for connect() on UDP

Revision 1.23  09/08/2005 05:35:34  yasu
moved the ability to determine whether the socket scripter was of a valid value to the SOCL

Revision 1.22  08/24/2005 09:25:13  yasu
added SOCL_SocketIsInvalid

Revision 1.21  08/11/2005 10:06:09  yasu
reflect the UDP bind result at write in the CPS layer

Revision 1.20  08/11/2005 07:56:32  yasu
explicitly get E port at UDP write

Revision 1.19  08/11/2005 07:30:03  yasu
revise error handling for pipe transmission for the write command

Revision 1.18  08/11/2005 05:08:51  yasu
revise link buffer management

Revision 1.17  08/10/2005 12:34:04  yasu
changed buffer operations

Revision 1.16  08/10/2005 04:51:12  yasu
revised process when link buffer is full

Revision 1.15  08/09/2005 12:27:12  yasu
revised data copy from link buffer process

Revision 1.14  08/09/2005 07:43:29  yasu
compile support for LANG=en

Revision 1.13  08/09/2005 04:31:10  yasu
revised validity detection for ASSERT process

Revision 1.12  08/09/2005 03:20:39  yasu
deleted mixed debug message

Revision 1.11  08/09/2005 01:21:02  yasu
revised sndbuf size limit process

Revision 1.10  08/08/2005 14:16:36  yasu
support for TCP send process

Revision 1.9  08/01/2005 07:02:50  seiki_masashi
revised to return EWOULDBLOCK when there is no free space in the buffer at non-blocking

Revision 1.8  08/01/2005 04:55:25  yasu
revised the SOCL_Write returned value

Revision 1.7  07/30/2005 22:30:14  yasu
revised so demo would work

Revision 1.6  07/30/2005 15:30:52  yasu
revisions in conjunction with command pipe separation

Revision 1.5  07/28/2005 11:16:12  yasu
excised shareable code to functions

Revision 1.4  07/27/2005 12:19:00  yasu
added sendto

Revision 1.3  07/25/2005 10:15:01  yasu
added the SOC library

Revision 1.2  07/22/2005 12:44:56  yasu
pseudo-implementation of asynchronous processing

Revision 1.1  07/19/2005 14:06:33  yasu
added SOCL_Read/Write

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_WriteBuffer(SOCLSocket* , const u8* , s32, u16, SOCLInAddr, s32);
static int  SOCLi_ExecWriteCommand(SOCLSocket* , const u8* , s32, s32, u16, SOCLInAddr, s32);
static int  SOCLi_WriteCallBack(void* );
static s32  SOCLi_AllocWriteBuffer(SOCLSocket* , s32, s32, s32* , s32);
static int  SOCLi_MemCpy(u8* , int, SOCLiCommandWrite* );

#if SOCL_OPTIMIZE_TCP_SEND
static int  SOCLi_GetOptimumSendBufLen(int);
#endif

/*---------------------------------------------------------------------------*
Name: SOCL_Write

Description: Send the data from the socket.

Arguments: socket          socket
buffer          data to be sent
buffer_len      length of data to be sent
remote_port     target host port
remote_ip       target host IP

remote_port/remote_ip are only valid when UDP is not performing
Connect. Values are ignored when UDP is performing TCP or Connect

Returns: positive value: number of bytes in the received message
negative value: error
 *---------------------------------------------------------------------------*/
int SOCL_Write(int s, const void* buffer, int buffer_len, u16 remote_port, SOCLInAddr remote_ip)
{
    return SOCL_WriteTo(s, buffer, buffer_len, remote_port, remote_ip, 0);
}

/*---------------------------------------------------------------------------*
Name: SOCL_WriteNoBlock

Description: Send the data in NOBLOCK mode from the socket.

Arguments: socket          socket
buffer          data to be sent
buffer_len      length of data to be sent
remote_port     target host port (only valid for UDP)
remote_ip     target host IP (only valid for UDP)

Returns: positive value: number of bytes in the sent message
negative value: error
 *---------------------------------------------------------------------------*/
int SOCL_WriteNoBlock(int s, const void* buffer, int buffer_len, u16 remote_port, SOCLInAddr remote_ip)
{
    return SOCL_WriteTo(s, buffer, buffer_len, remote_port, remote_ip, SOCL_MSG_DONTWAIT);
}

/*---------------------------------------------------------------------------*
Name: SOCL_WriteBlock

Description: Send the data in BLOCK mode from the socket.

Arguments: socket          socket
buffer          data to be sent
buffer_len      length of data to be sent
remote_port     target host port (only valid for UDP)
remote_ip     target host IP (only valid for UDP)

Returns: positive value: number of bytes in the sent message
negative value: error
 *---------------------------------------------------------------------------*/
int SOCL_WriteBlock(int s, const void* buffer, int buffer_len, u16 remote_port, SOCLInAddr remote_ip)
{
    return SOCL_WriteTo(s, buffer, buffer_len, remote_port, remote_ip, 0);
}

/*---------------------------------------------------------------------------*
Name: SOCL_WriteTo

Description: Send the data from the socket.

Arguments: socket          socket
buffer          data to be sent
buffer_len      length of data to be sent
remote_port     target host port
remote_ip       target host IP
flag            send flag

remote_port/remote_ip are only valid when UDP is not performing
Connect. Values are ignored when UDP is performing TCP or Connect

Returns: positive value: number of bytes in the received message
negative value: error
 *---------------------------------------------------------------------------*/
int SOCL_WriteTo(int s, const void* buffer, int buffer_len, u16 remote_port, SOCLInAddr remote_ip, int flags)
{
    SOCLSocket*     socket = (SOCLSocket*)s;
    SOCLiSocketSendCommandPipe*     send_pipe;
    int flag_block;
    int result;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;     // non-regulated value
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
    send_pipe = socket->send_pipe;
    SDK_ASSERT(send_pipe);      // initialization method is off

    //
// Reject access from other threads after locking.
// there is a particular need for careful exceptions for processes that access within the buffer
    //
    if ((flags & SOCL_MSG_DONTWAIT) || SOCL_SocketIsNoBlock(socket))
    {
        if (!OS_TryLockMutex(&send_pipe->h.in_use))
        {
            return SOCL_EWOULDBLOCK;
        }

        flag_block = SOCL_FLAGBLOCK_NOBLOCK;
    }
    else
    {
        OS_LockMutex(&send_pipe->h.in_use);
        flag_block = SOCL_FLAGBLOCK_BLOCK;
    }

    result = SOCLi_WriteBuffer(socket, buffer, buffer_len, remote_port, remote_ip, flag_block);
    OS_UnlockMutex(&send_pipe->h.in_use);

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_WriteBuffer

Description: Begins the WRITE process. However, exceptions controls are not performed.

Arguments: socket          socket
buffer          copy data
buffer_len      length of copy data
remote_port     target host port (only valid for UDP)
remote_ip     target host IP (only valid for UDP)
flag_block      flag to indicate BLOCK status

Returns: length of copied data (failed if 0)
 *---------------------------------------------------------------------------*/
static int SOCLi_WriteBuffer(SOCLSocket*  socket, const u8*  buffer, s32 buffer_len, u16 remote_port,
                             SOCLInAddr remote_ip, s32 flag_block)
{
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
    s32 sndbuf_len;
    s32 wrtbuf_in;
    s32 write_len;
    s32 buffer_min;
    s32 copied_len = 0;
#ifdef SDK_DEBUG
    s32 wrtbuf_size = (s32) send_pipe->buffer.area.size - 1;
#endif
    int result;

    SDK_ASSERT(send_pipe);              // initialization method is off
    SDK_ASSERT(send_pipe->exe_socket);  // initialization method is off

// gets the send slot (sndbuf) buffer size
    sndbuf_len = (s32) send_pipe->exe_socket->cps_socket.sndbuf.size;

    if (SOCL_SocketIsUDP(socket))
    {
        sndbuf_len -= SOCL_UDP_SNDBUF_NOCOPY_OFFSET;

// for UDP, when the transfer size is bigger than sndbuf/wrtbuf
// transfer will not take place
// confirms the sndbuf size for CPS socket connected to SEND PIPE
        SDK_ASSERT(sndbuf_len >= 0);

        if (buffer_len > sndbuf_len)
        {
            return SOCL_EMSGSIZE;
        }

// a sufficiently large buffer is needed to hold data for a single UDP
// transfer to be performed
        buffer_min = buffer_len;
    }
    else
    {
        sndbuf_len -= SOCL_TCP_SNDBUF_NOCOPY_OFFSET;

// there are two policies that divide if the buffer is small
// for TCP transfer
#ifdef SDK_TCPSEND_DOIT_SOON
// fill if even a small buffer is open at NOBLOCK
        buffer_min = (!SOCL_FLAGISBLOCK(flag_block)) ? 1 : MATH_MIN(buffer_len, sndbuf_len);
#else
// cannot fill even at NOBLOCK
        buffer_min = MATH_MIN(buffer_len, sndbuf_len);
#endif
    }

    SDK_ASSERT(wrtbuf_size >= sndbuf_len);

    while (buffer_len > 0)
    {
        //
// holds the buffer
//   BLOCK until at least a size is returned equal to buffer_min at BLOCK
//   returns a 0 when even the buffer_min cannot be got at NOBLOCK
        write_len = SOCLi_AllocWriteBuffer(socket, buffer_len, buffer_min, &wrtbuf_in, flag_block);

        //
// begin command process if buffer can be held
        //
        if (write_len > 0)
        {
            result = SOCLi_ExecWriteCommand(socket, buffer, write_len, wrtbuf_in, remote_port, remote_ip, flag_block);

            if (result <= 0)
            {
// the SOCLi_ExecWriteCommand will be in ERROR only at NOBLOCK
                return SOCL_EWOULDBLOCK;
            }

            buffer += write_len;
            buffer_len -= write_len;
            copied_len += write_len;
        }

        //
// at NOBLOCK end after one loop
        //
        if (!SOCL_FLAGISBLOCK(flag_block))
        {
            if (write_len <= 0)
            {
// notify that there is no free space in the buffer
                return SOCL_EWOULDBLOCK;
            }
            break;
        }
    }

    return (int)copied_len;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_AllocWriteBuffer

Description: Gets the size needed for the transfer buffer.

BLOCK until buffer can be held for the requested size.
When 0 is specified, handled as NOBLOCK.

Arguments: socket          socket
max_size             maximum buffer to be got
min_size             minimum buffer to be got
wrtbuf_in_ptr        buffer input head
flag_block           BLOCK until buffer got?

Returns: length of available buffer
 *---------------------------------------------------------------------------*/
static s32 SOCLi_AllocWriteBuffer(SOCLSocket* socket, s32 max_size, s32 min_size, s32* wrtbuf_in_ptr, s32 flag_block)
{
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
#ifdef SDK_DEBUG
    s32         wrtbuf_size = (s32) send_pipe->buffer.area.size - 1;
#endif
    s32         free_size;
    OSIntrMode  enable;

    SDK_ASSERT(send_pipe);
    SDK_ASSERT(wrtbuf_size >= min_size);

    //
// if the minimum and maximum sizes are contradictory, maximum size takes preference
// data greater than the maximum size is absolutely not necessary
// the minimum size is set to improve transfer efficiency
    //
    if (min_size > max_size)
    {
        min_size = max_size;
    }

    //
// gets a pointer to the link buffer
//   wrtbuf_in is for input/wrtbuf_out is for output
    //
    enable = OS_DisableInterrupts();
    while (1)
    {
        free_size = SOCLi_GetWriteBufferFreeSize(socket);

        //
// ends if sufficient buffer length is available
// sets the upper limit to not exceed the max_size
        //
        if (free_size >= min_size)
        {
            if (free_size >= max_size)
            {
                free_size = max_size;
            }

            *wrtbuf_in_ptr = (s32) send_pipe->buffer.in;
            break;
        }

        //
// if NOBLOCK, ends without BLOCK and got size of 0
        //
        if (!SOCL_FLAGISBLOCK(flag_block))
        {
            free_size = 0;
            break;
        }

        //
// wait until buffer is available if sufficient buffer is not ready
//   WriteCallBack must be made to not run from the beginning
//   of the determination that there is not enough buffer size until
//   entering the wait for available buffer within the SleepThread
        //
        OS_SleepThread(&send_pipe->buffer.waiting);

    }
    (void)OS_RestoreInterrupts(enable);

// returns the link buffer available memory
    return free_size;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_GetWriteBufferFreeSize

Description: Returns the available buffer size.
This function must be called during an interrupt disabled state.

Arguments: socket          socket

Returns: available buffer size
 *---------------------------------------------------------------------------*/
s32 SOCLi_GetWriteBufferFreeSize(SOCLSocket* socket)
{
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
    s32 wrtbuf_size;
    s32 wrtbuf_in;
    s32 wrtbuf_out;
    s32 available_len;

    SDK_ASSERT(send_pipe);

    wrtbuf_size = (s32) send_pipe->buffer.area.size;
    wrtbuf_in = (s32) send_pipe->buffer.in;
    wrtbuf_out = (s32) send_pipe->buffer.out;
    available_len = wrtbuf_out - wrtbuf_in - 1;

    if (available_len < 0)
    {
        available_len += wrtbuf_size;
    }

    return available_len;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ExecWriteCommand

Description: Begins the WRITE process. However, exceptions controls are not performed.
In addition it is necessary to confirm in advance that the data
can be copied to the link buffer.

Arguments: socket          socket
buffer          copy data
buffer_len      length of copy data
wrtbuf_in             link buffer insert position
remote_port     target host port (only valid for UDP)
remote_ip     target host IP (only valid for UDP)

Returns: length of copied data (failed if 0)
 *---------------------------------------------------------------------------*/
static int SOCLi_ExecWriteCommand(SOCLSocket*  socket, const u8*  buffer, s32 buffer_len, s32 wrtbuf_in, u16 remote_port,
                                  SOCLInAddr remote_ip, s32 flag_block)
{
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
    SOCLiCommandPacket*         command;
    u8*     wrtbuf;
    s32     wrtbuf_size;
    int     result;
    u16     saved_send_buffer_in;

#ifdef SDK_MY_DEBUG
    OS_TPrintf("socket=%08x exe_socket=%08x\n", socket, send_pipe->exe_socket);
#endif

// Throws a command to the send thread, and launches CPS_Write
// within the callback function.
//    Getting the command packet is processed in the mode based on flag_block
//    but the write process is performed in NORESULT.
    //
    command = SOCLi_CreateCommandPacket(SOCLi_WriteCallBack, send_pipe->exe_socket, flag_block);
    if (NULL == command)
    {
        return SOCL_EMFILE;
    }

    if (SOCL_FLAGISBLOCK(flag_block) && !SOCL_SocketIsUDP(socket))
    {
        command->h.flag_block = SOCL_FLAGBLOCK_NORESULT;
    }
    else
    {
        command->h.flag_block = SOCL_FLAGBLOCK_NOBLOCK;
    }

    wrtbuf = (u8*)send_pipe->buffer.area.data;
    wrtbuf_size = (s32) send_pipe->buffer.area.size;

// data is transferred in order to WriteBuffer
//    Supports both the sending of a single chunk and the need to
//    split into two chunks.
    //
    if (wrtbuf_in + buffer_len < wrtbuf_size)
    {
        command->write.buffer1 = wrtbuf + wrtbuf_in;
        command->write.buffer1_len = buffer_len;
        command->write.buffer2 = NULL;
        command->write.buffer2_len = 0;
        wrtbuf_in += buffer_len;
    }
    else
    {
        const u8*   buffer2;

        command->write.buffer1 = wrtbuf + wrtbuf_in;
        command->write.buffer1_len = wrtbuf_size - wrtbuf_in;
        command->write.buffer2 = wrtbuf;
        command->write.buffer2_len = buffer_len - command->write.buffer1_len;
        wrtbuf_in = command->write.buffer2_len;
        buffer2 = buffer + command->write.buffer1_len;
        MI_CpuCopy8(buffer2, command->write.buffer2, (u32) command->write.buffer2_len);
    }

    MI_CpuCopy8(buffer, command->write.buffer1, (u32) command->write.buffer1_len);

// moves the link buffer pointer
    saved_send_buffer_in = send_pipe->buffer.in;
    send_pipe->buffer.in = command->write.wrtbuf_after = (u16) wrtbuf_in;

    //
// when UDP communication is not connected and the destination
// is specified by the write argument, specifies the receiver in the command packet
    //
// UDP?
    if (SOCL_SocketIsUDP(socket))
    {
        if (socket->local_port == 0)
        {
            socket->local_port = CPS_SocGetEport(); // bind if not bound
            socket->cps_socket.local_port = socket->local_port;
        }

        command->write.local_port = socket->local_port;

// non-connected UDP
        if (socket->remote_ip == 0 || remote_ip != 0)
        {
            command->write.remote_ip = remote_ip;
            command->write.remote_port = remote_port;
        }

// connected UDP
        else
        {
            command->write.remote_ip = socket->remote_ip;
            command->write.remote_port = socket->remote_port;
        }
    }
    else
    {
        command->write.remote_ip = 0;               // no bind
    }

    result = SOCLi_ExecCommandPacketInSendPipe(send_pipe->exe_socket, command);

    if (result != SOCL_ESUCCESS)
    {
// Fails during NOBLOCK
// Set 0 for the return value and free the buffer.
        send_pipe->buffer.in = saved_send_buffer_in;
        buffer_len = 0;
    }

    return (int)buffer_len;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_WriteCallBack

Description: WRITE process callback.

Arguments: arg   pointer to the command packet

Returns: positive value: number of bytes in the sent message
negative value: error
 *---------------------------------------------------------------------------*/
static int SOCLi_WriteCallBack(void* arg)
{
    SOCLiCommandWrite*  cpacket = (SOCLiCommandWrite*)arg;
    SOCLSocket*         socket = cpacket->h.socket;
    SOCLiSocketSendCommandPipe*     send_pipe = socket->send_pipe;
    u8*     sndbuf;
    int     sndbuf_len;
    int     sndbuf_offset;
    int     copied = 0;
    int     result;

    SDK_ASSERT(send_pipe);

    if ((!SOCL_SocketIsTCP(socket)) || SOCL_SocketIsConnected(socket))  // process continues when closing
    {
// bind process performed when remote_ip is specified
//   used for UDP connection
        if (cpacket->remote_ip)
        {
#ifdef SDK_MY_DEBUG
            OS_TPrintf("WriteCallBack-BIND:%d %d %08x\n", cpacket->local_port, cpacket->remote_port, cpacket->remote_ip);
#endif
            CPS_SocBind(cpacket->local_port, cpacket->remote_port, cpacket->remote_ip);
        }

        //
// Gets the data from the link buffer.
// Copies to sndbuf on its own and then calls the SocWrite function.
// as it is serialized in the message queue, sndbuf is excepted
// even without mutex
        //
#ifdef SDK_MY_DEBUG
        OS_TPrintf("WriteCallBack-WRITE 1: %08x %d\n", cpacket->buffer1, cpacket->buffer1_len);
        OS_TPrintf("                    2: %08x %d\n", cpacket->buffer2, cpacket->buffer2_len);
        OS_DumpThreadList();
        OS_TPrintf("  CPS->rcvbuf %08x%5d\n", socket->cps_socket.rcvbuf.data, socket->cps_socket.rcvbuf.size);
        OS_TPrintf("  CPS->sndbuf %08x%5d\n", socket->cps_socket.sndbuf.data, socket->cps_socket.sndbuf.size);
        OS_TPrintf("  CPS->linbuf %08x%5d\n", socket->cps_socket.linbuf.data, socket->cps_socket.linbuf.size);
        OS_TPrintf("  CPS->outbuf %08x%5d\n", socket->cps_socket.outbuf.data, socket->cps_socket.outbuf.size);
#endif

        sndbuf_offset = SOCL_SocketIsTCP(socket) ? SOCL_TCP_SNDBUF_NOCOPY_OFFSET : SOCL_UDP_SNDBUF_NOCOPY_OFFSET;
        sndbuf = socket->cps_socket.sndbuf.data + sndbuf_offset;
#if SOCL_OPTIMIZE_TCP_SEND
        if (SOCL_SocketIsTCP(socket))
        {
            /* To improve send throughput, optimize data size attempted in a single transmission */
            sndbuf_len = SOCLi_GetOptimumSendBufLen((int)socket->cps_socket.sndbuf.size - sndbuf_offset);
        }
        else
        {
            sndbuf_len = (int)socket->cps_socket.sndbuf.size - sndbuf_offset;
        }

#else
        sndbuf_len = (int)socket->cps_socket.sndbuf.size - sndbuf_offset;
#endif

        for (;;)
        {
            result = SOCLi_MemCpy(sndbuf, sndbuf_len, cpacket);
            if (result <= 0)
            {
                break;
            }

#ifdef SDK_MY_DEBUG
            OS_TPrintf("CPS_SocWrite.sendbuf      =%d\n", sndbuf);
            OS_TPrintf("            .sendlen      =%d\n", result);
#endif
            result = (int)CPS_SocWrite(sndbuf, (u32) result);

#ifdef SDK_MY_DEBUG
            OS_TPrintf("CPS_SocWrite.result       =%d\n", result);
#endif

            if (result <= 0)
            {
// when the connection is lost
                if (SOCL_SocketIsTCP(socket))
                {
// You need to reconnect to process.
                    socket->state &= ~(SOCL_STATUS_CONNECTING | SOCL_STATUS_CONNECTED | SOCL_STATUS_CLOSING);
                }

                copied = SOCL_ETIMEDOUT;
                break;
            }

            copied += result;
        }
    }
    else
    {
        copied = SOCL_ETIMEDOUT;    // connection lost midway
    }

#ifdef SDK_MY_DEBUG
    OS_TPrintf("WriteCallBack-WRITE.copied=%d\n", copied);
#endif

// releases the link buffer, and wake any threads waiting on release
// pay attention to exception processes in order to receive this message
// clear the buffer even if errors are found
    send_pipe->buffer.out = cpacket->wrtbuf_after;
    OS_WakeupThread(&send_pipe->buffer.waiting);

    return copied;
}

#if SOCL_OPTIMIZE_TCP_SEND

/*---------------------------------------------------------------------------*
Name: SOCLi_GetOptimumSendBufLen

Description: Gets the size of send data that is given to CPS_SocWrite for obtaining optimal
                throughput during TCP transmission.

Arguments: len     -   The max size of the work buffer.

Returns: int - Returns the optimal send data size.
 *---------------------------------------------------------------------------*/
static int SOCLi_GetOptimumSendBufLen(int len)
{
    CPSSoc*     soc = CPS_CURSOC;
    int         temp = 0;
    OSIntrMode  enabled = OS_DisableInterrupts();

    if (soc != NULL)
    {
        if ((soc->remote_mss > 0) && (soc->remote_win > 0))
        {
            temp = 2 * MATH_IMin(MATH_IMin(soc->remote_mss, soc->remote_win), (int)SOCLiCPSConfig.mymss);
        }
        else
        {
            temp = 2 * (int)SOCLiCPSConfig.mymss;
        }
    }
    else
    {
        temp = 2 * (int)SOCLiCPSConfig.mymss;
    }
    (void)OS_RestoreInterrupts(enabled);

    if (temp > 0)
    {
        int count = len / temp;

        if (count > 0)
        {
            return temp * count;
        }
    }

    return len;
}
#endif

/*---------------------------------------------------------------------------*
Name: SOCLi_MemCpy

Description: COPY process for data that spans two buffers

Arguments: socket          socket
cpacket buffer data parameters

Returns: positive number or 0: number of bytes in the sent data
 *---------------------------------------------------------------------------*/
static int SOCLi_MemCpy(u8* sndbuf_ptr, int sndbuf_len, SOCLiCommandWrite* cpacket)
{
    s32 buffer1_len = cpacket->buffer1_len;
    s32 buffer2_len = cpacket->buffer2_len;

    if (buffer1_len > sndbuf_len)
    {
// if there is enough room in a part of buffer1
        buffer1_len = sndbuf_len;
        buffer2_len = 0;
    }
    else if (buffer2_len > sndbuf_len - buffer1_len)
    {
// if there is enough room in all of buffer1 and a part of buffer2
        buffer2_len = sndbuf_len - buffer1_len;
    }
    else
    {
// if there is not enough room in all of buffer1 and buffer2
// buffer1_len and buffer2_len are OK as is
    }

    if (buffer1_len > 0)
    {
        MI_CpuCopy8(cpacket->buffer1, sndbuf_ptr, (u32) buffer1_len);
        cpacket->buffer1 += buffer1_len;
        cpacket->buffer1_len -= buffer1_len;
    }

    if (buffer2_len > 0)
    {
        MI_CpuCopy8(cpacket->buffer2, sndbuf_ptr + buffer1_len, (u32) buffer2_len);
        cpacket->buffer2 += buffer2_len;
        cpacket->buffer2_len -= buffer2_len;
    }

    return buffer1_len + buffer2_len;
}
