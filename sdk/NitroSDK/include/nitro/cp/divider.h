/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CP - include
  File:     divider.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: divider.h,v $
  Revision 1.25  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.24  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.23  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.22  11/02/2004 10:24:30  yada
  just fix comment

  Revision 1.21  06/22/2004 09:20:53  yada
  only fix comment

  Revision 1.20  06/22/2004 08:02:15  yada
  set 0 to hi-32bit of denominator register even if denominator is 32bit

  Revision 1.19  2004/06/02 13:11:21  yasu
  Fix ifdef conditions with SDK_CP_NO_SAFE

  Revision 1.18  05/06/2004 10:45:13  takano_makoto
  Modify include file.

  Revision 1.17  05/06/2004 02:56:01  takano_makoto
  Change to thread safe.

  Revision 1.16  04/07/2004 02:03:17  yada
  fix header comment

  Revision 1.15  02/05/2004 08:38:44  yada
  only modify comment

  Revision 1.14  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.13  01/18/2004 04:36:40  nishida_kenji
  renames functions(ex. CP_GetDivRemainder_8 -> CP_GetDivRemainder8)

  Revision 1.12  01/18/2004 01:24:06  nishida_kenji
  CP_IsDivBusy returns s32

  Revision 1.11  01/17/2004 02:28:38  nishida_kenji
  convert 'inline' to 'static inline' in header files

  Revision 1.10  12/25/2003 00:19:29  nishida_kenji
  convert INLINE to inline

  Revision 1.9  12/08/2003 12:09:48  yada
  Made changes to the file to include

  Revision 1.8  11/18/2003 10:43:27  nishida_kenji
  Fixed a small bug

  Revision 1.7  11/14/2003 04:02:16  yada
  Changed the register access macro from REG_XXX to reg_XXX

  Revision 1.6  11/05/2003 00:58:16  yada
  Made the cast more strict

  Revision 1.5  11/04/2003 11:06:42  yada
  Corrected the forgotten inline of CP_GetDivRemainderImm_8

  Revision 1.4  11/04/2003 09:19:42  yada
  Attached a (u32) to one part of a constant    

  Revision 1.3  10/30/2003 11:42:45  yada
  Decided to use REGTypexx in the register type

  Revision 1.2  2003/10/30 04:59:49  yasu
  Made changes to the register definition file $IrisSDK/include/iris/hw at the bottom    

  Revision 1.1  10/30/2003 00:37:52  yada
  Created initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_CP_DIVIDER_H_
#define NITRO_CP_DIVIDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/os.h>
#include <nitro/cp/context.h>

//----------------------------------------------------------------
#define CP_DIV_32_32BIT_MODE   (0UL << REG_CP_DIVCNT_MODE_SHIFT)        // 32bit/32bit mode
#define CP_DIV_64_32BIT_MODE   (1UL << REG_CP_DIVCNT_MODE_SHIFT)        // 64bit/32bit mode
#define CP_DIV_64_64BIT_MODE   (2UL << REG_CP_DIVCNT_MODE_SHIFT)        // 64bit/64bit mode


/*---------------------------------------------------------------------------*
  Name:         CP_SetDivControl

  Description:  set parameter to divider control register

  Arguments:    parameter   parameter to set control register

  Returns:      None
 *---------------------------------------------------------------------------*/
static inline void CP_SetDivControl(u16 parameter)
{
    reg_CP_DIVCNT = parameter;
}

/*---------------------------------------------------------------------------*
  Name:         CP_SetDivImm32_32_NS_
                CP_SetDivImm64_32_NS_
                CP_SetDivImm64_64_NS_

  Description:  set numerator(dividend) and denominator(divisor).
                ignore setting mode.
                non thread safe.

  Arguments:    numer  : numerator (dividend)
                denom  : denominator(divisor)

  Returns:      None
 *---------------------------------------------------------------------------*/
//   32bit / 32bit
static inline void CP_SetDivImm32_32_NS_(u32 numer, u32 denom)
{
    *(REGType32 *)REG_DIV_NUMER_ADDR = numer;
    *(REGType64 *)REG_DIV_DENOM_ADDR = denom;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   64bit / 32bit
static inline void CP_SetDivImm64_32_NS_(u64 numer, u32 denom)
{
    *(REGType64 *)REG_DIV_NUMER_ADDR = numer;
    *(REGType64 *)REG_DIV_DENOM_ADDR = denom;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   64bit / 64bit
static inline void CP_SetDivImm64_64_NS_(u64 numer, u64 denom)
{
    *(REGType64 *)REG_DIV_NUMER_ADDR = numer;
    *(REGType64 *)REG_DIV_DENOM_ADDR = denom;
}

/*---------------------------------------------------------------------------*
  Name:         CP_SetDivImm32_32
                CP_SetDivImm64_32
                CP_SetDivImm64_64

  Description:  set numerator(dividend) and denominator(divisor).
                ignore setting mode.
                thread safe.

  Arguments:    numer  : numerator (dividend)
                denom  : denominator(divisor)

  Returns:      None
 *---------------------------------------------------------------------------*/
//   32bit / 32bit
static inline void CP_SetDivImm32_32(u32 numer, u32 denom)
{
#ifdef SDK_CP_NO_SAFE                  // disable irq if not thread-safe
    BOOL    enabled = OS_DisableIrq();
#endif

    CP_SetDivImm32_32_NS_(numer, denom);

#ifdef SDK_CP_NO_SAFE
    (void)OS_RestoreIrq(enabled);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   64bit / 32bit
static inline void CP_SetDivImm64_32(u64 numer, u32 denom)
{
#ifdef SDK_CP_NO_SAFE                  // disable irq if not thread-safe
    BOOL    enabled = OS_DisableIrq();
#endif

    CP_SetDivImm64_32_NS_(numer, denom);

#ifdef SDK_CP_NO_SAFE
    (void)OS_RestoreIrq(enabled);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   64bit / 64bit
static inline void CP_SetDivImm64_64(u64 numer, u64 denom)
{
#ifdef SDK_CP_NO_SAFE                  // disable irq if not thread-safe
    BOOL    enabled = OS_DisableIrq();
#endif

    CP_SetDivImm64_64_NS_(numer, denom);

#ifdef SDK_CP_NO_SAFE
    (void)OS_RestoreIrq(enabled);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         CP_SetDiv32_32
                CP_SetDiv64_32
                CP_SetDiv64_64

  Description:  set numerator(dividend) and denominator(divisor).
                set mode.
                thread safe.

  Arguments:    numer  : numerator (dividend)
                denom  : denominator(divisor)

  Returns:      None
 *---------------------------------------------------------------------------*/
//   32bit / 32bit
static inline void CP_SetDiv32_32(u32 numer, u32 denom)
{
#ifdef SDK_CP_NO_SAFE                  // disable irq if not thread-safe
    BOOL    enabled = OS_DisableIrq();
#endif

    reg_CP_DIVCNT = CP_DIV_32_32BIT_MODE;
    CP_SetDivImm32_32_NS_(numer, denom);

#ifdef SDK_CP_NO_SAFE
    (void)OS_RestoreIrq(enabled);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   64bit / 32bit
static inline void CP_SetDiv64_32(u64 numer, u32 denom)
{
#ifdef SDK_CP_NO_SAFE                  // disable irq if not thread-safe
    BOOL    enabled = OS_DisableIrq();
#endif

    reg_CP_DIVCNT = CP_DIV_64_32BIT_MODE;
    CP_SetDivImm64_32_NS_(numer, denom);

#ifdef SDK_CP_NO_SAFE
    (void)OS_RestoreIrq(enabled);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   64bit / 64bit
static inline void CP_SetDiv64_64(u64 numer, u64 denom)
{
#ifdef SDK_CP_NO_SAFE                  // disable irq if not thread-safe
    BOOL    enabled = OS_DisableIrq();
#endif

    reg_CP_DIVCNT = CP_DIV_64_64BIT_MODE;
    CP_SetDivImm64_64_NS_(numer, denom);

#ifdef SDK_CP_NO_SAFE
    (void)OS_RestoreIrq(enabled);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         CP_IsDivBusy

  Description:  check whether operation finished

  Arguments:    None

  Returns:      non-0 if finish, 0 if not
 *---------------------------------------------------------------------------*/
static inline s32 CP_IsDivBusy()
{
    return (reg_CP_DIVCNT & REG_CP_DIVCNT_BUSY_MASK);
}

/*---------------------------------------------------------------------------*
  Name:         CP_WaitDiv

  Description:  wait to finish operation

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static inline void CP_WaitDiv()
{
    while (CP_IsDivBusy())
    {
    }
}

/*---------------------------------------------------------------------------*
  Name:         CP_GetDivResultImm64
                CP_GetDivResultImm32
                CP_GetDivResultImm16
                CP_GetDivResultImm8

  Description:  get result(quotient) of division.
                don't check whether operation finish

  Arguments:    None

  Returns:      result(quotient) of division
 *---------------------------------------------------------------------------*/
//   as 64bit data
static inline s64 CP_GetDivResultImm64()
{
    return (s64)(*(REGType64 *)REG_DIV_RESULT_ADDR);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 32bit data
static inline s32 CP_GetDivResultImm32()
{
    return (s32)(*(REGType32 *)REG_DIV_RESULT_ADDR);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 16bit data
static inline s16 CP_GetDivResultImm16()
{
    return (s16)(*(REGType16 *)REG_DIV_RESULT_ADDR);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 8bit data
static inline s8 CP_GetDivResultImm8()
{
    return (s8)(*(REGType8 *)REG_DIV_RESULT_ADDR);
}

/*---------------------------------------------------------------------------*
  Name:         CP_GetDivResult64
                CP_GetDivResult32
                CP_GetDivResult16
                CP_GetDivResult8

  Description:  get result(quotient) of division.
                wait till operation finish

  Arguments:    None

  Returns:      result(quotient) of division
 *---------------------------------------------------------------------------*/
//   as 64bit data
static inline s64 CP_GetDivResult64()
{
    CP_WaitDiv();
    return CP_GetDivResultImm64();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 32bit data
static inline s32 CP_GetDivResult32()
{
    CP_WaitDiv();
    return CP_GetDivResultImm32();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 16bit data
static inline s16 CP_GetDivResult16()
{
    CP_WaitDiv();
    return CP_GetDivResultImm16();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 8bit data
static inline s8 CP_GetDivResult8()
{
    CP_WaitDiv();
    return CP_GetDivResultImm8();
}

/*---------------------------------------------------------------------------*
  Name:         CP_GetDivRemainderImm64
                CP_GetDivRemainderImm32
                CP_GetDivRemainderImm16
                CP_GetDivRemainderImm8

  Description:  get remainder of division.
                don't check whether operation finish

  Arguments:    None

  Returns:      remainder of division
 *---------------------------------------------------------------------------*/
//   as 64bit data
static inline s64 CP_GetDivRemainderImm64()
{
    return (s64)(*(REGType64 *)REG_DIVREM_RESULT_ADDR);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 32bit data
static inline s32 CP_GetDivRemainderImm32()
{
    return (s32)(*(REGType32 *)REG_DIVREM_RESULT_ADDR);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 16bit data
static inline s16 CP_GetDivRemainderImm16()
{
    return (s16)(*(REGType16 *)REG_DIVREM_RESULT_ADDR);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 8bit data
static inline s8 CP_GetDivRemainderImm8()
{
    return (s8)(*(REGType8 *)REG_DIVREM_RESULT_ADDR);
}

/*---------------------------------------------------------------------------*
  Name:         CP_GetDivRemainder64
                CP_GetDivRemainder32
                CP_GetDivRemainder16
                CP_GetDivRemainder8

  Description:  get remainder of division.
                wait till operation finish

  Arguments:    None

  Returns:      remainder of division
 *---------------------------------------------------------------------------*/
//   as 64bit data
static inline s64 CP_GetDivRemainder64()
{
    CP_WaitDiv();
    return CP_GetDivRemainderImm64();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 32bit data
static inline s32 CP_GetDivRemainder32()
{
    CP_WaitDiv();
    return CP_GetDivRemainderImm32();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 16bit data
static inline s16 CP_GetDivRemainder16()
{
    CP_WaitDiv();
    return CP_GetDivRemainderImm16();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//   as 8bit data
static inline s8 CP_GetDivRemainder8()
{
    CP_WaitDiv();
    return CP_GetDivRemainderImm8();
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_CP_DIVIDER_H_ */
#endif
