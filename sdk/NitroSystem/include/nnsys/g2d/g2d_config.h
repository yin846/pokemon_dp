/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_config.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_config.h,v $
  Revision 1.7  06/30/2005 04:22:26 AM  kitani_toshikazu
  Moved position of NNS_G2D_LIMIT_CELL_X_128 note 

  Revision 1.6  2005/06/01 06:19:51 AM  kitani_toshikazu
  NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED
  was added. 

  Revision 1.5  05/30/2005 10:14:19 AM  hatamoto_minoru
  Added support for the older versions.

  Revision 1.4  05/20/2005 05:07:49 AM  hatamoto_minoru
  Added NNS_G2D_MIN_ASSERT, NNS_G2D_MAX_ASSERT

  Revision 1.3  10/06/2004 01:24:21 AM  kitani_toshikazu
  Changed NNS_G2D_DMA_NO from the fixed value 3 to GX_GetDefaultDMA()

  Revision 1.2  07/24/2004 02:47:19 AM  kitani_toshikazu
  Added NNS_G2D_DMA_NO definition

  Revision 1.1  07/23/2004 02:48:39 AM  kitani_toshikazu
  Registered initial issue

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_CONFIG_H_
#define NNS_G2D_CONFIG_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_INLINE static inline 

// Changed NNS_G2D_DMA_NO from 3 to GX_GetDefaultDMA()
// #define NNS_G2D_DMA_NO 3
#define NNS_G2D_DMA_NO      GX_GetDefaultDMA() 


#define NNS_G2D_ASSERTMSG               SDK_ASSERTMSG
#define NNS_G2D_ASSERT                  SDK_ASSERT
#define NNS_G2D_NULL_ASSERT             SDK_NULL_ASSERT
#define NNS_G2D_MINMAX_ASSERT           SDK_MINMAX_ASSERT
#define NNS_G2D_MIN_ASSERT              SDK_MIN_ASSERT
#define NNS_G2D_MAX_ASSERT              SDK_MAX_ASSERT
#define NNS_G2D_WARNING                 SDK_WARNING
#define NNS_G2D_NON_ZERO_ASSERT( val )  SDK_ASSERTMSG( (val) != 0, "Non zero value is expexted in the context." )


// Enable the cell size limits of the past version
//#define NNS_G2D_LIMIT_CELL_X_128

// Use font resource from old version
#define NNS_G2D_FONT_USE_OLD_RESOURCE

// If input data includes double-size affine OBJ,
// assume that double-size affine OBJ position correction is used?
#define NNS_G2D_ASSUME_DOUBLEAFFINE_OBJPOS_ADJUSTED


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_CONFIG_H_

