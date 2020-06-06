/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     unitheap.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: unitheap.h,v $
  Revision 1.7  01/21/2005 04:59:49  kitamurs
  Changed year

  Revision 1.6  10/19/2004 01:22:32  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.5  07/21/2004 06:51:43  kitamurs
  Added NNS_FndGetMemBlockSizeForUnitHeap().

  Revision 1.4  06/22/2004 10:01:31  kitamurs
  Added NNS_FndCalcHeapSizeForUnitHeap().

  Revision 1.3  06/22/2004 08:24:38  kitamurs
  In the Dump function, made the package empty out during FINALROM.

  Revision 1.2  03/22/2004 01:26:56  kitamurs
  Made Dump function common among heaps

  Revision 1.1  03/05/2004 00:08:43  kitamurs
  Registration of the initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_UNITHEAP_H_
#define NNS_FND_UNITHEAP_H_

#include <nnsys/fnd/heapcommon.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    Type Definitions
   ======================================================================== */

typedef struct NNSiFndUntHeapMBlockHead NNSiFndUntHeapMBlockHead;

// Header information for memory block
struct NNSiFndUntHeapMBlockHead
{
    NNSiFndUntHeapMBlockHead*  pMBlkHdNext;    // Next block
};


typedef struct NNSiFndUntMBlockList NNSiFndUntMBlockList;

// Memory block list
struct NNSiFndUntMBlockList
{
    NNSiFndUntHeapMBlockHead*  head;           // Pointer for memory block linked to header
};


typedef struct NNSiFndUntHeapHead NNSiFndUntHeapHead;

// Header information for unit heap
struct NNSiFndUntHeapHead
{
    NNSiFndUntMBlockList    mbFreeList;     // Free list
    u32                     mBlkSize;       // Memory block size
};


/* ========================================================================
    Macro functions
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateUnitHeap

  Description:  Creates unit heap

  Arguments:    startAddress:  Start address of heap area
                heapSize:      Size of heap area
                memBlockSize:  Memory block size

  Returns:      If the function succeeds, a handle for the created unit heap is returned.
                If function fails, NNS_FND_INVALID_HEAP_HANDLE is returned.
 *---------------------------------------------------------------------------*/
#define                 NNS_FndCreateUnitHeap(startAddress, heapSize, memBlockSize) \
                            NNS_FndCreateUnitHeapEx(startAddress, heapSize, memBlockSize, NNS_FND_HEAP_DEFAULT_ALIGNMENT, 0)


/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetMemBlockSizeForUnitHeap

  Description:  Gets the memory block size for unit heap.

  Arguments:    heap:  Unit heap handle.

  Returns:      Returns memory block size for unit heap.
 *---------------------------------------------------------------------------*/
#define                 NNS_FndGetMemBlockSizeForUnitHeap(heap) \
                            (((const NNSiFndUntHeapHead*)((const u8*)((const void*)(heap)) + sizeof(NNSiFndHeapHead)))->mBlkSize)


/* ========================================================================
    Function Prototype
   ======================================================================== */

#if ! defined(NNS_FINALROM)

    void                    NNSi_FndDumpUnitHeap(
                                NNSFndHeapHandle    heap);

// #if ! defined(NNS_FINALROM)
#endif

NNSFndHeapHandle        NNS_FndCreateUnitHeapEx(
                            void*   startAddress,
                            u32     heapSize,
                            u32     memBlockSize,
                            int     alignment,
                            u16     optFlag);

void                    NNS_FndDestroyUnitHeap(
                            NNSFndHeapHandle    heap);

void*                   NNS_FndAllocFromUnitHeap(
                        	NNSFndHeapHandle    heap);

void                    NNS_FndFreeToUnitHeap(
                            NNSFndHeapHandle    heap,
                            void*               memBlock);

u32                     NNS_FndCountFreeBlockForUnitHeap(
                            NNSFndHeapHandle    heap);


u32                     NNS_FndCalcHeapSizeForUnitHeap(
                            u32     memBlockSize,
                            u32     memBlockNum,
                            int     alignment);


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_UNITHEAP_H_*/
#endif

