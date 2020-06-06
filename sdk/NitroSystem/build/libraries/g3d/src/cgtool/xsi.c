/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     xsi.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: xsi.c,v $
  Revision 1.5  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.4  08/05/2004 04:57:37  nishida_kenji
  bugfix(texmtxCalc_flagR_, texmtxCalc_flagRS_).

  Revision 1.3  07/28/2004 02:31:21  nishida_kenji
  optimized

  Revision 1.2  07/28/2004 01:16:45  nishida_kenji
  initial version.

  Revision 1.1  07/15/2004 12:56:56  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/cgtool/xsi.h>
#include <nnsys/g3d/gecom.h>

#include <nitro/code32.h>


//
// When SRTs of the texture matrix are all set to 0 or when set to other than Identity
//
static void
texmtxCalc_flag_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    fx32 ss_sin, ss_cos;
    fx32 st_sin, st_cos;
    fx32 tmpW, tmpH;
    fx64 tmp;
    fx32 p, q;

    tmpW = (s32)anm->origWidth << FX32_SHIFT;
    tmpH = (s32)anm->origHeight << FX32_SHIFT;
    FX_DivAsync(tmpH, tmpW);

    ss_sin = (fx32)((fx64)anm->scaleS * anm->sinR >> FX32_SHIFT);
    ss_cos = (fx32)((fx64)anm->scaleS * anm->cosR >> FX32_SHIFT);
    st_sin = (fx32)((fx64)anm->scaleT * anm->sinR >> FX32_SHIFT);
    st_cos = (fx32)((fx64)anm->scaleT * anm->cosR >> FX32_SHIFT);

    m->_00 = ss_cos;
    m->_11 = st_cos;

    m->_01 = st_sin * FX_GetDivResult() >> FX32_SHIFT;
    FX_DivAsync(tmpW, tmpH);

    tmp = ((fx64)anm->transS * anm->cosR + (fx64)anm->transT * anm->sinR) >> FX32_SHIFT;
    p = (fx32)((tmp * anm->scaleS) >> FX32_SHIFT);
    tmp = ((fx64)anm->transS * anm->sinR - (fx64)anm->transT * anm->cosR) >> FX32_SHIFT;
    q = (fx32)((tmp * anm->scaleT) >> FX32_SHIFT);

    m->_30 = (anm->origWidth * (ss_sin - p)) << 4;
    m->_31 = (-anm->origHeight * (st_cos + q - FX32_ONE)) << 4;

    m->_10 = -ss_sin * FX_GetDivResult() >> FX32_SHIFT;
}


//
// When Scale is not applied to the texture matrix
//
static void
texmtxCalc_flagS_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    fx32 tmpW, tmpH;
    fx32 p, q;

    tmpW = (s32)anm->origWidth << FX32_SHIFT;
    tmpH = (s32)anm->origHeight << FX32_SHIFT;
    FX_DivAsync(tmpH, tmpW);

    m->_00 = anm->cosR;
    m->_11 = anm->cosR;

    m->_01 = anm->sinR * FX_GetDivResult() >> FX32_SHIFT;
    FX_DivAsync(tmpW, tmpH);

    p = (fx32)(((fx64)anm->transS * anm->cosR + (fx64)anm->transT * anm->sinR) >> FX32_SHIFT);
    q = (fx32)(((fx64)anm->transS * anm->sinR - (fx64)anm->transT * anm->cosR) >> FX32_SHIFT);

    m->_30 = (anm->origWidth * (anm->sinR - p)) << 4;
    m->_31 = (-anm->origHeight * (anm->cosR + q - FX32_ONE)) << 4;

    m->_10 = -anm->sinR * FX_GetDivResult() >> FX32_SHIFT;
}


//
// When Rotation is not applied to the texture matrix
//
static void
texmtxCalc_flagR_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    fx32 p, q;

    m->_00 = anm->scaleS;
    m->_11 = anm->scaleT;

    m->_01 = 0;

    p = (fx32)(((fx64)anm->transS * anm->scaleS) >> FX32_SHIFT);
    q = (fx32)(((fx64)-anm->transT * anm->scaleT) >> FX32_SHIFT);

    m->_30 = (anm->origWidth * (-p)) << 4;
    m->_31 = (-anm->origHeight * (anm->scaleT + q - FX32_ONE)) << 4;

    m->_10 = 0;
}


//
// When Rotation and Scale are not applied to the texture matrix
//
static void
texmtxCalc_flagRS_(MtxFx44* m, const NNSG3dMatAnmResult* anm)
{
    fx32 p, q;

    m->_00 = FX32_ONE;
    m->_11 = FX32_ONE;

    m->_01 = 0;

    p = anm->transS;
    q = -anm->transT;

    m->_30 = (anm->origWidth * (-p)) << 4;
    m->_31 = (-anm->origHeight * (q)) << 4;

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

    m->_01 = st_sin * FX_GetDivResult() >> FX32_SHIFT;
    FX_DivAsync(tmpW, tmpH);

    m->_30 = (anm->origWidth * (ss_sin)) << 4;
    m->_31 = (-anm->origHeight * (st_cos - FX32_ONE)) << 4;

    m->_10 = -ss_sin * FX_GetDivResult() >> FX32_SHIFT;
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

    m->_01 = anm->sinR * FX_GetDivResult() >> FX32_SHIFT;
    FX_DivAsync(tmpW, tmpH);

    m->_30 = (anm->origWidth * (anm->sinR)) << 4;
    m->_31 = (-anm->origHeight * (anm->cosR - FX32_ONE)) << 4;

    m->_10 = -anm->sinR * FX_GetDivResult() >> FX32_SHIFT;
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
    m->_31 = (-anm->origHeight * (anm->scaleT - FX32_ONE)) << 4;

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
    NNSi_G3dSendTexSRTXsi

    Sets the texture matrix of the model data created with XSI.
 *---------------------------------------------------------------------------*/
void NNSi_G3dSendTexSRTXsi(const NNSG3dMatAnmResult* anm)
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

    {
        NNSG3dMatAnmResult* a = (NNSG3dMatAnmResult*) anm;
        if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE)
        {
            a->scaleS = a->scaleT = FX32_ONE;
        }
        if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO)
        {
            a->cosR = FX32_ONE;
            a->sinR = 0;
        }
        if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO)
        {
            a->transS = a->transT = 0;
        }
    }


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

