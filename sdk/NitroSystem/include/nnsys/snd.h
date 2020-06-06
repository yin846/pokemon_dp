/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     snd.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd.h,v $
  Revision 1.20  03/09/2005 00:25:01  ida
  Updated copyright year notation

  Revision 1.19  03/08/2005 05:44:32  ida
  Support for NNS_FROM_TOOL

  Revision 1.18  11/25/2004 05:35:19  ida
  using NNS_FROM_TOOL

  Revision 1.17  10/04/2004 08:52:57  ida
  add output_effect.h

  Revision 1.16  09/06/2004 07:25:47  ida
  for SDK_FROM_TOOL

  Revision 1.15  08/02/2004 05:37:24  ida
  add stream library

  Revision 1.14  07/16/2004 06:42:58  ida
  fix Project name

  Revision 1.13  06/28/2004 07:18:01  ida
  lock.h -> resource_mgr.h

  Revision 1.12  06/23/2004 06:36:14  ida
  add waveout.h

  Revision 1.11  06/21/2004 01:20:33  ida
  add lock.h

  Revision 1.10  05/13/2004 01:01:36  ida
  merge seqarc.h and data.h to seqdata.h

  Revision 1.9  04/26/2004 02:23:19  ida
  add heap.h sndarc_loader.h

  Revision 1.8  02/20/2004 05:03:08  ida
  change directory

  Revision 1.7  02/19/2004 08:57:14  ida
  ARM7 -> SDK

  Revision 1.6  02/13/2004 01:44:09  ida
  initial version

  Revision 1.5  02/09/2004 00:20:52  ida
  IRIS -> NITRO

  Revision 1.4  02/05/2004 05:28:25  ida
  Added player.h

  Revision 1.3  02/05/2004 05:02:32  ida
  (none)

  Revision 1.2  02/04/2004 07:37:20  ida
  (none)

  Revision 1.1  02/02/2004 04:50:43  ida
  Added snd library
  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_H_
#define NNS_SND_H_

#ifndef NNS_FROM_TOOL

#include <nnsys/snd/main.h>
#include <nnsys/snd/player.h>
#include <nnsys/snd/capture.h>
#include <nnsys/snd/sndarc_player.h>
#include <nnsys/snd/resource_mgr.h>
#include <nnsys/snd/waveout.h>
#include <nnsys/snd/stream.h>
#include <nnsys/snd/sndarc_stream.h>
#include <nnsys/snd/fader.h>
#include <nnsys/snd/output_effect.h>
#include <nnsys/snd/sndarc_loader.h>
#include <nnsys/snd/heap.h>

#endif // NNS_FROM_TOOL

#include <nnsys/snd/sndarc.h>
#include <nnsys/snd/seqdata.h>

#endif /* NNS_SND_H_*/


