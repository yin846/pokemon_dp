/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     heapcommoni.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: heapcommoni.h,v $
  Revision 1.4  01/21/2005 04:52:16  kitamurs
  Changed year

  Revision 1.3  10/15/2004 08:48:26  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.2  08/20/2004 00:08:21  kitamurs
  Added check functions

  Revision 1.1  07/26/2004 04:55:35  kitamurs
  Moved location -> build/libraries/fnd/include.

  Revision 1.4  03/26/2004 06:27:07  kitamurs
  Revised function to set/get fill value

  Revision 1.3  03/22/2004 01:26:56  kitamurs
  Made Dump function common among heaps

  Revision 1.2  03/05/2004 00:07:58  kitamurs
  Implemented memory fill feature

  Revision 1.1  03/01/2004 00:15:44  kitamurs
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_FND_HEAPCOMMONI_H_
#define NNS_FND_HEAPCOMMONI_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif


/* =======================================================================
    Type Definition
   ======================================================================== */

typedef s32 NNSiIntPtr;     // Signed integer type mutually convertible with void* pointer
typedef u32 NNSiUIntPtr;    // Signed integer type mutually convertible with void* pointer


/* ========================================================================
    Macro functions
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndRoundUp

  Description:  Rounds up to align with specified boundary.

  Arguments:    value:      Targeted data
                alignment:  Boundary value

  Returns:      Returns a value that has been rounded up to the specified boundary value.
 *---------------------------------------------------------------------------*/
#define NNSi_FndRoundUp(value, alignment) \
    (((value) + (alignment-1)) & ~(alignment-1))

#define NNSi_FndRoundUpPtr(ptr, alignment) \
    ((void*)NNSi_FndRoundUp(NNSiGetUIntPtr(ptr), alignment))

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndRoundDown

  Description:  Rounds down to align with specified boundary.

  Arguments:    value:      Targeted data
                alignment:  Boundary value

  Returns:      Returns a value that has been rounded down to the specified boundary.
 *---------------------------------------------------------------------------*/
#define NNSi_FndRoundDown(value, alignment) \
    ((value) & ~(alignment-1))

#define NNSi_FndRoundDownPtr(ptr, alignment) \
    ((void*)NNSi_FndRoundDown(NNSiGetUIntPtr(ptr), alignment))

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndGetBitValue

  Description:  Acquires a value at a specific bit position.

  Arguments:    data:  Data that contains the bit data to be acquired
                st:    Start bit (start from 0)
                bits:  Number of bits (maximum 31)

  Returns:      Returns the value at a specific bit position.
 *---------------------------------------------------------------------------*/
#define     NNSi_FndGetBitValue(data, st, bits) \
    (((data) >>(st)) & ((1 <<(bits)) -1))

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndSetBitValue

  Description:  Sets a value at a specific bit position.

  Arguments:    data:  The variable that holds the bit data that is to be set
                st:    Start bit (start from 0)
                bits:  Number of bits (maximum 31)
                val:   The bit data to be set

  Returns:      None.
 *---------------------------------------------------------------------------*/
#define     NNSi_FndSetBitValue(data, st, bits, val)                        \
                do                                                          \
                {                                                           \
                    u32 maskBits = (u32)((1 <<(bits)) -1);                   \
                    u32 newVal = (val) & maskBits; /* Mask for safety*/    \
                    (void)(maskBits <<= st);                                 \
                    (data) &= ~maskBits; /* Clears a setting region*/        \
                    (data) |= newVal <<(st);                                 \
                } while(FALSE);


/* ========================================================================
    inline function
   ======================================================================== */

/* ------------------------------------------------------------------------
    Pointer Operations
   ------------------------------------------------------------------------ */

static inline NNSiUIntPtr
NNSiGetUIntPtr(const void* ptr)
{
    return (NNSiUIntPtr)ptr;
}

static inline u32
GetOffsetFromPtr(const void* start, const void* end)
{
    return NNSiGetUIntPtr(end) - NNSiGetUIntPtr(start);
}

static inline void*
AddU32ToPtr(void* ptr, u32 val)
{
    return (void*)( NNSiGetUIntPtr(ptr) + val );
}

static inline const void*
AddU32ToCPtr(const void* ptr, u32 val)
{
    return (const void*)( NNSiGetUIntPtr(ptr) + val );
}

static inline void*
SubU32ToPtr(void* ptr, u32 val)
{
    return (void*)( NNSiGetUIntPtr(ptr) - val );
}

static inline const void*
SubU32ToCPtr(const void* ptr, u32 val)
{
    return (const void*)( NNSiGetUIntPtr(ptr) - val );
}

static inline int
ComparePtr(const void* a, const void* b)
{
    const u8* wa = a;
    const u8* wb = b;

    return wa - wb;
}


static inline u16
GetOptForHeap(const NNSiFndHeapHead* pHeapHd)
{
    return (u16)NNSi_FndGetBitValue(pHeapHd->attribute, 0, 8);
}

static inline void
SetOptForHeap(
    NNSiFndHeapHead*    pHeapHd,
    u16                 optFlag
)
{
    NNSi_FndSetBitValue(pHeapHd->attribute, 0, 8, optFlag);
}


/* ------------------------------------------------------------------------
    Memory fill
   ------------------------------------------------------------------------ */

static inline void
FillAllocMemory(
    NNSiFndHeapHead*    pHeapHd,
    void*               address,
    u32                 size
)
{

    if (GetOptForHeap(pHeapHd) & NNS_FND_HEAP_OPT_0_CLEAR)
    {
        MI_CpuFill32(address, 0, size);
    }
    else
    {
        #if ! defined(NNS_FINALROM)
            if (GetOptForHeap(pHeapHd) & NNS_FND_HEAP_OPT_DEBUG_FILL)
            {
                MI_CpuFill32(address, NNS_FndGetFillValForHeap(NNS_FND_HEAP_FILL_ALLOC), size);
            }
        #endif
    }
}

#if defined(NNS_FINALROM)
    #define FillFreeMemory(pHeapHd, address, size)  ((void) 0)
#else
    static inline void
    FillFreeMemory(
        NNSiFndHeapHead*    pHeapHd,
        void*               address,
        u32                 size
    )
    {
        if (GetOptForHeap(pHeapHd) & NNS_FND_HEAP_OPT_DEBUG_FILL)
        {
            MI_CpuFill32(address, NNS_FndGetFillValForHeap(NNS_FND_HEAP_FILL_FREE), size);
        }
    }
#endif

#if defined(NNS_FINALROM)
    #define FillNoUseMemory(pHeapHd, address, size)  ((void) 0)
#else
    static inline void
    FillNoUseMemory(
        NNSiFndHeapHead*    pHeapHd,
        void*               address,
        u32                 size
    )
    {
        if (GetOptForHeap(pHeapHd) & NNS_FND_HEAP_OPT_DEBUG_FILL)
        {
            MI_CpuFill32(address, NNS_FndGetFillValForHeap(NNS_FND_HEAP_FILL_NOUSE), size);
        }
    }
#endif


/* =======================================================================
    Function Prototypes
   ======================================================================== */

void        NNSi_FndInitHeapHead(
                NNSiFndHeapHead*    pHeapHd,
                u32                 signature,
                void*               heapStart,
                void*               heapEnd,
                u16                 optFlag);

void        NNSi_FndFinalizeHeap(
                NNSiFndHeapHead* pHeapHd);

void        NNSi_FndDumpHeapHead(
                NNSiFndHeapHead* pHeapHd);

#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_FND_HEAPCOMMONI_H_*/
#endif

