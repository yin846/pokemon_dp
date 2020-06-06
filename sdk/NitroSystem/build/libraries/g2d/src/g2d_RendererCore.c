/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2 D 
  
  File:     g2d_RendererCore.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_RendererCore.c,v $
  Revision 1.30  06/29/2005 08:30:27 AM  kitani_toshikazu
  Corrected comment

  Revision 1.29  06/02/2005 08:55:33 AM  kitani_toshikazu
  Cleaned up comments

  Revision 1.28  06/02/2005 01:56:52 AM  kitani_toshikazu
  Cleaned up the code

  Revision 1.27  2005/06/01 06:19:28 AM  kitani_toshikazu
  NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED
  was added. 

  Revision 1.26  2005/06/01 05:07:32 AM  kitani_toshikazu
  Cleaned up the code

  Revision 1.25  05/31/2005 11:27:39 AM  kitani_toshikazu
  Reivsed double-size affine OBJ correction calculations

  Revision 1.24  05/30/2005 12:04:38 PM  kitani_toshikazu
  Changed so that position correction only occurs when double-size affine flag is actually overwritten

  Revision 1.23  04/28/2005 02:31:09 AM  kitani_toshikazu
  Replaced NNSi_G2dRepeatXinCellSpace()
  with NNS_G2dRepeatXinCellSpace

  Revision 1.22  04/06/2005 08:45:09 AM  kitani_toshikazu
  (none)

  Revision 1.21  02/22/2005 12:51:14 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.20  02/21/2005 02:17:57 AM  hatamoto_minoru
  Response for warnings about implicit sign/unsign conversion.

  Revision 1.19  11/11/2004 05:21:26 AM  kitani_toshikazu
  Discontinued use of NNS_G2dSetRndCoreOamRegisterFuncEx() function.

  Revision 1.18  11/10/2004 05:34:46 AM  kitani_toshikazu
  NNSi_G2dGetRndCore2DMtxCacheAffineIndex
  NNSi_G2dSetRndCore2DMtxCacheAffineIndex.
  was moved. 

  Revision 1.16  11/09/2004 08:28:34 AM  kitani_toshikazu
  Added an assert sentence

  Revision 1.15  11/05/2004 06:12:35 AM  kitani_toshikazu
  Changed API.

  Revision 1.14  11/02/2004 05:17:03 AM  kitani_toshikazu
  Corrected the problem with the bDrawEnable flag update process

  Revision 1.13  10/29/2004 08:25:12 AM  kitani_toshikazu
  Inserted comments

  Revision 1.12  10/28/2004 10:45:33 AM  kitani_toshikazu
  Made changes to set NULL to the OAM block transfer function pointer
  when setting normal OAM registration function

  Revision 1.11  10/28/2004 02:02:04 AM  kitani_toshikazu
  Adjusted API

  Revision 1.10  10/28/2004 12:27:52 AM  kitani_toshikazu
  (none)

  Revision 1.9  10/28/2004 12:04:20 AM  kitani_toshikazu
  Optimized performance

  Revision 1.8  10/21/2004 11:01:42 AM  kitani_toshikazu
  Revised to optimize performance

  Revision 1.7  10/19/2004 11:58:24 AM  kitani_toshikazu
  Interface adjustment with renderer core.

  Revision 1.6  10/18/2004 01:15:31 AM  kitani_toshikazu
  Arranged renderer module, code.

  Revision 1.5  10/15/2004 02:04:51 AM  kitani_toshikazu
  Fixed render surface bActive flag determination bug

  Revision 1.4  10/14/2004 10:06:36 AM  kitani_toshikazu
  Changed composition of NNSG2dRndCore2DMtxCache members.

  Revision 1.3  10/14/2004 01:00:11 AM  kitani_toshikazu
  Dealt with the specification change regarding
  saving software sprite module and current matrix status

  Revision 1.2  10/08/2004 04:39:58 AM  kitani_toshikazu
  Changed function names.

  Revision 1.1  10/07/2004 12:52:32 PM  kitani_toshikazu
  Separated basic part of renderer to a separate module.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g2d/g2d_RendererCore.h>
#include "g2di_RendererMtxCache.h"      // For MatrixCache & State


#include <nnsys/g2d/fmt/g2d_Cell_data.h>
#include <nnsys/g2d/g2d_OamSoftwareSpriteDraw.h>
#include <nnsys/g2d/g2d_CellTransferManager.h>




#define NNSi_G2D_RNDCORE_DUMMY_FLAG                FALSE
#define NNSi_G2D_RNDCORE_OAMENTORY_SUCCESSFUL      TRUE



static NNSG2dRndCoreInstance*     pTheInstance_ = NULL;
static const MtxFx32              mtxIdentity_ = { FX32_ONE,        0, 
                                                          0, FX32_ONE, 
                                                          0,        0 };
// If the function is called in the Begin - End Rendering block
#define NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT(  ) \
       NNS_G2D_ASSERTMSG( pTheInstance_ != NULL, "Please call this method between Begin - End Rendering" ) 

// If the function is called outside the Begin - End Rendering block
#define NNS_G2D_RND_OUTSIDE_BEGINEND_ASSERT(  ) \
       NNS_G2D_ASSERTMSG( pTheInstance_ == NULL, "Please call this method outside Begin - End Rendering" ) 



//------------------------------------------------------------------------------
// MtxChache affine Index accessor
// 
//    NNS_G2D_SURFACETYPE_MAIN3D = 0x00,  // Use 3D Graphics Engine
//    NNS_G2D_SURFACETYPE_MAIN2D = 0x01,  // Use 2D Graphics Engine A
//    NNS_G2D_SURFACETYPE_SUB2D  = 0x02,  // Use 2D Graphics Engine B
//    NNS_G2D_SURFACETYPE_MAX    = 0x03
//
// Caution: the process is associated closely with the Enum value.
//
NNS_G2D_INLINE u16 GetMtxCacheAffineIndex_
( 
    const NNSG2dRndCore2DMtxCache*     pMtxCache,
    NNSG2dSurfaceType                  surfaceType,
    OAM_FLIP                           flipType
)
{
    return pMtxCache->affineIndex[flipType][surfaceType - NNS_G2D_SURFACETYPE_MAIN2D];
}

//------------------------------------------------------------------------------
// 
//
NNS_G2D_INLINE void SetMtxCacheAffineIndex_
( 
    NNSG2dRndCore2DMtxCache*    pMtxCache,
    NNSG2dSurfaceType           surfaceType,
    OAM_FLIP                    flipType,
    u16                         affineIndex
)
{
    pMtxCache->affineIndex[flipType][surfaceType - NNS_G2D_SURFACETYPE_MAIN2D] = affineIndex;
}

//------------------------------------------------------------------------------
// Checks if it was registered previously before registering it. This prevents redundant registration.
// 
NNS_G2D_INLINE BOOL IsMtxCacheRegisteredAsAffineParams_
(
    const u16 affineIdx
)
{
    return ( affineIdx != MtxCache_NOT_AVAILABLE ) ? TRUE : FALSE;
}



//------------------------------------------------------------------------------
NNS_G2D_INLINE const MtxFx32* RndCoreGetCurrentMtx_()
{
    if( pTheInstance_->pCurrentMxt )
    { 
        //if( pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE )
        // TODO: Display a warning.
        // Matrix that was set
        return pTheInstance_->pCurrentMxt;
    }else{
        //
        // Unit matrix
        //    
        return &mtxIdentity_;
    }
}

//------------------------------------------------------------------------------
// Is affine transformation (2D) currently applied to the renderer?
NNS_G2D_INLINE BOOL IsRndCore2DAffineTransformed_
(
    const NNSG2dRndCoreInstance* pRnd 
)
{
    return (BOOL)( pRnd->flipFlag == NNS_G2D_RENDERERFLIP_NONE && 
                   pRnd->pCurrentMtxCacheFor2D != NULL );
}

//------------------------------------------------------------------------------
// request the shift value that converts the byte size into
// the number of characters from the mapping type
NNS_G2D_INLINE int GetShitToConvByteTo2DChar_( GXOBJVRamModeChar mappingType )
{
    int shift = ( REG_GX_DISPCNT_EXOBJ_MASK & mappingType ) >> REG_GX_DISPCNT_EXOBJ_SHIFT;
    
    return shift;
}

//------------------------------------------------------------------------------
// Determine whether the value to be shifted is above the shift width boundary
NNS_G2D_INLINE BOOL IsAlignedShiftValueBoundary_( u32 sizeByte, int shiftBit )
{
    const int mask = (0x1 << shiftBit) - 1;
    return (BOOL)( (mask & sizeByte) == 0);
}

//------------------------------------------------------------------------------
// Caclulate the number of characters from the
// number of bytes, the mapping type, etc.
NNS_G2D_INLINE u32 GetNum2DCharacter_( u32 sizeByte, GXOBJVRamModeChar mappingType )
{
   
    const int shiftBit  = 5 + GetShitToConvByteTo2DChar_( mappingType ); // 5 means  /= 32
    u32       numChar   = sizeByte >> shiftBit;
    
    NNS_G2D_ASSERT( IsAlignedShiftValueBoundary_( sizeByte, shiftBit ) ); 
    
    return numChar;
}

//------------------------------------------------------------------------------
// Get the texture setting of the 3D Graphics Engine
// 
NNS_G2D_INLINE u32 GetTexBaseAddr3D_( const NNSG2dImageProxy* pImgProxy )
{
    NNS_G2D_NULL_ASSERT( pImgProxy );
    if( NNS_G2dIsImageReadyToUse( pImgProxy, NNS_G2D_VRAM_TYPE_3DMAIN ) )
    {
        return NNS_G2dGetImageLocation( pImgProxy, NNS_G2D_VRAM_TYPE_3DMAIN );
    }else{
        // TODO: warning
        return 0;
    }
}

//------------------------------------------------------------------------------
// perform palette configuration for the 3D Graphics Engine
//
NNS_G2D_INLINE u32 GetPltBaseAddr3D_( const NNSG2dImagePaletteProxy* pPltProxy )
{
    NNS_G2D_NULL_ASSERT( pPltProxy );
    if( NNS_G2dIsImagePaletteReadyToUse( pPltProxy, NNS_G2D_VRAM_TYPE_3DMAIN ) )
    {
        return NNS_G2dGetImagePaletteLocation( pPltProxy, NNS_G2D_VRAM_TYPE_3DMAIN );
    }else{
        // TODO: warning
        return 0;
    }
}


//------------------------------------------------------------------------------
// Currently, this only does the cast
NNS_G2D_INLINE NNS_G2D_VRAM_TYPE SurfaceTypeToVramType_( NNSG2dSurfaceType   surfaceType )
{
    return (NNS_G2D_VRAM_TYPE)surfaceType;
}

//------------------------------------------------------------------------------
// get the character offset of the 2D Graphics Engine
// 
NNS_G2D_INLINE u32 GetCharacterBase2D_
( 
    const NNSG2dImageProxy* pImgProxy, 
    NNSG2dSurfaceType       type 
)
{
    
    NNS_G2D_NULL_ASSERT( pImgProxy );
    NNS_G2D_ASSERT( type == NNS_G2D_SURFACETYPE_MAIN2D || 
                    type == NNS_G2D_SURFACETYPE_SUB2D );

    
    {   
        const NNS_G2D_VRAM_TYPE       vramType = SurfaceTypeToVramType_( type );
   
        if( NNS_G2dIsImageReadyToUse( pImgProxy, vramType ) )
        {
            u32 baseAddr = NNS_G2dGetImageLocation( pImgProxy, vramType );
            
            return GetNum2DCharacter_( baseAddr, pImgProxy->attr.mappingType );     
            
        }else{
            // TODO: warning
            return 0;
        }
    }
}



//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL IsOamAttrDoubleAffineMode_( const GXOamAttr* pAttr )
{
    return (BOOL)(pAttr->rsMode == 0x3);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void HandleCellCallBackFunc_
( 
    NNSG2dRndCoreDrawCellCallBack   pFunc,
    const NNSG2dCellData*           pCell 
) 
{
    //
    // call the callback function
    //
    // also passing theInstance_, which determined that the matrix is not taken in the argument...
    if( pFunc )
    {
        (*pFunc)( pTheInstance_, pCell ); 
    } 
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void HandleCellOamBackFunc_
( 
    NNSG2dRndCoreDrawOamCallBack    pFunc,
    const NNSG2dCellData*           pCell,
    u16                             oamIdx 
) 
{
    //
    // call the callback function
    //
    // also passing theInstance_, which determined that the matrix is not taken in the argument...
    // 
    if( pFunc )
    {
        (*pFunc)( pTheInstance_, pCell, oamIdx ); 
    } 
}

//------------------------------------------------------------------------------
// Create the flip matrix
// The operation such as the one created immediately before loading has been changed.
NNS_G2D_INLINE void MakeFlipMtx_
( 
    const MtxFx22*    pMtxSrc, 
    MtxFx22*          pMtxDst, 
    OAM_FLIP          type 
)
{
    NNS_G2D_NULL_ASSERT( pMtxSrc );
    NNS_G2D_NULL_ASSERT( pMtxDst );
    
    {
        *pMtxDst = *pMtxSrc;
        
        if( type & OAM_FLIP_H )
        {
            pMtxDst->_00 = -pMtxDst->_00;
            pMtxDst->_01 = -pMtxDst->_01;
        }
        
        if( type & OAM_FLIP_V )
        {
            pMtxDst->_10 = -pMtxDst->_10;
            pMtxDst->_11 = -pMtxDst->_11;
        }
    }

}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 LoadMtxCacheAsAffineParams_
( 
    NNSG2dRndCore2DMtxCache*               pMtxCache,
    const NNSG2dRndCoreSurface*            pSurface, 
    OAM_FLIP                               type  
)
{
    u16    affineIdx  = GetMtxCacheAffineIndex_( pMtxCache, pSurface->type, type );
    
    //
    // If this is a matrix not previously registered in the lower module as an affine parameter...
    //
    if( !IsMtxCacheRegisteredAsAffineParams_( affineIdx ) )
    {
        NNS_G2D_NULL_ASSERT( pTheInstance_->pFuncOamAffineRegister );        
        
        //
        // perform the registration, and store that result in the cache
        //
        {
            if( type == OAM_FLIP_NONE )
            {
                affineIdx = (*pTheInstance_->pFuncOamAffineRegister)( &pMtxCache->m22 );
            }else{
                MtxFx22     mtxTemp;
                MakeFlipMtx_( &pMtxCache->m22, &mtxTemp, type );
                affineIdx = (*pTheInstance_->pFuncOamAffineRegister)( &mtxTemp );
            }
                
            SetMtxCacheAffineIndex_( pMtxCache, pSurface->type, type, affineIdx );
        }
    }
    
    return affineIdx;
}


//------------------------------------------------------------------------------
NNS_G2D_INLINE NNS_G2D_VRAM_TYPE 
ConvertSurfaceTypeToVramType_( NNSG2dSurfaceType surfaceType )
{
    return (NNS_G2D_VRAM_TYPE)(surfaceType);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 GetFlipedOBJPosX_( GXOamAttr* pOam, const GXOamShape shape )
{
    return NNS_G2dIsRndCoreFlipH( pTheInstance_ ) ? 
           (u16)( -NNS_G2dRepeatXinCellSpace(  (s16)pOam->x ) - NNS_G2dGetOamSizeX( &shape ) ):
           (u16)NNS_G2dRepeatXinCellSpace( (s16)pOam->x );
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 GetFlipedOBJPosY_( GXOamAttr* pOam, const GXOamShape shape )
{
    return NNS_G2dIsRndCoreFlipV( pTheInstance_ ) ? 
           (u16)( -NNS_G2dRepeatYinCellSpace( (s16)pOam->y ) - NNS_G2dGetOamSizeY( &shape ) ) :
           (u16)NNS_G2dRepeatYinCellSpace( (s16)pOam->y );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void OverwriteOamFlipFlag_( GXOamAttr* pOam )
{
    NNS_G2D_NULL_ASSERT( pOam );
    {
        //
        // Manipulate the flip information
        //        
        const BOOL bFlipH = 
           NNS_G2dIsRndCoreFlipH( pTheInstance_ ) ^ NNSi_G2dGetOamFlipH( pOam );
        const BOOL bFlipV = 
           NNS_G2dIsRndCoreFlipV( pTheInstance_ ) ^ NNSi_G2dGetOamFlipV( pOam );
           
        pOam->attr01 &= ~GX_OAM_ATTR01_HF_MASK;
        pOam->attr01 |= bFlipH << GX_OAM_ATTR01_HF_SHIFT;
           
        pOam->attr01 &= ~GX_OAM_ATTR01_VF_MASK;
        pOam->attr01 |= bFlipV << GX_OAM_ATTR01_VF_SHIFT;
    }
}


//------------------------------------------------------------------------------
static void DoAffineTransforme_
( 
    const MtxFx22*    pMtxSR, 
    GXOamAttr*        pOam, 
    NNSG2dFVec2*      pBaseTrans
)
{
    NNSG2dFVec2          objTrans;
    GXOamEffect          effectTypeAfter;
    
    // Get oam's position data in Obj Loacal coordinate, and convert it to world coordinate.
    NNS_G2dGetOamTransFx32( pOam, &objTrans );
    
    //
    // If the OBJ was set for double-size affine,
    // position correction will be added to the normal OBJ display position.
    // Therefore, remove correction value temporarily.
    //
    NNSi_G2dRemovePositionAdjustmentFromDoubleAffineOBJ( pOam, &objTrans );               
                        
    // Do affine transform
    MulMtx22( pMtxSR, &objTrans, &objTrans );
    
    //
    // If affine transform was used, specify affine mode (double size or normal)
    // (Note: In previous versions, this processing was done in the low-level module.)
    // 
    // We want to set only affine mode, so set the proper value in the affine parameter number.
    // The GX library does not contain the proper API.
    // We plan to remedy this.
    //
    if( pTheInstance_->affineOverwriteMode != NNS_G2D_RND_AFFINE_OVERWRITE_NONE )
    {
        if( pTheInstance_->affineOverwriteMode == NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE  ) 
        {
           effectTypeAfter =  GX_OAM_EFFECT_AFFINE_DOUBLE;
        }else{
           effectTypeAfter =  GX_OAM_EFFECT_AFFINE;
        }
                            
        G2_SetOBJEffect( pOam, effectTypeAfter, 0 );
    }
    
    //
    // Display position in double-size affine mode differs from that in normal affine transform mode
    // so for OBJs set for double-size affine mode transform,
    // correction is performed in this function.
    // 
    {
        // If double-size affine, use position correction for double-size affine OBJs
        const BOOL bShouldAdjust = G2_GetOBJEffect( pOam ) == GX_OAM_EFFECT_AFFINE_DOUBLE;
                                 
        NNSi_G2dAdjustDifferenceOfRotateOrientation( pOam, 
                                                     pMtxSR, 
                                                     &objTrans, 
                                                     bShouldAdjust );
    }
    
    // add base offset( left top position of Obj )
    objTrans.x += pBaseTrans->x;
    objTrans.y += pBaseTrans->y;
    G2_SetOBJPosition( pOam, objTrans.x >> FX32_SHIFT, objTrans.y >> FX32_SHIFT  );
}

//------------------------------------------------------------------------------                                                             
NNS_G2D_INLINE void DoFlipTransforme_( GXOamAttr* pOam, NNSG2dFVec2* pBaseTrans )
{    
    //
    // flip process
    //
    if( pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE )
    {    
        
        
        const GXOamShape shape = NNS_G2dGetOAMSize( pOam ); 
                                
        OverwriteOamFlipFlag_( pOam );
        
        if( NNS_G2dIsRndCoreFlipH( pTheInstance_ ) )
        {
            pOam->x = -pOam->x - NNS_G2dGetOamSizeX( &shape );
        }
        
        if( NNS_G2dIsRndCoreFlipV( pTheInstance_ ) )
        {
            pOam->y = (u8)(-pOam->y - NNS_G2dGetOamSizeY( &shape ));
        }
    }
    
    // add base offset( left top position of Obj )
    pOam->x += pBaseTrans->x >> FX32_SHIFT;
    pOam->y += pBaseTrans->y >> FX32_SHIFT;
}


//------------------------------------------------------------------------------
// Investigates if the preparation for starting the drawing has completed with the renderer.
// Used only in the assert statements when building debug
//
static BOOL IsRndCoreReadyForBeginRendering_( NNSG2dRndCoreInstance* pRnd )
{
    NNS_G2D_NULL_ASSERT( pRnd );
    {
       //
       // Is the image proxy set?
       //
       if( !(pRnd->pImgProxy && pRnd->pPltProxy) )
       {
           OS_Warning("RendererCore:ImageProxy isn't ready.");
           return FALSE;
       }
       
       //
       // Is the drawing target surface set?
       //
       if( pRnd->pCurrentTargetSurface == NULL )
       {
           OS_Warning("RendererCore:TragetSurface isn't ready.");
           return FALSE;
       }else{
           // Is surface active?
           if( !pRnd->pCurrentTargetSurface->bActive )
           {
               OS_Warning("RendererCore:The current tragetSurface isn't active. Is it OK?");
               // Warning only. Does not make it FALSE.
           }
       }
       
       //
       // For drawing 2D, is OAM registration function set properly?       
       //
       if( pRnd->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN2D ||
           pRnd->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_SUB2D  )
       {
           if
           ( 
              ( pRnd->pFuncOamAffineRegister == NULL && 
                  pRnd->pFuncOamRegister == NULL      )            
           )
           {
               OS_Warning( "RendererCore:OAM-RegisterFunction For 2D Graphics Engine rendering isn't ready." );
               return FALSE;
           }
       }
    }
    // Passed the exam
    return TRUE;
}

//------------------------------------------------------------------------------
// Render the cell using the 2D Graphics Engine
//
// This is a process exclusive to 2D Graphics Engine rendering.
//
static void DrawCellToSurface2D_
( 
    const NNSG2dRndCoreSurface*    pSurface, 
    const NNSG2dCellData*          pCell
)
{
    NNSG2dFVec2          baseTrans;
    MtxFx22              mtxSR;
    const MtxFx32*       pCurrMtx = RndCoreGetCurrentMtx_();
    
    NNS_G2D_NULL_ASSERT( pSurface );
    NNS_G2D_NULL_ASSERT( pCell );
    
    
    // get current Mtx for affine transformation.
    NNSi_G2dGetMtxTrans( pCurrMtx, &baseTrans );  
    //
    // conversion to view local type
    //                  
    baseTrans.x -= pSurface->viewRect.posTopLeft.x;
    baseTrans.y -= pSurface->viewRect.posTopLeft.y;
    
    {
        const u32   baseCharOffset  = pTheInstance_->base2DCharOffset;
        const BOOL  bAffined        = IsRndCore2DAffineTransformed_( pTheInstance_ );
        
        u16          i;
        GXOamAttr*   pTempOam = &pTheInstance_->currentOam;
        u16          oamAffinIdx = NNS_G2D_OAM_AFFINE_IDX_NONE;
                        
        for( i = 0; i < pCell->numOAMAttrs; i++ )
        {
            //
            // Oam attribute pre-render callback
            //
            pTheInstance_->bDrawEnable = TRUE;
            NNS_G2dCopyCellAsOamAttr( pCell, i, pTempOam );
            
            HandleCellOamBackFunc_( pSurface->pBeforeDrawOamBackFunc, pCell, i );
            
            //
            // render skip decision
            //
            if( !pTheInstance_->bDrawEnable )
            {
                continue;
            }
            
            
            
            //
            // Calculation of the character number offset
            //
            pTempOam->charNo += baseCharOffset;
                
            
            //
            // coordinate transformation: the flip process or the affine transformation process
            //
            if( bAffined )
            {       
                //
                // function that decides if the current matrix stack must be loaded, and
                // loads it, and returns the index
                // Because it creates proper affine transform matrix by checking OBJ flip flag,
                // it should be executed every time in each OBJ's loop.
                //
                oamAffinIdx = 
                LoadMtxCacheAsAffineParams_( pTheInstance_->pCurrentMtxCacheFor2D, 
                                             pSurface, 
                                             TO_OAM_FLIP( NNSi_G2dGetOamFlipH( pTempOam ), 
                                                          NNSi_G2dGetOamFlipV( pTempOam ) ));
                NNSi_G2dGetMtxRS( pCurrMtx, &mtxSR );
                DoAffineTransforme_( &mtxSR, pTempOam, &baseTrans );
            }else{
                oamAffinIdx = NNS_G2D_OAM_AFFINE_IDX_NONE;
                DoFlipTransforme_( pTempOam, &baseTrans );
            }

            //
            // Invocation of drawing registration function
            // Third argument is not used. Pass a dummy argument.
            //
            NNS_G2D_NULL_ASSERT( pTheInstance_->pFuncOamRegister );
            
            if( NNSi_G2D_RNDCORE_OAMENTORY_SUCCESSFUL 
                != (*pTheInstance_->pFuncOamRegister)( pTempOam, 
                                                      oamAffinIdx, 
                                                      NNSi_G2D_RNDCORE_DUMMY_FLAG ) )
            {
                // we have no capacity for new oam datas.
                return;
            }
             
            //
            // Callback after Oam attribute drawing
            //
            HandleCellOamBackFunc_( pSurface->pAfterDrawOamBackFunc, pCell, i );
        }
    }
}

//------------------------------------------------------------------------------
static void DrawOamToSurface3D_
( 
    GXOamAttr*                     pOam
)
{
    // Matrix setting
    G3_LoadMtx43( &pTheInstance_->mtxFor3DGE );
        
    if( pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE )
    {   
        const GXOamShape shape = NNS_G2dGetOAMSize( pOam );
    
        //
        // gets the render position of the object
        //
    	const s16 posX = (s16)GetFlipedOBJPosX_( pOam, shape );
        const s16 posY = (s16)GetFlipedOBJPosY_( pOam, shape );
        const s16 posZ = -1;      
        
        //
        // Manipulate the flip information
        //        
        OverwriteOamFlipFlag_( pOam );
        
        
        NNS_G2dDrawOneOam3DDirectWithPosFast( posX, posY, posZ, 
                                      pOam, 
                                      &pTheInstance_->pImgProxy->attr, 
                                       pTheInstance_->baseTexAddr3D, 
                                       pTheInstance_->basePltAddr3D );        
    }else{
        const s16 posX = (s16)NNS_G2dRepeatXinCellSpace( (s16)pOam->x );
        const s16 posY = (s16)NNS_G2dRepeatYinCellSpace( (s16)pOam->y );
        const s16 posZ = -1;
        
        NNS_G2dDrawOneOam3DDirectWithPosFast( posX, posY, posZ, 
                                      pOam, 
                                      &pTheInstance_->pImgProxy->attr, 
                                       pTheInstance_->baseTexAddr3D, 
                                       pTheInstance_->basePltAddr3D );    
    }
    
}
//------------------------------------------------------------------------------
// Renders the cell using the 3D Graphics Engine
// 
static void DrawCellToSurface3D_
( 
    const NNSG2dRndCoreSurface*    pSurface, 
    const NNSG2dCellData*          pCell
)
{
    u16 i = 0;
    
    GXOamAttr*            pTempOam = &pTheInstance_->currentOam;
    
    
    NNS_G2D_NULL_ASSERT( pSurface );
    NNS_G2D_NULL_ASSERT( pCell );
    
    
    // Draw All Objects
    for( i = 0; i < pCell->numOAMAttrs; i++ )
    {
        //
        // Oam attribute pre-render callback
        //
        pTheInstance_->bDrawEnable = TRUE;
        NNS_G2dCopyCellAsOamAttr( pCell, i, pTempOam );
        
        HandleCellOamBackFunc_( pSurface->pBeforeDrawOamBackFunc, pCell, i );
        if( pTheInstance_->bDrawEnable )            
        {            
            // Drawing
            DrawOamToSurface3D_( pTempOam );
        }
        //
        // Oam attribute pre-render callback
        //
        HandleCellOamBackFunc_( pSurface->pAfterDrawOamBackFunc, pCell, i );
    }
}



//------------------------------------------------------------------------------
NNS_G2D_INLINE void DrawCellImpl_
( 
    const NNSG2dCellData*   pCell, 
    u32                     cellVramTransferHandle 
)
{
    const NNSG2dRndCoreSurface*   pSurface = pTheInstance_->pCurrentTargetSurface;
    
    //
    // If not active, drawing process does not do anything
    //
    if( !pSurface->bActive )
    {
        return;
    }
    
    // the culling and matrix load are included in the callback
    pTheInstance_->bDrawEnable = TRUE;
    HandleCellCallBackFunc_( pSurface->pBeforeDrawCellBackFunc, pCell );
    if( pTheInstance_->bDrawEnable )
    {        
        //
        // If this is a VRAM transmission cell, record in the VRAM transmission state object that the cell rendering was performed
        //
        if( cellVramTransferHandle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE )
        {
            NNSi_G2dSetVramTransferCellDrawnFlag
            ( 
                cellVramTransferHandle, 
                ConvertSurfaceTypeToVramType_( pSurface->type ),
                TRUE 
            );
        }
        
        //
        // for each type of surface...
        //
        switch( pSurface->type )
        {
        case NNS_G2D_SURFACETYPE_MAIN3D:
            DrawCellToSurface3D_( pSurface, pCell ); 
            break;
        case NNS_G2D_SURFACETYPE_MAIN2D:
        case NNS_G2D_SURFACETYPE_SUB2D:
            DrawCellToSurface2D_( pSurface, pCell );
            break;
        case NNS_G2D_SURFACETYPE_MAX:
        default:
            NNS_G2D_ASSERT(FALSE);
        }
    }
    HandleCellCallBackFunc_( pSurface->pAfterDrawCellBackFunc, pCell );
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRndCore

  Description:  Initializes the renderer core module.
                Make sure to execute it before using NNSG2dRndCoreInstance
                
                
  Arguments:    pRnd          [OUT] Renderer core
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitRndCore( NNSG2dRndCoreInstance* pRnd )
{
    MI_CpuFill16( pRnd, 0x0, sizeof( NNSG2dRndCoreInstance ) );
    
    pRnd->pCurrentTargetSurface = NULL;
    
    pRnd->affineOverwriteMode  = NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE;
    
    pRnd->pImgProxy = NULL;
    pRnd->pPltProxy = NULL;
    
    pRnd->flipFlag = NNS_G2D_RENDERERFLIP_NONE;
    
    pRnd->bDrawEnable = TRUE;
    
    pRnd->pCurrentMtxCacheFor2D = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRndCoreSurface

  Description:  Initializes the renderer core drawing target surface.
                Make sure to execute it before using NNSG2dRndCoreSurface
                
                
  Arguments:    pSurface        [OUT]    Renderer core drawing target surface
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitRndCoreSurface( NNSG2dRndCoreSurface* pSurface )
{
    MI_CpuFill16( pSurface, 0x0, sizeof( NNSG2dRndCoreSurface ) );
    
    pSurface->bActive   = TRUE;
    pSurface->type      = NNS_G2D_SURFACETYPE_MAX;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreImageProxy

  Description:  Sets the image proxy which the renderer core references for drawing.
                Unlike the renderer module, changing the image proxy
                in the Begin - EndRendering block is invalid.
                This is because the renderer core module calculates
                the parameter at the timing of BeginRendering.
                
                
                This function needs to be called outside the Begin-EndRendering block.
                                
  Arguments:    pRnd          [OUT] Renderer core
                pImgProxy            [IN]    Image proxy
                pPltProxy             [IN]   Image palette proxy
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreImageProxy
( 
    NNSG2dRndCoreInstance*              pRnd,
    const NNSG2dImageProxy*             pImgProxy, 
    const NNSG2dImagePaletteProxy*      pPltProxy
)
{
    NNS_G2D_RND_OUTSIDE_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_NULL_ASSERT( pImgProxy );
    NNS_G2D_NULL_ASSERT( pPltProxy );
    
    pRnd->pImgProxy = pImgProxy;
    pRnd->pPltProxy = pPltProxy;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreOamRegisterFunc

  Description:  Sets the function pointer group which the renderer core module uses for registering the OAM that represents  the drawing OBJ to the external module.
                OAM registration function must be set
                
                if the set surface is
                the 2D Graphics Engine drawing surface.
                Aside from this function, NNS_G2dSetRndCoreOamRegisterFuncEx(),
                
                which sets the block transfer registration function as an OAM registration function, is provided.
                Caution:
                
                When this function is called, the block transfer registration function is reset.
                
                
                This function needs to be called outside the Begin-EndRendering block.
                                
  Arguments:    pRnd          [OUT] Renderer core
                pFuncOamRegister           [IN]   OAM registration function
                pFuncOamAffineRegister           [IN]   Affine parameter registration function
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreOamRegisterFunc
(
    NNSG2dRndCoreInstance*          pRnd, 
    NNSG2dOamRegisterFunction       pFuncOamRegister,
    NNSG2dAffineRegisterFunction    pFuncOamAffineRegister
)
{
    NNS_G2D_RND_OUTSIDE_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_NULL_ASSERT( pFuncOamRegister );
    NNS_G2D_NULL_ASSERT( pFuncOamAffineRegister );
    
    pRnd->pFuncOamRegister       = pFuncOamRegister;
    pRnd->pFuncOamAffineRegister = pFuncOamAffineRegister;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreAffineOverwriteMode

  Description:  Sets the overwriting operation in affine transformation mode.
                
                The following is the definition of the enumerator that represents the overwriting procedure.
                
                typedef enum NNSG2dRendererAffineTypeOverwiteMode
                {
                    NNS_G2D_RND_AFFINE_OVERWRITE_NONE,  // Does not overwrite
                    NNS_G2D_RND_AFFINE_OVERWRITE_NORMAL,// Sets to normal affine transformation method
                    NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE // Sets to double-size affine transformation method
                  
                }NNSG2dRendererAffineTypeOverwiteMode;
                
                
                This function can be called in or outside the Begin-EndRendering block.
                
                
  Arguments:    pCurrentMxt         [IN]  Affine transform matrix
                mode            [IN]    Enumerator that represents the overwriting operation in affine transformation mode.
                                         (NNSG2dRendererAffineTypeOverwiteMode)
                                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreAffineOverwriteMode
( 
    NNSG2dRndCoreInstance*                  pRnd, 
    NNSG2dRendererAffineTypeOverwiteMode    mode 
)
{
    NNS_G2D_NULL_ASSERT( pRnd );
    
    pRnd->affineOverwriteMode = mode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreCurrentMtx3D

  Description:  Sets the affine transformation matrix.
                Affine transformation matrix is
                 used for the current matrix set to Geometry Engine
                when drawing with 3D Graphics Engine.
                
                The transformation matrix set here is valid in the Begin-EndRendering block.
                Because the setting is reset when NNS_G2dRndCoreEndRendering() is called,
                this function needs to be called again to make the setting for a new drawing process.
                
                
                Internally, 4x3 matrix that is going to be set to the current matrix of Geometry Engine is calculated.
                This function is used only for drawing with 3D Graphics Engine.
                If this function was called when drawing with 2D Graphics Engine,
                a warning message will be output.
                
                This function needs to be called in the Begin-EndRendering block.
                
                
  Arguments:    pCurrentMxt         [IN]  current transform matrix
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreCurrentMtx3D
( 
    const MtxFx32*              pCurrentMxt
)
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pCurrentMxt );
    NNS_G2D_WARNING( pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D,
       "This method works only for 3D graphics engine rendering." );
       
    {
       NNSG2dRndCoreInstance*      pRnd = pTheInstance_;
         
       pRnd->pCurrentMxt            = pCurrentMxt;
        
       //
       // If drawing with 3D Graphics Engine, calculate 4x3 matrix to set to the current matrix of
       // Geometry Engine in advance.
       // When drawing with 2D Graphics Engine,
       // pRnd->pCurrentMxt is used only for affine transformation of the cell, and 3x2 matrix is enough.
       //
       {
           //
           // Generate 4x3 matrix loaded to Geometry Engine
           //
           {
              pRnd->mtxFor3DGE._00 = pCurrentMxt->_00;
              pRnd->mtxFor3DGE._01 = pCurrentMxt->_01;
              pRnd->mtxFor3DGE._02 = 0;

              pRnd->mtxFor3DGE._10 = pCurrentMxt->_10;
              pRnd->mtxFor3DGE._11 = pCurrentMxt->_11;
              pRnd->mtxFor3DGE._12 = 0;

              pRnd->mtxFor3DGE._20 = 0;
              pRnd->mtxFor3DGE._21 = 0;
              pRnd->mtxFor3DGE._22 = FX32_ONE;

              pRnd->mtxFor3DGE._30 = pCurrentMxt->_20;
              pRnd->mtxFor3DGE._31 = pCurrentMxt->_21;
              pRnd->mtxFor3DGE._32 = pRnd->zFor3DSoftwareSprite;
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreCurrentMtx2D

  Description:  Sets the affine transformation matrix and affine transformation parameters
                applied to the OBJ for drawing with 2D Graphics Engine.
                Affine transformation matrix is
                 used for coordinate conversion of the OBJ location performed in CPU
                when drawing with 3D Graphics Engine.
                
                Affine parameter is set by using matrix cache.
                If NULL is specified as a pointer to the matrix cache, it is assumed that the affine transformation
                will not be performed.
                
                Affine parameter set here is valid in the Begin-EndRendering block.
                Because the setting is reset when NNS_G2dRndCoreEndRendering() is called,
                this function needs to be called again to make the setting.
                
                
                This function is used only when drawing with 2D Graphics Engine.
                If this function was called when drawing with 3D Graphics Engine,
                a warning message will be output.
                
                This function needs to be called in the Begin-EndRendering block.
                
                
  Arguments:    pMtx                           [IN]  current transform matrix
                pCurrentMtxCacheFor2D        [IN]   Matrix cache that stores affine parameters
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
static BOOL CheckMtx2DParamsValid
(
    const MtxFx32*                 pMtx,
    NNSG2dRndCore2DMtxCache*       pCurrentMtxCacheFor2D
)
{
    if( !( pMtx->_00 == FX32_ONE && pMtx->_01 == 0 &&
           pMtx->_10 == 0        && pMtx->_11 == FX32_ONE ) )
    {
       if( pCurrentMtxCacheFor2D == NULL )
       {
           NNS_G2D_WARNING( FALSE,
              "Make sure that you have to specified the affine-mtx for the 2D graphics engine when you use affine transformation." );
           return FALSE;
       }
    }else{
       //
       // No warning is given for the time being.
       //
       /*
       if( pCurrentMtxCacheFor2D != NULL )
       {
           NNS_G2D_WARNING( FALSE,
              "The affine mtx setting is useless when you don't use affine transformation." );
           return FALSE;
       }
       */
    }
    return TRUE;
}
//------------------------------------------------------------------------------
void NNS_G2dSetRndCoreCurrentMtx2D
( 
    const MtxFx32*                 pMtx,
    NNSG2dRndCore2DMtxCache*       pCurrentMtxCacheFor2D
)
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pMtx );
    
    NNS_G2D_WARNING( pTheInstance_->pCurrentTargetSurface->type != NNS_G2D_SURFACETYPE_MAIN3D,
       "This method works only for 2D graphics engine rendering." );
    NNS_G2D_ASSERT( CheckMtx2DParamsValid( pMtx, pCurrentMtxCacheFor2D ) );
    
           
    {
       NNSG2dRndCoreInstance*      pRnd = pTheInstance_;
       
       pRnd->pCurrentMxt            = pMtx;
       pRnd->pCurrentMtxCacheFor2D  = pCurrentMtxCacheFor2D;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCore3DSoftSpriteZvalue

  Description:  Sets the Z value of sprite to be used when drawing software sprite
                
  Arguments:    pRnd          [OUT] Renderer core
                z         [IN]    Z value used for drawing software sprite
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCore3DSoftSpriteZvalue( NNSG2dRndCoreInstance* pRnd, fx32 z )
{
    pRnd->zFor3DSoftwareSprite = z;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreSurface

  Description:  Sets the drawing target surface to renderer core
  
                It needs to be called outside the Begin-EndRendering block.
                
  Arguments:    pRnd          [OUT] Renderer core
                pSurface     [IN] Drawing target surface
                
  Returns:      None.
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreSurface
( 
    NNSG2dRndCoreInstance*      pRnd, 
    NNSG2dRndCoreSurface*       pSurface 
)
{
    NNS_G2D_RND_OUTSIDE_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_NULL_ASSERT( pSurface );
    //
    // TODO:Checks if it is active, and displays a warning message
    //
    pRnd->pCurrentTargetSurface = pSurface;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsRndCoreFlipH

  Description:  Obtains the drawing flip status of renderer core
                
  Arguments:    pRnd          [IN] Renderer core
                
  Returns:      TRUE when horizontal direction flip status is ON
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsRndCoreFlipH( const NNSG2dRndCoreInstance* pRnd )
{
    NNS_G2D_NULL_ASSERT( pRnd );
    return (BOOL)( (pRnd->flipFlag & NNS_G2D_RENDERERFLIP_H) != 0 );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsRndCoreFlipV

  Description:  Obtains the drawing flip status of renderer core
                
  Arguments:    pRnd          [IN] Renderer core
                
  Returns:      TRUE when vertical direction flip status is ON
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsRndCoreFlipV( const NNSG2dRndCoreInstance* pRnd )
{
    NNS_G2D_NULL_ASSERT( pRnd );
    return (BOOL)( (pRnd->flipFlag & NNS_G2D_RENDERERFLIP_V) != 0 );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreFlipMode

  Description:  Sets the drawing flip status of renderer core.
                It is possible to be called in or outside the Begin-EndRendering block.
                Calling this function when using affine transformation feature is prohibited, and assert will fail.
                
  Arguments:    pRnd          [OUT] Renderer core
                bFlipH     [IN]  Horizontal direction flip
                bFlipV     [IN]  Vertical direction flip
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreFlipMode
( 
    NNSG2dRndCoreInstance* pRnd, 
    BOOL bFlipH, 
    BOOL bFlipV 
)
{
    //
    // Checks if it is currently SR transformed
    // 
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_WARNING( pRnd->pCurrentMtxCacheFor2D == NULL, 
       "You can't use the flip function using affine transformation." );

    if( bFlipH )
    {
        pRnd->flipFlag |= NNS_G2D_RENDERERFLIP_H;
    }else{
        pRnd->flipFlag &= ~NNS_G2D_RENDERERFLIP_H;
    }
    
    if( bFlipV )
    {
        pRnd->flipFlag |= NNS_G2D_RENDERERFLIP_V;
    }else{
        pRnd->flipFlag &= ~NNS_G2D_RENDERERFLIP_V;
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreBeginRendering

  Description:  Starts drawing with renderer core
                Performs various preprocesses to start drawing.
                
                Note that image proxy and parameters of target render surface
                cannot be changed after calling this function.
                After calling this function, always call NNS_G2dRndCoreEndRendering() after the drawing is finished.
                
                Calling Begin-EndRendering function in the Begin-EndRendering block (nested call)  is prohibited.
                If called, assert will fail.
                
  Arguments:    pRnd          [IN] Renderer core
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreBeginRendering( NNSG2dRndCoreInstance* pRnd )
{
    NNS_G2D_NULL_ASSERT( pRnd );
    NNS_G2D_ASSERT( pTheInstance_ == NULL );
    NNS_G2D_ASSERTMSG( IsRndCoreReadyForBeginRendering_( pRnd ), 
       "NNSG2dRndCoreInstance isn't ready to begin rendering." );
    
    pTheInstance_ = pRnd;
    
    //
    // Preprocesses specific for 3D drawing
    //
    if( pRnd->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D )
    {
        // camera settings
        G3_MtxMode( GX_MTXMODE_PROJECTION );
        G3_PushMtx();
        G3_Translate( -pRnd->pCurrentTargetSurface->viewRect.posTopLeft.x, 
                      -pRnd->pCurrentTargetSurface->viewRect.posTopLeft.y, 0 );
        G3_MtxMode( GX_MTXMODE_POSITION );
        
        //
        // Calculation of base address
        //
        pRnd->baseTexAddr3D = GetTexBaseAddr3D_( pTheInstance_->pImgProxy );
        pRnd->basePltAddr3D = GetPltBaseAddr3D_( pTheInstance_->pPltProxy );
        
    }else{
    //
    // Preprocesses specific for 2D drawing
    //
        pTheInstance_->base2DCharOffset 
           = GetCharacterBase2D_( pTheInstance_->pImgProxy, 
                                  pTheInstance_->pCurrentTargetSurface->type );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreEndRendering

  Description:  Ends drawing with renderer core.
                Performs various postprocesses to end drawing.
                
                If the OAM block transfer function is set as OAM registration function,
                the contents of the internal OAM buffer is copied to the external module
                within this function by using block transfer function.
                Transfer is expected to always succeed, but if the transfer failed,
                assert will fail.
                (When the transfer fails, it is expected that there is an error
                with buffer size setting of internal OAM buffer.)
                
  Arguments:    None
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreEndRendering( void )
{
    NNS_G2D_NULL_ASSERT( pTheInstance_ );
     
    //
    // Postprocesses specific for 3D drawing
    //
    if( pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D )
    {
        // camera settings (restore)
        G3_MtxMode( GX_MTXMODE_PROJECTION );
        G3_PopMtx(1);
        G3_MtxMode( GX_MTXMODE_POSITION );
        
    }else{
    //
    // Postprocesses specific for 2D drawing
    //    
        pTheInstance_->base2DCharOffset = 0;
        //
        // Reset affine transformation setting
        //
        pTheInstance_->pCurrentMxt = NULL;
        pTheInstance_->pCurrentMtxCacheFor2D = NULL;
    }    
    pTheInstance_ = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreDrawCell

  Description:  Render cell
                It needs to be called in the Begin-EndRendering block.
                
  Arguments:    pCell:                    [IN]   Cell data
  
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreDrawCell( const NNSG2dCellData* pCell )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pCell );
    
    DrawCellImpl_( pCell, NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreDrawCellVramTransfer

  Description:  Draws the VRAM transfer cell.
                It needs to be called in the Begin-EndRendering block.
                
  Arguments:    pCell:                    [IN]   Cell data
                cellVramTransferHandle:   [IN] Handle of the cell VRAM transfer state object. 
  
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreDrawCellVramTransfer( const NNSG2dCellData* pCell, u32 cellVramTransferHandle )
{
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_NULL_ASSERT( pCell );
    
    NNS_G2D_ASSERT( cellVramTransferHandle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE );
    DrawCellImpl_( pCell, cellVramTransferHandle );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreDrawCellFast2D

  Description:  Draws the cell in high speed by using 2D Graphics Engine (OBJ feature).
                
                This function does not support calling of callback functions or affine transformation of cells.
                If this function was called while the affine transformation matrix for 2D (pCurrentMtxCacheFor2D) is set,
                assert will fail.
                
                It needs to be called in the Begin-EndRendering block.
                
  Arguments:    pCell:      [IN]   Cell data
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreDrawCellFast2D( const NNSG2dCellData* pCell )
{
    const NNSG2dRndCoreSurface*   pSurface = pTheInstance_->pCurrentTargetSurface;    
    NNSG2dFVec2          baseTrans;
    const MtxFx32*       pCurrMtx = RndCoreGetCurrentMtx_();
    
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    
    
    
    NNS_G2D_ASSERTMSG( pSurface->type == NNS_G2D_SURFACETYPE_MAIN2D || 
                      pSurface->type == NNS_G2D_SURFACETYPE_SUB2D,
                      "This method can work only for the 2D Graphics Engine." );
       
    NNS_G2D_ASSERTMSG( pTheInstance_->pCurrentMtxCacheFor2D == NULL,
                      "You can't use this method using affine transfomation." );
    NNS_G2D_NULL_ASSERT( pCell );   
    //------------------------------------------------------------------------------
    // Draw the cell using the 2D Graphics Engine
    // No support for callback invocation etc., but this version operates quickly.
    // 

    // get current Mtx for affine transformation.
    NNSi_G2dGetMtxTrans( pCurrMtx, &baseTrans );  
       
    //
    // conversion to view local type
    //                  
    baseTrans.x -= pSurface->viewRect.posTopLeft.x;
    baseTrans.y -= pSurface->viewRect.posTopLeft.y;
    //
    // Does virtually the same processing as NNS_G2dMakeCellToOams().
    // (However, it supports flip.)
    //
    //
    // Version that does not use block transfer
    // Each time an OBJ is registered, overhead is generated for the invocation of the draw registration function.
    // Peformance is inferior to version that uses block transfer but
    // the ratio of each frame used by this function for draw registration is small.
    // (Even when registering 128 X 2 OBJs). We don't think there is  problem with this method and have used it here.
    // 
    {
       u16          i;
       GXOamAttr*   pTempOam;
       const u32    baseCharOffset = pTheInstance_->base2DCharOffset;

       for( i = 0; i < pCell->numOAMAttrs; i++ )
       {
           pTempOam = &pTheInstance_->currentOam;
                        
           NNS_G2dCopyCellAsOamAttr( pCell, i, pTempOam );
           
           DoFlipTransforme_( pTempOam, &baseTrans );
                        
           pTempOam->charNo += baseCharOffset;
                        
           //
           // Invocation of drawing registration function
           //
           NNS_G2D_NULL_ASSERT( pTheInstance_->pFuncOamRegister );
           if( FALSE == (*pTheInstance_->pFuncOamRegister)( pTempOam, NNS_G2D_OAM_AFFINE_IDX_NONE, FALSE ) )
           {
              // we have no capacity for new oam datas.
              return;
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCoreCellCloneSource3D

  Description:  Calculates UV parameter that is referenced by NNS_G2dRndCoreDrawCellClone3D() in advance,
                and saves as UV parameter cache.
                Assuming that the input cell consists of OBJs that have the same image data,
                calculates UV value for OBJ with index 0, and saves as cache.
                
                Caution: The UV parameter of the flipped sprite is different from normal UV parameter.
                Therefore, this is not treated as an OBJ that has the same image data.
                This process is exclusively for 3D surfaces. If the current surface type is other than
                NNS_G2D_SURFACETYPE_MAIN3D, assert will fail.
                
                It needs to be called in the Begin-EndRendering block.
                
  Arguments:    pCell:      [IN]   Cell data
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetRndCoreCellCloneSource3D( const NNSG2dCellData* pCell )
{
    
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_ASSERTMSG( pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D,
                          "This method can work only for the 3D Graphics Engine." );
    NNS_G2D_NULL_ASSERT( pCell );         
    
    {   
        GXOamAttr*            pOam = &pTheInstance_->currentOam;
        // Typify the beginning OBJ...
        NNS_G2dCopyCellAsOamAttr( pCell, 0, pOam );
        // to set as parameter cache.
        // With the drawing after this,
        // this value is referenced for texture parameter.
        
        
        //
        // Reflect the flip setting of renderer to OAM attribute
        //        
        OverwriteOamFlipFlag_( pOam );
               
               
        NNS_G2dSetOamSoftEmuSpriteParamCache
        (
           pOam, 
           &pTheInstance_->pImgProxy->attr, 
           pTheInstance_->baseTexAddr3D, 
           pTheInstance_->basePltAddr3D 
        );    
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRndCoreDrawCellClone3D

  Description:  Draws the cell that consists of OBJs that have the same image data
                in high speed by using software sprite
                
                OBJs that have the same image data will have the same UV value
                when drawn as software sprites (square polygons with texture).
                
                As long as the OBJs have the same image data, there can be more than one OBJ in the cell.
                This function performs drawing by using the UV parameter cache that was calculated in advance.
                Because the calculation of UV value needed for normal software sprite drawing can be omitted,
                it works in high speed.
                To set UV parameter cache,
                calling of NNS_G2dSetRndCoreCellCloneSource3D() and initialization of parameters
                must be done before executing it.
                
                Caution: The UV parameter of a flipped sprite is different from a normal UV parameter.
                Flipped sprites are therefore not treated as having the same image data.
                
                
                This function supports affine transformation. Does not support calling of various callbacks.
                
                This process is exclusively for 3D surface. If the current surface type is other than
                NNS_G2D_SURFACETYPE_MAIN3D, assert will fail.
                
                It needs to be called in the Begin-EndRendering block.
                
  Arguments:    pCell:      [IN]   Cell data
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dRndCoreDrawCellClone3D( const NNSG2dCellData* pCell )
{
    u16 i = 0;
    
    GXOamAttr*            pOam = &pTheInstance_->currentOam;
    
    NNS_G2D_RND_BETWEEN_BEGINEND_ASSERT();
    NNS_G2D_ASSERTMSG( pTheInstance_->pCurrentTargetSurface->type == NNS_G2D_SURFACETYPE_MAIN3D,
                          "This method can work only for the 3D Graphics Engine." );
    NNS_G2D_NULL_ASSERT( pTheInstance_->pCurrentTargetSurface );
    NNS_G2D_NULL_ASSERT( pCell );
    
    if( pTheInstance_->flipFlag != NNS_G2D_RENDERERFLIP_NONE )
    {
        //
        // Flip drawing
        //
        for( i = 0; i < pCell->numOAMAttrs; i++ )
        {
           NNS_G2dCopyCellAsOamAttr( pCell, i, pOam );
           
           // Matrix setting
           G3_LoadMtx43( &pTheInstance_->mtxFor3DGE );
    
           // Drawing
           {               
               const GXOamShape shape = NNS_G2dGetOAMSize( pOam );
                
               //
               // gets the render position of the object
               //
               const s16 posX = (s16)GetFlipedOBJPosX_( pOam, shape );
               const s16 posY = (s16)GetFlipedOBJPosY_( pOam, shape );
               const s16 posZ = -1;      
                
               //
               // Reflect the flip setting of renderer to OAM attribute
               //        
               OverwriteOamFlipFlag_( pOam );
               
                
               NNS_G2dDrawOneOam3DDirectUsingParamCacheFast( posX, posY, posZ, pOam );    
           }
        }
    }else{
        //
        // Drawing without flip
        //
        for( i = 0; i < pCell->numOAMAttrs; i++ )
        {
           NNS_G2dCopyCellAsOamAttr( pCell, i, pOam );
           
           // Matrix setting
           G3_LoadMtx43( &pTheInstance_->mtxFor3DGE );
    
           {
               const s16 posX = (s16)NNS_G2dRepeatXinCellSpace( (s16)pOam->x );
               const s16 posY = (s16)NNS_G2dRepeatYinCellSpace( (s16)pOam->y );
               const s16 posZ = -1;
                               
               NNS_G2dDrawOneOam3DDirectUsingParamCacheFast( posX, posY, posZ, pOam );
           }
        }
    }
}

