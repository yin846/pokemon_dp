/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_Animation.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NCG_load.h,v $
  Revision 1.10  08/17/2005 01:57:35 AM  kitani_toshikazu
  Added comments

  Revision 1.9  06/13/2005 08:24:03 AM  kitani_toshikazu
  Added function that processes NNSG2dCharacterPosInfo 

  Revision 1.8  05/31/2005 06:55:53 AM  kitani_toshikazu
  Supported build in ProDG environment.

  Revision 1.7  02/22/2005 12:30:25 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.6  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.5  09/02/2004 07:08:10 AM  hatamoto_minoru
  Added BG related.

  Revision 1.4  07/23/2004 03:02:42 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.3  06/22/2004 06:28:21 AM  kitani_toshikazu
  Change of debug output functions to dummy functions during FINAL ROM build

  Revision 1.2  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.1  05/10/2004 04:42:28 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:55 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_NCG_LOAD_H_
#define NNS_G2D_NCG_LOAD_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// For OBJ (For the BG character data, please use NNS_G2dGetUnpackedBGCharacterData()). )
//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedCharacterData( void* pNcgrFile, NNSG2dCharacterData** ppCharData );
void NNS_G2dUnpackNCG( NNSG2dCharacterData* pCharData );

//------------------------------------------------------------------------------
// For BG (For the OBJ character data, please use NNS_G2dGetUnpackedBGCharacterData()). )
//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedBGCharacterData( void* pNcgrFile, NNSG2dCharacterData** ppCharData );
void NNS_G2dUnpackBGNCG( NNSG2dCharacterData* pCharData );

BOOL NNS_G2dGetUnpackedCharacterPosInfo
( 
    void*                        pNcgrFile, 
    NNSG2dCharacterPosInfo**     ppCharPosInfo 
);


#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintCharacterData( const NNSG2dCharacterData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintCharacterPosInfo( const NNSG2dCharacterPosInfo* pPosInfo ) NNS_G2D_DEBUG_FUNC_DECL_END
#else
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintCharacterData( const NNSG2dCharacterData* ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintCharacterPosInfo( const NNSG2dCharacterPosInfo* ) NNS_G2D_DEBUG_FUNC_DECL_END
#endif


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_NCG_LOAD_H_

