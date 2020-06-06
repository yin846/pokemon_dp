/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     sbc.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sbc.c,v $
  Revision 1.48  02/25/2005 05:38:42  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.47  02/22/2005 02:53:48  hatamoto_minoru
  In NODEDESC, BBY the NNS_G3D_RSFLAG_SKIP flag was not processed with a timing C callback. This was fixed.
  In NODEDESC, BBY, BB, the command address was out of alignment when processing after timing C was skipped.

  Revision 1.46  01/20/2005 05:17:29  nishida_kenji
  small fix.

  Revision 1.45  01/19/2005 06:20:14  nishida_kenji
  small fix.

  Revision 1.44  01/06/2005 05:54:54  nishida_kenji
  small fix

  Revision 1.43  12/22/2004 01:54:16  nishida_kenji
  small fix.

  Revision 1.42  12/21/2004 08:27:11  nishida_kenji
  supports env map & proj map.

  Revision 1.41  12/01/2004 06:30:14  nishida_kenji
  fix NNSi_G3dFuncSbc_BBY.

  Revision 1.40  11/09/2004 02:15:48  nishida_kenji
  fix comments.

  Revision 1.39  11/04/2004 07:19:02  nishida_kenji
  fix NNSi_G3dFuncSbc_NODEMIX(fx32 overflow).

  Revision 1.38  10/28/2004 05:20:15  nishida_kenji
  NNSi_G3dFuncSbc_NODEMIX gets faster.

  Revision 1.37  10/21/2004 02:15:25  nishida_kenji
  NODEDESC saves idxNode on NNSG3dRS::currentNodeDesc.

  Revision 1.36  10/07/2004 02:10:43  nishida_kenji
  Add comments.

  Revision 1.35  10/06/2004 04:44:23  nishida_kenji
  workaround to NNSi_G3dFuncSbc_NODEMIX.

  Revision 1.34  09/27/2004 09:57:51  nishida_kenji
  improve callback checking.

  Revision 1.33  09/16/2004 02:47:11  nishida_kenji
  reduce overhead of callbacks.

  Revision 1.32  09/10/2004 09:14:48  nishida_kenji
  keep up with NNS_G3dGlbFlushVP.

  Revision 1.31  09/03/2004 12:04:54  nishida_kenji
  small fix on NNSi_G3dFuncSbc_BBY.

  Revision 1.30  09/01/2004 01:32:30  nishida_kenji
  check limits(numMat, numNode, numShp).

  Revision 1.29  08/30/2004 11:45:51  nishida_kenji
  deal with NITRO-CPU Geometry FIFO glitch(2004/08/26).

  Revision 1.28  08/24/2004 09:33:00  nishida_kenji
  bugfix(updateHintVec_).

  Revision 1.27  08/05/2004 08:39:31  nishida_kenji
  split SBC exec loop as G3dDrawInternal_Loop_.

  Revision 1.26  08/05/2004 01:24:43  nishida_kenji
  small bug fix

  Revision 1.25  08/02/2004 12:34:57  nishida_kenji
  bug fix (in the case of texture SRT animation).

  Revision 1.24  07/26/2004 02:21:59  nishida_kenji
  bug fix (if NNS_G3D_RSFLAG_OPT_SKIP_SBCMTXCALC)

  Revision 1.23  07/24/2004 09:14:47  nishida_kenji
  add comments

  Revision 1.22  07/22/2004 07:24:31  nishida_kenji
  fix bugs if billboards are scaled.

  Revision 1.21  07/21/2004 06:34:18  nishida_kenji
  fix glitches (timing of callback) of NODE, MAT, NODEDESC

  Revision 1.20  07/21/2004 02:26:11  nishida_kenji
  NNS_G3dDraw checks hintvec obsoleteness

  Revision 1.19  07/20/2004 10:45:39  nishida_kenji
  (none)

  Revision 1.18  07/20/2004 07:26:51  nishida_kenji
  small fix.

  Revision 1.17  07/20/2004 01:37:05  nishida_kenji
  fixed comments

  Revision 1.16  07/17/2004 12:54:59  nishida_kenji
  NODEDESC_BB -> BB
  NODEDESC_BBY -> BBY

  Revision 1.15  07/16/2004 07:26:03  nishida_kenji
  NNS_G3dDraw runs without cmd buffer.

  Revision 1.13  07/15/2004 02:39:59  nishida_kenji
  add magW on matAnmResult.

  Revision 1.12  07/14/2004 11:32:29  nishida_kenji
  change sbc function name.
  revise runtime flags

  Revision 1.11  07/13/2004 07:17:28  nishida_kenji
  fix glitches on sbc's callback.

  Revision 1.10  07/13/2004 01:02:43  nishida_kenji
  Add NNS_G3D_RENDEROBJ_FLAG_NOGECMD
        NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW
        NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC.

  Revision 1.7  07/12/2004 06:44:17  nishida_kenji
  revise callback interface.

  Revision 1.1  07/07/2004 12:41:36  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/sbc.h>

#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/glbstate.h>
#include <nnsys/g3d/gecom.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>
#include <nnsys/g3d/util.h>


static void
G3dDrawInternal_Loop_(NNSG3dRS* pRS)
{
    //
    // Loop for SBC execution
    //
    do
    {
        pRS->flag &= ~NNS_G3D_RSFLAG_SKIP;
        NNS_G3D_ASSERTMSG(NNS_G3dFuncSbcTable[(*pRS->c) & NNS_G3D_SBCCMD_MASK],
                          "SBC command not found / disabled.");
        (*NNS_G3dFuncSbcTable[(*pRS->c) & NNS_G3D_SBCCMD_MASK])
                                (pRS, (*pRS->c) & (u32)NNS_G3D_SBCFLG_MASK);
    }
    while(!(pRS->flag & NNS_G3D_RSFLAG_RETURN));
}


static void
G3dDrawInternal_(NNSG3dRS* pRS, NNSG3dRenderObj* pRenderObj)
{
    NNS_G3D_NULL_ASSERT(pRS);
    NNS_G3D_NULL_ASSERT(pRenderObj);
  
    //
    // Init
    //
    // NOTICE:
    // First bit of isScaleCacheOne must be standing.
    // If not, there will be problems when Si3d is "classic scale off."
    // (The bit is raised in order for the scale of the parent of the root node to be computed as 1.)
    //
    MI_CpuClearFast(pRS, sizeof(*pRS));
    pRS->isScaleCacheOne[0] = 1; // Depends on endian method

    //
    // It's safer to set NNS_G3D_RSFLAG_NODE_VISIBLE.
    // This prepares for cases where the MAT command is unexpectedly
    // executed by a user-defined SBC.
    //
    pRS->flag = NNS_G3D_RSFLAG_NODE_VISIBLE;

    //
    // Select SBC to interpret
    // You can change the SBC by setting the pointer
    // to pRenderObj->ptrUserSbc.
    //
    if (pRenderObj->ptrUserSbc)
    {
        pRS->c = pRenderObj->ptrUserSbc;
    }
    else
    {
        pRS->c = (u8*)pRenderObj->resMdl + pRenderObj->resMdl->ofsSbc;
    }

    //
    // Configure pRenderObj
    //
    pRS->pRenderObj   = pRenderObj;

    //
    // Place information in cache that will be repeatedly used in pRenderObj->resMdl
    //
    pRS->pResNodeInfo = NNS_G3dGetNodeInfo(pRenderObj->resMdl);
    pRS->pResMat      = NNS_G3dGetMat(pRenderObj->resMdl);
    pRS->pResShp      = NNS_G3dGetShp(pRenderObj->resMdl);
    pRS->funcJntScale = NNS_G3dGetJointScale_FuncArray[pRenderObj->resMdl->info.scalingRule];
    pRS->funcJntMtx   = NNS_G3dSendJointSRT_FuncArray[pRenderObj->resMdl->info.scalingRule];
    pRS->funcTexMtx   = NNS_G3dSendTexSRT_FuncArray[pRenderObj->resMdl->info.texMtxMode];
    pRS->posScale     = pRenderObj->resMdl->info.posScale;
    pRS->invPosScale  = pRenderObj->resMdl->info.invPosScale;

    //
    // callback setup
    //
    if (pRenderObj->cbFunc && pRenderObj->cbCmd < NNS_G3D_SBC_COMMAND_NUM)
    {
        NNS_G3D_SBC_CALLBACK_TIMING_ASSERT(pRenderObj->cbTiming);
        pRS->cbVecFunc[pRenderObj->cbCmd] = pRenderObj->cbFunc;
        pRS->cbVecTiming[pRenderObj->cbCmd] = pRenderObj->cbTiming;
    }

    if (pRenderObj->flag & NNS_G3D_RENDEROBJ_FLAG_RECORD)
    {
        pRS->flag |= NNS_G3D_RSFLAG_OPT_RECORD;
    }

    if (pRenderObj->flag & NNS_G3D_RENDEROBJ_FLAG_NOGECMD)
    {
        pRS->flag |= NNS_G3D_RSFLAG_OPT_NOGECMD;
    }

    if (pRenderObj->flag & NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW)
    {
        pRS->flag |= NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW;
    }

    if (pRenderObj->flag & NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC)
    {
        pRS->flag |= NNS_G3D_RSFLAG_OPT_SKIP_SBCMTXCALC;
    }

    if (pRenderObj->cbInitFunc)
    {
        (*pRenderObj->cbInitFunc)(pRS);
    }

    G3dDrawInternal_Loop_(pRS);

    //
    // Clean up
    //
    // Asserts for notifying 'DO NOT MODIFY THEM'
    NNS_G3D_ASSERT(pRS->pRenderObj == pRenderObj);
    NNS_G3D_ASSERT(pRS == NNS_G3dRS);

    // To make use easier, record flag is reset automatically after recording.
    // The mode becomes Play mode the next time called.
    pRenderObj->flag &= ~NNS_G3D_RENDEROBJ_FLAG_RECORD; 
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
    NNS_G3dDraw

    Draws the model. Settings for animation, etc., are configured ahead of
    time by pRenderObj operations.
 *---------------------------------------------------------------------------*/
void
NNS_G3dDraw(NNSG3dRenderObj* pRenderObj)
{
    // NOTICE
    // NOT REENTRANT
    NNS_G3D_NULL_ASSERT(pRenderObj);
    NNS_G3D_NULL_ASSERT(pRenderObj->resMdl);
    NNS_G3D_ASSERTMSG(pRenderObj->resMdl->info.numMat <= NNS_G3D_SIZE_MAT_MAX,
                      "numMaterial exceeds NNS_G3D_SIZE_MAT_MAX");
    NNS_G3D_ASSERTMSG(pRenderObj->resMdl->info.numNode <= NNS_G3D_SIZE_JNT_MAX,
                      "numNode exceeds NNS_G3D_SIZE_JNT_MAX");
    NNS_G3D_ASSERTMSG(pRenderObj->resMdl->info.numShp <= NNS_G3D_SIZE_SHP_MAX,
                      "numShape exceeds NNS_G3D_SIZE_SHP_MAX");

    if (NNS_G3dRenderObjTestFlag(pRenderObj, NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE))
    {
        // Update because the bit vector for "hint" is not correct
        MI_CpuClear32(&pRenderObj->hintMatAnmExist[0], sizeof(u32) * (NNS_G3D_SIZE_MAT_MAX / 32));
        MI_CpuClear32(&pRenderObj->hintJntAnmExist[0], sizeof(u32) * (NNS_G3D_SIZE_JNT_MAX / 32));
        MI_CpuClear32(&pRenderObj->hintVisAnmExist[0], sizeof(u32) * (NNS_G3D_SIZE_JNT_MAX / 32));

        if (pRenderObj->anmMat)
            updateHintVec_(&pRenderObj->hintMatAnmExist[0], pRenderObj->anmMat);
        if (pRenderObj->anmJnt)
            updateHintVec_(&pRenderObj->hintJntAnmExist[0], pRenderObj->anmJnt);
        if (pRenderObj->anmVis)
            updateHintVec_(&pRenderObj->hintVisAnmExist[0], pRenderObj->anmVis);

        NNS_G3dRenderObjResetFlag(pRenderObj, NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE);
    }

    if (NNS_G3dRS)
    {
        G3dDrawInternal_(NNS_G3dRS, pRenderObj);
    }
    else
    {
        // Take local stack if SBC runtime structure region has not been secured.
        NNSG3dRS rs;
        NNS_G3dRS = &rs;
        G3dDrawInternal_(&rs, pRenderObj);
        NNS_G3dRS = NULL;
    }
}



//
// Evokes warning regarding the coding of the callback-determination code
// NOTICE:
// Be aware that the callback conditions inside the callback function
// can be overwritten by the user. In other words, the determination whether or not there is a callback is stored in the auto variable and must not be carried forward when making a determination about the next callback.
//  
//

/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_NOP

    mnemonic:   NNS_G3D_SBC_NOP
    operands:   none
    callbacks:  A/B/C

    Explanation of operations:
    Does nothing.

    Remarks:
    Callback is valid, but the timing specification is meaningless.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_NOP(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

#if !defined(NNS_G3D_SBC_CALLBACK_TIMING_A_DISABLE) || \
    !defined(NNS_G3D_SBC_CALLBACK_TIMING_B_DISABLE) || \
    !defined(NNS_G3D_SBC_CALLBACK_TIMING_C_DISABLE)
    
    if (rs->cbVecFunc[NNS_G3D_SBC_NOP])
    {
        (*rs->cbVecFunc[NNS_G3D_SBC_NOP])(rs);
    }
#endif

    rs->c++;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_RET

    mnemonic:   NNS_G3D_SBC_RET
    operands:   none
    callbacks:  A/B/C

    Explanation of operations:
    Returns NNS_G3D_FUNCSBC_STATUS_RETURN and ends execution of Sbc.

    Remarks:
    This is the command at the end of the SBC.
    Callback is valid, but the timing specification is meaningless.
    This command does not increment NNS_G3dRS->c.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_RET(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

#if !defined(NNS_G3D_SBC_CALLBACK_TIMING_A_DISABLE) || \
    !defined(NNS_G3D_SBC_CALLBACK_TIMING_B_DISABLE) || \
    !defined(NNS_G3D_SBC_CALLBACK_TIMING_C_DISABLE)

    if (rs->cbVecFunc[NNS_G3D_SBC_RET])
    {
        (*rs->cbVecFunc[NNS_G3D_SBC_RET])(rs);
    }
#endif

    rs->flag |= NNS_G3D_RSFLAG_RETURN;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_NODE

    mnemonic:   NNS_G3D_SBC_NODE
    operands:   nodeID
                flags(1byte)
                    visible if 0x01 is on
    callbacks:  A: before
                B: none
                C: after
    
    Explanation of operations:
    - Calls the TIMING_A callback
    - Performs check before execution of visibility animation
       If visibility animation exists:
           - Sets the NNS_G3D_RSFLAG_NODE_VISIBLE bit in accordance with the
           result (not the returned value) of the visibility animation blend function
       If visibility animation does not exist:
           - References "flags" to set the NNS_G3D_RSFLAG_NODE_VISIBLE bit
    - Calls the TIMING_B callback
    - Adds rs->c

    Remarks:
    The NNS_G3D_RSFLAG_NODE_VISIBLE bit is only used inside this function.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_NODE(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW))
    {
        BOOL                    cbFlag;
        NNSG3dSbcCallBackTiming cbTiming;
        u32                     curNode;

        curNode = rs->currentNode = *(rs->c + 1);
        rs->flag |= NNS_G3D_RSFLAG_CURRENT_NODE_VALID;
        rs->pVisAnmResult = &rs->tmpVisAnmResult;

        // Callback A
        // If NNS_G3D_RSFLAG_SKIP is set internally, the
        // visibility calculation can be captured using the
        // operation rs->pVisAnmResult.
        cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_NODE, &cbTiming);

        if (!cbFlag)
        {
            NNS_G3D_NULL_ASSERT(rs->pRenderObj->funcBlendVis);

            if (rs->pRenderObj->anmVis                                            &&
                NNSi_G3dBitVecCheck(&rs->pRenderObj->hintVisAnmExist[0], curNode) &&
                (*rs->pRenderObj->funcBlendVis)(rs->pVisAnmResult, rs->pRenderObj->anmVis, curNode))
            {
                // This code covers when the AnmObj list has been set, the hint vector bit for the visibility animation is ON, and there actually is a visibility animation in the visibility animation blender.
                //  
                //  
                ;
            }
            else
            {
                // If there is no visibility animation,
                // then judge from the operand's data.
                rs->pVisAnmResult->isVisible = *(rs->c + 2) & 1;
            }
        }
        
        // Callback B
        // The calculated visibility can be modified by manipulating rs->pVisAnmResult.
        //   
        cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_NODE, &cbTiming);

        if (!cbFlag)
        {
            if (rs->pVisAnmResult->isVisible)
            {
                rs->flag |= NNS_G3D_RSFLAG_NODE_VISIBLE;
            }
            else
            {
                rs->flag &= ~NNS_G3D_RSFLAG_NODE_VISIBLE;
            }
        }

        // Callback C
        // You can insert any process before the next command.
        (void)NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_NODE, cbTiming);
    }

    rs->c += 3;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_MTX

    mnemonic:   NNS_G3D_SBC_MTX
    operands:   idxMtx
    callbacks:  A: before
                B: none
                C: after

    Explanation of operations:
    - Skip this command entirely if NNS_G3D_RSFLAG_NODE_VISIBLE is OFF
    - Calls the TIMING_A callback
    - Load the current matrix from the matrix stack
    - Calls the TIMING_B callback
    - Adds rs->c

    Remarks:
    Everything including the callback is skipped if NNS_G3D_RSFLAG_NODE_VISIBLE is OFF.
    The mode must be set to POSITION_VECTOR before these operations start.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_MTX(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

    // The MTX command is put in the Draw category
    // From the command created before MAT,SHP
    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW) &&
        (rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE))
    {
        BOOL                    cbFlag;
        NNSG3dSbcCallBackTiming cbTiming;

        // Callback A
        // If NNS_G3D_RSFLAG_SKIP is set internally, the
        // substitute the matrix restore operation.
        cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_MTX, &cbTiming);

        if (!cbFlag)
        {
            u32 arg = *(rs->c + 1);
            NNS_G3D_ASSERT(arg < 31);

            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &arg,
                                    G3OP_MTX_RESTORE_NPARAMS);
            }
        }

        // Callback C
        // You can insert any process before the next command.
        (void)NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_MTX, cbTiming);
    }

    rs->c += 2;
}


void
NNSi_G3dFuncSbc_MAT_InternalDefault(NNSG3dRS* rs,
                                    u32 opt,
                                    const NNSG3dResMatData* mat,
                                    u32 idxMat)
{
    static const u32 matColorMask_[8] =
    {
        0x00000000,
        0x00007fff,
        0x7fff0000,
        0x7fff7fff,
        0x00008000,
        0x0000ffff,
        0x7fff8000,
        0x7fffffff
    };
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;

    NNS_G3D_NULL_ASSERT(rs);
    NNS_G3D_NULL_ASSERT(mat);

    rs->currentMat = (u8)idxMat;
    rs->flag |= NNS_G3D_RSFLAG_CURRENT_MAT_VALID;
    // must specify used region when callback is called
    rs->pMatAnmResult = &rs->tmpMatAnmResult;

    // Callback A
    // If NNS_G3D_RSFLAG_SKIP is set internally, the
    // substitute the material settings code.
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_MAT, &cbTiming);

    if (!cbFlag)
    {
        NNSG3dMatAnmResult* pResult;

        //
        // Determine whether to use the calculation-result buffer.
        //
        if (rs->pRenderObj->recMatAnm &&
            !(rs->flag & NNS_G3D_RSFLAG_OPT_RECORD))
        {
            // If buffer can be used:
            pResult = (rs->pRenderObj->recMatAnm + idxMat);
        }
        else
        {
            //
            // In this "if" block, the material data is fetched and calculated and
            // the appropriate data is set to pResult.
            //
            if ((opt == NNS_G3D_SBCFLG_001 || opt == NNS_G3D_SBCFLG_010) &&
                NNSi_G3dBitVecCheck(&rs->isMatCached[0], idxMat))
            {
                // If the MAT cached flag is standing:
                if (rs->pRenderObj->recMatAnm)
                {
                    // The previous calculation result is registered in the buffer, so fetch from there
                    pResult = (rs->pRenderObj->recMatAnm + idxMat);
                }
                else
                {
                    // The previous calculation result is registered in cache, so fetch from there
                    pResult = &NNS_G3dRSOnGlb.matCache[idxMat];
                }
            }
            else
            {
                if (rs->pRenderObj->recMatAnm)
                {
                    // The MAT cached flag is standing
                    NNSi_G3dBitVecSet(&rs->isMatCached[0], idxMat);

                    // If there is a buffer, register in the buffer regardless of the instruction option
                    pResult = (rs->pRenderObj->recMatAnm + idxMat);
                }
                else if (opt == NNS_G3D_SBCFLG_010)
                {
                    // The MAT cached flag is standing
                    NNSi_G3dBitVecSet(&rs->isMatCached[0], idxMat);

                    // Set the write destination in the MAT cache
                    pResult = &NNS_G3dRSOnGlb.matCache[idxMat];
                }
                else
                {
                    // Use the region in the NNSG3dRS
                    pResult = &rs->tmpMatAnmResult;
                }

                //
                // Set the model data's material information
                // Combine with the G3dGlb default
                //
                {
                    pResult->flag           = (NNSG3dMatAnmResultFlag) 0;
                    if (NNS_G3dGetMatDataByIdx(rs->pResMat, idxMat)->flag & NNS_G3D_MATFLAG_WIREFRAME)
                    {
                        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_WIREFRAME;
                    }
                }
                {
                    // Diffuse & Ambient & VtxColor
                    u32 mask = matColorMask_[(mat->flag >> 6) & 7];
                    pResult->prmMatColor0 = (NNS_G3dGlb.prmMatColor0 & ~mask) |
                                            (mat->diffAmb & mask);
                }

                {
                    // Specular & Emission & Shininess
                    u32 mask = matColorMask_[(mat->flag >> 9) & 7];
                    pResult->prmMatColor1 = (NNS_G3dGlb.prmMatColor1 & ~mask) |
                                            (mat->specEmi & mask);
                }

                pResult->prmPolygonAttr = (NNS_G3dGlb.prmPolygonAttr & ~mat->polyAttrMask) |
                                          (mat->polyAttr & mat->polyAttrMask);

                pResult->prmTexImage = mat->texImageParam;
                pResult->prmTexPltt  = mat->texPlttBase;

                // If texture matrix exists in the model,
                // set the texture matrix from the model
                if (mat->flag & NNS_G3D_MATFLAG_TEXMTX_USE)
                {
                    const u8* p = (const u8*)mat + sizeof(NNSG3dResMatData);

                    if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_SCALEONE))
                    {
                        pResult->scaleS = *((fx32*)(p));
                        pResult->scaleT = *((fx32*)(p + 4));
                        p += 8;
                    }
                    else
                    {
                        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE;
                    }

                    if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_ROTZERO))
                    {
                        pResult->sinR = *((fx16*)(p));
                        pResult->cosR = *((fx16*)(p + 2));
                        p += 4;
                    }
                    else
                    {
                        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO;
                    }

                    if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_TRANSZERO))
                    {
                        pResult->transS = *((fx32*)(p));
                        pResult->transT = *((fx32*)(p + 4));
                    }
                    else
                    {
                        pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO;
                    }

                    pResult->flag |= NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET;
                }

                NNS_G3D_NULL_ASSERT(rs->pRenderObj->funcBlendMat);

                // Check the material animation
                if (rs->pRenderObj->anmMat &&
                    NNSi_G3dBitVecCheck(&rs->pRenderObj->hintMatAnmExist[0], idxMat))
                {
                    // Based on the material animation calculation, pResult will probably change
                    (void)(*rs->pRenderObj->funcBlendMat)(pResult, rs->pRenderObj->anmMat, idxMat);
                }

                //
                // Texture width and height information is added if there is texture matrix
                // and animation is attached.
                //
                if ( pResult->flag & (NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET |
                                      NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT))
                {
                    pResult->origWidth  = mat->origWidth;
                    pResult->origHeight = mat->origHeight;
                    pResult->magW = mat->magW;
                    pResult->magH = mat->magH;
                }
            }
        }
        rs->pMatAnmResult = pResult;
    }

    // Callback B
    // If NNS_G3D_RSFLAG_SKIP is set internally, the
    // the material's transfer operation can be substituted.
    cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_MAT, &cbTiming);

    if (!cbFlag)
    {
        NNSG3dMatAnmResult* pResult;

        NNS_G3D_NULL_ASSERT(rs->pMatAnmResult);
        pResult = rs->pMatAnmResult;

        // Check whether there is transparent material
        if (pResult->prmPolygonAttr & REG_G3_POLYGON_ATTR_ALPHA_MASK)
        {
            // Send geometry command when not transparent or when a wire frame display is used
            //  
            
            if (pResult->flag & NNS_G3D_MATANM_RESULTFLAG_WIREFRAME)
            {
                // If wireframe, the alpha sent to the geometry engine becomes 0
                pResult->prmPolygonAttr &= ~REG_G3_POLYGON_ATTR_ALPHA_MASK;
            }

            // Transparent flag OFF
            rs->flag &= ~NNS_G3D_RSFLAG_MAT_TRANSPARENT;

            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                // Push to stack and transmit
                u32 cmd[7];

                cmd[0] = (G3OP_DIF_AMB      << 0) |
                         (G3OP_SPE_EMI      << 8) |
                         (G3OP_POLYGON_ATTR << 16);
                cmd[1] = pResult->prmMatColor0;
                cmd[2] = pResult->prmMatColor1;
                cmd[3] = pResult->prmPolygonAttr;
                cmd[4] = (G3OP_TEXIMAGE_PARAM << 0) |
                         (G3OP_TEXPLTT_BASE << 8);
                cmd[5] = pResult->prmTexImage;
                cmd[6] = pResult->prmTexPltt;

                NNS_G3dGeBufferData_N(&cmd[0], 7);

                if (pResult->flag & (NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET |
                                     NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT))
                {

                    // Send texture matrix
                    NNS_G3D_NULL_ASSERT(rs->funcTexMtx);
                    (*rs->funcTexMtx)(pResult);
                }
            }
        }
        else
        {
            // Transparent flag ON
            rs->flag |= NNS_G3D_RSFLAG_MAT_TRANSPARENT;
        }
    }

    // Callback C
    // You can insert any process before the next command.
    (void) NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_MAT, cbTiming);
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_MAT

    mnemonic:   NNS_G3D_SBC_MAT([000], [001], [010])
    operands:   idxMat
    callbacks:  A: before
                B: during
                C: after

    Explanation of operations:
    - Skip this command entirely if NNS_G3D_RSFLAG_NODE_VISIBLE is OFF
    - Calls the TIMING_A callback
    - Get/calculate the material information
       Operations relating to the MAT cache will differ, depending on the "opt" settings:
       If the nth bit of rs->isMatCached is standing (where n = idxMat), then the material
       information can be cached in MAT cache and used.

       [000] Do not reference MAT cache
       [001] Read/write to MAT cache
       [010] Read MAT cache

       If set to NNS_G3D_RSFLAG_MAT_TRANSPARENT,
       then the process up to adding rs->c is skipped.
    - Calls the TIMING_B callback
    - Send the material information to the geometry engine
    - Call the TIMING_C callback
    - Adds rs->c

    Remarks:
    Deciding how best to use [000],[001],[010] is done at time of callback.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_MAT(NNSG3dRS* rs, u32 opt)
{
    NNS_G3D_NULL_ASSERT(rs);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW))
    {
        u32 idxMat;
        idxMat = *(rs->c + 1);

        // Skip command if not VISIBLE or if currently already in same MatID state
        if ((rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE) ||
            !((rs->flag & NNS_G3D_RSFLAG_CURRENT_MAT_VALID) &&
            (idxMat == rs->currentMat)))
        {
            const NNSG3dResMatData* mat =
                NNS_G3dGetMatDataByIdx(rs->pResMat, idxMat);

            NNS_G3D_NULL_ASSERT(NNS_G3dFuncSbcMatTable[mat->itemTag]);
            (*NNS_G3dFuncSbcMatTable[mat->itemTag])(rs, opt, mat, idxMat);
        }
    }
    rs->c += 2;
}


void
NNSi_G3dFuncSbc_SHP_InternalDefault(NNSG3dRS* rs,
                                   u32,
                                   const NNSG3dResShpData* shp,
                                   u32)
{
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;
    NNS_G3D_NULL_ASSERT(rs && shp);

    // Callback A
    // If NNS_G3D_RSFLAG_SKIP is set internally, the
    // substitute display list transfer operation.
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_SHP, &cbTiming);

    if (!cbFlag)
    {
        if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
        {
            NNS_G3dGeSendDL((u8*)shp + shp->ofsDL, shp->sizeDL);
        }
    }

    // Callback B
    (void) NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_SHP, &cbTiming);

    // Return of state
    // In the future, state processes and other processes may be inserted

    // Callback C
    // You can insert any process before the next command.
    (void) NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_SHP, cbTiming);
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_SHP

    mnemonic:   NNS_G3D_SBC_SHP
    operands:   idxShp
    callbacks:  A: before
                B: during
                C: after

    Explanation of operations:
    - Skip this entire command if NNS_G3D_RSFLAG_NODE_VISIBLE is OFF and NNS_G3D_RSFLAG_MAT_TRANSPARENT is ON
        
    - Calls the TIMING_A callback
    - Call display list
    - Calls the TIMING_B callback
    - Return the state changed by the display list
    - Call the TIMING_C callback
    - Add rs->c

    Remarks:
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_SHP(NNSG3dRS* rs, u32 opt)
{
    NNS_G3D_NULL_ASSERT(rs);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW))
    {
        if ((rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE) &&
            !(rs->flag & NNS_G3D_RSFLAG_MAT_TRANSPARENT))
        {
            u32 idxShp = *(rs->c + 1);
            const NNSG3dResShpData* shp =
                NNS_G3dGetShpDataByIdx(rs->pResShp, idxShp);

            NNS_G3D_NULL_ASSERT(NNS_G3dFuncSbcShpTable[shp->itemTag]);
            (*NNS_G3dFuncSbcShpTable[shp->itemTag])(rs, opt, shp, idxShp);
        }
    }
    rs->c += 2;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_NODEDESC([000], [001], [010], [011])

    mnemonic:   NNS_G3D_SBC_NODEDESC
    operands:   [000]: idxNode, idxNodeParent, flags
                [001]: idxNode, idxNodeParent, flags, idxMtxDest
                [010]: idxNode, idxNodeParent, flags, idxMtxSrc
                [011]: idxNode, idxNodeParent, flags, idxMtxDest, idxMtxSrc
                0 <= idxMtxSrc, idxMtxDest <= 30
                x = 0 --> checks visibility anm and change currentNode
                x = 1 --> no
    callbacks:  A: before
                B: during
                C: after

    Explanation of operations:
    - If [010], [011] restore idxMtxSrc to current matrix
    - Calls the TIMING_A callback
    - Multiply the current matrix by the model/animation matrix
    - Calls the TIMING_B callback
    - If [001], [011] store current matrix in idxMtxDest
    - Call the TIMING_C callback
    - Add rs->c

    Remarks:
    Unlike with NNS_G3dFuncSbc_NODE, the NODE state is not changed
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_NODEDESC(NNSG3dRS* rs, u32 opt)
{
    u32 cmdLen = 4;
    static const u8 pivotUtil_[9][4] =
    {
        {4, 5, 7, 8},
        {3, 5, 6, 8},
        {3, 4, 6, 7},
        
        {1, 2, 7, 8},
        {0, 2, 6, 8},
        {0, 1, 6, 7},

        {1, 2, 4, 5},
        {0, 2, 3, 5},
        {0, 1, 3, 4}
    };
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;
    u32 idxNode = *(rs->c + 1);
    rs->currentNodeDesc = (u8)idxNode;
    rs->flag |= NNS_G3D_RSFLAG_CURRENT_NODEDESC_VALID;

    NNS_G3D_NULL_ASSERT(rs);

    if (rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCMTXCALC)
    {
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }


        if (opt == NNS_G3D_SBCFLG_001 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
#if 1
            // After this the matrix can be used as-is, so the
            // store operation must be replaced with the restore operation
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxDest = *(rs->c + 4);
                NNS_G3D_ASSERT(idxMtxDest < 31);

                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &idxMtxDest,
                                    G3OP_MTX_RESTORE_NPARAMS);
            }
#endif
        }
        rs->c += cmdLen;
        return;
    }

    {
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            u32 idxMtxSrc;

            ++cmdLen;
            if (opt == NNS_G3D_SBCFLG_010)
            {
                idxMtxSrc = *(rs->c + 4);
            }
            else
            {
                idxMtxSrc = *(rs->c + 5);
            }
            NNS_G3D_ASSERT(idxMtxSrc < 31);

            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &idxMtxSrc,
                                    G3OP_MTX_RESTORE_NPARAMS);
            }
        }
    }

    // must specify used region when callback is called
    rs->pJntAnmResult = &rs->tmpJntAnmResult;

    // Callback A
    // If NNS_G3D_RSFLAG_SKIP is set internally, the
    // the joint SRT data GET operation can be replaced
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_NODEDESC, &cbTiming);

    if (!cbFlag)
    {
        NNSG3dJntAnmResult* pAnmResult;
        BOOL                isUseRecordData;
//        u32 idxNodeParent = *(rs->c + 2);
//        u32 flags         = *(rs->c + 3);

        //
        // Determine whether to use the calculation-result buffer.
        //
        if (rs->pRenderObj->recJntAnm)
        {
            // point the external buffer
            pAnmResult = (rs->pRenderObj->recJntAnm + idxNode);
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_RECORD))
            {
                // play the external buffer data
                isUseRecordData = TRUE;
            }
            else
            {
                // record the data in the external buffer
                isUseRecordData = FALSE;
            }
        }
        else
        {
            // temporarily use the buffer internal to the function
            isUseRecordData = FALSE;
            pAnmResult = &rs->tmpJntAnmResult;
        }

        if (!isUseRecordData)
        {
            // Only NNSG3dJntAnmResult::flag must be cleared in advance before the calculation
            pAnmResult->flag = (NNSG3dJntAnmResultFlag) 0;

            NNS_G3D_NULL_ASSERT(rs->pRenderObj->funcBlendJnt);
            if (rs->pRenderObj->anmJnt &&
                (*rs->pRenderObj->funcBlendJnt)(pAnmResult, rs->pRenderObj->anmJnt, idxNode))
            {
                //
                // anmResult is being obtained using the joint animation resource
                //
                ;
            }
            else
            {
                //
                // get anmResult using the static model resource
                //
                NNSG3dResNodeData* pNd =
                    NNS_G3dGetNodeDataByIdx(rs->pResNodeInfo, idxNode);
                u8* p = (u8*)pNd + sizeof(*pNd);

                //
                // Translation
                //
                if (pNd->flag & NNS_G3D_SRTFLAG_TRANS_ZERO)
                {
                    pAnmResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO;
                }
                else
                {
                    pAnmResult->trans.x = *(fx32*)(p + 0);
                    pAnmResult->trans.y = *(fx32*)(p + 4);
                    pAnmResult->trans.z = *(fx32*)(p + 8);
                    p += 12;
                }

                //
                // Rotation
                //
                if (pNd->flag & NNS_G3D_SRTFLAG_ROT_ZERO)
                {
                    pAnmResult->flag |= NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO;
                }
                else
                {
                    if (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_EXIST)
                    {
                        //
                        // If compressed (mainly single-axle rotation),
                        // restore the compressed matrix.
                        //
                        fx32 A = *(fx16*)(p + 0);
                        fx32 B = *(fx16*)(p + 2);
                        u32 idxPivot = (u32)( (pNd->flag & NNS_G3D_SRTFLAG_IDXPIVOT_MASK) >> 
                                                        NNS_G3D_SRTFLAG_IDXPIVOT_SHIFT );
                        // clear anmResult.rot
                        MI_Zero36B(&pAnmResult->rot);
                        
                        pAnmResult->rot.a[idxPivot] =
                            (pNd->flag & NNS_G3D_SRTFLAG_PIVOT_MINUS) ?
                                -FX32_ONE :
                                FX32_ONE;
                        
                        pAnmResult->rot.a[pivotUtil_[idxPivot][0]] = A;
                        pAnmResult->rot.a[pivotUtil_[idxPivot][1]] = B;

                        pAnmResult->rot.a[pivotUtil_[idxPivot][2]] =
                            (pNd->flag & NNS_G3D_SRTFLAG_SIGN_REVC) ? -B : B;

                        pAnmResult->rot.a[pivotUtil_[idxPivot][3]] =
                            (pNd->flag & NNS_G3D_SRTFLAG_SIGN_REVD) ? -A : A;

                        p += 2 * sizeof(fx16);
                    }
                    else
                    {
                        // NOTICE:
                        // Do not replace in the memory copy API
                        // because of the implicit casting from fx16 to fx32

                        fx16* pp = (fx16*)p;
                        pAnmResult->rot.a[0] = pNd->_00;
                        pAnmResult->rot.a[1] = *(pp + 0);
                        pAnmResult->rot.a[2] = *(pp + 1);
                        pAnmResult->rot.a[3] = *(pp + 2);
                        pAnmResult->rot.a[4] = *(pp + 3);
                        pAnmResult->rot.a[5] = *(pp + 4);
                        pAnmResult->rot.a[6] = *(pp + 5);
                        pAnmResult->rot.a[7] = *(pp + 6);
                        pAnmResult->rot.a[8] = *(pp + 7);

                        p += 8 * sizeof(fx16);
                    }
                }

                //
                // Scale
                //

                //
                // NOTICE:
                // For MayaSSC or Si3d classic scale off,
                // the calculation when the flag is not active will also occur
                // during NNS_G3D_SRTFLAG_SCALE_ONE.
                //
                NNS_G3D_NULL_ASSERT(rs->funcJntScale);
                (*rs->funcJntScale)(pAnmResult, (fx32*)p, rs->c, pNd->flag);
            }
        }
        rs->pJntAnmResult = pAnmResult;
    }

    // Callback B
    // If NNS_G3D_RSFLAG_SKIP is set internally, the
    // the send to the geometry engine operations can be replaced
    cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_NODEDESC, &cbTiming);

    if (!cbFlag)
    {
        //
        // send to the geometry engine
        //
        if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
        {
            NNS_G3D_NULL_ASSERT(rs->pJntAnmResult);
            NNS_G3D_NULL_ASSERT(rs->funcJntMtx);

            (*rs->funcJntMtx)(rs->pJntAnmResult);
        }
    }

    rs->pJntAnmResult = NULL;

    // Callback C
    // You can insert any process before the next command.
    // Also, by turning ON NNS_G3D_RSFLAG_SKIP,
    // the current matrix store operations can be replaced
    cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_NODEDESC, cbTiming);

    if (opt == NNS_G3D_SBCFLG_001 ||
        opt == NNS_G3D_SBCFLG_011)
    {
        ++cmdLen;

        if (!cbFlag)
        {
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxDest = *(rs->c + 4);
                NNS_G3D_ASSERT(idxMtxDest < 31);

                NNS_G3dGeBufferOP_N(G3OP_MTX_STORE,
                                    &idxMtxDest,
                                    G3OP_MTX_STORE_NPARAMS);
            }
        }
    }

    rs->c += cmdLen;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_BB([000], [001], [010], [011])

    mnemonic:   NNS_G3D_SBC_BB
    operands:   [000]: idxNode
                [001]: idxNode, idxMtxDest
                [010]: idxNode, idxMtxSrc
                [011]: idxNode, idxMtxDest, idxMtxSrc
                0 <= idxMtxSrc, idxMtxDest <= 30
    callbacks:  A: before
                B: during
                C: after

    Explanation of operations:
    - If [010], [011] restore idxMtxSrc to current matrix
    - Calls the TIMING_A callback
    -   push the projection matrix and set the unit matrix
    -   wait for the geometry engine to stop, and take out the current matrix
    -  use the CPU to process so that this becomes the billboard matrix, then store in the current matrix
    - Calls the TIMING_B callback
    - If [001], [011] store current matrix in idxMtxDest
    - Call the TIMING_C callback
    - Add rs->c

    Remarks:
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_BB(NNSG3dRS* rs, u32 opt)
{
    //
    // parallel billboard display in the camera projection plane
    //
    u32 cmdLen = 2;

    static u32 bbcmd1[] =
    {
        ((G3OP_MTX_POP << 0)       |
         (G3OP_MTX_MODE << 8)      |
         (G3OP_MTX_LOAD_4x3 << 16) |
         (G3OP_MTX_SCALE << 24)),
        1,
        GX_MTXMODE_POSITION_VECTOR,
        FX32_ONE, 0, 0, 
        0, FX32_ONE, 0,
        0, 0, FX32_ONE,
        0, 0, 0,   // This is subject to change  (Trans)
        0, 0, 0    // This is subject to change  (Scale)
    };
    
    VecFx32* trans = (VecFx32*)&bbcmd1[12];
    VecFx32* scale = (VecFx32*)&bbcmd1[15];
    MtxFx44 m;
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;

    NNS_G3D_NULL_ASSERT(rs);

    if (rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW)
    {
        // billboard goes into the Draw category
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }
        if (opt == NNS_G3D_SBCFLG_001 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }
        rs->c += cmdLen;
        return;
    }

    {
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;

            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxSrc;
                if (opt == NNS_G3D_SBCFLG_010)
                {
                    idxMtxSrc = *(rs->c + 2);
                }
                else
                {
                    idxMtxSrc = *(rs->c + 3);
                }
                NNS_G3D_ASSERT(idxMtxSrc < 31);

                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &idxMtxSrc,
                                    1);
            }
        }
    }

    // Callback A
    // If NNS_G3D_RSFLAG_SKIP is set internally, the
    // the billboard matrix configuration operations can be replaced
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_BB, &cbTiming);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD) &&
        (!cbFlag))
    {
        // flash the buffer
        NNS_G3dGeFlushBuffer();

        //
        // immediate sending is possible up to next callback point
        //

        // command  transmission :
        // change to PROJ mode
        // save the projection matrix
        // set the unit matrix
        reg_G3X_GXFIFO = ((G3OP_MTX_MODE << 0) |
                          (G3OP_MTX_PUSH << 8) |
                          (G3OP_MTX_IDENTITY << 16));
        reg_G3X_GXFIFO = (u32)GX_MTXMODE_PROJECTION;
        reg_G3X_GXFIFO = 0; // 2004/08/26 geometry fifo glitch

        // Wait further for the geometry engine to stop
        // get the current matrix
        while (G3X_GetClipMtx(&m))
            ;

        if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
        {
            // If the camera matrix is in the projection matrix,
            // the camera matrix with SRT must be multiplied from the back in advance
            const MtxFx43* cam = NNS_G3dGlbGetSrtCameraMtx();
            MtxFx44 tmp;

            MTX_Copy43To44(cam, &tmp);
            MTX_Concat44(&m, &tmp, &m);
        }
        else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
        {
            const MtxFx43* cam = NNS_G3dGlbGetCameraMtx();
            MtxFx44 tmp;

            MTX_Copy43To44(cam, &tmp);
            MTX_Concat44(&m, &tmp, &m);
        }

        // billboard matrix calculation
        trans->x = m._30;
        trans->y = m._31;
        trans->z = m._32;

        scale->x = VEC_Mag((VecFx32*)&m._00);
        scale->y = VEC_Mag((VecFx32*)&m._10);
        scale->z = VEC_Mag((VecFx32*)&m._20);

        if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
        {
            // projection matrix POP
            // return to POS_VEC
            // set the inverse matrix of the camera with SRT
            reg_G3X_GXFIFO = (G3OP_MTX_POP << 0)  |
                             (G3OP_MTX_MODE << 8) |
                             (G3OP_MTX_LOAD_4x3 << 16);
            MI_CpuSend32(&bbcmd1[1],
                         &reg_G3X_GXFIFO,
                         2 * sizeof(u32));
            MI_CpuSend32(NNS_G3dGlbGetInvSrtCameraMtx(),
                         &reg_G3X_GXFIFO,
                         G3OP_MTX_LOAD_4x3_NPARAMS * sizeof(u32));

            // multiply by current matrix
            // multiply the calculated scale
            reg_G3X_GXFIFO = (G3OP_MTX_MULT_4x3 << 0) | (G3OP_MTX_SCALE << 8);
            MI_CpuSend32(&bbcmd1[3],
                         &reg_G3X_GXFIFO,
                         sizeof(MtxFx43) + sizeof(VecFx32));
        }
        else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
        {
            // projection matrix POP
            // return to POS_VEC
            // set the inverse matrix of the camera
            reg_G3X_GXFIFO = (G3OP_MTX_POP << 0)  |
                             (G3OP_MTX_MODE << 8) |
                             (G3OP_MTX_LOAD_4x3 << 16);
            MI_CpuSend32(&bbcmd1[1],
                         &reg_G3X_GXFIFO,
                         2 * sizeof(u32));
            MI_CpuSend32(NNS_G3dGlbGetInvCameraMtx(),
                         &reg_G3X_GXFIFO,
                         G3OP_MTX_LOAD_4x3_NPARAMS * sizeof(u32));

            // multiply by current matrix
            // multiply the calculated scale
            reg_G3X_GXFIFO = (G3OP_MTX_MULT_4x3 << 0) | (G3OP_MTX_SCALE << 8);
            MI_CpuSend32(&bbcmd1[3],
                         &reg_G3X_GXFIFO,
                         sizeof(MtxFx43) + sizeof(VecFx32));
        }
        else
        {
            // projection matrix POP
            // return to POS_VEC
            // store in the current matrix
            // multiply the calculated scale
            MI_CpuSend32(&bbcmd1[0],
                         &reg_G3X_GXFIFO,
                         18 * sizeof(u32));
        }
    }

    // Callback C
    // You can insert any process before the next command.
    // Also, by turning ON NNS_G3D_RSFLAG_SKIP,
    // the current matrix store operations can be replaced
    cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_BB, cbTiming);

    if (opt == NNS_G3D_SBCFLG_001 ||
        opt == NNS_G3D_SBCFLG_011)
    {
        ++cmdLen;

        if (!cbFlag)
        {
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxDest;
                idxMtxDest = *(rs->c + 2);

                NNS_G3D_ASSERT(idxMtxDest < 31);

                // Here, it may not be possible to immediately send.
                NNS_G3dGeBufferOP_N(G3OP_MTX_STORE,
                                    &idxMtxDest,
                                    G3OP_MTX_STORE_NPARAMS);
            }
        }
    }

    rs->c += cmdLen;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_BBY([000], [001], [010], [011])

    mnemonic:   NNS_G3D_SBC_BBY
    operands:   [000]: idxNode
                [001]: idxNode, idxMtxDest
                [010]: idxNode, idxMtxSrc
                [011]: idxNode, idxMtxDest, idxMtxSrc
                0 <= idxMtxSrc, idxMtxDest <= 30
    callbacks:  A: before
                B: during
                C: after

    Explanation of operations:
    - If [010], [011] restore idxMtxSrc to current matrix
    - Calls the TIMING_A callback
    -   push the projection matrix and set the unit matrix
    -   wait for the geometry engine to stop, and take out the current matrix
    -  use the CPU to process so that it becomes the Y-axis billboard matrix, then store it in the current matrix
    - Calls the TIMING_B callback
    - If [001], [011] store current matrix in idxMtxDest
    - Call the TIMING_C callback
    - Add rs->c

    Remarks:
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_BBY(NNSG3dRS* rs, u32 opt)
{
    u32 cmdLen = 2;
    MtxFx44 m;

    static u32 bbcmd1[] =
    {
        ((G3OP_MTX_POP << 0)       |
         (G3OP_MTX_MODE << 8)      |
         (G3OP_MTX_LOAD_4x3 << 16) |
         (G3OP_MTX_SCALE << 24)),
        1,
        GX_MTXMODE_POSITION_VECTOR,
        FX32_ONE, 0, 0, // This is subject to change  (4x3Mtx)
        0, FX32_ONE, 0,
        0, 0, FX32_ONE,
        0, 0, 0,   
        0, 0, 0    // This is subject to change  (Scale)
    };
    VecFx32* trans = (VecFx32*)&bbcmd1[12];
    VecFx32* scale = (VecFx32*)&bbcmd1[15];
    MtxFx43* mtx   = (MtxFx43*)&bbcmd1[3];
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;

    NNS_G3D_NULL_ASSERT(rs);

    if (rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW)
    {
        // billboard goes into the Draw category
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }
        if (opt == NNS_G3D_SBCFLG_001 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
        }
        rs->c += cmdLen;
        return;
    }

    {
        if (opt == NNS_G3D_SBCFLG_010 ||
            opt == NNS_G3D_SBCFLG_011)
        {
            ++cmdLen;
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxSrc;
                if (opt == NNS_G3D_SBCFLG_010)
                {
                    idxMtxSrc = *(rs->c + 2);
                }
                else
                {
                    idxMtxSrc = *(rs->c + 3);
                }
                NNS_G3D_ASSERT(idxMtxSrc < 31);
                NNS_G3dGeBufferOP_N(G3OP_MTX_RESTORE,
                                    &idxMtxSrc,
                                    1);
            }
        }
    }

    // Callback A
    // If NNS_G3D_RSFLAG_SKIP is set internally, the
    // the billboard matrix configuration operations can be replaced
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_BBY, &cbTiming);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD) &&
        (!cbFlag))
    {
        // flash the buffer
        NNS_G3dGeFlushBuffer();

        //
        // immediate sending is possible up to next callback point
        //

        // command  transmission :
        // change to PROJ mode
        // save the projection matrix
        // set the unit matrix
        reg_G3X_GXFIFO = ((G3OP_MTX_MODE << 0) |
                          (G3OP_MTX_PUSH << 8) |
                          (G3OP_MTX_IDENTITY << 16));
        reg_G3X_GXFIFO = (u32)GX_MTXMODE_PROJECTION;
        reg_G3X_GXFIFO = 0; // 2004/08/26 geometry fifo glitch

        // Wait further for the geometry engine to stop
        // get the current matrix (clip matrix)
        while (G3X_GetClipMtx(&m))
            ;

        if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
        {
            // If the camera matrix is in the projection matrix,
            // the camera matrix with SRT must be multiplied from the back in advance
            const MtxFx43* cam = NNS_G3dGlbGetSrtCameraMtx();
            MtxFx44 tmp;

            MTX_Copy43To44(cam, &tmp);
            MTX_Concat44(&m, &tmp, &m);
        }
        else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
        {
            // If the camera matrix is in the projection matrix,
            // the camera matrix must be multiplied from the back in advance
            const MtxFx43* cam = NNS_G3dGlbGetCameraMtx();
            MtxFx44 tmp;

            MTX_Copy43To44(cam, &tmp);
            MTX_Concat44(&m, &tmp, &m);
        }

        // billboard matrix calculation

        // 1: divert the translation
        trans->x = m._30;
        trans->y = m._31;
        trans->z = m._32;

        // 2: approximate the scale with the size of the vector on each line
        scale->x = VEC_Mag((VecFx32*)&m._00);
        scale->y = VEC_Mag((VecFx32*)&m._10);
        scale->z = VEC_Mag((VecFx32*)&m._20);

        // 3: the rotation  matrix will be the rotation matrix for the X-axis as a the result, so
        //    set an appropriate value
        if (m._11 != 0 || m._12 != 0)
        {
            VEC_Normalize((VecFx32*)&m._10, (VecFx32*)&mtx->_10);

            mtx->_21 = -mtx->_12;
            mtx->_22 = mtx->_11;
        }
        else
        {
            VEC_Normalize((VecFx32*)&m._20, (VecFx32*)&mtx->_20);
            
            mtx->_12 = -mtx->_21;
            mtx->_11 = mtx->_22;
        }

        if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
        {
            // projection matrix POP
            // return to POS_VEC
            // set the inverse matrix of the camera with SRT
            reg_G3X_GXFIFO = (G3OP_MTX_POP << 0)  |
                             (G3OP_MTX_MODE << 8) |
                             (G3OP_MTX_LOAD_4x3 << 16);
            MI_CpuSend32(&bbcmd1[1],
                         &reg_G3X_GXFIFO,
                         2 * sizeof(u32));
            MI_CpuSend32(NNS_G3dGlbGetInvSrtCameraMtx(),
                         &reg_G3X_GXFIFO,
                         G3OP_MTX_LOAD_4x3_NPARAMS * sizeof(u32));
            
            // multiply by current matrix
            // multiply the calculated scale
            reg_G3X_GXFIFO = (G3OP_MTX_MULT_4x3 << 0) | (G3OP_MTX_SCALE << 8);
            MI_CpuSend32(&bbcmd1[3],
                         &reg_G3X_GXFIFO,
                         sizeof(MtxFx43) + sizeof(VecFx32));
        }
        else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
        {
            // projection matrix POP
            // return to POS_VEC
            // set the inverse matrix of the camera
            reg_G3X_GXFIFO = (G3OP_MTX_POP << 0)  |
                             (G3OP_MTX_MODE << 8) |
                             (G3OP_MTX_LOAD_4x3 << 16);
            MI_CpuSend32(&bbcmd1[1],
                         &reg_G3X_GXFIFO,
                         2 * sizeof(u32));
            MI_CpuSend32(NNS_G3dGlbGetInvCameraMtx(),
                         &reg_G3X_GXFIFO,
                         G3OP_MTX_LOAD_4x3_NPARAMS * sizeof(u32));
            
            // multiply by current matrix
            // multiply the calculated scale
            reg_G3X_GXFIFO = (G3OP_MTX_MULT_4x3 << 0) | (G3OP_MTX_SCALE << 8);
            MI_CpuSend32(&bbcmd1[3],
                         &reg_G3X_GXFIFO,
                         sizeof(MtxFx43) + sizeof(VecFx32));
        }
        else
        {
            // projection matrix POP
            // return to POS_VEC
            // store in the current matrix
            // multiply the calculated scale
            MI_CpuSend32(&bbcmd1[0], &reg_G3X_GXFIFO, 18 * sizeof(u32));
        }
    }

    // Callback C
    // You can insert any process before the next command.
    // Also, by turning ON NNS_G3D_RSFLAG_SKIP,
    // the current matrix store operations can be replaced
    cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_BBY, cbTiming);

    if (opt == NNS_G3D_SBCFLG_001 ||
        opt == NNS_G3D_SBCFLG_011)
    {
        ++cmdLen;

        if (!cbFlag)
        {
            if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD))
            {
                u32 idxMtxDest;
                idxMtxDest = *(rs->c + 2);

                NNS_G3D_ASSERT(idxMtxDest < 31);
                // Here, it may not be possible to immediately send.
                NNS_G3dGeBufferOP_N(G3OP_MTX_STORE,
                                    &idxMtxDest,
                                    G3OP_MTX_STORE_NPARAMS);
            }
        }
    }

    rs->c += cmdLen;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_NODEMIX

    mnemonic:   NNS_G3D_SBC_NODEMIX
    operands:   idxMtxDest, numMtx, idxStack1, idxMtx1, ratio1, ...
                                    idxStackN, idxMtxN, ratioN
    callbacks:  none

    Explanation of operations:
    - Calculates the sum (ratio * InvM(InvN) * AnmM), then stores it in the  matrix stack idxMtxDest.
      In other words, it blends with the matrix that performs the operation for returning the vertex to each joint local coordinate
      via the inverse matrix (InvM, InvN) and multiplies by the animation matrix.
    - The inverse matrix of the modeling conversion matrix (InvM, InvN) is stored in the model resource.
      It it calculated when converting using g3dcvtr.
    - matrix multiplication is performed using the geometry engine.
    - vertices with applied weighted envelopes are stored with global coordinates.

    Remarks:
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_NODEMIX(NNSG3dRS* rs, u32)
{
    fx64 w = 0;
    const NNSG3dResEvpMtx* evpMtx =
        (const NNSG3dResEvpMtx*)((u8*)rs->pRenderObj->resMdl +
                                    rs->pRenderObj->resMdl->ofsEvpMtx);
    u32 numMtx = *(rs->c + 2);
    u8* p = rs->c + 3;

    NNS_G3D_ASSERT(numMtx >= 2);

#if (NNS_G3D_USE_EVPCACHE)
    {
        u32 i;
        struct
        {
            MtxFx43 M;
            MtxFx33 N;
        } sum;
        MtxFx44* pX;
        MtxFx33* pY;

        MI_CpuClearFast(&sum, sizeof(sum));
        NNS_G3dGeFlushBuffer();

        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_StoreMtx(1);
        G3_Identity();
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        for (i = 0; i < numMtx; ++i)
        {
            u32 idxJnt = *(p + 1);
            BOOL evpCached = NNSi_G3dBitVecCheck(&rs->isEvpCached[0], idxJnt);

            pX = &NNS_G3dRSOnGlb.evpCache[idxJnt].M;
            if (!evpCached)
            {
                NNSi_G3dBitVecSet(&rs->isEvpCached[0], idxJnt);

                G3_RestoreMtx(*p);
                G3_MtxMode(GX_MTXMODE_POSITION);
                G3_MultMtx43(&evpMtx[idxJnt].invM);
            }

            if (i != 0)
            {
                sum.N.m[0][0] += (w * pY->m[0][0]) >> FX32_SHIFT;
                sum.N.m[0][1] += (w * pY->m[0][1]) >> FX32_SHIFT;
                sum.N.m[0][2] += (w * pY->m[0][2]) >> FX32_SHIFT;

                sum.N.m[1][0] += (w * pY->m[1][0]) >> FX32_SHIFT;
                sum.N.m[1][1] += (w * pY->m[1][1]) >> FX32_SHIFT;
                sum.N.m[1][2] += (w * pY->m[1][2]) >> FX32_SHIFT;

                sum.N.m[2][0] += (w * pY->m[2][0]) >> FX32_SHIFT;
                sum.N.m[2][1] += (w * pY->m[2][1]) >> FX32_SHIFT;
                sum.N.m[2][2] += (w * pY->m[2][2]) >> FX32_SHIFT;
            }

            if (!evpCached)
            {
                while (G3X_GetClipMtx(pX))
                    ;
                G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
                G3_MultMtx33(&evpMtx[idxJnt].invN);
            }

            w = *(p + 2) << 4;

            sum.M.m[0][0] += (w * pX->m[0][0]) >> FX32_SHIFT;
            sum.M.m[0][1] += (w * pX->m[0][1]) >> FX32_SHIFT;
            sum.M.m[0][2] += (w * pX->m[0][2]) >> FX32_SHIFT;

            sum.M.m[1][0] += (w * pX->m[1][0]) >> FX32_SHIFT;
            sum.M.m[1][1] += (w * pX->m[1][1]) >> FX32_SHIFT;
            sum.M.m[1][2] += (w * pX->m[1][2]) >> FX32_SHIFT;

            sum.M.m[2][0] += (w * pX->m[2][0]) >> FX32_SHIFT;
            sum.M.m[2][1] += (w * pX->m[2][1]) >> FX32_SHIFT;
            sum.M.m[2][2] += (w * pX->m[2][2]) >> FX32_SHIFT;

            sum.M.m[3][0] += (w * pX->m[3][0]) >> FX32_SHIFT;
            sum.M.m[3][1] += (w * pX->m[3][1]) >> FX32_SHIFT;
            sum.M.m[3][2] += (w * pX->m[3][2]) >> FX32_SHIFT;

            p += 3;
            pY = &NNS_G3dRSOnGlb.evpCache[idxJnt].N;

            if (!evpCached)
            {
                while (G3X_GetVectorMtx(pY))
                    ;
            }
        }
        sum.N.m[0][0] += (w * pY->m[0][0]) >> FX32_SHIFT;
        sum.N.m[0][1] += (w * pY->m[0][1]) >> FX32_SHIFT;
        sum.N.m[0][2] += (w * pY->m[0][2]) >> FX32_SHIFT;

        sum.N.m[1][0] += (w * pY->m[1][0]) >> FX32_SHIFT;
        sum.N.m[1][1] += (w * pY->m[1][1]) >> FX32_SHIFT;
        sum.N.m[1][2] += (w * pY->m[1][2]) >> FX32_SHIFT;

        sum.N.m[2][0] += (w * pY->m[2][0]) >> FX32_SHIFT;
        sum.N.m[2][1] += (w * pY->m[2][1]) >> FX32_SHIFT;
        sum.N.m[2][2] += (w * pY->m[2][2]) >> FX32_SHIFT;

        G3_LoadMtx43((const MtxFx43*)&sum.N); // This gets overwritten, so it's OK if the garbage data gets mixed up
        G3_MtxMode(GX_MTXMODE_POSITION);
        G3_LoadMtx43(&sum.M);
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_RestoreMtx(1);
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    }
#else
    {
        u32 i;
        struct
        {
            MtxFx43 M;
            MtxFx33 N;
        } sum;
        MtxFx44 X;
        MtxFx33 Y;

        MI_CpuClearFast(&sum, sizeof(sum));
        NNS_G3dGeFlushBuffer();

        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_StoreMtx(1);
        G3_Identity();
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

        for (i = 0; i < numMtx; ++i)
        {
            u32 idxJnt = *(p + 1);
            
            G3_RestoreMtx(*p);
            G3_MtxMode(GX_MTXMODE_POSITION);
            G3_MultMtx43(&evpMtx[idxJnt].invM);

            if (i != 0)
            {
                sum.N.m[0][0] += (w * Y.m[0][0]) >> FX32_SHIFT;
                sum.N.m[0][1] += (w * Y.m[0][1]) >> FX32_SHIFT;
                sum.N.m[0][2] += (w * Y.m[0][2]) >> FX32_SHIFT;

                sum.N.m[1][0] += (w * Y.m[1][0]) >> FX32_SHIFT;
                sum.N.m[1][1] += (w * Y.m[1][1]) >> FX32_SHIFT;
                sum.N.m[1][2] += (w * Y.m[1][2]) >> FX32_SHIFT;

                sum.N.m[2][0] += (w * Y.m[2][0]) >> FX32_SHIFT;
                sum.N.m[2][1] += (w * Y.m[2][1]) >> FX32_SHIFT;
                sum.N.m[2][2] += (w * Y.m[2][2]) >> FX32_SHIFT;
            }

            while (G3X_GetClipMtx(&X))
                ;
            G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
            G3_MultMtx33(&evpMtx[idxJnt].invN);

            w = *(p + 2) << 4;
            sum.M.m[0][0] += (w * X.m[0][0]) >> FX32_SHIFT;
            sum.M.m[0][1] += (w * X.m[0][1]) >> FX32_SHIFT;
            sum.M.m[0][2] += (w * X.m[0][2]) >> FX32_SHIFT;

            sum.M.m[1][0] += (w * X.m[1][0]) >> FX32_SHIFT;
            sum.M.m[1][1] += (w * X.m[1][1]) >> FX32_SHIFT;
            sum.M.m[1][2] += (w * X.m[1][2]) >> FX32_SHIFT;

            sum.M.m[2][0] += (w * X.m[2][0]) >> FX32_SHIFT;
            sum.M.m[2][1] += (w * X.m[2][1]) >> FX32_SHIFT;
            sum.M.m[2][2] += (w * X.m[2][2]) >> FX32_SHIFT;

            sum.M.m[3][0] += (w * X.m[3][0]) >> FX32_SHIFT;
            sum.M.m[3][1] += (w * X.m[3][1]) >> FX32_SHIFT;
            sum.M.m[3][2] += (w * X.m[3][2]) >> FX32_SHIFT;
            p += 3;

            while (G3X_GetVectorMtx(&Y))
                ;
        }
        sum.N.m[0][0] += (w * Y.m[0][0]) >> FX32_SHIFT;
        sum.N.m[0][1] += (w * Y.m[0][1]) >> FX32_SHIFT;
        sum.N.m[0][2] += (w * Y.m[0][2]) >> FX32_SHIFT;

        sum.N.m[1][0] += (w * Y.m[1][0]) >> FX32_SHIFT;
        sum.N.m[1][1] += (w * Y.m[1][1]) >> FX32_SHIFT;
        sum.N.m[1][2] += (w * Y.m[1][2]) >> FX32_SHIFT;

        sum.N.m[2][0] += (w * Y.m[2][0]) >> FX32_SHIFT;
        sum.N.m[2][1] += (w * Y.m[2][1]) >> FX32_SHIFT;
        sum.N.m[2][2] += (w * Y.m[2][2]) >> FX32_SHIFT;

        G3_LoadMtx43((const MtxFx43*)&sum.N); // This gets overwritten, so it's OK if the garbage data gets mixed up
        G3_MtxMode(GX_MTXMODE_POSITION);
        G3_LoadMtx43(&sum.M);
        G3_MtxMode(GX_MTXMODE_PROJECTION);
        G3_RestoreMtx(1);
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    }
#endif
    G3_StoreMtx(*(rs->c + 1));
    rs->c += 3 + *(rs->c + 2) * 3;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_CALLDL

    mnemonic:   NNS_G3D_SBC_CALLDL
    operands:   rel_addr(1 word), size(1 word)
    callbacks:  A: before
                B: none
                C: after
    Explanation of operations:
    - Calls the TIMING_A callback
    -  perform the display list transmission
    - Call the TIMING_C callback
    - Add rs->c

    Remarks:
    This command may be needed when first setting the matrices with NODEDESC, and implementing
    a conversion method that renders a simple model at high speed by calling CALLDL.
    This is one method for user hacking.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_CALLDL(NNSG3dRS* rs, u32)
{
    BOOL cbFlag;
    NNSG3dSbcCallBackTiming cbTiming;

    NNS_G3D_NULL_ASSERT(rs);

    // Callback A
    // If NNS_G3D_RSFLAG_SKIP is set internally, the
    // the display list send operations can be replaced
    cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_CALLDL, &cbTiming);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD) &&
        (!cbFlag))
    {
        u32 rel_addr;
        u32 size;
        rel_addr = (u32)( (*(rs->c + 1) << 0 ) |
                          (*(rs->c + 2) << 8 ) |
                          (*(rs->c + 3) << 16) |
                          (*(rs->c + 4) << 24) );

        size = (u32)( (*(rs->c + 5) << 0 ) |
                      (*(rs->c + 6) << 8 ) |
                      (*(rs->c + 7) << 16) |
                      (*(rs->c + 8) << 24) );

        NNS_G3dGeSendDL(rs->c + rel_addr, size);
    }

    // Callback C
    // You can insert any process before the next command.
    (void) NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_CALLDL, cbTiming);

    rs->c += 1 + sizeof(u32) + sizeof(u32);
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_POSSCALE([000], [001])

    mnemonic:   NNS_G3D_SBC_POSSCALE
    operands:   none
    callbacks:  A: none
                B: none
                C: none

    Explanation of operations:
    -   multiply the current matrix by the scaling matrix
       [000]  set the posScale inside the model data as an element and multiply by the scaling matrix
       [001]  set the invPosScale inside the model data as an element and multiply by the scaling matrix
    - Add rs->c

    Remarks:
    When the pos_scale of the imd file is non-zero, this command is output.
    It must be placed immediately before the render.
    It does not place a callback, since there is no deed for user attention.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_POSSCALE(NNSG3dRS* rs, u32 opt)
{
    VecFx32 s;
    NNS_G3D_NULL_ASSERT(rs);
    NNS_G3D_ASSERT(opt == NNS_G3D_SBCFLG_000 ||
                   opt == NNS_G3D_SBCFLG_001);

    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_NOGECMD) &&
        !(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW))
    {
        if (opt == NNS_G3D_SBCFLG_000)
        {
            s.x = s.y = s.z = rs->posScale;
        }
        else
        {
            // NNS_G3D_SBCFLG_001
            s.x = s.y = s.z = rs->invPosScale;
        }
        NNS_G3dGeBufferOP_N(G3OP_MTX_SCALE,
                            (u32*)&s.x,
                            G3OP_MTX_SCALE_NPARAMS);
    }

    rs->c += 1;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_ENVMAP

    mnemonic:   NNS_G3D_SBC_ENVMAP
    operands:   idxMat, flags (reserved)
    callbacks:  A: Customization for Normal -> texture coordinate mapping matrix
                B: Customization for effect_mtx settings
                C: Customization for normal transformation

    Explanation of operations:
    nrm * (C: normal transformation matrix) * (B: effect matrix set in .imd) *
          (A: mapping matrix) * (texture matrix set in material) 
    is the calculation performed. This calculation can be customized by setting your own callbacks to generate values A, B and C. 
     Under default conditions, mapping is performed in such a way that a single texture image is applied to both the foreground and background, respectively. 
      
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_ENVMAP(NNSG3dRS* rs, u32)
{
    NNS_G3D_NULL_ASSERT(rs);

    // If not VISIBLE the instruction will be skipped.
    // Even if the MatID status is the same, if the node is different you must recalculate.
    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW) &&
        (rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE))
    {
        BOOL cbFlag;
        NNSG3dSbcCallBackTiming cbTiming;

        // If texture SRI animation is attached,
        // because TEXGEN will become GX_TEXGEN_TEXCOORD,
        // it will reset to GX_TEXGEN_NORMAL.
        if ((rs->pMatAnmResult->prmTexImage & REG_G3_TEXIMAGE_PARAM_TGEN_MASK) !=
                (GX_TEXGEN_NORMAL << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT))
        {
            static u32 cmd[] =
            {
                G3OP_TEXIMAGE_PARAM,
                0
            };
            rs->pMatAnmResult->prmTexImage &= ~REG_G3_TEXIMAGE_PARAM_TGEN_MASK;
            rs->pMatAnmResult->prmTexImage |= GX_TEXGEN_NORMAL << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT;

            cmd[1] = rs->pMatAnmResult->prmTexImage;
            NNS_G3dGeBufferData_N(&cmd[0], 2);
        }

        // Sets the texture matrix and the operation target.
        NNS_G3dGeMtxMode(GX_MTXMODE_TEXTURE);

        // Callback A
        // If NNS_G3D_RSFLAG_SKIP is set internally, the
        // it is possible to overwrite the gMtx setting.
        cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_ENVMAP, &cbTiming);
        if (!cbFlag)
        {
            // Processes that can be customized with callback A
            s32 width, height;
            width = (s32)rs->pMatAnmResult->origWidth;
            height =(s32)rs->pMatAnmResult->origHeight;

            // NOTICE:
            // S,T coordinates cannot be not divided by q after transformation, therefore it will not be possible to implement Paraboloid Mapping.
            {
                //     0.5   0   0   0
                // m =  0  -0.5  0   0
                //      0    0   1   0
                //     0.5  0.5  0   1

                // Map so that one texture each is mapped respectively to the front and back faces.
                NNS_G3dGeScale(width << (FX32_SHIFT + 3), -height << (FX32_SHIFT + 3), FX32_ONE << 4);
                NNS_G3dGeTexCoord(width << (FX32_SHIFT - 1), height << (FX32_SHIFT - 1));
            }
        }

        // Callback B
        // If NNS_G3D_RSFLAG_SKIP is set internally, the
        // it is possible to overwrite the effect_mtx setting.
        cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_ENVMAP, &cbTiming);
        if (!cbFlag)
        {
            u32 idxMat = *(rs->c + 1);
            const NNSG3dResMatData* mat =
                NNS_G3dGetMatDataByIdx(rs->pResMat, idxMat);

            // If effect_mtx exists in a NITRO intermediate file,
            // applies effect_mtx
            if (mat->flag & NNS_G3D_MATFLAG_EFFECTMTX)
            {
                // If effect_mtx exists in a NITRO intermediate file,
                const MtxFx44* effect_mtx;
                const u8* p = (const u8*)mat + sizeof(NNSG3dResMatData);

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_SCALEONE))
                {
                    p += sizeof(fx32) + sizeof(fx32); // scaleS, scaleT
                }

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_ROTZERO))
                {
                    p += sizeof(fx16) + sizeof(fx16); // rotSin, rotCos
                }

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_TRANSZERO))
                {
                    p += sizeof(fx32) + sizeof(fx32); // transS, transT
                }

                effect_mtx = (const MtxFx44*)p;
                NNS_G3dGeMultMtx44(effect_mtx);
            }
        }

        // Callback C
        // If NNS_G3D_RSFLAG_SKIP is set internally, the
        // you can overwrite the input normal vector's transformation matrix.
        cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_ENVMAP, cbTiming);
        if (!cbFlag)
        {
            MtxFx33 n;
            NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
            NNS_G3dGetCurrentMtx(NULL, &n);
            NNS_G3dGeMtxMode(GX_MTXMODE_TEXTURE);

            //
            // Transforms the input normal to world coordinate system directions.
            // The normal vector that is input has the same direction as the geometry command (joint coordinate system).
            //
            if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
            {
                // In the case of NNS_G3dGlbFlushWVP,
                // joint coordinate system -> local coordinate system -> view coordinate system
                // conversions are performed.
                NNS_G3dGeMultMtx33((const MtxFx33*)NNS_G3dGlbGetCameraMtx()); // World coordinate system -> view coordinate system
                NNS_G3dGeMultMtx33(NNS_G3dGlbGetBaseRot());                   // Local coordinate system -> world coordinate system
                NNS_G3dGeMultMtx33(&n);                                       // Joint coordinate system -> local coordinate system

            }
            else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
            {
                // In the case of NNS_G3dGlbFlushVP,
                // it will become the world coordinate system directional vector as-is.
                NNS_G3dGeMultMtx33((const MtxFx33*)NNS_G3dGlbGetCameraMtx()); // World coordinate system -> view coordinate system
                NNS_G3dGeMultMtx33(&n);                                       // Joint coordinate system -> world coordinate system
            }
            else
            {
                NNS_G3dGeMultMtx33(&n);                            // Joint coordinate system -> view coordinate system
            }
        }

        // Returns the operation target matrix to its origin.
        NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
    }
    rs->c += 3;
}


/*---------------------------------------------------------------------------*
    NNSi_G3dFuncSbc_PRJMAP

    mnemonic:   NNS_G3D_SBC_PRJMAP([000], [001], [010])
    operands:   idxMat
    callbacks:  A: Customization for vertex coordinate -> texture coordinate mapping matrix.
                B: Customization for effect_mtx settings
                C: Customization for vertex coordinate transformation.

    Explanation of operations:
    Since it must extract values from a calculated texture matrix and set them in TexCoord,
    it calculates using a position coordinate matrix from which it can extract.
    After that it extracts and sets to the texture matrix and TexCoord.
    The texture matrix that was set by the MAT command will be overwritten, and become invalid.
 *---------------------------------------------------------------------------*/
void
NNSi_G3dFuncSbc_PRJMAP(NNSG3dRS* rs, u32 )
{
    //
    // TexCoord must be set for part of the texture matrix.
    // Therefore, extraction is done after NNS_G3D_MTXSTACK_SYS is used for calculation.
    //

    NNS_G3D_NULL_ASSERT(rs);

    // If not VISIBLE the instruction will be skipped.
    // Even if the MatID status is the same, if the node is different you must recalculate.
    if (!(rs->flag & NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW) &&
        (rs->flag & NNS_G3D_RSFLAG_NODE_VISIBLE))
    {
        BOOL cbFlag;
        NNSG3dSbcCallBackTiming cbTiming;

        // Extracts a current position coordinate matrix that will be used later.
        MtxFx43 m;
        NNS_G3dGetCurrentMtx(&m, NULL);
        NNS_G3dGeStoreMtx(NNS_G3D_MTXSTACK_SYS);

        // If texture SRI animation is attached,
        // because TEXGEN will become GX_TEXGEN_TEXCOORD,
        // it will be reset to GX_TEXGEN_VERTEX.
        if ((rs->pMatAnmResult->prmTexImage & REG_G3_TEXIMAGE_PARAM_TGEN_MASK) !=
                (GX_TEXGEN_VERTEX << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT))
        {
            static u32 cmd[] =
            {
                G3OP_TEXIMAGE_PARAM,
                0
            };
            rs->pMatAnmResult->prmTexImage &= ~REG_G3_TEXIMAGE_PARAM_TGEN_MASK;
            rs->pMatAnmResult->prmTexImage |= GX_TEXGEN_VERTEX << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT;

            cmd[1] = rs->pMatAnmResult->prmTexImage;
            NNS_G3dGeBufferData_N(&cmd[0], 2);
        }

        // Callback A
        // If NNS_G3D_RSFLAG_SKIP is set internally, the
        // it is possible to overwrite the gMtx setting.
        cbFlag = NNSi_G3dCallBackCheck_A(rs, NNS_G3D_SBC_PRJMAP, &cbTiming);
        if (!cbFlag)
        {
            // Processes that can be customized with callback A
            s32 width, height;
            width = (s32)rs->pMatAnmResult->origWidth;
            height = (s32)rs->pMatAnmResult->origHeight;

            {
                static MtxFx44 mtx = {
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, FX32_ONE << 4, 0,
                    0, 0, 0, FX32_ONE << 4
                };

                mtx._00 = width << (FX32_SHIFT + 3);
                mtx._11 = -height << (FX32_SHIFT + 3);
                mtx._30 = width << (FX32_SHIFT + 3);
                mtx._31 = height << (FX32_SHIFT + 3);

                //        0.5   0   0   0
                // mtx =   0  -0.5  0   0
                //         0    0   1   0
                //        0.5  0.5  0   1
                NNS_G3dGeLoadMtx44(&mtx);
            }
        }

        // Callback B
        // If NNS_G3D_RSFLAG_SKIP is set internally, the
        // it is possible to overwrite the effect_mtx setting.
        cbFlag = NNSi_G3dCallBackCheck_B(rs, NNS_G3D_SBC_PRJMAP, &cbTiming);
        if (!cbFlag)
        {
            u32 idxMat = *(rs->c + 1);
            const NNSG3dResMatData* mat =
                NNS_G3dGetMatDataByIdx(rs->pResMat, idxMat);

            // If effect_mtx exists in a NITRO intermediate file,
            // applies effect_mtx
            if (mat->flag & NNS_G3D_MATFLAG_EFFECTMTX)
            {
                // If effect_mtx exists in a NITRO intermediate file,
                const MtxFx44* effect_mtx;
                const u8* p = (const u8*)mat + sizeof(NNSG3dResMatData);

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_SCALEONE))
                {
                    p += sizeof(fx32) + sizeof(fx32); // scaleS, scaleT
                }

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_ROTZERO))
                {
                    p += sizeof(fx16) + sizeof(fx16); // rotSin, rotCos
                }

                if (!(mat->flag & NNS_G3D_MATFLAG_TEXMTX_TRANSZERO))
                {
                    p += sizeof(fx32) + sizeof(fx32); // transS, transT
                }

                effect_mtx = (const MtxFx44*)p;
                NNS_G3dGeMultMtx44(effect_mtx);
            }
        }

        // Callback C
        // If NNS_G3D_RSFLAG_SKIP is set internally, the
        // you can overwrite the input normal vector's transformation matrix.
        cbFlag = NNSi_G3dCallBackCheck_C(rs, NNS_G3D_SBC_PRJMAP, cbTiming);
        if (!cbFlag)
        {
            MtxFx44 tex_mtx;

            //
            // Transforms the input coordinates to world coordinate system coordinates.
            // The normal vector that is input has the same direction as the geometry command (joint coordinate system).
            //
            if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_WVP)
            {
                // In the case of NNS_G3dGlbFlushWVP,
                // joint coordinate system -> local coordinate system -> world coordinate system
                // conversions are performed.
                NNS_G3dGeTranslateVec(NNS_G3dGlbGetBaseTrans());
                NNS_G3dGeMultMtx33(NNS_G3dGlbGetBaseRot());  // Local coordinate system -> world coordinate system
                NNS_G3dGeMultMtx43(&m);                      // Joint coordinate system -> local coordinate system

            }
            else if (NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_FLUSH_VP)
            {
                // In the case of NNS_G3dGlbFlushVP,
                // it will become the world coordinate system directional vector as-is.
                NNS_G3dGeMultMtx43(&m);                      // Joint coordinate system -> world coordinate system
            }
            else
            {
                // In the case of NNS_G3dGlbFlushP,
                // joint coordinate system -> camera coordinate system -> world coordinate system
                // conversions are performed.
                NNS_G3dGeMultMtx43(NNS_G3dGlbGetInvV()); // Camera coordinate system -> world coordinate system
                NNS_G3dGeMultMtx43(&m);                            // Joint coordinate system -> camera coordinate system
            }

            //
            // Reads back the calculated texture matrix from the position coordinate matrix stack.
            //
            {
                NNS_G3dGeFlushBuffer();

                G3_MtxMode(GX_MTXMODE_PROJECTION);
                G3_PushMtx();
                G3_Identity();

                while(G3X_GetClipMtx(&tex_mtx))
                    ;

                G3_PopMtx(1);
                G3_MtxMode(GX_MTXMODE_TEXTURE);
            }

            NNS_G3dGeLoadMtx44(&tex_mtx);
            NNS_G3dGeTexCoord(tex_mtx._30 >> 4, tex_mtx._31 >> 4);
        }

        // Returns the operation target matrix to its origin.
        NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
        NNS_G3dGeRestoreMtx(NNS_G3D_MTXSTACK_SYS);
    }
    rs->c += 3;
}




////////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//

/*---------------------------------------------------------------------------*
    NNS_G3dRS

    This is the pointer to the structure that holds the state held by the program (the state machine state)
    during rendering. Memory for the structure is allocated in the stack (DTCM) in NNS_G3dDraw.
     NNS_G3dRS is cleaned up with a NULL when NNS_G3dDraw ends.
 *---------------------------------------------------------------------------*/
NNSG3dRS* NNS_G3dRS = NULL;


/*---------------------------------------------------------------------------*
    NNS_G3dRSOnGlb

    This is the state held by the program (the state machine state) during rendering,
    and since the size is so large, this is not placed in the stack region.
 *---------------------------------------------------------------------------*/
NNSG3dRSOnGlb NNS_G3dRSOnGlb;


/*---------------------------------------------------------------------------*
    NNS_G3dFuncSbcTable

    The SBC code handler is registered.
    This is not a const because the user can re-write it.
 *---------------------------------------------------------------------------*/
NNSG3dFuncSbc NNS_G3dFuncSbcTable[NNS_G3D_SBC_COMMAND_NUM] =
{
    &NNSi_G3dFuncSbc_NOP,
    &NNSi_G3dFuncSbc_RET,
    &NNSi_G3dFuncSbc_NODE,
    &NNSi_G3dFuncSbc_MTX,
    &NNSi_G3dFuncSbc_MAT,
    &NNSi_G3dFuncSbc_SHP,
    &NNSi_G3dFuncSbc_NODEDESC,
    &NNSi_G3dFuncSbc_BB,

    &NNSi_G3dFuncSbc_BBY,
    &NNSi_G3dFuncSbc_NODEMIX,
    &NNSi_G3dFuncSbc_CALLDL,
    &NNSi_G3dFuncSbc_POSSCALE,
    &NNSi_G3dFuncSbc_ENVMAP,
    &NNSi_G3dFuncSbc_PRJMAP
};


/*---------------------------------------------------------------------------*
    NNS_G3dFuncSbcShpTable

    Individual tags at the top of the shape data are indexed to this table.
    Register the handler according to material data type.
 *---------------------------------------------------------------------------*/
NNSG3dFuncSbc_ShpInternal NNS_G3dFuncSbcShpTable[NNS_G3D_SIZE_SHP_VTBL_NUM] =
{
    &NNSi_G3dFuncSbc_SHP_InternalDefault
};


/*---------------------------------------------------------------------------*
    NNS_G3dFuncSbcMatTable

    Individual tags at the top of the material data are indexed to this table.
    Register the handler according to material data type.
 *---------------------------------------------------------------------------*/
NNSG3dFuncSbc_MatInternal NNS_G3dFuncSbcMatTable[NNS_G3D_SIZE_MAT_VTBL_NUM] =
{
    &NNSi_G3dFuncSbc_MAT_InternalDefault
};

