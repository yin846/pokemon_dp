/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     waveout.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: waveout.c,v $
  Revision 1.8  02/21/2005 02:37:17  ida
  Warning measures

  Revision 1.7  10/07/2004 04:26:43  ida
  fix typo SND_CHANNEL_LOOP_REPEAT

  Revision 1.6  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.5  08/02/2004 05:29:26  ida
  add comments

  Revision 1.4  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.3  07/05/2004 06:27:21  ida
  unuse SND_GetSharedWork

  Revision 1.2  06/28/2004 07:12:31  ida
  for new snd_drv interface

  Revision 1.1  06/23/2004 06:20:43  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/waveout.h>

#include <nitro/snd.h>
#include <nnsys/misc.h>
#include <nnsys/snd/resource_mgr.h>

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndWaveOut
{
    int chNo;
    int sampleRate;
    BOOL activeFlag;
    BOOL startFlag;
    u32 startCommandTag;
} NNSSndWaveOut;

/******************************************************************************
	static variable
 ******************************************************************************/

static NNSSndWaveOut sWaveOut[ SND_CHANNEL_NUM ];

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutAllocChannel

  Description:  Allocates channel for waveform play.

  Arguments:    chNo - Channel number

  Returns:      If allocation succeeds, returns waveform play handle.
                If allocation fails, returns NNS_SND_WAVEOUT_INVALID_HANDLE.
 *---------------------------------------------------------------------------*/
NNSSndWaveOutHandle NNS_SndWaveOutAllocChannel( int chNo )
{
    NNSSndWaveOut* waveout;
    
    NNS_MINMAX_ASSERT( chNo, SND_CHANNEL_MIN, SND_CHANNEL_MAX );
    
    if ( ! NNS_SndLockChannel( (u32)( 1 << chNo ) ) ) return NNS_SND_WAVEOUT_INVALID_HANDLE;
    
    waveout = & sWaveOut[ chNo ];
    waveout->chNo = chNo;
    waveout->activeFlag = FALSE;
    
    return waveout;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutFreeChannel

  Description:  Frees channel that was allocated to play a waveform.

  Arguments:    handle - Waveform play handle

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutFreeChannel( NNSSndWaveOutHandle handle )
{
    NNS_NULL_ASSERT( handle );
    
    NNS_SndUnlockChannel( (u32)( 1 << handle->chNo ) );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutStart

  Description:  Starts waveform play.

  Arguments:    handle - Waveform play handle
                format   - Waveform data format
                dataaddr - Waveform data beginning address
                loopFlag - Loop flag
                loopStartSample - Loop start sample position
                samples         - Number of samples
                sampleRate - sampling rate
                volume - Volume
                speed      - Play speed
                pan        - Pan

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndWaveOutStart(
    NNSSndWaveOutHandle handle,
    NNSSndWaveFormat format,
    const void* dataaddr,
    BOOL loopFlag,
    int loopStartSample,
    int samples,
    int sampleRate,
    int volume, 
    int speed,
    int pan
)
{
    int loopStart;
    int loopLen;
    u64 timer;
    
    NNS_NULL_ASSERT( handle );
    NNS_ALIGN4_ASSERT( dataaddr );
    NNS_MINMAX_ASSERT( volume, NNS_SND_WAVEOUT_VOLUME_MIN, NNS_SND_WAVEOUT_VOLUME_MAX );
    NNS_MINMAX_ASSERT( pan, NNS_SND_WAVEOUT_PAN_MIN, NNS_SND_WAVEOUT_PAN_MAX );    
    
    switch( format )
    {
    case NNS_SND_WAVE_FORMAT_PCM8:
        NNS_ASSERTMSG( ( loopStartSample & 0x03 ) == 0 , "loopStartSample must be a multiple of 4" );
        NNS_ASSERTMSG( ( samples & 0x03 ) == 0 , "samples must be a multiple of 4" );
        loopStart = ( loopStartSample >> 2 );
        loopLen = ( samples >> 2 ) - loopStart;
        break;
    case NNS_SND_WAVE_FORMAT_PCM16:
        NNS_ASSERTMSG( ( loopStartSample & 0x01 ) == 0 , "loopStartSample must be a multiple of 2" );
        NNS_ASSERTMSG( ( samples & 0x01 ) == 0 , "samples must be a multiple of 2" );
        loopStart = ( loopStartSample >> 1 );
        loopLen = ( samples >> 1 ) - loopStart;
        break;
    }
    
    timer = (u64)SND_TIMER_CLOCK * 0x8000 / sampleRate / speed;
    if ( timer < SND_CHANNEL_TIMER_MIN ) timer = SND_CHANNEL_TIMER_MIN;
    else if ( timer > SND_CHANNEL_TIMER_MAX ) timer = SND_CHANNEL_TIMER_MAX;
    
    SND_SetupChannelPcm(
        handle->chNo,
        (SNDWaveFormat)format,
        dataaddr,
        loopFlag ? SND_CHANNEL_LOOP_REPEAT : SND_CHANNEL_LOOP_1SHOT,
        loopStart,
        loopLen,
        volume,
        SND_CHANNEL_DATASHIFT_NONE,
        (int)timer,
        pan
    );
    SND_StartTimer(
        (u32)( 1 << handle->chNo ),
        0, 0, 0
    );
    
    handle->activeFlag = TRUE;
    handle->startFlag = FALSE;
    handle->startCommandTag = SND_GetCurrentCommandTag();
    handle->sampleRate = sampleRate;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutStop

  Description:  Stops waveform play.

  Arguments:    handle - Waveform play handle

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutStop( NNSSndWaveOutHandle handle )
{
    NNS_NULL_ASSERT( handle );

    if ( ! handle->activeFlag ) return;
    
    SND_StopTimer(
        (u32)( 1 << handle->chNo ),
        0, 0, 0
    );
    
    handle->activeFlag = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutSetVolume

  Description:  Changes waveform play volume.

  Arguments:    handle - Waveform play handle
                volume - Volume

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutSetVolume( NNSSndWaveOutHandle handle, int volume )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) return;

    SND_SetChannelVolume(
        (u32)( 1 << handle->chNo ),
        volume,
        SND_CHANNEL_DATASHIFT_NONE
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutSetSpeed

  Description:  Changes waveform play, play speed.

  Arguments:    handle - Waveform play handle
                speed      - Play speed

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutSetSpeed( NNSSndWaveOutHandle handle, int speed )
{
    u64 timer;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) return;

    timer = (u64)SND_TIMER_CLOCK * 0x8000 / handle->sampleRate / speed;
    if ( timer < SND_CHANNEL_TIMER_MIN ) timer = SND_CHANNEL_TIMER_MIN;
    else if ( timer > SND_CHANNEL_TIMER_MAX ) timer = SND_CHANNEL_TIMER_MAX;
    
    SND_SetChannelTimer(
        (u32)( 1 << handle->chNo ),
        (int)timer
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutSetPan

  Description:  Changes waveform play pan.

  Arguments:    handle - Waveform play handle
                pan        - Pan

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutSetPan( NNSSndWaveOutHandle handle, int pan )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) return;

    SND_SetChannelPan(
        (u32)( 1 << handle->chNo ),
        pan
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutIsPlaying

  Description:  Checks whether waveform play is in progress.

  Arguments:    handle - Waveform play handle

  Returns:      If playing, TRUE. If stopped FALSE
 *---------------------------------------------------------------------------*/
BOOL NNS_SndWaveOutIsPlaying( NNSSndWaveOutHandle handle )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) return FALSE;
    
    if ( ! handle->startFlag ) {
        if ( ! SND_IsFinishedCommandTag( handle->startCommandTag ) ) return TRUE;
        
        handle->startFlag = TRUE;
    }
    
    if ( SND_GetChannelStatus() & ( 1 << handle->chNo ) ) {
        return TRUE;
    }
    
    handle->activeFlag = FALSE;
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndWaveOutWaitForChannelStop

  Description:  Waits until waveform play is completely stopped.

  Arguments:    handle - Waveform play handle

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndWaveOutWaitForChannelStop( NNSSndWaveOutHandle handle )
{
    u32 commandTag;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! handle->activeFlag ) {
        if ( ( SND_GetChannelStatus() & ( 1 << handle->chNo ) ) == 0 ) return;
        
        commandTag = SND_GetCurrentCommandTag();
        (void)SND_FlushCommand( SND_COMMAND_BLOCK );
        SND_WaitForCommandProc( commandTag );
        
        return;
    }
    else {
        if ( ! handle->startFlag ) {
            SND_WaitForCommandProc( handle->startCommandTag );
            handle->startFlag = TRUE;
        }
        while( 1 ) {
            if ( ( SND_GetChannelStatus() & ( 1 << handle->chNo ) ) == 0 ) return;        
        }
    }
}

/*====== End of waveout.c ======*/


