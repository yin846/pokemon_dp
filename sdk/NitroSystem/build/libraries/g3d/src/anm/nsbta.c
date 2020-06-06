/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     nsbva.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nsbta.c,v $
  Revision 1.16  11/09/2004 02:15:48  nishida_kenji
  fix comments.

  Revision 1.15  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.14  09/22/2004 05:36:34  nishida_kenji
  performance tuning(including bug fix)

  Revision 1.13  08/04/2004 07:53:03  kitani_toshikazu
  Changed the interpolation computation method in order to avoid  rounding-down errors

  Revision 1.12  08/04/2004 02:32:04  kitani_toshikazu
  Fixed the bug that caused the interpolation flag for decimal frames  to be incorrectly interpreted as the interpolation flag for integer  cull frames.

  Revision 1.11  08/03/2004 08:32:55  kitani_toshikazu
  Described judgment of parameter default values and portions that reflect on flags

  Revision 1.10  08/03/2004 08:15:01  kitani_toshikazu
  Cleaned up the code

  Revision 1.9  08/03/2004 01:01:04  kitani_toshikazu
  Cleaned up the code

  Revision 1.8  08/02/2004 12:31:12  kitani_toshikazu
  Implemented a part for fetching the animation result

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/anm/nsbta.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>


//---------------------------------------------------------------------
static fx32
GetTexSRTAnmVectorVal_(const NNSG3dResTexSRTAnm* pTexAnm, 
                       u32 info, 
                       u32 data, 
                       u32 frame)
{
    u32 idx, idx_sub;
    u32 last_interp;
    const void* pDataHead;

    if( info & NNS_G3D_TEXSRTANM_ELEM_CONST )
    {
        return (fx32)data;
    }

    pDataHead = (const void*)((u8*)pTexAnm + data);

    if (!(info & NNS_G3D_TEXSRTANM_ELEM_STEP_MASK))
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_1 is selected
        idx = frame;
        goto TEXSRT_VAL_NONINTERP;
    }

    // data is input in each frame from last_interp
    // last_interp is either a multiple of 2 or 4
    last_interp = (NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_MASK & info) >>
                                NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_TEXSRTANM_ELEM_STEP_2)
    {
        // NNS_G3D_MATCANM_ELEM_STEP_2 is selected
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // impossible if anything other than the last frame
                idx = (last_interp >> 1) + 1;
                goto TEXSRT_VAL_NONINTERP;
            }
            else
            {
                // Since the final frame is not on an odd number, 50:50 interpolation is needed
                idx = frame >> 1;
                goto TEXSRT_VAL_INTERP_2;
            }
        }
        else
        {
            // Even frame, so no interpolation is needed
            idx = frame >> 1;
            goto TEXSRT_VAL_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_4 is selected
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                // No interpolation starting from last_interp
                idx = (last_interp >> 2) + (frame & 3);
                goto TEXSRT_VAL_NONINTERP;
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

                if (info & NNS_G3D_TEXSRTANM_ELEM_FX16)
                {
                    v = *((const fx16*)pDataHead + idx);
                    v_sub = *((const fx16*)pDataHead + idx_sub);
                }
                else
                {
                    v = *((const fx32*)pDataHead + idx);
                    v_sub = *((const fx32*)pDataHead + idx_sub);
                }
                return (v + v + v + v_sub) >> 2;
            }
            else
            {
                // becomes 50:50 interpolation
                idx = frame >> 2;
                goto TEXSRT_VAL_INTERP_2;
            }
        }
        else
        {
            // Frames are exactly the multiples of 4.
            idx = frame >> 2;
            goto TEXSRT_VAL_NONINTERP;
        }
    }
    NNS_G3D_ASSERT(0);
TEXSRT_VAL_NONINTERP:
    if (info & NNS_G3D_TEXSRTANM_ELEM_FX16)
    {
        return *((const fx16*)pDataHead + idx);
    }
    else
    {
        return *((const fx32*)pDataHead + idx);
    }
TEXSRT_VAL_INTERP_2:
    {
        fx32 v0, v1;
        if (info & NNS_G3D_TEXSRTANM_ELEM_FX16)
        {
            v0 = *((const fx16*)pDataHead + idx);
            v1 = *((const fx16*)pDataHead + idx + 1);
        }
        else
        {
            v0 = *((const fx32*)pDataHead + idx);
            v1 = *((const fx32*)pDataHead + idx + 1);
        }
        return ((v0 + v1) >> 1);
    }
}


//---------------------------------------------------------------------
static u32
GetTexSRTAnmSinCosVal_(const NNSG3dResTexSRTAnm* pTexAnm, 
                       u32 info,
                       u32 data,
                       u32 frame)
{
    u32 idx, idx_sub;
    u32 last_interp;
    const void* pDataHead;

    NNS_G3D_ASSERT(!(info & NNS_G3D_TEXSRTANM_ELEM_FX16));

    if(info & NNS_G3D_TEXSRTANM_ELEM_CONST)
    {
        return data;
    }

    pDataHead = (const void*)((u8*)pTexAnm + data);

    if (!(info & NNS_G3D_TEXSRTANM_ELEM_STEP_MASK))
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_1 is selected
        idx = frame;
        goto TEXSRT_SINCOS_NONINTERP;
    }

    // data is input in each frame from last_interp
    // last_interp is either a multiple of 2 or 4
    last_interp = (NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_MASK & info) >>
                                NNS_G3D_TEXSRTANM_ELEM_LAST_INTERP_SHIFT;

    if (info & NNS_G3D_TEXSRTANM_ELEM_STEP_2)
    {
        // NNS_G3D_MATCANM_ELEM_STEP_2 is selected
        if (frame & 1)
        {
            if (frame > last_interp)
            {
                // impossible if anything other than the last frame
                idx = (last_interp >> 1) + 1;
                goto TEXSRT_SINCOS_NONINTERP;
            }
            else
            {
                // Since the final frame is not on an odd number, 50:50 interpolation is needed
                idx = frame >> 1;
                goto TEXSRT_SINCOS_INTERP_2;
            }
        }
        else
        {
            // Even frame, so no interpolation is needed
            idx = frame >> 1;
            goto TEXSRT_SINCOS_NONINTERP;
        }
    }
    else
    {
        // NNS_G3D_TEXSRTANM_ELEM_STEP_4 is selected
        if (frame & 3)
        {
            if (frame > last_interp)
            {
                // No interpolation starting from last_interp
                idx = (last_interp >> 2) + (frame & 3);
                goto TEXSRT_SINCOS_NONINTERP;
            }

            // With interpolation management
            if (frame & 1)
            {
                fx32 s, s_sub;
                fx32 c, c_sub;

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

                s = *((const fx16*)pDataHead + 2 * idx);
                c = *((const fx16*)pDataHead + 2 * idx + 1);
                s_sub = *((const fx16*)pDataHead + 2 * idx_sub);
                c_sub = *((const fx16*)pDataHead + 2 * idx_sub + 1);

                s = (s + s + s + s_sub) >> 2;
                c = (c + c + c + c_sub) >> 2;
                return (u32)((s & 0xffff) | (c << 16));
            }
            else
            {
                // becomes 50:50 interpolation
                idx = frame >> 2;
                goto TEXSRT_SINCOS_INTERP_2;
            }
        }
        else
        {
            // Frames are exactly the multiples of 4.
            idx = frame >> 2;
            goto TEXSRT_SINCOS_NONINTERP;
        }
    }
    NNS_G3D_ASSERT(0);
TEXSRT_SINCOS_NONINTERP:
    return *((const u32*)pDataHead + idx);
TEXSRT_SINCOS_INTERP_2:
    {
        fx32 s0, s1;
        fx32 c0, c1;
        s0 = *((const fx16*)pDataHead + 2 * idx);
        c0 = *((const fx16*)pDataHead + 2 * idx + 1);

        s1 = *((const fx16*)pDataHead + 2 * idx + 2);
        c1 = *((const fx16*)pDataHead + 2 * idx + 3);

        return (u32)((((s0 + s1) >> 1) & 0xffff) | (((c0 + c1) >> 1) << 16));
    }
}


static void 
GetTexSRTAnm_(const NNSG3dResTexSRTAnm* pTexAnm, 
              u16                       idx, 
              u32                       frame, 
              NNSG3dMatAnmResult*       pResult)
{
    NNS_G3D_ASSERT( idx < pTexAnm->dict.numEntry );
    NNS_G3D_ASSERT( frame < pTexAnm->numFrame );
    NNS_G3D_ASSERT( (pTexAnm->anmHeader.category0 == 'M') && (pTexAnm->anmHeader.category1 == 'TA') );
    
    {
        const NNSG3dResDictTexSRTAnmData* pAnmData = 
            (const NNSG3dResDictTexSRTAnmData*)NNS_G3dGetResDataByIdx( &pTexAnm->dict, idx );
        NNSG3dMatAnmResultFlag flag = pResult->flag;
        
        //
        // Trans
        //
        {
            fx32 transS, transT;
            
            transS = GetTexSRTAnmVectorVal_(pTexAnm,
                                            pAnmData->transS,
                                            pAnmData->transSEx,
                                            frame);
            transT = GetTexSRTAnmVectorVal_(pTexAnm,
                                            pAnmData->transT,
                                            pAnmData->transTEx,
                                            frame);
            //
            // Sets the flag value that describes the characteristics of data.
            // With this setting, it becomes possible to reduce the cost of texture matrix calculation described later.
            // 
            if (transS == 0 && transT == 0 )
            {
                flag |=  NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO;
            }
            else
            {
                flag &= ~NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO;
                pResult->transS = transS;
                pResult->transT = transT;
            }
        }
        
        //
        // Rotation
        //
        {
            
            u32 data= GetTexSRTAnmSinCosVal_(pTexAnm, 
                                             pAnmData->rot,    
                                             pAnmData->rotEx,    
                                             frame); 

            //
            // Sets the flag value that describes the characteristics of data.
            //
            if (data == ((FX32_ONE << 16) | 0)) // sin = 0, cos = 1
            {
                flag |=  NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO;
            }
            else
            {
                pResult->sinR = (fx16)(data & 0x0000FFFF);
                pResult->cosR = (fx16)(data >> 16);
                flag &= ~NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO;
            }
        }
        
        //
        // Scale
        //
        {        
            fx32 scaleS, scaleT;

            scaleS = GetTexSRTAnmVectorVal_(pTexAnm,
                                            pAnmData->scaleS,
                                            pAnmData->scaleSEx,
                                            frame);
            scaleT = GetTexSRTAnmVectorVal_(pTexAnm,
                                            pAnmData->scaleT,
                                            pAnmData->scaleTEx,
                                            frame);    
           
            //
            // Sets the flag value that describes the characteristics of data.
            //
            if (scaleS == FX32_ONE && scaleT == FX32_ONE )
            {
                flag |=  NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE;
            }
            else
            {
                flag &= ~NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE;
                pResult->scaleS = scaleS;
                pResult->scaleT = scaleT;
            }
        }

        pResult->flag = flag;
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmObjInitNsBta

    Initializes NNSG3dAnmObj for .nsbta resources.
    This is called from NNS_G3dInitAnmObj.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dAnmObjInitNsBta(NNSG3dAnmObj* pAnmObj,
                        void* pResAnm,
                        const NNSG3dResMdl* pResMdl)
{
    u32 i;
    NNSG3dResTexSRTAnm* srtAnm;
    const NNSG3dResMat* mat;

    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pResAnm);
    NNS_G3D_NULL_ASSERT(pResMdl);

    srtAnm = (NNSG3dResTexSRTAnm*)pResAnm;
    mat = NNS_G3dGetMat(pResMdl);
    pAnmObj->funcAnm = (void*) NNS_G3dFuncAnmMatNsBtaDefault;
    pAnmObj->numMapData = pResMdl->info.numMat;

    // Zero clear the mapData first
    MI_CpuClear16(&pAnmObj->mapData[0], sizeof(u16) * pAnmObj->numMapData);
    
    for (i = 0; i < srtAnm->dict.numEntry; ++i)
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&srtAnm->dict, i);
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
    NNSi_G3dAnmObjInitNsBta

    pResult: Stores the material animation result.
    pAnmObj: pointer to animation object
    dataIdx: Index that indicates the storage location for the data in the resource.
 *---------------------------------------------------------------------------*/
void NNSi_G3dAnmCalcNsBta(NNSG3dMatAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx)
{
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);

    {
        const NNSG3dResTexSRTAnm* pTexAnm 
                = (const NNSG3dResTexSRTAnm*)pAnmObj->resAnm;
                        
        GetTexSRTAnm_( pTexAnm,
                       (u16)dataIdx, 
                       (u32)FX_Whole( pAnmObj->frame ),
                       pResult );
        //
        // When using Texture SRT, forcibly set the texture coordinate generation mode
        // to GX_TEXGEN_TEXCOORD.
        //
        pResult->prmTexImage &= ~REG_G3_TEXIMAGE_PARAM_TGEN_MASK;
        pResult->prmTexImage |= GX_TEXGEN_TEXCOORD << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT;
        
        //
        // Without setting this flag, Texture matrix is not sent to the graphics engine.
        //
        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET;        
        
    }
}


