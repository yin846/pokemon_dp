/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     allocator.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: allocator.h,v $
  Revision 1.5  01/21/2005 04:59:49 AM  kitamurs
  Changed year

  Revision 1.4  07/22/2004 04:06:44 AM  kitamurs
  defaultAlignment -> alignment

  Revision 1.3  07/21/2004 10:06:49 AM  kitamurs
  Added for frame heap, unit heap, SDK heap

  Revision 1.2  07/21/2004 05:32:36 AM  kitamurs
  Corrected which was missing'_'

  Revision 1.1  07/21/2004 05:12:43 AM  kitamurs
  Registration of the initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_ALLOCATOR_H_
#define NNS_FND_ALLOCATOR_H_

#include <nnsys/fnd/heapcommon.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    Type Definition
   ======================================================================== */

typedef struct NNSFndAllocator NNSFndAllocator;

typedef void*   (*NNSFndFuncAllocatorAlloc)(
                    NNSFndAllocator*    pAllocator,
                    u32                 size);

typedef void    (*NNSFndFuncAllocatorFree)(
                    NNSFndAllocator*    pAllocator,
                    void*               memBlock);

typedef struct NNSFndAllocatorFunc NNSFndAllocatorFunc;

struct NNSFndAllocatorFunc
{
    NNSFndFuncAllocatorAlloc    pfAlloc;
    NNSFndFuncAllocatorFree     pfFree;
};

struct NNSFndAllocator
{
    NNSFndAllocatorFunc const * pFunc;
    void*                       pHeap;
    u32                         heapParam1;
    u32                         heapParam2;
};


/* ========================================================================
    Function Prototype
   ======================================================================== */

void*           NNS_FndAllocFromAllocator(
                    NNSFndAllocator*    pAllocator,
                    u32                 size);

void            NNS_FndFreeToAllocator(
                    NNSFndAllocator*    pAllocator,
                    void*               memBlock);

void            NNS_FndInitAllocatorForExpHeap(
                    NNSFndAllocator*    pAllocator,
                    NNSFndHeapHandle    heap,
                    int                 alignment);

void            NNS_FndInitAllocatorForFrmHeap(
                    NNSFndAllocator*    pAllocator,
                    NNSFndHeapHandle    heap,
                    int                 alignment);

void            NNS_FndInitAllocatorForUnitHeap(
                    NNSFndAllocator*    pAllocator,
                    NNSFndHeapHandle    heap);

void            NNS_FndInitAllocatorForSDKHeap(
                    NNSFndAllocator*    pAllocator,
                    OSArenaId           id,
                    OSHeapHandle        heap);


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_ALLOCATOR_H_*/
#endif

