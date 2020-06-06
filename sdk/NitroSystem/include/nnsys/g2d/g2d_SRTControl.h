/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_SRTControl.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_SRTControl.h,v $
  Revision 1.6  2005/02/23 00:05:24  takano_makoto
  fix copyright header.

  Revision 1.5  02/07/2005 05:39:30 AM  kitani_toshikazu
  Fixed ASSERT statement.

  Revision 1.4  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.3  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:54 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_SRTCONTROL_H_
#define NNS_G2D_SRTCONTROL_H_


#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_SRTControl_data.h>

#ifdef __cplusplus
extern "C" {
#endif




void NNSi_G2dSrtcSetTrans   ( NNSG2dSRTControl* pCtrl, s16 x, s16 y );
void NNSi_G2dSrtcSetSRTRotZ ( NNSG2dSRTControl* pCtrl, u16 rotZ );
void NNSi_G2dSrtcSetSRTScale( NNSG2dSRTControl* pCtrl, fx32 x, fx32 y );

void NNSi_G2dSrtcSetMatrix2D( NNSG2dSRTControl* pCtrl, fx32 x, fx32 y );
void NNSi_G2dSrtcSetMatrix3D( NNSG2dSRTControl* pCtrl, fx32 x, fx32 y );


void NNSi_G2dSrtcInitControl        ( NNSG2dSRTControl* pCtrl, NNSG2dSRTControlType type );
void NNSi_G2dSrtcSetInitialValue    ( NNSG2dSRTControl* pCtrl );
void NNSi_G2dSrtcBuildMatrixFromSRT_2D( const NNSG2dSRTControl* pCtrl , MtxFx22* pDst );
void NNSi_G2dSrtcBuildMatrixFromSRT_3D( const NNSG2dSRTControl* pCtrl , MtxFx22* pDst );


// inline functions
//------------------------------------------------------------------------------


NNS_G2D_INLINE void  NNSi_G2dSrtcResetAffineFlag( NNSG2dSRTControl* pSRT )
{
    NNS_G2D_NULL_ASSERT( pSRT );
    NNS_G2D_ASSERT( pSRT->type == NNS_G2D_SRTCONTROLTYPE_SRT );
    pSRT->srtData.SRT_EnableFlag = 0x0000;
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void  NNSi_G2dSrtcAffineFlagON( NNSG2dSRTControl* pSRT, u16 newFlag )
{
    NNS_G2D_NULL_ASSERT( pSRT );
    NNS_G2D_ASSERT( pSRT->type == NNS_G2D_SRTCONTROLTYPE_SRT );
    pSRT->srtData.SRT_EnableFlag |= (u16)newFlag;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void  NNSi_G2dSrtcAffineFlagOFF( NNSG2dSRTControl* pSRT, u16 newFlag )
{
    NNS_G2D_NULL_ASSERT( pSRT );
    NNS_G2D_ASSERT( pSRT->type == NNS_G2D_SRTCONTROLTYPE_SRT );
    pSRT->srtData.SRT_EnableFlag &= ~(u16)newFlag;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL  NNSi_G2dSrtcIsAffineEnable( const NNSG2dSRTControl* pSRT, NNSG2dAffineEnable flag )
{
    NNS_G2D_NULL_ASSERT( pSRT );
    NNS_G2D_ASSERT( pSRT->type == NNS_G2D_SRTCONTROLTYPE_SRT );
    return ( pSRT->srtData.SRT_EnableFlag & (u16)flag ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL  NNSi_G2dSrtcIsAffineEnable_SR( const NNSG2dSRTControl* pSRT )
{
    NNS_G2D_NULL_ASSERT( pSRT );
    NNS_G2D_ASSERT( pSRT->type == NNS_G2D_SRTCONTROLTYPE_SRT );
    
    return ( NNSi_G2dSrtcIsAffineEnable( pSRT, NNS_G2D_AFFINEENABLE_SCALE ) || 
             NNSi_G2dSrtcIsAffineEnable( pSRT, NNS_G2D_AFFINEENABLE_ROTATE ) ) ? TRUE : FALSE;
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_SRTCONTROL_H_

