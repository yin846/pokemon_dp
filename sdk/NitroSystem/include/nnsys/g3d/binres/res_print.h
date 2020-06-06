/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_print.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_print.h,v $
  Revision 1.8  09/17/2004 01:59:01  nishida_kenji
  REVISED

  Revision 1.7  09/15/2004 04:51:38  nishida_kenji
  rename functions for Vis anm.

  Revision 1.6  07/07/2004 12:41:07  nishida_kenji
  small fix

  Revision 1.5  07/06/2004 11:57:36  nishida_kenji
  Add NNS_G3dPrintDL(under construction)

  Revision 1.4  06/28/2004 08:53:35  kitani_toshikazu
  Described the animation type related to the accessor

  Revision 1.3  06/16/2004 23:59:16  nishida_kenji
  NitroSystem -> NITRO-System

  Revision 1.2  06/15/2004 00:50:11  nishida_kenji
  (none)

  Revision 1.1  06/11/2004 04:11:12  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_BINRES_RES_PRINT_H_
#define NNSG3D_BINRES_RES_PRINT_H_

#include <nnsys/g3d/binres/res_struct.h>


#ifdef __cplusplus
extern "C" {
#endif


#if defined(SDK_DEBUG) || defined(_CVTR)
#define NNS_RES_PRINT
#define NNS_RES_PRINT_END ;
#else
#define NNS_RES_PRINT     inline
#define NNS_RES_PRINT_END {}
#endif
//
// Common
//
NNS_RES_PRINT void NNS_G3dPrintBinFile(const u8*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintResName(const NNSG3dResName*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintFileHeader(const NNSG3dResFileHeader*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintDataBlockHeader(const NNSG3dResDataBlockHeader*) NNS_RES_PRINT_END

fx32 CvtrFX_Mul( fx32 v1, fx32 v2 );

//
// .nsbmd, .nsbtx
//
NNS_RES_PRINT void NNS_G3dPrintDictEntryItem_TEX(u32,
                                                 const NNSG3dResName*,
                                                 const NNSG3dResDictTexData*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintTex(const NNSG3dResTex*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintDictEntryItem_PLTT(u32, 
                                                  const NNSG3dResName*,
                                                  const NNSG3dResDictPlttData*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNSBTX(const u8*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintMdlInfo(const NNSG3dResMdlInfo*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNodeSRTInfo(const NNSG3dResNodeInfo*, u32) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNodeInfo(const NNSG3dResNodeInfo*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintSbc(const u8*, u32, const NNSG3dResMdl*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintDL(const u32*, u32) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintMatData(const NNSG3dResMat*, u32) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintMat(const NNSG3dResMat*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintShp(const NNSG3dResShp*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintMdl(const NNSG3dResName*, const NNSG3dResMdl*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintMdlSet(const NNSG3dResMdlSet*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNSBMD(const u8*) NNS_RES_PRINT_END

//
// .nsbca
//
NNS_RES_PRINT void NNS_G3dPrintJntAnm( const NNSG3dResJntAnm* ) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintJntAnmSet( const NNSG3dResJntAnmSet* ) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNSBCA(const u8* ) NNS_RES_PRINT_END

//
// .nsbva
//
NNS_RES_PRINT void NNS_G3dPrintVisAnm(const NNSG3dResVisAnm* ) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintVisAnmSet(const NNSG3dResVisAnmSet* ) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNSBVA(const u8*) NNS_RES_PRINT_END

//
// .nsbma
//
NNS_RES_PRINT void NNS_G3dPrintMatColAnm(const NNSG3dResMatCAnm*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintMatColAnmSet( const NNSG3dResMatCAnmSet* ) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNSBMA(const u8*) NNS_RES_PRINT_END

//
// .nsbtp
//
NNS_RES_PRINT void NNS_G3dPrintTexPatAnm( const NNSG3dResTexPatAnm* ) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintTexPatAnmSet( const NNSG3dResTexPatAnmSet* ) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNSBTP(const u8* ) NNS_RES_PRINT_END

//
// .nsbta
//
NNS_RES_PRINT void NNS_G3dPrintTexSRTAnm(const NNSG3dResTexSRTAnm*) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintTexSRTAnmSet( const NNSG3dResTexSRTAnmSet* ) NNS_RES_PRINT_END
NNS_RES_PRINT void NNS_G3dPrintNSBTA(const u8*) NNS_RES_PRINT_END







#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif

