/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_misc.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_misc.c,v $
Revision 1.3  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.2  08/18/2005 13:19:05  yasu
moved the Roundup4 function

Revision 1.1  08/07/2005 13:07:59  yasu
displayed names for error codes

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/soc.h>

struct errortable
{
    int code;
    const char*     name;
};

static struct errortable    errtable[] =
{
#include "errtable.h"
};

/*---------------------------------------------------------------------------*
Name: SOCL_GetErrName

Description: gets the SOCL_EXXXX error code name

Arguments: error code
  
Returns: error code Enum name
 *---------------------------------------------------------------------------*/
const char* SOCL_GetErrName(int errcode)
{
    int i;

    for (i = 0; i < sizeof(errtable) / sizeof(errtable[0]); i++)
    {
        if (errtable[i].code == errcode)
        {
            return errtable[i].name;
        }
    }

    return "";
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ROUNDUP4/SOCLi_RoundUp4
  
Description: rounds up to multiples of 4
to reduce code size, one macro and one function was prepared
if the size is a constant, the use of the macro should optimize
the compiler
  
Arguments: size     the value to round up to
  
Returns: rounds size to the nearest multiple of 4
 *---------------------------------------------------------------------------*/
u32 SOCLi_RoundUp4(u32 size)
{
    return SOCLi_ROUNDUP4(size);
}
