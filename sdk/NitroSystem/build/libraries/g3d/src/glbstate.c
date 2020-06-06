/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) - 
  File:     glbstate.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: glbstate.c,v $
  Revision 1.16  02/25/2005 05:38:42 AM  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.15  11/04/2004 12:17:10 AM  nishida_kenji
  APIs to get inverse matrices.

  Revision 1.14  10/07/2004 02:10:43 AM  nishida_kenji
  Add comments.

  Revision 1.13  09/21/2004 09:55:54 AM  nishida_kenji
  NNS_G3dGlbGetInvCameraMtx AKA NNS_G3dGlbGetInvV.
  NNS_G3dGlbGetSrtCameraMtx AKA NNS_G3dGlbGetWV.
  NNS_G3dGlbGetInvSrtCameraMtx AKA NNS_G3dGlbGetInvWV.

  Revision 1.12  09/21/2004 08:54:19 AM  nishida_kenji
  fix NNS_G3dGlbFlushWVP(light direction).

  Revision 1.11  09/10/2004 09:11:38 AM  nishida_kenji
  Add NNS_G3dGlbFlushVP,
  rename NNS_G3dGlbFlush to NNS_G3dGlbFlushP, and
  NNS_G3dGlbFlushAlt to NNS_G3dGlbFlushWVP.

  Revision 1.10  07/22/2004 07:23:48 AM  nishida_kenji
  add
  const MtxFx43* NNS_G3dGlbGetSrtCameraMtx(void);
  const MtxFx43* NNS_G3dGlbGetInvSrtCameraMtx(void);
  for billboarding.

  Revision 1.9  07/22/2004 05:08:17 AM  nishida_kenji
  fix NNS_G3dGlbFlushAlt(lighting & baseSRT)

  Revision 1.8  07/16/2004 07:25:20 AM  nishida_kenji
  add viewport as global state

  Revision 1.7  07/15/2004 12:58:57 PM  nishida_kenji
  reset texImageParam when GlbFlush.

  Revision 1.6  07/14/2004 11:31:03 AM  nishida_kenji
  default light color becomes brighter.

  Revision 1.5  07/12/2004 02:24:50 AM  nishida_kenji
  (none)

  Revision 1.4  07/10/2004 08:48:00 AM  nishida_kenji
  (none)

  Revision 1.3  07/08/2004 12:45:57 PM  nishida_kenji
  (none)

  Revision 1.2  07/07/2004 12:38:32 PM  nishida_kenji
  REVISED

  Revision 1.1  07/02/2004 05:57:00 AM  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/g3d/glbstate.h>
#include <nnsys/g3d/gecom.h>

//
//
// Functions
//
//

/*---------------------------------------------------------------------------*
    NNS_G3dGlbInit

    Initialize NNS_G3dGlb structure
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbInit(void)
{
    // Constant definitions
    NNS_G3dGlb.cmd0 = (G3OP_MTX_MODE << 0)     |
                      (G3OP_MTX_LOAD_4x4 << 8) |
                      (G3OP_MTX_MODE << 16)    |
                      (G3OP_MTX_LOAD_4x3 << 24);
    NNS_G3dGlb.mtxmode_proj   = GX_MTXMODE_PROJECTION;
    NNS_G3dGlb.mtxmode_posvec = GX_MTXMODE_POSITION_VECTOR;

    NNS_G3dGlb.cmd1 = (G3OP_LIGHT_VECTOR << 0)  |
                      (G3OP_LIGHT_VECTOR << 8)  |
                      (G3OP_LIGHT_VECTOR << 16) |
                      (G3OP_LIGHT_VECTOR << 24);

    NNS_G3dGlb.cmd2 = (G3OP_DIF_AMB << 0)       |
                      (G3OP_SPE_EMI << 8)       |
                      (G3OP_POLYGON_ATTR << 16) |
                      (G3OP_VIEWPORT << 24);

    NNS_G3dGlb.cmd3 = (G3OP_LIGHT_COLOR << 0)  |
                      (G3OP_LIGHT_COLOR << 8)  |
                      (G3OP_LIGHT_COLOR << 16) |
                      (G3OP_LIGHT_COLOR << 24);

    NNS_G3dGlb.cmd4 = (G3OP_MTX_MULT_4x3 << 0) |
                      (G3OP_MTX_SCALE << 8)    |
                      (G3OP_TEXIMAGE_PARAM << 16);
        
    // Set as unit matrices.
    MTX_Identity43(&NNS_G3dGlb.cameraMtx);
    MTX_Identity44(&NNS_G3dGlb.projMtx);

    // Set light vector to invalid value
    NNS_G3dGlb.lightVec[0] = GX_PACK_LIGHTVECTOR_PARAM(0, -FX16_SQRT1_3, -FX16_SQRT1_3, -FX16_SQRT1_3);
    NNS_G3dGlb.lightVec[1] = GX_PACK_LIGHTVECTOR_PARAM(1, -FX16_ONE, 0, 0);
    NNS_G3dGlb.lightVec[2] = GX_PACK_LIGHTVECTOR_PARAM(2,  FX16_ONE - 1, 0, 0);
    NNS_G3dGlb.lightVec[3] = GX_PACK_LIGHTVECTOR_PARAM(3, 0, -FX16_ONE, 0);

    // Set material color could change
    NNS_G3dGlb.prmMatColor0 = GX_PACK_DIFFAMB_PARAM(GX_RGB(16, 16, 16),
                                                    GX_RGB(16, 16, 16),
                                                    TRUE);
    NNS_G3dGlb.prmMatColor1 = GX_PACK_SPECEMI_PARAM(GX_RGB(16, 16, 16),
                                                    GX_RGB(16, 16, 16),
                                                    TRUE);

    // Set PolygonAttr to safe settings
    NNS_G3dGlb.prmPolygonAttr = GX_PACK_POLYGONATTR_PARAM(0xf,  // Light ON
                                                          GX_POLYGONMODE_MODULATE,
                                                          GX_CULL_BACK,
                                                          0,  // PolygonID
                                                          31, // Alpha
                                                          GX_POLYGON_ATTR_MISC_NONE);

    NNS_G3dGlb.prmViewPort = GX_PACK_VIEWPORT_PARAM(0, 0, 255, 191);
    
    NNS_G3dGlb.lightColor[0] = GX_PACK_LIGHTCOLOR_PARAM(0, GX_RGB(31, 31, 31));
    NNS_G3dGlb.lightColor[1] = GX_PACK_LIGHTCOLOR_PARAM(1, GX_RGB(31,  0,  0));
    NNS_G3dGlb.lightColor[2] = GX_PACK_LIGHTCOLOR_PARAM(2, GX_RGB( 0, 31,  0));
    NNS_G3dGlb.lightColor[3] = GX_PACK_LIGHTCOLOR_PARAM(3, GX_RGB( 0,  0, 31));

    // Base SRT
    NNS_G3dGlb.prmBaseTrans.x = 0;
    NNS_G3dGlb.prmBaseTrans.y = 0;
    NNS_G3dGlb.prmBaseTrans.z = 0;

    MTX_Identity33(&NNS_G3dGlb.prmBaseRot);

    NNS_G3dGlb.prmBaseScale.x = FX32_ONE;
    NNS_G3dGlb.prmBaseScale.y = FX32_ONE;
    NNS_G3dGlb.prmBaseScale.z = FX32_ONE;
    NNS_G3dGlb.prmTexImageParam = 0;

    NNS_G3dGlb.flag = (NNSG3dGlbFlag) 0;

    NNS_G3dGlb.camPos.x = NNS_G3dGlb.camPos.y = NNS_G3dGlb.camPos.z = 0;
    NNS_G3dGlb.camUp.x = NNS_G3dGlb.camUp.z = 0;
    NNS_G3dGlb.camUp.y = FX32_ONE;
    NNS_G3dGlb.camTarget.x = NNS_G3dGlb.camTarget.y = 0;
    NNS_G3dGlb.camTarget.z = -FX32_ONE;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbFlushP

    Reflect content of NNSG3dGlb in the geometry engine.
    Projection change matrix is set in current projection matrix.
    In current position coordinate array, thing combining camera matrix and world change matrix are set.

    Color etc. is sent in case a Sbc SHP command comes unexpectedly.
    After processing the command, mode is POSITION/VECTOR.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbFlushP(void)
{
    static const u32 sz = (sizeof(NNS_G3dGlb.cmd0)              +
                           sizeof(NNS_G3dGlb.mtxmode_proj)      +
                           sizeof(NNS_G3dGlb.projMtx)           +
                           sizeof(NNS_G3dGlb.mtxmode_posvec)    +
                           sizeof(NNS_G3dGlb.cameraMtx)         +
                           sizeof(NNS_G3dGlb.cmd1)              +
                           sizeof(NNS_G3dGlb.lightVec[0]) * 4   +
                           sizeof(NNS_G3dGlb.cmd2)              +
                           sizeof(NNS_G3dGlb.prmMatColor0)      +
                           sizeof(NNS_G3dGlb.prmMatColor1)      +
                           sizeof(NNS_G3dGlb.prmPolygonAttr)    +
                           sizeof(NNS_G3dGlb.prmViewPort)       +
                           sizeof(NNS_G3dGlb.cmd4)              +
                           sizeof(NNS_G3dGlb.prmBaseTrans)      +
                           sizeof(NNS_G3dGlb.prmBaseRot)        +
                           sizeof(NNS_G3dGlb.prmBaseScale)      +
                           sizeof(NNS_G3dGlb.prmTexImageParam)  +
                           sizeof(NNS_G3dGlb.cmd3)              +
                           sizeof(NNS_G3dGlb.lightColor[0]) * 4) / 4;

    NNS_G3D_ASSERT(NNS_G3dGlb.cmd0 == (G3OP_MTX_MODE << 0)     |
                                      (G3OP_MTX_LOAD_4x4 << 8) |
                                      (G3OP_MTX_MODE << 16)    |
                                      (G3OP_MTX_LOAD_4x3 << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.mtxmode_proj == GX_MTXMODE_PROJECTION);
    NNS_G3D_ASSERT(NNS_G3dGlb.mtxmode_posvec == GX_MTXMODE_POSITION_VECTOR);
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd1 == (G3OP_LIGHT_VECTOR << 0)  |
                                      (G3OP_LIGHT_VECTOR << 8)  |
                                      (G3OP_LIGHT_VECTOR << 16) |
                                      (G3OP_LIGHT_VECTOR << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd2 == (G3OP_DIF_AMB << 0)       |
                                      (G3OP_SPE_EMI << 8)       |
                                      (G3OP_POLYGON_ATTR << 16) |
                                      (G3OP_VIEWPORT << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd3 == (G3OP_LIGHT_COLOR << 0)  |
                                      (G3OP_LIGHT_COLOR << 8)  |
                                      (G3OP_LIGHT_COLOR << 16) |
                                      (G3OP_LIGHT_COLOR << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd4 == (G3OP_MTX_MULT_4x3 << 0) |
                                      (G3OP_MTX_SCALE << 8)    |
                                      (G3OP_TEXIMAGE_PARAM << 16));

    NNS_G3dGeBufferData_N((u32*)&NNS_G3dGlb.cmd0, sz);

    NNS_G3dGlb.flag &= ~NNS_G3D_GLB_FLAG_FLUSH_WVP;
    NNS_G3dGlb.flag &= ~NNS_G3D_GLB_FLAG_FLUSH_VP;
    //
    // After processing the command, mode is POSITION/VECTOR.
    //
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbFlushVP

    Reflect content of NNSG3dGlb in the geometry engine.
    In current projection matrix, thing combining projection change matrix and camera matrix is set.
    In current position coordinate matrix, world change matrix is set.

    Color etc. is sent in case a Sbc SHP command comes unexpectedly.
    After processing the command, mode is POSITION/VECTOR.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbFlushVP(void)
{
    static const u32 sz = (sizeof(NNS_G3dGlb.cmd1)              +
                           sizeof(NNS_G3dGlb.lightVec[0]) * 4   +
                           sizeof(NNS_G3dGlb.cmd2)              +
                           sizeof(NNS_G3dGlb.prmMatColor0)      +
                           sizeof(NNS_G3dGlb.prmMatColor1)      +
                           sizeof(NNS_G3dGlb.prmPolygonAttr)    +
                           sizeof(NNS_G3dGlb.prmViewPort)       +
                           sizeof(NNS_G3dGlb.cmd3)              +
                           sizeof(NNS_G3dGlb.lightColor[0]) * 4 +
                           sizeof(NNS_G3dGlb.cmd4)              +
                           sizeof(NNS_G3dGlb.prmBaseRot)        +
                           sizeof(NNS_G3dGlb.prmBaseTrans)      +
                           sizeof(NNS_G3dGlb.prmBaseScale)      +
                           sizeof(NNS_G3dGlb.prmTexImageParam)) / 4;

    NNS_G3D_ASSERT(NNS_G3dGlb.cmd0 == (G3OP_MTX_MODE << 0)     |
                                      (G3OP_MTX_LOAD_4x4 << 8) |
                                      (G3OP_MTX_MODE << 16)    |
                                      (G3OP_MTX_LOAD_4x3 << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.mtxmode_proj == GX_MTXMODE_PROJECTION);
    NNS_G3D_ASSERT(NNS_G3dGlb.mtxmode_posvec == GX_MTXMODE_POSITION_VECTOR);
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd1 == (G3OP_LIGHT_VECTOR << 0)  |
                                      (G3OP_LIGHT_VECTOR << 8)  |
                                      (G3OP_LIGHT_VECTOR << 16) |
                                      (G3OP_LIGHT_VECTOR << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd2 == (G3OP_DIF_AMB << 0)       |
                                      (G3OP_SPE_EMI << 8)       |
                                      (G3OP_POLYGON_ATTR << 16) |
                                      (G3OP_VIEWPORT << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd3 == (G3OP_LIGHT_COLOR << 0)  |
                                      (G3OP_LIGHT_COLOR << 8)  |
                                      (G3OP_LIGHT_COLOR << 16) |
                                      (G3OP_LIGHT_COLOR << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd4 == (G3OP_MTX_MULT_4x3 << 0) |
                                      (G3OP_MTX_SCALE << 8)    |
                                      (G3OP_TEXIMAGE_PARAM << 16));

    // Switch to projection matrix mode and set projection matrix.
    NNS_G3dGeBufferOP_N((G3OP_MTX_MODE << 0) | (G3OP_MTX_LOAD_4x4 << 8),
                        (u32*)&NNS_G3dGlb.mtxmode_proj,
                        (sizeof(NNS_G3dGlb.mtxmode_proj) +
                         sizeof(NNS_G3dGlb.projMtx)) / 4);    

    // Multiply camera matrix by projection matrix
    NNS_G3dGeBufferOP_N((G3OP_MTX_MULT_4x3 << 0),
                        (u32*)&NNS_G3dGlb.cameraMtx,
                        sizeof(NNS_G3dGlb.cameraMtx) / 4);

    NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
    NNS_G3dGeBufferOP_N(G3OP_MTX_IDENTITY, (u32*)&NNS_G3dGlb.cmd1, sz);

    NNS_G3dGlb.flag &= ~NNS_G3D_GLB_FLAG_FLUSH_WVP;
    NNS_G3dGlb.flag |=  NNS_G3D_GLB_FLAG_FLUSH_VP;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbFlushWVP

    Reflect content of NNSG3dGlb in the geometry engine.
    In current projection matrix, thing combining projection change matrix, camera matrix, and world change matrix is set.
    In current position coordinate matrix, unit matrix is set.

    Color etc. is sent in case a Sbc SHP command comes unexpectedly.
    After processing the command, mode is POSITION/VECTOR.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbFlushWVP(void)
{
    static const u32 sz = (sizeof(NNS_G3dGlb.cmd1)              +
                           sizeof(NNS_G3dGlb.lightVec[0]) * 4   +
                           sizeof(NNS_G3dGlb.cmd2)              +
                           sizeof(NNS_G3dGlb.prmMatColor0)      +
                           sizeof(NNS_G3dGlb.prmMatColor1)      +
                           sizeof(NNS_G3dGlb.prmPolygonAttr)    +
                           sizeof(NNS_G3dGlb.prmViewPort)       +
                           sizeof(NNS_G3dGlb.cmd3)              +
                           sizeof(NNS_G3dGlb.lightColor[0]) * 4) / 4;

    NNS_G3D_ASSERT(NNS_G3dGlb.cmd0 == (G3OP_MTX_MODE << 0)     |
                                      (G3OP_MTX_LOAD_4x4 << 8) |
                                      (G3OP_MTX_MODE << 16)    |
                                      (G3OP_MTX_LOAD_4x3 << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.mtxmode_proj == GX_MTXMODE_PROJECTION);
    NNS_G3D_ASSERT(NNS_G3dGlb.mtxmode_posvec == GX_MTXMODE_POSITION_VECTOR);
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd1 == (G3OP_LIGHT_VECTOR << 0)  |
                                      (G3OP_LIGHT_VECTOR << 8)  |
                                      (G3OP_LIGHT_VECTOR << 16) |
                                      (G3OP_LIGHT_VECTOR << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd2 == (G3OP_DIF_AMB << 0)       |
                                      (G3OP_SPE_EMI << 8)       |
                                      (G3OP_POLYGON_ATTR << 16) |
                                      (G3OP_VIEWPORT << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd3 == (G3OP_LIGHT_COLOR << 0)  |
                                      (G3OP_LIGHT_COLOR << 8)  |
                                      (G3OP_LIGHT_COLOR << 16) |
                                      (G3OP_LIGHT_COLOR << 24));
    NNS_G3D_ASSERT(NNS_G3dGlb.cmd4 == (G3OP_MTX_MULT_4x3 << 0) |
                                      (G3OP_MTX_SCALE << 8)    |
                                      (G3OP_TEXIMAGE_PARAM << 16));


    // Switch to projection matrix mode and set projection matrix.
    NNS_G3dGeBufferOP_N((G3OP_MTX_MODE << 0) | (G3OP_MTX_LOAD_4x4 << 8),
                        (u32*)&NNS_G3dGlb.mtxmode_proj,
                        (sizeof(NNS_G3dGlb.mtxmode_proj) +
                         sizeof(NNS_G3dGlb.projMtx)) / 4);

    // Multiply camera matrix by projection matrix
    NNS_G3dGeBufferOP_N((G3OP_MTX_MULT_4x3 << 0),
                        (u32*)&NNS_G3dGlb.cameraMtx,
                        sizeof(NNS_G3dGlb.cameraMtx) / 4);

    // Multiply the base matrix by the projection matrix
    NNS_G3dGeBufferOP_N((G3OP_MTX_MULT_4x3 << 0) | (G3OP_MTX_SCALE << 8),
                        (u32*)&NNS_G3dGlb.prmBaseRot,
                        (sizeof(NNS_G3dGlb.prmBaseRot) +
                         sizeof(NNS_G3dGlb.prmBaseTrans)   +
                         sizeof(NNS_G3dGlb.prmBaseScale)) / 4);

    NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);

    // Apply inverse matrix of base rotation matrix to light vector (change from world to local)
    {
        MtxFx43 inv;
        int result;
        result = MTX_Inverse33(&NNS_G3dGlb.prmBaseRot ,(MtxFx33*)&inv);
        NNS_G3D_ASSERT(!result);
        NNS_G3dGeLoadMtx43(&inv);
    }

    NNS_G3dGeBufferData_N((u32*)&NNS_G3dGlb.cmd1, sz);

    NNS_G3dGeIdentity();

    NNS_G3dGeBufferOP_N(G3OP_TEXIMAGE_PARAM, &NNS_G3dGlb.prmTexImageParam, 1);

    NNS_G3dGlb.flag |= NNS_G3D_GLB_FLAG_FLUSH_WVP;
    NNS_G3dGlb.flag &= ~NNS_G3D_GLB_FLAG_FLUSH_VP;

    //
    // After processing the command, mode is POSITION/VECTOR.
    //
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbSetBaseTrans

    Sets Translation of NNS_G3dGlb base matrix (world change matrix).
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbSetBaseTrans(const VecFx32* pTrans)
{
    NNS_G3D_NULL_ASSERT(pTrans);

    NNS_G3dGlb.prmBaseTrans = *pTrans;
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVBASE_UPTODATE       |
                         NNS_G3D_GLB_FLAG_INVBASECAMERA_UPTODATE |
                         NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbSetBaseScale

    Sets Scale of NNS_G3dGlb base matrix (world change matrix).
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbSetBaseScale(const VecFx32* pScale)
{
    NNS_G3D_NULL_ASSERT(pScale);

    NNS_G3dGlb.prmBaseScale = *pScale;
    NNS_G3dGlb.flag &= ~(NNS_G3D_GLB_FLAG_INVBASE_UPTODATE       |
                         NNS_G3D_GLB_FLAG_INVBASECAMERA_UPTODATE |
                         NNS_G3D_GLB_FLAG_BASECAMERA_UPTODATE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbLightVector

    Change the light vector of the global state.
    Geometry engine does not reflect the change until NNS_G3dGlbFlush is called.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbLightVector(GXLightId lightID,
                      fx16 x,
                      fx16 y,
                      fx16 z)
{
    GX_LIGHTID_ASSERT(lightID);

    NNS_G3D_ASSERT(x >= -FX16_ONE && x < FX16_ONE);
    NNS_G3D_ASSERT(y >= -FX16_ONE && y < FX16_ONE);
    NNS_G3D_ASSERT(z >= -FX16_ONE && z < FX16_ONE);

    NNS_G3dGlb.lightVec[lightID] = GX_PACK_LIGHTVECTOR_PARAM(lightID, x, y, z);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbLightColor

    Sets light color of NNS_G3dGlb.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbLightColor(GXLightId lightID, GXRgb rgb)
{
    GX_LIGHTID_ASSERT(lightID);
    GXRGB_ASSERT(rgb);

    NNS_G3dGlb.lightColor[lightID] = GX_PACK_LIGHTCOLOR_PARAM(lightID, rgb);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbMaterialColorDiffAmb

    Sets diffuse and ambient attributes of NNS_G3dGlb.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbMaterialColorDiffAmb(GXRgb diffuse,
                               GXRgb ambient,
                               BOOL  IsSetVtxColor)
{
    GXRGB_ASSERT(diffuse);
    GXRGB_ASSERT(ambient);

    NNS_G3dGlb.prmMatColor0 = GX_PACK_DIFFAMB_PARAM(diffuse, ambient, IsSetVtxColor);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbMaterialColorSpecEmi

    Sets the specular and emission attributes of NNS_G3dGlb.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbMaterialColorSpecEmi(GXRgb specular,
                               GXRgb emission,
                               BOOL IsShininess)
{
    GXRGB_ASSERT(specular);
    GXRGB_ASSERT(emission);

    NNS_G3dGlb.prmMatColor1 = GX_PACK_SPECEMI_PARAM(specular, emission, IsShininess);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbPolygonAttr

    Sets PolygonAttr of NNS_G3dGlb.
 *---------------------------------------------------------------------------*/
void 
NNS_G3dGlbPolygonAttr(int light,
                      GXPolygonMode polyMode,
                      GXCull cullMode,
                      int polygonID,
                      int alpha,
                      int misc // GXPolygonAttrMisc
                      )
{
    GX_LIGHTMASK_ASSERT(light);
    GX_POLYGONMODE_ASSERT(polyMode);
    GX_CULL_ASSERT(cullMode);
    GX_POLYGON_ATTR_POLYGONID_ASSERT(polygonID);
    GX_POLYGON_ATTR_ALPHA_ASSERT(alpha);

    NNS_G3dGlb.prmPolygonAttr = GX_PACK_POLYGONATTR_PARAM(light,
                                                          polyMode,
                                                          cullMode,
                                                          polygonID,
                                                          alpha,
                                                          misc);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetInvW

    Returns a pointer to the inverse matrix of the modeling matrix.
 *---------------------------------------------------------------------------*/
const MtxFx43*
NNS_G3dGlbGetInvW(void)
{
    if (!(NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_INVBASE_UPTODATE))
    {
        MtxFx43 tmp;
        s32 result;
        MTX_ScaleApply43((MtxFx43*)&NNS_G3dGlb.prmBaseRot,
                         &tmp,
                         NNS_G3dGlb.prmBaseScale.x,
                         NNS_G3dGlb.prmBaseScale.y,
                         NNS_G3dGlb.prmBaseScale.z);
        result = MTX_Inverse43(&tmp, &NNS_G3dGlb.invBaseMtx);
        NNS_G3D_ASSERT(!result);
        NNS_G3dGlb.flag |= NNS_G3D_GLB_FLAG_INVBASE_UPTODATE;
    }
    return &NNS_G3dGlb.invBaseMtx;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetInvV

    Returns a pointer to the inverse matrix of the camera matrix.
 *---------------------------------------------------------------------------*/
const MtxFx43*
NNS_G3dGlbGetInvV(void)
{
    if (!(NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_INVCAMERA_UPTODATE))
    {
        s32 result = MTX_Inverse43(&NNS_G3dGlb.cameraMtx, &NNS_G3dGlb.invCameraMtx);
        NNS_G3D_ASSERT(!result);
        NNS_G3dGlb.flag |= NNS_G3D_GLB_FLAG_INVCAMERA_UPTODATE;
    }
    return &NNS_G3dGlb.invCameraMtx;
}


static int mtx_inverse44(const MtxFx44 * pSrc, MtxFx44 * pDst)
{
    MtxFx44 gjm;
    int i, j, k;
    fx64c ww;
    fx32 w;

    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    MTX_Copy44(pSrc, &gjm);
    MTX_Identity44(pDst);

    for (i = 0; i < 4; ++i)
    {
        fx32 max = 0;
        int swp = i;

        // ---- partial pivoting -----
        for( k = i ; k < 4 ; k++ )
        {
            fx32 ftmp = (gjm.m[k][i] < 0) ? -gjm.m[k][i] : gjm.m[k][i];

            if ( ftmp > max )
            {
                max = ftmp;
                swp = k;
            }
        }

        // check singular matrix
        //(or can't solve inverse matrix with this algorithm)
        if ( max == 0 )
        {
            return -1;
        }

        // swap row
        if( swp != i )
        {
            for ( k = 0 ; k < 4 ; k++ )
            {
                fx32 tmp;
                tmp = gjm.m[i][k];
                gjm.m[i][k] = gjm.m[swp][k];
                gjm.m[swp][k] = tmp;

                tmp = pDst->m[i][k];
                pDst->m[i][k] = pDst->m[swp][k];
                pDst->m[swp][k] = tmp;
            }
        }

        // ---- pivoting end ----
        ww = FX_InvFx64c(gjm.m[i][i]);
        for ( j = 0 ; j < 4 ; ++j )
        {
            gjm.m[i][j] = FX_Mul32x64c(gjm.m[i][j], ww);
            pDst->m[i][j] = FX_Mul32x64c(pDst->m[i][j], ww);
        }
        
        for ( k = 0 ; k < 4 ; ++k )
        {
            if ( k == i )
                continue;
            
            w = gjm.m[k][i];
            for ( j = 0 ; j < 4 ; ++j )
            {
                gjm.m[k][j]   -= ((fx64)w * gjm.m[i][j]) >> FX32_SHIFT;
                pDst->m[k][j] -= ((fx64)w * pDst->m[i][j]) >> FX32_SHIFT;
            }
        }
    }
    return 0;
}




/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetInvP

    Returns a pointer to the inverse matrix of the projection matrix.
 *---------------------------------------------------------------------------*/
const MtxFx44*
NNS_G3dGlbGetInvP(void)
{
    if (!(NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE))
    {
        s32 result = mtx_inverse44(&NNS_G3dGlb.projMtx, &NNS_G3dGlb.invProjMtx);
        NNS_G3D_ASSERT(!result);
        NNS_G3dGlb.flag |= NNS_G3D_GLB_FLAG_INVPROJ_UPTODATE;
    }
    return &NNS_G3dGlb.invProjMtx;
}



static void
calcSrtCameraMtx_(void)
{
    s32 result;

    MTX_Concat43((MtxFx43*)&NNS_G3dGlb.prmBaseRot,
                 &NNS_G3dGlb.cameraMtx,
                 &NNS_G3dGlb.srtCameraMtx);
    MTX_ScaleApply43(&NNS_G3dGlb.srtCameraMtx,
                     &NNS_G3dGlb.srtCameraMtx,
                     NNS_G3dGlb.prmBaseScale.x,
                     NNS_G3dGlb.prmBaseScale.y,
                     NNS_G3dGlb.prmBaseScale.z);
    result = MTX_Inverse43(&NNS_G3dGlb.srtCameraMtx,
                           &NNS_G3dGlb.invSrtCameraMtx);
    NNS_G3D_ASSERT(!result);
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetWV

    Returns a pointer to the matrix multiplied by the BaseSRT matrix in the camera matrix.
 *---------------------------------------------------------------------------*/
const MtxFx43*
NNS_G3dGlbGetWV(void)
{
    if (!(NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_SRTCAMERA_UPTODATE))
    {
        calcSrtCameraMtx_();
        NNS_G3dGlb.flag |= NNS_G3D_GLB_FLAG_SRTCAMERA_UPTODATE;
    }
    
    return &NNS_G3dGlb.srtCameraMtx;
}


/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetInvWV

    Returns a pointer to the inverse matrix of the matrix multiplied by the BaseSRT matrix in the camera matrix.
 *---------------------------------------------------------------------------*/
const MtxFx43*
NNS_G3dGlbGetInvWV(void)
{
    if (!(NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_SRTCAMERA_UPTODATE))
    {
        calcSrtCameraMtx_();
        NNS_G3dGlb.flag |= NNS_G3D_GLB_FLAG_SRTCAMERA_UPTODATE;
    }
    
    return &NNS_G3dGlb.invSrtCameraMtx;
}


const MtxFx44*
NNS_G3dGlbGetInvVP(void)
{
    if (!(NNS_G3dGlb.flag & NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE))
    {
        MtxFx44 tmp;
        const MtxFx43* invCam  = NNS_G3dGlbGetInvV();
        const MtxFx44* invProj = NNS_G3dGlbGetInvP();
        
        MTX_Copy43To44(invCam, &tmp);
        MTX_Concat44(invProj, &tmp, &NNS_G3dGlb.invCameraProjMtx);

        NNS_G3dGlb.flag |= NNS_G3D_GLB_FLAG_INVCAMERAPROJ_UPTODATE;
    }
    return &NNS_G3dGlb.invCameraProjMtx;
}




/*---------------------------------------------------------------------------*
    NNS_G3dGlbGetViewPort

    Extracts the viewport coordinates that are set in NNS_G3dGlb.
 *---------------------------------------------------------------------------*/
void
NNS_G3dGlbGetViewPort(int* px1, int* py1, int* px2, int* py2)
{
    if (px1)
        *px1 = (int)( NNS_G3dGlb.prmViewPort & 0xff );
    if (py1)
        *py1 = (int)( (NNS_G3dGlb.prmViewPort >> 8) & 0xff );
    if (px2)
        *px2 = (int)( (NNS_G3dGlb.prmViewPort >> 16) & 0xff );
    if (py2)
        *py2 = (int)( (NNS_G3dGlb.prmViewPort >> 24) & 0xff );
}







////////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//

/*---------------------------------------------------------------------------*
    NNS_G3dGlb

    This is a structure that stores the global states such as cameras and lighting used by G3D.
 *---------------------------------------------------------------------------*/
NNSG3dGlb NNS_G3dGlb;


