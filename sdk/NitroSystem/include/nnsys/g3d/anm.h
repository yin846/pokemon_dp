/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     anm.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: anm.h,v $
  Revision 1.13  07/27/2004 01:44:53 PM  nishida_kenji
  remove an obsolete function.

  Revision 1.12  07/21/2004 06:32:04 AM  nishida_kenji
  add NNS_G3D_MATANM_RESULTFLAG_WIREFRAME

  Revision 1.11  07/20/2004 08:03:14 AM  nishida_kenji
  small fix

  Revision 1.10  07/20/2004 01:30:08 AM  nishida_kenji
  (none)

  Revision 1.9  07/19/2004 10:53:10 AM  nishida_kenji
  change arguments of animation calc functions

  Revision 1.8  07/15/2004 12:58:22 PM  nishida_kenji
  rename functions

  Revision 1.7  07/15/2004 02:43:42 AM  nishida_kenji
  add magW, magH field on matAnmResult.

  Revision 1.6  07/14/2004 11:31:37 AM  nishida_kenji
  (none)

  Revision 1.5  07/12/2004 12:50:59 PM  nishida_kenji
  refactoring

  Revision 1.4  07/12/2004 09:26:13 AM  nishida_kenji
  change order of member data on NNSG3dJntAnmResult.

  Revision 1.3  07/10/2004 08:46:57 AM  nishida_kenji
  (none)

  Revision 1.2  07/09/2004 07:18:43 AM  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:39:33 PM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_ANM_H_
#define NNSG3D_ANM_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
    Summary of the animation operations:

    When conducting animation processing related to the matID and the nodeID,
    The Blend function registered in the NNSG3dRenderObj is called from the SBC interpeter.
    The result (BlendMat assumes the responsibility internally) will be stored in NNSG3d[Mat|Jnt]AnmResult*.
    NNSG3dAnmObj* is a data member of NNSG3dRenderObj,
    and is an animation object list.
 *---------------------------------------------------------------------------*/

////////////////////////////////////////////////////////////////////////////////
//
// Definition of the structure and typedef
//


struct NNSG3dResMdl_;
/*---------------------------------------------------------------------------*
    NNSG3dAnimInitFunc

    The type of function that initializes the NNSG3dAnmObj. void* is the pointer to the animation resource, since
    depending on the kind of animation resource, the initialization method ofNNSG3dAnmObj will differ.
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dAnimInitFunc)(NNSG3dAnmObj*,
                                   void*,
                                   const NNSG3dResMdl*);






/*---------------------------------------------------------------------------*
    NNSG3dAnmObjInitFunc

    category0 and category1 are the same as those of NNSG3dResAnmHeader.
 *---------------------------------------------------------------------------*/
typedef struct
{
    u8                  category0;
    u8                  dummy_;
    u16                 category1;
    NNSG3dAnimInitFunc  func;
}
NNSG3dAnmObjInitFunc;


/*---------------------------------------------------------------------------*
    About the material animation operations

    The calculation of the material animation successively overwrites the values in NNSG3dMatAnmResult,
    and is performed by sending the data to the geometry engine at the end.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SCALEONE  = 0x00000001,
    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_ROTZERO   = 0x00000002,
    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_TRANSZERO = 0x00000004,

    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_SET       = 0x00000008,
    NNS_G3D_MATANM_RESULTFLAG_TEXMTX_MULT      = 0x00000010,
    NNS_G3D_MATANM_RESULTFLAG_WIREFRAME        = 0x00000020
}
NNSG3dMatAnmResultFlag;


/*---------------------------------------------------------------------------*
    NNSG3dMatAnmResult

    This is a structure for storing the calculated material information.
    The calculation is done inside the MAT command of SBC.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dMatAnmResult_
{
    NNSG3dMatAnmResultFlag flag;
    u32                    prmMatColor0;
    u32                    prmMatColor1;
    u32                    prmPolygonAttr;
    u32                    prmTexImage;
    u32                    prmTexPltt;

    // No corresponding value is set for the
    // SCALEONE/ROTZERO/TRANSZERO of the flag.
    fx32                   scaleS, scaleT;
    fx16                   sinR, cosR;
    fx32                   transS, transT;

    u16                    origWidth, origHeight;
    fx32                   magW, magH;
}
NNSG3dMatAnmResult;


/*---------------------------------------------------------------------------*
    About the joint animation operations

    They do not support quarter neon.
 *---------------------------------------------------------------------------*/
typedef enum
{
    NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE      = 0x00000001,
    NNS_G3D_JNTANM_RESULTFLAG_ROT_ZERO       = 0x00000002,
    NNS_G3D_JNTANM_RESULTFLAG_TRANS_ZERO     = 0x00000004,
    NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE   = 0x00000008,
    NNS_G3D_JNTANM_RESULTFLAG_SCALEEX1_ONE   = 0x00000010,
    NNS_G3D_JNTANM_RESULTFLAG_MAYA_SSC       = 0x00000020
//    NNS_G3D_JNTANM_RESULTFLAG_ROT_QUATERNION = 0x00000040
}
NNSG3dJntAnmResultFlag;


/*---------------------------------------------------------------------------*
    NNSG3dJntAnmResult

    This is a structure for storing the calculated node information.
    The calculation is done inside the NODEDESC command of SBC.
    scaleEx0 and scaleEx1 are used to store additional scale information
    for the Classic Scale off of the Maya SSC and Si3d.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dJntAnmResult_
{
    NNSG3dJntAnmResultFlag flag;
    VecFx32                scale;
    VecFx32                scaleEx0;
    VecFx32                scaleEx1;
    MtxFx33                rot;
    VecFx32                trans;
}
NNSG3dJntAnmResult;


/*---------------------------------------------------------------------------*
    About the visibility animation operations

    These operations make the joint entirely visible or invisible.
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    NNSG3dVisAnmResult

    This is a structure for storing the calculated visibility information.
    The calculation is done inside the NODE command of SBC.
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dVisAnmResult_
{
    BOOL isVisible;
}
NNSG3dVisAnmResult;


// material animation calculation function
typedef void (*NNSG3dFuncAnmMat)(NNSG3dMatAnmResult*,
                                 const NNSG3dAnmObj*,
                                 u32);

// joint animation calculation function
typedef void (*NNSG3dFuncAnmJnt)(NNSG3dJntAnmResult*,
                                 const NNSG3dAnmObj*,
                                 u32);

// visibility animation calculation function
typedef void (*NNSG3dFuncAnmVis)(NNSG3dVisAnmResult*,
                                 const NNSG3dAnmObj*,
                                 u32);


////////////////////////////////////////////////////////////////////////////////
//
// Declaration of Functions
//

//
// The default material animation blend function.
// In short, the animation results that were calculated later have priority.
//
BOOL NNSi_G3dAnmBlendMat(NNSG3dMatAnmResult* pResult,
                         const NNSG3dAnmObj* pAnmObj,
                         u32 matID);

BOOL NNSi_G3dAnmBlendJnt(NNSG3dJntAnmResult*,
                         const NNSG3dAnmObj*,
                         u32);

BOOL NNSi_G3dAnmBlendVis(NNSG3dVisAnmResult*,
                         const NNSG3dAnmObj*,
                         u32);

////////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//

//
// The pointer to the default animation blend function
// This is used to initially configure the NNSG3dRenderObj in NNS_G3dRenderObjInit.
//
extern NNSG3dFuncAnmBlendMat NNS_G3dFuncBlendMatDefault;
extern NNSG3dFuncAnmBlendJnt NNS_G3dFuncBlendJntDefault;
extern NNSG3dFuncAnmBlendVis NNS_G3dFuncBlendVisDefault;

//
// The pointer to the default animation calculation function
// This is used to initially configure the NNSG3dAnmObj in NNS_G3dAnmObjInit.
//
extern NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBmaDefault;
extern NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBtpDefault;
extern NNSG3dFuncAnmMat NNS_G3dFuncAnmMatNsBtaDefault;
extern NNSG3dFuncAnmJnt NNS_G3dFuncAnmJntNsBcaDefault;
extern NNSG3dFuncAnmVis NNS_G3dFuncAnmVisNsBvaDefault;

extern u32 NNS_G3dAnmFmtNum;
extern NNSG3dAnmObjInitFunc NNS_G3dAnmObjInitFuncArray[NNS_G3D_ANMFMT_MAX];


#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif

