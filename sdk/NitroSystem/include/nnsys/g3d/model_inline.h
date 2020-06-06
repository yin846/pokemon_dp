/*---------------------------------------------------------------------------*
  Project:  NitroSystem - G3D(Hokotate) -
  File:     model_inline.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: model_inline.h,v $
  Revision 1.4  10/12/2004 02:18:34  nishida_kenji
  Add accessors for PolygonAttr's flags.

  Revision 1.3  07/20/2004 10:43:49  nishida_kenji
  add comments

  Revision 1.2  07/20/2004 06:47:02  nishida_kenji
  small bug fix

  Revision 1.1  07/16/2004 07:23:08  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNSG3D_MODEL_INLINE_
#define NNSG3D_MODEL_INLINE_

//
// DO NOT INCLUDE THIS FILE DIRECTLY
//

#ifdef __cplusplus
extern "C" {
#endif

void NNSi_G3dModifyMatFlag(NNSG3dResMdl* pMdl, BOOL isOn, NNSG3dMatFlag flag);
void NNSi_G3dModifyPolygonAttrMask(NNSG3dResMdl* pMdl, BOOL isOn, u32 mask);

//
// To Use Global data
//

/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbDiff

    This function uses the diffuse value configured by NNS_G3dGlbMaterialColorDiffAmb
    as a diffuse value for the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbDiff(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          FALSE,
                          NNS_G3D_MATFLAG_DIFFUSE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbAmb

    This function uses the ambient value configured by NNS_G3dGlbMaterialColorDiffAmb
    as an ambient value for the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbAmb(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          FALSE,
                          NNS_G3D_MATFLAG_AMBIENT);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbSpec

    This function uses the specular value configured by NNS_G3dGlbMaterialColorSpecEmi
    as a specular value for the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbSpec(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          FALSE,
                          NNS_G3D_MATFLAG_SPECULAR);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbEmi

    This function uses the emission value configured by NNS_G3dGlbMaterialColorSpecEmi
    as an emission value for the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbEmi(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          FALSE,
                          NNS_G3D_MATFLAG_EMISSION);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbLightEnableFlag

    This function uses the light enable flag configured by NNS_G3dGlbPolygonAttr
    as the light enable flag of the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbLightEnableFlag(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_LE_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbPolygonMode

    This function uses the polygon mode configured by NNS_G3dGlbPolygonAttr
    as the polygon mode of the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbPolygonMode(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_PM_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbCullMode

    This function uses the cull mode configured by NNS_G3dGlbPolygonAttr
    as the cull mode of the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbCullMode(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_BK_MASK |
                                  REG_G3_POLYGON_ATTR_FR_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbPolygonID

    This function uses the polygon ID configured by NNS_G3dGlbPolygonAttr
    as the polygon ID for the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbPolygonID(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_ID_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbAlpha

    This function uses the alpha value configured by NNS_G3dGlbPolygonAttr
    as the alpha value of the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbAlpha(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_ALPHA_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbFogEnableFlag

    This function uses the fog enable flag configured by NNS_G3dGlbPolygonAttr
    as the fog enable flag of the material inside the model.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbFogEnableFlag(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_FE_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbDepthTestCond

    This function uses the depth test conditions configured by NNS_G3dGlbPolygonAttr
    as the depth test conditions for the material inside the model.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbDepthTestCond(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_DT_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlb1Dot

    This function uses the 1-dot polygon display specifications configured by NNS_G3dGlbPolygonAttr
    as the 1-dot polygon display specifications for the material inside the model.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlb1Dot(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_D1_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbFarClip

    This function uses the FAR plane intersecting polygon display specifications configured by NNS_G3dGlbPolygonAttr
    as the FAR plane intersecting polygon display specifications for the material inside the model.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbFarClip(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_FC_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseGlbXLDepthUpdate

    This function uses the translucent polygon depth value update enable flag configured by NNS_G3dGlbPolygonAttr
    as the translucent polygon depth value update enable flag for the material inside the model.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseGlbXLDepthUpdate(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  FALSE,
                                  REG_G3_POLYGON_ATTR_XL_MASK);
}


//
// To Use Model's Data
//

/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlDiff

    This function uses the diffuse values for the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlDiff(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          TRUE,
                          NNS_G3D_MATFLAG_DIFFUSE);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlAmb

    This function uses the ambient values for the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlAmb(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          TRUE,
                          NNS_G3D_MATFLAG_AMBIENT);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlSpec

    This function uses the specular values for the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlSpec(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          TRUE,
                          NNS_G3D_MATFLAG_SPECULAR);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlEmi

    This function uses the emission values for the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlEmi(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyMatFlag(pMdl,
                          TRUE,
                          NNS_G3D_MATFLAG_EMISSION);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlLightEnableFlag

    This function uses the light enable flags of the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlLightEnableFlag(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_LE_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlPolygonMode

    This function uses the polygon modes of the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlPolygonMode(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_PM_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlCullMode

    This function uses the cull modes of the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlCullMode(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_BK_MASK |
                                  REG_G3_POLYGON_ATTR_FR_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlPolygonID

    This function uses the polygon IDs of the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlPolygonID(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_ID_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlAlpha

    This function uses the alpha values of the individual materials that are configured inside the model resource.
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlAlpha(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_ALPHA_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlFogEnableFlag

    This function uses the fog enable flags of the individual materials that are configured inside the model resource.
     
    (this is the default configuration of the .nsbmd file)
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlFogEnableFlag(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_FE_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlDepthTestCond

    This function uses the depth test conditions for the individual materials that are configured inside the model resource.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlDepthTestCond(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_DT_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdl1Dot

    This function uses the 1-dot polygon display specifications for the individual materials  that are configured inside the model resource.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdl1Dot(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_D1_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlFarClip

    This function uses the FAR plane intersecting polygon display specifications for the individual materials  that are configured inside the model resource.
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlFarClip(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_FC_MASK);
}


/*---------------------------------------------------------------------------*
    NNS_G3dMdlUseMdlXLDepthUpdate

    Use the update enable flag when updating the depth value of semi-transparent polygons set within the model resource.
     
 *---------------------------------------------------------------------------*/
NNS_G3D_INLINE void
NNS_G3dMdlUseMdlXLDepthUpdate(NNSG3dResMdl* pMdl)
{
    NNSi_G3dModifyPolygonAttrMask(pMdl,
                                  TRUE,
                                  REG_G3_POLYGON_ATTR_XL_MASK);
}





#ifdef __cplusplus
}
#endif

#endif

