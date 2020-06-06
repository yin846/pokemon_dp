/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_print_internal.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_print_internal.h,v $
  Revision 1.4  06/28/2004 08:56:50  kitani_toshikazu
  Described the animation type related to the accessor

  Revision 1.3  06/16/2004 23:59:38  nishida_kenji
  NitroSystem -> NITRO-System

  Revision 1.2  06/14/2004 06:24:18  nishida_kenji
  (none)

  Revision 1.1  06/11/2004 04:10:30  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_BINRES_RES_PRINT_INTERNAL_H_
#define NNSG3D_BINRES_RES_PRINT_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _CVTR

#include <stdlib.h>
#include <stdio.h>
#include "g3dcvtr.h"

#define RES_PRINTF printf
#define FX_MUL_    CvtrFX_Mul
#else

#include <nitro.h>
#define RES_PRINTF OS_Printf
#define FX_MUL_    FX_Mul
#endif

#include <nitro/gx.h>
#include <nnsys/g3d/binres/res_print.h>

#if defined(SDK_DEBUG) || defined(_CVTR)

NNS_RES_PRINT void tabPlus_(void) NNS_RES_PRINT_END
NNS_RES_PRINT void tabMinus_(void) NNS_RES_PRINT_END
NNS_RES_PRINT void tabPrint_(void) NNS_RES_PRINT_END
NNS_RES_PRINT void printFx32_(fx32 v) NNS_RES_PRINT_END
NNS_RES_PRINT f32 fx32Tof32_(fx32 v) NNS_RES_PRINT_END
NNS_RES_PRINT void chkDict_(const NNSG3dResDict* dict,
                            const NNSG3dResName* name,
                            u32 idx) NNS_RES_PRINT_END

#endif // #if defined(SDK_DEBUG) || defined(_CVTR)

#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif 

