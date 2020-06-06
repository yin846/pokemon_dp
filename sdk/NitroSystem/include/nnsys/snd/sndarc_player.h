/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     sndarc_player.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sndarc_player.h,v $
  Revision 1.16  07/16/2004 06:42:58  ida
  fix Project name

  Revision 1.15  06/23/2004 06:37:34  ida
  erase compatibility code

  Revision 1.14  06/08/2004 00:52:09  ida
  workaround player heap

  Revision 1.13  05/27/2004 06:35:50  ida
  added NNS_SndArcPlayerAutoLoad

  Revision 1.12  05/24/2004 05:41:44  ida
  WithPlayerNo -> inline function

  Revision 1.11  05/13/2004 06:40:59  ida
  added NNS_SndArcPlayerAutoLoad*

  Revision 1.10  05/10/2004 01:35:01  ida
  added NNS_SndArcPlayerStartSeq*Ex

  Revision 1.9  04/30/2004 05:05:38  ida
  fixed comments

  Revision 1.8  04/26/2004 02:24:35  ida
  for file system

  Revision 1.7  03/30/2004 05:36:13  ida
  hid functions

  Revision 1.6  03/25/2004 10:08:46  ida
  added StartSeqWithPlayerNo

  Revision 1.5  03/24/2004 06:28:01  ida
  added GetPlayingPlayerNo

  Revision 1.4  03/22/2004 07:24:41  ida
  added StartSeqEx functions

  Revision 1.3  02/20/2004 05:03:08  ida
  change directory

  Revision 1.1  02/13/2004 01:44:09  ida
  initial version
  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_SNDARC_PLAYER_H_
#define NNS_SND_SNDARC_PLAYER_H_

#include <nitro/types.h>
#include <nnsys/snd/heap.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	structure declaration
 ******************************************************************************/

struct NNSSndHandle;

/******************************************************************************
	public function declaration
 ******************************************************************************/

BOOL NNS_SndArcPlayerSetup( NNSSndHeapHandle heap );

BOOL NNS_SndArcPlayerStartSeq( struct NNSSndHandle* handle, int seqNo );
BOOL NNS_SndArcPlayerStartSeqArc( struct NNSSndHandle* handle, int seqArcNo, int index );

BOOL NNS_SndArcPlayerStartSeqEx(
    struct NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    int seqNo
);
BOOL NNS_SndArcPlayerStartSeqArcEx(
    struct NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    int seqArcNo,
    int index
);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_SNDARC_PLAYER_H_*/


