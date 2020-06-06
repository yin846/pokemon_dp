/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     .c

  Copyright 2004, 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_RendererMtxStack.hpp,v $
  Revision 1.12  06/21/2005 09:05:33 AM  kitani_toshikazu
  Revised copyright notation

  Revision 1.11  06/21/2005 08:55:10 AM  kitani_toshikazu
  Internal values that control affine transform state were not initialized correctly. This bug was fixed.  

  Revision 1.10  11/16/2004 08:33:43 AM  kitani_toshikazu
  Added NNSi_G2dInitRndMtxStack()

  Revision 1.9  10/27/2004 11:58:58 PM  kitani_toshikazu
  (none)

  Revision 1.8  10/19/2004 11:58:55 AM  kitani_toshikazu
  Optimization process when affine is not used

  Revision 1.7  10/18/2004 09:47:54 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.6  10/15/2004 02:04:01 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.5  10/14/2004 10:06:14 AM  kitani_toshikazu
  Introduced flag for branching to process when affine transformation not being used.

  Revision 1.4  10/14/2004 01:02:00 AM  kitani_toshikazu
  Cleaned up the code

  Revision 1.3  10/07/2004 12:46:59 PM  kitani_toshikazu
  Added Z parameter to NNSi_G2dTranslate()

  Revision 1.2  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.1  04/21/2004 11:49:04 AM  kitani_toshikazu
  Renamed functions

  Revision 1.1  04/21/2004 07:20:08 AM  kitani_toshikazu
  Updated the folder structures related to the internal module

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

//
// This module implements MtrixStack used inside g2d_renderer.c
// This should rightfully be described in the same file, but it is separated out for maintainability of code.
// 
//


//
// This module should be exposed inside the library only.
//

//
// Mtx32 is referenced by renderer module.
//

#ifndef NNS_G2DI_RENDERERMTXSTACK_HPP_
#define NNS_G2DI_RENDERERMTXSTACK_HPP_

#include <nitro.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>
#include "g2di_Mtx32.h"



#define NNS_G2D_MAXLENGTH_OF_2DMTXSTACK         32
#define G2Di_RENDERER_MTX_SR_NONE               NNS_G2D_OAM_AFFINE_IDX_NONE

static MtxFx32        mtxStack_        [NNS_G2D_MAXLENGTH_OF_2DMTXSTACK];
static MtxFx32        mtxStackFor2DHW_ [NNS_G2D_MAXLENGTH_OF_2DMTXSTACK];
static fx32           fxZStack_[NNS_G2D_MAXLENGTH_OF_2DMTXSTACK];
static s32            stackPos_ = 0;

static u16            firstSRTransformedMtxIdx_   = G2Di_RENDERER_MTX_SR_NONE;


//------------------------------------------------------------------------------
// Is SR transformation going to be used?
// If not, make the process more efficient.
// 
// If SR transformation is not being used, addition and subtraction is  sufficient for parallel translation transform.
// Also, you do not need to worry about 2DHW matrix stacks.
//(If bDonotUseSRTransform == TRUE then mtxStackFor2DHW_ is not updated)
//
static BOOL           bDonotUseSRTransform = FALSE;
static MtxFx32        mtxI_                = { FX32_ONE, 0, 0, FX32_ONE, 0, 0 };
//------------------------------------------------------------------------------





//------------------------------------------------------------------------------
static void           NNSi_G2dInitRndMtxStack();
static BOOL           NNSi_G2dIsStackEmpty();
static BOOL           NNSi_G2dStackHasEnoughCapacity();

static void           NNSi_G2dMtxPush     ();
static void           NNSi_G2dMtxPop      ();
static void           NNSi_G2dTranslate   ( fx32 x, fx32 y, fx32 z );
static void           NNSi_G2dSetTrans    ( fx32 x, fx32 y, fx32 z );
static void           NNSi_G2dRotate      ( fx32 sin, fx32 cos );
static void           NNSi_G2dScale       ( fx32 sx, fx32 sy );
static void           NNSi_G2dIdentity    ( );

static const MtxFx32* NNSi_G2dGetCurrentMtx();
static u16            NNSi_G2dGetMtxStackPos();

static void           NNSi_G2dSetRndMtxStackSRTransformed();
static BOOL           NNSi_G2dIsRndCurrentMtxSRTransformed();

// Inside the module
static void RestoreRndMtxStackStateNotSR_();// Internal
static void CheckIfRndCurrentMtxIsSRTransformed_();

//------------------------------------------------------------------------------
// Set the Renderer's internal state to the non-SR affine transformed state.
// Called by CheckIfRndCurrentMtxIsSRTransformed_().
// 
NNS_G2D_INLINE void RestoreRndMtxStackStateNotSR_()
{
    firstSRTransformedMtxIdx_ = G2Di_RENDERER_MTX_SR_NONE;
}

//------------------------------------------------------------------------------
// Initialization of stack
NNS_G2D_INLINE void NNSi_G2dInitRndMtxStack( )
{
    stackPos_                 = 0;
    RestoreRndMtxStackStateNotSR_();
}

//------------------------------------------------------------------------------
// Is SR transformation going to be used?
// If not, make the process more efficient.
// 
NNS_G2D_INLINE void NNSi_G2dSetRndMtxStackSRTransformEnableFlag( BOOL bEnable )
{
    bDonotUseSRTransform = !bEnable;
}



//------------------------------------------------------------------------------
// Set the Renderer's internal state to the SR affine transformation  state.
// Called from NNS_G2dScale() NNS_G2dRotZ().
//
// Illegal to call when in SR affine transformed state
//
NNS_G2D_INLINE void NNSi_G2dSetRndMtxStackSRTransformed()
{
    NNS_G2D_ASSERTMSG( firstSRTransformedMtxIdx_ ==  G2Di_RENDERER_MTX_SR_NONE,
                    "firstSRTransformedMtxIdx_ must be set only in the first time of using SR transform." );
    
    NNSI_G2D_DEBUGMSG1( "Affine SR == ON : in stack_position %d \n", 
                         NNSi_G2dGetMtxStackPos() );
                         
    
    firstSRTransformedMtxIdx_ = NNSi_G2dGetMtxStackPos();
}

//------------------------------------------------------------------------------
// Check if current matrix is SR transformed
// If the matrix is SR transformed, then OBJ must be subjected to affine  transformation (2D Graphics Engine)
//
//
NNS_G2D_INLINE BOOL NNSi_G2dIsRndCurrentMtxSRTransformed()
{
    return (BOOL)( !bDonotUseSRTransform &&
                   firstSRTransformedMtxIdx_ != G2Di_RENDERER_MTX_SR_NONE );
}

//------------------------------------------------------------------------------
// 
// Called inside NNS_G2dPopMtx().
// Check if current matrix after NNS_G2dPopMtx() is SR affine transformed.
// If it is determined that it is not SR affine transformed, update the state inside the Renderer.
//
NNS_G2D_INLINE void CheckIfRndCurrentMtxIsSRTransformed_()
{
    if( NNSi_G2dIsRndCurrentMtxSRTransformed() )
    {
        //
        // Get the current position of the matrix stack
        // If the current position was initially ahead of the SR-transformed
        // position, current matrix after NNS_G2dPopMtx() is treated as if it has not been SR transformed.
        // 
        const u16 currentIdx = NNSi_G2dGetMtxStackPos();
        {
            if( firstSRTransformedMtxIdx_ > currentIdx )
            {
                NNSI_G2D_DEBUGMSG1( "Affine SR == OFF : in stack_position %d \n",
                                     currentIdx );
                                     
                RestoreRndMtxStackStateNotSR_();
            }
        }
    }
}


//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dIsStackEmpty()
{
    // return ( (u32)pMtxCurr <=  (u32)&mtxStack_[0] ) ? TRUE : FALSE;
    return (BOOL)(stackPos_ <= 0);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dStackHasEnoughCapacity()
{
    // return ( (u32)(pMtxCurr + 1) < (u32)&mtxStack_[NNS_G2D_MAXLENGTH_OF_2DMTXSTACK] ) ? TRUE : FALSE;
    return (BOOL)(stackPos_ + 1 < NNS_G2D_MAXLENGTH_OF_2DMTXSTACK);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dMtxPush()
{
    if( NNSi_G2dStackHasEnoughCapacity() )
    {
        // copy current.
        const int nextPos_ = stackPos_ + 1;
        
        if( bDonotUseSRTransform )
        {
            
            mtxStack_[nextPos_]._20 = mtxStack_[stackPos_]._20;
            mtxStack_[nextPos_]._21 = mtxStack_[stackPos_]._21;
            fxZStack_[nextPos_]     = fxZStack_[stackPos_];
            
        }else{
            
            mtxStack_[nextPos_]         = mtxStack_[stackPos_];
            mtxStackFor2DHW_[nextPos_]  = mtxStackFor2DHW_[stackPos_];
            fxZStack_[nextPos_]         = fxZStack_[stackPos_];
            
        }
        stackPos_ = nextPos_;
    }else{
        OS_Warning( "The mtxStack overflow in G2D library." );
    }
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dMtxPop()
{
    if( !NNSi_G2dIsStackEmpty() )
    {
        stackPos_--;
        //
        // Get the current position of the matrix stack
        // If the current position was initially ahead of the SR-transformed
        // position, current matrix after NNS_G2dPopMtx() is treated as if it has not been SR transformed.
        // 
        {
            if( firstSRTransformedMtxIdx_ > stackPos_ )
            {
                NNSI_G2D_DEBUGMSG1( "Affine SR == OFF : in stack_position %d \n",
                                     stackPos_ );
                                     
                RestoreRndMtxStackStateNotSR_();
            }
        }
    }else{
        OS_Warning( "The mtxStack is empty, so we can't pop it any more" );
    } 
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dTranslate( fx32 x, fx32 y, fx32 z )
{
    if( bDonotUseSRTransform )
    {
        mtxStack_[stackPos_]._20 += x;
        mtxStack_[stackPos_]._21 += y;
        fxZStack_[stackPos_] += z;
    }else{
        MtxFx32 temp;
 
        NNSi_G2dSetTranslate( &temp, x, y );
        NNSi_G2dMtxConcat32( &temp, &mtxStack_[stackPos_], &mtxStack_[stackPos_] );
        NNSi_G2dMtxConcat32( &temp, &mtxStackFor2DHW_[stackPos_], &mtxStackFor2DHW_[stackPos_] );
        
        fxZStack_[stackPos_] += z;
    }
    //NNSi_G2dDumpMtx32( pMtxCurr );
}

//------------------------------------------------------------------------------
// Configure the parallel translation
// You can avoid the use of push/pop
NNS_G2D_INLINE void NNSi_G2dSetTrans( fx32 x, fx32 y, fx32 z )
{
    mtxStack_[stackPos_]._20 = x;
    mtxStack_[stackPos_]._21 = y;
    if( !bDonotUseSRTransform )
    {
        mtxStackFor2DHW_[stackPos_]._20 = x;
        mtxStackFor2DHW_[stackPos_]._21 = y;
    }
    
    fxZStack_[stackPos_] = z;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dRotate( fx32 sin, fx32 cos )
{
    NNS_G2D_ASSERTMSG( !bDonotUseSRTransform,
        "Renderer module is the -- DonotUseSRTransform -- Mode.You can't use SR Transfomation.");
        
    {
        MtxFx32 temp;
        NNSi_G2dSetRotate( &temp, sin, cos );
      
        NNSi_G2dMtxConcat32( &temp, &mtxStack_[stackPos_], &mtxStack_[stackPos_] );
        NNSi_G2dMtxConcat32( &temp, &mtxStackFor2DHW_[stackPos_], &mtxStackFor2DHW_[stackPos_] );
    }
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dScale( fx32 sx, fx32 sy )
{
    NNS_G2D_ASSERTMSG( !bDonotUseSRTransform,
        "Renderer module is the -- DonotUseSRTransform -- Mode.You can't use SR Transfomation.");
        
    {
        MtxFx32 temp;
        NNSi_G2dSetScale( &temp, sx, sy );
        NNSi_G2dMtxConcat32( &temp, &mtxStack_[stackPos_], &mtxStack_[stackPos_] );
        NNSi_G2dSetScaleFor2DHW( &temp, sx, sy );
        NNSi_G2dMtxConcat32( &temp, &mtxStackFor2DHW_[stackPos_], &mtxStackFor2DHW_[stackPos_] );
    }
}

//------------------------------------------------------------------------------
static void NNSi_G2dIdentity( )
{
    
    if( bDonotUseSRTransform )
    {
        mtxStack_[stackPos_]._20 = 0;
        mtxStack_[stackPos_]._21 = 0;
        
    }else{
        MtxFx32* pMtxCurr = &mtxStack_[stackPos_];
        
        pMtxCurr->_00 = FX32_ONE;
        pMtxCurr->_01 = 0;
        pMtxCurr->_10 = 0;
        pMtxCurr->_11 = FX32_ONE;
        pMtxCurr->_20 = 0;
        pMtxCurr->_21 = 0;
        
        mtxStackFor2DHW_[stackPos_] = *pMtxCurr;
    }
    
    fxZStack_[stackPos_] = 0;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE const MtxFx32* NNSi_G2dGetCurrentMtx()
{
    if( bDonotUseSRTransform )
    {
        mtxI_._20 = mtxStack_[stackPos_]._20;
        mtxI_._21 = mtxStack_[stackPos_]._21;
        
        return &mtxI_;
        
    }else{
        return &mtxStack_[stackPos_];
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE const MtxFx32* NNSi_G2dGetCurrentMtxFor2DHW()
{
    if( bDonotUseSRTransform )
    {
        mtxI_._20 = mtxStack_[stackPos_]._20;
        mtxI_._21 = mtxStack_[stackPos_]._21;
        
        return &mtxI_;
        
    }else{
        return &mtxStackFor2DHW_[stackPos_];
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE fx32 NNSi_G2dGetCurrentZ()
{
    return fxZStack_[stackPos_];
}

//------------------------------------------------------------------------------
// Get the current MtxStack position
NNS_G2D_INLINE u16 NNSi_G2dGetMtxStackPos( )
{
    // return (u16)(( (u32)pMtxCurr - (u32)&mtxStack_[0] ) / sizeof( MtxFx32 ) );
    return (u16)( stackPos_ );
}







#endif // NNS_G2DI_RENDERERMTXSTACK_HPP_

