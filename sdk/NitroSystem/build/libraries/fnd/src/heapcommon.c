/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Foundation library
  File:     heapcommon.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: heapcommon.c,v $
  Revision 1.13  01/21/2005 04:55:27 AM  kitamurs
  Changed year

  Revision 1.12  10/15/2004 08:49:14 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.11  10/15/2004 06:30:59 AM  kitamurs
  Changed so range of arguments is checked.

  Revision 1.10  10/15/2004 06:10:44 AM  kitamurs
  Added description of arguments.

  Revision 1.9  07/26/2004 04:56:48 AM  kitamurs
  Moved the location of heapcommoni.h

  Revision 1.8  06/22/2004 08:24:50 AM  kitamurs
  In the Dump function, made the package empty out during FINALROM.

  Revision 1.7  03/26/2004 10:21:25 AM  kitamurs
  Bundled the no use initialization in the common portion

  Revision 1.6  03/26/2004 06:25:21 AM  kitamurs
  Revised function to set/get fill value

  Revision 1.5  03/22/2004 01:26:49 AM  kitamurs
  Made Dump function common among heaps

  Revision 1.4  03/15/2004 08:28:13 AM  kitamurs
  Added initialization of heap child list since it was missing

  Revision 1.3  03/05/2004 12:07:02 AM  kitamurs
  Implemented memory fill feature

  Revision 1.2  03/01/2004 12:16:24 AM  kitamurs
  Implemented hierarchical structure of heap

  Revision 1.1  02/24/2004 02:46:56 AM  kitamurs
  Implemented expanded heap

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/fnd/heapcommon.h>
#include <nnsys/fnd/expheap.h>
#include <nnsys/fnd/frameheap.h>
#include <nnsys/fnd/unitheap.h>
#include "heapcommoni.h"


/* ========================================================================
    static variables
   ======================================================================== */

/* ------------------------------------------------------------------------
    list related
   ------------------------------------------------------------------------ */

static NNSFndList sRootList;                // root heap list
static BOOL sRootListInitialized = FALSE;   // if sRootList is initialized, True


/* ------------------------------------------------------------------------
    fill related
   ------------------------------------------------------------------------ */

#if ! defined(NNS_FINALROM)

    static u32 sFillVals[NNS_FND_HEAP_FILL_MAX] =
    {
        0xC3C3C3C3, // Value to fill when creating heap
        0xF3F3F3F3, // Value to fill when allocating memory block
        0xD3D3D3D3, // Value to fill when releasing memory block
    };

// #if ! defined(NNS_FINALROM)
#endif


/* ========================================================================
    static functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    list related
   ------------------------------------------------------------------------ */

/*---------------------------------------------------------------------------*
  Name:         FindContainHeap

  Description:  Recursively searches the heap containing the specified memory block from the list.  
                

  Arguments:    pList:     pointer to list
                memBlock:  pointer to memory block

  Returns:      If the heap that allocated the specified memory block is found, 
                returns a pointer to that heap. 
                Returns NULL, if it is not found.
 *---------------------------------------------------------------------------*/
static NNSiFndHeapHead*
FindContainHeap(
    NNSFndList* pList,
    const void* memBlock
)
{
    NNSiFndHeapHead* pHeapHd = NULL;
    while (NULL != (pHeapHd = NNS_FndGetNextListObject(pList, pHeapHd)))
    {
        if ( NNSiGetUIntPtr(pHeapHd->heapStart) <= NNSiGetUIntPtr(memBlock)
         &&  NNSiGetUIntPtr(memBlock) < NNSiGetUIntPtr(pHeapHd->heapEnd)
        )
        {
            NNSiFndHeapHead* pChildHeapHd = FindContainHeap(&pHeapHd->childList, memBlock);
            if(pChildHeapHd)
            {
                return pChildHeapHd;
            }

            return pHeapHd;
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         FindListContainHeap

  Description:  Searches the parent heap that contains the heap, and returns a pointer to the parent heap list. 
                  

  Arguments:    pHeapHd:  pointer to the header of search target heap.

  Returns:      If the parent heap that includes the specified heap is found:
                Returns a pointer to the parent heap's child list. 
                If the parent heap is not found, a pointer to the root list is returned.
 *---------------------------------------------------------------------------*/
static NNSFndList*
FindListContainHeap(NNSiFndHeapHead* pHeapHd)
{
    NNSFndList* pList = &sRootList;

    NNSiFndHeapHead* pContainHeap = FindContainHeap(&sRootList, pHeapHd);

    if(pContainHeap)
    {
        pList = &pContainHeap->childList;
    }

    return pList;
}

#if 1
    static inline void
    DumpHeapList() {}
#else
    static void
    DumpHeapList()
    {
        NNSiFndHeapHead* pHeapHd = NULL;
        int count = 0;

        OS_Printf("Dump Heap List\n");
        while (NULL != (pHeapHd = NNS_FndGetNextListObject(&sRootList, pHeapHd)))
        {
            count++;
            OS_Printf("[%d] -> %p %08X\n", count, pHeapHd, pHeapHd->signature);
        }
    }
#endif

/* ========================================================================
    External Functions (Non-Public)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndInitHeapHead

  Description:  Initializes heap header.

  Arguments:    pHeapHd:    Pointer to the heap header.
                signature:  signature.
                heapStart:  start address of heap memory.
                heapEnd:    end address +1 of heap memory.
                optFlag:    heap option

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNSi_FndInitHeapHead(
    NNSiFndHeapHead*    pHeapHd,
    u32                 signature,
    void*               heapStart,
    void*               heapEnd,
    u16                 optFlag
)
{
    pHeapHd->signature = signature;

    pHeapHd->heapStart = heapStart;
    pHeapHd->heapEnd   = heapEnd;

    pHeapHd->attribute = 0;
    SetOptForHeap(pHeapHd, optFlag);

    FillNoUseMemory(
        pHeapHd,
        heapStart,
        GetOffsetFromPtr(heapStart, heapEnd));

    NNS_FND_INIT_LIST(&pHeapHd->childList, NNSiFndHeapHead, link);

    // heap list operation
    if(! sRootListInitialized)
    {
        NNS_FND_INIT_LIST(&sRootList, NNSiFndHeapHead, link);
        sRootListInitialized = TRUE;
    }

    NNS_FndAppendListObject(FindListContainHeap(pHeapHd), pHeapHd);
    DumpHeapList();

}

/*---------------------------------------------------------------------------*
  Name:         NNSi_FndFinalizeHeap

  Description:  Takes care of the heap common.

  Arguments:    pHeapHd:  Pointer to the heap header.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNSi_FndFinalizeHeap(NNSiFndHeapHead* pHeapHd)
{
    // heap list operation
    NNS_FndRemoveListObject(FindListContainHeap(pHeapHd), pHeapHd);
    DumpHeapList();
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_FndDumpHeapHead

  Description:  Displays heap header information.

  Arguments:    pHeapHd:  Pointer to the heap header.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNSi_FndDumpHeapHead(NNSiFndHeapHead* pHeapHd)
{
    OS_Printf("[NNS Foundation ");

    switch(pHeapHd->signature)
    {
    case NNSI_EXPHEAP_SIGNATURE: OS_Printf("Exp");   break;
    case NNSI_FRMHEAP_SIGNATURE: OS_Printf("Frame"); break;
    case NNSI_UNTHEAP_SIGNATURE: OS_Printf("Unit");  break;
    default:
        NNS_ASSERT(FALSE);
    }

    OS_Printf(" Heap]\n");

    OS_Printf("    whole [%p - %p)\n", pHeapHd, pHeapHd->heapEnd);
}


/* ========================================================================
    External functions (public)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_FndFindContainHeap

  Description:  Searches for heap containing memory block.

  Arguments:    memBlock:  memory block to be searched.

  Returns:      If the heap that includes the specified memory block is found, 
                returns a handle to that heap. 
                If it is not found, NNS_FND_HEAP_INVALID_HANDLE is returned.
 *---------------------------------------------------------------------------*/
NNSFndHeapHandle
NNS_FndFindContainHeap(const void* memBlock)
{
    return FindContainHeap(&sRootList, memBlock);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_FndDumpHeap

  Description:  Displays the information in the heap.
                This function is for debugging.

  Arguments:    heap:    Handle for frame heap

  Returns:      None.
 *---------------------------------------------------------------------------*/
#if ! defined(NNS_FINALROM)

    void
    NNS_FndDumpHeap(NNSFndHeapHandle heap)
    {
        NNSiFndHeapHead* pHeapHd = heap;
        switch(pHeapHd->signature)
        {
        case NNSI_EXPHEAP_SIGNATURE: NNSi_FndDumpExpHeap(heap);  break;
        case NNSI_FRMHEAP_SIGNATURE: NNSi_FndDumpFrmHeap(heap);  break;
        case NNSI_UNTHEAP_SIGNATURE: NNSi_FndDumpUnitHeap(heap); break;
        default:
            OS_Printf("[NNS Foundation] dump heap : unknown heap. - %p\n", heap);
        }
    }

// #if ! defined(NNS_FINALROM)
#endif

/*---------------------------------------------------------------------------*
  Name:         NNS_FndSetFillValForHeap

  Description:  Sets the value that is placed in memory when creating the heap and allocating/freeing memory blocks. 
                
                This function is for use in debugging.
                Always returns 0 in the final ROM version library (FINALROM).

  Arguments:    type    :  type of value to get
                val:   set value

  Returns:      Returns the previous value that was set in memory when memory block was allocated.
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)

    u32
    NNS_FndSetFillValForHeap(
        int     type,
        u32     val
    )
    {
        NNS_ASSERT(type < NNS_FND_HEAP_FILL_MAX);

        {
            u32 oldVal = sFillVals[type];
            sFillVals[type] = val;
            return oldVal;
        }
    }

// #if ! defined(NNS_FINALROM)
#endif


/*---------------------------------------------------------------------------*
  Name:         NNS_FndGetFillValForHeap

  Description:  Gets the value that is placed in memory when creating the heap and allocating/freeing memory blocks. 
                
                This function is for use in debugging.
                Always returns 0 in the final ROM version library (FINALROM).

  Arguments:    type    :  type of value to get

  Returns:      Returns the value set in the memory of the type specified.
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)

    u32
    NNS_FndGetFillValForHeap(int type)
    {
        NNS_ASSERT(type < NNS_FND_HEAP_FILL_MAX);

        return sFillVals[type];
    }

// #if ! defined(NNS_FINALROM)
#endif

