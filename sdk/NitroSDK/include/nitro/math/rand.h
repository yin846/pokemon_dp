/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH - include
  File:     rand.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: rand.h,v $
  Revision 1.4  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.3  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.2  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.1  12/14/2004 13:11:59  takano_makoto
  Moved from FX to MATH

  Revision 1.1  12/13/2004 07:44:20  takano_makoto
  Moved from RND to FX

  Revision 1.1  12/09/2004 12:35:22  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/types.h>

#ifndef NITRO_MATH_RAND_H_
#define NITRO_MATH_RAND_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    u64     x;                         // Random number value
    u64     mul;                       // Multiple
    u64     add;                       // The number to be added
}
MATHRandContext32;


typedef struct
{
    u32     x;                         // Random number value
    u32     mul;                       // Multiple
    u32     add;                       // The number to be added
}
MATHRandContext16;


/*---------------------------------------------------------------------------*
  Name:         MATH_InitRand32

  Description:  Initialize 32bit random number context using the linear congruential method.

  Arguments:    context  Pointer to random number context
  
                seed    Random-number seed to set as initial value
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_InitRand32(MATHRandContext32 *context, u64 seed)
{
    context->x = seed;
    context->mul = (1566083941LL << 32) + 1812433253LL;
    context->add = 2531011;
}


/*---------------------------------------------------------------------------*
  Name:         MATH_Rand32

  Description:  The function that gets 32bit random number context using the linear congruential method.    

  Arguments:    context  Pointer to random number context
  
                max     Specifies the range of numbers that can be obtained. The range is 0 to max-1.     
                        When 0 is specified, the value is a 32bit value for the entire range.    
  
  Returns:      A 32bit random number
 *---------------------------------------------------------------------------*/
static inline u32 MATH_Rand32(MATHRandContext32 *context, u32 max)
{
    context->x = context->mul * context->x + context->add;

    // If the "max" argument is a constant, optimized by compiler.
    if (max == 0)
    {
        return (u32)(context->x >> 32);
    }
    else
    {
        return (u32)(((context->x >> 32) * max) >> 32);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MATH_InitRand16

  Description:  Initialize 16bit random number context using the linear congruential method.

  Arguments:    context  Pointer to random number context
  
                seed    Random-number seed to set as initial value
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MATH_InitRand16(MATHRandContext16 *context, u32 seed)
{
    context->x = seed;
    context->mul = 1566083941LL;
    context->add = 2531011;
}


/*---------------------------------------------------------------------------*
  Name:         MATH_Rand16

  Description:  The function that gets 16bit random number context using  the linear congruential method.

  Arguments:    context  Pointer to random number context
  
                max     Specifies the range of numbers that can be obtained. The range is 0 to max-1.     
                        When 0 is specified, the value is a 16bit value for the entire range.   
  
  Returns:      A 16bit random number
 *---------------------------------------------------------------------------*/
static inline u16 MATH_Rand16(MATHRandContext16 *context, u16 max)
{
    context->x = context->mul * context->x + context->add;

    // If the "max" argument is a constant, optimized by compiler.
    if (max == 0)
    {
        return (u16)(context->x >> 16);
    }
    else
    {
        return (u16)(((context->x >> 16) * max) >> 16);
    }
}


#ifdef __cplusplus
} /* extern "C"*/
#endif


#endif // NITRO_MATH_RAND_H_
