/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     capture.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: capture.h,v $
  Revision 1.19  08/18/2005 07:03:38 AM  ida
  Added NNS_SndCaptureIsActive function
  Added NNS_SndCaptureIsActive function

  Revision 1.18  10/21/2004 02:11:17 AM  ida
  NNS_SndCaptureInit and NNS_SndCaptureMain become private

  Revision 1.17  09/13/2004 06:38:50 AM  ida
  add NNS_SndCaptureStartSampling

  Revision 1.16  09/06/2004 07:26:05 AM  ida
  adjust interface

  Revision 1.15  08/19/2004 06:42:38 AM  ida
  for sleep mode

  Revision 1.14  07/16/2004 06:42:58 AM  ida
  fix Project name

  Revision 1.13  07/13/2004 12:56:53 AM  ida
  include nitro/types.h

  Revision 1.12  06/29/2004 06:50:10 AM  ida
  add interval argument

  Revision 1.11  06/28/2004 07:11:32 AM  ida
  add effect library

  Revision 1.10  06/11/2004 01:54:49 AM  ida
  remove NNSi_SndCaptureStop

  Revision 1.9  06/09/2004 08:46:25 AM  ida
  add comments

  Revision 1.8  03/26/2004 11:08:26 AM  ida
  brush up

  Revision 1.7  03/26/2004 04:22:47 AM  ida
  workaround reverb

  Revision 1.6  03/25/2004 10:04:20 AM  ida
  CapFormat -> CaptureFormat

  Revision 1.5  03/22/2004 02:19:18 AM  ida
  add effect functions

  Revision 1.4  02/20/2004 05:03:08 AM  ida
  change directory

  Revision 1.2  02/19/2004 08:57:14 AM  ida
  ARM7 -> SDK

  Revision 1.1  02/13/2004 01:44:09 AM  ida
  initial version

  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_CAPTURE_H_
#define NNS_SND_CAPTURE_H_

#include <nitro/types.h>
#include <nitro/snd.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	enum definition
 ******************************************************************************/

typedef enum
{
    NNS_SND_CAPTURE_FORMAT_PCM16,
    NNS_SND_CAPTURE_FORMAT_PCM8
} NNSSndCaptureFormat;

typedef enum {
    NNS_SND_CAPTURE_TYPE_REVERB,
    NNS_SND_CAPTURE_TYPE_EFFECT,
    NNS_SND_CAPTURE_TYPE_SAMPLING
} NNSSndCaptureType;

/******************************************************************************
	type definition
 ******************************************************************************/

typedef void (*NNSSndCaptureCallback)(
    void* bufferL,
    void* bufferR,
    u32 len,
    NNSSndCaptureFormat format,
    void* arg
);
typedef NNSSndCaptureCallback NNSSndCaptureEffectCallback;

/******************************************************************************
	public function declaration
 ******************************************************************************/

void NNS_SndCaptureCreateThread( u32 threadPrio );

BOOL NNS_SndCaptureStartReverb(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int volume
);
void NNS_SndCaptureStopReverb( int frames );
void NNS_SndCaptureSetReverbVolume( int volume, int frames );

BOOL NNS_SndCaptureStartEffect(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int interval,
    NNSSndCaptureEffectCallback callback,
    void* arg
);
void NNS_SndCaptureStopEffect( void );

BOOL NNS_SndCaptureStartSampling(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int interval,
    NNSSndCaptureCallback callback,
    void* arg
);
void NNS_SndCaptureStopSampling( void );

BOOL NNS_SndCaptureIsActive( void );
NNSSndCaptureType NNS_SndCaptureGetCaptureType( void );

/******************************************************************************
	private function declaration
 ******************************************************************************/

void NNSi_SndCaptureInit( void );
void NNSi_SndCaptureMain( void );

BOOL NNSi_SndCaptureStart(
    NNSSndCaptureType type,
    void* buffer0,
    void* buffer1,
    u32 bufLen,
    NNSSndCaptureFormat format,
    SNDCaptureIn input,
    SNDCaptureOut output,
    BOOL loopFlag,
    int sampleRate,
    int volume,
    int pan0,
    int pan1,
    int interval,
    NNSSndCaptureCallback callback,
    void* arg    
);
void NNSi_SndCaptureStop( void );

void NNSi_SndCaptureBeginSleep( void );
void NNSi_SndCaptureEndSleep( void );

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_CAPTURE_H_*/


