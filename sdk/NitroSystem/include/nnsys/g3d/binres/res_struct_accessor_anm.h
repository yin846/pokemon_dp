/*---------------------------------------------------------------------------*
  Project:  NitroSystem - G3D(Hokotate) -
  File:     res_struct_accessor_anm.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_struct_accessor_anm.h,v $
  Revision 1.13  10/25/2004 01:26:58  nishida_kenji
  Remove NNSi_G3dGetVisAnm.

  Revision 1.12  09/22/2004 05:44:39  nishida_kenji
  remove unused codes.

  Revision 1.11  09/13/2004 11:02:13  nishida_kenji
  remove unused codes

  Revision 1.10  08/03/2004 08:43:55  nishida_kenji
  Add NNS_G3dGetAnmByIdx, NNS_G3dGetAnmByName

  Revision 1.9  08/03/2004 04:25:58  nishida_kenji
  Add NNSi_G3dIsValidAnmRes

  Revision 1.8  07/26/2004 01:33:14  hatamoto_minoru
  change function name.
  NNS_G3dGetNNSG3dResJntAnmByIdx -> NNS_G3dGetJntAnmByIdx

  Revision 1.7  07/22/2004 09:15:52  kitani_toshikazu
  Renamed NNS_G3dGetTexPatAnm() to NNS_G3dGetTexPatAnmByIdx()

  Revision 1.6  07/22/2004 06:02:37  kitani_toshikazu
  Renamed Functions
  NNS_G3dGetTexPatAnmDataByIdx()
      => NNSi_G3dGetTexPatAnmDataByIdx()
  NNSi_G3dGetTexNameByIdx()
      => NNSi_G3dGetTexPatAnmTexNameByIdx()
  NNSi_G3dGetPlttNameByIdx()
      => NNSi_G3dGetTexPatAnmPlttNameByIdx()

  Revision 1.5  07/22/2004 05:42:15  kitani_toshikazu
  Renamed Functions

  Revision 1.4  07/21/2004 11:54:46  kitani_toshikazu
  Changed the argument format of NNS_G3dGetJntSRTAnmResult()

  Revision 1.3  07/21/2004 09:08:31  kitani_toshikazu
  Added NNS_G3dGetJntSRTAnmResult()

  Revision 1.2  07/06/2004 04:19:28  kitani_toshikazu
  Added NNSi_G3dGetTexPatAnmFVByFVIndex()

  Revision 1.1  06/28/2004 08:53:35  kitani_toshikazu
  Described the animation type related to the accessor

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_BINRES_RES_STRUCT_ACCESSOR_ANM_H_
#define NNSG3D_BINRES_RES_STRUCT_ACCESSOR_ANM_H_

#include <nnsys/g3d/binres/res_struct.h>


#ifdef _CVTR
#include <assert.h>
#define NNS_G3D_ASSERT(x) assert((x))
#define NNS_G3D_NULL_ASSERT(x) assert(NULL != (x))
#else
#include <nnsys/g3d/config.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

//
// Common
//
const void* 
NNSi_G3dGetBinaryBlockFromFile
( 
    const u8*  pFileHead, 
    u32        fileSignature, 
    u32        blockSignature 
);

BOOL NNSi_G3dIsValidAnmRes(const void* pRes);
void* NNS_G3dGetAnmByIdx(const void* pRes, u32 idx);
void* NNS_G3dGetAnmByName(const void* pRes, const NNSG3dResName* pName);

//
// NCBVA
//
const NNSG3dResVisAnm*
NNS_G3dGetVisAnmByIdx( const NNSG3dResVisAnmSet* pAnmSet, u8 idx  );

const NNSG3dResVisAnmSet*
NNS_G3dGetVisAnmSet( const u8* pFileHead );


//
// NCBTP
//

const NNSG3dResName* 
NNSi_G3dGetTexPatAnmTexNameByIdx( const NNSG3dResTexPatAnm* pPatAnm, u8 texIdx );

const NNSG3dResName* 
NNSi_G3dGetTexPatAnmPlttNameByIdx( const NNSG3dResTexPatAnm* pPatAnm, u8 plttIdx );


const NNSG3dResTexPatAnmFV* 
NNSi_G3dGetTexPatAnmFVByFVIndex( const NNSG3dResTexPatAnm* pPatAnm, u32 idx, u32 fvIdx );

const NNSG3dResTexPatAnmFV* 
NNSi_G3dGetTexPatAnmFV( const NNSG3dResTexPatAnm* pPatAnm, u32 idx, u32 frame );




const NNSG3dResDictTexPatAnmData* 
NNSi_G3dGetTexPatAnmDataByIdx( const NNSG3dResTexPatAnm* pPatAnm, u32 idx );

const NNSG3dResTexPatAnm*
NNS_G3dGetTexPatAnmByIdx( const NNSG3dResTexPatAnmSet* pAnmSet, u8 idx );

const NNSG3dResTexPatAnmSet*
NNS_G3dGetTexPatAnmSet( const u8* pFileHead );


//
// NSBTA
//
const NNSG3dResTexSRTAnm*
NNS_G3dGetTexSRTAnmByIdx( const NNSG3dResTexSRTAnmSet* pTexSRTSet, u8 idx );

const NNSG3dResTexSRTAnmSet*
NNS_G3dGetTexSRTAnmSet( const u8* pFileHead );


//
// NSBMA
//
const NNSG3dResMatCAnm*
NNS_G3dGetMatCAnmByIdx( const NNSG3dResMatCAnmSet* pAnmSet, u8 idx );

const NNSG3dResMatCAnmSet*
NNS_G3dGetMatCAnmSet( const u8* pFileHead );


//
// NSBCA
//
const NNSG3dResJntAnmSRTTag*
NNS_G3dGetJntAnmSRTTag
(
    const NNSG3dResJntAnm*       pJntAnm, 
    int                          nodeIdx
);

const NNSG3dResJntAnm* 
NNS_G3dGetJntAnmByIdx
( 
    const NNSG3dResJntAnmSet*    pJntAnmSet, 
    int                          idx 
);

const NNSG3dResJntAnmSet*
NNS_G3dGetJntAnmSet( const u8* pFileHead );


#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // NNSG3D_BINRES_RES_STRUCT_ACCESSOR_ANM_H_

