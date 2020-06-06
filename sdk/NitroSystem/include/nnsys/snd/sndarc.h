/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     sndarc.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sndarc.h,v $
  Revision 1.46  08/18/2005 07:04:14  ida
  Added the function NNS_SndArcInitWithResult

  Revision 1.45  03/08/2005 08:37:07  ida
  Added file_open flag

  Revision 1.44  03/08/2005 05:40:26  ida
  Support for NNS_FROM_TOOL

  Revision 1.43  01/19/2005 06:23:33  ida
  Added flags member to NNSSndArcWaveArcInfo

  Revision 1.42  01/06/2005 04:07:49  ida
  Updated copyright year notation

  Revision 1.41  01/05/2005 07:08:12  ida
  Added allocChBitFlag member

  Revision 1.40  11/25/2004 05:35:52  ida
  using NNS_FROM_TOOL

  Revision 1.39  11/22/2004 04:29:30  ida
  added NNS_SND_ARC_STRM_FORCE_STEREO macro

  Revision 1.38  11/16/2004 08:45:24  ida
  for STEREO_F type

  Revision 1.37  11/01/2004 08:50:12  ida
  NNS_SND_ARC_LOAD_* macro move from sndarc_loader.h to sndarc.h

  Revision 1.36  09/13/2004 05:26:03  ida
  add NNS_SndArcGetSeqArcSeqCount

  Revision 1.35  09/06/2004 07:26:22  ida
  for SDK_FROM_TOOL

  Revision 1.34  08/23/2004 01:38:09  ida
  add NNS_SndArcSetup

  Revision 1.33  08/02/2004 05:38:09  ida
  add stream info

  Revision 1.32  07/16/2004 06:42:58  ida
  fix Project name

  Revision 1.31  07/06/2004 07:46:44  ida
  workaround multi wave archive for one bank

  Revision 1.30  06/08/2004 00:51:32  ida
  changed interface

  Revision 1.29  05/28/2004 01:58:38  ida
  add NNSSndArcGroupItem::mask

  Revision 1.28  05/27/2004 06:35:15  ida
  add NNS_SndArcGetGroupInfo

  Revision 1.27  05/26/2004 02:25:56  ida
  add NNS_SndArcInitOnMemory

  Revision 1.26  05/24/2004 05:42:37  ida
  fix wrong argument name

  Revision 1.25  05/21/2004 02:49:51  ida
  reserved file loading flag

  Revision 1.24  05/21/2004 02:13:18  ida
  NNS_SndArcGetSymbolTableSize -> SymbolDataSize

  Revision 1.23  05/13/2004 02:21:52  ida
  warcNo -> waveArcNo

  Revision 1.22  05/13/2004 01:04:27  ida
  modify sndarc file header

  Revision 1.21  05/12/2004 06:39:58  ida
  modify data header structure

  Revision 1.20  05/11/2004 08:12:48  ida
  for new FS library

  Revision 1.19  04/30/2004 04:38:25  ida
  add NNS_SndArcGetPlayerInfo

  Revision 1.18  04/28/2004 07:47:26  ida
  fixed comments

  Revision 1.17  04/28/2004 01:55:49  ida
  add comments

  Revision 1.16  04/28/2004 01:25:27  ida
  add comments

  Revision 1.15  04/27/2004 07:51:14  ida
  seqarc offset to u32

  Revision 1.14  04/27/2004 04:30:36  ida
  add NNS_SND_ARC_INVALID_FILE_ID

  Revision 1.13  04/27/2004 01:47:58  ida
  add symbol table

  Revision 1.12  04/26/2004 02:24:04  ida
  for file system

  Revision 1.11  04/06/2004 23:54:22  ida
  NNS_Snd -> NNSi_Snd

  Revision 1.10  04/02/2004 06:53:58  ida
  add warning message

  Revision 1.9  03/30/2004 06:44:19  ida
  change header

  Revision 1.8  03/22/2004 07:25:01  ida
  use NNSSndSeqParam

  Revision 1.7  03/10/2004 08:40:10  ida
  reconstruct data structure

  Revision 1.6  03/10/2004 06:36:08  ida
  (none)

  Revision 1.5  02/20/2004 05:03:08  ida
  change directory

  Revision 1.3  02/19/2004 08:57:14  ida
  ARM7 -> SDK

  Revision 1.2  02/16/2004 06:22:13  ida
  (none)

  Revision 1.1  02/13/2004 01:44:09  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_SNDARC_H_
#define NNS_SND_SNDARC_H_

#include <nitro/types.h>
#include <nitro/snd.h>      // for SNDBinaryFileHeader
#include <nnsys/snd/seqdata.h>  // for NNSSndSeqParam

/* if include from Other Environment for exsample VC or BCB,*/
/* please define NNS_FROM_TOOL*/
#ifndef NNS_FROM_TOOL

#include <nitro/os.h>       // for OSMutex
#include <nitro/fs.h>       // for FSFile and FSFileID
#include <nnsys/snd/heap.h>     // for NNSSndHeapHandle

#endif // NNS_FROM_TOOL

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
    macro definition
 ******************************************************************************/

#define NNS_SND_ARC_SUPPORTED_FILE_VERSION           0x0006

#define NNS_SND_ARC_LOAD_SEQ    ( 1 << 0 )
#define NNS_SND_ARC_LOAD_BANK   ( 1 << 1 )
#define NNS_SND_ARC_LOAD_WAVE   ( 1 << 2 )
#define NNS_SND_ARC_LOAD_SEQARC ( 1 << 3 )
#define NNS_SND_ARC_LOAD_ALL   0xff

#define NNS_SND_ARC_SIGNATURE_HEAD 'TADS'
#define NNS_SND_ARC_SIGNATURE_INFO 'OFNI'
#define NNS_SND_ARC_SIGNATURE_FAT  ' TAF'
#define NNS_SND_ARC_SIGNATURE_SYMB 'BMYS'

#define NNS_SND_ARC_INVALID_FILE_ID 0xffffffff

#define NNS_SND_ARC_INVALID_WAVEARC_NO 0xffff

#define NNS_SND_ARC_BANK_TO_WAVEARC_NUM 4


#define NNS_SND_ARC_STRM_FORCE_STEREO ( 1 << 0 )

#define NNS_SND_ARC_WAVEARC_SINGLE_LOAD ( 1 << 0 )

/******************************************************************************
    enum definition
 ******************************************************************************/

typedef enum NNSSndArcSndType {    
    NNS_SNDARC_SNDTYPE_SEQ,
    NNS_SNDARC_SNDTYPE_BANK,
    NNS_SNDARC_SNDTYPE_WAVEARC,
    NNS_SNDARC_SNDTYPE_SEQARC,
    
    NNS_SNDARC_SNDTYPE_INVALID = 0xff
} NNSSndArcSndType;

/******************************************************************************
    sound archive internal structure
 ******************************************************************************/

#ifdef _MSC_VER
#pragma warning( disable : 4200 ) // warning: zero-sized array in struct/union
#endif

//-----------------------------------------------------------------------------
// info structures

typedef struct NNSSndArcSeqInfo
{
    u32 fileId;
    struct NNSSndSeqParam param;
} NNSSndArcSeqInfo;

typedef struct NNSSndArcSeqArcInfo
{
    u32 fileId;
} NNSSndArcSeqArcInfo;

typedef struct NNSSndArcBankInfo
{
    u32 fileId;
    u16 waveArcNo[ NNS_SND_ARC_BANK_TO_WAVEARC_NUM ];
} NNSSndArcBankInfo;

typedef struct NNSSndArcWaveArcInfo
{
    u32 fileId : 24;
    u32 flags  :  8;
} NNSSndArcWaveArcInfo;

typedef struct NNSSndArcStrmInfo
{
    u32 fileId;
    u8 volume;
    u8 playerPrio;
    u8 playerNo;
    u8 flags; 
} NNSSndArcStrmInfo;

typedef struct NNSSndArcPlayerInfo
{
    u8 seqMax;
    u8 padding;
    u16 allocChBitFlag;
    u32 heapSize;
} NNSSndArcPlayerInfo;

typedef struct NNSSndArcStrmPlayerInfo
{
    u8 numChannels;
    u8 chNoList[ 2 ];
} NNSSndArcStrmPlayerInfo;

typedef struct NNSSndArcGroupItem
{
    u8 type;
    u8 loadFlag;
    u16 padding;
    u32 index;
} NNSSndArcGroupItem;

typedef struct NNSSndArcGroupInfo
{
    u32 count;
    struct NNSSndArcGroupItem item[ 0 ];
} NNSSndArcGroupInfo;

//-----------------------------------------------------------------------------
// symbol structures

typedef struct NNSSndArcSeqArcOffset
{
    u32 symbol;
    u32 table;
} NNSSndArcSeqArcOffset;

typedef struct NNSSndArcSeqArcOffsetTable
{
    u32 count;
    NNSSndArcSeqArcOffset offset[ 0 ];
} NNSSndArcSeqArcOffsetTable;

typedef struct NNSSndArcOffsetTable
{
    u32 count;
    u32 offset[ 0 ];
} NNSSndArcOffsetTable;

//-----------------------------------------------------------------------------
//  FAT structures

typedef struct NNSSndArcFileInfo
{
    u32 offset;
    u32 size;
    void* mem;
    u32 reserved;
} NNSSndArcFileInfo;

/******************************************************************************
    sound archive block structure
 ******************************************************************************/

typedef struct NNSSndArcFat
{
    struct SNDBinaryBlockHeader blockHeader;
    
    u32 count;
    NNSSndArcFileInfo files[0];
} NNSSndArcFat;

typedef struct NNSSndArcInfo
{
    struct SNDBinaryBlockHeader blockHeader;
    
    u32 seqOffset;
    u32 seqArcOffset;
    u32 bankOffset;
    u32 waveArcOffset;
    u32 playerInfoOffset;
    u32 groupInfoOffset;
    u32 strmPlayerInfoOffset;
    u32 strmOffset;
} NNSSndArcInfo;

typedef struct NNSSndArcSymbol
{
    struct SNDBinaryBlockHeader blockHeader;
    
    u32 seqOffset;
    u32 seqArcOffset;
    u32 bankOffset;
    u32 waveArcOffset;
    u32 playerOffset;
    u32 groupOffset;
    u32 strmPlayerOffset;
    u32 strmOffset;
} NNSSndArcSymbol;

/******************************************************************************
    sound archive header
 ******************************************************************************/

typedef struct NNSSndArcHeader
{
    struct SNDBinaryFileHeader fileHeader;
    u32 symbolDataOffset;
    u32 symbolDataSize;
    u32 infoOffset;
    u32 infoSize;
    u32 fatOffset;
    u32 fatSize;
    u32 fileImageOffset;
    u32 fileImageSize;
} NNSSndArcHeader;

#ifdef _MSC_VER
#pragma warning( default : 4200 ) // warning: zero-sized array in struct/union
#endif

/******************************************************************************
    sound archive structure
 ******************************************************************************/

typedef struct NNSSndArc
{
    NNSSndArcHeader header;
    BOOL file_open;
#ifndef NNS_FROM_TOOL
    FSFile file;
    FSFileID fileId;
#endif // NNS_FROM_TOOL
    struct NNSSndArcFat* fat;
    struct NNSSndArcSymbol* symbol;    
    struct NNSSndArcInfo* info;
} NNSSndArc;


/******************************************************************************
    public function declaration
 ******************************************************************************/

#ifndef NNS_FROM_TOOL
void NNS_SndArcInit( NNSSndArc* arc, const char* filePath, NNSSndHeapHandle heap, BOOL symbolLoadFlag );
BOOL NNS_SndArcInitWithResult( NNSSndArc* arc, const char* filePath, NNSSndHeapHandle heap, BOOL symbolLoadFlag );
BOOL NNS_SndArcSetup( NNSSndArc* arc, NNSSndHeapHandle heap, BOOL symbolLoadFlag );
#endif // NNS_FROM_TOOL
void NNS_SndArcInitOnMemory( NNSSndArc* arc, void* data );

NNSSndArc* NNS_SndArcSetCurrent( NNSSndArc* arc );
NNSSndArc* NNS_SndArcGetCurrent( void );

const NNSSndSeqParam* NNS_SndArcGetSeqParam( int seqNo );
const NNSSndSeqParam* NNS_SndArcGetSeqArcParam( int seqArcNo, int index );

const NNSSndArcSeqInfo* NNS_SndArcGetSeqInfo( int seqNo );
const NNSSndArcSeqArcInfo* NNS_SndArcGetSeqArcInfo( int seqNo );
const NNSSndArcBankInfo* NNS_SndArcGetBankInfo( int bankNo );
const NNSSndArcWaveArcInfo* NNS_SndArcGetWaveArcInfo( int waveArcNo );
const NNSSndArcStrmInfo* NNS_SndArcGetStrmInfo( int strmNo );
const NNSSndArcPlayerInfo* NNS_SndArcGetPlayerInfo( int playerNo );
const NNSSndArcGroupInfo* NNS_SndArcGetGroupInfo( int groupNo );
const NNSSndArcStrmPlayerInfo* NNS_SndArcGetStrmPlayerInfo( int playerNo );

u32 NNS_SndArcGetSeqCount( void );
u32 NNS_SndArcGetSeqArcCount( void );
u32 NNS_SndArcGetBankCount( void );
u32 NNS_SndArcGetWaveArcCount( void );
u32 NNS_SndArcGetStrmCount( void );
u32 NNS_SndArcGetGroupCount( void );
u32 NNS_SndArcGetSeqArcSeqCount( int seqArcNo );

#ifndef NNS_FROM_TOOL
s32   NNS_SndArcReadFile( u32 fileId, void* buffer, s32 size, s32 offset );
FSFileID NNS_SndArcGetFileID( void );
#endif // NNS_FROM_TOOL
u32   NNS_SndArcGetFileSize( u32 fileId );
u32   NNS_SndArcGetFileOffset( u32 fileId );
void* NNS_SndArcGetFileAddress( u32 fileId );
void  NNS_SndArcSetFileAddress( u32 fileId, void* address );

const char* NNS_SndArcGetSeqSymbol( int seqNo );
const char* NNS_SndArcGetSeqArcSymbol( int seqArcNo );
const char* NNS_SndArcGetSeqArcIdxSymbol( int seqArcNo, int index );
const char* NNS_SndArcGetBankSymbol( int bankNo );
const char* NNS_SndArcGetWaveArcSymbol( int waveArcNo );
const char* NNS_SndArcGetStrmSymbol( int strmNo );
const char* NNS_SndArcGetGroupSymbol( int groupNo );


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_SNDARC_H_*/


