/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     g3d.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g3d.h,v $
  Revision 1.3  08/17/2004 10:44:20 AM  nishida_kenji
  Add 1mat1shp.h

  Revision 1.2  07/26/2004 06:26:41 AM  nishida_kenji
  add comments.

  Revision 1.1  07/21/2004 03:39:01 AM  nishida_kenji
  Initial check in

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G3D_H_
#define NNS_G3D_H_

//
// Precautions when using G3D
// - Use NNS_G3dSendDL, not MI_SendGXCommandAsync.
// - Do not call G3D function from multiple threads.
// - Use the SDK G3_XXXX command, after executing NNS_G3dGeFlushBuffer(), and after flushing the
//    geometry command cache that the G3D has.
//
// ADVICE
// - DO NOT USE MI_SendGXCommandAsync DIRECTLY. USE NNS_G3dSendDL INSTEAD.
// - DO NOT USE G3D FUNCTIONS FROM MULTIPLE THREADS.
// - Call G3_XXX on SDK after executing NNS_G3dGeFlushBuffer() --- This waits
//    until NNS_G3dSendDL completes, then flushes a geometry command buffer
//    which G3D may have.
//

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/glbstate.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/sbc.h>
#include <nnsys/g3d/1mat1shp.h>
#include <nnsys/g3d/model.h>
#include <nnsys/g3d/cgtool.h>
#include <nnsys/g3d/mem.h>
#include <nnsys/g3d/util.h>

#endif

