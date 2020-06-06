/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     cgtool.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: cgtool.c,v $
  Revision 1.8  10/07/2004 02:10:43 AM  nishida_kenji
  Add comments.

  Revision 1.7  07/28/2004 01:16:21 AM  nishida_kenji
  xsi, 3dsmax

  Revision 1.6  07/27/2004 01:47:06 PM  nishida_kenji
  remove an obsolete function.

  Revision 1.5  07/15/2004 12:58:38 PM  nishida_kenji
  rename functions

  Revision 1.4  07/14/2004 11:31:23 AM  nishida_kenji
  (none)

  Revision 1.3  07/13/2004 01:02:55 AM  nishida_kenji
  (none)

  Revision 1.2  07/08/2004 12:46:06 PM  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:41:43 PM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/cgtool.h>

#include <nnsys/g3d/cgtool/basic.h>
#include <nnsys/g3d/cgtool/maya.h>
#include <nnsys/g3d/cgtool/si3d.h>
#include <nnsys/g3d/cgtool/3dsmax.h>
#include <nnsys/g3d/cgtool/xsi.h>


//
// Function pointer array for eliminating the differences between the scale calculations of various CG tools
//
NNSG3dGetJointScale NNS_G3dGetJointScale_FuncArray[NNS_G3D_FUNC_SENDJOINTSRT_MAX] =
{
    &NNSi_G3dGetJointScaleBasic,
#ifndef NNS_G3D_MAYA_DISABLE
    &NNSi_G3dGetJointScaleMaya,
#else
    NULL,
#endif
#ifndef NNS_G3D_SI3D_DISABLE
    &NNSi_G3dGetJointScaleSi3d
#else
    NULL
#endif
};


//
// Function pointer array to eliminate differences between the joint calculations of each CG tool
//
NNSG3dSendJointSRT  NNS_G3dSendJointSRT_FuncArray[NNS_G3D_FUNC_SENDJOINTSRT_MAX] =
{
    &NNSi_G3dSendJointSRTBasic,
#ifndef NNS_G3D_MAYA_DISABLE
    &NNSi_G3dSendJointSRTMaya,
#else
    NULL,
#endif
#ifndef NNS_G3D_SI3D_DISABLE
    &NNSi_G3dSendJointSRTSi3d
#else
    NULL
#endif
};


//
// Function pointer array to eliminate differences between the texture calculations of each CG tool
//
NNSG3dSendTexSRT    NNS_G3dSendTexSRT_FuncArray[NNS_G3D_FUNC_SENDTEXSRT_MAX] =
{
#ifndef NNS_G3D_MAYA_DISABLE
    &NNSi_G3dSendTexSRTMaya,
#else
    NULL,
#endif
#ifndef NNS_G3D_SI3D_DISABLE
    &NNSi_G3dSendTexSRTSi3d,
#else
    NULL,
#endif
#ifndef NNS_G3D_3DSMAX_DISABLE
    &NNSi_G3dSendTexSRT3dsMax,
#else
    NULL,
#endif
#ifndef NNS_G3D_XSI_DISABLE
    &NNSi_G3dSendTexSRTXsi
#else
    NULL
#endif
};


