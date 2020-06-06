/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) - 
  File:     basic.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: basic.h,v $
  Revision 1.4  07/15/2004 12:57:24 PM  nishida_kenji
  rename functions

  Revision 1.3  07/09/2004 07:19:03 AM  nishida_kenji
  (none)

  Revision 1.2  07/08/2004 12:47:21 PM  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:40:00 PM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_CGTOOL_BASIC_H_
#define NNSG3D_CGTOOL_BASIC_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/anm.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Declaration of Functions
//

void NNSi_G3dSendJointSRTBasic(const NNSG3dJntAnmResult* result);
void NNSi_G3dGetJointScaleBasic(NNSG3dJntAnmResult* pResult,
                                const fx32* p,
                                const u8* cmd,
                                u32 srtflag);

#ifdef __cplusplus
}/* extern "C"*/
#endif


#endif

