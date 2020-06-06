/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     glbstate.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: glbstate.h,v $
  Revision 1.14  11/04/2004 12:16:37 AM  nishida_kenji
  APIs to get inverse matrices.

  Revision 1.13  09/21/2004 09:55:45 AM  nishida_kenji
  NNS_G3dGlbGetInvCameraMtx AKA NNS_G3dGlbGetInvV.
  NNS_G3dGlbGetSrtCameraMtx AKA NNS_G3dGlbGetWV.
  NNS_G3dGlbGetInvSrtCameraMtx AKA NNS_G3dGlbGetInvWV.

  Revision 1.12  09/10/2004 09:13:52 AM  nishida_kenji
  Add NNS_G3dGlbFlushVP and NNS_G3dGlbSetProjectionMtx,
  rename NNS_G3dGlbFlush to NNS_G3dGlbFlushP, and
  NNS_G3dGlbFlushAlt to NNS_G3dGlbFlushWVP.

  Revision 1.11  09/03/2004 12:27:32 PM  nishida_kenji
  add accessors.

  Revision 1.10  07/27/2004 04:59:49 AM  nishida_kenji
  NNSG3dGlb::flag becomes u32.

  Revision 1.9  07/27/2004 04:40:47 AM  nishida_kenji
  Add NNS_G3D_INLINE to prototypes.

  Revision 1.8  07/22/2004 07:22:56 AM  nishida_kenji
  add
  const MtxFx43* NNS_G3dGlbGetSrtCameraMtx(void);
  const MtxFx43* NNS_G3dGlbGetInvSrtCameraMtx(void);
  for billboarding.

  Revision 1.7  07/16/2004 07:27:03 AM  nishida_kenji
  Add viewport, camPos, camUp, camTraget to NNS_G3dGlb

  Revision 1.6  07/15/2004 12:58:09 PM  nishida_kenji
  reset texImageParam when GlbFlush.

  Revision 1.5  07/12/2004 02:25:59 AM  nishida_kenji
  (none)

  Revision 1.4  07/10/2004 08:46:57 AM  nishida_kenji
  (none)

  Revision 1.3  07/09/2004 07:18:27 AM  nishida_kenji
  (none)

  Revision 1.2  07/07/2004 12:38:46 PM  nishida_kenji
  REVISED

  Revision 1.1  07/02/2004 05:57:55 AM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_GLBSTATE_H_
#define NNSG3D_GLBSTATE_H_

#include <nnsys/g3d/config.h>

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Definition of the structure and typedef
//

typedef enum
{
    // ON when NNS_G3dGlbFlushAlt was used
    // OFF when NNS_G3dGlbFlush was used
    NNS_G3D_GLB_FLAG_FLUSH_WVP              = 0x00000001,
    NNS_G3D_GLB_FLAG_FLUSH_VP               = 0x00000002,
    NNS_G3D_GLB_FLAG_INVBASE_UPTODATE       = 0x00000004,
    NNS_G3D_GLB_FLAG_INVCAMERA_UPTODATE     = 0x00000008,
    NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE       = 0x00000010,
    NNS_G3D_GLB_FLAG_INVBASECAMERA_UPTODATE = 0x00000020,
    NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE = 0x00000040,

    NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE    = 0x00000080,

    NNS_G3D_GLB_FLAG_SRTCAMERA_UPTODATE     = NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE,
    NNS_G3D_GLB_FLAG_FLUSH_ALT              = NNS_G3D_GLB_FLAG_FLUSH_WVP
}
NNSG3dGlbFlag;


/*---------------------------------------------------------------------------*
    NNSG3dGlb

    This is a guideline for not holding on to the global state wherever possible.
    However, there is the possibility that the data members will increase. Access via an API is preferable.
 *---------------------------------------------------------------------------*/
typedef struct
{
    u32     cmd0;             // G3_MtxMode -> G3_LoadMtx44 -> G3_MtxMode -> G3_LoadMtx43
    u32     mtxmode_proj;     // GX_MTXMODE_PROJECTION
    MtxFx44 projMtx;          // The projection matrix
    //
    u32     mtxmode_posvec;   // GX_MTXMODE_POSITION_VECTOR
    MtxFx43 cameraMtx;        // The camera matrix
    
    u32     cmd1;             // G3_LightVector * 4
    u32     lightVec[4];      // The light vector

    u32     cmd2;             // G3_MatColorDiffAmb -> G3_MatColorSpecEmi -> G3_PolygonAttr -> G3_ViewPort
    u32     prmMatColor0;
    u32     prmMatColor1;
    u32     prmPolygonAttr;
    u32     prmViewPort;

    u32     cmd3;             // G3_LightColor * 4
    u32     lightColor[4];    // The light color

    u32     cmd4;             // G3_MultMtx43 -> G3_Scale -> G3_TexImageParam
    MtxFx33 prmBaseRot;
    VecFx32 prmBaseTrans;
    VecFx32 prmBaseScale;
    u32     prmTexImageParam;

    u32 flag;   // NNSG3dGlbFlag
    MtxFx43 invCameraMtx;
    MtxFx43 srtCameraMtx;
    MtxFx43 invSrtCameraMtx;

    MtxFx43 invBaseMtx;

    MtxFx44 invProjMtx;
    MtxFx44 invCameraProjMtx;

    VecFx32 camPos;
    VecFx32 camUp;
    VecFx32 camTarget;
}
NNSG3dGlb;

////////////////////////////////////////////////////////////////////////////////
//
// Declaration of Functions
//

//
// Inline functions
//
NNS_G3D_INLINE void NNS_G3dGlbLookAt(const VecFx32 * camPos,
                                     const VecFx32 * camUp,
                                     const VecFx32 * target);
NNS_G3D_INLINE void NNS_G3dGlbFrustum(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f);
NNS_G3D_INLINE void NNS_G3dGlbFrustumW(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW);
NNS_G3D_INLINE void NNS_G3dGlbPerspective(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f);
NNS_G3D_INLINE void NNS_G3dGlbPerspectiveW(fx32 fovySin, fx32 fovyCos, fx32 aspect, fx32 n, fx32 f, fx32 scaleW);
NNS_G3D_INLINE void NNS_G3dGlbOrtho(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f);
NNS_G3D_INLINE void NNS_G3dGlbOrthoW(fx32 t, fx32 b, fx32 l, fx32 r, fx32 n, fx32 f, fx32 scaleW);
NNS_G3D_INLINE void NNS_G3dGlbSetProjectionMtx(const MtxFx44* m);

NNS_G3D_INLINE void NNS_G3dGlbSetBaseRot(const MtxFx33* pRot);
NNS_G3D_INLINE void NNS_G3dGlbSetViewPort(int x1, int y1, int x2, int y2);

NNS_G3D_INLINE const MtxFx43* NNS_G3dGlbGetCameraMtx(void);
NNS_G3D_INLINE const MtxFx44* NNS_G3dGlbGetProjectionMtx(void);
NNS_G3D_INLINE const MtxFx33* NNS_G3dGlbGetBaseRot(void);
NNS_G3D_INLINE const VecFx32* NNS_G3dGlbGetBaseTrans(void);
NNS_G3D_INLINE const VecFx32* NNS_G3dGlbGetBaseScale(void);
NNS_G3D_INLINE const VecFx32* NNS_G3dGlbGetCameraPos(void);
NNS_G3D_INLINE const VecFx32* NNS_G3dGlbGetCameraUp(void);
NNS_G3D_INLINE const VecFx32* NNS_G3dGlbGetCameraTarget(void);

NNS_G3D_INLINE void NNS_G3dGlbFlush(void);
NNS_G3D_INLINE void NNS_G3dGlbFlushAlt(void);
NNS_G3D_INLINE const MtxFx43* NNS_G3dGlbGetSrtCameraMtx(void);
NNS_G3D_INLINE const MtxFx43* NNS_G3dGlbGetInvSrtCameraMtx(void);


//
// Non-Inline functions
//
void NNS_G3dGlbInit(void);
void NNS_G3dGlbFlushP(void);
void NNS_G3dGlbFlushVP(void);
void NNS_G3dGlbFlushWVP(void);
void NNS_G3dGlbSetBaseTrans(const VecFx32* pTrans);
void NNS_G3dGlbSetBaseScale(const VecFx32* pScale);
void NNS_G3dGlbLightVector(GXLightId lightID, fx16 x, fx16 y, fx16 z);
void NNS_G3dGlbLightColor(GXLightId lightID, GXRgb rgb);
void NNS_G3dGlbMaterialColorDiffAmb(GXRgb diffuse, GXRgb ambient, BOOL IsSetVtxColor);
void NNS_G3dGlbMaterialColorSpecEmi(GXRgb specular, GXRgb emission, BOOL IsShininess);
void NNS_G3dGlbPolygonAttr(int light,
                           GXPolygonMode polyMode,
                           GXCull cullMode,
                           int polygonID,
                           int alpha,
                           int misc // GXPolygonAttrMisc
                          );
const MtxFx43* NNS_G3dGlbGetInvV(void);
const MtxFx43* NNS_G3dGlbGetInvW(void);
const MtxFx44* NNS_G3dGlbGetInvP(void);
const MtxFx43* NNS_G3dGlbGetWV(void);
const MtxFx43* NNS_G3dGlbGetInvWV(void);
const MtxFx44* NNS_G3dGlbGetInvVP(void);




void NNS_G3dGlbGetViewPort(int* px1, int* py1, int* px2, int* py2);

////////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//

extern NNSG3dGlb NNS_G3dGlb;


#ifdef __cplusplus
}/* extern "C"*/
#endif

#include <nnsys/g3d/glbstate_inline.h>

#endif

