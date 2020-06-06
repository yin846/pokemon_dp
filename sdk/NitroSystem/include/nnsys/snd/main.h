/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     main.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.h,v $
  Revision 1.17  02/17/2005 07:51:33  ida
  Changed function names from Get to Read

  Revision 1.16  02/16/2005 07:46:27  ida
  Added driver information functions

  Revision 1.15  10/07/2004 08:44:41  ida
  auto sleep process

  Revision 1.14  10/07/2004 05:57:40  ida
  add NNS_SndSetMonoFlag

  Revision 1.13  08/19/2004 06:43:49  ida
  for sleep mode

  Revision 1.12  08/10/2004 02:28:30  ida
  add NNS_SndStopSoundAll

  Revision 1.11  07/16/2004 06:42:58  ida
  fix Project name

  Revision 1.10  06/23/2004 06:36:51  ida
  workaround shared work

  Revision 1.9  06/21/2004 01:21:21  ida
  remove NNSi_SndSetOutputMode

  Revision 1.8  06/09/2004 08:45:58  ida
  add NNS_SndStopChannelAll

  Revision 1.7  05/06/2004 04:47:32  ida
  workaround SNDSharedWork

  Revision 1.6  04/28/2004 07:47:26  ida
  fixed comments

  Revision 1.5  04/02/2004 06:49:15  ida
  hide SetOutputMode

  Revision 1.4  03/25/2004 10:06:35  ida
  add SetOutputMode

  Revision 1.3  02/20/2004 05:03:08  ida
  change directory

  Revision 1.1  02/13/2004 01:44:09  ida
  initial version
  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_MAIN_H_
#define NNS_SND_MAIN_H_

#include <nitro/snd.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	public function declaration
 ******************************************************************************/

void NNS_SndInit( void );
void NNS_SndMain( void );

void NNS_SndStopSoundAll( void );
void NNS_SndStopChannelAll( void );
void NNS_SndSetMasterVolume( int volume );
void NNS_SndSetMonoFlag( BOOL flag );

BOOL NNS_SndUpdateDriverInfo( void );
BOOL NNS_SndReadDriverChannelInfo( int chNo, SNDChannelInfo* info );

/******************************************************************************
	private function declaration
 ******************************************************************************/
BOOL NNSi_SndReadDriverPlayerInfo( int playerNo, SNDPlayerInfo* info );
BOOL NNSi_SndReadDriverTrackInfo( int playerNo, int trackNo, SNDTrackInfo* info );

/******************************************************************************
	ghost functions
 ******************************************************************************/

static inline void NNS_SndBeginSleep( void ) {}
static inline void NNS_SndEndSleep( void ) {}


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_MAIN_H_*/


