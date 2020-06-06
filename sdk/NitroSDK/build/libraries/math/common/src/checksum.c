/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH -
  File:     checksum.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: checksum.c,v $
  Revision 1.3  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.2  04/13/2005 04:52:33  seiki_masashi
  Changed allow Checksum16 to be used regardless of the byte boundary.

  Revision 1.1  04/12/2005 11:12:27  seiki_masashi
  Added checksum.h

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/math/checksum.h>

/*---------------------------------------------------------------------------*
  Name:         MATHi_Checksum16Update

  Description:  Requests the 16bit Checksum value.

  Arguments:    input   Pointer to input data.
                length  Length of input data

  Returns:      Calculation result.
 *---------------------------------------------------------------------------*/
static inline u32 MATHi_Checksum16Update(const void *input, u32 length)
{
    u32     sum;
    u32     n;
    BOOL    fSwap = FALSE;

    sum = 0;

    if (((u32)input) & 1)
    {
        sum += (*((u8 *)input)++ << 8); // BIGENDIAN
        length--;
        fSwap = TRUE;
    }

    while ((length >> 17) > 0)
    {
        length -= (1 << 17);
        for (n = (1 << 16); n > 0; n--)
        {
            sum += *((u16 *)input)++;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum = (u16)((sum >> 16) + sum);
    }

    for (n = (length >> 1); n > 0; n--)
    {
        sum += *((u16 *)input)++;
    }

    if (length & 1)
    {
        sum += *((u8 *)input);
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = (sum >> 16) + (sum & 0xffff);

    if (fSwap)
    {
        sum = ((sum << 24) | (sum << 8)) >> 16;
    }

    return sum;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum16Update

  Description:  Updates the 16bit Checksum value with added data.

  Arguments:    context MATHChecksum16Context structure
                input   Pointer to input data.
                length  Length of input data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_Checksum16Update(MATHChecksum16Context * context, const void *input, u32 length)
{
    u32     sum;

    sum = MATHi_Checksum16Update(input, length);

    sum += *context;
    sum += (sum >> 16);

    *context = (u16)sum;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcChecksum16

  Description:  Calculates the 16bit Checksum.
  
  Arguments:    data    Pointer to input data.
                dataLength  Length of input data.
  
  Returns:      Checksum value.
 *---------------------------------------------------------------------------*/
u16 MATH_CalcChecksum16(const void *data, u32 dataLength)
{
    u32     sum;

    sum = MATHi_Checksum16Update(data, dataLength);

    return (u16)(~sum);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum8Update

  Description:  Requests the 8bit Checksum value.

  Arguments:    input   Pointer to input data.
                length  Length of input data

  Returns:      Calculation result.
 *---------------------------------------------------------------------------*/
static inline u32 MATHi_Checksum8Update(const void *input, u32 length)
{
    u32     sum;
    u32     n;

    sum = 0;

    if (((u32)input) & 1)
    {
        sum += *((u8 *)input)++;
        length--;
    }

    while ((length >> 17) > 0)
    {
        length -= (1 << 17);
        for (n = (1 << 16); n > 0; n--)
        {
            sum += *((u16 *)input)++;
        }

        sum = (sum >> 16) + (sum & 0xffff);
        sum = (u16)(sum + (sum >> 16));
    }

    for (n = (length >> 1); n > 0; n--)
    {
        sum += *((u16 *)input)++;
    }

    if (length & 1)
    {
        sum += *((u8 *)input);
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (u16)sum;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_Checksum8Update

  Description:  Updates the 8bit Checksum value with added data.

  Arguments:    context MATHChecksum8Context structure
                input   Pointer to input data.
                length  Length of input data

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_Checksum8Update(MATHChecksum8Context * context, const void *input, u32 length)
{
    u32     sum;

    sum = MATHi_Checksum8Update(input, length);

    sum += *context;
    sum += (sum >> 16);

    *context = (u16)sum;
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcChecksum8

  Description:  Calculates the 16bit Checksum.
  
  Arguments:    data    Pointer to input data.
                dataLength  Length of input data.
  
  Returns:      Checksum value.
 *---------------------------------------------------------------------------*/
u8 MATH_CalcChecksum8(const void *data, u32 dataLength)
{
    u32     sum;

    sum = MATHi_Checksum8Update(data, dataLength);

    sum = (sum >> 8) + (sum & 0xff);
    sum += (sum >> 8);

    return (u8)(~sum);
}
