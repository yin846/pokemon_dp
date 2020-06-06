/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_resolve.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_resolve.c,v $
Revision 1.9  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.8  09/19/2005 14:08:29  yasu
added a closing state

Revision 1.7  09/15/2005 12:51:01  yasu
DHCP Requested IP support

Revision 1.6  08/24/2005 09:25:13  yasu
added SOCL_SocketIsInvalid

Revision 1.5  08/17/2005 11:48:13  yasu
SOC_InetAtoN revisions

Revision 1.4  08/02/2005 06:19:03  yasu
converted SOCL_GetHostID to a function

Revision 1.3  07/30/2005 22:30:14  yasu
revised so demo would work

Revision 1.2  07/30/2005 15:31:36  yasu
revisions in conjunction with command pipe separation

Revision 1.1  07/25/2005 10:15:01  yasu
added the SOC library

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

#define CPS_RCVBUF          SOCL_TCP_SOCKET_CPS_RCVBUF_SIZE
#define CPS_SNDBUF          SOCL_TCP_SOCKET_CPS_SNDBUF_SIZE
#define CPS_RCVBUF_ALIGNED  ((CPS_RCVBUF + 3) &~3)
#define CPS_SNDBUF_ALIGNED  ((CPS_SNDBUF + 3) &~3)

/*---------------------------------------------------------------------------*
Name: SOCL_Resolve

Description: As for the IP address from the DNS server.
  
Arguments: hostname        host name
  
Returns: an error if the IP address is 0
 *---------------------------------------------------------------------------*/
SOCLInAddr SOCL_Resolve(const char* hostname)
{
    u8*         buffers;
    CPSSoc      soc;
    SOCLInAddr  hostip;

    if (!hostname)
    {
        return 0;
    }

    buffers = SOCLi_Alloc(CPS_RCVBUF_ALIGNED + CPS_SNDBUF_ALIGNED);

    if (buffers == NULL)
    {
        return 0;
    }

    MI_CpuClear8(&soc, sizeof(CPSSoc)); // temporarily clears 0
    soc.rcvbuf.data = buffers;
    soc.rcvbuf.size = CPS_RCVBUF;
    soc.sndbuf.data = buffers + CPS_RCVBUF_ALIGNED;
    soc.sndbuf.size = CPS_SNDBUF;

    CPS_SocRegister(&soc);
    hostip = CPS_Resolve((char*)hostname);
    CPS_SocUnRegister();

    SOCLi_Free(buffers);

    return hostip;
}

/*---------------------------------------------------------------------------*
Name: SOCL_InetAtoH

Description: Converts text IP addresses to numeric format.
  
Arguments: hostname        IP address written in text
  
Returns: an error if the IP address is 0
 *---------------------------------------------------------------------------*/
SOCLInAddr SOCL_InetAtoH(const char* hostname)
{
    SOCLInAddr  hostip;
    OSIntrMode  enable;
    CPSInAddr   dns0, dns1;

// Temporarily halts feed from the DNS server and performs conversion from text to numeric format.
// Used a routine inside CPS to reduce the size of the code.
// in future, need to call the conversion routine within CPS directly
    enable = OS_DisableInterrupts();
    dns0 = CPSDnsIp[0];
    dns1 = CPSDnsIp[1];
    CPSDnsIp[0] = 0;
    CPSDnsIp[1] = 0;
    hostip = CPS_Resolve((char*)hostname);
    CPSDnsIp[0] = dns0;
    CPSDnsIp[1] = dns1;
    (void)OS_RestoreInterrupts(enable);

    return hostip;
}

/*---------------------------------------------------------------------------*
Name: SOCL_GetResolver/SOCL_SetResolver

Description: Gets and sets the host IP address for the current DNS server.
If the network connection is not ready, returns a negative value to
SOCL_ENETRESET.
 *---------------------------------------------------------------------------*/
int SOCL_GetResolver(SOCLInAddr* dns1, SOCLInAddr* dns2)
{
    SDK_ASSERT(dns1);
    SDK_ASSERT(dns2);

    if (SOCL_GetHostID() == 0)
    {
        return SOCL_ENETRESET;
    }

    *dns1 = (SOCLInAddr) CPSDnsIp[0];
    *dns2 = (SOCLInAddr) CPSDnsIp[1];
    return 0;
}

int SOCL_SetResolver(const SOCLInAddr dns1, const SOCLInAddr dns2)
{
    if (SOCL_GetHostID() == 0)
    {
        return SOCL_ENETRESET;
    }

    CPSDnsIp[0] = (CPSInAddr) dns1;
    CPSDnsIp[1] = (CPSInAddr) dns2;
    return 0;
}

/*---------------------------------------------------------------------------*
Name: SOCL_GetRemote

Description: Checks the socket connection target.
  
Arguments: s		socket
port		the remote host's port number
ip		the remote host's IP address
  
Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :  
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOCL_GetRemote(int s, u16* port, SOCLInAddr* ip)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // not initialized
    }

    if (SOCL_SocketIsTCP(socket) && (!SOCL_SocketIsConnected(socket) || SOCL_SocketIsClosing(socket)))
    {
        return SOCL_ENOTCONN;   // not connected
    }

    *port = socket->cps_socket.remote_port;
    *ip = socket->cps_socket.remote_ip;

    return SOCL_ESUCCESS;
}

/*---------------------------------------------------------------------------*
Name: SOCL_GetHostID

Description:	Gets the self-host host IP address.
Remains at 0 until gotten.
  
Arguments: None
  
Returns: the host's IPv4 IP address (HostByteOrder)
 *---------------------------------------------------------------------------*/
SOCLInAddr SOCL_GetHostID(void)
{
// If the link is not established, or
// there is no interrupt and in the middle of a DHCP request, sleeps
    if (CPSMyIp == 0)
    {
        if ((SOCLiDhcpState & (SOCL_DHCP_REQUEST | SOCL_DHCP_CALLBACK)) == SOCL_DHCP_REQUEST)
        {
            if (OS_GetProcMode() != OS_PROCMODE_IRQ)
            {
                OS_Sleep(10);   // vacates a 10ms process
            }
        }
    }
    else if (SOCLiRequestedIP == 0)
    {
// if the IP address is not saved, saves it
        SOCLiRequestedIP = CPSMyIp;
    }

    return (volatile SOCLInAddr)CPSMyIp;
}
