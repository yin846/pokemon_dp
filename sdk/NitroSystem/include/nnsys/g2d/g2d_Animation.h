/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_Animation.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Animation.h,v $
  Revision 1.20  07/07/2005 06:57:11 AM  kitani_toshikazu
  NNS_G2dGetAnimCtrlCurrentElemIdxVal()
  Added NNS_G2dGetAnimCtrlCurrentAnimSequence()

  Revision 1.19  06/29/2005 12:44:11 AM  kitani_toshikazu
  Added NNSi_G2dIsAnimCtrlLoopAnim() 

  Revision 1.18  11/04/2004 10:13:19 AM  kitani_toshikazu
  (none)

  Revision 1.17  09/30/2004 02:11:04 AM  kitani_toshikazu
  Added NNSi_G2dCallbackFuncHandling()

  Revision 1.16  07/29/2004 12:34:20 AM  kitani_toshikazu
  Changed function names.

  Revision 1.15  07/28/2004 08:13:01 AM  kitani_toshikazu
  Corrected comment

  Revision 1.14  07/28/2004 12:11:37 AM  kitani_toshikazu
  Correspondence with the padding warning

  Revision 1.13  07/27/2004 04:29:31 AM  kitani_toshikazu
  Added static keyword to inline function declaration.

  Revision 1.12  07/24/2004 06:58:58 AM  kitani_toshikazu
  Changed function names related to animation controller.

  Revision 1.11  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.10  07/17/2004 06:52:19 AM  kitani_toshikazu
  Changed name from NNS_G2dSetAnimCtrlCurrentFrameNoReset
  to NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime.

  Revision 1.9  07/14/2004 12:28:38 PM  kitani_toshikazu
  Supported for play method control data.

  Revision 1.8  06/22/2004 06:31:54 AM  kitani_toshikazu
  Made accessor inline.

  Revision 1.7  06/01/2004 05:20:33 AM  kitani_toshikazu
  Changes for using expansion palette.

  Revision 1.6  05/28/2004 10:50:52 AM  kitani_toshikazu
  Added accessor to NNSG2dAnimController

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
 
#ifndef NNS_G2D_ANIMATION_H_
#define NNS_G2D_ANIMATION_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

#ifdef __cplusplus
extern "C" {
#endif


//
// Aliases of functions with names changed
// Previous functions declared as aliases to preserve compatibility.
// 
#define NNS_G2dGetCurrentElement               NNS_G2dGetAnimCtrlCurrentElement
#define NNS_G2dGetNextElement                  NNS_G2dGetAnimCtrlNextElement
#define NNS_G2dGetNormalizedTime               NNS_G2dGetAnimCtrlNormalizedTime
#define NNS_G2dSetCallBackFunctor              NNS_G2dSetAnimCtrlCallBackFunctor
#define NNS_G2dSetCallBackFunctorAtAnimFrame   NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame
#define NNS_G2dGetAnimSpeed                    NNS_G2dGetAnimCtrlSpeed
#define NNS_G2dSetAnimSpeed                    NNS_G2dSetAnimCtrlSpeed
#define NNS_G2dResetAnimationState             NNS_G2dResetAnimCtrlState
#define NNS_G2dInitCallBackFunctor             NNS_G2dInitAnimCallBackFunctor

#define NNS_G2dBindAnimController 						NNS_G2dBindAnimCtrl
#define NNS_G2dGetAnimControllerType 					NNS_G2dGetAnimCtrlType
#define NNS_G2dInitAnimController 						NNS_G2dInitAnimCtrl
#define NNS_G2dInitAnimControllerCallBackFunctor 	NNS_G2dInitAnimCtrlCallBackFunctor
#define NNS_G2dIsAnimControllerActive 					NNS_G2dIsAnimCtrlActive
#define NNS_G2dStartAnimController 					NNS_G2dStartAnimCtrl
#define NNS_G2dStopAnimController 						NNS_G2dStopAnimCtrl
#define NNS_G2dTickAnimController 						NNS_G2dTickAnimCtrl



#define NNS_G2D_ASSERT_ANIMATIONTYPE_VALID( val )   \
        NNS_G2D_MINMAX_ASSERT( val, NNS_G2D_ANIMATIONTYPE_CELL, NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION )


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimFrame

  Description:  Pair of pointer to animation instance and number of display frames.
                Minimum unit of animation.
                Multiple frame columns form NNSG2dAnimSequence.
                
 *---------------------------------------------------------------------------*/
typedef NNSG2dAnimFrameData NNSG2dAnimFrame;


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimSequence

  Description:  Represents a series of ordered animation data.
                It has multiple frame columns.
                All NNSG2dAnimFrame in the sequence have same NNSG2dAnimationType.
                
 *---------------------------------------------------------------------------*/
typedef NNSG2dAnimSequenceData NNSG2dAnimSequence;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnmCallBackPtr

  Description:  Animation Callback
                
 *---------------------------------------------------------------------------*/
// data = NNSG2dAnimCallBackFunctor.param
typedef void (*NNSG2dAnmCallBackPtr)( u32 data, fx32 currentFrame );


/*---------------------------------------------------------------------------*
  
  Name:         NNSG2dAnmCallbackType

  Description:  Animation Callback Type
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dAnmCallbackType
{    
    NNS_G2D_ANMCALLBACKTYPE_NONE = 0,  
    NNS_G2D_ANMCALLBACKTYPE_LAST_FRM,  // Call when last frame of animation sequence finishes.
    NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM,  // Call during playback of specified frame.
    NNS_G2D_ANMCALLBACKTYPE_EVER_FRM,  // Call every frame.
    AnmCallbackType_MAX

}NNSG2dAnmCallbackType;

/*---------------------------------------------------------------------------*
  
  Name:         NNSG2dAnimCallBackFunctor

  Description:  Groups function pointers for animation callback
                and user-defined u32 data to be passed as an argument during call.
                Maintained in NNSG2dAnimController.
                // Alias NNSG2dAnimCallBackFunctor was added.
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCallBackFunctor
{   
    NNSG2dAnmCallbackType          type;            // callback type
    u32                            param;           // parameters that can be used by the user
    NNSG2dAnmCallBackPtr           pFunc;           // callback function pointer
    u16                            frameIdx;        // frame number (used when type == NNS_G2D_ANMCALLBACKTYPE_SPEC_FRM)
    u16                            pad16_;          // The padding
    
}NNSG2dCallBackFunctor, NNSG2dAnimCallBackFunctor;



/*---------------------------------------------------------------------------*
  
  Name:         NNSG2dAnimController

  Description:  Structure for maintaining animation status
                Maintains animation data
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimController
{
    const NNSG2dAnimFrame*      pCurrent;               // current animation frame
    
    BOOL                        bReverse;               // reverse flag
    BOOL                        bActive;                // active flag

    fx32                        currentTime;            // current time
    fx32                        speed;                  // speed
    
    NNSG2dAnimationPlayMode     overriddenPlayMode;     // Set when the programmer overrides play mode.
                                                        // control value: NNS_G2D_ANIMATIONPLAYMODE_INVALID
                                                        
    const NNSG2dAnimSequence*   pAnimSequence;          // associated animation sequence
    NNSG2dAnimCallBackFunctor   callbackFunctor;        // callback functor
    
}NNSG2dAnimController;


//------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------

void NNSi_G2dCallbackFuncHandling
( 
    const NNSG2dCallBackFunctor*  pFunctor, 
    u16 currentFrameIdx 
);


BOOL           NNS_G2dTickAnimCtrl
( 
    NNSG2dAnimController*     pAnimCtrl, 
    fx32                      frames 
);

BOOL           NNS_G2dSetAnimCtrlCurrentFrame
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
);

BOOL           NNS_G2dSetAnimCtrlCurrentFrameNoResetCurrentTime
( 
    NNSG2dAnimController*     pAnimCtrl, 
    u16                       index
);

u16 NNS_G2dGetAnimCtrlCurrentFrame
(
    const NNSG2dAnimController*     pAnimCtrl
);

void            NNS_G2dInitAnimCtrl
( 
    NNSG2dAnimController*     pAnimCtrl 
);

void            NNS_G2dInitAnimCtrlCallBackFunctor
( 
    NNSG2dAnimController*     pAnimCtrl 
);

void            NNS_G2dInitAnimCallBackFunctor
( 
    NNSG2dAnimCallBackFunctor*   pCallBack 
);

void            NNS_G2dResetAnimCtrlState
( 
    NNSG2dAnimController*     pAnimCtrl
);

void            NNS_G2dBindAnimCtrl
( 
    NNSG2dAnimController*        pAnimCtrl, 
    const NNSG2dAnimSequence*    pAnimSequence 
);

void* NNS_G2dGetAnimCtrlCurrentElement  ( NNSG2dAnimController* pAnimCtrl );
void* NNS_G2dGetAnimCtrlNextElement     ( NNSG2dAnimController* pAnimCtrl );// testing
fx32  NNS_G2dGetAnimCtrlNormalizedTime  ( NNSG2dAnimController* pAnimCtrl );// testing

void NNS_G2dSetAnimCtrlCallBackFunctor
( 
    NNSG2dAnimController*   pAnimCtrl, 
    NNSG2dAnmCallbackType   type, 
    u32                     param, 
    NNSG2dAnmCallBackPtr    pFunc 
);

void NNS_G2dSetAnimCtrlCallBackFunctorAtAnimFrame
( 
    NNSG2dAnimController*   pAnimCtrl, 
    u32                     param, 
    NNSG2dAnmCallBackPtr    pFunc, 
    u16                     frameIdx 
);

//------------------------------------------------------------------------------
// [Library internal limited public function]
BOOL NNSi_G2dIsAnimCtrlLoopAnim( const NNSG2dAnimController* pAnimCtrl );

//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------
static NNSG2dAnimationType NNS_G2dGetAnimCtrlType
( 
    const NNSG2dAnimController*  pAnimCtrl 
);

static void NNS_G2dSetAnimCtrlSpeed
( 
    NNSG2dAnimController*       pAnimCtrl, 
    fx32                        speed 
);

static fx32 NNS_G2dGetAnimCtrlSpeed
( 
    const NNSG2dAnimController*       pAnimCtrl
);

static void NNS_G2dStartAnimCtrl
( 
    NNSG2dAnimController*       pAnimCtrl 
);

static void NNS_G2dStopAnimCtrl
( 
    NNSG2dAnimController*       pAnimCtrl 
);

static BOOL NNS_G2dIsAnimCtrlActive
( 
    const NNSG2dAnimController*       pAnimCtrl 
);

static void NNS_G2dSetAnimCtrlPlayModeOverridden
( 
    NNSG2dAnimController*       pAnimCtrl,
    NNSG2dAnimationPlayMode     playMode 
);

static void NNS_G2dResetAnimCtrlPlayModeOverridden
( 
    NNSG2dAnimController*       pAnimCtrl
);

static fx32 NNS_G2dGetAnimCtrlCurrentTime
( 
    const NNSG2dAnimController*       pAnimCtrl 
);

static void NNS_G2dSetAnimCtrlCurrentTime
( 
    NNSG2dAnimController*       pAnimCtrl,
    fx32                        time
);

static u16 NNS_G2dGetAnimCtrlCurrentElemIdxVal
( 
    const NNSG2dAnimController*       pAnimCtrl 
);

static const NNSG2dAnimSequence* 
NNS_G2dGetAnimCtrlCurrentAnimSequence
( 
    const NNSG2dAnimController*       pAnimCtrl 
);



#ifdef __cplusplus
} /* extern "C"*/
#endif

#include <nnsys/g2d/g2d_Animation_inline.h>
#endif // NNS_G2D_ANIMATION_H_


