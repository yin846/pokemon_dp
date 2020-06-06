/*---------------------------------------------------------------------------*
Project: NitroWiFi - include - soc
File: socket.h

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl.h,v $
Revision 1.10  03/10/2006 09:20:22  kitase_hirotake
  INDENT SOURCE

Revision 1.9  08/05/2005 05:06:28  seiki_masashi
given its reference in cps.h, added #include <nitro/math.h>
added #include <string.h> for NitroInet compatibility

Revision 1.8  07/18/2005 13:16:45  yasu
created send/receive threads

Revision 1.7  07/15/2005 13:32:51  yasu
adjusted header for soc library

Revision 1.6  07/15/2005 09:28:58  yasu
temporary fixes to get it through the builds

Revision 1.5  06/28/2005 02:36:08  yasu
commented out unused variables

Revision 1.4  06/23/2005 12:57:06  yasu
SDK_BIG_ENDIAN/SDK_LITTLE_ENDIAN support

Revision 1.3  06/22/2005 13:16:54  yasu
prefix change" SO_ -> SOC_

Revision 1.2  06/22/2005 12:07:28  yasu
created a socket API definitions file

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_SOCKET_H_
#define NITROWIFI_SOCKET_H_

#include <string.h>
#include <nitro/types.h>
#include <nitro/os.h>
#include <nitro/math.h>
#include <nitroWiFi/cps.h>
#include <nitroWiFi/socl.h>
#include <nitroWiFi/soc.h>

#ifdef __cplusplus

extern "C" {
#endif

#ifdef __cplusplus

}
#endif

#endif // NITROWIFI_SOCKET_H_
