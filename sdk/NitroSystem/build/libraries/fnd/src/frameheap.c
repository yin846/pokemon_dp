/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     frameheap.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: frameheap.c,v $
  Revision 1.18  01/21/2005 04:55:27 AM  kitamurs
  Changed year

  Revision 1.17  10/15/2004 09:04:40 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.16  10/15/2004 08:49:14 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.15  07/26/2004 04:59:20 AM  kitamurs
  Moved the location of heapcommoni.h
  Added NNS_FndResizeForMBlockFrmHeap()

  Revision 1.14  06/22/2004 08:24:50 AM  kitamurs
  In the Dump function, made the package empty out during FINALROM.

  Revision 1.13  06/22/2004 12:12:34 AM  kitamurs
  Handled the Warning

  Revision 1.12  03/26/2004 10:21:22 AM  kitamurs
  Bundled the no use initialization in the common portion

  Revision 1.11  03/25/2004 08:12:03 AM  kitamurs
  Changed contents of Dump

  Revision 1.10  03/23/2004 12:23:32 AM  kitamurs
  Deleted NNS_FndGetFreeSizeForFrmHeap()

  Revision 1.9  03/22/2004 01:26:49 AM  kitamurs
  Made Dump function common among heaps

  Revision 1.8  03/19/2004 12:06:55 AM  kitamurs
  Fixed comments.

  Revision 1.7  03/10/2004 07:11:44 AM  kitamurs
  Added function to get address of free area

  Revision 1.6  03/05/2004 12:05:58 AM  kitamurs
  - Changed the type of heap handle
  - Changed so that flag can be set when heap is created.

  Revision 1.5  03/01/2004 12:16:38 AM  kitamurs
  Implemented API

  Revision 1.4  02/24/2004 02:46:56 AM  kitamurs
  Implemented frame heap

  Revision 1.3  02/09/2004 05:02:10 AM  kitamurs
  Organized the function names

  Revision 1.2  02/06/2004 06:55:39 AM  kitamurs
  Fixed mistakes in comments

  Revision 1.1  02/06/2004 01:32:14 AM  kitamurs
  Registered the initial version. API definitions only.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/frameheap.h>
#include "heapcommoni.h"

/* ========================================================================
    Macro constant
   ======================================================================== */

// Minimum alignment value
#define MIN_ALIGNMENT           4


/* ========================================================================
    Static functions
   ======================================================================== */

static inline BOOL
IsValidFrmHeapHandle(NNSFndHeapHandle handle)
{
    if(handle == NNS_FND_HEAP_INVALID_HANDLE)
    {
        return FALSE;
    }

    {
        NNSiFndHeapHead* pHeapHd = handle;
        return pHeapHd->signature == NNSI_FRMHEAP_SIGNATURE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetFrmHeapHeadPtrFromHeapHead

  Description:  Get pointer to frame heap header from pointer to heap header.

  Arguments:    pHHead:  Pointer to the heap header.

  Returns:      Returns the pointer to the frame heap header.
 *---------------------------------------------------------------------------*/
static inline NNSiFndFrmHeapHead*
GetFrmHeapHeadPtrFromHeapHead(NNSiFndHeapHead* pHHead)
{
    return AddU32ToPtr(pHHead, sizeof(NNSiFndHeapHead));
}

/*---------------------------------------------------------------------------*
  Name:         GetHeapHeadPtrFromFrmHeapHead

  Description:  Gets heap header pointer from frame heap header pointer.

  Arguments:    pFrmHeapHd:  frame heap header pointer.

  Returns:      Returns the pointer to the heap header.
 *---------------------------------------------------------------------------*/
static inline NNSiFndHeapHead*
GetHeapHeadPtrFromFrmHeapHead(NNSiFndFrmHeapHead* pFrmHeapHd)
{
    return SubU32ToPtr(pFrmHeapHd, sizeof(NNSiFndHeapHead));
}

/*---------------------------------------------------------------------------*
  Name:         InitFrameHeap

  Description:  Initializes the frame heap.

  Arguments:    startAddress:  Starting address of memory used as frame heap.
                endAddress:    Ending address +1 of the memory to be used as the frame heap.
                optFlag:       Option flag.

  Returns:      Returns the pointer to the heap header.
 *---------------------------------------------------------------------------*/
static NNSiFndHeapHead*
InitFrameHeap(
    void*   startAddress,
    void*   endAddress,
    u16     optFlag
)
{
    NNSiFndHeapHead* pHeapHd = startAddress;
    NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    NNSi_FndInitHeapHead(       // Heap common initialization
        pHeapHd,
        NNSI_FRMHEAP_SIGNATURE,
        AddU32ToPtr(pFrmHeapHd, sizeof(NNSiFndFrmHeapHead)),    // heapStart
        endAddress,                                             // heapEnd
        optFlag);

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;

    pFrmHeapHd->pState = NULL;   // State saving state location

    return pHeapHd;
}


/*---------------------------------------------------------------------------*
  Name:         AllocFromHead

  Description:  Allocates the memory block from the top of the heap.
                There is an alignment specification.

  Arguments:    pHHead:  Pointer to the heap header.
                size:    Size of the memory block to be allocated.
                alignment:  Alignment value.

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                Returns NULL if unsuccessful.
 *---------------------------------------------------------------------------*/
static void*
AllocFromHead(
    NNSiFndFrmHeapHead* pFrmHeapHd,
    u32                 size,
    int                 alignment
)
{
    void* newBlock = NNSi_FndRoundUpPtr(pFrmHeapHd->headAllocator, alignment);
    void* endAddress = AddU32ToPtr(newBlock, size);

    if(NNSiGetUIntPtr(endAddress) > NNSiGetUIntPtr(pFrmHeapHd->tailAllocator))
    {
        return NULL;
    }

    FillAllocMemory(  // Fill memory
        GetHeapHeadPtrFromFrmHeapHead(pFrmHeapHd),
        pFrmHeapHd->headAllocator,
        GetOffsetFromPtr(pFrmHeapHd->headAllocator, endAddress));

    pFrmHeapHd->headAllocator = endAddress;

    return newBlock;
}

/*---------------------------------------------------------------------------*
  Name:         AllocFromTail

  Description:  Allocates a memory block from the tail of the heap.
                There is an alignment specification.

  Arguments:    pHHead:     Pointer to the heap header.
                size:       Size of the memory block to be allocated.
                alignment:  Alignment value.

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                Returns NULL if unsuccessful.
 *---------------------------------------------------------------------------*/
static void*
AllocFromTail(
    NNSiFndFrmHeapHead* pFrmHeapHd,
    u32                 size,
    int                 alignment
)
{
    void* newBlock = NNSi_FndRoundDownPtr(SubU32ToPtr(pFrmHeapHd->tailAllocator, size), alignment);

    if(NNSiGetUIntPtr(newBlock) < NNSiGetUIntPtr(pFrmHeapHd->headAllocator))
    {
        return NULL;
    }

    FillAllocMemory(  // Fill memory
        GetHeapHeadPtrFromFrmHeapHead(pFrmHeapHd),
        newBlock,
        GetOffsetFromPtr(newBlock, pFrmHeapHd->tailAllocator));

    pFrmHeapHd->tailAllocator = newBlock;

    return newBlock;
}

/*---------------------------------------------------------------------------*
  Name:         FreeHead

  Description:  Free memory blocks allocated from the head of the heap area all at once.

  Arguments:    pHeapHd:  heap header pointer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
FreeHead(NNSiFndHeapHead* pHeapHd)
{
    NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    FillFreeMemory(
        pHeapHd,
        pHeapHd->heapStart,
        GetOffsetFromPtr(pHeapHd->heapStart, pFrmHeapHd->headAllocator));

    pFrmHeapHd->headAllocator = pHeapHd->heapStart;
    pFrmHeapHd->pState = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         FreeTail

  Description:  Free the memory blocks allocated from the tail of the heap area all at once.

  Arguments:    pHeapHd:  heap header pointer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
FreeTail(NNSiFndHeapHead* pHeapHd)
{
    NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    FillFreeMemory(
        pHeapHd,
        pFrmHeapHd->tailAllocator,
        GetOffsetFromPtr(pFrmHeapHd->tailAllocator, pHeapHd->heapEnd));

    /*
        In order not to accidentally restore released memory blocks due to recovery of the heap allocation state,
        reset the tail allocation pointer of the save information.
     */
    {
        NNSiFndFrmHeapState* pState;
        for (pState = pFrmHeapHd->pState; pState; pState = pState->pPrevState)
        {
            pState->tailAllocator = pHeapHd->heapEnd;
        }
    }

    pFrmHeapHd->tailAllocator = pHeapHd->heapEnd;
}

/*---------------------------------------------------------------------------*
  Name:         PrintSize

  Description:  Output size and percentage.

  Arguments:    size:       Target size.
                wholeSize:  Whole size.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static void
    PrintSize(
        u32     size,
        u32     wholeSize
    )
    {
        OS_Printf("%9d (%6.2f%%)", size, 100.0 * size / wholeSize);
    }

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    External Functions (Non-Public)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndGetFreeStartForFrmHeap

  Description:  Gets the starting address of the free area of the frame heap.

  Arguments:    heap: Handle for frame heap

  Returns:      Returns the starting address of the free area of the frame heap.
 *---------------------------------------------------------------------------*/
void*
NNSi_FndGetFreeStartForFrmHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    return GetFrmHeapHeadPtrFromHeapHead(heap)->headAllocator;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndGetFreeEndForFrmHeap

  Description:  Gets the end address of the free area of the frame heap.

  Arguments:    heap: Handle for frame heap

  Returns:      Returns the end address +1 of the free area of the frame heap.
 *---------------------------------------------------------------------------*/
void*
NNSi_FndGetFreeEndForFrmHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    return GetFrmHeapHeadPtrFromHeapHead(heap)->tailAllocator;
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_FndDumpFrmHeap

  Description:  Displays the data inside the frame heap.
                This function is for debugging.

  Arguments:    heap:    Handle for frame heap

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    void
    NNSi_FndDumpFrmHeap(NNSFndHeapHandle heap)
    {
        NNS_ASSERT(IsValidFrmHeapHandle(heap));

        {
            NNSiFndHeapHead *const pHeapHd = heap;
            NNSiFndFrmHeapHead *const pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);
            const u32 heapSize = GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd);

            NNSi_FndDumpHeapHead(pHeapHd);

            OS_Printf(  "     head [%p - %p) ", pHeapHd->heapStart, pFrmHeapHd->headAllocator);
            PrintSize(GetOffsetFromPtr(pHeapHd->heapStart, pFrmHeapHd->headAllocator), heapSize);
            OS_Printf("\n     free                           ");
            PrintSize(GetOffsetFromPtr(pFrmHeapHd->headAllocator, pFrmHeapHd->tailAllocator), heapSize);
            OS_Printf("\n     tail [%p - %p) ", pFrmHeapHd->tailAllocator, pHeapHd->heapEnd);
            PrintSize(GetOffsetFromPtr(pFrmHeapHd->tailAllocator, pHeapHd->heapEnd), heapSize);
            OS_Printf("\n");

            if (pFrmHeapHd->pState)
            {
                NNSiFndFrmHeapState* pState;

                OS_Printf("    state : [tag]   [head]      [tail]\n");

                for (pState = pFrmHeapHd->pState; pState; pState = pState->pPrevState)
                {
                    OS_Printf("            '%c%c%c%c' : %p %p\n", pState->tagName >>24, (pState->tagName >>16) & 0xFF, (pState->tagName >>8) & 0xFF, pState->tagName & 0xFF,
                                            pState->headAllocator, pState->tailAllocator);
                }
            }

            OS_Printf("\n");
        }
    }

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    External functions (public)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateFrmHeapEx

  Description:  Creates a frame heap.

  Arguments:    startAddress: Start address of heap area
                size:         Size of heap area
                optFlag:      Option flag.

  Returns:      Returns the handle for the created frame heap if the function succeeds.
                Returns NNS_FND_INVALID_HEAP_HANDLE if the function fails.

  Memo:         Base is not thread save.
                When you thread save, please try to add the argument specifying the heap attribute,
                or please suppress the function that sets the attribute.
 *---------------------------------------------------------------------------*/
NNSFndHeapHandle
NNS_FndCreateFrmHeapEx(
    void*   startAddress,
    u32     size,
    u16     optFlag
)
{
    void* endAddress;

    SDK_NULL_ASSERT(startAddress);

    endAddress   = NNSi_FndRoundDownPtr(AddU32ToPtr(startAddress, size), MIN_ALIGNMENT);
    startAddress = NNSi_FndRoundUpPtr(startAddress, MIN_ALIGNMENT);

    if ( NNSiGetUIntPtr(startAddress) > NNSiGetUIntPtr(endAddress)
     ||  GetOffsetFromPtr(startAddress, endAddress) < sizeof(NNSiFndHeapHead) + sizeof(NNSiFndFrmHeapHead)
    )
    {
        return NNS_FND_HEAP_INVALID_HANDLE;
    }

    {   // Initialize frame heap
        NNSiFndHeapHead* pHHead = InitFrameHeap(startAddress, endAddress, optFlag);
        return pHHead;  // The pointer to the heap header is used as-is as the handle value.  
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_FndDestroyFrmHeap

  Description:  Destroys the frame heap.

  Arguments:    heap: Handle for frame heap

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndDestroyFrmHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    NNSi_FndFinalizeHeap(heap);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromFrmHeapEx

  Description:  Allocates a memory block from the frame heap
                The memory block alignment can be specified.
                If a negative alignment value is specified, an empty region is searched for from the back of the heap.

  Arguments:    heap:      Handle for frame heap
                size:      Size of the memory block to be allocated (in bytes)
                alignment: Alignment of the memory block to be allocated.
                           4, 8, 16, 32, -4, -8, -16 or -32 may be specified.

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                Returns NULL if unsuccessful.
 *---------------------------------------------------------------------------*/
void*
NNS_FndAllocFromFrmHeapEx(
    NNSFndHeapHandle    heap,
    u32                 size,
    int                 alignment
)
{
    void* memory = NULL;
    NNSiFndFrmHeapHead* pFrmHeapHd;

    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    // alignment check
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= 32);

    pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);

    if (size == 0)
    {
        size = 1;
    }

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);

    if (alignment >= 0)   // Allocate from the front of the heap
    {
        memory = AllocFromHead(pFrmHeapHd, size, alignment);
    }
    else                    // Allocate from the end of the heap
    {
        memory = AllocFromTail(pFrmHeapHd, size, -alignment);
    }

    return memory;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeToFrmHeap

  Description:  This function returns the memory block to the frame heap.

  Arguments:    heap: Handle for frame heap
                mode: Method of returning memory block

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndFreeToFrmHeap(
    NNSFndHeapHandle    heap,
    int                 mode
)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    if (mode & NNS_FND_FRMHEAP_FREE_HEAD)
    {
        FreeHead(heap);
    }

    if (mode & NNS_FND_FRMHEAP_FREE_TAIL)
    {
        FreeTail(heap);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocatableSizeForFrmHeapEx

  Description:  Get the maximum allocatable size in the frame heap.
                The memory block alignment can be specified.

  Arguments:    heap:      Handle for frame heap
                alignment: Alignment of the memory block to be allocated.
                           4, 8, 16, or 32 may be specified for the value.

  Returns:      Returns the maximum allocatable size in the frame heap (in bytes)
 *---------------------------------------------------------------------------*/
u32
NNS_FndGetAllocatableSizeForFrmHeapEx(
    NNSFndHeapHandle    heap,
    int                 alignment
)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    // alignment check
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= 32);

    alignment = abs(alignment); // Convert to a positive value just to be sure

    {
        const NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        const void* block = NNSi_FndRoundUpPtr(pFrmHeapHd->headAllocator, alignment);

        if (NNSiGetUIntPtr(block) > NNSiGetUIntPtr(pFrmHeapHd->tailAllocator))
        {
            return 0;
        }

        return GetOffsetFromPtr(block, pFrmHeapHd->tailAllocator);
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_FndRecordStateForFrmHeap

  Description:  This function records the current use status of the frame heap.
                You can return to the recorded memory usage status later.
                20 bytes are used to record the status.

  Arguments:    heap:     Handle for frame heap
                tagName:  Tag name to apply to the status record.

  Returns:      Returns TRUE if successful.
                Returns FALSE if unsuccessful.
 *---------------------------------------------------------------------------*/
BOOL
NNS_FndRecordStateForFrmHeap(
    NNSFndHeapHandle    heap,
    u32                 tagName
)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    {
        NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        void* oldHeadAllocator = pFrmHeapHd->headAllocator;

        // Allocate memory for saving information.
        NNSiFndFrmHeapState* pState = AllocFromHead(pFrmHeapHd, sizeof(NNSiFndFrmHeapState), MIN_ALIGNMENT);
        if (! pState)
        {
            return FALSE;
        }

        // Store the current status
        pState->tagName       = tagName;
        pState->headAllocator = oldHeadAllocator;
        pState->tailAllocator = pFrmHeapHd->tailAllocator;
        pState->pPrevState    = pFrmHeapHd->pState;

        pFrmHeapHd->pState = pState;

        return TRUE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeByStateToFrmHeap

  Description:  This function returns the frame heap memory block based on the recorded status.
                This function uses the specified tag name to restore the memory usage status in effect immediately prior to
                calling NNS_FndRecordStateForFrmHeap().
                If 0 is specified for the tag name the status is returned to that in effect
                right before the last call of NNS_FndRecordStateForFrmHeap().

                When a memory block is returned by specifying a tag name,
                the recorded information will disappear if NNS_FndRecordStateForFrmHeap()
                is called later.

  Arguments:    heap:     Handle for frame heap
                tagName:  Tag name given to the status record.

  Returns:      Returns TRUE if successful.
                Returns FALSE if unsuccessful.
 *---------------------------------------------------------------------------*/
BOOL
NNS_FndFreeByStateToFrmHeap(
    NNSFndHeapHandle    heap,
    u32                 tagName
)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    {
        NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(heap);
        NNSiFndFrmHeapState* pState = pFrmHeapHd->pState;

        if (tagName != 0)   // The tag name specification exists
        {
            for(; pState; pState = pState->pPrevState)
            {
                if(pState->tagName == tagName)
                    break;
            }
        }

        if (! pState)
        {
            return FALSE;
        }

        pFrmHeapHd->headAllocator = pState->headAllocator;
        pFrmHeapHd->tailAllocator = pState->tailAllocator;

        pFrmHeapHd->pState = pState->pPrevState;

        return TRUE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAdjustFrmHeap

  Description:  This function frees the open region of the frame heap from the
                heap region and reduces the heap region by that amount.
                There must not be memory blocks allocated from the back of heap memory. 
                 

  Arguments:    heap:     Handle for frame heap

  Returns:      Returns the frame heap size in bytes after reduction if successful.
                 
                Returns 0 if unsuccessful.
 *---------------------------------------------------------------------------*/
u32
NNS_FndAdjustFrmHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidFrmHeapHandle(heap));

    {
        NNSiFndHeapHead* pHeapHd = heap;
        NNSiFndFrmHeapHead* pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

        // The function fails if there are any memory blocks allocated from end.
        if(0 < GetOffsetFromPtr(pFrmHeapHd->tailAllocator, pHeapHd->heapEnd))
        {
            return 0;
        }

        pFrmHeapHd->tailAllocator = pHeapHd->heapEnd = pFrmHeapHd->headAllocator;

        return GetOffsetFromPtr(heap, pHeapHd->heapEnd);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndResizeForMBlockFrmHeap

  Description:  This function changes the size of the memory block allocated from the frame heap.

                The memory block to be resized must be the end
                memory block allocated in the forward direction from the available region of the heap.

  Arguments:    heap:      Handle for frame heap
                memBlock:  Pointer to the memory block to be resized.
                newSize:   The new size to be allocated (in bytes).
                           If a value less than 4 is specified processing is performed as if 4 was specified.

  Returns:      Returns the size of the resized memory block (in bytes) if the function is successful.
                Returns 0 if the function fails.
 *---------------------------------------------------------------------------*/
u32
NNS_FndResizeForMBlockFrmHeap(
    NNSFndHeapHandle    heap,
    void*               memBlock,
    u32                 newSize
)
{
    NNSiFndHeapHead* pHeapHd = NULL;
    NNSiFndFrmHeapHead* pFrmHeapHd = NULL;

    NNS_ASSERT(IsValidFrmHeapHandle(heap));
    NNS_ASSERT(memBlock == NNSi_FndRoundDownPtr(memBlock, MIN_ALIGNMENT));  // Check if at the minimum alignment boundary.

    pHeapHd = heap;
    pFrmHeapHd = GetFrmHeapHeadPtrFromHeapHead(pHeapHd);

    NNS_ASSERT(
            ComparePtr(pHeapHd->heapStart, memBlock) <= 0
        &&  ComparePtr(pFrmHeapHd->headAllocator, memBlock) > 0);           // Be sure that memory blocks exist in the forward direction
    NNS_ASSERT(
            pFrmHeapHd->pState == NULL
        ||  ComparePtr(pFrmHeapHd->pState, memBlock) < 0);                  // Be sure that there is no status saved after the memory block

    /*
        Set up so newSize cannot be 0.
        If it is somehow 0, it's because the memory block specified by memBlock has ceased to exist.
    */
    if (newSize == 0)
    {
        newSize = 1;
    }
    newSize = NNSi_FndRoundUp(newSize, MIN_ALIGNMENT);

    {
        const u32 oldSize = GetOffsetFromPtr(memBlock, pFrmHeapHd->headAllocator);
        void* endAddress = AddU32ToPtr(memBlock, newSize);

        if (newSize == oldSize)  // If the block size is not changed
        {
            return newSize;
        }

        if (newSize > oldSize)  // During magnification
        {
            if (ComparePtr(endAddress, pFrmHeapHd->tailAllocator) > 0)  // If the size is insufficient
            {
                return 0;
            }

            FillAllocMemory(heap, pFrmHeapHd->headAllocator, newSize - oldSize);
        }
        else                    // During reduction
        {
            FillFreeMemory(heap, endAddress, oldSize - newSize);
        }

        pFrmHeapHd->headAllocator = endAddress;

        return newSize;
    }
}

