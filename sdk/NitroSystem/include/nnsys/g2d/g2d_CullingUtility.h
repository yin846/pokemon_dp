 /*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_CullingUtility.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_CullingUtility.h,v $
  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:54 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_G2D_CULLINGUTILITY_H_
#define NNS_G2D_CULLINGUTILITY_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <nitro.h>
#include <nnsys/g2d/fmt/g2d_Vec_data.h>

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsInViewCircle

  Description:  Determines if in visible area (circle)
                
                
  Arguments:    pvUL:            visible area rectangle (upper left)
                pvSize:          visible area rectangle (size)
                pos:             target object position
                boundingR:      boundary circle radius
                
                
  Returns:      if in visible area
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsInViewCircle( const NNSG2dFVec2* pvUL, const NNSG2dFVec2* pvSize, const NNSG2dFVec2* pos, fx32 boundingR );

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif// NNS_G2D_CULLINGUTILITY_H_

