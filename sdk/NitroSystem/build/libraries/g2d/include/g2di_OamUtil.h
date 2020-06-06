/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     .c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_OamUtil.h,v $
  Revision 1.19  06/02/2005 01:57:43 AM  kitani_toshikazu
  NNSi_G2dRemovePositionAdjustmentFromDoubleAffineOBJ
  Added.

  Revision 1.18  2005/06/01 05:06:52 AM  kitani_toshikazu
  Fixed-point multiplication => replace with shift

  Revision 1.17  05/31/2005 11:27:07 AM  kitani_toshikazu
  Reivsed double-size affine OBJ correction calculations

  Revision 1.16  05/20/2005 08:48:43 AM  hatamoto_minoru
  Revised copyright year

  Revision 1.15  04/28/2005 02:31:09 AM  kitani_toshikazu
  Replaced NNSi_G2dRepeatXinCellSpace()
  with NNS_G2dRepeatXinCellSpace

  Revision 1.14  10/28/2004 12:28:03 AM  kitani_toshikazu
  (none)

  Revision 1.13  10/28/2004 12:03:41 AM  kitani_toshikazu
  Changed the method used for accessing flip information

  Revision 1.12  10/18/2004 09:47:54 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.11  10/08/2004 04:37:28 AM  kitani_toshikazu
  Internal modules such as NNSi_G2dGetOamSize to the release module

  Revision 1.10  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.9  06/23/2004 08:53:36 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.8  06/23/2004 02:01:07 AM  kitani_toshikazu
  Revised the boundary values of the OBJ wrapping process

  Revision 1.7  06/22/2004 06:33:50 AM  kitani_toshikazu
  Fixed a bug in the screen wrapping process

  Revision 1.6  06/11/2004 04:51:45 AM  kitani_toshikazu
  Fixed a bug in NNSi_G2dGetOamX,Y()

  Revision 1.5  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.4  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.3  04/23/2004 12:27:24 AM  kitani_toshikazu
  Added workaround for color palette swap table.

  Revision 1.2  04/21/2004 11:49:04 AM  kitani_toshikazu
  Renamed functions

  Revision 1.1  04/21/2004 07:20:08 AM  kitani_toshikazu
  Updated the folder structures related to the internal module

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2DI_OAMUTIL_H_
#define NNS_G2DI_OAMUTIL_H_

#include <nitro.h>

#include <nnsys/g2d/fmt/g2d_Vec_data.h>
#include <nnsys/g2d/fmt/g2d_Oam_data.h>

#include <nnsys/g2d/g2d_OAM_Types.h>
//
// Utility functions for OAM data processing.
//

//------------------------------------------------------------------------------
// OAM Utilitys
//------------------------------------------------------------------------------
static fx32             NNSi_G2dGetOamSin( const NNSG2dOamAffineParams* pOamAff );
static fx32             NNSi_G2dGetOamCos( const NNSG2dOamAffineParams* pOamAff );
static s16              NNSi_G2dGetOamX( const GXOamAttr* oamAttr );
static s16              NNSi_G2dGetOamY( const GXOamAttr* oamAttr );
static u16              NNSi_G2dGetOamName( const GXOamAttr* oamAttr );
static u16              NNSi_G2dGetOamColorParam( const GXOamAttr* oamAttr );
static BOOL             NNSi_G2dGetOamFlipH( const GXOamAttr* oamAttr );
static BOOL             NNSi_G2dGetOamFlipV( const GXOamAttr* oamAttr );
static void             NNSi_G2dSetInvisible( GXOamAttr* oamAttr );
static BOOL             NNSi_G2dIsVisible( const GXOamAttr* pOam );
static u16              NNSi_G2dGetTexelS( GXTexSizeS s );
static u16              NNSi_G2dGetTexelT( GXTexSizeT t );
static u16              NNSi_G2dGetTextureBaseAddrOffs( u16 charIdx, 
                                                 GXTexSizeS sS, 
                                                 GXTexSizeT sT, 
                                                 u16 bytePerTexel );
static u16              NNSi_G2dGetTextureU( u16 charIdx, GXTexSizeS sS );
static u16              NNSi_G2dGetTextureV( u16 charIdx, GXTexSizeS sS );


static BOOL             NNSi_G2dIsRSEnable( const GXOamAttr* pOam );
static  u16             NNSi_G2dGetAffineIdx( const GXOamAttr* pOam );
static void             NNSi_G2dAdjustDifferenceOfRotateOrientation
                                                ( 
                                                   const GXOamAttr* pOam, 
                                                   const MtxFx22* pM, 
                                                   NNSG2dFVec2* pV, 
                                                   BOOL adjustDoubleAffine 
                                                );

static const u16 NNSI_G2D_NNSI_G2D_SIZE_OBJ_CHAR_TEXEL_S = 0x8;
static const u16 NNSI_G2D_NNSI_G2D_SIZE_OBJ_CHAR_TEXEL_T = 0x8;
static const u16 NNSI_G2D_SIZE_OBJ_CHAR = NNSI_G2D_NNSI_G2D_SIZE_OBJ_CHAR_TEXEL_S * NNSI_G2D_NNSI_G2D_SIZE_OBJ_CHAR_TEXEL_T;


//------------------------------------------------------------------------------
NNS_G2D_INLINE  fx32 NNSi_G2dGetOamSin( const NNSG2dOamAffineParams* pOamAff )
{
    NNS_G2D_NULL_ASSERT( pOamAff );
    // see G2_SetOBJAffine()
    return (fx32)((u32)pOamAff->PB << 4);   // sin = oam->PB
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE fx32 NNSi_G2dGetOamCos( const NNSG2dOamAffineParams* pOamAff )
{
    NNS_G2D_NULL_ASSERT( pOamAff );
    // see G2_SetOBJAffine()
    return (fx32)((u32)pOamAff->PA << 4);   // cos = oam->PA
}

//------------------------------------------------------------------------------
// Adjustment of value done by a separate function
NNS_G2D_INLINE s16 NNSi_G2dGetOamX( const GXOamAttr* oamAttr )
{
    NNS_G2D_NULL_ASSERT( oamAttr ); 
    {
        s16 val = (s16)( ( GX_OAM_ATTR01_X_MASK & oamAttr->attr01 ) >> GX_OAM_ATTR01_X_SHIFT );
        return val;
    }
}


//------------------------------------------------------------------------------
// Adjustment of value done by a separate function
NNS_G2D_INLINE s16 NNSi_G2dGetOamY( const GXOamAttr* oamAttr )
{
    NNS_G2D_NULL_ASSERT( oamAttr ); 
    {
        s16 val = (s16)( ( GX_OAM_ATTR01_Y_MASK & oamAttr->attr01 ) >> GX_OAM_ATTR01_Y_SHIFT  );
        return val;
    }
}


//------------------------------------------------------------------------------
// Adjustment of value done by a separate function
// 0 --- 255
NNS_G2D_INLINE s16 NNSi_G2dRepeatXinScreenArea( s16 srcX )
{
    if( srcX > 0x0FF )
    { 
        srcX |= 0xFF00; 
    }
    return srcX;
}

//------------------------------------------------------------------------------
// Adjustment of value done by a separate function
// 0 --- 191
NNS_G2D_INLINE s16 NNSi_G2dRepeatYinScreenArea( s16 srcY )
{
    if( srcY > 0xBF )
    { 
        srcY |= 0xFF00; 
    }
    return srcY;
}

//------------------------------------------------------------------------------
// The following functions were consolidated into NNS_G2dRepeatXinCellSpace, which is found in g2d_Cell_data.h
//NNS_G2D_INLINE s16 NNSi_G2dRepeatXinCellSpace( s16 srcX )
//NNS_G2D_INLINE s16 NNSi_G2dRepeatYinCellSpace( s16 srcY )

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetOamX( GXOamAttr* oamAttr, s16 x )
{
    NNS_G2D_NULL_ASSERT( oamAttr ); 
    oamAttr->attr01 |= ( GX_OAM_ATTR01_X_MASK & ( (u32)x << GX_OAM_ATTR01_X_SHIFT ) );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetOamY( GXOamAttr* oamAttr, s16 y )
{
    NNS_G2D_NULL_ASSERT( oamAttr ); 
    oamAttr->attr01 |= ( GX_OAM_ATTR01_Y_MASK & ( (u32)y << GX_OAM_ATTR01_Y_SHIFT ) );
} 

//------------------------------------------------------------------------------
// We use Name ( the return value of this func. ) as key of Texture table.
NNS_G2D_INLINE u16 NNSi_G2dGetOamName( const GXOamAttr* oamAttr )
{
    NNS_G2D_NULL_ASSERT( oamAttr );
    return (u16)(( GX_OAM_ATTR2_NAME_MASK & oamAttr->attr2 ) >> GX_OAM_ATTR2_NAME_SHIFT);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNSi_G2dGetOamColorParam( const GXOamAttr* oamAttr )
{
    NNS_G2D_NULL_ASSERT( oamAttr );
    return (u16)( ( GX_OAM_ATTR2_CPARAM_MASK & oamAttr->attr2 ) >> GX_OAM_ATTR2_CPARAM_SHIFT );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dGetOamFlipH( const GXOamAttr* oamAttr )
{
    //return (BOOL)( ( GX_OAM_ATTR01_HF_MASK & oamAttr->attr01 ) >> GX_OAM_ATTR01_HF_SHIFT );
    return (BOOL)( oamAttr->flipH );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dGetOamFlipV( const GXOamAttr* oamAttr )
{
//    return (BOOL)(( GX_OAM_ATTR01_VF_MASK & oamAttr->attr01 ) >> GX_OAM_ATTR01_VF_SHIFT);
    return (BOOL)( oamAttr->flipV );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNSi_G2dGetOamAffineIdx( const GXOamAttr* oamAttr )
{
    return (u16)(( GX_OAM_ATTR01_RS_MASK & oamAttr->attr01 ) >> GX_OAM_ATTR01_RS_SHIFT );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetInvisible( GXOamAttr* oamAttr )
{
    NNS_G2D_NULL_ASSERT( oamAttr );
    
    oamAttr->attr0 &= ~GX_OAM_ATTR01_RSENABLE_MASK;    
    oamAttr->attr0 |= ( 0x2 << GX_OAM_ATTR01_RSENABLE_SHIFT );
}



//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dIsVisible( const GXOamAttr* pOam )
{
    NNS_G2D_NULL_ASSERT( pOam );
    return ( 0x2 != ( GX_OAM_ATTR01_RSENABLE_MASK & pOam->attr0 ) >> GX_OAM_ATTR01_RSENABLE_SHIFT ) ? TRUE : FALSE; 
}

//------------------------------------------------------------------------------
static u16 NNSi_G2dGetTexelS( GXTexSizeS s )
{
    return (u16)( 0x0001 << ( 0x3 + s ) );
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNSi_G2dGetTexelT( GXTexSizeT t )
{
    return (u16)( 0x0001 << ( 0x3 + t ) );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNSi_G2dGetTextureBaseAddrOffs( u16 charIdx, GXTexSizeS sS, GXTexSizeT sT, u16 bytePerTexel )
{    
#pragma unused ( sS )
#pragma unused ( sT )
#pragma unused ( bytePerTexel )

    const u16 retOffs = (u16)( charIdx * NNSI_G2D_SIZE_OBJ_CHAR );
    NNS_G2D_MINMAX_ASSERT( retOffs, 0, bytePerTexel * NNSi_G2dGetTexelS( sS ) * NNSi_G2dGetTexelT( sT ) );
    
    return retOffs;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNSi_G2dGetTextureU( u16 charIdx, GXTexSizeS sS )
{
    const int   numCharPerS = NNSi_G2dGetTexelS( sS ) / NNSI_G2D_NNSI_G2D_SIZE_OBJ_CHAR_TEXEL_S;
    const int   U           = charIdx % numCharPerS;
    
    return (u16)(U * NNSI_G2D_NNSI_G2D_SIZE_OBJ_CHAR_TEXEL_S);
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNSi_G2dGetTextureV( u16 charIdx, GXTexSizeS sS )
{
    const int numCharPerS = NNSi_G2dGetTexelS( sS ) / NNSI_G2D_NNSI_G2D_SIZE_OBJ_CHAR_TEXEL_S;
    const int V = charIdx / numCharPerS;
    
    return (u16)(V * NNSI_G2D_NNSI_G2D_SIZE_OBJ_CHAR_TEXEL_T);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dIsRSEnable( const GXOamAttr* pOam )
{
    return ( ( pOam->attr01 & GX_OAM_ATTR01_RSENABLE_MASK ) != 0 ) ? TRUE : FALSE; 
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 NNSi_G2dGetAffineIdx( const GXOamAttr* pOam )
{
    if( NNSi_G2dIsRSEnable( pOam ) )
    {
        return (u16)( ( pOam->attr01 & GX_OAM_ATTR01_RS_MASK ) >> GX_OAM_ATTR01_RS_SHIFT ); 
    }else{
        // TODO : warning
        return 0;
    }
        
}


//------------------------------------------------------------------------------
// The name should be changed
// Function that gets position info for OAMs the make up the cell
NNS_G2D_INLINE void NNS_G2dGetOamTransFx32( const GXOamAttr* pOam, NNSG2dFVec2* pRet )
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pRet );
    
    
    pRet->x = (fx32)(NNS_G2dRepeatXinCellSpace( NNSi_G2dGetOamX( pOam ) ) * FX32_ONE);
    pRet->y = (fx32)(NNS_G2dRepeatYinCellSpace( NNSi_G2dGetOamY( pOam ) ) * FX32_ONE);
}

//------------------------------------------------------------------------------
// If OBJ was set for double-size affine
// the correction value used when displaying double-size affine OBJs is removed.
//
NNS_G2D_INLINE void NNSi_G2dRemovePositionAdjustmentFromDoubleAffineOBJ
( 
    const GXOamAttr*  pOam, 
    NNSG2dFVec2*      pV
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pV );    
    
#ifdef NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED
    if( G2_GetOBJEffect( pOam ) == GX_OAM_EFFECT_AFFINE_DOUBLE  )
    {
        // - 1 means / 2
        const GXOamShape     oamShape = NNS_G2dGetOAMSize( pOam );
        const int halfW = NNS_G2dGetOamSizeX( &oamShape ) << (FX32_SHIFT - 1); 
        const int halfH = NNS_G2dGetOamSizeY( &oamShape ) << (FX32_SHIFT - 1); 
           
        // adjusting for difference of affine orientation.
        pV->x += halfW;
        pV->y += halfH;    
    }
#else // NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED

    #pragma unused( pOam )
    #pragma unused( pV )

    // Do Nothing.

#endif // NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dAdjustDifferenceOfRotateOrientation
( 
    const GXOamAttr*  pOam, 
    const MtxFx22*    pM, 
    NNSG2dFVec2*      pV, 
    BOOL              adjustDoubleAffine 
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pM );
    NNS_G2D_NULL_ASSERT( pV );
    {
        const GXOamShape     oamShape = NNS_G2dGetOAMSize( pOam );
        const int           halfW = NNS_G2dGetOamSizeX( &oamShape ) >> 1; // >> 1 == / 2
        const int           halfH = NNS_G2dGetOamSizeY( &oamShape ) >> 1; 
        
        // adjusting for difference of affine orientation.
        pV->x += -(halfW << FX32_SHIFT) +   pM->_00 * halfW + pM->_10 * halfH;
        pV->y += -(halfH << FX32_SHIFT) +   pM->_01 * halfW + pM->_11 * halfH;    
        
        // Adjusting the positon of OAM for double affine mode.
        if( adjustDoubleAffine )
        {
            pV->x -= halfW << FX32_SHIFT;
            pV->y -= halfH << FX32_SHIFT;        
        }
    }
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif // NNS_G2DI_OAMUTIL_H_

