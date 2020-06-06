/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     util.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util.h,v $
  Revision 1.17  11/09/2004 04:55:55  nishida_kenji
  remove unused codes.

  Revision 1.16  11/04/2004 00:06:55  nishida_kenji
  Add NNS_G3dLocalOriginToScrPos, NNS_G3dWorldPosToScrPos,
  and NNS_G3dScrPosToWorldLine.

  Revision 1.15  09/03/2004 12:11:33  nishida_kenji
  add NNS_G3dGetCurrentMtx

  Revision 1.14  08/03/2004 08:42:58  nishida_kenji
  NNS_G3dGetAnmByIdx, NNS_G3dGetAnmByName move to res_struct_accessor_anm.h

  Revision 1.13  08/03/2004 04:25:21  nishida_kenji
  fix comments.

  Revision 1.12  08/03/2004 01:45:17  nishida_kenji
  Add NNS_G3dGetAnmByIdx, NNS_G3dGetAnmByName, NNS_G3D_GET_ANM.

  Revision 1.11  07/27/2004 04:58:51  nishida_kenji
  Add NNS_G3D_INLINE

  Revision 1.10  07/21/2004 05:41:29  nishida_kenji
  small bug fix

  Revision 1.9  07/20/2004 09:40:37  nishida_kenji
  Add NNS_G3D_DEFRESNAME

  Revision 1.7  07/19/2004 10:54:40  nishida_kenji
  add macros to search an ID by literal string.

  Revision 1.6  07/17/2004 12:54:02  nishida_kenji
  Add NNS_G3dResDefaultSetup, NNS_G3dResDefaultDestroy
  NNS_G3dGetSbcCmdLen, NNS_G3dSearchSbcCmd
  NNS_G3dGetParentNodeID, NNS_G3dGetChildNodeIDList

  Revision 1.5  07/16/2004 07:26:29  nishida_kenji
  Add NNS_G3dInit.

  Revision 1.4  07/13/2004 05:30:45  nishida_kenji
  NNS_G3dGetResultMtx, NNS_G3dSetResultMtx

  Revision 1.1  07/10/2004 08:46:42  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_UTIL_H_
#define NNSG3D_UTIL_H_

#include <nnsys/g3d/config.h>
#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
// The 16th character cannot be used if this is not done when for C++
#define NNS_G3D_UTIL_RESNAME_LEN 17
#else
#define NNS_G3D_UTIL_RESNAME_LEN 16
#endif


/*---------------------------------------------------------------------------*
    NNSG3dUtilResName

    This is a shared structure used when defining the resource name inside the program.
    It should be used through NNS_G3D_DEFRESNAME.
    It also exists in C++ so that a full 16 characters can be used.
 *---------------------------------------------------------------------------*/
typedef union
{
    char _0[NNS_G3D_UTIL_RESNAME_LEN];
    struct
    {
        NNSG3dResName resName;
#ifdef __cplusplus
        u32           _1;
#endif
    };
}
NNSG3dUtilResName;

/*---------------------------------------------------------------------------*
    NNS_G3D_DEFRESNAME

    This is a macro used when using the resource name on the program.
    A full 16 characters can also be used with C++.

    WARNING
    The compiler must be implemented such that static memory has been cleared to 0 ahead of time.  
     
 *---------------------------------------------------------------------------*/
#define NNS_G3D_DEFRESNAME(var, str) \
    static const NNSG3dUtilResName var ATTRIBUTE_ALIGN(4) = { str }


//
// Macro for when searching using a string literal
// NOTICE:
// This must be made into an inline function.
//
#define NNS_G3D_GET_JNTID(pMdl, pJntID, literal)                                          \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        *pJntID = NNS_G3dGetNodeIdxByName(NNS_G3dGetNodeInfo(pMdl), (NNSG3dResName*)name); \
    } while(0)

#define NNS_G3D_GET_MATID(pMdl, pMatID, literal)                                          \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        *pMatID = NNS_G3dGetMatIdxByName(NNS_G3dGetMat(pMdl), (NNSG3dResName*)name);      \
    } while(0)

#define NNS_G3D_GET_SHPID(pMdl, pShpID, literal)                                          \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        *pShpID = NNS_G3dGetShpIdxByName(NNS_G3dGetShp(pMdl), (NNSG3dResName*)name);      \
    } while(0)

#define NNS_G3D_GET_MDL(pMdlSet, pMdl, literal)                                           \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        pMdl = NNS_G3dGetMdlByName(pMdlSet, (NNSG3dResName*)name);                        \
    } while(0)


//
// Get the animation resource from the animation resource file
//
#define NNS_G3D_GET_ANM(pRes, pResAnm, literal)                                           \
    do {                                                                                  \
        static const char name[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = literal;    \
        pResAnm = NNS_G3dGetAnmByName(pRes, pName);                                       \
    } while(0)


//
// Bit vector
//
NNS_G3D_INLINE BOOL NNSi_G3dBitVecCheck(const u32* vec, u32 idx);
NNS_G3D_INLINE void NNSi_G3dBitVecSet(u32* vec, u32 idx);
NNS_G3D_INLINE void NNSi_G3dBitVecReset(u32* vec, u32 idx);


//
// Get and change from the matrix stack
//
void NNS_G3dGetCurrentMtx(MtxFx43* m, MtxFx33* n);
BOOL NNS_G3dGetResultMtx(const NNSG3dRenderObj* pRenderObj,
                         MtxFx43* pos,
                         MtxFx33* nrm,
                         u32 nodeID);
BOOL NNS_G3dSetResultMtx(const NNSG3dRenderObj* pRenderObj,
                         const MtxFx43* pos,
                         const MtxFx33* nrm,
                         u32 nodeID);


//
// Default initialization
//
void NNS_G3dInit(void);

//
// SBC analysis utility
//
int NNS_G3dGetSbcCmdLen(const u8* c);
const u8* NNS_G3dSearchSbcCmd(const u8* c, u8 cmd);
const u8* NNS_G3dGetParentNodeID(int* parentID, const u8* sbc, u32 nodeID);
int NNS_G3dGetChildNodeIDList(u8* idList, const u8* sbc, u32 nodeID);

//
// Resource setup / End processing utility
//
BOOL NNS_G3dResDefaultSetup(void* pResData);
void NNS_G3dResDefaultRelease(void* pResData);

//
// Coordinate transformation utilities
//
int NNS_G3dLocalOriginToScrPos(int* px, int* py);
int NNS_G3dWorldPosToScrPos(const VecFx32* pWorld, int* px, int* py);
int NNS_G3dScrPosToWorldLine(int px, int py, VecFx32* pNear, VecFx32* pFar);


#ifdef __cplusplus
}/* extern "C"*/
#endif

#include <nnsys/g3d/util_inline.h>

#endif

