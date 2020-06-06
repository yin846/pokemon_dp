/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - include
  File:     snd.h

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd.h,v $
  Revision 1.21  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.20  2005/09/08 12:28:09  yasu
  Decision code using SDK_WIN32 has also been added to SDK_FROM_TOOL decision code.

  Revision 1.19  04/12/2005 06:15:11  ida
  Moved SND to NITRO-SDK

  Revision 1.10  04/12/2005 00:53:19  ida
  Added midiplayer.h

  Revision 1.9  02/02/2005 05:10:10  ida
  for SDK_FROM_TOOL

  Revision 1.8  01/19/2005 06:24:31  ida
  Changed so can also refer to bank from tools.

  Revision 1.7  12/09/2004 02:01:49  ida
  Deleted the #include of init.h

  Revision 1.6  09/06/2004 07:25:27  ida
  for SDK_FROM_TOOL

  Revision 1.5  06/23/2004 06:31:45  ida
  change filename

  Revision 1.4  06/11/2004 01:53:49  ida
  port.h -> command.h

  Revision 1.3  05/12/2004 06:39:19  ida
  add data.h

  Revision 1.2  04/08/2004 07:29:02  ida
  fix Project name

  Revision 1.1  03/26/2004 06:26:43  ida
  add SND-SDK header files

  Revision 1.8  03/22/2004 02:05:29  ida
  remove static_channel
  add alarm

  Revision 1.7  03/10/2004 06:32:26  ida
  remove data.h

  Revision 1.6  03/08/2004 05:40:38  ida
  rename files

  Revision 1.5  03/08/2004 04:42:36  ida
  divide port.h -> main.h & port.h

  Revision 1.4  02/20/2004 03:57:05  ida
  add ARM9 snd library

  Revision 1.3  02/19/2004 07:07:58  ida
  add NITRO System snd libraries

  Revision 1.2  02/13/2004 00:17:09  ida
  snd ARM7 -> common

  Revision 1.1  2004/02/12 13:14:09  yasu
  move from snd_sp.h

  Revision 1.4  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  01/13/2004 02:06:30  ida
  Initial version

  Revision 1.2  01/06/2004 07:11:48  ida
  Revised copyright display.    

  Revision 1.1  12/22/2003 04:01:21  ida
  Initial version.
  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_SND_H_
#define NITRO_SND_H_

/* if include from Other Environment for exsample VC or BCB,*/
/* please define SDK_FROM_TOOL*/
#if !(defined(SDK_WIN32) || defined(SDK_FROM_TOOL))

#include <nitro/snd/common/main.h>
#include <nitro/snd/common/alarm.h>
#include <nitro/snd/common/command.h>
#include <nitro/snd/common/global.h>

#ifdef SDK_ARM9

#include <nitro/snd/ARM9/interface.h>

#endif // SDK_ARM9

#endif // SDK_FROM_TOOL

#include <nitro/snd/common/work.h>
#include <nitro/snd/common/channel.h>
#include <nitro/snd/common/capture.h>
#include <nitro/snd/common/exchannel.h>
#include <nitro/snd/common/data.h>
#include <nitro/snd/common/bank.h>
#include <nitro/snd/common/mml.h>
#include <nitro/snd/common/seq.h>
#include <nitro/snd/common/midiplayer.h>
#include <nitro/snd/common/util.h>

#endif /* NITRO_SND_H_*/
