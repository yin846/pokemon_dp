/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     nsbma.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nsbma.h,v $
  Revision 1.5  07/19/2004 10:55:22  nishida_kenji
  change arguments of animation calc functions

  Revision 1.4  07/15/2004 12:57:42  nishida_kenji
  rename functions

  Revision 1.3  07/12/2004 12:50:59  nishida_kenji
  refactoring

  Revision 1.2  07/10/2004 08:46:57  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:40:16  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_ANM_NSBMA_H_
#define NNSG3D_ANM_NSBMA_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>

#ifdef __cplusplus
extern "C" {
#endif

void NNSi_G3dAnmObjInitNsBma(NNSG3dAnmObj* pAnmObj,
                             void* pResAnm,
                             const NNSG3dResMdl* pResMdl);

void NNSi_G3dAnmCalcNsBma(NNSG3dMatAnmResult* pResult,
                          const NNSG3dAnmObj* pAnmObj,
                          u32 dataIdx);

#ifdef __cplusplus
}/* extern "C"*/
#endif







#endif

