/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     xsi.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: xsi.h,v $
  Revision 1.1  07/15/2004 12:57:11  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_CGTOOL_XSI_H_
#define NNSG3D_CGTOOL_XSI_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/binres/res_struct.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Function Declarations
//

void NNSi_G3dSendTexSRTXsi(const NNSG3dMatAnmResult* anm);



#ifdef __cplusplus
}/* extern "C"*/
#endif


#endif


