/*---------------------------------------------------------------------------*
  Project:  NitroSystem - G3D(Hokotate) -
  File:     sbc.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: sbc.h,v $
  Revision 1.21  12/21/2004 08:26:54  nishida_kenji
  supports env map & proj map.

  Revision 1.20  10/28/2004 05:20:06  nishida_kenji
  NNSi_G3dFuncSbc_NODEMIX gets faster.

  Revision 1.19  10/21/2004 02:15:58  nishida_kenji
  Add NNS_G3dRSGetCurrentNodeDescID.

  Revision 1.18  10/20/2004 01:31:06  nishida_kenji
  Add accessors for NNSG3dRS.

  Revision 1.17  09/27/2004 09:57:20  nishida_kenji
  Add callback vector and accessors to NNSG3dRS.
  Remove NNSG3dRS::cbFunc, cbCmdPtr, cbCmd, cbTiming.

  Revision 1.16  09/16/2004 02:47:17  nishida_kenji
  reduce overhead of callbacks.

  Revision 1.15  07/27/2004 05:00:32  nishida_kenji
  NNSG3dRS::flag becomes u32.

  Revision 1.14  07/27/2004 04:40:38  nishida_kenji
  Add NNS_G3D_INLINE to prototypes.

  Revision 1.13  07/21/2004 06:31:20  nishida_kenji
  add pVisAnmResult for callback functions.

  Revision 1.12  07/20/2004 10:45:20  nishida_kenji
  fixed comments

  Revision 1.11  07/20/2004 07:26:36  nishida_kenji
  move NNSG3dSbcCallBackTiming to kernel.h

  Revision 1.10  07/17/2004 12:56:57  nishida_kenji
  NODEDESC_BB -> BB
  NODEDESC_BBY -> BBY
  callback condition added

  Revision 1.9  07/14/2004 11:32:35  nishida_kenji
  change sbc function name.
  revise runtime flags

  Revision 1.8  07/13/2004 07:17:41  nishida_kenji
  fix glitches on sbc's callback.

  Revision 1.7  07/13/2004 01:02:29  nishida_kenji
  Add NNS_G3D_RENDEROBJ_FLAG_NOGECMD
        NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW
        NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC.

  Revision 1.6  07/12/2004 12:50:59  nishida_kenji
  refactoring

  Revision 1.5  07/12/2004 06:44:36  nishida_kenji
  revise callback interface.

  Revision 1.4  07/10/2004 08:46:57  nishida_kenji
  (none)

  Revision 1.3  07/09/2004 07:18:20  nishida_kenji
  (none)

  Revision 1.2  07/08/2004 12:46:33  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:39:44  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_SBC_H_
#define NNSG3D_SBC_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/kernel.h>
#include <nnsys/g3d/anm.h>
#include <nnsys/g3d/cgtool.h>

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//
// Definition of the structure and typedef
//

/*---------------------------------------------------------------------------*
    NNSG3dRSFlag

    A collection of 1-bit flags inside NNSG3dRS.
    They are set and referenced inside functions such as NNS_G3dDraw and the SBC command function.

    If NNS_G3D_RSFLAG_SKIP is configured inside the callback, the operations outside the
    callback judgment will be skipped until the end of that SBC command.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RSFLAG_NODE_VISIBLE           = 0x00000001,    // Turned ON and OFF by the NODE command
    NNS_G3D_RSFLAG_MAT_TRANSPARENT        = 0x00000002,    // Turned ON and OFF by the MAT command
    NNS_G3D_RSFLAG_CURRENT_NODE_VALID     = 0x00000004,    // Turned ON by the NODE command
    NNS_G3D_RSFLAG_CURRENT_MAT_VALID      = 0x00000008,    // Turned ON by the MAT command
    NNS_G3D_RSFLAG_CURRENT_NODEDESC_VALID = 0x00000010,
    NNS_G3D_RSFLAG_RETURN                 = 0x00000020,    // Turned ON by the RET command
    NNS_G3D_RSFLAG_SKIP                   = 0x00000040,    // User flag for callbacks (skips to the next callback in the command).

    // Below are the pre-execution values determined by the NNSG3dRenderObj
    NNS_G3D_RSFLAG_OPT_RECORD             = 0x00000080,    // Stores calculation results in NNSG3dRenderObj > recJntAnm, recMatAnm.
    NNS_G3D_RSFLAG_OPT_NOGECMD            = 0x00000100,    // Does not send geometry commands.
    NNS_G3D_RSFLAG_OPT_SKIP_SBCDRAW       = 0x00000200,    // Skips Draw type SBC commands. Also skips callbacks.
    NNS_G3D_RSFLAG_OPT_SKIP_SBCMTXCALC    = 0x00000400     // Skips MtxCalc type SBC commands. Also skips callbacks.

    // An overview of the Draw-related SBC commands
    // NODE         --- NNSi_G3dFuncSbc_NODE
    // MTX          --- NNSi_G3dFuncSbc_MTX
    // MAT          --- NNSi_G3dFuncSbc_MAT
    // SHP          --- NNSi_G3dFuncSbc_SHP
    // NODEDESC_BB  --- NNSi_G3dFuncSbc_BB
    // NODEDESC_BBY --- NNSi_G3dFuncSbc_BBY
    // POSSCALE     --- NNSi_G3dFuncSbc_POSSCALE

    // An overview of the MtxCalc-related SBC commands
    // NODEDESC     --- NNSi_G3dFuncSbc_NODEDESC
}
NNSG3dRSFlag;


/*---------------------------------------------------------------------------*
    NNSG3dRS

    This is used in storing the state of the rendering engine. Be aware that normally it is allocated to
    the stack so the DTCM can be used. Stores the variables, etc. that perform the exchange. A pointer is
    stored in NNS_G3dRS.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dRS_
{
    // Base information
    u8*                               c;                   // Pointer to the currently referenced SBC command.
    NNSG3dRenderObj*                  pRenderObj;
    u32                               flag;                // NNSG3dRSFlag

    // Callback function vector
    NNSG3dSbcCallBackFunc             cbVecFunc[NNS_G3D_SBC_COMMAND_NUM];   // Callback function vector
    u8                                cbVecTiming[NNS_G3D_SBC_COMMAND_NUM]; // NNSG3dSbcCallBackTiming vector

    // When the NNS_G3D_RSFLAG_CURRENT_NODE_VALID flag is ON, the nodeID of
    // the previously called NODE command (the one that is currently executing) is stored.
    u8                                currentNode;

    // When the NNS_G3D_RSFLAG_CURRENT_MAT_VALID flag is ON, the material ID of
    // the previously called MAT command (the one that is currently executing) is stored.
    // If the material ID specified by the MAT command is the same, sending material information is omitted..
    // Therefore, when a callback or the like changes material data
    // you must turn NNS_G3D_RSFLAG_CURRENT_MAT_VALID OFF.
    u8                                currentMat;

    // When the NNS_G3D_RSFLAG_CURRENT_NODEDESC_VALID flag is ON, the nodeID of the
    // previously called NODEDESC command (the one that is currently executing) is stored.
    u8                                currentNodeDesc;

    u8                                dummy_;
    // If NNSG3dMatAnmResult,NNSG3dJntAnmResult,NNSG3dVisAnmResult are currently being calculated
    // a pointer will be stored in this member.
    // You can now change calculation results with a callback.

    // NULL until the first MAT command is called
    // After that, stores the calculation result of MAT command executed immediately before.
    NNSG3dMatAnmResult*               pMatAnmResult;

    // NULL until the first NODEDESC command is called.
    // After that, stores the calculation result of NODEDESC command executed immediately before.
    NNSG3dJntAnmResult*               pJntAnmResult;

    // NULL until the first NODE command is called.
    // After that, stores the calculation result of NODE command executed immediately before
    NNSG3dVisAnmResult*               pVisAnmResult;

    // The bit will be set if material data is already cached.
    // The body of the data will be stored in matCache in NNSG3dRSOnHeap.
    u32                               isMatCached[NNS_G3D_SIZE_MAT_MAX / 32];

    // When saving data related to scaling, if data is (1.0, 1.0, 1.0),
    // instead of storing the data you can turn ON the corresponding bits.
    u32                               isScaleCacheOne[NNS_G3D_SIZE_JNT_MAX / 32];

#if (NNS_G3D_USE_EVPCACHE)
    u32                               isEvpCached[NNS_G3D_SIZE_JNT_MAX / 32];
#endif

    // Caches information that is repeatedly used in resMdl.
    // The user does not need to touch this.
    const NNSG3dResNodeInfo*          pResNodeInfo;
    const NNSG3dResMat*               pResMat;
    const NNSG3dResShp*               pResShp;
    fx32                              posScale;
    fx32                              invPosScale;
    NNSG3dGetJointScale               funcJntScale;
    NNSG3dSendJointSRT                funcJntMtx;
    NNSG3dSendTexSRT                  funcTexMtx;

    NNSG3dMatAnmResult                tmpMatAnmResult;
    NNSG3dJntAnmResult                tmpJntAnmResult;
    NNSG3dVisAnmResult                tmpVisAnmResult;
}
NNSG3dRS;


/*---------------------------------------------------------------------------*
    NNSG3dRSOnGlb

    Among the states of the rendering engine, this stores those which are not suitable to be put in the
    stack due to their large size. It uses the global variable NNS_G3dRSOnGlb.
    It remains in the state as allocated in the main memory from start to finish.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dRSOnGlb_
{
    //
    // The material information cached by the SBC MAT command.
    // The data is valid if the bit corresponding to the isMatChached of NNSG3dRS is ON.
    //
    struct NNSG3dMatAnmResult_ matCache[NNS_G3D_SIZE_MAT_MAX];

    //
    // For the Maya Segment Scale Compensate and the SI3D Classic Scale Off,
    // the scale related data that corresponds to the joint is there for storage.
    // It is written by the NODEDESC command of the SBC.
    // For Maya SSC:
    // The reciprocal of the scale goes into the flag argument of NODEDESC
    // when NNS_G3D_SBC_NODEDESC_FLAG_MAYASSC_PARENT is ON.
    //
    // For SI3D Classic Scale Off:
    // When the NNS_G3D_SCALINGRULE_SI3D is designated,
    // the scale exponent up to the parent is stored.
    // 
    // When the bit that corresponds to the isScaleCacheOne of NNSG3dRS is OFF,
    // a value is stored only when the scale is not 1.0.
    //
    struct
    {
        VecFx32 s;
        VecFx32 inv;
    }
    scaleCache[NNS_G3D_SIZE_JNT_MAX];

#if (NNS_G3D_USE_EVPCACHE)
    struct
    {
        MtxFx44 M;
        MtxFx33 N;
    }
    evpCache[NNS_G3D_SIZE_JNT_MAX];
#endif
}
NNSG3dRSOnGlb;


/*---------------------------------------------------------------------------*
    NNSG3dFuncSbc

    The pointer format of the SBC command functions
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dFuncSbc)(NNSG3dRS*, u32);


/*---------------------------------------------------------------------------*
    NNSG3dFuncSbc_[Mat|Shp]Internal

    The pointer format of the functions inside the MAT/SHP commands of the SBC.
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dFuncSbc_MatInternal)(NNSG3dRS*, u32, const NNSG3dResMatData*, u32);
typedef void (*NNSG3dFuncSbc_ShpInternal)(NNSG3dRS*, u32, const NNSG3dResShpData*, u32);


/*---------------------------------------------------------------------------*
    Matrix stack index definition not used in the code generated by g3dcvtr
    NNS_G3D_MTXSTACK_SYS is reserved inside G3D, and
    NNS_G3D_MTXSTACK_USER can be used by user.
 *---------------------------------------------------------------------------*/

#define NNS_G3D_MTXSTACK_SYS  (30)
#define NNS_G3D_MTXSTACK_USER (29)


////////////////////////////////////////////////////////////////////////////////
//
// Declaration of Functions
//

//
// accessor for NNS_G3dRS
//
NNS_G3D_INLINE void NNS_G3dRSSetCallBack(NNSG3dRS* rs, NNSG3dSbcCallBackFunc func, u8 cmd, NNSG3dSbcCallBackTiming timing);
NNS_G3D_INLINE void NNS_G3dRSResetCallBack(NNSG3dRS* rs, u8 cmd);
NNS_G3D_INLINE NNSG3dRenderObj* NNS_G3dRSGetRenderObj(NNSG3dRS* rs);
NNS_G3D_INLINE NNSG3dMatAnmResult* NNS_G3dRSGetMatAnmResult(NNSG3dRS* rs);
NNS_G3D_INLINE NNSG3dJntAnmResult* NNS_G3dRSGetJntAnmResult(NNSG3dRS* rs);
NNS_G3D_INLINE NNSG3dVisAnmResult* NNS_G3dRSGetVisAnmResult(NNSG3dRS* rs);
NNS_G3D_INLINE u8* NNS_G3dRSGetSbcPtr(NNSG3dRS* rs);
NNS_G3D_INLINE void NNS_G3dRSSetFlag(NNSG3dRS* rs, NNSG3dRSFlag flag);
NNS_G3D_INLINE void NNS_G3dRSResetFlag(NNSG3dRS* rs, NNSG3dRSFlag flag);
NNS_G3D_INLINE int NNS_G3dRSGetCurrentMatID(const NNSG3dRS* rs);
NNS_G3D_INLINE int NNS_G3dRSGetCurrentNodeID(const NNSG3dRS* rs);
NNS_G3D_INLINE int NNS_G3dRSGetCurrentNodeDescID(const NNSG3dRS* rs);
NNS_G3D_INLINE fx32 NNS_G3dRSGetPosScale(const NNSG3dRS* rs);
NNS_G3D_INLINE fx32 NNS_G3dRSGetInvPosScale(const NNSG3dRS* rs);



//
// Drawing
//
void NNS_G3dDraw(struct NNSG3dRenderObj_* pRenderObj);

//
// SBC instructions
//
void NNSi_G3dFuncSbc_NOP(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_RET(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_NODE(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_MTX(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_MAT(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_SHP(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_NODEDESC(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_BB(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_BBY(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_NODEMIX(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_CALLDL(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_POSSCALE(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_ENVMAP(NNSG3dRS*, u32);
void NNSi_G3dFuncSbc_PRJMAP(NNSG3dRS*, u32);


// Operation when MAT/SHP itemTag is set to 0.
void NNSi_G3dFuncSbc_SHP_InternalDefault(NNSG3dRS* rs,
                                        u32 opt,
                                        const NNSG3dResShpData* shp,
                                        u32 idxShp);

void NNSi_G3dFuncSbc_MAT_InternalDefault(NNSG3dRS* rs,
                                        u32 opt,
                                        const NNSG3dResMatData* mat,
                                        u32 idxMat);

//
// Checking callbacks
//
NNS_G3D_INLINE BOOL NNSi_G3dCallBackCheck_A(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming* pTiming);
NNS_G3D_INLINE BOOL NNSi_G3dCallBackCheck_B(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming* pTiming);
NNS_G3D_INLINE BOOL NNSi_G3dCallBackCheck_C(NNSG3dRS* rs, u8 cmd, NNSG3dSbcCallBackTiming timing);
NNS_G3D_INLINE NNSG3dSbcCallBackTiming NNSi_CheckPossibilityOfCallBack(NNSG3dRS* rs, u8 cmd);

////////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//
extern NNSG3dFuncSbc NNS_G3dFuncSbcTable[NNS_G3D_SBC_COMMAND_NUM];
extern NNSG3dFuncSbc_MatInternal NNS_G3dFuncSbcMatTable[NNS_G3D_SIZE_MAT_VTBL_NUM];
extern NNSG3dFuncSbc_ShpInternal NNS_G3dFuncSbcShpTable[NNS_G3D_SIZE_SHP_VTBL_NUM];
extern NNSG3dRS* NNS_G3dRS;
extern NNSG3dRSOnGlb NNS_G3dRSOnGlb;



#ifdef __cplusplus
}/* extern "C"*/
#endif

#include <nnsys/g3d/sbc_inline.h>

#endif

