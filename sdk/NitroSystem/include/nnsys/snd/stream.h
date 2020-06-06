/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     stream.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: stream.h,v $
  Revision 1.6  05/30/2005 23:40:52  ida
  Updated copyright year notation

  Revision 1.5  05/30/2005 07:29:46  ida
  Support for proDG

  Revision 1.4  10/07/2004 08:44:41  ida
  auto sleep process

  Revision 1.3  08/19/2004 06:42:59  ida
  for sleep mode

  Revision 1.2  08/04/2004 01:59:48  ida
  workaround alloc channel

  Revision 1.1  08/02/2004 05:38:21  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_STREAM_H_
#define NNS_SND_STREAM_H_

#include <nitro/types.h>
#include <nitro/snd.h>
#include <nitro/spi.h>
#include <nnsys/fnd/list.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	macro definition
 ******************************************************************************/

#define NNS_SND_STRM_CHANNEL_MAX 16

#define NNS_SND_STRM_PAN_MIN   0
#define NNS_SND_STRM_PAN_MAX 127

#define NNS_SND_STRM_TIMER_CLOCK ( SND_TIMER_CLOCK / 32 )
#define NNS_SND_STRM_TIMER_MIN ( ( SND_CHANNEL_TIMER_MIN + 31 ) / 32 )
#define NNS_SND_STRM_TIMER_MAX (   SND_CHANNEL_TIMER_MAX        / 32 )

/******************************************************************************
	type definition
 ******************************************************************************/

typedef enum NNSSndStrmFormat
{
    NNS_SND_STRM_FORMAT_PCM8,
    NNS_SND_STRM_FORMAT_PCM16
} NNSSndStrmFormat;

typedef enum NNSSndStrmCallbackStatus
{
    NNS_SND_STRM_CALLBACK_SETUP,
    NNS_SND_STRM_CALLBACK_INTERVAL
} NNSSndStrmCallbackStatus;

typedef void (*NNSSndStrmCallback)(
    NNSSndStrmCallbackStatus status,
    int numChannles,
    void* buffer[],
    u32 len,
    NNSSndStrmFormat format,
    void* arg
);

/******************************************************************************
	structure definition
 ******************************************************************************/
typedef struct NNSSndStrm
{
    NNSFndLink link;
    PMSleepCallbackInfo preSleepInfo;
    PMSleepCallbackInfo postSleepInfo;
    
    NNSSndStrmFormat format;
    BOOL activeFlag : 1;
    BOOL startFlag:   1;
    
    u32 chBufLen;
    int interval;
    NNSSndStrmCallback callback;
    void* callbackArg;
    int curBuffer;
    
    int volume;
    
    int alarmNo;
    u32 chBitMask;
    int numChannels;
    u8 channelNo[ NNS_SND_STRM_CHANNEL_MAX ];
} NNSSndStrm;

/******************************************************************************
	public function declaration
 ******************************************************************************/

void NNS_SndStrmInit( NNSSndStrm* stream );

BOOL NNS_SndStrmAllocChannel( NNSSndStrm* stream, int numChannels, const u8 chNoList[] );
void NNS_SndStrmFreeChannel( NNSSndStrm* stream );


BOOL NNS_SndStrmSetup(
    NNSSndStrm* stream,
    NNSSndStrmFormat format,
    void* buffer,
    u32 bufSize,
    int timer,
    int interval,
    NNSSndStrmCallback callback,
    void* arg
);
void NNS_SndStrmStart( NNSSndStrm* stream );
void NNS_SndStrmStop( NNSSndStrm* stream );

void NNS_SndStrmSetVolume( NNSSndStrm* stream, int volume );
void NNS_SndStrmSetChannelVolume( NNSSndStrm* stream, int chNo, int volume );
void NNS_SndStrmSetChannelPan( NNSSndStrm* stream, int chNo, int pan );

/******************************************************************************
	ghost functions
 ******************************************************************************/

static inline void NNS_SndStrmBeginSleep( NNSSndStrm* stream ) {
#pragma unused( stream )
}
static inline void NNS_SndStrmEndSleep( NNSSndStrm* stream ) {
#pragma unused( stream )
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_STREAM_H_*/


