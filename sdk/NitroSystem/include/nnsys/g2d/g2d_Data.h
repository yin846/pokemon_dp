/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_Data.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Data.h,v $
  Revision 1.14  05/31/2005 06:55:53 AM  kitani_toshikazu
  Supported build in ProDG environment.

  Revision 1.13  05/23/2005 06:15:18 AM  hatamoto_minoru
  Added g2d_Font_data.h.

  Revision 1.12  02/22/2005 12:39:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.11  02/21/2005 02:15:10 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.10  12/21/2004 04:05:29 AM  kitani_toshikazu
  Added g2d_Oam_data.h

  Revision 1.9  12/17/2004 07:49:59 AM  kitani_toshikazu
  Defined assert when referencing tools

  Revision 1.8  11/25/2004 06:11:28 AM  nishida
  Inserted line feed as a file terminator. (so that warnings can be output with gcc)

  Revision 1.7  09/02/2004 07:08:10 AM  hatamoto_minoru
  Added BG related.

  Revision 1.6  07/23/2004 03:02:42 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.5  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.4  06/22/2004 06:31:20 AM  kitani_toshikazu
  Change of debug output functions to dummy functions during FINAL ROM build

  Revision 1.3  05/10/2004 04:42:28 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:54 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_DATA_H_
#define NNS_G2D_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>
#include <nnsys/g2d/fmt/g2d_Anim_data.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>
#include <nnsys/g2d/fmt/g2d_MultiCell_data.h>
#include <nnsys/g2d/fmt/g2d_Entity_data.h>
#include <nnsys/g2d/fmt/g2d_Character_data.h>
#include <nnsys/g2d/fmt/g2d_Screen_data.h>
#include <nnsys/g2d/fmt/g2d_Oam_data.h>
#include <nnsys/g2d/fmt/g2d_Font_data.h>


#define NNS_G2D_UNPACK_OFFSET_PTR( ptr, baseOffs )  (ptr) = (void*)((u32)(ptr) + (u32)baseOffs)

#ifdef SDK_FINALROM

#ifdef __SNC__
#define NNS_G2D_DEBUG_FUNC_DECL_BEGIN   static inline
#else
#define NNS_G2D_DEBUG_FUNC_DECL_BEGIN   inline
#endif
#define NNS_G2D_DEBUG_FUNC_DECL_END     {}

#else // SDK_FINALROM

#define NNS_G2D_DEBUG_FUNC_DECL_BEGIN   
#define NNS_G2D_DEBUG_FUNC_DECL_END     ;

#endif


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_DATA_H_

