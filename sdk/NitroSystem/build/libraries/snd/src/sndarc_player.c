/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     sndarc_player.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sndarc_player.c,v $
  Revision 1.41  03/02/2005 00:41:33  ida
  Changed operation so that bank and waveform data used in sequence archive
  is loaded into the player heap.

  Revision 1.40  02/28/2005 04:20:03  ida
  Changed operation so that player heap is cleared during allocation

  Revision 1.39  02/28/2005 02:07:29  ida
  Shared load processing with sndarc_loader.c

  Revision 1.38  01/06/2005 02:51:20  ida
  Updated copyright year notation

  Revision 1.37  01/05/2005 06:47:06  ida
  Added calling of NNS_SndPlayerSetAllocatableChannel

  Revision 1.36  10/21/2004 02:13:03  ida
  for NNS_SndArcLoadFile become private

  Revision 1.35  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.34  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.33  07/13/2004 05:43:05  ida
  change API name

  Revision 1.32  07/06/2004 07:46:44  ida
  workaround multi wave archive for one bank

  Revision 1.31  06/23/2004 06:20:22  ida
  for new snd_drv interface

  Revision 1.30  06/09/2004 06:16:46  ida
  fixed comments

  Revision 1.29  06/08/2004 00:52:03  ida
  workaround player heap

  Revision 1.28  05/28/2004 08:09:43  ida
  change function name

  Revision 1.27  05/27/2004 06:35:50  ida
  added NNS_SndArcPlayerAutoLoad

  Revision 1.26  05/24/2004 07:33:31  ida
  change function name

  Revision 1.25  05/24/2004 05:41:44  ida
  WithPlayerNo -> inline function

  Revision 1.24  05/14/2004 02:15:47  ida
  add failed load check

  Revision 1.23  05/13/2004 06:40:59  ida
  added NNS_SndArcPlayerAutoLoad*

  Revision 1.22  05/13/2004 02:23:18  ida
  add loaded data check

  Revision 1.21  05/13/2004 01:02:12  ida
  merge seqarc.h and data.h to seqdata.h

  Revision 1.20  05/12/2004 06:39:58  ida
  modify data header structure

  Revision 1.19  05/10/2004 01:35:01  ida
  added NNS_SndArcPlayerStartSeq*Ex

  Revision 1.18  04/30/2004 06:31:21  ida
  use NNS_SndPlayerSetSeqNo

  Revision 1.17  04/30/2004 05:05:38  ida
  fixed comments

  Revision 1.16  04/30/2004 04:38:59  ida
  set sequence max in NNS_SndArcPlayerInit

  Revision 1.15  04/26/2004 02:57:53  ida
  for file system

  Revision 1.14  04/06/2004 23:54:22  ida
  NNS_Snd -> NNSi_Snd

  Revision 1.13  04/05/2004 02:33:44  ida
  add comments

  Revision 1.12  04/02/2004 06:45:53  ida
  hide private functions

  Revision 1.11  03/30/2004 05:36:13  ida
  hid functions

  Revision 1.10  03/25/2004 10:08:41  ida
  added StartSeqWithPlayerNo

  Revision 1.9  03/24/2004 06:31:25  ida
  added GetPlayingPlayerNo

  Revision 1.8  03/22/2004 07:24:09  ida
  added StartSeqEx functions

  Revision 1.7  03/10/2004 08:40:10  ida
  reconstruct data structure

  Revision 1.6  02/25/2004 05:38:04  ida
  add code32.h

  Revision 1.5  02/25/2004 02:32:20  ida
  workaround fadeOut

  Revision 1.4  02/23/2004 03:23:26  ida
  change function prefix

  Revision 1.3  02/20/2004 05:02:59  ida
  change directory

  Revision 1.3  02/19/2004 01:44:42  ida
  add NULL check

  Revision 1.2  02/16/2004 06:22:07  ida
  (none)

  Revision 1.1  02/13/2004 01:42:05  ida
  initial version

  $NoKeywords: $
  *---------------------------------------------------------------------------*/
#include <nnsys/snd/sndarc_player.h>

#include <nnsys/misc.h>
#include <nnsys/snd/sndarc.h>
#include <nnsys/snd/player.h>
#include <nnsys/snd/sndarc_loader.h>

/******************************************************************************
	static function declarations
 ******************************************************************************/

static BOOL StartSeq(
    NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    const NNSSndArcSeqInfo* info,
    int seqNo
);
static BOOL StartSeqArc(
    NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    const NNSSndSeqArcSeqInfo* sound,
    const NNSSndSeqArc* seqArc,
    int seqArcNo,
    int index
);

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcPlayerSetup

  Description:  Set up the player using player information in the sound archive
                 

  Arguments:    heap - The sound heap for creating the player heap

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcPlayerSetup( NNSSndHeapHandle heap )
{
    NNSSndArc* arc = NNS_SndArcGetCurrent();
    int playerNo;
    const NNSSndArcPlayerInfo* playerInfo;
    
    NNS_NULL_ASSERT( arc );
    
    for( playerNo = 0; playerNo < NNS_SND_PLAYER_NUM ; ++playerNo )
    {
        playerInfo = NNS_SndArcGetPlayerInfo( playerNo );
        if ( playerInfo == NULL ) continue;
        
        NNS_SndPlayerSetPlayableSeqCount( playerNo, playerInfo->seqMax );
        NNS_SndPlayerSetAllocatableChannel( playerNo, playerInfo->allocChBitFlag );
        
        if ( playerInfo->heapSize > 0 && heap != NNS_SND_HEAP_INVALID_HANDLE )
        {
            int i;
            
            for( i = 0; i < playerInfo->seqMax ; i++ )
            {
                if ( ! NNS_SndPlayerCreateHeap( playerNo, heap, playerInfo->heapSize ) ) {
                    return FALSE;
                }
            }
        }
    }
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcPlayerStartSeq

  Description:  Plays sequence

  Arguments:    handle - Sound handle
                seqNo - Sequence number

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcPlayerStartSeq( NNSSndHandle* handle, int seqNo )
{
    const NNSSndArcSeqInfo* info;
    
    NNS_NULL_ASSERT( handle );
    
    info = NNS_SndArcGetSeqInfo( seqNo );
    if ( info == NULL ) return FALSE;    
    
    return StartSeq(
        handle,
        info->param.playerNo,
        info->param.bankNo,
        info->param.playerPrio,
        info,
        seqNo
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcPlayerStartSeqEx

  Description:  Sets a number of parameters and plays sequence

  Arguments:    handle - Sound handle
                playerNo   - Player number (-1 is entered if player number not specified)
                bankNo     - Bank number (-1 is entered if not specified)
                playerPrio - Player priority (-1 is entered if not specified)
                seqNo - Sequence number

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcPlayerStartSeqEx(
    NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    int seqNo
)
{
    const NNSSndArcSeqInfo* info;
   
    NNS_NULL_ASSERT( handle );
    NNS_MAX_ASSERT( playerNo,   NNS_SND_PLAYER_NUM-1 );
    NNS_MAX_ASSERT( playerPrio, NNS_SND_PLAYER_PRIO_MAX );

    info = NNS_SndArcGetSeqInfo( seqNo );
    if ( info == NULL ) return FALSE;    
    
    return StartSeq(
        handle,
        playerNo   >= 0 ? playerNo   : info->param.playerNo,
        bankNo     >= 0 ? bankNo     : info->param.bankNo,
        playerPrio >= 0 ? playerPrio : info->param.playerPrio,
        info,
        seqNo
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcPlayerStartSeqArc

  Description:  Plays sequence archive

  Arguments:    handle - Sound handle
                seqArcNo - sequence archive number
                index    - Index number

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcPlayerStartSeqArc( NNSSndHandle* handle, int seqArcNo, int index )
{
    const NNSSndArcSeqArcInfo* info;
    const NNSSndSeqArcSeqInfo* sound;
    const NNSSndSeqArc* seqArc;
    
    NNS_NULL_ASSERT( handle );
    
    info = NNS_SndArcGetSeqArcInfo( seqArcNo );
    if ( info == NULL ) return FALSE;
    seqArc = (NNSSndSeqArc*)NNS_SndArcGetFileAddress( info->fileId );
    if ( seqArc == NULL ) return FALSE;
    sound = NNSi_SndSeqArcGetSeqInfo( seqArc, index );
    if ( sound == NULL ) return FALSE;
    
    return StartSeqArc(
        handle,
        sound->param.playerNo,
        sound->param.bankNo,
        sound->param.playerPrio,
        sound,
        seqArc,
        seqArcNo,
        index
    );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcPlayerStartSeqArcEx

  Description:  Sets a number of parameters and plays sequence archive

  Arguments:    handle - Sound handle
                playerNo   - Player number (-1 is entered if player number not specified)
                bankNo     - Bank number (-1 is entered if not specified)
                playerPrio - Player priority (-1 is entered if not specified)
                seqArcNo - sequence archive number
                index    - Index number

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcPlayerStartSeqArcEx(
    NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    int seqArcNo,
    int index
)
{
    const NNSSndArcSeqArcInfo* info;
    const NNSSndSeqArc* seqArc;
    const NNSSndSeqArcSeqInfo* sound;
    
    NNS_NULL_ASSERT( handle );
    NNS_MAX_ASSERT( playerNo,   NNS_SND_PLAYER_NUM-1 );
    NNS_MAX_ASSERT( playerPrio, NNS_SND_PLAYER_PRIO_MAX );
    
    info = NNS_SndArcGetSeqArcInfo( seqArcNo );
    if ( info == NULL ) return FALSE;
    seqArc = (NNSSndSeqArc*)NNS_SndArcGetFileAddress( info->fileId );
    if ( seqArc == NULL ) return FALSE;
    sound = NNSi_SndSeqArcGetSeqInfo( seqArc, index );
    if ( sound == NULL ) return FALSE;
    
    return StartSeqArc(
        handle,
        playerNo   >= 0 ? playerNo   : sound->param.playerNo,
        bankNo     >= 0 ? bankNo     : sound->param.bankNo,
        playerPrio >= 0 ? playerPrio : sound->param.playerPrio,
        sound,
        seqArc,
        seqArcNo,
        index
    );
}

/******************************************************************************
	static function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         StartSeq

  Description:  Plays sequence

  Arguments:    handle - Sound handle
                playerNo - Player number
                bankNo - Bank number
                playerPrio - Player priority
                info       - Pointer to sequence-information structure
                seqNo - Sequence number

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
static BOOL StartSeq(
    NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    const NNSSndArcSeqInfo* info,
    int seqNo
)
{
    NNSSndSeqPlayer* player;
    NNSSndHeapHandle heap;
    NNSSndSeqData* seq;
    SNDBankData* bank;
    NNSSndArcLoadResult result;
    
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( playerPrio, 0, NNS_SND_PLAYER_PRIO_MAX );
    NNS_NULL_ASSERT( info );
    
    /* Allocate player*/
    player = NNSi_SndPlayerAllocSeqPlayer( handle, playerNo, playerPrio );
    if ( player == NULL ) return FALSE;
    
    /* Allocate player heap*/
    heap = NNSi_SndPlayerAllocHeap( playerNo, player );
    
    /* Obtain or load bank and waveform data*/
    result = NNSi_SndArcLoadBank( bankNo, NNS_SND_ARC_LOAD_BANK | NNS_SND_ARC_LOAD_WAVE, heap, FALSE, &bank );
    if ( result != NNS_SND_ARC_LOAD_SUCESS ) {
        NNSi_SndPlayerFreeSeqPlayer( player );
        return FALSE;
    }
    
    /* Obtain or load sequence data*/
    result = NNSi_SndArcLoadSeq( seqNo, NNS_SND_ARC_LOAD_SEQ, heap, FALSE, &seq );
    if ( result != NNS_SND_ARC_LOAD_SUCESS ) {
        NNSi_SndPlayerFreeSeqPlayer( player );
        return FALSE;
    }
    
    /* Play sequence*/
    NNSi_SndPlayerStartSeq(
        player,
        (u8*)seq + seq->baseOffset,
        0,
        bank
    );
    
    /* Set parameters*/
    NNS_SndPlayerSetInitialVolume( handle, info->param.volume );
    NNS_SndPlayerSetChannelPriority( handle, info->param.channelPrio );
    NNS_SndPlayerSetSeqNo( handle, seqNo );    
    
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         StartSeqArc

  Description:  Plays sequence archive

  Arguments:    handle - Sound handle
                playerNo - Player number
                bankNo - Bank number
                playerPrio - Player priority
                sound      - Sequence information structure in the sequence archive
                seqArc     - Sequence archive pointer
                seqArcNo - sequence archive number
                index    - Index number

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
static BOOL StartSeqArc(
    NNSSndHandle* handle,
    int playerNo,
    int bankNo,
    int playerPrio,
    const NNSSndSeqArcSeqInfo* sound,
    const NNSSndSeqArc* seqArc,
    int seqArcNo,
    int index
)
{
    NNSSndSeqPlayer* player;
    NNSSndHeapHandle heap;
    SNDBankData* bank;
    NNSSndArcLoadResult result;
    
    NNS_NULL_ASSERT( handle );
    NNS_MINMAX_ASSERT( playerNo, 0, NNS_SND_PLAYER_NO_MAX );
    NNS_MINMAX_ASSERT( playerPrio, 0, NNS_SND_PLAYER_PRIO_MAX );
    NNS_NULL_ASSERT( sound );
    NNS_NULL_ASSERT( seqArc );
    
    /* Allocate player*/
    player = NNSi_SndPlayerAllocSeqPlayer( handle, playerNo, playerPrio );
    if ( player == NULL ) return FALSE;
    
    /* Allocate player heap*/
    heap = NNSi_SndPlayerAllocHeap( playerNo, player );
    
    /* Obtain or load bank and waveform data*/
    result = NNSi_SndArcLoadBank( bankNo, NNS_SND_ARC_LOAD_BANK | NNS_SND_ARC_LOAD_WAVE, heap, FALSE, &bank );
    if ( result != NNS_SND_ARC_LOAD_SUCESS ) {
        NNSi_SndPlayerFreeSeqPlayer( player );
        return FALSE;
    }
    
    /* Play sequence*/
    NNSi_SndPlayerStartSeq(
        player,
        (u8*)seqArc + seqArc->baseOffset,
        sound->offset,
        bank
    );
    
    /* Set parameters*/    
    NNS_SndPlayerSetInitialVolume( handle, sound->param.volume );
    NNS_SndPlayerSetChannelPriority( handle, sound->param.channelPrio );    
    NNS_SndPlayerSetSeqArcNo( handle, seqArcNo, index );
    
    return TRUE;
}

/*====== End of sndarc_player.c ======*/


