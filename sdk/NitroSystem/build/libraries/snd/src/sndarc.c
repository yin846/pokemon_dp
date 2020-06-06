/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     sndarc.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sndarc.c,v $
  Revision 1.44  08/18/2005 07:04:14  ida
  Added the function NNS_SndArcInitWithResult

  Revision 1.43  03/16/2005 06:33:11  ida
  Added ASSERT message

  Revision 1.42  03/08/2005 08:37:37  ida
  Added assert for when NNS_SndArcInitOnMemory is used

  Revision 1.41  03/08/2005 05:39:49  ida
  Support for NNS_FROM_TOOL

  Revision 1.40  02/21/2005 02:37:17  ida
  Warning measures

  Revision 1.39  01/06/2005 02:51:20  ida
  Updated copyright year notation

  Revision 1.38  12/21/2004 05:44:45  ida
  Arranged the NNS_SndArcSetup function

  Revision 1.37  12/01/2004 06:17:50  ida
  Coping with forgetting NULL check

  Revision 1.36  10/19/2004 01:53:43  ida
  load header in NNS_SndArcSetup

  Revision 1.35  09/13/2004 05:26:03  ida
  add NNS_SndArcGetSeqArcSeqCount

  Revision 1.34  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.33  09/06/2004 07:24:05  ida
  for SDK_SMALL_BUILD

  Revision 1.32  08/23/2004 01:38:09  ida
  add NNS_SndArcSetup

  Revision 1.31  08/02/2004 05:28:44  ida
  add stream info

  Revision 1.30  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.29  07/06/2004 07:46:44  ida
  workaround multi wave archive for one bank

  Revision 1.28  06/21/2004 01:10:46  ida
  for sound archive without symbol data

  Revision 1.27  06/09/2004 08:45:34  ida
  fixed comments

  Revision 1.26  06/08/2004 00:51:24  ida
  changed interface

  Revision 1.25  05/27/2004 06:35:15  ida
  add NNS_SndArcGetGroupInfo

  Revision 1.24  05/26/2004 02:25:56  ida
  add NNS_SndArcInitOnMemory

  Revision 1.23  05/21/2004 08:19:20  ida
  add assertion

  Revision 1.22  05/21/2004 02:11:50  ida
  NNS_SndArcLoadFile add offset arg

  Revision 1.21  05/13/2004 02:24:19  ida
  Parameter structure  -> Information structure

  Revision 1.20  05/13/2004 01:02:21  ida
  merge seqarc.h and data.h to seqdata.h
  fixed comments

  Revision 1.19  05/12/2004 07:08:58  ida
  add check invalid info

  Revision 1.18  05/12/2004 06:39:58  ida
  modify data header structure

  Revision 1.17  05/11/2004 08:07:02  ida
  add assert signature

  Revision 1.16  04/30/2004 04:38:25  ida
  add NNS_SndArcGetPlayerInfo

  Revision 1.15  04/28/2004 07:47:16  ida
  fixed comments

  Revision 1.14  04/28/2004 01:25:27  ida
  add comments

  Revision 1.13  04/27/2004 01:49:40  ida
  for symbol table

  Revision 1.12  04/26/2004 02:57:17  ida
  for file system

  Revision 1.11  04/06/2004 23:54:22  ida
  NNS_Snd -> NNSi_Snd

  Revision 1.10  03/30/2004 06:43:20  ida
  change header

  Revision 1.9  03/10/2004 08:40:10  ida
  reconstruct data structure

  Revision 1.8  03/10/2004 06:36:08  ida
  (none)

  Revision 1.7  03/08/2004 06:59:05  ida
  remove OS_Printf

  Revision 1.6  02/20/2004 05:02:59  ida
  change directory

  Revision 1.3  02/19/2004 08:57:10  ida
  ARM7 -> SDK

  Revision 1.2  02/16/2004 06:22:07  ida
  (none)

  Revision 1.1  02/13/2004 01:42:05  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/sndarc.h>

/* if include from Other Environment for exsample VC or BCB,*/
/* please define NNS_FROM_TOOL*/
#ifndef NNS_FROM_TOOL

#include <nitro/snd.h>
#include <nnsys/misc.h>
#include <nnsys/snd/player.h>

#else

#define NNS_ASSERT(exp)           ((void) 0)
#define NNS_NULL_ASSERT(exp)           ((void) 0)
#define NNS_ALIGN4_ASSERT(exp)           ((void) 0)
#define NNS_MAX_ASSERT(exp, max)           ((void) 0)

#ifndef _MSC_VER
static inline
#else
__inline
#endif
void MI_CpuCopy32( register const void *srcp, register void *destp, register u32 size )
{
    const u32* sp = (const u32*)srcp;
    u32* dp = (u32*)destp;
    u32* dpe = (u32*)((char*)dp + size);
    
    while( dp < dpe ) {
        *dp++ = *sp++;
    }
}

#ifdef _MSC_VER
#pragma warning( disable : 4018 ) // warning: signed/unsigned mismatch
#pragma warning( disable : 4311 ) // warning: pointer truncation from 'type' to 'type'
#pragma warning( disable : 4312 ) // warning: conversion from 'type' to 'type' of greater size
#endif

#endif // NNS_FROM_TOOL

/******************************************************************************
    static variable
 ******************************************************************************/

static const char null_string = '\0';
static NNSSndArc* sCurrent = NULL;

/******************************************************************************
    static function declaration
 ******************************************************************************/

static const char* GetSymbol(
    const NNSSndArcOffsetTable* table,
    int index,
    const void* base
);
static void* GetPtr( void* base, u32 offset );
static const void* GetPtrConst( const void* base, u32 offset );
static const NNSSndArcOffsetTable* GetOffsetTable( const NNSSndArcInfo* info, u32 offset );
static void InfoDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );
static void FatDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );
static void SymbolDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 );

/******************************************************************************
    inline functions
 ******************************************************************************/

#ifndef _MSC_VER
static inline
#else
__inline
#endif
void* GetPtr( void* base, u32 offset )
{
    if ( offset == 0 ) return NULL;
    return (u8*)base + offset;
}

#ifndef _MSC_VER
static inline
#else
__inline
#endif
const void* GetPtrConst( const void* base, u32 offset )
{
    if ( offset == 0 ) return NULL;
    return (const u8*)base + offset;
}

#ifndef _MSC_VER
static inline
#else
__inline
#endif
const NNSSndArcOffsetTable* GetOffsetTable( const NNSSndArcInfo* info, u32 offset )
{
    return (const NNSSndArcOffsetTable*)GetPtrConst( info, offset );
}

/******************************************************************************
    public functions
 ******************************************************************************/

#ifndef SDK_SMALL_BUILD

#ifndef NNS_FROM_TOOL
/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcInit

  Description:  Initializes the Sound Archive structure

  Arguments:    arc            - Pointer to the Sound Archive structure
                filePath       - sound archive file path
                heap - Sound Heap
                symbolLoadFlag - Flag indicating whether to load the symbol data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndArcInit( NNSSndArc* arc, const char* filePath, NNSSndHeapHandle heap, BOOL symbolLoadFlag )
{
    BOOL result;
    
    NNS_ASSERT( FS_IsAvailable() );
    NNS_NULL_ASSERT( arc );
    
    /* Initialization of the NNSSndArc structure*/ 
    arc->info = NULL;
    arc->fat  = NULL;
    arc->symbol = NULL;    

    /* Get the file ID*/
    result = FS_ConvertPathToFileID( & arc->fileId, filePath );
    NNS_ASSERTMSG( result, "Cannot find file %s\n", filePath );
    if ( ! result ) return;
    
    /* Open the file*/
    FS_InitFile( & arc->file );
    result = FS_OpenFileFast( & arc->file, arc->fileId );
    NNS_ASSERTMSG( result, "Cannot open file %s\n", filePath );
    if ( ! result ) return;
    
    arc->file_open = TRUE;
    
    /* Setup*/
    result = NNS_SndArcSetup( arc, heap, symbolLoadFlag );
    NNS_ASSERT( result );
    if ( ! result ) return;
    
    /* Set as the current archive*/
    sCurrent = arc;    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcInitWithResult

  Description:  Initialize the sound archive structure (with return value)

  Arguments:    arc            - Pointer to the Sound Archive structure
                filePath       - sound archive file path
                heap - Sound Heap
                symbolLoadFlag - Flag indicating whether to load the symbol data

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcInitWithResult( NNSSndArc* arc, const char* filePath, NNSSndHeapHandle heap, BOOL symbolLoadFlag )
{
    BOOL result;
    
    NNS_ASSERT( FS_IsAvailable() );
    NNS_NULL_ASSERT( arc );
    
    /* Initialization of the NNSSndArc structure*/ 
    arc->info = NULL;
    arc->fat  = NULL;
    arc->symbol = NULL;    

    /* Get the file ID*/
    result = FS_ConvertPathToFileID( & arc->fileId, filePath );
    if ( ! result ) return FALSE;
    
    /* Open the file*/
    FS_InitFile( & arc->file );
    result = FS_OpenFileFast( & arc->file, arc->fileId );
    if ( ! result ) return FALSE;
    
    arc->file_open = TRUE;
    
    /* Setup*/
    result = NNS_SndArcSetup( arc, heap, symbolLoadFlag );
    if ( ! result ) return FALSE;
    
    /* Set as the current archive*/
    sCurrent = arc;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcSetup

  Description:  Sets up the Sound Archive structure

  Arguments:    arc            - Pointer to the Sound Archive structure
                heap - Sound Heap
                symbolLoadFlag - Flag indicating whether to load the symbol data

  Returns:      success or failure
 *---------------------------------------------------------------------------*/
BOOL NNS_SndArcSetup( NNSSndArc* arc, NNSSndHeapHandle heap, BOOL symbolLoadFlag )
{
    BOOL result;
    s32 readSize;
    
    NNS_NULL_ASSERT( arc );
    NNS_ASSERT( arc->file_open );
    
    /* Load header*/
    result = FS_SeekFile( & arc->file, 0, FS_SEEK_SET );
    if ( ! result ) return FALSE;
    
    readSize = FS_ReadFile(
        & arc->file,
        & arc->header,
        sizeof( arc->header )
    );
    if ( readSize != sizeof( arc->header ) ) return FALSE;
    NNS_ASSERT(
        arc->header.fileHeader.signature[0] == 'S' &&
        arc->header.fileHeader.signature[1] == 'D' &&
        arc->header.fileHeader.signature[2] == 'A' &&
        arc->header.fileHeader.signature[3] == 'T' 
    );
    NNS_ASSERT( arc->header.fileHeader.version >= NNS_SND_ARC_SUPPORTED_FILE_VERSION );
    
    if ( heap != NNS_SND_HEAP_INVALID_HANDLE )
    {
        /* Load sound information table*/
        arc->info = (NNSSndArcInfo*)NNS_SndHeapAlloc( heap, arc->header.infoSize, InfoDisposeCallback, (u32)arc, 0 );
        if ( arc->info == NULL ) return FALSE;
        result = FS_SeekFile( & arc->file, (s32)( arc->header.infoOffset ), FS_SEEK_SET );
        if ( ! result ) return FALSE;
        readSize = FS_ReadFile( & arc->file, arc->info, (s32)( arc->header.infoSize ) );
        if ( readSize != arc->header.infoSize ) return FALSE;
        NNS_ASSERT( arc->info->blockHeader.kind == NNS_SND_ARC_SIGNATURE_INFO );
        
        /* Load file allocation table*/
        arc->fat = (NNSSndArcFat*)NNS_SndHeapAlloc( heap, arc->header.fatSize, FatDisposeCallback, (u32)arc, 0 );
        if ( arc->fat == NULL ) return FALSE;
        result = FS_SeekFile( & arc->file, (s32)( arc->header.fatOffset ), FS_SEEK_SET );
        if ( ! result ) return FALSE;
        readSize = FS_ReadFile( & arc->file, arc->fat, (s32)( arc->header.fatSize ) );
        if ( readSize != arc->header.fatSize ) return FALSE;
        NNS_ASSERT( arc->fat->blockHeader.kind == NNS_SND_ARC_SIGNATURE_FAT );
        
        /* Load symbol table*/
        if ( symbolLoadFlag && arc->header.symbolDataSize > 0 )
        {
            arc->symbol = (NNSSndArcSymbol*)NNS_SndHeapAlloc( heap, arc->header.symbolDataSize, SymbolDisposeCallback, (u32)arc, 0 );
            if ( arc->symbol == NULL ) return FALSE;
            result = FS_SeekFile( & arc->file, (s32)( arc->header.symbolDataOffset ), FS_SEEK_SET );
            if ( ! result ) return FALSE;
            
            readSize = FS_ReadFile( & arc->file, arc->symbol, (s32)( arc->header.symbolDataSize ) );
            if ( readSize != arc->header.symbolDataSize ) return FALSE;
            NNS_ASSERT( arc->symbol->blockHeader.kind == NNS_SND_ARC_SIGNATURE_SYMB );
        }
    }
    
    return TRUE;
}
#endif // NNS_FROM_TOOL

#endif /* SDK_SMALL_BUILD*/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcInitOnMemory

  Description:  Initialize the Sound Archive structure using the
                Initializes the Sound Archive structure

  Arguments:    arc            - Pointer to the Sound Archive structure
                data - Pointer to the Sound Archive data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndArcInitOnMemory( NNSSndArc* arc, void* data )
{
    NNSSndArcHeader* header = (NNSSndArcHeader*)data;
    NNSSndArcFileInfo* file;
    int fileId;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( data );
    NNS_ALIGN4_ASSERT( data );
    
    MI_CpuCopy32( header, &arc->header, sizeof( arc->header ) );

    NNS_ASSERT(
        arc->header.fileHeader.signature[0] == 'S' &&
        arc->header.fileHeader.signature[1] == 'D' &&
        arc->header.fileHeader.signature[2] == 'A' &&
        arc->header.fileHeader.signature[3] == 'T' 
    );
    NNS_ASSERT( arc->header.fileHeader.version >= NNS_SND_ARC_SUPPORTED_FILE_VERSION );
    
    arc->info   = (NNSSndArcInfo*)GetPtr( header, arc->header.infoOffset );
    arc->fat    = (NNSSndArcFat*)GetPtr( header, arc->header.fatOffset );
    arc->symbol = (NNSSndArcSymbol*)GetPtr( header, arc->header.symbolDataOffset );
    
    NNS_NULL_ASSERT( arc->info );
    NNS_NULL_ASSERT( arc->fat );
    
    NNS_ASSERT( arc->info->blockHeader.kind == NNS_SND_ARC_SIGNATURE_INFO );
    NNS_ASSERT( arc->fat->blockHeader.kind  == NNS_SND_ARC_SIGNATURE_FAT );
    NNS_ASSERT( arc->symbol == NULL || arc->symbol->blockHeader.kind == NNS_SND_ARC_SIGNATURE_SYMB );
    
    for( fileId = 0; fileId < arc->fat->count ; fileId ++ )
    {
        file = & arc->fat->files[ fileId ];
        
        file->mem = GetPtr( header, file->offset );
        NNS_NULL_ASSERT( file->mem );
    }
    
    arc->file_open = FALSE;
    
    /* Set as the current archive*/
    sCurrent = arc;    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcSetCurrent

  Description:  Sets up current sound archive

  Arguments:    arc - The Sound Archive to be set

  Returns:      The current sound archive prior to setting
 *---------------------------------------------------------------------------*/
NNSSndArc* NNS_SndArcSetCurrent( NNSSndArc* arc )
{
    NNSSndArc* oldArc = sCurrent;
    sCurrent = arc;
    return oldArc;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetCurrent

  Description:  Gets current sound archive

  Arguments:    None.

  Returns:      Current sound archive
 *---------------------------------------------------------------------------*/
NNSSndArc* NNS_SndArcGetCurrent( void )
{
    return sCurrent;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqParam

  Description:  Gets sequence parameter

  Arguments:    seqNo - Sequence number

  Returns:      Sequence parameter structure
 *---------------------------------------------------------------------------*/
const NNSSndSeqParam* NNS_SndArcGetSeqParam( int seqNo )
{
    const NNSSndArcSeqInfo* info;

    info = NNS_SndArcGetSeqInfo( seqNo );
    if ( info == NULL ) return NULL;
    
    return & info->param;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqArcParam

  Description:  Get sequence archive parameter

  Arguments:    seqArcNo - sequence archive number
                index    - sequence archive index

  Returns:      Sequence parameter structure
 *---------------------------------------------------------------------------*/
const NNSSndSeqParam* NNS_SndArcGetSeqArcParam( int seqArcNo, int index )
{
    const NNSSndArcSeqArcInfo* info;
    const NNSSndSeqArc* seqArc;
    const NNSSndSeqArcSeqInfo* sound;
    
    info = NNS_SndArcGetSeqArcInfo( seqArcNo );
    if ( info == NULL ) return NULL;
    
    seqArc = (const NNSSndSeqArc*)NNS_SndArcGetFileAddress( info->fileId );
    if ( seqArc == NULL ) return NULL;
    
    sound = NNSi_SndSeqArcGetSeqInfo( seqArc, index );
    if ( sound == NULL ) return NULL;

    return & sound->param;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqInfo

  Description:  Get sequence information

  Arguments:    seqNo - Sequence number

  Returns:      Pointer to the  sequence information structure;
                NULL if process fails
 *---------------------------------------------------------------------------*/
const NNSSndArcSeqInfo* NNS_SndArcGetSeqInfo( int seqNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->seqOffset );
    if ( table == NULL ) return NULL;
    
    if ( seqNo < 0 ) return NULL;
    if ( seqNo >= table->count ) return NULL;
    
    return (const NNSSndArcSeqInfo*)GetPtrConst( arc->info, table->offset[ seqNo ] );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqArcInfo

  Description:  Get sequence archive information

  Arguments:    seqArcNo - sequence archive number

  Returns:      Pointer to the sequence archive information structure
                NULL if process fails
 *---------------------------------------------------------------------------*/
const NNSSndArcSeqArcInfo* NNS_SndArcGetSeqArcInfo( int seqArcNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->seqArcOffset );
    if ( table == NULL ) return NULL;
    
    if ( seqArcNo < 0 ) return NULL;
    if ( seqArcNo >= table->count ) return NULL;
    
    return (const NNSSndArcSeqArcInfo*)GetPtrConst( arc->info, table->offset[ seqArcNo ] );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetBankInfo

  Description:  Get bank information

  Arguments:    bankNo - Bank number

  Returns:      Pointer to the  bank information structure
                NULL if process fails
 *---------------------------------------------------------------------------*/
const NNSSndArcBankInfo* NNS_SndArcGetBankInfo( int bankNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->bankOffset );
    if ( table == NULL ) return NULL;
    
    if ( bankNo < 0 ) return NULL;
    if ( bankNo >= table->count ) return NULL;
    
    return (const NNSSndArcBankInfo*)GetPtrConst( arc->info, table->offset[ bankNo ] );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetWaveArcInfo

  Description:  Gets the waveform archive information

  Arguments:    waveArcNo - waveform archive number

  Returns:      Pointer to the  waveform archive information structure;
                NULL if process fails
 *---------------------------------------------------------------------------*/
const NNSSndArcWaveArcInfo* NNS_SndArcGetWaveArcInfo( int waveArcNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->waveArcOffset );
    if ( table == NULL ) return NULL;
    
    if ( waveArcNo < 0 ) return NULL;
    if ( waveArcNo >= table->count ) return NULL;
    
    return (const NNSSndArcWaveArcInfo*)GetPtrConst( arc->info, table->offset[ waveArcNo ] );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetStrmInfo

  Description:  gets stream information

  Arguments:    strmNo - stream number

  Returns:      pointer to stream information structure
                NULL if process fails
 *---------------------------------------------------------------------------*/
const NNSSndArcStrmInfo* NNS_SndArcGetStrmInfo( int strmNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->strmOffset );
    if ( table == NULL ) return NULL;
    
    if ( strmNo < 0 ) return NULL;
    if ( strmNo >= table->count ) return NULL;
    
    return (const NNSSndArcStrmInfo*)GetPtrConst( arc->info, table->offset[ strmNo ] );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetPlayerInfo

  Description:  Gets player information

  Arguments:    playerNo - Player number

  Returns:      Pointer to the  player information structure
                NULL if process fails
 *---------------------------------------------------------------------------*/
const NNSSndArcPlayerInfo* NNS_SndArcGetPlayerInfo( int playerNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->playerInfoOffset );
    if ( table == NULL ) return NULL;
    
    if ( playerNo < 0 ) return NULL;
    if ( playerNo >= table->count ) return NULL;
    
    return (const NNSSndArcPlayerInfo*)GetPtrConst( arc->info, table->offset[ playerNo ] );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetStrmPlayerInfo

  Description:  gets stream player information

  Arguments:    playerNo - Player number

  Returns:      pointer to stream player information structure
                NULL if process fails
 *---------------------------------------------------------------------------*/
const NNSSndArcStrmPlayerInfo* NNS_SndArcGetStrmPlayerInfo( int playerNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->strmPlayerInfoOffset );
    if ( table == NULL ) return NULL;
    
    if ( playerNo < 0 ) return NULL;
    if ( playerNo >= table->count ) return NULL;
    
    return (const NNSSndArcStrmPlayerInfo*)GetPtrConst( arc->info, table->offset[ playerNo ] );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetGroupInfo

  Description:  Get group information

  Arguments:    groupNo - Group number

  Returns:      Pointer to the  group information structure
                NULL if process fails
 *---------------------------------------------------------------------------*/
const NNSSndArcGroupInfo* NNS_SndArcGetGroupInfo( int groupNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->groupInfoOffset );
    if ( table == NULL ) return NULL;
    
    if ( groupNo < 0 ) return NULL;
    if ( groupNo >= table->count ) return NULL;
    
    return (const NNSSndArcGroupInfo*)GetPtrConst( arc->info, table->offset[ groupNo ] );
}


/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqCount

  Description:  Gets the number of sets of sequence data

  Arguments:    None.

  Returns:      Number of sets of sequence data
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetSeqCount( void )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->seqOffset );
    if ( table == NULL ) return 0;
    
    return table->count ;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqArcCount

  Description:  Gets the number of sets of sequence archive data

  Arguments:    None.

  Returns:      Number of sets of sequence archive data
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetSeqArcCount( void )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->seqArcOffset );
    if ( table == NULL ) return 0;
    
    return table->count ;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetBankCount

  Description:  Gets the number of sets of bank data

  Arguments:    None.

  Returns:      Number of sets of bank data
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetBankCount( void )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->bankOffset );
    if ( table == NULL ) return 0;
    
    return table->count ;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetWaveArcCount

  Description:  gets wave archive data count

  Arguments:    None.

  Returns:      wave archive data count
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetWaveArcCount( void )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->waveArcOffset );
    if ( table == NULL ) return 0;
    
    return table->count ;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetStrmCount

  Description:  gets stream data count

  Arguments:    None.

  Returns:      stream data count
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetStrmCount( void )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->strmOffset );
    if ( table == NULL ) return 0;
    
    return table->count ;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetGroupCount

  Description:  Gets the number of sets of group information

  Arguments:    None.

  Returns:      Number of sets of group information
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetGroupCount( void )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->info );
    
    table = GetOffsetTable( arc->info, arc->info->groupInfoOffset );
    if ( table == NULL ) return 0;
    
    return table->count ;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqArcSeqCount

  Description:  Gets the number of sequences in the sequence archive.

  Arguments:    seqArcNo - sequence archive number

  Returns:      number of sequences
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetSeqArcSeqCount( int seqArcNo )
{
    const NNSSndArcSeqArcInfo* info;
    const NNSSndSeqArc* seqArc;
    
    info = NNS_SndArcGetSeqArcInfo( seqArcNo );
    if ( info == NULL ) return 0;
    
    seqArc = (const NNSSndSeqArc*)NNS_SndArcGetFileAddress( info->fileId );
    if ( seqArc == NULL ) return 0;
    
    return NNSi_SndSeqArcGetSeqCount( seqArc );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetFileOffset

  Description:  gets file offset from file ID

  Arguments:    fileId - file ID

  Returns:      file offset
                returns 0 if it fails
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetFileOffset( u32 fileId )
{
    NNSSndArc* arc = sCurrent;
    NNS_NULL_ASSERT( arc );    
    NNS_NULL_ASSERT( arc->fat );
    
    if ( fileId >= arc->fat->count ) return 0;
    return arc->fat->files[ fileId ].offset;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetFileSize

  Description:  Gets the file size from the file ID

  Arguments:    fileId - file ID

  Returns:      File size
                returns 0 if it fails
 *---------------------------------------------------------------------------*/
u32 NNS_SndArcGetFileSize( u32 fileId )
{
    NNSSndArc* arc = sCurrent;
    NNS_NULL_ASSERT( arc );    
    NNS_NULL_ASSERT( arc->fat );
    
    if ( fileId >= arc->fat->count ) return 0;
    return arc->fat->files[ fileId ].size;
}

/* if include from Other Environment for exsample VC or BCB,*/
/* please define NNS_FROM_TOOL*/
#ifndef NNS_FROM_TOOL
/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcReadFile

  Description:  Reads file into memory

  Arguments:    fileId - file ID
                buffer - Starting address of buffer to load
                size   - Size of buffer to load
                offset - Read offset

  Returns:      The read-in size
                   -1 if process fails
 *---------------------------------------------------------------------------*/
s32 NNS_SndArcReadFile( u32 fileId, void* buffer, s32 size, s32 offset )
{
#ifndef SDK_SMALL_BUILD
    
    NNSSndArc* arc = sCurrent;
    const NNSSndArcFileInfo* file;
    s32 readSize;
    
    NNS_ASSERT( FS_IsAvailable() );
    NNS_NULL_ASSERT( arc );    
    NNS_NULL_ASSERT( arc->fat );
    NNS_ASSERTMSG( arc->file_open, "Cannot use this function for the Sound Archive initialized by NNS_SndArcInitOnMemory\n" );
    
    if ( fileId >= arc->fat->count ) return -1;
    file = & arc->fat->files[ fileId ];
    
    if ( size > file->size - offset ) size = (s32)( file->size - offset );
    
    if ( ! FS_SeekFile( & arc->file, (s32)( file->offset + offset ), FS_SEEK_SET ) ) {
        return -1;
    }
    
    readSize = FS_ReadFile( & arc->file, buffer, size );
    
    return readSize;
    
#else  /* SDK_SMALL_BUILD*/
    
#pragma unused( fileId, buffer, size, offset )
    return -1;
    
#endif /* SDK_SMALL_BUILD*/
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetFileID

  Description:  gets sound archive file ID

  Arguments:    None.

  Returns:      file ID
 *---------------------------------------------------------------------------*/
FSFileID NNS_SndArcGetFileID( void )
{
    NNSSndArc* arc = sCurrent;
    
    NNS_NULL_ASSERT( arc );    
    NNS_NULL_ASSERT( arc->fat );
    NNS_ASSERTMSG( arc->file_open, "Cannot use this function for the Sound Archive initialized by NNS_SndArcInitOnMemory\n" );
    
    return arc->fileId;
}

#endif // NNS_FROM_TOOL


/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetFileAddress

  Description:  Gets the stored file address

  Arguments:    fileId - file ID

  Returns:      File address
 *---------------------------------------------------------------------------*/
void* NNS_SndArcGetFileAddress( u32 fileId )
{
    NNSSndArc* arc = sCurrent;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->fat );
    
    if ( fileId >= arc->fat->count ) return NULL;
    return arc->fat->files[ fileId ].mem;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcSetFileAddress

  Description:  Stores the file address

  Arguments:    fileId - file ID
                address - File Address

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndArcSetFileAddress( u32 fileId, void* address )
{
    NNSSndArc* arc = sCurrent;

    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->fat );    
    NNS_MAX_ASSERT( fileId, arc->fat->count - 1 );
    
    arc->fat->files[ fileId ].mem = address;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqSymbol

  Description:  Gets sequence symbol

  Arguments:    seqNo - Sequence number

  Returns:      Sequence symbol character-string
                If process fails, empty string
 *---------------------------------------------------------------------------*/
const char* NNS_SndArcGetSeqSymbol( int seqNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    
    if ( arc->symbol == NULL ) return & null_string;
    
    table = (const NNSSndArcOffsetTable*)GetPtrConst( arc->symbol, arc->symbol->seqOffset );
    if ( table == NULL ) return & null_string;
    
    return GetSymbol( table, seqNo, arc->symbol );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqArcSymbol

  Description:  Gets sequence archive symbol

  Arguments:    seqArcNo - sequence archive number

  Returns:      Sequence archive symbol character-string
                If process fails, empty string
 *---------------------------------------------------------------------------*/
const char* NNS_SndArcGetSeqArcSymbol( int seqArcNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcSeqArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    
    if ( arc->symbol == NULL ) return & null_string;
    
    table = (const NNSSndArcSeqArcOffsetTable*)GetPtrConst( arc->symbol, arc->symbol->seqArcOffset );
    if ( table == NULL ) return & null_string;
    
    if ( seqArcNo < 0 ) return & null_string;
    if ( seqArcNo >= table->count ) return & null_string;
    
    if ( table->offset[ seqArcNo ].symbol == 0 ) return & null_string;
    
    return (const char*)GetPtrConst( arc->symbol, table->offset[ seqArcNo ].symbol );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetSeqArcIdxSymbol

  Description:  Gets sequence archive index symbol

  Arguments:    seqArcNo - sequence archive number
                index    - Index number

  Returns:      Sequence archive index symbol character-string
                If process fails, empty string
 *---------------------------------------------------------------------------*/
const char* NNS_SndArcGetSeqArcIdxSymbol( int seqArcNo, int index )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcSeqArcOffsetTable* table;
    const NNSSndArcOffsetTable* symbolTable;
    
    NNS_NULL_ASSERT( arc );
    
    if ( arc->symbol == NULL ) return & null_string;
    
    table = (const NNSSndArcSeqArcOffsetTable*)GetPtrConst( arc->symbol, arc->symbol->seqArcOffset );
    if ( table == NULL ) return & null_string;
    
    if ( seqArcNo < 0 ) return & null_string;
    if ( seqArcNo >= table->count ) return & null_string;
    
    symbolTable = (const NNSSndArcOffsetTable*)GetPtrConst( arc->symbol, table->offset[ seqArcNo ].table );
    if ( symbolTable == NULL ) return & null_string;
    
    return GetSymbol( symbolTable, index, arc->symbol );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetBankSymbol

  Description:  Gets bank symbol

  Arguments:    bankNo - Bank number

  Returns:      Bank symbol character-string
                If process fails, empty string
 *---------------------------------------------------------------------------*/
const char* NNS_SndArcGetBankSymbol( int bankNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    
    if ( arc->symbol == NULL ) return & null_string;

    table = (const NNSSndArcOffsetTable*)GetPtrConst( arc->symbol, arc->symbol->bankOffset );
    if ( table == NULL ) return & null_string;
    
    return GetSymbol( table, bankNo, arc->symbol );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetWaveArcSymbol

  Description:  Gets waveform archive symbol

  Arguments:    waveArcNo - waveform archive number

  Returns:      Waveform archive symbol character-string
                If process fails, empty string
 *---------------------------------------------------------------------------*/
const char* NNS_SndArcGetWaveArcSymbol( int waveArcNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    
    if ( arc->symbol == NULL ) return & null_string;

    table = (const NNSSndArcOffsetTable*)GetPtrConst( arc->symbol, arc->symbol->waveArcOffset );
    if ( table == NULL ) return & null_string;
    
    return GetSymbol( table, waveArcNo, arc->symbol );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetStrmSymbol

  Description:  gets stream symbol

  Arguments:    strmNo - stream number

  Returns:      stream symbol string
                If process fails, empty string
 *---------------------------------------------------------------------------*/
const char* NNS_SndArcGetStrmSymbol( int strmNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    
    if ( arc->symbol == NULL ) return & null_string;
    
    table = (const NNSSndArcOffsetTable*)GetPtrConst( arc->symbol, arc->symbol->strmOffset );
    if ( table == NULL ) return & null_string;
    
    return GetSymbol( table, strmNo, arc->symbol );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndArcGetGroupSymbol

  Description:  Gets group symbol

  Arguments:    groupNo - Group number

  Returns:      Waveform archive symbol character-string
                If process fails, empty string
 *---------------------------------------------------------------------------*/
const char* NNS_SndArcGetGroupSymbol( int groupNo )
{
    NNSSndArc* arc = sCurrent;
    const NNSSndArcOffsetTable* table;
    
    NNS_NULL_ASSERT( arc );
    
    if ( arc->symbol == NULL ) return & null_string;
    
    table = (const NNSSndArcOffsetTable*)GetPtrConst( arc->symbol, arc->symbol->groupOffset );
    if ( table == NULL ) return & null_string;
    
    return GetSymbol( table, groupNo, arc->symbol );
}


/******************************************************************************
    static functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         GetSymbol

  Description:  Gets symbol from symbol offset table

  Arguments:    table - Pointer to symbol offset table
                index    - Index number
                base  - Offset base address

  Returns:      Symbol character-string
                If process fails, empty string
 *---------------------------------------------------------------------------*/
static const char* GetSymbol( const NNSSndArcOffsetTable* table, int index, const void* base )
{
    if ( index < 0 ) return & null_string;
    if ( index >= table->count ) return & null_string;
    
    if ( table->offset[ index ] == 0 ) return & null_string;
    
    return (const char*)GetPtrConst( base, table->offset[ index ] );
}

/*---------------------------------------------------------------------------*
  Name:         InfoDisposeCallback

  Description:  Callback function that gets called when sound information table is destroyed

  Arguments:    mem   - Starting address of memory block (Not used)
                size  - Size of memory block (Not used)
                data1 - User data (pointer to sound archive)
                data2 - User data (Not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InfoDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 )
{
    NNSSndArc* arc = (NNSSndArc*)data1;
    
    (void)mem;
    (void)size;
    (void)data2;
    
    NNS_NULL_ASSERT( arc );
    
    arc->info = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         FatDisposeCallback

  Description:  Callback function that gets called when file allocation table is destroyed

  Arguments:    mem   - Starting address of memory block (Not used)
                size  - Size of memory block (Not used)
                data1 - User data (pointer to sound archive)
                data2 - User data (Not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FatDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 )
{
    NNSSndArc* arc = (NNSSndArc*)data1;
    int i;
    
    (void)mem;
    (void)size;
    (void)data2;
    
    NNS_NULL_ASSERT( arc );
    NNS_NULL_ASSERT( arc->fat );
    
    for( i = 0; i < arc->fat->count ; i++ ) {
#ifndef NNS_FROM_TOOL
        NNS_ASSERTMSG(
            arc->fat->files[ i ].mem == NULL,
            "Cannot clear sndarc FAT block, because some file is on memory."
        );
#endif // NNS_FROM_TOOL
    }
    
    arc->fat = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         SymbolDisposeCallback

  Description:  Callback function that gets called when symbol data is destroyed

  Arguments:    mem   - Starting address of memory block (Not used)
                size  - Size of memory block (Not used)
                data1 - User data (pointer to sound archive)
                data2 - User data (Not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SymbolDisposeCallback( void* mem, u32 size, u32 data1, u32 data2 )
{
    NNSSndArc* arc = (NNSSndArc*)data1;

    (void)mem;
    (void)size;
    (void)data2;
    
    NNS_NULL_ASSERT( arc );

    arc->symbol = NULL;
}
    
/*====== End of sndarc.c ======*/


