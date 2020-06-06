/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: soc.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: soc.c,v $
Revision 1.30  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.29  09/17/2005 11:51:58  yasu
added link break upon detecting ConfigError

Revision 1.28  09/17/2005 09:05:27  yasu
revisions around DHCP in conjunction with link drop handling

Revision 1.27  09/15/2005 12:50:48  yasu
revisions around Alloc/Free

Revision 1.26  09/08/2005 05:35:34  yasu
moved the ability to determine whether the socket scripter was of a valid value to the SOCL

Revision 1.25  09/02/2005 08:00:13  yasu
deleted local host IP assert

Revision 1.24  08/19/2005 12:49:49  yada
SOC_GetHostByName() and SOC_GetHostByAddr() are now able to return host name

Revision 1.23  08/19/2005 05:05:23  yasu
mtu/rwin support

Revision 1.22  08/17/2005 11:48:13  yasu
SOC_InetAtoN revisions

Revision 1.21  08/17/2005 04:10:27  yasu
added SOC_Poll functionality

Revision 1.20  08/12/2005 14:20:58  yasu
revised formatting error in string address returned by SOC_InetNtoP

Revision 1.19  08/12/2005 09:53:07  yasu
adjusted arguments for Recv/Send and settings for block/noblock within the socket constructor

Revision 1.18  08/09/2005 02:01:04  yasu
SOC_Poll bug fix

Revision 1.17  08/04/2005 04:15:37  yasu
SOC_MSG_PEEK tentative support

Revision 1.16  08/02/2005 03:14:00  yasu
fixed problem with normal operations for Free process wrapper

Revision 1.15  08/01/2005 13:25:21  yasu
made the SOC_Listen/Accept operations more socket-like

Revision 1.14  08/01/2005 07:40:28  yasu
comments added

Revision 1.13  07/30/2005 15:34:08  yasu
revisions in conjunction with command pipe separation

Revision 1.12  2005/07/28 14:14:22  seiki_masashi
added SOC_InetNtoP, SOC_InetPtoN

Revision 1.11  07/28/2005 11:26:31  yasu
fixed typo bug

Revision 1.10  07/28/2005 11:15:23  yasu
implemented SOC_Poll

Revision 1.9  07/27/2005 13:18:01  yasu
added SOC_GetHostByAddr

Revision 1.8  07/27/2005 12:21:44  yasu
IP_AtoN order work

Revision 1.7  07/27/2005 12:01:10  yasu
added sendto

Revision 1.6  07/25/2005 14:22:24  yasu
binding for SOC_Startup and SOCL_Startup

Revision 1.5  07/25/2005 10:15:01  yasu
added the SOC library

Revision 1.4  07/22/2005 14:11:11  yasu
bug fixes

Revision 1.3  07/22/2005 13:55:35  yasu
added listen accept

Revision 1.2  07/22/2005 12:44:56  yasu
pseudo-implementation of asynchronous processing

Revision 1.1  07/18/2005 13:16:27  yasu
created send/receive threads

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/soc.h>
#include <nitroWiFi/socl.h>

const u8    SOCAddrAny[SOC_ALEN] = {   0,  0,  0,  0};
const u8    SOCAddrLoopback[SOC_ALEN] = { 127,  0,  0,  1};
const u8    SOCAddrLimited[SOC_ALEN] = { 255,255,255,255};

/*---------------------------------------------------------------------------*
Name: SOC_Socket

Description: Creates the socket.
memory use parameters use the default settings

Arguments: pf              protocol family
at present set SOC_PF_INET
type            socket type
SOC_SOCK_STREAM : TCP socket
SOC_SOCK_DGRAM  : UDP socket
protocol        protocol specifier
at present set to 0

Returns: positive number or 0: socket indicator
at present the NDS address space uses an int expression for positive values
and the pointer value is returned as is.
negative number: error
currently supported error values are as below
- SOC_ENOMEM: insufficient memory
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOC_Socket(int pf, int type, int protocol)
{
    (void)pf;   // unused
    (void)protocol;

    SDK_ASSERT(pf == SOC_PF_INET);
    SDK_ASSERT(type == SOC_SOCK_STREAM || type == SOC_SOCK_DGRAM);
    SDK_ASSERT(protocol == 0);

    return(type == SOC_SOCK_STREAM) ? SOCL_TcpSocket() : SOCL_UdpSocket();
}

/*---------------------------------------------------------------------------*
Name: SOC_Bind

Description: performs socket settings (bind) from the local port
In the CPS function, the connection target port and address are set at the same time
and then connected to, here the local port settings values are
only saved within the socket structure.

Arguments: s       socket indicator
sockAddr        local port number, address
either SOCSockAddr type or SOCSockAddrIn type
the following are some sample settings

SOCSockAddrIn  a;
MI_CpuClear8(&a, sizeof(a));
a.len    = sizeof(SOCSockAddrIn);
a.family = SOC_PF_INET;
a.port   = SOC_HtoNs(local_port)
a.addr   = SOC_HtoNl(SOC_INADDR_ANY);
SOC_Bind(socket, &a);

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOC_Bind(int s, const void* sockAddr)
{
    const SOCSockAddrIn*    addr = (const SOCSockAddrIn*)sockAddr;

    SDK_ASSERT(addr != NULL);
    SDK_ASSERT(addr->len == sizeof(SOCSockAddrIn)); // not used
    SDK_ASSERT(addr->family == SOC_PF_INET);        // not used
    return SOCL_Bind(s, SOC_NtoHs(addr->port));
}

/*---------------------------------------------------------------------------*
Name: SOC_Connect

Description: performs socket settings (bind) from the remote port
In the CPS function, the connection target port and address are set at the same time
and then connected to, but here the setting values saved within the socket structure
in SOC_Bind are used.

Arguments: s       socket indicator
sockAddr        remote port number, address
either SOCSockAddr type or SOCSockAddrIn type
the following are some sample settings

SOCSockAddrIn  a;
MI_CpuClear8(&a, sizeof(a));
a.len    = sizeof(SOCSockAddrIn);
a.family = SOC_PF_INET;
a.port   = SOC_HtoNs(remote_port)
a.addr   = SOC_HtoNl(remote_IP_address);
SOC_Connect(socket, &a);

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOC_Connect(int s, const void* sockAddr)
{
    const SOCSockAddrIn*    addr = (const SOCSockAddrIn*)sockAddr;

    SDK_ASSERT(addr != NULL);
    SDK_ASSERT(addr->len == sizeof(SOCSockAddrIn)); // not used
    SDK_ASSERT(addr->family == SOC_PF_INET);        // not used
    return SOCL_Connect(s, SOC_NtoHs(addr->port), SOC_NtoHl(addr->addr.addr));
}

/*---------------------------------------------------------------------------*
Name: SOC_Read/SOC_Recv/SOC_RecvFrom

Description: Copies the data from the socket.

Arguments: s       socket indicator
buf             buffer read in
len             size of buffer read in
flag            settings flag during copy
currently only supports SOC_MSG_DONTWAIT
sockFrom        data for host that sent data read in

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOC_Read(int s, void* buf, int len)
{
    return SOCL_ReadFrom(s, buf, len, NULL, NULL, 0);
}

int SOC_Recv(int s, void* buf, int len, int flags)
{
    return SOCL_ReadFrom(s, buf, len, NULL, NULL, flags);
}

int SOC_RecvFrom(int s, void* buf, int len, int flags, void* sockFrom)
{
    u16         port;
    SOCLInAddr  ip;
    int         result;

    result = SOCL_ReadFrom(s, buf, len, &port, &ip, flags);

    if (result >= 0 && sockFrom)
    {
        SOCSockAddrIn*  addr = (SOCSockAddrIn*)sockFrom;
        addr->port = SOC_HtoNs(port);
        addr->addr.addr = SOC_HtoNl(ip);
    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOC_Write/SOC_Send

Description: Copies data to the socket.

Arguments: s       socket indicator
buf             data to be written
len             size of data to be written
flag            settings flag during copy
currently only supports SOC_MSG_DONTWAIT

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOC_Write(int s, const void* buf, int len)
{
    return SOCL_WriteTo(s, buf, len, 0, (SOCLInAddr) 0, 0);
}

int SOC_Send(int s, const void* buf, int len, int flags)
{
    return SOCL_WriteTo(s, buf, len, 0, (SOCLInAddr) 0, flags);
}

int SOC_SendTo(int s, const void* buf, int len, int flags, const void* sockTo)
{
    SOCSockAddrIn*  addr;
    u16         port;
    SOCLInAddr  ip;

    if (sockTo)
    {
        addr = (SOCSockAddrIn*)sockTo;
        port = SOC_NtoHs(addr->port);
        ip = SOC_NtoHl(addr->addr.addr);
    }
    else
    {
        port = 0;
        ip = 0;
    }

    return SOCL_WriteTo(s, buf, len, port, ip, flags);
}

/*---------------------------------------------------------------------------*
Name: SOC_Shutdown

Description: Shuts the socket down.

Arguments: s       socket indicator
how             type of shutdown process
SO_SHUT_RDWR send/receive process

at present, as a result of limitations in the CPS function
only SOC_SHUT_RDWR is supported

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOC_Shutdown(int s, int how)
{
    SDK_ASSERT(how == SOC_SHUT_RDWR);
    (void)how;  // unused
    return SOCL_Shutdown(s);
}

/*---------------------------------------------------------------------------*
Name: SOC_Close

Description: Closes the socket.

Arguments: s       socket indicator

Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOC_Close(int s)
{
    return SOCL_Close(s);
}

/*---------------------------------------------------------------------------*
Name: SOC_GetHostByName

Description: gets the host entry from the host name
Given CPS limitations, the only obtainable data is the IP address.

Arguments: name      host name

Returns: hostent   pointer to the host entry data structure
 *---------------------------------------------------------------------------*/
SOCHostEnt* SOC_GetHostByName(const char* name)
{
    static SOCHostEnt   hostent;
    static u8*          hostAddrList[2];
    static SOCLInAddr   hostip;
    static char         entName[SOC_MAXDNAME + 1];
    u32 h;

    h = (u32) SOCL_Resolve(name);
    if (h == 0)
    {
        return NULL;
    }

//---- host name copy
    (void)STD_CopyLString(&entName[0], name, SOC_MAXDNAME + 1);

    hostent.name = &entName[0];
    hostent.aliases = NULL;
    hostent.addrType = SOC_PF_INET;
    hostent.length = sizeof(SOCLInAddr);
    hostent.addrList = hostAddrList;
    hostAddrList[0] = (u8*) &hostip;
    hostAddrList[1] = NULL;
    hostip = SOC_HtoNl(h);

    return &hostent;
}

/*---------------------------------------------------------------------------*
Name: SOC_GetHostByAddr

Description: gets the host entry from the address
In actuality, only the address is stored within the entry structure.

Arguments: s       socket indicator

Returns: hostent   pointer to the host entry data structure
 *---------------------------------------------------------------------------*/
SOCHostEnt* SOC_GetHostByAddr(const void* addr, int len, int type)
{
    static SOCHostEnt   hostent;
    static u8*          hostAddrList[2];
    static SOCLInAddr   hostip;
    static char         entName[SOC_MAXDNAME + 1];

    SDK_ASSERT(type == SOC_PF_INET);
    SDK_ASSERT(len == sizeof(SOCLInAddr));

//---- converts the address to text
    (void)SOC_InetNtoP(SOC_AF_INET, addr, &entName[0], SOC_MAXDNAME + 1);

    hostent.name = &entName[0];
    hostent.aliases = NULL;
    hostent.addrType = (s16) type;
    hostent.length = (s16) len;
    hostent.addrList = hostAddrList;
    hostAddrList[0] = (u8*) &hostip;
    hostAddrList[1] = NULL;
    hostip = *(SOCLInAddr*)addr;

    return &hostent;
}

/*---------------------------------------------------------------------------*
Name: SOC_GetSockName

Description: gets the bound local address and port for the socket

Arguments: s    socket
sockAddr  the socket's bind target

SOCSockAddrIn  a;
a.len    = sizeof(SOCSockAddrIn);
a.family = SOC_PF_INET;
a.port   = SOC_HtoNs(local_port)
a.addr   = SOC_HtoNl(local_ip);

Returns: 0  success
failure negative value
 *---------------------------------------------------------------------------*/
int SOC_GetSockName(int s, void* sockAddr)
{
    SOCSockAddrIn*  addr = (SOCSockAddrIn*)sockAddr;
    u32 local_ip;
    u32 local_port;

    if (!s)
    {
        return SOC_ENETRESET;
    }

    local_ip = (u32) SOCL_GetHostID();
    local_port = (u32) SOCL_GetHostPort(s);

    if (local_ip == 0)
    {
        local_port = 0;
    }

    addr->len = sizeof(SOCSockAddrIn);
    addr->family = SOC_PF_INET;
    addr->port = SOC_HtoNs(local_port);
    addr->addr.addr = SOC_HtoNl(local_ip);

    return 0;
}

/*---------------------------------------------------------------------------*
Name: SOC_GetHostID

Description: Gets the host IP address.

Arguments: None

Returns: the host's IPv4 IP address (NetworkByteOrder)
 *---------------------------------------------------------------------------*/
long SOC_GetHostID(void)
{
    u32 hostid = (u32) SOCL_GetHostID();

    return (long)SOC_HtoNl(hostid);
}

/*---------------------------------------------------------------------------*
Name: SOC_GetResolver/SOC_SetResolver

Description: Gets and sets the host IP address for the current DNS server.
If the network connection is not ready, returns a negative value to
SOCL_ENETRESET.
 *---------------------------------------------------------------------------*/
int SOC_SetResolver(const SOCInAddr* dns1, const SOCInAddr* dns2)
{
    return SOCL_SetResolver(SOC_NtoHl(dns1->addr), SOC_NtoHl(dns2->addr));
}

int SOC_GetResolver(SOCInAddr* dns1, SOCInAddr* dns2)
{
    u32 dns1a, dns2a;
    int result;

    result = SOCL_GetResolver(&dns1a, &dns2a);
    if (result >= 0)
    {
        dns1->addr = SOC_HtoNl(dns1a);
        dns2->addr = SOC_HtoNl(dns2a);
    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOC_Listen

Description: performs connection waiting from the socket
given the CPS spec, does nothing at present

Arguments: s       socket indicator

Returns:      0
 *---------------------------------------------------------------------------*/
int SOC_Listen(int s, int backlog)
{
    return SOCL_Listen(s, backlog);
}

/*---------------------------------------------------------------------------*
Name: SOC_Accept

Description: performs connection waiting from the socket
does not work in NOBLOCK mode at present

Arguments: s       socket indicator
sockAddr        connection target address

Returns: positive number or 0: success (accepted socket indicator)
negative number: error
 *---------------------------------------------------------------------------*/
int SOC_Accept(int s, void* sockAddr)
{
    SOCSockAddrIn*  addr = (SOCSockAddrIn*)sockAddr;
    u16         remote_port;
    SOCLInAddr  remote_ip;
    int         result;

    result = SOCL_Accept(s, &remote_port, &remote_ip);

    if (result >= 0)
    {
        addr->port = SOC_HtoNs(remote_port);
        addr->addr.addr = SOC_HtoNl(remote_ip);
    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOC_Fcntl

Description: changes socket settings
at present only SOC_O_NONBLOCK can be set

Arguments: s       socket indicator
cmd             command
mode            the value to be set/read in

Returns: positive number or 0: Succeeded
negative number: error
 *---------------------------------------------------------------------------*/
int SOC_Fcntl(int s, int cmd, int mode)
{
    int t;

    if (!s)
    {
        return -1;
    }

    switch (cmd)
    {
    case SOC_F_GETFL:
        t = SOCL_IsBlock(s) ? 0 : SOC_O_NONBLOCK;
        return (int)t;

    case SOC_F_SETFL:
        if (mode & SOC_O_NONBLOCK)
        {
            SOCL_SetNoBlock(s);
        }
        else
        {
            SOCL_SetBlock(s);
        }
        break;

    default:
        SDK_ASSERT(0);
        break;
    }

    return 0;
}

/*---------------------------------------------------------------------------*
Name: SOC_Startup

Description: performs socket parameter settings

Arguments: config          settings data structure

Returns: positive number or 0: Succeeded
negative number: error
 *---------------------------------------------------------------------------*/
static void*  (*AllocFunc_SOC) (u32 name, s32 size);
static void* AllocFunc_SOCL(u32 size)
{
    s32     msize = (s32) (size + sizeof(s32));
    s32*    m = AllocFunc_SOC(0U, msize);

// hide the parameters needed for the call in the memory header
    if (m)
    {
        m[0] = (s32) msize;
        m++;
    }

    return (void*)m;
}

static void (*FreeFunc_SOC) (u32 name, void *ptr, s32 size);
static void FreeFunc_SOCL(void* ptr)
{
    s32*    m = (s32*)ptr;

// copy the parameters needed for the call from the memory header
    if (m)
    {
        m--;
        FreeFunc_SOC(0U, (void*)m, m[0]);
    }
}

int SOC_Startup(const SOCConfig* config)
{
    static SOCLConfig   soclConfig;

    soclConfig.use_dhcp = (config->flag == SOC_FLAG_DHCP) ? TRUE : FALSE;
    soclConfig.host_ip.my_ip = SOC_NtoHl(config->addr.addr);
    soclConfig.host_ip.net_mask = SOC_NtoHl(config->netmask.addr);
    soclConfig.host_ip.gateway_ip = SOC_NtoHl(config->router.addr);
    soclConfig.host_ip.dns_ip[0] = SOC_NtoHl(config->dns1.addr);
    soclConfig.host_ip.dns_ip[1] = SOC_NtoHl(config->dns2.addr);
    soclConfig.alloc = AllocFunc_SOCL;
    soclConfig.free = FreeFunc_SOCL;
    AllocFunc_SOC = config->alloc;
    FreeFunc_SOC = config->free;
    soclConfig.cmd_packet_max = SOCL_CMDPACKET_MAX;
    soclConfig.mtu = config->mtu;
    soclConfig.rwin = config->rwin;

    return SOCL_Startup(&soclConfig);
}

/*---------------------------------------------------------------------------*
Name: SOC_Cleanup

Description: performs socket parameter release

Arguments:

Returns: positive number or 0: Succeeded
negative number: error
 *---------------------------------------------------------------------------*/
int SOC_Cleanup(void)
{
    return SOCL_Cleanup();
}

/*---------------------------------------------------------------------------*
Name: SOC_InetNtoA

Description: converts the IP address to text

Arguments: in  address

Returns: text
 *---------------------------------------------------------------------------*/
char* SOC_InetNtoA(SOCInAddr in)
{
    static char buffer[SOC_INET_ADDRSTRLEN];

    (void)SOC_InetNtoP(SOC_AF_INET, &in.addr, buffer, sizeof(buffer));

    return buffer;
}

/*---------------------------------------------------------------------------*
Name: SOC_InetAtoN

Description: converts text to an IP address

Arguments: cp  text
inp IP address

Returns: TRUE succeeded   FALSE failed
 *---------------------------------------------------------------------------*/
int SOC_InetAtoN(const char* cp, SOCInAddr* inp)
{
    u32 h = (u32) SOCL_InetAtoH(cp);

    if (h == 0)
    {
        return FALSE;
    }

    inp->addr = SOC_HtoNl(h);
    return TRUE;
}

/*---------------------------------------------------------------------------*
Name: SOC_InetNtoP

Description: converts the IP address to text

Arguments: in  address

Returns: text
 *---------------------------------------------------------------------------*/
const char* SOC_InetNtoP(int af, const void* src, char* dst, unsigned len)
{
    u32 addr32;
    u8  addr[4];

    if (af != SOC_AF_INET)
    {
        return NULL;
    }

    if (len < SOC_INET_ADDRSTRLEN)
    {
        return NULL;
    }

    MI_CpuCopy8(src, &addr32, sizeof(addr32));

    SOC_U32to4U8(addr32, addr);
    (void)OS_SNPrintf(dst, SOC_INET_ADDRSTRLEN, "%d.%d.%d.%d", addr[3], addr[2], addr[1], addr[0]);

    return (const char*)dst;
}

/*---------------------------------------------------------------------------*
Name: SOC_InetPtoN

Description: converts text to an IP address

Arguments: af  address family
src pointer to the passed text string
dst pointer to the buffer returning the numeric address

Returns: 0: succeeded   1: failed   SOC_EAFNOSUPPORT: af is not a SOC_AF_INET value
 *---------------------------------------------------------------------------*/
int SOC_InetPtoN(int af, const char* src, void* dst)
{
    u32 h;
    u32 addr;

    if (af != SOC_AF_INET)
    {
        return SOC_EAFNOSUPPORT;
    }

    h = (u32) SOCL_InetAtoH(src);

    if (h == 0)
    {
        return 1;
    }

    addr = SOC_HtoNl(h);
    MI_CpuCopy8(&addr, dst, sizeof(addr));

    return 0;
}

/*---------------------------------------------------------------------------*
Name: SOC_U32to4U8

Description: replaces a U32 value with a u8x4 array

Arguments: adr_u32         u32 value
adr_4u8         output target

Returns: None
 *---------------------------------------------------------------------------*/
void SOC_U32to4U8(u32 adr_u32, u8* adr_4u8)
{
    adr_4u8[0] = (u8) ((adr_u32 >> 24) & 0xff);
    adr_4u8[1] = (u8) ((adr_u32 >> 16) & 0xff);
    adr_4u8[2] = (u8) ((adr_u32 >> 8) & 0xff);
    adr_4u8[3] = (u8) ((adr_u32 >> 0) & 0xff);
}

/*---------------------------------------------------------------------------*
Name: SOC_Poll

Description: Checks if there is an indicator for possible reads and writes
within the specified socket.

Arguments: fds             SOCPollFD array
nfds            SOCPollFD number
timeout         timeout
(0 indicates immediate end/
negative value indicates no timeout)

Returns: positive value: number of socket that matched the conditions
0: timeout
negative value: error
 *---------------------------------------------------------------------------*/
#define SOC_POLLING_INTERVAL    1   // unit: ms
int SOC_Poll(SOCPollFD* fds, unsigned int nfds, OSTick timeout)
{
    SOCPollFD*  fp;
    BOOL        is_enable_timeout;
    s64         time;
    int         i, num_good;
    int         status, status_mask;

    is_enable_timeout = (timeout != (OSTick) SOC_INFTIM);
    time = (s64) timeout;

    for (;;)
    {
        fp = fds;
        num_good = 0;

        for (i = 0; i < nfds; i++)
        {
            status_mask = fp->events | SOC_POLLERR | SOC_POLLHUP | SOC_POLLNVAL;
            status = SOCL_GetStatus(fp->fd) & status_mask;
            if (status)
            {
                num_good++;
            }

            fp->revents = (s16) status;
            fp++;
        }

        if (num_good > 0 || (is_enable_timeout && time <= 0))
        {
            break;
        }

        OS_Sleep(SOC_POLLING_INTERVAL);
        time -= (s64) OS_MilliSecondsToTicks(SOC_POLLING_INTERVAL);
    }

    return num_good;
}

/*---------------------------------------------------------------------------*
Name: SOC_GetLinkState

Description: returns TRUE if link has been established

Arguments: interface   not used
*status     link status
Returns: None
 *---------------------------------------------------------------------------*/
void SOC_GetLinkState(SOCInterface* interface, BOOL* status)
{
    if (WCM_GetPhase() == WCM_PHASE_DCF)            // OK if link is established
    {
        *status = TRUE;
    }
    else
    {
        *status = FALSE;

        if (OS_GetProcMode() != OS_PROCMODE_IRQ)    // if not in an interrupt
        {
            OS_Sleep(1);    // vacates a 1ms process
        }
    }
    (void)interface;
}

/*---------------------------------------------------------------------------*
Name: SOC_GetConfigError

Description: checks whether an error occurred during the Config process

Arguments: interface   not used

Returns: error cause: 0 indicates no error
 *---------------------------------------------------------------------------*/
s32 SOC_GetConfigError(SOCInterface* interface)
{
    s32 result = 0;

    if (WCM_GetPhase() == WCM_PHASE_DCF)
    {
        if ((SOCLiDhcpState & SOCL_DHCP_ERROR) == SOCL_DHCP_ERROR)
        {
// Under the current WCM spec, the ability to detect a link break
// occurs several seconds after the actual link is broken,
// so it is possible that the DHCP might get a timeout first.
// As a result, DHCP failures can be seen as resulting from link breaks
// and when re-connecting, it will be safer to begin by establishing the link.
            result = SOC_IP_ERR_DHCP_TIMEOUT;
        }
    }
    else
    {
        result = SOC_IP_ERR_LINK_DOWN;
    }

#ifdef SDK_MY_DEBUG
    switch (CPSNoIpReason)
    {
    case CPS_NOIP_REASON_DHCPDISCOVERY:
        OS_TPrintf("CPS_NOIP_REASON_DHCPDISCOVERY\n");
        break;

    case CPS_NOIP_REASON_LINKOFF:
        OS_TPrintf("CPS_NOIP_REASON_LINKOFF\n");
        break;

    case CPS_NOIP_REASON_LEASETIMEOUT:
        OS_TPrintf("CPS_NOIP_REASON_LEASETIMEOUT\n");
        break;

    case CPS_NOIP_REASON_COLLISION:
        OS_TPrintf("CPS_NOIP_REASON_COLLISION\n");
        break;

    default:
        OS_TPrintf("DEFAULT???\n");
    }
#endif

    (void)interface;
    return result;
}
