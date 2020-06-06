/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_Renderer.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Renderer.h,v $
  Revision 1.33  06/02/2005 08:54:55 AM  kitani_toshikazu
  Cleaned up comments

  Revision 1.32  05/31/2005 12:06:07 AM  kitani_toshikazu
  Changed copyright year

  Revision 1.31  05/30/2005 07:23:02 AM  kitani_toshikazu
  Added NNS_G2dIsRendererOverwriteEnable() as experiment

  Revision 1.30  12/14/2004 12:15:35 AM  kitani_toshikazu
  (none)

  Revision 1.29  12/13/2004 08:24:46 AM  kitani_toshikazu
  Merged branch g2d_multiCell

  Revision 1.26  11/02/2004 05:19:36 AM  kitani_toshikazu
  Added NNS_G2dSetRendererOverwrite function.

  Revision 1.25  10/29/2004 04:07:48 AM  kitani_toshikazu
  Added pCurrentSurface member to NNSG2dRendererInstance.

  Revision 1.24  10/28/2004 10:46:16 AM  kitani_toshikazu
  Rendering Optimization Flag
  Added NNS_G2D_RDR_OPZHINT_LOCK_PARAMS processing.
  Added parameter rewrite item.

  Revision 1.23  10/28/2004 12:06:31 AM  kitani_toshikazu
  Optimized performance

  Revision 1.22  10/21/2004 11:02:03 AM  kitani_toshikazu
  Revised to optimize performance

  Revision 1.21  10/19/2004 11:59:23 AM  kitani_toshikazu
  Interface adjustment with renderer core.

  Revision 1.20  10/18/2004 09:47:44 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.19  10/08/2004 04:40:56 AM  kitani_toshikazu
  (none)

  Revision 1.18  10/07/2004 12:59:48 PM  kitani_toshikazu
  Separated basic part of renderer to a separate module.

  Revision 1.17  09/17/2004 01:49:38 AM  kitani_toshikazu
  Implemented affine conversion mode rewrite feature.

  Revision 1.16  09/15/2004 07:40:56 AM  kitani_toshikazu
  Changed accessor to Internal.

  Revision 1.15  09/15/2004 06:49:23 AM  kitani_toshikazu
  Added accessor.

  Revision 1.14  09/15/2004 05:52:45 AM  kitani_toshikazu
  (none)

  Revision 1.13  08/26/2004 07:20:08 AM  kitani_toshikazu
  Implementation of the Z offset features

  Revision 1.12  08/23/2004 08:50:44 AM  kitani_toshikazu
  Addition of callback, flip rendering, and other render optimization features.

  Revision 1.11  08/17/2004 06:02:39 AM  kitani_toshikazu
  Implementation of the automatic Z value offset features

  Revision 1.10  07/29/2004 02:44:49 AM  hatamoto_minoru
  Fixed separate names of functions.

  Revision 1.9  07/29/2004 12:34:20 AM  kitani_toshikazu
  Changed function names.

  Revision 1.8  07/27/2004 04:28:34 AM  kitani_toshikazu
  Added comment

  Revision 1.7  07/24/2004 07:42:20 AM  kitani_toshikazu
  Changing of function names.

  Revision 1.6  06/22/2004 06:29:21 AM  kitani_toshikazu
  Provided support for visible culling in cell units.

  Revision 1.5  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.4  05/10/2004 04:42:28 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.3  04/23/2004 12:26:50 AM  kitani_toshikazu
  Added workaround for color palette swap table.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:54 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
//
// The abstraction layer of rendering HWs.
// Rendering HWs include 2D Graphic Engine A - B, and 3D Graphic Engine.
// 
#ifndef NNS_G2D_RENDERER_H_
#define NNS_G2D_RENDERER_H_


#include <nitro.h>

#include <nnsys/g2d/g2d_Softsprite.h>
#include <nnsys/g2d/g2d_Entity.h>
#include <nnsys/g2d/g2d_Node.h>
#include <nnsys/g2d/g2d_MultiCellAnimation.h>
#include <nnsys/g2d/g2d_PaletteTable.h>
#include <nnsys/g2d/fmt/g2d_Character_data.h>
#include <nnsys/g2d/g2d_RendererCore.h>


#ifdef __cplusplus
extern "C" {
#endif

//
// aliases of functions with names changed
// Previous functions declared as aliases to preserve compatibility.
// 
#define NNS_G2dAddTargetSurface             NNS_G2dAddRendererTargetSurface
#define NNS_G2dSetCurrentImageProxy         NNS_G2dSetRendererImageProxy






/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndCellCullingFunction
  
  Description:   This is the pointer that does visible culling.
  
  Arguments:    pCell: Cell
                pMtx    : conversion matrix
                pTrans  : parallel movement value
               
  Returns:      If visible, TRUE
  
 *---------------------------------------------------------------------------*/
 
typedef BOOL(*NNSG2dRndCellCullingFunction)
(
    const NNSG2dCellData*   pCell,
    const MtxFx32*          pMtx,
    const NNSG2dViewRect*   pViewRect
);

struct NNSG2dRendererInstance;
struct NNSG2dRenderSurface;
/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndDrawCellCallBack
  
  Description:   This is the callback called during rendering (called before
                and after rendering).
  
  Arguments:    pRend   : pointer to renderer
                pSurface: pointer to render surface
                pCell: Cell
                pMtx    : conversion matrix
               
  Returns:      None.
  
 *---------------------------------------------------------------------------*/
 
typedef void(*NNSG2dRndDrawCellCallBack)
(
    struct NNSG2dRendererInstance*         pRend,
    struct NNSG2dRenderSurface*            pSurface,
    const NNSG2dCellData*                  pCell,
    const MtxFx32*                         pMtx
);


/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndDrawOamCallBack
  
  Description:   This is the callback called during rendering of internal
                and after rendering).
  
  Arguments:    pRend   : pointer to renderer
                pSurface: pointer to render surface
                pCell: Cell
                pMtx    : conversion matrix
               
  Returns:      None.
  
 *---------------------------------------------------------------------------*/
 
typedef void(*NNSG2dRndDrawOamCallBack)
(
    struct NNSG2dRendererInstance*         pRend,
    struct NNSG2dRenderSurface*            pSurface,
    const NNSG2dCellData*                  pCell,
    u16                                    oamIdx,
    const MtxFx32*                         pMtx
);



/*---------------------------------------------------------------------------*
  Name:         NNSG2dRenderSurface

  Description:  This structure expresses the target of the rendering.
                
                It holds the function pointer that registers the OamAffineParamater function that registers Oams.
                It is responsible for the actual  issuing of the render instruction.
                (The "issue of render instruction" may be handled as a separate concept.)
               
                It holds the pointer to other entities and makes the list structure.
                
 *---------------------------------------------------------------------------*/
    
typedef struct NNSG2dRenderSurface
{
    //
    // NNSG2dRndCoreSurface
    // Maintains the basic characteristics of the rendering target surface.
    //
    union
    {
        NNSG2dRndCoreSurface              coreSurface;
        struct
        {
            NNSG2dViewRect                    viewRect;                 
            BOOL                              bActive;                  
            NNSG2dSurfaceType                 type;                     
            
            NNSG2dRndCoreDrawCellCallBack     pBeforeCoreDrawCellBack;  
            NNSG2dRndCoreDrawCellCallBack     pAfterCoreDrawCellBack;
    
            NNSG2dRndCoreDrawOamCallBack      pBeforeCoreDrawOamBack;
            NNSG2dRndCoreDrawOamCallBack      pAfterCoreDrawOamBack;
        };
    };
    
    NNSG2dOamRegisterFunction         pFuncOamRegister;         // Oam registration function
    NNSG2dAffineRegisterFunction      pFuncOamAffineRegister;   // Affine registration function
    
    void*                             pNextSurface;             // next surface
    
    
    NNSG2dRndCellCullingFunction      pFuncVisibilityCulling;   // visibility culling function
    
    NNSG2dRndDrawCellCallBack         pBeforeDrawCellBackFunc;
    NNSG2dRndDrawCellCallBack         pAfterDrawCellBackFunc;
    
    NNSG2dRndDrawOamCallBack          pBeforeDrawOamBackFunc;
    NNSG2dRndDrawOamCallBack          pAfterDrawOamBackFunc;
    
              
}
NNSG2dRenderSurface;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererOptimizeHint

  Description:  This is information about the render optimization of the renderer.
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dRendererOptimizeHint
{
    NNS_G2D_RDR_OPZHINT_NONE              = 0x0,
    NNS_G2D_RDR_OPZHINT_NOT_SR            = 0x1, // Does not use SR(Scale Rotate) conversion.
    NNS_G2D_RDR_OPZHINT_LOCK_PARAMS       = 0x2  // Locks parameters.
                                                 // 
                                                 // Can only be specified when there is only one surface.
                                                 // Image proxy and surface types 
                                                 // can be changed inside the renderer's Begin-End Rendering blocks. 
                                                 // As a result, Begin-End Rendering can be exeucted
                                                 // at the same time as the renderer's Begin-End Rendering. 
                                                 //
                                                 // Begin-End Rendering of the renderer core is done with each
                                                 // cell rendering.

}NNSG2dRendererOptimizeHint;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererOverwriteParam

  Description:  This is the OBJ attribute overwrite item.

 *---------------------------------------------------------------------------*/
typedef enum NNSG2dRendererOverwriteParam
{
    NNS_G2D_RND_OVERWRITE_NONE          = 0x0, // Do not carry out OBJ attribute overwrite.
    NNS_G2D_RND_OVERWRITE_PRIORITY      = 0x1, // rendering priority
    NNS_G2D_RND_OVERWRITE_PLTTNO        = 0x2, // palette number
    NNS_G2D_RND_OVERWRITE_MOSAIC        = 0x4, // mosaic ON OFF
    NNS_G2D_RND_OVERWRITE_OBJMODE       = 0x8, // object mode
    NNS_G2D_RND_OVERWRITE_PLTTNO_OFFS   = 0x10, // Palette number (add offset)
    NNS_G2D_RND_OVERWRITE_MAX
    
}NNSG2dRendererOverwriteParam;
// 
// other candidates
// OBJMode
// mosaic ONOFF

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererInstance

  Description:  Renderer Entity
                Maintains the NNSG2dRenderSurface list.

 *---------------------------------------------------------------------------*/
typedef struct NNSG2dRendererInstance
{
    NNSG2dRndCoreInstance             rendererCore;        // renderer core
        
    
    NNSG2dRenderSurface*              pTargetSurfaceList;  // surface list of rendering target
    NNSG2dRenderSurface*              pCurrentSurface;     // surface currently having render processing done
                                                           // (Set to NULL unless renderer core is having render processing done)
    
    const NNSG2dPaletteSwapTable*     pPaletteSwapTbl;     // color palette change table (NULL possible)
    u32                               opzHint;             // information for rendering optimization
                                                           // NNSG2dRendererOptimizeHint
                                                           
    fx32                              spriteZoffsetStep;   // amount of automatic Z-offset for software sprite
    
    u32                               overwriteEnableFlag; // NNSG2dRendererOverwriteParam
    u16                               overwritePriority;   // rendering priority
    u16                               overwritePlttNo;     // palette number
    GXOamMode                         overwriteObjMode;                      
    BOOL                              overwriteMosaicFlag;
    
    u16                               overwritePlttNoOffset;// Palette number (with offset added)
    u16                               pad16_;
}
NNSG2dRendererInstance;






//------------------------------------------------------------------------------
// initialization related
void NNS_G2dInitRenderer( NNSG2dRendererInstance* pRend );
void NNS_G2dAddRendererTargetSurface
( 
    NNSG2dRendererInstance* pRend, 
    NNSG2dRenderSurface* pNew 
);
void NNS_G2dInitRenderSurface( NNSG2dRenderSurface* pSurface );
void NNS_G2dSetRendererFlipMode
( 
    NNSG2dRendererInstance* pRend, 
    BOOL bFlipH, 
    BOOL bFlipV 
);

//------------------------------------------------------------------------------
// Rendering Related Functions


void NNS_G2dBeginRendering   ( NNSG2dRendererInstance* pRendererInstance );
void NNS_G2dBeginRenderingEx ( NNSG2dRendererInstance* pRendererInstance, u32 opzHint );

void NNS_G2dDrawCell           ( const NNSG2dCellData* pCell );
void NNS_G2dDrawCellAnimation  ( const NNSG2dCellAnimation* pCellAnim );
void NNS_G2dDrawMultiCell      ( const NNSG2dMultiCellInstance* pMC );
void NNS_G2dDrawMultiCellAnimation( const NNSG2dMultiCellAnimation* pMCAnim );

void NNS_G2dDrawEntity         ( NNSG2dEntity* pEntity );

void NNS_G2dEndRendering   ( );

//------------------------------------------------------------------------------
// Matrix Operations
void NNS_G2dPushMtx();
void NNS_G2dPopMtx();
void NNS_G2dTranslate(fx32 x, fx32 y, fx32 z );
void NNS_G2dSetTrans(fx32 x, fx32 y, fx32 z );
void NNS_G2dScale(fx32 x, fx32 y, fx32 z );
void NNS_G2dRotZ( fx32 sin, fx32 cos );





//------------------------------------------------------------------------------
// Palette Change Related Functions
void NNS_G2dSetRendererPaletteTbl
( 
    NNSG2dRendererInstance*         pRend, 
    const NNSG2dPaletteSwapTable*   pTbl 
);
const NNSG2dPaletteSwapTable* NNS_G2dGetRendererPaletteTbl
( 
    NNSG2dRendererInstance* pRend 
);
void NNS_G2dResetRendererPaletteTbl
( 
    NNSG2dRendererInstance* pRend 
);

//------------------------------------------------------------------------------
// Image Information Related Functions
void NNS_G2dSetRendererImageProxy
( 
    NNSG2dRendererInstance*             pRend, 
    const NNSG2dImageProxy*             pImgProxy, 
    const NNSG2dImagePaletteProxy*      pPltProxy
);





//------------------------------------------------------------------------------
// inline
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNS_G2dSetRendererSpriteZoffset
( 
    NNSG2dRendererInstance* pRend, 
    fx32                    spriteZoffset 
)
{
    pRend->spriteZoffsetStep = spriteZoffset;
}



//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNS_G2dSetRendererAffineOverwriteMode
( 
    NNSG2dRendererInstance*                 pRend, 
    NNSG2dRendererAffineTypeOverwiteMode    affineMode
)
{
    NNS_G2D_NULL_ASSERT( pRend );
    pRend->rendererCore.affineOverwriteMode = affineMode;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dRendererAffineTypeOverwiteMode 
NNS_G2dGetRendererAffineOverwriteMode
( 
    const NNSG2dRendererInstance* pRend
)
{
    NNS_G2D_NULL_ASSERT( pRend );
    return pRend->rendererCore.affineOverwriteMode;
}

//------------------------------------------------------------------------------
// Move to Core
NNS_G2D_INLINE void NNS_G2dSetRendererSurfaceActiveFlag
( 
    NNSG2dRenderSurface*    pSurface, 
    BOOL                    bActive
)
{
    NNS_G2D_NULL_ASSERT( pSurface );
    pSurface->coreSurface.bActive = bActive;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNS_G2dGetRendererSurfaceActiveFlag
( 
    const NNSG2dRenderSurface*    pSurface
)
{
    NNS_G2D_NULL_ASSERT( pSurface );
    return pSurface->coreSurface.bActive;
}

//------------------------------------------------------------------------------
// Accessor Rendering Priority
NNS_G2D_INLINE void NNS_G2dSetRendererOverwritePriority
(
    NNSG2dRendererInstance*     pRend,
    u16                         Priority
)
{
    pRend->overwritePriority = Priority;
}

NNS_G2D_INLINE u16 NNS_G2dGetRendererOverwritePriority
(
    const NNSG2dRendererInstance* pRend
)
{
    return pRend->overwritePriority;
}
//------------------------------------------------------------------------------
// Accessor
// palette number
NNS_G2D_INLINE void NNS_G2dSetRendererOverwritePlttNo
(
    NNSG2dRendererInstance*     pRend,
    u16                         plttNo
)
{
    pRend->overwritePlttNo = plttNo;
}

NNS_G2D_INLINE u16 NNS_G2dGetRendererOverwritePlttNo
(
    const NNSG2dRendererInstance* pRend    
)
{
    return pRend->overwritePlttNo;
}

//------------------------------------------------------------------------------
// Under development
NNS_G2D_INLINE void NNS_G2dSetRendererOverwritePlttNoOffset
(
    NNSG2dRendererInstance*     pRend,
    u16                         plttNoOffset
)
{
    pRend->overwritePlttNoOffset = plttNoOffset;
}

//------------------------------------------------------------------------------
// Under development
NNS_G2D_INLINE u16 NNS_G2dGetRendererOverwritePlttNoOffset
(
    const NNSG2dRendererInstance* pRend    
)
{
    return pRend->overwritePlttNoOffset;
}


NNS_G2D_INLINE void NNS_G2dSetRendererOverwriteMosaicFlag
(
    NNSG2dRendererInstance*     pRend,
    BOOL                        overwriteMosaicFlag
)
{
    pRend->overwriteMosaicFlag = overwriteMosaicFlag;
}

NNS_G2D_INLINE BOOL NNS_G2dGetRendererOverwriteMosaicFlag
(
    const NNSG2dRendererInstance* pRend    
)
{
    return pRend->overwriteMosaicFlag;
}

NNS_G2D_INLINE void NNS_G2dSetRendererOverwriteOBJMode
(
    NNSG2dRendererInstance*     pRend,
    GXOamMode                   overwriteObjMode
)
{
    pRend->overwriteObjMode = overwriteObjMode;
}

NNS_G2D_INLINE GXOamMode NNS_G2dGetRendererOverwriteOBJMode
(
    const NNSG2dRendererInstance* pRend    
)
{
    return pRend->overwriteObjMode;
}



//------------------------------------------------------------------------------
// OAM Overwrite Flag Operations
NNS_G2D_INLINE void 
NNS_G2dSetRendererOverwriteEnable
( 
    NNSG2dRendererInstance* pRnd, 
    NNSG2dRendererOverwriteParam flag 
)
{
    pRnd->overwriteEnableFlag |= flag;
}

NNS_G2D_INLINE void 
NNS_G2dSetRendererOverwriteDisable
( 
    NNSG2dRendererInstance* pRnd, 
    NNSG2dRendererOverwriteParam flag 
)
{
    pRnd->overwriteEnableFlag &= ~flag;
}
 
NNS_G2D_INLINE BOOL 
NNS_G2dIsRendererOverwriteEnable
( 
    const NNSG2dRendererInstance* pRnd, 
    NNSG2dRendererOverwriteParam flag 
)
{
    return (BOOL)( pRnd->overwriteEnableFlag & flag );
}


//------------------------------------------------------------------------------
// Rendering Optimization Related Functions
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dOptimizeRenderer

  Description:  This function carries out opitmization of the renderer.
                The use of this API was discontinued.
                If this function is called, a warning message is output.
                
                NNS_G2dBeginRenderingEx() is used instead of this.
                
                
  Arguments:    pRend   :      renderer entity
                
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dOptimizeRenderer( NNSG2dRendererInstance* pRend )
{
#pragma unused( pRend )
    OS_Warning("Sorry, NNS_G2dOptimizeRenderer() is obsolete.Please avoid calling this func.");
}



#ifdef __cplusplus
} /* extern "C"*/
#endif



#endif // NNS_G2D_RENDERER_H_

