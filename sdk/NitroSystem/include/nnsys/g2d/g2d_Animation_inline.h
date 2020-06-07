/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_Animation_inline.h

  Copyright 2004, 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Animation_inline.h,v $
  Revision 1.6  07/19/2005 12:25:10 AM  kitani_toshikazu
  Revised copyright notation

  Revision 1.5  07/07/2005 06:58:25 AM  kitani_toshikazu
  NNS_G2dGetAnimCtrlCurrentElemIdxVal()
  Added NNS_G2dGetAnimCtrlCurrentAnimSequence()

  Revision 1.4  10/19/2004 01:41:29 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.3  07/29/2004 12:34:20 AM  kitani_toshikazu
  Changed function names.

  Revision 1.2  07/24/2004 06:58:58 AM  kitani_toshikazu
  Changed function names related to animation controller.

  Revision 1.1  07/23/2004 02:47:17 AM  kitani_toshikazu
  Separated inline function definitions.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_ANIMATION_INLINE_H_
#define NNS_G2D_ANIMATION_INLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlType

  Description:  Gets animation controller animation types.
                
  Arguments:    pAnimCtrl:            animation controller
                
                
                
  Returns:      controller type
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE NNSG2dAnimationType NNS_G2dGetAnimCtrlType
( 
    const NNSG2dAnimController*  pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    if( pAnimCtrl->pAnimSequence != NULL )
    {
        return NNS_G2dGetAnimSequenceAnimType( pAnimCtrl->pAnimSequence );
    }else{
        return NNS_G2D_ANIMATIONTYPE_MAX;
    }   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlSpeed

  Description:  Sets animation speed of animation controller.
                
  Arguments:    pAnimCtrl:            animation controller
                speed       :           speed
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetAnimCtrlSpeed
( 
    NNSG2dAnimController*       pAnimCtrl, 
    fx32                        speed 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->speed = speed;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlSpeed

  Description:  Gets animation speed of animation controller.
                
  Arguments:    pAnimCtrl:            animation controller
                
                
                
  Returns:      animation speed
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE fx32 NNS_G2dGetAnimCtrlSpeed
( 
    const NNSG2dAnimController*       pAnimCtrl
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return pAnimCtrl->speed;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dStartAnimCtrl

  Description:  Starts animation play of animation controller.
                
  Arguments:    pAnimCtrl:            animation controller
                
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dStartAnimCtrl
( 
    NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->bActive = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dStopAnimCtrl

  Description:  Ends animation play of animation controller.
                
  Arguments:    pAnimCtrl:            animation controller
                
                
                
  Returns:      controller type
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dStopAnimCtrl
( 
    NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->bActive = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsAnimCtrlActive

  Description:  Checks whether the animation controller is playing animation.
                
  Arguments:    pAnimCtrl:            animation controller
                
                
                
  Returns:      TRUE if animation is playing
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE BOOL NNS_G2dIsAnimCtrlActive
( 
    const NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return pAnimCtrl->bActive;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlPlayModeOverridden

  Description:  Overrides the playback animation sequence playback method specified in the file data. 
                
                
  Arguments:    pAnimCtrl:            animation controller
                playMode :            animation play mode
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetAnimCtrlPlayModeOverridden
( 
    NNSG2dAnimController*       pAnimCtrl,
    NNSG2dAnimationPlayMode     playMode 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    NNS_G2D_MINMAX_ASSERT( playMode, NNS_G2D_ANIMATIONPLAYMODE_FORWARD,
                                 NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP );
                                 
    pAnimCtrl->overriddenPlayMode = playMode;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetAnimCtrlPlayModeOverridden

  Description:  Resets overridden animation play mode.
                (Returns to play mode specified by file information.)
                
  Arguments:    pAnimCtrl:            animation controller
                
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dResetAnimCtrlPlayModeOverridden
( 
    NNSG2dAnimController*       pAnimCtrl
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->overriddenPlayMode = NNS_G2D_ANIMATIONPLAYMODE_INVALID;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentTime

  Description:  Gets the current animation frame display time of the animation controller.
                
  Arguments:    pAnimCtrl:            animation controller
                
                
                
  Returns:      current animation frame display time
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE fx32 NNS_G2dGetAnimCtrlCurrentTime
( 
    const NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return pAnimCtrl->currentTime;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimCtrlCurrentTime

  Description:  Sets the current animation frame display time of the animation controller.
                
  Arguments:    pAnimCtrl:            animation controller
                time     :           current animation frame display time
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void NNS_G2dSetAnimCtrlCurrentTime
( 
    NNSG2dAnimController*       pAnimCtrl,
    fx32                        time
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    pAnimCtrl->currentTime = time;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentElemIdxVal

  Description:  Gets the index value from the current animation result of the Animation controller. 
                Gets the index value from the current animation result. 
                
  Arguments:    pAnimCtrl:            animation controller
                
                
  Returns:      Index value from the current animation result
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE u16 NNS_G2dGetAnimCtrlCurrentElemIdxVal
( 
    const NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    //
    // Regardless of the animation type, the first two bytes must be used as the index value. 
    // 
    //
    {
        const NNSG2dAnimData* pAnmRes 
           = (const NNSG2dAnimData*)pAnimCtrl->pCurrent->pContent;
    
        return (*pAnmRes);        
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetAnimCtrlCurrentAnimSequence

  Description:  Gets the index value from the current animation result of the Animation controller. 
                Gets the current animation sequence.
                
  Arguments:    pAnimCtrl:            animation controller
                
                
  Returns:      Current animation sequence
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE const NNSG2dAnimSequence* 
NNS_G2dGetAnimCtrlCurrentAnimSequence
( 
    const NNSG2dAnimController*       pAnimCtrl 
)
{
    NNS_G2D_NULL_ASSERT( pAnimCtrl );
    return pAnimCtrl->pAnimSequence;
}


#ifdef __cplusplus
} /* extern "C"*/
#endif


#endif // NNS_G2D_ANIMATION_INLINE_H_


