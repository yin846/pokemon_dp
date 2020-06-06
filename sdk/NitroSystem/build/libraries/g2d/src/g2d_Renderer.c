/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2 D 
  
  File:     g2d_Renderer.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Renderer.c,v $
  Revision 1.59  06/29/2005 08:30:27 AM  kitani_toshikazu
  Corrected comment

  Revision 1.58  06/02/2005 01:56:52 AM  kitani_toshikazu
  Cleaned up the code

  
  Revision 1.56  02/22/2005 12:51:14 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.55  02/21/2005 02:17:57 AM  hatamoto_minoru
  Response for warnings about implicit sign/unsign conversion.

  Revision 1.54  02/08/2005 08:22:13 AM  kitani_toshikazu
  IsNotCircularLinked_()
  Fixed internal bug 

  Revision 1.53  12/13/2004 11:57:39 PM  kitani_toshikazu
  (none)

  Revision 1.52  12/13/2004 08:24:28 AM  kitani_toshikazu
  Merged branch g2d_multiCell

  Revision 1.51.2.3  2004/12/13 02:54:14  kitani_toshikazu
  Merged the new multicell module

  Revision 1.51.2.2  2004/12/09 07:36:23  kitani_toshikazu
  Support new multi-cell module

  Revision 1.51.2.1  2004/12/09 06:37:24  kitani_toshikazu
  Support new multi-cell module

  Revision 1.51  12/09/2004 05:43:04 AM  kitani_toshikazu
  Fixed initialization that was missed in the initialization function.

  Revision 1.50  12/02/2004 11:23:49 AM  kitani_toshikazu
  Fixed the timing of the load to the matrix cache of the current matrix.
  (Moved it to before render core configuration)

  Revision 1.49  11/11/2004 05:21:26 AM  kitani_toshikazu
  Discontinued use of NNS_G2dSetRndCoreOamRegisterFuncEx() function.

  Revision 1.48  11/05/2004 06:12:40 AM  kitani_toshikazu
  Changed API.

  Revision 1.47  11/04/2004 10:14:16 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.46  11/02/2004 05:17:51 AM  kitani_toshikazu
  Integrated parameter rewrite callback functions.

  Revision 1.45  10/29/2004 08:40:41 AM  kitani_toshikazu
  Added an assert sentence

  Revision 1.44  10/29/2004 08:25:12 AM  kitani_toshikazu
  Inserted comments

  Revision 1.43  10/29/2004 04:06:25 AM  kitani_toshikazu
  Added code to avoid cells that don't use SRT animation, and PushPop
  not needed in rendering multi-cell animation.

  Revision 1.42  10/28/2004 10:44:30 AM  kitani_toshikazu
  Rendering Optimization Flag
  Added NNS_G2D_RDR_OPZHINT_LOCK_PARAMS processing.
  Added parameter rewrite item.

  Revision 1.40  10/28/2004 12:04:23 AM  kitani_toshikazu
  Optimized performance

  Revision 1.39  10/21/2004 11:01:29 AM  kitani_toshikazu
  Added NNS_G2dSetTrans()

  Revision 1.36  10/18/2004 01:15:58 AM  kitani_toshikazu
  Arranged renderer module, code.

  Revision 1.35  10/15/2004 02:04:56 AM  kitani_toshikazu
  Fixed render surface bActive flag determination bug

  Revision 1.34  10/14/2004 10:07:23 AM  kitani_toshikazu
  Supported splitting up of matrix cache module.

  Revision 1.33  10/14/2004 12:59:38 AM  kitani_toshikazu
  Arranged code (matrix stack, splitting up of code related to matrix cache)

  Revision 1.30  10/07/2004 12:47:54 PM  kitani_toshikazu
  Separated basic parts into separate modules.

  Revision 1.29  09/17/2004 01:47:49 AM  kitani_toshikazu
  Implemented affine conversion mode rewrite feature.

  Revision 1.28  09/15/2004 05:49:55 AM  kitani_toshikazu
  Affine transformation mode configuration routine removed from lower module.

  Revision 1.27  09/13/2004 05:50:45 AM  kitani_toshikazu
  Enabled render surface bActive flag.

  Revision 1.26  09/08/2004 06:25:42 AM  hatamoto_minoru
  Fixed the bug that resulted in a psuedo-infinite loop during SRT cell drawing.

  Revision 1.25  08/26/2004 07:53:10 AM  kitani_toshikazu
  Corrected a bug in the flip processing during 2D rendering

  Revision 1.24  08/26/2004 07:19:52 AM  kitani_toshikazu
  Implementation of the Z offset features

  Revision 1.23  08/23/2004 08:54:11 AM  kitani_toshikazu
  Addition of callback, flip rendering, and other render optimization features.

  Revision 1.22  08/17/2004 06:02:21 AM  kitani_toshikazu
  Implementation of the automatic Z value offset features

  Revision 1.20  07/29/2004 12:33:59 AM  kitani_toshikazu
  Changed function names.

  Revision 1.18  07/24/2004 07:42:05 AM  kitani_toshikazu
  Changing of function names.

  Revision 1.17  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.16  07/17/2004 07:23:46 AM  kitani_toshikazu
  Changed file name from g2d_OamSoftwareSpriteEmu to g2d_OamSoftwareSpriteDraw.

  Revision 1.15  07/17/2004 06:57:02 AM  kitani_toshikazu
  Reflected changes in function names.

  Revision 1.14  07/16/2004 02:35:06 AM  kitani_toshikazu
  Fixed bug with the character offset settings when drawing 2D graphics

  Revision 1.13  07/14/2004 11:56:09 AM  kitani_toshikazu
  Support for VRAM Transfer animations
  Fixed bug with 3DView local conversion
  Made material cache feature more efficient

  Revision 1.12  06/30/2004 11:33:11 AM  kitani_toshikazu
  Corrected multi-cell drawing part

  Revision 1.11  06/29/2004 06:48:38 AM  kitani_toshikazu
  Fixed bug with the NNS_G2dDrawEntity() function's palette conversion table process

  Revision 1.10  06/22/2004 06:34:13 AM  kitani_toshikazu
  Provided support for visible culling in cell units.

  Revision 1.8  06/01/2004 05:20:57 AM  kitani_toshikazu
  Changes for using expansion palette.

  Revision 1.6  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.5  04/23/2004 12:27:24 AM  kitani_toshikazu
  Added workaround for color palette swap table.

  Revision 1.4  04/21/2004 11:49:04 AM  kitani_toshikazu
  Renamed functions

  Revision 1.3  04/21/2004 07:11:27 AM  kitani_toshikazu
  Updated the folder structures related to the internal module

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g2d/g2d_Renderer.h>

#include "g2d_Internal.h"
#include "g2di_RendererMtxStack.hpp"    // For MatrixStack
#include "g2di_RendererMtxState.h"      // For MatrixCache & State


#include <nnsys/g2d/fmt/g2d_Oam_data.h>
#include <nnsys/g2d/g2d_OamSoftwareSpriteDraw.h> // Auto Z offset
#include <nnsys/g2d/g2d_SRTControl.h>








//------------------------------------------------------------------------------
#define NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pRnd ) \
    NNS_G2D_ASSERTMSG( (pRnd) != NULL, "Please call this method between Begin - End Rendering" ) \


//------------------------------------------------------------------------------
// current renderer entity
// Set when calling Begin-End Rendering()
static NNSG2dRendererInstance*      pCurrentInstance_   = NULL; 


//------------------------------------------------------------------------------
// Additional variable: Added for Vram transfer animation
static u32      currenVramTransferHandle_ = NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE;




typedef struct MCRenderState
{
    u16                         currentCellAnimIdx;
    u16                         pad16_;
    NNSG2dRndCore2DMtxCache*    cellAnimMtxCacheTbl[256];
    BOOL                        bDrawMC;

}MCRenderState;

static MCRenderState        mcRenderState_;







//------------------------------------------------------------------------------
// Checks whether NNSG2dRenderSurface list is not circular reference.
// Called in ASSERT macro during initialization and when registering NNSG2dRenderSurface.
// 
static BOOL IsNotCircularLinked_
( 
    const NNSG2dRenderSurface*      pList, 
    const NNSG2dRenderSurface*      pNew 
)
{
    const NNSG2dRenderSurface*  pCursor = pList;
    
    while( pCursor != NULL )
    {
        // Found the same pointer
        if( pCursor == pNew ) 
        {
            // NG
            return FALSE;
        }    
        pCursor = (const NNSG2dRenderSurface*)pCursor->pNextSurface;
    }
    
    // Circular reference is not generated
    return TRUE;
}




//------------------------------------------------------------------------------
// Process the palette number write in the OAMAttribute of the OBJ
// DrawCellToSurface2D_()
// DrawCellToSurface3D_()
// Actually executed inside just before the draw command is issued
// 
NNS_G2D_INLINE void OBJPaletteChangeHandling_( GXOamAttr* pOam )
{
    const NNSG2dPaletteSwapTable* pTbl 
        = NNS_G2dGetRendererPaletteTbl( pCurrentInstance_ );
    
    NNS_G2D_NULL_ASSERT( pOam );
    
    if( pTbl != NULL )
    {
        // Rewrite the palette number
        const u16 newIdx 
           = NNS_G2dGetPaletteTableValue( pTbl, NNSi_G2dGetOamColorParam( pOam ) );
        pOam->cParam = newIdx;
    }
}

//------------------------------------------------------------------------------
// Determines the flip condition of the renderer, then configures the translation
NNS_G2D_INLINE void FlipTranslate_( int x, int y )
{
    const int x_ = NNS_G2dIsRndCoreFlipH( &pCurrentInstance_->rendererCore ) ? -x : x;    
    const int y_ = NNS_G2dIsRndCoreFlipV( &pCurrentInstance_->rendererCore ) ? -y : y;
                            
    NNS_G2dTranslate( x_ << FX32_SHIFT , y_ << FX32_SHIFT, 0 );
}

//------------------------------------------------------------------------------
// Multiply the affine conversion information held by NNSG2dSRTControl by the current matrix.
// The NNSG2dSRTControl type must be NNS_G2D_SRTCONTROLTYPE_SRT.
//
//
NNS_G2D_INLINE void SetSrtControlToMtxStack_( const NNSG2dSRTControl* pSrtCtrl )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pSrtCtrl );
    NNS_G2D_ASSERTMSG( pSrtCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT, "TODO: show msg " );
    
    // T
    if( NNSi_G2dSrtcIsAffineEnable( pSrtCtrl, NNS_G2D_AFFINEENABLE_TRANS ) )
    {
        //
        // Reflect results of flip, and update translation parameters.
        //
        FlipTranslate_( pSrtCtrl->srtData.trans.x, pSrtCtrl->srtData.trans.y );                 
    }
    
    // R
    if( NNSi_G2dSrtcIsAffineEnable( pSrtCtrl, NNS_G2D_AFFINEENABLE_ROTATE ) )
    {
        NNS_G2dRotZ( FX_SinIdx( pSrtCtrl->srtData.rotZ ), FX_CosIdx( pSrtCtrl->srtData.rotZ ) );
    }
    
    // S
    if( NNSi_G2dSrtcIsAffineEnable( pSrtCtrl, NNS_G2D_AFFINEENABLE_SCALE ) )
    {
        NNS_G2dScale( pSrtCtrl->srtData.scale.x, pSrtCtrl->srtData.scale.y, FX32_ONE );
    }

}


//------------------------------------------------------------------------------
// Call before and after the rendering of the Cell that uses the VRAM transmission.
NNS_G2D_INLINE void BeginDrawVramTransferedCell_( u32 cellVramTransferHandle )
{
    NNS_G2D_ASSERT( currenVramTransferHandle_ == NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );
    NNS_G2D_ASSERT( cellVramTransferHandle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );
    
    currenVramTransferHandle_ = cellVramTransferHandle;
}

//------------------------------------------------------------------------------
// Call before and after the rendering of the Cell that uses the VRAM transmission.
NNS_G2D_INLINE void EndDrawVramTransferedCell_( )
{
    NNS_G2D_ASSERT( currenVramTransferHandle_ != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );    
    currenVramTransferHandle_ = NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE;
}

//------------------------------------------------------------------------------
// Is the Renderer currently rendering the VRAM transmission animation Cell?
NNS_G2D_INLINE BOOL IsRendererDrawingVramTransferedCell_( )
{
    return (BOOL)(currenVramTransferHandle_ != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE);
}

//------------------------------------------------------------------------------
// Gets the handle of the current ram transfer animation cell.
NNS_G2D_INLINE u32 GetCurrentVramTransfereHandle_()
{
    return currenVramTransferHandle_;
}



//------------------------------------------------------------------------------
// Registers the renderer core module to the callback, and customizes the operation.
// The renderer core module implements operation that is the same as existing modules through customization.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// callback before cell rendering
// * culling process
// * loading of current matrix to 2D matrix cache
// * callback call before cell rendering of renderer module
//
static void RndCoreCBFuncBeforeCell_
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell
)    
{   
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    {
       NNSG2dRenderSurface* pCurrentSurface = pCurrentInstance_->pCurrentSurface;
        
       //
       // culling process
       //
       if( pCurrentSurface->pFuncVisibilityCulling != NULL )
       { 
            
           if( !(*pCurrentSurface->pFuncVisibilityCulling)( pCell, 
                                                     NNSi_G2dGetCurrentMtx() , 
                                                     &pCurrentSurface->coreSurface.viewRect ) )
           {
              // 
              // configure to skip draw in pRend
              // 
              pRend->bDrawEnable = FALSE;
              return;
           }else{
              pRend->bDrawEnable = TRUE;
           }
       }
       
       //
       // existing callback call
       //
       if( *pCurrentSurface->pBeforeDrawCellBackFunc )
       {   
           (*pCurrentSurface->pBeforeDrawCellBackFunc)( pCurrentInstance_,
                                                 pCurrentSurface,
                                                 pCell,
                                                 NNSi_G2dGetCurrentMtx() );
       }
    }
}
//------------------------------------------------------------------------------
// callback after cell rendering
//
// Registered as callback function in render core module.
static void RndCoreCBFuncAfterCell_
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell
)    
{
#pragma unused( pRend )
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    {
       NNSG2dRenderSurface* pCurrentSurface = pCurrentInstance_->pCurrentSurface;
    
       if( *pCurrentSurface->pAfterDrawCellBackFunc )
       {
           (*pCurrentSurface->pAfterDrawCellBackFunc)( pCurrentInstance_,
                                                 pCurrentSurface,
                                                 pCell,
                                                 NNSi_G2dGetCurrentMtx() );
       }
    }
}
//------------------------------------------------------------------------------
// callback before OJB rendering
//
// Registered as callback function in render core module.
//
// Overwrites Oam parameters.
// 
// In the current implementation, because parameters are overwritten before the call to pBeforeDrawOamBackFunc, 
// it is possible that parameters will be completely overwritten in the case of OBJs not drawn
// when performing object-level culling using pBeforeDrawOamBackFunc, etc.  
//
// There should be few OBJs for which individual OBJ culling needs to be carried out, and the current implementation is based on a decision to not worry about efficient processing in such cases. 
// Current implementation
//
// 
static void RndCoreCBFuncBeforeOBJ_
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell,
    u16                             oamIdx
)
{
    GXOamAttr*    pTempOam = &pRend->currentOam;
    
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    
    //
    // write parameter
    //
    // palette conversion table
    OBJPaletteChangeHandling_( pTempOam );
    
    if( pCurrentInstance_->overwriteEnableFlag != NNS_G2D_RND_OVERWRITE_NONE )
    {
       // drawing priority
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_PRIORITY ) )
       {
           pTempOam->priority = pCurrentInstance_->overwritePriority;
       }
        
       // palette number
       // Caution: This is prioritized. (Overwrites palette conversion table results.)
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_PLTTNO ) )
       {
           pTempOam->cParam = pCurrentInstance_->overwritePlttNo;
       }
       
       // Palette number (Added offset)
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS ) )
       {
           pTempOam->cParam = 0xF & ( pTempOam->cParam + pCurrentInstance_->overwritePlttNoOffset );
       }
       
       // Mosaic
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_MOSAIC ) )
       {
           G2_OBJMosaic( pTempOam, pCurrentInstance_->overwriteMosaicFlag );
       }
        
       // OBJ Mode
       if( NNS_G2dIsRendererOverwriteEnable( pCurrentInstance_, NNS_G2D_RND_OVERWRITE_OBJMODE ) )
       {
           G2_SetOBJMode( pTempOam, pCurrentInstance_->overwriteObjMode, G2_GetOBJColorParam(pTempOam));
       }
    }
    
    //
    // Callback call
    //
    {
       NNSG2dRenderSurface* pCurrentSurface = pCurrentInstance_->pCurrentSurface;
       if( *pCurrentSurface->pBeforeDrawOamBackFunc )
       {
           (*pCurrentSurface->pBeforeDrawOamBackFunc)( pCurrentInstance_,
                                                 pCurrentSurface,
                                                 pCell,
                                                 oamIdx,
                                                 NNSi_G2dGetCurrentMtx() );
       }
    }
}
//------------------------------------------------------------------------------
// Callback after drawing OBJ
//
// Registered as callback function in render core module.
static void RndCoreCBFuncAfterOBJ_
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell,
    u16                             oamIdx
)
{
#pragma unused( pRend )
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    {
       NNSG2dRenderSurface* pCurrentSurface = pCurrentInstance_->pCurrentSurface;
       if( *pCurrentSurface->pAfterDrawOamBackFunc )
       {
           (*pCurrentSurface->pAfterDrawOamBackFunc)( pCurrentInstance_,
                                                 pCurrentSurface,
                                                 pCell,
                                                 oamIdx,
                                                 NNSi_G2dGetCurrentMtx() );
       }
    }
}



//------------------------------------------------------------------------------
// Start rendering for render core 2D surface.
NNS_G2D_INLINE void BeginRndCoreRendering2D_
(
    NNSG2dRendererInstance*  pRnd,
    NNSG2dRenderSurface*     pSurface 
)
{
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_ASSERT( pRnd->pCurrentSurface == NULL ); 
    NNS_G2D_ASSERT( pSurface->type != NNS_G2D_SURFACETYPE_MAIN3D );
    NNS_G2D_NULL_ASSERT( pSurface );
    
    //
    // perform surface settings in render core
    //
    pRnd->pCurrentSurface = pSurface;
    NNS_G2dSetRndCoreSurface( &pRnd->rendererCore, &pSurface->coreSurface );
       
    //
    // settings of registration function for 2D drawing
    //
    {
        {
            NNS_G2dSetRndCoreOamRegisterFunc( &pRnd->rendererCore,
                                              pSurface->pFuncOamRegister,
                                              pSurface->pFuncOamAffineRegister );
        }
    }
    
    NNS_G2dRndCoreBeginRendering( &pRnd->rendererCore );
}

//------------------------------------------------------------------------------
// Starts rendering for render core 3D surface.
NNS_G2D_INLINE void BeginRndCoreRendering3D_
(
    NNSG2dRendererInstance*  pRnd, 
    NNSG2dRenderSurface*     pSurface 
)
{
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_ASSERT( pRnd->pCurrentSurface == NULL ); 
    NNS_G2D_ASSERT( pSurface->type == NNS_G2D_SURFACETYPE_MAIN3D );
    NNS_G2D_NULL_ASSERT( pSurface );
    
    //
    // perform surface settings in render core
    //
    pRnd->pCurrentSurface = pSurface;
    NNS_G2dSetRndCoreSurface( &pRnd->rendererCore, &pSurface->coreSurface );
       
    
    NNS_G2dRndCoreBeginRendering( &pRnd->rendererCore );
}

//------------------------------------------------------------------------------
// Ends drawing of render core.
NNS_G2D_INLINE void EndRndCoreRendering_( void )
{
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCurrentInstance_->pCurrentSurface );
    
    pCurrentInstance_->pCurrentSurface = NULL;
    NNS_G2dRndCoreEndRendering( );
}


//------------------------------------------------------------------------------
// render core module 2D drawing
// Made a separate function to improve the readability of the DrawCellImpl_ function.
NNS_G2D_INLINE void DoRenderByRndCore2D_( 
    const NNSG2dCellData*    pCell ,
    NNSG2dRndCoreInstance*   pRndCore
)
{
#pragma unused( pRndCore )
    NNS_G2D_NULL_ASSERT( pCell );
    NNS_G2D_NULL_ASSERT( pRndCore );
    //
    // Sets matrix cache in renderer.
    // (Z value must be previously set.))
    //
    {
       NNSG2dRndCore2DMtxCache* pMtx2D = NULL;

       //
       // If affine converting, sets MtxCache for 2D affine conversion.
       //
       if( NNSi_G2dIsRndCurrentMtxSRTransformed() )
       {
           //
           // If rendering a multi-cell...
           //
           if( mcRenderState_.bDrawMC )
           {
               //
               // Checks table to see whether it was previously loaded.
               // Identical cell animations in a multi-cell must reference the same affine parameters.
               // The renderer uses affine transformation, and stores the rendered cell animation matrix cache in a table.
               //
               pMtx2D 
                  = mcRenderState_.cellAnimMtxCacheTbl[mcRenderState_.currentCellAnimIdx];
               //
               // In the case of cell animations that have never been rendered...
               //
               if( pMtx2D == NULL )
               {
                  //
                  // loads the current matrix into matrix cache.
                  //
                  NNSi_G2dMCMStoreCurrentMtxToMtxCache();
                  //
                  // Acquires matrix cache of loaded matrix.
                  //
                  pMtx2D = NNSi_G2dMCMGetCurrentMtxCache();
                  //
                  // Stores the matrix cache in a table.
                  //
                  mcRenderState_.cellAnimMtxCacheTbl[mcRenderState_.currentCellAnimIdx] = pMtx2D;
              }
           }else{
               //
               // loads the current matrix into matrix cache.
               //
               NNSi_G2dMCMStoreCurrentMtxToMtxCache();
               //
               // Acquires matrix cache of loaded matrix.
               //
               pMtx2D = NNSi_G2dMCMGetCurrentMtxCache();
           }
       }                                                   
       //
       // sets matrix cache for affine conversion
       //
       NNS_G2dSetRndCoreCurrentMtx2D( NNSi_G2dGetCurrentMtx(), pMtx2D );
    }
    
    
    //
    // Passes render core to render process
    //
    if( IsRendererDrawingVramTransferedCell_( ) )
    {
        NNS_G2dRndCoreDrawCellVramTransfer( pCell, GetCurrentVramTransfereHandle_() );
    }else{
        NNS_G2dRndCoreDrawCell( pCell );
    }
}
              
//------------------------------------------------------------------------------
// Render core module, 3D render
// Made a separate function to improve the readability of the DrawCellImpl_ function.
NNS_G2D_INLINE void DoRenderByRndCore3D_
( 
    const NNSG2dCellData*    pCell ,
    NNSG2dRndCoreInstance*   pRndCore
)
{
    NNS_G2D_NULL_ASSERT( pCell );
    NNS_G2D_NULL_ASSERT( pRndCore );
    
    //
    // transfers drawing process to render core
    //
    NNS_G2dSetRndCore3DSoftSpriteZvalue( pRndCore, NNSi_G2dGetCurrentZ() );
    //
    // Set current matrix in render core.
    // (Z value must be previously set.))
    //
    NNS_G2dSetRndCoreCurrentMtx3D( NNSi_G2dGetCurrentMtx() );
    //
    // Passes render core to render process
    //
    if( IsRendererDrawingVramTransferedCell_( ) )
    {
        NNS_G2dRndCoreDrawCellVramTransfer( pCell, GetCurrentVramTransfereHandle_() );
    }else{
        NNS_G2dRndCoreDrawCell( pCell );
    }
}

//------------------------------------------------------------------------------
// Render cell
static void DrawCellImpl_( const NNSG2dCellData* pCell )
{
    NNSG2dRndCoreInstance*   pRndCore   = NULL;
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCell );

    pRndCore = &pCurrentInstance_->rendererCore;
    
    {
        NNSG2dRenderSurface*      pSurface = pCurrentInstance_->pTargetSurfaceList;
        //
        // When using only one surface, it is possible to perform just once, at time of Begin - End,
        // Parameter configuration can be avoided. 
        //
        if( pCurrentInstance_->opzHint & NNS_G2D_RDR_OPZHINT_LOCK_PARAMS )
        {
           // 
           // BeginRndCoreRenderingXX_() and EndRndCoreRendering_() are not called for each rendering function. 
           // This speeds up the operation. 
           // 
           //
           // If drawing to a 2D surface...
           //       
           if( pSurface->type != NNS_G2D_SURFACETYPE_MAIN3D )
           {
              //
              // actual draw process (2D)
              //                    
              DoRenderByRndCore2D_( pCell, pRndCore );    
           }else{
              //
              // actual draw process (3D)
              //                    
              DoRenderByRndCore3D_( pCell, pRndCore );
           }
        }else{
           // for each render surfaces...
           while( pSurface )
           {
              if( pSurface->bActive )
              {                
                  //
                  // If drawing to a 2D surface...
                  //
                  if( pSurface->type != NNS_G2D_SURFACETYPE_MAIN3D )
                  {
                     //
                     // actual draw process (2D)
                     //                    
                     BeginRndCoreRendering2D_( pCurrentInstance_, pSurface );
                         DoRenderByRndCore2D_( pCell, pRndCore );
                     EndRndCoreRendering_();
                  }else{
                     //
                     // actual draw process (3D)
                     //                    
                     BeginRndCoreRendering3D_( pCurrentInstance_, pSurface );
                         DoRenderByRndCore3D_( pCell, pRndCore );
                     EndRndCoreRendering_();
                  }
              }
              // Next surface ...
              pSurface = pSurface->pNextSurface;
           }
        }
    }
}



//------------------------------------------------------------------------------
static void DrawCellAnimationImpl_( const NNSG2dCellAnimation* pCellAnim )
{
    NNSG2dCellData*      pCell = NULL;
    NNS_G2D_NULL_ASSERT( pCellAnim );
    
    pCell = (NNSG2dCellData*)NNS_G2dGetCellAnimationCurrentCell( pCellAnim );
    NNS_G2D_NULL_ASSERT( pCell );
           
    //
    // When SRT animation is not used, avoids useless PushPop.
    //
    if( pCellAnim->srtCtrl.srtData.SRT_EnableFlag == NNS_G2D_AFFINEENABLE_NONE )
    {
           //
           // If this is a Cell that uses the VRAM transmission animation...
           //
           if( NNSi_G2dIsCellAnimVramTransferHandleValid( pCellAnim ) )
           {
                
              BeginDrawVramTransferedCell_( NNSi_G2dGetCellAnimVramTransferHandle( pCellAnim ) );
                  DrawCellImpl_( pCell );
              EndDrawVramTransferedCell_();
                
           }else{
                  DrawCellImpl_( pCell );
           }
    }else{
       NNS_G2dPushMtx(); 
           SetSrtControlToMtxStack_( &pCellAnim->srtCtrl );        
           //
           // If this is a Cell that uses the VRAM transmission animation...
           //
           if( NNSi_G2dIsCellAnimVramTransferHandleValid( pCellAnim ) )
           {
                
              BeginDrawVramTransferedCell_( NNSi_G2dGetCellAnimVramTransferHandle( pCellAnim ) );
                  DrawCellImpl_( pCell );
              EndDrawVramTransferedCell_();
                
           }else{
                  DrawCellImpl_( pCell );
           }     
       NNS_G2dPopMtx(1);
    }
}


//------------------------------------------------------------------------------
// Draws Node.
// This has been changed from externally released function to internal function.
static void DrawNode_( const NNSG2dNode* pNode )
{
    NNS_G2D_NULL_ASSERT( pNode );
    NNS_G2D_ASSERTMSG( pNode->type == NNS_G2D_NODETYPE_CELL, "NNS_G2D_NODETYPE_CELL is expected." );
    
    if( pNode->bVisible )
    {
        //
        // TODO :
        // codes below this line should be "pNode->type" depended.
        // for now, we expect the "pNode->type" is always "NNS_G2D_NODETYPE_CELL"
        //        
        NNSG2dCellAnimation*    pCellAnim = (NNSG2dCellAnimation*)pNode->pContent;
        NNS_G2D_NULL_ASSERT( pCellAnim );
        
        //
        // Should seldom become pNode->srtCtrl.srtData.SRT_EnableFlag == NNS_G2D_AFFINEENABLE_NONE
        // Therefore, does not do the process of avoiding Push Pop by condition branching like in
        // other places.
        // 
        NNS_G2dPushMtx();
            SetSrtControlToMtxStack_( &pNode->srtCtrl );    
            {                       
                DrawCellAnimationImpl_( pCellAnim );
            }
        NNS_G2dPopMtx(1);
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void DrawNode2_
( 
    const NNSG2dMultiCellInstance*         pMC, 
    const NNSG2dMultiCellHierarchyData*    pNodeData 
)
{
    const u16 cellAnimIdx          = NNSi_G2dGetMC2NodeCellAinmIdx( pNodeData );
    const NNSG2dMCCellAnimation*   cellAnimArray = pMC->pCellAnimInstasnces;
    NNS_G2D_MINMAX_ASSERT( cellAnimIdx, 0, pMC->pCurrentMultiCell->numCellAnim );
    mcRenderState_.currentCellAnimIdx = cellAnimIdx;
    
    NNS_G2dPushMtx();
       FlipTranslate_( pNodeData->posX, pNodeData->posY );
       {                       
           DrawCellAnimationImpl_( &cellAnimArray[cellAnimIdx].cellAnim );
       }
    NNS_G2dPopMtx(1);
}

//------------------------------------------------------------------------------
// Is the automatic Z value offset valid?
NNS_G2D_INLINE BOOL IsAutoZoffsetEnable_( void )
{
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    return (BOOL)pCurrentInstance_->spriteZoffsetStep;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRenderer

  Description:  Initializes the Renderer instance
                
                
  Arguments:    pRend   :      [OUT] Renderer instance
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitRenderer( NNSG2dRendererInstance* pRend )
{
    NNS_G2D_NULL_ASSERT( pRend );
    
    NNS_G2dInitRndCore( &pRend->rendererCore );
    
    pRend->pTargetSurfaceList   = NULL;
    pRend->pCurrentSurface      = NULL;
    pRend->pPaletteSwapTbl      = NULL;
    
    //
    // All are invalid in for optimization hint and prescribed value (optimization not performed)
    //
    pRend->opzHint = NNS_G2D_RDR_OPZHINT_NONE;
    
    pRend->spriteZoffsetStep = 0;
    
    // The overwriteEnableFlag is NNSG2dRendererOverwriteParam
    pRend->overwriteEnableFlag = NNS_G2D_RND_OVERWRITE_NONE; 
    pRend->overwritePriority   = 0;
    pRend->overwritePlttNo     = 0;  
    pRend->overwriteObjMode    = GX_OAM_MODE_NORMAL;
    pRend->overwriteMosaicFlag = FALSE;
    pRend->overwritePlttNoOffset = 0;
    
    //
    // initialization of matrix cache
    //
    NNSi_G2dMCMInitMtxCache();
    //
    // Sets use mode of matrix stack module.
    //
    NNSi_G2dSetRndMtxStackSRTransformEnableFlag( TRUE );
    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dAddRendererTargetSurface

  Description:  Adds the NNSG2dRenderSurface to the Renderer
                
                
  Arguments:    pRend   :      [OUT] Renderer instance
                pNew:       [IN] The NNSG2dRenderSurface to add
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dAddRendererTargetSurface( NNSG2dRendererInstance* pRend, NNSG2dRenderSurface* pNew )
{
    NNS_G2D_NULL_ASSERT( pRend );
    NNS_G2D_NULL_ASSERT( pNew );
    NNS_G2D_ASSERTMSG( IsNotCircularLinked_( pRend->pTargetSurfaceList, pNew ),
        "Circular linked lists is detected in NNS_G2dAddRendererTargetSurface()" );
    
    // add_front
    pNew->pNextSurface          = pRend->pTargetSurfaceList;
    pRend->pTargetSurfaceList   = pNew;
    
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRenderSurface

  Description:  Initializes the render surface.
                Callback function is registered internally in render core module.
                
                
  Arguments:    pSurface:      [OUT] Render surface instance
                
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitRenderSurface( NNSG2dRenderSurface* pSurface )
{
    NNS_G2D_NULL_ASSERT( pSurface );
    
    MI_CpuFill16( pSurface, 0x0, sizeof( NNSG2dRenderSurface ) );
    
    pSurface->coreSurface.bActive = TRUE;
    
    pSurface->coreSurface.type = NNS_G2D_SURFACETYPE_MAX;
    
    //
    // Registers callback function in render core module.
    //
    {
        NNSG2dRndCoreSurface* pS = &pSurface->coreSurface;
        
        pS->pBeforeDrawCellBackFunc   = RndCoreCBFuncBeforeCell_;
        pS->pAfterDrawCellBackFunc    = RndCoreCBFuncAfterCell_;
        pS->pBeforeDrawOamBackFunc    = RndCoreCBFuncBeforeOBJ_;
        pS->pAfterDrawOamBackFunc     = RndCoreCBFuncAfterOBJ_;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBeginRendering

  Description:  Performs configurations on the Renderer before rendering.
                Call before calling the rendering method of the Renderer.
                Do not call from within Begin End Rendering.
                
                Also, the NNS_G2dBeginRenderingEx() function is provided, which can specify
                hint flags for drawing optimization, and fulfill a role similar to this function.
                
                
  Arguments:    pRendererInstance:      [IN]  Renderer instance
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dBeginRendering( NNSG2dRendererInstance* pRendererInstance )
{
    NNS_G2D_NULL_ASSERT( pRendererInstance );
    NNS_G2D_ASSERTMSG( pCurrentInstance_ == NULL, 
        "Must be NULL, Make sure calling Begin - End correctly." );
    
    pCurrentInstance_ = pRendererInstance;
    
    NNSi_G2dMCMCleanupMtxCache();
    
    G3_PushMtx();            
    
    G3_Identity();
    NNSi_G2dIdentity();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBeginRenderingEx

  Description:  Makes settings necessary to begin rendering.
                Call before calling the rendering method of the Renderer.
                Do not call from within Begin End Rendering.
                For this function, hint flags can be specified to optimize rendering.
                
                Hints are created using a bitwise OR operation on the NNSG2dRendererOptimizeHint enumerator.  
                 
                
                Optimization hint flags are reset with NNS_G2dEndRendering().
                In other words, optimization hint flags are only valid in the
                Begin - End Rendering block of the renderer.
                
                This function calls NNS_G2dBeginRendering() as usual
                after performing preprocessing for optimization.
                
                
  Arguments:    pRendererInstance       [IN]  Renderer
                opzHint            [IN]      Optimization hint
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dBeginRenderingEx 
( 
    NNSG2dRendererInstance* pRendererInstance, 
    u32                     opzHint 
)
{
    NNS_G2D_NULL_ASSERT( pRendererInstance );
    NNS_G2D_ASSERT( pRendererInstance->opzHint == NNS_G2D_RDR_OPZHINT_NONE );
    
    pRendererInstance->opzHint = opzHint;
    //
    // Preprocessing for drawing optimization
    //
    {
       if( opzHint & NNS_G2D_RDR_OPZHINT_NOT_SR )
       {
           NNSi_G2dSetRndMtxStackSRTransformEnableFlag( FALSE );
       }
       
       //
       // When using only one surface, it is possible to perform just once, at time of Begin - End,
       // the configuration of surface parameters to the render core, performed for each cell draw.
       //
       if( opzHint & NNS_G2D_RDR_OPZHINT_LOCK_PARAMS )
       {
           NNSG2dRndCoreInstance*  pRndCore = &pRendererInstance->rendererCore;
           NNSG2dRenderSurface*    pSurface = pRendererInstance->pTargetSurfaceList;
            
           NNS_G2D_ASSERTMSG( pSurface->pNextSurface == NULL,
              "The number of target surface must be ONE. when you spesified the NNS_G2D_RDR_OPZHINT_LOCK_PARAMS flag." );  
              
           if( pSurface->bActive )
           {                
              //
              // If drawing to a 2D surface...
              //
              if( pSurface->type != NNS_G2D_SURFACETYPE_MAIN3D )
              {
                  BeginRndCoreRendering2D_( pRendererInstance, pSurface );
              }else{
                  BeginRndCoreRendering3D_( pRendererInstance, pSurface );
              }
           }
       }
    }

    
    NNS_G2dBeginRendering( pRendererInstance );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEndRendering

  Description:  Performs settings after renderer draw.
                The modified internal state is returned to its original status after rendering.
                
                When an optimization hint is specified (when NNS_G2dBeginRenderingEx() is used),
                This processing is performed after the optimization.
                
                Optimization hint flags are reset by this function.
                In other words, optimization hint flags are only valid in the
                Begin - End Rendering block of the renderer.
                
                
  Arguments:    None
  
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dEndRendering()
{
    // check push pop are called correctly.
    NNS_G2D_NULL_ASSERT( pCurrentInstance_ );
    
    
    G3_PopMtx(1);
    
    {
        //
        // If optimization flag is set, performs that process.
        //
        const u32 opzHint = pCurrentInstance_->opzHint;
        if( opzHint != NNS_G2D_RDR_OPZHINT_NONE )
        {
            //
            // Returns SR conversion settings of stack to original.
            //
            if( opzHint & NNS_G2D_RDR_OPZHINT_NOT_SR )
            {
                NNSi_G2dSetRndMtxStackSRTransformEnableFlag( TRUE );
            }
            
            //
            // When using only one surface, it is possible to perform just once, at time of Begin - End,
            // of renderer, the configuration of surface parameters to the render core, performed for each cell draw.
            //
            if( opzHint & NNS_G2D_RDR_OPZHINT_LOCK_PARAMS )
            {
                EndRndCoreRendering_();
            }
            //
            // Resets optimization flag.
            // (In other words, optimization flag is only valid in Begin - End.))
            //
            pCurrentInstance_->opzHint = NNS_G2D_RDR_OPZHINT_NONE;
        }    
    }
    
    pCurrentInstance_ = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawCell

  Description:  Renders the Cell.
                
                
  Arguments:    pCell: [IN] The Cell instance to render
                                  
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawCell( const NNSG2dCellData* pCell )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCell );
    
    if( IsAutoZoffsetEnable_() )
    {
        const fx32 offset = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pCurrentInstance_->spriteZoffsetStep );
        
        DrawCellImpl_( pCell );
        
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( offset );
        NNSi_G2dResetOamSoftEmuAutoZOffset();
    }else{
        DrawCellImpl_( pCell );
    }
}
   


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawCellAnimation

  Description:  Renders the CellAnimation.
                
                
  Arguments:    pMC             : [IN] The CellAnimation to render
                                  
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawCellAnimation( const NNSG2dCellAnimation* pCellAnim )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pCellAnim );
    
    if( IsAutoZoffsetEnable_() )
    {
        const fx32 offset = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pCurrentInstance_->spriteZoffsetStep );
        
        DrawCellAnimationImpl_( pCellAnim );
        
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( offset );
        NNSi_G2dResetOamSoftEmuAutoZOffset();
    }else{
        DrawCellAnimationImpl_( pCellAnim );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawMultiCell

  Description:  Renders the multi-cell.
                
                
  Arguments:    pMC             : [IN] The multicell instance to render
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawMultiCell
( 
    const NNSG2dMultiCellInstance*      pMC 
)
{
    u16 i;
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pMC );
    if( pMC->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       //
       // Only initializes the number of cell animations that use the multi- cell 2D rendering matrix cache table.
       //
       for( i = 0; i < pMC->pCurrentMultiCell->numCellAnim; i++ )
       {
           mcRenderState_.cellAnimMtxCacheTbl[i] = NULL;
       }
       mcRenderState_.bDrawMC = TRUE;
        
       if( IsAutoZoffsetEnable_() )
       {
           const fx32 offset = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
           NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
           NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pCurrentInstance_->spriteZoffsetStep );
            
           for( i = 0; i < pMC->pCurrentMultiCell->numNodes; i++ )
           {
              DrawNode2_( pMC, &pMC->pCurrentMultiCell->pHierDataArray[i] );
           }
            
           NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
           NNSi_G2dSetOamSoftEmuAutoZOffsetStep( offset );
           NNSi_G2dResetOamSoftEmuAutoZOffset();
       }else{
           for( i = 0; i < pMC->pCurrentMultiCell->numNodes; i++ )
           {
              DrawNode2_( pMC, &pMC->pCurrentMultiCell->pHierDataArray[i] );
           }
       }
        
       mcRenderState_.bDrawMC = FALSE;
    
    }else{
       const NNSG2dNode* pNode = pMC->pCellAnimInstasnces;
       if( IsAutoZoffsetEnable_() )
       {
           const fx32 offset = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
           
           NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
           NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pCurrentInstance_->spriteZoffsetStep );
            
           for( i = 0; i < pMC->pCurrentMultiCell->numNodes; i++ )
           {
              DrawNode_( &pNode[i] );
           }
            
           NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
           NNSi_G2dSetOamSoftEmuAutoZOffsetStep( offset );
           NNSi_G2dResetOamSoftEmuAutoZOffset();
       }else{
           for( i = 0; i < pMC->pCurrentMultiCell->numNodes; i++ )
           {
              DrawNode_( &pNode[i] );
           }
       }
    }
}






/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawMultiCellAnimation

  Description:  Draws multi-cell animation.
                Calls NNS_G2dDrawMultiCell() internally.
                
                The point that considers the SRT animation results of multi-cell
                animation is a point different from NNS_G2dDrawMultiCell().
                
                
  Arguments:    pMCAnim         : [IN] The multicell animation to render
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawMultiCellAnimation
( 
    const NNSG2dMultiCellAnimation*     pMCAnim 
)
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pMCAnim );
    
    if( pMCAnim->srtCtrl.srtData.SRT_EnableFlag == NNS_G2D_AFFINEENABLE_NONE )
    {
        NNS_G2dDrawMultiCell( &pMCAnim->multiCellInstance );    
    }else{
        NNS_G2dPushMtx();
            SetSrtControlToMtxStack_( &pMCAnim->srtCtrl );
            NNS_G2dDrawMultiCell( &pMCAnim->multiCellInstance );    
        NNS_G2dPopMtx(1);
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawEntity

  Description:  Renders the entity.
                Performs draw using the palette conversion table when the
                entity data has the palette conversion table.
                
                
  Arguments:    pEntity: [IN] The entity instance to render
                               
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawEntity( NNSG2dEntity* pEntity )
{
    BOOL bAffined = FALSE;
    
    BOOL bPaletteChange                 = FALSE;
    const NNSG2dPaletteSwapTable* pTbl  = NULL;
    
    
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    //
    // Configure the palette
    //
    bPaletteChange = NNS_G2dIsEntityPaletteTblEnable( pEntity );
    if( bPaletteChange )
    {
        pTbl = NNS_G2dGetRendererPaletteTbl( pCurrentInstance_ );
        NNS_G2dSetRendererPaletteTbl( pCurrentInstance_, pEntity->pPaletteTbl );
    }
    
    //
    // Renders according to the type of the Entity
    //
    {       
        switch( pEntity->pEntityData->type )
        {
        case NNS_G2D_ENTITYTYPE_CELL:
            {
                NNSG2dCellAnimation*  
                  pCellAnim = (NNSG2dCellAnimation*)pEntity->pDrawStuff;
                NNS_G2D_NULL_ASSERT( pCellAnim );
                NNS_G2dDrawCellAnimation( pCellAnim );
            }
            break;
        case NNS_G2D_ENTITYTYPE_MULTICELL:
            {
                NNSG2dMultiCellAnimation*  
                  pMCAnim = (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff;
                NNS_G2D_NULL_ASSERT( pMCAnim );
                NNS_G2dDrawMultiCellAnimation( pMCAnim );
            }
            break;
        default:
            NNS_G2D_ASSERTMSG( FALSE, "TODO: msg ");
        }    
    }
    
    //
    // Restore the palette to its original state
    //
    if( bPaletteChange )
    {
        if( pTbl != NULL )
        {
            NNS_G2dSetRendererPaletteTbl( pCurrentInstance_, pTbl );
        }else{
            NNS_G2dResetRendererPaletteTbl( pCurrentInstance_ );
        }
    }
}




/*---------------------------------------------------------------------------*
  Name:         NNS_G2dPushMtx

  Description:  Operates the matrix stack internal to the Renderer.
                The matrix stack is processed by the CPU.
                
                Due to differences in scale parameter interpretation on the 2D and 3D graphics engine,
                there are two separate internal matrix stacks, one for 2D and one for 3D. (NNS_G2dScale)
                
                
  Arguments:    None
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dPushMtx()
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    //
    // Renderer module is better in terms of performance, so matrix calculation
    // is performed by CPU regardless of use state of graphics engine.
    //
    if( !(pCurrentInstance_->opzHint & NNS_G2D_RDR_OPZHINT_NOT_SR) )
    {
        const u16 lastPos = NNSi_G2dGetMtxStackPos();
        NNSi_G2dMtxPush();
        {
            const u16 newPos = NNSi_G2dGetMtxStackPos();
            //
            // Updates state of current matrix
            //
            NNSi_G2dMCMSetMtxStackPushed( newPos, lastPos );
        }
    }else{
        NNSi_G2dMtxPush();
    }    
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dPopMtx

  Description:  Pops the matrix stack inside the Renderer.
                
  Arguments:    None
                 
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dPopMtx()
{ 
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dMtxPop();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTranslate

  Description:  Multiplies the current matrix by the translation matrix in the renderer.
                
  Arguments:    x:      [IN]  trans x
                y:      [IN]  trans y
                z:      [IN]  trans z
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dTranslate(fx32 x, fx32 y, fx32 z )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dTranslate( x, y, z );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTranslate

  Description:  Sets translation component of current matrix in renderer.
                
  Arguments:    x:      [IN]  trans x
                y:      [IN]  trans y
                z:      [IN]  trans z
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetTrans(fx32 x, fx32 y, fx32 z )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dSetTrans( x, y, z );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dScale

  Description:  Multiplies the current matrix by the scale matrix in the Renderer.
                
  Arguments:    x:      [IN]  scale x
                y:      [IN]  scale y
                z:      [IN]  scale z
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dScale(fx32 x, fx32 y, fx32 z )
{
#pragma unused( z )
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dScale( x, y );
    // 
    // When including the affine transform, the Mtx must be applied to HW. ( 2D Graphics Engine )
    // If in a state where the current SR transform is not included,
    //      it is newly configured to a state where the SR transform is included.
    //
    if( !NNSi_G2dIsRndCurrentMtxSRTransformed() )
    {
        NNS_G2D_WARNING( pCurrentInstance_->rendererCore.flipFlag == NNS_G2D_RENDERERFLIP_NONE, 
                    "You can't use affine transformation using flip function." );
        NNSi_G2dSetRndMtxStackSRTransformed();
    }
    
    NNSi_G2dMCMSetCurrentMtxSRChanged();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRotZ

  Description:  Multiplies the current matrix by the rotation matrix in the Renderer.
                
  Arguments:    sin:      [IN]  sin value
                cos:      [IN]  cos value
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRotZ( fx32 sin, fx32 cos )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT( pCurrentInstance_ );
    NNSi_G2dRotate( sin, cos );
    
    // 
    // When including the SR transform, the Mtx must be applied to HW. ( 2D Graphics Engine )
    // If in a state where the current SR transform is not included,
    //      it is newly configured to a state where the SR transform is included.
    //
    if( !NNSi_G2dIsRndCurrentMtxSRTransformed() )
    {
        NNS_G2D_WARNING( pCurrentInstance_->rendererCore.flipFlag == NNS_G2D_RENDERERFLIP_NONE, 
                    "You can't use affine transformation using flip function." );
        NNSi_G2dSetRndMtxStackSRTransformed();
    }
    
    NNSi_G2dMCMSetCurrentMtxSRChanged();   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRendererFlipMode

  Description:  Configures the flip rendering settings of the renderer.
                Caution:
                    The affine conversion features cannot be used if the flip rendering is in effect.
                
                Can be called either inside or outside of the Begin - End rendering block.
                
                
  Arguments:    pRend   :       [OUT] Renderer instance
                bFlipH:      [IN]  Using H flip? 
                bFlipV:      [IN]  Using V flip? 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRendererFlipMode
( 
    NNSG2dRendererInstance* pRend, 
    BOOL bFlipH, 
    BOOL bFlipV 
)
{    
    NNS_G2D_WARNING( !NNSi_G2dIsRndCurrentMtxSRTransformed(), 
       "You can't use the flip function using affine transformation." );
    NNS_G2D_NULL_ASSERT( pRend );
    
    NNS_G2dSetRndCoreFlipMode( &pRend->rendererCore, bFlipH, bFlipV );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRendererPaletteTbl

  Description:  Configures the palette conversion table settings in the Renderer entity.
  
                Can be called either inside or outside of the Begin - End rendering block.
                
  Arguments:    pRend   :      [OUT] Renderer instance
                pTbl:       [IN] Palette conversion table
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRendererPaletteTbl
( 
    NNSG2dRendererInstance*         pRend, 
    const NNSG2dPaletteSwapTable*   pTbl 
)
{
    NNS_G2D_NULL_ASSERT( pRend );
    NNS_G2D_NULL_ASSERT( pTbl );
    
    pRend->pPaletteSwapTbl = pTbl;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetRendererPaletteTbl TODO:

  Description:  Gets the color palette conversion table settings of the Renderer entity.
                
                Can be called either inside or outside of the Begin - End rendering block.
                
  Arguments:    pRend   :      [OUT] Renderer instance
                
  Returns:      color palette conversion table
  
 *---------------------------------------------------------------------------*/
const NNSG2dPaletteSwapTable* 
NNS_G2dGetRendererPaletteTbl( NNSG2dRendererInstance* pRend )
{
    NNS_G2D_NULL_ASSERT( pRend );
    
    return pRend->pPaletteSwapTbl;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetRendererPaletteTbl

  Description:  Resets the color palette conversion table settings in the Renderer entity.
  
                Can be called either inside or outside of the Begin - End rendering block.
                
  Arguments:    pRend   :      [OUT] Renderer instance
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetRendererPaletteTbl( NNSG2dRendererInstance* pRend )
{
    NNS_G2D_NULL_ASSERT( pRend );
    pRend->pPaletteSwapTbl = NULL;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRendererImageProxy

  Description:  Configures the image information and the palette information in the Renderer entity.
                Execute this before using the Renderer entity.
                
                
                This function can also be used inside the Begin End Rendering() block.
                
                Caution:
                When rendering with the optimization flag and NNS_G2D_RDR_OPZHINT_LOCK_PARAMS flag enabled, 
                their use is prohibited in the Begin End Rendering() block. 
                
                This is because the Begin-End Rendering() in the renderer core module will be called 
                at the same time as Begin-End Rendering() in the renderer module. 
                
                
                In the renderer core module, it is forbidden to change the image proxy settings inside Begin-End Rendering().   
                
                (This is to perform preprocessing of ImageProxy-related parameters in Begin Rendering().)
                
                
                
                
                
  Arguments:    pRend   :      [OUT] Renderer instance
                pImgProxy:  [IN]  The image information configured in the Renderer
                pPltProxy:  [IN]  The palette information configured in the Renderer
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRendererImageProxy
( 
    NNSG2dRendererInstance*             pRend, 
    const NNSG2dImageProxy*             pImgProxy, 
    const NNSG2dImagePaletteProxy*      pPltProxy
)
{
    NNS_G2D_NULL_ASSERT( pRend );
    NNS_G2D_NULL_ASSERT( pImgProxy );
    NNS_G2D_NULL_ASSERT( pPltProxy );
    
    SDK_WARNING( pImgProxy->attr.bExtendedPlt == pPltProxy->bExtendedPlt, 
        "Palette type mismatching was detected.\n Make sure that you use the correct palette." );
    
    NNS_G2D_WARNING( !(pRend->opzHint & NNS_G2D_RDR_OPZHINT_LOCK_PARAMS),
        "Avoid calling this function, when you specified the optimize flag NNS_G2D_RDR_OPZHINT_LOCK_PARAMS." );
    
    NNS_G2dSetRndCoreImageProxy( &pRend->rendererCore, pImgProxy, pPltProxy );
}


