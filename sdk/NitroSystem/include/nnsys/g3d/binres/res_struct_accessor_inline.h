/*---------------------------------------------------------------------------*
  Project:  NitroSystem - G3D(Hokotate) -
  File:     res_struct_accessor_inline.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_struct_accessor_inline.h,v $
  Revision 1.4  07/16/2004 07:24:35  nishida_kenji
  add accessors.

  Revision 1.3  07/14/2004 11:29:40  nishida_kenji
  rename NNS_G3dGetMdlSet to NNS_G3dCheckMdlSet,
            NNS_G3dGetTex to NNS_G3dCheckGetTex.
  now, NNS_G3dGetMdlSet, NNS_G3dGetTex have different meanings.

  Revision 1.2  2004/06/23 09:11:58  nishida_kenji
  (none)

  Revision 1.1  06/22/2004 01:31:42  nishida_kenji
  split inline functions.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_BINRES_RES_STRUCT_ACCESSOR_INLINE_H_
#define NNSG3D_BINRES_RES_STRUCT_ACCESSOR_INLINE_H_
//
// DO NOT INCLUDE THIS FILE DIRECTLY
//

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Dictionaries
//
//

NNS_G3D_RES_INLINE const NNSG3dResName*
NNS_G3dGetResNameByIdx(const NNSG3dResDict* dict, u32 idx)
{
    NNSG3dResDictEntryHeader* hdr;
    NNS_G3D_NULL_ASSERT(dict);
    NNS_G3D_ASSERT(idx < dict->numEntry);

    hdr = (NNSG3dResDictEntryHeader*)((u8*)dict + dict->ofsEntry);
    return (NNSG3dResName*)((u8*)hdr      +
                            hdr->ofsName  +
                            sizeof(NNSG3dResName) * idx);
}

NNS_G3D_RES_INLINE void*
NNS_G3dGetResDataByIdx(const NNSG3dResDict* dict, u32 idx)
{
    NNSG3dResDictEntryHeader* hdr;
    NNS_G3D_NULL_ASSERT(dict);
    NNS_G3D_ASSERT(idx < dict->numEntry);

    hdr = (NNSG3dResDictEntryHeader*)((u8*)dict + dict->ofsEntry);
    return (void*)(&hdr->data[0] + hdr->sizeUnit * idx);  
}



//
// Accessors for NNSG3dResTex
//

NNS_G3D_RES_INLINE const NNSG3dResName*
NNS_G3dGetTexNameByIdx(const NNSG3dResTex* tex, u32 idx)
{
    NNS_G3D_NULL_ASSERT(tex);

    return NNS_G3dGetResNameByIdx(&tex->dict, idx);
}

NNS_G3D_RES_INLINE int
NNS_G3dGetTexIdxByName(const NNSG3dResTex* tex, const NNSG3dResName* name)
{
    NNS_G3D_NULL_ASSERT(tex);
    NNS_G3D_NULL_ASSERT(name);

    return NNS_G3dGetResDictIdxByName(&tex->dict, name);
}


NNS_G3D_RES_INLINE const NNSG3dResName*
NNS_G3dGetPlttNameByIdx(const NNSG3dResTex* tex, u32 idx)
{
    const NNSG3dResDict* dict;

    NNS_G3D_NULL_ASSERT(tex);
    dict = (const NNSG3dResDict*)((u8*)tex + tex->plttInfo.ofsDict);
    return NNS_G3dGetResNameByIdx(dict, idx);
}


NNS_G3D_RES_INLINE int
NNS_G3dGetPlttIdxByName(const NNSG3dResTex* tex, const NNSG3dResName* name)
{
    const NNSG3dResDict* dict;

    NNS_G3D_NULL_ASSERT(tex);
    NNS_G3D_NULL_ASSERT(name);
    dict = (const NNSG3dResDict*)((u8*)tex + tex->plttInfo.ofsDict);
    return NNS_G3dGetResDictIdxByName(dict, name);
}

//
// Accessors for NNSG3dResNodeInfo
//

NNS_G3D_RES_INLINE const NNSG3dResName*
NNS_G3dGetNodeNameByIdx(const NNSG3dResNodeInfo* nodeinfo, u32 idx)
{
    NNS_G3D_NULL_ASSERT(nodeinfo);
    return NNS_G3dGetResNameByIdx(&nodeinfo->dict, idx);
}


NNS_G3D_RES_INLINE int
NNS_G3dGetNodeIdxByName(const NNSG3dResNodeInfo* nodeinfo, const NNSG3dResName* name)
{
    NNS_G3D_NULL_ASSERT(nodeinfo);
    NNS_G3D_NULL_ASSERT(name);

    return NNS_G3dGetResDictIdxByName(&nodeinfo->dict, name);
}


//
// Accessors for NNSG3dResMat
//

NNS_G3D_RES_INLINE const NNSG3dResName*
NNS_G3dGetMatNameByIdx(const NNSG3dResMat* mat, u32 idx)
{
    NNS_G3D_NULL_ASSERT(mat);
    return NNS_G3dGetResNameByIdx(&mat->dict, idx);
}


NNS_G3D_RES_INLINE int
NNS_G3dGetMatIdxByName(const NNSG3dResMat* mat, const NNSG3dResName* name)
{
    NNS_G3D_NULL_ASSERT(mat);
    NNS_G3D_NULL_ASSERT(name);

    return NNS_G3dGetResDictIdxByName(&mat->dict, name);
}

//
// Accessors for NNSG3dResShp
//

NNS_G3D_RES_INLINE const NNSG3dResName*
NNS_G3dGetShpNameByIdx(const NNSG3dResShp* shp, u32 idx)
{
    NNS_G3D_NULL_ASSERT(shp);
    return NNS_G3dGetResNameByIdx(&shp->dict, idx);
}


NNS_G3D_RES_INLINE int
NNS_G3dGetShpIdxByName(const NNSG3dResShp* shp, const NNSG3dResName* name)
{
    NNS_G3D_NULL_ASSERT(shp);
    NNS_G3D_NULL_ASSERT(name);

    return NNS_G3dGetResDictIdxByName(&shp->dict, name);
}


//
// Accessors for NNSG3dResMdl
//

NNS_G3D_RES_INLINE NNSG3dResMdlInfo*
NNS_G3dGetMdlInfo(const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);

    return (NNSG3dResMdlInfo*)&mdl->info;
}


NNS_G3D_RES_INLINE NNSG3dResNodeInfo*
NNS_G3dGetNodeInfo(const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);

    return (NNSG3dResNodeInfo*)&mdl->nodeInfo;
}


NNS_G3D_RES_INLINE u8*
NNS_G3dGetSbc(const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);

    return (u8*)((u8*)mdl + mdl->ofsSbc);
}


NNS_G3D_RES_INLINE NNSG3dResMat*
NNS_G3dGetMat(const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);

    return (NNSG3dResMat*)((u8*)mdl + mdl->ofsMat);
}


NNS_G3D_RES_INLINE NNSG3dResShp*
NNS_G3dGetShp(const NNSG3dResMdl* mdl)
{
    NNS_G3D_NULL_ASSERT(mdl);

    return (NNSG3dResShp*)((u8*)mdl + mdl->ofsShp);
}

//
// Accessors for NNSG3dResMdlSet
//

NNS_G3D_RES_INLINE NNSG3dResMdl*
NNS_G3dGetMdlByIdx(const NNSG3dResMdlSet* mdlSet, u32 idx)
{
    NNSG3dResDictMdlSetData* data;
    NNS_G3D_NULL_ASSERT(mdlSet);

    data = (NNSG3dResDictMdlSetData*)NNS_G3dGetResDataByIdx(&mdlSet->dict, idx);
    return (NNSG3dResMdl*)((u8*)mdlSet + data->offset);
}


NNS_G3D_RES_INLINE const NNSG3dResName*
NNS_G3dGetMdlNameByIdx(const NNSG3dResMdlSet* mdlSet, u32 idx)
{
    NNS_G3D_NULL_ASSERT(mdlSet);
    return NNS_G3dGetResNameByIdx(&mdlSet->dict, idx);
}


NNS_G3D_RES_INLINE NNSG3dResMdl*
NNS_G3dGetMdlByName(const NNSG3dResMdlSet* mdlSet, const NNSG3dResName* name)
{
    NNSG3dResDictMdlSetData* data;
    NNS_G3D_NULL_ASSERT(mdlSet);
    NNS_G3D_NULL_ASSERT(name);

    data = (NNSG3dResDictMdlSetData*)NNS_G3dGetResDataByName(&mdlSet->dict, name);
    return (NNSG3dResMdl*)((u8*)mdlSet + data->offset);
}


NNS_G3D_RES_INLINE int
NNS_G3dGetMdlIdxByName(const NNSG3dResMdlSet* mdlSet, const NNSG3dResName* name)
{
    NNS_G3D_NULL_ASSERT(mdlSet);
    NNS_G3D_NULL_ASSERT(name);

    return NNS_G3dGetResDictIdxByName(&mdlSet->dict, name);
}

//
// Accessors for a data block header
//
NNS_G3D_RES_INLINE NNSG3dResMdlSet*
NNS_G3dCheckMdlSet(const NNSG3dResDataBlockHeader* dataBlkHeader)
{
    NNS_G3D_NULL_ASSERT(dataBlkHeader);

    if (dataBlkHeader->kind == NNS_G3D_DATABLK_MDLSET)
    {
        return (NNSG3dResMdlSet*)dataBlkHeader;
    }
    else
    {
        return NULL;
    }
}


NNS_G3D_RES_INLINE NNSG3dResTex*
NNS_G3dCheckTex(const NNSG3dResDataBlockHeader* dataBlkHeader)
{
    NNS_G3D_NULL_ASSERT(dataBlkHeader);

    if (dataBlkHeader->kind == NNS_G3D_DATABLK_TEX)
    {
        return (NNSG3dResTex*)dataBlkHeader;
    }
    else
    {
        return NULL;
    }
}

//
// Undocumented APIs
//

NNS_G3D_RES_INLINE NNSG3dResNodeData*
NNS_G3dGetNodeDataByIdx(const NNSG3dResNodeInfo* info, u32 idx)
{
    NNSG3dResDictNodeData* data;
    NNS_G3D_NULL_ASSERT(info);

    data = (NNSG3dResDictNodeData*)NNS_G3dGetResDataByIdx(&info->dict, idx);
    return (NNSG3dResNodeData*)((u8*)info + data->offset);
}


NNS_G3D_RES_INLINE NNSG3dResMatData*
NNS_G3dGetMatDataByIdx(const NNSG3dResMat* mat, u32 idx)
{
    NNSG3dResDictMatData* data;
    NNS_G3D_NULL_ASSERT(mat);

    data = (NNSG3dResDictMatData*)NNS_G3dGetResDataByIdx(&mat->dict, idx);
    return (NNSG3dResMatData*)((u8*)mat + data->offset);
}


NNS_G3D_RES_INLINE NNSG3dResShpData*
NNS_G3dGetShpDataByIdx(const NNSG3dResShp* shp, u32 idx)
{
    NNSG3dResDictShpData* data;
    NNS_G3D_NULL_ASSERT(shp);

    data = (NNSG3dResDictShpData*)NNS_G3dGetResDataByIdx(&shp->dict, idx);
    return (NNSG3dResShpData*)((u8*)shp + data->offset);
}

NNS_G3D_RES_INLINE NNSG3dResDictTexData*
NNS_G3dGetTexDataByName(const NNSG3dResTex* tex, const NNSG3dResName* name)
{
    NNS_G3D_NULL_ASSERT(tex);
    NNS_G3D_NULL_ASSERT(name);

    return (NNSG3dResDictTexData*)NNS_G3dGetResDataByName(&tex->dict, name);
}


NNS_G3D_RES_INLINE NNSG3dResDictTexData*
NNS_G3dGetTexDataByIdx(const NNSG3dResTex* tex, u32 idx)
{
    NNS_G3D_NULL_ASSERT(tex);

    return (NNSG3dResDictTexData*)NNS_G3dGetResDataByIdx(&tex->dict, idx);
}


NNS_G3D_RES_INLINE NNSG3dResDictPlttData*
NNS_G3dGetPlttDataByName(const NNSG3dResTex* tex, const NNSG3dResName* name)
{
    NNSG3dResDict* dict;
    NNS_G3D_NULL_ASSERT(tex);
    NNS_G3D_NULL_ASSERT(name);

    dict = (NNSG3dResDict*)((u8*)tex + tex->plttInfo.ofsDict);
    return (NNSG3dResDictPlttData*)NNS_G3dGetResDataByName(dict, name);
}


NNS_G3D_RES_INLINE NNSG3dResDictPlttData*
NNS_G3dGetPlttDataByIdx(const NNSG3dResTex* tex, u32 idx)
{
    NNSG3dResDict* dict;
    NNS_G3D_NULL_ASSERT(tex);

    dict = (NNSG3dResDict*)((u8*)tex + tex->plttInfo.ofsDict);
    return (NNSG3dResDictPlttData*)NNS_G3dGetResDataByIdx(dict, idx);
}



//
// Accessors for animation resource
//
NNS_G3D_RES_INLINE BOOL
NNS_G3dIsMatAnm(const void* pAnm)
{
    const NNSG3dResAnmHeader* hdr = (const NNSG3dResAnmHeader*) pAnm;
    return hdr->category0 == 'M';
}


NNS_G3D_RES_INLINE BOOL
NNS_G3dIsJntAnm(const void* pAnm)
{
    const NNSG3dResAnmHeader* hdr = (const NNSG3dResAnmHeader*) pAnm;
    return hdr->category0 == 'J';
}


NNS_G3D_RES_INLINE BOOL
NNS_G3dIsVisAnm(const void* pAnm)
{
    const NNSG3dResAnmHeader* hdr = (const NNSG3dResAnmHeader*) pAnm;
    return hdr->category0 == 'V';
}


#ifdef __cplusplus
}
#endif

#endif

