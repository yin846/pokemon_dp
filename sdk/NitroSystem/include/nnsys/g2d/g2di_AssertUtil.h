/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  File:     g2di_AssertUtil.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_AssertUtil.h,v $
  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.3  03/14/2005 11:54:34 PM  hatamoto_minoru
  tab -> sp

  Revision 1.2  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.2  12/10/2004 10:15:02 AM  hatamoto_minoru
  Added comments and ASSERT.

  Revision 1.1  11/30/2004 09:17:04 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef ASSERT_UTIL_H_
#define ASSERT_UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif


// check alignment
#define NNS_G2D_IS_ALIGNED(addr, align) (((u32)(addr) & ((align)-1)) == 0)

#define NNS_G2D_ALIGN_ASSERT(addr, align)                   \
    NNS_G2D_ASSERTMSG(                                      \
        NNS_G2D_IS_ALIGNED((addr), (align)),                \
        "'%s'=%p must be %d byte aligned.", #addr, (addr) )

// check valid memory address
#define NNS_G2D_IS_VALID_POINTER(p) ((HW_ITCM_IMAGE <= ((u32)(p))) && (((u32)(p)) <= HW_CTRDG_RAM_END))

#define NNS_G2D_POINTER_ASSERT(p)                                           \
    NNS_G2D_ASSERTMSG(                                                      \
        NNS_G2D_IS_VALID_POINTER(p),                                        \
        "'%s'=%p is invalid memory address.", #p, (p) );

#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // ASSERT_UTIL_H_


