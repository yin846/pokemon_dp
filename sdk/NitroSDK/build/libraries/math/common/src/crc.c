/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MATH -
  File:     crc.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: crc.c,v $
  Revision 1.5  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.4  04/13/2005 11:57:54  seiki_masashi
  GetDigest -> GetHash

  Revision 1.3  04/11/2005 10:35:37  seiki_masashi
  Added the Utility function.

  Revision 1.2  04/11/2005 08:24:27  seiki_masashi
  small fix

  Revision 1.1  04/11/2005 01:48:53  seiki_masashi
  Added crc.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/math/crc.h>

/*****************************************************************************/
/* CRC-8 common*/
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC8InitTable

  Description:  Initializes the MATHCRC8Table structure used for requesting the CRC-8 value.
  
  Arguments:    table MATHCRC8Table structure
                poly  Bit representation of the generator polynomial with the top bit removed.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC8InitTable(MATHCRC8Table * table, u8 poly)
{
    u32     r;
    u32     i, j;
    u8     *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 0x80)
            {
                r = (r << 1) ^ poly;
            }
            else
            {
                r <<= 1;
            }
        }
        t[i] = (u8)r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC8InitTableRev

  Description:  Initializes the MATHCRC8Table structure used for requesting the CRC-8 value.
                The lower-order bit is handled as the higher-order digit.
  
  Arguments:    table MATHCRC8Table structure
                poly  Bit representation of the generator polynomial with the top bit removed where the top and bottom have been reversed.
                       
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC8InitTableRev(MATHCRC8Table * table, u8 poly)
{
    u32     r;
    u32     i, j;
    u8     *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 1)
            {
                r = (r >> 1) ^ poly;
            }
            else
            {
                r >>= 1;
            }
        }
        t[i] = (u8)r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC8Update

  Description:  Updates the CRC-8 value with added data.
  
  Arguments:    table   Pointer to the table for calculation, MATHCRC8Table
                context MATHCRC8Context structure
                input   Pointer to input data.
                length  Length of input data
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC8Update(const MATHCRC8Table * table, MATHCRC8Context * context, const void *input,
                 u32 length)
{
    u32     r;
    u32     i;
    const u8 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = t[(r ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC8Context) r;
}

/*****************************************************************************/
/* CRC-16 common*/
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC16InitTable

  Description:  Initializes the MATHCRC16Table structure used for requesting the CRC-16 value.
  
  Arguments:    table MATHCRC16Table structure
                poly  Bit representation of the generator polynomial with the top bit removed.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC16InitTable(MATHCRC16Table * table, u16 poly)
{
    u32     r;
    u32     i, j;
    u16    *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i << 8;
        for (j = 0; j < 8; j++)
        {
            if (r & 0x8000)
            {
                r = (r << 1) ^ poly;
            }
            else
            {
                r <<= 1;
            }
        }
        t[i] = (u16)r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC16InitTableRev

  Description:  Initializes the MATHCRC16Table structure used for requesting the CRC-16 value.
                The lower-order bit is handled as the higher-order digit.
  
  Arguments:    table MATHCRC16Table structure
                poly  Bit representation of the generator polynomial with the top bit removed where the top and bottom have been reversed.
                        
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC16InitTableRev(MATHCRC16Table * table, u16 poly)
{
    u32     r;
    u32     i, j;
    u16    *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 1)
            {
                r = (r >> 1) ^ poly;
            }
            else
            {
                r >>= 1;
            }
        }
        t[i] = (u16)r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC16Update

  Description:  Updates the CRC-16 value with added data.
  
  Arguments:    table   Pointer to the table for calculation, MATHCRC16Table
                context MATHCRC16Context structure
                input   Pointer to input data.
                length  Length of input data
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC16Update(const MATHCRC16Table * table, MATHCRC16Context * context, const void *input,
                  u32 length)
{
    u32     r;
    u32     i;
    const u16 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = (r << 8) ^ t[((r >> 8) ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC16Context) r;
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC16UpdateRev

  Description:  Updates the CRC-16 value with added data.
                The lower-order bit is handled as the higher-order digit.
  
  Arguments:    table   Pointer to the table for calculation, MATHCRC16Table
                context MATHCRC16Context structure
                input   Pointer to input data.
                length  Length of input data
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC16UpdateRev(const MATHCRC16Table * table, MATHCRC16Context * context, const void *input,
                     u32 length)
{
    u32     r;
    u32     i;
    const u16 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = (r >> 8) ^ t[(r ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC16Context) r;
}

/*****************************************************************************/
/* CRC-32 common*/
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC32InitTable

  Description:  Initializes the MATHCRC32Table structure used for requesting the CRC-32 value.
  
  Arguments:    table MATHCRC32Table structure
                poly  Bit representation of the generator polynomial with the top bit removed.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC32InitTable(MATHCRC32Table * table, u32 poly)
{
    u32     r;
    u32     i, j;
    u32    *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i << 24;
        for (j = 0; j < 8; j++)
        {
            if (r & 0x80000000U)
            {
                r = (r << 1) ^ poly;
            }
            else
            {
                r <<= 1;
            }
        }
        t[i] = r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC32InitTableRev

  Description:  Initializes the MATHCRC32Table structure used for requesting the CRC-32 value.
                The lower-order bit is handled as the higher-order digit.
  
  Arguments:    table MATHCRC32Table structure
                poly  Bit representation of the generator polynomial with the top bit removed where the top and bottom have been reversed.
                       
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATHi_CRC32InitTableRev(MATHCRC32Table * table, u32 poly)
{
    u32     r;
    u32     i, j;
    u32    *t = table->table;

    for (i = 0; i < 256; i++)
    {
        r = i;
        for (j = 0; j < 8; j++)
        {
            if (r & 1)
            {
                r = (r >> 1) ^ poly;
            }
            else
            {
                r >>= 1;
            }
        }
        t[i] = r;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC32Update

  Description:  Updates the CRC-32 value with added data.
  
  Arguments:    table   Pointer to the table for calculation, MATHCRC32Table
                context MATHCRC32Context structure
                input   Pointer to input data.
                length  Length of input data
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC32Update(const MATHCRC32Table * table, MATHCRC32Context * context, const void *input,
                  u32 length)
{
    u32     r;
    u32     i;
    const u32 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = (r << 8) ^ t[((r >> 24) ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC32Context) r;
}

/*---------------------------------------------------------------------------*
  Name:         MATHi_CRC32UpdateRev

  Description:  Updates the CRC-32 value with added data.
                The lower-order bit is handled as the higher-order digit.
  
  Arguments:    table   Pointer to the table for calculation, MATHCRC32Table
                context MATHCRC32Context structure
                input   Pointer to input data.
                length  Length of input data
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
MATHi_CRC32UpdateRev(const MATHCRC32Table * table, MATHCRC32Context * context, const void *input,
                     u32 length)
{
    u32     r;
    u32     i;
    const u32 *t = table->table;
    u8     *data = (u8 *)input;

    r = (u32)*context;
    for (i = 0; i < length; i++)
    {
        r = (r >> 8) ^ t[(r ^ *data) & 0xff];
        data++;
    }
    *context = (MATHCRC32Context) r;
}


/*****************************************************************************/
/* Utility function.*/
/*****************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC8

  Description:  Calculates the CRC-8 value.

  Arguments:    table   Pointer to the table for calculation, MATHCRC8Table
                data    Pointer to input data.
                dataLength  Length of input data.

  Returns:      Calculation result.
 *---------------------------------------------------------------------------*/
u8 MATH_CalcCRC8(const MATHCRC8Table * table, const void *data, u32 dataLength)
{
    MATHCRC8Context context;
    MATH_CRC8Init(&context);
    MATH_CRC8Update(table, &context, data, dataLength);
    return MATH_CRC8GetHash(&context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC16

  Description:  Calculates the CRC-16 value.

  Arguments:    table   Pointer to the table for calculation, MATHCRC16Table
                data    Pointer to input data.
                dataLength  Length of input data.

  Returns:      Calculation result.
 *---------------------------------------------------------------------------*/
u16 MATH_CalcCRC16(const MATHCRC16Table * table, const void *data, u32 dataLength)
{
    MATHCRC16Context context;
    MATH_CRC16Init(&context);
    MATH_CRC16Update(table, &context, data, dataLength);
    return MATH_CRC16GetHash(&context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC16CCITT

  Description:  Calculates the CRC-16/CCITT values.

  Arguments:    table   Pointer to the table for calculation, MATHCRC16Table
                data    Pointer to input data.
  Arguments:    data    Pointer to input data.
                dataLength  Length of input data.

  Returns:      Calculation result.
 *---------------------------------------------------------------------------*/
u16 MATH_CalcCRC16CCITT(const MATHCRC16Table * table, const void *data, u32 dataLength)
{
    MATHCRC16Context context;
    MATH_CRC16CCITTInit(&context);
    MATH_CRC16CCITTUpdate(table, &context, data, dataLength);
    return MATH_CRC16CCITTGetHash(&context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC32

  Description:  Calculates the CRC-32 value.

  Arguments:    table   Pointer to the table for calculation, MATHCRC32Table
                data    Pointer to input data.
                dataLength  Length of input data.

  Returns:      Calculation result.
 *---------------------------------------------------------------------------*/
u32 MATH_CalcCRC32(const MATHCRC32Table * table, const void *data, u32 dataLength)
{
    MATHCRC32Context context;
    MATH_CRC32Init(&context);
    MATH_CRC32Update(table, &context, data, dataLength);
    return MATH_CRC32GetHash(&context);
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcCRC32POSIX

  Description:  Calculates the CRC-32/POSIX values.

  Arguments:    table   Pointer to the table for calculation, MATHCRC32Table
                data    Pointer to input data.
                dataLength  Length of input data.

  Returns:      Calculation result.
 *---------------------------------------------------------------------------*/
u32 MATH_CalcCRC32POSIX(const MATHCRC32Table * table, const void *data, u32 dataLength)
{
    MATHCRC32Context context;
    MATH_CRC32POSIXInit(&context);
    MATH_CRC32POSIXUpdate(table, &context, data, dataLength);
    return MATH_CRC32POSIXGetHash(&context);
}
