/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     expheap.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: expheap.h,v $
  Revision 1.8  01/21/2005 04:59:49 AM  kitamurs
  Changed year

  Revision 1.7  10/15/2004 07:28:52 AM  kitamurs
  - Changed argument names
  - Changed SDK_... to NNS_...
  Changed to return TRUE when the check function is NNS_FINALROM.

  Revision 1.6  08/20/2004 12:08:54 AM  kitamurs
  Added check functions

  Revision 1.5  06/22/2004 08:24:38 AM  kitamurs
  In the Dump function, made the package empty out during FINALROM.

  Revision 1.4  03/22/2004 01:26:56 AM  kitamurs
  Made Dump function common among heaps

  Revision 1.3  03/05/2004 12:04:51 AM  kitamurs
  - Changed the type of heap handle
  - Changed so that flag can be set when heap is created.

  Revision 1.2  03/01/2004 12:14:01 AM  kitamurs
  Cleaned up unnecessary or overlapping portion

  Revision 1.1  02/24/2004 02:46:56 AM  kitamurs
  Implemented expanded heap

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_EXPHEAP_H_
#define NNS_FND_EXPHEAP_H_

#include <nnsys/fnd/heapcommon.h>

#ifdef __cplusplus
extern "C" {
#endif


/* =======================================================================
    Constant definitions
   ======================================================================== */

// Memory allocation direction
enum
{
    NNS_FND_EXPHEAP_ALLOC_DIR_FRONT,    // Allocate from front
    NNS_FND_EXPHEAP_ALLOC_DIR_REAR      // Allocate from rear
};

// Memory allocation mode
enum
{
    /*
        If this attribute value is set, 
        the software will wait for a memory region of the same size or larger than that 
        to be allocated and allocate a memory block from the first available memory region found.
    */
    NNS_FND_EXPHEAP_ALLOC_MODE_FIRST,

    /*
        If this attribute value is set, 
        software searches for an empty area that is closest in size to the memory block to be allocated  
        and allocates a memory block from that empty area. 
    */
    NNS_FND_EXPHEAP_ALLOC_MODE_NEAR
};


/* =======================================================================
    Type Definitions
   ======================================================================== */

typedef struct NNSiFndExpHeapMBlockHead NNSiFndExpHeapMBlockHead;

// Header information for memory block
struct NNSiFndExpHeapMBlockHead
{
    u16                         signature;      // Signature
    u16                         attribute;      // Attributes
                                                // [8:groupID]
                                                // [7:alignment]
                                                // [1:temporary flag]

    u32                         blockSize;      // block size (data area only)

    NNSiFndExpHeapMBlockHead*   pMBHeadPrev;    // Previous block
    NNSiFndExpHeapMBlockHead*   pMBHeadNext;    // Next block
};

typedef struct NNSiFndExpMBlockList NNSiFndExpMBlockList;

// Memory block list
struct NNSiFndExpMBlockList
{
    NNSiFndExpHeapMBlockHead*   head;   // Pointer to the memory block linked to the head of the expanded heap
    NNSiFndExpHeapMBlockHead*   tail;   // Pointer to the memory block linked to the tail of the expanded heap
};

typedef struct NNSiFndExpHeapHead NNSiFndExpHeapHead;

// Header information for expanded heap
struct NNSiFndExpHeapHead
{
    NNSiFndExpMBlockList        mbFreeList;     // Free list
    NNSiFndExpMBlockList        mbUsedList;     // Used list

    u16                         groupID;        // Current group ID (lower 8 bits only)
    u16                         feature;        // Attribute
};

// Callback function type called for every memory block
typedef void        (*NNSFndHeapVisitor)(
                        void*               memBlock,
                        NNSFndHeapHandle    heap,
                        u32                 userParam);

/* =======================================================================
    Macro functions
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndCreateExpHeap

  Description:  Creates expanded heap

  Arguments:    startAddress: Start address of heap area
                size:         Size of heap area

  Returns:      If the function succeeds, a handle for the created expanded heap is returned.
                If the function fails, NNS_FND_HEAP_INVALID_HANDLE is returned.
 *---------------------------------------------------------------------------*/
#define             NNS_FndCreateExpHeap(startAddress, size) \
                        NNS_FndCreateExpHeapEx(startAddress, size, 0)

/*---------------------------------------------------------------------------*
  Name:         NNS_FndAllocFromExpHeap

  Description:  Allocates a memory block from the expanded heap.
                Alignment of memory block is 4-byte fixed.

  Arguments:    heap:   Handle for expanded heap
                size:   Size of memory block to be allocated (in bytes)

  Returns:      Returns the pointer
                to the allocated memory block if the allocation is successful.
                Returns NULL if unsuccessful.
 *---------------------------------------------------------------------------*/
#define             NNS_FndAllocFromExpHeap(heap, size) \
                        NNS_FndAllocFromExpHeapEx(heap, size, NNS_FND_HEAP_DEFAULT_ALIGNMENT)

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetAllocatableSizeForExpHeap

  Description:  Gets a memory block of the maximum allocatable size from the expanded heap.
                Alignment of the memory block is 4-byte fixed.

  Arguments:    heap:     Handle for expanded heap

  Returns:      Returns the maximum allocatable size from the expanded heap (in bytes).
 *---------------------------------------------------------------------------*/
#define             NNS_FndGetAllocatableSizeForExpHeap(heap) \
                        NNS_FndGetAllocatableSizeForExpHeapEx(heap, NNS_FND_HEAP_DEFAULT_ALIGNMENT)


/* =======================================================================
    Function Prototypes
   ======================================================================== */

#if ! defined(NNS_FINALROM)

    void                NNSi_FndDumpExpHeap(
                            NNSFndHeapHandle    heap);

// #if ! defined(NNS_FINALROM)
#endif

NNSFndHeapHandle    NNS_FndCreateExpHeapEx(
                        void*   startAddress,
                        u32     size,
                        u16     optFlag);

void                NNS_FndDestroyExpHeap(
                        NNSFndHeapHandle    heap);

void*               NNS_FndAllocFromExpHeapEx(
                        NNSFndHeapHandle    heap,
                        u32                 size,
                        int                 alignment);

u32                 NNS_FndResizeForMBlockExpHeap(
                        NNSFndHeapHandle    heap,
                        void*               memBlock,
                        u32                 size);

void                NNS_FndFreeToExpHeap(
                        NNSFndHeapHandle    heap,
                        void*               memBlock);

u32                 NNS_FndGetTotalFreeSizeForExpHeap(
                        NNSFndHeapHandle    heap);

u32                 NNS_FndGetAllocatableSizeForExpHeapEx(
                        NNSFndHeapHandle    heap,
                        int                 alignment);

u16                 NNS_FndSetAllocModeForExpHeap(
                        NNSFndHeapHandle    heap,
                        u16                 mode);

u16                 NNS_FndGetAllocModeForExpHeap(
                        NNSFndHeapHandle    heap);

u16                 NNS_FndSetGroupIDForExpHeap(
                        NNSFndHeapHandle    heap,
                        u16                 groupID);

u16                 NNS_FndGetGroupIDForExpHeap(
                        NNSFndHeapHandle    heap);

void                NNS_FndVisitAllocatedForExpHeap(
                        NNSFndHeapHandle    heap,
                        NNSFndHeapVisitor   visitor,
                        u32                 userParam);

u32                 NNS_FndGetSizeForMBlockExpHeap(
                        const void*         memBlock);

u16                 NNS_FndGetGroupIDForMBlockExpHeap(
                        const void*         memBlock);

u16                 NNS_FndGetAllocDirForMBlockExpHeap(
                        const void*         memBlock);

#if defined(NNS_FINALROM)

    #define             NNS_FndCheckExpHeap(heap, optFlag)                      (TRUE)

    #define             NNS_FndCheckForMBlockExpHeap(memBlock, heap, optFlag)   (TRUE)

// #if defined(NNS_FINALROM)
#else

    BOOL                NNS_FndCheckExpHeap(
                            NNSFndHeapHandle    heap,
                            u32                 optFlag);

    BOOL                NNS_FndCheckForMBlockExpHeap(
                            const void*         memBlock,
                            NNSFndHeapHandle    heap,
                            u32                 optFlag);

// #if defined(NNS_FINALROM)
#endif


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_EXPHEAP_H_*/
#endif

