/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     anm.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: anm.c,v $
  Revision 1.20  10/18/2004 08:13:22 AM  nishida_kenji
  Keep up with NNS_G3dAnmObjEnableID and NNS_G3dAnmObjDisableID.

  Revision 1.19  10/07/2004 02:10:43 AM  nishida_kenji
  Add comments.

  Revision 1.18  09/13/2004 11:56:02 AM  nishida_kenji
  NNSi_G3dAnmBlendJnt and blendScaleVec_ to be ARM binary.

  Revision 1.17  09/10/2004 09:06:35 AM  nishida_kenji
  small fix

  Revision 1.16  09/10/2004 01:43:39 AM  nishida_kenji
  improve precision of motion blend.

  Revision 1.15  08/04/2004 09:01:00 AM  nishida_kenji
  bugfix(NNSi_G3dAnmBlendJnt)

  Revision 1.14  08/04/2004 07:54:07 AM  nishida_kenji
  small fix

  Revision 1.13  08/04/2004 07:50:06 AM  nishida_kenji
  bugfix(NNSi_G3dAnmBlendJnt)

  Revision 1.12  07/27/2004 01:47:12 PM  nishida_kenji
  remove an obsolete function.

  Revision 1.11  07/23/2004 09:34:59 AM  nishida_kenji
  fix NNSi_G3dAnmBlendVis

  Revision 1.10  07/20/2004 01:31:27 AM  nishida_kenji
  (none)

  Revision 1.9  07/19/2004 10:56:44 AM  nishida_kenji
  change arguments of animation calc functions

  Revision 1.8  07/17/2004 01:00:02 PM  nishida_kenji
  NNS_G3dAnmUseDefaultFunc is to be removed

  Revision 1.1  07/07/2004 12:41:43 PM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/kernel.h>

#include <nnsys/g3d/anm/nsbca.h>
#include <nnsys/g3d/anm/nsbma.h>
#include <nnsys/g3d/anm/nsbta.h>
#include <nnsys/g3d/anm/nsbtp.h>
#include <nnsys/g3d/anm/nsbva.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>

//
// Pointer to default animation blend function (Global variables)
// Function pointer that is set when initializing AnmObj and RenderObj
//
NNSG3dFuncAnmBlendMat NNS_G3dFuncBlendMatDefault = &NNSi_G3dAnmBlendMat;
NNSG3dFuncAnmBlendJnt NNS_G3dFuncBlendJntDefault = &NNSi_G3dAnmBlendJnt;
NNSG3dFuncAnmBlendVis NNS_G3dFuncBlendVisDefault = &NNSi_G3dAnmBlendVis;

//
// Default handler of .nsbma format (material color animation)
//
NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBmaDefault =
#ifndef NNS_G3D_NSBMA_DISABLE
    &NNSi_G3dAnmCalcNsBma;
#else
    NULL;
#endif


//
// Default handler of .nsbtp format (texture pattern animation)
//
NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBtpDefault =
#ifndef NNS_G3D_NSBTP_DISABLE
    &NNSi_G3dAnmCalcNsBtp;
#else
    NULL;
#endif


//
// Default handler of .nsbta format (texture SRT animation)
//
NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBtaDefault =
#ifndef NNS_G3D_NSBTA_DISABLE
    &NNSi_G3dAnmCalcNsBta;
#else
    NULL;
#endif


//
// Default handler of .nsbca format (joint animation)
//
NNSG3dFuncAnmJnt NNS_G3dFuncAnmJntNsBcaDefault =
#ifndef NNS_G3D_NSBCA_DISABLE
    &NNSi_G3dAnmCalcNsBca;
#else
    NULL;
#endif


//
// Default handler of .nsbva format (visibility animation)
//
NNSG3dFuncAnmVis NNS_G3dFuncAnmVisNsBvaDefault =
#ifndef NNS_G3D_NSBVA_DISABLE
    &NNSi_G3dAnmCalcNsBva;
#else
    NULL;
#endif


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmBlendMat

    Default material animation blend function
    Each animation will write data to pResult in order.
 *---------------------------------------------------------------------------*/
BOOL NNSi_G3dAnmBlendMat(NNSG3dMatAnmResult* pResult,
                         const NNSG3dAnmObj* pAnmObj,
                         u32 matID)
{
    BOOL rval = FALSE;
    const NNSG3dAnmObj* p;
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);

    p = pAnmObj;
    do
    {
        // Convert from matID to resource ID
        u32 dataIdx = p->mapData[matID];
        if (dataIdx & NNS_G3D_ANMOBJ_MAPDATA_EXIST)
        {
            NNSG3dFuncAnmMat func = (NNSG3dFuncAnmMat)p->funcAnm;
            NNS_G3D_NULL_ASSERT(func);
            (*func)(pResult,
                    p,
                    dataIdx & NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
            rval = TRUE;
        }
        p = p->next;
    }
    while(p);

    return rval;
}

#include <nitro/code32.h>
static void
blendScaleVec_(VecFx32* v0, const VecFx32* v1, fx32 ratio, BOOL isV1One)
{
    if (isV1One)
    {
        v0->x += ratio;
        v0->y += ratio;
        v0->z += ratio;
    }
    else
    {
        v0->x += ratio * v1->x >> FX32_SHIFT;
        v0->y += ratio * v1->y >> FX32_SHIFT;
        v0->z += ratio * v1->z >> FX32_SHIFT;
    }
}
#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmBlendJnt

    Default joint animation blend function
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
BOOL NNSi_G3dAnmBlendJnt(NNSG3dJntAnmResult* pResult,
                         const NNSG3dAnmObj* pAnmObj,
                         u32 nodeID)
{
    NNS_G3D_NULL_ASSERT(pResult && pAnmObj);

    if (!pAnmObj->next)
    {
        //
        // When only one joint animation exists
        //
        u32 dataIdx = pAnmObj->mapData[nodeID];
        if ((dataIdx & (NNS_G3D_ANMOBJ_MAPDATA_EXIST |
                        NNS_G3D_ANMOBJ_MAPDATA_DISABLED)) ==
                NNS_G3D_ANMOBJ_MAPDATA_EXIST)
        {
            NNSG3dFuncAnmJnt func = (NNSG3dFuncAnmJnt)pAnmObj->funcAnm;
            NNS_G3D_NULL_ASSERT(func);

            (*func)(pResult,
                    pAnmObj,
                    dataIdx & NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
            return TRUE;
        }
        else
        {
            //
            // Since there is no animation for the nodeID, use the model data JointSRT
            //
            return FALSE;
        }
    }
    else
    {
        //
        // When multiple joint animations exist
        //
        const NNSG3dAnmObj* p;
        NNSG3dJntAnmResult r;
        fx32 sumOfRatio = 0;
        const NNSG3dAnmObj* pLastAnmObj;
        int numBlend = 0;

        p = pAnmObj;
        do
        {
            //
            // Calculate the total ratio of each animation:
            // Ratio of each animation must be equal or larger than 0 and less than FX32_ONE
            //
            u32 dataIdx = p->mapData[nodeID];
            if ((dataIdx & (NNS_G3D_ANMOBJ_MAPDATA_EXIST |
                            NNS_G3D_ANMOBJ_MAPDATA_DISABLED)) ==
                    NNS_G3D_ANMOBJ_MAPDATA_EXIST)
            {
                NNS_G3D_ASSERT(p->ratio >= 0 && p->ratio <= FX32_ONE);
                sumOfRatio += p->ratio;

                pLastAnmObj = p;
                ++numBlend;
            }
            p = p->next;
        }
        while(p);

        if (sumOfRatio == 0)
        {
            //
            // Is there any animation for the nodeID?
            // Since ratios are all zero, use the model data JointSRT.
            //
            return FALSE;
        }

        if (numBlend == 1)
        {
            // Actually, only one animation is ON
            NNSG3dFuncAnmJnt func = (NNSG3dFuncAnmJnt)pLastAnmObj->funcAnm;
            u32 dataIdx = pLastAnmObj->mapData[nodeID];
            NNS_G3D_NULL_ASSERT(func);

            (*func)(pResult,
                    pLastAnmObj,
                    dataIdx & NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
            return TRUE;
        }

        // Must clear to zero before blending
        MI_CpuClearFast(pResult, sizeof(*pResult));
        pResult->flag = (NNSG3dJntAnmResultFlag)-1;

        p = pAnmObj;
        do
        {
            // Convert from nodeID to index in the resource
            u32 dataIdx = p->mapData[nodeID];
            if ((dataIdx & (NNS_G3D_ANMOBJ_MAPDATA_EXIST |
                            NNS_G3D_ANMOBJ_MAPDATA_DISABLED)) ==
                    NNS_G3D_ANMOBJ_MAPDATA_EXIST &&
                (p->ratio > 0))
            {
                fx32 ratio;
                NNSG3dFuncAnmJnt func = (NNSG3dFuncAnmJnt)p->funcAnm;
                (*func)(&r, p, dataIdx & NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);

                // Obtains the ratio of this animation to the whole.
                if (sumOfRatio != FX32_ONE)
                {
                    ratio = FX_Div(p->ratio, sumOfRatio);
                }
                else
                {
                    ratio = p->ratio;
                }

                // scale, scaleEx0, scaleEx1 blend
                blendScaleVec_(&pResult->scale,
                               &r.scale,
                               ratio,
                               r.flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE);

                blendScaleVec_(&pResult->scaleEx0,
                               &r.scaleEx0,
                               ratio,
                               r.flag & NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE);

                blendScaleVec_(&pResult->scaleEx1,
                               &r.scaleEx1,
                               ratio,
                               r.flag & NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE);

                // trans blend
                if (!(r.flag & NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO))
                {
                    pResult->trans.x += (fx32)((fx64)ratio * r.trans.x >> FX32_SHIFT);
                    pResult->trans.y += (fx32)((fx64)ratio * r.trans.y >> FX32_SHIFT);
                    pResult->trans.z += (fx32)((fx64)ratio * r.trans.z >> FX32_SHIFT);
                }

                // rot blend
                if (!(r.flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO))
                {
                    pResult->rot._00 += ratio * r.rot._00 >> FX32_SHIFT;
                    pResult->rot._01 += ratio * r.rot._01 >> FX32_SHIFT;
                    pResult->rot._02 += ratio * r.rot._02 >> FX32_SHIFT;
                    pResult->rot._10 += ratio * r.rot._10 >> FX32_SHIFT;
                    pResult->rot._11 += ratio * r.rot._11 >> FX32_SHIFT;
                    pResult->rot._12 += ratio * r.rot._12 >> FX32_SHIFT;
                }
                else
                {
                    pResult->rot._00 += ratio;
                    pResult->rot._11 += ratio;
                }

                pResult->flag &= r.flag;
            }

            p = p->next;
        }
        while(p);

        // Use cross product to determine line 3.
        VEC_CrossProduct((VecFx32*)&pResult->rot._00,
                         (VecFx32*)&pResult->rot._10,
                         (VecFx32*)&pResult->rot._20);
        
        VEC_Normalize((VecFx32*)&pResult->rot._00, (VecFx32*)&pResult->rot._00);
        VEC_Normalize((VecFx32*)&pResult->rot._20, (VecFx32*)&pResult->rot._20);

        // Orthagonalize all three axes
        VEC_CrossProduct((VecFx32*)&pResult->rot._20,
                         (VecFx32*)&pResult->rot._00,
                         (VecFx32*)&pResult->rot._10);

        return TRUE;
    }
}
#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmBlendVis

    Default visibility animation blend function
    Obtain the logical sum of the result
 *---------------------------------------------------------------------------*/
BOOL NNSi_G3dAnmBlendVis(NNSG3dVisAnmResult* pResult,
                         const NNSG3dAnmObj* pAnmObj,
                         u32 nodeID)
{
    BOOL rval = FALSE;
    const NNSG3dAnmObj* p;
    NNSG3dVisAnmResult tmp;
    NNS_G3D_NULL_ASSERT(pResult && pAnmObj);

    p = pAnmObj;
    pResult->isVisible = FALSE;
    do
    {
        // Convert from nodeID to resource ID
        u32 dataIdx = p->mapData[nodeID];

        if (dataIdx & NNS_G3D_ANMOBJ_MAPDATA_EXIST)
        {
            NNSG3dFuncAnmVis func = (NNSG3dFuncAnmVis)p->funcAnm;
            NNS_G3D_NULL_ASSERT(func);

            (*func)(&tmp,
                    p,
                    dataIdx & NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD);
            pResult->isVisible |= tmp.isVisible;
            rval = TRUE;
        }
        p = p->next;
    }
    while(p);

    return rval;
}


////////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//

/*---------------------------------------------------------------------------*
    NNS_G3dAnmFmtNum

    Number of animation formats registered in NNS_G3dAnmObjInitFuncArray.
    When an animation format is added, increment this value.
    (Must be less than NNS_G3D_ANMFMT_MAX.)
    Add an entry to NNS_G3dAnmObjInitFuncArray.
 *---------------------------------------------------------------------------*/
u32 NNS_G3dAnmFmtNum = 5;


/*---------------------------------------------------------------------------*
    NNS_G3dAnmObjInitFuncArray

    {anmCategory0, dummy, anmCategory1, initFunc}
 *---------------------------------------------------------------------------*/
NNSG3dAnmObjInitFunc NNS_G3dAnmObjInitFuncArray[NNS_G3D_ANMFMT_MAX] =
{
    {'M', 0, 'MA', &NNSi_G3dAnmObjInitNsBma},
    {'M', 0, 'TP', &NNSi_G3dAnmObjInitNsBtp},
    {'M', 0, 'TA', &NNSi_G3dAnmObjInitNsBta},
    {'V', 0, 'VA', &NNSi_G3dAnmObjInitNsBva},
    {'J', 0, 'CA', &NNSi_G3dAnmObjInitNsBca}
};


