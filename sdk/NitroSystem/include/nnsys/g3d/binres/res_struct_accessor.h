/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_struct_accessor.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_struct_accessor.h,v $
  Revision 1.11  10/06/2004 04:45:13  nishida_kenji
  add NNS_G3D_ASSERTMSG.

  Revision 1.10  07/27/2004 10:45:36  nishida_kenji
  small fix

  Revision 1.9  07/19/2004 10:55:01  nishida_kenji
  remove obsolete functions

  Revision 1.8  07/16/2004 07:24:35  nishida_kenji
  add accessors.

  Revision 1.7  07/14/2004 11:30:05  nishida_kenji
  rename NNS_G3dGetMdlSet to NNS_G3dCheckMdlSet,
            NNS_G3dGetTex to NNS_G3dCheckGetTex.
  now, NNS_G3dGetMdlSet, NNS_G3dGetTex have different meanings.

  Revision 1.6  06/28/2004 08:53:35  kitani_toshikazu
  Described the animation type related to the accessor

  Revision 1.5  2004/06/23 09:12:06  nishida_kenji
  (none)

  Revision 1.4  06/22/2004 01:31:48  nishida_kenji
  split inline functions.

  Revision 1.3  06/16/2004 23:59:16  nishida_kenji
  NitroSystem -> NITRO-System

  Revision 1.2  06/15/2004 00:50:11  nishida_kenji
  (none)

  Revision 1.1  06/11/2004 04:11:12  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_BINRES_RES_STRUCT_ACCESSOR_H_
#define NNSG3D_BINRES_RES_STRUCT_ACCESSOR_H_

#include <nnsys/g3d/binres/res_struct.h>

#include <nnsys/g3d/binres/res_struct_accessor_anm.h>

#ifdef _CVTR
#include <assert.h>
#define NNS_G3D_ASSERT(x) assert((x))
#define NNS_G3D_ASSERTMSG(x, y) assert((x))
#define NNS_G3D_NULL_ASSERT(x) assert(NULL != (x))
#else
#include <nnsys/g3d/config.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Dictionaries
//
//

//
// inline functions
//
NNS_G3D_RES_INLINE const NNSG3dResName* NNS_G3dGetResNameByIdx(const NNSG3dResDict* dict, u32 idx);
NNS_G3D_RES_INLINE void*                NNS_G3dGetResDataByIdx(const NNSG3dResDict* dict, u32 idx);

//
// non-inline functions
//
void* NNS_G3dGetResDataByName(const NNSG3dResDict* dict, const NNSG3dResName* name);
int   NNS_G3dGetResDictIdxByName(const NNSG3dResDict* dict, const NNSG3dResName* name);

////////////////////////////////////////////////////////////////////////////////
//
// Model(.nsbmd, nsbtx)
//
//

//
// Accessors for NNSG3dResTex(inline)
//
NNS_G3D_RES_INLINE const NNSG3dResName* NNS_G3dGetTexNameByIdx(const NNSG3dResTex* tex, u32 idx);
NNS_G3D_RES_INLINE int                  NNS_G3dGetTexIdxByName(const NNSG3dResTex* tex,
                                                               const NNSG3dResName* name);
NNS_G3D_RES_INLINE const NNSG3dResName* NNS_G3dGetPlttNameByIdx(const NNSG3dResTex* tex, u32 idx);
NNS_G3D_RES_INLINE int                  NNS_G3dGetPlttIdxByName(const NNSG3dResTex* tex,
                                                                const NNSG3dResName* name);


//
// Accessors for NNSG3dResNodeInfo(inline)
//
NNS_G3D_RES_INLINE const NNSG3dResName* NNS_G3dGetNodeNameByIdx(const NNSG3dResNodeInfo* nodeinfo, u32 idx);
NNS_G3D_RES_INLINE int                  NNS_G3dGetNodeIdxByName(const NNSG3dResNodeInfo* nodeinfo,
                                                                const NNSG3dResName* name);


//
// Accessors for NNSG3dResMat(inline)
//

NNS_G3D_RES_INLINE const NNSG3dResName* NNS_G3dGetMatNameByIdx(const NNSG3dResMat* mat, u32 idx);
NNS_G3D_RES_INLINE int                  NNS_G3dGetMatIdxByName(const NNSG3dResMat* mat,
                                                               const NNSG3dResName* name);


//
// Accessors for NNSG3dResShp(inline)
//

NNS_G3D_RES_INLINE const NNSG3dResName* NNS_G3dGetShpNameByIdx(const NNSG3dResShp* shp, u32 idx);
NNS_G3D_RES_INLINE int                  NNS_G3dGetShpIdxByName(const NNSG3dResShp* shp,
                                                               const NNSG3dResName* name);

//
// Accessors for NNSG3dResMdl(inline)
//

NNS_G3D_RES_INLINE NNSG3dResMdlInfo*  NNS_G3dGetMdlInfo(const NNSG3dResMdl* mdl);
NNS_G3D_RES_INLINE NNSG3dResNodeInfo* NNS_G3dGetNodeInfo(const NNSG3dResMdl* mdl);
NNS_G3D_RES_INLINE u8*                NNS_G3dGetSbc(const NNSG3dResMdl* mdl);
NNS_G3D_RES_INLINE NNSG3dResMat*      NNS_G3dGetMat(const NNSG3dResMdl* mdl);
NNS_G3D_RES_INLINE NNSG3dResShp*      NNS_G3dGetShp(const NNSG3dResMdl* mdl);


//
// Accessors for NNSG3dResMdlSet(inline)
//

NNS_G3D_RES_INLINE NNSG3dResMdl*        NNS_G3dGetMdlByIdx(const NNSG3dResMdlSet* mdlSet, u32 idx);
NNS_G3D_RES_INLINE const NNSG3dResName* NNS_G3dGetMdlNameByIdx(const NNSG3dResMdlSet* mdlSet, u32 idx);
NNS_G3D_RES_INLINE NNSG3dResMdl*        NNS_G3dGetMdlByName(const NNSG3dResMdlSet* mdlSet,
                                                            const NNSG3dResName* name);
NNS_G3D_RES_INLINE int                  NNS_G3dGetMdlIdxByName(const NNSG3dResMdlSet* mdlSet,
                                                               const NNSG3dResName* name);


//
// Accessors for a data block header(inline)
//
NNS_G3D_RES_INLINE NNSG3dResMdlSet* NNS_G3dCheckMdlSet(const NNSG3dResDataBlockHeader* dataBlkHeader);
NNS_G3D_RES_INLINE NNSG3dResTex*    NNS_G3dCheckTex(const NNSG3dResDataBlockHeader* dataBlkHeader);


//
// Accessors for a data block header
//
NNSG3dResMdlSet* NNS_G3dGetMdlSet(const NNSG3dResFileHeader* header);
NNSG3dResTex* NNS_G3dGetTex(const NNSG3dResFileHeader* header);


//
// Accessors for a binary file header
//

NNSG3dResDataBlockHeader*
NNS_G3dGetDataBlockHeaderByIdx(const NNSG3dResFileHeader* header, u32 idx);


//
// Undocumented APIs(inline)
//

NNS_G3D_RES_INLINE NNSG3dResNodeData* NNS_G3dGetNodeDataByIdx(const NNSG3dResNodeInfo* info, u32 idx);
NNS_G3D_RES_INLINE NNSG3dResMatData*  NNS_G3dGetMatDataByIdx(const NNSG3dResMat* mat, u32 idx);
NNS_G3D_RES_INLINE NNSG3dResShpData*  NNS_G3dGetShpDataByIdx(const NNSG3dResShp* shp, u32 idx);
NNS_G3D_RES_INLINE NNSG3dResDictTexData* NNS_G3dGetTexDataByName(const NNSG3dResTex* tex, const NNSG3dResName* name);
NNS_G3D_RES_INLINE NNSG3dResDictTexData* NNS_G3dGetTexDataByIdx(const NNSG3dResTex* tex, u32 idx);
NNS_G3D_RES_INLINE NNSG3dResDictPlttData* NNS_G3dGetPlttDataByName(const NNSG3dResTex* tex, const NNSG3dResName* name);
NNS_G3D_RES_INLINE NNSG3dResDictPlttData* NNS_G3dGetPlttDataByIdx(const NNSG3dResTex* tex, u32 idx);

////////////////////////////////////////////////////////////////////////////////
//
// Animation
//
//
NNS_G3D_RES_INLINE BOOL NNS_G3dIsMatAnm(const void* pAnm);
NNS_G3D_RES_INLINE BOOL NNS_G3dIsJntAnm(const void* pAnm);
NNS_G3D_RES_INLINE BOOL NNS_G3dIsVisAnm(const void* pAnm);





#ifdef __cplusplus
}/* extern "C"*/
#endif

#include <nnsys/g3d/binres/res_struct_accessor_inline.h>


#endif // NNSG3D_BINRES_RES_STRUCT_ACCESSOR_H_

