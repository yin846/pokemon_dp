/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     gecom.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gecom.h,v $
  Revision 1.11  10/13/2004 09:14:45 AM  nishida_kenji
  small fix(const correctness).

  Revision 1.10  09/22/2004 06:11:53 AM  nishida_kenji
  Add NNS_G3dGeUseFastDma(read warning).

  Revision 1.9  08/30/2004 11:46:27 AM  nishida_kenji
  deal with NITRO-CPU Geometry FIFO glitch(2004/08/26).

  Revision 1.8  07/27/2004 04:40:50 AM  nishida_kenji
  Add NNS_G3D_INLINE to prototypes.

  Revision 1.7  07/12/2004 12:50:59 PM  nishida_kenji
  refactoring

  Revision 1.6  07/12/2004 07:21:18 AM  nishida_kenji
  (none)

  Revision 1.5  07/12/2004 02:28:02 AM  nishida_kenji
  (none)

  Revision 1.4  07/10/2004 08:46:57 AM  nishida_kenji
  (none)

  Revision 1.3  07/09/2004 07:18:31 AM  nishida_kenji
  (none)

  Revision 1.2  07/07/2004 12:39:00 PM  nishida_kenji
  REVISED

  Revision 1.1  07/02/2004 05:57:55 AM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_GECOM_H_
#define NNSG3D_GECOM_H_

#include <nnsys/g3d/config.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// To prepare for when the geometry command / display list
// transmission wrapper command is cached,
// the geometry command should be sent via this function.
//

////////////////////////////////////////////////////////////////////////////////
//
// Definition of the structure and typedef
//

/*---------------------------------------------------------------------------*
    NNSG3dGeBuffer

    This is a data structure for caching when the previous command is being transmitted.
    It is normally allocated to the stack region (DTCM).
 *---------------------------------------------------------------------------*/
typedef struct NNSG3dGeBuffer_
{
    u32 idx;

    // If you want to send a geometry command while the
    // geometry command list is being transmitted (NNS_G3dGeSendDL), buffering is performed here.
    u32 data[NNS_G3D_SIZE_COMBUFFER];
}
NNSG3dGeBuffer;


////////////////////////////////////////////////////////////////////////////////
//
// Declaration of Functions
//

//
// inline functions
//
NNS_G3D_INLINE void NNS_G3dGeBufferData_N(const u32* args, u32 num);
NNS_G3D_INLINE void NNS_G3dGeMtxMode(GXMtxMode mode);
NNS_G3D_INLINE void NNS_G3dGePushMtx(void);
NNS_G3D_INLINE void NNS_G3dGePopMtx(int num);
NNS_G3D_INLINE void NNS_G3dGeStoreMtx(int num);
NNS_G3D_INLINE void NNS_G3dGeRestoreMtx(int num);
NNS_G3D_INLINE void NNS_G3dGeIdentity(void);
NNS_G3D_INLINE void NNS_G3dGeScaleVec(const VecFx32* vec);
NNS_G3D_INLINE void NNS_G3dGeScale(fx32 x, fx32 y, fx32 z);
NNS_G3D_INLINE void NNS_G3dGeTranslateVec(const VecFx32* vec);
NNS_G3D_INLINE void NNS_G3dGeTranslate(fx32 x, fx32 y, fx32 z);
NNS_G3D_INLINE void NNS_G3dGeColor(GXRgb rgb);
NNS_G3D_INLINE void NNS_G3dGeNormal(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeTexCoord(fx32 s, fx32 t);
NNS_G3D_INLINE void NNS_G3dGeVtx(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVtx10(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVtxXY(fx16 x, fx16 y);
NNS_G3D_INLINE void NNS_G3dGeVtxXZ(fx16 x, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVtxYZ(fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVtxDiff(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGePolygonAttr(
                        int light,
                        GXPolygonMode polyMode,
                        GXCull cullMode,
                        int polygonID,
                        int alpha,
                        int misc // GXPolygonAttrMisc
                        );
NNS_G3D_INLINE void NNS_G3dGeTexImageParam(
                        GXTexFmt texFmt,
                        GXTexGen texGen,
                        GXTexSizeS s,
                        GXTexSizeT t,
                        GXTexRepeat repeat,
                        GXTexFlip flip,
                        GXTexPlttColor0 pltt0,
                        u32 addr
                        );
NNS_G3D_INLINE void NNS_G3dGeTexPlttBase(u32 addr, GXTexFmt texfmt);
NNS_G3D_INLINE void NNS_G3dGeMaterialColorDiffAmb(
                        GXRgb diffuse,
                        GXRgb ambient,
                        BOOL IsSetVtxColor);
NNS_G3D_INLINE void NNS_G3dGeMaterialColorSpecEmi(
                        GXRgb specular,
                        GXRgb emission,
                        BOOL IsShininess);
NNS_G3D_INLINE void NNS_G3dGeLightVector(
                        GXLightId lightID,
                        fx16 x,
                        fx16 y,
                        fx16 z);
NNS_G3D_INLINE void NNS_G3dGeLightColor(GXLightId lightID, GXRgb rgb);
NNS_G3D_INLINE void NNS_G3dGeBegin(GXBegin primitive);
NNS_G3D_INLINE void NNS_G3dGeEnd(void);
NNS_G3D_INLINE void NNS_G3dGeViewPort(int x1, int y1, int x2, int y2);
NNS_G3D_INLINE void NNS_G3dGeBoxTest(const GXBoxTestParam * box);
NNS_G3D_INLINE void NNS_G3dGePositionTest(fx16 x, fx16 y, fx16 z);
NNS_G3D_INLINE void NNS_G3dGeVectorTest(fx16 x, fx16 y, fx16 z);

NNS_G3D_INLINE void NNS_G3dGeLoadMtx44(const MtxFx44* m);
NNS_G3D_INLINE void NNS_G3dGeLoadMtx43(const MtxFx43* m);
NNS_G3D_INLINE void NNS_G3dGeMultMtx44(const MtxFx44 * m);
NNS_G3D_INLINE void NNS_G3dGeMultMtx43(const MtxFx43 * m);
NNS_G3D_INLINE void NNS_G3dGeMultMtx33(const MtxFx33 * m);
NNS_G3D_INLINE void NNS_G3dGeShininess(const u32 * table);

//
// non-inline functions
//
BOOL NNS_G3dGeIsSendDLBusy(void);
BOOL NNS_G3dGeIsBufferExist(void);
BOOL NNS_G3dGeIsImmOK(void);
BOOL NNS_G3dGeIsBufferOK(u32 numWord);
void NNS_G3dGeWaitSendDL(void);
void NNS_G3dGeSetBuffer(NNSG3dGeBuffer* p);
NNSG3dGeBuffer* NNS_G3dGeReleaseBuffer(void);
void NNS_G3dGeFlushBuffer(void);
void NNS_G3dGeSendDL(const void* src, u32 szByte);
void NNS_G3dGeBufferOP_N(u32 op, const u32* args, u32 num);
void NNS_G3dGeUseFastDma(BOOL cond); // Undocumented

////////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//

#ifdef __cplusplus
}/* extern "C"*/
#endif

#include <nnsys/g3d/gecom_inline.h>

#endif

