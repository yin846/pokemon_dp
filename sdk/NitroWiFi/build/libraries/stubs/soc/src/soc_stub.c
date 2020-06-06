/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries - stub - soc
File: soc_stub.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: soc_stub.c,v $
Revision 1.4  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.3  08/11/2005 14:08:00  yasu
conversion to WEAK SYMBOL

Revision 1.2  08/10/2005 02:15:33  yasu
deleted SOC_GetMtu

Revision 1.1  08/04/2005 08:17:23  yada
stub added

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitroWiFi.h>
#include <nitroWiFi/soc_stub.h>

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// used to get through gctest
// this should disappear if implemented

//----------------------------------------------------------------
SDK_WEAK_SYMBOL int SOC_GetAddrInfo(const char*  nodeName, const char*  servName, const SOCAddrInfo*  hints,
                                    SOCAddrInfo **  res)
{
#pragma unused(nodeName, servName, hints, res)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL void SOC_FreeAddrInfo(SOCAddrInfo* head)
{
#pragma unused(head)
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL int SOC_GetNameInfo(const void*  sa, char*  node, unsigned nodeLen, char*  service, unsigned serviceLen,
                                    int flags)
{
#pragma unused(sa, node, nodeLen, service, serviceLen, flags)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 ICMP_Cancel(ICMPInfo* info, s32 err)
{
#pragma unused(info, err)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 ICMP_Socket(int af, ICMPInfo* info)
{
#pragma unused(af, info)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 ICMP_Ping(ICMPInfo*  info, const void*  data, s32 len, const void*  remote, ICMPCallback callback,
                              s32*  result)
{
#pragma unused(info, data, len, remote, callback, result)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL s32 ICMP_Close(ICMPInfo* info)
{
#pragma unused(info)
    return 0;
}

//----------------------------------------------------------------
SDK_WEAK_SYMBOL u16 IPi_CheckSum(IPHeader* ip)
{
#pragma unused(ip)
    return 0;
}
