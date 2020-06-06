/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     util.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util.c,v $
  Revision 1.17  02/25/2005 05:38:42  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.16  01/27/2005 07:33:08  nishida
  Corrected the year of the copyright date.

  Revision 1.15  01/06/2005 02:44:17  nishida_kenji
  update NNS_G3dSbcCmdLen.

  Revision 1.14  11/04/2004 00:07:00  nishida_kenji
  Add NNS_G3dLocalOriginToScrPos, NNS_G3dWorldPosToScrPos,
  and NNS_G3dScrPosToWorldLine.

  Revision 1.13  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.12  10/06/2004 04:44:19  nishida_kenji
  workaround to NNSi_G3dFuncSbc_NODEMIX.

  Revision 1.11  09/16/2004 01:15:47  nishida_kenji
  fix NNS_G3dGetResultMtx(works well even if  pos == NULL && nrm == NULL).

  Revision 1.10  09/03/2004 12:11:46  nishida_kenji
  add NNS_G3dGetCurrentMtx

  Revision 1.9  08/26/2004 06:41:53  nishida_kenji
  bugfix(NNS_G3dGetChildNodeIDList)

  Revision 1.8  08/03/2004 08:43:15  nishida_kenji
  NNS_G3dGetAnmByIdx, NNS_G3dGetAnmByName move to res_struct_accessor_anm.c

  Revision 1.7  08/03/2004 01:45:24  nishida_kenji
  Add NNS_G3dGetAnmByIdx, NNS_G3dGetAnmByName, NNS_G3D_GET_ANM.

  Revision 1.6  07/19/2004 10:55:51  nishida_kenji
  small bug fix

  Revision 1.5  07/17/2004 12:54:13  nishida_kenji
  Add NNS_G3dResDefaultSetup, NNS_G3dResDefaultDestroy
  NNS_G3dGetSbcCmdLen, NNS_G3dSearchSbcCmd
  NNS_G3dGetParentNodeID, NNS_G3dGetChildNodeIDList

  Revision 1.4  07/16/2004 07:24:58  nishida_kenji
  add NNS_G3dInit.

  Revision 1.3  07/13/2004 07:15:26  nishida_kenji
  Add comments.

  Revision 1.2  07/13/2004 05:30:55  nishida_kenji
  NNS_G3dGetResultMtx, NNS_G3dSetResultMtx

  Revision 1.1  07/10/2004 08:47:38  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/util.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/glbstate.h>

#include <nnsys/gfd.h>


/*---------------------------------------------------------------------------*
    NNS_G3dGetCurrentMtx

    Obtains the current position coordinate matrix and the current directional vector matrix.
    When the argument is NULL, obtainment of that matrix is omitted.
    Destroys the matrix stack of the projection matrix when executed.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGetCurrentMtx(MtxFx43* m, MtxFx33* n)
{
    NNS_G3dGeFlushBuffer();

    // Use projection matrix as identity
    // and fetch clipping matrix and vector matrix.
    // Then you can fetch current position coordinate matrix and current direction vector matrix.
    G3_MtxMode(GX_MTXMODE_PROJECTION);
    G3_PushMtx();
    G3_Identity();

    if (m)
    {
        MtxFx44 tmp;
        while(G3X_GetClipMtx(&tmp))
            ;
        MTX_Copy44To43(&tmp, m);
    }

    if (n)
    {
        while(G3X_GetVectorMtx(n))
            ;
    }

    // Restore original projection matrix, switch to Position/Vector matrix mode.
    G3_PopMtx(1);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGetResultMtx

    If a matrix corresponding to nodeID remains in the matrix stack after the
    execution of NNS_G3dDraw, that matrix gets fetched from the matrix stack.
    TRUE is returned if a matrix has been fetched. Otherwise, FALSE is returned.
    If TRUE is returned, the matrix mode is changed to Position/Vector.
    pos, nrm are set if not NULL.
    Be careful that the obtained matrix is not multiplied by pos_scale.
    (Vertex coordinates are scaled with the attribute specified by <model_info> in the .imd Intermediate file.
      Stored as type fx32 in NNSG3dResMdlInfo::posScale.))

    Procedure and Precautions:
    The joint matrices are all stored in the matrix stack when g3cvtr is set
    with the -s option for the conversion process.
    Rendering can be skipped if the NNSG3dRenderObj::flag NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW is set
    and NNS_G3dDraw is called;
    it is possible to skip matrix calculations by setting NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC and calling NNS_G3dDraw.
     

    These features can be used for the following type of programming:
    - First set a matrix on the matrix stack.
    - Process the matrix that has been set in the matrix stack
    - Render using the processed matrix
     
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dGetResultMtx(const NNSG3dRenderObj* pRenderObj,
                    MtxFx43* pos,
                    MtxFx33* nrm,
                    u32 nodeID)
{
    const NNSG3dResNodeData* nd;
    u32 stackID;

    // With pos and nrm, NULL is OK.
    NNS_G3D_NULL_ASSERT(pRenderObj);

    nd = NNS_G3dGetNodeDataByIdx(&pRenderObj->resMdl->nodeInfo, nodeID);
    stackID = (nd->flag & (u32)NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK)
                                >> NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT;

    if (stackID != 31)
    {
        NNS_G3dGeRestoreMtx((int)stackID);
        if (pos || nrm)
        {
            NNS_G3dGetCurrentMtx(pos, nrm);
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dSetResultMtx

    If settings are made so that a matrix corresponding to nodeID remains in the matrix stack after the execution of NNS_G3dDraw,
    the matrices *pos and *nrm will be assigned in the corresponding location in the stack.
    Return TRUE if assigned could be made; return FALSE if it could not.
    If TRUE is returned, the matrix mode is changed to Position/Vector.
    It does not matter of nrm is NULL, but pos must not be NULL.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dSetResultMtx(const NNSG3dRenderObj* pRenderObj,
                    const MtxFx43* pos,
                    const MtxFx33* nrm,
                    u32 nodeID)
{
    const NNSG3dResNodeData* nd;
    u32 stackID;

    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(pos); // nrm can be NULL

    nd = NNS_G3dGetNodeDataByIdx(&pRenderObj->resMdl->nodeInfo, nodeID);
    stackID = (nd->flag & (u32)NNS_G3D_SRTFLAG_IDXMTXSTACK_MASK)
                                >> NNS_G3D_SRTFLAG_IDXMTXSTACK_SHIFT;

    if (stackID != 31)
    {
        // Flush the buffer
        NNS_G3dGeFlushBuffer();

        // Data can be sent immediately from here on
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        if (nrm)
        {
            // Only 4x3 matrices can be loaded
            reg_G3X_GXFIFO = G3OP_MTX_LOAD_4x3;
            MI_CpuSend32(&nrm->_00, &reg_G3X_GXFIFO, sizeof(*nrm));
            reg_G3X_GXFIFO = 0;
            reg_G3X_GXFIFO = 0;
            reg_G3X_GXFIFO = 0;

            G3_MtxMode(GX_MTXMODE_POSITION);
            G3_LoadMtx43(pos);
            G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
        }
        else
        {
            G3_LoadMtx43(pos);
        }

        G3_StoreMtx((int)stackID);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dInit

    Performs the default initialization.
 *---------------------------------------------------------------------------*/
void
NNS_G3dInit(void)
{
    G3X_Init();

    //
    // Initialize the global states (camera matrix & projection matrix, etc.) stored in the G3D
    //
    NNS_G3dGlbInit();

    //
    // OK if not GX_FIFOINTR_COND_DISABLE, but GX_FIFOINTR_COND_EMPTY is better
    // than GX_FIFOINTR_COND_UNDERHALF
    //
    G3X_SetFifoIntrCond(GX_FIFOINTR_COND_EMPTY);
}


/*---------------------------------------------------------------------------*
    NNS_G3dSbcCmdLen

    The table showing the SBC command lengths (in bytes).
    If -1, the corresponding SBC command doesn't exist. If 0, the command has variable length.
 *---------------------------------------------------------------------------*/
const s8 NNS_G3dSbcCmdLen[256] = 
{
    // -1 means the command does not exit
    // > means undefined length
     1,  1,  3,  2,  2,  2,  4,  2,  2,  0,  9,  1,  3,  3, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1,  2, -1,  5,  3,  3,  0, -1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1,  2, -1,  5,  3,  3, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1,  6,  4,  4, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};


/*---------------------------------------------------------------------------*
    NNS_G3dGetSbcCmdLen

    Returns the length of SBC command *c.
    Returns -1 if*c is not an SBC command.
 *---------------------------------------------------------------------------*/
int
NNS_G3dGetSbcCmdLen(const u8* c)
{
    int cmdLen;
    NNS_G3D_NULL_ASSERT(c);

    cmdLen = NNS_G3dSbcCmdLen[*c];

    if (cmdLen < 0)
    {
        return -1;
    }
    else if (cmdLen == 0)
    {
        if (*c == NNS_G3D_SBC_NODEMIX)
        {
            return *(c + 2) * 3 + 3;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return cmdLen;
    }
}


/*---------------------------------------------------------------------------*
    NNS_G3dSearchSbcCmd

    Searches for the SBC command (cmd) inside the string of SBC code.
    If found, returns the pointer of the SBC command.
    If not found, returns NULL.
    The flag parts inside SBC commands are ignored.
 *---------------------------------------------------------------------------*/
const u8*
NNS_G3dSearchSbcCmd(const u8* c, u8 cmd)
{
    int x;
    NNS_G3D_NULL_ASSERT(c);
    NNS_G3D_ASSERTMSG(NNS_G3dSbcCmdLen[cmd] >= 0, "unknown command");

    cmd &= NNS_G3D_SBCCMD_MASK;
    while((x = (*c & NNS_G3D_SBCCMD_MASK)) != NNS_G3D_SBC_RET)
    {
        if (cmd == x)
        {
            return c;
        }
        else
        {
            int cmdLen = NNS_G3dGetSbcCmdLen(c);
            NNS_G3D_ASSERTMSG(cmdLen > 0, "unknown command");
            c += cmdLen;
        }
    }
    return NULL;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGetParentNodeID

    The ID of the node that is the parent of nodeID gets stored in *parentID.
    Returns the pointer to the NODEDESC command set by nodeID.
    If cannot be found, NULL is returned and *parentID is not changed.
 *---------------------------------------------------------------------------*/
const u8*
NNS_G3dGetParentNodeID(int* parentID, const u8* sbc, u32 nodeID)
{
    const u8* tmp = sbc;

    NNS_G3D_NULL_ASSERT(parentID);
    NNS_G3D_NULL_ASSERT(sbc);
    NNS_G3D_ASSERTMSG(nodeID < NNS_G3D_SIZE_JNT_MAX,
                      "specified nodeID is illegal");

    while((tmp = NNS_G3dSearchSbcCmd(tmp, NNS_G3D_SBC_NODEDESC)))
    {
        // Search for match for own node ID
        if (*(tmp + 1) == nodeID)
        {
            // Fetch parent
            // No support for illegal SBC, such as multiple parents for a child
            *parentID = *(tmp + 2);
            return tmp;
        }

        {
            int cmdLen = NNS_G3dGetSbcCmdLen(tmp);
            NNS_G3D_ASSERTMSG(cmdLen > 0, "unknown command");
            tmp += cmdLen;
        }
    }
    return NULL;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGetChildNodeIDList

    Creates a list of node IDs for children of nodeID in idList.
    idList must have sufficient space.
    Returns the number of child nodes.
    Returns 0 if none found and nothing gets written to idList.
 *---------------------------------------------------------------------------*/
int
NNS_G3dGetChildNodeIDList(u8* idList, const u8* sbc, u32 nodeID)
{
    const u8* tmp = sbc;
    int num = 0;

    NNS_G3D_NULL_ASSERT(idList);
    NNS_G3D_ASSERTMSG(nodeID < NNS_G3D_SIZE_JNT_MAX,
                      "specified nodeID is illegal");

    while((tmp = NNS_G3dSearchSbcCmd(tmp, NNS_G3D_SBC_NODEDESC)))
    {
        // Search for match for parent node ID
        if (*(tmp + 2) == nodeID && *(tmp + 2) != *(tmp + 1))
        {
            // Add
            *(idList + num++) = *(tmp + 1);
        }

        {
            int cmdLen = NNS_G3dGetSbcCmdLen(tmp);
            NNS_G3D_ASSERTMSG(cmdLen > 0, "unknown command");
            tmp += cmdLen;
        }
    }

    return num;
}


/*---------------------------------------------------------------------------*
    NNS_G3dResDefaultSetup

    Sets up the G3D resource *pData. The actions differ, depending on the
    data format as follows:

    For .nsbmd files:
        - Secures a region for textures and palettes
        - Loads textures and palettes into VRAM
        - Binds textures the models in the files

    For .nsbtx files:
        - Secures a region for textures and palettes
        - Loads textures and palettes into VRAM

    Other cases:
        Nothing gets done.
 *---------------------------------------------------------------------------*/
BOOL
NNS_G3dResDefaultSetup(void* pResData)
{
    u8* binFile = (u8*)pResData;
    BOOL failed = FALSE;

    NNS_G3D_NULL_ASSERT(pResData);

    switch(*(u32*)&binFile[0])
    {
    case NNS_G3D_SIGNATURE_NSBTX: // BTX0
    case NNS_G3D_SIGNATURE_NSBMD: // BMD0
        {
            NNSG3dResTex* tex;
            u32 szTex, szTex4x4, szPltt;
            BOOL sucTex    = TRUE;
            BOOL sucTex4x4 = TRUE;
            BOOL sucPltt   = TRUE;
            NNSG3dTexKey keyTex;
            NNSG3dTexKey keyTex4x4;
            NNSG3dPlttKey keyPltt;

            tex = NNS_G3dGetTex((NNSG3dResFileHeader*) pResData);
            if (tex)
            {
                // Get the necessary size
                szTex    = NNS_G3dTexGetRequiredSize(tex);
                szTex4x4 = NNS_G3dTex4x4GetRequiredSize(tex);
                szPltt   = NNS_G3dPlttGetRequiredSize(tex);

                if (szTex > 0)
                {
                    // If it exists, store in the texture image slot
                    keyTex = NNS_GfdAllocTexVram(szTex, FALSE, 0);
                    if (keyTex == NNS_GFD_ALLOC_ERROR_TEXKEY)
                    {
                        sucTex = FALSE;
                    }
                }
                else
                {
                    keyTex = 0;
                }

                if (szTex4x4 > 0)
                {
                    // If it exists, store in the texture image slot
                    keyTex4x4 = NNS_GfdAllocTexVram(szTex4x4, TRUE, 0);
                    if (keyTex4x4 == NNS_GFD_ALLOC_ERROR_TEXKEY)
                    {
                        sucTex4x4 = FALSE;
                    }
                }
                else
                {
                    keyTex4x4 = 0;
                }

                if (szPltt > 0)
                {
                    // If it exists, store in the texture palette slot
                    keyPltt = 
                        NNS_GfdAllocPlttVram(szPltt,
                                            tex->tex4x4Info.flag & NNS_G3D_RESPLTT_USEPLTT4,
                                            0);
                    if (keyPltt == NNS_GFD_ALLOC_ERROR_PLTTKEY)
                    {
                        sucPltt = FALSE;
                    }
                }
                else
                {
                    keyPltt = 0;
                }

                if (!sucTex || !sucTex4x4 || !sucPltt)
                {
                    // Rollback process when fails
                    int status;

                    status = NNS_GfdFreePlttVram(keyPltt);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreePlttVram failed");

                    status = NNS_GfdFreeTexVram(keyTex4x4);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreeTexVram failed");

                    status = NNS_GfdFreeTexVram(keyTex);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreeTexVram failed");

                    return FALSE;
                }

                // Key assignments
                NNS_G3dTexSetTexKey(tex, keyTex, keyTex4x4);
                NNS_G3dPlttSetPlttKey(tex, keyPltt);

                // Load to VRAM
                NNS_G3dTexLoad(tex, TRUE);
                NNS_G3dPlttLoad(tex, TRUE);
            }

            if (*(u32*)&binFile[0] == NNS_G3D_SIGNATURE_NSBMD)
            {
                // If NSBMD, all models are bound
                NNSG3dResMdlSet* mdlSet = NNS_G3dGetMdlSet((NNSG3dResFileHeader*) pResData);
                NNS_G3D_NULL_ASSERT(mdlSet);

                if (tex)
                {
                    // Bind model set
                    (void)NNS_G3dBindMdlSet(mdlSet, tex);
                }
            }
        }
        return TRUE;
        break;
    case NNS_G3D_SIGNATURE_NSBCA: // BCA0
    case NNS_G3D_SIGNATURE_NSBVA: // BVA0
    case NNS_G3D_SIGNATURE_NSBMA: // BMA0
    case NNS_G3D_SIGNATURE_NSBTP: // BTP0
    case NNS_G3D_SIGNATURE_NSBTA: // BTA0
        return TRUE;
        break;
    default:
        NNS_G3D_ASSERTMSG(1==0, "unknown file signature: '%c%c%c%c' found.\n",
                                binFile[0], binFile[1], binFile[2], binFile[3]);
        return FALSE;
        break;
    };
}


/*---------------------------------------------------------------------------*
    NNS_G3dResDefaultRelease

    Executes normal operations before releasing the region for the G3D resource *pData
    The actions differ, depending on the data format as follows:

    For .nsbmd files:
        - Releases region for textures and palettes

    For .nsbtx files:
        - Releases region for textures and palettes

    Other cases:
        Nothing gets done.

    NOTICE:
    The user must act to release the pResData memory region.
 *---------------------------------------------------------------------------*/
void
NNS_G3dResDefaultRelease(void* pResData)
{
    u8* binFile = (u8*)pResData;
    BOOL failed = FALSE;

    NNS_G3D_NULL_ASSERT(pResData);

    switch(*(u32*)&binFile[0])
    {
    case NNS_G3D_SIGNATURE_NSBMD: // BMD0
        {
            NNSG3dResTex* tex;
            NNSG3dResMdlSet* mdlSet = NNS_G3dGetMdlSet(pResData);
            NNS_G3D_NULL_ASSERT(mdlSet);
            tex = NNS_G3dGetTex((NNSG3dResFileHeader*) pResData);

            if (tex)
            {
                // Releases all models
                NNS_G3dReleaseMdlSet(mdlSet);
            }
        }
        // don't break
    case NNS_G3D_SIGNATURE_NSBTX: // BTX0
        {
            NNSG3dResTex* tex;
            NNSG3dPlttKey plttKey;
            NNSG3dTexKey  texKey, tex4x4Key;
            int status;
            tex = NNS_G3dGetTex((NNSG3dResFileHeader*) pResData);

            if (tex)
            {
                // Releases keys from Texture block
                plttKey   = NNS_G3dPlttReleasePlttKey(tex);
                NNS_G3dTexReleaseTexKey(tex, &texKey, &tex4x4Key);

                if (plttKey > 0)
                {
                    // Released if secured in texture palette slot
                    status = NNS_GfdFreePlttVram(plttKey);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreePlttVram failed");
                }

                if (tex4x4Key > 0)
                {
                    // Released if secured in texture image slot
                    status = NNS_GfdFreeTexVram(tex4x4Key);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreeTexVram failed");
                }

                if (texKey > 0)
                {
                    // Released if secured in texture image slot
                    status = NNS_GfdFreeTexVram(texKey);
                    NNS_G3D_ASSERTMSG(!status, "NNS_GfdFreeTexVram failed");
                }
            }
        }
        break;
    case NNS_G3D_SIGNATURE_NSBCA: // BCA0
    case NNS_G3D_SIGNATURE_NSBVA: // BVA0
    case NNS_G3D_SIGNATURE_NSBMA: // BMA0
    case NNS_G3D_SIGNATURE_NSBTP: // BTP0
    case NNS_G3D_SIGNATURE_NSBTA: // BTA0
        break;
    default:
        NNS_G3D_ASSERTMSG(1==0, "unknown file signature: '%c%c%c%c' found.\n",
                                binFile[0], binFile[1], binFile[2], binFile[3]);
        break;
    };
}


/*---------------------------------------------------------------------------*
    NNS_G3dLocalOriginToScrPos

    Looks for the position on the screen of the local coordinate-type origin.
    The current position coordinate matrix and the current projection matrix must be configured.

    If the return value is 0, the coordinates on the screen are stored in *px and *py.
    If the return value is -1, it's offscreen, but the coordinates that will be the directional marker are stored in *px and *py.
 *---------------------------------------------------------------------------*/
int
NNS_G3dLocalOriginToScrPos(int* px, int* py)
{
    // Call after configuring so that the coordinates can be converted from
    // local to clip coordinates with the current position local
    // coordinates matrix and the current projection conversion matrix.
    
    VecFx32 vec;
    fx32 w;
    fx64c invW;
    int x1, y1, x2, y2;
    int dx, dy;
    int rval;

    NNS_G3D_NULL_ASSERT(px);
    NNS_G3D_NULL_ASSERT(py);

    // look for the clip coordinates for the local coordinates (0,0,0).
    NNS_G3dGePositionTest(0, 0, 0);
    NNS_G3dGeFlushBuffer();
    while(G3X_GetPositionTestResult(&vec, &w))
        ;

    invW = FX_InvFx64c(w);

    // conversion to normalized screen coordinate type
    vec.x = (FX_Mul32x64c(vec.x, invW) + FX32_ONE) / 2;
    vec.y = (FX_Mul32x64c(vec.y, invW) + FX32_ONE) / 2;

    // the local coordinate origin is being converted to a normalized screen coordinate type
    if (vec.x < 0 || vec.y < 0 || vec.x > FX32_ONE || vec.y > FX32_ONE)
    {
        // When off-screen
        rval = -1;
    }
    else
    {
        rval = 0;
    }

    NNS_G3dGlbGetViewPort(&x1, &y1, &x2, &y2);
    dx = x2 - x1;
    dy = y2 - y1;

    // Further conversion to screen coordinate type
    // The result must be further interpolated if the BG is made to move horizontally, etc.
    *px = x1 + ((vec.x * dx + FX32_HALF) >> FX32_SHIFT);
    *py = 191 - y1 - ((vec.y * dy + FX32_HALF) >> FX32_SHIFT);

    
    return rval;
}


/*---------------------------------------------------------------------------*
    NNS_G3dWorldPosToScrPos

    Looks for the onscreen position from the world coordinates
    The camera matrix and the projection matrix must be configured in the NNS_G3dGlb structure

    If the return value is 0, the coordinates on the screen are stored in *px and *py.
    If the return value is -1, it's offscreen, but the coordinates that will be the directional marker are stored in *px and *py.
 *---------------------------------------------------------------------------*/
int
NNS_G3dWorldPosToScrPos(const VecFx32* pWorld, int* px, int* py)
{
    // apply the viewing conversion and the projection conversion
    // in the world coordinates

    const MtxFx44* proj;
    const MtxFx43* camera;
    VecFx32 tmp;
    VecFx32 vec;
    fx32 w;
    fx64c invW;
    int x1, y1, x2, y2;
    int dx, dy;
    int rval;

    NNS_G3D_NULL_ASSERT(pWorld);
    NNS_G3D_NULL_ASSERT(px);
    NNS_G3D_NULL_ASSERT(py);

    proj = NNS_G3dGlbGetProjectionMtx();
    camera = NNS_G3dGlbGetCameraMtx();

    MTX_MultVec43(pWorld, camera, &tmp);

    w = (fx32)(((fx64)tmp.x * proj->_03 +
                (fx64)tmp.y * proj->_13 +
                (fx64)tmp.z * proj->_23) >> FX32_SHIFT);
    w += proj->_33;

    FX_InvAsync(w);

    vec.x = (fx32)(((fx64)tmp.x * proj->_00 +
                    (fx64)tmp.y * proj->_10 +
                    (fx64)tmp.z * proj->_20) >> FX32_SHIFT);
    vec.x += proj->_30;

    vec.y = (fx32)(((fx64)tmp.x * proj->_01 +
                    (fx64)tmp.y * proj->_11 +
                    (fx64)tmp.z * proj->_21) >> FX32_SHIFT);
    vec.y += proj->_31;

    invW = FX_GetInvResultFx64c();

    // conversion to normalized screen coordinate type
    vec.x = (FX_Mul32x64c(vec.x, invW) + FX32_ONE) / 2;
    vec.y = (FX_Mul32x64c(vec.y, invW) + FX32_ONE) / 2;

    // the local coordinate origin is being converted to a normalized screen coordinate type
    if (vec.x < 0 || vec.y < 0 || vec.x > FX32_ONE || vec.y > FX32_ONE)
    {
        // When off-screen
        rval = -1;
    }
    else
    {
        rval = 0;
    }

    NNS_G3dGlbGetViewPort(&x1, &y1, &x2, &y2);
    dx = x2 - x1;
    dy = y2 - y1;

    // Further conversion to screen coordinate type
    // The result must be further interpolated if the BG is made to move horizontally, etc.
    *px = x1 + ((vec.x * dx + FX32_HALF) >> FX32_SHIFT);
    *py = 191 - y1 - ((vec.y * dy + FX32_HALF) >> FX32_SHIFT);
    
    return rval;
}


/*---------------------------------------------------------------------------*
    NNS_G3dScrPosToWorldLine

    The points on the Near clip plane and the Far clip plane that correspond to the
    onscreen position as world coordinates types.
    If pFar is NULL, the calculation of the point on the Far clip plane will be abbreviated.

    If the return value is 0, the onscreen point is in the viewport.
    If the return value is -1, the onscreen point is outside the viewport.
 *---------------------------------------------------------------------------*/
int
NNS_G3dScrPosToWorldLine(int px, int py, VecFx32* pNear, VecFx32* pFar)
{
    int rval;
    int x1, y1, x2, y2;
    int dx, dy;
    fx32 x, y;
    const MtxFx44* m;
    VecFx32 vNear, vFar;
    fx64c invWNear, invWFar;
    fx32 wNear, wFar;

    NNS_G3D_NULL_ASSERT(pNear);

    // conversion to normalized screen coordinate type from BG screen coordinate type
    NNS_G3dGlbGetViewPort(&x1, &y1, &x2, &y2);
    dx = x2 - x1;
    dy = y2 - y1;

    x = FX_Div((px - x1) << FX32_SHIFT, dx << FX32_SHIFT);
    y = FX_Div((py + y1 - 191) << FX32_SHIFT, -dy << FX32_SHIFT);

    if (x < 0 || y < 0 || x > FX32_ONE || y > FX32_ONE)
    {
        rval = -1;
    }
    else
    {
        rval = 0;
    }

    // Becomes a +- 1 cube
    x = (x - FX32_HALF) * 2;
    y = (y - FX32_HALF) * 2;

    // Get the inverse matrix of the matrix that multiplied the projection matrix and the camera matrix
    m = NNS_G3dGlbGetInvVP();

    // The point on the NEAR plane is (x, y, -FX32_ONE, FX32_ONE)
    // The point on the FAR plane is (x, y,  FX32_ONE, FX32_ONE)
    // Apply the inverse matrix and look for the world coordinate-type point
    wNear   = m->_33 + (fx32)(((fx64)x * m->_03 + (fx64)y * m->_13) >> FX32_SHIFT);
    FX_InvAsync(wNear - m->_23);

    vNear.x = m->_30 + (fx32)(((fx64)x * m->_00 + (fx64)y * m->_10) >> FX32_SHIFT);
    vNear.y = m->_31 + (fx32)(((fx64)x * m->_01 + (fx64)y * m->_11) >> FX32_SHIFT);
    vNear.z = m->_32 + (fx32)(((fx64)x * m->_02 + (fx64)y * m->_12) >> FX32_SHIFT);

    if (pFar)
    {
        vFar.x = vNear.x + m->_20;
        vFar.y = vNear.y + m->_21;
        vFar.z = vNear.z + m->_22;
        wFar   = wNear + m->_23;
    }

    vNear.x -= m->_20;
    vNear.y -= m->_21;
    vNear.z -= m->_22;

    invWNear = FX_GetInvResultFx64c();
    if (pFar)
        FX_InvAsync(wFar);
    
    pNear->x = FX_Mul32x64c(vNear.x, invWNear);
    pNear->y = FX_Mul32x64c(vNear.y, invWNear);
    pNear->z = FX_Mul32x64c(vNear.z, invWNear);

    if (pFar)
    {
        invWFar = FX_GetInvResultFx64c();

        pFar->x = FX_Mul32x64c(vFar.x, invWFar);
        pFar->y = FX_Mul32x64c(vFar.y, invWFar);
        pFar->z = FX_Mul32x64c(vFar.z, invWFar);
    }

    return rval;
}





















