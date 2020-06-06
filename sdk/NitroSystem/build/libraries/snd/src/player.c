/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     player.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: player.c,v $
  Revision 1.76  08/08/2005 04:50:45  ida
  Added assert when handle is disconnected.

  Revision 1.75  04/27/2005 02:29:22  ida
  Replaced with implementation that use SND_PrepareSeq

  Revision 1.74  03/17/2005 07:31:43  ida
  Added NNS_SndPlayerSetPlayerVolume function

  Revision 1.73  03/01/2005 02:12:59  ida
  Added NNS_SndPlayerSetTrackPanRange function

  Revision 1.72  02/28/2005 04:19:46  ida
  Changed operation so that player heap is cleared during allocation

  Revision 1.71  02/17/2005 07:50:51  ida
  Changed function names from Get to Read

  Revision 1.70  02/16/2005 07:46:04  ida
  Added driver information functions

  Revision 1.69  01/06/2005 02:51:20  ida
  Updated copyright year notation

  Revision 1.68  01/05/2005 06:45:42  ida
  Added NNS_SndPlayerSetTrackAllocatableChannel function

  Revision 1.67  10/21/2004 02:10:14  ida
  NNS_SndPlayerInit and NNS_SndPlayerMain become private

  Revision 1.66  10/19/2004 01:52:58  ida
  fix unmatched variable name

  Revision 1.65  10/06/2004 02:23:54  ida
  set volume 0 when stop seq in fadeout

  Revision 1.64  10/04/2004 05:13:25  ida
  fix fadeout bug

  Revision 1.63  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.62  08/23/2004 01:39:00  ida
  workaround player heap dispose

  Revision 1.61  08/02/2004 05:28:23  ida
  use NNSSndFader

  Revision 1.60  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.59  07/15/2004 03:25:12  ida
  optimize NNS_SndPlayer*All functions

  Revision 1.58  07/05/2004 06:27:07  ida
  unuse SND_GetSharedWork

  Revision 1.57  06/23/2004 06:19:53  ida
  workaround shared work

  Revision 1.56  06/21/2004 01:10:08  ida
  for new snd_drv interface

  Revision 1.55  06/11/2004 01:51:48  ida
  for new snd_drv interface

  Revision 1.54  06/09/2004 05:59:05  ida
  fixed comments

  Revision 1.53  06/08/2004 00:50:00  ida
  workaround player heap

  Revision 1.52  05/28/2004 08:09:43  ida
  change function name

  Revision 1.51  05/26/2004 01:58:53  ida
  fix assert

  Revision 1.50  05/24/2004 07:33:22  ida
  change function name

  Revision 1.49  05/24/2004 05:41:10  ida
  ByNumber -> ByPlayerNo

  Revision 1.48  05/21/2004 06:57:46  ida
  NNS_SndHandleReleasePlayer -> NNS_SndHandleReleaseSeq

  Revision 1.47  05/21/2004 02:11:16  ida
  fix assert

  Revision 1.46  05/13/2004 01:02:07  ida
  merge seqarc.h and data.h to seqdata.h

  Revision 1.45  05/12/2004 06:39:58  ida
  modify data header structure

  Revision 1.44  05/10/2004 05:29:41  ida
  add NNS_SndPlayerGetTick

  Revision 1.43  05/10/2004 04:59:28  ida
  add NNS_SndPlayer[Read/Write]Variable

  Revision 1.42  05/10/2004 04:07:52  ida
  add NNS_SndPlayerGetSeqType

  Revision 1.41  05/10/2004 01:34:38  ida
  add NNS_SndPlayerSet*Priority

  Revision 1.40  05/06/2004 04:47:15  ida
  workaround SNDSharedWork

  Revision 1.39  05/06/2004 00:43:29  ida
  SNDRecvPort -> SNDSharedWork

  Revision 1.38  04/30/2004 07:35:38  ida
  add NNS_SndPlayerGetSeqNo

  Revision 1.37  04/30/2004 06:30:27  ida
  add NNS_SndPlayerCountPlayingSeqBySeqNo
  add NNS_SndPlayerStopSeqBySeqNo

  Revision 1.36  04/28/2004 07:47:16  ida
  fixed comments

  Revision 1.35  04/28/2004 01:55:49  ida
  add comments

  Revision 1.34  04/26/2004 08:45:19  ida
  fadeout finish after volume update

  Revision 1.33  04/26/2004 06:29:01  ida
  fix NNS_SndPlayerStopSeqByNumber hung-up bug

  Revision 1.32  04/26/2004 02:22:17  ida
  remove NNS_SndPlayerStartSeqArc

  Revision 1.31  04/15/2004 04:19:28  ida
  use NNSFndList

  Revision 1.30  04/12/2004 01:26:09  ida
  remove dump functions

  Revision 1.29  04/08/2004 08:16:09  ida
  add NNS_SndPlayerSetTrackMute

  Revision 1.28  04/08/2004 08:07:37  ida
  workaround multi sequence player

  Revision 1.27  04/05/2004 02:33:44  ida
  add comments

  Revision 1.26  04/02/2004 11:33:25  ida
  add comments

  Revision 1.25  04/02/2004 07:08:02  ida
  instantly stop seq when fadeFrame == 0

  Revision 1.24  04/02/2004 06:46:21  ida
  IsPlayingByNumber -> CountPlayingSeqByNumber

  Revision 1.23  2004/03/30 08:26:23  ida
  fix move volume bug

  Revision 1.22  03/26/2004 11:05:32  ida
  modified fadeout

  Revision 1.21  03/25/2004 10:07:18  ida
  fix fadeout bug when fadeFrame == 0

  Revision 1.20  03/24/2004 06:31:07  ida
  add NNS_SndPlayerMoveVolume()

  Revision 1.19  03/22/2004 10:43:05  ida
  NNS_SndHandleIsValid

  Revision 1.18  03/22/2004 07:23:39  ida
  add ByNumber functions

  Revision 1.17  03/18/2004 00:40:08  ida
  add tempo change function

  Revision 1.16  03/11/2004 02:33:40  ida
  PausePlayer -> PauseSeq

  Revision 1.15  03/10/2004 08:40:10  ida
  reconstruct data structure

  Revision 1.14  03/10/2004 06:36:08  ida
  (none)

  Revision 1.13  03/08/2004 06:58:51  ida
  using port tag system

  Revision 1.12  03/01/2004 02:59:11  ida
  fix fadeVolume initialize miss

  Revision 1.11  02/25/2004 05:49:54  ida
  add NNS_SndPlayerPause

  Revision 1.10  02/25/2004 05:38:25  ida
  workaround track parameters

  Revision 1.9  02/25/2004 02:32:20  ida
  workaround fadeOut

  Revision 1.8  02/24/2004 08:12:19  ida
  workaround recv port

  Revision 1.7  02/23/2004 03:23:31  ida
  change function prefix

  Revision 1.6  02/20/2004 07:43:54  ida
  remove ARM7 code

  Revision 1.5  02/20/2004 05:02:59  ida
  change directory

  Revision 1.3  02/19/2004 08:57:10  ida
  ARM7 -> SDK

  Revision 1.2  02/19/2004 01:46:54  ida
  fix get player status miss

  Revision 1.1  02/13/2004 01:42:05  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/player.h>

#include <nitro/os.h>
#include <nitro/snd.h>
#include <nnsys/misc.h>
#include <nnsys/snd/main.h>

/******************************************************************************
	macro definition
 ******************************************************************************/

#define FADER_SHIFT 8

/******************************************************************************
	structure definition
 ******************************************************************************/

typedef struct NNSSndPlayerHeap
{
    NNSFndLink link;
    NNSSndHeapHandle handle;
    NNSSndSeqPlayer* player;
    int playerNo;
} NNSSndPlayerHeap;

/******************************************************************************
	static variables
 ******************************************************************************/

static NNSSndSeqPlayer sSeqPlayer[ SND_PLAYER_NUM ];
static NNSSndPlayer sPlayer[ NNS_SND_PLAYER_NUM ];
static NNSFndList sPrioList;
static NNSFndList sFreeList;

/******************************************************************************
	static function declarations
 ******************************************************************************/

static void InitPlayer( NNSSndSeqPlayer* seqPlayer );
static void ShutdownPlayer( NNSSndSeqPlayer* seqPlayer );
static void ForceStopSeq( NNSSndSeqPlayer* seqPlayer );
static NNSSndSeqPlayer* AllocSeqPlayer( int prio );
static void InsertPlayerList( NNSSndPlayer* player, NNSSndSeqPlayer* seqPlayer );
static void InsertPrioList( NNSSndSeqPlayer* seqPlayer );
static void SetPlayerPriority( NNSSndSeqPlayer* seqPlayer, int priority );
static void PlayerHeapDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetPlayerVolume

  Description:  Volume setting for each player

  Arguments:    playerNo - Player number
                volume - Volume

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetPlayerVolume( int playerNo, int volume )
{
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( volume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    sPlayer[ playerNo ].volume = (u8)volume;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetPlayableSeqCount

  Description:  Sets the maximum number of sequences that can be played back simultaneously

  Arguments:    playerNo - Player number
                seqCount - Maximum number of sequences that can be played back simultaneously

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetPlayableSeqCount( int playerNo, int seqCount )
{
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( seqCount, 0, SND_PLAYER_NUM );
    
    sPlayer[ playerNo ].playableSeqCount = (u16)seqCount;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetAllocatableChannel

  Description:  Sets the allocatable channels

  Arguments:    playerNo - Player number
                chBitFlag - Bit flag for the allocatable channel
                            Specifying 0 makes all channels allocatable

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetAllocatableChannel( int playerNo, u32 chBitFlag )
{
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( chBitFlag, 0, 0xffff );
    
    sPlayer[ playerNo ].allocChBitFlag = chBitFlag;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCreateHeap

  Description:  Create player heap

  Arguments:    playerNo - Player number
                heap - Sound Heap
                size  - The size of the player heap

  Returns:      Whether or not the player heap was created successfully
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerCreateHeap( int playerNo, NNSSndHeapHandle heap, u32 size )
{
    NNSSndHeapHandle playerHeapHandle;
    NNSSndPlayerHeap* playerHeap;    
    void* buffer;
    
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    
    /* Allocates a buffer for use as the player heap*/
    buffer = NNS_SndHeapAlloc( heap, sizeof( NNSSndPlayerHeap ) + size, PlayerHeapDisposeCallback, 0, 0 );
    if ( buffer == NULL ) {
        return FALSE;
    }
    
    /* Initialize the player heap structure*/
    playerHeap = (NNSSndPlayerHeap*)buffer;
    
    playerHeap->player = NULL;
    playerHeap->playerNo = playerNo;
    playerHeap->handle = NNS_SND_HEAP_INVALID_HANDLE;
    
    /* Build player heap*/
    playerHeapHandle = NNS_SndHeapCreate(
        (u8*)buffer + sizeof( NNSSndPlayerHeap ),
        size
    );
    if ( playerHeapHandle == NNS_SND_HEAP_INVALID_HANDLE ) {
        return FALSE;
    }
    
    playerHeap->handle = playerHeapHandle;    
    NNS_FndAppendListObject( &sPlayer[ playerNo ].heapList, playerHeap );
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeq

  Description:  Stop sequence

  Arguments:    handle - Sound handle
                fadeFrame - Fade-out frame

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeq( NNSSndHandle* handle, int fadeFrame )
{
    NNS_NULL_ASSERT( handle );
    
    NNSi_SndPlayerStopSeq( handle->player, fadeFrame );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqByPlayerNo

  Description:  Stop sequence

  Arguments:    playerNo - Player number
                fadeFrame - Fade-out frame

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqByPlayerNo( int playerNo, int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    NNS_MINMAX_ASSERT( playerNo , 0, NNS_SND_PLAYER_NO_MAX );
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP &&
             seqPlayer->player == & sPlayer[ playerNo ] )
        {
            NNSi_SndPlayerStopSeq( seqPlayer, fadeFrame );            
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqBySeqNo

  Description:  Stop sequence

  Arguments:    seqNo - Sequence number
                fadeFrame - Fade-out frame

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqBySeqNo( int seqNo, int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP &&
             seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQ &&
             seqPlayer->seqNo == seqNo )
        {
            NNSi_SndPlayerStopSeq( seqPlayer ,fadeFrame );
        }
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqBySeqArcNo

  Description:  Stop sequence

  Arguments:    seqArcNo - sequence archive number
                fadeFrame - Fade-out frame

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqBySeqArcNo( int seqArcNo, int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP &&
             seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQARC &&
             seqPlayer->seqNo == seqArcNo )
        {
            NNSi_SndPlayerStopSeq( seqPlayer ,fadeFrame );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqBySeqArcIdx

  Description:  Stop sequence

  Arguments:    seqArcNo - sequence archive number
                index    - Index number
                fadeFrame - Fade-out frame

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqBySeqArcIdx( int seqArcNo, int index, int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP &&
             seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQARC &&
             seqPlayer->seqNo == seqArcNo &&
             seqPlayer->seqArcIndex == index )
        {
            NNSi_SndPlayerStopSeq( seqPlayer ,fadeFrame );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerStopSeqAll

  Description:  Stops all sequences

  Arguments:    fadeFrame - Fade-out frame

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerStopSeqAll( int fadeFrame )
{
    NNSSndSeqPlayer* seqPlayer;
    int i;
    
    for( i = 0; i < SND_PLAYER_NUM ; i++ )
    {
        seqPlayer = & sSeqPlayer[ i ];
        
        if ( seqPlayer->status != NNS_SND_SEQ_PLAYER_STATUS_STOP )
        {
            NNSi_SndPlayerStopSeq( seqPlayer, fadeFrame );
        }
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerPause

  Description:  Pauses or restarts a sequence

  Arguments:    handle - Sound handle
                flag   - Pause or restart

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerPause( NNSSndHandle* handle, BOOL flag )
{
    NNS_NULL_ASSERT( handle );

    NNSi_SndPlayerPause( handle->player, flag );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerPauseByPlayerNo

  Description:  Pauses or restarts a sequence

  Arguments:    playerNo - Player number
                flag   - Pause or restart

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerPauseByPlayerNo( int playerNo, BOOL flag )
{
    NNSSndSeqPlayer* seqPlayer;
    NNSSndSeqPlayer* next;
    
    NNS_MINMAX_ASSERT( playerNo , 0, NNS_SND_PLAYER_NO_MAX );
    
    for( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPlayer[ playerNo ].playerList, NULL );
         seqPlayer != NULL ; seqPlayer = next )
    {
        next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPlayer[ playerNo ].playerList, seqPlayer );
        
        NNSi_SndPlayerPause( seqPlayer, flag );
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerPauseAll

  Description:  Pauses or restarts a sequence

  Arguments:    flag   - Pause or restart

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerPauseAll( BOOL flag )
{
    NNSSndSeqPlayer* seqPlayer;
    NNSSndSeqPlayer* next;
    
    for( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, NULL );
         seqPlayer != NULL ; seqPlayer = next )
    {
        next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer );
        
        NNSi_SndPlayerPause( seqPlayer, flag );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHandleInit

  Description:  Initializes the sound handle

  Arguments:    handle - Sound handle

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHandleInit( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );
    
    handle->player = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndHandleReleaseSeq

  Description:  Releases the sequence from the sound handle

  Arguments:    handle - Sound handle

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndHandleReleaseSeq( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    NNS_ASSERT( handle == handle->player->handle );
    
    handle->player->handle = NULL;  
    handle->player = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCountPlayingSeqByPlayerNo

  Description:  Get number of sequences being played back

  Arguments:    playerNo - Player number

  Returns:      Number of sequences being played back
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerCountPlayingSeqByPlayerNo( int playerNo )
{
    NNS_MINMAX_ASSERT( playerNo , 0, NNS_SND_PLAYER_NO_MAX );
    
    return sPlayer[ playerNo ].playerList.numObjects;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCountPlayingSeqBySeqNo

  Description:  Get number of sequences being played back

  Arguments:    seqNo - Sequence number

  Returns:      Number of sequences being played back
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerCountPlayingSeqBySeqNo( int seqNo )
{
    int count = 0;
    
    NNSSndSeqPlayer* seqPlayer = NULL;
    while ( ( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer ) ) != NULL )
    {
        if ( seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQ &&
             seqPlayer->seqNo == seqNo )
        {
            count++;
        }
    }
    
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCountPlayingSeqBySeqArcNo

  Description:  Get number of sequences being played back

  Arguments:    seqArcNo - sequence archive number

  Returns:      Number of sequences being played back
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerCountPlayingSeqBySeqArcNo( int seqArcNo )
{
    int count = 0;
    
    NNSSndSeqPlayer* seqPlayer = NULL;
    while ( ( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer ) ) != NULL )
    {
        if ( seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQARC &&
             seqPlayer->seqNo == seqArcNo )
        {
            count++;
        }
    }
    
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerCountPlayingSeqBySeqArcIdx

  Description:  Get number of sequences being played back

  Arguments:    seqArcNo - sequence archive number
                index    - Index number

  Returns:      Number of sequences being played back
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerCountPlayingSeqBySeqArcIdx( int seqArcNo, int index )
{
    int count = 0;
    
    NNSSndSeqPlayer* seqPlayer = NULL;
    while ( ( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer ) ) != NULL )
    {
        if ( seqPlayer->seqType == NNS_SND_PLAYER_SEQ_TYPE_SEQARC &&
             seqPlayer->seqNo == seqArcNo &&
             seqPlayer->seqArcIndex == index )
        {
            count++;
        }
    }
    
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetVolume

  Description:  Update sequence volume

  Arguments:    handle - Sound handle
                volume - Volume

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetVolume( NNSSndHandle* handle, int volume )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( volume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    handle->player->extVolume = (u8)volume;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetInitialVolume

  Description:  Sets the initial sequence volume

  Arguments:    handle - Sound handle
                volume - Volume

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetInitialVolume( NNSSndHandle* handle, int volume )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( volume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    handle->player->initVolume = (u8)volume;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerMoveVolume

  Description:  Changes sequence volume

  Arguments:    handle - Sound handle
                targetVolume - Target volume
                frames - Number of frames whose volume is to be changed

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerMoveVolume( NNSSndHandle* handle, int targetVolume, int frames )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( targetVolume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    // Prohibited during fadeout
    if ( handle->player->status == NNS_SND_SEQ_PLAYER_STATUS_FADEOUT ) return;
    
    NNSi_SndFaderSet( & handle->player->fader, targetVolume << FADER_SHIFT, frames );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetPlayerPriority

  Description:  Changes the player's priority

  Arguments:    handle - Sound handle
                priority - Player's priority

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetPlayerPriority( NNSSndHandle* handle, int priority )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( priority, 0, NNS_SND_PLAYER_PRIO_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SetPlayerPriority( handle->player, priority );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetChannelPriority

  Description:  Changes channel priority

  Arguments:    handle - Sound handle
                priority - Channel priority

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetChannelPriority( NNSSndHandle* handle, int priority )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( priority, 0, 127 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetPlayerChannelPriority( handle->player->playerNo, priority );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackMute

  Description:  Mutes a track

  Arguments:    handle - Sound handle
                trackBitMask - Track bit mask
                flag         - Mute flag

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackMute( NNSSndHandle* handle, u16 trackBitMask, BOOL flag )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackMute(
        handle->player->playerNo,
        trackBitMask,
        flag
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackVolume

  Description:  Change track volume

  Arguments:    handle - Sound handle
                trackBitMask - Track bit mask
                volume - Volume

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackVolume( NNSSndHandle* handle, u16 trackBitMask, int volume )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( volume, 0, SND_CALC_DECIBEL_SCALE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackVolume(
        handle->player->playerNo,
        trackBitMask,
        SND_CalcDecibel( volume )
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackPitch

  Description:  Changes the track pitch

  Arguments:    handle - Sound handle
                trackBitMask - Track bit mask
                pitch        - Pitch

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackPitch( NNSSndHandle* handle, u16 trackBitMask, int pitch )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( pitch, NNS_SND_PLAYER_TRACK_PITCH_MIN, NNS_SND_PLAYER_TRACK_PITCH_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackPitch( handle->player->playerNo, trackBitMask, pitch );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackPan

  Description:  Changes the track pan

  Arguments:    handle - Sound handle
                trackBitMask - Track bit mask
                pan        - Pan

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackPan( NNSSndHandle* handle, u16 trackBitMask, int pan )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( pan, NNS_SND_PLAYER_TRACK_PAN_MIN, NNS_SND_PLAYER_TRACK_PAN_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackPan( handle->player->playerNo, trackBitMask, pan );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackPanRange

  Description:  Changes the track pan range

  Arguments:    handle - Sound handle
                trackBitMask - Track bit mask
                panRange     - Pan range

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackPanRange( NNSSndHandle* handle, u16 trackBitMask, int panRange )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( panRange, NNS_SND_PLAYER_TRACK_PAN_RANGE_MIN, NNS_SND_PLAYER_TRACK_PAN_RANGE_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackPanRange( handle->player->playerNo, trackBitMask, panRange );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackModDepth

  Description:  Changes the modulation depth

  Arguments:    handle - Sound handle
                trackBitMask - Track bit mask
                depth        - Modulation depth

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackModDepth( NNSSndHandle* handle, u16 trackBitMask, int depth )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( depth, 0, 255 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
	SND_SetTrackModDepth( handle->player->playerNo, trackBitMask, depth );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackModSpeed

  Description:  Changes the modulation speed

  Arguments:    handle - Sound handle
                trackBitMask - Track bit mask
                speed        - Modulation speed

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackModSpeed( NNSSndHandle* handle, u16 trackBitMask, int speed )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( speed, 0, 255 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackModSpeed( handle->player->playerNo, trackBitMask, speed );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTrackAllocatableChannel

  Description:  Changes the channel which can be allocated

  Arguments:    handle - Sound handle
                trackBitMask - Track bit mask
                chBitFlag - Bit flag for the allocatable channel
                               Specifying 0 makes all channels allocatable

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTrackAllocatableChannel( NNSSndHandle* handle, u16 trackBitMask, u32 chBitFlag )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( chBitFlag, 0, 0xffff );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetTrackAllocatableChannel( handle->player->playerNo, trackBitMask, chBitFlag );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetTempoRatio

  Description:  Changes the tempo ratio

  Arguments:    handle - Sound handle
                ratio  - Tempo ratio

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetTempoRatio( NNSSndHandle* handle, int ratio )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( ratio, 1, 65535 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    SND_SetPlayerTempoRatio( handle->player->playerNo, ratio );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetSeqNo

  Description:  Sets the sequence number

  Arguments:    handle - Sound handle
                seqNo - Sequence number

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetSeqNo( NNSSndHandle* handle, int seqNo )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( seqNo, 0, NNS_SND_PLAYER_SEQ_NO_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    handle->player->seqType = NNS_SND_PLAYER_SEQ_TYPE_SEQ;
    handle->player->seqNo = (u16)seqNo;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerSetSeqArcNo

  Description:  Sets the sequence archive number

  Arguments:    handle - Sound handle
                seqArcNo - sequence archive number
                index    - Index number

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndPlayerSetSeqArcNo( NNSSndHandle* handle, int seqArcNo, int index )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( seqArcNo, 0, NNS_SND_PLAYER_SEQ_NO_MAX );
    NNS_MINMAX_ASSERT( index, 0, NNS_SND_PLAYER_SEQARC_INDEX_MAX );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return;
    
    handle->player->seqType = NNS_SND_PLAYER_SEQ_TYPE_SEQARC;
    handle->player->seqNo       = (u16)seqArcNo;
    handle->player->seqArcIndex = (u16)index;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetSeqType

  Description:  Gets the sequence type

  Arguments:    handle - Sound handle

  Returns:      Sequence type
 *---------------------------------------------------------------------------*/
NNSSndPlayerSeqType NNS_SndPlayerGetSeqType( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return NNS_SND_PLAYER_SEQ_TYPE_INVALID;
    
    return (NNSSndPlayerSeqType)( handle->player->seqType );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetSeqNo

  Description:  Get sequence number

  Arguments:    handle - Sound handle

  Returns:      The sequence number or -1
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerGetSeqNo( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );

    if ( ! NNS_SndHandleIsValid( handle ) ) return -1;
    
    if ( handle->player->seqType != NNS_SND_PLAYER_SEQ_TYPE_SEQ ) return -1;

    return handle->player->seqNo;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetSeqArcNo

  Description:  Gets the sequence archive number

  Arguments:    handle - Sound handle

  Returns:      The sequence archive number or -1
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerGetSeqArcNo( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );

    if ( ! NNS_SndHandleIsValid( handle ) ) return -1;
    
    if ( handle->player->seqType != NNS_SND_PLAYER_SEQ_TYPE_SEQARC ) return -1;

    return handle->player->seqNo;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetSeqArcIdx

  Description:  Gets sequence archive index symbol

  Arguments:    handle - Sound handle

  Returns:      The sequence archive index or -1
 *---------------------------------------------------------------------------*/
int NNS_SndPlayerGetSeqArcIdx( NNSSndHandle* handle )
{
    NNS_NULL_ASSERT( handle );

    if ( ! NNS_SndHandleIsValid( handle ) ) return -1;
    
    if ( handle->player->seqType != NNS_SND_PLAYER_SEQ_TYPE_SEQARC ) return -1;
    
    return handle->player->seqArcIndex;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerReadVariable

  Description:  Reads the local sequence variable

  Arguments:    handle - Sound handle
                varNo  - Variable number
                var    - Address the variable is to be loaded into

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerReadVariable( NNSSndHandle* handle, int varNo, s16* var )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_NULL_ASSERT( handle );
    NNS_NULL_ASSERT( var );
    NNS_MINMAX_ASSERT( varNo, 0, SND_PLAYER_VARIABLE_NUM-1 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return FALSE;
    
    seqPlayer = handle->player;
    
    if ( ! seqPlayer->startFlag ) {
        *var = SND_DEFAULT_VARIABLE;
        return TRUE;
    }
    
    *var = SND_GetPlayerLocalVariable( seqPlayer->playerNo, varNo );
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerReadGlobalVariable

  Description:  Reads the global sequence variable

  Arguments:    varNo  - Variable number
                var    - Address the variable is to be loaded into

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerReadGlobalVariable( int varNo, s16* var )
{
    NNS_NULL_ASSERT( var );
    NNS_MINMAX_ASSERT( varNo, 0, SND_GLOBAL_VARIABLE_NUM-1 );
    
    *var = SND_GetPlayerGlobalVariable( varNo );
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerWriteVariable

  Description:  Writes the local sequence variable

  Arguments:    handle - Sound handle
                varNo  - Variable number
                var    - Value to be written

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerWriteVariable( NNSSndHandle* handle, int varNo, s16 var )
{
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( varNo, 0, SND_PLAYER_VARIABLE_NUM-1 );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return FALSE;
    
    SND_SetPlayerLocalVariable( handle->player->playerNo, varNo, var );

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerWriteGlobalVariable

  Description:  Writes the global sequence variable

  Arguments:    varNo  - Variable number
                var    - Value to be written

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerWriteGlobalVariable( int varNo, s16 var )
{
    NNS_MINMAX_ASSERT( varNo, 0, SND_GLOBAL_VARIABLE_NUM-1 );
    
    SND_SetPlayerGlobalVariable( varNo, var );

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerGetTick

  Description:  Gets the number of ticks since playback began

  Arguments:    handle - Sound handle

  Returns:      Number of ticks
 *---------------------------------------------------------------------------*/
u32 NNS_SndPlayerGetTick( NNSSndHandle* handle )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return 0;
    
    seqPlayer = handle->player;
    
    if ( ! seqPlayer->startFlag ) {
        // pre start
        return 0;
    }
    
    return SND_GetPlayerTickCounter( seqPlayer->playerNo );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerReadDriverPlayerInfo

  Description:  Gets player information for the driver

  Arguments:    handle - Sound handle
                info - Player information structure that holds the obtained player information.

  Returns:      Whether or not the information was retrieved successfully
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerReadDriverPlayerInfo( NNSSndHandle* handle, SNDPlayerInfo* info )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return FALSE;
    
    seqPlayer = handle->player;
    NNS_NULL_ASSERT( seqPlayer );
    
    return NNSi_SndReadDriverPlayerInfo( seqPlayer->playerNo, info );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndPlayerReadDriverTrackInfo

  Description:  Gets track information for the driver

  Arguments:    handle - Sound handle
                trackNo - Track number
                info - Track information structure that holds the obtained player information

  Returns:      Whether or not the information was retrieved successfully
 *---------------------------------------------------------------------------*/
BOOL NNS_SndPlayerReadDriverTrackInfo( NNSSndHandle* handle, int trackNo, SNDTrackInfo* info )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_NULL_ASSERT( handle );
    
    if ( ! NNS_SndHandleIsValid( handle ) ) return FALSE;
    
    seqPlayer = handle->player;
    NNS_NULL_ASSERT( seqPlayer );
    
    return NNSi_SndReadDriverTrackInfo( seqPlayer->playerNo, trackNo, info );
}

/******************************************************************************
	private functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerInit

  Description:  Initializes the player library

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerInit( void )
{
    NNSSndPlayer* player;
    int playerNo;
    
    NNS_FND_INIT_LIST( &sPrioList, NNSSndSeqPlayer, prioLink );
    NNS_FND_INIT_LIST( &sFreeList, NNSSndSeqPlayer, prioLink );
    
    for( playerNo = 0; playerNo < SND_PLAYER_NUM ; playerNo++ )
    {
        sSeqPlayer[ playerNo ].status = NNS_SND_SEQ_PLAYER_STATUS_STOP;
        sSeqPlayer[ playerNo ].playerNo = (u8)playerNo;
        NNS_FndAppendListObject( & sFreeList, & sSeqPlayer[ playerNo ] );
    }
    
    for( playerNo = 0; playerNo < NNS_SND_PLAYER_NUM ; playerNo++ )
    {
        player = &sPlayer[ playerNo ];
        
        NNS_FND_INIT_LIST( & player->playerList, NNSSndSeqPlayer, playerLink );
        NNS_FND_INIT_LIST( & player->heapList, NNSSndPlayerHeap, link );
        player->volume = 127;
        player->playableSeqCount = 1;
        player->allocChBitFlag = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerMain

  Description:  Frame work for the player library

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerMain( void )
{
    NNSSndSeqPlayer* seqPlayer;
    NNSSndSeqPlayer* next;
	u32 status;
    int fader;
    
    // Gets the player's status
    status = SND_GetPlayerStatus();
    
    for( seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, NULL );
         seqPlayer != NULL ; seqPlayer = next )
    {
        next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, seqPlayer );
        
        // Check start flag
        if ( ! seqPlayer->startFlag ) {
            if ( SND_IsFinishedCommandTag( seqPlayer->commandTag ) )
            {
                seqPlayer->startFlag = TRUE;
            }
        }
        
        // Stop check from ARM7 side
        if ( seqPlayer->startFlag )
        {
            if ( ( status & ( 1 << seqPlayer->playerNo ) ) == 0 ) {
                ShutdownPlayer( seqPlayer );
                continue;
            }
        }
        
        // Update fader
        NNSi_SndFaderUpdate( & seqPlayer->fader );
        
        // Update parameter
        fader
            = SND_CalcDecibel( seqPlayer->initVolume )
            + SND_CalcDecibel( seqPlayer->extVolume )
            + SND_CalcDecibel( seqPlayer->player->volume )
            + SND_CalcDecibel( NNSi_SndFaderGet( & seqPlayer->fader ) >> FADER_SHIFT )
            ;
        if ( fader < SND_VOLUME_DB_MIN ) fader = SND_VOLUME_DB_MIN;
        else if ( fader > SND_VOLUME_DB_MAX ) fader = SND_VOLUME_DB_MAX;
        
        if ( fader != seqPlayer->volume )
        {
            SND_SetPlayerVolume( seqPlayer->playerNo, fader );
            seqPlayer->volume = (s16)fader;
        }
        
        // Fadeout completion check
        if ( seqPlayer->status == NNS_SND_SEQ_PLAYER_STATUS_FADEOUT )
        {
            if ( NNSi_SndFaderIsFinished( & seqPlayer->fader ) )
            {
                ForceStopSeq( seqPlayer );
            }
        }
        
        // Prepare flag
        if ( seqPlayer->prepareFlag ) {
            SND_StartPreparedSeq( seqPlayer->playerNo );
            seqPlayer->prepareFlag = FALSE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerAllocSeqPlayer

  Description:  Allocates a sequence player

  Arguments:    handle - Sound handle
                playerNo - Player number
                prio     - Player priority

  Returns:      The allocated sequence player or NULL if allocation failed
                 
 *---------------------------------------------------------------------------*/
NNSSndSeqPlayer* NNSi_SndPlayerAllocSeqPlayer( NNSSndHandle* handle, int playerNo, int prio )
{
    NNSSndSeqPlayer* seqPlayer;
    NNSSndPlayer* player;

    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( prio, 0, NNS_SND_PLAYER_PRIO_MAX );
    
    player = & sPlayer[ playerNo ];
    
    // Disconnect sound handle
    if ( NNS_SndHandleIsValid( handle ) ) {
        NNS_SndHandleReleaseSeq( handle );
    }
    
    // Check the maximum number of sequences which can be played back simultaneously for each player
    if ( player->playerList.numObjects >= player->playableSeqCount )
    {
        // Get the lowest priority object
        seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & player->playerList, NULL );
        if ( seqPlayer == NULL ) return NULL;
        if ( prio < seqPlayer->prio ) return NULL;
        
        ForceStopSeq( seqPlayer );
    }
    
    // Allocation processing
    seqPlayer = AllocSeqPlayer( prio );
    if ( seqPlayer == NULL ) return NULL;
    
    // Initialization
    InsertPlayerList( player, seqPlayer );
    
    // Connect to sound handle
    seqPlayer->handle = handle;
    handle->player = seqPlayer;
    
    return seqPlayer;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerFreeSeqPlayer

  Description:  Frees the sequence player

  Arguments:    seqPlayer - Sequence player

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerFreeSeqPlayer( NNSSndSeqPlayer* seqPlayer )
{
    NNS_NULL_ASSERT( seqPlayer );

    ShutdownPlayer( seqPlayer );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerStartSeq

  Description:  Starts a sequence

  Arguments:    seqPlayer - Sequence player
                seqDataBase   - Base address of sequence data
                seqDataOffset - Sequence data offset
                bank - Bank data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerStartSeq(
    NNSSndSeqPlayer* seqPlayer,
    const void* seqDataBase,
    u32 seqDataOffset,
    const SNDBankData* bank
)
{
    NNSSndPlayer* player;
    
    NNS_NULL_ASSERT( seqPlayer );
    NNS_NULL_ASSERT( seqDataBase );
    NNS_NULL_ASSERT( bank );
    
    player = seqPlayer->player;
    NNS_NULL_ASSERT( player );
    
    SND_PrepareSeq(
        seqPlayer->playerNo,
        seqDataBase,
        seqDataOffset,
        bank
    );
    if ( player->allocChBitFlag ) {
        SND_SetTrackAllocatableChannel(
            seqPlayer->playerNo,
            0xffff,
            player->allocChBitFlag
        );
    }
    
    // Initilizaton processing
    InitPlayer( seqPlayer );
    seqPlayer->commandTag = SND_GetCurrentCommandTag();
    seqPlayer->prepareFlag = TRUE;
    seqPlayer->status = NNS_SND_SEQ_PLAYER_STATUS_PLAY;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerStopSeq

  Description:  Stop sequence

  Arguments:    seqPlayer - Sequence player
                fadeFrame - Fade-out frame

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerStopSeq( NNSSndSeqPlayer* seqPlayer, int fadeFrame )
{
    if ( seqPlayer == NULL ) return;
    if ( seqPlayer->status == NNS_SND_SEQ_PLAYER_STATUS_STOP ) return;
    
    if ( fadeFrame == 0 )
    {
        ForceStopSeq( seqPlayer );
        return;
    }
    
    NNSi_SndFaderSet( & seqPlayer->fader, 0, fadeFrame );
    
    SetPlayerPriority( seqPlayer, 0 );
    
    seqPlayer->status = NNS_SND_SEQ_PLAYER_STATUS_FADEOUT;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerPause

  Description:  Pauses or restarts a sequence

  Arguments:    seqPlayer - Sequence player
                flag      - Pause or restart

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndPlayerPause( NNSSndSeqPlayer* seqPlayer, BOOL flag )
{
    if ( seqPlayer == NULL ) return;
    
    if ( flag != seqPlayer->pauseFlag )
    {
        SND_PauseSeq( seqPlayer->playerNo, flag );        
        seqPlayer->pauseFlag = (u8)flag;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndPlayerAllocHeap

  Description:  Allocate player heap

  Arguments:    playerNo - Player number
                seqPlayer - Sequence player that will use the heap

  Returns:      Allocated heap
 *---------------------------------------------------------------------------*/
NNSSndHeapHandle NNSi_SndPlayerAllocHeap( int playerNo, NNSSndSeqPlayer* seqPlayer )
{
    NNSSndPlayer* player;
    NNSSndPlayerHeap* heap;
    
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    
    player = & sPlayer[ playerNo ];
    
    heap = (NNSSndPlayerHeap*)NNS_FndGetNextListObject( & player->heapList, NULL );
    if ( heap == NULL ) return NULL;
    
    NNS_FndRemoveListObject( & player->heapList, heap );
    
    heap->player = seqPlayer;
    seqPlayer->heap = heap;
    
    NNS_SndHeapClear( heap->handle );
    
    return heap->handle;
}

/******************************************************************************
	static functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         InitPlayer

  Description:  Initializes the sequence player

  Arguments:    seqPlayer - Sequence player

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitPlayer( NNSSndSeqPlayer* seqPlayer )
{
    NNS_NULL_ASSERT( seqPlayer );
    
    seqPlayer->pauseFlag = FALSE;
    seqPlayer->startFlag = FALSE;
    seqPlayer->prepareFlag = FALSE;
    
    seqPlayer->seqType = NNS_SND_PLAYER_SEQ_TYPE_INVALID;
    
    seqPlayer->volume = 0;
    
    seqPlayer->initVolume = 127;
    seqPlayer->extVolume = 127;
    
    NNSi_SndFaderInit( & seqPlayer->fader );
    NNSi_SndFaderSet( & seqPlayer->fader, 127 << FADER_SHIFT, 1 );
}

/*---------------------------------------------------------------------------*
  Name:         InsertPlayerList

  Description:  Adds players to the player list

  Arguments:    player      - Player
                seqPlayer - Sequence player

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InsertPlayerList( NNSSndPlayer* player, NNSSndSeqPlayer* seqPlayer )
{
    NNSSndSeqPlayer* next = NULL;
    while ( ( next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & player->playerList, next ) ) != NULL )
    {
        if ( seqPlayer->prio < next->prio ) break;
    }
    
    NNS_FndInsertListObject( & player->playerList, next, seqPlayer );
    
    seqPlayer->player = player;
}

/*---------------------------------------------------------------------------*
  Name:         InsertPrioList

  Description:  Adds to the priority list

  Arguments:    seqPlayer - Sequence player

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InsertPrioList( NNSSndSeqPlayer* seqPlayer )
{
    NNSSndSeqPlayer* next = NULL;
    while ( ( next = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, next ) ) != NULL )
    {
        if ( seqPlayer->prio < next->prio ) break;
    }

    NNS_FndInsertListObject( & sPrioList, next, seqPlayer );
}

/*---------------------------------------------------------------------------*
  Name:         ForceStopSeq

  Description:  Forcibly stops a sequence

  Arguments:    seqPlayer - Sequence player

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ForceStopSeq( NNSSndSeqPlayer* seqPlayer )
{
    if ( seqPlayer->status == NNS_SND_SEQ_PLAYER_STATUS_FADEOUT ) {
        SND_SetPlayerVolume( seqPlayer->playerNo, SND_VOLUME_DB_MIN );
    }
    SND_StopSeq( seqPlayer->playerNo );
    ShutdownPlayer( seqPlayer );
}

/*---------------------------------------------------------------------------*
  Name:         AllocSeqPlayer

  Description:  Allocates a sequence player

  Arguments:    prio     - Player priority

  Returns:      The allocated sequence player or NULL if allocation failed
                 
 *---------------------------------------------------------------------------*/
static NNSSndSeqPlayer* AllocSeqPlayer( int prio )
{
    NNSSndSeqPlayer* seqPlayer;
    
    NNS_MINMAX_ASSERT( prio, 0, NNS_SND_PLAYER_PRIO_MAX );
    
    seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sFreeList, NULL );
    if ( seqPlayer == NULL )
    {
        // get lowest priority player
        seqPlayer = (NNSSndSeqPlayer*)NNS_FndGetNextListObject( & sPrioList, NULL );        
        NNS_NULL_ASSERT( seqPlayer );
        
        if ( prio < seqPlayer->prio ) return NULL;
        
        ForceStopSeq( seqPlayer );
    }
    NNS_FndRemoveListObject( & sFreeList, seqPlayer );
    
    seqPlayer->prio = (u8)prio;
    
    InsertPrioList( seqPlayer );
    
    return seqPlayer;
}

/*---------------------------------------------------------------------------*
  Name:         ShutdownPlayer

  Description:  Shuts down player after sequence stops

  Arguments:    seqPlayer - Sequence player

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ShutdownPlayer( NNSSndSeqPlayer* seqPlayer )
{
    NNSSndPlayer* player;
    
    NNS_NULL_ASSERT( seqPlayer );
    
    // Disconnect sound handle
    if ( seqPlayer->handle != NULL ) {
        NNS_ASSERT( seqPlayer == seqPlayer->handle->player );
        seqPlayer->handle->player = NULL;
        seqPlayer->handle = NULL;
    }
    
    // Disconnect player
    player = seqPlayer->player;
    NNS_NULL_ASSERT( player );
    NNS_FndRemoveListObject( & player->playerList, seqPlayer );        
    seqPlayer->player = NULL;
    
    // Free heap
    if ( seqPlayer->heap != NULL ) {
        NNS_FndAppendListObject( & player->heapList, seqPlayer->heap );
        seqPlayer->heap->player = NULL;
        seqPlayer->heap = NULL;
    }
    
    // Move to free list
    NNS_FndRemoveListObject( &sPrioList, seqPlayer );
    NNS_FndAppendListObject( &sFreeList, seqPlayer );
    
    seqPlayer->status = NNS_SND_SEQ_PLAYER_STATUS_STOP;
}

/*---------------------------------------------------------------------------*
  Name:         PlayerHeapDisposeCallback

  Description:  Performs processing when the player heap is disposed of

  Arguments:    mem    - The start address of the memory block
                size  - The size of the memory block
                data1 - User data (Not used)
                data2 - User data (Not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void PlayerHeapDisposeCallback( void* mem, u32 /*size*/, u32 /*data1*/, u32 /*data2*/ )

{
    NNSSndPlayerHeap* heap = (NNSSndPlayerHeap*)mem;
    NNSSndSeqPlayer* seqPlayer;
    
    if ( heap->handle == NNS_SND_HEAP_INVALID_HANDLE ) return;
    
    // Clear heap
    NNS_SndHeapDestroy( heap->handle );
    
    seqPlayer = heap->player;
    if ( seqPlayer != NULL ) {
        // Disconnect player during use
        seqPlayer->heap = NULL;
    }
    else {
        // Delete from heap list when not being used
        NNS_FndRemoveListObject( &sPlayer[ heap->playerNo ].heapList, heap );
    }
}

/*---------------------------------------------------------------------------*
  Name:         SetPlayerPriority

  Description:  Changes the player's priority

  Arguments:    seqPlayer - Sequence player
                priority - Player's priority

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetPlayerPriority( NNSSndSeqPlayer* seqPlayer, int priority )
{
    NNSSndPlayer* player;
    
    NNS_NULL_ASSERT( seqPlayer );
    NNS_MINMAX_ASSERT( priority, 0, NNS_SND_PLAYER_PRIO_MAX );
    
    player = seqPlayer->player;
    if ( player != NULL ) {
        NNS_FndRemoveListObject( & player->playerList, seqPlayer );
        seqPlayer->player = NULL;
    }
    NNS_FndRemoveListObject( & sPrioList, seqPlayer );
    
    seqPlayer->prio = (u8)priority;
    
    if ( player != NULL ) {
        InsertPlayerList( player, seqPlayer );
    }
    InsertPrioList( seqPlayer );
}


/*====== End of player.c ======*/


