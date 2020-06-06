/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2di_RendererMtxCache.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_RendererMtxCache.h,v $
  Revision 1.10  2005/02/23 12:02:42 AM  takano_makoto
  fix copyright header.

  Revision 1.9  02/07/2005 10:01:30 AM  kitani_toshikazu
  Fixed the problem in the matrix cache number operation section.

  Revision 1.8  11/16/2004 08:35:32 AM  kitani_toshikazu
  Corrected bug in NNSi_RMCUseNewMtxCache(). 
  (Matrix cache 0 was not used.)

  Revision 1.7  11/02/2004 05:18:16 AM  kitani_toshikazu
  Changed function names.

  Revision 1.6  10/27/2004 11:58:40 PM  kitani_toshikazu
  Moved OAM_FLIP.

  Revision 1.5  10/19/2004 11:58:55 AM  kitani_toshikazu
  Optimization process when affine is not used

  Revision 1.4  10/18/2004 05:52:30 AM  kitani_toshikazu
  Revised copy. Function name revisions.

  Revision 1.3  10/15/2004 02:04:16 AM  kitani_toshikazu
  Changed function names.

  Revision 1.2  10/14/2004 10:07:26 AM  kitani_toshikazu
  Supported splitting up of matrix cache module.

  Revision 1.1  10/14/2004 01:01:38 AM  kitani_toshikazu
  Initial version registration. Split from Renderer.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2DI_RENDERERMTXCACHE_H_
#define NNS_G2DI_RENDERERMTXCACHE_H_

#include <nnsys/g2d/g2d_RendererCore.h> // For NNSG2dRndCore2DMtxCache, NNSG2dSurfaceType
#include "g2d_Internal.h"
#include "g2di_RendererMtxStack.hpp"// For MatrixStack 


//------------------------------------------------------------------------------
typedef enum OAM_FLIP
{
    OAM_FLIP_NONE   = 0,
    OAM_FLIP_H      = 0x01,
    OAM_FLIP_V      = 0x02,
    OAM_FLIP_HV     = 0x03,
    OAM_FLIP_MAX    = NNS_G2D_OAMFLIP_PATTERN_NUM

}OAM_FLIP;

#define OAM_FLIP_ASSERT( val )                                       \
        NNS_G2D_MINMAX_ASSERT( (val), OAM_FLIP_NONE, OAM_FLIP_HV )    \

// convert from flags to OAM_FLIP
#define TO_OAM_FLIP( bFlipH, bFlipV )           (OAM_FLIP)(( bFlipV << 1 ) | ( bFlipH ))


//------------------------------------------------------------------------------

#define G2Di_NUM_MTX_CACHE                      32



//------------------------------------------------------------------------------
// Renderer 2DMatrix Cache
// Carries out role of caching actual affine parameter registration to 2D graphics  engine.
// Duplicate registrations are not executed, and the result of the past   registration is returned.
// 
// If the same NNSG2dRndCore2DMtxCache Index is specified by this,
// the same affine parameters are referenced.
//
// The affineIndex has MtxCache_NOT_AVAILABLE, which means the affine parameter
// registration was not done with the standard values.
// 
// Reset to the default value is performed by NNSi_G2dMCMCleanupMtxCache(), which is called  by NNS_G2dEndRendering().
// In other words, affine parameters can only be shared in identical 
// NNS_G2dBeginRendering() NNS_G2dEndRendering() blocks.
//
// 
// This module is not directly handled from the render module, 
// but handled via the RendererMtxState module socket. 
//
//
// Function names NNSi_RMC.....()          
static NNSG2dRndCore2DMtxCache             mtxCacheBuffer_[G2Di_NUM_MTX_CACHE];

//------------------------------------------------------------------------------
// Introduced to hide NNSG2dRndCore2DMtxCache from the user.
static u16                          currentMtxCachePos_ = 0;



//------------------------------------------------------------------------------
// Functions restricted to inside the module.
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Module's externally public functions
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_RMCInitMtxCache()
{
    int i;
    for( i = 0; i < G2Di_NUM_MTX_CACHE; i++ )
    {
        NNS_G2dInitRndCore2DMtxCache( &mtxCacheBuffer_[i] );
    }
    currentMtxCachePos_ = 0;
}

NNS_G2D_INLINE void NNSi_RMCResetMtxCache()
{
    int i;
    //
    // Initialize up to location used
    //
    for( i = 0; i < currentMtxCachePos_; i++ )
    {
        NNS_G2dInitRndCore2DMtxCache( &mtxCacheBuffer_[i] );
    }
    currentMtxCachePos_ = 0;
}

//------------------------------------------------------------------------------
// Acquire matrix cache with index.
NNS_G2D_INLINE NNSG2dRndCore2DMtxCache* NNSi_RMCGetMtxCacheByIdx( u16 idx )
{
    NNS_G2D_MINMAX_ASSERT( idx, 0, G2Di_NUM_MTX_CACHE - 1);
    return &mtxCacheBuffer_[idx];
}             

//------------------------------------------------------------------------------
// Use a new matrix cache
NNS_G2D_INLINE u16 NNSi_RMCUseNewMtxCache()
{
    const u16 ret = currentMtxCachePos_;
    
    if( currentMtxCachePos_ < G2Di_NUM_MTX_CACHE - 1)
    {
       currentMtxCachePos_++;
    }else{
       // Matrix cache is used up
       NNS_G2D_WARNING( FALSE, "MtxCache is running out. G2d ignores the user request"
                               ", and uses MtxCache-Idx = 31.");
    }
    
    return ret;
}

#endif // NNS_G2DI_RENDERERMTXCACHE_H_

