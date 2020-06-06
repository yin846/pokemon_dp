/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_Softsprite.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Softsprite.h,v $
  Revision 1.13  11/04/2004 10:13:28 AM  kitani_toshikazu
  Added comment

  Revision 1.12  10/28/2004 12:06:12 AM  kitani_toshikazu
  Added high speed rendering function

  Revision 1.11  08/23/2004 08:47:33 AM  kitani_toshikazu
  Changes to the UV value and the argument type

  Revision 1.10  08/17/2004 06:02:47 AM  kitani_toshikazu
  Changed the Z value type for NNSi_G2dDrawSpriteFast(), etc

  Revision 1.9  07/24/2004 07:42:20 AM  kitani_toshikazu
  Changing of function names.

  Revision 1.8  07/23/2004 07:49:25 AM  kitani_toshikazu
  NNS_G2dDrawOneOam3DDirectWithPosAffineFast()
  did not work properly for anything but double affine mode. Fixed.

  Revision 1.7  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.6  06/02/2004 09:54:00 AM  kitani_toshikazu
  Changed NNS_G2dDrawSpriteEx => NNSi_G2dDrawSpriteEx

  Revision 1.5  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.4  05/10/2004 04:42:28 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.3  04/26/2004 04:33:14 AM  kitani_toshikazu
  Process to change interface names

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:54 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_SOFTWARESPRITE_H_
#define NNS_G2D_SOFTWARESPRITE_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>
#include <nnsys/g2d/g2d_Image.h>


#ifdef __cplusplus
extern "C" {
#endif

//
// aliases of functions with names changed
// Previous functions declared as aliases to preserve compatibility.
// 
#define NNS_G2dSetCurrentPolygonID      NNS_G2dSetSpriteCurrentPolygonID
#define NNS_G2dSetDefaultAttr           NNS_G2dSetSpriteDefaultAttr
#define NNS_G2dGetDefaultAttr           NNS_G2dGetSpriteDefaultAttr



/*---------------------------------------------------------------------------*
  Name:         NNSG2dSpriteAttr

  Description:  Sprite Attributes
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dSpriteAttr
{
    NNS_G2D_SPRITEATTR_ALPHA  = 0x1,
    NNS_G2D_SPRITEATTR_TEXTUE = 0x2,
    NNS_G2D_SPRITEATTR_COLOR  = 0x4,
    NNS_G2D_SPRITEATTR_UV     = 0x8,
    NNS_G2D_SPRITEATTR_FLIP   = 0x10,
    NNS_G2D_SPRITEATTR_ROTO   = 0x20,
    NNS_G2D_SPRITEATTR_ROTXY  = 0x40

}NNSG2dSpriteAttr;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dSimpleSprite

  Description:  Sprite Entity
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dSimpleSprite
{
    NNSG2dSVec2        pos;        // position
    NNSG2dSVec2        size;       // The size
    
    u16                rotZ;       // rotation angle ( unit = 360 / 65535 )
    u8                 priority;   // display priority
    u8                 alpha;      // alpha ( 0 - 31 )
    
}
NNSG2dSimpleSprite;

//------------------------------------------------------------------------------
typedef struct NNSG2dBasicSprite
{
    NNSG2dSimpleSprite        simpleParams;   // NNSG2dSimpleSprite
    NNSG2dImageAttr*          pTextureAttr;   // texture attributes
    u32                       texAddr;        // texture address
    u32                       plttAddr;       // palette address
    u16                       plttOffset;     // only when use palette texture
    GXRgb                     color;          // color
}
NNSG2dBasicSprite;

//------------------------------------------------------------------------------
typedef struct NNSG2dExtendedSprite
{
    NNSG2dBasicSprite         basicParams;// NNSG2dBasicSprite
    NNSG2dFVec2               uvUL;       // UV (square: upper left)
    NNSG2dFVec2               uvLR;       // UV (square: upper right)
    BOOL                      flipH;      // horizontal flip
    BOOL                      flipV;      // vertical flip
    NNSG2dSVec2               rotO;       // rotation center
    u16                       rotX;       // X rotation
    u16                       rotY;       // Y rotation
}
NNSG2dExtendedSprite;




//------------------------------------------------------------------------------
// Initialization
void NNS_G2dSetupSoftwareSpriteCamera();

//------------------------------------------------------------------------------
// attribute setting
void NNS_G2dSetSpriteAttrEnable( u32 attr );
BOOL NNS_G2dIsSpriteAttrEnable( NNSG2dSpriteAttr attr );
void NNS_G2dSetSpriteCurrentPolygonID( u16 id );

void                          NNS_G2dSetSpriteDefaultAttr( const NNSG2dExtendedSprite* pE );
const NNSG2dExtendedSprite*   NNS_G2dGetSpriteDefaultAttr( );

//------------------------------------------------------------------------------
// Drawing
void NNS_G2dDrawSpriteSimpleFast( const NNSG2dSimpleSprite* pS );
void NNS_G2dDrawSpriteBasicFast( const NNSG2dBasicSprite* pB );
void NNS_G2dDrawSpriteExtendedFast( const NNSG2dExtendedSprite* pE );


//------------------------------------------------------------------------------
//
// API substituting direct parameters
//
//------------------------------------------------------------------------------
void NNS_G2dDrawSpriteFast
( 
    s16 px, s16 py, int pz, 
    int sx, int sy, 
    int U0, int V0, int U1, int V1 
);

//------------------------------------------------------------------------------
// Inline functions
//------------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawSpriteSimple ... NNS_G2dDrawSprite

  Description:  Render a sprite.
                The contents of the current matrix in the graphics engine are stored before and after the function is called.
                
                
  Arguments:    Omitted
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dDrawSpriteSimple( const NNSG2dSimpleSprite* pS )
{
    G3_PushMtx();
       NNS_G2dDrawSpriteSimpleFast( pS );
    G3_PopMtx(1);
}

NNS_G2D_INLINE void NNS_G2dDrawSpriteBasic( const NNSG2dBasicSprite* pB )
{
    G3_PushMtx();
       NNS_G2dDrawSpriteBasicFast( pB );
    G3_PopMtx(1);
}
NNS_G2D_INLINE void NNS_G2dDrawSpriteExtended( const NNSG2dExtendedSprite* pE )
{
    G3_PushMtx();
       NNS_G2dDrawSpriteExtendedFast( pE );
    G3_PopMtx(1);
}

NNS_G2D_INLINE void NNS_G2dDrawSprite
( 
    s16 px, s16 py, int pz, 
    int sx, int sy, 
    int U0, int V0, int U1, int V1 
)
{
    G3_PushMtx();
       NNS_G2dDrawSpriteFast( px, py, pz, sx, sy, U0, V0, U1, V1 );
    G3_PopMtx(1);
}


//------------------------------------------------------------------------------
// library internal release
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Note that the type of the Z value and the UV value are different than in NNS_G2dDrawSpriteFast.
void NNSi_G2dDrawSpriteFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
);

//------------------------------------------------------------------------------
void NNSi_G2dDrawSpriteWithMtxFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    const MtxFx22* baseMtx, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
);

//------------------------------------------------------------------------------
void NNSi_G2dDrawSpriteWithMtxDoubleAffineFast
( 
    s16 px, s16 py, fx32 pz, 
    int sx, int sy, 
    const MtxFx22* baseMtx, 
    fx32 U0, fx32 V0, fx32 U1, fx32 V1 
);




#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_SOFTWARESPRITE_H_

