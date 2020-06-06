/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     output_effect.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: output_effect.h,v $
  Revision 1.3  01/17/2005 02:54:16  ida
  Updated copyright year notation

  Revision 1.2  01/17/2005 01:26:23  ida
  added NNS_SndCaptureSetPostOutputEffectCallback

  Revision 1.1  10/04/2004 08:50:31  ida
  initial version

  Revision 1.2  09/15/2004 04:53:40  ida
  change function interface

  Revision 1.1  08/02/2004 05:38:21  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_OUTPUT_EFFECT_H_
#define NNS_SND_OUTPUT_EFFECT_H_

#include <nitro/types.h>
#include <nnsys/snd/capture.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	enum declaration
 ******************************************************************************/

typedef enum {
    NNS_SND_CAPTURE_OUTPUT_EFFECT_NORMAL,
    NNS_SND_CAPTURE_OUTPUT_EFFECT_SURROUND,
    NNS_SND_CAPTURE_OUTPUT_EFFECT_HEADPHONE,
    NNS_SND_CAPTURE_OUTPUT_EFFECT_MONO
} NNSSndCaptureOutputEffectType;

/******************************************************************************
	public function declaration
 ******************************************************************************/

BOOL NNS_SndCaptureStartOutputEffect( void* buffer_p, u32 bufSize, NNSSndCaptureOutputEffectType type );
void NNS_SndCaptureStopOutputEffect( void );
void NNS_SndCaptureChangeOutputEffect( NNSSndCaptureOutputEffectType type );
void NNS_SndCaptureSetOutputEffectCallback( NNSSndCaptureCallback func, void* arg );
void NNS_SndCaptureSetPostOutputEffectCallback( NNSSndCaptureCallback func, void* arg );

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_OUTPUT_EFFECT_H_*/


