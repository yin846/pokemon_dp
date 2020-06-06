/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     .h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_PaletteTable.h,v $
  Revision 1.3  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.2  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.1  04/23/2004 12:26:50 AM  kitani_toshikazu
  Added workaround for color palette swap table.



  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_PALETTETABLE_H_
#define NNS_G2D_PALETTETABLE_H_ 

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
#define NNS_G2D_NUM_COLOR_PALETTE   16




//------------------------------------------------------------------------------
typedef struct NNSG2dPaletteSwapTable
{
    u16        paletteIndex[NNS_G2D_NUM_COLOR_PALETTE];

}NNSG2dPaletteSwapTable;






//------------------------------------------------------------------------------
void NNS_G2dInitializePaletteTable  ( NNSG2dPaletteSwapTable* pPlttTbl );
void NNS_G2dSetPaletteTableValue    ( NNSG2dPaletteSwapTable* pPlttTbl, u16 beforeIdx, u16 afterIdx );
u16 NNS_G2dGetPaletteTableValue     ( const NNSG2dPaletteSwapTable* pPlttTbl, u16 beforeIdx );








#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_PALETTETABLE_H_

