/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     kernel.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: kernel.c,v $
  Revision 1.27  02/25/2005 05:38:42  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.26  11/09/2004 02:15:48  nishida_kenji
  fix comments.

  Revision 1.25  10/18/2004 11:15:35  nishida_kenji
  small fix.

  Revision 1.23  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.22  09/27/2004 09:58:07  nishida_kenji
  Add NNS_G3dRenderObjSetInitFunc,
  remove NNSG3dRenderObj::cbCmdPtr.

  Revision 1.21  08/26/2004 07:18:54  nishida_kenji
  Use NNS_G3D_ANMOBJ_SIZE_MATANM, NNS_G3D_ANMOBJ_SIZE_JNTANM, and NNS_G3D_ANMOBJ_SIZE_VISANM

  Revision 1.20  08/24/2004 09:32:48  nishida_kenji
  bugfix(updateHintVec_).

  Revision 1.18  08/04/2004 09:01:36  nishida_kenji
  the default value of NNSG3dAnmObj::ratio is FX32_ONE.

  Revision 1.17  08/03/2004 06:47:56  nishida_kenji
  bugfix(updateHintVec_).

  Revision 1.16  07/26/2004 02:21:26  nishida_kenji
  bug fix(NNS_G3dRenderObjRemoveAnmObj)

  Revision 1.15  07/21/2004 06:33:24  nishida_kenji
  Add NNS_G3dRenderObjResetCallBack

  Revision 1.14  07/21/2004 02:25:19  nishida_kenji
  revise NNS_G3dRenderObjRemoveAnmObj

  Revision 1.13  07/20/2004 07:27:04  nishida_kenji
  Add an API for callback.

  Revision 1.12  07/20/2004 01:37:05  nishida_kenji
  fixed comments

  Revision 1.11  07/19/2004 10:56:18  nishida_kenji
  NNS_G3dAnmObjInit takes ptr to resTex

  Revision 1.10  07/17/2004 12:55:28  nishida_kenji
  Add NNS_G3dBindMdlSet, NNS_G3dReleaseMdlSet

  Revision 1.9  07/16/2004 07:24:14  nishida_kenji
  Add NNS_G3dForceBindMdl[Tex|Pltt].

  Revision 1.6  07/14/2004 11:33:08  nishida_kenji
  Add functions to setup textures.

  Revision 1.1  07/07/2004 12:41:36  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>


/*---------------------------------------------------------------------------*
    NNS_G3dAnmObjCalcSizeRequired

    Returns the memory size needed by NNSG3dAnmObj for animation resources
    and model resources.
 *---------------------------------------------------------------------------*/
u32
NNS_G3dAnmObjCalcSizeRequired(const void* pAnm,
                              const NNSG3dResMdl* pMdl)
{
    const NNSG3dResAnmHeader* hdr;
    NNS_G3D_NULL_ASSERT(pAnm && pMdl);

    hdr = (const NNSG3dResAnmHeader*)pAnm;
    switch(hdr->category0)
    {
    case 'M': // material animation
        return NNS_G3D_ANMOBJ_SIZE_MATANM(pMdl);
        break;
    case 'J': // character(joint) animation
    case 'V': // visibility animation
        return NNS_G3D_ANMOBJ_SIZE_JNTANM(pMdl);
        break;
    default:
        NNS_G3D_ASSERT(0);
        return 0;
        break;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dInitAnmObj

    initialize pAnmObj for pResAnm and pResMdl

    NOTICE:
    pAnmObj must be holding at least the size of
    NNS_G3dAnmObjCalcSizeRequired(pResAnm, pResMdl).
 *---------------------------------------------------------------------------*/
void
NNS_G3dAnmObjInit(NNSG3dAnmObj* pAnmObj,
                  void* pResAnm,
                  const NNSG3dResMdl* pResMdl,
                  const NNSG3dResTex* pResTex)
{
    // Sometimes, programmer-defined animations will not have resources.
    // In such cases, it is OK to fake and input just the header part.
    const NNSG3dResAnmHeader* hdr;
    u32 i;
    NNS_G3D_NULL_ASSERT(pAnmObj);
    NNS_G3D_NULL_ASSERT(pResAnm);
    NNS_G3D_NULL_ASSERT(pResMdl);
    // resTex can be NULL, except for texture pattern animation.
    NNS_G3D_ASSERT(NNS_G3dAnmFmtNum <= NNS_G3D_ANMFMT_MAX);

    pAnmObj->frame = 0;
    pAnmObj->resAnm = (void*)pResAnm;

    // Animation function settings.
    // These are decided by category1
    pAnmObj->next = NULL;
    pAnmObj->priority = 127;
    pAnmObj->ratio = FX32_ONE;
    pAnmObj->resTex = pResTex;

    hdr = (const NNSG3dResAnmHeader*)pResAnm;

    for (i = 0; i < NNS_G3dAnmFmtNum; ++i)
    {
        if (NNS_G3dAnmObjInitFuncArray[i].category0 == hdr->category0 &&
            NNS_G3dAnmObjInitFuncArray[i].category1 == hdr->category1)
        {
            (*NNS_G3dAnmObjInitFuncArray[i].func)(pAnmObj, pResAnm, pResMdl);
            break;
        }
    }
    NNS_G3D_ASSERT(i != NNS_G3dAnmFmtNum);
}


/*---------------------------------------------------------------------------
    NNS_G3dAnmObjEnableID

    Attempts to play the animation corresponding to the JntID or the MatID.
    (default)
 *---------------------------------------------------------------------------*/
void
NNS_G3dAnmObjEnableID(NNSG3dAnmObj* pAnmObj, int id)
{
    NNS_G3D_NULL_ASSERT(pAnmObj);

    if (id >= 0 && id < pAnmObj->numMapData &&
        (pAnmObj->mapData[id] & NNS_G3D_ANMOBJ_MAPDATA_EXIST))
        pAnmObj->mapData[id] &= ~NNS_G3D_ANMOBJ_MAPDATA_DISABLED;
}


/*---------------------------------------------------------------------------
    NNS_G3dAnmObjEnableID

    Tries not to play the animation corresponding to the JntID or the MatID.
 *---------------------------------------------------------------------------*/
void
NNS_G3dAnmObjDisableID(NNSG3dAnmObj* pAnmObj, int id)
{
    NNS_G3D_NULL_ASSERT(pAnmObj);

    if (id >= 0 && id < pAnmObj->numMapData &&
        (pAnmObj->mapData[id] & NNS_G3D_ANMOBJ_MAPDATA_EXIST))
        pAnmObj->mapData[id] |= NNS_G3D_ANMOBJ_MAPDATA_DISABLED;
}



/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjInit

    Initializes NNSG3dRenderObj. pResMdl may be specified as NULL.
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjInit(NNSG3dRenderObj* pRenderObj,
                     NNSG3dResMdl* pResMdl)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    // pResMdl can be NULL

    MI_CpuClear32(pRenderObj, sizeof(NNSG3dRenderObj));

    // Sets the default handler for blending
    pRenderObj->funcBlendMat = NNS_G3dFuncBlendMatDefault;
    pRenderObj->funcBlendJnt = NNS_G3dFuncBlendJntDefault;
    pRenderObj->funcBlendVis = NNS_G3dFuncBlendVisDefault;

    pRenderObj->resMdl = pResMdl;
}


/*---------------------------------------------------------------------------*
    addLink_

    Supports cases where "item" is already in the list, but if this list
    has not been sorted, the order may not be correct.
 *---------------------------------------------------------------------------*/
static void
addLink_(NNSG3dAnmObj** l, NNSG3dAnmObj* item)
{
#ifdef NITRO_DEBUG
    // Error if "item" is already in "l."
    {
        NNSG3dAnmObj* p = *l;
        while(p)
        {
            NNS_G3D_ASSERT(p != item);
            p = p->next;
        }
    }
#endif
    if (!(*l))
    {
        // Becomes first element if no list exists.
        *l = item;
    }
    else if (!((*l)->next))
    {
        // If the list has 1 element:
        if ((*l)->priority > item->priority)
        {
            // Connect list to tail of "item"
            NNSG3dAnmObj* p = item;
            while(p->next)
            {
                p = p->next;
            }
            p->next = (*l);
            *l = item;
        }
        else
        {
            // Connect to end of existing list (though just 1 element)
            (*l)->next = item;
        }
    }
    else
    {
        NNSG3dAnmObj* p = *l;
        NNSG3dAnmObj* x = (*l)->next;

        while(x)
        {
            if (x->priority >= item->priority)
            {
                // Insert into existing list
                NNSG3dAnmObj* pp = item;
                while(pp->next)
                {
                    pp = pp->next;
                }
                p->next = item;
                pp->next = x;
                return;
            }
            p = x;
            x = x->next;
        }
        // Connect to the end of existing list
        p->next = item;
    }
}


/*---------------------------------------------------------------------------*
    updateHintVec_

    Bit corresponding to resource ID registered in pAnmObj is set to ON.
 *---------------------------------------------------------------------------*/
static void
updateHintVec_(u32* pVec, const NNSG3dAnmObj* pAnmObj)
{
    const NNSG3dAnmObj* p = pAnmObj;
    while(p)
    {
        int i;
        for (i = 0; i < p->numMapData; ++i)
        {
            if (p->mapData[i] & NNS_G3D_ANMOBJ_MAPDATA_EXIST)
            {
                pVec[i >> 5] |= 1 << (i & 31);
            }
        }
        p = p->next;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjAddAnm

    Add pAnmObj to pRenderObj. Look at pAnmObj's header information and
    add to the pertinent list.
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjAddAnmObj(NNSG3dRenderObj* pRenderObj,
                          NNSG3dAnmObj* pAnmObj)
{
    const NNSG3dResAnmHeader* hdr;
    NNS_G3D_NULL_ASSERT(pRenderObj &&
                        pAnmObj    &&
                        pAnmObj->resAnm);

    hdr = (const NNSG3dResAnmHeader*)pAnmObj->resAnm;

    switch(hdr->category0)
    {
    case 'M': // material animation
        updateHintVec_(&pRenderObj->hintMatAnmExist[0], pAnmObj);
        addLink_(&pRenderObj->anmMat, pAnmObj);
        break;
    case 'J': // character(joint) animation
        updateHintVec_(&pRenderObj->hintJntAnmExist[0], pAnmObj);
        addLink_(&pRenderObj->anmJnt, pAnmObj);
        break;
    case 'V': // visibility animation
        updateHintVec_(&pRenderObj->hintVisAnmExist[0], pAnmObj);
        addLink_(&pRenderObj->anmVis, pAnmObj);
        break;
    default:
        NNS_G3D_ASSERT(0);
        break;
    }
}


/*---------------------------------------------------------------------------*
    removeLink_

    Remove "item" from the list. TRUE is returned if "item" was in the list.
    FALSE is returned if it was not.
 *---------------------------------------------------------------------------*/
static BOOL
removeLink_(NNSG3dAnmObj** l, NNSG3dAnmObj* item)
{
    NNSG3dAnmObj* p;
    NNSG3dAnmObj* pp;

    if (!*l)
    {
        // If not even one AnmObj exists:
        return FALSE;
    }
    
    if (*l == item)
    {
        // If first AnmObj has been removed:
        *l = (*l)->next;
        item->next = NULL;
        return TRUE;
    }

    // Search list
    p = (*l)->next;
    pp = (*l);
    while(p)
    {
        if (p == item)
        {
            pp->next = p->next;
            p->next = NULL;

            return TRUE;
        }
        pp = p;
        p = p->next;
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjRemoveAnmObj

    Remove pAnmObj from pRenderObj.
    Hint-use bit vector is not changed.
    Update is performed when NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE has been checked inside NNS_G3dDraw.
     
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjRemoveAnmObj(NNSG3dRenderObj* pRenderObj,
                             NNSG3dAnmObj* pAnmObj)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(pAnmObj);

    if (removeLink_(&pRenderObj->anmMat, pAnmObj) ||
        removeLink_(&pRenderObj->anmJnt, pAnmObj) ||
        removeLink_(&pRenderObj->anmVis, pAnmObj))
    {
        NNS_G3dRenderObjSetFlag(pRenderObj, NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE);
        return;
    }
    
    //
    // NOTICE:
    // If nothing has been removed, it is probably best to output a warning.
    //
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjSetCallBack

    Registers the callback process to be done while drawing.
    When the cmd command is executed, callback is called at the point
    that is specified by "timing" (one of three types can be selected).
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjSetCallBack(NNSG3dRenderObj* pRenderObj,
                            NNSG3dSbcCallBackFunc func,
                            u8*,          // NOTICE: Eliminated the address specification of callbacks
                            u8 cmd,
                            NNSG3dSbcCallBackTiming timing)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);

    pRenderObj->cbFunc = func;
    pRenderObj->cbCmd = cmd;
    pRenderObj->cbTiming = (u8)timing;
}



/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjResetCallBack

    Deletes the callback process done while drawing.
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjResetCallBack(NNSG3dRenderObj* pRenderObj)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);

    pRenderObj->cbFunc = NULL;
    pRenderObj->cbCmd = 0;
    pRenderObj->cbTiming = 0;
}


/*---------------------------------------------------------------------------*
    NNS_G3dRenderObjSetInitFunc

    Sets the callback to be called immediately before drawing.
 *---------------------------------------------------------------------------*/
void
NNS_G3dRenderObjSetInitFunc(NNSG3dRenderObj* pRenderObj,
                            NNSG3dSbcCallBackFunc func)
{
    NNS_G3D_NULL_ASSERT(pRenderObj);
    pRenderObj->cbInitFunc = func;
}


////////////////////////////////////////////////////////////////////////////////
//
// Model code <-> Texture code
//
//

//
// Texture / 4x4Texture / Pltt lifecycle
//
// 1 Get size with GetRequiredSize
// 2 Acquire texKey or plttKey (VRAM Manager code is not called)
// 3 Set the key with SetTexKey or SetPlttKey
// 4 Load texture/palette data into VRAM
// 5 Bind texture/palette data to model resource you want to draw
// 6 Draw using the model resource
//(7 Unbind if model resource will use different texture set)
// 8 Release if it will no longer be used from any model (VRAM Manager code is not called)
//
// No special order for 4,5
//

/*---------------------------------------------------------------------------*
    NNS_G3dTexGetRequiredSize

    Returns the size of the texture (other than 4x4 compressed texture)  held by the Texture block
 *---------------------------------------------------------------------------*/
u32
NNS_G3dTexGetRequiredSize(const NNSG3dResTex* pTex)
{
    NNS_G3D_NULL_ASSERT(pTex);

    return (u32)(pTex->texInfo.sizeTex << 3);
}


/*---------------------------------------------------------------------------*
    NNS_G3dTex4x4GetRequiredSize

    Returns the size of the 4x4 compressed texture held by the Texture block
 *---------------------------------------------------------------------------*/
u32
NNS_G3dTex4x4GetRequiredSize(const NNSG3dResTex* pTex)
{
    NNS_G3D_NULL_ASSERT(pTex);

    return (u32)(pTex->tex4x4Info.sizeTex << 3);
}


/*---------------------------------------------------------------------------*
    NNS_G3dTexSetTexKey

    Assigns texture key to the texture inside the Texture block
 *---------------------------------------------------------------------------*/
void
NNS_G3dTexSetTexKey(NNSG3dResTex* pTex,
                    NNSG3dTexKey texKey,
                    NNSG3dTexKey tex4x4Key)
{
    if (texKey > 0)
    {
        NNS_G3D_NULL_ASSERT(pTex);
        NNS_G3D_ASSERTMSG(!(pTex->texInfo.flag & NNS_G3D_RESTEX_LOADED),
                        "NNS_G3dTexSetTexKey: Tex already loaded.");
        NNS_G3D_ASSERTMSG(!(pTex->texInfo.sizeTex > (texKey & 0x7fff0000) >> 15),
                        "NNS_G3dTexSetTexKey: texKey size not enough, "
                        "0x%05x bytes required.", pTex->texInfo.sizeTex << 3);
        pTex->texInfo.vramKey = texKey;
    }

    if (tex4x4Key > 0)
    {
        NNS_G3D_ASSERTMSG(!(pTex->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED),
                        "NNS_G3dTexSetTexKey(4x4): Tex already loaded.");
        NNS_G3D_ASSERTMSG(tex4x4Key & 0x80000000,
                        "NNS_G3dTexSetTexKey(4x4): texKey is not for 4x4comp textures");
        NNS_G3D_ASSERTMSG(!(pTex->tex4x4Info.sizeTex > (tex4x4Key & 0x7fff0000) >> 15),
                        "NNS_G3dTexSetTexKey(4x4): texKey size not enough, "
                        "0x%05x bytes required.", pTex->tex4x4Info.sizeTex << 3);
        pTex->tex4x4Info.vramKey = tex4x4Key;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dTexLoad

    Loads the texture inside pTex to the location indicated by the texture key.
    Specifying TRUE for exec_begin_end
    will switch the VRAM bank internally. If FALSE is specified, the user must switch the VRAM bank both before and after calling this function.
     
 *---------------------------------------------------------------------------*/
void
NNS_G3dTexLoad(NNSG3dResTex* pTex, BOOL exec_begin_end)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_WARNING(!(pTex->texInfo.flag & NNS_G3D_RESTEX_LOADED),
                    "NNS_G3dTexLoad: texture already loaded");
    NNS_G3D_WARNING(!(pTex->tex4x4Info.flag & NNS_G3D_RESTEX4x4_LOADED),
                    "NNS_G3dTex4x4Load: texture already loaded");

    if (exec_begin_end)
    {
        GX_BeginLoadTex();
    }

    {
        u32 sz;
        void* pData;
        u32 from;

        sz = (u32)pTex->texInfo.sizeTex << 3;
        if (sz > 0) // If texture data does not exist, sz == 0.
        {
            NNS_G3D_ASSERT(pTex->texInfo.vramKey != 0);
            NNS_G3D_ASSERTMSG(pTex->texInfo.sizeTex <= (pTex->texInfo.vramKey & 0x7fff0000) >> 15,
                              "NNS_G3dTexLoad: texKey size not enough, "
                              "0x%05x bytes required.", pTex->texInfo.sizeTex << 3);
            
            pData = (u8*)pTex + pTex->texInfo.ofsTex;
            from = (pTex->texInfo.vramKey & 0xffff) << 3;

            GX_LoadTex(pData, from, sz);
    
            pTex->texInfo.flag |= NNS_G3D_RESTEX_LOADED;
        }
    }

    {
        u32 sz;
        void* pData;
        void* pDataPlttIdx;
        u32 from;

        sz = (u32)pTex->tex4x4Info.sizeTex << 3;

        if (sz > 0) // If texture data does not exist, sz == 0.
        {
            NNS_G3D_ASSERT(pTex->tex4x4Info.sizeTex != 0);
            NNS_G3D_ASSERTMSG(pTex->tex4x4Info.vramKey & 0x80000000,
                              "NNS_G3dTexLoad(4x4): texKey is not for 4x4comp textures");
            NNS_G3D_ASSERTMSG(pTex->tex4x4Info.sizeTex <= (pTex->tex4x4Info.vramKey & 0x7fff0000) >> 15,
                              "NNS_G3dTexLoad(4x4): texKey size not enough, "
                              "0x%05x bytes required.", pTex->tex4x4Info.sizeTex << 3);

            pData = (u8*)pTex + pTex->tex4x4Info.ofsTex;
            pDataPlttIdx = (u8*)pTex + pTex->tex4x4Info.ofsTexPlttIdx;
            from = (pTex->tex4x4Info.vramKey & 0xffff) << 3;

            GX_LoadTex(pData, from, sz);
            GX_LoadTex(pDataPlttIdx, GX_COMP4x4_PLTT_IDX(from), sz >> 1);
    
            pTex->tex4x4Info.flag |= NNS_G3D_RESTEX4x4_LOADED;
        }
    }

    if (exec_begin_end)
    {
        GX_EndLoadTex();
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dTexReleaseTexKey

    Releases the texture-key assignment inside the Texture block.
    The key is passed as the return value to the user.
    The VRAM's texture region itself does not get released.
    The user must release this using the key.
 *---------------------------------------------------------------------------*/
void
NNS_G3dTexReleaseTexKey(NNSG3dResTex* pTex,
                        NNSG3dTexKey* texKey,
                        NNSG3dTexKey* tex4x4Key)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_NULL_ASSERT(texKey && tex4x4Key);

    pTex->texInfo.flag &= ~NNS_G3D_RESTEX_LOADED;
    pTex->tex4x4Info.flag &= ~NNS_G3D_RESTEX4x4_LOADED;

    *texKey = pTex->texInfo.vramKey;
    pTex->texInfo.vramKey = 0;

    *tex4x4Key = pTex->tex4x4Info.vramKey;
    pTex->tex4x4Info.vramKey = 0;
}


/*---------------------------------------------------------------------------*
    NNS_G3dPlttGetRequiredSize

    Returns the size of the palette held by the Texture block.
 *---------------------------------------------------------------------------*/
u32
NNS_G3dPlttGetRequiredSize(const NNSG3dResTex* pTex)
{
    NNS_G3D_NULL_ASSERT(pTex);

    return (u32)(pTex->plttInfo.sizePltt << 3);
}


/*---------------------------------------------------------------------------*
    NNS_G3dPlttSetPlttKey

    Assigns palette key to palette inside the Texture block.
 *---------------------------------------------------------------------------*/
void
NNS_G3dPlttSetPlttKey(NNSG3dResTex* pTex, NNSG3dPlttKey plttKey)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_ASSERTMSG(!(pTex->plttInfo.flag & NNS_G3D_RESPLTT_LOADED),
                      "NNS_G3dPlttSetPlttKey: Palette already loaded.");
    NNS_G3D_ASSERTMSG(!(pTex->plttInfo.sizePltt > (plttKey >> 16)),
                      "NNS_G3dPlttSetPlttKey: plttKey size not enough, "
                      "0x%05x bytes required.", pTex->plttInfo.sizePltt << 3);

    pTex->plttInfo.vramKey = plttKey;
}


/*---------------------------------------------------------------------------*
    NNS_G3dPlttLoad

    Loads the palette inside pTex to the location indicated by the palette key.
    Specifying TRUE for exec_begin_end
    will switch the VRAM bank internally. If FALSE is specified, the user must switch the VRAM bank both before and after calling this function.
     
 *---------------------------------------------------------------------------*/
void
NNS_G3dPlttLoad(NNSG3dResTex* pTex, BOOL exec_begin_end)
{
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_WARNING(!(pTex->plttInfo.flag & NNS_G3D_RESPLTT_LOADED),
                    "NNS_G3dPlttLoad: texture already loaded");

    if (exec_begin_end)
    {
        GX_BeginLoadTexPltt();
    }

    {
        u32 sz;
        void* pData;
        u32 from;

        sz = (u32)pTex->plttInfo.sizePltt << 3;
        pData = (u8*)pTex + pTex->plttInfo.ofsPlttData;
        from = (pTex->plttInfo.vramKey & 0xffff) << 3;

        GX_LoadTexPltt(pData, from, sz);
    
        pTex->plttInfo.flag |= NNS_G3D_RESPLTT_LOADED;
    }

    if (exec_begin_end)
    {
        GX_EndLoadTexPltt();
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dPlttReleasePlttKey

    Releases the palette-key assignment inside the Texture block.
    The key is passed as the return value to the user.
    The VRAM's texture region itself does not get released.
    The user must release this using the key.
 *---------------------------------------------------------------------------*/
NNSG3dPlttKey
NNS_G3dPlttReleasePlttKey(NNSG3dResTex* pTex)
{
    u32 rval;
    NNS_G3D_NULL_ASSERT(pTex);

    pTex->plttInfo.flag &= ~NNS_G3D_RESPLTT_LOADED;
    
    rval = pTex->plttInfo.vramKey;
    pTex->plttInfo.vramKey = 0;
    return rval;
}


static void
bindMdlTex_Internal_(NNSG3dResMat* pMat,
                     NNSG3dResDictTexToMatIdxData* pBindData,
                     const NNSG3dResTex* pTex,
                     const NNSG3dResDictTexData* pTexData)
{
    u8* base = (u8*)pMat + pBindData->offset;
    u32 vramOffset;
    u32 j;

    NNS_G3D_ASSERTMSG((pTex->texInfo.vramKey != 0 || pTex->texInfo.sizeTex == 0),
                      "No texture key assigned");
    NNS_G3D_ASSERTMSG((pTex->tex4x4Info.vramKey != 0 || pTex->tex4x4Info.sizeTex == 0),
                      "No texture(4x4) key assigned");

    if ((pTexData->texImageParam & 0x1c000000) != 0x14000000)
    {
        // Some texture other than 4x4
        vramOffset = pTex->texInfo.vramKey & 0xffff;
    }
    else
    {
        // A 4x4 texture
        vramOffset = pTex->tex4x4Info.vramKey & 0xffff;
    }

    for (j = 0; j < pBindData->numIdx; ++j)
    {
        s32 w, h;

        // Perform setup of texture information in each matData.
        NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(pMat, *(base + j));

        matData->texImageParam |= (pTexData->texImageParam + vramOffset);
        w = (s32)((pTexData->extraParam) & 0x7ff);
        h = (s32)((pTexData->extraParam >> 11) & 0x7ff);

        matData->magW = (w != matData->origWidth) ?
                        FX_Div(w << FX32_SHIFT, matData->origWidth << FX32_SHIFT) :
                        FX32_ONE;
        matData->magH = (h != matData->origHeight) ?
                        FX_Div(h << FX32_SHIFT, matData->origHeight << FX32_SHIFT) :
                        FX32_ONE;
    }

    // Set state to bind state.
    pBindData->flag |= 1;
}

static void
releaseMdlTex_Internal_(NNSG3dResMat* pMat,
                        NNSG3dResDictTexToMatIdxData* pData)
{
    // If state is bind state, release
    u8* base = (u8*)pMat + pData->offset;
    u32 j;

    for (j = 0; j < pData->numIdx; ++j)
    {
        // Perform setup of texture information in each matData.
        NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(pMat, *(base + j));

        // Leave flip & repeat & texgen and reset
        matData->texImageParam &= ~0x3ff0ffff;
        matData->magH = matData->magW = FX32_ONE;
    }

    // Set to the unbind state
    pData->flag &= ~1;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlTex

    The texture entry of the material associated with the texture name in the model
    is associated with the texture data having the same texture name inside the texture block.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlTex(NNSG3dResMdl* pMdl, const NNSG3dResTex* pTex)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictTex;
    u32 i;
    BOOL result = TRUE;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);

    mat     = NNS_G3dGetMat(pMdl);
    dictTex = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);
    
    //  
    // Loop for each in the model resource's texture name -> material index list dictionary  //
    for (i = 0; i < dictTex->numEntry; ++i)
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(dictTex, i);

        // Search inside Texture block for entries with same name
        const NNSG3dResDictTexData* texData = NNS_G3dGetTexDataByName(pTex, name);

        if (texData)
        {
            // If there, get the bind data relating to the nth texture, where n = i.
            NNSG3dResDictTexToMatIdxData* data =
                (NNSG3dResDictTexToMatIdxData*) NNS_G3dGetResDataByIdx(dictTex, i);
        
            if (!(data->flag & 1))
            {
                // Bind if not in the bind state
                bindMdlTex_Internal_(mat, data, pTex, texData);
            }
        }
        else
        {
            result = FALSE;
        }
    }
    return result;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlTexEx

    Inside the model, take the palette entry of the material associated
    with the palette named pResName, and associate it with the palette
    named pResName inside the Texture block.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlTexEx(NNSG3dResMdl* pMdl,
                    const NNSG3dResTex* pTex,
                    const NNSG3dResName* pResName)
{
    NNSG3dResMat* mat;
    NNSG3dResDict* dictTex;
    const NNSG3dResDictTexData* texData;
    NNSG3dResDictTexToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_NULL_ASSERT(pResName);

    mat     = NNS_G3dGetMat(pMdl);
    dictTex = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);

    // Search the dictionary inside the Texture block
    texData = NNS_G3dGetTexDataByName(pTex, pResName);

    if (texData)
    {
        // Search inside the model's texture -> material index list dictionary
        data = (NNSG3dResDictTexToMatIdxData*)
                    NNS_G3dGetResDataByName(dictTex, pResName);

        if (data && !(data->flag & 1))
        {
            // If there is data corresponding to pResName in both model and texture
            // Bind if not in the bind state
            bindMdlTex_Internal_(mat, data, pTex, texData);
            return TRUE;
        }
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dForceBindMdlTex

    Texture name used within model -> Forcibly binds the texture entry for the material stored as the textToMatListIdx-th entry in the material list dictionary and the textIdx-th texture in the texture block.
      
      
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dForceBindMdlTex(NNSG3dResMdl* pMdl,
                       const NNSG3dResTex* pTex,
                       u32 texToMatListIdx,
                       u32 texIdx)
{
    NNSG3dResMat* mat;
    NNSG3dResDict* dictTex;
    const NNSG3dResDictTexData* texData;
    NNSG3dResDictTexToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);

    mat     = NNS_G3dGetMat(pMdl);
    dictTex = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);

    // Uses index lookup to get the texture data in the texture block
    texData = NNS_G3dGetTexDataByIdx(pTex, texIdx);

    // Search inside the model's texture -> material index list dictionary
    data = (NNSG3dResDictTexToMatIdxData*)
                NNS_G3dGetResDataByIdx(dictTex, texToMatListIdx);

    if (data)
    {
        // Forcibly binds the textures if the texture having the name pResName exists for the material.
        bindMdlTex_Internal_(mat, data, pTex, texData);
        return TRUE;
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlTex

    Releases the texture-entry association of the material associated with the texture in the model.
     
 *---------------------------------------------------------------------------*/
void
NNS_G3dReleaseMdlTex(NNSG3dResMdl* pMdl)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictTex;
    u32 i;

    NNS_G3D_NULL_ASSERT(pMdl);

    mat      = NNS_G3dGetMat(pMdl);
    dictTex  = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);
    
    //  
    // Loop for each in the model resource's texture name -> material index list dictionary  //
    for (i = 0; i < dictTex->numEntry; ++i)
    {
        NNSG3dResDictTexToMatIdxData* data =
            (NNSG3dResDictTexToMatIdxData*) NNS_G3dGetResDataByIdx(dictTex, i);

        if (data->flag & 1)
        {
            // Release if in the binded state
            releaseMdlTex_Internal_(mat, data);
        }
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlTexEx

    Releases the association of the texture entry of the material associated with the texture havin gth ename "pResName" in the model.
     
 *---------------------------------------------------------------------------*/
BOOL NNS_G3dReleaseMdlTexEx(NNSG3dResMdl* pMdl, const NNSG3dResName* pResName)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictTex;
    NNSG3dResDictTexToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pResName);

    mat      = NNS_G3dGetMat(pMdl);
    dictTex  = (NNSG3dResDict*)((u8*)mat + mat->ofsDictTexToMatList);
    data     = (NNSG3dResDictTexToMatIdxData*)NNS_G3dGetResDataByName(dictTex, pResName);

    if (data && (data->flag & 1))
    {
        // Release if in the binded state
        releaseMdlTex_Internal_(mat, data);
        return TRUE;
    }
    return FALSE;
}


static void
bindMdlPltt_Internal_(NNSG3dResMat* pMat,
                      NNSG3dResDictPlttToMatIdxData* pBindData,
                      const NNSG3dResTex* pTex,
                      const NNSG3dResDictPlttData* pPlttData)
{
    // Bind if not in the bind state
    u8* base = (u8*)pMat + pBindData->offset;
    u16 plttBase = pPlttData->offset;
    u16 vramOffset = (u16)(pTex->plttInfo.vramKey & 0xffff);
    u32 j;

    NNS_G3D_ASSERTMSG((pTex->plttInfo.vramKey != 0 || pTex->plttInfo.sizePltt == 0),
                       "No palette key assigned");
    
    // If 4colors, this bit is set
    if (!(pPlttData->flag & 1))
    {
        // Four-bit shift if other than 4colors.
        // If 4colors, shift is 3-bit shift so left as-is
        plttBase >>= 1;
        vramOffset >>= 1;
    }

    for (j = 0; j < pBindData->numIdx; ++j)
    {
        // Perform setup of palette information for each matData.
        NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(pMat, *(base + j));
        matData->texPlttBase = (u16)(plttBase + vramOffset);
    }

    // Set to bind state
    pBindData->flag |= 1;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlPltt

    Associates the palette-entry of the material associated with the palette name in the model
    with the palette data having the same palette name inside the texture block.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlPltt(NNSG3dResMdl* pMdl, const NNSG3dResTex* pTex)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    u32 i;
    BOOL result = TRUE;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);

    for (i = 0; i < dictPltt->numEntry; ++i)
    {
        // Palette name inside the model -> Remove the palette name from the material index array dictionary
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(dictPltt, i);
        
        // Get the data field corresponding to the palette name from the Texture block
        const NNSG3dResDictPlttData* plttData = NNS_G3dGetPlttDataByName(pTex, name);

        if (plttData)
        {
            // Palette name exists in Texture block
            NNSG3dResDictPlttToMatIdxData* data;
            data = (NNSG3dResDictPlttToMatIdxData*) NNS_G3dGetResDataByIdx(dictPltt, i);

            if (!(data->flag & 1))
            {
                bindMdlPltt_Internal_(mat, data, pTex, plttData);
            }
        }
        else
        {
            result = FALSE;
        }
    }
    return result;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlPlttEx

    Inside the model, take the palette entry of the material associated
    with the palette named pResName, and associate it with the palette
    named pResName inside the Texture block.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlPlttEx(NNSG3dResMdl* pMdl,
                     const NNSG3dResTex* pTex,
                     const NNSG3dResName* pResName)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    const NNSG3dResDictPlttData* plttData;
    NNSG3dResDictPlttToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);
    NNS_G3D_NULL_ASSERT(pResName);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);

    // Get the data field corresponding to the palette name from the Texture block
    plttData = NNS_G3dGetPlttDataByName(pTex, pResName);

    if (plttData)
    {
        // Palette name exists in Texture block
        data = (NNSG3dResDictPlttToMatIdxData*)
                    NNS_G3dGetResDataByName(dictPltt, pResName);
        
        if (data && !(data->flag & 1))
        {
            bindMdlPltt_Internal_(mat, data, pTex, plttData);
            return TRUE;
        }
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dForceBindMdlPltt

    Palette name used within model -> Forcibly binds the palette for the material stored as the pltttoMatListIdx-th entry in the material list dictionary and the plttIdx-th palette in the texture block.
      
      
 *---------------------------------------------------------------------------*/
BOOL NNS_G3dForceBindMdlPltt(NNSG3dResMdl* pMdl,
                             const NNSG3dResTex* pTex,
                             u32 plttToMatListIdx,
                             u32 plttIdx)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    const NNSG3dResDictPlttData* plttData;
    NNSG3dResDictPlttToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pTex);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);
    plttData = NNS_G3dGetPlttDataByIdx(pTex, plttIdx);

    data = (NNSG3dResDictPlttToMatIdxData*)
                NNS_G3dGetResDataByIdx(dictPltt, plttToMatListIdx);
        
    if (data && !(data->flag & 1))
    {
        bindMdlPltt_Internal_(mat, data, pTex, plttData);
        return TRUE;
    }
    return FALSE;
}



/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlPltt

    Release the association of the palette-entry of the material associated with the palette in the model.
     
 *---------------------------------------------------------------------------*/
void
NNS_G3dReleaseMdlPltt(NNSG3dResMdl* pMdl)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    u32 i;

    NNS_G3D_NULL_ASSERT(pMdl);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);
    for (i = 0; i < dictPltt->numEntry; ++i)
    {
        NNSG3dResDictPlttToMatIdxData* data =
            (NNSG3dResDictPlttToMatIdxData*) NNS_G3dGetResDataByIdx(dictPltt, i);
         
        if (data->flag & 1)
        {
            // If state is bind state, release
#if 0
            u32 j;
            // Do not execute, since OK with just flag operations
            u8* base = (u8*)mat + data->offset;

            for (j = 0; j < data->numIdx; ++j)
            {
                // Reset each matData
                NNSG3dResMatData* matData = NNS_G3dGetMatDataByIdx(mat, *(base + j));
                matData->texPlttBase = 0;
            }
#endif
            // Unbind
            data->flag &= ~1;
        }
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlPlttEx

    Release the association of the palette-entry of the material assciated with the palette having the name "pResName" in the model.
     
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dReleaseMdlPlttEx(NNSG3dResMdl* pMdl, const NNSG3dResName* pResName)
{
    NNSG3dResMat*  mat;
    NNSG3dResDict* dictPltt;
    NNSG3dResDictPlttToMatIdxData* data;

    NNS_G3D_NULL_ASSERT(pMdl);
    NNS_G3D_NULL_ASSERT(pResName);

    mat      = NNS_G3dGetMat(pMdl);
    dictPltt = (NNSG3dResDict*)((u8*)mat + mat->ofsDictPlttToMatList);
    data = (NNSG3dResDictPlttToMatIdxData*)
                NNS_G3dGetResDataByName(dictPltt, pResName);

    if (data && (data->flag & 1))
    {
        // If state is bind state, release
        data->flag &= ~1;
        return TRUE;
    }
    return FALSE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dBindMdlSet

    Associate each model with texture/palette.
    Texture must have TexKey, PlttKey set.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dBindMdlSet(NNSG3dResMdlSet* pMdlSet, const NNSG3dResTex* pTex)
{
    u32 i;
    BOOL result = TRUE;

    NNS_G3D_NULL_ASSERT(pMdlSet);
    NNS_G3D_NULL_ASSERT(pTex);

    for (i = 0; i < pMdlSet->dict.numEntry; ++i)
    {
        NNSG3dResMdl* mdl = NNS_G3dGetMdlByIdx(pMdlSet, i);
        NNS_G3D_NULL_ASSERT(mdl);

        result &= NNS_G3dBindMdlTex(mdl, pTex);
        result &= NNS_G3dBindMdlPltt(mdl, pTex);
    }
    return result;
}


/*---------------------------------------------------------------------------*
    NNS_G3dReleaseMdlSet

    Release each model from the texture/palette association.
 *---------------------------------------------------------------------------*/
void
NNS_G3dReleaseMdlSet(NNSG3dResMdlSet* pMdlSet)
{
    u32 i;
    NNS_G3D_NULL_ASSERT(pMdlSet);

    for (i = 0; i < pMdlSet->dict.numEntry; ++i)
    {
        NNSG3dResMdl* mdl = NNS_G3dGetMdlByIdx(pMdlSet, i);
        NNS_G3D_NULL_ASSERT(mdl);

        NNS_G3dReleaseMdlTex(mdl);
        NNS_G3dReleaseMdlPltt(mdl);
    }
}

