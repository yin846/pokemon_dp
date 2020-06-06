/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     util_inline.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util_inline.h,v $
  Revision 1.2  08/02/2004 02:26:56  nishida_kenji
  suppress a warning.

  Revision 1.1  07/12/2004 12:49:26  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_UTIL_INLINE_H_
#define NNSG3D_UTIL_INLINE_H_

//
// DO NOT INCLUDE THIS FILE DIRECTLY
//


#ifdef __cplusplus
extern "C" {
#endif



NNS_G3D_INLINE BOOL
NNSi_G3dBitVecCheck(const u32* vec, u32 idx)
{
    NNS_G3D_NULL_ASSERT(vec);
    return (BOOL)(vec[idx >> 5] & (1 << (idx & 31)));
}


NNS_G3D_INLINE void
NNSi_G3dBitVecSet(u32* vec, u32 idx)
{
    NNS_G3D_NULL_ASSERT(vec);
    vec[idx >> 5] |= 1 << (idx & 31);
}


NNS_G3D_INLINE void
NNSi_G3dBitVecReset(u32* vec, u32 idx)
{
    NNS_G3D_NULL_ASSERT(vec);
    vec[idx >> 5] &= ~(1 << (idx & 31));
}




#ifdef __cplusplus
}
#endif



#endif

