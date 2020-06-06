/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     nsbma.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nsbma.c,v $
  Revision 1.16  11/09/2004 02:15:48  nishida_kenji
  fix comments.

  Revision 1.15  10/22/2004 00:38:38  nishida_kenji
  bugfix(ver. 10/12 alpha animation).

  Revision 1.14  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.13  09/17/2004 07:46:55  nishida_kenji
  performance tuning(including small bug fix)

  Revision 1.12  08/04/2004 07:53:03  kitani_toshikazu
  Changed the interpolation computation method in order to avoid  rounding-down errors

  Revision 1.11  08/04/2004 02:32:05  kitani_toshikazu
  Fixed the bug that caused the interpolation flag for decimal frames  to be incorrectly interpreted as the interpolation flag for integer  cull frames.

  Revision 1.10  08/03/2004 08:14:48  kitani_toshikazu
  Fixed bug having to do with part that fetches alpha value

  Revision 1.9  08/03/2004 01:01:04  kitani_toshikazu
  Cleaned up the code

  Revision 1.8  08/02/2004 12:31:16  kitani_toshikazu
  Implemented a part for fetching the animation result

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/anm/nsbma.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>


//---------------------------------------------------------------------
static u16
GetMatColAnmValue_(const NNSG3dResMatCAnm* pAnm, u32 info, u32 frame)
{
    const u16* pDataHead;
    u32 last_interp;
    u32 idx;

    if (info & NNS_G3D_MATCANM_ELEM_CONST)
    {
        return (u16)(info & NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_MASK);
    }

    pDataHead = (const u16*)((u8*)pAnm + (info & NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_MASK));
    
    if (!(info & NNS_G3D_MATCANM_ELEM_STEP_MASK))
    {
        // NNS_G3D_MATCANM_ELEM_STEP_1 is selected
        return *(pDataHead + frame);
    }
    
    //
    // When it is not a constant, but NNS_G3D_MATCANM_ELEM_STEP_2 or NNS_G3D_MATCANM_ELEM_STEP_4
    //
    
    // data is input in each frame from last_interp
    // last_interp is either a multiple of 2 or 4
    last_interp = (NNS_G3D_MATCANM_ELEM_LAST_INTERP_MASK & info)
                          >> NNS_G3D_MATCANM_ELEM_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_MATCANM_ELEM_STEP_2)
    {
        // NNS_G3D_MATCANM_ELEM_STEP_2 is selected
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // impossible if anything other than the last frame
                return *(pDataHead + (last_interp >> 1) + 1);
            }
            else
            {
                // Since the final frame is not on an odd number, 50:50 interpolation is needed
                idx = frame >> 1;
                goto MATC_INTERP_50_50;
            }
        }
        else
        {
            // Even frame, so no interpolation is needed
            return *(pDataHead + (frame >> 1));
        }
    }
    else
    {
        // NNS_G3D_MATCANM_ELEM_STEP_4 is selected
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                return *(pDataHead + (last_interp >> 2) + (frame & 3));
            }

            // With interpolation management
            if (frame & 1)
            {
                u32 idx_sub;
                u32 v, v_sub;
                u32 c, c_sub;
                u32 rval;
                const u32 cmask0 = 0x7c1f;
                const u32 cmask1 = 0x03e0;

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
                v = *(pDataHead + idx);
                v_sub = *(pDataHead + idx_sub);

                c = v & cmask0;
                c_sub = v_sub & cmask0;
                rval = ((c + c + c + c_sub) >> 2) & cmask0;

                c = v & cmask1;
                c_sub = v_sub & cmask1;
                rval |= ((c + c + c + c_sub) >> 2) & cmask1;

                return (u16)rval;
            }
            else
            {
                // It becomes the 50:50 interpolation.
                idx = frame >> 2;
                goto MATC_INTERP_50_50;
            }
        }
        else
        {
            // Frames are exactly the multiples of 4.
            return *(pDataHead + (frame >> 2));
        }
    }
    NNS_G3D_ASSERT(0);
MATC_INTERP_50_50:
    {
        u32 rval;
        u32 v0 = *(pDataHead + idx);
        u32 v1 = *(pDataHead + idx + 1);
        const u32 cmask0 = 0x7c1f;
        const u32 cmask1 = 0x03e0;

        // red & blue
        rval = (((v0 & cmask0) + (v1 & cmask0)) >> 1) & cmask0;
        // green
        rval |= (((v0 & cmask1) + (v1 & cmask1)) >> 1) & cmask1;

        return (u16) rval;
    }
}


//------------------------------------------------------------------------------
static u16
GetMatColAnmuAlphaValue_(const NNSG3dResMatCAnm* pAnm, 
                         u32 info, 
                         u32 frame)
{
    const u8* pDataHead;
    u32 last_interp;
    
    if( info & NNS_G3D_MATCANM_ELEM_CONST )
    {
        return (u16)(info & NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_MASK);
    }

    pDataHead = (const u8*)((u8*)pAnm + (info & NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_MASK));

    if (!(info & NNS_G3D_MATCANM_ELEM_STEP_MASK))
    {
        // NNS_G3D_MATCANM_ELEM_STEP_1 is selected
        return *(pDataHead + frame);
    }

    //
    // When it is not a constant, but NNS_G3D_MATCANM_ELEM_STEP_2 or NNS_G3D_MATCANM_ELEM_STEP_4
    //
    
    // data is input in each frame from last_interp
    // last_interp is either a multiple of 2 or 4
    last_interp = (NNS_G3D_MATCANM_ELEM_LAST_INTERP_MASK & info)
                          >> NNS_G3D_MATCANM_ELEM_LAST_INTERP_SHIFT;
    if (info & NNS_G3D_MATCANM_ELEM_STEP_2)
    {
        // NNS_G3D_MATCANM_ELEM_STEP_2 is selected
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // impossible if anything other than the last frame
                return *(pDataHead + (last_interp >> 1) + 1);
            }
            else
            {
                // Since the final frame is not on an odd number, 50:50 interpolation is needed
                return (u16)((*(pDataHead + (frame >> 1)) + *(pDataHead + (frame >> 1) + 1)) >> 1);
            }
        }
        else
        {
            // Even frame, so no interpolation is needed
            return *(pDataHead + (frame >> 1));
        }
    }
    else
    {
        // NNS_G3D_MATCANM_ELEM_STEP_4 is selected
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                return *(pDataHead + (last_interp >> 2) + (frame & 3));
            }

            // With interpolation management
            if (frame & 1)
            {
                u32 idx, idx_sub;
                u32 v, v_sub;

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
                v = *(pDataHead + idx);
                v_sub = *(pDataHead + idx_sub);

                return (u16)((v + v + v + v_sub) >> 2);
            }
            else
            {
                // It becomes the 50:50 interpolation.
                return (u16)((*(pDataHead + (frame >> 2)) + *(pDataHead + (frame >> 2) + 1)) >> 1);
            }
        }
        else
        {
            // Frames are exactly the multiples of 4.
            return *(pDataHead + (frame >> 2));
        }
    }
    NNS_G3D_ASSERT(0);
}


/*---------------------------------------------------------------------------*
  Name:         GetMatColAnm_

  Description:  Gets the animation results for the frames having the specified numbers when using material color animation.
                 
                
  Arguments:    pAnm      :      material color  animation
                idx         :      specified material number
                frame:      specified frame
                pResult:      Animation Result
                
  Returns:      material color animation
  
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
GetMatColAnm_(const NNSG3dResMatCAnm* pAnm, 
              u16 idx, 
              u32 frame, 
              NNSG3dMatAnmResult* pResult)
{
    NNS_G3D_NULL_ASSERT( pAnm );
    NNS_G3D_ASSERT( idx < pAnm->dict.numEntry );
    NNS_G3D_ASSERT( frame < pAnm->numFrame );
    NNS_G3D_NULL_ASSERT( pResult );
    NNS_G3D_ASSERT( (pAnm->anmHeader.category0 == 'M') && (pAnm->anmHeader.category1 == 'MA') );
    
    {
        u16 diffuse, ambient, emission, specular, polygon_alpha;
        const NNSG3dResDictMatCAnmData*  pAnmData = 
           (const NNSG3dResDictMatCAnmData*)NNS_G3dGetResDataByIdx( &pAnm->dict, idx );

        // diffuse ambient
        diffuse = GetMatColAnmValue_(pAnm, pAnmData->diffuse, frame);
        ambient = GetMatColAnmValue_(pAnm, pAnmData->ambient, frame);
        pResult->prmMatColor0 = (pResult->prmMatColor0 & 0x00008000) |
                                diffuse                              |
                                (ambient << 16);

        // specular emission
        emission = GetMatColAnmValue_(pAnm, pAnmData->emission, frame);
        specular = GetMatColAnmValue_(pAnm, pAnmData->specular, frame);
        pResult->prmMatColor1 = (pResult->prmMatColor1 & 0x00008000) |
                                specular                             |
                                (emission << 16);

        // Alpha
        polygon_alpha = GetMatColAnmuAlphaValue_( pAnm, pAnmData->polygon_alpha, frame );
        pResult->prmPolygonAttr = (pResult->prmPolygonAttr & ~REG_G3_POLYGON_ATTR_ALPHA_MASK) |
                                  (polygon_alpha << REG_G3_POLYGON_ATTR_ALPHA_SHIFT);
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmObjInitNsBma

    Initializes NNSG3dAnmObj for .nsbma resources.
    This is called from NNS_G3dInitAnmObj.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dAnmObjInitNsBma(NNSG3dAnmObj* pAnmObj,
                        void* pResAnm,
                        const NNSG3dResMdl* pResMdl)
{
    u32 i;
    NNSG3dResMatCAnm* matCAnm;
    const NNSG3dResMat* mat;

    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pResAnm);
    NNS_G3D_NULL_ASSERT(pResMdl);

    matCAnm = (NNSG3dResMatCAnm*)pResAnm;
    mat = NNS_G3dGetMat(pResMdl);
    pAnmObj->funcAnm = (void*) NNS_G3dFuncAnmMatNsBmaDefault;
    pAnmObj->numMapData = pResMdl->info.numMat;

    // Zero clear the mapData first
    MI_CpuClear16(&pAnmObj->mapData[0], sizeof(u16) * pAnmObj->numMapData);

    for (i = 0; i < matCAnm->dict.numEntry; ++i)
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&matCAnm->dict, i);
        int idx = NNS_G3dGetMatIdxByName(mat, name);
        if (!(idx < 0))
        {
            // When the resource for resource ID i exists,
            // associate the material ID idx with the resource ID i.
            pAnmObj->mapData[idx] = (u16)(i | NNS_G3D_ANMOBJ_MAPDATA_EXIST);
        }
        
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmCalcNsBma

    pResult: Stores the material animation result.
    pAnmObj:
    dataIdx: Index that indicates the storage location for the data in the resource.
 *---------------------------------------------------------------------------*/
void NNSi_G3dAnmCalcNsBma(NNSG3dMatAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx)
{
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);
    {
        const NNSG3dResMatCAnm*     pMatAnm 
                = (const NNSG3dResMatCAnm*)pAnmObj->resAnm;
        //
        // Obtains the animation result
        //
        GetMatColAnm_( pMatAnm,
                       (u16)dataIdx,                    // TODO: argument type needs to match
                       (u32)FX_Whole( pAnmObj->frame ),
                       pResult );
    }
}

