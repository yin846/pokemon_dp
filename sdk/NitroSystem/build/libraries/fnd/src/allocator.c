/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     allocator.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: allocator.c,v $
  Revision 1.9  2005/02/25 02:25:22 AM  kitamura
  Warning measures

  Revision 1.8  01/21/2005 04:55:27 AM  kitamurs
  Changed year

  Revision 1.7  10/15/2004 09:04:40 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.6  10/15/2004 08:49:14 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.5  07/22/2004 05:21:33 AM  kitamurs
  Revised description.

  Revision 1.4  07/22/2004 04:05:27 AM  kitamurs
  Attached function description.

  Revision 1.3  07/21/2004 10:07:24 AM  kitamurs
  Added code for frame heap, unit heap, SDK heap

  Revision 1.2  07/21/2004 05:32:45 AM  kitamurs
  Corrected missing '_' (underscore)

  Revision 1.1  07/21/2004 05:16:21 AM  kitamurs
  Registration of the initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/expheap.h>
#include <nnsys/fnd/frameheap.h>
#include <nnsys/fnd/unitheap.h>
#include <nnsys/fnd/allocator.h>

/* ========================================================================
    static functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    For Exp Heap
   ------------------------------------------------------------------------ */

static void*
AllocatorAllocForExpHeap(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;
    int const alignment = (int)pAllocator->heapParam1;
    return NNS_FndAllocFromExpHeapEx(heap, size, alignment);
}

static void
AllocatorFreeForExpHeap(
    NNSFndAllocator*    pAllocator,
    void*               memBlock
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;
    NNS_FndFreeToExpHeap(heap, memBlock);
}

/* ------------------------------------------------------------------------
    For Frame Heap
   ------------------------------------------------------------------------ */

static void*
AllocatorAllocForFrmHeap(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;
    int const alignment = (int)pAllocator->heapParam1;
    return NNS_FndAllocFromFrmHeapEx(heap, size, alignment);
}

/*
    In the frame heap it is not possible to free memory in memory block units.
    Therefore, in this implementation this will do nothing.
*/
static void
AllocatorFreeForFrmHeap(
    NNSFndAllocator*    /*pAllocator*/,
    void*               /*memBlock*/
)
{
}


/* ------------------------------------------------------------------------
    For Unit Heap
   ------------------------------------------------------------------------ */

/*
    Null is returned because a block larger than the unit heap's memory block size cannot be allocated.
     
*/
static void*
AllocatorAllocForUnitHeap(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;

    if (size > NNS_FndGetMemBlockSizeForUnitHeap(heap))
    {
        return NULL;
    }

    return NNS_FndAllocFromUnitHeap(heap);
}

static void
AllocatorFreeForUnitHeap(
    NNSFndAllocator*    pAllocator,
    void*               memBlock
)
{
    NNSFndHeapHandle const heap = pAllocator->pHeap;
    NNS_FndFreeToUnitHeap(heap, memBlock);
}


/* ------------------------------------------------------------------------
    For SDK heap
   ------------------------------------------------------------------------ */

static void*
AllocatorAllocForSDKHeap(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    OSHeapHandle const heap = (int)pAllocator->pHeap;
    OSArenaId const id = (OSArenaId)pAllocator->heapParam1;
    return OS_AllocFromHeap(id, heap, size);
}

static void
AllocatorFreeForSDKHeap(
    NNSFndAllocator*    pAllocator,
    void*               memBlock
)
{
    OSHeapHandle const heap = (int)pAllocator->pHeap;
    OSArenaId const id = (OSArenaId)pAllocator->heapParam1;
    OS_FreeToHeap(id, heap, memBlock);
}



/* ========================================================================
    External functions
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromAllocator

  Description:  Allocates memory blocks from the allocator.

                Actual allocation will depend on the allocator and the implementation
                of the memory manager that is associated with it.

  Arguments:    pAllocator:  Address of the NNSFndAllocator structure
                size:        Memory block size (bytes)

  Returns:      If memory block allocation succeeds, returns the beginning address of the block.
                Returns NULL if allocation fails.
 *---------------------------------------------------------------------------*/
void*
NNS_FndAllocFromAllocator(
    NNSFndAllocator*    pAllocator,
    u32                 size
)
{
    NNS_ASSERT(pAllocator);
    return (*pAllocator->pFunc->pfAlloc)(pAllocator, size);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeToAllocator

  Description:  Frees a memory block and returns it to allocator.

                Actual return will depend on the allocator and the implementation
                of the memory manager that is associated with it.

  Arguments:    pAllocator:  Address of the NNSFndAllocator structure
                memBlock:    Pointer to the memory block to be freed

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndFreeToAllocator(
    NNSFndAllocator*    pAllocator,
    void*               memBlock
)
{
    NNS_ASSERT(pAllocator && memBlock);
    (*pAllocator->pFunc->pfFree)(pAllocator, memBlock);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitAllocatorForExpHeap

  Description:  Initializes the allocator so it can allocate and free memory from the extended heap.
                The alignment argument specifies alignment values for all memory blocks that
                are allocated via the allocator.

  Arguments:    pAllocator:  Address of the NNSFndAllocator structure
                heap:        Handle for expanded heap
                alignment:   Alignment value to apply to each memory block that is allocated

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndInitAllocatorForExpHeap(
    NNSFndAllocator*    pAllocator,
    NNSFndHeapHandle    heap,
    int                 alignment
)
{
    static const NNSFndAllocatorFunc sAllocatorFunc =
    {
        AllocatorAllocForExpHeap,
        AllocatorFreeForExpHeap,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = heap;
    pAllocator->heapParam1 = (u32)alignment;
    pAllocator->heapParam2 = 0; // no use
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitAllocatorForFrmHeap

  Description:  Initializes the allocator so it can allocate and free memory from the frame heap.
                The alignment argument specifies alignment values for all memory blocks that
                are allocated via the allocator.

  Arguments:    pAllocator:  Address of the NNSFndAllocator structure
                heap:        Handle for frame heap
                alignment:   Alignment value to apply to each memory block that is allocated

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndInitAllocatorForFrmHeap(
    NNSFndAllocator*    pAllocator,
    NNSFndHeapHandle    heap,
    int                 alignment
)
{
    static const NNSFndAllocatorFunc sAllocatorFunc =
    {
        AllocatorAllocForFrmHeap,
        AllocatorFreeForFrmHeap,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = heap;
    pAllocator->heapParam1 = (u32)alignment;
    pAllocator->heapParam2 = 0; // no use
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitAllocatorForUnitHeap

  Description:  Initializes the allocator so it can allocate and free memory from the unit heap.
                It is not possible to allocate a memory block that is larger than the unit heap memory block size.
                In such a case, the NS_FndAllocFromAllocator() function will return NULL.

  Arguments:    pAllocator:  Address of the NNSFndAllocator structure
                heap:        Unit heap handle.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndInitAllocatorForUnitHeap(
    NNSFndAllocator*    pAllocator,
    NNSFndHeapHandle    heap
)
{
    static const NNSFndAllocatorFunc sAllocatorFunc =
    {
        AllocatorAllocForUnitHeap,
        AllocatorFreeForUnitHeap,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = heap;
    pAllocator->heapParam1 = 0; // no use
    pAllocator->heapParam2 = 0; // no use
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndInitAllocatorForSDKHeap

  Description:  Initializes the allocator for allocating and freeing memory from the heap created with the NITRO-SDK's OS_CreateHeap() function. 
                  

  Arguments:    pAllocator:  Address of the NNSFndAllocator structure
                id:          Arena ID of the arena that the heap is located in.
                heap:        Heap handle

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndInitAllocatorForSDKHeap(
    NNSFndAllocator*    pAllocator,
    OSArenaId           id,
    OSHeapHandle        heap
)
{
    static const NNSFndAllocatorFunc sAllocatorFunc =
    {
        AllocatorAllocForSDKHeap,
        AllocatorFreeForSDKHeap,
    };

    pAllocator->pFunc = &sAllocatorFunc;
    pAllocator->pHeap = (void*)heap;
    pAllocator->heapParam1 = id;
    pAllocator->heapParam2 = 0; // no use
}


