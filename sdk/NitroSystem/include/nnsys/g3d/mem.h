/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     mem.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mem.h,v $
  Revision 1.3  07/26/2004 05:18:12  nishida_kenji
  Add alloc/free APIs.

  Revision 1.1  07/10/2004 08:46:46  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_MEM_H_
#define NNSG3D_MEM_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>

#include <nnsys/fnd/allocator.h>

//
// This is the API set that accompanies allocation and deallocation of the heap.
// It is a utility, so the user in not particularly forced to use it.
//
// Also, mem.h cannot be included from other than a mem.c G3D source.
//

#ifdef __cplusplus
extern "C" {
#endif

NNSG3dRenderObj* NNS_G3dAllocRenderObj(NNSFndAllocator* pAlloc);
void NNS_G3dFreeRenderObj(NNSFndAllocator* pAlloc,
                          NNSG3dRenderObj* pRenderObj);



NNSG3dAnmObj* NNS_G3dAllocAnmObj(NNSFndAllocator* pAlloc,
                                 const void* pAnm,
                                 const NNSG3dResMdl* pMdl);
void NNS_G3dFreeAnmObj(NNSFndAllocator* pAlloc,
                       NNSG3dAnmObj* pAnmObj);



NNSG3dJntAnmResult* NNS_G3dAllocRecBufferJnt(NNSFndAllocator* pAlloc,
                                             const NNSG3dResMdl* pResMdl);
void NNS_G3dFreeRecBufferJnt(NNSFndAllocator* pAlloc,
                             NNSG3dJntAnmResult* pRecBuf);



NNSG3dMatAnmResult* NNS_G3dAllocRecBufferMat(NNSFndAllocator* pAlloc,
                                             const NNSG3dResMdl* pResMdl);
void NNS_G3dFreeRecBufferMat(NNSFndAllocator* pAlloc,
                             NNSG3dMatAnmResult* pRecBuf);


#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif


