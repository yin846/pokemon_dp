/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     heap.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: heap.h,v $
  Revision 1.9  08/10/2004 12:25:36 AM  ida
  add NNS_SndHeapDestroy

  Revision 1.8  07/16/2004 06:42:58 AM  ida
  fix Project name

  Revision 1.7  06/08/2004 12:48:29 AM  ida
  changed interface

  Revision 1.6  05/25/2004 04:47:50 AM  ida
  removed NNS_SndHeapFindMemoryBlockAll

  Revision 1.5  05/14/2004 12:57:36 AM  ida
  NNS_SndHeapSetCurrent return oldHeapHandle

  Revision 1.4  04/30/2004 12:42:25 AM  ida
  added NNS_SndHeapGetSize

  Revision 1.3  04/28/2004 07:47:26 AM  ida
  fixed comments

  Revision 1.2  04/28/2004 02:56:30 AM  ida
  add comments

  Revision 1.1  04/26/2004 02:22:52 AM  ida
  added heap.h

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_HEAP_H_
#define NNS_SND_HEAP_H_

#include <nitro/types.h>
#include <nitro/snd.h>
#include <nnsys/fnd.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	macro definition
 ******************************************************************************/

#define NNS_SND_HEAP_INVALID_HANDLE NNS_FND_HEAP_INVALID_HANDLE

/******************************************************************************
	structure declarations
 ******************************************************************************/

struct NNSSndHeap;

/******************************************************************************
	type definition
 ******************************************************************************/

typedef void (*NNSSndHeapDisposeCallback)( void* mem, u32 size, u32 data1, u32 data2 );
typedef struct NNSSndHeap* NNSSndHeapHandle;

/******************************************************************************
	public function declarations
 ******************************************************************************/

NNSSndHeapHandle NNS_SndHeapCreate( void* startAddress, u32 size );
void NNS_SndHeapDestroy( NNSSndHeapHandle heap );

void* NNS_SndHeapAlloc( NNSSndHeapHandle heap, u32 size, NNSSndHeapDisposeCallback callback, u32 data1, u32 data2 );
void NNS_SndHeapClear( NNSSndHeapHandle heap );

int NNS_SndHeapSaveState( NNSSndHeapHandle heap );
void NNS_SndHeapLoadState( NNSSndHeapHandle heap, int level );
int NNS_SndHeapGetCurrentLevel( NNSSndHeapHandle heap );

u32 NNS_SndHeapGetSize( NNSSndHeapHandle heap );
u32 NNS_SndHeapGetFreeSize( NNSSndHeapHandle heap );

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_HEAP_H_*/


