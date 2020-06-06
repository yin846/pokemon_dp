/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_Animation.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NMC_load.h,v $
  Revision 1.8  05/31/2005 06:55:53 AM  kitani_toshikazu
  Supported build in ProDG environment.

  Revision 1.7  02/22/2005 12:30:25 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.6  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.5  07/23/2004 03:02:42 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.4  06/22/2004 06:28:21 AM  kitani_toshikazu
  Change of debug output functions to dummy functions during FINAL ROM build

  Revision 1.3  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:55 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_NMC_LOAD_H_
#define NNS_G2D_NMC_LOAD_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL NNS_G2dGetUnpackedMultiCellBank
( 
    void*                       pNmcrFile, 
    NNSG2dMultiCellDataBank**   ppMCBank 
);
void NNS_G2dUnpackNMC( NNSG2dMultiCellDataBank* pMCellData );
const NNSG2dMultiCellData* NNS_G2dGetMultiCellDataByIdx( const NNSG2dMultiCellDataBank* pDataBank, u16 idx );


#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintNodeHierData   ( const NNSG2dMultiCellHierarchyData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintMultiCellInfo  ( const NNSG2dMultiCellData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintMultiCellBank  ( const NNSG2dMultiCellDataBank* p1) NNS_G2D_DEBUG_FUNC_DECL_END
#else
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintNodeHierData   ( const NNSG2dMultiCellHierarchyData* ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintMultiCellInfo  ( const NNSG2dMultiCellData* ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNS_G2dPrintMultiCellBank  ( const NNSG2dMultiCellDataBank* ) NNS_G2D_DEBUG_FUNC_DECL_END
#endif

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_NMC_LOAD_H_

