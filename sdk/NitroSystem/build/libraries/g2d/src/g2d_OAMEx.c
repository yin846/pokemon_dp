/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_OAMEx.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_OAMEx.c,v $
  Revision 1.29  09/01/2005 12:56:30 AM  kitani_toshikazu
  Revised assert conditions in NNS_G2dSetOamManExEntryFunctions()

  Revision 1.28  06/29/2005 08:30:39 AM  kitani_toshikazu
  Corrected comment

  Revision 1.27  05/31/2005 11:27:50 AM  kitani_toshikazu
  Added assert.

  Revision 1.26  04/28/2005 02:31:09 AM  kitani_toshikazu
  Replaced NNSi_G2dRepeatXinCellSpace()
  with NNS_G2dRepeatXinCellSpace

  Revision 1.25  03/08/2005 07:03:15 AM  kitani_toshikazu
  Added NNSG2d_SetOamManExDrawOrderType()

  Revision 1.24  02/22/2005 01:06:46 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.23  02/21/2005 09:02:42 AM  kitani_toshikazu
  Improved time-sharing display processing method.

  Revision 1.22  02/21/2005 02:16:37 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.21  02/18/2005 10:59:56 AM  kitani_toshikazu
  Fixed bugs regarding time-share display of affine parameters.

  Revision 1.20  02/16/2005 10:26:22 AM  kitani_toshikazu
  Added comment

  Revision 1.19  02/16/2005 10:21:30 AM  kitani_toshikazu
  Changed the drawing order for affine-transformed OAMs so that it matches that of regular OAMs

  Revision 1.18  12/01/2004 02:15:18 AM  kitani_toshikazu
  Changed the rendering method (method of registering OAM to an external module)

  Revision 1.17  11/15/2004 08:28:36 AM  kitani_toshikazu
  Cleaned up the code

  Revision 1.16  10/18/2004 05:51:53 AM  kitani_toshikazu
  Revised comments.

  Revision 1.15  09/15/2004 05:51:06 AM  kitani_toshikazu
  Transferred the affine transformation mode setting process out of the manager

  Revision 1.14  07/28/2004 12:11:20 AM  kitani_toshikazu
  Correspondence with the padding warning

  Revision 1.13  07/26/2004 04:44:33 AM  kitani_toshikazu
  Changed function names.

  Revision 1.12  07/24/2004 02:47:52 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.11  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.10  06/23/2004 08:54:10 AM  kitani_toshikazu
  Got rid of the default render registration function

  Revision 1.9  06/23/2004 08:00:34 AM  hatamoto_minoru
  Added numLastFrameDrawn initialization processing.
  Handle OBJs not displayed with time division display.

  Revision 1.8  06/22/2004 06:34:29 AM  kitani_toshikazu
  NNS_G2dSetOamManExDoubleAffineFlag()
  Added NNS_G2dGetOamManExDoubleAffineFlag().

  Revision 1.7  06/21/2004 02:31:16 AM  kitani_toshikazu
  Combined OAM managers Ex2 and Ex.

  Revision 1.3  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_OAMEx.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>

#include "g2d_Internal.h"





static GXOamAttr       defaultOam_ = { 193, 193, 193 };







//------------------------------------------------------------------------------
// Check the appropriateness of the various registration functions
NNS_G2D_INLINE BOOL IsOamEntryFuncsValid_
( 
    const NNSG2dOamManagerInstanceEx*  pMan,
    const NNSG2dOamExEntryFunctions*   pF 
)
{
    const BOOL bValid = (BOOL)(( pF                       != NULL ) &&
                               ( pF->getOamCapacity       != NULL ) &&    
                               ( pF->entryNewOam          != NULL ) );
                      
    // When managing affine parameters, 
    if( pMan->pAffineBuffer != NULL || pMan->lengthAffineBuffer != 0 )
    {
        // The associated callbacks must be correctly set. 
        return (BOOL)( bValid &&
                      ( pF->getAffineCapacity    != NULL ) &&
                      ( pF->entryNewAffine       != NULL ) );
    }else{
        return bValid;  
    }
    
}

//------------------------------------------------------------------------------
// Get new NNSG2dOamChunk from shared NNSG2dOamChunk.
// Shared NNSG2dOamChunk is set at time of initialization
//
NNS_G2D_INLINE NNSG2dOamChunk* GetNewOamChunk_( NNSG2dOamManagerInstanceEx* pMan, const GXOamAttr* pOam )
{
    NNSG2dOamChunk*   pRet = NULL;
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
    
    if( pMan->numUsedOam < pMan->numPooledOam )
    {
        
        pRet = &pMan->pPoolOamChunks[pMan->numUsedOam];
        pMan->numUsedOam++;
        
        pRet->oam = *pOam;
        return pRet;
    }else{
        NNSI_G2D_DEBUGMSG0("We have no capacity for a new Oam.");
        return NULL;
    }
}

//------------------------------------------------------------------------------
// Add NNSG2dOamChunk to start of NNSG2dOamChunkList
NNS_G2D_INLINE void AddFront_( NNSG2dOamChunkList* pOamList, NNSG2dOamChunk* pChunk )
{
    pChunk->pNext       = pOamList->pChunks;
    pOamList->pChunks   = pChunk;
    
    pOamList->numChunks++;
}

//------------------------------------------------------------------------------
// Add NNSG2dOamChunk to end of NNSG2dOamChunkList
NNS_G2D_INLINE void AddBack_( NNSG2dOamChunkList* pOamList, NNSG2dOamChunk* pChunk )
{
    pChunk->pNext               = NULL;
    
    if( pOamList->pLastChunk != NULL )
    {
        // After the first time
        pOamList->pLastChunk->pNext = pChunk;
    }else{
        // First time
        pOamList->pChunks    = pChunk;
    }
    
    // Update the end of the list
    pOamList->pLastChunk = pChunk;
    
    pOamList->numChunks++;
}

//------------------------------------------------------------------------------
// Register Oam to lower module
NNS_G2D_INLINE void EntryOamToToBaseModule_( NNSG2dOamManagerInstanceEx* pMan, const GXOamAttr* pOam, u16 totalOam )
{
    (void)(*pMan->oamEntryFuncs.entryNewOam)( pOam, totalOam );
}


//------------------------------------------------------------------------------
// Determine whether affineProxy is valid
NNS_G2D_INLINE BOOL IsAffineProxyValid_( NNSG2dOamManagerInstanceEx* pMan )
{
    return ( pMan->pAffineBuffer != NULL && pMan->lengthAffineBuffer != 0 ) ? 
        TRUE : FALSE;
}
//------------------------------------------------------------------------------
// Check whether enough space remains for new registration.
NNS_G2D_INLINE BOOL HasEnoughCapacity_( NNSG2dOamManagerInstanceEx* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    return ( pMan->numAffineBufferUsed < pMan->lengthAffineBuffer ) ? TRUE : FALSE;
}
//------------------------------------------------------------------------------
// Perform conversion affineProxyIdx => affineHWIndex
NNS_G2D_INLINE u16 ConvertAffineIndex_( NNSG2dOamManagerInstanceEx* pMan, u16 affineProxyIdx )
{
    NNS_G2D_ASSERT( IsAffineProxyValid_( pMan ) );
    NNS_G2D_ASSERT( affineProxyIdx < pMan->lengthAffineBuffer );
        
    return pMan->pAffineBuffer[affineProxyIdx].affineHWIndex;            
}

//------------------------------------------------------------------------------
// Change all NNSG2dOamChunk reference affine parameter data inside
// NNSG2dOamChunkList to data actually set in HW.
//
// We changed the drawing order of affine-transformed OAMs so that it matches that of regular OAMs.
// Previously, affine-transformed OAMs were drawn in the reverse order that they were saved.
// 
// If you define NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_  and rebuild the library,
// you can have it behave like the old implementation.
//
//#define NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_ 1

NNS_G2D_INLINE void ReindexOamChunkList_
( 
    NNSG2dOamManagerInstanceEx*     pMan, 
    NNSG2dOamChunkList*             pChunkList
)
{
#ifdef NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_
    //
    // Old implementation
    //
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pChunkList );
    {
        NNSG2dOamChunk*    pChunk     = pChunkList->pAffinedChunks;
        NNSG2dOamChunk*    pNextChunk = NULL;
        
        //
        // To the end of the list...
        //
        while( pChunk != NULL )
        {
            //
            // Convert index from AffineProxyIndex into actual HW Index
            //
            const u16 index = ConvertAffineIndex_( pMan, pChunk->affineProxyIdx );
            
            //OS_Printf("AffProxy_Idx = %d, => HW_Idx = %d \n", pChunk->affineProxyIdx, index );
            
            pNextChunk = pChunk->pNext;
            
            // If index has valid value.....
            if( index != NNS_G2D_OAMEX_HW_ID_NOT_INIT )
            {
                // Replace the OAM Index
                // G2_SetOBJEffect( &pChunk->oam, NNS_G2dGetOamManExDoubleAffineFlag( pMan ), index );
                pChunk->oam.rsParam = index;
                
                //
                // Move to normal OamChunkList
                // The registration order will be the reverse of that used with a normal OamChunkList
                //
                AddFront_( pChunkList, pChunk );
            }
            pChunk     = pNextChunk;
        }
    }
#else // NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_
    //
    // Current implementation
    //
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pChunkList );
    {
        NNSG2dOamChunk*    pChunk     = pChunkList->pAffinedChunks;
        NNSG2dOamChunk*    pHeadChunk = NULL;
        NNSG2dOamChunk*    pPrevChunk = NULL;
        int                numAffinedOam = 0;
        
        
        //
        // Continue to the end of the affine-transformed OAM list...
        //
        while( pChunk != NULL )
        {
            //
            // Convert index from AffineProxyIndex into actual HW Index
            //
            const u16 index = ConvertAffineIndex_( pMan, pChunk->affineProxyIdx );
            
            // If index has valid value.....
            if( index != NNS_G2D_OAMEX_HW_ID_NOT_INIT )
            {
                // Store the top of the (valid) affine-transformed OAM list
                if( pHeadChunk == NULL )
                {
                    pHeadChunk = pChunk;
                }
                
                //
                // Replace the OAM Index
                //
                pChunk->oam.rsParam = index;
                
                
                // Go to the next chunk
                pPrevChunk = pChunk;
                pChunk     = pChunk->pNext;
                numAffinedOam++;
                
            }else{
                // Remove from list
                if( pPrevChunk != NULL )
                {
                    pPrevChunk->pNext = pChunk->pNext;
                }
                pMan->numUsedOam--;
                // Go to the next chunk
                pChunk     = pChunk->pNext;
            }
        }
        
        //
        // pPrevChunk->pNext => end of affine-transformed OAM list
        // Collect the affine-transformed OAM list and insert it at the top of the normal chunk list
        // There is no change to the order of the affine-transformed OAM list
        // 
        if( numAffinedOam != 0 ) 
        {
           pPrevChunk->pNext     = pChunkList->pChunks;
           pChunkList->pChunks   = pHeadChunk;
           pChunkList->numChunks += numAffinedOam;
           pChunkList->pAffinedChunks = NULL;
        }
    }
#endif // NNS_G2D_OAMEX_USE_OLD_REINDEXOAMCHUNKLIST_
}

//------------------------------------------------------------------------------
// Process affine-transformed NNSG2dOamChunk so that it can be drawn.
//
// The affine-transformed NNSG2dOamChunk is stored on a different list than normal NNSG2dOamChunk. 
// Because NNSG2dOamChunk references the affine parameters in the internal affineProxy array's "Index", 
// the value of the normal affine parameter Index value must be rewritten. 
// 
// 
NNS_G2D_INLINE void ReindexAffinedOams_( NNSG2dOamManagerInstanceEx* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( pMan->numAffineBufferUsed != 0 );
    NNS_G2D_ASSERT( IsAffineProxyValid_( pMan ) );
    
    {
       u16 i;
       //
       // For all NNSG2dOamChunkList ...
       //
       for( i = 0; i < pMan->lengthOfOrderingTbl; i++ )
       {
           ReindexOamChunkList_( pMan, &pMan->pOamOrderingTbl[i] );
       }
    }
}




//------------------------------------------------------------------------------
// Check whether the specified rendering priority is appropriate.
// The value pMan->lengthOfOrderingTbl is freely set by the user at the time the manager is initialized.
NNS_G2D_INLINE BOOL IsPriorityValid_( NNSG2dOamManagerInstanceEx* pMan, u8 priority )
{
    return ( priority < pMan->lengthOfOrderingTbl ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
// Specify AffineIndex in NNSG2dOamChunkList and register OAMAttribute
//
// The new NNSG2dOamChunk is stored in the special list of  affine-transformed NNSG2dOamChunk.
// TheNNSG2dOamChunk dedicated list performs Index processing again with AffinedOams_() 
// and inserts into the normal NNSG2dOamChunk. 
//
NNS_G2D_INLINE BOOL EntryNewOamWithAffine_
( 
    NNSG2dOamChunkList*       pOamList, 
    NNSG2dOamChunk*           pOamChunk, 
    u16                       affineIdx, 
    NNSG2dOamExDrawOrder      drawOrderType
)
{
    NNS_G2D_NULL_ASSERT( pOamList );
    
    if( pOamChunk != NULL )
    {
        //
        // Do not use Affine
        //
        if( affineIdx == NNS_G2D_OAM_AFFINE_IDX_NONE )
        {
           if( drawOrderType == NNSG2D_OAMEX_DRAWORDER_BACKWARD )
           {
               //
               // Move to normal OamChunkList
               //
               AddFront_( pOamList, pOamChunk );
           }else{
               AddBack_( pOamList, pOamChunk );
           }
        }else{
            
            pOamChunk->affineProxyIdx = affineIdx;
            if( drawOrderType == NNSG2D_OAMEX_DRAWORDER_BACKWARD )
            {
               //
               // Once Affine list is done
               // add to top
               //
               pOamChunk->pNext            = pOamList->pAffinedChunks;
               pOamList->pAffinedChunks    = pOamChunk;
            }else{
               //
               // Once Affine list is done
               // add to end
               //
               pOamChunk->pNext = NULL;
               if( pOamList->pLastAffinedChunk != NULL )
               {
                  pOamList->pLastAffinedChunk->pNext = pOamChunk;
               }else{
                  pOamList->pAffinedChunks = pOamChunk;
               }
               pOamList->pLastAffinedChunk = pOamChunk;
            }
        }
        return TRUE;
    }else{
        return FALSE;
    }
}



//------------------------------------------------------------------------------
// Advance the list's pointer by "num" amount only
NNS_G2D_INLINE NNSG2dOamChunk* AdvancePointer_( NNSG2dOamChunk* pChunk, u16 num )
{
    NNS_G2D_NULL_ASSERT( pChunk );
    while( num > 0 )
    {
        pChunk = pChunk->pNext;
        num--;
        NNS_G2D_NULL_ASSERT( pChunk );
    }
    
    return pChunk;
}

//------------------------------------------------------------------------------
// Draw OamChunkList
// Collect together all OamChunk with same drawing priority to form OamChunkList
//
NNS_G2D_INLINE u16 DrawOamChunks_
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    NNSG2dOamChunkList*             pOamList, 
    NNSG2dOamChunk*                 pChunk, 
    u16                             numOam, 
    u16                             capacityOfHW, 
    u16                             numTotalOamDrawn 
)
{
    //
    // Until all OBJ have been drawn or there is no more free space
    // 
    while( numOam > 0 && (capacityOfHW - numTotalOamDrawn) > 0 )
    {
        //
        // Copy value
        //
        EntryOamToToBaseModule_( pMan, &pChunk->oam, numTotalOamDrawn );
        
        
        pChunk = pChunk->pNext;
        // 
        // If end of list has been reached, start again from top of list
        //
        if( pChunk == NULL )
        {
            pChunk = pOamList->pChunks;
        }
        
        
        numOam--;
        numTotalOamDrawn++;
    }
    return numTotalOamDrawn;
} 



//------------------------------------------------------------------------------
// Reflect AffineProxy in lower module.
// (Normally, the lower module propagates the reflection to HW)
NNS_G2D_INLINE void LoadAffineProxyToBaseModule_( NNSG2dOamManagerInstanceEx* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( pMan->numAffineBufferUsed != 0 );
    
    {    
       u16        count     = 0x0;
       u16        i         = pMan->lastFrameAffineIdx;
       const u16  numAffine = pMan->numAffineBufferUsed;
       const u16  capacity  = (*pMan->oamEntryFuncs.getAffineCapacity)();
           
       NNSG2dAffineParamProxy*   pAff = NULL;
       //
       // Aim of process is to register in continuance from previous frame
       //
       while( ( count < numAffine ) && 
              ( count < capacity ) )
       {
           if( i >= numAffine )
           {
              i = 0;
           }
           pAff = &pMan->pAffineBuffer[ i ];
           
           // store HW affine index.
           pAff->affineHWIndex 
              = (*pMan->oamEntryFuncs.entryNewAffine)( &pAff->mtxAffine, count );
           
           // pMan->lastFrameAffineIdx = i;
           
           i++;
           count++;    
       }
       
       pMan->lastFrameAffineIdx = i;
    }
}

//------------------------------------------------------------------------------
//
// Calculate the range for draw registration
//
// Procedure:
//
// Set all lists as lists that are not drawn.
// Scan lists and, while counting the draw OAM chunks, put information for  drawing in lists.
// End process when there is no more free space or when all chunks have  been checked.
//
NNS_G2D_INLINE void 
CalcDrawnListArea_( NNSG2dOamManagerInstanceEx* pMan )
{
    NNSG2dOamChunk*       pChunk    = NULL;
    NNSG2dOamChunkList*   pOamList  = NULL;
    u16       numTotalOamDrawn      = 0;
    const u16 capacityOfHW          = ( *(pMan->oamEntryFuncs.getOamCapacity) )();
    
    u16       i;
    
    
    //
    // Reset the drawing flag in all chunk lists
    //
    for( i = 0; i < pMan->lengthOfOrderingTbl; i++ )
    {
       pMan->pOamOrderingTbl[i].bDrawn = FALSE;
    }
    
    
    //
    // Scan data, starting from the position where the previous frame ended.
    //
    i = (u16)(pMan->lastRenderedOrderingTblIdx);
    
    //
    // Continue until all chunks in all chunk lists have been inspected...
    //
    while( numTotalOamDrawn < pMan->numUsedOam )
    {
       // If end of ordering table has been reached, return to start of table.
       if( i >= pMan->lengthOfOrderingTbl )
       {
           i = 0;
       }
       
       pOamList   = &pMan->pOamOrderingTbl[i];
       //
       // If chunks are registered in chunk list....
       //
       if( pOamList->numChunks != 0 )
       {
           const u16 currentCapacity = (u16)(capacityOfHW - numTotalOamDrawn);
           
           //
           // Set as chunk list that should be drawn
           // Also, update the number of the last drawn chunk list
           //
           pOamList->bDrawn                 = TRUE;
           pMan->lastRenderedOrderingTblIdx = i;
           
           //
           // If all chunks in list can be registered
           //
           if( pOamList->numChunks <= currentCapacity )
           {
              pOamList->numLastFrameDrawn = 0;
              pOamList->numDrawn          = pOamList->numChunks;   
              
           }else{
              // If drawn to the end of the list, in the next frame, drawing is performed from the list after this chunk list. 
              // 
              if( (pOamList->numDrawn + pOamList->numLastFrameDrawn) / pOamList->numChunks > 0 )
              {
                  pMan->lastRenderedOrderingTblIdx = (u16)(i+1);
              }
              
              //
              // From the number of chunks drawn in the previous frame, 
              // calculate the offset count to provide in this frame before drawing. 
              //
              pOamList->numLastFrameDrawn = (u16)((pOamList->numDrawn + 
                                            pOamList->numLastFrameDrawn ) % pOamList->numChunks);
              
              pOamList->numDrawn          = currentCapacity;// Draw as many as possible    
              //
              // Quit
              //
              break;
           }
           numTotalOamDrawn += pOamList->numDrawn;
       }
       i++;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetOamManExBuffer

  Description:  Resets the manager.
                                
  Arguments:    pMan                :   [OUT]  The manager instance 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetOamManExBuffer( NNSG2dOamManagerInstanceEx* pOam )
{
    NNS_G2D_NULL_ASSERT( pOam );
    pOam->numUsedOam = 0;
    pOam->numAffineBufferUsed = 0;
    
    // reset the ordering table.
    {
        u16 i = 0;
        for( i = 0;i < pOam->lengthOfOrderingTbl; i++ )
        {
            pOam->pOamOrderingTbl[i].numChunks      = 0;
            pOam->pOamOrderingTbl[i].pChunks        = NULL;
            pOam->pOamOrderingTbl[i].pAffinedChunks = NULL;
            pOam->pOamOrderingTbl[i].pLastChunk        = NULL;
            pOam->pOamOrderingTbl[i].pLastAffinedChunk = NULL;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetOamManExInstance

  Description:  Creates the manager itself.


                
                
                
  Arguments:    pOam                 [OUT]    The expanded OAM manager instance
                pOamOrderingTbl      [IN]    Starting address of ordering table
                lengthOfOrderingTbl    [IN]  Length of ordering table
                numPooledOam          [IN]    Number of OBJChunk
                pPooledOam            [IN]   Pointer to OBJChunk array
                lengthAffineBuffer     [IN]  Length of affine parameter buffer array
                pAffineBuffer      [IN]     Pointer to affine parameter buffer

                
  Returns:      Success or failure of initialization (At present time, always succeeds)
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetOamManExInstance
( 
    NNSG2dOamManagerInstanceEx*     pOam, 
    NNSG2dOamChunkList*             pOamOrderingTbl, 
    u8                              lengthOfOrderingTbl,
    u16                             numPooledOam,
    NNSG2dOamChunk*                 pPooledOam,
    u16                             lengthAffineBuffer,
    NNSG2dAffineParamProxy*         pAffineBuffer 
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_ASSERT( lengthOfOrderingTbl != 0 );
    NNS_G2D_NULL_ASSERT( pPooledOam );
    NNS_G2D_ASSERT( numPooledOam != 0 );
    
    // Drawing order type
    // (To maintain backward compatibility, the default order is reverse)
    pOam->drawOrderType = NNSG2D_OAMEX_DRAWORDER_BACKWARD;
    
    //
    // Zero-clear chunk list elements
    //
    MI_CpuClear32( pOamOrderingTbl, lengthOfOrderingTbl * sizeof( NNSG2dOamChunkList ) );
    
    
    pOam->pOamOrderingTbl           = pOamOrderingTbl;
    pOam->lengthOfOrderingTbl       = lengthOfOrderingTbl;
    
    pOam->numPooledOam              = numPooledOam;
    pOam->pPoolOamChunks            = pPooledOam;
    
    pOam->lengthAffineBuffer    = lengthAffineBuffer;
    pOam->pAffineBuffer         = pAffineBuffer;
    
    
    
    //
    // Initialization related to registration functions
    //
    {
        NNSG2dOamExEntryFunctions*  pFuncs = &pOam->oamEntryFuncs;
        
        pFuncs->getOamCapacity       = NULL;
        pFuncs->getAffineCapacity    = NULL;
        pFuncs->entryNewOam          = NULL;
        pFuncs->entryNewAffine       = NULL;
    }
    
    
    NNS_G2dResetOamManExBuffer( pOam );
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNSG2d_SetOamManExDrawOrderType

  Description:  Sets manager's OAM drawing order type.
                Note that the OAM drawing order specified value is NNSG2D_OAMEX_DRAWORDER_BACKWARD. 
                
                This is to maintain compatibility with previous versions.

  Arguments:    pOam :   [OUT]  The manager instance 
                drawOrderType        :   [IN]  Drawing order type
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSG2d_SetOamManExDrawOrderType
( 
    NNSG2dOamManagerInstanceEx*    pOam, 
    NNSG2dOamExDrawOrder           drawOrderType
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    
    pOam->drawOrderType = drawOrderType;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetOamManExEntryFunctions

  Description:  Sets various registration functions in the manager.
                You must call this function and set the registration functions  before using the manager.

                
                
                
  Arguments:    pMan                :   [OUT]  The manager instance 
                pSrc    :   [IN]  Registration functions
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetOamManExEntryFunctions
( 
    NNSG2dOamManagerInstanceEx*        pMan, 
    const NNSG2dOamExEntryFunctions*   pSrc 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    {
        NNSG2dOamExEntryFunctions*  pDst = &pMan->oamEntryFuncs;    
        
        NNS_G2D_NULL_ASSERT( pSrc );

        if( pSrc->getOamCapacity != NULL )
        {
            pDst->getOamCapacity = pSrc->getOamCapacity;
        }
        
        if( pSrc->getAffineCapacity != NULL )
        {
            pDst->getAffineCapacity = pSrc->getAffineCapacity;
        }
        
        if( pSrc->entryNewOam != NULL )
        {
            pDst->entryNewOam = pSrc->entryNewOam;
        }
        
        if( pSrc->entryNewAffine != NULL )
        {
            pDst->entryNewAffine = pSrc->entryNewAffine;
        }
        
        NNS_G2D_ASSERT( IsOamEntryFuncsValid_( pMan, pDst ) );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManExOam

  Description:  Specifies and registers OBJ in extended OAM in the order  of registration at time of drawing.
                When actually applying to the hardware, the specified order is used.

                
                If there is enough space, registration is performed and TRUE is returned.
                
                
                
  Arguments:    pMan                :   [OUT]  The manager instance 
                pOam :   [IN]  OAMAttribute 
                priority:   [IN]  Drawing priority
                affineIdx:   [IN]  Affine index
                
  Returns:      registration success or failure
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dEntryOamManExOam
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const GXOamAttr*               pOam, 
    u8                             priority 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_ASSERT( IsPriorityValid_( pMan, priority ) );
    
    {
        NNSG2dOamChunkList*   pOamList    = &pMan->pOamOrderingTbl[ priority ];
        NNSG2dOamChunk*       pOamChunk   = GetNewOamChunk_( pMan, pOam );
         
        return EntryNewOamWithAffine_( pOamList, 
                                       pOamChunk, 
                                       NNS_G2D_OAM_AFFINE_IDX_NONE, 
                                       pMan->drawOrderType );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManExOamWithAffineIdx

  Description:  Specifies AffineIndex and registers OAMAttribute in manager.
                Specify the return value for NNS_G2dEntryOamManExAffine().
                
                If there is enough space, registration is performed and TRUE  is returned.
                
                
                
  Arguments:    pMan                :   [OUT]  The manager instance 
                pOam :   [IN]  OAMAttribute 
                priority:   [IN]  Drawing priority
                affineIdx:   [IN]  Affine index
                
  Returns:      registration success or failure
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dEntryOamManExOamWithAffineIdx
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const GXOamAttr*               pOam, 
    u8                             priority, 
    u16                            affineIdx 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
    // Is affine transformation enabled for specified OAM attribute?
    NNS_G2D_ASSERT( pOam->rsMode & 0x1 );
    NNS_G2D_ASSERT( IsPriorityValid_( pMan, priority ) );
    
    {
        NNSG2dOamChunkList*   pOamList    = &pMan->pOamOrderingTbl[ priority ];
        NNSG2dOamChunk*       pOamChunk   = GetNewOamChunk_( pMan, pOam );
         
        return EntryNewOamWithAffine_( pOamList, 
                                       pOamChunk, 
                                       affineIdx, 
                                       pMan->drawOrderType );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManExAffine

  Description:  Registers Affine parameters in manager.
                If the manager has enough space, the affine transformation
                matrix is stored in NNSG2dAffineParamProxy and Index is returned.
                If a buffer array for the affine parameters has not been set
                
                up, the function fails on ASSERT.
                The OBJ references the affine parameters inside the manager using the Index of NNSG2dAffineParamProxy.
                
                
                
  Arguments:    pMan                :      [OUT]  The manager instance 
                mtx :      [IN]  affine transform matrix
  
  Returns:      The Index of the internal NNSG2dAffineParamProxy
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dEntryOamManExAffine
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const MtxFx22*                 mtx 
)
{
    NNS_G2D_NULL_ASSERT( pMan ); 
    NNS_G2D_NULL_ASSERT( mtx );
    NNS_G2D_ASSERT( IsAffineProxyValid_( pMan ) );
    
    if( HasEnoughCapacity_( pMan ) )
    {
        NNSG2dAffineParamProxy* pAffineProxy 
           = &pMan->pAffineBuffer[pMan->numAffineBufferUsed];
           
        pAffineProxy->mtxAffine     = *mtx;
        pAffineProxy->affineHWIndex = NNS_G2D_OAMEX_HW_ID_NOT_INIT;
        
        return pMan->numAffineBufferUsed++;
    }   
    
    return NNS_G2D_OAM_AFFINE_IDX_NONE;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dApplyOamManExToBaseModule

  Description:  Reflects the contents of the manager in HW.
                In actuality, the contents are not reflected in HW, but in the lower module.
                The contents will not be reflected in the drawing until the lower module reflects the contents in HW.
                
                
                
  Arguments:    pMan                :      [OUT]  The manager instance 
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dApplyOamManExToBaseModule( NNSG2dOamManagerInstanceEx* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pMan->oamEntryFuncs.getOamCapacity );
    {
        u16       numTotalOamDrawn = 0;
        const u16 capacityOfHW     = ( *(pMan->oamEntryFuncs.getOamCapacity) )();
        
        if( pMan->numUsedOam != 0 )
        {
            //
            // Prepare affine-transformed Oam
            // 
            if( pMan->numAffineBufferUsed != 0 )
            {
                // Apply contents of Affine Proxy in lower module and  determine Affine Index
                LoadAffineProxyToBaseModule_( pMan );
                // Re-index
                ReindexAffinedOams_( pMan );
            }
            
            //
            // Calculate the chunk list registration range
            //
            CalcDrawnListArea_( pMan );
            
            //
            // Perform actual drawing registration for external module
            //
            {
                u16 i = 0;
                NNSG2dOamChunk*       pChunk    = NULL;
                NNSG2dOamChunkList*   pOamList  = NULL;
        
                for( i = 0; i < pMan->lengthOfOrderingTbl; i++ )
                {
                    pOamList  = &pMan->pOamOrderingTbl[i];
                    //
                    // If chunk list needs to be drawn...
                    //  
                    if( pOamList->bDrawn )
                    {
                        NNS_G2D_ASSERT( pOamList->numLastFrameDrawn < pOamList->numChunks );
                        
                        pChunk = AdvancePointer_( pOamList->pChunks, pOamList->numLastFrameDrawn );               
                         
                        numTotalOamDrawn = DrawOamChunks_( pMan, 
                                                 pOamList, 
                                                 pChunk, 
                                                 pOamList->numDrawn, 
                                                 capacityOfHW, 
                                                 numTotalOamDrawn );
                    }
                }
            }
        }
        
        //
        // Clear remaining space with default value
        //
        while( capacityOfHW > numTotalOamDrawn )
        {
            // Clear with default value
            EntryOamToToBaseModule_( pMan, &defaultOam_, numTotalOamDrawn );
            numTotalOamDrawn++;
        }
    }
}





