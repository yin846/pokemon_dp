/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfdi_LinkedListVramMan_Common.h

  Copyright 2004, 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfdi_LinkedListVramMan_Common.h,v $
  Revision 1.6  08/17/2005 11:51:48 PM  kitani_toshikazu
  Revised copyright notation

  Revision 1.5  08/17/2005 08:09:56 AM  kitani_toshikazu
  Added a function for debug display

  Revision 1.4  10/18/2004 12:28:06 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.3  09/24/2004 09:32:23 AM  kitani_toshikazu
  Added NNSi_GfdInitLnkVramMan().

  Revision 1.2  09/10/2004 10:43:30 AM  kitani_toshikazu
  Changed API.

  Revision 1.1  09/03/2004 08:14:24 AM  kitani_toshikazu
  Registered linked list format manager.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_
#define NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_


#include <nnsys/gfd.h>

//------------------------------------------------------------------------------
typedef struct NNSiGfdLnkVramBlock NNSiGfdLnkVramBlock;

/*---------------------------------------------------------------------------*
  Name:         NNSiGfdLnkVramBlock

  Description:  Memory region management block
  
 *---------------------------------------------------------------------------*/
struct NNSiGfdLnkVramBlock
{
    u32                         addr;       // Region start address
    u32                         szByte;     // Region size (zero may not be used)
    
    NNSiGfdLnkVramBlock*        pBlkPrev;   // The previous region (No address-positional relation)
    NNSiGfdLnkVramBlock*        pBlkNext;   // The next region (No address-positional relation)
    
};

/*---------------------------------------------------------------------------*
  Name:         NNSiGfdLnkMemRegion

  Description:  Memory interval
                For items satisfying end > start
  
 *---------------------------------------------------------------------------*/
typedef struct NNSiGfdLnkMemRegion
{
    u32       start;
    u32       end;
    
}NNSiGfdLnkMemRegion;

/*---------------------------------------------------------------------------*
  Name:         NNSiGfdLnkVramMan

  Description:  Manager
                Unlike ordinary heaps, there is no management information list for used regions
                This is because with no relationship between the management information region (in main memory) and
                the management region address (in VRAM), it is difficult to use the look-up for management information
                for the used regions from the TextureKey (having the address and size information).
                When freeing, returns to the free area of the area used from the address + size. 
                
                
 *---------------------------------------------------------------------------*/
typedef struct NNSiGfdLnkVramMan
{
    NNSiGfdLnkVramBlock*         pFreeList;         // Unused region block list
      
}NNSiGfdLnkVramMan;




//------------------------------------------------------------------------------
// Function Declaration
//------------------------------------------------------------------------------
void NNSi_GfdDumpLnkVramManFreeListInfo
( 
    const NNSiGfdLnkVramBlock*      pFreeListHead,
    u32                             szReserved 
);

void 
NNSi_GfdInitLnkVramMan( NNSiGfdLnkVramMan* pMgr );


NNSiGfdLnkVramBlock* 
NNSi_GfdInitLnkVramBlockPool
( 
    NNSiGfdLnkVramBlock*    pArrayHead, 
    u32                     lengthOfArray 
); 

BOOL
NNSi_GfdAddNewFreeBlock
(
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32                     baseAddr,
    u32                     szByte
);


BOOL
NNSi_GfdAllocLnkVram
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32*                    pRetAddr,
    u32                     szByte
); 

BOOL
NNSi_GfdAllocLnkVramAligned
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32*                    pRetAddr,
    u32                     szByte,
    u32                     alignment
);

BOOL NNSi_GfdFreeLnkVram
( 
    NNSiGfdLnkVramMan*      pMan, 
    NNSiGfdLnkVramBlock**   ppBlockPoolList,
    u32                     addr,
    u32                     szByte
);





#endif // NNS_GFDI_LINKEDLISTVRAMMAN_COMMON_H_


