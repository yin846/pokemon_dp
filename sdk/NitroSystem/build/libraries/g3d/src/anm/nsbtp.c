/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     nsbta.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nsbtp.c,v $
  Revision 1.15  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.14  08/04/2004 02:50:51  nishida_kenji
  bugfix(NNSG3dMatAnmResult::prmTexImage not correct).

  Revision 1.13  07/24/2004 05:10:08  nishida_kenji
  fix comments.

  Revision 1.12  07/22/2004 07:21:07  kitani_toshikazu
  Fixed bug in part that reflects animation result palette

  Revision 1.11  07/22/2004 06:03:08  kitani_toshikazu
  Renamed Functions
  NNS_G3dGetTexPatAnmDataByIdx()
      => NNSi_G3dGetTexPatAnmDataByIdx()
  NNSi_G3dGetTexNameByIdx()
      => NNSi_G3dGetTexPatAnmTexNameByIdx()
  NNSi_G3dGetPlttNameByIdx()
      => NNSi_G3dGetTexPatAnmPlttNameByIdx()

  Revision 1.10  07/22/2004 03:05:16  kitani_toshikazu
  Fixed comments.

  Revision 1.9  07/22/2004 02:58:09  kitani_toshikazu
  Implemented palette change

  Revision 1.8  07/21/2004 09:07:00  kitani_toshikazu
  Implemented a part for fetching the animation result
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/anm/nsbtp.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmObjInitNsBtp

    Initializes NNSG3dAnmObj for .nsbtp resources.
    This is called from NNS_G3dInitAnmObj.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dAnmObjInitNsBtp(NNSG3dAnmObj*       pAnmObj,
                        void*               pResAnm,
                        const NNSG3dResMdl* pResMdl)
{
    u32 i;
    NNSG3dResTexPatAnm* tpAnm = (NNSG3dResTexPatAnm*)pResAnm;
    const NNSG3dResMat* mat = NNS_G3dGetMat(pResMdl);

    NNS_G3D_NULL_ASSERT(pAnmObj->resTex);

    pAnmObj->funcAnm    = (void*) NNS_G3dFuncAnmMatNsBtpDefault;
    pAnmObj->numMapData = pResMdl->info.numMat;
    pAnmObj->resAnm     = tpAnm;

    // Zero clear the mapData first
    MI_CpuClear16(&pAnmObj->mapData[0], sizeof(u16) * pAnmObj->numMapData);

    for (i = 0; i < tpAnm->dict.numEntry; ++i)
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&tpAnm->dict, i);
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
    SetTexParamaters_

    Set the texture-related part of pResult
 *---------------------------------------------------------------------------*/
static void SetTexParamaters_
( 
    const NNSG3dResTex*     pTex, 
    const NNSG3dResName*    pTexName, 
    NNSG3dMatAnmResult*     pResult 
)
{
    NNS_G3D_NULL_ASSERT( pTex );
    NNS_G3D_NULL_ASSERT( pTexName );
    NNS_G3D_NULL_ASSERT( pResult );
    NNS_G3D_ASSERTMSG((pTex->texInfo.vramKey != 0 || pTex->texInfo.sizeTex == 0),
                      "No texture key assigned");
    NNS_G3D_ASSERTMSG((pTex->tex4x4Info.vramKey != 0 || pTex->tex4x4Info.sizeTex == 0),
                      "No texture(4x4) key assigned");
    {
        //
        // From names, consult dictionary for texture data
        //
        const NNSG3dResDictTexData* pData = NNS_G3dGetTexDataByName( pTex, pTexName );        
        NNS_G3D_NULL_ASSERT( pData );
        
        {
            const u32 vramOffset = (( pData->texImageParam & 0x1c000000) != 0x14000000) ?
                                    pTex->texInfo.vramKey       & 0xffff :  // Some texture other than 4x4
                                    pTex->tex4x4Info.vramKey    & 0xffff ;  // A 4x4 texture

            // Leave "flip" and "repeat" and "texgen" and reset, then combine.
            // Data about materials must be left.
            pResult->prmTexImage &= ~0x3ff0ffff;
            pResult->prmTexImage |= pData->texImageParam + vramOffset;
            
            pResult->origWidth  = (u16)(( pData->extraParam & NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_MASK ) >> 
                                    NNS_G3D_TEXIMAGE_PARAMEX_ORIGW_SHIFT);
            pResult->origHeight = (u16)(( pData->extraParam & NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_MASK ) >> 
                                    NNS_G3D_TEXIMAGE_PARAMEX_ORIGH_SHIFT);
            
            //
            // Compute the horizontal and vertical scale ratios needed for calculating the texture matrix
            //
            {
                const s32 w = (s32)((pData->extraParam) & 0x7ff);
                const s32 h = (s32)((pData->extraParam >> 11) & 0x7ff);
                
                
                pResult->magW = ( w != pResult->origWidth ) ?
                                FX_Div( w << FX32_SHIFT, pResult->origWidth << FX32_SHIFT) :
                                FX32_ONE;
                pResult->magH = ( h != pResult->origHeight ) ?
                                FX_Div( h << FX32_SHIFT, pResult->origHeight << FX32_SHIFT) :
                                FX32_ONE;
            }
        }
    }
}


/*---------------------------------------------------------------------------*
    SetPlttParamaters_

    Set the palette-related part of pResult
 *---------------------------------------------------------------------------*/
static void SetPlttParamaters_
( 
    const NNSG3dResTex*     pTex, 
    const NNSG3dResName*    pPlttName, 
    NNSG3dMatAnmResult*     pResult 
)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_NULL_ASSERT(pPlttName);
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_ASSERTMSG((pTex->plttInfo.vramKey != 0 || pTex->plttInfo.sizePltt == 0),
         "No palette key assigned");
    
    {
        // Get the data field corresponding to the palette name from the Texture block
        const NNSG3dResDictPlttData* pPlttData = NNS_G3dGetPlttDataByName(pTex, pPlttName);
        u16 plttBase    = pPlttData->offset;
        u16 vramOffset  = (u16)(pTex->plttInfo.vramKey & 0xffff);
        
        NNS_G3D_NULL_ASSERT(pPlttData);    
        
        
        // If 4colors, bit is standing
        if (!(pPlttData->flag & 1))
        {
            // Four-bit shift if other than 4colors.
            // If 4colors, shift is 3-bit shift so left as-is
            plttBase >>= 1;
            vramOffset >>= 1;
        }
        
        //
        // Set address
        //
        pResult->prmTexPltt = (u32)(plttBase + vramOffset);
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmCalcNsBtp

    pResult: Stores the material animation result.
    pAnmObj:
    dataIdx: Index that indicates the storage location for the data in the resource.
 *---------------------------------------------------------------------------*/
void NNSi_G3dAnmCalcNsBtp(NNSG3dMatAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx)
{
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pAnmObj->resTex);
    
    {
        //
        // Obtains the texture pattern FV that corresponds to the current frame.
        //
        const NNSG3dResTexPatAnm*     pPatAnm 
            = (const NNSG3dResTexPatAnm*)pAnmObj->resAnm;
            
        const NNSG3dResTexPatAnmFV*   pTexFV 
            = NNSi_G3dGetTexPatAnmFV( pPatAnm, 
                                     (u16)dataIdx, 
                                     (u16)FX_Whole( pAnmObj->frame ) );
        NNS_G3D_NULL_ASSERT(pTexFV);
        
        //
        // Set the texture-related part of pResult
        //
        SetTexParamaters_( pAnmObj->resTex, 
                           NNSi_G3dGetTexPatAnmTexNameByIdx( pPatAnm, pTexFV->idTex ),
                           pResult ); 
        //                   
        // Set the palette-related part of pResult
        //
        
        //
        // When accessing direct texture format data under the current implementation of the converter,
        // 255 is assigned to the member pTexFV->idPltt. When 255 is set for pTexFV->idPltt, the library must skip palette setup processing.
        //  
        //
        if( pTexFV->idPltt != 255 )
        {
            SetPlttParamaters_( pAnmObj->resTex, 
                                NNSi_G3dGetTexPatAnmPlttNameByIdx( pPatAnm, pTexFV->idPltt ),
                                pResult ); 
        }                                     
    }
}

