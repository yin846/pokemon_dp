/*---------------------------------------------------------------------------*
  Project:  NitroSystem - G3D(Hokotate) -
  File:     config.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: config.h,v $
  Revision 1.18  01/27/2005 07:36:36 AM  nishida
  Corrected the year of the copyright date.

  Revision 1.17  01/12/2005 04:04:59 AM  nishida
  fix comment.

  Revision 1.16  10/28/2004 05:21:18 AM  nishida_kenji
  if NNS_G3D_USE_EVPCACHE is 0, NNSi_G3dFuncSbc_NODEMIX saves memory.

  Revision 1.15  08/31/2004 07:45:29 AM  nishida_kenji
  small fix

  Revision 1.14  08/31/2004 07:38:39 AM  nishida_kenji
  users can overwrite some of macros without modifying config.h.

  Revision 1.13  08/27/2004 07:12:32 AM  nishida_kenji
  add NNS_G3D_USE_FASTGXDMA

  Revision 1.12  07/27/2004 01:45:31 PM  nishida_kenji
  space for 3dsmax and xsi.

  Revision 1.11  07/26/2004 06:26:46 AM  nishida_kenji
  add comments.

  Revision 1.10  07/17/2004 12:58:20 PM  nishida_kenji
  add switches

  Revision 1.2  07/02/2004 05:57:36 AM  nishida_kenji
  Add some configurations.

  Revision 1.1  06/15/2004 09:53:48 AM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_CONFIG_H_
#define NNSG3D_CONFIG_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif


// The size of the pointer table to the initialization function of the animation binary
#ifndef NNS_G3D_ANMFMT_MAX
#define NNS_G3D_ANMFMT_MAX 10
#endif

// The maximum number of joint material shapes
#ifndef NNS_G3D_SIZE_JNT_MAX
#define NNS_G3D_SIZE_JNT_MAX 64
#endif

#ifndef NNS_G3D_SIZE_MAT_MAX
#define NNS_G3D_SIZE_MAT_MAX 64
#endif

#ifndef NNS_G3D_SIZE_SHP_MAX
#define NNS_G3D_SIZE_SHP_MAX 64
#endif

//
// The buffer size (in words) of the geometry command held by G3D.
// However, this buffer is not allocated by default.
// It can be allocated with NNS_G3dGeSetBuffer.
//
#ifndef NNS_G3D_SIZE_COMBUFFER
#define NNS_G3D_SIZE_COMBUFFER 192
#endif

//
// Check for the presence of each of the callbacks if the comment is taken out.
// The code becomes invalid.
//

//#define NNS_G3D_SBC_CALLBACK_TIMING_A_DISABLE
//#define NNS_G3D_SBC_CALLBACK_TIMING_B_DISABLE
//#define NNS_G3D_SBC_CALLBACK_TIMING_C_DISABLE


//
// The codes for each of the CG tools become invalid if the comments are taken out.
//

//#define NNS_G3D_MAYA_DISABLE
//#define NNS_G3D_SI3D_DISABLE
//#define NNS_G3D_3DSMAX_DISABLE
//#define NNS_G3D_XSI_DISABLE

//
// The codes for each of the animations become invalid if the comment is taken out.
//

//#define NNS_G3D_NSBMA_DISABLE
//#define NNS_G3D_NSBTP_DISABLE
//#define NNS_G3D_NSBTA_DISABLE
//#define NNS_G3D_NSBCA_DISABLE
//#define NNS_G3D_NSBVA_DISABLE

// The size of the dispatch table accessed by the SHP command of SBC. (This is needed because multiple kinds of MAT binary formats are supported.)
#ifndef NNS_G3D_SIZE_SHP_VTBL_NUM
#define NNS_G3D_SIZE_SHP_VTBL_NUM 4
#endif

// The size of the dispatch table accessed by the MAT command of SBC. (This is needed because multiple kinds of SHP binary formats are supported.)
#ifndef NNS_G3D_SIZE_MAT_VTBL_NUM
#define NNS_G3D_SIZE_MAT_VTBL_NUM 4
#endif

// The number of entries in the function table used to perform the joint calculations on each CG tool
#ifndef NNS_G3D_FUNC_SENDJOINTSRT_MAX
#define NNS_G3D_FUNC_SENDJOINTSRT_MAX 3
#endif

// The number of entries in the function table used to perform the texture matrix calculations on each CG tool
#ifndef NNS_G3D_FUNC_SENDTEXSRT_MAX
#define NNS_G3D_FUNC_SENDTEXSRT_MAX 4
#endif

// If the comments are taken out, MI_SendGXCommandAsyncFast is used for transmission of the display list by NNS_G3dGeSendDL rather than MI_SendGXCommandAsync.
//  
//#define NNS_G3D_USE_FASTGXDMA

//
// If you do not use the cache when you are using a weighted envelope,
// you can conserve memory by defining 0.
//
#ifndef NNS_G3D_USE_EVPCACHE
#define NNS_G3D_USE_EVPCACHE 1
#endif


#define NNS_G3D_INLINE static inline


//
// Don't touch them
//

#define NNS_G3D_ASSERTMSG     SDK_ASSERTMSG
#define NNS_G3D_ASSERT        SDK_ASSERT
#define NNS_G3D_NULL_ASSERT   SDK_NULL_ASSERT
#define NNS_G3D_WARNING       SDK_WARNING


#if defined(NNS_G3D_MAYA_DISABLE) && defined(NNS_G3D_SI3D_DISABLE) && \
    defined(NNS_G3D_3DSMAX_DISABLE) && defined(NNS_G3D_XSI_DISABLE)
#error You cannot disable all of the CG tools for G3D.
#endif

#if (NNS_G3D_FUNC_SENDJOINTSRT_MAX < 3)
#error NNS_G3D_FUNC_SENDJOINTSRT_MAX must be 3 or above.
#endif

#if (NNS_G3D_FUNC_SENDTEXSRT_MAX < 2)
#error NNS_G3D_FUNC_SENDTEXSRT_MAX must be 2 or above.
#endif

#ifdef __cplusplus
}
#endif

#endif

