/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     frameheap.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: frameheap.h,v $
  Revision 1.9  01/21/2005 04:59:49 AM  kitamurs
  Changed year

  Revision 1.8  10/19/2004 01:19:43 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.7  07/26/2004 04:56:00 AM  kitamurs
  Added NNS_FndResizeForMBlockFrmHeap().

  Revision 1.6  06/22/2004 08:24:38 AM  kitamurs
  In the Dump function, made the package empty out during FINALROM.

  Revision 1.5  03/23/2004 12:23:42 AM  kitamurs
  Deleted NNS_FndGetFreeSizeForFrmHeap()

  Revision 1.4  03/22/2004 01:26:56 AM  kitamurs
  Made Dump function common among heaps

  Revision 1.3  03/10/2004 07:11:38 AM  kitamurs
  Added function to get address of free area

  Revision 1.2  03/05/2004 12:05:45 AM  kitamurs
  - Changed the type of heap handle
  - Changed so that flag can be set when heap is created.

  Revision 1.1  03/01/2004 12:15:44 AM  kitamurs
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_FRAMEHEAP_H_
#define NNS_FND_FRAMEHEAP_H_

#include <nnsys/fnd/heapcommon.h>

#ifdef __cplusplus
extern "C" {
#endif


/* =======================================================================
    Constant definitions
   ======================================================================== */

#define NNS_FND_FRMHEAP_FREE_HEAD (1 <<0)
#define NNS_FND_FRMHEAP_FREE_TAIL (1 <<1)
#define NNS_FND_FRMHEAP_FREE_ALL  (NNS_FND_FRMHEAP_FREE_HEAD | NNS_FND_FRMHEAP_FREE_TAIL)


/* =======================================================================
    Type Definition
   ======================================================================== */

typedef struct NNSiFndFrmHeapState NNSiFndFrmHeapState;

// Structure for state storage
struct NNSiFndFrmHeapState
{
	u32						tagName;        // Tag name
	void*					headAllocator;  // Head location of frame heap
	void*					tailAllocator;  // Tail location of frame  heap
	NNSiFndFrmHeapState*    pPrevState;     // Pointer for previous state storage
};

typedef struct NNSiFndFrmHeapHead NNSiFndFrmHeapHead;

// Header information for frame heap
struct NNSiFndFrmHeapHead
{
	void*				    headAllocator;  // Pointer for head memory reserve
	void*				    tailAllocator;  // Pointer for tail memory reserve

	NNSiFndFrmHeapState*	pState;         // State storage parameter
};


/* =======================================================================
    Macro functions
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateFrmHeap

  Description:  Creates frame heap

  Arguments:    startAddress: Start address of heap area
                size:         Size of heap area

  Returns:      If function succeeds, a handle for the created frame heap is returned.
                If function fails, NNS_FND_INVALID_HEAP_HANDLE is returned.
 *---------------------------------------------------------------------------*/
#define             NNS_FndCreateFrmHeap(startAddress, size) \
                        NNS_FndCreateFrmHeapEx(startAddress, size, 0)

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromFrmHeap

  Description:  Reserves memory block from frame heap
                Alignment of memory block is 4-byte fixed.

  Arguments:    heap:   Handle for frame heap
                size:   Size of memory block to be reserved (bytes)

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                If fails, NULL is returned.
 *---------------------------------------------------------------------------*/
#define             NNS_FndAllocFromFrmHeap(heap, size) \
                        NNS_FndAllocFromFrmHeapEx(heap, size, NNS_FND_HEAP_DEFAULT_ALIGNMENT)

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocatableSizeForFrmHeap

  Description:  Get maximum allocatable size in frame heap.
                Alignment of memory block is 4-byte fixed.

  Arguments:    heap:      Handle for frame heap

  Returns:      Returns maximum allocatable size in frame heap (unit of byte)
 *---------------------------------------------------------------------------*/
#define             NNS_FndGetAllocatableSizeForFrmHeap(heap) \
                        NNS_FndGetAllocatableSizeForFrmHeapEx(heap, NNS_FND_HEAP_DEFAULT_ALIGNMENT)


/* =======================================================================
    Function Prototype
   ======================================================================== */

void*               NNSi_FndGetFreeStartForFrmHeap(
                        NNSFndHeapHandle    heap);

void*               NNSi_FndGetFreeEndForFrmHeap(
                        NNSFndHeapHandle    heap);

#if ! defined(NNS_FINALROM)

    void                NNSi_FndDumpFrmHeap(
                            NNSFndHeapHandle    heap);

// #if ! defined(NNS_FINALROM)
#endif

NNSFndHeapHandle    NNS_FndCreateFrmHeapEx(
                        void*   startAddress,
                        u32     size,
                        u16     optFlag);

void                NNS_FndDestroyFrmHeap(
                        NNSFndHeapHandle    heap);

void*               NNS_FndAllocFromFrmHeapEx(
                        NNSFndHeapHandle    heap,
                        u32                 size,
                        int                 alignment);

void                NNS_FndFreeToFrmHeap(
                        NNSFndHeapHandle    heap,
                        int                 mode);

u32                 NNS_FndGetAllocatableSizeForFrmHeapEx(
                        NNSFndHeapHandle    heap,
                        int                 alignment);

BOOL                NNS_FndRecordStateForFrmHeap(
                        NNSFndHeapHandle    heap,
                        u32                 tagName);

BOOL                NNS_FndFreeByStateToFrmHeap(
                        NNSFndHeapHandle    heap,
                        u32                 tagName);

u32                 NNS_FndAdjustFrmHeap(
                        NNSFndHeapHandle    heap);

u32                 NNS_FndResizeForMBlockFrmHeap(
                        NNSFndHeapHandle    heap,
                        void*               memBlock,
                        u32                 newSize);


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_FRAMEHEAP_H_*/
#endif

