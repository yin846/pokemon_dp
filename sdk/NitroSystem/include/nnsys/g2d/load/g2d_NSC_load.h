/*---------------------------------------------------------------------------*
  Project:   NitroSystem - Graphics 2D
  File:     g2d_NSC_load.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NSC_load.h,v $
  Revision 1.5  05/31/2005 06:55:53 AM  kitani_toshikazu
  Supported build in ProDG environment.

  Revision 1.4  02/22/2005 12:30:25 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.3  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.2  09/03/2004 09:30:24 AM  hatamoto_minoru
  Corrected a file name in the comments.
  Corrected the macro name to prevent multiple includes.

  Revision 1.1  09/02/2004 07:07:16 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_NSC_LOAD_H_
#define NNS_G2D_NSC_LOAD_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

#ifdef __cplusplus
extern "C" {
#endif


BOOL NNS_G2dGetUnpackedScreenData( void* pNscrFile, NNSG2dScreenData** ppScrData );

#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN
void NNS_G2dPrintScreenData( const NNSG2dScreenData* p1)
NNS_G2D_DEBUG_FUNC_DECL_END
#else
NNS_G2D_DEBUG_FUNC_DECL_BEGIN
void NNS_G2dPrintScreenData( const NNSG2dScreenData* )
NNS_G2D_DEBUG_FUNC_DECL_END
#endif


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_NSC_LOAD_H_

