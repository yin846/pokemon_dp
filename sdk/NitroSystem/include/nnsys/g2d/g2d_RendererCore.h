/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2 D 
  
  File:     g2d_RendererCore.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_RendererCore.h,v $
  Revision 1.16  06/06/2005 12:49:11 AM  kitani_toshikazu
  Revised copyright notation

  Revision 1.15  06/02/2005 08:54:47 AM  kitani_toshikazu
  Cleaned up comments

  Revision 1.14  11/11/2004 05:27:23 AM  kitani_toshikazu
  Discontinued use of NNS_G2dSetRndCoreOamRegisterFuncEx() function.

  Revision 1.13  11/10/2004 05:34:30 AM  kitani_toshikazu
  NNSi_G2dGetRndCore2DMtxCacheAffineIndex
  NNSi_G2dSetRndCore2DMtxCacheAffineIndex.
  was moved. 

  Revision 1.11  11/05/2004 06:11:48 AM  kitani_toshikazu
  Changed API.

  Revision 1.10  11/02/2004 05:18:58 AM  kitani_toshikazu
  Integrated parameter rewrite callback functions.

  Revision 1.9  10/28/2004 02:02:27 AM  kitani_toshikazu
  Adjusted API

  Revision 1.8  10/28/2004 12:06:31 AM  kitani_toshikazu
  Optimized performance

  Revision 1.7  10/21/2004 11:02:03 AM  kitani_toshikazu
  Revised to optimize performance

  Revision 1.6  10/19/2004 11:59:23 AM  kitani_toshikazu
  Interface adjustment with renderer core.

  Revision 1.5  10/18/2004 09:47:44 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.4  10/18/2004 01:16:16 AM  kitani_toshikazu
  Arranged renderer module, code.

  Revision 1.3  10/14/2004 10:04:38 AM  kitani_toshikazu
  Changed composition of NNSG2dRndCore2DMtxCache members.

  Revision 1.2  10/14/2004 01:02:35 AM  kitani_toshikazu
  Added mtxFor3DGE member.

  Revision 1.1  10/07/2004 01:00:01 PM  kitani_toshikazu
  Separated basic part of renderer to a separate module.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_RENDERERCORE_H_
#define NNS_G2D_RENDERERCORE_H_


#include <nitro.h>

#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>

#include <nnsys/g2d/fmt/g2d_Oam_data.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>



#ifdef __cplusplus
extern "C" {
#endif

#define MtxCache_NOT_AVAILABLE                  0xFFFF
#define MtxCache_NOT_AVAILABLE_ForMemFill       0xFFFFFFFF // for memory fill
#define NNS_G2D_NUMBER_OF_2DGRAPHICS_ENGINE     2
#define NNS_G2D_RNDCORE_INTERNAL_OAMBUFFER_SIZE 128
#define NNS_G2D_OAMFLIP_PATTERN_NUM             0x04 // (OAM_FLIP_NONE, OAM_FLIP_H, OAM_FLIP_V, OAM_FLIP_HV )
    

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererAffineTypeOverwiteMode

  Description:  These are the overwrite attributes of the affine setting  part of the OAM attributes.
 *---------------------------------------------------------------------------*/
 
typedef enum NNSG2dRendererAffineTypeOverwiteMode
{
    NNS_G2D_RND_AFFINE_OVERWRITE_NONE,  // Does not overwrite.
    NNS_G2D_RND_AFFINE_OVERWRITE_NORMAL,// Sets to normal affine transformation method
    NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE // Sets to double affine conversion format
    
}NNSG2dRendererAffineTypeOverwiteMode;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dSurfaceType

  Description:  This is the rendering target.
                
 *---------------------------------------------------------------------------*/
// The enum value should not be changed.
// This is related to the internal operation.
typedef enum NNSG2dSurfaceType
{
    NNS_G2D_SURFACETYPE_MAIN3D = 0x00,  // Uses the 3D graphics engine.
    NNS_G2D_SURFACETYPE_MAIN2D = 0x01,  // Uses the 2D graphics engine A.
    NNS_G2D_SURFACETYPE_SUB2D  = 0x02,  // Uses the 2D graphics engine B.
    NNS_G2D_SURFACETYPE_MAX    = 0x03
    
}NNSG2dSurfaceType;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRendererFlip

  Description:  This is the renderer and flip render information.
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dRendererFlip
{
    NNS_G2D_RENDERERFLIP_NONE   = 0x00,
    NNS_G2D_RENDERERFLIP_H      = 0x01,
    NNS_G2D_RENDERERFLIP_V      = 0x02
    
}NNSG2dRendererFlip;


//------------------------------------------------------------------------------
// Carries out role of caching actual affine parameter registration to 2D graphics  engine.
// Duplicate registrations are not executed, and the result of the past   registration is returned.
// 
// If the same NNSG2dRndCore2DMtxCache Index is specified by this,
// the same affine parameters are referenced.
//
// The affineIndex has MtxCache_NOT_AVAILABLE, which means the affine parameter
// registration was not done with the standard values.
// 
typedef struct NNSG2dRndCore2DMtxCache
{
    MtxFx22         m22;
    u16             affineIndex[NNS_G2D_OAMFLIP_PATTERN_NUM][NNS_G2D_NUMBER_OF_2DGRAPHICS_ENGINE];
}NNSG2dRndCore2DMtxCache;


/*---------------------------------------------------------------------------*
  Name:         NNSG2dViewRect

  Description:  This rectangle expresses the visible area.
 *---------------------------------------------------------------------------*/
 
typedef struct NNSG2dViewRect
{
    NNSG2dFVec2            posTopLeft;      // Upper left position of visible area
    NNSG2dFVec2            sizeView;        // view size
    
}NNSG2dViewRect;


 /*---------------------------------------------------------------------------*
  Name:         NNSG2dOamRegisterFunction

  Description:  This is the function pointer used in Oam registration.
                
  Arguments:    pOam :                  Oam attribute newly registered
                affineIndex :           affine index
                                        (may be 32 bytes or more)
                                        (If not used, NNS_G2D_OAM_AFFINE_IDX_NONE
                                        is specified.)
                                        
                bDoubleAffine :         If render in double affine mode or not
                                        (currently not used)
                
  Returns:      registration success or failure
  
 *---------------------------------------------------------------------------*/
typedef BOOL (*NNSG2dOamRegisterFunction)     
( 
    const GXOamAttr*    pOam, 
    u16                 affineIndex, 
    BOOL                bDoubleAffine 
);

typedef BOOL (*NNSG2dOamBlockRegisterFunction)     
( 
    const GXOamAttr*    pOam, 
    u16                 num 
);

                         
/*---------------------------------------------------------------------------*
  Name:         NNSG2dAffineRegisterFunction
  
  Description:  This is the function pointer used with affine parameter registration.
  
  Arguments:    mtx :                  affine conversion matrix
               
  Returns:      affineIndex registered (may be 32 bytes or more)
  
 *---------------------------------------------------------------------------*/
 
typedef u16 (*NNSG2dAffineRegisterFunction)   
( 
    const MtxFx22* mtx 
);


struct NNSG2dRndCoreInstance;
struct NNSG2dRndCoreSurface;

typedef void(*NNSG2dRndCoreDrawCellCallBack)
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell
);

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndDrawOamCallBack
  
  Description:  This is the callback called when rendering the Oam attributes
                and after rendering).
  
  Arguments:    pRend   : pointer to renderer
                pSurface: pointer to render surface
                pCell: Cell
                pMtx    : conversion matrix
               
  Returns:      None.
  
 *---------------------------------------------------------------------------*/
 
typedef void(*NNSG2dRndCoreDrawOamCallBack)
(
    struct NNSG2dRndCoreInstance*   pRend,
    const NNSG2dCellData*           pCell,
    u16                             oamIdx
);

/*---------------------------------------------------------------------------*
  Name:         NNSG2dRndCoreOamOverwriteCallBack
  
  Description:  This is the callback for overwriting the Oam parameters.
                This callback was integrated with the callback
                NNSG2dRndDrawOamCallBack during Oam attribute rendering and
                
                discontinued.
                Carry out overwrite processing of the same parameters for  pRend->currentOam in NNSG2dRndDrawOamCallBack.
                
  
  Arguments:    pRend   : pointer to renderer
                pCell: temporary cell (possible to rewrite)
                oamIdx  : Cell number
               
  Returns:      None.
  
 *---------------------------------------------------------------------------*/
 



//------------------------------------------------------------------------------
typedef struct NNSG2dRndCoreSurface
{
    NNSG2dViewRect                    viewRect;                     // This rectangle expresses the visible area.
    BOOL                              bActive;                      // Is it enabled?
    NNSG2dSurfaceType                 type;                         // surface type
    
    
    NNSG2dRndCoreDrawCellCallBack         pBeforeDrawCellBackFunc;  // includes visibility culling function
    NNSG2dRndCoreDrawCellCallBack         pAfterDrawCellBackFunc;   
    
    NNSG2dRndCoreDrawOamCallBack          pBeforeDrawOamBackFunc;   // callback for Oam parameter overwrite
                                                                    // NNSG2dRndCoreOamOverwriteCallBack
                                                                    // pOamOverwriteFunc was integrated with this callback
    NNSG2dRndCoreDrawOamCallBack          pAfterDrawOamBackFunc;    
}
NNSG2dRndCoreSurface;

struct NNSG2dImageProxy;
struct NNSG2dImagePaletteProxy;

//------------------------------------------------------------------------------
typedef struct NNSG2dRndCoreInstance
{
    NNSG2dRndCoreSurface*                   pCurrentTargetSurface; // rendering target surface
    NNSG2dRendererAffineTypeOverwiteMode    affineOverwriteMode;   // (standard value: NNS_G2D_RND_AFFINE_OVERWRITE_DOUBLE)
    
    //
    // image data referenced by OBJ
    //
    const struct NNSG2dImageProxy*          pImgProxy;             // image information
    const struct NNSG2dImagePaletteProxy*   pPltProxy;             // palette information
    u32                                     base2DCharOffset;      // 2D character base offset
                                                                   // called during BeginRendering call
    u32                                     baseTexAddr3D;         // 3D texture base offset
    u32                                     basePltAddr3D;         // 3D texture palette base offset

    //
    // registration functions related to 2D
    // These must be set up when carrying out 2D HW rendering.
    // pFuncOamRegister and pFuncOamBlockRegister are used exclusively.
    //
    NNSG2dOamRegisterFunction             pFuncOamRegister;        // Oam registration function
    NNSG2dAffineRegisterFunction          pFuncOamAffineRegister;  // Affine registration function
    
    
    // NNSG2dRendererFlip
    // If uses rendering flip or not.
    // Take note that if it is used, affine conversion cannot be used.
    u32                                     flipFlag;              
    
    // 
    // If pCurrentMtxCacheFor2D is set up, affine conversion is carried out for OBJ during OBJ rendering with 2D HW.
    // Due to 2D hardware restrictions affine conversion and flip processing
    // must be exclusively used.
    // Therefore, if pCurrentMtxCacheFor2D was set up with
    // flipFlag != NNS_G2D_RENDERERFLIP_NONE, the renderer core module will display a warning message.
    //
    NNSG2dRndCore2DMtxCache*                pCurrentMtxCacheFor2D;      // current matrix cache(for 2D HW)
    // 
    // pCurrentMxt is the current matrix used during 3D HW rendering.
    // If it was not set up, the unit matrix is used.
    //
    const MtxFx32*                          pCurrentMxt;           // current matrix
    
    
    
    BOOL                                    bDrawEnable;           // Is the rendering enabled?
    
    fx32                                    zFor3DSoftwareSprite;  // Z-value
    
    GXOamAttr                               currentOam;            // current render processing target OAM
    
    MtxFx43                                 mtxFor3DGE;            // matrix for 3D Graphics Engine
                                                                   // generated from pCurrentMxt
    
    
    
                                                                       
}NNSG2dRndCoreInstance;



//------------------------------------------------------------------------------
// Initialization
void NNS_G2dInitRndCore( NNSG2dRndCoreInstance* pRnd );
void NNS_G2dInitRndCoreSurface( NNSG2dRndCoreSurface* pSurface );


//------------------------------------------------------------------------------
// Setting of image proxy
struct NNSG2dImageProxy;
struct NNSG2dImagePaletteProxy;

void NNS_G2dSetRndCoreImageProxy
(
    NNSG2dRndCoreInstance*                     pRnd,
    const struct NNSG2dImageProxy*             pImgProxy, 
    const struct NNSG2dImagePaletteProxy*      pPltProxy
);

//------------------------------------------------------------------------------
// 2D HW Registration Function
void NNS_G2dSetRndCoreOamRegisterFunc
(
    NNSG2dRndCoreInstance*          pRnd,
    NNSG2dOamRegisterFunction       pFuncOamRegister,
    NNSG2dAffineRegisterFunction    pFuncOamAffineRegister
);

//------------------------------------------------------------------------------
// Setting
void NNS_G2dSetRndCoreAffineOverwriteMode
(   NNSG2dRndCoreInstance*                  pRnd, 
    NNSG2dRendererAffineTypeOverwiteMode    mode 
);

void NNS_G2dSetRndCore3DSoftSpriteZvalue
( 
    NNSG2dRndCoreInstance* pRnd, fx32 z 
);

void NNS_G2dSetRndCoreSurface
( 
    NNSG2dRndCoreInstance* pRnd, 
    NNSG2dRndCoreSurface* pSurface 
);

//------------------------------------------------------------------------------
// Matrix Related Functions
void NNS_G2dSetRndCoreCurrentMtx3D
( 
    const MtxFx32*          pCurrentMxt
);
void NNS_G2dSetRndCoreCurrentMtx2D
( 
    const MtxFx32*                 pMtx,
    NNSG2dRndCore2DMtxCache*       pCurrentMtxCacheFor2D
);


//------------------------------------------------------------------------------
// Flip Related Functions
BOOL NNS_G2dIsRndCoreFlipH( const NNSG2dRndCoreInstance* pRnd );
BOOL NNS_G2dIsRndCoreFlipV( const NNSG2dRndCoreInstance* pRnd );
void NNS_G2dSetRndCoreFlipMode( NNSG2dRndCoreInstance* pRnd, BOOL bFlipH, BOOL bFlipV );



//------------------------------------------------------------------------------
// Drawing
void NNS_G2dRndCoreBeginRendering( NNSG2dRndCoreInstance* pRnd );
void NNS_G2dRndCoreEndRendering( void );

void NNS_G2dRndCoreDrawCell( const NNSG2dCellData* pCell );
void NNS_G2dRndCoreDrawCellVramTransfer( const NNSG2dCellData* pCell, u32 cellVramTransferHandle );

void NNS_G2dRndCoreDrawCellFast2D( const NNSG2dCellData* pCell );

void NNS_G2dSetRndCoreCellCloneSource3D ( const NNSG2dCellData* pCell );
void NNS_G2dRndCoreDrawCellClone3D      ( const NNSG2dCellData* pCell );

//------------------------------------------------------------------------------
// inline function
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitRndCore2DMtxCache

  Description:  This function initializes the matrix cache.
                It initializes the affine index information in the matrix cache.
                
                Using NNS_G2dSetRndCore2DMtxCacheMtxParams()
                internally calls when setting the matrix cache contents. 
                
  Arguments:    pMtxCache:      matrix cache
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dInitRndCore2DMtxCache
( 
    NNSG2dRndCore2DMtxCache* pMtxCache 
)
{
    MI_CpuFillFast( pMtxCache->affineIndex, 
                    MtxCache_NOT_AVAILABLE_ForMemFill, 
                    sizeof( pMtxCache->affineIndex ) );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetRndCore2DMtxCacheMtxParams

  Description:  This function sets up the matrix in the matrix cache.
                Internally NNS_G2dInitRndCore2DMtxCache is called and the affine index information is initialized.
                
                
  Arguments:    pMtxCache:      Cell data
                pM:             affine conversion matrix
                                (This is the matrix set in the 2D graphics engine affine parameters.
                                 Take note that the scale elements are  treated differently from a general conversion matrix.))
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetRndCore2DMtxCacheMtxParams
( 
    NNSG2dRndCore2DMtxCache* pMtxCache, 
    MtxFx22*          pM 
)
{
    NNS_G2dInitRndCore2DMtxCache( pMtxCache );
    pMtxCache->m22 = *pM;    
}



#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_RENDERERCORE_H_

