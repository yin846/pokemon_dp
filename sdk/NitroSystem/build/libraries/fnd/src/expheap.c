/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     expheap.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: expheap.c,v $
  Revision 1.24  2005/02/25 02:25:22 AM  kitamurs
  Warning measures

  Revision 1.23  01/21/2005 04:55:27 AM  kitamurs
  Changed year

  Revision 1.22  10/15/2004 09:04:40 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.21  10/15/2004 07:27:45 AM  kitamurs
  - Changed argument names
  - Changed SDK_... to NNS_...

  Revision 1.20  10/15/2004 06:35:04 AM  kitamurs
  Revised comments for argument names.

  Revision 1.19  08/20/2004 07:21:02 AM  kitamurs
  Added comments

  Revision 1.18  08/20/2004 12:07:55 AM  kitamurs
  Added check functions

  Revision 1.17  07/26/2004 04:56:39 AM  kitamurs
  Moved the location of heapcommoni.h

  Revision 1.16  06/22/2004 08:24:50 AM  kitamurs
  In the Dump function, made the package empty out during FINALROM.

  Revision 1.15  03/26/2004 10:21:14 AM  kitamurs
  * Summarized all the common parts of the no use initialization
  * feature 0 clear

  Revision 1.14  03/26/2004 07:01:04 AM  kitamurs
  File correction during memory block saves

  Revision 1.13  03/26/2004 06:26:10 AM  kitamurs
  File correction during memory block releases

  Revision 1.12  03/25/2004 08:11:35 AM  kitamurs
  * Changed the default value for the group ID
  * Changed the Dump details

  Revision 1.11  03/22/2004 01:26:49 AM  kitamurs
  Made Dump function common among heaps

  Revision 1.10  03/19/2004 12:06:33 AM  kitamurs
  Fixed comments.

  Revision 1.9  03/15/2004 08:29:23 AM  kitamurs
  Coding format correction

  Revision 1.8  03/05/2004 12:05:21 AM  kitamurs
  - Changed the type of heap handle
  - Changed so that flag can be set when heap is created.

  Revision 1.7  03/01/2004 12:14:05 AM  kitamurs
  Cleaned up unnecessary or overlapping portion

  Revision 1.6  02/24/2004 02:46:56 AM  kitamurs
  Implemented expanded heap

  Revision 1.5  02/09/2004 05:02:10 AM  kitamurs
  Organized the function names

  Revision 1.4  02/06/2004 02:54:06 AM  kitamurs
  Added the argument userParam for NNS_FndVisitAllocatedOfExpHeap

  Revision 1.3  02/06/2004 12:07:01 AM  kitamurs
  Converted tabs to spaces

  Revision 1.2  02/05/2004 08:43:24 AM  kitamurs
  Added a function for fetching the memory block group ID

  Revision 1.1  02/05/2004 01:53:40 AM  kitamurs
  Registered the initial version. API definitions only.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/expheap.h>
#include "heapcommoni.h"


/* ========================================================================
    Macro constants
   ======================================================================== */

// Free memory block signature
#define MBLOCK_FREE_SIGNATURE   ('FR')

// Signature for the memory block being used
#define MBLOCK_USED_SIGNATURE   ('UD')

// Maximum group ID value
#define MAX_GROUPID             0xff

// Default value for the group ID
#define DEFAULT_GROUPID         0

// Minimum alignment value
#define MIN_ALIGNMENT           4

// Default memory allocation mode
#define DEFAULT_ALLOC_MODE      NNS_FND_EXPHEAP_ALLOC_MODE_FIRST


/* ========================================================================
    Structure definitions
   ======================================================================== */
typedef struct NNSiMemRegion NNSiMemRegion;

struct NNSiMemRegion
{
    void*       start;
    void*       end;
};


/* ========================================================================
    Macro functions
   ======================================================================== */

#if ! defined(NNS_FINALROM)

    // For outputting warnings when checking the heap
    #define HEAP_WARNING(exp, ...) \
        (void) ((exp) && (OS_Printf(__VA_ARGS__), 0))

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    Static functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    Pointer Operations
   ------------------------------------------------------------------------ */

static inline void*
MaxPtr(void* a, void* b)
{
    return NNSiGetUIntPtr(a) > NNSiGetUIntPtr(b) ? a: b;
}

static inline BOOL
IsValidExpHeapHandle(NNSFndHeapHandle handle)
{
    if (handle == NNS_FND_HEAP_INVALID_HANDLE)
    {
        return FALSE;
    }

    {
        NNSiFndHeapHead* pHeapHd = handle;
        return pHeapHd->signature == NNSI_EXPHEAP_SIGNATURE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetExpHeapHeadPtrFromHeapHead

  Description:  Gets the pointer to the extended heap header from the pointer to the heap header.

  Arguments:    pHHead:  Pointer to the heap header.

  Returns:      Returns the pointer to the extended heap header.
 *---------------------------------------------------------------------------*/
static inline NNSiFndExpHeapHead*
GetExpHeapHeadPtrFromHeapHead(NNSiFndHeapHead* pHHead)
{
    return AddU32ToPtr(pHHead, sizeof(NNSiFndHeapHead));
}

/*---------------------------------------------------------------------------*
  Name:         GetHeapHeadPtrFromExpHeapHead

  Description:  Gets the pointer to the heap header from the pointer to the extended heap header.

  Arguments:    pEHHead:  Pointer to the extended heap header.

  Returns:      Returns the pointer to the heap header.
 *---------------------------------------------------------------------------*/
static inline NNSiFndHeapHead*
GetHeapHeadPtrFromExpHeapHead(NNSiFndExpHeapHead* pEHHead)
{
    return SubU32ToPtr(pEHHead, sizeof(NNSiFndHeapHead));
}

/*---------------------------------------------------------------------------*
  Name:         GetExpHeapHeadPtrFromHandle

  Description:  Gets the pointer to the extended heap header from the extended heap handle.

  Arguments:    heap:  Extended heap handle.

  Returns:      Returns the pointer to the extended heap header.
 *---------------------------------------------------------------------------*/
static inline NNSiFndExpHeapHead*
GetExpHeapHeadPtrFromHandle(NNSFndHeapHandle heap)
{
    return GetExpHeapHeadPtrFromHeapHead(heap);
}

/*---------------------------------------------------------------------------*
  Name:         GetMemPtrForMBlock

  Description:  Gets the pointer to the memory block
                from the pointer to the NNSiFndExpHeapMBlockHead structure.

  Arguments:    pMBlkHd:  Pointer to the NNSiFndExpHeapMBlockHead structure.

  Returns:      Returns the pointer to the memory block.
 *---------------------------------------------------------------------------*/
static inline void*
GetMemPtrForMBlock(NNSiFndExpHeapMBlockHead* pMBlkHd)
{
    return AddU32ToPtr(pMBlkHd, sizeof(NNSiFndExpHeapMBlockHead));
}

static inline const void*
GetMemCPtrForMBlock(const NNSiFndExpHeapMBlockHead* pMBlkHd)
{
    return AddU32ToCPtr(pMBlkHd, sizeof(NNSiFndExpHeapMBlockHead));
}

/*---------------------------------------------------------------------------*
  Name:         GetMBlockHeadPtr

  Description:  Gets the pointer to the NNSiFndExpHeapMBlockHead structure
                from the pointer to the memory block.
                from the pointer to the NNSiFndExpHeapMBlockHead structure.

  Arguments:    memBlock:  Pointer to the memory block.

  Returns:      Returns the pointer to the NNSiFndExpHeapMBlockHead structure.
 *---------------------------------------------------------------------------*/
static inline NNSiFndExpHeapMBlockHead*
GetMBlockHeadPtr(void* memBlock)
{
    return SubU32ToPtr(memBlock, sizeof(NNSiFndExpHeapMBlockHead));
}

static inline const NNSiFndExpHeapMBlockHead*
GetMBlockHeadCPtr(const void* memBlock)
{
    return SubU32ToCPtr(memBlock, sizeof(NNSiFndExpHeapMBlockHead));
}

static inline void*
GetMBlockEndAddr(NNSiFndExpHeapMBlockHead* pMBHead)
{
    return AddU32ToPtr(GetMemPtrForMBlock(pMBHead), pMBHead->blockSize);
}

/* ------------------------------------------------------------------------
    NNSiFndExpHeapMBlockHead Structure Access Function
   ------------------------------------------------------------------------ */

/*---------------------------------------------------------------------------*
  Name:         GetAlignmentForMBlock

  Description:  Gets the alignment value for the NNSiFndExpHeapMBlockHead structure.

  Arguments:    pMBlkHd:  Pointer to the NNSiFndExpHeapMBlockHead structure.

  Returns:      Returns the alignment value for the NNSiFndExpHeapMBlockHead structure.
 *---------------------------------------------------------------------------*/
static inline u16
GetAlignmentForMBlock(const NNSiFndExpHeapMBlockHead* pMBlkHd)
{
    return (u16)NNSi_FndGetBitValue(pMBlkHd->attribute, 8, 7);
}

/*---------------------------------------------------------------------------*
  Name:         SetAlignmentForMBlock

  Description:  Sets the alignment value for the NNSiFndExpHeapMBlockHead structure.

  Arguments:    pMBlkHd:    Pointer to the NNSiFndExpHeapMBlockHead structure.
                alignment:  Alignment value to be set

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
SetAlignmentForMBlock(
    NNSiFndExpHeapMBlockHead*   pMBlkHd,
    u16                         alignment
)
{
    NNSi_FndSetBitValue(pMBlkHd->attribute, 8, 7, alignment);
}

static inline u16
GetGroupIDForMBlock(const NNSiFndExpHeapMBlockHead* pMBHead)
{
    return (u16)NNSi_FndGetBitValue(pMBHead->attribute, 0, 8);
}

static inline void
SetGroupIDForMBlock(
    NNSiFndExpHeapMBlockHead*   pMBHead,
    u16                         id
)
{
    NNSi_FndSetBitValue(pMBHead->attribute, 0, 8, id);
}

static inline u16
GetAllocDirForMBlock(const NNSiFndExpHeapMBlockHead* pMBHead)
{
    return (u16)NNSi_FndGetBitValue(pMBHead->attribute, 15, 1);
}

static inline void
SetAllocDirForMBlock(
    NNSiFndExpHeapMBlockHead*   pMBHead,
    u16                         mode
)
{
    NNSi_FndSetBitValue(pMBHead->attribute, 15, 1, mode);
}


/* ------------------------------------------------------------------------
    NNSiFndExpHeapHead Structure Access Function
   ------------------------------------------------------------------------ */

/*---------------------------------------------------------------------------*
  Name:         GetAllocMode

  Description:  Gets the memory allocation mode for the extended heap

  Arguments:    pEHHead:  Pointer to the extended heap header.

  Returns:      Returns the memory allocation mode for the extended heap.
 *---------------------------------------------------------------------------*/
static inline u16
GetAllocMode(NNSiFndExpHeapHead* pEHHead)
{
    return (u16)NNSi_FndGetBitValue(pEHHead->feature, 0, 1);
}

/*---------------------------------------------------------------------------*
  Name:         SetAllocMode

  Description:  Sets the memory allocation mode for the extended heap.

  Arguments:    pEHHead:  Pointer to the extended heap header.
                mode:     Memory allocation mode.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
SetAllocMode(
    NNSiFndExpHeapHead* pEHHead,
    u16                 mode
)
{
    NNSi_FndSetBitValue(pEHHead->feature, 0, 1, mode);
}

static void
GetRegionOfMBlock(
    NNSiMemRegion*              region,
    NNSiFndExpHeapMBlockHead*   block
)
{
    region->start = SubU32ToPtr(block, GetAlignmentForMBlock(block));
    region->end   = GetMBlockEndAddr(block);
}


/* ------------------------------------------------------------------------
    Memory Block List Operation
   ------------------------------------------------------------------------ */

static NNSiFndExpHeapMBlockHead*
RemoveMBlock(
    NNSiFndExpMBlockList*       list,
    NNSiFndExpHeapMBlockHead*   block
)
{
    NNSiFndExpHeapMBlockHead *const prev = block->pMBHeadPrev;
    NNSiFndExpHeapMBlockHead *const next = block->pMBHeadNext;

    // previous reference link
    if (prev)
    {
        prev->pMBHeadNext = next;
    }
    else
    {
        list->head = next;
    }

    // next reference link
    if (next)
    {
        next->pMBHeadPrev = prev;
    }
    else
    {
        list->tail = prev;
    }

    return prev;
}

static NNSiFndExpHeapMBlockHead*
InsertMBlock(
    NNSiFndExpMBlockList*       list,
    NNSiFndExpHeapMBlockHead*   target,
    NNSiFndExpHeapMBlockHead*   prev
)
{
    NNSiFndExpHeapMBlockHead* next;

    // previous reference link
    target->pMBHeadPrev = prev;
    if (prev)
    {
        next = prev->pMBHeadNext;
        prev->pMBHeadNext = target;
    }
    else
    {
        next = list->head;
        list->head = target;
    }

    // next reference link
    target->pMBHeadNext = next;
    if (next)
    {
        next->pMBHeadPrev = target;
    }
    else
    {
        list->tail = target;
    }

    return target;
}

/*---------------------------------------------------------------------------*
  Name:         AppendMBlock

  Description:  Adds a memory block to the end of the list.

  Arguments:    link:   The list to add
                block:  The memory block to add

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void
AppendMBlock(
    NNSiFndExpMBlockList*       list,
    NNSiFndExpHeapMBlockHead*   block
)
{
    (void)InsertMBlock(list, block, list->tail);
}

/*---------------------------------------------------------------------------*
  Name:         InitMBlock

  Description:  Initializes the memory block.

  Arguments:    pRegion:    Pointer to the structure representing the region used for the memory block.
                signature:  Memory block signature.

  Returns:      Returns the pointer to the initialized memory block.
 *---------------------------------------------------------------------------*/
static NNSiFndExpHeapMBlockHead*
InitMBlock(
    const NNSiMemRegion*    pRegion,
    u16                     signature
)
{
    NNSiFndExpHeapMBlockHead* block = pRegion->start;

    block->signature = signature;
    block->attribute = 0;
    block->blockSize = GetOffsetFromPtr(GetMemPtrForMBlock(block), pRegion->end);
    block->pMBHeadPrev = NULL;
    block->pMBHeadNext = NULL;

    return block;
}

/*---------------------------------------------------------------------------*
  Name:         InitFreeMBlock

  Description:  Initializes the memory block for use as a free block.

  Arguments:    pRegion:    Pointer to the structure representing the region used for the memory block.

  Returns:      Returns the pointer to the initialized memory block.
 *---------------------------------------------------------------------------*/
static inline NNSiFndExpHeapMBlockHead*
InitFreeMBlock(
    const NNSiMemRegion*    pRegion
)
{
    return InitMBlock(pRegion, MBLOCK_FREE_SIGNATURE);
}

/*---------------------------------------------------------------------------*
  Name:         InitExpHeap

  Description:  Initializes the extended heap.

  Arguments:    startAddress:  Memory start address for the extended heap.
                endAddress:    The memory end address for the extended heap is incremented by one.
                optFlag:       Option flag.

  Returns:      Returns the pointer to the heap header.
 *---------------------------------------------------------------------------*/
static NNSiFndHeapHead*
InitExpHeap(
    void*   startAddress,
    void*   endAddress,
    u16     optFlag
)
{
    NNSiFndHeapHead* pHeapHd = startAddress;
    NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);

    NNSi_FndInitHeapHead(           // Heap common initialization
        pHeapHd,
        NNSI_EXPHEAP_SIGNATURE,
        AddU32ToPtr(pExpHeapHd, sizeof(NNSiFndExpHeapHead)),    // heapStart
        endAddress,                                             // heapEnd
        optFlag);

    pExpHeapHd->groupID = DEFAULT_GROUPID;      // Group ID
    pExpHeapHd->feature = 0;
    SetAllocMode(pExpHeapHd, DEFAULT_ALLOC_MODE);

    // Creates a free block
    {
        NNSiFndExpHeapMBlockHead* pMBHead;
        NNSiMemRegion region;
        region.start = pHeapHd->heapStart;
        region.end   = pHeapHd->heapEnd;
        pMBHead = InitFreeMBlock(&region);

        // Block list
        pExpHeapHd->mbFreeList.head = pMBHead;
        pExpHeapHd->mbFreeList.tail = pMBHead;
        pExpHeapHd->mbUsedList.head = NULL;
        pExpHeapHd->mbUsedList.tail = NULL;

        return pHeapHd;
    }
}

/*---------------------------------------------------------------------------*
  Name:         AllocUsedBlockFromFreeBlock

  Description:  Allocates a new memory block from the free blocks.

  Arguments:    pEHHead:      Pointer to the extended heap header.
                pMBHeadFree:  Pointer to the free block header.
                mblock:       Address for the memory block to be allocated.
                size:         Size of the memory block to be allocated.
                direction:    Allocation direction.

  Returns:      Returns a pointer to the top of the allocated memory block.
 *---------------------------------------------------------------------------*/
static void*
AllocUsedBlockFromFreeBlock(
    NNSiFndExpHeapHead*         pEHHead,
    NNSiFndExpHeapMBlockHead*   pMBHeadFree,
    void*                       mblock,
    u32                         size,
    u16                         direction
)
{
    NNSiMemRegion freeRgnT;
    NNSiMemRegion freeRgnB;
    NNSiFndExpHeapMBlockHead* pMBHeadFreePrev;

    GetRegionOfMBlock(&freeRgnT, pMBHeadFree);
    freeRgnB.end   = freeRgnT.end;
    freeRgnB.start = AddU32ToPtr(mblock, size);
    freeRgnT.end   = SubU32ToPtr(mblock, sizeof(NNSiFndExpHeapMBlockHead));

    pMBHeadFreePrev = RemoveMBlock(&pEHHead->mbFreeList, pMBHeadFree);  // Deletes the free block

    // If there is no space for creating a free block
    if (GetOffsetFromPtr(freeRgnT.start, freeRgnT.end) < sizeof(NNSiFndExpHeapMBlockHead))
    {
        freeRgnT.end = freeRgnT.start;  // Includes the alignment value for the block being used
    }
    else
    {
        pMBHeadFreePrev = InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgnT), pMBHeadFreePrev);
    }

    // If there is no space for creating a free block
    if (GetOffsetFromPtr(freeRgnB.start, freeRgnB.end) < sizeof(NNSiFndExpHeapMBlockHead))
    {
        freeRgnB.start= freeRgnB.end;   // Includes the block being used
    }
    else
    {
        (void)InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&freeRgnB), pMBHeadFreePrev);
    }

    // Fills the memory for debugging
    FillAllocMemory(GetHeapHeadPtrFromExpHeapHead(pEHHead), freeRgnT.end, GetOffsetFromPtr(freeRgnT.end, freeRgnB.start));

    // Creates a new block
    {
        NNSiFndExpHeapMBlockHead* pMBHeadNewUsed;
        NNSiMemRegion region;

        region.start = SubU32ToPtr(mblock, sizeof(NNSiFndExpHeapMBlockHead));
        region.end   = freeRgnB.start;

        pMBHeadNewUsed = InitMBlock(&region, MBLOCK_USED_SIGNATURE);
        SetAllocDirForMBlock(pMBHeadNewUsed, direction);
        SetAlignmentForMBlock(pMBHeadNewUsed, (u16)GetOffsetFromPtr(freeRgnT.end, pMBHeadNewUsed));
        SetGroupIDForMBlock(pMBHeadNewUsed, pEHHead->groupID);
        AppendMBlock(&pEHHead->mbUsedList, pMBHeadNewUsed);
    }

    return mblock;
}

/*---------------------------------------------------------------------------*
  Name:         AllocFromHead

  Description:  Allocates the memory block from the top of the heap.

  Arguments:    pHeapHd:    Pointer to the heap header.
                size:       Size of the memory block to be allocated.
                alignment:  Alignment value.

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                Returns NULL if unsuccessful.
 *---------------------------------------------------------------------------*/
static void*
AllocFromHead(
    NNSiFndHeapHead*    pHeapHd,
    u32                 size,
    int                 alignment
)
{
    NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);

    // Allocate the first one found?
    const BOOL bAllocFirst = GetAllocMode(pExpHeapHd) == NNS_FND_EXPHEAP_ALLOC_MODE_FIRST;

    NNSiFndExpHeapMBlockHead* pMBlkHd      = NULL;
    NNSiFndExpHeapMBlockHead* pMBlkHdFound = NULL;
    u32 foundSize = 0xffffffff;
    void* foundMBlock = NULL;

    // Search for free block
    for (pMBlkHd = pExpHeapHd->mbFreeList.head; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadNext)
    {
        void *const mblock    = GetMemPtrForMBlock(pMBlkHd);
        void *const reqMBlock = NNSi_FndRoundUpPtr(mblock, alignment);
        const u32 offset      = GetOffsetFromPtr(mblock, reqMBlock);    // Generated offset

        if ( pMBlkHd->blockSize >= size + offset
         &&  foundSize > pMBlkHd->blockSize )
        {
            pMBlkHdFound  = pMBlkHd;
            foundSize     = pMBlkHd->blockSize;
            foundMBlock   = reqMBlock;

            if (bAllocFirst || foundSize == size)
            {
                break;
            }
        }
    }

    if (! pMBlkHdFound) // No blocks matching the conditions were found
    {
        return NULL;
    }

    return AllocUsedBlockFromFreeBlock( // Allocates a region from the free block that was found
            pExpHeapHd,
            pMBlkHdFound,
            foundMBlock,
            size,
            NNS_FND_EXPHEAP_ALLOC_DIR_FRONT);
}

/*---------------------------------------------------------------------------*
  Name:         AllocFromTail

  Description:  Allocates the memory block from the tail of the heap.

  Arguments:    pHeapHd:    Pointer to the heap header.
                size:       Size of the memory block to be allocated.
                alignment:  Alignment value.

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                Returns NULL if unsuccessful.
 *---------------------------------------------------------------------------*/
static void*
AllocFromTail(
    NNSiFndHeapHead*    pHeapHd,
    u32                 size,
    int                 alignment
)
{
    NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);

    // Allocate the first one found?
    const BOOL bAllocFirst = GetAllocMode(pExpHeapHd) == NNS_FND_EXPHEAP_ALLOC_MODE_FIRST;

    NNSiFndExpHeapMBlockHead* pMBlkHd      = NULL;
    NNSiFndExpHeapMBlockHead* pMBlkHdFound = NULL;
    u32 foundSize = 0xffffffff;
    void* foundMBlock = NULL;

    // Search for free block
    for (pMBlkHd = pExpHeapHd->mbFreeList.tail; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadPrev)
    {
        void *const mblock    = GetMemPtrForMBlock(pMBlkHd);
        void *const mblockEnd = AddU32ToPtr(mblock, pMBlkHd->blockSize);
        void *const reqMBlock = NNSi_FndRoundDownPtr(SubU32ToPtr(mblockEnd, size), alignment);  // Aligned address

        if ( ComparePtr(reqMBlock, mblock) >= 0
         &&  foundSize > pMBlkHd->blockSize )
        {
            pMBlkHdFound = pMBlkHd;
            foundSize    = pMBlkHd->blockSize;
            foundMBlock  = reqMBlock;

            if (bAllocFirst || foundSize == size)
            {
                break;
            }
        }
    }

    if (! pMBlkHdFound) // No blocks matching the conditions were found
    {
        return NULL;
    }

    return AllocUsedBlockFromFreeBlock( // Allocates a region from the free block that was found
            pExpHeapHd,
            pMBlkHdFound,
            foundMBlock,
            size,
            NNS_FND_EXPHEAP_ALLOC_DIR_REAR);
}

/*---------------------------------------------------------------------------*
  Name:         RecycleRegion

  Description:  Inserts the empty region in the free memory block.
                If it is adjacent to the free block it expands the free block.
                If there is no adjacent block, or the block is not large enough
                to be used as a free block, the alignment value of the used block adjacent to the end of the memory space is used.
                If there is no used block next to the end of the free block the function fails.

  Arguments:    pEHHead:  Pointer to the extended heap header.
                pRegion:  Pointer to the free region.

  Returns:      Returns TRUE if the function is successful.
                Returns FALSE if it fails.
 *---------------------------------------------------------------------------*/
static BOOL
RecycleRegion(
    NNSiFndExpHeapHead*     pEHHead,
    const NNSiMemRegion*    pRegion
)
{
    NNSiFndExpHeapMBlockHead* pBlkPrFree  = NULL;   // Immediately preceding free block
    NNSiMemRegion freeRgn = *pRegion;

    // Searches for free area next to the specified one.
    {
        NNSiFndExpHeapMBlockHead* pBlk;

        for (pBlk = pEHHead->mbFreeList.head; pBlk; pBlk = pBlk->pMBHeadNext)
        {
            if (pBlk < pRegion->start)
            {
                pBlkPrFree = pBlk;
                continue;
            }

            if (pBlk == pRegion->end)   // Is the block next to the end?
            {
                // Combines available regions
                freeRgn.end = GetMBlockEndAddr(pBlk);
                (void)RemoveMBlock(&pEHHead->mbFreeList, pBlk);

                // Pads the header with NoUse
                FillNoUseMemory(GetHeapHeadPtrFromExpHeapHead(pEHHead), pBlk, sizeof(NNSiFndExpHeapMBlockHead));
            }
            break;
        }
    }

    // Is the immediately preceding free block adjacent to the front?
    if (pBlkPrFree && GetMBlockEndAddr(pBlkPrFree) == pRegion->start)
    {
        // Combines the available regions
        freeRgn.start = pBlkPrFree;
        pBlkPrFree = RemoveMBlock(&pEHHead->mbFreeList, pBlkPrFree);
    }

    if (GetOffsetFromPtr(freeRgn.start, freeRgn.end) < sizeof(NNSiFndExpHeapMBlockHead)) // Size not suitable for use as a block
    {
        return FALSE;   // Control reaches this point if an attempt is made to shrink a small sized block using NNS_FndResizeForMBlockExpHeap() and
                        // there is no free block after it. 
    }

    // Fills the memory for debugging
    FillFreeMemory(GetHeapHeadPtrFromExpHeapHead(pEHHead), pRegion->start, GetOffsetFromPtr(pRegion->start, pRegion->end));

    (void)InsertMBlock(&pEHHead->mbFreeList,
        InitFreeMBlock(&freeRgn),   // Creates the free block
        pBlkPrFree);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         CheckMBlock

  Description:  Checks whether the header contents for the memory block are appropriate.

  Arguments:    pMBHead:    Pointer to the header for the memory block to be checked.
                pHeapHd:    Pointer to the extended heap header.
                signature:  Memory block signature
                heapType:   Memory block type (used or free)
                flag:       Flag.

  Returns:      Returns TRUE if the header contents for the memory block are appropriate,
                otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    CheckMBlock(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        NNSiFndHeapHead*                pHeapHd,
        u16                             signature,
        const char*                     heapType,
        u32                             flag
    )
    {
        const BOOL bPrint = 0 != (flag & NNS_FND_HEAP_ERROR_PRINT);
        const void *const memBlock = GetMemCPtrForMBlock(pMBHead);

        if (pHeapHd)
        {
            if ( NNSiGetUIntPtr(pMBHead) < NNSiGetUIntPtr(pHeapHd->heapStart)
              || NNSiGetUIntPtr(memBlock) > NNSiGetUIntPtr(pHeapHd->heapEnd)
            )
            {
                HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Bad %s memory block address. - address %08X, heap area [%08X - %08X)\n",
                    heapType, memBlock, pHeapHd->heapStart, pHeapHd->heapEnd);
                return FALSE;
            }
        }
        else
        {
            if (NNSiGetUIntPtr(pMBHead) >= 0x11000000)
            {
                HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Bad %s memory block address. - address %08X\n",
                    heapType, memBlock);
                return FALSE;
            }
        }

        if (pMBHead->signature != signature)    // Is the signature different?
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Bad %s memory block signature. - address %08X, signature %04X\n",
                heapType, memBlock, pMBHead->signature);
            return FALSE;
        }

        if (pMBHead->blockSize >= 0x01000000)   // Too big?
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Too large %s memory block. - address %08X, block size %08X\n",
                heapType, memBlock, pMBHead->blockSize);
            return FALSE;
        }

        if (pHeapHd)
        {
            if (NNSiGetUIntPtr(memBlock) + pMBHead->blockSize > NNSiGetUIntPtr(pHeapHd->heapEnd))
            {
                HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " wrong size %s memory block. - address %08X, block size %08X\n",
                    heapType, memBlock, pMBHead->blockSize);
                return FALSE;
            }
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckUsedMBlock

  Description:  Checks whether the header contents for the memory block being used are appropriate.

  Arguments:    pMBHead:  Pointer to the header for the memory block to be checked.
                pHeapHd:  Pointer to the extended heap header.
                flag:     Flag.

  Returns:      Returns TRUE if the header contents for the memory block are appropriate,
                otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static inline BOOL
    CheckUsedMBlock(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        NNSiFndHeapHead*                pHeapHd,
        u32                             flag
    )
    {
        return CheckMBlock(pMBHead, pHeapHd, MBLOCK_USED_SIGNATURE, "used", flag);
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckFreeMBlock

  Description:  Checks whether the header contents for the free memory block are appropriate.

  Arguments:    pMBHead:  Pointer to the header for the memory block to be checked.
                pHeapHd:  Pointer to the extended heap header.
                flag:     Flag.

  Returns:      Returns TRUE if the header contents for the memory block are appropriate,
                otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static inline BOOL
    CheckFreeMBlock(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        NNSiFndHeapHead*                pHeapHd,
        u32                             flag
    )
    {
        return CheckMBlock(pMBHead, pHeapHd, MBLOCK_FREE_SIGNATURE, "free", flag);
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckMBlockPrevPtr

  Description:  Checks whether the link to the front of the memory block is correct.

  Arguments:    pMBHead:      Pointer to the header for the memory block to be checked.
                pMBHeadPrev:  Pointer to the header for the memory block before the one to be checked.
                flag:         Flag.

  Returns:      Returns TRUE if the link to the front of the memory block is correct,
                otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    CheckMBlockPrevPtr(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        const NNSiFndExpHeapMBlockHead* pMBHeadPrev,
        u32                             flag
    )
    {
        const BOOL bPrint = 0 != (flag & NNS_FND_HEAP_ERROR_PRINT);

        if (pMBHead->pMBHeadPrev != pMBHeadPrev)
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Wrong link memory block. - address %08X, previous address %08X != %08X\n",
                GetMemCPtrForMBlock(pMBHead), pMBHead->pMBHeadPrev, pMBHeadPrev);
            return FALSE;
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckMBlockNextPtr

  Description:  Checks whether the link to the next memory block is correct.

  Arguments:    pMBHead:      Pointer to the header for the memory block to be checked.
                pMBHeadNext:  Pointer to the header for the memory block after the one to be checked.
                flag:         Flag.

  Returns:      Returns TRUE if the link to the next memory block is correct,
                otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    CheckMBlockNextPtr(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        const NNSiFndExpHeapMBlockHead* pMBHeadNext,
        u32                             flag
    )
    {
        const BOOL bPrint = 0 != (flag & NNS_FND_HEAP_ERROR_PRINT);

        if (pMBHead->pMBHeadNext != pMBHeadNext)
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Wrong link memory block. - address %08X, next address %08X != %08X\n",
                GetMemCPtrForMBlock(pMBHead), pMBHead->pMBHeadNext, pMBHeadNext);
            return FALSE;
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         CheckMBlockLinkTail

  Description:  Checks whether the memory block pointer is at the head or tail of the memory block list.

  Arguments:    pMBHead:      Pointer to the header for the memory block to be checked.
                pMBHeadTail:  Pointer to the memory block at the head or tail of the memory block list.
                headType:     String indicating the head or tail.
                flag:         Flag.

  Returns:      Returns TRUE if the memory block pointer is at the head or tail of the memory block list,
                otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    CheckMBlockLinkTail(
        const NNSiFndExpHeapMBlockHead* pMBHead,
        const NNSiFndExpHeapMBlockHead* pMBHeadTail,
        const char*                     heapType,
        u32                             flag
    )
    {
        const BOOL bPrint = 0 != (flag & NNS_FND_HEAP_ERROR_PRINT);

        if (pMBHead != pMBHeadTail)
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Wrong memory brock list %s pointer. - address %08X, %s address %08X != %08X\n",
                heapType, GetMemCPtrForMBlock(pMBHead), heapType, pMBHead, pMBHeadTail);
            return FALSE;
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         IsValidUsedMBlock

  Description:  Checks whether the memory block being used is appropriate.

  Arguments:    memBlock:  The memory block to be checked.
                heap:      The handle for the extended heap containing the memory block.
                           If NULL is specified, no check is run to see whether
                           the memory block is included in the heap.

  Returns:      Returns TRUE if there is no problem with the specified memory block.
                Returns FALSE if there is a problem.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    static BOOL
    IsValidUsedMBlock(
        const void*         memBlock,
        NNSFndHeapHandle    heap
    )
    {
        NNSiFndHeapHead* pHeapHd = heap;

        if (! memBlock)
        {
            return FALSE;
        }

        return CheckUsedMBlock(GetMBlockHeadCPtr(memBlock), pHeapHd, 0);
    }

// #if ! defined(NNS_FINALROM)
#endif

/* ========================================================================
    External Functions (Non-Public)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndDumpExpHeap

  Description:  Shows internal extended heap information.
                This function is for debugging.

  Arguments:    heap:    Handle for expanded heap

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    void
    NNSi_FndDumpExpHeap(NNSFndHeapHandle heap)
    {
        NNS_ASSERT(IsValidExpHeapHandle(heap));

        {
            u32  usedSize = 0;
            u32  usedCnt = 0;
            u32  freeSize = 0;
            u32  freeCnt = 0;

            NNSiFndHeapHead* pHeapHd = heap;
            NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHandle(pHeapHd);

            NNSi_FndDumpHeapHead(pHeapHd);

            OS_Printf("     attr  address:   size    gid aln   prev_ptr next_ptr\n");   // Header line

            // ---------------- UsedBlock Dump ----------------
            OS_Printf("    (Used Blocks)\n" );

            if (pExpHeapHd->mbUsedList.head == NULL)
            {
                OS_Printf("     NONE\n");
            }
            else
            {
                NNSiFndExpHeapMBlockHead* pMBHead;

                for (pMBHead = pExpHeapHd->mbUsedList.head; pMBHead; pMBHead = pMBHead->pMBHeadNext)
                {
                    if (pMBHead->signature != MBLOCK_USED_SIGNATURE)
                    {
                        OS_Printf("    xxxxx %08x: --------  --- ---  (-------- --------)\nabort\n", pMBHead);
                        break;
                    }

                    OS_Printf("    %s %08x: %8d  %3d %3d  (%08x %08x)\n",
                        GetAllocDirForMBlock(pMBHead) == NNS_FND_EXPHEAP_ALLOC_DIR_REAR ? " rear" : "front",
                        GetMemPtrForMBlock(pMBHead),
                        pMBHead->blockSize,
                        GetGroupIDForMBlock( pMBHead ),
                        GetAlignmentForMBlock( pMBHead ),
                        pMBHead->pMBHeadPrev ? GetMemPtrForMBlock(pMBHead->pMBHeadPrev): NULL,
                        pMBHead->pMBHeadNext ? GetMemPtrForMBlock(pMBHead->pMBHeadNext): NULL
                    );

                    // ---- Use Quantity
                    usedSize += sizeof(NNSiFndExpHeapMBlockHead) + pMBHead->blockSize + GetAlignmentForMBlock(pMBHead);

                    usedCnt ++;
                }
            }

            // ---------------- FreeBlock Dump ----------------
            OS_Printf("    (Free Blocks)\n" );

            if (pExpHeapHd->mbFreeList.head == NULL)
            {
                OS_Printf("     NONE\n" );
            }
            else
            {
                NNSiFndExpHeapMBlockHead* pMBHead;

                for (pMBHead = pExpHeapHd->mbFreeList.head; pMBHead; pMBHead = pMBHead->pMBHeadNext)
                {
                    if (pMBHead->signature != MBLOCK_FREE_SIGNATURE)
                    {
                        OS_Printf("    xxxxx %08x: --------  --- ---  (-------- --------)\nabort\n", pMBHead);
                        break;
                    }

                    OS_Printf("    %s %08x: %8d  %3d %3d  (%08x %08x)\n",
                        " free",
                        GetMemPtrForMBlock(pMBHead),
                        pMBHead->blockSize,
                        GetGroupIDForMBlock( pMBHead ),
                        GetAlignmentForMBlock( pMBHead ),
                        pMBHead->pMBHeadPrev ? GetMemPtrForMBlock(pMBHead->pMBHeadPrev): NULL,
                        pMBHead->pMBHeadNext ? GetMemPtrForMBlock(pMBHead->pMBHeadNext): NULL
                    );

                    freeCnt ++;
                }
            }

            OS_Printf("\n");

            {
                u32 heapSize  = GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd); // Heap size (data region size)
                OS_Printf("    %d / %d bytes (%6.2f%%) used (U:%d F:%d)\n",
                                   usedSize, heapSize, 100.0 * usedSize / heapSize, usedCnt, freeCnt);
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
  Name:         NNS_FndCreateExpHeapEx

  Description:  Creates expanded heap

  Arguments:    startAddress: Start address of heap area
                size:         Size of heap area
                optFlag:      Option flag.

  Returns:      If the function succeeds, a handle for the created expanded heap is returned.
                If the function fails, NNS_FND_HEAP_INVALID_HANDLE is returned.
 *---------------------------------------------------------------------------*/
NNSFndHeapHandle
NNS_FndCreateExpHeapEx(
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
     ||  GetOffsetFromPtr(startAddress, endAddress) < sizeof(NNSiFndHeapHead) + sizeof(NNSiFndExpHeapHead)
                                                        + sizeof(NNSiFndExpHeapMBlockHead) + MIN_ALIGNMENT
    )
    {
        return NNS_FND_HEAP_INVALID_HANDLE;
    }

    {   // Initialization for the Extended heap
        NNSiFndHeapHead* pHeapHd = InitExpHeap(startAddress, endAddress, optFlag);
        return pHeapHd;  // The pointer to the heap header is used as-is for the handle value
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndDestroyExpHeap

  Description:  Destroys the extended heap

  Arguments:    heap: Handle for expanded heap

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndDestroyExpHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    NNSi_FndFinalizeHeap(heap);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromExpHeapEx

  Description:  Allocate memory block from extended heap.
                The memory block alignment can be specified.
                If a negative alignment value is specified, an empty region is searched for from the back of the heap.

  Arguments:    heap:      Handle for extended heap
                size:      Size of memory block to be allocated (in bytes)
                alignment: Alignment of the memory block to be allocated.
                           4, 8, 16, 32, -4, -8, -16 or -32 may be specified.

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                Returns NULL if unsuccessful.
 *---------------------------------------------------------------------------*/
void*
NNS_FndAllocFromExpHeapEx(
    NNSFndHeapHandle    heap,
    u32                 size,
    int                 alignment)
{
    void* memory = NULL;

    NNS_ASSERT(IsValidExpHeapHandle(heap));

    // alignment check
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= 32);

    if (size == 0)
    {
        size = 1;
    }

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);    // The size actually allocated

    if (alignment >= 0)     // Allocates from the front
    {
        memory = AllocFromHead(heap, size, alignment);
    }
    else                    // Allocates from the back
    {
        memory = AllocFromTail(heap, size, -alignment);
    }

    return memory;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndResizeForMBlockExpHeap

  Description:  Changes the size of the memory block allocated from the extended heap.

  Arguments:    heap:     Handle for expanded heap
                memBlock: Pointer to the memory block whose size is to be changed.
                size:     The new size to be allocated (in bytes).

  Returns:      Returns the size of the memory block that has changed if the function is successful.
                Returns 0 if the function fails.
 *---------------------------------------------------------------------------*/
u32
NNS_FndResizeForMBlockExpHeap(
    NNSFndHeapHandle    heap,
    void*               memBlock,
    u32                 size
)
{
    NNSiFndExpHeapHead* pEHHead;
    NNSiFndExpHeapMBlockHead* pMBHead;

    NNS_ASSERT(IsValidExpHeapHandle(heap));
    NNS_ASSERT(IsValidUsedMBlock(memBlock, heap));

    pEHHead = GetExpHeapHeadPtrFromHandle(heap);
    pMBHead = GetMBlockHeadPtr(memBlock);

    size = NNSi_FndRoundUp(size, MIN_ALIGNMENT);
    if (size == pMBHead->blockSize)  // If the block size is not changed
    {
        return size;
    }

    // For expanding the new area
    if (size > pMBHead->blockSize)
    {
        void* crUsedEnd = GetMBlockEndAddr(pMBHead);   // The end address for the block currently being used
        NNSiFndExpHeapMBlockHead* block;

        // Searches for the next free block
        for (block = pEHHead->mbFreeList.head; block; block = block->pMBHeadNext)
        {
            if (block == crUsedEnd)
            {
                break;
            }
        }

        // There is no next free block or the size is inadequate
        if (! block || size > pMBHead->blockSize + sizeof(NNSiFndExpHeapMBlockHead) + block->blockSize)
        {
            return 0;
        }

        {
            NNSiMemRegion rgnNewFree;
            void* oldFreeStart;
            NNSiFndExpHeapMBlockHead* nextBlockPrev;

            // Gets the free block region and temporarily removes the free block
            GetRegionOfMBlock(&rgnNewFree, block);
            nextBlockPrev = RemoveMBlock(&pEHHead->mbFreeList, block);

            oldFreeStart = rgnNewFree.start;
            rgnNewFree.start = AddU32ToPtr(memBlock, size); // The region position to be newly freed

            // If the remainder is smaller than the memory block size
            if (GetOffsetFromPtr(rgnNewFree.start, rgnNewFree.end) < sizeof(NNSiFndExpHeapMBlockHead))
            {
                rgnNewFree.start = rgnNewFree.end;  // It is absorbed into the block being used
            }

            pMBHead->blockSize = GetOffsetFromPtr(memBlock, rgnNewFree.start);  // Changes the target block size

            // If the remainder is equal to or larger than the memory block size
            if (GetOffsetFromPtr(rgnNewFree.start, rgnNewFree.end) >= sizeof(NNSiFndExpHeapMBlockHead))
            {
                (void)InsertMBlock(&pEHHead->mbFreeList, InitFreeMBlock(&rgnNewFree), nextBlockPrev);   // Creates a new free block
            }

            FillAllocMemory(  // Extended partial file
                heap,
                oldFreeStart,
                GetOffsetFromPtr(oldFreeStart, rgnNewFree.start));
        }
    }
    // When shrinking the new area
    else
    {
        NNSiMemRegion rgnNewFree;
        const u32 oldBlockSize = pMBHead->blockSize;

        rgnNewFree.start = AddU32ToPtr(memBlock, size); // The region position to be newly freed
        rgnNewFree.end   = GetMBlockEndAddr(pMBHead);   // The end address for the block currently being used

        pMBHead->blockSize = size;  // Changes the target block size

        if (! RecycleRegion(pEHHead, &rgnNewFree))    // Tries returning the free list
        {
            pMBHead->blockSize = oldBlockSize;  // Restores to original form if failed
        }
    }

    return pMBHead->blockSize;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFreeToExpHeap

  Description:  Returns the memory block to the extended heap.

  Arguments:    heap:     Handle for expanded heap
                memBlock: Pointer to the memory block to be returned.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndFreeToExpHeap(
    NNSFndHeapHandle    heap,
    void*               memBlock
)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    {
        NNSiFndHeapHead* pHeapHd = heap;
        NNSiFndExpHeapHead* pExpHeapHd = GetExpHeapHeadPtrFromHandle(pHeapHd);
        NNSiFndExpHeapMBlockHead* pMBHead = GetMBlockHeadPtr(memBlock);
        NNSiMemRegion region;

        // Is it included in this heap?
        NNS_ASSERT(pHeapHd->heapStart <= memBlock && memBlock < pHeapHd->heapEnd);

        GetRegionOfMBlock(&region, pMBHead);
        (void)RemoveMBlock(&pExpHeapHd->mbUsedList, pMBHead);   // Remove from the list being used
        (void)RecycleRegion(pExpHeapHd, &region);   // Add the specified size from the specified address to the free region
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetTotalFreeSizeForExpHeap

  Description:  Gets the total size of the empty regions of the extended heap.

  Arguments:    heap:     Handle for expanded heap

  Returns:      Returns the total size of the empty regions in the extended heap (in bytes).
 *---------------------------------------------------------------------------*/
u32
NNS_FndGetTotalFreeSizeForExpHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    {
        u32 sumSize = 0;
        NNSiFndExpHeapHead* pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        NNSiFndExpHeapMBlockHead* pMBHead;

        for(pMBHead = pEHHead->mbFreeList.head; pMBHead; pMBHead = pMBHead->pMBHeadNext)
        {
            sumSize += pMBHead->blockSize;
        }

        return sumSize;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocatableSizeForExpHeapEx

  Description:  Gets a region of the maximum allocatable size in the extended heap.
                The memory block alignment can be specified.

  Arguments:    heap:      Handle for extended heap
                alignment: Alignment of the memory block to be allocated.
                           A value of 4, 8, 16, or 32 may be specified.

  Returns:      Returns a region of the maximum allocatable size in the extended heap (in bytes)
 *---------------------------------------------------------------------------*/
u32
NNS_FndGetAllocatableSizeForExpHeapEx(
    NNSFndHeapHandle    heap,
    int                 alignment
)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    // alignment check
    NNS_ASSERT(alignment % MIN_ALIGNMENT == 0);
    NNS_ASSERT(MIN_ALIGNMENT <= abs(alignment) && abs(alignment) <= 32);

    alignment = abs(alignment); // Convert to a positive value just to be sure

    {
        NNSiFndExpHeapHead* pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        u32 maxSize = 0;
        u32 offsetMin = 0xFFFFFFFF;
        NNSiFndExpHeapMBlockHead* pMBlkHd;

        for (pMBlkHd = pEHHead->mbFreeList.head; pMBlkHd; pMBlkHd = pMBlkHd->pMBHeadNext)
        {
            // Memory block position including the alignment
            void* baseAddress = NNSi_FndRoundUpPtr(GetMemPtrForMBlock(pMBlkHd), alignment);

            if (NNSiGetUIntPtr(baseAddress) < NNSiGetUIntPtr(GetMBlockEndAddr(pMBlkHd)))
            {
                const u32 blockSize = GetOffsetFromPtr(baseAddress, GetMBlockEndAddr(pMBlkHd));
                // Empty area due to the alignment
                const u32 offset = GetOffsetFromPtr(GetMemPtrForMBlock(pMBlkHd), baseAddress);

                /*
                    If the size is large, or the size is the same but wasted space is small,
                    the memory block is updated
                 */
                if ( maxSize < blockSize
                 ||  (maxSize == blockSize && offsetMin > offset)
                )
                {
                    maxSize = blockSize;
                    offsetMin= offset;
                }
            }
        }

        return maxSize;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndSetAllocModeForExpHeap

  Description:  Sets the memory allocation mode for the expanded heap.

  Arguments:    heap:  Handle for expanded heap
                mode:  Memory allocation mode.

  Returns:      Returns the memory allocation mode for the previous expanded heap.
 *---------------------------------------------------------------------------*/
u16
NNS_FndSetAllocModeForExpHeap(
    NNSFndHeapHandle    heap,
    u16                 mode
)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    {
        NNSiFndExpHeapHead *const pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        u16 oldMode = GetAllocMode(pEHHead);
        SetAllocMode(pEHHead, mode);

        return oldMode;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocModeForExpHeap

  Description:  Gets the memory allocation mode for the extended heap

  Arguments:    heap:    Handle for expanded heap

  Returns:      Returns the memory allocation mode for the extended heap.
 *---------------------------------------------------------------------------*/
u16
NNS_FndGetAllocModeForExpHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));
    return GetAllocMode(GetExpHeapHeadPtrFromHandle(heap));
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndSetGroupIDForExpHeap

  Description:  Sets the group ID for the extended heap.

  Arguments:    heap:    Handle for expanded heap
                groupID: The group ID value to be set.

  Returns:      Returns the previous group ID value.
 *---------------------------------------------------------------------------*/
u16
NNS_FndSetGroupIDForExpHeap(
    NNSFndHeapHandle    heap,
    u16                 groupID)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));
    NNS_ASSERT(groupID <= MAX_GROUPID);

    {
        NNSiFndExpHeapHead* pEHHead = GetExpHeapHeadPtrFromHandle(heap);
        u16 oldGroupID = pEHHead->groupID;
        pEHHead->groupID = groupID;

        return oldGroupID;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetGroupIDForExpHeap

  Description:  Gets the group ID for the extended heap.

  Arguments:    heap:    Handle for expanded heap

  Returns:      Returns the group ID value.
 *---------------------------------------------------------------------------*/
u16
NNS_FndGetGroupIDForExpHeap(NNSFndHeapHandle heap)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));

    return GetExpHeapHeadPtrFromHandle(heap)->groupID;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndVisitAllocatedForExpHeap

  Description:  Causes the function specified by the user to be called
                for all the memory blocks allocated from the extended heap.
                The visitor function is called on these memory blocks in the order they were allocated.

                The visitor type NNSFndHeapVisitor is defined as below.

                    typedef void (*NNSFndHeapVisitor)(
                                    void*               memBlock,
                                    NNSFndHeapHandle    heap,
                                    u32                 userParam);

                                        memBlock:   Pointer to the memory block.
                                        heap:       Heap including the memory block.
                                        userParam:  User parameter.

  Arguments:    heap:       Handle for expanded heap
                visitor:    Function called for each memory block.
                userParam:  User-specified parameter to be passed to the visitor function

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_FndVisitAllocatedForExpHeap(
    NNSFndHeapHandle    heap,
    NNSFndHeapVisitor   visitor,
    u32                 userParam
)
{
    NNS_ASSERT(IsValidExpHeapHandle(heap));
    SDK_NULL_ASSERT(visitor);

    {
        NNSiFndExpHeapMBlockHead* pMBHead = GetExpHeapHeadPtrFromHandle(heap)->mbUsedList.head;

        while (pMBHead)
        {
            NNSiFndExpHeapMBlockHead* pMBHeadNext = pMBHead->pMBHeadNext;
            (*visitor)(GetMemPtrForMBlock(pMBHead), heap, userParam);
            pMBHead = pMBHeadNext;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetSizeForMBlockExpHeap

  Description:  Gets the size of the memory block allocated from the extended heap.

  Arguments:    memBlock:  Pointer to the memory block for getting the size.

  Returns:      Returns the size of the specified memory block (in bytes).
 *---------------------------------------------------------------------------*/
u32
NNS_FndGetSizeForMBlockExpHeap(const void* memBlock)
{
    NNS_ASSERT(IsValidUsedMBlock(memBlock, NULL));

    return GetMBlockHeadCPtr(memBlock)->blockSize;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetGroupIDForMBlockExpHeap

  Description:  Gets the group ID for the memory block allocated from the extended heap.

  Arguments:    memBlock:  Pointer to the memory block for getting the group ID.

  Returns:      Returns the group ID for the specified memory block.
 *---------------------------------------------------------------------------*/
u16
NNS_FndGetGroupIDForMBlockExpHeap(const void* memBlock)
{
    NNS_ASSERT(IsValidUsedMBlock(memBlock, NULL));

    return GetGroupIDForMBlock(GetMBlockHeadCPtr(memBlock));
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocDirForMBlockExpHeap

  Description:  Gets the allocation direction for the memory block allocated from the extended heap.

  Arguments:    memBlock:  Pointer to the memory block.

  Returns:      Returns the allocation direction for the specified memory block.
 *---------------------------------------------------------------------------*/
u16
NNS_FndGetAllocDirForMBlockExpHeap(const void* memBlock)
{
    NNS_ASSERT(IsValidUsedMBlock(memBlock, NULL));

    return GetAllocDirForMBlock(GetMBlockHeadCPtr(memBlock));
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCheckExpHeap

  Description:  Checks whether the extended heap has been destroyed.

  Arguments:    heap:     Handle for expanded heap
                optFlag:  Flag.

  Returns:      Returns TRUE if the heap is normal.
                Returns FALSE if the heap has an error.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    BOOL
    NNS_FndCheckExpHeap(
        NNSFndHeapHandle    heap,
        u32                 optFlag
    )
    {
        const BOOL bPrint = 0 != (optFlag & NNS_FND_HEAP_ERROR_PRINT);
        u32  totalBytes  = 0;

        if (! IsValidExpHeapHandle(heap))
        {
            HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Invalid heap handle. - %08X\n", heap);
            return FALSE;
        }

        {
            NNSiFndHeapHead *const pHeapHd = heap;
            NNSiFndExpHeapHead *const pExpHeapHd = GetExpHeapHeadPtrFromHeapHead(pHeapHd);
            NNSiFndExpHeapMBlockHead* pMBHead = NULL;
            NNSiFndExpHeapMBlockHead* pMBHeadPrev = NULL;

            // Block used
            for (pMBHead = pExpHeapHd->mbUsedList.head; pMBHead; pMBHeadPrev = pMBHead, pMBHead = pMBHead->pMBHeadNext)
            {
                if ( ! CheckUsedMBlock(pMBHead, pHeapHd, optFlag)
                  || ! CheckMBlockPrevPtr(pMBHead, pMBHeadPrev, optFlag)   // Is the pointer to the previous block the same as the pointer to the memory block in the previous loop?
                )
                {
                    return FALSE;
                }

                // calculates size occupied
                totalBytes += sizeof(NNSiFndExpHeapMBlockHead) + pMBHead->blockSize + GetAlignmentForMBlock(pMBHead);
            }

            if (! CheckMBlockLinkTail(pMBHeadPrev, pExpHeapHd->mbUsedList.tail, "tail", optFlag))  // Is the last block referring to the pointer to the final block?
            {
                return FALSE;
            }

            // free block
            pMBHead = NULL;
            pMBHeadPrev = NULL;
            for (pMBHead = pExpHeapHd->mbFreeList.head; pMBHead; pMBHeadPrev = pMBHead, pMBHead = pMBHead->pMBHeadNext)
            {
                if ( ! CheckFreeMBlock(pMBHead, pHeapHd, optFlag)
                  || ! CheckMBlockPrevPtr(pMBHead, pMBHeadPrev, optFlag)   // Is the pointer to the previous block the same as the pointer to the memory block in the previous loop?
                )
                {
                    return FALSE;
                }

                // calculates size occupied
                totalBytes += sizeof(NNSiFndExpHeapMBlockHead) + pMBHead->blockSize;
            }

            if (! CheckMBlockLinkTail(pMBHeadPrev, pExpHeapHd->mbFreeList.tail, "tail", optFlag))  // Is the last block referring to the pointer to the final block?
            {
                return FALSE;
            }

            // Display all results.
            if (totalBytes != GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd))
            {
                HEAP_WARNING(bPrint, "[NNS Foundation " "Exp" " Heap]" " Incorrect total memory block size. - heap size %08X, sum size %08X\n",
                    GetOffsetFromPtr(pHeapHd->heapStart, pHeapHd->heapEnd), totalBytes);
                return FALSE;
            }

            return TRUE;
        }
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCheckForMBlockExpHeap

  Description:  This function checks if the memory block of the extended heap was destroyed.

  Arguments:    memBlock:  Pointer to the memory block checked.
                heap:      Handle for expanded heap
                optFlag:   Flag.

  Returns:      If the memory block is valid, TRUE.
                If the memory block has an error, FALSE.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    BOOL
    NNS_FndCheckForMBlockExpHeap(
        const void*         memBlock,
        NNSFndHeapHandle    heap,
        u32                 optFlag
    )
    {
        const NNSiFndExpHeapMBlockHead* pMBHead = NULL;
        NNSiFndHeapHead *const pHeapHd = heap;

        if (! memBlock)
        {
            return FALSE;
        }

        pMBHead = GetMBlockHeadCPtr(memBlock);

        if (! CheckUsedMBlock(pMBHead, pHeapHd, optFlag))
        {
            return FALSE;
        }

        if (pMBHead->pMBHeadPrev)
        {
            if ( ! CheckUsedMBlock(pMBHead->pMBHeadPrev, pHeapHd, optFlag)     // Check of signature and size of previous block.
              || ! CheckMBlockNextPtr(pMBHead->pMBHeadPrev, pMBHead, optFlag)  // Is the pointer for the next block of the previous block pointing to itself?
            )
            {
                return FALSE;
            }
        }
        else
        {
            if (pHeapHd)
            {
                // If prev is NULL the head pointer of the list should be pointing to itself.
                if (! CheckMBlockLinkTail(pMBHead, GetExpHeapHeadPtrFromHeapHead(pHeapHd)->mbUsedList.head, "head", optFlag))
                {
                    return FALSE;
                }
            }
        }

        if (pMBHead->pMBHeadNext)
        {
            if ( ! CheckUsedMBlock(pMBHead->pMBHeadNext, pHeapHd, optFlag)     // Check of signature and size of next block.
              || ! CheckMBlockPrevPtr(pMBHead->pMBHeadNext, pMBHead, optFlag)  // Is the pointer for the previous block of the next block pointing to itself?
            )
            {
                return FALSE;
            }
        }
        else
        {
            if (pHeapHd)
            {
                // If next is NULL the tail pointer of the list should be pointing to itself.
                if (! CheckMBlockLinkTail(pMBHead, GetExpHeapHeadPtrFromHeapHead(pHeapHd)->mbUsedList.tail, "tail", optFlag))
                {
                    return FALSE;
                }
            }
        }

        return TRUE;
    }

// #if ! defined(NNS_FINALROM)
#endif

