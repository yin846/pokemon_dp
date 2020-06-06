/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     kernel.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: kernel.h,v $
  Revision 1.31  11/09/2004 02:15:32  nishida_kenji
  fix comments.

  Revision 1.30  10/20/2004 01:30:42  nishida_kenji
  Add accessors for NNSG3dRenderObj.

  Revision 1.29  10/18/2004 10:54:54  nishida_kenji
  small fix.

  Revision 1.28  10/18/2004 09:00:12  nishida_kenji
  small fix.

  Revision 1.27  10/18/2004 08:12:50  nishida_kenji
  Add NNS_G3dAnmObjEnableID and NNS_G3dAnmObjDisableID.

  Revision 1.26  10/13/2004 09:14:40  nishida_kenji
  small fix(const correctness).

  Revision 1.25  09/27/2004 09:55:36  nishida_kenji
  Add NNS_G3dRenderObjSetInitFunc,
  remove NNSG3dRenderObj::cbCmdPtr.

  Revision 1.24  09/01/2004 01:32:00  nishida_kenji
  small fix.

  Revision 1.23  08/26/2004 07:18:45  nishida_kenji
  Add NNS_G3D_ANMOBJ_SIZE_MATANM, NNS_G3D_ANMOBJ_SIZE_JNTANM, and NNS_G3D_ANMOBJ_SIZE_VISANM

  Revision 1.22  08/03/2004 04:26:35  nishida_kenji
  Add NNS_G3dAnmObjGetNumFrame.

  Revision 1.21  07/27/2004 23:44:58  nishida_kenji
  Add padding to suppress warnings.

  Revision 1.20  07/27/2004 05:00:16  nishida_kenji
  NNSG3dRenderObj::flag becomes u32.

  Revision 1.19  07/27/2004 04:40:44  nishida_kenji
  Add NNS_G3D_INLINE to prototypes.

  Revision 1.18  07/26/2004 02:22:23  nishida_kenji
  add comments

  Revision 1.17  07/21/2004 06:31:43  nishida_kenji
  Add NNS_G3dRenderObjResetCallBack

  Revision 1.16  07/21/2004 02:17:43  nishida_kenji
  add APIs for RenderObj's flag

  Revision 1.15  07/20/2004 10:44:50  nishida_kenji
  add APIs, etc.

  Revision 1.14  07/20/2004 07:57:44  nishida_kenji
  add ptrUser to NNSG3dRenderObj

  Revision 1.13  07/20/2004 07:26:15  nishida_kenji
  Add an API for callback.

  Revision 1.12  07/20/2004 01:30:52  nishida_kenji
  add comments

  Revision 1.11  07/19/2004 10:53:51  nishida_kenji
  NNS_G3dAnmObjInit takes ptr to resTex

  Revision 1.10  07/17/2004 12:56:11  nishida_kenji
  Add NNS_G3dBindMdlSet, NNS_G3dReleaseMdlSet

  Revision 1.9  07/16/2004 07:24:07  nishida_kenji
  Add NNS_G3dForceBindMdl[Tex|Pltt].

  Revision 1.7  07/14/2004 11:33:03  nishida_kenji
  Add functions to setup textures.

  Revision 1.5  07/13/2004 01:02:24  nishida_kenji
  Add NNS_G3D_RENDEROBJ_FLAG_NOGECMD
        NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW
        NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC.

  Revision 1.1  07/07/2004 12:39:44  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_KERNEL_H_
#define NNSG3D_KERNEL_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/binres/res_struct_accessor_anm.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Definition of the structure and typedef
//

#define NNS_G3D_SIZE_MAT_MAX_MAX 256
#define NNS_G3D_SIZE_JNT_MAX_MAX 256
#define NNS_G3D_SIZE_SHP_MAX_MAX 256

#if (NNS_G3D_SIZE_MAT_MAX <= 0 || NNS_G3D_SIZE_MAT_MAX > NNS_G3D_SIZE_MAT_MAX_MAX)
#error NNS_G3D_SIZE_MAT_MAX range error.
#endif

#if (NNS_G3D_SIZE_JNT_MAX <= 0 || NNS_G3D_SIZE_JNT_MAX > NNS_G3D_SIZE_JNT_MAX_MAX)
#error NNS_G3D_SIZE_JNT_MAX range error.
#endif

#if (NNS_G3D_SIZE_SHP_MAX <= 0 || NNS_G3D_SIZE_SHP_MAX > NNS_G3D_SIZE_SHP_MAX_MAX)
#error NNS_G3D_SIZE_SHP_MAX range error.
#endif

#if (NNS_G3D_SIZE_MAT_MAX % 32 != 0)
#error NNS_G3D_SIZE_MAT_MAX must be a multiple of 32.
#endif

#if (NNS_G3D_SIZE_JNT_MAX % 32 != 0)
#error NNS_G3D_SIZE_JNT_MAX must be a multiple of 32.
#endif

#if (NNS_G3D_SIZE_SHP_MAX % 32 != 0)
#error NNS_G3D_SIZE_SHP_MAX must be a multiple of 32.
#endif

typedef u32 NNSG3dTexKey;    // compatible with NNSGfdTexKey
typedef u32 NNSG3dPlttKey;   // compatible with NNSGfdPlttKey


/*---------------------------------------------------------------------------*
    NNSG3dAnmObj

    The structure that is referenced by NNSG3dRenderObj. The user must allocate and free memory on his or her own.
     The NNS_G3dAnmObjInit function is used to initialize.
    The purpose of this structure is to
    - Specify pairing for animation resources and the functions that process them;
    - Associate animation resources and model resources; and
    - Maintain the current animation frame.

    frame:      Designates which frame to play
    ratio:      Used by the animation  blend  function
    resAnm:     The pointers to the individual animation resources
    funcAnm:    The pointer to the function that plays the frame in the resAnm frame-numbered position
    next:       Configured by NNS_G3dRenderObjBindAnmObj and
                NNS_G3dRenderObjReleaseAnmObj
    resTex:     The pointer to the texture resource. Only configured when animation
                NNS_G3dRenderObjReleaseAnmObj
    priority:   The priority when NNSG3dRenderObj is registered
    numMapData: A table of number of entries that take the index correspondence of the model resource
                and the animation resource
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dAnmObj_
{
    fx32                  frame;
    fx32                  ratio;
    void*                 resAnm;     // The pointer to the animation data block inside the resource  file
    void*                 funcAnm;    // Cast in the function pointer of each animation. Takes the default, but can be changed.
    struct NNSG3dAnmObj_* next;
    const NNSG3dResTex*   resTex;     // When the information about the texture  block is needed (the texture pattern animation only)
    u8                    priority;
    u8                    numMapData;
    u16                   mapData[1]; // Becomes numMapData arrays (NNSG3dAnmObjMapData)
}
NNSG3dAnmObj;

//
// The size of the memory needed by NNSG3dAnmObj is determined from the categories of the model resource and the animation.
// For material animation, use the macro below.
// In actuality, it is sizeof(NNSG3dAnmObj) + sizeof(u16) * (pMdl->info.numMat - 1)
// but it uses a 4 byte boundary.
//
#define NNS_G3D_ANMOBJ_SIZE_MATANM(pMdl) ((sizeof(NNSG3dAnmObj) + sizeof(u16) * pMdl->info.numMat) & ~3)

//
// The size of the memory needed by NNSG3dAnmObj is determined from the categories of the model resource and the animation.
// Use the macro below for joint animation and visibility animation.
// In actuality, it is sizeof(NNSG3dAnmObj) + sizeof(u16) * (pMdl->info.numNode - 1)
// but it uses a 4 byte boundary.
//
#define NNS_G3D_ANMOBJ_SIZE_JNTANM(pMdl) ((sizeof(NNSG3dAnmObj) + sizeof(u16) * pMdl->info.numNode) & ~3)
#define NNS_G3D_ANMOBJ_SIZE_VISANM(pMdl) ((sizeof(NNSG3dAnmObj) + sizeof(u16) * pMdl->info.numNode) & ~3)

/*---------------------------------------------------------------------------*
    NNSG3dAnmObjMapData

    Useful enumerator for data to be stored in the mapData array NSSG3dAnmObj.
     
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_ANMOBJ_MAPDATA_EXIST     = 0x0100,
    NNS_G3D_ANMOBJ_MAPDATA_DISABLED  = 0x0200,
    NNS_G3D_ANMOBJ_MAPDATA_DATAFIELD = 0x00ff
}
NNSG3dAnmObjMapData;


/*---------------------------------------------------------------------------*
    The animation  blend  function typedef
 *---------------------------------------------------------------------------*/
// Define with anm.h
struct NNSG3dMatAnmResult_;
struct NNSG3dJntAnmResult_;
struct NNSG3dVisAnmResult_;


// material  animation  blend  function
typedef BOOL (*NNSG3dFuncAnmBlendMat)(struct NNSG3dMatAnmResult_*,
                                      const NNSG3dAnmObj*,
                                      u32);

// joint  animation  blend  function
typedef BOOL (*NNSG3dFuncAnmBlendJnt)(struct NNSG3dJntAnmResult_*,
                                      const NNSG3dAnmObj*,
                                      u32);

// visibility  animation  blend  function
typedef BOOL (*NNSG3dFuncAnmBlendVis)(struct NNSG3dVisAnmResult_*,
                                      const NNSG3dAnmObj*,
                                      u32);


/*---------------------------------------------------------------------------*
    NNSG3dRenderObjFlag

    This is the flag held by NNSG3dRenderObj. It can customize the operations of NNS_G3dDraw.
    
    NNS_G3D_RENDEROBJ_FLAG_RECORD
        When NNS_G3dDraw is executed, the calculation result of the material is stored
        in recJntAnm and recMatAnm. After execution finishes, this flag is reset.
        If this flag is reset and recJntAnm and recMatAnm are not NULL,
        the calculation results inside recJntAnm and recMatAnm are used as-is.
    NNS_G3D_RENDEROBJ_FLAG_NOGECMD
        This does not send the geometry command when NNS_G3dDraw is executed.
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW
        When NNS_G3dDraw is executed, this skips the execution of SBC rendering-related commands.
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC
        When NNS_G3dDraw is executed, this skips the execution of SBC matrix calculation-related commands.

    NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE
        If the hintXXXAnmExist on the set/reset flag is not set correctly inside G3D
         (if it has been set when NNS_G3dRenderObjRemoveAnmObj has been set), 

    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW and NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC
     are enabled for models that have been converted with the -s option specified to g3dcvtr.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_RENDEROBJ_FLAG_RECORD             = 0x00000001,
    NNS_G3D_RENDEROBJ_FLAG_NOGECMD            = 0x00000002,
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_DRAW      = 0x00000004,
    NNS_G3D_RENDEROBJ_FLAG_SKIP_SBC_MTXCALC   = 0x00000008,
    NNS_G3D_RENDEROBJ_FLAG_HINT_OBSOLETE      = 0x00000010
}
NNSG3dRenderObjFlag;


/*---------------------------------------------------------------------------*
    NNSG3dSbcCallBackFunc

    The pointer to the callback function stored inside NNSG3dRS
 *---------------------------------------------------------------------------*/
struct NNSG3dRS_;
typedef void (*NNSG3dSbcCallBackFunc)(struct NNSG3dRS_*);


/*---------------------------------------------------------------------------*
    NNSG3dSbcCallBackTiming

    This can designate three kinds of timing for the callback that starts up in the SBC command.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_SBC_CALLBACK_TIMING_NONE = 0x00,
    NNS_G3D_SBC_CALLBACK_TIMING_A    = 0x01,
    NNS_G3D_SBC_CALLBACK_TIMING_B    = 0x02,
    NNS_G3D_SBC_CALLBACK_TIMING_C    = 0x03
}
NNSG3dSbcCallBackTiming;

#define NNS_G3D_SBC_CALLBACK_TIMING_ASSERT(x)               \
    NNS_G3D_ASSERT(x == NNS_G3D_SBC_CALLBACK_TIMING_NONE || \
                   x == NNS_G3D_SBC_CALLBACK_TIMING_A    || \
                   x == NNS_G3D_SBC_CALLBACK_TIMING_B    || \
                   x == NNS_G3D_SBC_CALLBACK_TIMING_C)

/*---------------------------------------------------------------------------*
    NNSG3dRenderObj

    The members, etc. of the structure are always subject to change
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dRenderObj_
{
    u32 flag; // NNSG3dRenderObjFlag

    // NOTICE:
    // The contents of NNSG3dResMdl are not rewritten inside NNS_G3dDraw.
    // (With the exception of when the callback was used, etc.)
    NNSG3dResMdl*         resMdl;
    NNSG3dAnmObj*         anmMat;
    NNSG3dFuncAnmBlendMat funcBlendMat;
    NNSG3dAnmObj*         anmJnt;
    NNSG3dFuncAnmBlendJnt funcBlendJnt;
    NNSG3dAnmObj*         anmVis;
    NNSG3dFuncAnmBlendVis funcBlendVis;

    // Information for the callback
    NNSG3dSbcCallBackFunc cbFunc;              // There is no callback if NULL
    u8                    cbCmd;               // Designate the stopping position with the command. NNS_G3D_SBC_***** (define with res_struct.h)
    u8                    cbTiming;            // NNSG3dSbcCallBackTiming (define with sbc.h)
    u16                   dummy_;

    // Called immediately before starting rendering. Generally, it is used to
    // set the callback vector for NNS_G3dRS.
    NNSG3dSbcCallBackFunc cbInitFunc;

    // The pointer to the region that the user manages
    // This can be used on the callback if the pointer is configured in advance.
    void*                 ptrUser;

    //
    // Use the SBC stored inside the resMdl as a default, but
    // depending on the pointer being stored in ptrUserSbc, the user-defined
    // SBC will be used.
    //
    // Example:
    // When wanting to output a simple model (1 material, 1 shape) to various places
    // using a particle system, etc., it is efficient to substitute the pointer to the code into
    // ptrUserSbc by creating the following kind of SBC code.
    // 
    // MAT[000] 0
    // MTX 0
    // SHP 0
    // MTX 1
    // SHP 0
    // ....
    // MTX n
    // SHP 0
    // RET
    //
    // The matrix must be set into the matrix stack in advance.
    // It is probably best to use a callback to make changes when
    // wanting to slightly alter the material on a shape-by-shape basis.
    //
    u8*                   ptrUserSbc;

    //
    // The pointer to the buffering region of the calculation result
    // When wanting to share the calculation result of the joint or material over
    // multiple frames or multiple models, set the buffer in recJntAnm/recMatAnm.
    //
    // When the NNS_G3D_RENDEROBJ_FLAG_RECORD flag is ON,
    // for those that are not NULL on recJntAnm and recMatAnm
    // the calculation result of each of their the materials and joints are recorded.
    //
    // When the NNS_G3D_RENDEROBJ_FLAG_RECORD flag is OFF,
    // for those that are not NULL for recJntAnm and recMatAnm
    // they are used as the calculation results of each of their materials and joints.
    //
    // The user must allocate memory as follows:
    // For recJntAnm:
    // Allocate sizeof(NNSG3dMatAnmResult) * resMdl->info.numMat bytes
    // For recMatAnm:
    // Allocate sizeof(NNSG3dMatAnmResult) * resMdl->info.numMat bytes
    // for recMatAnm.
    //
    struct NNSG3dJntAnmResult_*   recJntAnm;
    struct NNSG3dMatAnmResult_*   recMatAnm;

    //
    // When animation is added, the bit becomes 1 if it is defined with
    // respect to the matID/nodeID. Each ID can be a maximum of 256, so they can be managed by 8 words apiece.
    // However, even when the animation is deleted, the bit remains 1.
    // The SBC interpreter checks this field,
    // and decides whether or not to call NNSG3dFuncBlendMatXXX.
    // The thing to note is that the only thing one can be certain of is that there is no animation associated with the matID/nodeID when bit is 0.
    //  
    //
    u32 hintMatAnmExist[NNS_G3D_SIZE_MAT_MAX / 32];
    u32 hintJntAnmExist[NNS_G3D_SIZE_JNT_MAX / 32];
    u32 hintVisAnmExist[NNS_G3D_SIZE_JNT_MAX / 32];
}
NNSG3dRenderObj;

// When buffering the NNSG3dJntAnmResult calculation result,
// the size calculation macro of the buffer to be used is
#define NNS_G3D_RENDEROBJ_JNTBUFFER_SIZE(numJnt) \
    ((u32)(sizeof(NNSG3dJntAnmResult) * (numJnt)))

// When buffering the NNSG3dMatAnmResult calculation result,
// the size calculation macro of the buffer to be used is
#define NNS_G3D_RENDEROBJ_MATBUFFER_SIZE(numMat) \
    ((u32)(sizeof(NNSG3dMatAnmResult) * (numMat)))





////////////////////////////////////////////////////////////////////////////////
//
// Declaration of Functions
//

//
// inline functions for NNSG3dAnmObj
//
NNS_G3D_INLINE void NNS_G3dAnmObjSetFrame(NNSG3dAnmObj* pAnmObj, fx32 frame);
NNS_G3D_INLINE void NNS_G3dAnmObjSetBlendRatio(NNSG3dAnmObj* pAnmObj, fx32 ratio);
NNS_G3D_INLINE fx32 NNS_G3dAnmObjGetNumFrame(const NNSG3dAnmObj* pAnmObj);


//
// non-inline functions for NNSG3dAnmObj
//
u32 NNS_G3dAnmObjCalcSizeRequired(const void* pAnm, const NNSG3dResMdl* pMdl);
void NNS_G3dAnmObjInit(NNSG3dAnmObj* pAnmObj,
                       void* pResAnm,
                       const NNSG3dResMdl* pResMdl,
                       const NNSG3dResTex* pResTex);
void NNS_G3dAnmObjEnableID(NNSG3dAnmObj* pAnmObj, int id);
void NNS_G3dAnmObjDisableID(NNSG3dAnmObj* pAnmObj, int id);


//
// inline functions for NNSG3dRenderObj
//
NNS_G3D_INLINE void NNS_G3dRenderObjSetFlag(NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag);
NNS_G3D_INLINE void NNS_G3dRenderObjResetFlag(NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag);
NNS_G3D_INLINE BOOL NNS_G3dRenderObjTestFlag(NNSG3dRenderObj* pRenderObj, NNSG3dRenderObjFlag flag);
NNS_G3D_INLINE u8* NNS_G3dRenderObjSetUserSbc(NNSG3dRenderObj* pRenderObj, u8* sbc);
NNS_G3D_INLINE void NNS_G3dRenderObjSetJntAnmBuffer(NNSG3dRenderObj* pRenderObj, struct NNSG3dJntAnmResult_* buf);
NNS_G3D_INLINE void NNS_G3dRenderObjSetMatAnmBuffer(NNSG3dRenderObj* pRenderObj, struct NNSG3dMatAnmResult_* buf);

NNS_G3D_INLINE struct NNSG3dJntAnmResult_* NNS_G3dRenderObjReleaseJntAnmBuffer(NNSG3dRenderObj* pRenderObj);
NNS_G3D_INLINE struct NNSG3dMatAnmResult_* NNS_G3dRenderObjReleaseMatAnmBuffer(NNSG3dRenderObj* pRenderObj);
NNS_G3D_INLINE void* NNS_G3dRenderObjSetUserPtr(NNSG3dRenderObj* pRenderObj, void* ptr);

NNS_G3D_INLINE NNSG3dResMdl* NNS_G3dRenderObjGetResMdl(NNSG3dRenderObj* pRenderObj);
NNS_G3D_INLINE void NNS_G3dRenderObjSetBlendFuncMat(NNSG3dRenderObj* pRenderObj, NNSG3dFuncAnmBlendMat func);
NNS_G3D_INLINE void NNS_G3dRenderObjSetBlendFuncJnt(NNSG3dRenderObj* pRenderObj, NNSG3dFuncAnmBlendJnt func);
NNS_G3D_INLINE void NNS_G3dRenderObjSetBlendFuncVis(NNSG3dRenderObj* pRenderObj, NNSG3dFuncAnmBlendVis func);


//
// non-inline functions for NNSG3dRenderObj
//
void NNS_G3dRenderObjInit(NNSG3dRenderObj* pRenderObj, NNSG3dResMdl* pResMdl);
void NNS_G3dRenderObjAddAnmObj(NNSG3dRenderObj* pRenderObj, NNSG3dAnmObj* pAnmObj);
void NNS_G3dRenderObjRemoveAnmObj(NNSG3dRenderObj* pRenderObj, NNSG3dAnmObj* pAnmObj);
void NNS_G3dRenderObjSetCallBack(NNSG3dRenderObj* pRenderObj,
                                 NNSG3dSbcCallBackFunc func,
                                 u8*,
                                 u8 cmd,
                                 NNSG3dSbcCallBackTiming timing);
void NNS_G3dRenderObjResetCallBack(NNSG3dRenderObj* pRenderObj);
void NNS_G3dRenderObjSetInitFunc(NNSG3dRenderObj* pRenderObj, NNSG3dSbcCallBackFunc func);


//
// non-inline functions for Model/Texture binding
//

// Texture Key
u32 NNS_G3dTexGetRequiredSize(const NNSG3dResTex* pTex);
u32 NNS_G3dTex4x4GetRequiredSize(const NNSG3dResTex* pTex);
void NNS_G3dTexSetTexKey(NNSG3dResTex* pTex,
                         NNSG3dTexKey texKey,
                         NNSG3dTexKey tex4x4Key);
void NNS_G3dTexLoad(NNSG3dResTex* pTex, BOOL exec_begin_end);
void NNS_G3dTexReleaseTexKey(NNSG3dResTex* pTex,
                             NNSG3dTexKey* texKey,
                             NNSG3dTexKey* tex4x4Key);

// Palette Key
u32 NNS_G3dPlttGetRequiredSize(const NNSG3dResTex* pTex);
void NNS_G3dPlttSetPlttKey(NNSG3dResTex* pTex, NNSG3dPlttKey plttKey);
void NNS_G3dPlttLoad(NNSG3dResTex* pTex, BOOL exec_begin_end);
NNSG3dPlttKey NNS_G3dPlttReleasePlttKey(NNSG3dResTex* pTex);

// Model -> Texture binding
BOOL NNS_G3dBindMdlTex(NNSG3dResMdl* pMdl, const NNSG3dResTex* pTex);
BOOL NNS_G3dBindMdlTexEx(NNSG3dResMdl* pMdl,
                         const NNSG3dResTex* pTex,
                         const NNSG3dResName* pResName);
BOOL NNS_G3dForceBindMdlTex(NNSG3dResMdl* pMdl,
                            const NNSG3dResTex* pTex,
                            u32 texToMatListIdx,
                            u32 texIdx);

void NNS_G3dReleaseMdlTex(NNSG3dResMdl* pMdl);
BOOL NNS_G3dReleaseMdlTexEx(NNSG3dResMdl* pMdl, const NNSG3dResName* pResName);

// Model -> Palette binding
BOOL NNS_G3dBindMdlPltt(NNSG3dResMdl* pMdl, const NNSG3dResTex* pTex);
BOOL NNS_G3dBindMdlPlttEx(NNSG3dResMdl* pMdl,
                          const NNSG3dResTex* pTex,
                          const NNSG3dResName* pResName);
BOOL NNS_G3dForceBindMdlPltt(NNSG3dResMdl* pMdl,
                             const NNSG3dResTex* pTex,
                             u32 plttToMatListIdx,
                             u32 plttIdx);
void NNS_G3dReleaseMdlPltt(NNSG3dResMdl* pMdl);
BOOL NNS_G3dReleaseMdlPlttEx(NNSG3dResMdl* pMdl, const NNSG3dResName* pResName);

BOOL NNS_G3dBindMdlSet(NNSG3dResMdlSet* pMdlSet, const NNSG3dResTex* pTex);
void NNS_G3dReleaseMdlSet(NNSG3dResMdlSet* pMdlSet);


//
// Misc inline functions
//

#ifdef __cplusplus
}
#endif

#include <nnsys/g3d/kernel_inline.h>

#endif

