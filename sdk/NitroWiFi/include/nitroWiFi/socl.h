/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - include
File: socl.h

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl.h,v $
Revision 1.65  03/10/2006 09:20:22  kitase_hirotake
  INDENT SOURCE

Revision 1.64  01/25/2006 13:21:45  yasu
Changed the Thread command queue size from 8 to 32.

Revision 1.63  12/06/2005 11:54:51  yasu
addressed issue in TCP BLOCK Write
introduced a macro for determining BLOCK mode

Revision 1.62  10/26/2005 00:52:53  seiki_masashi
Added SOCL_Set/GetSslHandshakePriority function

Revision 1.61  10/18/2005 06:12:44  terui
Made changes related to increasing TCP send throughput

Revision 1.60  10/14/2005 01:57:06  yasu
Corrected a problem where close processing was incomplete whenever the command queue was full.

Revision 1.59  09/28/2005 11:39:56  yasu
cleaned up the code
added SOCL_CalmDown()

Revision 1.58  09/27/2005 14:18:54  yasu
asynchronous support for SOC_Close

Revision 1.57  09/21/2005 11:19:31  yasu
added SOCL_SetYieldWait()

Revision 1.56  09/19/2005 14:08:51  yasu
added a closing state

Revision 1.55  09/17/2005 03:05:05  seiki_masashi
avoided \x5c at line ends

Revision 1.54  09/16/2005 12:13:30  yasu
activated the event counter for FinalRom as well

Revision 1.53  09/16/2005 05:11:34  yasu
added RequestedIP setting API

Revision 1.52  09/15/2005 13:13:17  yasu
DHCP Requested IP support

Revision 1.51  09/14/2005 06:18:49  seiki_masashi
added SOCL_AddRandomSeed function

Revision 1.50  09/14/2005 06:00:50  yasu
SOCLi_CheckCount support

Revision 1.49  09/08/2005 05:35:45  yasu
moved the ability to determine whether the socket scripter was of a valid value to the SOCL

Revision 1.48  09/02/2005 08:45:31  yasu
prepared an API that can change IP address settings within SOCLConfig

Revision 1.47  09/01/2005 06:40:26  yasu
consolidated processes determining whether a socket is active

Revision 1.46  09/01/2005 06:12:54  yasu
consolidated processes determining whether a socket is active

Revision 1.45  08/26/2005 03:48:52  yasu
revisions made in conjunction with changes to the SOCL_GetStatus UDP receiving system

Revision 1.44  08/25/2005 08:28:05  yasu
added buffering process for UDP receiving

Revision 1.43  08/24/2005 09:22:13  yasu
added SOCL_SocketIsInvalid

Revision 1.42  2005/08/23 02:43:12  adachi_hiroaki
temporarily set RWIN to 8x of MSS

Revision 1.41  08/22/2005 04:32:07  yasu
revised the size for UDP cps recvbuf

Revision 1.40  08/22/2005 03:24:30  yasu
changes such that the defined values for sndbuf, etc., are calculated from MTU_MAX, etc.

Revision 1.39  08/19/2005 05:59:45  yasu
changed the MTU default value to 576

Revision 1.38  08/19/2005 05:06:54  yasu
mtu/rwin support

Revision 1.37  08/18/2005 13:54:12  yasu
added socket list operations

Revision 1.36  08/18/2005 08:55:37  yasu
added SOCL_EALREADY

Revision 1.35  08/17/2005 11:49:03  yasu
SOC_InetAtoN revisions

Revision 1.34  08/17/2005 04:10:39  yasu
added SOC_Poll functionality

Revision 1.33  08/13/2005 09:37:53  yasu
added consumed_min controls

Revision 1.32  08/12/2005 09:53:45  yasu
cleaned up the API for accessing socket-internal flags

Revision 1.31  08/11/2005 08:33:38  yasu
revised priorities

Revision 1.30  08/10/2005 13:42:23  yasu
changed the buffer size

Revision 1.29  08/10/2005 09:33:51  seiki_masashi
added SSL support

Revision 1.28  08/10/2005 02:12:55  yasu
revised priorities for receiving/sending threads

Revision 1.27  08/09/2005 12:27:46  yasu
changed the TCP wrtbuf size

Revision 1.26  08/09/2005 09:53:36  yasu
revised the maximum UDP datagram length

Revision 1.25  08/09/2005 01:21:31  yasu
revised the wrtbuf size

Revision 1.24  08/08/2005 14:15:59  yasu
CPS thread priority settings

Revision 1.23  08/08/2005 05:59:32  seiki_masashi
added SSL-specific constant definitions

Revision 1.22  08/07/2005 13:08:45  yasu
displayed names for error codes

Revision 1.21  08/06/2005 11:56:43  seiki_masashi
added SOCL_EnableSsl

Revision 1.20  08/05/2005 05:05:57  seiki_masashi
given its reference in cps.h, added #include <nitro/math.h>

Revision 1.19  08/02/2005 06:17:18  yasu
converted SOCL_GetHostID to a function

Revision 1.18  08/02/2005 05:06:30  yasu
revisions such that Error is not returned as a ConfigError within DHCP

Revision 1.17  08/01/2005 14:05:38  seiki_masashi
added EAGAIN

Revision 1.16  08/01/2005 13:26:00  yasu
revised thread-specific parameters
added SOCL_Listen/Accept

Revision 1.15  08/01/2005 08:52:58  yasu
added SOCL_EWOULDBLOCK

Revision 1.14  08/01/2005 04:56:36  yasu
decreased the SOC thread priority

Revision 1.13  07/30/2005 22:55:18  yasu
changed the name of the conditional macro

Revision 1.12  07/30/2005 22:30:59  yasu
revisions to make the code work again

Revision 1.11  07/30/2005 15:34:39  yasu
revisions in conjunction with command pipe separation

Revision 1.10  07/28/2005 11:15:17  yasu
implemented SOC_Poll

Revision 1.9  07/27/2005 12:23:07  yasu
added sendto

Revision 1.8  07/27/2005 11:59:48  yasu
added SOC functions

Revision 1.7  07/26/2005 00:08:01  yasu
binding for SOC_Startup and SOCL_Startup

Revision 1.6  07/25/2005 10:15:01  yasu
added the SOC library

Revision 1.5  07/22/2005 13:57:13  yasu
added listen accept

Revision 1.4  07/22/2005 12:45:15  yasu
pseudo-implementation of asynchronous processing

Revision 1.3  07/19/2005 14:05:14  yasu
added SOCL_Read/Write

Revision 1.2  07/18/2005 13:16:45  yasu
created send/receive threads

Revision 1.1  07/15/2005 13:32:51  yasu
adjusted header for soc library

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_SOCL_H_
#define NITROWIFI_SOCL_H_

#include <nitro/types.h>
#include <nitro/os.h>
#include <nitro/math.h>
#include <nitroWiFi/cps.h>
#include <nitroWiFi/ssl.h>
#include <nitroWiFi/soc_errcode.h>

#ifdef __cplusplus

extern "C" {
#endif

//---------------------------------------------------------------------------
//  SOCL library settings
//---------------------------------------------------------------------------
//  general use
typedef CPSInAddr   SOCLInAddr;
#define SOCL2CPSInAddr(socl)    ((CPSInAddr) (socl))

//  Send throughput optimization switch
#define SOCL_OPTIMIZE_TCP_SEND  1

//---------------------------------------------------------------------------

//  socket structure

//---------------------------------------------------------------------------

// link buffer
typedef struct SOCLiSocketRingBuffer
{
    CPSSocBuf       area;   // struct {u32 size; u8* data;}
    vu16        in;
    vu16        out;
    OSThreadQueue   waiting;

} SOCLiSocketRingBuffer;

// UDP data header
typedef struct SOCLiSocketUdpData
{
    struct SOCLiSocketUdpData*  next;
    u16         size;
    u16         remote_port;
    SOCLInAddr  remote_ip;

} SOCLiSocketUdpData;

// UDP data header control block
typedef struct SOCLiSocketUdpDataControl
{
    SOCLiSocketUdpData * volatile in;
    SOCLiSocketUdpData * volatile out;
    vu16        size;
    u16         size_max;
    OSThreadQueue   waiting;

} SOCLiSocketUdpDataControl;

// command pipe
typedef struct SOCLSocketCommandPipe
{
    OSMessageQueue  queue;  // command queue
    OSThread        thread; // command execution thread
    OSMutex         in_use; // for command insertion exclusion adjustment
} SOCLiSocketCommandPipe;

// command pipe for receiving
typedef struct SOCLiSocketRecvCommandPipe
{
    SOCLiSocketCommandPipe  h;
    vs32    consumed;       // the volume of the consumed receiving buffer
    u16     consumed_min;   // minimum consume
    s8      flag_noconsume; // CONSUME or not
    u8      padding[1];
    SOCLiSocketUdpDataControl   udpdata;

// consumed points to u16 sndbuf.size but
// given that consumed can temporarily become negative as part of the calculation process,
// it should be vs32, not vu16, and as consumed_min is a fixed value, it should be u16
} SOCLiSocketRecvCommandPipe;

// command pipe for sending
typedef struct SOCLiSocketSendCommandPipe
{
    SOCLiSocketCommandPipe  h;
    SOCLiSocketRingBuffer   buffer;     // struct {u32 size; u8* data;}
    struct SOCLSocket*      exe_socket; // send run socket
} SOCLiSocketSendCommandPipe;

// socket
typedef struct SOCLSocket
{
    CPSSoc  cps_socket;         // always place at the header (for reference by UDP arriving Callback)
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    SOCLiSocketSendCommandPipe*     send_pipe;
    vs32        result;         // the command handler's returned value
    vs16        state;          // socket status
    s8          flag_block;     // OS_MESSAGE_NOBLOCK(0)/BLOCK(1)/BLOCK_NO_RESULT(3)
    s8          flag_mode;      // TCP(0)UDP(1)UDPSEND(2)ICMP(3)SSL(4)
    u16         local_port;
    u16         remote_port;
    SOCLInAddr  remote_ip;

    struct SOCLSocket*  next;   // managed by the next socket list
} SOCLSocket;

// flag_block
#define SOCL_FLAGBLOCK_BLOCK    OS_MESSAGE_BLOCK        // 1
#define SOCL_FLAGBLOCK_NOBLOCK  OS_MESSAGE_NOBLOCK      // 0
#define SOCL_FLAGBLOCK_NORESULT (OS_MESSAGE_BLOCK | 2)  // although command inserts are handled in block, there is no wait for the process to end
#define SOCL_FLAGISBLOCK(x)     ((x) & OS_MESSAGE_BLOCK)

static inline BOOL SOCL_SocketIsBlock(SOCLSocket* socket)
{
    return(socket->flag_block == SOCL_FLAGBLOCK_BLOCK);
}

static inline BOOL SOCL_SocketIsNoBlock(SOCLSocket* socket)
{
    return(socket->flag_block == SOCL_FLAGBLOCK_NOBLOCK);
}

static inline void SOCL_SocketSetBlock(SOCLSocket* socket)
{
    socket->flag_block = SOCL_FLAGBLOCK_BLOCK;
}

static inline void SOCL_SocketSetNoBlock(SOCLSocket* socket)
{
    socket->flag_block = SOCL_FLAGBLOCK_NOBLOCK;
}

// flag_mode
#define SOCL_FLAGMODE_TCP       0
#define SOCL_FLAGMODE_UDP       1
#define SOCL_FLAGMODE_UDPSEND   2
#define SOCL_FLAGMODE_ICMP      3
#define SOCL_FLAGMODE_SSL       4

static inline BOOL SOCL_SocketIsTCP(SOCLSocket* socket)
{
    return(socket->flag_mode == SOCL_FLAGMODE_TCP || socket->flag_mode == SOCL_FLAGMODE_SSL);
}

static inline BOOL SOCL_SocketIsUDP(SOCLSocket* socket)
{
    return(socket->flag_mode == SOCL_FLAGMODE_UDP);
}

static inline BOOL SOCL_SocketIsUDPSend(SOCLSocket* socket)
{
    return(socket->flag_mode == SOCL_FLAGMODE_UDPSEND);
}

static inline BOOL SOCL_SocketIsICMP(SOCLSocket* socket)
{
    return(socket->flag_mode == SOCL_FLAGMODE_ICMP);
}

static inline BOOL SOCL_SocketIsSSL(SOCLSocket* socket)
{
    return(socket->flag_mode == SOCL_FLAGMODE_SSL);
}

// state
#define SOCL_STATUS_CREATE          0x0001
#define SOCL_STATUS_CONNECTING      0x0002
#define SOCL_STATUS_CONNECTED       0x0004
#define SOCL_STATUS_CLOSING         0x0008
#define SOCL_STATUS_WAIT_CLOSE      0x0010
#define SOCL_STATUS_WAIT_RELEASE    0x0020
#define SOCL_STATUS_ERROR           0x0040

static inline BOOL SOCL_SocketIsState(SOCLSocket* socket, u16 state)
{
    return(s32) socket->state & (s32) state;
}

static inline BOOL SOCL_SocketIsCreated(SOCLSocket* socket)
{
    return socket && SOCL_SocketIsState(socket, SOCL_STATUS_CREATE);
}

static inline BOOL SOCL_SocketIsConnecting(SOCLSocket* socket)
{
    return SOCL_SocketIsState(socket, SOCL_STATUS_CONNECTING);
}

static inline BOOL SOCL_SocketIsConnected(SOCLSocket* socket)
{
    return SOCL_SocketIsState(socket, SOCL_STATUS_CONNECTED);
}

static inline BOOL SOCL_SocketIsClosing(SOCLSocket* socket)
{
    return SOCL_SocketIsState(socket, SOCL_STATUS_CLOSING);
}

static inline BOOL SOCL_SocketIsWaitingClose(SOCLSocket* socket)
{
    return SOCL_SocketIsState(socket, SOCL_STATUS_WAIT_CLOSE);
}

static inline BOOL SOCL_SocketIsError(SOCLSocket* socket)
{
    return SOCL_SocketIsState(socket, SOCL_STATUS_ERROR);
}

// API flags
#define SOCL_MSG_PEEK       0x02
#define SOCL_MSG_DONTWAIT   0x04

//---------------------------------------------------------------------------
//  SSL-specific
//---------------------------------------------------------------------------
typedef CPSSslConnection    SOCLSslConnection;
typedef CPSCaInfo       SOCLCaInfo;
typedef int (*SOCLSslAuthHandler) (int result, SOCLSslConnection*con, int level);

//      Authentication error codes
#define SOCL_CERT_OUTOFDATE                 CPS_CERT_OUTOFDATE
#define SOCL_CERT_BADSERVER                 CPS_CERT_BADSERVER

#define SOCL_CERT_ERRMASK                   CPS_CERT_ERRMASK

#define SOCL_CERT_NOROOTCA                  CPS_CERT_NOROOTCA
#define SOCL_CERT_BADSIGNATURE              CPS_CERT_BADSIGNATURE
#define SOCL_CERT_UNKNOWN_SIGALGORITHM      CPS_CERT_UNKNOWN_SIGALGORITHM
#define SOCL_CERT_UNKNOWN_PUBKEYALGORITHM   CPS_CERT_UNKNOWN_PUBKEYALGORITHM

//---------------------------------------------------------------------------
//  socket thread command packet structure
//---------------------------------------------------------------------------
typedef int (*SOCLiCommandHandler) (void*);             // command callback function

//

// common to commands (all command packet headers are in this format)

//
typedef struct SOCLiCommandHeader
{
    SOCLiCommandHandler handler;    // function for calling from the socket thread
    SOCLSocket*         socket;     // the only access here is changes to status flags
    OSMessageQueue*     response;   // response target for completed callback
    s8  flag_mode;                  // TCP(0)UDP(1)UDPSEND(2)ICMP(3)SSL(4)
    s8  flag_block;                 // OS_MESSAGE_NOBLOCK(0) or BLOCK(1)
    u8  padding[2];

} SOCLiCommandHeader;

//

// command: SOCL_Socket

//
typedef struct SOCLiCommandCreateSocket
{
    SOCLiCommandHeader  h;

} SOCLiCommandCreateSocket;

//

// command: SOCL_Bind/SOCL_Connect

//
typedef struct SOCLiCommandBind
{
    SOCLiCommandHeader  h;
    u16         local_port;     // CPS_Bind parameter
    u16         remote_port;    // CPS_Bind parameter
    SOCLInAddr  remote_ip;      // CPS_Bind parameter
} SOCLiCommandBind;

//

// command: SOCL_ListenAccept

//
typedef struct SOCLiCommandListenAccept
{
    SOCLiCommandHeader  h;
    u16 local_port; // CPS_Bind parameter
    u8  padding[2];
    volatile u16*           remote_port_ptr;    // CPS_SocWho parameter
    volatile SOCLInAddr*    remote_ip_ptr;      // CPS_SocWho parameter
} SOCLiCommandListenAccept;

//

// command: SOCL_Read

//
typedef struct SOCLiCommandRead
{
    SOCLiCommandHeader  h;
    u8*         buffer;         // CPS_Read/Consume parameter
    s32         buffer_len;     // CPS_Read/Consume parameter
    u16*        remote_port;    // location for recipient port storage
    SOCLInAddr*     remote_ip;  // location for recipient IP storage
    s8  flag_noconsume;         // is consumer performed?
    u8  padding[3];
} SOCLiCommandRead;

//

// command: SOCL_Consume

//
typedef struct SOCLiCommandConsume
{
    SOCLiCommandHeader  h;

} SOCLiCommandConsume;

//

// command: SOCL_Write

//
typedef struct SOCLiCommandWrite
{
    SOCLiCommandHeader  h;
    u8*         buffer1;
    s32         buffer1_len;
    u8*         buffer2;
    s32         buffer2_len;
    u16         wrtbuf_after;
    u8          padding[2];
    u16         local_port;
    u16         remote_port;
    SOCLInAddr  remote_ip;

} SOCLiCommandWrite;

//

// command: SOCL_Shutdown

//
typedef struct SOCLiCommandShutdown
{
    SOCLiCommandHeader  h;

} SOCLiCommandShutdown;

//

// command: SOCL_Close

//
typedef struct SOCLiCommandClose
{
    SOCLiCommandHeader  h;

} SOCLiCommandClose;

//

// command: SOCL_EnableSsl

//
typedef struct SOCLiCommandEnableSsl
{
    SOCLiCommandHeader  h;
    SOCLSslConnection*  connection;
} SOCLiCommandEnableSsl;

//
// a packet size that can be stored by any command packet
// this member is added every time a command is added
//
typedef union SOCLiCommandPacket
{
    SOCLiCommandHeader          h;
    SOCLiCommandCreateSocket    create_socket;
    SOCLiCommandBind        bind;
    SOCLiCommandListenAccept    listen_accept;
    SOCLiCommandRead        read;
    SOCLiCommandConsume         consume;
    SOCLiCommandWrite       write;
    SOCLiCommandShutdown        shutdown;
    SOCLiCommandClose       close;
    SOCLiCommandEnableSsl       enable_ssl;

} SOCLiCommandPacket;

//
// command packet queue
//
// the maximum number of socket command packets that can be queued simultaneously
// as the BLOCK process is performed when obtaining this,
// it won't be fatal even if the number is exceeded (it is an error during NOBLOCK)
//
#define SOCL_CMDPACKET_MAX  64

//---------------------------------------------------------------------------

//  library initialization parameters

//---------------------------------------------------------------------------
typedef struct
{
    BOOL    use_dhcp;           // if TRUE, use dhcp
    struct
    {
        SOCLInAddr  my_ip;
        SOCLInAddr  net_mask;
        SOCLInAddr  gateway_ip;
        SOCLInAddr  dns_ip[2];
    } host_ip;
    void*  (*alloc) (u32);
    void (*free) (void*);

    u32     cmd_packet_max;     // maximum number of command packets
    u32     lan_buffer_size;    // if 0, saved within *alloc
    void*   lan_buffer;         // if 0, default (16384) settings

// CPS thread priorities
// if 0, SOCL_CPS_SOCKET_THREAD_PRIORITY
    s32     cps_thread_prio;

// communication buffer size
    s32     mtu;                //default 1500
    s32     rwin;
} SOCLConfig;

#define SOCLi_MTUtoMSS(mtu)             ((mtu) - 40)

#define SOCL_LAN_BUFFER_SIZE_DEFAULT    16384
#define SOCL_MTU_SIZE_MAX               1500
#define SOCL_MTU_SIZE_MIN               576

#define SOCL_MTU_SIZE_DEFAULT           576
#define SOCL_RWIN_SIZE_DEFAULT          (SOCLi_MTUtoMSS(SOCL_MTU_SIZE_DEFAULT) * 8)

#define SOCL_DHCP_REQUEST               0x01
#define SOCL_DHCP_CALLBACK              0x02
#define SOCL_DHCP_ERROR                 SOCL_DHCP_CALLBACK

extern int          SOCLiDhcpState;
extern SOCLInAddr   SOCLiRequestedIP;
extern u32          SOCLiYieldWait;

static inline void SOCL_SetRequestedIP(SOCLInAddr ip)
{
    SOCLiRequestedIP = ip;
}

static inline void SOCL_SetYieldWait(u32 wait)
{
    SOCLiYieldWait = wait;
}

static inline void SOCL_SetSslHandshakePriority(u32 prio)
{
    CPS_SetSslHandshakePriority(prio);
}

static inline u32 SOCL_GetSslHandshakePriority(void)
{
    return CPS_GetSslHandshakePriority();
}

//---------------------------------------------------------------------------

// socket structure initialization parameters (see socl_const.c)

//---------------------------------------------------------------------------
typedef struct SOCLSocketThreadParam
{
    u16 stack_size;
    u8  priority;
    u8  queue_max;

} SOCLSocketCommandPipeParam;

typedef struct SOCLSocketBufferParam
{
    u16 rcvbuf_size;
    u16 rcvbuf_consume_min;
    u16 sndbuf_size;
    u16 linbuf_size;
    u16 outbuf_size;
    u16 wrtbuf_size;                        // ring buffer for writes
    u16 udpbuf_size;                        // UDP receiving buffer
} SOCLSocketBufferParam;

typedef struct SOCLSocketParam
{
    s8  flag_mode;                          // TCP/UDP settings 0:TCP 1:UDP 3:PING 4:SSL
    s8  flag_block;                         // BLOCK MODE   0:NOBLOCK 1:BLOCK
    SOCLSocketBufferParam       buffer;     // send/receive buffer settings
    SOCLSocketCommandPipeParam  recv_pipe;  // receiving socket process thread use
    SOCLSocketCommandPipeParam  send_pipe;  // sending socket process thread use
} SOCLSocketParam;

extern SOCLSocketParam  SOCLSocketParamTCP;
extern SOCLSocketParam  SOCLSocketParamUDP;
extern SOCLSocketParam  SOCLSocketParamUDPSend;

#define SOCL_CPS_SOCKET_THREAD_PRIORITY 11  // cps thread prio

//--- TCP
#define SOCL_TCP_SOCKET_CPS_RCVBUF_SIZE (1460 * 2)  // cps.rcvbuf_size

// minimum length consumed by cps.rcvbuf (UDP fixed to 1 for TCP ACK controls)
#define SOCL_TCP_SOCKET_CPS_RCVBUF_CONSUME_MIN  1460

#if SOCL_OPTIMIZE_TCP_SEND
#define SOCL_TCP_SOCKET_CPS_SNDBUF_SIZE     ((SOCLi_MTUtoMSS(SOCL_MTU_SIZE_DEFAULT) * 2) + SOCL_TCP_SNDBUF_NOCOPY_OFFSET)   // cps.sndbuf_size
#define SOCL_TCP_SOCKET_SEND_WRTBUF_SIZE    ((SOCL_TCP_SOCKET_CPS_SNDBUF_SIZE - SOCL_TCP_SNDBUF_NOCOPY_OFFSET) * 2 + 1)     // wrtbuf_size
#else
#define SOCL_TCP_SOCKET_CPS_SNDBUF_SIZE     (SOCL_MTU_SIZE_MAX + 14)                    // cps.sndbuf_size
#define SOCL_TCP_SOCKET_SEND_WRTBUF_SIZE    (SOCL_TCP_SOCKET_CPS_SNDBUF_SIZE * 2 + 1)   // wrtbuf_size
#endif

#define SOCL_TCP_SOCKET_RECV_THREAD_PRIORITY    12      // thread.priority
#define SOCL_TCP_SOCKET_SEND_THREAD_PRIORITY    13      // thread.priority
#define SOCL_TCP_SOCKET_RECV_THREAD_STACK_SIZE  2048    // thread.stack_size
#define SOCL_TCP_SOCKET_SEND_THREAD_STACK_SIZE  2048    // thread.stack_size
#define SOCL_TCP_SOCKET_RECV_THREAD_QUEUE_MAX   32      // thread.queue_max
#define SOCL_TCP_SOCKET_SEND_THREAD_QUEUE_MAX   32      // thread.queue_max

//--- UDP
// SOCL_UDP_SOCKET_CPS_SNDBUF_SIZE is the value for controlling the maximum
// datagram length that can be sent via UDP
#define SOCL_UDP_SOCKET_CPS_RCVBUF_SIZE         1472    // cps.rcvbuf_size
#define SOCL_UDP_SOCKET_CPS_SNDBUF_SIZE         1514    // cps.buffer_size
#define SOCL_UDP_SOCKET_SEND_WRTBUF_SIZE        (SOCL_UDP_SOCKET_CPS_SNDBUF_SIZE + 256 + 1) // wrtbuf_size
#define SOCL_UDP_SOCKET_RECV_UDPBUF_SIZE        2048    // udpbuf_size
#define SOCL_UDP_SOCKET_RECV_THREAD_PRIORITY    12      // thread.priority
#define SOCL_UDP_SOCKET_SEND_THREAD_PRIORITY    13      // thread.priority
#define SOCL_UDP_SOCKET_RECV_THREAD_STACK_SIZE  2048    // thread.stack_size
#define SOCL_UDP_SOCKET_SEND_THREAD_STACK_SIZE  2048    // thread.stack_size
#define SOCL_UDP_SOCKET_RECV_THREAD_QUEUE_MAX   32      // thread.queue_max
#define SOCL_UDP_SOCKET_SEND_THREAD_QUEUE_MAX   32      // thread.queue_max
#ifdef SDK_USE_CHAROUT
#define SOCL_TCP_SOCKET_CPS_LINBUF_SIZE 128             // cps.linbuf_size
#define SOCL_TCP_SOCKET_CPS_OUTBUF_SIZE 512             // cps.outbuf_size
#define SOCL_UDP_SOCKET_CPS_LINBUF_SIZE 128             // cps.linbuf_size
#define SOCL_UDP_SOCKET_CPS_OUTBUF_SIZE 512             // cps.outbuf_size
#else
#define SOCL_TCP_SOCKET_CPS_LINBUF_SIZE 0               // cps.linbuf_size
#define SOCL_TCP_SOCKET_CPS_OUTBUF_SIZE 0               // cps.outbuf_size
#define SOCL_UDP_SOCKET_CPS_LINBUF_SIZE 0               // cps.linbuf_size
#define SOCL_UDP_SOCKET_CPS_OUTBUF_SIZE 0               // cps.outbuf_size
#endif

#define SOCL_TCP_SNDBUF_NOCOPY_OFFSET   (14 + 20 + 20)
#define SOCL_UDP_SNDBUF_NOCOPY_OFFSET   (14 + 20 + 8)

//---------------------------------------------------------------------------
//  API prototype declaration
//---------------------------------------------------------------------------
int     SOCLi_StartupCommandPacketQueue(s32 cp_max_count);
int     SOCLi_CleanupCommandPacketQueue(void);
void    SOCLi_CommandPacketHandler(void* arg);

SOCLiCommandPacket*         SOCLi_AllocCommandPacket(s32 flag);
SOCLiCommandPacket*         SOCLi_CreateCommandPacket(SOCLiCommandHandler handler, SOCLSocket* socket, s32 flag);
SOCLiSocketCommandPipe*     SOCLi_GetCtrlPipe(SOCLSocket* socket);
void    SOCLi_FreeCommandPacket(SOCLiCommandPacket* command);
int     SOCLi_SendCommandPacket(SOCLiSocketCommandPipe* pipe, SOCLiCommandPacket* command);
int     SOCLi_SendCommandPacketToCtrlPipe(SOCLSocket* socket, SOCLiCommandPacket* command);
int     SOCLi_ExecCommandPacket(SOCLiSocketCommandPipe* pipe, SOCLiCommandPacket* command);
int     SOCLi_ExecCommandPacketInRecvPipe(SOCLSocket* socket, SOCLiCommandPacket* command);
int     SOCLi_ExecCommandPacketInSendPipe(SOCLSocket* socket, SOCLiCommandPacket* command);
int     SOCLi_ExecCommandPacketInCtrlPipe(SOCLSocket* socket, SOCLiCommandPacket* command);

#define SOCLi_ROUNDUP4(size)    MATH_ROUNDUP((size), 4)
u32         SOCLi_RoundUp4(u32 size);

int         SOCL_Startup(const SOCLConfig* socl_config);
int         SOCL_Cleanup(void);
extern int  SOCLiUDPSendSocket;

void        SOCLi_SetMyIPinConfig(SOCLInAddr ip, SOCLInAddr mask, SOCLInAddr gateway);
void        SOCLi_SetMyIP(void);
void        SOCLi_DhcpTimeout(void);

int         SOCL_CreateSocket(const SOCLSocketParam* param);
int         SOCL_Bind(int s, u16 local_port);
int         SOCL_Connect(int s, u16 remote_port, SOCLInAddr remote_ip);
int         SOCL_Listen(int s, int backlog);
int         SOCL_Accept(int s, volatile u16* remote_port_ptr, volatile SOCLInAddr* remote_ip_ptr);
int         SOCL_ListenAccept(int s, volatile u16* remote_port_ptr, volatile SOCLInAddr* remote_ip_ptr);
int         SOCL_Read(int, void* , int, u16* , SOCLInAddr* );
int         SOCL_ReadFrom(int, void* , int, u16* , SOCLInAddr* , int);
int         SOCL_ReadNoBlock(int, void* , int, u16* , SOCLInAddr* );
int         SOCL_ReadBlock(int, void* , int, u16* , SOCLInAddr* );
int         SOCL_Write(int, const void* buffer, int buffer_len, u16 port, SOCLInAddr ip);
int         SOCL_WriteTo(int, const void* buffer, int buffer_len, u16 port, SOCLInAddr ip, int flags);
int         SOCL_WriteNoBlock(int s, const void* buffer, int buffer_len, u16 port, SOCLInAddr ip);
int         SOCL_WriteBlock(int s, const void* buffer, int buffer_len, u16 port, SOCLInAddr ip);
int         SOCL_Shutdown(int s);
int         SOCL_IsClosed(int s);
int         SOCL_Close(int s);
int         SOCL_CloseAll(void);
int         SOCL_CalmDown(void);
int         SOCL_GetRemote(int s, u16* port, SOCLInAddr* ip);
SOCLInAddr  SOCL_Resolve(const char* hostname);
SOCLInAddr  SOCL_InetAtoH(const char* hostname);
int         SOCL_GetResolver(SOCLInAddr* dns1, SOCLInAddr* dns2);
int         SOCL_SetResolver(const SOCLInAddr dns1, const SOCLInAddr dns2);
SOCLInAddr  SOCL_GetHostID(void);

const char*     SOCL_GetErrName(int errcode);
int SOCL_GetStatus(int s);
int SOCL_EnableSsl(int s, SOCLSslConnection* con);

// ERROR-CODE BEGINS
#define SOCL_ESUCCESS       0
#define SOCL_ENOMEM         SOC_ENOMEM
#define SOCL_EMFILE         SOC_EMFILE
#define SOCL_ECONNRESET     SOC_ECONNRESET
#define SOCL_ECANCELED      SOC_ECANCELED
#define SOCL_EALREADY       SOC_EALREADY
#define SOCL_ENOTCONN       SOC_ENOTCONN
#define SOCL_ENETRESET      SOC_ENETRESET
#define SOCL_EINVAL         SOC_EINVAL
#define SOCL_ENOBUFS        SOC_ENOBUFS
#define SOCL_ETIMEDOUT      SOC_ETIMEDOUT
#define SOCL_EWOULDBLOCK    SOC_EWOULDBLOCK
#define SOCL_EAGAIN         SOC_EAGAIN
#define SOCL_EINPROGRESS    SOC_EINPROGRESS
#define SOCL_EISCONN        SOC_EISCONN
#define SOCL_EMSGSIZE       SOC_EMSGSIZE

// ERROR-CODE ENDS
#define SOCL_POLLRDNORM         SOC_POLLRDNORM
#define SOCL_POLLWRNORM         SOC_POLLWRNORM
#define SOCL_POLLERR            SOC_POLLERR
#define SOCL_POLLHUP            SOC_POLLHUP
#define SOCL_POLLNVAL           SOC_POLLNVAL

#define SOCL_POLLERRORSTATUS    (SOCL_POLLERR | SOCL_POLLHUP | SOCL_POLLNVAL)
#define SOCL_POLLREADSTATUS     (SOCL_POLLRDNORM | SOCL_POLLERRORSTATUS)
#define SOCL_POLLWRITESTATUS    (SOCL_POLLWRNORM | SOCL_POLLERRORSTATUS)

//---------------------------------------------------------------------------
// private functions
SOCLSocket*     SOCLi_StartupSocket(const SOCLSocketParam* param);
void        SOCLi_CleanupSocket(SOCLSocket* socket);
void        SOCLi_TrashSocket(void);
s32     SOCLi_GetWriteBufferFreeSize(SOCLSocket* socket);
s32     SOCLi_GetReadBufferOccpiedSize(SOCLSocket* socket);
int     SOCLi_UdpRecvCallback(u8* data, u32 len, CPSSoc* soc);

void    SOCLi_SocketRegister(SOCLSocket* socket);
void    SOCLi_SocketUnregister(SOCLSocket* socket);
void    SOCLi_SocketRegisterTrash(SOCLSocket* socket);
void    SOCLi_SocketUnregisterTrash(SOCLSocket* socket);
int     SOCL_SocketIsInvalid(SOCLSocket* socket);
int     SOCL_SocketIsInTrash(SOCLSocket* socket);
extern SOCLSocket*  SOCLiSocketList;
extern SOCLSocket*  SOCLiSocketListTrash;

/*---------------------------------------------------------------------------*
Name: SOCL_TcpSocket/SOCL_UdpSocket

Description: Creates a socket using default parameters.
by making it inline, it TCP/UDP checks that occur in compile
during optimization processing are resolved, and we can try to get
the unneeded parameter table into the deadstrip.
 *---------------------------------------------------------------------------*/
static inline int SOCL_TcpSocket(void)
{
    return SOCL_CreateSocket(&SOCLSocketParamTCP);
}

static inline int SOCL_UdpSocket(void)
{
    return SOCL_CreateSocket(&SOCLSocketParamUDP);
}

static inline int SOCL_UdpSendSocket(void)
{
    return SOCL_CreateSocket(&SOCLSocketParamUDPSend);
}

/*---------------------------------------------------------------------------*
Name: SOCLi_Alloc/SOCLi_Free

Description: for calling the Alloc/Free functions specified in SOCLStartup()
 *---------------------------------------------------------------------------*/
extern CPSConfig        SOCLiCPSConfig;
extern const SOCLConfig*    SOCLiConfigPtr;

static inline void* SOCLi_Alloc(u32 size)
{
    return SOCLiConfigPtr->alloc(size);
}

static inline void SOCLi_Free(void* ptr)
{
    SOCLiConfigPtr->free(ptr);
}

/*---------------------------------------------------------------------------*
Name: SOCL_GetHostPort

Description: Obtains the self-host host port address.
Remains at 0 until gotten.
 *---------------------------------------------------------------------------*/
static inline u16 SOCL_GetHostPort(int s)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    return s ? (u16) socket->local_port : (u16) 0;
}

/*---------------------------------------------------------------------------*
Name: SOCL_SetBlock
Name: SOCL_SetNoBlock
Name: SOCL_IsBlock
Name: SOCL_IsNoBlock

Description: obtains/sets socket types
 *---------------------------------------------------------------------------*/
static inline void SOCL_SetBlock(int s)
{
    SOCL_SocketSetBlock((SOCLSocket*)s);
}

static inline void SOCL_SetNoBlock(int s)
{
    SOCL_SocketSetNoBlock((SOCLSocket*)s);
}

static inline BOOL SOCL_IsBlock(int s)
{
    return SOCL_SocketIsBlock((SOCLSocket*)s);
}

static inline BOOL SOCL_IsNoBlock(int s)
{
    return SOCL_SocketIsNoBlock((SOCLSocket*)s);
}

/*---------------------------------------------------------------------------*
Name: SOCL_GetReadStatus
Name: SOCL_GetWriteStatus

Description: obtains the socket R/W status
 *---------------------------------------------------------------------------*/
static inline int SOCL_GetReadStatus(int s)
{
    return(SOCL_GetStatus(s) & SOCL_POLLREADSTATUS);
}

static inline int SOCL_GetWriteStatus(int s)
{
    return(SOCL_GetStatus(s) & SOCL_POLLWRITESTATUS);
}

/*---------------------------------------------------------------------------*
Name: SOCL_AddRandomSeed

Description: Generates a random seed used by SSL.
Need to regularly provide the returned values from the OS_GetLowEntropyData function.
Can also be called before SOCL initialization.
 *---------------------------------------------------------------------------*/
static inline void SOCL_AddRandomSeed(void* seed, u32 len)
{
    CPS_SslAddRandomSeed(seed, len);
}

// SOCLi_IsInitialized
#define SOCLi_IsInitialized()   (NULL != SOCLConfigPtr)

/*---------------------------------------------------------------------------*
Name: SOCLi_CheckCount
Description: a counter that counts the number of time a specific process occurs
 *---------------------------------------------------------------------------*/
enum
{
    SOCL_CHECKCOUNT_UDPRCVCB_1  = 0,
    SOCL_CHECKCOUNT_UDPRCVCB_2,
    SOCL_CHECKCOUNT_MAX
};
extern u32 SOCLi_CheckCount[SOCL_CHECKCOUNT_MAX];   // the main part is within socl_read.c
#ifdef __cplusplus

}
#endif

#endif // NITROWIFI_SOCL_H_
