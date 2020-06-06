/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     maya.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: maya.h,v $
  Revision 1.4  07/15/2004 12:57:24  nishida_kenji
  rename functions

  Revision 1.3  07/09/2004 07:19:03  nishida_kenji
  (none)

  Revision 1.2  07/08/2004 12:47:21  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:40:00  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_CGTOOL_MAYA_H_
#define NNSG3D_CGTOOL_MAYA_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/binres/res_struct.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Declaration of Functions
//

void NNSi_G3dSendJointSRTMaya(const NNSG3dJntAnmResult* result);
void NNSi_G3dGetJointScaleMaya(NNSG3dJntAnmResult* pResult,
                               const fx32* p,
                               const u8* cmd,
                               u32 srtflag);
void NNSi_G3dSendTexSRTMaya(const NNSG3dMatAnmResult* anm);

#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif

