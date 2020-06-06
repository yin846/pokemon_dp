/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2 D
  
  File:     g2d_Animation.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Animation.c,v $
  Revision 1.24  06/29/2005 08:30:39 AM  kitani_toshikazu
  Corrected comment

  Revision 1.23  06/29/2005 12:42:31 AM  kitani_toshikazu
  Added NNSi_G2dIsAnimCtrlLoopAnim() 

  Revision 1.22  06/27/2005 02:54:32 AM  kitani_toshikazu
  (none)

  Revision 1.21  06/27/2005 02:30:42 AM  kitani_toshikazu
  Fixed reset timing of currentTime in NNS_G2dResetAnimCtrlState(). 

  Revision 1.20  06/24/2005 02:17:57 AM  kitani_toshikazu
  Added support for playing animation sequences with their starting animation frame's display frame length set to 0 frames. 
  

  Revision 1.19  05/31/2005 12:06:07 AM  kitani_toshikazu
  Changed copyright year

  Revision 1.18  05/27/2005 08:48:19 AM  kitani_toshikazu
  Fixed bug that caused data with animation frames having 0 display time
  to fall into an infinite loop

  Revision 1.17  10/19/2004 11:57:53 AM  kitani_toshikazu
  Changed internal functions to inline functions

  Revision 1.16  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.15  09/30/2004 02:10:57 AM  kitani_toshikazu
  Added NNSi_G2dCallbackFuncHandling()

  Revision 1.14  08/05/2004 07:03:39 AM  kitani_toshikazu
  Fixed bug in NNS_G2dSetAnimCtrlCurrentFrame()

  Revision 1.13  07/29/2004 12:11:21 AM  kitani_toshikazu
  Changed function names.

  Revision 1.12  07/28/2004 08:12:42 AM  kitani_toshikazu
  Corrected comment

  Revision 1.11  07/24/2004 06:58:45 AM  kitani_toshikazu
  Changed function names related to animation controller.

  Revision 1.10  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.9  07/17/2004 06:56:35 AM  kitani_toshikazu
  Changed name from NNS_G2dSetAnimCtrlCurrentFrameNoReset
  to NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime.

  Revision 1.8  07/14/2004 12:25:08 PM  kitani_toshikazu
  NNS_G2dSetAnimCtrlCurrentFrameNoReset(), etc.
  Added function to support Multi cell play

  Revision 1.7  05/28/2004 10:50:16 AM  kitani_toshikazu
  Added accessor to NNSG2dAnimController

  Revision 1.6  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.5  05/11/2004 09:31:05 AM  kitani_toshikazu
  Commented out the test code inside IsReversePlayAnim_()

  Revision 1.4  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.3  04/26/2004 04:34:00 AM  kitani_toshikazu
  Process to change interface names

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_Animation.h>
#include <stdlib.h>


//------------------------------------------------------------------------------
// private funcs


//------------------------------------------------------------------------------
NNS_G2D_INLINE const NNSG2dAnimFrame* GetFrameBegin_( const NNSG2dAnimSequence* pSequence )
{    
    NNS_G2D_NULL_ASSERT( pSequence );
    return pSequence->pAnmFrameArray;
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE const NNSG2dAnimFrame* GetFrameEnd_( const NNSG2dAnimSequence* pSequence )
{    
    NNS_G2D_NULL_ASSERT( pSequence );
    return pSequence->pAnmFrameArray + ( pSequence->numFrames );
}
//------------------------------------------------------------------------------
NNS_G2D_INLINE const NNSG2dAnimFrame* GetFrameLoopBegin_( const NNSG2dAnimSequence* pSequence )
{
    NNS_G2D_NULL_ASSERT( pSequence );
    
    return pSequence->pAnmFrameArray + pSequence->loopStartFrameIdx;
}
//------------------------------------------------------------------------------
// TODO : NOT EFFICIENT !
NNS_G2D_INLINE u16 GetCurrentFrameIdx_( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return (u16)(( (u32)pAnimCtrl->pCurrent - (u32)pAnimCtrl->pAnimSequence->pAnmFrameArray ) 
                    / sizeof( NNSG2dAnimFrameData ));
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dAnimationPlayMode GetAnimationPlayMode_
( 
    const NNSG2dAnimController* pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    //
    // If setting is pAnimCtrl->overriddenPlayMode,
    // the playback mode is overridden.
    // 
    if( pAnimCtrl->overriddenPlayMode != NNS_G2D_ANIMATIONPLAYMODE_INVALID )
    {
        return pAnimCtrl->overriddenPlayMode;
    }else{
        return pAnimCtrl->pAnimSequence->playMode;
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL IsLoopAnimSequence_( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    {
    const NNSG2dAnimationPlayMode   playMode = GetAnimationPlayMode_( pAnimCtrl );
    

    return ( playMode == NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP || 
             playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP ) ? TRUE : FALSE;
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL IsReversePlayAnim_( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    {
    const NNSG2dAnimationPlayMode   playMode = GetAnimationPlayMode_( pAnimCtrl );
    

    return ( playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE || 
             playMode == NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP ) ? TRUE : FALSE;
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL IsAnimCtrlMovingForward_( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );    
    return ( pAnimCtrl->speed > 0 )^(pAnimCtrl->bReverse) ? TRUE : FALSE;
}




//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL ShouldAnmCtrlMoveNext_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pCurrent );
    
    if( pAnimCtrl->bActive && 
        (pAnimCtrl->currentTime >= FX32_ONE * (int)pAnimCtrl->pCurrent->frames) )
    {
        return TRUE;
    }
    return FALSE;
}


//------------------------------------------------------------------------------
NNS_G2D_INLINE  void CallbackFuncHandling_( const NNSG2dCallBackFunctor*  pFunctor, u16 currentFrameIdx )
{
    NNS_G2D_NULL_ASSERT( pFunctor );
    
    switch( pFunctor->type )
    {
    // call at the specified frame.
    case NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM:
        if( currentFrameIdx == pFunctor->frameIdx )
        {
            (*pFunctor->pFunc)( pFunctor->param, currentFrameIdx );
        }   
        break;
    // Called every frame.
    case NNS_G2D_ANMCALLBACKTYPE_EVER_FRM:        
        (*pFunctor->pFunc)( pFunctor->param, currentFrameIdx );
        break;
    }
}

//------------------------------------------------------------------------------
// Wrapper for external publication
void NNSi_G2dCallbackFuncHandling( const NNSG2dCallBackFunctor*  pFunctor, u16 currentFrameIdx )
{
    CallbackFuncHandling_( pFunctor, currentFrameIdx );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL IsReachStartEdge_( const NNSG2dAnimController* pAnimCtrl, const NNSG2dAnimFrame* pFrame )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pFrame );
    
    return ( pFrame <= ( GetFrameLoopBegin_( pAnimCtrl->pAnimSequence ) - 1 ) ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL IsReachEdge_( const NNSG2dAnimController* pAnimCtrl, const NNSG2dAnimFrame* pFrame )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pFrame );
    
    
    if( IsAnimCtrlMovingForward_( pAnimCtrl ) )
    {
        // tail edge of the animation sequence.
        return ( pFrame >= GetFrameEnd_( pAnimCtrl->pAnimSequence ) ) ? TRUE : FALSE;
    }else{
        // head edge of the animation sequence.
        return IsReachStartEdge_( pAnimCtrl, pFrame );
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void MoveNext_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    if( IsAnimCtrlMovingForward_( pAnimCtrl ) )
    {
        pAnimCtrl->pCurrent++;
        
    }else{
        pAnimCtrl->pCurrent--;    
    }
        
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void SequenceEdgeHandleCommon_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    //
    // Processing for call of NNS_G2D_ANMCALLBACKTYPE_LAST_FRM type callback
    // 
    // 
    if( pAnimCtrl->callbackFunctor.type == NNS_G2D_ANMCALLBACKTYPE_LAST_FRM )
    {
        NNS_G2D_NULL_ASSERT( pAnimCtrl->callbackFunctor.pFunc );
        (*pAnimCtrl->callbackFunctor.pFunc)( pAnimCtrl->callbackFunctor.param, pAnimCtrl->currentTime );
    }
    
    // 
    // Reset process
    //
    if( !IsLoopAnimSequence_( pAnimCtrl ) )
    {
        // If not looped playback, stop update 
        NNS_G2dStopAnimCtrl( pAnimCtrl );
    }else{
        NNS_G2dResetAnimCtrlState( pAnimCtrl );
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void SequenceEdgeHandleReverse_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    // Reverse the playback direction
    pAnimCtrl->bReverse = pAnimCtrl->bReverse^TRUE;
    
    //
    // Is this the animation start frame?
    // 
    if( IsReachStartEdge_( pAnimCtrl, pAnimCtrl->pCurrent ) )
    {
        SequenceEdgeHandleCommon_( pAnimCtrl );
    }
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void SequenceEdgeHandleNormal_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    SequenceEdgeHandleCommon_( pAnimCtrl );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void ValidateAnimFrame_( NNSG2dAnimController* pAnimCtrl, const NNSG2dAnimFrame** pFrame )
{
    if( *pFrame > GetFrameEnd_( pAnimCtrl->pAnimSequence ) - 1 )
    {
        *pFrame = GetFrameEnd_( pAnimCtrl->pAnimSequence ) - 1;
    }else if( *pFrame < GetFrameBegin_( pAnimCtrl->pAnimSequence ) ){
        *pFrame = GetFrameBegin_( pAnimCtrl->pAnimSequence );
    }
}

//------------------------------------------------------------------------------
static void SequenceEdgeHandle_( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    if( IsReversePlayAnim_( pAnimCtrl ) )
    {
        SequenceEdgeHandleReverse_( pAnimCtrl );
    }else{
        SequenceEdgeHandleNormal_( pAnimCtrl );
    }
    
    ValidateAnimFrame_( pAnimCtrl, &pAnimCtrl->pCurrent );
}

//------------------------------------------------------------------------------
// This is the implementation of SetAnimCtrlCurrentFrame.
// NNS_G2dSetAnimCtrlCurrentFrame()
// Called from NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime(). 
// 
//
static BOOL SetAnimCtrlCurrentFrameImpl_
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    
    
    // Valid index? 
    if( index < pAnimCtrl->pAnimSequence->numFrames )
    {
        // set
        pAnimCtrl->pCurrent = &pAnimCtrl->pAnimSequence->pAnmFrameArray[index];
        return TRUE;   
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentElement
                NNS_G2dGetAnimCtrlNextElement

  Description:  Gets the current (next) animation result. 
                Used by the user after casting the result of this function to the proper type based on the type of animation data associated with the actual NNSG2dAnimController.  
                
                
  Arguments:    pAnimCtrl:           [IN]  Actual NNSG2dAnimController 
                
  Returns:      Pointer to the current animation result
  
 *---------------------------------------------------------------------------*/
void* NNS_G2dGetAnimCtrlCurrentElement( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pCurrent );
    NNS_G2D_NULL_ASSERT( (void*)pAnimCtrl->pCurrent->pContent );
    
    return (void*)pAnimCtrl->pCurrent->pContent;
}
//------------------------------------------------------------------------------
void* NNS_G2dGetAnimCtrlNextElement( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    {
        const NNSG2dAnimFrame*      pNext = NULL;
        
        if( IsAnimCtrlMovingForward_( pAnimCtrl ) )
        {
            pNext = pAnimCtrl->pCurrent + 1;
        }else{
            pNext = pAnimCtrl->pCurrent - 1;
        }
        
        ValidateAnimFrame_( pAnimCtrl, &pNext );
        NNS_G2D_NULL_ASSERT( pNext->pContent );
        
        return (void*)pNext->pContent;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlNormalizedTime

  Description:  Gets the time displayed for the animation frame currently being displayed displayed as an fx32 type,
                where 0.0 represents the display start time and 1.0 represents the display end time.  
                
  Arguments:    pAnimCtrl:          [IN]  Actual NNSG2dAnimController 
                
  Returns:      Animation frame display time 
  
 *---------------------------------------------------------------------------*/
fx32 NNS_G2dGetAnimCtrlNormalizedTime(  NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pCurrent );
    
    return FX_Div( pAnimCtrl->currentTime, FX32_ONE * pAnimCtrl->pCurrent->frames );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickAnimCtrl

  Description:  Advances the time for NNSG2dAnimController
                
  Arguments:    pAnimCtrl:          [OUT]  NNSG2dAnimController instance
                frames:             [IN] The time to advance (in frames)
                
                
  Returns:      Whether or not the playback track has changed
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dTickAnimCtrl( NNSG2dAnimController* pAnimCtrl, fx32 frames ) 
{
    BOOL    bChangeFrame = FALSE;
    
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pCurrent );
    NNS_G2D_ASSERTMSG( frames >=  0, "frames must be Greater than zero");
    
    
    if( pAnimCtrl->bActive != TRUE )
    {
        return FALSE;
    } 
       
    //
    // update current time.
    //
    pAnimCtrl->currentTime += abs( FX_Mul( pAnimCtrl->speed, frames ) );
    
    
    //    
    // do actual works for changing current animation frame.
    //
    while( ShouldAnmCtrlMoveNext_( pAnimCtrl ) )
    {
        bChangeFrame = TRUE; // Frame has changed
        
        pAnimCtrl->currentTime -= FX32_ONE * (int)pAnimCtrl->pCurrent->frames;    
        MoveNext_( pAnimCtrl );
        
        // if we reach the edge of animation, we have to reset status.
        if( IsReachEdge_( pAnimCtrl, pAnimCtrl->pCurrent ) )
        {
            SequenceEdgeHandle_( pAnimCtrl );
        }
        
        // call the callback function.
        if( pAnimCtrl->callbackFunctor.type != NNS_G2D_ANMCALLBACKTYPE_NONE )
        {
            CallbackFuncHandling_( &pAnimCtrl->callbackFunctor, GetCurrentFrameIdx_( pAnimCtrl ) );
        }    
    }    
    return bChangeFrame;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCurrentFrame

  Description:  Sets the animation controller's playback animation frame
                
  Arguments:    pAnimCtrl:        [OUT]  NNSG2dAnimController instance
                index    :        [IN]  Animation sequence number 
                        
  Returns:      TRUE if the change has succeeded
  
 *---------------------------------------------------------------------------*/
BOOL           NNS_G2dSetAnimCtrlCurrentFrame
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    {
        const BOOL result = SetAnimCtrlCurrentFrameImpl_( pAnimCtrl, index );
        
        if( result )
        {
            // Reset the current animation frame display time
            pAnimCtrl->currentTime   = 0;
        }
        return result;
    }       
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime

  Description:  Sets the animation controller's playback animation frame
                Does not reset the display time counter of the current frame in the animation controller when the frame changes.  
                
                
                Accordingly, even if an animation frame is switched in an interval that is less than the display time for one animation frame, the animation sequence can be advanced.  
                
                

                
  Arguments:    pAnimCtrl:           [OUT]  Actual NNSG2dAnimController 
                index    :           [IN]  Animation sequence number 
                        
  Returns:      TRUE if the change has succeeded
  
 *---------------------------------------------------------------------------*/
BOOL           NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    //
    // Do not reset the current animation frame display time
    //
    return SetAnimCtrlCurrentFrameImpl_( pAnimCtrl, index );
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentFrame

  Description:  Gets the current animation frame number in NNSG2dAnimController. 
                
  Arguments:    pAnimCtrl:          [OUT]  NNSG2dAnimController instance
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dGetAnimCtrlCurrentFrame
(
    const NNSG2dAnimController*     pAnimCtrl
)
{
    return GetCurrentFrameIdx_( pAnimCtrl );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitAnimCtrl

  Description:  Initializes NNSG2dAnimController
                
  Arguments:    pAnimCtrl:          [OUT]  NNSG2dAnimController instance
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitAnimCtrl( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    NNS_G2dInitAnimCallBackFunctor( &pAnimCtrl->callbackFunctor );
    
    pAnimCtrl->pCurrent              = NULL;
    
    pAnimCtrl->bReverse              = FALSE;
    pAnimCtrl->bActive               = TRUE;
    
    pAnimCtrl->currentTime           = 0;
    pAnimCtrl->speed                 = FX32_ONE;
    
    pAnimCtrl->overriddenPlayMode    = NNS_G2D_ANIMATIONPLAYMODE_INVALID;
    pAnimCtrl->pAnimSequence         = NULL;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitAnimCtrlCallBackFunctor

  Description:  Initializes callbacks inside NNSG2dAnimController
                
  Arguments:    pAnimCtrl:          [OUT]  NNSG2dAnimController instance
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitAnimCtrlCallBackFunctor( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    NNS_G2dInitAnimCallBackFunctor( &pAnimCtrl->callbackFunctor );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitAnimCallBackFunctor

  Description:  Initializes callback functors
                
  Arguments:    pCallBack:          [OUT] NNSG2dCallBackFunctor instance
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitAnimCallBackFunctor( NNSG2dCallBackFunctor* pCallBack )
{
    NNS_G2D_NULL_ASSERT( pCallBack );
    
    pCallBack->type     = NNS_G2D_ANMCALLBACKTYPE_NONE;
    pCallBack->param    = 0x0;
    pCallBack->pFunc    = NULL;  
    pCallBack->frameIdx = 0;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetAnimCtrlState

  Description:  Resets the state of NNSG2dAnimController
                
  Arguments:    pAnimCtrl:          [OUT]  NNSG2dAnimController instance
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetAnimCtrlState( NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    
    if( IsAnimCtrlMovingForward_( pAnimCtrl ) )
    {
        pAnimCtrl->pCurrent      = GetFrameLoopBegin_( pAnimCtrl->pAnimSequence );
    }else{
        pAnimCtrl->pCurrent      = GetFrameEnd_( pAnimCtrl->pAnimSequence ) - 1;
    }
    
    //
    // Resets currentTime. 
    //
    pAnimCtrl->currentTime   = 0;
    
    //
    // Advances the zero frame counter. 
    // An animation frame with a display frame length of 0 exists at the beginning. 
    // Routine to support animation sequence. 
    //
    (void)NNS_G2dTickAnimCtrl( pAnimCtrl, 0 );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBindAnimCtrl

  Description:  Associates animation data with the NNSG2dAnimController instance
                
  Arguments:    pAnimCtrl:           [OUT]  NNSG2dAnimController instance
                pAnimSequence:       [IN]   Animation data
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dBindAnimCtrl( NNSG2dAnimController* pAnimCtrl, const NNSG2dAnimSequence* pAnimSequence )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimSequence );
    
    pAnimCtrl->pAnimSequence = pAnimSequence;
    
    NNS_G2dResetAnimCtrlState( pAnimCtrl );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCallBackFunctor

  Description:  Sets a callback.
                To set the type of callback that is called at a specified frame,
                use NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame()
                
  Arguments:    pAnimCtrl:         [OUT]  NNSG2dAnimController instance
                type    :              [IN] Callback type
                param:             [IN]  User-defined information
                pFunc:             [IN] callback function pointer
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetAnimCtrlCallBackFunctor
( 
    NNSG2dAnimController*   pAnimCtrl, 
    NNSG2dAnmCallbackType   type, 
    u32                     param, 
    NNSG2dAnmCallBackPtr    pFunc 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( (void*)pFunc );
    NNS_G2D_MINMAX_ASSERT( type, NNS_G2D_ANMCALLBACKTYPE_NONE, AnmCallbackType_MAX );
    NNS_G2D_ASSERTMSG( type != NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM, 
        "Use NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame() instead." );
        
    pAnimCtrl->callbackFunctor.pFunc     = pFunc;
    pAnimCtrl->callbackFunctor.param     = param;
    pAnimCtrl->callbackFunctor.type      = type;
    pAnimCtrl->callbackFunctor.frameIdx  = 0; // meanless for these kind of type.
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame

  Description:  Sets a callback.
                To set the type of callback that is called at a specified frame,
                use NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame()
                
  Arguments:    pAnimCtrl:         [OUT]  Actual NNSG2dAnimController 
                param:             [IN]  User-defined information
                pFunc:             [IN] callback function pointer
                frameIdx:          [IN] Animation frame number that invokes callback
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame
( 
    NNSG2dAnimController*   pAnimCtrl, 
    u32                     param, 
    NNSG2dAnmCallBackPtr    pFunc, 
    u16                     frameIdx 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( (void*)pFunc );
    
    pAnimCtrl->callbackFunctor.type  = NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM;
    
    pAnimCtrl->callbackFunctor.pFunc     = pFunc;
    pAnimCtrl->callbackFunctor.param     = param;
    pAnimCtrl->callbackFunctor.frameIdx  = frameIdx;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dIsAnimCtrlLoopAnim

  Description:  Checks if the animation controller is playing a loop animation. 
                
                
                [Library internal public function]
               
  Arguments:    pAnimCtrl:          [IN]  Actual NNSG2dAnimController 
                
  Returns:      If playing a loop animation, returns TRUE. 
  
 *---------------------------------------------------------------------------*/
BOOL NNSi_G2dIsAnimCtrlLoopAnim( const NNSG2dAnimController* pAnimCtrl )
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_NULL_ASSERT( pAnimCtrl->pAnimSequence );
    
    return IsLoopAnimSequence_( pAnimCtrl );
}

