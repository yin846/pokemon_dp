/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     unitheap.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: unitheap.c,v $
  Revision 1.15  01/21/2005 04:55:27  kitamurs
  Changed year

  Revision 1.14  10/15/2004 09:04:40  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.13  10/15/2004 08:49:14  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.12  10/15/2004 06:14:09  kitamurs
  Revised arguments description.

  Revision 1.11  07/26/2004 04:57:08  kitamurs
  Moved the location of heapcommoni.h

  Revision 1.10  06/22/2004 10:01:23  kitamurs
  Added NNS_FndCalcHeapSizeForUnitHeap().

  Revision 1.9  06/22/2004 08:24:50  kitamurs
  In the Dump function, made the package empty out during FINALROM.

  Revision 1.8  03/26/2004 10:21:28  kitamurs
  Bundled the no use initialization in the common portion

  Revision 1.7  03/25/2004 08:12:35  kitamurs
  Fixed the mistake in Dump size

  Revision 1.6  03/22/2004 01:26:49  kitamurs
  Made Dump function common among heaps

  Revision 1.5  03/19/2004 00:07:09  kitamurs
  Fixed comments.

  Revision 1.4  03/05/2004 00:07:38  kitamurs
  Implemented API

  Revision 1.3  02/24/2004 02:46:56  kitamurs
  Implemented expanded heap

  Revision 1.2  02/09/2004 05:02:10  kitamurs
  Organized the function names

  Revision 1.1  02/06/2004 08:29:45  kitamurs
  Registered the initial version. API definitions only.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/unitheap.h>
#include "heapcommoni.h"


/* ========================================================================
    Macro constants
   ======================================================================== */

// Minimum alignment value
#define MIN_ALIGNMENT           4


/* ========================================================================
    static functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    Memory Block List Operation
   ------------------------------------------------------------------------ */

static NNSiFndUntHeapMBlockHead*
PopMBlock(NNSiFndUntMBlockList* list)
{
    NNSiFndUntHeapMBlockHead* block = list->head;
    if (block)
    {
        list->head = block->pMBlkHdNext;
    }

    return block;
}

/*---------------------------------------------------------------------------*
  Name:         PushMBlock

  Description:  Adds memory block to top of list.

  Arguments:    link:   The list to add
                block:  The memory block to add

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
PushMBlock(
    NNSiFndUntMBlockList*       list,
    NNSiFndUntHeapMBlockHead*   block
)
{
    block->pMBlkHdNext = list->head;
    list->head = block;
}


/*---------------------------------------------------------------------------*
  Name:         GetUnitHeapHeadPtrFromHeapHead

  Description:  Gets the unit heap header pointer from the heap header pointer.

  Arguments:    pHeapHd:  Pointer to the heap header.

  Returns:      Returns the unit heap header pointer.
 *---------------------------------------------------------------------------*/
static inline NNSiFndUntHeapHead*
GetUnitHeapHeadPtrFromHeapHead(NNSiFndHeapHead* pHeapHd)
{
    return AddU32ToPtr(pHeapHd, sizeof(NNSiFndHeapHead));
}

static inline BOOL
IsValidUnitHeapHandle(NNSFndHeapHandle handle)
{
    if(handle == NNS_FND_HEAP_INVALID_HANDLE)
    {
        return FALSE;
    }

    {
        NNSiFndHeapHead* pHeapHd = handle;
        return pHeapHd->signature == NNSI_UNTHEAP_SIGNATURE;
    }
}


/* ========================================================================
    External Functions (Non-Public)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndDumpUnitHeap

  Description:  Displays the information inside unit heap.
                This function is for debugging.

  Arguments:    heap:    Unit heap handle.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    void
    NNSi_FndDumpUnitHeap(NNSFndHeapHandle heap)
    {
        NNS_ASSERT(IsValidUnitHeapHandle(heap));

        {
            NNSiFndHeapHead *const pHeapHd = heap;
            NNSiFndUntHeapHead *const pUnitHeapHd = GetUnitHeapHeadPtrFromHeapHead(pHeapHd);
            const u32 heapSize = GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd);

        	const u32 freeSize = NNS_FndCountFreeBlockForUnitHeap(heap) * pUnitHeapHd->mBlkSize;
        	const u32 usedSize = heapSize - freeSize;

            NNSi_FndDumpHeapHead(pHeapHd);

        	OS_Printf( "    %d / %d bytes (%6.2f%%) used\n",
        											usedSize, heapSize, 100.0f * usedSize / heapSize);
        }
    }

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    External functions (public)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateUnitHeapEx

  Description:  Creates unit heap

  Arguments:    startAddress:  Start address of heap area
                heapSize:      Size of heap area
                memBlockSize:  Memory block size
                alignment:     Memory block alignment.
                               4, 8, 16, or 32 may be specified for the value.
                optFlag:       Option flag.

  Returns:      If the function succeeds, a handle for the created unit heap is returned.
                If function fails, NNS_FND_INVALID_HEAP_HANDLE is returned.
 *---------------------------------------------------------------------------*/
NNSFndHeapHandle
NNS_FndCreateUnitHeapEx(
    void*   startAddress,
    u32     heapSize,
    u32     memBlockSize,
    int     alignment,
    u16     optFlag
)
{
    NNSiFndHeapHead* pHeapHd;
    void* heapEnd;

    SDK_NULL_ASSERT(startAddress);

    // alignment check
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= alignment && alignment <= 32);

    pHeapHd = NNSi_FndRoundUpPtr(startAddress, MIN_ALIGNMENT);
    heapEnd = NNSi_FndRoundDownPtr(AddU32ToPtr(startAddress, heapSize), MIN_ALIGNMENT);

    if (ComparePtr(pHeapHd, heapEnd) > 0)
    {
        return NNS_FND_HEAP_INVALID_HANDLE;
    }

    memBlockSize = NNSi_FndRoundUp(memBlockSize, alignment);    // Real block size

    {
        NNSiFndUntHeapHead* pUntHeapHd = GetUnitHeapHeadPtrFromHeapHead(pHeapHd);
        void* heapStart = NNSi_FndRoundUpPtr(AddU32ToPtr(pUntHeapHd, sizeof(NNSiFndUntHeapHead)), alignment);
        u32 elementNum;

        if (ComparePtr(heapStart, heapEnd) > 0)
        {
            return NNS_FND_HEAP_INVALID_HANDLE;
        }

        elementNum = GetOffsetFromPtr(heapStart, heapEnd) / memBlockSize;
        if (elementNum == 0)
        {
            return NNS_FND_HEAP_INVALID_HANDLE;
        }

        heapEnd = AddU32ToPtr(heapStart, elementNum * memBlockSize);

        NNSi_FndInitHeapHead(           // Heap common initialization
            pHeapHd,
            NNSI_UNTHEAP_SIGNATURE,
            heapStart,
            heapEnd,
            optFlag);

        pUntHeapHd->mbFreeList.head = heapStart;
        pUntHeapHd->mBlkSize = memBlockSize;

        {
            NNSiFndUntHeapMBlockHead* pMBlkHd = pUntHeapHd->mbFreeList.head;
            int i;

            for (i = 0; i < elementNum - 1; ++i, pMBlkHd = pMBlkHd->pMBlkHdNext)
            {
                pMBlkHd->pMBlkHdNext = AddU32ToPtr(pMBlkHd, memBlockSize);
            }

            pMBlkHd->pMBlkHdNext = NULL;
        }

        return pHeapHd;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndDestroyUnitHeap

  Description:  Destroys unit heap.

  Arguments:    heap: Unit heap handle.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndDestroyUnitHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidUnitHeapHandle(heap));

    NNSi_FndFinalizeHeap(heap);
}


/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromUnitHeap

  Description:  Allocates memory block from unit heap.

  Arguments:    heap:   Unit heap handle.

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                If fails, NULL is returned.
 *---------------------------------------------------------------------------*/
void*
NNS_FndAllocFromUnitHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidUnitHeapHandle(heap));

    {
        NNSiFndUntHeapHead* pUntHeapHd = GetUnitHeapHeadPtrFromHeapHead(heap);
        NNSiFndUntHeapMBlockHead* pMBlkHd = PopMBlock(&pUntHeapHd->mbFreeList);

        if (pMBlkHd)
        {
            FillAllocMemory(heap, pMBlkHd, pUntHeapHd->mBlkSize);
        }

        return pMBlkHd;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeToUnitHeap

  Description:  Returns unit heap to memory block.

  Arguments:    heap:     Unit heap handle.
                memBlock: Pointer to the memory block to be returned.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndFreeToUnitHeap(
    NNSFndHeapHandle    heap,
    void*               memBlock
)
{
    NNS_ASSERT(IsValidUnitHeapHandle(heap));

    {
        NNSiFndUntHeapHead* pUntHeapHd = GetUnitHeapHeadPtrFromHeapHead(heap);

        FillFreeMemory(heap, memBlock, pUntHeapHd->mBlkSize);

        PushMBlock(&pUntHeapHd->mbFreeList, memBlock);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCountFreeBlockForUnitHeap

  Description:  Gets the number of empty memory blocks in the unit heap.

  Arguments:    heap:     Unit heap handle.

  Returns:      Returns the number of empty memory blocks in the unit heap.
 *---------------------------------------------------------------------------*/
u32
NNS_FndCountFreeBlockForUnitHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidUnitHeapHandle(heap));

    {
        NNSiFndUntHeapHead* pUntHeapHd = GetUnitHeapHeadPtrFromHeapHead(heap);
        NNSiFndUntHeapMBlockHead* pMBlkHd = pUntHeapHd->mbFreeList.head;
        u32 cnt = 0;

        for (; pMBlkHd; pMBlkHd = pMBlkHd->pMBlkHdNext)
        {
            ++cnt;
        }

        return cnt;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCalcHeapSizeForUnitHeap

  Description:  Gets the size of the heap required from the size of the memory blocks and the number of blocks.

  Arguments:    memBlockSize:  Size of the memory block (in bytes).
                memBlockNum:   Total number of reserved memory blocks.
                alignment:     Memory block alignment.

  Returns:      Returns the required heap size.
 *---------------------------------------------------------------------------*/
u32
NNS_FndCalcHeapSizeForUnitHeap(
    u32     memBlockSize,
    u32     memBlockNum,
    int     alignment
)
{
    return
          // The size the heap uses internally
          sizeof(NNSiFndHeapHead) + sizeof(NNSiFndUntHeapHead)

          // The maximum size required for alignment adjustment
        + (alignment - 4)

          // The size required for all units
        + memBlockNum * NNSi_FndRoundUp(memBlockSize, alignment);
}

