/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     si3d.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: si3d.c,v $
  Revision 1.13  02/25/2005 05:39:01  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.12  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.11  09/21/2004 05:56:20  nishida_kenji
  bugfix NNSi_G3dSendTexSRTSi3d(if texscale == 1.0 and textrans != 0.0).

  Revision 1.10  07/28/2004 02:04:11  nishida_kenji
  bug fix(in the case that a root node is scaled)

  Revision 1.9  07/15/2004 12:57:28  nishida_kenji
  rename functions

  Revision 1.8  07/15/2004 02:42:28  nishida_kenji
  add magW on anmMatResult.

  Revision 1.7  07/12/2004 12:50:46  nishida_kenji
  refactoring

  Revision 1.6  07/12/2004 09:26:33  nishida_kenji
  (none)

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
#include <nnsys/g3d/cgtool/si3d.h>

#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/sbc.h>
#include <nnsys/g3d/util.h>

#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
    NNSi_G3dSendJointSRTSi3d

    This configures the joint matrix in the geometry engine.
    Normally, the pointer is stored in NNS_G3dSendJointSRT_FuncArray and
    called when NNS_G3D_SCALINGRULE_SI3D has been specified (in cases where
    <model_info>::scaling_rule is "si3d.")

    When called, the mode must be Position/Vector and the matrix that is
    the subject of the process must be in the current matrix.
 *---------------------------------------------------------------------------*/
void NNSi_G3dSendJointSRTSi3d(const NNSG3dJntAnmResult* result)
{
    BOOL rtFlag = FALSE;
    // At this time,
    // the mode must be Position/Vector.
    // The matrix that is the subject of the process must be in the current matrix.

    // If necessary, the following computation is done, where
    // a(parent)->b(child)->c(grandchild)
    // Sc * (SbSa) * Rc * T'c * inv(SbSa) * Sb * Sa * Rb * T'b * inv(Sa) * Sa * Ra * T'a

    u32 flagScaleEx = result->flag & (u32)(NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                                           NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE);

    //
    // The number in ScaleEx0 is the power of the scale, while the number
    // in ScaleEx1 is the power of the inverse of the scale.
    // Thus, neither on its own is 1.
    //
    NNS_G3D_ASSERT(flagScaleEx == 0 ||
                   flagScaleEx == (NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                                   NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE));

    if (!flagScaleEx)
    {
        // Perform scaling if inverse scale to parent is not 1
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scaleEx1.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO))
    {
        // If Trans is not 0
        if (!flagScaleEx)
        {
            // Multiply translation as prescribed if inverse scale to parent is not 1
            VecFx32 tmp;
            tmp.x = (fx32)((fx64)result->trans.x * result->scaleEx0.x >> FX32_SHIFT);
            tmp.y = (fx32)((fx64)result->trans.y * result->scaleEx0.y >> FX32_SHIFT);
            tmp.z = (fx32)((fx64)result->trans.z * result->scaleEx0.z >> FX32_SHIFT);
            
            NNS_G3dGeBufferOP_N(G3OP_MTX_TRANS,
                                (u32*)&tmp.x,
                                G3OP_MTX_TRANS_NPARAMS);
        }
        else
        {
            rtFlag = TRUE;
        }
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO))
    {
        // Multiply by rotation matrix if rotation matrix is not a unit matrix.
        if (rtFlag)
        {
            // HACK ALERT
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
        if (rtFlag)
        {
            // Send translation as-is if inverse scale to parent is 1
            NNS_G3dGeBufferOP_N(G3OP_MTX_TRANS,
                                (u32*)&result->trans.x,
                                G3OP_MTX_TRANS_NPARAMS);
        }
    }

    if (!flagScaleEx)
    {
        // Perform scaling if scale to parent is not 1
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scaleEx0.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }

    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE))
    {
        // Perform scaling if own scale is not 1
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&result->scale.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dGetJointScaleSi3d

    Performs scaling in consideration of Si3d's Classic Scale Off.
    The cumulative scale value to the parent node is put in pResult->scaleEx0.
    The cumulative inverse scale value to the parent node is put inpResult->scaleEx1.
    Own cumulative scale value vector & cumulative inverse scale value get
    put in NNS_G3dRSOnGlb.scaleCache.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dGetJointScaleSi3d(NNSG3dJntAnmResult* pResult,
                          const fx32* p,
                          const u8* cmd,
                          u32 srtflag)
{
    //
    // srtflag is NNSG3dResNodeData::flag.
    // cmd indicates the currently processing SBC (NODEDESC command)
    //

    //
    // The cumulative scale value to the parent node is put in ScaleEx0, ScaleEx1
    //
    u32 nodeID;
    u32 parentID;
    NNS_G3D_NULL_ASSERT(pResult && cmd && p && NNS_G3dRS);
    
    nodeID = *(cmd + 1);
    parentID = *(cmd + 2);

    if (srtflag & NNS_G3D_SRTFLAG_SCALE_ONE)
    {
        // Own scale is 1, so set flag
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE;

        if (NNSi_G3dBitVecCheck(&NNS_G3dRS->isScaleCacheOne[0], parentID))
        {
            // Cumulative scale value left as 1
            NNSi_G3dBitVecSet(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

            pResult->flag |= (NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                              NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE);
        }
        else
        {
            // Inherit parent's cumulative scale value
            MI_CpuCopy32(&NNS_G3dRSOnGlb.scaleCache[parentID],
                         &NNS_G3dRSOnGlb.scaleCache[nodeID],
                         sizeof(VecFx32) + sizeof(VecFx32));

            MI_CpuCopy32(&NNS_G3dRSOnGlb.scaleCache[parentID],
                         &pResult->scaleEx0,
                         sizeof(VecFx32) + sizeof(VecFx32));
        }
    }
    else
    {
        // Set own scale value
        pResult->scale.x = *(p + 0);
        pResult->scale.y = *(p + 1);
        pResult->scale.z = *(p + 2);

        if (NNSi_G3dBitVecCheck(&NNS_G3dRS->isScaleCacheOne[0], parentID))
        {
            // Own scale value becomes cumulative scale value
            // Copy NNSG3dResNodeData Sx,Sy,Sz,InvSx,InvSy,InvSz.
            MI_CpuCopy32(p,
                         &NNS_G3dRSOnGlb.scaleCache[nodeID],
                         sizeof(VecFx32) + sizeof(VecFx32));

            // NOTICE
            // scaleOne flag for nodeID=0 is set beforehand.
            // This is done because the root node's parent node indicates that the value
            // is scaleOne, so the root node must be reset if it is not scaleOne
            // It is an error for this code to leave outside the "if."
            NNSi_G3dBitVecReset(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

            // The cumulative scale value to the parent is 1, so set the flag.
            pResult->flag |= (NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                              NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE);
        }
        else
        {
            // NOTICE
            // scaleOne flag for nodeID=0 is set beforehand.
            // This is done because the root node's parent node indicates that the value
            // is scaleOne, so the root node must be reset if it is not scaleOne
            // It is an error for this code to leave outside the "if."
            NNSi_G3dBitVecReset(&NNS_G3dRS->isScaleCacheOne[0], nodeID);

            // Multiply parent's cumulative scale value by own scale value  and get own scale value.
            NNS_G3dRSOnGlb.scaleCache[nodeID].s.x =
                (fx32)((fx64)(*(p + 0)) * NNS_G3dRSOnGlb.scaleCache[parentID].s.x >> FX32_SHIFT);
            NNS_G3dRSOnGlb.scaleCache[nodeID].s.y =
                (fx32)((fx64)(*(p + 1)) * NNS_G3dRSOnGlb.scaleCache[parentID].s.y >> FX32_SHIFT);
            NNS_G3dRSOnGlb.scaleCache[nodeID].s.z =
                (fx32)((fx64)(*(p + 2)) * NNS_G3dRSOnGlb.scaleCache[parentID].s.z >> FX32_SHIFT);
            
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.x =
                (fx32)((fx64)(*(p + 3)) * NNS_G3dRSOnGlb.scaleCache[parentID].inv.x >> FX32_SHIFT);
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.y =
                (fx32)((fx64)(*(p + 4)) * NNS_G3dRSOnGlb.scaleCache[parentID].inv.y >> FX32_SHIFT);
            NNS_G3dRSOnGlb.scaleCache[nodeID].inv.z =
                (fx32)((fx64)(*(p + 5)) * NNS_G3dRSOnGlb.scaleCache[parentID].inv.z >> FX32_SHIFT);

            // Set the cumulative scale value to the parent
            MI_CpuCopy32(&NNS_G3dRSOnGlb.scaleCache[parentID],
                         &pResult->scaleEx0,
                         sizeof(VecFx32) + sizeof(VecFx32));
        }
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dSendTexSRTSi3d

    Sets the texture matrix of the model data created with SoftImage3D.
 *---------------------------------------------------------------------------*/
void NNSi_G3dSendTexSRTSi3d(const NNSG3dMatAnmResult* anm)
{
    struct
    {
        u32     cmd;
        u32     mode_tex;
        MtxFx43 m;
        u32     mode_pos_vec;
    }
    data;

    NNS_G3D_NULL_ASSERT(anm);
    NNS_G3D_ASSERT(anm->flag & (NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET |
                                NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT));

    if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET)
    {
        data.cmd = (G3OP_MTX_MODE << 0)     |
                   (G3OP_MTX_LOAD_4x3 << 8) |
                   (G3OP_MTX_MODE << 16);
    }
    else
    {
        data.cmd = (G3OP_MTX_MODE << 0)     |
                   (G3OP_MTX_MULT_4x3 << 8) |
                   (G3OP_MTX_MODE << 16);
    }

    data.mode_tex     = GX_MTXMODE_TEXTURE;
    data.mode_pos_vec = GX_MTXMODE_POSITION_VECTOR;

    data.m._01 = data.m._02 = data.m._10 = data.m._12 = data.m._20 =
    data.m._21 = data.m._22 = data.m._32 = 0;

    if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO)
    {
        data.m._30 = 0;
        data.m._31 = 0;

        if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE)
        {
            data.m._00 = FX32_ONE;
            data.m._11 = FX32_ONE;
        }
        else
        {
            data.m._00 = anm->scaleS;
            data.m._11 = anm->scaleT;
        }
    }
    else if (anm->flag & NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE)
    {
        data.m._30 = -(anm->transS << 4) * anm->origWidth;
        data.m._31 = -(anm->transT << 4) * anm->origHeight;
        data.m._00 = FX32_ONE;
        data.m._11 = FX32_ONE;
    }
    else
    {
        data.m._30 = -(fx32)((fx64)anm->scaleS * anm->transS >> FX32_SHIFT - 4) * anm->origWidth;
        data.m._31 = -(fx32)((fx64)anm->scaleT * anm->transT >> FX32_SHIFT - 4) * anm->origHeight;
        data.m._00 = anm->scaleS;
        data.m._11 = anm->scaleT;
    }

    if (anm->magW != FX32_ONE)
    {
        data.m._00 = (fx32)((fx64)anm->magW * data.m._00 >> FX32_SHIFT);
        data.m._30 = (fx32)((fx64)anm->magW * data.m._30 >> FX32_SHIFT);

    }

    if (anm->magH != FX32_ONE)
    {
        data.m._11 = (fx32)((fx64)anm->magH * data.m._11 >> FX32_SHIFT);
        data.m._31 = (fx32)((fx64)anm->magH * data.m._31 >> FX32_SHIFT);
    }

    NNS_G3dGeBufferData_N((u32*)&data.cmd, sizeof(data) / 4);
}





