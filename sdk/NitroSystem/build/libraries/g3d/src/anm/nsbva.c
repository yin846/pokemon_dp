/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     nsbva.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nsbva.c,v $
  Revision 1.15  02/25/2005 05:38:52  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.14  10/25/2004 01:26:34  nishida_kenji
  Expand NNSi_G3dGetVisAnm.

  Revision 1.13  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.12  07/24/2004 05:06:09  nishida_kenji
  fix comments.

  Revision 1.11  07/22/2004 06:04:09  kitani_toshikazu
  (none)

  Revision 1.9  07/21/2004 09:07:00  kitani_toshikazu
  Implemented a part for fetching the animation result
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/anm/nsbva.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmObjInitNsBva

    Initializes NNSG3dAnmObj for .nsbva resources.
    This is called from NNS_G3dInitAnmObj.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dAnmObjInitNsBva(NNSG3dAnmObj* pAnmObj,
                        void* pResAnm,
                        const NNSG3dResMdl* pResMdl)
{
    u32 i;
    NNSG3dResVisAnm* visAnm;
    const NNSG3dResNodeInfo* jnt;

    visAnm = (NNSG3dResVisAnm*)pResAnm;
    jnt = NNS_G3dGetNodeInfo(pResMdl);
    pAnmObj->funcAnm = (void*) NNS_G3dFuncAnmVisNsBvaDefault;
    pAnmObj->numMapData = pResMdl->info.numNode;
   
    pAnmObj->resAnm = (void*)visAnm;
    
    //
    // In the case of visibility animation, defined for all nodes
    //
    for (i = 0; i < pAnmObj->numMapData; ++i)
    {
        pAnmObj->mapData[i] = (u16)(i | NNS_G3D_ANMOBJ_MAPDATA_EXIST);
    }
}


/*---------------------------------------------------------------------------*
    NNSi_G3dAnmCalcNsBva

    pResult: Stores the results of the visibility animation
    pAnmObj:
    dataIdx: Index that indicates the storage location for the data in the resource.

 *---------------------------------------------------------------------------*/
void NNSi_G3dAnmCalcNsBva(NNSG3dVisAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx)
{
    NNS_G3D_NULL_ASSERT(pResult);
    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pAnmObj->resAnm);

    {
        u32 frame = (u32)FX_Whole(pAnmObj->frame);
        const NNSG3dResVisAnm* pVisAnm = pAnmObj->resAnm;
        u32 pos = frame * pVisAnm->numNode + dataIdx;
        u32 n = pos >> 5;
        u32 mask = 1U << (pos & 0x1f);

        pResult->isVisible = (BOOL)(pVisAnm->visData[n] & mask);
    }
}



















