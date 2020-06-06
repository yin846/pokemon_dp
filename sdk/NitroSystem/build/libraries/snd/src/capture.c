/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     capture.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: capture.c,v $
  Revision 1.37  08/18/2005 07:03:38 AM  ida
  Added NNS_SndCaptureIsActive function
  Added NNS_SndCaptureIsActive function

  Revision 1.36  05/06/2005 02:52:31 AM  ida
  Revised output setting for effects

  Revision 1.35  04/26/2005 08:34:56 AM  ida
  Optimized AlarmCallback

  Revision 1.34  02/21/2005 02:37:17 AM  ida
  Warning measures

  Revision 1.33  10/21/2004 02:09:49 AM  ida
  NNS_SndCaptureInit and NNS_SndCaptureMain become private

  Revision 1.32  10/19/2004 01:52:04 AM  ida
  add comments

  Revision 1.31  10/07/2004 04:27:26 AM  ida
  fix typo SND_CHANNEL_LOOP_REPEAT

  Revision 1.30  10/04/2004 05:14:01 AM  ida
  optimize AlarmCallback

  Revision 1.29  09/13/2004 06:38:34 AM  ida
  add NNS_SndCaptureStartSampling

  Revision 1.28  09/07/2004 01:16:56 AM  ida
  use nnsys/misc.h

  Revision 1.27  09/06/2004 07:22:43 AM  ida
  adjust interface

  Revision 1.26  08/19/2004 06:41:06 AM  ida
  for sleep mode

  Revision 1.25  08/02/2004 05:27:49 AM  ida
  use NNSSndFader

  Revision 1.24  07/16/2004 06:42:49 AM  ida
  fix Project name

  Revision 1.23  07/15/2004 03:24:37 AM  ida
  add assertion

  Revision 1.22  07/13/2004 05:42:12 AM  ida
  SND_CAPTURE_0_CHANNEL -> SND_CAPTURE_0_OUT_CHANNEL

  Revision 1.21  07/09/2004 06:53:18 AM  ida
  workaround single capture

  Revision 1.20  07/05/2004 06:26:23 AM  ida
  change symbol name

  Revision 1.19  06/29/2004 06:44:18 AM  ida
  add interval argument

  Revision 1.18  06/28/2004 07:11:13 AM  ida
  add effect library

  Revision 1.17  06/23/2004 06:18:48 AM  ida
  for new snd_drv interface

  Revision 1.16  06/21/2004 01:08:39 AM  ida
  use capture lock

  Revision 1.15  06/11/2004 01:50:57 AM  ida
  for new snd_drv interface

  Revision 1.14  06/09/2004 08:45:34 AM  ida
  fixed comments

  Revision 1.13  04/05/2004 02:33:44 AM  ida
  add comments

  Revision 1.12  03/30/2004 08:26:23 AM  ida
  fix move volume bug

  Revision 1.11  03/30/2004 12:49:25 AM  ida
  assert bufSize

  Revision 1.10  03/26/2004 11:05:14 AM  ida
  brush up

  Revision 1.9  03/26/2004 04:26:40 AM  ida
  FORMAT_8BIT -> FORMAT_PCM8

  Revision 1.8  03/26/2004 04:21:38 AM  ida
  workaround reverb

  Revision 1.7  03/25/2004 10:03:42 AM  ida
  use LockChannels

  Revision 1.6  03/22/2004 02:19:02 AM  ida
  add effect functions

  Revision 1.5  02/23/2004 03:23:34 AM  ida
  change function prefix

  Revision 1.4  02/20/2004 05:02:59 AM  ida
  change directory

  Revision 1.2  02/19/2004 08:57:10 AM  ida
  ARM7 -> SDK

  Revision 1.1  02/13/2004 01:42:05 AM  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/capture.h>

#include <nitro/mi.h>
#include <nitro/snd.h>
#include <nnsys/misc.h>
#include <nnsys/snd/resource_mgr.h>
#include <nnsys/snd/fader.h>

/******************************************************************************
	macro definitions
 ******************************************************************************/

#define CAPTURE_0 ( 1 << 0 )
#define CAPTURE_1 ( 1 << 1 )

#define THREAD_STACK_SIZE      1024
#define THREAD_MESSAGE_BUF_SIZE   8

/******************************************************************************
	structure definitions
 ******************************************************************************/

typedef struct CaptureParam
{
    BOOL activeFlag;
    NNSSndCaptureType type;
    NNSSndCaptureFormat format;
    void* bufferL;
    void* bufferR;
    u32 bufLen;
    u32 blockSize;  // Size of 1 block
    int curBuffer;
    
    u32 chBitMask;
    u32 playChBitMask;
    u32 capBitMask;
    int alarmNo;
    
    int interval;
    NNSSndCaptureCallback callback;
    void* callbackArg;
    
    NNSSndFader fader;
    BOOL fadeOutFlag;
    int volume;
} CaptureParam;

typedef struct EffectInfo
{
    CaptureParam* cap;
    u32 blockSize;
    u32 offset;
    void* bufL_p;
    void* bufR_p;
} EffectInfo;

/******************************************************************************
	static variables
 ******************************************************************************/

static CaptureParam sCapParam;

static OSThread sThread;
static u64 sThreadStack[ THREAD_STACK_SIZE / sizeof(u64) ];
static OSMessageQueue sMesgQ;
static OSMessage sMesgBuf[ THREAD_MESSAGE_BUF_SIZE ];

static BOOL sIsThreadCreated = FALSE;
static EffectInfo sEffectInfoBuffer[ THREAD_MESSAGE_BUF_SIZE ];
static int sCurEffectInfo;

/******************************************************************************
	static function declarations
 ******************************************************************************/
static void AlarmCallback( void* arg );
static void CaptureThread( void* arg );

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureStartReverb

  Description:  start reverb

  Arguments:    buffer_p   - reverb buffer top address
                bufSize    - reverb buffer size
                format     - capture format
                sampleRate - sampling rate
                volume     - reverb component volume

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndCaptureStartReverb(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int volume
)
{
    void* bufL_p;
    void* bufR_p;
    
    NNS_MINMAX_ASSERT( volume, 0, 63 );
    
	NNS_SndCaptureStopReverb( 0 );
    
    if ( sCapParam.activeFlag ) return FALSE;
    
    bufL_p = buffer_p;
    bufR_p = (u8*)buffer_p + bufSize / 2;
    
    MI_CpuClear32( buffer_p, bufSize );
    DC_FlushRange( buffer_p, bufSize );   
    
    return NNSi_SndCaptureStart(
        NNS_SND_CAPTURE_TYPE_REVERB,
        bufL_p,
        bufR_p,
        bufSize / 2,
        format,
        SND_CAPTURE_IN_MIXER,
        SND_CAPTURE_OUT_NORMAL,
        TRUE,  // loop
        sampleRate,
        volume,
        0,     // pan-L
        127,   // pan-R
        0,     // interval
        NULL,  // callback
        0      // arg
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureSetReverbVolume

  Description:  volume setting of reverb component

  Arguments:    volume - target volume
                frames - number of frames to change

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureSetReverbVolume( int volume, int frames )
{
    CaptureParam* cap = & sCapParam;
    
    NNS_MINMAX_ASSERT( volume, 0, 63 );
    
    if ( ! cap->activeFlag ) return;
    if ( cap->type != NNS_SND_CAPTURE_TYPE_REVERB ) return;
    
    NNSi_SndFaderSet( & cap->fader, volume << 8, frames );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureStopReverb

  Description:  stops reverb

  Arguments:    frames - fade out frame

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureStopReverb( int frames )
{
    CaptureParam* cap = & sCapParam;
    
    if ( ! cap->activeFlag ) return;
    if ( cap->type != NNS_SND_CAPTURE_TYPE_REVERB ) return;    
    
    if ( frames == 0 ) {
        NNSi_SndCaptureStop();
        return;
    }
    
    NNSi_SndFaderSet( & cap->fader, 0, frames );
    
    cap->fadeOutFlag = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureStartEffect

  Description:  starts effect

  Arguments:    buffer_p   - Effect buffer start address
                bufSize  - Effect buffer size
                format     - capture format
                sampleRate - sampling rate
                interval - Callback interval
                callback - Callback function
                arg  - Callback argument

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndCaptureStartEffect(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int interval,
    NNSSndCaptureEffectCallback callback,
    void* arg
)
{
    void* bufL_p;
    void* bufR_p;
    
    NNS_SndCaptureStopEffect();
    
    if ( sCapParam.activeFlag ) return FALSE;
    
    bufL_p = buffer_p;
    bufR_p = (u8*)buffer_p + bufSize / 2;
    
    MI_CpuClear32( buffer_p, bufSize );
    DC_FlushRange( buffer_p, bufSize );
    
    return NNSi_SndCaptureStart(
        NNS_SND_CAPTURE_TYPE_EFFECT,
        bufL_p,
        bufR_p,
        bufSize / 2,
        format,
        SND_CAPTURE_IN_MIXER,
        SND_CAPTURE_OUT_NORMAL,
        TRUE,  // loop
        sampleRate,
        127,
        0,     // pan-L
        127,   // pan-R
        interval,
        callback,
        arg
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureStopEffect

  Description:  Stops effect.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureStopEffect( void )
{
    if ( ! sCapParam.activeFlag ) return;
    if ( sCapParam.type != NNS_SND_CAPTURE_TYPE_EFFECT ) return;
    
    NNSi_SndCaptureStop();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureStartSampling

  Description:  Starts sampling.

  Arguments:    buffer_p   - Sampling buffer start address
                bufSize  - Sampling buffer size
                format     - capture format
                sampleRate - sampling rate
                interval - Callback interval
                callback - Callback function
                arg  - Callback argument

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndCaptureStartSampling(    
    void* buffer_p,
    u32 bufSize,
    NNSSndCaptureFormat format,
    int sampleRate,
    int interval,
    NNSSndCaptureCallback callback,
    void* arg
)
{
    void* bufL_p;
    void* bufR_p;
    
    NNS_SndCaptureStopSampling();
    
    if ( sCapParam.activeFlag ) return FALSE;
    
    bufL_p = buffer_p;
    bufR_p = (u8*)buffer_p + bufSize / 2;
    
    MI_CpuClear32( buffer_p, bufSize );
    DC_FlushRange( buffer_p, bufSize );
    
    return NNSi_SndCaptureStart(
        NNS_SND_CAPTURE_TYPE_SAMPLING,
        bufL_p,
        bufR_p,
        bufSize / 2,
        format,
        SND_CAPTURE_IN_MIXER,
        SND_CAPTURE_OUT_NORMAL,
        TRUE,  // loop
        sampleRate,
        127,
        0,     // pan-L
        127,   // pan-R
        interval,
        callback,
        arg
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureStopSampling

  Description:  Stops sampling.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureStopSampling( void )
{
    if ( ! sCapParam.activeFlag ) return;
    if ( sCapParam.type != NNS_SND_CAPTURE_TYPE_SAMPLING ) return;
    
    NNSi_SndCaptureStop();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureCreateThread

  Description:  Creates capture thread.

  Arguments:    threadPrio - thread priority

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndCaptureCreateThread( u32 threadPrio )
{
    NNS_ASSERT( OS_IsThreadAvailable() );
    
    if ( sIsThreadCreated ) return;
    
    sCurEffectInfo = 0;
    
    OS_InitMessageQueue( & sMesgQ, sMesgBuf, THREAD_MESSAGE_BUF_SIZE );
    
    OS_CreateThread(
        &sThread,
        CaptureThread,
        NULL,
        sThreadStack + THREAD_STACK_SIZE / sizeof(u64),
        THREAD_STACK_SIZE,
        threadPrio
    );
    
    sIsThreadCreated = TRUE;
    
    OS_WakeupThreadDirect( &sThread );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureIsActive

  Description:  Retrieves whether capture is being used or not. 

  Arguments:    None.

  Returns:      Being used or not.
 *---------------------------------------------------------------------------*/
BOOL NNS_SndCaptureIsActive( void )
{
    return sCapParam.activeFlag;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndCaptureGetCaptureType

  Description:  Retrieves type of capture being used currently. 

  Arguments:    None.

  Returns:      The type of capture being used currently. 
 *---------------------------------------------------------------------------*/
NNSSndCaptureType NNS_SndCaptureGetCaptureType( void )
{
    NNS_ASSERT( NNS_SndCaptureIsActive() );
    
    return sCapParam.type;
}

/******************************************************************************
	private functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndCaptureInit

  Description:  Initializes the capture library

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndCaptureInit( void )
{
    sIsThreadCreated = FALSE;
    
    sCapParam.activeFlag = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndCaptureMain

  Description:  Capture library framework

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndCaptureMain( void )
{
    CaptureParam* cap;
    NNSSndFader* fader;
    int volume;
    
    cap = & sCapParam;
    
    if ( cap->activeFlag && cap->type == NNS_SND_CAPTURE_TYPE_REVERB )
    {
        fader = & cap->fader;
        
        // Fader process
        NNSi_SndFaderUpdate( fader );
        
        // Fadeout completion check
        if ( cap->fadeOutFlag )
        {
            if ( NNSi_SndFaderIsFinished( fader ) )
            {
                NNSi_SndCaptureStop();
                return;
            }
        }
        
        volume = ( NNSi_SndFaderGet( fader ) >> 8 );
        
        if ( volume != cap->volume )
        {
            SND_SetChannelVolume(
                cap->playChBitMask,
                volume,
                SND_CHANNEL_DATASHIFT_NONE
            );
            
            cap->volume = volume;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndCaptureStart

  Description:  starts capture

  Arguments:    type - Capture type
                buffer0 - Capture buffer 0 start address
                buffer1 - Capture buffer 1 start address
                bufLen  - Capture buffer size
                format     - Capture format
                input   - Capture input settings
                output  - Capture output settings
                loopFlag - Loop flag
                sampleRate - sampling rate
                volume  - Output volume
                pan0     - Capture 0 output pan
                pan1     - Capture 1 output pan
                interval - Callback interval
                callback - Callback function
                arg  - Callback argument

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
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
)
{
    SNDWaveFormat wave_format;
    SNDCaptureFormat capture_format;
    u32 chBitMask = 0;
    u32 playChBitMask = 0;
    u32 capBitMask = 0;
    int alarmNo = -1;
    unsigned int samples;
    int timer;
    u32 alarmTimer;
    u32 alarmOffset;
    BOOL is8bit;
    CaptureParam* cap = & sCapParam;
    
    NNS_ASSERTMSG( ( (u32)buffer0 & 0x1f ) == 0, "buffer address must be aligned to 32 bytes boundary." );
    NNS_ASSERTMSG( ( (u32)buffer1 & 0x1f ) == 0, "buffer address must be aligned to 32 bytes boundary." );
    NNS_ASSERTMSG( ( bufLen & 0x1f ) == 0, "buffer size must be aligned to 32 bytes boundary." );
    NNS_MINMAX_ASSERT( pan0, SND_CHANNEL_PAN_MIN, SND_CHANNEL_PAN_MAX );
    NNS_MINMAX_ASSERT( pan1, SND_CHANNEL_PAN_MIN, SND_CHANNEL_PAN_MAX );
    NNS_MAX_ASSERT( bufLen >> 2, SND_CAPTURE_LEN_MAX );
    
    DC_FlushRange( buffer0, bufLen );
    DC_FlushRange( buffer1, bufLen );
    
    /* Parameter calculations*/
    is8bit = format == NNS_SND_CAPTURE_FORMAT_PCM8 ? TRUE: FALSE;
    
    timer = SND_TIMER_CLOCK / sampleRate;
    NNS_MINMAX_ASSERT( timer, SND_CHANNEL_TIMER_MIN, SND_CHANNEL_TIMER_MAX );
    
    if ( callback != NULL )
    {        
        NNS_ASSERTMSG( ( ( bufLen / interval ) & 0x1f ) == 0, "buffer size divided by inverval must be aligned to 32 bytes boundary." );
        
        samples = bufLen;
        if ( ! is8bit ) samples >>= 1;
        NNS_ASSERTMSG( ( samples % interval ) == 0, "interval must be a multiple of sample count." );
        
        timer = ( ( timer + 16 ) & ~0x1f );
        alarmTimer = ( timer >> 5 ) * ( samples / interval );
        
        alarmOffset = 32; // FIFO size
        if ( ! is8bit ) alarmOffset >>= 1; // byte to sample
        alarmOffset *= ( timer >> 5 );
    }
    
    wave_format    = is8bit ? SND_WAVE_FORMAT_PCM8    : SND_WAVE_FORMAT_PCM16;
    capture_format = is8bit ? SND_CAPTURE_FORMAT_PCM8 : SND_CAPTURE_FORMAT_PCM16;
    
    chBitMask  |= ( 1 << SND_CAPTURE_0_OUT_CHANNEL ) | ( 1 << SND_CAPTURE_1_OUT_CHANNEL );
    capBitMask |= ( 1 << SND_CAPTURE_0 ) | ( 1 << SND_CAPTURE_1 );

    if ( type != NNS_SND_CAPTURE_TYPE_SAMPLING ) {
        playChBitMask = chBitMask;
    }
    
    /* Resource lock*/    
    if ( callback != NULL )
    {
        alarmNo = NNS_SndAllocAlarm();
        if ( alarmNo < 0 ) return FALSE;
    }
    if ( ! NNS_SndLockCapture( capBitMask ) ) {
        if ( alarmNo >= 0 ) NNS_SndFreeAlarm( alarmNo );
        return FALSE;
    }
    if ( ! NNS_SndLockChannel( chBitMask ) ) {
        if ( alarmNo >= 0 ) NNS_SndFreeAlarm( alarmNo );
        NNS_SndUnlockCapture( capBitMask );
        return FALSE;
    }
    
    /* Channel and capture settings*/
    SND_SetupChannelPcm(
        SND_CAPTURE_0_OUT_CHANNEL,
        wave_format,
        buffer0,
        loopFlag ? SND_CHANNEL_LOOP_REPEAT : SND_CHANNEL_LOOP_1SHOT,
        0,
        (int)( bufLen >> 2 ),
        volume,
        SND_CHANNEL_DATASHIFT_NONE,
        timer,
        pan0
    );
    SND_SetupCapture(
        SND_CAPTURE_0,
        capture_format,
        buffer0,
        bufLen >> 2,
        loopFlag,
        input,
        output
    );
    SND_SetupChannelPcm(
        SND_CAPTURE_1_OUT_CHANNEL,
        wave_format,
        buffer1,
        loopFlag ? SND_CHANNEL_LOOP_REPEAT : SND_CHANNEL_LOOP_1SHOT,
        0,
        (int)( bufLen >> 2 ),
        volume,
        SND_CHANNEL_DATASHIFT_NONE,
        timer,
        pan1
    );
    SND_SetupCapture(
        SND_CAPTURE_1,
        capture_format,
        buffer1,
        bufLen >> 2,
        loopFlag,
        input,
        output
    );
    
    if ( alarmNo >= 0 )
    {
        SND_SetupAlarm(
            alarmNo,
            alarmTimer + alarmOffset,
            alarmTimer,
            AlarmCallback,
            cap
        );
    }
    
    /* Output selector settings*/
    if ( type == NNS_SND_CAPTURE_TYPE_EFFECT )
    {
        SND_SetOutputSelector(
            SND_OUTPUT_CHANNEL1,
            SND_OUTPUT_CHANNEL3,
            SND_CHANNEL_OUT_BYPASS,
            SND_CHANNEL_OUT_BYPASS
        );
    }
    
    /* Start timer*/
    SND_StartTimer(
        playChBitMask,
        capBitMask,
        alarmNo >= 0 ? (u32)( 1 << alarmNo ) : 0,
        0
    );
    
    /* Store parameters*/
    cap->activeFlag = TRUE;
    cap->type = type;
    
    cap->chBitMask = chBitMask;
    cap->playChBitMask = playChBitMask;
    cap->capBitMask = capBitMask;
    cap->alarmNo = alarmNo;
    
    cap->format = format;
    cap->bufferL = buffer0;
    cap->bufferR = buffer1;
    cap->bufLen = bufLen;
    cap->blockSize = bufLen / interval;
    
    cap->curBuffer = 0;
    
    cap->interval = interval;
    cap->callback = callback;
    cap->callbackArg = arg;
    
    cap->volume = volume;
    
    NNSi_SndFaderInit( & cap->fader );
    NNSi_SndFaderSet( & cap->fader, volume << 8, 1 );
    
    cap->fadeOutFlag = FALSE;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndCaptureStop

  Description:  stops capture

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndCaptureStop( void )
{
    CaptureParam* cap = & sCapParam;
    u32 commandTag;
    BOOL useAlarm;

    if ( ! cap->activeFlag ) return;
    
    useAlarm = cap->alarmNo >= 0 ? TRUE : FALSE;
    
    /* stop timer*/
    SND_StopTimer(
        cap->playChBitMask,
        cap->capBitMask,
        useAlarm ? (u32)( 1 << cap->alarmNo ) : 0,
        0
    );
    
    if ( useAlarm ) {
        // Waits for ARM7 processing when using an alarm so that a callback is not triggered.
        //  
        commandTag = SND_GetCurrentCommandTag();
        (void)SND_FlushCommand( SND_COMMAND_BLOCK );
        SND_WaitForCommandProc( commandTag );
        
        // Destroy accumulated messages.
        while( OS_ReceiveMessage( &sMesgQ, NULL, OS_MESSAGE_NOBLOCK ) ) {
        }
    }
    
    /* Release resource*/
    if ( cap->capBitMask ) NNS_SndUnlockCapture( cap->capBitMask );
    if ( cap->chBitMask ) NNS_SndUnlockChannel( cap->chBitMask );
    if ( useAlarm ) NNS_SndFreeAlarm( cap->alarmNo );
    
    /* output selector return*/
    if ( cap->type == NNS_SND_CAPTURE_TYPE_EFFECT )
    {
        SND_SetOutputSelector(
            SND_OUTPUT_MIXER,
            SND_OUTPUT_MIXER,
            SND_CHANNEL_OUT_MIXER,
            SND_CHANNEL_OUT_MIXER
        );
    }
    
    /* change status*/
    cap->activeFlag = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndCaptureBeginSleep

  Description:  Sleep mode preprocessing.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndCaptureBeginSleep( void )
{
    CaptureParam* cap;
    u32 commandTag;
    
    cap = & sCapParam;
    
    if ( ! cap->activeFlag ) return;
    
    SND_StopTimer(
        cap->playChBitMask,
        cap->capBitMask,
        cap->alarmNo >= 0 ? (u32)( 1 << cap->alarmNo ) : 0,
        0
    );
        
    commandTag = SND_GetCurrentCommandTag();
    (void)SND_FlushCommand( SND_COMMAND_BLOCK );
    SND_WaitForCommandProc( commandTag );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndCaptureEndSleep

  Description:  Sleep mode post-processing.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndCaptureEndSleep( void )
{
    CaptureParam* cap;
    
    cap = & sCapParam;
        
    if ( ! cap->activeFlag ) return;
    
    cap->curBuffer = 0;
    
    MI_CpuClear32( cap->bufferL, cap->bufLen );
    MI_CpuClear32( cap->bufferR, cap->bufLen );
    DC_FlushRange( cap->bufferL, cap->bufLen );
    DC_FlushRange( cap->bufferR, cap->bufLen );
    
    SND_StartTimer(
        cap->playChBitMask,
        cap->capBitMask,
        cap->alarmNo >= 0 ? (u32)( 1 << cap->alarmNo ) : 0,
        0
    );
}

/******************************************************************************
	static function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         AlarmCallback

  Description:  alarm callback function

  Arguments:    arg - callback argument  (capture parameter)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void AlarmCallback( void* arg )
{
    CaptureParam* cap = (CaptureParam*)arg;
    const u32 blockSize = cap->blockSize; // Size of 1 block
    const u32 offset = blockSize * cap->curBuffer;
    void* bufL_p = (u8*)cap->bufferL + offset;
    void* bufR_p = (u8*)cap->bufferR + offset;
    BOOL result;
    
    if ( sIsThreadCreated ) {
        EffectInfo* info = & sEffectInfoBuffer[ sCurEffectInfo ];
        
        info->cap = cap;
        info->blockSize = blockSize;
        info->offset = offset;
        info->bufL_p = bufL_p; 
        info->bufR_p = bufR_p;
        
        result = OS_SendMessage( & sMesgQ, (OSMessage)info, OS_MESSAGE_NOBLOCK );
        
        // Unless the capture thread is processed periodically,
        // the result becomes FALSE.
        NNS_WARNING( result, "Capture thread is too busy." );
        
        sCurEffectInfo++;
        if ( sCurEffectInfo >= THREAD_MESSAGE_BUF_SIZE ) sCurEffectInfo = 0;
    }
    else {
        // invalidate cache
        DC_InvalidateRange( bufL_p, blockSize );
        DC_InvalidateRange( bufR_p, blockSize );
        
        cap->callback(
            bufL_p,
            bufR_p,
            blockSize,
            cap->format,
            cap->callbackArg
        );
    }
    
    // switch buffer
    cap->curBuffer++;
    if ( cap->curBuffer >= cap->interval ) cap->curBuffer = 0;
}

/*---------------------------------------------------------------------------*
  Name:         CaptureThread

  Description:  Captures threads.

  Arguments:    arg - user data (unused)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CaptureThread( void* /*arg*/ )
{
    OSMessage msg;
    EffectInfo* info;
    
    while( 1 ) {
        (void)OS_ReceiveMessage( & sMesgQ, &msg, OS_MESSAGE_BLOCK );
        
        info = (EffectInfo*)msg;
        
        // invalidate cache
        DC_InvalidateRange( info->bufL_p, info->blockSize );
        DC_InvalidateRange( info->bufR_p, info->blockSize );
        
        info->cap->callback(
            info->bufL_p,
            info->bufR_p,
            info->blockSize,
            info->cap->format,
            info->cap->callbackArg
        );
    }
}

/*====== End of capture.c ======*/


