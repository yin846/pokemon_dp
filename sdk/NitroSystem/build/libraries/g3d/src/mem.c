/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     mem.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mem.c,v $
  Revision 1.2  07/26/2004 05:18:18  nishida_kenji
  Add alloc/free APIs.

  Revision 1.1  07/10/2004 08:47:45  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/mem.h>


/*---------------------------------------------------------------------------*
    NNS_G3dAllocRenderObj

    Secure a region from the heap for NNSG3dRenderObj for the region.
    Initialize using NNS_G3dRenderObjInit.
 *---------------------------------------------------------------------------*/
NNSG3dRenderObj*
NNS_G3dAllocRenderObj(NNSFndAllocator* pAlloc)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    return (NNSG3dRenderObj*)
           NNS_FndAllocFromAllocator(pAlloc, sizeof(NNSG3dRenderObj));
}


/*---------------------------------------------------------------------------*
    NNS_G3dFreeRenderObj

    Release the region secured for NNSG3dRenderObj.
    Note that the region referenced by the pointer stored by NNSG3dRenderObj does not get released.
    The user must do this separately..
 *---------------------------------------------------------------------------*/
void
NNS_G3dFreeRenderObj(NNSFndAllocator* pAlloc,
                     NNSG3dRenderObj* pRenderObj)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    NNS_FndFreeToAllocator(pAlloc, pRenderObj);
}


/*---------------------------------------------------------------------------*
    NNS_G3dAllocAnmObj

    Secure a region from the heap for the NNSG3dAnmObj region.
    Initialize using NNS_G3dAnmObjInit.
 *---------------------------------------------------------------------------*/
NNSG3dAnmObj*
NNS_G3dAllocAnmObj(NNSFndAllocator* pAlloc,
                   const void* pAnm,
                   const NNSG3dResMdl* pMdl)
{
    u32 sz;
    NNS_G3D_NULL_ASSERT(pAlloc);
    NNS_G3D_NULL_ASSERT(pAnm);
    NNS_G3D_NULL_ASSERT(pMdl);

    sz = NNS_G3dAnmObjCalcSizeRequired(pAnm, pMdl);
    return (NNSG3dAnmObj*) NNS_FndAllocFromAllocator(pAlloc, sz);
}


/*---------------------------------------------------------------------------*
    NNS_G3dFreeAnmObj

    Release the region secured for NNSG3dAnmObj.
    Note that the region  referenced by the pointer stored by NNSG3dAnmObj does not get released.
    The user must do this separately.
 *---------------------------------------------------------------------------*/
void
NNS_G3dFreeAnmObj(NNSFndAllocator* pAlloc,
                  NNSG3dAnmObj* pAnmObj)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    NNS_FndFreeToAllocator(pAlloc, pAnmObj);
}


/*---------------------------------------------------------------------------*
    NNS_G3dAllocRecBufferJnt

    Secure a buffer region that can store NNSG3dRenderObj for storing the
    joint calculation results. The size of this region is determined
    based on the number of joints in NNSG3dResMdl.
 *---------------------------------------------------------------------------*/
NNSG3dJntAnmResult*
NNS_G3dAllocRecBufferJnt(NNSFndAllocator* pAlloc,
                         const NNSG3dResMdl* pResMdl)
{
    u32 numJnt;
    NNS_G3D_NULL_ASSERT(pAlloc);
    NNS_G3D_NULL_ASSERT(pResMdl);

    numJnt = pResMdl->info.numNode;

    return (NNSG3dJntAnmResult*)
            NNS_FndAllocFromAllocator(pAlloc,
                                      NNS_G3D_RENDEROBJ_JNTBUFFER_SIZE(numJnt));
}


/*---------------------------------------------------------------------------*
    NNS_G3dFreeRecBufferJnt

    Release the buffer region that was secured for storing joint calculation results.
 *---------------------------------------------------------------------------*/
void
NNS_G3dFreeRecBufferJnt(NNSFndAllocator* pAlloc,
                        NNSG3dJntAnmResult* pRecBuf)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    NNS_FndFreeToAllocator(pAlloc, pRecBuf);
}


/*---------------------------------------------------------------------------*
    NNS_G3dAllocRecBufferMat

    Secure a buffer region that can store NNSG3dRenderObj for storing the
    joint calculation results. The size of this region is determined
    based on the number of joints in NNSG3dResMdl.
 *---------------------------------------------------------------------------*/
NNSG3dMatAnmResult*
NNS_G3dAllocRecBufferMat(NNSFndAllocator* pAlloc,
                         const NNSG3dResMdl* pResMdl)
{
    u32 numMat;
    NNS_G3D_NULL_ASSERT(pAlloc);
    NNS_G3D_NULL_ASSERT(pResMdl);   

    numMat = pResMdl->info.numMat;

    return (NNSG3dMatAnmResult*)
            NNS_FndAllocFromAllocator(pAlloc,
                                      NNS_G3D_RENDEROBJ_MATBUFFER_SIZE(numMat));
}


/*---------------------------------------------------------------------------*
    NNS_G3dFreeRecBufferMat

    Release the buffer region that was secured for storing material calculation results.
 *---------------------------------------------------------------------------*/
void
NNS_G3dFreeRecBufferMat(NNSFndAllocator* pAlloc,
                        NNSG3dMatAnmResult* pRecBuf)
{
    NNS_G3D_NULL_ASSERT(pAlloc);

    NNS_FndFreeToAllocator(pAlloc, pRecBuf);
}

