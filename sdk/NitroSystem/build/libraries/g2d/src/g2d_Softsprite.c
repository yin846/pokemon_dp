/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_Softsprite.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Softsprite.c,v $
  Revision 1.28  06/29/2005 08:30:27 AM  kitani_toshikazu
  Corrected comment

  Revision 1.27  05/31/2005 12:06:07 AM  kitani_toshikazu
  Changed copyright year

  Revision 1.26  05/27/2005 07:44:46 AM  kitani_toshikazu
  Changed direction of square polygon planes. (To display correctly in CULL_BACK mode)

  Revision 1.25  11/24/2004 08:57:51 AM  kitani_toshikazu
  (none)

  Revision 1.24  11/24/2004 08:53:11 AM  kitani_toshikazu
  Fixed bug that made NNS_G2dSetSpriteAttrEnable() operate incorrectly the second and subsequent times executed.

  Revision 1.23  11/09/2004 08:27:49 AM  kitani_toshikazu
  * FX32_ONE => << FX32_SHIFT overwrite

  Revision 1.22  11/04/2004 10:14:26 AM  kitani_toshikazu
  Added comment

  Revision 1.21  10/28/2004 12:04:10 AM  kitani_toshikazu
  Optimized performance

  Revision 1.20  10/21/2004 11:01:47 AM  kitani_toshikazu
  Revised to optimize performance

  Revision 1.19  10/18/2004 06:53:02 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.18  10/14/2004 01:00:22 AM  kitani_toshikazu
  Dealt with the specification change regarding
  Change specifications regarding storage of current matrix state

  Revision 1.17  10/07/2004 12:48:31 PM  kitani_toshikazu
  Optimized performance
  Removed the wraparound process from inside the DrawSprite functions

  Revision 1.16  09/27/2004 05:45:32 AM  kitani_toshikazu
  Enabled polygon ID settings

  Revision 1.15  08/24/2004 11:11:34 AM  kitani_toshikazu
  Because of bugs, reverted to original rectangular polygon drawing method.

  Revision 1.14  08/24/2004 12:09:37 AM  kitani_toshikazu
  (none)

  Revision 1.13  08/23/2004 08:45:47 AM  kitani_toshikazu
  Changes to the UV value and the argument type

  Revision 1.12  08/17/2004 06:02:00 AM  kitani_toshikazu
  Changed the Z value type for NNSi_G2dDrawSpriteFast(), etc

  Revision 1.11  07/27/2004 08:59:57 AM  hatamoto_minoru
  Corrected discrepancy in software sprite display time.

  Revision 1.10  07/24/2004 07:42:02 AM  kitani_toshikazu
  Changing of function names.

  Revision 1.9  07/23/2004 07:49:13 AM  kitani_toshikazu
  NNS_G2dDrawOneOam3DDirectWithPosAffineFast()
  did not work properly for anything but double affine mode. Fixed.

  Revision 1.8  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.7  06/11/2004 04:54:05 AM  kitani_toshikazu
  Fixed bug in NNS_G2dDrawSpriteFast()

  Revision 1.6  06/08/2004 05:12:19 AM  hatamoto_minoru
  Fixed bug in DrawSprite_

  Revision 1.5  06/02/2004 09:54:18 AM  kitani_toshikazu
  Changed NNS_G2dDrawSpriteEx => NNSi_G2dDrawSpriteEx

  Revision 1.4  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.3  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nnsys/g2d/g2d_Softsprite.h>


static  NNSG2dExtendedSprite              defaultSpriteAttr_;
static  u32                         defaultAttrEnable_  = 0x0;
static  u16                         currentPolygonID_   = 0x0;



static void DrawSprite_( const NNSG2dSimpleSprite* pS, const NNSG2dBasicSprite* pB, const NNSG2dExtendedSprite* pE );

//---------------------------------------------------------------------------
// The specifications have been changed so the current matrix is not saved.
// All functions that use this function have the same characteristics.
static void DrawSprite_( const NNSG2dSimpleSprite* pS, const NNSG2dBasicSprite* pB, const NNSG2dExtendedSprite* pE )
{
    const fx32      size = FX32_HALF;
    
    fx32 U0 = 0; 
    fx32 U1 = pS->size.x << FX32_SHIFT; 
    
    fx32 V0 = 0; 
    fx32 V1 = pS->size.y << FX32_SHIFT; 
    
    // Overwrite the UV values.
    if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_UV ) )
    {
    
        U0 = pE->uvUL.x;
        U1 = pE->uvLR.x;
        V0 = pE->uvUL.y;
        V1 = pE->uvLR.y;
    }
    
    // Flip
    if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_FLIP ) )
    {
        fx32    temp;
        // H
        if( pE->flipH )
        {
            temp = U0;
            U0 = U1;
            U1 = temp;
        }
        // V
        if( pE->flipV )
        {
            temp = V0;
            V0 = V1;
            V1 = temp;
        }
    }
    
    {
        //
        // SRT
        //
        G3_Translate( ( pS->pos.x << FX32_SHIFT ) + ((pS->size.x >> 1) << FX32_SHIFT ), 
                      ( pS->pos.y << FX32_SHIFT ) + ((pS->size.y >> 1) << FX32_SHIFT ), 
                      ( (int)pS->priority << FX32_SHIFT ) );
                      
        // RotO
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_ROTO ) )
        {
            G3_Translate( pE->rotO.x << FX32_SHIFT , 
                          pE->rotO.y << FX32_SHIFT , 
                          0 );            
        }
        // RotXY
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_ROTXY ) )
        {
            G3_RotX( FX_SinIdx( pE->rotX ), FX_CosIdx( pE->rotX ) );
            G3_RotY( FX_SinIdx( pE->rotY ), FX_CosIdx( pE->rotY ) );
        }
        G3_RotZ( FX_SinIdx( pS->rotZ ), FX_CosIdx( pS->rotZ ) );
        
        
        // RotO
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_ROTO ) )
        {
            G3_Translate( -pE->rotO.x << FX32_SHIFT , 
                          -pE->rotO.y << FX32_SHIFT , 
                          0 );
        }
        G3_Scale( pS->size.x << FX32_SHIFT, 
                  pS->size.y << FX32_SHIFT, 
                  FX32_ONE );
        
        
        //
        //
        //
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_TEXTUE ) )
        {
            // Texture setting.
            if( pB->pTextureAttr )
            {
                const NNSG2dImageAttr* pAttr = pB->pTextureAttr;
                
                G3_TexImageParam( pAttr->fmt,        // use alpha texture
                         GX_TEXGEN_TEXCOORD,         // use texcoord
                         pAttr->sizeS,               // 16 pixels
                         pAttr->sizeT,               // 16 pixels
                         GX_TEXREPEAT_NONE,          // no repeat
                         GX_TEXFLIP_NONE,            // no flip
                         pAttr->plttUse,             // use color 0 of the palette
                         pB->texAddr                 // the offset of the texture image
                         );
                
                //
                // If palette format....
                //                         
                if( NNS_G2dIsPaletteImageFmt( pAttr ) )
                {
                    G3_TexPlttBase( (u32)(pB->plttAddr + 32 * pB->plttOffset), pAttr->fmt );                                
                }
                
            }else{
                OS_Warning("The Texture Attribute of software sprite must be specified.");
            }
        }
        
        //
        // alpha blending
        //
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_ALPHA ) )
        {
            //
            // Caution: If alpha = 0  drawing does not take place
            //
            if( pS->alpha == 0 )
            {
                return;
            }
            
            //
            // Note: Internally, G2D freely makes changes to the register settings of the Graphics Engine.
            //       Moreover, the settings cannot be restored.
            //
            G3_PolygonAttr(GX_LIGHTMASK_NONE,              // disable lights
                   GX_POLYGONMODE_MODULATE,                // modulation mode
                   GX_CULL_NONE,                           // cull back
                   currentPolygonID_,                      // polygon ID(0 - 63)
                   pS->alpha,                              // alpha(0 - 31)
                   0                                       // OR of GXPolygonAttrMisc's value
                   );
        }
        //
        // VertexColor
        //
        if( NNS_G2dIsSpriteAttrEnable( NNS_G2D_SPRITEATTR_COLOR ) )
        {
            G3_Color( pB->color );
        }
        
        //
        // Draw a quad polygon. (Changed rendering plane direction)
        //
        G3_Begin( GX_BEGIN_QUADS );
            
            G3_TexCoord( U0, V1 );
            G3_Vtx10( 0 - size,0 + size, 0 ); 
            
            G3_TexCoord( U1, V1 );
            G3_Vtx10( 0 + size, 0 + size, 0 ); 
            
            G3_TexCoord( U1, V0 );
            G3_Vtx10( 0 + size, -size, 0 ); 
            
            G3_TexCoord( U0, V0 );
            G3_Vtx10( 0 - size, -size, 0 );
            
        G3_End( );
    }
}

//------------------------------------------------------------------------------
static void SetAffine43( MtxFx43* pDst, const MtxFx22* pSubMtx, fx32 x, fx32 y, fx32 z )
{
    NNS_G2D_NULL_ASSERT(pDst);
    NNS_G2D_NULL_ASSERT(pSubMtx);
    
    
    pDst->_00 = pSubMtx->_00;
    pDst->_01 = pSubMtx->_01;
    pDst->_02 = 0;
    pDst->_10 = pSubMtx->_10;
    pDst->_11 = pSubMtx->_11;
    pDst->_12 = 0;
    pDst->_20 = 0;
    pDst->_21 = 0;
    pDst->_22 = FX32_ONE;
    pDst->_30 = x;
    pDst->_31 = y;
    pDst->_32 = z;
}
//------------------------------------------------------------------------------
static void SetBaseMtx_( const MtxFx22* baseMtx )
{
    //
    // RTS
    //
    MtxFx43       mtx;
    SetAffine43( &mtx, baseMtx, 0, 0, 0 );
    
    // base
    G3_MultMtx43( &mtx );
    
}
//------------------------------------------------------------------------------
static void DrawOneQuad_( fx32 U0, fx32 U1, fx32 V0, fx32 V1 )
{
    const fx32      size = FX32_ONE;
            
    {
        //
        // Draw a quad polygon.
        //
        G3_Begin( GX_BEGIN_QUADS);
            
            G3_TexCoord( U0, V1 );
            G3_Vtx( 0 ,0 + size, 0 ); 
            
            G3_TexCoord( U1, V1 );
            G3_Vtx( 0 + size, 0 + size, 0 ); 
            
            G3_TexCoord( U1, V0 );
            G3_Vtx( 0 + size, 0 , 0 ); 
            
            G3_TexCoord( U0, V0 );
            G3_Vtx( 0 , 0, 0 );
            
            
        G3_End( );
    }
}

//------------------------------------------------------------------------------
// The OAM manager is only used when 2D emulation drawing
// The UV settings are configured externally
static void DrawOneQuadCenter_( fx32 U0, fx32 U1, fx32 V0, fx32 V1 )
{
    const fx32      size = FX32_HALF;
            
    {
        //
        // Draw a quad polygon.
        //
        G3_Begin( GX_BEGIN_QUADS);
            
            G3_TexCoord( U0, V1 );
            G3_Vtx( 0 -size ,0 + size, 0 ); 
            
            G3_TexCoord( U1, V1 );
            G3_Vtx( 0 + size, 0 + size, 0 ); 
            
            G3_TexCoord( U1, V0 );
            G3_Vtx( 0 + size, -size , 0 ); 
            
            G3_TexCoord( U0, V0 );
            G3_Vtx( -size , -size, 0 );
            
        G3_End( );
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetupSoftwareSpriteCamera

  Description:  Sets the 3D Graphic Engine's camera for sprite drawing.
                
                
  Arguments:    None
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetupSoftwareSpriteCamera()
{
    G3_ViewPort(0, 0, 255, 191);                           // Viewport
    {
        G3_OrthoW( FX32_ONE * 0,
                  FX32_ONE * 192,
                  FX32_ONE * 0,
                  FX32_ONE * 256,
                  FX32_ONE * -1024,
                  FX32_ONE * 1024,
                  FX32_ONE * 1024,
                  NULL );
        G3_StoreMtx(0);
        // Restore matrix mode to original
        G3_MtxMode( GX_MTXMODE_POSITION );        
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetSpriteAttrEnable

  Description:  Sets the SoftwareSprite enable attributes
                
                
  Arguments:    attr:         [IN] Each NNSG2dSpriteAttr value or their logical sum. 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetSpriteAttrEnable( u32 attr )
{
    defaultAttrEnable_ = attr;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsSpriteAttrEnable

  Description:  Determines if SoftwareSprite attributes are enabled.
                
                
  Arguments:    attr:         [IN] Each NNSG2dSpriteAttr value (Note: Logical sum is not taken.) 
                
  Returns:      Whether attribute is enabled or not
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsSpriteAttrEnable( NNSG2dSpriteAttr attr )
{
    return (BOOL)(defaultAttrEnable_ & (u32)attr);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetSpriteCurrentPolygonID

  Description:  Sets the polygon ID used for SoftwareSprite drawing
                
                
  Arguments:    id:         [IN] Polygon ID used for SoftwareSprite drawing (0-63) 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetSpriteCurrentPolygonID( u16 id )
{
    NNS_G2D_MINMAX_ASSERT( id, 0, 63 ); 
    
    currentPolygonID_ = id; 
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetSpriteDefaultAttr

  Description:  Sets the default value for SoftwareSprite
                Insufficient information is supplemented by this value.
                
                
  Arguments:    pE:         [IN]  Sprite
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetSpriteDefaultAttr( const NNSG2dExtendedSprite* pE )
{
    defaultSpriteAttr_ = *pE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetSpriteDefaultAttr

  Description:  Gets the pointer for referencing the SoftwareSprite default  value
                
  Arguments:    None
                
  Returns:      Pointer for referencing the SoftwareSprite default value
  
 *---------------------------------------------------------------------------*/
const NNSG2dExtendedSprite* NNS_G2dGetSpriteDefaultAttr( )
{
    return &defaultSpriteAttr_;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawSpriteSimpleFast ... NNS_G2dDrawSpriteExtendedFast

  Description:  Draws SoftwareSprite.
                Insufficient information is supplemented by the default value.
                Note that the contents of the current matrix in the geometry engine are not stored before and after the function is called. 
                
                
                The versions that save the contents do not add "Fast" to the end of the function name. 
                NNS_G2dDrawSpriteSimple ... NNS_G2dDrawSpriteExtended
                were prepared. 
                
                
  Arguments:    pS,pB,pE:         [IN]  Sprite
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawSpriteSimpleFast( const NNSG2dSimpleSprite* pS )
{
    DrawSprite_( pS, &defaultSpriteAttr_.basicParams, &defaultSpriteAttr_ ); 
}
//---------------------------------------------------------------------------
void NNS_G2dDrawSpriteBasicFast( const NNSG2dBasicSprite* pB )
{
    DrawSprite_( &pB->simpleParams, pB, &defaultSpriteAttr_ ); 
}
//---------------------------------------------------------------------------
void NNS_G2dDrawSpriteExtendedFast( const NNSG2dExtendedSprite* pE )
{
    DrawSprite_( &pE->basicParams.simpleParams, &pE->basicParams, pE ); 
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawSpriteFast

  Description:  Draws SoftwareSprite
                
                
  Arguments:    px:         [IN]  x
                py:         [IN]  y
                pz:         [IN]  z
                sx:         [IN]  size x
                sy:         [IN]  size y
                U0:         [IN]  UV value (upper left U)
                V0:         [IN]  UV value (upper left V)
                U1:         [IN]  UV value (lower right U)
                V1:         [IN]  UV value (lower right V)
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawSpriteFast
( 
    s16 px, s16 py, int pz, 
    int sx, int sy, 
    int U0, int V0, int U1, int V1 
)
{
    // T
    G3_Translate( FX32_ONE * px, FX32_ONE * py, FX32_ONE * pz );    
    // S
    G3_Scale( FX32_ONE * sx, FX32_ONE * sy, FX32_ONE );
       
    DrawOneQuad_( FX32_ONE * U0, FX32_ONE * U1, FX32_ONE * V0, FX32_ONE * V1 );    
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDrawSpriteWithMtxFast

  Description:  Draws a sprite.
                The Z value and UV value type differs from NNS_G2dDrawSpriteFast.
                Used from the Renderer core module.             
                
                
  Arguments:    Omitted
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dDrawSpriteFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
)
{
    // T
    G3_Translate( px << FX32_SHIFT, py << FX32_SHIFT, pz );    
    // S
    G3_Scale( sx << FX32_SHIFT, sy << FX32_SHIFT, FX32_ONE );
       
    {
       const fx32      size = FX32_ONE;
           
           
       //
       // Draw a quad polygon.
       //
       G3_Begin( GX_BEGIN_QUADS);
           
           G3_TexCoord( U0, V1 );
           G3_Vtx( 0 ,0 + size, 0 ); 
                       
           G3_TexCoord( U1, V1 );
           G3_Vtx( 0 + size, 0 + size, 0 ); 
                   
           G3_TexCoord( U1, V0 );
           G3_Vtx( 0 + size, 0 , 0 ); 
               
           G3_TexCoord( U0, V0 );
           G3_Vtx( 0 , 0, 0 );
          
       G3_End( );
    }
    
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDrawSpriteWithMtxFast

  Description:  Render a sprite.
                Matrix specified with arguments.
                Sprites are drawn the same way as OBJ in double-size affine mode.
                This function is used when using the OAM manager's software
                sprite emulation feature.
                
                
  Arguments:    Omitted
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dDrawSpriteWithMtxFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    const MtxFx22* baseMtx, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
)
{
    // T
    G3_Translate( FX32_ONE * px + FX32_HALF * sx, FX32_ONE * py + FX32_HALF * sy, pz );    
    // R or S
    SetBaseMtx_( baseMtx );
    // S
    G3_Scale( FX32_ONE * sx, FX32_ONE * sy, FX32_ONE );
       
    DrawOneQuadCenter_( U0, U1, V0, V1 );

}




/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDrawSpriteWithMtxDoubleAffineFast

  Description:  Draws sprites.
                Matrix specified with arguments
                Sprites are drawn the same way as OBJ in double-size  affine mode.
                This function is used when using the OAM manager's software
                sprite emulation feature.
                
                
  Arguments:    Omitted
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dDrawSpriteWithMtxDoubleAffineFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    const MtxFx22* baseMtx, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
)
{
    // T
    // Note that it is not FX32_HALF but FX32_ONE. Double-size mode behavior
    G3_Translate( FX32_ONE * px + FX32_ONE * sx, FX32_ONE * py + FX32_ONE * sy, pz );    
    // R or S
    SetBaseMtx_( baseMtx );
    // S
    G3_Scale( FX32_ONE * sx, FX32_ONE * sy, FX32_ONE );
       
    DrawOneQuadCenter_( U0, U1, V0, V1 );
}











