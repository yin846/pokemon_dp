/*---------------------------------------------------------------------------*
Project: NitroWiFi - Top - include
File: nitroWiFi.h

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: nitroWiFi.h,v $
Revision 1.5  03/10/2006 09:20:22  kitase_hirotake
  INDENT SOURCE

Revision 1.4  07/15/2005 08:35:20  yasu
added SSL related header file

Revision 1.3  07/07/2005 10:43:50  terui
added WCM library support
added support for conversion from IW_ to WCM_

Revision 1.2  06/22/2005 13:18:06  yasu
support for conversion from SO_ to SOC_

Revision 1.1  06/21/2005 13:04:43  yasu
created header file registry template
  
$NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_H_
#define NITROWIFI_H_

#ifdef SDK_ARM9

#include <nitroWiFi/socket.h>
#include <nitroWiFi/wcm.h>

#include <nitroWiFi/ssl.h>

#ifndef SDKWIFI_NO_SO_SYMBOLS
#include <nitroWiFi/so2soc.h>
#include <nitroWiFi/iw2wcm.h>
#endif

#else //SDK_ARM7
#endif

#endif //NITROWIFI_H_
