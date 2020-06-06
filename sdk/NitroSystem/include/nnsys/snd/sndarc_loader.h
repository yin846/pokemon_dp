/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     sndarc_loader.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sndarc_loader.h,v $
  Revision 1.11  03/08/2005 05:42:03  ida
  Changed arguments of NNSi_SndArcLoadGroup function.

  Revision 1.10  02/28/2005 23:36:03  ida
  Updated copyright year notation

  Revision 1.9  02/28/2005 02:06:37  ida
  Changed interface to private functions.

  Revision 1.8  11/01/2004 08:50:12  ida
  NNS_SND_ARC_LOAD_* macro move from sndarc_loader.h to sndarc.h

  Revision 1.7  10/21/2004 02:12:47  ida
  NNS_SndArcLoadFile become private

  Revision 1.6  07/16/2004 06:42:58  ida
  fix Project name

  Revision 1.5  06/08/2004 00:51:54  ida
  changed interface

  Revision 1.4  05/27/2004 06:35:33  ida
  add NNS_SndArcLoadGroup

  Revision 1.3  05/12/2004 06:39:58  ida
  modify data header structure

  Revision 1.2  04/30/2004 05:24:50  ida
  add comments

  Revision 1.1  04/26/2004 02:23:02  ida
  add sndarc_loader.h

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_SNDARC_LOADER_H_
#define NNS_SND_SNDARC_LOADER_H_

#include <nitro/types.h>
#include <nnsys/snd/heap.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	structure declaration
 ******************************************************************************/

struct NNSSndSeqData;
struct NNSSndSeqArc;
struct SNDBankData;
struct SNDWaveArc;

/******************************************************************************
	enum definition
 ******************************************************************************/

typedef enum NNSSndArcLoadResult {
    NNS_SND_ARC_LOAD_SUCESS = 0,
    NNS_SND_ARC_LOAD_ERROR_INVALID_GROUP_NO,
    NNS_SND_ARC_LOAD_ERROR_INVALID_SEQ_NO,
    NNS_SND_ARC_LOAD_ERROR_INVALID_SEQARC_NO,
    NNS_SND_ARC_LOAD_ERROR_INVALID_BANK_NO,
    NNS_SND_ARC_LOAD_ERROR_INVALID_WAVEARC_NO,
    NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_SEQ,
    NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_SEQARC,
    NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_BANK,
    NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE
} NNSSndArcLoadResult;

/******************************************************************************
	public function declaration
 ******************************************************************************/

BOOL NNS_SndArcLoadGroup( int groupNo, NNSSndHeapHandle heap );

BOOL NNS_SndArcLoadSeq( int seqNo, NNSSndHeapHandle heap );
BOOL NNS_SndArcLoadSeqArc( int seqArcNo, NNSSndHeapHandle heap );
BOOL NNS_SndArcLoadBank( int bankNo, NNSSndHeapHandle heap );
BOOL NNS_SndArcLoadWaveArc( int waveArcNo, NNSSndHeapHandle heap );

BOOL NNS_SndArcLoadSeqEx( int seqNo, u32 loadFlag, NNSSndHeapHandle heap );
BOOL NNS_SndArcLoadBankEx( int bankNo, u32 loadFlag, NNSSndHeapHandle heap );

/******************************************************************************
	private function declaration
 ******************************************************************************/

void* NNSi_SndArcLoadFile(
    u32 fileId,
    NNSSndHeapDisposeCallback callback,
    u32 data1,
    u32 data2,
    NNSSndHeapHandle heap
);

NNSSndArcLoadResult NNSi_SndArcLoadGroup( int groupNo, NNSSndHeapHandle heap );
NNSSndArcLoadResult NNSi_SndArcLoadSeq( int seqNo, u32 loadFlag, NNSSndHeapHandle heap, BOOL bSetAddr, struct NNSSndSeqData** pData );
NNSSndArcLoadResult NNSi_SndArcLoadSeqArc( int seqArcNo, u32 loadFlag, NNSSndHeapHandle heap, BOOL bSetAddr, struct NNSSndSeqArc** pData );
NNSSndArcLoadResult NNSi_SndArcLoadBank( int bankNo, u32 loadFlag, NNSSndHeapHandle heap, BOOL bSetAddr, struct SNDBankData** pData );
NNSSndArcLoadResult NNSi_SndArcLoadWaveArc( int waveArcNo, u32 loadFlag, NNSSndHeapHandle heap, BOOL bSetAddr, struct SNDWaveArc** pData );

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_SNDARC_LOADER_H_*/


