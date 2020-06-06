/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     main.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $
  Revision 1.39  08/08/2005 04:50:02  ida
  Added code to handle times when the NNS_SndUpdateDriverInfo function fails

  Revision 1.38  02/17/2005 07:50:51  ida
  Changed function names from Get to Read

  Revision 1.37  02/16/2005 07:45:52  ida
  Added driver information functions

  Revision 1.36  10/21/2004 02:12:10  ida
  for NNS_SndInitResourceMgr become private

  Revision 1.35  10/07/2004 08:44:41  ida
  auto sleep process

  Revision 1.34  10/07/2004 05:57:40  ida
  add NNS_SndSetMonoFlag

  Revision 1.33  10/04/2004 08:52:16  ida
  stop surround in NNS_SndStopSoundAll()

  Revision 1.32  09/15/2004 07:29:39  ida
  for SDK_SMALL_BUILD

  Revision 1.31  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.30  09/06/2004 07:23:39  ida
  for SDK_SMALL_BUILD

  Revision 1.29  08/19/2004 06:41:17  ida
  for sleep mode

  Revision 1.28  08/10/2004 02:28:30  ida
  add NNS_SndStopSoundAll

  Revision 1.27  08/02/2004 05:28:12  ida
  add stream library

  Revision 1.26  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.25  07/05/2004 06:26:47  ida
  unuse SND_StopChannelAll

  Revision 1.24  06/28/2004 07:11:57  ida
  lock -> resource_mgr

  Revision 1.23  06/23/2004 06:19:38  ida
  workaround shared work

  Revision 1.22  06/21/2004 01:09:42  ida
  for new snd_drv interface

  Revision 1.21  06/11/2004 01:51:37  ida
  for new snd_drv interface

  Revision 1.20  06/09/2004 08:45:58  ida
  add NNS_SndStopChannelAll

  Revision 1.19  06/08/2004 00:49:44  ida
  remove send error check

  Revision 1.18  05/24/2004 05:40:40  ida
  update master volume instantly

  Revision 1.17  05/06/2004 04:47:15  ida
  workaround SNDSharedWork

  Revision 1.16  04/28/2004 07:47:16  ida
  fixed comments

  Revision 1.15  04/05/2004 02:33:44  ida
  add comments

  Revision 1.14  04/02/2004 06:49:15  ida
  hide SetOutputMode

  Revision 1.13  03/26/2004 04:21:54  ida
  add NNS_SndCaptureMain

  Revision 1.12  03/25/2004 10:04:39  ida
  add SetOutputMode

  Revision 1.11  03/11/2004 02:33:27  ida
  add SND_RecvPort

  Revision 1.10  03/08/2004 06:58:36  ida
  using port tag system

  Revision 1.9  03/05/2004 08:26:27  ida
  SND_PortXXX -> SND_XXX

  Revision 1.8  02/25/2004 05:38:40  ida
  NNS_ -> NNSi_

  Revision 1.7  02/23/2004 03:23:38  ida
  change function prefix

  Revision 1.6  02/20/2004 05:02:59  ida
  change directory

  Revision 1.2  02/19/2004 01:47:04  ida
  add comments

  Revision 1.1  02/13/2004 01:42:05  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/main.h>

#include <nitro/spi.h>
#include <nnsys/misc.h>
#include <nnsys/snd/capture.h>
#include <nnsys/snd/player.h>
#include <nnsys/snd/sndarc_stream.h>
#include <nnsys/snd/resource_mgr.h>

/******************************************************************************
	static variables
 ******************************************************************************/

static PMSleepCallbackInfo sPreSleepCallback;
static PMSleepCallbackInfo sPostSleepCallback;

static SNDDriverInfo sDriverInfo[2] ATTRIBUTE_ALIGN( 32 );
static u32 sDriverInfoCommandTag;
static s8 sCurDriverInfo;
static BOOL sDriverInfoFirstFlag;

/******************************************************************************
	static function declaration
 ******************************************************************************/
static void BeginSleep( void* );
static void EndSleep( void* );

static const SNDDriverInfo* GetCurDriverInfo( void );

/******************************************************************************
	inline functions
 ******************************************************************************/
static inline const SNDDriverInfo* GetCurDriverInfo( void )
{
    if ( sCurDriverInfo < 0 ) return NULL;
    return &sDriverInfo[ sCurDriverInfo ];
}

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndInit

  Description:  initialize sound library

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndInit( void )
{
    {
        // Double initialization check
        static BOOL initialized = FALSE;
        if ( initialized ) return;
        initialized = TRUE;
    }
    
    // Initialize sound driver
    SND_Init();
    
    // Register sleep callback
    PM_SetSleepCallbackInfo( &sPreSleepCallback, BeginSleep, NULL );
    PM_SetSleepCallbackInfo( &sPostSleepCallback, EndSleep, NULL );
    
    PM_PrependPreSleepCallback( &sPreSleepCallback );
    PM_AppendPostSleepCallback( &sPostSleepCallback );
    
    // Initialize each library
    NNSi_SndInitResourceMgr();
    NNSi_SndCaptureInit();
    NNSi_SndPlayerInit();
    
    // Initialize driver information
    sCurDriverInfo = -1;
    sDriverInfoFirstFlag = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndMain

  Description:  sound library framework

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndMain( void )
{
    // Receive ARM7 response
    while ( SND_RecvCommandReply( SND_COMMAND_NOBLOCK ) != NULL ) {}
    
    // Framework for each library
    NNSi_SndPlayerMain();
    NNSi_SndCaptureMain();
#ifndef SDK_SMALL_BUILD    
    NNSi_SndArcStrmMain();
#endif /* SDK_SMALL_BUILD*/
    
    // Issue ARM7 command
    (void)SND_FlushCommand( SND_COMMAND_NOBLOCK );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndSetMasterVolume

  Description:  set master volume

  Arguments:    volume - master volume

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndSetMasterVolume( int volume )
{
    NNS_MINMAX_ASSERT( volume, 0, SND_MASTER_VOLUME_MAX );
    
    SND_SetMasterVolume( volume );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndSetMonoFlag

  Description:  Changes the mono flag

  Arguments:    flag - whether to enable mono

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndSetMonoFlag( BOOL flag )
{
    if ( flag ) SND_SetMasterPan( SND_CHANNEL_PAN_CENTER );
    else SND_ResetMasterPan();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStopSoundAll

  Description:  Immediately stops all sounds

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStopSoundAll( void )
{
    u32 commandTag;
    
    NNS_SndPlayerStopSeqAll( 0 );
#ifndef SDK_SMALL_BUILD
    NNS_SndArcStrmStopAll( 0 );
#endif /* SDK_SMALL_BUILD*/    
    NNSi_SndCaptureStop();
    
    SNDi_SetSurroundDecay( 0 );    
    SND_StopTimer( 0xffff, 0xffff, 0xffff, 0 );
    
    // Wait for process done with ARM7
    commandTag = SND_GetCurrentCommandTag();
    (void)SND_FlushCommand( SND_COMMAND_BLOCK );
    SND_WaitForCommandProc( commandTag );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndStopChannelAll

  Description:  Stops all channels

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndStopChannelAll( void )
{
    SND_StopUnlockedChannel( 0xffff, 0 );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndUpdateDriverInfo

  Description:  Updates driver information

  Arguments:    None.

  Returns:      Whether update was successful or not
 *---------------------------------------------------------------------------*/
BOOL NNS_SndUpdateDriverInfo( void )
{
    if ( ! sDriverInfoFirstFlag )
    {
        // Receive ARM7 response
        while ( SND_RecvCommandReply( SND_COMMAND_NOBLOCK ) != NULL ) {}
        
        if ( ! SND_IsFinishedCommandTag( sDriverInfoCommandTag ) ) {
            // Update incomplete
            return FALSE;
        }
        
        // Update completed
        if ( sCurDriverInfo < 0 ) sCurDriverInfo = 1;
        
        SND_ReadDriverInfo( &sDriverInfo[ sCurDriverInfo ] );
        sDriverInfoCommandTag = SND_GetCurrentCommandTag();
        
        if ( sCurDriverInfo == 0 ) sCurDriverInfo = 1;
        else sCurDriverInfo = 0;
        
        DC_InvalidateRange( &sDriverInfo[ sCurDriverInfo ], sizeof( SNDDriverInfo ) );
        
        // Issue ARM7 command
        (void)SND_FlushCommand( SND_COMMAND_NOBLOCK );
        
        return TRUE;
    }
    else
    {
        // First update
        SND_ReadDriverInfo( &sDriverInfo[0] );
        sDriverInfoCommandTag = SND_GetCurrentCommandTag();
        sDriverInfoFirstFlag = FALSE;
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndReadDriverChannelInfo

  Description:  Gets channel information

  Arguments:    chNo - Channel number
                info - Channel information structure that holds the obtained information.

  Returns:      Whether information was obtained or not
 *---------------------------------------------------------------------------*/
BOOL NNS_SndReadDriverChannelInfo( int chNo, SNDChannelInfo* info )
{
    const SNDDriverInfo* driverInfo;
    
    NNS_NULL_ASSERT( info );
    
    driverInfo = GetCurDriverInfo();
    if ( driverInfo == NULL ) return FALSE;
    
    return SND_ReadChannelInfo( driverInfo, chNo, info );
}

/******************************************************************************
	private functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndReadDriverPlayerInfo

  Description:  Gets player information

  Arguments:    playerNo - Player number (physical number)
                info - Player information structure that holds the obtained information.

  Returns:      Whether information was obtained or not
 *---------------------------------------------------------------------------*/
BOOL NNSi_SndReadDriverPlayerInfo( int playerNo, SNDPlayerInfo* info )
{
    const SNDDriverInfo* driverInfo;
    
    NNS_NULL_ASSERT( info );
    
    driverInfo = GetCurDriverInfo();
    if ( driverInfo == NULL ) return FALSE;

    return SND_ReadPlayerInfo( driverInfo, playerNo, info );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndReadDriverTrackInfo

  Description:  Obtains track information

  Arguments:    playerNo - Player number (physical number)
                trackNo - Track number
                info - Track information structure that holds the obtained information

  Returns:      Whether information was obtained or not
 *---------------------------------------------------------------------------*/
BOOL NNSi_SndReadDriverTrackInfo( int playerNo, int trackNo, SNDTrackInfo* info )
{
    const SNDDriverInfo* driverInfo;
    
    NNS_NULL_ASSERT( info );
    
    driverInfo = GetCurDriverInfo();
    if ( driverInfo == NULL ) return FALSE;
    
    return SND_ReadTrackInfo( driverInfo, playerNo, trackNo, info );
}

/******************************************************************************
	static functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         BeginSleep

  Description:  Prepares for sleep mode

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void BeginSleep( void* )
{
    u32 commandTag;
    
    NNSi_SndCaptureBeginSleep();
    
    // Sends dummy commands to empty command buffer
    SND_StopTimer( 0, 0, 0, 0 );
    
    // Wait for process done with ARM7
    commandTag = SND_GetCurrentCommandTag();
    (void)SND_FlushCommand( SND_COMMAND_BLOCK );
    SND_WaitForCommandProc( commandTag );
}

/*---------------------------------------------------------------------------*
  Name:         EndSleep

  Description:  Processes after sleep mode

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void EndSleep( void* )
{
    NNSi_SndCaptureEndSleep();
}

/*====== End of main.c ======*/


