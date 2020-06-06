/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2 D
  
  File:     g2d_CellAnimation.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_CellAnimation.c,v $
  Revision 1.33  06/29/2005 08:30:39 AM  kitani_toshikazu
  Corrected comment

  Revision 1.32  06/02/2005 02:06:29 AM  kitani_toshikazu
  Cleaned up the code

  Revision 1.31  06/01/2005 06:19:28 AM  kitani_toshikazu
  NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED
  was added. 

  Revision 1.30  2005/06/01 05:08:34  kitani_toshikazu
  Fixed-point multiplication => replace with shift

  Revision 1.29  05/31/2005 11:27:39 AM  kitani_toshikazu
  Reivsed double-size affine OBJ correction calculations

  Revision 1.28  05/30/2005 12:04:38 PM  kitani_toshikazu
  Changed so that position correction only occurs when double-size affine flag is actually overwritten

  Revision 1.27  05/27/2005 08:47:06 AM  kitani_toshikazu
  Animation frames that have a 0 display time
  are no longer updated. 

  Revision 1.26  02/22/2005 12:51:14 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.25  02/21/2005 02:16:29 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.24  12/09/2004 02:53:18 AM  kitani_toshikazu
  In order to fix display problem with first animation frame in VRAM animation, 
  modified NNS_G2dInitCellAnimationVramTransfered().

  Revision 1.23  10/27/2004 11:56:09 PM  kitani_toshikazu
  New animation type
  Support for NNS_G2D_ANIMELEMENT_INDEX_T

  Revision 1.22  10/18/2004 09:47:54 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.21  10/08/2004 04:38:16 AM  kitani_toshikazu
  (none)

  Revision 1.20  08/26/2004 07:21:10 AM  kitani_toshikazu
  (none)

  Revision 1.19  08/24/2004 11:12:06 AM  kitani_toshikazu
  Changed so that this is performed only when the results of the SRT animation are needed.

  Revision 1.18  08/23/2004 08:46:24 AM  kitani_toshikazu
  Implementation of the SRT animation features

  Revision 1.17  07/29/2004 12:11:21 AM  kitani_toshikazu
  Changed function names.

  Revision 1.16  07/28/2004 08:12:42 AM  kitani_toshikazu
  Corrected comment

  Revision 1.15  07/24/2004 06:58:45 AM  kitani_toshikazu
  Changed function names related to animation controller.

  Revision 1.14  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.13  07/17/2004 06:56:03 AM  kitani_toshikazu
  Updated to reflect a function name change

  Revision 1.12  07/16/2004 10:02:15 AM  kitani_toshikazu
  Changed name from NNS_G2dRequestVramTransfer() to NNS_G2dSetCellTransferStateRequested().

  Revision 1.11  07/16/2004 07:23:43 AM  kitani_toshikazu
  Complied with a function name change

  Revision 1.10  07/14/2004 12:24:04 PM  kitani_toshikazu
  Support for Vram transfer animation.

  Revision 1.9  05/28/2004 07:22:01 AM  kitani_toshikazu
  Changed MultiCell animation update method.

  Revision 1.8  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.7  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.6  04/26/2004 04:34:00 AM  kitani_toshikazu
  Process to change interface names

  Revision 1.5  04/23/2004 12:27:24 AM  kitani_toshikazu
  Added workaround for color palette swap table.

  Revision 1.4  04/21/2004 11:49:04 AM  kitani_toshikazu
  Renamed functions

  Revision 1.3  04/21/2004 07:11:27 AM  kitani_toshikazu
  Updated the folder structures related to the internal module

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_CellAnimation.h>
#include <nnsys/g2d/g2d_SRTControl.h>

#include <nnsys/g2d/load/g2d_NCE_load.h>
#include <nnsys/g2d/fmt/g2d_Oam_data.h>

#include "g2d_Internal.h"

//------------------------------------------------------------------------------
// Applies animation changes
//
// This is a bit disorderly, but regardless of the animation format, it will be accessed by NNSG2dAnimDataSRT.
// (If the animation format is NNS_G2D_ANIMELEMENT_INDEX, it will cause incorrect data to be put into the SRT section.)
//     
static void ApplyCurrentAnimResult_( NNSG2dCellAnimation* pCellAnim )
{
    const NNSG2dAnimDataSRT*   pAnimResult  = NULL;
    const NNSG2dCellDataBank*  pCellBank    = NULL;
    
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCellAnim->pCellDataBank );
    
    //
    // Changed so that animation frames with 0 display time will not update.
    //
    if( pCellAnim->animCtrl.pCurrent->frames == 0 )
    {
        return; 
    }
    
    //
    // Planning to handle multiple formats
    //
    pAnimResult = (const NNSG2dAnimDataSRT*)NNS_G2dGetAnimCtrlCurrentElement( &pCellAnim->animCtrl );
    pCellBank   = pCellAnim->pCellDataBank;
    
    NNSI_G2D_DEBUGMSG0( "pAnimResult->index = %d\n", pAnimResult->index );
    
    pCellAnim->pCurrentCell 
        = NNS_G2dGetCellDataByIdx( pCellBank, pAnimResult->index );
    NNS_G2D_NULL_ASSERT( pCellAnim->pCurrentCell );
    
    //
    // Use SRT
    //
    {
       const NNSG2dAnimationElement elemType 
           = NNSi_G2dGetAnimSequenceElementType( pCellAnim->animCtrl.pAnimSequence->animType );
       NNSi_G2dSrtcInitControl  ( &pCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
       
       if( elemType != NNS_G2D_ANIMELEMENT_INDEX )
       {
           if( elemType == NNS_G2D_ANIMELEMENT_INDEX_T )
           {
              const NNSG2dAnimDataT*  pAnmResT = (const NNSG2dAnimDataT*)pAnimResult;
              NNSi_G2dSrtcSetTrans     ( &pCellAnim->srtCtrl, pAnmResT->px, pAnmResT->py );// T
           }else{
              NNSi_G2dSrtcSetSRTScale  ( &pCellAnim->srtCtrl, pAnimResult->sx, pAnimResult->sy );// S    
              NNSi_G2dSrtcSetSRTRotZ   ( &pCellAnim->srtCtrl, pAnimResult->rotZ );// R    
              NNSi_G2dSrtcSetTrans     ( &pCellAnim->srtCtrl, pAnimResult->px, pAnimResult->py );// T    
           }
       }
    }
    
    
   
    //
    // If VRAM transfer information is set...
    //
    if( NNS_G2dCellDataBankHasVramTransferData( pCellBank ) && 
        NNSi_G2dIsCellAnimVramTransferHandleValid( pCellAnim ) )
    {
        const NNSG2dCellVramTransferData*   pCellTransferData 
            = NNSi_G2dGetCellVramTransferData( pCellBank, pAnimResult->index );   
        //
        // Requests a transfer
        //
        NNS_G2dSetCellTransferStateRequested( pCellAnim->cellTransferStateHandle,
                                              pCellTransferData->srcDataOffset,
                                              pCellTransferData->szByte );
    }
}

//------------------------------------------------------------------------------
// Initializes cell animation.
// Part that implements NNS_G2dInitCellAnimation.
NNS_G2D_INLINE void InitCellAnimationImpl_
( 
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq, 
    const NNSG2dCellDataBank*   pCellDataBank,
    u32                         cellTransferStateHandle 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    

    pCellAnim->pCellDataBank            = pCellDataBank;
    pCellAnim->cellTransferStateHandle   = cellTransferStateHandle;
    
    // TODO : The srtCtrl type needs to be set according to the animation format
    //       It needs to be reset each time there is a bind.
    NNSi_G2dSrtcInitControl( &pCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
    
    NNS_G2dInitAnimCtrl( &pCellAnim->animCtrl );
    NNS_G2dSetCellAnimationSequence( pCellAnim, pAnimSeq );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitCellAnimation

  Description:  Initializes a NNSG2dCellAnimation instance.
                
  Arguments:    pCellAnim:           [OUT]  The cell animation instance
                pAnimSeq:            [IN]   Animation data
                pCellDataBank:       [IN]   Cell databank 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitCellAnimation
( 
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq, 
    const NNSG2dCellDataBank*   pCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    
    InitCellAnimationImpl_( pCellAnim, 
                            pAnimSeq, 
                            pCellDataBank, 
                            NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE ); 
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitCellAnimationVramTransfered

  Description:  VRAM transfer animation. Initialize cell animation.
                
  Arguments:    pCellAnim             :           [OUT]  The actual cell animation 
                pAnimSeq:            [IN]   Animation data
                pCellBank:           [IN]   Cell databank 
                vramStateHandle:     [IN] Handle of the cell VRAM transfer state object. 
                dstAddr3D:           [IN]  Transfer destination data (for 3D)
                dstAddr2DMain:       [IN]  Transfer destination data (for 2D main)
                dstAddr2DSub:        [IN]  Transfer destination data (for 2D sub)
                pSrcNCGR:            [IN] Transfer source data (NCGR)
                pSrcNCBR:            [IN] Transfer source data (NCBR)
                szSrcData:           [IN]  Transfer source data size
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitCellAnimationVramTransfered
(
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq, 
    NNSG2dCellDataBank*         pCellBank,
    u32                         vramStateHandle,
    u32                         dstAddr3D,
    u32                         dstAddr2DMain,
    u32                         dstAddr2DSub,
    void*                       pSrcNCGR,
    void*                       pSrcNCBR,
    u32                         szSrcData
     
)
{
    //
    // Has VRAM transfer data
    //
    NNS_G2D_ASSERT( NNS_G2dCellDataBankHasVramTransferData( pCellBank ) );
    
    //
    // Initializes Vram transfer association
    //
    {                                                
        NNSG2dVramTransferData*    pVramData = (NNSG2dVramTransferData*)pCellBank->pVramTransferData;
        
        // Initializes the setup work
        NNSi_G2dInitCellTransferState( vramStateHandle,
                                        dstAddr3D,              // Sets the dst transfer destination Vram address
                                        dstAddr2DMain,          // Sets the dst transfer destination Vram address
                                        dstAddr2DSub,           // Sets the dst transfer destination Vram address
                                        pVramData->szByteMax,   // Maximum data size for dst transfer
                                        pSrcNCGR,               // src 2D char-data
                                        pSrcNCBR,               // src 3D Tex-data
                                        szSrcData );            // src size
           
        NNSi_G2dSetCellAnimVramTransferHandle( pCellAnim, vramStateHandle );
    }
    
    //
    // Initialize cell animation
    //
    InitCellAnimationImpl_( pCellAnim, pAnimSeq, pCellBank, vramStateHandle );        
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellAnimationSequence

  Description:  Sets the animation sequence to the cell animation.

  Arguments:    pCellAnim             :      [OUT]  Cell animation instance
                pAnimSeq:     [IN]      Animation sequence
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellAnimationSequence
( 
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pAnimSeq ) == NNS_G2D_ANIMATIONTYPE_CELL, 
                       "A cell-Animation's Data is expected");
    
    NNS_G2dBindAnimCtrl( &pCellAnim->animCtrl, pAnimSeq );
    ApplyCurrentAnimResult_( pCellAnim );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellAnimationSequenceNoReset

  Description:  Sets the animation sequence to the cell animation.
                The internal playback animation frame number and the current frame display time are not reset.

  Arguments:    pCellAnim             :    [OUT]  Cell animation instance
                pAnimSeq:   [IN]      Animation sequence
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellAnimationSequenceNoReset
( 
    NNSG2dCellAnimation*        pCellAnim, 
    const NNSG2dAnimSequence*   pAnimSeq 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pAnimSeq ) == NNS_G2D_ANIMATIONTYPE_CELL,
                       "A cell-Animation's Data is expected");
    
    {
        const u16 frameIdx = NNS_G2dGetAnimCtrlCurrentFrame( &pCellAnim->animCtrl );
        
        // The counter is not reset
        pCellAnim->animCtrl.pAnimSequence = pAnimSeq;
        
        if( !NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime( &pCellAnim->animCtrl, frameIdx ) )
        {
            NNS_G2dResetAnimationState( &pCellAnim->animCtrl );
        }
    }
    
    ApplyCurrentAnimResult_( pCellAnim );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickCellAnimation

  Description:  Advances the NNSG2dCellAnimation time
                
  Arguments:    pCellAnim             :          [OUT]  Cell animation instance
                frames:             [IN] The time to advance (in frames)
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dTickCellAnimation( NNSG2dCellAnimation* pCellAnim, fx32 frames )
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCellAnim->animCtrl.pAnimSequence );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pCellAnim->animCtrl.pAnimSequence ) 
        == NNS_G2D_ANIMATIONTYPE_CELL, "A cell-Animation's Data is expected");
    
    if( NNS_G2dTickAnimCtrl( &pCellAnim->animCtrl, frames ) )
    {
        //
        // Uses the animation results if a frame update occurs
        //
        ApplyCurrentAnimResult_( pCellAnim );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellAnimationCurrentFrame

  Description:  Sets the animation frame to the cell animation.
                Nothing occurs if an invalid frame number is specified.
                
  Arguments:    pCellAnim             :           [OUT]  Cell animation instance
                frameIndex:          [IN]  Animation frame number 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellAnimationCurrentFrame
( 
    NNSG2dCellAnimation*    pCellAnim, 
    u16                     frameIndex 
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCellAnim->animCtrl.pAnimSequence );
    
    if( NNS_G2dSetAnimCtrlCurrentFrame( &pCellAnim->animCtrl, frameIndex ) )
    {
        //
        // Uses the animation results if a frame update occurs
        //
        ApplyCurrentAnimResult_( pCellAnim );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellAnimationSpeed

  Description:  Sets the playback speed of the cell animation.
                
  Arguments:    pCellAnim             :           [OUT]  Cell animation instance
                speed:          [IN]  Playback speed 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellAnimationSpeed
(
    NNSG2dCellAnimation*     pCellAnim,
    fx32                     speed  
)
{
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCellAnim->animCtrl.pAnimSequence );
    
    NNS_G2dSetAnimCtrlSpeed( &pCellAnim->animCtrl, speed );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMakeCellToOams

  Description:  Converts a cell to an Oam column
                
  Arguments:    pDstOams:           [OUT] Buffer for storing Oam
                numDstOam:          [IN] Buffer length 
                pCell:              [IN] Conversion source cell 
                pMtxSR:             [IN] Conversion to set in the cell (optional)
                pBaseTrans:         [IN] Parallel translation to set in the cell (optional)
                bDoubleAffine :      [IN] DoubleAffine mode or not 
                
  Returns:      Number of Oam attributes when actually converting
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dMakeCellToOams
( 
    GXOamAttr*              pDstOams,
    u16                     numDstOam, 
    const NNSG2dCellData*   pCell, 
    const MtxFx22*          pMtxSR, 
    const NNSG2dFVec2*      pBaseTrans,
    u16                     affineIndex,
    BOOL                    bDoubleAffine 
)
{
    u16             i = 0;
    NNSG2dFVec2     objTrans;
    GXOamAttr*      pDstOam = NULL;
    const u16       numOBJ 
        = ( numDstOam < pCell->numOAMAttrs ) ? numDstOam : pCell->numOAMAttrs;
    
    
    for( i = 0; i < numOBJ; i++ )
    {
        pDstOam = &pDstOams[i];
        
        NNS_G2dCopyCellAsOamAttr( pCell, i, pDstOam );
        
        //
        // If the OBJ position needs to be changed...
        //
        if( pMtxSR != NULL || pBaseTrans != NULL )
        {
            //
            // Gets the position
            //
            NNS_G2dGetOamTransFx32( pDstOam, &objTrans );
                    
            //
            // If affine transform is specified...
            // 
            if( pMtxSR != NULL )
            {   
                //
                // If OBJ was set for double-size affine
                // Temporarily remove the correction value added by NITRO-CHARACTER
                //
                NNSi_G2dRemovePositionAdjustmentFromDoubleAffineOBJ( pDstOam, 
                                                                     &objTrans );               
                {
                    // Overwrite
                    const GXOamEffect effectTypeAfter = ( bDoubleAffine ) ? 
                                             GX_OAM_EFFECT_AFFINE_DOUBLE : GX_OAM_EFFECT_AFFINE;                
                    const BOOL bShouldAdjust = ( effectTypeAfter  == GX_OAM_EFFECT_AFFINE_DOUBLE );
        
                    MulMtx22( pMtxSR, &objTrans, &objTrans );
                    
                    // Set affine Index
                    G2_SetOBJEffect( pDstOam, effectTypeAfter, affineIndex );
                    
                    NNSi_G2dAdjustDifferenceOfRotateOrientation( pDstOam, 
                                                                 pMtxSR, 
                                                                 &objTrans, 
                                                                 bShouldAdjust );
                }
            }
            //
            // If parallel translation portion is specified...
            // 
            if( pBaseTrans != NULL )
            {
                objTrans.x += pBaseTrans->x;
                objTrans.y += pBaseTrans->y;
            }
            
            //
            // Write back
            //
            // 0x800 => Intended to round off
            G2_SetOBJPosition( pDstOam, 
                               ( objTrans.x + 0x800 )>> FX32_SHIFT, 
                               ( objTrans.y + 0x800 )>> FX32_SHIFT );
        }
    }
    return numOBJ;
}


