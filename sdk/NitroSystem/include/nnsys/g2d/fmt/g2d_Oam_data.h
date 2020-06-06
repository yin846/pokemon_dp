/*---------------------------------------------------------------------------*
  Project:   NNS - 2D
  File:     g2d_Oam_data.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Oam_data.h,v $
  Revision 1.11  06/29/2005 01:01:03 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.10  01/24/2005 06:26:54 AM  kitani_toshikazu
  Corrected the copyright statement.

  Revision 1.9  01/19/2005 07:41:43 AM  kitani_toshikazu
  Changed the OAM size table to global variable.

  Revision 1.8  10/28/2004 12:27:28 AM  kitani_toshikazu
  (none)

  Revision 1.7  10/28/2004 12:08:28 AM  kitani_toshikazu
  Put switch code in NNS_G2dGetOamSizeXXX() in table form.

  Revision 1.6  10/12/2004 03:00:10 AM  kitani_toshikazu
  (none)

  Revision 1.5  10/08/2004 04:41:12 AM  kitani_toshikazu
  (none)

  Revision 1.4  10/07/2004 12:59:39 PM  kitani_toshikazu
  Added NNS_G2dGetOamSizeX() and such

  Revision 1.3  2004/06/23 06:47:52 AM  nishida_kenji
  Fix header comments.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:55 AM  kitani_toshikazu
  append G2D Lib.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_OAM_DATA_H_
#define NNS_G2D_OAM_DATA_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_OAM_AFFINE_IDX_NONE                0xFFFE        // The Affine Index value to specify when registering an OAM that does not use Affine in the OAM manager.

typedef struct objSize
{
    u16    x;
    u16    y;
}objSize;
/*
(shape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT
(shape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT
*/
/*
GX_OAM_SHAPE_8x8   = (0 << GX_OAM_ATTR01_SHAPE_SHIFT) | (0 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_16x16 = (0 << GX_OAM_ATTR01_SHAPE_SHIFT) | (1 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_32x32 = (0 << GX_OAM_ATTR01_SHAPE_SHIFT) | (2 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_64x64 = (0 << GX_OAM_ATTR01_SHAPE_SHIFT) | (3 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_16x8  = (1 << GX_OAM_ATTR01_SHAPE_SHIFT) | (0 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_32x8  = (1 << GX_OAM_ATTR01_SHAPE_SHIFT) | (1 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_32x16 = (1 << GX_OAM_ATTR01_SHAPE_SHIFT) | (2 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_64x32 = (1 << GX_OAM_ATTR01_SHAPE_SHIFT) | (3 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_8x16  = (2 << GX_OAM_ATTR01_SHAPE_SHIFT) | (0 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_8x32  = (2 << GX_OAM_ATTR01_SHAPE_SHIFT) | (1 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_16x32 = (2 << GX_OAM_ATTR01_SHAPE_SHIFT) | (2 << GX_OAM_ATTR01_SIZE_SHIFT),
GX_OAM_SHAPE_32x64 = (2 << GX_OAM_ATTR01_SHAPE_SHIFT) | (3 << GX_OAM_ATTR01_SIZE_SHIFT)
*/

//
// When referencing from tool, declare it in the inline function defined in header
//
#ifdef NNS_FROM_TOOL
    #define NNS_G2D_OBJSIZE_TBL_STATIC static 
#else // NNS_FROM_TOOL
    #define NNS_G2D_OBJSIZE_TBL_STATIC
    extern const u16 NNSi_objSizeHTbl[3][4];
    extern const u16 NNSi_objSizeWTbl[3][4];
#endif // NNS_FROM_TOOL


#define NNS_G2D_DEFINE_NNSI_OBJSIZEWTBL        \
NNS_G2D_OBJSIZE_TBL_STATIC                     \
const u16 NNSi_objSizeWTbl[3][4]=               \
{                                              \
    {                                          \
        8,                                     \
        16,                                    \
        32,                                    \
        64                                     \
    },                                         \
    {                                          \
        16,                                    \
        32,                                    \
        32,                                    \
        64                                     \
    },                                         \
    {                                          \
        8,                                     \
        8,                                     \
        16,                                    \
        32                                     \
    }                                          \
}                                              \


#define NNS_G2D_DEFINE_NNSI_OBJSIZEHTBL        \
NNS_G2D_OBJSIZE_TBL_STATIC                     \
const u16 NNSi_objSizeHTbl[3][4]=               \
{                                              \
    {                                          \
        8,                                     \
        16,                                    \
        32,                                    \
        64,                                    \
    },                                         \
    {                                          \
        8,                                     \
        8,                                     \
        16,                                    \
        32,                                    \
    },                                         \
    {                                          \
        16,                                    \
        32,                                    \
        32,                                    \
        64                                     \
    }                                          \
}                                              \


//------------------------------------------------------------------------------
// Moved from g2di_OamUtil.h
static inline GXOamShape NNS_G2dGetOAMSize( const GXOamAttr* oamAttr )
{
    //const GXOamShape  result = (GXOamShape)( oamAttr->shape );
    const GXOamShape  result = (GXOamShape)((GX_OAM_ATTR01_SHAPE_MASK | GX_OAM_ATTR01_SIZE_MASK) & oamAttr->attr01);
    
    // GX_OAM_SHAPE_ASSERT(result);
    return result;
}

//------------------------------------------------------------------------------
// Moved from g2di_OamUtil.h
static inline int NNS_G2dGetOamSizeX( const GXOamShape* oamShape )
{
    // NNS_G2D_NULL_ASSERT(oamShape);
#ifdef NNS_FROM_TOOL
    NNS_G2D_DEFINE_NNSI_OBJSIZEWTBL;
#endif // NNS_FROM_TOOL
    return NNSi_objSizeWTbl[(*oamShape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT]
                           [(*oamShape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT];
}

//------------------------------------------------------------------------------
// Moved from g2di_OamUtil.h
static inline int NNS_G2dGetOamSizeY( const GXOamShape* oamShape )
{
#ifdef NNS_FROM_TOOL
    NNS_G2D_DEFINE_NNSI_OBJSIZEHTBL;
#endif // NNS_FROM_TOOL
    // NNS_G2D_NULL_ASSERT(oamShape);
    return NNSi_objSizeHTbl[(*oamShape & GX_OAM_ATTR01_SHAPE_MASK) >> GX_OAM_ATTR01_SHAPE_SHIFT]
                           [(*oamShape & GX_OAM_ATTR01_SIZE_MASK) >> GX_OAM_ATTR01_SIZE_SHIFT];
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_OAM_DATA_H_


