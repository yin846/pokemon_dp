/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) - 
  File:     cgtool.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: cgtool.h,v $
  Revision 1.6  10/07/2004 02:10:33 AM  nishida_kenji
  Add comments.

  Revision 1.5  07/27/2004 01:45:01 PM  nishida_kenji
  remove an obsolete function.

  Revision 1.4  07/14/2004 11:31:45 AM  nishida_kenji
  (none)

  Revision 1.3  07/12/2004 12:50:59 PM  nishida_kenji
  refactoring

  Revision 1.2  07/08/2004 12:46:45 PM  nishida_kenji
  (none)

  Revision 1.1  07/07/2004 12:39:33 PM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_CGTOOL_H_
#define NNSG3D_CGTOOL_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/anm.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
    NNSG3dGetJointScale

    Pointer to the function that performs the scale calculation of joints.
    Keep it separated because each CG tool has differences.
*---------------------------------------------------------------------------*/
typedef void (*NNSG3dGetJointScale)(NNSG3dJntAnmResult* pResult,
                                    const fx32* p,
                                    const u8* cmd,
                                    u32 srtflag);

/*---------------------------------------------------------------------------*
    NNSG3dSendJointSRT

    This configures the joint matrix in the geometry engine.
    The matrix mode must be in Position/Vector mode at the point this function is called.
    Also, the matrix to be processed must be in the current matrix.
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dSendJointSRT)(const NNSG3dJntAnmResult*);


/*---------------------------------------------------------------------------*
    NNSG3dSendTexSRT

    This function configures the texture SRT matrix in the geometry engine.
    After execution, the matrix mode becomes Position/Vector mode.
 *---------------------------------------------------------------------------*/
typedef void (*NNSG3dSendTexSRT)(const NNSG3dMatAnmResult*);


/*---------------------------------------------------------------------------*
    NNS_G3dSendJointSRT_FuncArray

    The function pointer array corresponding to the value of NNSG3dScalingRule(<model_info>::scaling_rule)
     When sending joint matrices, 
    from the G3D, they must be accessed via this function pointer vector. 
 *---------------------------------------------------------------------------*/
extern NNSG3dGetJointScale NNS_G3dGetJointScale_FuncArray[NNS_G3D_FUNC_SENDJOINTSRT_MAX];
extern NNSG3dSendJointSRT NNS_G3dSendJointSRT_FuncArray[NNS_G3D_FUNC_SENDJOINTSRT_MAX];


/*---------------------------------------------------------------------------*
    NNS_G3dSendTexSRT_FuncArray

    The function pointer array corresponding to the value of NNSG3dTexMtxMode(<model_info>::tex_matrix_mode).
     When sending texture matrices, 
    from the G3D, they must be accessed via this function pointer vector. 
 *---------------------------------------------------------------------------*/
extern NNSG3dSendTexSRT   NNS_G3dSendTexSRT_FuncArray[NNS_G3D_FUNC_SENDTEXSRT_MAX];


#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif

