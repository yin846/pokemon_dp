/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     basic.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: basic.c,v $
  Revision 1.8  07/15/2004 12:57:28 PM  nishida_kenji
  rename functions

  Revision 1.7  07/12/2004 09:26:33 AM  nishida_kenji
  (none)

  Revision 1.6  07/12/2004 02:25:15 AM  nishida_kenji
  (none)

  Revision 1.5  07/10/2004 08:48:00 AM  nishida_kenji
  (none)

  Revision 1.4  07/09/2004 12:46:22 PM  nishida_kenji
  add comments

  Revision 1.3  07/09/2004 07:17:27 AM  nishida_kenji
  (none)

  Revision 1.2  07/08/2004 12:46:18 PM  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:42:23 PM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/cgtool/basic.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/binres/res_struct.h>


/*---------------------------------------------------------------------------*
    NNSi_G3dSendJointSRTBasic

    This configures the joint matrix in the geometry engine.
    Normally, the pointer is stored in NNS_G3dSendJointSRT_FuncArray and
    called when NNS_G3D_SCALINGRULE_STANDARD is specified (in cases where
    <model_info>::scaling_rule is "standard.")

    When called, the mode must be Position/Vector and the matrix that is
    the subject of the process must be in the current matrix.
 *---------------------------------------------------------------------------*/
void NNSi_G3dSendJointSRTBasic(const NNSG3dJntAnmResult* result)
{
    NNS_G3D_NULL_ASSERT(result);

    // At this time,
    // the matrix mode must be Position/Vector mode.
    // The matrix to be processed must be the current matrix.
    if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO))
    {
        if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO))
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
            // Position/Vector mode is slow, but 
            // using a lot of CPU cycles to send a command to change the mode is not desirable. 
            // 
            NNS_G3dGeBufferOP_N(G3OP_MTX_TRANS,
                                (u32*)&result->trans.x,
                                G3OP_MTX_TRANS_NPARAMS);
        }
    }
    else
    {
        if (!(result->flag & NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO))
        {
            NNS_G3dGeBufferOP_N(G3OP_MTX_MULT_3x3,
                                (u32*)&result->rot._00,
                                G3OP_MTX_MULT_3x3_NPARAMS);
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
    NNSi_G3dGetJointScaleBasic

    Performs the most basic of scaling, without using *(p + 3), *(p + 4) or *(p + 5).
 *---------------------------------------------------------------------------*/
void NNSi_G3dGetJointScaleBasic(NNSG3dJntAnmResult* pResult,
                                const fx32* p,
                                const u8*,
                                u32 srtflag)
{
    // NOTICE:
    // cmd can be NULL
    // If NNS_G3D_SRTFLAG_SCALE_ONE is ON, p can be NULL

    NNS_G3D_NULL_ASSERT(pResult);
    
    if (srtflag & NNS_G3D_SRTFLAG_SCALE_ONE)
    {
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE;
    }
    else
    {
        NNS_G3D_NULL_ASSERT(p);

        pResult->scale.x = *(p + 0);
        pResult->scale.y = *(p + 1);
        pResult->scale.z = *(p + 2);
    }

    // Not used, but best if set anyway (to balance with blend)
    pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE |
                     NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE;
}


