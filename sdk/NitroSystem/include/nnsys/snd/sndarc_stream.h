/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     sndarc_stream.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sndarc_stream.h,v $
  Revision 1.18  08/18/2005 07:04:39  ida
  For changes in the size of the OSThreadQueue

  Revision 1.17  05/06/2005 02:54:10  ida
  Added NNS_SndArcStrmCreatePrepareThread.

  Revision 1.16  03/24/2005 04:52:01  ida
  Updated copyright year notation

  Revision 1.15  03/23/2005 04:46:26  ida
  Added NNS_SndArcStrmGetTimeLength

  Revision 1.14  11/22/2004 04:28:00  ida
  added NNS_SndArcStrmPrepareEx2 function

  Revision 1.13  11/04/2004 06:20:18  ida
  add NNSi_SndArcStrmStopByChannel

  Revision 1.12  10/21/2004 02:13:42  ida
  NNS_SndArcStrmMain become private

  Revision 1.11  10/19/2004 01:56:04  ida
  add NNSi_SndArcStrmSetPlayerBuffer

  Revision 1.10  10/07/2004 08:44:41  ida
  auto sleep process

  Revision 1.9  10/05/2004 06:59:56  ida
  add NNS_SndArcStrmStartEx

  Revision 1.8  09/15/2004 07:29:39  ida
  for SDK_SMALL_BUILD

  Revision 1.7  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.6  08/23/2004 01:38:26  ida
  add NNS_SndArcStrmSetupPlayer

  Revision 1.5  08/19/2004 06:42:28  ida
  for sleep mode

  Revision 1.4  08/10/2004 02:29:08  ida
  add NNS_SndArcStrmStopAll

  Revision 1.3  08/04/2004 01:59:35  ida
  workaround alloc channel

  Revision 1.2  08/03/2004 05:25:38  ida
  add offset argument

  Revision 1.1  08/02/2004 05:38:21  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_SNDARC_STREAM_H_
#define NNS_SND_SNDARC_STREAM_H_

#ifndef SDK_SMALL_BUILD

#include <nitro/types.h>
#include <nitro/os.h>
#include <nnsys/misc.h>
#include <nnsys/fnd.h>
#include <nnsys/snd/heap.h>
#include <nnsys/snd/stream.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	macro definition
 ******************************************************************************/

#define NNS_SND_STRM_PLAYER_NUM 4
#define NNS_SND_STRM_PLAYER_MIN 0
#define NNS_SND_STRM_PLAYER_MAX 3

#define NNS_SND_STRM_PLAYER_PRIO_MIN 0
#define NNS_SND_STRM_PLAYER_PRIO_MAX 127

#define NNS_SND_STRM_THREAD_STACK_SIZE      1024

/******************************************************************************
	structure declaration
 ******************************************************************************/

struct NNSSndStrmPlayer;

/******************************************************************************
	type definition
 ******************************************************************************/

typedef enum NNSSndArcStrmCallbackStatus
{
    NNS_SND_ARC_STRM_CALLBACK_DATA_END
} NNSSndArcStrmCallbackStatus;

typedef struct NNSSndArcStrmCallbackInfo
{
    int playerNo;
    int strmNo;
} NNSSndArcStrmCallbackInfo;

typedef struct NNSSndArcStrmCallbackParam
{
    int strmNo;
    u32 offset;
} NNSSndArcStrmCallbackParam;

typedef BOOL (*NNSSndArcStrmCallback)(
    NNSSndArcStrmCallbackStatus status,
    const NNSSndArcStrmCallbackInfo* info,
    NNSSndArcStrmCallbackParam* param,
    void* arg
);

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndStrmHandle
{
    struct NNSSndStrmPlayer* player;
} NNSSndStrmHandle;

typedef struct NNSSndStrmThread
{
    OSThread thread;
    u64 stack[ NNS_SND_STRM_THREAD_STACK_SIZE / sizeof(u64) ];
    OSThreadQueue threadQ;
#ifndef SDK_THREAD_INFINITY
#if OS_SIZEOF_OSTHREADQUEUE == 16
    u16 padding;
#endif
#endif
    OSMutex mutex;
    
    NNSFndList commandList;
} NNSSndStrmThread;

/******************************************************************************
	inline functions
 ******************************************************************************/

static inline BOOL NNS_SndStrmHandleIsValid( NNSSndStrmHandle* handle )
{
    NNS_NULL_ASSERT( handle );
    return handle->player != NULL;
}

/******************************************************************************
	public function declaration
 ******************************************************************************/

void NNS_SndArcStrmInit( u32 threadPrio, NNSSndHeapHandle heap );
BOOL NNS_SndArcStrmSetupPlayer( NNSSndHeapHandle heap );
void NNS_SndArcStrmCreatePrepareThread( NNSSndStrmThread* thread, u32 threadPrio );

BOOL NNS_SndArcStrmStart( struct NNSSndStrmHandle* handle, int strmNo, u32 offset );
void NNS_SndArcStrmStop( struct NNSSndStrmHandle* handle, int fadeFrame );
void NNS_SndArcStrmStopAll( int fadeFrame );

BOOL NNS_SndArcStrmPrepare( struct NNSSndStrmHandle* handle, int strmNo, u32 offset );
void NNS_SndArcStrmStartPrepared( struct NNSSndStrmHandle* handle );
BOOL NNS_SndArcStrmIsPrepared( struct NNSSndStrmHandle* handle );

BOOL NNS_SndArcStrmStartEx( struct NNSSndStrmHandle* handle, int playerNo, int playerPrio, int strmNo, u32 offset );
BOOL NNS_SndArcStrmPrepareEx( struct NNSSndStrmHandle* handle, int playerNo, int playerPrio, int strmNo, u32 offset );
BOOL NNS_SndArcStrmStartEx2(
    struct NNSSndStrmHandle* handle,
    int playerNo,
    int playerPrio,
    int strmNo,
    u32 offset,
    NNSSndStrmCallback strmCallback,
    void* strmCallbackArg,
    NNSSndArcStrmCallback sndArcStrmCallback,
    void* sndArcStrmCallbackArg
);
BOOL NNS_SndArcStrmPrepareEx2(
    struct NNSSndStrmHandle* handle,
    int playerNo,
    int playerPrio,
    int strmNo,
    u32 offset,
    NNSSndStrmCallback strmCallback,
    void* strmCallbackArg,
    NNSSndArcStrmCallback sndArcStrmCallback,
    void* sndArcStrmCallbackArg
);

BOOL NNS_SndArcStrmAllocChannel( int playerNo );
void NNS_SndArcStrmFreeChannel( int playerNo );

void NNS_SndArcStrmMoveVolume( struct NNSSndStrmHandle* handle, int volume, int frames );
void NNS_SndArcStrmSetChannelVolume( struct NNSSndStrmHandle* handle, int chNo, int volume );
void NNS_SndArcStrmSetChannelPan( struct NNSSndStrmHandle* handle, int chNo, int pan );

u32 NNS_SndArcStrmGetTimeLength( struct NNSSndStrmHandle* handle );
u32 NNS_SndArcStrmGetCurrentPlayingPos( struct NNSSndStrmHandle* handle );

void NNS_SndStrmHandleInit( struct NNSSndStrmHandle* handle );
void NNS_SndStrmHandleRelease( struct NNSSndStrmHandle* handle );

/******************************************************************************
	private function declaration
 ******************************************************************************/

void NNSi_SndArcStrmMain( void );
void NNSi_SndArcStrmSetPlayerBuffer( int playerNo, void* buffer, u32 len );
void NNSi_SndArcStrmSetDecodeBuffer( void* buffer, u32 len );
void NNSi_SndArcStrmStopByChannel( u32 chBitMask );

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* SDK_SMALL_BUILD*/

#endif /* NNS_SND_SNDARC_STREAM_H_*/


