/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os.h,v $
  Revision 1.36  07/27/2005 08:37:55  seiki_masashi
  Added entropy.h

  Revision 1.35  07/21/2005 08:06:09  yada
  add argument.h

  Revision 1.34  04/25/2005 09:25:01  terui
  Add to header that includes china.h for ARM9

  Revision 1.33  02/28/2005 06:26:02  yosizaki
  do-indent.

  Revision 1.32  02/09/2005 10:15:36  terui
  Add to header that includes vramExclusive.h for ARM9

  Revision 1.31  09/02/2004 08:12:37  yada
  add ownerInfo.h

  Revision 1.30  09/01/2004 05:20:27  yada
  add reset.h

  Revision 1.29  05/10/2004 13:01:24  yada
  add profile.h functionCost.h

  Revision 1.28  04/12/2004 13:11:28  yada
  add callTrace.h

  Revision 1.27  2004/02/27 01:55:00  yasu
  add OS_SpinWait, move OS_Halt into os_system.c

  Revision 1.26  02/25/2004 12:29:25  yada
  Changes related to switch from systemClock to Tick

  Revision 1.25  02/23/2004 13:30:33  yada
  valarm.h to both A9/7

  Revision 1.24  02/23/2004 12:55:38  yada
  Added valarm.h

  Revision 1.23  02/12/2004 12:41:25  yada
  Added alloc.h

  Revision 1.22  2004/02/12 10:56:04  yasu
  new location of include files ARM9/ARM7

  Revision 1.21  02/10/2004 02:59:29  yada
  Added system.h

  Revision 1.20  02/10/2004 02:45:39  yada
  Removed utility.h

  Revision 1.19  02/10/2004 02:28:38  yada
  Removed utility.h

  Revision 1.18  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.17  02/05/2004 02:59:45  yada
  Added utility.h

  Revision 1.16  02/03/2004 12:18:44  yada
  Added alarm.h

  Revision 1.15  02/02/2004 06:29:35  yada
  Added systemClock.h

  Revision 1.14  12/26/2003 07:44:26  yada
  Added arena

  Revision 1.13  12/22/2003 02:34:39  yada
  Added os/init.h

  Revision 1.12  12/18/2003 08:18:04  yada
  Removed include of os/writeBuffer.h

  Revision 1.11  12/12/2003 02:27:14  yada
  Corrected due to the name change of include (os/system.h and hw/armArch.h)

  Revision 1.10  12/08/2003 13:21:31  yada
  Added file to include

  Revision 1.9  2003/11/30 04:20:42  yasu
  Added cpsr.h

  Revision 1.8  11/29/2003 11:11:29  yada
  Added mutex.h

  Revision 1.7  11/26/2003 02:50:58  yada
  Added message.h

  Revision 1.6  2003/11/25 11:18:36  yasu
  Added emulator.h

  Revision 1.5  11/25/2003 06:53:10  yada
  Updated os

  Revision 1.4  2003/11/25 05:00:39  yasu
  Added thread.h/context.h

  Revision 1.3  2003/11/17 00:23:22  yasu
  Separated OS_Printf, etc. to iris/os/printf.h

  Revision 1.2  2003/11/14 06:14:12  yasu
  Added OS_Warning() OS_Panic()

  Revision 1.1  2003/11/13 11:59:25  yasu
  Created functions relating to OS_Printf

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_H_
#define NITRO_OS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/os/common/system.h>
#include <nitro/os/common/interrupt.h>
#include <nitro/os/common/thread.h>
#include <nitro/os/common/context.h>
#include <nitro/os/common/spinLock.h>
#include <nitro/os/common/timer.h>
#include <nitro/os/common/systemCall.h>
#include <nitro/os/common/printf.h>
#include <nitro/os/common/emulator.h>
#include <nitro/os/common/message.h>
#include <nitro/os/common/mutex.h>
#include <nitro/os/common/exception.h>
#include <nitro/os/common/init.h>
#include <nitro/os/common/arena.h>
#include <nitro/os/common/alloc.h>
#include <nitro/os/common/tick.h>
#include <nitro/os/common/alarm.h>
#include <nitro/os/common/valarm.h>
#include <nitro/os/common/system.h>
#include <nitro/os/common/reset.h>
#include <nitro/os/common/ownerInfo.h>
#include <nitro/os/common/entropy.h>

//---- profiler
#include <nitro/os/common/profile.h>
#include <nitro/os/common/callTrace.h>
#include <nitro/os/common/functionCost.h>

#ifdef	SDK_ARM9
#include <nitro/os/ARM9/protectionRegion.h>
#include <nitro/os/ARM9/tcm.h>
#include <nitro/os/ARM9/cache.h>
#include <nitro/os/ARM9/protectionUnit.h>
#include <nitro/os/ARM9/vramExclusive.h>
#include <nitro/os/ARM9/china.h>
#include <nitro/os/ARM9/argument.h>
#else  //SDK_ARM7
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

/* NITRO_OS_H_ */
#endif
