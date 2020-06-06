/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     kernel_inline.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: kernel_inline.h,v $
  Revision 1.12  10/20/2004 01:30:50  nishida_kenji
  Add accessors for NNSG3dRenderObj.

  Revision 1.11  10/18/2004 10:54:54  nishida_kenji
  small fix.

  Revision 1.10  10/18/2004 09:00:12  nishida_kenji
  small fix.

  Revision 1.9  10/18/2004 08:12:50  nishida_kenji
  Add NNS_G3dAnmObjEnableID and NNS_G3dAnmObjDisableID.

  Revision 1.8  10/13/2004 09:14:40  nishida_kenji
  small fix(const correctness).

  Revision 1.7  08/03/2004 04:26:38  nishida_kenji
  Add NNS_G3dAnmObjGetNumFrame.

  Revision 1.6  07/21/2004 02:17:43  nishida_kenji
  add APIs for RenderObj's flag

  Revision 1.5  07/20/2004 10:45:02  nishida_kenji
  add APIs and comments.

  Revision 1.4  07/12/2004 12:50:59  nishida_kenji
  refactoring

  Revision 1.3  07/12/2004 02:25:41  nishida_kenji
  use NNS_G3D_INLINE

  Revision 1.2  07/08/2004 12:46:41  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:39:44  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_KERNEL_INLINE_H_
#define NNSG3D_KERNEL_INLINE_H_
//
// DO NOT INCLUDE THIS FILE DIRECTLY
//

#include <nnsys/g3d/config.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// inline functions for NNSG3dAnmObj
//

/*---------------------------------------------------------------------------
    NNS_G3dAnmObjSetFrame

    This function configures the play frame of the animation object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dAnmObjSetFrame(NNSG3dAnmObj* pAnmObj, fx32 frame)
{
    NNS_G3D_NULL_ASSERT(pAnmObj);

    pAnmObj->frame = frame;
}


/*---------------------------------------------------------------------------
    NNS_G3dAnmObjSetBlendRatio

    Configures the blending ratio (from 0 to FX32_ONE) in the animation object.
    The animation blend function references this.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dAnmObjSetBlendRatio(NNSG3dAnmObj* pAnmObj, fx32 ratio)
{
    NNS_G3D_NULL_ASSERT(pAnmObj);

    pAnmObj->ratio = ratio;
}


/*---------------------------------------------------------------------------
    NNS_G3dAnmObjGetNumFrame

    Gets the number of animation frames
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE fx32
NNS_G3dAnmObjGetNumFrame(const NNSG3dAnmObj* pAnmObj)
{
    const NNSG3dResAnmCommon* p;
    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_ASSERT(NNSi_G3dIsValidAnmRes(pAnmObj->resAnm));

    p = (const NNSG3dResAnmCommon*) pAnmObj->resAnm;
    return p->numFrame * FX32_ONE;
}





//
// inline functions for NNSG3dRenderObj
//

/*---------------------------------------------------------------------------
    NNS_G3dRenderObjSetFlag

    Sets what was designated by the execution control flag of the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRenderObjSetFlag(NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);

    pRenderObj->flag |= flag;
}


/*---------------------------------------------------------------------------
    NNS_G3dRenderObjResetFlag

    Resets what was designated by the execution control flag of the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRenderObjResetFlag(NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);

    pRenderObj->flag &= ~flag;
}


/*---------------------------------------------------------------------------
    NNS_G3dRenderObjTestFlag

    Tests whether or not the execution control flag of the rendering object has been set.
    Returns TRUE if the designated flags are all set.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE BOOL
NNS_G3dRenderObjTestFlag(NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);

    return (pRenderObj->flag & flag) == flag;
}


/*---------------------------------------------------------------------------
    NNS_G3dRenderObjSetUserSbc

    Configures the user-defined SBC in the rendering object.
    The return value is the user-defined SBC that had been configured up to then.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE u8*
NNS_G3dRenderObjSetUserSbc(NNSG3dRenderObj* pRenderObj, u8* sbc)
{
    u8* rval;
    NNS_G3D_NULL_ASSERT(pRenderObj);
    // sbc can be NULL

    rval = pRenderObj->ptrUserSbc;
    pRenderObj->ptrUserSbc = sbc;
    return rval;
}


/*---------------------------------------------------------------------------
    NNS_G3dRenderObjSetJntAnmBuffer

    Configures the buffer that registers and plays the calculation result of the joint
    in the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRenderObjSetJntAnmBuffer(NNSG3dRenderObj* pRenderObj,
                                struct NNSG3dJntAnmResult_* buf)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(buf);

//    pRenderObj->flag |= NNS_G3D_RENDEROBJ_FLAG_RECORD;
    pRenderObj->recJntAnm = buf;
}


/*---------------------------------------------------------------------------
    NNS_G3dRenderObjSetMatAnmBuffer

    Configures the buffer that registers and plays the calculation result of the material
    in the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRenderObjSetMatAnmBuffer(NNSG3dRenderObj* pRenderObj,
                                struct NNSG3dMatAnmResult_* buf)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(buf);

//    pRenderObj->flag |= NNS_G3D_RENDEROBJ_FLAG_RECORD;
    pRenderObj->recMatAnm = buf;
}


/*---------------------------------------------------------------------------
    NNS_G3dRenderObjReleaseJntAnmBuffer

    Extracts the buffer that registers and plays the calculation result of the joint
    out of the rendering object. The return value is the pointer to the extracted buffer.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE struct NNSG3dJntAnmResult_*
NNS_G3dRenderObjReleaseJntAnmBuffer(NNSG3dRenderObj* pRenderObj)
{
    struct NNSG3dJntAnmResult_* rval;
    NNS_G3D_NULL_ASSERT(pRenderObj);

    // Reset the RECORD flag when both buffers disappear
    if (!pRenderObj->recMatAnm)
        pRenderObj->flag &= ~NNS_G3D_RENDEROBJ_FLAG_RECORD;
    rval = pRenderObj->recJntAnm;
    pRenderObj->recJntAnm = NULL;
    return rval;
}


/*---------------------------------------------------------------------------
    NNS_G3dRenderObjReleaseMatAnmBuffer

    Extracts the buffer that registers and plays the calculation result of the material
    out of the rendering object. The return value is the pointer to the extracted buffer.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE struct NNSG3dMatAnmResult_*
NNS_G3dRenderObjReleaseMatAnmBuffer(NNSG3dRenderObj* pRenderObj)
{
    struct NNSG3dMatAnmResult_* rval;
    NNS_G3D_NULL_ASSERT(pRenderObj);

    // Reset the RECORD flag when both buffers disappear
    if (!pRenderObj->recJntAnm)
        pRenderObj->flag &= ~NNS_G3D_RENDEROBJ_FLAG_RECORD;
    rval = pRenderObj->recMatAnm;
    pRenderObj->recMatAnm = NULL;
    return rval;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjSetUserPtr

    Sets the pointer to the region that the user can use with a callback
    in the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void*
NNS_G3dRenderObjSetUserPtr(NNSG3dRenderObj* pRenderObj, void* ptr)
{
    void* rval = pRenderObj->ptrUser;
    pRenderObj->ptrUser = ptr;
    return rval;
}


/*---------------------------------------------------------------------------*
    NNSG3dRenderObjGetResMdl

    Returns a pointer to the model resource retained by the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE NNSG3dResMdl*
NNS_G3dRenderObjGetResMdl(NNSG3dRenderObj* pRenderObj)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    return pRenderObj->resMdl;
}


/*---------------------------------------------------------------------------*
    NNSG3dRenderObjSetBlendFuncMat

    Sets the (independent) material blend function in the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRenderObjSetBlendFuncMat(NNSG3dRenderObj* pRenderObj,
                                NNSG3dFuncAnmBlendMat func)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(func);

    pRenderObj->funcBlendMat = func;
}


/*---------------------------------------------------------------------------*
    NNSG3dRenderObjSetBlendFuncJnt

    Sets the (independent) joint blend function in the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRenderObjSetBlendFuncJnt(NNSG3dRenderObj* pRenderObj,
                                NNSG3dFuncAnmBlendJnt func)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(func);

    pRenderObj->funcBlendJnt = func;
}


/*---------------------------------------------------------------------------*
    NNSG3dRenderObjSetBlendFuncVis

    Sets the (independent) visibility blend function in the rendering object.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dRenderObjSetBlendFuncVis(NNSG3dRenderObj* pRenderObj,
                                NNSG3dFuncAnmBlendVis func)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(func);

    pRenderObj->funcBlendVis = func;
}



//
// Misc inline functions
//




#ifdef __cplusplus
}
#endif

#endif

