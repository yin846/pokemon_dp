/*---------------------------------------------------------------------------*
  Project:  NitroSDK - GX -
  File:     fx_mtx44.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_mtx44.c,v $
  Revision 1.25  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.24  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.23  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.22  11/02/2004 17:57:55  takano_makoto
  fix comments.

  Revision 1.21  06/21/2004 10:23:55  takano_makoto
  move MTX_Frustum, MTX_Perspective, MTX_Ortho to inline function.

  Revision 1.20  04/27/2004 01:20:22  takano_makoto
  Modify MTX_Frustum, MTX_Perspective, MTX_Ortho.

  Revision 1.19  04/27/2004 01:16:16  takano_makoto
  Add MTX_FrustumW, MTX_PerspectiveW, MTX_OrthoW.

  Revision 1.18  03/09/2004 06:49:41  takano_makoto
  Bug fix in MTX_Concat44.

  Revision 1.17  02/18/2004 09:01:18  nishida_kenji
  Add comments for code32.h.

  Revision 1.16  02/12/2004 01:35:50  nishida_kenji
  Work around to optimization of simple matrix functions.

  Revision 1.15  2004/02/10 08:38:53  nishida_kenji
  Work around to optimization of simple matrix functions.

  Revision 1.14  2004/02/10 06:04:11  nishida_kenji
  Add MTX_TransApply44.

  Revision 1.13  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.12  02/03/2004 06:43:03  nishida_kenji
  Add comments.

  Revision 1.11  01/27/2004 00:41:21  nishida_kenji
  small fix and comments

  Revision 1.10  01/26/2004 07:20:45  nishida_kenji
  include code32.h

  Revision 1.8  01/09/2004 09:27:28  nishida_kenji
  fix comments

  Revision 1.7  12/25/2003 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.6  12/12/2003 08:08:34  nishida_kenji
  bug fix in MTX_Concat44

  Revision 1.5  12/11/2003 06:12:39  nishida_kenji
  add MTX_Concat

  Revision 1.4  12/09/2003 11:43:10  nishida_kenji
  improve asserts

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/code32.h>              // Always generate ARM binary for efficiency

#include <nitro/fx/fx_mtx44.h>
#include <nitro/fx/fx_vec.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/mi/memory.h>

inline fx32 mul64(fx64 x, fx32 y)
{
    return (fx32)((x * y) >> FX32_SHIFT);
}

asm void MTX_Identity44_(register MtxFx44* pDst)
{
    mov   r2, #4096                    // FX32_ONE
    mov   r3, #0
    stmia r0!, {r2,r3}
    mov   r1, #0
    stmia r0!, {r1,r3}
    stmia r0!, {r1,r2,r3}
    stmia r0!, {r1,r3}
    stmia r0!, {r1,r2,r3}
    stmia r0!, {r1,r3}
    stmia r0!, {r1,r2}

    bx    lr
}

asm void MTX_Copy44To33_(register const MtxFx44* pSrc, register MtxFx33* pDst)
{
    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}
    
    bx    lr
}

asm void MTX_Copy44To43_(register const MtxFx44* pSrc, register MtxFx43* pDst)
{
    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    ldmia r0!, {r2-r3, r12}
    add   r0, r0, #4
    stmia r1!, {r2-r3, r12}

    bx    lr
}

/*---------------------------------------------------------------------------*
  Name:         MTX_TransApply44

  Description:  Calc T(x, y, z) * pSrc = pDst

  Arguments:    pSrc         a pointer to a 4x4 matrix
                pDst         a pointer to a 4x4 matrix
                x            x trans factor
                y            y trans factor
                z            z trans factor

  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_TransApply44(const MtxFx44 *pSrc, MtxFx44 *pDst, fx32 x, fx32 y, fx32 z)
{
    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    if (pSrc != pDst)
    {
        MI_Copy48B(pSrc, pDst);
    }
    {
        fx64    xx = x;
        fx64    yy = y;
        fx64    zz = z;

        pDst->_30 =
            pSrc->_30 + (fx32)((xx * pSrc->_00 + yy * pSrc->_10 + zz * pSrc->_20) >> FX32_SHIFT);
        pDst->_31 =
            pSrc->_31 + (fx32)((xx * pSrc->_01 + yy * pSrc->_11 + zz * pSrc->_21) >> FX32_SHIFT);
        pDst->_32 =
            pSrc->_32 + (fx32)((xx * pSrc->_02 + yy * pSrc->_12 + zz * pSrc->_22) >> FX32_SHIFT);
        pDst->_33 =
            pSrc->_33 + (fx32)((xx * pSrc->_03 + yy * pSrc->_13 + zz * pSrc->_23) >> FX32_SHIFT);
    }
}

asm void MTX_Transpose44_(register const MtxFx44* pSrc, register MtxFx44* pDst)
{
    stmfd sp!, {r4-r11}

    ldr   r12, [r0, #48]
    ldmia r0!, {r2-r11}
    stmia r1!, {r2, r6, r10, r12}
    ldr   r12, [r0, #12]
    stmia r1!, {r3, r7, r11, r12}

    ldmia r0,  {r10-r11}
    ldr   r12, [r0, #16]
    stmia r1!, {r4, r8, r10, r12}

    ldr   r12, [r0, #20]
    stmia r1!, {r5, r9, r11, r12}
    
    ldmfd sp!, {r4-r11}
    bx    lr
}


#include <nitro/code16.h>
asm void MTX_Scale44_(register MtxFx44 * pDst, register fx32 x, register fx32 y, register fx32 z)
{
    stmia r0!, {r1}
    mov   r1, #0
    str   r2, [r0, #16]
    mov   r2, #0
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2}
    add   r0, r0, #4
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r2}
    mov   r3, #1
    lsl   r3, r3, #12
    stmia r0!, {r1, r2}
    str   r3, [r0, #0]
    bx lr
}
#include <nitro/code32.h>




/*---------------------------------------------------------------------------*
  Name:         MTX_ScaleApply44

  Description:  This function performs the operation equivalent to
                MTX_Scale44 + MTX_Concat44.

  Arguments:    pSrc         a pointer to a 4x4 matrix
                pDst         a pointer to a 4x4 matrix
                x            x scale factor
                y            y scale factor
                z            z scale factor
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_ScaleApply44(const MtxFx44 *pSrc, MtxFx44 *pDst, fx32 x, fx32 y, fx32 z)
{
    fx64    v;

    SDK_NULL_ASSERT(pSrc);
    SDK_NULL_ASSERT(pDst);

    v = (fx64)x;
    pDst->_00 = mul64(v, pSrc->_00);
    pDst->_01 = mul64(v, pSrc->_01);
    pDst->_02 = mul64(v, pSrc->_02);
    pDst->_03 = mul64(v, pSrc->_03);

    v = (fx64)y;
    pDst->_10 = mul64(v, pSrc->_10);
    pDst->_11 = mul64(v, pSrc->_11);
    pDst->_12 = mul64(v, pSrc->_12);
    pDst->_13 = mul64(v, pSrc->_13);

    v = (fx64)z;
    pDst->_20 = mul64(v, pSrc->_20);
    pDst->_21 = mul64(v, pSrc->_21);
    pDst->_22 = mul64(v, pSrc->_22);
    pDst->_23 = mul64(v, pSrc->_23);

    if (pSrc != pDst)
    {
        register fx32 t1, t2;
        t1 = pSrc->_30;
        t2 = pSrc->_31;
        pDst->_30 = t1;
        pDst->_31 = t2;

        t1 = pSrc->_32;
        t2 = pSrc->_33;
        pDst->_32 = t1;
        pDst->_33 = t2;
    }
}

#include <nitro/code16.h>
asm void MTX_RotX44_(register MtxFx44 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    
    str   r2, [r0, #20]
    str   r2, [r0, #40]
    str   r1, [r0, #24]
    neg   r1, r1
    str   r1, [r0, #36]

    mov   r1, #1
    mov   r2, #0
    lsl   r1, r1, #12
    mov   r3, #0

    stmia r0!, {r1, r2, r3}
    stmia r0!, {r2, r3}
    add   r0, #8
    stmia r0!, {r2, r3}
    add   r0, #8
    stmia r0!, {r2, r3}
    stmia r0!, {r2, r3}
    str   r1, [r0, #0]

    bx    lr
}
#include <nitro/code32.h>

#include <nitro/code16.h>
asm void MTX_RotY44_(register MtxFx44 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    str   r2, [r0, #0]
    str   r2, [r0, #40]
    str   r1, [r0, #32]
    neg   r1, r1
    str   r1, [r0, #8]

    mov   r3, #1
    mov   r1, #0
    lsl   r3, r3, #12
    mov   r2, #0

    str   r2, [r0, #4]
    add   r0, #12
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r2}
    str   r2, [r0, #4]
    add   r0, #12
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2, r3}

    bx    lr
}
#include <nitro/code32.h>

#include <nitro/code16.h>
asm void MTX_RotZ44_(register MtxFx44 * pDst, register fx32 sinVal, register fx32 cosVal)
{
    str   r2, [r0, #0]
    str   r2, [r0, #20]
    str   r1, [r0, #4]
    neg   r1, r1
    str   r1, [r0, #16]

    mov   r3, #1
    mov   r1, #0
    lsl   r3, r3, #12
    mov   r2, #0

    add   r0, #8
    stmia r0!, {r1, r2}
    add   r0, #8
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r2}
    stmia r0!, {r1, r2, r3}

    bx    lr
}
#include <nitro/code32.h>


/*---------------------------------------------------------------------------*
  Name:         MTX_RotAxis44

  Description:  Sets a rotation matrix about an arbitrary axis.

  Arguments:    pDst         a pointer to a 4x4 matrix
                vec          a pointer to a vector containing the x,y,z axis
                             components. *vec must be a unit vector.
                sinVal       sine of an angle of rotation
                cosVal       cosine of an angle of rotation
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_RotAxis44(MtxFx44 *pDst, const VecFx32 *vec, fx32 sinVal, fx32 cosVal)
{
    fx64    t, ss, cc, xx, yy, zz;
    fx32    t01;
    fx32    s2;
    SDK_NULL_ASSERT(pDst);
    SDK_NULL_ASSERT(vec);

    t = (fx64)(FX32_ONE - cosVal);
    ss = (fx64)sinVal;
    cc = (fx64)cosVal;
    xx = (fx64)vec->x;
    yy = (fx64)vec->y;
    zz = (fx64)vec->z;

    t01 = (fx32)((xx * xx * t) >> (FX64_SHIFT + FX64_SHIFT));
    pDst->_00 = t01 + cosVal;

    t01 = (fx32)((yy * yy * t) >> (FX64_SHIFT + FX64_SHIFT));
    pDst->_11 = t01 + cosVal;

    t01 = (fx32)((zz * zz * t) >> (FX64_SHIFT + FX64_SHIFT));
    pDst->_22 = t01 + cosVal;

    t01 = (fx32)((t * xx * yy) >> (FX64_SHIFT + FX64_SHIFT));
    s2 = (fx32)((ss * zz) >> FX64_SHIFT);
    pDst->_01 = t01 + s2;
    pDst->_10 = t01 - s2;

    t01 = (fx32)((t * xx * zz) >> (FX64_SHIFT + FX64_SHIFT));
    s2 = (fx32)((ss * yy) >> FX64_SHIFT);
    pDst->_02 = t01 - s2;
    pDst->_20 = t01 + s2;

    t01 = (fx32)((t * yy * zz) >> (FX64_SHIFT + FX64_SHIFT));
    s2 = (fx32)((ss * xx) >> FX64_SHIFT);
    pDst->_12 = t01 + s2;
    pDst->_21 = t01 - s2;

    pDst->_03 = pDst->_13 = pDst->_23 = pDst->_30 = pDst->_31 = pDst->_32 = 0;
    pDst->_33 = FX32_ONE;
}


/*---------------------------------------------------------------------------*
  Name:         MTX_Concat44

  Description:  Concatenates two 4x4 matrices.
                The order of operation is A x B = AB.
                OK for any of ab == a == b.

  Arguments:    a            a pointer to a 4x4 matrix
                b            a pointer to a 4x4 matrix
                ab           a pointer to the resultant matrix
  
  Returns:      none
 *---------------------------------------------------------------------------*/
void MTX_Concat44(const MtxFx44 *a, const MtxFx44 *b, MtxFx44 *ab)
{
    MtxFx44 tmp;
    MtxFx44 *p;

    register fx32 x, y, z, w;
    register fx32 xx, yy, zz, ww;

    SDK_NULL_ASSERT(a);
    SDK_NULL_ASSERT(b);
    SDK_NULL_ASSERT(ab);

    if (ab == b)
    {
        p = &tmp;
    }
    else
    {
        p = ab;
    }

    // compute (a x b) -> p

    // row 0
    x = a->_00;
    y = a->_01;
    z = a->_02;
    w = a->_03;

    p->_00 =
        (fx32)(((fx64)x * b->_00 + (fx64)y * b->_10 + (fx64)z * b->_20 +
                (fx64)w * b->_30) >> FX32_SHIFT);
    p->_01 =
        (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21 +
                (fx64)w * b->_31) >> FX32_SHIFT);
    p->_03 =
        (fx32)(((fx64)x * b->_03 + (fx64)y * b->_13 + (fx64)z * b->_23 +
                (fx64)w * b->_33) >> FX32_SHIFT);

    xx = b->_02;
    yy = b->_12;
    zz = b->_22;
    ww = b->_32;

    p->_02 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);

    // row 1
    x = a->_10;
    y = a->_11;
    z = a->_12;
    w = a->_13;

    p->_12 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);
    p->_11 =
        (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21 +
                (fx64)w * b->_31) >> FX32_SHIFT);
    p->_13 =
        (fx32)(((fx64)x * b->_03 + (fx64)y * b->_13 + (fx64)z * b->_23 +
                (fx64)w * b->_33) >> FX32_SHIFT);

    xx = b->_00;
    yy = b->_10;
    zz = b->_20;
    ww = b->_30;

    p->_10 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);

    // row 2
    x = a->_20;
    y = a->_21;
    z = a->_22;
    w = a->_23;

    p->_20 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);
    p->_21 =
        (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21 +
                (fx64)w * b->_31) >> FX32_SHIFT);
    p->_23 =
        (fx32)(((fx64)x * b->_03 + (fx64)y * b->_13 + (fx64)z * b->_23 +
                (fx64)w * b->_33) >> FX32_SHIFT);

    xx = b->_02;
    yy = b->_12;
    zz = b->_22;
    ww = b->_32;

    p->_22 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);

    // row 3
    x = a->_30;
    y = a->_31;
    z = a->_32;
    w = a->_33;

    p->_32 = (fx32)(((fx64)x * xx + (fx64)y * yy + (fx64)z * zz + (fx64)w * ww) >> FX32_SHIFT);
    p->_31 =
        (fx32)(((fx64)x * b->_01 + (fx64)y * b->_11 + (fx64)z * b->_21 +
                (fx64)w * b->_31) >> FX32_SHIFT);
    p->_30 =
        (fx32)(((fx64)x * b->_00 + (fx64)y * b->_10 + (fx64)z * b->_20 +
                (fx64)w * b->_30) >> FX32_SHIFT);
    p->_33 =
        (fx32)(((fx64)x * b->_03 + (fx64)y * b->_13 + (fx64)z * b->_23 +
                (fx64)w * b->_33) >> FX32_SHIFT);

    if (p == &tmp)
    {
        *ab = tmp;
    }
}

#include <nitro/codereset.h>
