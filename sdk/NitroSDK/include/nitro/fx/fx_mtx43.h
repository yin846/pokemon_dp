/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FX -
  File:     fx_mtx43.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_mtx43.h,v $
  Revision 1.25  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.24  11/14/2005 05:10:03  okubata_ryoma
  Added support for SDK_INLINE

  Revision 1.23  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.22  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.21  06/21/2004 10:31:11  takano_makoto
  Add MTX_LookAt()

  Revision 1.20  02/12/2004 01:36:02  nishida_kenji
  Work around to optimization of simple matrix functions.

  Revision 1.19  2004/02/10 07:30:10  nishida_kenji
  Work around to optimization of simple matrix functions.

  Revision 1.18  02/06/2004 04:41:59  nishida_kenji
  Removed MTX_Trans43

  Revision 1.17  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.16  02/03/2004 06:43:15  nishida_kenji
  Add comments.

  Revision 1.15  01/29/2004 00:20:27  nishida_kenji
  append 'static' to inline function prototypes.

  Revision 1.14  01/26/2004 03:28:29  kitani_toshikazu
  Fixed MTX_Invert43 translate bug , append MTX_Trans43() and MTX_TransApply43()

  Revision 1.13  01/17/2004 08:17:02  nishida_kenji
  add MTX_MultVec43

  Revision 1.12  01/17/2004 07:17:21  nishida_kenji
  remove external include guards

  Revision 1.11  12/11/2003 09:18:24  nishida_kenji
  fix comments

  Revision 1.10  12/11/2003 06:20:31  nishida_kenji
  (none)

  Revision 1.9  12/11/2003 06:12:50  nishida_kenji
  add MTX_Concat

  Revision 1.8  12/09/2003 11:26:54  nishida_kenji
  improve asserts and comments

  Revision 1.7  12/04/2003 07:31:50  nishida_kenji
  MTX_Inverse33, MTX_Inverse43

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_FX_MTX43_H_
#define NITRO_FX_MTX43_H_

#include <nitro/gx/g3_util.h>
#include <nitro/fx/fx.h>
#include <nitro/mi/memory.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

SDK_DECL_INLINE void MTX_Identity43(MtxFx43 *pDst);
SDK_DECL_INLINE void MTX_Copy43(const MtxFx43 *pSrc, MtxFx43 *pDst);
SDK_DECL_INLINE void MTX_Copy43To33(const MtxFx43 *pSrc, MtxFx33 *pDst);
SDK_DECL_INLINE void MTX_Copy43To44(const MtxFx43 *pSrc, MtxFx44 *pDst);
SDK_DECL_INLINE void MTX_Transpose43(const MtxFx43 *pSrc, MtxFx43 *pDst);
void    MTX_TransApply43(const MtxFx43 *pSrc, MtxFx43 *pDst, fx32 x, fx32 y, fx32 z);
SDK_DECL_INLINE void MTX_Scale43(MtxFx43 *pDst, fx32 x, fx32 y, fx32 z);
void    MTX_ScaleApply43(const MtxFx43 *pSrc, MtxFx43 *pDst, fx32 x, fx32 y, fx32 z);
SDK_DECL_INLINE void MTX_RotX43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal);
SDK_DECL_INLINE void MTX_RotY43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal);
SDK_DECL_INLINE void MTX_RotZ43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal);
void    MTX_RotAxis43(MtxFx43 *pDst, const VecFx32 *vec, fx32 sinVal, fx32 cosVal);
int     MTX_Inverse43(const MtxFx43 *pSrc, MtxFx43 *pDst);
void    MTX_Concat43(const MtxFx43 *a, const MtxFx43 *b, MtxFx43 *ab);
void    MTX_MultVec43(const VecFx32 *vec, const MtxFx43 *m, VecFx32 *dst);

void    MTX_Identity43_(register MtxFx43 *pDst);
void    MTX_Copy43To44_(register const MtxFx43 *pSrc, register MtxFx44 *pDst);
void    MTX_Transpose43_(register const MtxFx43 *pSrc, register MtxFx43 *pDst);
void    MTX_Scale43_(register MtxFx43 *pDst, register fx32 x, register fx32 y, register fx32 z);
void    MTX_RotX43_(register MtxFx43 *pDst, register fx32 sinVal, register fx32 cosVal);
void    MTX_RotY43_(register MtxFx43 *pDst, register fx32 sinVal, register fx32 cosVal);
void    MTX_RotZ43_(register MtxFx43 *pDst, register fx32 sinVal, register fx32 cosVal);

//----------------------------------------------------------------------------
// Implementation of inline function
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         MTX_Identity43

  Description:  Sets a matrix to identity.

  Arguments:    pDst         a pointer to a 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Identity43(MtxFx43 *pDst)
{
    SDK_NULL_ASSERT(pDst);
    MTX_Identity43_(pDst);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_Copy43

  Description:  Copies a 4x3 matrix to a 4x3 matrix.

  Arguments:    pSrc         a pointer to a 4x3 matrix
                pDst         a pointer to a 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Copy43(const MtxFx43 *pSrc, MtxFx43 *pDst)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);
    MI_Copy48B(pSrc, pDst);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_Copy43To33

  Description:  Copies a 4x3 matrix to a 3x3 matrix.

  Arguments:    pSrc         a pointer to a 4x3 matrix
                pDst         a pointer to a 3x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Copy43To33(const MtxFx43 *pSrc, MtxFx33 *pDst)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);
    MI_Copy36B(pSrc, pDst);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_Copy43To44

  Description:  Copies a 4x3 matrix to a 4x4 matrix.

  Arguments:    pSrc         a pointer to a 4x3 matrix
                pDst         a pointer to a 4x4 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Copy43To44(const MtxFx43 *pSrc, MtxFx44 *pDst)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);
    MTX_Copy43To44_(pSrc, pDst);
}


/*---------------------------------------------------------------------------*
  Name:         MTX_Transpose43

  Description:  Computes the transpose of a 4x3 matrix.
                Since matrices are 3x4, fourth row is lost and becomes (0,0,0).

  Arguments:    pSrc         a pointer to a 4x3 matrix
                pDst         a pointer to a 4x3 matrix

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Transpose43(const MtxFx43 *pSrc, MtxFx43 *pDst)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    MTX_Transpose43_(pSrc, pDst);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_Scale43

  Description:  Sets a scaling matrix.

  Arguments:    pDst         a pointer to a 4x3 matrix
                x            x scale factor
                y            y scale factor
                z            z scale factor
  
  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_Scale43(MtxFx43 *pDst, fx32 x, fx32 y, fx32 z)
{
    SDK_NULL_ASSERT(pDst);
    MTX_Scale43_(pDst, x, y, z);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_RotX43

  Description:  Sets a rotation matrix about the X axis.

  Arguments:    pDst         a pointer to a 4x3 matrix
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_RotX43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal)
{
    SDK_NULL_ASSERT(pDst);
    MTX_RotX43_(pDst, sinVal, cosVal);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_RotY43

  Description:  Sets a rotation matrix about the Y axis.

  Arguments:    pDst         a pointer to a 4x3 matrix
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_RotY43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal)
{
    SDK_NULL_ASSERT(pDst);
    MTX_RotY43_(pDst, sinVal, cosVal);
}

/*---------------------------------------------------------------------------*
  Name:         MTX_RotZ43

  Description:  Sets a rotation matrix about the Z axis.

  Arguments:    pDst         a pointer to a 4x3 matrix
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void MTX_RotZ43(MtxFx43 *pDst, fx32 sinVal, fx32 cosVal)
{
    SDK_NULL_ASSERT(pDst);
    MTX_RotZ43_(pDst, sinVal, cosVal);
}


/*---------------------------------------------------------------------------*
  Name:         MTX_LookAt

  Description:  Computes a matrix to transform points to camera coordinates.

  Arguments:    camPos       camera position
                camUp        camera 'up' direction
                target       camera aim point
                mtx          a 4x3 matrix to be set if not NULL

  Returns:      none
 *---------------------------------------------------------------------------*/
SDK_INLINE void
MTX_LookAt(const VecFx32 *camPos, const VecFx32 *camUp, const VecFx32 *target, MtxFx43 *mtx)
{
    SDK_NULL_ASSERT(mtx);
    G3i_LookAt_(camPos, camUp, target, FALSE, mtx);
}


#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif
