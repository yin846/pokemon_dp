/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     maya.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: maya.c,v $
  Revision 1.12  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.11  08/04/2004 07:14:18  nishida_kenji
  bugfix(texmtxCalc_flagS_)

  Revision 1.10  07/15/2004 12:57:28  nishida_kenji
  rename functions

  Revision 1.9  07/15/2004 02:42:28  nishida_kenji
  add magW on anmMatResult.

  Revision 1.8  07/12/2004 12:50:46  nishida_kenji
  refactoring

  Revision 1.7  07/12/2004 09:26:33  nishida_kenji
  (none)

  Revision 1.6  07/12/2004 05:44:19  nishida_kenji
  small bug fix

  Revision 1.5  07/12/2004 02:25:15  nishida_kenji
  (none)

  Revision 1.4  07/10/2004 08:48:00  nishida_kenji
  (none)

  Revision 1.3  07/09/2004 12:46:22  nishida_kenji
  add comments

  Revision 1.2  07/08/2004 12:46:18  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:42:23  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/cgtool/maya.h>

#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/sbc.h>
#include <nnsys/g3d/util.h>


/*---------------------------------------------------------------------------*
    NNSi_G3dSendJointSRTMaya

    This configures the joint matrix in the geometry engine.
    Normally, the pointer is stored in NNS_G3dSendJointSRT_FuncArray and
    called when NNS_G3D_SCALINGRULE_MAYA has been specified (in cases where
    <model_info>::scaling_rule is "maya".)

    When called, the mode must be Position/Vector and the matrix that is
    the subject of the process must be in the current matrix.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dSendJointSRTMaya(const NNSG3dJntAnmResult* result)
{
    // At this time,
    // the mode must be Position/Vector.
    // The matrix that is the subject of the process must be in the current matrix.

    // T->IS->R->S

    BOOL trFlag = FALSE;

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO))
    {
        trFlag = TRUE;
    }

    if ((result->flag & NNS_G3D_JNTANM_RESULTFLAG_MAYA_SSC) &&
        !(result->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE))
    {
        if (trFlag)
        {
            NNS_G3dGeBufferOP_N(G3OP_MTX_TRANS,
                                (u32*)&result->trans.x,
                                G3OP_MTX_TRANS_NPARAMS);
            trFlag = FALSE;
        }
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scaleEx0.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO))
    {
        if (trFlag)
        {
            // HACK ALLERT
            // It is assumed that "rot" and "trans" are contiguous.
            // That is to say, the code depends on the order of the members of NNSG3dJntAnmResult.
            NNS_G3dGeBufferOP_N(G3OP_MTX_MULT_4x3,
                                (u32*)&result->rot._00,
                                G3OP_MTX_MULT_4x3_NPARAMS);
        }
        else
        {
            NNS_G3dGeBufferOP_N(G3OP_MTX_MULT_3x3,
                                (u32*)&result->rot._00,
                                G3OP_MTX_MULT_3x3_NPARAMS);
        }
    }
    else
    {
        if (trFlag)
        {
            NNS_G3dGeBufferOP_N(G3OP_MTX_TRANS,
                                (u32*)&result->trans.x,
                                G3OP_MTX_TRANS_NPARAMS);
        }
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE))
    {
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scale.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dGetJointScaleMaya

    Performs scaling in consideration of Maya's Segment Scale Compensate.
    For node performing SSC, the inverse vector of the parent scale is put in pResult->scaleEx0.
    For parent of node performing SSC, the inverse vector of the scale is
    put in NNS_G3dRSOnGlb.scaleCache.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dGetJointScaleMaya(NNSG3dJntAnmResult* pResult,
                          const fx32* p,
                          const u8* cmd,
                          u32 srtflag)
{
    //
    // srtflag is NNSG3dResNodeData::flag.
    // cmd indicates the currently processing SBC (NODEDESC command)
    //
    u8 opFlag;

    NNS_G3D_NULL_ASSERT(pResult && cmd && p && NNS_G3dRS);
    opFlag = *(cmd + 3);

    if (srtflag & NNS_G3D_SRTFLAG_SCALE_ONE)
    {
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE;
        if (opFlag & NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_PARENT)
        {
            u32 nodeID = *(cmd + 1);

            // Reset bit, since unit scale for parent of node performing SSC
            NNSi_G3dBitVecSet(&NNS_G3dRS->isScaleCacheOne[0], nodeID);
        }
    }
    else
    {
        pResult->scale.x = *(p + 0);
        pResult->scale.y = *(p + 1);
        pResult->scale.z = *(p + 2);

        if (opFlag & NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_PARENT)
        {
            u32 nodeID = *(cmd + 1);

            // Reset bit, since parent of node performing SSC is not in unit scale.
            NNSi_G3dBitVecReset(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

            // Put in scaleCache of nodeID, since self is parent of SSC.
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.x = *(p + 3);
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.y = *(p + 4);
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.z = *(p + 5);
        }
    }

    if (opFlag & NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_APPLY)
    {
        u32 parentID = *(cmd + 2);
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_MAYA_SSC;

        if (NNSi_G3dBitVecCheck(&NNS_G3dRS->isScaleCacheOne[0], parentID))
        {
            pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE;
        }
        else
        {
            // Put into scaleEx0
            pResult->scaleEx0 = NNS_G3dRSOnGlb.scaleCache[parentID].inv;
        }
    }

    // Not used, but best to set anyway (to balance with blend)
    pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE;
}



//
//
// Texture matrix coefficients
//
//
#include <nitro/code32.h>

//
// When SRT of the texture matrix are all set to 0 or when set to other than Identity
//
static void
texmtxCalc_flag_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    fx32 ss_sin, ss_cos;
    fx32 st_sin, st_cos;
    fx32 tmpW, tmpH;

    tmpW = (s32)anm->origWidth << FX32_SHIFT;
    tmpH = (s32)anm->origHeight << FX32_SHIFT;
    FX_DivAsync(tmpH, tmpW);

    ss_sin = (fx32)((fx64)anm->scaleS * anm->sinR >> FX32_SHIFT);
    ss_cos = (fx32)((fx64)anm->scaleS * anm->cosR >> FX32_SHIFT);
    st_sin = (fx32)((fx64)anm->scaleT * anm->sinR >> FX32_SHIFT);
    st_cos = (fx32)((fx64)anm->scaleT * anm->cosR >> FX32_SHIFT);

    m->_00 = ss_cos;
    m->_11 = st_cos;

    m->_01 = -st_sin * FX_GetDivResult() >> FX32_SHIFT;
    FX_DivAsync(tmpW, tmpH);

    m->_30 = ((-ss_sin - ss_cos + anm->scaleS) * anm->origWidth << 3) -
             (fx32)((fx64)anm->scaleS * anm->transS >> FX32_SHIFT - 4) * anm->origWidth;
    m->_31 = (( st_sin - st_cos - anm->scaleT + FX32_ONE * 2) * anm->origHeight << 3) +
             (fx32)((fx64)anm->scaleT * anm->transT >> FX32_SHIFT - 4) * anm->origHeight;

    m->_10 = ss_sin * FX_GetDivResult() >> FX32_SHIFT;
}


//
// When Scale is not applied to the texture matrix
//
static void
texmtxCalc_flagS_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    fx32 tmpW, tmpH;

    tmpW = (s32)anm->origWidth << FX32_SHIFT;
    tmpH = (s32)anm->origHeight << FX32_SHIFT;
    FX_DivAsync(tmpH, tmpW);

    m->_00 = anm->cosR;
    m->_11 = anm->cosR;

    m->_01 = -anm->sinR * FX_GetDivResult() >> FX32_SHIFT;
    FX_DivAsync(tmpW, tmpH);

    m->_30 = ((-anm->sinR - anm->cosR + FX32_ONE) * anm->origWidth << 3) -
             (anm->transS * anm->origWidth << 4);
    m->_31 = (( anm->sinR - anm->cosR + FX32_ONE) * anm->origHeight << 3) +
             (anm->transT * anm->origHeight << 4);

    m->_10 = anm->sinR * FX_GetDivResult() >> FX32_SHIFT;
}


//
// When Rotation is not applied to the texture matrix
//
static void
texmtxCalc_flagR_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    m->_00 = anm->scaleS;
    m->_11 = anm->scaleT;

    m->_01 = 0;

    m->_30 = -(fx32)((fx64)anm->scaleS * anm->transS >> FX32_SHIFT - 4) * anm->origWidth;
    m->_31 = ((- anm->scaleT - anm->scaleT + FX32_ONE * 2) * anm->origHeight << 3) +
             (fx32)((fx64)anm->scaleT * anm->transT >> FX32_SHIFT - 4) * anm->origHeight;

    m->_10 = 0;
}


//
// When Rotation and Scale are not applied to the texture matrix
//
static void
texmtxCalc_flagRS_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    m->_00 = FX32_ONE;
    m->_11 = FX32_ONE;

    m->_01 = 0;

    m->_30 = -(anm->transS * anm->origWidth) << 4;
    m->_31 =  (anm->transT * anm->origHeight) << 4;

    m->_10 = 0;
}


//
// When Translation is not applied to the texture matrix
//
static void
texmtxCalc_flagT_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    fx32 ss_sin, ss_cos;
    fx32 st_sin, st_cos;
    fx32 tmpW, tmpH;

    tmpW = (s32)anm->origWidth << FX32_SHIFT;
    tmpH = (s32)anm->origHeight << FX32_SHIFT;
    FX_DivAsync(tmpH, tmpW);

    ss_sin = (fx32)((fx64)anm->scaleS * anm->sinR >> FX32_SHIFT);
    ss_cos = (fx32)((fx64)anm->scaleS * anm->cosR >> FX32_SHIFT);
    st_sin = (fx32)((fx64)anm->scaleT * anm->sinR >> FX32_SHIFT);
    st_cos = (fx32)((fx64)anm->scaleT * anm->cosR >> FX32_SHIFT);

    m->_00 = ss_cos;
    m->_11 = st_cos;

    m->_01 = -st_sin * FX_GetDivResult() >> FX32_SHIFT;
    FX_DivAsync(tmpW, tmpH);

    m->_30 = ((-ss_sin - ss_cos + anm->scaleS) * anm->origWidth << 3);
    m->_31 = (( st_sin - st_cos - anm->scaleT + FX32_ONE * 2) * anm->origHeight << 3);

    m->_10 = ss_sin * FX_GetDivResult() >> FX32_SHIFT;
}


//
// When Translation and Scale are not applied to the texture matrix
//
static void
texmtxCalc_flagTS_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    fx32 tmpW, tmpH;

    tmpW = (s32)anm->origWidth << FX32_SHIFT;
    tmpH = (s32)anm->origHeight << FX32_SHIFT;
    FX_DivAsync(tmpH, tmpW);

    m->_00 = anm->cosR;
    m->_11 = anm->cosR;

    m->_01 = -anm->sinR * FX_GetDivResult() >> FX32_SHIFT;
    FX_DivAsync(tmpW, tmpH);

    m->_30 = ((-anm->sinR - anm->cosR + FX32_ONE) * anm->origWidth << 3);
    m->_31 = (( anm->sinR - anm->cosR + FX32_ONE) * anm->origHeight << 3);

    m->_10 = anm->sinR * FX_GetDivResult() >> FX32_SHIFT;
}


//
// When Translation and Rotation are not applied to the texture matrix
//
static void
texmtxCalc_flagTR_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    m->_00 = anm->scaleS;
    m->_11 = anm->scaleT;

    m->_01 = 0;

    m->_30 = 0;
    m->_31 = ((-2 * anm->scaleT + FX32_ONE * 2) * anm->origHeight << 3);

    m->_10 = 0;
}


//
// When the texture matrix is unit matrix
//
static void
texmtxCalc_flagTRS_(MtxFx44* m, const NNSG3dMatAnmResult*)
{
    m->_00 = FX32_ONE;
    m->_01 = 0;
    m->_10 = 0;
    m->_11 = FX32_ONE;
    m->_30 = 0;
    m->_31 = 0;
}


static void
(*calcTexMtx_[8])(MtxFx44*, const NNSG3dMatAnmResult*) =
{
    &texmtxCalc_flag_,
    &texmtxCalc_flagS_,
    &texmtxCalc_flagR_,
    &texmtxCalc_flagRS_,
    &texmtxCalc_flagT_,
    &texmtxCalc_flagTS_,
    &texmtxCalc_flagTR_,
    &texmtxCalc_flagTRS_
};


/*---------------------------------------------------------------------------*
    NNSi_G3dSendTexSRTMaya

    Sets the texture matrix of the model data created with Maya.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dSendTexSRTMaya(const NNSG3dMatAnmResult* anm)
{
    struct
    {
        u32     cmd;
        u32     mode_tex;
        MtxFx44 m;
        u32     mode_pos_vec;
    }
    data;

    NNS_G3D_NULL_ASSERT(anm);
    NNS_G3D_ASSERT(anm->flag & (NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET |
                                NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT));

    if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET)
    {
        data.cmd = (G3OP_MTX_MODE << 0)     |
                   (G3OP_MTX_LOAD_4x4 << 8) |
                   (G3OP_MTX_MODE << 16);
    }
    else
    {
        data.cmd = (G3OP_MTX_MODE << 0)     |
                   (G3OP_MTX_MULT_4x4 << 8) |
                   (G3OP_MTX_MODE << 16);
    }

    data.mode_tex     = GX_MTXMODE_TEXTURE;
    data.mode_pos_vec = GX_MTXMODE_POSITION_VECTOR;

    data.m._02 = data.m._03 = data.m._12 = data.m._13 =
    data.m._20 = data.m._21 = data.m._22 = data.m._23 = data.m._32 = 0;
    data.m._33 = FX32_ONE;

    (*calcTexMtx_[anm->flag & (NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE |
                               NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO  |
                               NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO)])(&data.m, anm);

    if (anm->magW != FX32_ONE)
    {
        data.m._00 = (fx32)((fx64)anm->magW * data.m._00 >> FX32_SHIFT);
        data.m._01 = (fx32)((fx64)anm->magW * data.m._01 >> FX32_SHIFT);
        data.m._30 = (fx32)((fx64)anm->magW * data.m._30 >> FX32_SHIFT);
    }

    if (anm->magH != FX32_ONE)
    {
        data.m._10 = (fx32)((fx64)anm->magH * data.m._10 >> FX32_SHIFT);
        data.m._11 = (fx32)((fx64)anm->magH * data.m._11 >> FX32_SHIFT);
        data.m._31 = (fx32)((fx64)anm->magH * data.m._31 >> FX32_SHIFT);
    }

    NNS_G3dGeBufferData_N((u32*)&data.cmd, sizeof(data) / 4);
}
#include <nitro/codereset.h>


