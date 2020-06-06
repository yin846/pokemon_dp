/*---------------------------------------------------------------------------*
  Project:  NitroSystem - G3D(Hokotate) -
  File:     1mat1shp.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: 1mat1shp.c,v $
  Revision 1.5  10/07/2004 02:10:43 AM  nishida_kenji
  Add comments.

  Revision 1.4  09/27/2004 11:42:13 AM  nishida_kenji
  fix a glitch(if pos_scale != 0, cur mtx is scaled after execution.)

  Revision 1.3  09/09/2004 11:50:33 AM  nishida_kenji
  bugfix NNS_G3dDraw1Mat1Shp(the case of models with texcoords)

  Revision 1.2  09/03/2004 12:07:12 PM  nishida_kenji
  NNS_G3dDraw1Mat1Shp applies pos_scale to the current mtx firstly.

  Revision 1.1  08/17/2004 10:38:05 AM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/1mat1shp.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/cgtool.h>


/*---------------------------------------------------------------------------*
    NNS_G3dDraw1Mat1Shp

    Draws specifying the matID and shpID. This function is for drawing simple models.
    If sendMat is FALSE, no material will be sent.
    See the 1mat1shp sample for details on how to use.
 *---------------------------------------------------------------------------*/
void
NNS_G3dDraw1Mat1Shp(const NNSG3dResMdl* pResMdl,
                    u32 matID,
                    u32 shpID,
                    BOOL sendMat)
{
    NNS_G3D_NULL_ASSERT(pResMdl);

    if (pResMdl->info.posScale != FX32_ONE)
    {
        // When the scale correction by pos_scale is needed
        NNS_G3dGeScale(pResMdl->info.posScale,
                       pResMdl->info.posScale,
                       pResMdl->info.posScale);
    }

    if (sendMat)
    {
        //
        // When sending materials:
        // -It is impossible to play the material animation. (If the material data stored in the model data is rewritten, the same result can be obtained.)
        //    
        // - Global material information that is stored in the NNS_G3dGlb structure is not reflected. The material data
        //   stored in the model data is reflected.
        //
        const NNSG3dResMatData* mat;
        u32 cmd[7];
        NNS_G3D_ASSERT(matID < pResMdl->info.numMat);

        {
            // Obtains the pointer to NNSG3dResMatData
            const NNSG3dResMat* p = NNS_G3dGetMat(pResMdl);
            NNS_G3D_NULL_ASSERT(p);
            mat = NNS_G3dGetMatDataByIdx(p, matID);
            NNS_G3D_NULL_ASSERT(mat);
        }

        // Returns without drawing if transparent
        if (!(mat->polyAttr & REG_G3_POLYGON_ATTR_ALPHA_MASK))
            return;

        cmd[0] = (G3OP_DIF_AMB      << 0) |
                 (G3OP_SPE_EMI      << 8) |
                 (G3OP_POLYGON_ATTR << 16);
        cmd[1] = mat->diffAmb;
        cmd[2] = mat->specEmi;
        cmd[3] = mat->polyAttr;

        if (mat->flag & NNS_G3D_MATFLAG_WIREFRAME)
        {
            // For wire frame display, alpha to be sent to the geometry engine becomes 0.
            // Set the ALPHA bit to 0.
            cmd[3] &= ~REG_G3_POLYGON_ATTR_ALPHA_MASK;
        }

        cmd[4] = (G3OP_TEXIMAGE_PARAM << 0) | (G3OP_TEXPLTT_BASE << 8);
        cmd[5] = mat->texImageParam;
        cmd[6] = mat->texPlttBase;

        NNS_G3dGeBufferData_N(&cmd[0], 7);

        if (mat->flag & NNS_G3D_MATFLAG_TEXMTX_USE)
        {
            NNSG3dSendTexSRT func = NNS_G3dSendTexSRT_FuncArray[pResMdl->info.texMtxMode];
            // Set the texture matrix
            NNSG3dMatAnmResult dummy;
            const u8* p = (const u8*)mat + sizeof(NNSG3dResMatData);
            NNS_G3D_NULL_ASSERT(func);

            dummy.flag = NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET;
            dummy.origWidth = mat->origWidth;
            dummy.origHeight = mat->origHeight;
            dummy.magW = mat->magW;
            dummy.magH = mat->magH;

            // Set Scale of the texture matrix
            if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_SCALEONE))
            {
                dummy.scaleS = *((fx32*)(p));
                dummy.scaleT = *((fx32*)(p + 4));
                p += 8;
            }
            else
            {
                dummy.flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE;
            }

            // Set Rotation of the texture matrix
            if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_ROTZERO))
            {
                dummy.sinR = *((fx16*)(p));
                dummy.cosR = *((fx16*)(p + 2));
                p += 4;
            }
            else
            {
                dummy.flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO;
            }

            // Set Translation of the texture matrix
            if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_TRANSZERO))
            {
                dummy.transS = *((fx32*)(p));
                dummy.transT = *((fx32*)(p + 4));
            }
            else
            {
                dummy.flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO;
            }

            // Set the texture matrix to the geometry engine
            (*func)(&dummy);
        }
    }

    {
        // Send Shp
        const NNSG3dResShp* p = NNS_G3dGetShp(pResMdl);
        const NNSG3dResShpData* shp;
        NNS_G3D_ASSERT(shpID < pResMdl->info.numShp);
        NNS_G3D_NULL_ASSERT(p);
        shp = NNS_G3dGetShpDataByIdx(p, shpID);
        NNS_G3D_NULL_ASSERT(shp);

        NNS_G3dGeSendDL((u8*)shp + shp->ofsDL, shp->sizeDL);
    }

    if (pResMdl->info.invPosScale != FX32_ONE)
    {
        // Restore the original when applying scale correction using pos_scale
        NNS_G3dGeScale(pResMdl->info.invPosScale,
                       pResMdl->info.invPosScale,
                       pResMdl->info.invPosScale);
    }
}









