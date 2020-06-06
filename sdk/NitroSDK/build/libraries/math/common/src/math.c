/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH -
  File:     math.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: math.c,v $
  Revision 1.6  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  11/24/2005 03:09:27  seiki_masashi
  Added reference mounting (written by terui) from the MATH_CountPopulation ARM assembler as a comment.

  Revision 1.4  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.3  02/18/2005 07:12:45  seiki_masashi
  warning measures

  Revision 1.2  01/11/2005 07:40:17  takano_makoto
  fix copyright header.

  Revision 1.1  01/06/2005 06:25:50  seiki_masashi
  Made change so library for ARM7 is also created

  Revision 1.2  12/15/2004 09:17:38  seiki_masashi
  Added MATH_CountPopulation

  Revision 1.1  12/14/2004 10:51:26  seiki_masashi
  Added MATH genre

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/math.h>

/*---------------------------------------------------------------------------*
  Name:         MATH_CountLeadingZerosFunc

  Description:  Requests the number of 0 bits from the top, expressed as 32-bit binary     
                (For ARM9 Thumb or ARM7 that does not have CLZ command)

  Arguments:    x

  Returns:      The number of contiguous 0 bits from the top    
 *---------------------------------------------------------------------------*/
u32 MATH_CountLeadingZerosFunc(u32 x)
{
    u32     y;
    u32     n = 32;

    // Use dichotomizing search to find place where 0s end.     
    y = x >> 16;
    if (y != 0)
    {
        n -= 16;
        x = y;
    }
    y = x >> 8;
    if (y != 0)
    {
        n -= 8;
        x = y;
    }
    y = x >> 4;
    if (y != 0)
    {
        n -= 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0)
    {
        n -= 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0)
    {
        n -= 2;
    }                                  // x == 0b10 or 0b11 -> n -= 2
    else
    {
        n -= x;
    }                                  // x == 0b00 or 0b01 -> n -= x

    return n;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CountPopulation

  Description:  Requests the number of 1 bits, expressed as 32-bit binary    

  Arguments:    x

  Returns:      Binary expression of the number of 1 bits    
 *---------------------------------------------------------------------------*/
u8 MATH_CountPopulation(u32 x)
{
    // Note below that in the ARM code, shift and arithmetic operations are done simultaneously.
    // For the Release Build and after: no-stall 13 command + bx lr.     

    // Without directly counting 32bits, each 2 bits is counted as 1 and     
    // stored in the same 2bit location.    
    // In other words, every 2 bits is converted as:
    // Where x -> x' the 2-bit value is x' = x - (x >> 1)
    x -= ((x >> 1) & 0x55555555);

    // Think in units of 4bits. The upper 2bit gets added to the "1" number     
    // stored in the 2bits, and this gets stored as the first "1" number in the 4bits in that location.    
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);

    // Do the same in units of 8 bits.
    // However, the maximum result for each digit is 8, and this is
    // held in 4bit, so masking is not necessary ahead of time.
    x += (x >> 4);

    // In this following computations, unnecessary parts are masked.    
    x &= 0x0f0f0f0f;

    // Get the sum in units of 16 bits of the upper 8bit and lower 8bit.    
    x += (x >> 8);

    // Do the same in units of 32 bits.
    x += (x >> 16);

    // The lower 8bit value is the result.
    return (u8)x;
}

#if 0
// Reference mounting from the assembler.
// This is faster when the number of bits not standing is greater.
/*---------------------------------------------------------------------------*
  Name:         MATH_CountPopulation_Asm

  Description:  Counts bits that are set to 1 when value is expressed as 32 bits.

  Arguments:    value   -   Value to count.

  Returns:      u32     -   Number of bits. Value is 0-32.
 *---------------------------------------------------------------------------*/
#include <nitro/code32.h>
asm u32 MATH_CountPopulation_Asm(u32 value)
{
    mov     r1 ,    #0
    mov     r2 ,    #1

@loop:
    clz     r3 ,    r0
    rsbs    r3 ,    r3 ,    #31
    bcc     @end
    add     r1 ,    r1 ,    #1
    mvn     r3 ,    r2 ,    LSL r3
    and     r0 ,    r0 ,    r3
    b       @loop

@end:
    mov     r0 ,    r1
    bx      lr
}
#include <nitro/codereset.h>
#endif
