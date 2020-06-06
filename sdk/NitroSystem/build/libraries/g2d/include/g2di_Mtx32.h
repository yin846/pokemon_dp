/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     .c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_Mtx32.h,v $
  Revision 1.7  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.6  06/22/2004 09:06:45 AM  kitani_toshikazu
  Invalidated the debug output at the time of FINALROM build

  Revision 1.5  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.4  05/10/2004 07:25:18 AM  kitani_toshikazu
  fix some bugs.

  Revision 1.3  04/21/2004 12:28:58 PM  kitani_toshikazu
  Changed the scope of NNSi_G2dMtxConcat32 NNSi_G2dDumpMtx32

  Revision 1.2  04/21/2004 11:49:04 AM  kitani_toshikazu
  Renamed functions

  Revision 1.1  04/21/2004 07:20:08 AM  kitani_toshikazu
  Updated the folder structures related to the internal module

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//
// 
//


//
// This module should be exposed inside the library only.
//

//
// Mtx32 is referenced by renderer module.
//

#ifndef NNS_G2DI_MTX32_H_
#define NNS_G2DI_MTX32_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>

//------------------------------------------------------------------------------

static void           NNSi_G2dSetTranslate( MtxFx32 *m, fx32 x, fx32 y );
static void           NNSi_G2dSetRotate   ( MtxFx32 *m, fx32 sin, fx32 cos );
static void           NNSi_G2dSetScale    ( MtxFx32 *m, fx32 sx, fx32 sy );
static void           NNSi_G2dGetMtxTrans ( const MtxFx32 *m, NNSG2dFVec2 *pRet );
static void           NNSi_G2dGetMtxRS    ( const MtxFx32 *m, MtxFx22* pRet );


void                  NNSi_G2dMtxConcat32  ( const MtxFx32 *a, 
                                             const MtxFx32 *b, 
                                             MtxFx32 *ab 
                                            );

#ifdef SDK_FINALROM                                            
inline void                  NNSi_G2dDumpMtx32     ( const MtxFx32* ){}
#else  // SDK_FINALROM
void                  NNSi_G2dDumpMtx32     ( const MtxFx32 *m );
#endif // SDK_FINALROM

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetTranslate( MtxFx32 *m, fx32 x, fx32 y )
{
    NNS_G2D_NULL_ASSERT( m );
    
    m->_00 = FX32_ONE;
    m->_01 = 0;
    m->_10 = 0;
    m->_11 = FX32_ONE;
    m->_20 = x;
    m->_21 = y;
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetRotate( MtxFx32 *m, fx32 sin, fx32 cos )
{
    NNS_G2D_NULL_ASSERT( m );
    
    m->_00 = cos;
    m->_01 = sin;
    m->_10 = -sin;
    m->_11 = cos;
    m->_20 = 0;
    m->_21 = 0;
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetScale( MtxFx32 *m, fx32 sx, fx32 sy )
{
    NNS_G2D_NULL_ASSERT( m );
    
    m->_00 = sx;
    m->_01 = 0;
    m->_10 = 0;
    m->_11 = sy;
    m->_20 = 0;
    m->_21 = 0;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetScaleFor2DHW( MtxFx32 *m, fx32 sx, fx32 sy )
{
    NNS_G2D_NULL_ASSERT( m );
    
    m->_00 = FX_Inv( sx );
    m->_01 = 0;
    m->_10 = 0;
    m->_11 = FX_Inv( sy );
    m->_20 = 0;
    m->_21 = 0;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dGetMtxTrans( const MtxFx32 *m, NNSG2dFVec2 *pRet )
{
    NNS_G2D_NULL_ASSERT( pRet );
    NNS_G2D_NULL_ASSERT( m );
    
    pRet->x = m->_20;
    pRet->y = m->_21;
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dGetMtxRS( const MtxFx32 *m, MtxFx22* pRet )
{
    NNS_G2D_NULL_ASSERT( pRet );
    NNS_G2D_NULL_ASSERT( m );
    
    pRet->_00 = m->_00;
    pRet->_01 = m->_01;
    pRet->_10 = m->_10;
    pRet->_11 = m->_11;
}

//------------------------------------------------------------------------------
// FIX ME !
// This function should be located in global utility module or SDK.
// To be implented in the SDK
// This is to be deleted as soon as the recommended implementation is completed.
NNS_G2D_INLINE void MulMtx22( const MtxFx22* m, const NNSG2dFVec2* pSrc, NNSG2dFVec2* pDst )
{
    NNSG2dFVec2    temp;
    NNSG2dFVec2*   pV = pDst;
    if( pDst == pSrc )
    {
        pV = &temp;
    }
    
    //pV->x = FX_Mul( m->_00, pSrc->x ) + FX_Mul( m->_10, pSrc->y );
    //pV->y = FX_Mul( m->_01, pSrc->x ) + FX_Mul( m->_11, pSrc->y );
    
    pV->x = (fx32)( (((s64) (m->_00) * pSrc->x + 0x800LL) 
                   + ((s64) (m->_10) * pSrc->y + 0x800LL)) >> FX32_SHIFT );
    pV->y = (fx32)( (((s64) (m->_01) * pSrc->x + 0x800LL) 
                   + ((s64) (m->_11) * pSrc->y + 0x800LL)) >> FX32_SHIFT );
    
    if( pV == &temp )
    {
        *pDst = temp;
    }
}



#endif // NNS_G2DI_MTX32_H_

