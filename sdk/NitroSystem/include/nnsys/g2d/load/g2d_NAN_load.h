/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_NAN_load.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NAN_load.h,v $
  Revision 1.11  05/31/2005 06:55:53 AM  kitani_toshikazu
  Supported build in ProDG environment.

  Revision 1.10  02/22/2005 12:30:25 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.9  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.8  07/28/2004 08:13:01 AM  kitani_toshikazu
  Corrected comment

  Revision 1.7  07/23/2004 03:02:42 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.6  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.5  07/01/2004 04:23:44 AM  kitani_toshikazu
  Added NNS_G2dGetUnpackedMCAnimBank()

  Revision 1.4  06/22/2004 06:28:22 AM  kitani_toshikazu
  Change of debug output functions to dummy functions during FINAL ROM build

  Revision 1.3  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:55 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_NAN_LOAD_H_
#define NNS_G2D_NAN_LOAD_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>

BOOL NNS_G2dGetUnpackedAnimBank( void* pNanrFile, NNSG2dAnimBankData** ppAnimBank );
BOOL NNS_G2dGetUnpackedMCAnimBank( void* pNanrFile, NNSG2dAnimBankData** ppAnimBank );

void NNS_G2dUnpackNAN( NNSG2dAnimBankData* pData );

//------------------------------------------------------------------------------
static inline u16 NNS_G2dGetNumAnimSequence( const NNSG2dAnimBankData* pAnimBank )
{
    NNS_G2D_NULL_ASSERT( pAnimBank );
    return pAnimBank->numSequences;
}

const NNSG2dAnimSequenceData* NNS_G2dGetAnimSequenceByIdx( const NNSG2dAnimBankData* pAnimBank, u16 idx );


//------------------------------------------------------------------------------
const char* NNS_G2dGetAnimSequenceTypeStr  ( NNSG2dAnimationType );


//------------------------------------------------------------------------------
//#include "nnsys/g2d/load/g2di_Begin_LoadDebugFuncDecl.h"

#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimContents       ( const void* p1, NNSG2dAnimationElement p2) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimFrame          ( const NNSG2dAnimFrameData* p1, NNSG2dAnimationElement p2) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimSequence       ( const NNSG2dAnimSequenceData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimBank           ( const NNSG2dAnimBankData* p1) NNS_G2D_DEBUG_FUNC_DECL_END
#else
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimContents       ( const void* , NNSG2dAnimationElement ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimFrame          ( const NNSG2dAnimFrameData* , NNSG2dAnimationElement ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimSequence       ( const NNSG2dAnimSequenceData* ) NNS_G2D_DEBUG_FUNC_DECL_END
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void        NNS_G2dPrintAnimBank           ( const NNSG2dAnimBankData* ) NNS_G2D_DEBUG_FUNC_DECL_END
#endif

//#include "nnsys/g2d/load/g2di_End_LoadDebugFuncDecl.h"
//------------------------------------------------------------------------------

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_NAN_LOAD_H_

