/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     nsbca.c

  Copyright 2004-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nsbca.c,v $
  Revision 1.21  06/16/2005 07:48:11  nishida_kenji
  Corrected errors related to node processing where SRT was not used at all.

  Revision 1.20  05/12/2005 07:06:40  nishida_kenji
  Corrected display bug that occurred when -OS option was specified when converting .ica file

  Revision 1.19  02/25/2005 05:38:51  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.18  02/21/2005 09:24:17  takano_makoto
  fix comment.

  Revision 1.17  02/21/2005 07:02:31  takano_makoto
  fix frame interpolation of rotate to avoid warping.

  Revision 1.16  11/02/2004 01:22:39  nishida_kenji
  bugfix(if scale >= 8.0 or scale <= 0.125, interpolation not works).

  Revision 1.15  10/26/2004 07:47:57  nishida_kenji
  support decimal frame interpolation.

  Revision 1.14  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.13  07/24/2004 07:39:32  nishida_kenji
  keep up with g3dcvtr's -OT, -OS, -OR.

  Revision 1.12  07/24/2004 05:03:19  nishida_kenji
  optimize

  Revision 1.11  07/23/2004 12:49:53  nishida_kenji
  runtime(initial version)

  Revision 1.7  07/20/2004 01:37:06  nishida_kenji
  fixed comments

  Revision 1.6  07/19/2004 10:58:13  nishida_kenji
  change arguments of animation calc functions

  Revision 1.5  07/15/2004 12:57:36  nishida_kenji
  rename functions

  Revision 1.4  07/12/2004 12:50:46  nishida_kenji
  refactoring

  Revision 1.2  07/09/2004 12:46:11  nishida_kenji
  add comments

  Revision 1.1  07/07/2004 12:42:02  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/anm/nsbca.h>
#include <nnsys/g3d/sbc.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmObjInitNsBca

    Initialize NNSG3dAnmObj for the .nsbca resource.
    This is called from NNS_G3dInitAnmObj.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dAnmObjInitNsBca(NNSG3dAnmObj* pAnmObj,
                        void* pResAnm,
                        const NNSG3dResMdl* pResMdl)
{
    u32 i;
    u16* ofsArray;
    NNSG3dResJntAnm* jntAnm;
    const NNSG3dResNodeInfo* jnt;

    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pResAnm);
    NNS_G3D_NULL_ASSERT(pResMdl);

    pAnmObj->resAnm = pResAnm;
    jntAnm = (NNSG3dResJntAnm*)pResAnm;
    jnt = NNS_G3dGetNodeInfo(pResMdl);
    pAnmObj->funcAnm = NNS_G3dFuncAnmJntNsBcaDefault;
    pAnmObj->numMapData = pResMdl->info.numNode;

    
    // Zero clear the mapData first
    MI_CpuClear16(&pAnmObj->mapData[0], sizeof(u16) * pAnmObj->numMapData);

    //
    // The nodeID of the model is stored in NNSG3dJntAnmSRTTag.
    //
    ofsArray = (u16*)((u8*)jntAnm + sizeof(NNSG3dResJntAnm));
    
    for (i = 0; i < jntAnm->numNode; ++i)
    {
        // For .nsbca, the index of the current resource and the node ID of the model are identical.
        NNSG3dResJntAnmSRTTag* pTag =
            (NNSG3dResJntAnmSRTTag*)((u8*)jntAnm + ofsArray[i]);
        pAnmObj->mapData[i] = (u16)((pTag->tag >> NNS_G3D_JNTANM_SRTINFO_NODE_SHIFT) |
                                    NNS_G3D_ANMOBJ_MAPDATA_EXIST);
    }
}


static void
getJntSRTAnmResult_(const NNSG3dResJntAnm* pJntAnm, 
                    u32                    dataIdx, 
                    fx32                   Frame,
                    NNSG3dJntAnmResult*    pResult);

static void
getTransData_(fx32* pVal,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm);

static void
getTransDataEx_(fx32* pVal,
                fx32 Frame,
                const u32* pData,
                const NNSG3dResJntAnm* pJntAnm);

static void
getScaleData_(fx32* s_invs,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm);

static void
getScaleDataEx_(fx32* s_invs,
                fx32 Frame,
                const u32* pData,
                const NNSG3dResJntAnm* pJntAnm);


static void
getRotData_(MtxFx33* pRot,
            fx32 Frame,
            const u32* pData,
            const NNSG3dResJntAnm* pJntAnm);

static void
getRotDataEx_(MtxFx33* pRot,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm);

static BOOL
getRotDataByIdx_(MtxFx33* pRot,
                 void* pArrayRot3,
                 void* pArrayRot5,
                 NNSG3dJntAnmRIdx info);


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmCalcNsBca

    pResult: Stores the result of the joint animation.
    pAnmObj:
    dataIdx: Index that indicates the storage location for the data in the resource.
 *---------------------------------------------------------------------------*/
void NNSi_G3dAnmCalcNsBca(NNSG3dJntAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx)
{
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);

    //
    // joint animation implementation method
    // 
    // This function is designed to set the following: 
    // pResult->flag,
    // pResult->trans,
    // pResult->rot.
    //   
    // As for the pResult->flag, be sure to set it when:
    // NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO
    // NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO
    // NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE
    // are not rotating or moving.
    //
    // pResult->scale, pResult->scaleEx0 and pResult->scaleEx1can be calculated by
    // preparing an argument and calling (*NNS_G3dRS->funcJntScale).
    // The remaining flags of pResult->flag can also be set here.
    // These configurations are due to the fact that separate calculations must be made for each CG tool. Specifically, one of
    // NNS_G3dGetJointScaleBasic,
    // NNS_G3dGetJointScaleMaya,
    // NNS_G3dGetJointScaleSi3d
    // will be called.
    // The arguments are along the lines of
    // void NNS_G3dGetJointScaleBasic(NNSG3dJntAnmResult* pResult,
    //                                const fx32* p,
    //                                const u8* cmd,
    //                                u32 srtflag)
    // and will process pResult.
    // Now we'll describe the 2nd and consecutive arguments:
    // p is a six-word array that holds the scale and the scale reciprocal for the joint.
    // NNS_G3dRS->c goes into cmd.
    // Build and input the flag corresponding to NNSG3dSRTFlag
    // (defined in res_struct.h) in srtflag.
    
    {
        fx32 frame;
        NNSG3dResJntAnm* anm = (NNSG3dResJntAnm*)(pAnmObj->resAnm);

        if (pAnmObj->frame >= (anm->numFrame << FX32_SHIFT))
            frame = (anm->numFrame << FX32_SHIFT) - 1;
        else if (pAnmObj->frame < 0)
            frame = 0;
        else
            frame = pAnmObj->frame;
        
        //
        // Put the result of the frame number of the dataIdx -th resource
        // of the joint animation resource pAnm into result.
        //

        getJntSRTAnmResult_(anm,
                            dataIdx,
                            frame,
                            pResult);
    }
}


/*---------------------------------------------------------------------------*
    vecCross_

    For the rotational matrix, the absolute value doesn't go above FX32_ONE, so
    there is no need to also cast the vector product to fx64.
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
NNS_G3D_INLINE void
vecCross_(const VecFx32 * a, const VecFx32 * b, VecFx32 * axb)
{
    fx32 x, y, z;
    NNS_G3D_NULL_ASSERT(a);
    NNS_G3D_NULL_ASSERT(b);
    NNS_G3D_NULL_ASSERT(axb);

    x = (a->y * b->z - a->z * b->y) >> FX32_SHIFT;
    y = (a->z * b->x - a->x * b->z) >> FX32_SHIFT;
    z = (a->x * b->y - a->y * b->x) >> FX32_SHIFT;

    axb->x = x;
    axb->y = y;
    axb->z = z;
}
#include <nitro/codereset.h>


/*---------------------------------------------------------------------------*
    getMdlTrans_

    use the translation of the model
 *---------------------------------------------------------------------------*/
static void 
getMdlTrans_(NNSG3dJntAnmResult* pResult)
{
    u32 idxNode;
    NNSG3dResNodeData* pNd;
    NNS_G3D_NULL_ASSERT(NNS_G3dRS);

    // HACK ALLERT
    // Internal to the NODEDESC command, so begin to take the data from there
    idxNode = *(NNS_G3dRS->c + 1);
    pNd = NNS_G3dGetNodeDataByIdx(NNS_G3dRS->pResNodeInfo, idxNode);
    
    // Translation
    if (pNd->flag & NNS_G3D_SRTFLAG_TRANS_ZERO)
    {
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO;
    }
    else
    {
        u8* p = (u8*)pNd + sizeof(*pNd);
        pResult->trans.x = *(fx32*)(p + 0);
        pResult->trans.y = *(fx32*)(p + 4);
        pResult->trans.z = *(fx32*)(p + 8);
    }
}


/*---------------------------------------------------------------------------*
    getMdlScale_

    use the scale of the model
 *---------------------------------------------------------------------------*/
static void
getMdlScale_(NNSG3dJntAnmResult* pResult)
{
    u32 idxNode;
    NNSG3dResNodeData* pNd;
    u8* p;
    NNS_G3D_NULL_ASSERT(NNS_G3dRS);
    NNS_G3D_NULL_ASSERT(NNS_G3dRS->funcJntScale);

    // HACK ALLERT
    // Internal to the NODEDESC command, so begin to take the data from there
    idxNode = *(NNS_G3dRS->c + 1);
    pNd = NNS_G3dGetNodeDataByIdx(NNS_G3dRS->pResNodeInfo, idxNode);
    p = (u8*)pNd + sizeof(*pNd);
    
    // Translation
    if (!(pNd->flag & NNS_G3D_SRTFLAG_TRANS_ZERO))
    {
        p += 12;
    }

    // Rotation
    if (!(pNd->flag & NNS_G3D_SRTFLAG_ROT_ZERO))
    {
        if (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_EXIST)
        {
            p += 2 * sizeof(fx16);
        }
        else
        {
            p += 8 * sizeof(fx16);
        }
    }

    // Scale
    (*NNS_G3dRS->funcJntScale)(pResult, (fx32*)p, NNS_G3dRS->c, pNd->flag);
}


// Start to pull data from Rot3
// The first index of the array that stores the positions of the non-zero and non-pivot
// elements will become the pivot position.
static const u8 pivotUtil_[9][4] =
{
    {4, 5, 7, 8},
    {3, 5, 6, 8},
    {3, 4, 6, 7},

    {1, 2, 7, 8},
    {0, 2, 6, 8},
    {0, 1, 6, 7},

    {1, 2, 4, 5},
    {0, 2, 3, 5},
    {0, 1, 3, 4}
};


/*---------------------------------------------------------------------------*
    getMdlRot_

    use the rotation of the model
 *---------------------------------------------------------------------------*/
static void
getMdlRot_(NNSG3dJntAnmResult* pResult)
{
    u32 idxNode;
    NNSG3dResNodeData* pNd;
    u8* p;
    NNS_G3D_NULL_ASSERT(NNS_G3dRS);

    // HACK ALLERT
    // Internal to the NODEDESC command, so begin to take the data from there
    idxNode = *(NNS_G3dRS->c + 1);
    pNd = NNS_G3dGetNodeDataByIdx(NNS_G3dRS->pResNodeInfo, idxNode);
    p = (u8*)pNd + sizeof(*pNd);
    
    // Translation
    if (!(pNd->flag & NNS_G3D_SRTFLAG_TRANS_ZERO))
    {
        p += 12;
    }

    // Rotation
    if (!(pNd->flag & NNS_G3D_SRTFLAG_ROT_ZERO))
    {
        if (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_EXIST)
        {
            // If compressed (mainly single-axle rotation),
            // restore the compressed matrix.
            fx32 A = *(fx16*)(p + 0);
            fx32 B = *(fx16*)(p + 2);
            u32 idxPivot = (u32)( (pNd->flag & NNS_G3D_SRTFLAG_IDXPIVOT_MASK) >> 
                                            NNS_G3D_SRTFLAG_IDXPIVOT_SHIFT );
            // clear anmResult.rot
            MI_Zero36B(&pResult->rot);
            
            pResult->rot.a[idxPivot] =
                (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_MINUS) ?
                    -FX32_ONE :
                    FX32_ONE;
            
            pResult->rot.a[pivotUtil_[idxPivot][0]] = A;
            pResult->rot.a[pivotUtil_[idxPivot][1]] = B;

            pResult->rot.a[pivotUtil_[idxPivot][2]] =
                (pNd->flag & NNS_G3D_SRTFLAG_SIGN_REVC) ? -B : B;

            pResult->rot.a[pivotUtil_[idxPivot][3]] =
                (pNd->flag & NNS_G3D_SRTFLAG_SIGN_REVD) ? -A : A;
        }
        else
        {
            // NOTICE:
            // Do not replace in the memory copy API
            // because of the implicit casting from fx16 to fx32

            fx16* pp = (fx16*)p;
            pResult->rot.a[0] = pNd->_00;
            pResult->rot.a[1] = *(pp + 0);
            pResult->rot.a[2] = *(pp + 1);
            pResult->rot.a[3] = *(pp + 2);
            pResult->rot.a[4] = *(pp + 3);
            pResult->rot.a[5] = *(pp + 4);
            pResult->rot.a[6] = *(pp + 5);
            pResult->rot.a[7] = *(pp + 6);
            pResult->rot.a[8] = *(pp + 7);
        }
    }
    else
    {
        pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO;
    }
}


/*---------------------------------------------------------------------------*
    getJntSRTAnmResult_

    Set the rotation, translation, and flag in pResult.
    Set the scale and the scale reciprocal in pS_invS.
 *---------------------------------------------------------------------------*/
static void
getJntSRTAnmResult_(const NNSG3dResJntAnm* pJntAnm, 
                    u32                    dataIdx, 
                    fx32                   Frame,
                    NNSG3dJntAnmResult*    pResult)
{
    NNSG3dResJntAnmSRTTag* pAnmSRTTag;
    NNSG3dJntAnmSRTTag     tag;
    const u32*             pData;
    BOOL                   IsDecimalFrame;
    fx32                   pS_invS[6]; // Changed point argument to local array

    // Check the input
    NNS_G3D_NULL_ASSERT(pJntAnm);
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_ASSERT(dataIdx < pJntAnm->numNode);
    NNS_G3D_ASSERT(Frame < (pJntAnm->numFrame << FX32_SHIFT));
    NNS_G3D_ASSERT(pJntAnm->anmHeader.category0 == 'J' &&
                   pJntAnm->anmHeader.category1 == 'CA');

    {
        const u16* ofsTag = (const u16*)((u8*) pJntAnm + sizeof(NNSG3dResJntAnm));
        // Get the SRT tag of the dataIdx-th data inside the animation resource
        pAnmSRTTag = (NNSG3dResJntAnmSRTTag*)((u8*) pJntAnm + ofsTag[dataIdx]);

        tag = (NNSG3dJntAnmSRTTag)pAnmSRTTag->tag;
    }

    // check if no animation exists
    if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY)
    {
        pResult->flag = (NNSG3dJntAnmResultFlag)(NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE |
                                                 NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO  |
                                                 NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO);
        goto GET_JOINTSCALE;
    }

    // The variable-length data in accordance with the pAnmSRTTag value in the region where pAnmSRTTag continues is stored
    pData = (const u32*)((const u8*)pAnmSRTTag + sizeof(NNSG3dResJntAnmSRTTag));

    IsDecimalFrame = (BOOL)((Frame & (FX32_ONE - 1)) &&
                            (pJntAnm->flag & NNS_G3D_JNTANM_OPTION_INTERPOLATION));

    // reset first
    pResult->flag = (NNSG3dJntAnmResultFlag) 0;

    //
    // Get the Translation
    //
    if (!(tag & (NNS_G3D_JNTANM_SRTINFO_IDENTITY_T | NNS_G3D_JNTANM_SRTINFO_BASE_T)))
    {
        // TX
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_TX))
        {
            // Call the function that extracts TX
            // The data offset will be from pAnmSRTTag
            if (IsDecimalFrame)
            {
                getTransDataEx_(&pResult->trans.x,
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getTransData_(&pResult->trans.x,
                              Frame,
                              pData,
                              pJntAnm);
            }
            pData += 2;
        }
        else
        {
            pResult->trans.x = *(fx32*)pData;
            pData += 1;
        }

        // TY
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_TY))
        {
            // Call the function that extracts TY
            // The data offset will be from pAnmSRTTag
            if (IsDecimalFrame)
            {
                getTransDataEx_(&pResult->trans.y,
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getTransData_(&pResult->trans.y,
                              Frame,
                              pData,
                              pJntAnm);
            }
            pData += 2;
        }
        else
        {
            pResult->trans.y = *(fx32*)pData;
            pData += 1;
        }

        // TZ
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_TZ))
        {
            // Call the function that extracts TZ
            // The data offset will be from pAnmSRTTag
            if (IsDecimalFrame)
            {
                getTransDataEx_(&pResult->trans.z,
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getTransData_(&pResult->trans.z,
                              Frame,
                              pData,
                              pJntAnm);
            }
            pData += 2;
        }
        else
        {
            pResult->trans.z = *(fx32*)pData;
            pData += 1;
        }
    }
    else
    {
        if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_T)
        {
            // Trans = (0, 0, 0)
            pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO;
        }
        else
        {
            // get the Trans of the model
            getMdlTrans_(pResult);
        }
    }

    //
    // get the Rotation
    //
    if (!(tag & (NNS_G3D_JNTANM_SRTINFO_IDENTITY_R | NNS_G3D_JNTANM_SRTINFO_BASE_R)))
    {
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_R))
        {
            // Call the function that extracts R
            // The data offset will be from pJntAnm->ofsRot3,
            // or from pJntAnm->ofsRot5
            if (IsDecimalFrame)
            {
                getRotDataEx_(&pResult->rot,
                              Frame,
                              pData,
                              pJntAnm);
            }
            else
            {
                getRotData_(&pResult->rot,
                            Frame,
                            pData,
                            pJntAnm);
            }
            pData += 2;
        }
        else
        {
            // This code extracts the R of the const
            if (getRotDataByIdx_(&pResult->rot,
                                 (void*)((u8*)pJntAnm + pJntAnm->ofsRot3),
                                 (void*)((u8*)pJntAnm + pJntAnm->ofsRot5),
                                 (NNSG3dJntAnmRIdx)*pData))
            {
                vecCross_((const VecFx32*)&pResult->rot._00,
                          (const VecFx32*)&pResult->rot._10,
                          (VecFx32*)&pResult->rot._20);
            }
            pData += 1;
        }
    }
    else
    {
        if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_R)
        {
            // Rot = Identity
            pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO;
        }
        else
        {
            // get the Rot of the model
            getMdlRot_(pResult);
        }
    }

    //
    // get the Scale
    //
    if (!(tag & (NNS_G3D_JNTANM_SRTINFO_IDENTITY_S | NNS_G3D_JNTANM_SRTINFO_BASE_S)))
    {
        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_SX))
        {
            // Call the function that extracts SX
            // The data offset will be from pAnmSRTTag.
            fx32 sx_invsx[2];
            if (IsDecimalFrame)
            {
                getScaleDataEx_(&sx_invsx[0],
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getScaleData_(&sx_invsx[0],
                              Frame,
                              pData,
                              pJntAnm);
            }
            *(pS_invS + 0) = sx_invsx[0];
            *(pS_invS + 3) = sx_invsx[1];
        }
        else
        {
            *(pS_invS + 0) = *(fx32*)(pData + 0);
            *(pS_invS + 3) = *(fx32*)(pData + 1);
        }
        pData += 2;

        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_SY))
        {
            // Call the function that extracts SY
            // The data offset will be from pAnmSRTTag.
            fx32 sy_invsy[2];
            if (IsDecimalFrame)
            {
                getScaleDataEx_(&sy_invsy[0],
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getScaleData_(&sy_invsy[0],
                              Frame,
                              pData,
                              pJntAnm);
            }
            *(pS_invS + 1) = sy_invsy[0];
            *(pS_invS + 4) = sy_invsy[1];
        }
        else
        {
            *(pS_invS + 1) = *(fx32*)(pData + 0);
            *(pS_invS + 4) = *(fx32*)(pData + 1);

        }
        pData += 2;

        if (!(tag & NNS_G3D_JNTANM_SRTINFO_CONST_SZ))
        {
            // Call the function that extracts SZ
            // The data offset will be from pAnmSRTTag.
            fx32 sz_invsz[2];
            if (IsDecimalFrame)
            {
                getScaleDataEx_(&sz_invsz[0],
                                Frame,
                                pData,
                                pJntAnm);
            }
            else
            {
                getScaleData_(&sz_invsz[0],
                              Frame,
                              pData,
                              pJntAnm);
            }
            *(pS_invS + 2) = sz_invsz[0];
            *(pS_invS + 5) = sz_invsz[1];
        }
        else
        {
            *(pS_invS + 2) = *(fx32*)(pData + 0);
            *(pS_invS + 5) = *(fx32*)(pData + 1);
        }
        pData += 2;
    }
    else
    {
        if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_S)
        {
            // Scale = (1, 1, 1)
            pResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE;
        }
        else
        {
            // get the Scale of the model
            getMdlScale_(pResult);

            // Internal scaling info for NNSG3dJntAnmResult in getMdlScale_
            // is set, so return.
            return;
        }
    }
GET_JOINTSCALE:
    //
    // Set the NNSG3dJntAnmResult scaling info
    //
    (*NNS_G3dRS->funcJntScale)(
        pResult, 
        pS_invS,
        NNS_G3dRS->c,
        ((pResult->flag & NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE) ?
                NNS_G3D_SRTFLAG_SCALE_ONE :
                0) // Need to change the flag
    );
}


/*---------------------------------------------------------------------------*
    getTransData_

    Get the translation data and put it into *pVal
 *---------------------------------------------------------------------------*/
static void
getTransData_(fx32* pVal,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm)
{
    u32 frame = (u32)FX_Whole(Frame);
    void* pArray = (void*)((u8*)pJntAnm + *(pData + 1));
    NNSG3dJntAnmTInfo info = (NNSG3dJntAnmTInfo)*pData;
    u32 last_interp;
    u32 idx;
    u32 idx_sub;

    NNS_G3D_NULL_ASSERT(pVal);
    NNS_G3D_NULL_ASSERT(pArray);

    if (!(info & NNS_G3D_JNTANM_TINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_TINFO_STEP1 is selected
        idx = frame;
        goto TRANS_NONINTERP;
    }

    // data is input in each frame from last_interp
    // last_interp is either a multiple of 2 or 4
    last_interp = ((u32)info & NNS_G3D_JNTANM_TINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_TINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_TINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_TINFO_STEP_2 is selected
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // impossible if anything other than the last frame
                idx = (last_interp >> 1) + 1;
                goto TRANS_NONINTERP;
            }
            else
            {
                // Since the final frame is not on an odd number, 50:50 interpolation is needed
                idx = frame >> 1;
                goto TRANS_INTERP_2;
            }
        }
        else
        {
            // Even frame, so no interpolation is needed
            idx = frame >> 1;
            goto TRANS_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_TINFO_STEP_4 is being selected
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                idx = (last_interp >> 2) + (frame & 3);
                goto TRANS_NONINTERP;
            }

            // With interpolation management
            if (frame & 1)
            {
                fx32 v, v_sub;
                if (frame & 2)
                {
                    // Interpolate with 3:1 position
                    idx_sub = (frame >> 2);
                    idx = idx_sub + 1;
                }
                else
                {
                    // Interpolate with 1:3 position
                    idx = (frame >> 2);
                    idx_sub = idx + 1;
                }
                
                // Interpolation for when 1:3 and 3:1
                if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
                {
                    v = *((fx16*)pArray + idx);
                    v_sub = *((fx16*)pArray + idx_sub);
                    *pVal = (v + v + v + v_sub) >> 2;
                }
                else
                {
                    v = *((fx32*)pArray + idx);
                    v_sub = *((fx32*)pArray + idx_sub);
                    *pVal = (fx32)(((fx64)v + v + v + v_sub) >> 2);
                }
                return;
            }
            else
            {
                // becomes 50:50 interpolation
                idx = frame >> 2;
                goto TRANS_INTERP_2;
            }
        }
        else
        {
            // The frame is set to be an exact multiple of 4.
            idx = frame >> 2;
            goto TRANS_NONINTERP;
        }
    }
    NNS_G3D_ASSERT(0);
TRANS_INTERP_2:
    if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
    {
        *pVal = (*((fx16*)pArray + idx) + *((fx16*)pArray + idx + 1)) >> 1;
    }
    else
    {
        fx32 v1 = *((fx32*)pArray + idx) >> 1;
        fx32 v2 = *((fx32*)pArray + idx + 1) >> 1;
        *pVal = v1 + v2;
    }
    return;
TRANS_NONINTERP:
    if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
    {
        *pVal = *((fx16*)pArray + idx);
    }
    else
    {
        *pVal = *((fx32*)pArray + idx);
    }
    return;
}


/*---------------------------------------------------------------------------*
    getTransDataEx_

    Get the translation data, and put it in *pVal
    Interpolate when after the decimal point
 *---------------------------------------------------------------------------*/
static void
getTransDataEx_(fx32* pVal,
                fx32 Frame,
                const u32* pData,
                const NNSG3dResJntAnm* pJntAnm)
{
    // In this function,
    // 0 only frames greater than 0 and less than numFrame will appear.
    void* pArray = (void*)((u8*)pJntAnm + *(pData + 1));
    NNSG3dJntAnmTInfo info = (NNSG3dJntAnmTInfo)*pData;

    u32 last_interp;
    u32 idx;
    fx32 remainder;
    int step;
    u32 step_shift;
    u32 frame;

    NNS_G3D_NULL_ASSERT(pVal);
    NNS_G3D_NULL_ASSERT(pArray);

    frame = (u32)FX_Whole(Frame);

    if (frame == pJntAnm->numFrame - 1)
    {
        //
        // When the frame is between numFrame - 1 and numFrame
        // When exactly between the first and last frame
        //

        //
        // First, get the final frame data index
        //
        if (!(info & NNS_G3D_JNTANM_TINFO_STEP_MASK))
        {
            idx = frame;
        }
        else if (info & NNS_G3D_JNTANM_TINFO_STEP_2)
        {
            idx = (frame >> 1) + (frame & 1);
        }
        else
        {
            idx = (frame >> 2) + (frame & 3);
        }

        //
        // Use a flag to decide whether to extrapolate or to return the final data
        //
        if (pJntAnm->flag & NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION)
        {
            // when extrapolating, do it with the first and last data (loop)
            fx32 v0, v1;
            remainder = Frame & (FX32_ONE - 1);

            if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
            {
                v0 = *((fx16*)pArray + idx);
                v1 = *(fx16*)pArray;
            }
            else
            {
                v0 = *((fx32*)pArray + idx);
                v1 = *(fx32*)pArray;
            }

            *pVal = v0 + (((v1 - v0) * remainder) >> FX32_SHIFT);
            return;
        }
        else
        {
            // When returning the final data
            if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
            {
                *pVal = *((fx16*)pArray + idx);
            }
            else
            {
                *pVal = *((fx32*)pArray + idx);
            }
            return;
        }
    }

    if (!(info & NNS_G3D_JNTANM_TINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_TINFO_STEP1 is selected
        goto TRANS_EX_0;
    }

    // data is input in each frame from last_interp
    // last_interp is either a multiple of 2 or 4
    last_interp = ((u32)info & NNS_G3D_JNTANM_TINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_TINFO_LAST_INTERP_SHIFT;
    
    if (info & NNS_G3D_JNTANM_TINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_TINFO_STEP_2 is selected
        // The inclusion of the sign is due to consideration of the discarded data
        if (frame >= last_interp)
        {
            // impossible if anything other than the last frame
            idx = (last_interp >> 1);
            goto TRANS_EX_0_1;
        }
        else
        {
            idx = frame >> 1;
            remainder = Frame & (FX32_ONE * 2 - 1);
            step = 2;
            step_shift = 1;
            goto TRANS_EX;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_TINFO_STEP_4 is being selected
        if (frame >= last_interp)
        {
            // Due to the conditions under which this function is called, since the frame does not have to be the
            // final data, the boundary will not be exceeded in the code below
            idx = (frame >> 2) + (frame & 3);
            goto TRANS_EX_0_1;
        }
        else
        {
            idx = frame >> 2;
            remainder = Frame & (FX32_ONE * 4 - 1);
            step = 4;
            step_shift = 2;
            goto TRANS_EX;
        }
    }
    NNS_G3D_ASSERT(0);
TRANS_EX_0:
    idx = (u32)frame;
TRANS_EX_0_1:
    remainder = Frame & (FX32_ONE - 1);
    step = 1;
    step_shift = 0;
TRANS_EX:
    {
        fx32 v0, v1;
        if (info & NNS_G3D_JNTANM_TINFO_FX16ARRAY)
        {
            v0 = *((fx16*)pArray + idx);
            v1 = *((fx16*)pArray + idx + 1);
        }
        else
        {
            v0 = *((fx32*)pArray + idx);
            v1 = *((fx32*)pArray + idx + 1);
        }

        *pVal = ((v0 * step) + (((v1 - v0) * remainder) >> FX32_SHIFT)) >> step_shift;
    }
    return;
}


/*---------------------------------------------------------------------------*
    getScaleData_

    Get the scale and its inverse, then put them in s_invs[0] and s_invs[1]
 *---------------------------------------------------------------------------*/
static void
getScaleData_(fx32* s_invs,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm)
{
    u32 frame = (u32)FX_Whole(Frame);
    void* pArray = (void*)((u8*)pJntAnm + *(pData + 1));
    NNSG3dJntAnmSInfo info = (NNSG3dJntAnmSInfo)*pData;
    u32 last_interp;
    u32 idx;
    u32 idx_sub;

    NNS_G3D_NULL_ASSERT(s_invs);
    NNS_G3D_NULL_ASSERT(pArray);

    if (!(info & NNS_G3D_JNTANM_SINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_SINFO_STEP_1 is being selected
        idx = frame;
        goto SCALE_NONINTERP;
    }

    // data is input in each frame from last_interp
    // last_interp is either a multiple of 2 or 4
    last_interp = ((u32)info & NNS_G3D_JNTANM_SINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_SINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_SINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_SINFO_STEP_2 is selected
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // impossible if anything other than the last frame
                idx = (last_interp >> 1) + 1;
                goto SCALE_NONINTERP;
            }
            else
            {
                // Since the final frame is not on an odd number, 50:50 interpolation is needed
                idx = frame >> 1;
                goto SCALE_INTERP_2;
            }
        }
        else
        {
            // Even frame, so no interpolation is needed
            idx = frame >> 1;
            goto SCALE_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_TINFO_STEP_4 is being selected
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                // No interpolation starting from last_interp
                idx = (last_interp >> 2) + (frame & 3);
                goto SCALE_NONINTERP;
            }

            // With interpolation management
            if (frame & 1)
            {
                fx32 v, v_sub;
                if (frame & 2)
                {
                    // Interpolate with 3:1 position
                    idx_sub = (frame >> 2);
                    idx = idx_sub + 1;
                }
                else
                {
                    // Interpolate with 1:3 position
                    idx = (frame >> 2);
                    idx_sub = idx + 1;
                }
                
                if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
                {
                    v = *((fx16*)pArray + 2 * idx);
                    v_sub = *((fx16*)pArray + 2 * idx_sub);
                    s_invs[0] = (v + (v << 1) + v_sub) >> 2;

                    v = *((fx16*)pArray + 2 * idx + 1);
                    v_sub = *((fx16*)pArray + 2 * idx_sub + 1);
                    s_invs[1] = (v + (v << 1) + v_sub) >> 2;
                }
                else
                {
                    v = *((fx32*)pArray + 2 * idx);
                    v_sub = *((fx32*)pArray + 2 * idx_sub);
                    s_invs[0] = (fx32)(((fx64)v + v + v + v_sub) >> 2);

                    v = *((fx32*)pArray + 2 * idx + 1);
                    v_sub = *((fx32*)pArray + 2 * idx_sub + 1);
                    s_invs[1] = (fx32)(((fx64)v + v + v + v_sub) >> 2);
                }
                return;
            }
            else
            {
                // becomes 50:50 interpolation
                idx = frame >> 2;
                goto SCALE_INTERP_2;
            }
        }
        else
        {
            // The frame is set to be an exact multiple of 4.
            idx = frame >> 2;
            goto SCALE_NONINTERP;
        }
    }
SCALE_NONINTERP:
    if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
    {
        s_invs[0] = *((fx16*)pArray + 2 * idx);
        s_invs[1] = *((fx16*)pArray + 2 * idx + 1);
    }
    else
    {
        s_invs[0] = *((fx32*)pArray + 2 * idx);
        s_invs[1] = *((fx32*)pArray + 2 * idx + 1);
    }
    return;
SCALE_INTERP_2:
    if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
    {
        s_invs[0] = (*((fx16*)pArray + 2 * idx) + *((fx16*)pArray + 2 * idx + 2)) >> 1;
        s_invs[1] = (*((fx16*)pArray + 2 * idx + 1) + *((fx16*)pArray + 2 * idx + 3)) >> 1;
    }
    else
    {
        s_invs[0] = (*((fx32*)pArray + 2 * idx) + (*((fx32*)pArray + 2 * idx + 2))) >> 1;
        s_invs[1] = (*((fx32*)pArray + 2 * idx + 1) + (*((fx32*)pArray + 2 * idx + 3))) >> 1;
    }
    return;
}


/*---------------------------------------------------------------------------*
    getScaleDataEx_

    Get the scale and its inverse, then put them in s_invs[0] and s_invs[1]
    Interpolate when after the decimal point
 *---------------------------------------------------------------------------*/
static void
getScaleDataEx_(fx32* s_invs,
                fx32 Frame,
                const u32* pData,
                const NNSG3dResJntAnm* pJntAnm)
{
    void* pArray = (void*)((u8*)pJntAnm + *(pData + 1));
    NNSG3dJntAnmSInfo info = (NNSG3dJntAnmSInfo)*pData;
    u32 last_interp;
    u32 idx0, idx1;
    fx32 remainder;
    int step;
    u32 step_shift;
    u32 frame;

    frame = (u32)FX_Whole(Frame);

    if (frame == pJntAnm->numFrame - 1)
    {
        //
        // When the frame is between numFrame - 1 and numFrame
        // When exactly between the first and last frame
        //

        //
        // First, get the final frame data index
        //
        if (!(info & NNS_G3D_JNTANM_SINFO_STEP_MASK))
        {
            idx0 = frame;
        }
        else if (info & NNS_G3D_JNTANM_SINFO_STEP_2)
        {
            idx0 = (frame >> 1) + (frame & 1);
        }
        else
        {
            idx0 = (frame >> 2) + (frame & 3);
        }

        //
        // Use a flag to decide whether to extrapolate or to return the final data
        //
        if (pJntAnm->flag & NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION)
        {
            idx1 = 0;
            goto SCALE_EX_0_1;
        }
        else
        {
            if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
            {
                s_invs[0] = *((fx16*)pArray + 2 * idx0);
                s_invs[1] = *((fx16*)pArray + 2 * idx0 + 1);
            }
            else
            {
                s_invs[0] = *((fx32*)pArray + 2 * idx0);
                s_invs[1] = *((fx32*)pArray + 2 * idx0 + 1);
            }
            return;
        }
    }

    if (!(info & NNS_G3D_JNTANM_SINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_SINFO_STEP_1 is being selected
        goto SCALE_EX_0;
    }

    last_interp = ((u32)info & NNS_G3D_JNTANM_SINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_SINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_SINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_SINFO_STEP_2 is selected
        // The inclusion of the sign is due to consideration of the discarded data
        if (frame >= last_interp)
        {
            // impossible if anything other than the last frame
            idx0 = (last_interp >> 1);
            idx1 = idx0 + 1;
            goto SCALE_EX_0_1;
        }
        else
        {
            idx0 = frame >> 1;
            idx1 = idx0 + 1;
            remainder = Frame & (FX32_ONE * 2 - 1);
            step = 2;
            step_shift = 1;
            goto SCALE_EX;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_SINFO_STEP_4 is selected
        if (frame >= last_interp)
        {
            // Due to the conditions under which this function is called, since the frame does not have to be the
            // final data, the boundary will not be exceeded in the code below
            idx0 = (frame >> 2) + (frame & 3);
            idx1 = idx0 + 1;
            goto SCALE_EX_0_1;
        }
        else
        {
            idx0 = frame >> 2;
            idx1 = idx0 + 1;
            remainder = Frame & (FX32_ONE * 4 - 1);
            step = 4;
            step_shift = 2;
            goto SCALE_EX;
        }
    }
    NNS_G3D_ASSERT(0);
SCALE_EX_0:
    idx0 = (u32)frame;
    idx1 = idx0 + 1;
SCALE_EX_0_1:
    remainder = Frame & (FX32_ONE - 1);
    step = 1;
    step_shift = 0;
SCALE_EX:
    {
        fx32 s0, s1;
        fx32 inv0, inv1;

        if (info & NNS_G3D_JNTANM_SINFO_FX16ARRAY)
        {
            s0   = *((fx16*)pArray + 2 * idx0);
            inv0 = *((fx16*)pArray + 2 * idx0 + 1);

            s1   = *((fx16*)pArray + 2 * idx1);
            inv1 = *((fx16*)pArray + 2 * idx1 + 1);
        }
        else
        {
            s0   = *((fx32*)pArray + 2 * idx0);
            inv0 = *((fx32*)pArray + 2 * idx0 + 1);

            s1   = *((fx32*)pArray + 2 * idx1);
            inv1 = *((fx32*)pArray + 2 * idx1 + 1);
        }

        s_invs[0] = ((s0 * step) + (((s1 - s0) * remainder) >> FX32_SHIFT)) >> step_shift;
        s_invs[1] = ((inv0 * step) + (((inv1 - inv0) * remainder) >> FX32_SHIFT)) >> step_shift;
    }

    return;
}



// When the interpolation process between frames is necessary, only the linear interpolation of matrices is performed
// if G3D_NORMALIZE_ROT_MTX is invalid and the shape of the model may be deformed
// when the rotation angle between keyframes is large.
// If G3D_NORMALIZE_ROT_MTX is valid, the model is normalized to
// reduce distortion
#define G3D_NORMALIZE_ROT_MTX

#ifdef G3D_NORMALIZE_ROT_MTX
#define ROT_FILTER_SHIFT    0
#else
#define ROT_FILTER_SHIFT    1
#endif

/*---------------------------------------------------------------------------*
    getRotData_

    Obtains the rotation matrix and sets to *pRot
 *---------------------------------------------------------------------------*/
static void
getRotData_(MtxFx33* pRot,
            fx32 Frame,
            const u32* pData,
            const NNSG3dResJntAnm* pJntAnm)
{
    u32 frame = (u32)FX_Whole(Frame);
    void* pArray = (void*)((u8*)pJntAnm + *(pData + 1));
    void* pArrayRot3 = (void*)((u8*)pJntAnm + pJntAnm->ofsRot3);
    void* pArrayRot5 = (void*)((u8*)pJntAnm + pJntAnm->ofsRot5);
    NNSG3dJntAnmRInfo info = (NNSG3dJntAnmRInfo)*pData;
    u16* p = (u16*)pArray;

    u32 last_interp;
    u32 idx;
    u32 idx_sub;

    if (!(info & NNS_G3D_JNTANM_RINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_RINFO_STEP_1is selected
        idx = frame;
        goto ROT_NONINTERP;
    }

    last_interp = ((u32)info & NNS_G3D_JNTANM_RINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_RINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_RINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_RINFO_STEP_2 is selected
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // impossible if anything other than the last frame
                idx = (last_interp >> 1) + 1;
                goto ROT_NONINTERP;
            }
            else
            {
                // Since the final frame is not on an odd number, 50:50 interpolation is needed
                idx = frame >> 1;
                goto ROT_INTERP_2;
            }
        }
        else
        {
            // Even frame, so no interpolation is needed
            idx = frame >> 1;
            goto ROT_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_RINFO_STEP_4 is selected
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                idx = (last_interp >> 2) + (frame & 3);
                goto ROT_NONINTERP;
            }

            // With interpolation management
            if (frame & 1)
            {
                MtxFx33 tmp;
                BOOL    doCross = FALSE;
                if (frame & 2)
                {
                    // Interpolate with 3:1 position
                    idx_sub = (frame >> 2);
                    idx = idx_sub + 1;
                }
                else
                {
                    // Interpolate with 1:3 position
                    idx = (frame >> 2);
                    idx_sub = idx + 1;
                }
                doCross |= getRotDataByIdx_(pRot,
                                            pArrayRot3,
                                            pArrayRot5,
                                            (NNSG3dJntAnmRIdx)p[idx]);
                doCross |= getRotDataByIdx_(&tmp,
                                            pArrayRot3,
                                            pArrayRot5,
                                            (NNSG3dJntAnmRIdx)p[idx_sub]);
                pRot->_00 = (pRot->_00 * 3 + tmp._00) >> (2 * ROT_FILTER_SHIFT);
                pRot->_01 = (pRot->_01 * 3 + tmp._01) >> (2 * ROT_FILTER_SHIFT);
                pRot->_02 = (pRot->_02 * 3 + tmp._02) >> (2 * ROT_FILTER_SHIFT);
                pRot->_10 = (pRot->_10 * 3 + tmp._10) >> (2 * ROT_FILTER_SHIFT);
                pRot->_11 = (pRot->_11 * 3 + tmp._11) >> (2 * ROT_FILTER_SHIFT);
                pRot->_12 = (pRot->_12 * 3 + tmp._12) >> (2 * ROT_FILTER_SHIFT);
                
#ifdef G3D_NORMALIZE_ROT_MTX
                VEC_Normalize( (VecFx32*)(&pRot->_00), (VecFx32*)(&pRot->_00) );
                VEC_Normalize( (VecFx32*)(&pRot->_10), (VecFx32*)(&pRot->_10) );
#endif
                
                if (!doCross)
                {
                    // If both areRot3 format data,
                    // skip calculation of cross product.
                    pRot->_20 = (pRot->_20 * 3 + tmp._20) >> (2 * ROT_FILTER_SHIFT);
                    pRot->_21 = (pRot->_21 * 3 + tmp._21) >> (2 * ROT_FILTER_SHIFT);
                    pRot->_22 = (pRot->_22 * 3 + tmp._22) >> (2 * ROT_FILTER_SHIFT);

#ifdef G3D_NORMALIZE_ROT_MTX
                    VEC_Normalize( (VecFx32*)(&pRot->_20), (VecFx32*)(&pRot->_20) );
#endif
                }
                else
                {
                    vecCross_((const VecFx32*)&pRot->_00,
                              (const VecFx32*)&pRot->_10,
                              (VecFx32*)&pRot->_20);
                }
                return;
            }
            else
            {
                // becomes 50:50 interpolation
                idx = frame >> 2;
                goto ROT_INTERP_2;
            }
        }
        else
        {
            idx = frame >> 2;
            goto ROT_NONINTERP;
        }
    }
ROT_INTERP_2:
    {
        MtxFx33 tmp;
        BOOL    doCross = FALSE;
        doCross |= getRotDataByIdx_(pRot,
                                    pArrayRot3,
                                    pArrayRot5,
                                    (NNSG3dJntAnmRIdx)p[idx]);
        doCross |= getRotDataByIdx_(&tmp,
                                    pArrayRot3,
                                    pArrayRot5,
                                    (NNSG3dJntAnmRIdx)p[idx + 1]);
        pRot->_00 = (pRot->_00 + tmp._00) >> ROT_FILTER_SHIFT;
        pRot->_01 = (pRot->_01 + tmp._01) >> ROT_FILTER_SHIFT;
        pRot->_02 = (pRot->_02 + tmp._02) >> ROT_FILTER_SHIFT;
        pRot->_10 = (pRot->_10 + tmp._10) >> ROT_FILTER_SHIFT;
        pRot->_11 = (pRot->_11 + tmp._11) >> ROT_FILTER_SHIFT;
        pRot->_12 = (pRot->_12 + tmp._12) >> ROT_FILTER_SHIFT;
        
#ifdef G3D_NORMALIZE_ROT_MTX
        VEC_Normalize( (VecFx32*)(&pRot->_00), (VecFx32*)(&pRot->_00) );
        VEC_Normalize( (VecFx32*)(&pRot->_10), (VecFx32*)(&pRot->_10) );
#endif
        
        if (!doCross)
        {
            pRot->_20 = (pRot->_20 + tmp._20) >> ROT_FILTER_SHIFT;
            pRot->_21 = (pRot->_21 + tmp._21) >> ROT_FILTER_SHIFT;
            pRot->_22 = (pRot->_22 + tmp._22) >> ROT_FILTER_SHIFT;
            
#ifdef G3D_NORMALIZE_ROT_MTX
            VEC_Normalize( (VecFx32*)(&pRot->_20), (VecFx32*)(&pRot->_20) );
#endif
        }
        else
        {
            vecCross_((const VecFx32*)&pRot->_00,
                      (const VecFx32*)&pRot->_10,
                      (VecFx32*)&pRot->_20);
        }
        return;
    }
ROT_NONINTERP:
    if (getRotDataByIdx_(pRot,
                         pArrayRot3,
                         pArrayRot5,
                         (NNSG3dJntAnmRIdx)p[idx]))
    {
        vecCross_((const VecFx32*)&pRot->_00,
                  (const VecFx32*)&pRot->_10,
                  (VecFx32*)&pRot->_20);
    }
    return;
}


/*---------------------------------------------------------------------------*
    getRotData_

    Obtains the rotation matrix, and sets to *pRot.
    Interpolate when after the decimal point
 *---------------------------------------------------------------------------*/
static void
getRotDataEx_(MtxFx33* pRot,
              fx32 Frame,
              const u32* pData,
              const NNSG3dResJntAnm* pJntAnm)
{
    // In this function,
    // 0 only frames greater than 0 and less than numFrame will appear.

    void* pArray = (void*)((u8*)pJntAnm + *(pData + 1));
    void* pArrayRot3 = (void*)((u8*)pJntAnm + pJntAnm->ofsRot3);
    void* pArrayRot5 = (void*)((u8*)pJntAnm + pJntAnm->ofsRot5);
    NNSG3dJntAnmRInfo info = (NNSG3dJntAnmRInfo)*pData;

    u32 last_interp;
    u32 idx0, idx1;
    fx32 remainder;
    int step;
    u32 step_shift;
    u32 frame;
    u16* p = (u16*)pArray;

    frame = (u32)FX_Whole(Frame);

    if (frame == pJntAnm->numFrame - 1)
    {
        //
        // When the frame is between numFrame - 1 and numFrame
        // When exactly between the first and last frame
        //

        //
        // First, get the final frame data index
        //
        if (!(info & NNS_G3D_JNTANM_RINFO_STEP_MASK))
        {
            idx0 = frame;
        }
        else if (info & NNS_G3D_JNTANM_RINFO_STEP_2)
        {
            idx0 = (frame >> 1) + (frame & 1);
        }
        else
        {
            idx0 = (frame >> 2) + (frame & 3);
        }

        //
        // Use a flag to decide whether to extrapolate or to return the final data
        //
        if (pJntAnm->flag & NNS_G3D_JNTANM_OPTION_END_TO_START_INTERPOLATION)
        {
            // when extrapolating, do it with the first and last data (loop)
            idx1 = 0;
            goto ROT_EX_0_1;
        }
        else
        {
            // When returning the final data
            if (getRotDataByIdx_(pRot,
                                 pArrayRot3,
                                 pArrayRot5,
                                 (NNSG3dJntAnmRIdx)p[idx0]))
            {
                vecCross_((const VecFx32*)&pRot->_00,
                          (const VecFx32*)&pRot->_10,
                          (VecFx32*)&pRot->_20);
            }
            return;
        }
    }

    if (!(info & NNS_G3D_JNTANM_RINFO_STEP_MASK))
    {
        // NNS_G3D_JNTANM_RINFO_STEP_1is selected
        goto ROT_EX_0;
    }

    last_interp = ((u32)info & NNS_G3D_JNTANM_RINFO_LAST_INTERP_MASK) >>
                                    NNS_G3D_JNTANM_RINFO_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_JNTANM_RINFO_STEP_2)
    {
        // NNS_G3D_JNTANM_RINFO_STEP_2 is selected
        // The inclusion of the sign is due to consideration of the discarded data
        if (frame >= last_interp)
        {
            // impossible if anything other than the last frame
            idx0 = (last_interp >> 1);
            idx1 = idx0 + 1;
            goto ROT_EX_0_1;
        }
        else
        {
            idx0 = frame >> 1;
            idx1 = idx0 + 1;
            remainder = Frame & (FX32_ONE * 2 - 1);
            step = 2;
            step_shift = 1;
            goto ROT_EX;
        }
    }
    else
    {
        // NNS_G3D_JNTANM_RINFO_STEP_4 is selected
        if (frame >= last_interp)
        {
            // Due to the conditions under which this function is called, since the frame does not have to be the
            // final data, the boundary will not be exceeded in the code below
            idx0 = (frame >> 2) + (frame & 3);
            idx1 = idx0 + 1;
            goto ROT_EX_0_1;
        }
        else
        {
            idx0 = frame >> 2;
            idx1 = idx0 + 1;
            remainder = Frame & (FX32_ONE * 4 - 1);
            step = 4;
            step_shift = 2;
            goto ROT_EX;
        }
    }
    NNS_G3D_ASSERT(0);
ROT_EX_0:
    idx0 = (u32)frame;
    idx1 = idx0 + 1;
ROT_EX_0_1:
    remainder = Frame & (FX32_ONE - 1);
    step = 1;
    step_shift = 0;
ROT_EX:
    {
        MtxFx33 r0, r1;
        BOOL doCross = FALSE;
        doCross |= getRotDataByIdx_(&r0,
                                    pArrayRot3,
                                    pArrayRot5,
                                    (NNSG3dJntAnmRIdx)p[idx0]);
        doCross |= getRotDataByIdx_(&r1,
                                    pArrayRot3,
                                    pArrayRot5,
                                    (NNSG3dJntAnmRIdx)p[idx1]);

        pRot->_00 = ((r0._00 * step) + (((r1._00 - r0._00) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_01 = ((r0._01 * step) + (((r1._01 - r0._01) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_02 = ((r0._02 * step) + (((r1._02 - r0._02) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_10 = ((r0._10 * step) + (((r1._10 - r0._10) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_11 = ((r0._11 * step) + (((r1._11 - r0._11) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
        pRot->_12 = ((r0._12 * step) + (((r1._12 - r0._12) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);

#ifdef G3D_NORMALIZE_ROT_MTX
        VEC_Normalize( (VecFx32*)(&pRot->_00), (VecFx32*)(&pRot->_00) );
        VEC_Normalize( (VecFx32*)(&pRot->_10), (VecFx32*)(&pRot->_10) );
#endif


        if (!doCross)
        {
            // If both areRot3 format data,
            // skip calculation of cross product.
            pRot->_20 = ((r0._20 * step) + (((r1._20 - r0._20) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
            pRot->_21 = ((r0._21 * step) + (((r1._21 - r0._21) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);
            pRot->_22 = ((r0._22 * step) + (((r1._22 - r0._22) * remainder) >> FX32_SHIFT)) >> (step_shift * ROT_FILTER_SHIFT);

#ifdef G3D_NORMALIZE_ROT_MTX
            VEC_Normalize( (VecFx32*)(&pRot->_20), (VecFx32*)(&pRot->_20) );
#endif
        }
        else
        {
            vecCross_((const VecFx32*)&pRot->_00,
                      (const VecFx32*)&pRot->_10,
                      (VecFx32*)&pRot->_20);
        }
        return;
    }
}


/*---------------------------------------------------------------------------*
    getRotDataByIdx_

    Takes the data of Rot3 or Rot5 from index, and stores in pRot
 *---------------------------------------------------------------------------*/
static BOOL
getRotDataByIdx_(MtxFx33* pRot,
                 void* pArrayRot3,
                 void* pArrayRot5,
                 NNSG3dJntAnmRIdx info)
{
    if (info & NNS_G3D_JNTANM_RIDX_PIVOT)
    {
        fx16* data;
        fx32 A, B;
        u32 idxPivot;

        pRot->_00 = pRot->_01 = pRot->_02 =
        pRot->_10 = pRot->_11 = pRot->_12 =
        pRot->_20 = pRot->_21 = pRot->_22 = 0;

        // data[0] NNSG3dJntAnmPivotInfo flag data
        // non-zero elements of data[1] and data[2] matrices
        data = (fx16*)pArrayRot3 + ((info & NNS_G3D_JNTANM_RIDX_IDXDATA_MASK) * 3);
        A = *(data + 1);
        B = *(data + 2);
        idxPivot = (u32)( (*data & NNS_G3D_JNTANM_PIVOTINFO_IDXPIVOT_MASK) >>
                                 NNS_G3D_JNTANM_PIVOT_INFO_IDXPIVOT_SHIFT );

        pRot->a[idxPivot] =
            (*data & NNS_G3D_JNTANM_PIVOTINFO_MINUS) ? -FX32_ONE : FX32_ONE;

        pRot->a[pivotUtil_[idxPivot][0]] = A;
        pRot->a[pivotUtil_[idxPivot][1]] = B;

        pRot->a[pivotUtil_[idxPivot][2]] =
            (*data & NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVC) ? -B : B;
        pRot->a[pivotUtil_[idxPivot][3]] =
            (*data & NNS_G3D_JNTANM_PIVOTINFO_SIGN_REVD) ? -A : A;

        // Filled up to the third line
        return FALSE;
    }
    else
    {
        // Brings the data from Rot5
        fx16* data = (fx16*)pArrayRot5 +
                     ((info & NNS_G3D_JNTANM_RIDX_IDXDATA_MASK) * 5);
        fx16 _12;

        _12 = (fx16)(data[4] & 7);
        pRot->_11 = (data[4] >> 3);

        _12 = (fx16)((_12 << 3) | (data[0] & 7));
        pRot->_00 = (data[0] >> 3);

        _12 = (fx16)((_12 << 3) | (data[1] & 7));
        pRot->_01 = (data[1] >> 3);

        _12 = (fx16)((_12 << 3) | (data[2] & 7));
        pRot->_02 = (data[2] >> 3);

        _12 = (fx16)((_12 << 3) | (data[3] & 7));
        pRot->_10 = (data[3] >> 3);

        // Cast the value shifted so that it is sign extended to fx16
        // and cast to fx32 implicitly
        pRot->_12 = ((fx16)(_12 << 3) >> 3);

        // The third line must be obtained by cross product
        return TRUE;
    }
}



