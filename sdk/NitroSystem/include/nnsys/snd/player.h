/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     player.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: player.h,v $
  Revision 1.55  04/27/2005 02:29:22  ida
  Replaced with implementation that use SND_PrepareSeq

  Revision 1.54  03/17/2005 07:31:44  ida
  Added NNS_SndPlayerSetPlayerVolume function

  Revision 1.53  03/01/2005 02:12:59  ida
  Added NNS_SndPlayerSetTrackPanRange function

  Revision 1.52  02/17/2005 07:51:33  ida
  Changed function names from Get to Read

  Revision 1.51  02/16/2005 07:46:27  ida
  Added driver information functions

  Revision 1.50  01/06/2005 04:07:49  ida
  Updated copyright year notation

  Revision 1.49  01/05/2005 06:45:42  ida
  Added NNS_SndPlayerSetTrackAllocatableChannel function

  Revision 1.48  10/21/2004 02:11:04  ida
  NNS_SndPlayerInit and NNS_SndPlayerMain become private

  Revision 1.47  10/19/2004 01:55:17  ida
  fix unmatched variable name

  Revision 1.46  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.45  08/02/2004 05:37:49  ida
  use NNSSndFader

  Revision 1.44  07/16/2004 06:42:58  ida
  fix Project name

  Revision 1.43  06/23/2004 06:37:22  ida
  erase compatibility code

  Revision 1.42  06/21/2004 01:21:49  ida
  add NNSSndSeqPlayer::startFlag

  Revision 1.41  06/11/2004 01:55:05  ida
  portTag -> commandTag

  Revision 1.40  06/09/2004 06:02:25  ida
  add range macro

  Revision 1.39  06/08/2004 00:50:19  ida
  workaround player heap

  Revision 1.38  05/28/2004 08:09:43  ida
  change function name

  Revision 1.37  05/24/2004 07:33:22  ida
  change function name

  Revision 1.36  05/24/2004 05:41:10  ida
  ByNumber -> ByPlayerNo

  Revision 1.35  05/21/2004 06:57:46  ida
  NNS_SndHandleReleasePlayer -> NNS_SndHandleReleaseSeq

  Revision 1.34  05/21/2004 02:11:08  ida
  add NNS_SND_PLAYER_NO_MAX
  add NNS_SND_PLAYER_PRIO_MAX

  Revision 1.33  05/12/2004 06:39:58  ida
  modify data header structure

  Revision 1.32  05/10/2004 05:29:41  ida
  add NNS_SndPlayerGetTick

  Revision 1.31  05/10/2004 04:59:28  ida
  add NNS_SndPlayer[Read/Write]Variable

  Revision 1.30  05/10/2004 04:07:52  ida
  add NNS_SndPlayerGetSeqType

  Revision 1.29  05/10/2004 01:34:38  ida
  add NNS_SndPlayerSet*Priority

  Revision 1.28  05/10/2004 00:24:36  ida
  player num : 16 -> 32

  Revision 1.27  04/30/2004 07:35:38  ida
  add NNS_SndPlayerGetSeqNo

  Revision 1.26  04/30/2004 06:30:40  ida
  add NNS_SndPlayerCountPlayingSeqBySeqNo
  add NNS_SndPlayerStopSeqBySeqNo

  Revision 1.25  04/28/2004 07:47:26  ida
  fixed comments

  Revision 1.24  04/28/2004 01:55:49  ida
  add comments

  Revision 1.23  04/26/2004 02:22:35  ida
  remove NNS_SndPlayerStartSeqArc

  Revision 1.22  04/15/2004 04:19:28  ida
  use NNSFndList

  Revision 1.21  04/12/2004 01:25:46  ida
  PLAYER_NUM 32 -> 16

  Revision 1.20  04/08/2004 08:16:09  ida
  add NNS_SndPlayerSetTrackMute

  Revision 1.19  04/08/2004 08:07:37  ida
  workaround multi sequence player

  Revision 1.18  04/02/2004 06:46:30  ida
  IsPlayingByNumber -> CountPlayingSeqByNumber

  Revision 1.17  03/30/2004 08:26:23  ida
  fix move volume bug

  Revision 1.16  03/26/2004 11:08:38  ida
  modified fadeout

  Revision 1.15  03/24/2004 06:27:02  ida
  add parameter functions

  Revision 1.14  03/22/2004 10:43:21  ida
  NNS_SndHandleIsValid

  Revision 1.13  03/22/2004 07:23:51  ida
  add ByNumber functions

  Revision 1.12  03/18/2004 00:40:08  ida
  add tempo change function

  Revision 1.11  03/10/2004 08:40:10  ida
  reconstruct data structure

  Revision 1.10  03/10/2004 06:36:08  ida
  (none)

  Revision 1.9  03/08/2004 06:58:51  ida
  using port tag system

  Revision 1.8  02/25/2004 05:50:06  ida
  add NNS_SndPlayerPause

  Revision 1.7  02/25/2004 05:38:54  ida
  workaround track parameters

  Revision 1.6  02/25/2004 02:32:24  ida
  workaround fadeOut

  Revision 1.5  02/20/2004 07:45:29  ida
  remove fadeOut argument

  Revision 1.4  02/20/2004 05:03:08  ida
  change directory

  Revision 1.2  02/19/2004 08:57:14  ida
  ARM7 -> SDK

  Revision 1.1  02/13/2004 01:44:09  ida
  initial version
  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_PLAYER_H_
#define NNS_SND_PLAYER_H_

#include <nitro/types.h>
#include <nnsys/misc.h>  // for NNS_NULL_ASSERT
#include <nnsys/fnd.h>   // for NNSFndList and NNSFndLink
#include <nnsys/snd/heap.h>  // for NNSSndHeapHandle
#include <nnsys/snd/fader.h>  // for NNSSndFader

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	macro definition
 ******************************************************************************/

#define NNS_SND_PLAYER_NUM 32

#define NNS_SND_PLAYER_NO_MAX (NNS_SND_PLAYER_NUM-1)

#define NNS_SND_PLAYER_TRACK_PITCH_MIN -32768
#define NNS_SND_PLAYER_TRACK_PITCH_MAX  32767
#define NNS_SND_PLAYER_TRACK_PAN_MIN -128
#define NNS_SND_PLAYER_TRACK_PAN_MAX  127
#define NNS_SND_PLAYER_TRACK_PAN_RANGE_MIN    0
#define NNS_SND_PLAYER_TRACK_PAN_RANGE_MAX  127
#define NNS_SND_PLAYER_PRIO_MAX 127

#define NNS_SND_PLAYER_SEQ_NO_MAX       0xffff
#define NNS_SND_PLAYER_SEQARC_INDEX_MAX 0xffff

/******************************************************************************
	struct declaration
 ******************************************************************************/

struct SNDBankData;
struct NNSSndSeqArcHeader;
struct NNSSndSeqHeader;
struct NNSSndSeqPlayer;
struct NNSSndPlayer;
struct NNSSndPlayerHeap;

struct SNDPlayerInfo;
struct SNDTrackInfo;

/******************************************************************************
	enum definition
 ******************************************************************************/
    
enum NNSSndSeqPlayerStatus
{
    NNS_SND_SEQ_PLAYER_STATUS_STOP,
    NNS_SND_SEQ_PLAYER_STATUS_PLAY,
    NNS_SND_SEQ_PLAYER_STATUS_FADEOUT
};

typedef enum NNSSndPlayerSeqType {
    NNS_SND_PLAYER_SEQ_TYPE_INVALID,
    NNS_SND_PLAYER_SEQ_TYPE_SEQ,
    NNS_SND_PLAYER_SEQ_TYPE_SEQARC
} NNSSndPlayerSeqType;

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndHandle
{
    struct NNSSndSeqPlayer* player;
} NNSSndHandle;

typedef struct NNSSndSeqPlayer
{
    struct NNSSndHandle* handle;
    struct NNSSndPlayer* player;
    struct NNSSndPlayerHeap* heap;
    NNSFndLink playerLink;
    NNSFndLink prioLink;
    NNSSndFader fader;
    
    u8 status;
    u8 startFlag;
    u8 pauseFlag;
    u8 prepareFlag;
    
    u32 commandTag;
    
    u16 seqType;
    u16 pad2;
    u16 seqNo;
    u16 seqArcIndex;
    
    u8 playerNo;
    u8 prio;    
    s16 volume;
    
    u8 initVolume;
    u8 extVolume;
    u16 pad3_;
} NNSSndSeqPlayer;

typedef struct NNSSndPlayer
{
    NNSFndList playerList; // lower priority order
    NNSFndList heapList;
    u32 playableSeqCount;
    u32 allocChBitFlag;
    u8 volume;
    u8 pad_;
    u16 pad2_;
} NNSSndPlayer;

/******************************************************************************
	inline functions
 ******************************************************************************/

static inline BOOL NNS_SndHandleIsValid( struct NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );
    return handle->player != NULL;
}

/******************************************************************************
	public function declaration
 ******************************************************************************/

void NNS_SndPlayerStopSeq( NNSSndHandle* handle, int fadeFrame );
void NNS_SndPlayerStopSeqByPlayerNo( int playerNo, int fadeFrame );
void NNS_SndPlayerStopSeqBySeqNo( int seqNo, int fadeFrame );
void NNS_SndPlayerStopSeqBySeqArcNo( int seqArcNo, int fadeFrame );
void NNS_SndPlayerStopSeqBySeqArcIdx( int seqArcNo, int index, int fadeFrame );
void NNS_SndPlayerStopSeqAll( int fadeFrame );

void NNS_SndPlayerPause( NNSSndHandle* handle, BOOL flag );
void NNS_SndPlayerPauseByPlayerNo( int playerNo, BOOL flag );
void NNS_SndPlayerPauseAll( BOOL flag );


void NNS_SndPlayerSetTempoRatio( NNSSndHandle* handle, int ratio );
void NNS_SndPlayerSetVolume( NNSSndHandle* handle, int volume );
void NNS_SndPlayerSetInitialVolume( NNSSndHandle* handle, int volume );
void NNS_SndPlayerMoveVolume( NNSSndHandle* handle, int targetVolume, int frames );
void NNS_SndPlayerSetPlayerPriority( NNSSndHandle* handle, int priority );
void NNS_SndPlayerSetChannelPriority( NNSSndHandle* handle, int priority );

void NNS_SndPlayerSetTrackMute( NNSSndHandle* handle, u16 trackBitMask, BOOL flag );
void NNS_SndPlayerSetTrackVolume( NNSSndHandle* handle, u16 trackBitMask, int volume );
void NNS_SndPlayerSetTrackPitch( NNSSndHandle* handle, u16 trackBitMask, int pitch );
void NNS_SndPlayerSetTrackPan( NNSSndHandle* handle, u16 trackBitMask, int pan );
void NNS_SndPlayerSetTrackPanRange( NNSSndHandle* handle, u16 trackBitMask, int panRange );
void NNS_SndPlayerSetTrackModDepth( NNSSndHandle* handle, u16 trackBitMask, int depth );
void NNS_SndPlayerSetTrackModSpeed( NNSSndHandle* handle, u16 trackBitMask, int speed );
void NNS_SndPlayerSetTrackAllocatableChannel( NNSSndHandle* handle, u16 trackBitMask, u32 chBitFlag );


u32 NNS_SndPlayerGetTick( NNSSndHandle* handle );
int NNS_SndPlayerCountPlayingSeqByPlayerNo( int playerNo );
int NNS_SndPlayerCountPlayingSeqBySeqNo( int seqNo );
int NNS_SndPlayerCountPlayingSeqBySeqArcNo( int seqArcNo );
int NNS_SndPlayerCountPlayingSeqBySeqArcIdx( int seqArcNo, int index );


void NNS_SndPlayerSetSeqNo( NNSSndHandle* handle, int seqNo );
void NNS_SndPlayerSetSeqArcNo( NNSSndHandle* handle, int seqArcNo, int index );
NNSSndPlayerSeqType NNS_SndPlayerGetSeqType( NNSSndHandle* handle );
int NNS_SndPlayerGetSeqNo( NNSSndHandle* handle );
int NNS_SndPlayerGetSeqArcNo( NNSSndHandle* handle );
int NNS_SndPlayerGetSeqArcIdx( NNSSndHandle* handle );


BOOL NNS_SndPlayerReadVariable( NNSSndHandle* handle, int varNo, s16* var );
BOOL NNS_SndPlayerReadGlobalVariable( int varNo, s16* var );
BOOL NNS_SndPlayerWriteVariable( NNSSndHandle* handle, int varNo, s16 var );
BOOL NNS_SndPlayerWriteGlobalVariable( int varNo, s16 var );


void NNS_SndPlayerSetPlayerVolume( int playerNo, int volume );
void NNS_SndPlayerSetPlayableSeqCount( int playerNo, int seqCount );
void NNS_SndPlayerSetAllocatableChannel( int playerNo, u32 chBitFlag );
BOOL NNS_SndPlayerCreateHeap( int playerNo, NNSSndHeapHandle heap, u32 size );

BOOL NNS_SndPlayerReadDriverPlayerInfo( NNSSndHandle* handle, struct SNDPlayerInfo* info );
BOOL NNS_SndPlayerReadDriverTrackInfo( NNSSndHandle* handle, int trackNo, struct SNDTrackInfo* info );

void NNS_SndHandleInit( NNSSndHandle* handle );
void NNS_SndHandleReleaseSeq( NNSSndHandle* handle );


/******************************************************************************
	private function declaration
 ******************************************************************************/

void NNSi_SndPlayerInit( void );
void NNSi_SndPlayerMain( void );

void NNSi_SndPlayerStartSeq(
    NNSSndSeqPlayer* seqPlayer,
    const void* seqDataBase,
    u32 seqDataOffset,
    const struct SNDBankData* bank
);

NNSSndSeqPlayer* NNSi_SndPlayerAllocSeqPlayer( NNSSndHandle* handle, int playerNo, int prio );
void NNSi_SndPlayerFreeSeqPlayer( NNSSndSeqPlayer* seqPlayer );

void NNSi_SndPlayerStopSeq( NNSSndSeqPlayer* seqPlayer, int fadeFrame );
void NNSi_SndPlayerPause( NNSSndSeqPlayer* seqPlayer, BOOL flag );

NNSSndHeapHandle NNSi_SndPlayerAllocHeap( int playerNo, NNSSndSeqPlayer* seqPlayer );

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_PLAYER_H_*/


