/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2di_RendererMtxState.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_RendererMtxState.h,v $
  Revision 1.9  02/22/2005 12:51:14 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.8  02/21/2005 02:17:57 AM  hatamoto_minoru
  Response for warnings about implicit sign/unsign conversion.

  Revision 1.7  02/08/2005 06:26:39 AM  kitani_toshikazu
  Fixed the problem that executed the process in NNSi_G2dMCMStoreCurrentMtxToMtxCache() unconditionally.

  Revision 1.6  11/16/2004 08:38:39 AM  kitani_toshikazu
  Changed the matrix cache number allocation algorithm
  to reduce the amount matrix cache numbers that are used.

  Revision 1.5  11/02/2004 05:18:16 AM  kitani_toshikazu
  Changed function names.

  Revision 1.4  10/27/2004 11:58:08 PM  kitani_toshikazu
  Moved OAM_FLIP.

  Revision 1.3  10/18/2004 05:52:30 AM  kitani_toshikazu
  Revised copy. Function name revisions.

  Revision 1.2  10/15/2004 02:03:44 AM  kitani_toshikazu
  Changed function names.

  Revision 1.1  10/14/2004 10:08:28 AM  kitani_toshikazu
  Initial version registration. Split from matrix cache module.

  Revision 1.1  10/14/2004 01:01:38 AM  kitani_toshikazu
  Initial version registration. Split from Renderer.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2DI_RENDERERMTXSTATE_H_
#define NNS_G2DI_RENDERERMTXSTATE_H_

#include <nnsys/g2d/g2d_Renderer.h>

#include "g2d_Internal.h"
#include "g2di_RendererMtxStack.hpp"// For MatrixStack 
#include "g2di_RendererMtxCache.h"

//
// Matrix Cache state Manager
//
// Internal module that manages matrix stack status, and
// processes loading to the matrix stack to matrix cache.
//
// This manages matrix stack status by wrapping operations on the matrix stack.
// 
// It is used by the rendering module.
// The rendering module does not directly manipulate the matrix cache.
// It carries out all operations via this module's methods.
//
// List of operations on the matrix stack.
//      A: SR value for the current matrix is changed.
//      B: SR value for the current matrix is loaded into matrix cache.
//      C: Matrix stack is pushed.
//
// The following function calls are supported:
//
//      A: NNSi_G2dMCMSetCurrentMtxSRChanged()
//      B: NNSi_G2dMCMStoreCurrentMtxToMtxCache( )
//      C: NNSi_G2dMCMSetMtxStackPushed( u16 newPos, u16 lastPos )
//
//    
//
// In contrast, the renderer core module never references this module.
// The renderer core module only uses the matrix cache module.
//
// Function naming NNSi_G2dMCM




//------------------------------------------------------------------------------
// Type definitions. Used only inside the module.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// These express the state of a matrix in the matrix stack.
typedef enum MCMRndMtxStateType
{
    MCM_MTX_NOT_SR = 0,                        // SR not changed
    MCM_MTX_SR_NOT_CACHELOADED,                // SR changed. Load to matrix cache not completed.
    MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED,   // SR changed. Load to matrix cache not completed.
                                               // Stack being manipulated
    MCM_MTX_SR_CACHELOADED                     // SR changed. Load to matrix cache completed.

}MCMRndMtxStateType;

//------------------------------------------------------------------------------
// Expresses the state of the matrix stack.
// Acquires a stack structure that is the same size as the matrices in the matrix stack.
//
// It uses this data to control loading to matrix cache.
//
// Stores information other than the matrix itself.
typedef struct MCMMtxState
{
    u16                      mtxCacheIdx; // Matrix cache number
    u16                      groupID;     // Group IDs that reference the same matrix cache
    u16                      stateType;   // MCMRndMtxStateType
    u16                      pad16;
    
}MCMMtxState;

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 GetMtxStateMtxCacheIdx_( const MCMMtxState* pMtxState )
{
    return pMtxState->mtxCacheIdx;
}

NNS_G2D_INLINE void SetMtxStateMtxCacheIdx_( MCMMtxState* pMtxState, u16 cacheIdx )
{
    pMtxState->mtxCacheIdx = cacheIdx;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 GetMtxStateGroupID_( const MCMMtxState* pMtxState )
{
    return pMtxState->groupID;
}

NNS_G2D_INLINE void SetMtxStateGroupID_( MCMMtxState* pMtxState, u16 groupID )
{
    pMtxState->groupID = groupID;
}



//------------------------------------------------------------------------------
// Data that expresses the state of the matrix stack.
// This stack is the same size as the matrix stack.
// The current position is aligned with the matrix stack.
static MCMMtxState           mtxStateStack_[G2Di_NUM_MTX_CACHE];
static u16                   groupID_ = 0;



//------------------------------------------------------------------------------
// Functions only available internally
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Acquires the state structure of the current matrix.
NNS_G2D_INLINE MCMMtxState* GetCuurentMtxState_()
{
    return &mtxStateStack_[NNSi_G2dGetMtxStackPos()];
}

//------------------------------------------------------------------------------
// Goes through the stack's parent level
// and if a parent level's groupID is the same,
// sets it to cache load finished.
// (If the groupID is the same, it references the same matrix cache.)))
NNS_G2D_INLINE void SetParentMtxStateLoaded_( u16 mtxCacheIdx, u16 groupID )
{
    int i;
    const u16 currntStackPos    = NNSi_G2dGetMtxStackPos();
    
    for( i = currntStackPos; i >= 0; i-- )
    {
        
        if( GetMtxStateGroupID_( &mtxStateStack_[i] ) != groupID )
        {
            break;
        }else{
            // Sets to cache load finished.
            mtxStateStack_[i].stateType   = MCM_MTX_SR_CACHELOADED;
            SetMtxStateMtxCacheIdx_( &mtxStateStack_[i], mtxCacheIdx );
        }
        
        NNSI_G2D_DEBUGMSG1( "Set Loaded => mtxStateStack_[%d].mtxCacheIdx = %d\n", 
                             i, 
                             GetMtxStateMtxCacheIdx_( &mtxStateStack_[i] ) );
    }
}

//------------------------------------------------------------------------------
// Acquires a new groupID.
NNS_G2D_INLINE u16 GetNewGroupID_()
{
    return groupID_++;
}

//------------------------------------------------------------------------------
// Acquires a new groupID.
NNS_G2D_INLINE void InitGroupID_()
{
    groupID_ = 0;
}

//------------------------------------------------------------------------------
// Functions available externally.
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dMCMInitMtxCache()
{
    NNSi_G2dInitRndMtxStack();
    NNSi_RMCInitMtxCache();
    
    InitGroupID_();
    
    //
    // Initialize matrix test
    //
    MI_CpuClearFast( mtxStateStack_, sizeof( mtxStateStack_ ) );
}

//------------------------------------------------------------------------------
// Acquires a pointer to the current NNSG2dRndCore2DMtxCache.
//
NNS_G2D_INLINE NNSG2dRndCore2DMtxCache*   NNSi_G2dMCMGetCurrentMtxCache()
{
    return NNSi_RMCGetMtxCacheByIdx( GetMtxStateMtxCacheIdx_( GetCuurentMtxState_() ) );
}

//------------------------------------------------------------------------------
// Deletes the content of MtxChache.
//
// This is a process exclusive to 2D Graphics Engine rendering.
//
NNS_G2D_INLINE void NNSi_G2dMCMCleanupMtxCache()
{
    //
    // Matrix stack list
    //
    NNSi_G2dInitRndMtxStack();
    //
    // Matrix cache list
    //
    NNSi_RMCResetMtxCache();

    InitGroupID_();
    
    //
    // Initialize matrix test
    //
    MI_CpuClearFast( mtxStateStack_, sizeof( mtxStateStack_ ) );
}

//------------------------------------------------------------------------------
// Acquires whether it is necessary to load the current matrix to cache.
NNS_G2D_INLINE BOOL NNSi_G2dMCMShouldCurrentMtxBeLoadedToMtxCache( )
{
    MCMMtxState*     pCurrMtxState = GetCuurentMtxState_();
    //
    // If SR has changed, and
    // is not loaded to cache...
    //       
    return (BOOL)( pCurrMtxState->stateType == MCM_MTX_SR_NOT_CACHELOADED ||
                   pCurrMtxState->stateType == MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED );
}

//------------------------------------------------------------------------------
// the current matrix's state is set to SR changed.
NNS_G2D_INLINE void NNSi_G2dMCMSetCurrentMtxSRChanged()
{
    MCMMtxState*     pCurrMtxState = GetCuurentMtxState_();
    
    //
    // The process will change according to the state of the current matrix.
    //
    switch( pCurrMtxState->stateType )
    {
    case MCM_MTX_SR_NOT_CACHELOADED:
        //
        // If this status precedes loading, there is no need for processes such as those that update the state of the current matrix.
        // That is, if the load process immediately precedes rendering,
        // the matrix cache will not be erased no matter how many times you transform SR.
        //
        return;
    case MCM_MTX_NOT_SR:
    case MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED:
    case MCM_MTX_SR_CACHELOADED:
        // Matrix stack: Updates the state of the current matrix.
        {
            // Sets a group ID as a separate group that must
            // use a new matrix cache.
            SetMtxStateGroupID_( pCurrMtxState, GetNewGroupID_() );
            
            //
            // SR has changed, but load to cache has not finished.
            //
            pCurrMtxState->stateType = MCM_MTX_SR_NOT_CACHELOADED;
            
            NNSI_G2D_DEBUGMSG1( "currentMtxCachePos change to %d at %d\n", 
                                 pCurrMtxState->mtxCacheIdx,
                                 NNSi_G2dGetMtxStackPos() );
        }
        break;
    }
}


//------------------------------------------------------------------------------
// Performs processing that corresponds to a push operation on the current matrix.
NNS_G2D_INLINE void NNSi_G2dMCMSetMtxStackPushed( u16 newPos, u16 lastPos )
{
    
    mtxStateStack_[newPos] = mtxStateStack_[lastPos];
    //
    // If stateType was  MCM_MTX_SR_NOT_CACHELOADED,
    // changes state to MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED.
    // 
    // ( As a result SR is again transformed, and behavior will change when
    //    NNSi_G2dMCMSetCurrentMtxSRChanged() is run))
    //    
    if( mtxStateStack_[lastPos].stateType == MCM_MTX_SR_NOT_CACHELOADED )
    {
        mtxStateStack_[newPos].stateType = MCM_MTX_SR_NOT_CACHELOADED_STACKCHANGED;
    }else{
        mtxStateStack_[newPos].stateType   = mtxStateStack_[lastPos].stateType;
    }
}

//------------------------------------------------------------------------------
// Stores in MtxChache that specified the current matrix
// 
// If the OBJ that uses the flip feature needs to be affine transformed,
// create a special matrix and store it.
//
// This is a process exclusive to 2D Graphics Engine rendering.
//
static void NNSi_G2dMCMStoreCurrentMtxToMtxCache( )
{   
    // Do not do anything if not necessary
    if( NNSi_G2dMCMShouldCurrentMtxBeLoadedToMtxCache() )
    {
        MCMMtxState* pCurrentState = GetCuurentMtxState_();            
        const u16 mtxCacheIdx = NNSi_RMCUseNewMtxCache();
        const u16 groupID     = GetMtxStateGroupID_( pCurrentState );
           
        NNS_G2D_MINMAX_ASSERT( mtxCacheIdx, 0, G2Di_NUM_MTX_CACHE - 1 );
           
        //
        // Determine the cache index
        //
        SetMtxStateMtxCacheIdx_( pCurrentState, mtxCacheIdx );
           
        ////
        //// If there is a need...
        ////
        //if( mtxCacheIdx != NNS_G2D_OAM_AFFINE_IDX_NONE )
        {
           //
           // Initialize the cache
           //
           NNS_G2dInitRndCore2DMtxCache( NNSi_RMCGetMtxCacheByIdx( mtxCacheIdx ) );
                  
           //
           // Copy the matrix to the cache
           //    
           NNSi_G2dGetMtxRS( NNSi_G2dGetCurrentMtxFor2DHW(), 
                                &NNSi_G2dMCMGetCurrentMtxCache()->m22 );
               
           NNSI_G2D_DEBUGMSG1( "Mtx No %d is cache loaded To %d = %d\n", 
                                        mtxStateStackPos_, 
                                        pCurrMtxState->mtxCacheIdx );
        }
        //
        // Shares SR transformation. Updates the parent level status to loaded.
        //
        SetParentMtxStateLoaded_( mtxCacheIdx, groupID );
    }
}


#endif // NNS_G2DI_RENDERERMTXSTATE_H_

