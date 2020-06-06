/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_OamSoftwareSpriteDraw.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_OamSoftwareSpriteDraw.h,v $
  Revision 1.7  06/29/2005 02:45:20 AM  kitani_toshikazu
  Changed copyright text 

  Revision 1.6  06/29/2005 01:01:03 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.5  10/28/2004 12:06:31 AM  kitani_toshikazu
  Optimized performance

  Revision 1.4  08/26/2004 07:20:19 AM  kitani_toshikazu
  Implementation of the Z offset features

  Revision 1.3  08/23/2004 08:47:38 AM  kitani_toshikazu
  Changes to the UV value and the argument type

  Revision 1.2  08/17/2004 06:02:39 AM  kitani_toshikazu
  Implementation of the automatic Z value offset features

  Revision 1.1  07/17/2004 07:23:12 AM  kitani_toshikazu
  Changed file name from g2d_OamSoftwareSpriteEmu to g2d_OamSoftwareSpriteDraw.

  Revision 1.1  07/17/2004 06:53:36 AM  kitani_toshikazu
  Separated OAM software emulation drawing relation as separate module.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_OAMSOFTWARESPRITEEMU_H_
#define NNS_G2D_OAMSOFTWARESPRITEEMU_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_Image.h> // TODO : Is this dependency OK? Need to investigate.

#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dUVFlipCorrectFunc

  Description:  Sets the pointer to the UV value correction function
                used with Oam software sprite emulation. 
                
 *---------------------------------------------------------------------------*/
typedef void (*NNS_G2dOamSoftEmuUVFlipCorrectFunc)( fx32*   pRetU0,  
                                                    fx32*   pRetV0, 
                                                    fx32*   pRetU1,  
                                                    fx32*   pRetV1,
                                                    BOOL   bFilpH,
                                                    BOOL   bFilpV );

//------------------------------------------------------------------------------
void    NNS_G2dDrawOneOam3DDirectFast
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
);



//------------------------------------------------------------------------------
void    NNS_G2dDrawOneOam3DDirectWithPosFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
);



//------------------------------------------------------------------------------
void    NNS_G2dDrawOneOam3DDirectWithPosAffineFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr,
    const MtxFx22*                  pMtx
);

//------------------------------------------------------------------------------
void NNS_G2dSetOamSoftEmuSpriteParamCache
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
);

//------------------------------------------------------------------------------
// NNS_G2dDrawOneOam3DDirectUsingParamCache() is not used.
void NNS_G2dDrawOneOam3DDirectUsingParamCacheFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam
);

//------------------------------------------------------------------------------
// Related to callback to correct UV values
void    NNS_G2dSetOamSoftEmuUVFlipCorrectFunc( NNS_G2dOamSoftEmuUVFlipCorrectFunc pFunc );
void    NNS_G2dResetOamSoftEmuUVFlipCorrectFunc();


//------------------------------------------------------------------------------
// Related to functionality that automatically offsets Z values.

void NNSi_G2dSetOamSoftEmuAutoZOffsetFlag   ( BOOL flag );
fx32 NNSi_G2dGetOamSoftEmuAutoZOffset       ( void );
void NNSi_G2dResetOamSoftEmuAutoZOffset     ( void );
void NNSi_G2dSetOamSoftEmuAutoZOffsetStep   ( fx32 step );
fx32 NNSi_G2dGetOamSoftEmuAutoZOffsetStep   ( void );



//------------------------------------------------------------------------------
// inline function
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
NNS_G2D_INLINE void    NNS_G2dDrawOneOam3DDirect
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    G3_PushMtx();
       NNS_G2dDrawOneOam3DDirectFast
       ( 
           pOam, pTexImageAttr, 
           texBaseAddr, pltBaseAddr
       );
    G3_PopMtx(1);
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void    NNS_G2dDrawOneOam3DDirectWithPos
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    G3_PushMtx();
       NNS_G2dDrawOneOam3DDirectWithPosFast
       ( 
           posX, posY, posZ, 
           pOam, pTexImageAttr, 
           texBaseAddr, pltBaseAddr
       );
    G3_PopMtx(1);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void    NNS_G2dDrawOneOam3DDirectWithPosAffine
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr,
    const MtxFx22*                  pMtx
)
{
    G3_PushMtx();
       NNS_G2dDrawOneOam3DDirectWithPosAffineFast
       ( 
           posX, posY, posZ, 
           pOam, pTexImageAttr, 
           texBaseAddr, pltBaseAddr, pMtx 
       );
    G3_PopMtx(1);
}


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_OAMSOFTWARESPRITEEMU_H_

