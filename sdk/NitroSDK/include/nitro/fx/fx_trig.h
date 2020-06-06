/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FX -
  File:     fx_trig.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fx_trig.h,v $
  Revision 1.22  01/18/2006 02:11:21  kitase_hirotake
  do-indent

  Revision 1.21  11/14/2005 05:10:03  okubata_ryoma
  Added support for SDK_INLINE

  Revision 1.20  03/03/2005 13:11:32  takano_makoto
  Revised variable declarations that were missing extern

  Revision 1.19  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.18  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.17  06/16/2004 11:23:42  seiki_masashi
  FX_AtanIdx, FX_Atan2Idx

  Revision 1.16  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.15  02/04/2004 11:49:33  nishida_kenji
  small fix.

  Revision 1.14  01/27/2004 11:15:10  nishida_kenji
  appends 'static' to inline function prototypes.

  Revision 1.13  01/17/2004 09:32:31  nishida_kenji
  FX_SinIdx and FX_CosIdx take int.

  Revision 1.12  01/17/2004 07:17:21  nishida_kenji
  remove external include guards

  Revision 1.11  01/17/2004 02:28:38  nishida_kenji
  convert 'inline' to 'static inline' in header files

  Revision 1.10  01/16/2004 11:25:53  nishida_kenji
  small fix

  Revision 1.9  01/16/2004 10:38:31  nishida_kenji
  add FX_Init for table setup

  Revision 1.8  12/25/2003 00:27:13  nishida_kenji
  remove needless APIs

  Revision 1.7  12/25/2003 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.6  12/18/2003 11:36:17  nishida_kenji
  bug fix on SinIdx and CosIdx

  Revision 1.5  12/11/2003 12:07:33  nishida_kenji
  improve FX_xxx_TO_yyy's precision

  Revision 1.4  12/11/2003 09:18:24  nishida_kenji
  fix comments

  Revision 1.3  12/11/2003 01:17:46  nishida_kenji
  add macros and comments

  Revision 1.2  12/10/2003 05:37:47  nishida_kenji
  FX_Atan, FX_Atan2

  Revision 1.1  12/10/2003 00:14:03  nishida_kenji
  new

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#ifndef NITRO_FXTRIG_H_
#define NITRO_FXTRIG_H_

#include <nitro/fx/fx.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Type definition
//----------------------------------------------------------------------------
#ifdef SDK_FX_DYNAMIC_TABLE
extern fx16 FX_SinCosTable_[];
#else
extern const fx16 FX_SinCosTable_[];
#endif

//----------------------------------------------------------------------------
// Declaration of function
//----------------------------------------------------------------------------

// deg must be in fx32/fx16 format
#define FX_DEG_TO_RAD(deg)            ((fx32)((FX64C_TWOPI_360 * (deg) + 0x80000000LL) >> 32))
#define FX_DEG_TO_IDX(deg)            ((u16) ((FX64C_65536_360 * (deg) + 0x80000000000LL) >> 44))

// rad must be in fx32/fx16 format
#define FX_RAD_TO_DEG(rad)            ((fx32)((FX64C_360_TWOPI * (rad) + 0x80000000LL) >> 32))
#define FX_RAD_TO_IDX(rad)            ((u16)((FX64C_65536_TWOPI * (rad) + 0x80000000000LL) >> 44))

#define FX_IDX_TO_RAD(idx)            ((fx32)((FX64C_TWOPI_65536 * (idx) + 0x80000LL) >> 20))
#define FX_IDX_TO_DEG(idx)            ((fx32)((FX64C_360_65536 * (idx) + 0x80000LL) >> 20))

SDK_DECL_INLINE fx16 FX_SinIdx(int idx);
SDK_DECL_INLINE fx16 FX_CosIdx(int idx);

u16     FX_AtanIdx(fx32 x);
u16     FX_Atan2Idx(fx32 y, fx32 x);

fx16    FX_Atan(fx32 x);
fx16    FX_Atan2(fx32 y, fx32 x);

fx64c   FX_SinFx64c(fx32 rad);
fx64c   FX_CosFx64c(fx32 rad);

//----------------------------------------------------------------------------
// Implementation of inline function
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         FX_SinIdx

  Description:  Obtain approximation of sine by table reference

  Arguments:    idx        treat (2 * pi / 65536) as 1

  Returns:      approximated sine value in fx16 format
 *---------------------------------------------------------------------------*/
SDK_INLINE fx16 FX_SinIdx(int idx)
{
    SDK_MINMAX_ASSERT(idx, 0, 65535);

    return FX_SinCosTable_[((idx >> 4) << 1)];
}

/*---------------------------------------------------------------------------*
  Name:         FX_CosIdx

  Description:  Obtain approximation of cosine by table reference

  Arguments:    idx        treat (2 * pi / 65536) as 1

  Returns:      approximated cosine value in fx16 format
 *---------------------------------------------------------------------------*/
SDK_INLINE fx16 FX_CosIdx(int idx)
{
    SDK_MINMAX_ASSERT(idx, 0, 65535);

    return FX_SinCosTable_[((idx >> 4) << 1) + 1];
}

#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif
