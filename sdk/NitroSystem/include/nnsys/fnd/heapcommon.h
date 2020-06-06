/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     heapcommon.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: heapcommon.h,v $
  Revision 1.11  01/21/2005 04:59:49  kitamurs
  Changed year

  Revision 1.10  10/19/2004 01:22:18  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.9  10/15/2004 06:06:33  kitamurs
  Changed argument name

  Revision 1.8  08/20/2004 00:08:54  kitamurs
  Added check functions

  Revision 1.7  06/22/2004 08:24:38  kitamurs
  In the Dump function, made the package empty out during FINALROM.

  Revision 1.6  03/26/2004 06:27:37  kitamurs
  Revised function to set/get fill value

  Revision 1.5  03/22/2004 01:26:56  kitamurs
  Made Dump function common among heaps

  Revision 1.4  03/19/2004 00:07:35  kitamurs
  Fixed comments.

  Revision 1.3  03/05/2004 00:06:55  kitamurs
  Implemented memory fill feature

  Revision 1.2  03/01/2004 00:16:17  kitamurs
  Implemented hierarchical structure of heap

  Revision 1.1  02/24/2004 02:46:56  kitamurs
  Implemented expanded heap

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_HEAPCOMMON_H_
#define NNS_FND_HEAPCOMMON_H_

#include <nitro/types.h>
#include <nnsys/fnd/list.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    Macro constant
   ======================================================================== */

// Invalid heap handle
#define NNS_FND_HEAP_INVALID_HANDLE     NULL

// Default alignment size when memory is allocated from  heap
#define NNS_FND_HEAP_DEFAULT_ALIGNMENT    4

// Signature of expanded heap
#define NNSI_EXPHEAP_SIGNATURE    ('EXPH')

// Signature of frame heap
#define NNSI_FRMHEAP_SIGNATURE    ('FRMH')

// Signature of unit heap
#define NNSI_UNTHEAP_SIGNATURE    ('UNTH')


/* ------------------------------------------------------------------------
    Fill-related
   ------------------------------------------------------------------------ */

// Zero out memory when memory is allocated.
#define NNS_FND_HEAP_OPT_0_CLEAR        (1 <<0)

// Memory fill when heap is created, memory is allocated, or memory is freed.
#define NNS_FND_HEAP_OPT_DEBUG_FILL     (1 <<1)


/* ------------------------------------------------------------------------
    Heap check related
   ------------------------------------------------------------------------ */

//  If this bit stands, output error
#define NNS_FND_HEAP_ERROR_PRINT        (1 <<0)


/* ========================================================================
    enum constant
   ======================================================================== */

enum {
    NNS_FND_HEAP_FILL_NOUSE,    // When debug fill is not used
    NNS_FND_HEAP_FILL_ALLOC,    // When debug fill is allocated
    NNS_FND_HEAP_FILL_FREE,     // When debug fill is freed

    NNS_FND_HEAP_FILL_MAX
};


/* =======================================================================
    Type Definition
   ======================================================================== */

typedef struct NNSiFndHeapHead NNSiFndHeapHead;

// Header common among heaps
struct NNSiFndHeapHead
{
    u32             signature;

    NNSFndLink      link;
    NNSFndList      childList;

    void*           heapStart;      // Heap start address
    void*           heapEnd;        // Heap end (+1) address

    u32             attribute;      // Attribute
                                    // [8:Option flag]
};

typedef NNSiFndHeapHead* NNSFndHeapHandle;   // Type to represent heap handle


/* ========================================================================
    Macro functions
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetHeapStartAddress

  Description:  Get start address of memory area used by heap

  Arguments:    heap:  Heap handle

  Returns:      Return start address of memory area used by heap
 *---------------------------------------------------------------------------*/
#define             NNS_FndGetHeapStartAddress(heap) \
                        ((void*)(heap))

/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetHeapEndAddress

  Description:  Get end address +1 of memory area used by heap

  Arguments:    heap:  Heap handle

  Returns:      Return end address +1 of memory area used by heap
 *---------------------------------------------------------------------------*/
#define             NNS_FndGetHeapEndAddress(heap) \
                        (((NNSiFndHeapHead*)(heap))->heapEnd)


/* =======================================================================
    Function Prototypes
   ======================================================================== */

NNSFndHeapHandle    NNS_FndFindContainHeap(
                        const void* memBlock);

#if defined(NNS_FINALROM)
    #define             NNS_FndDumpHeap(heap)				((void)0)
#else
	void                NNS_FndDumpHeap(
	                        NNSFndHeapHandle heap);
#endif

#if defined(NNS_FINALROM)
    #define             NNS_FndSetFillValForHeap(type, val) (0)
#else
    u32                 NNS_FndSetFillValForHeap(
                            int     type,
                            u32     val);
#endif

#if defined(NNS_FINALROM)
    #define             NNS_FndGetFillValForHeap(type)      (0)
#else
    u32                 NNS_FndGetFillValForHeap(
                            int     type);
#endif


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_HEAPCOMMON_H_*/
#endif

