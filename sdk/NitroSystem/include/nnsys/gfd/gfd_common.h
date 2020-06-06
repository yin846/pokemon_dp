/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  
  File: gfd_common.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_common.h,v $
  Revision 1.3  11/22/2004 08:46:32 AM  kitani_toshikazu
  Added NNS_GFD_INLINE definition

  Revision 1.2  09/29/2004 02:03:51 AM  kitani_toshikazu
  Added comments

  Revision 1.1  07/08/2004 07:18:45 AM  kitani_toshikazu
  Added the VRAM transmission manager


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_GFD_COMMON_H_
#define NNS_GFD_COMMON_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
#define NNS_GFD_NON_ZERO_ASSERT( val )  SDK_ASSERTMSG( ( val != 0 ), "Non-Zero value is expected in this context." );

#define NNS_GFD_ASSERT           SDK_ASSERT
#define NNS_GFD_ASSERTMSG        SDK_ASSERTMSG
#define NNS_GFD_NULL_ASSERT      SDK_NULL_ASSERT
#define NNS_GFD_MINMAX_ASSERT    SDK_MINMAX_ASSERT

#define NNS_GFD_INLINE          static inline


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_COMMON_H_

