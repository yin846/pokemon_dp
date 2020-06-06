/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: soc_startup.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_startup.c,v $
Revision 1.18  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.17  10/05/2005 11:15:45  yasu
Buried version string.

Revision 1.16  09/28/2005 11:44:37  yasu
cleaned up the code
added SOCL_CalmDown()
as we don't yet have official support for the CPS library, covered in processing in SOC

Revision 1.15  09/21/2005 11:19:40  yasu
added SOCL_SetYieldWait()

Revision 1.14  09/15/2005 13:14:46  yasu
DHCP timeout support

Revision 1.13  09/15/2005 12:51:01  yasu
DHCP Requested IP support

Revision 1.12  09/02/2005 08:47:05  yasu
prepared an API that can change IP address settings within SOCLConfig

Revision 1.11  08/22/2005 04:33:02  yasu
made the UDP receiving buffer size fixed

Revision 1.10  08/19/2005 05:05:36  yasu
mtu/rwin support

Revision 1.9  08/18/2005 13:17:59  yasu
moved cleanup API to separate file

Revision 1.8  08/08/2005 14:15:38  yasu
CPS thread priority settings

Revision 1.7  07/30/2005 22:30:14  yasu
revised so demo would work

Revision 1.6  07/30/2005 15:33:45  yasu
revisions in conjunction with command pipe separation

Revision 1.5  07/25/2005 14:22:24  yasu
binding for SOC_Startup and SOCL_Startup

Revision 1.4  07/23/2005 14:16:49  yasu
concealed debug messages

Revision 1.3  07/22/2005 12:44:56  yasu
pseudo-implementation of asynchronous processing

Revision 1.2  07/19/2005 14:06:33  yasu
added SOCL_Read/Write

Revision 1.1  07/18/2005 13:16:28  yasu
created send/receive threads

Revision 1.1  07/15/2005 13:33:11  yasu
adjusted soc library source

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>
#include "../wcm/include/wcm_cpsif.h"

//---------------------------------------------------------------------------*
//
//  save the pointer to the SOCL Config value
//  need to set value here before calling SOCL function
//
CPSConfig       SOCLiCPSConfig;
const SOCLConfig*   SOCLiConfigPtr = NULL;
int         SOCLiUDPSendSocket = 0;
int         SOCLiDhcpState = 0;
SOCLInAddr  SOCLiRequestedIP = 0;
u32         SOCLiYieldWait = 0;

//---------------------------------------------------------------------------*
static void SOCLi_StartupCPS(void);
static int  SOCLi_StartupSOCL(void);
static void SOCLi_SetMyIP(void);
BOOL        SOCL_LinkIsOn(void);

#include <nitroWiFi/version.h>
#include <nitro/version_begin.h>
#define MODULE  "WiFi"SDK_WIFI_VERSION_STRING
static char id_string[] = SDK_MIDDLEWARE_STRING("NINTENDO", MODULE);
#include <nitro/version_end.h>

/*---------------------------------------------------------------------------*
Name: SOCL_Startup

Description: Initializes the library.
Initialize the SOCLConfig parameters in conjunction with the
library specs before calling functions.

Arguments: socl_config     pointer to the SOCLConfig structure
socl_hostip     IP address setting value
if NULL, get from DHCP

Returns: if 0 end normally
if -1, insufficient memory
 *---------------------------------------------------------------------------*/
int SOCL_Startup(const SOCLConfig* socl_config)
{
    SDK_USING_MIDDLEWARE(id_string);
    SDK_ASSERT(socl_config);

    if (SOCLiConfigPtr)
    {
        return 0;
    }

    SOCLiConfigPtr = socl_config;
    SOCLi_StartupCPS();                     // CPS     service launch
    return SOCLi_StartupSOCL();             // SOCL    service launch
}

static int SOCLi_StartupSOCL(void)
{
    int r;

// Command service launch
    r = SOCLi_StartupCommandPacketQueue((int)SOCLiConfigPtr->cmd_packet_max);

// creates a UDP Send socket
    if (r >= 0)
    {
        SOCLiUDPSendSocket = SOCL_UdpSendSocket();
    }

    return r;
}

static void SOCLi_StartupCPS(void)
{
    //
//  settings parameters to use CPS functions
    //
    CPSConfig*          cps_config = &SOCLiCPSConfig;
    const SOCLConfig*   socl_config = SOCLiConfigPtr;

    SDK_ASSERT(socl_config->alloc);
    SDK_ASSERT(socl_config->free);

    MI_CpuClear8(cps_config, sizeof(CPSConfig));
    cps_config->alloc = socl_config->alloc;
    cps_config->free = socl_config->free;
    cps_config->link_is_on = SOCL_LinkIsOn; // function to return the state of a physical link
    cps_config->random_seed = 0;            // if 0, gets a random seed from OS_Tick
    cps_config->yield_wait = SOCLiYieldWait;    // sleep time during CPS library yield

// if socl_config->lan_buffer_size is 0, sets the default value
// if socl_config->lan_buffer is NULL, hold within alloc
    if (socl_config->lan_buffer_size)
    {
        cps_config->lan_buflen = socl_config->lan_buffer_size;
    }
    else
    {
        cps_config->lan_buflen = SOCL_LAN_BUFFER_SIZE_DEFAULT;
    }

    if (socl_config->lan_buffer)
    {
        cps_config->lan_buf = socl_config->lan_buffer;
    }
    else
    {
        cps_config->lan_buf = SOCLi_Alloc(cps_config->lan_buflen);
    }

// MTU/MSS settings
//  if socl_config->mtu is 0, sets the default value
    {
        s32 mtu, mss, rwin;

        mtu = socl_config->mtu ? socl_config->mtu : SOCL_MTU_SIZE_DEFAULT;
        rwin = socl_config->rwin ? socl_config->rwin : SOCL_RWIN_SIZE_DEFAULT;
        mss = SOCLi_MTUtoMSS(mtu);

        cps_config->mymss = (u32) mss;
        SOCLSocketParamTCP.buffer.rcvbuf_size = (u16) rwin;
        SOCLSocketParamTCP.buffer.rcvbuf_consume_min = (u16) (rwin / 2);
    }

// if the hostip setting in HostIP is 0, use DHCP
// if DHCP is not available, it can be determined with SOCLiDhcpState
    CPSMyIp = 0x00000000;

    if (socl_config->use_dhcp)
    {
        SOCLiDhcpState = SOCL_DHCP_REQUEST;
        cps_config->mode = 0;
        cps_config->dhcp_callback = SOCLi_DhcpTimeout;
        cps_config->requested_ip = SOCLiRequestedIP;
    }
    else
    {
        SOCLiDhcpState = 0;
        cps_config->mode = CPS_DONOTUSE_DHCP;
        cps_config->dhcp_callback = SOCLi_SetMyIP;
    }

// CPS thread priorities
    CPS_SetThreadPriority(socl_config->cps_thread_prio ? socl_config->cps_thread_prio : (u32) SOCL_CPS_SOCKET_THREAD_PRIORITY);

    WCM_SetRecvDCFCallback(CPSi_RecvCallbackFunc);

    {   // temporary strategy for asynchronous Cleanup
        extern void CPS_SetScavengerCallback(void (*f) (void));
        CPS_SetScavengerCallback(SOCLi_TrashSocket);
    }

#ifdef SDK_MY_DEBUG
    OS_TPrintf("CPS_Startup\n");
#endif
    CPS_Startup(cps_config);

    return;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_SetMyIPinConfig

Description: replaces the IP address field registered within SOCLConfig

Arguments: None

Returns: None
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void SOCLi_SetMyIPinConfig(SOCLInAddr ip, SOCLInAddr mask, SOCLInAddr gateway)
{
    SOCLConfig*     config = (SOCLConfig*)SOCLiConfigPtr;

    if (config)
    {
        config->host_ip.my_ip = ip;
        config->host_ip.net_mask = mask;
        config->host_ip.gateway_ip = gateway;
    }

    return;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_SetMyIP

Description: IP address manual settings

Arguments: None

Returns: None
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void SOCLi_SetMyIP(void)
{
    const SOCLConfig*   config = SOCLiConfigPtr;

    SDK_ASSERT(config);
    CPSMyIp = SOCL2CPSInAddr(config->host_ip.my_ip);
    CPSNetMask = SOCL2CPSInAddr(config->host_ip.net_mask);
    CPSGatewayIp = SOCL2CPSInAddr(config->host_ip.gateway_ip);
    CPSDnsIp[0] = SOCL2CPSInAddr(config->host_ip.dns_ip[0]);
    CPSDnsIp[1] = SOCL2CPSInAddr(config->host_ip.dns_ip[1]);
    SOCLiDhcpState |= SOCL_DHCP_CALLBACK;
    return;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_DhcpTimeout

Description: the DHCP Timeout callback

Arguments: None

Returns: None
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void SOCLi_DhcpTimeout(void)
{
    SOCLiDhcpState |= (SOCL_DHCP_CALLBACK | SOCL_DHCP_ERROR);
    return;
}

/*---------------------------------------------------------------------------*
Name: SOCL_LinkIsOn

Description: determines the physical link connection

Arguments: None

Returns: TRUE if connected
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL BOOL SOCL_LinkIsOn(void)
{
    return WCM_GetApMacAddress() != NULL;
}
