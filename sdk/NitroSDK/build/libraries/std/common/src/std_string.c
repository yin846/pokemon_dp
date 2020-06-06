/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - STD
  File:     std_string.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: std_string.c,v $
  Revision 1.13  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.12  10/25/2005 09:34:59  yosizaki
  support 'c' and 's' in sscanf().

  Revision 1.11  10/21/2005 10:13:54  yosizaki
  fix bugs.

  Revision 1.10  10/21/2005 08:44:05  yosizaki
  add STD_TSScanf().

  Revision 1.9  09/27/2005 05:18:38  yada
  just arrange src list

  Revision 1.8  09/26/2005 08:49:35  okubata_ryoma
  Added STD_CompareNString, STD_CompareLString

  Revision 1.7  09/26/2005 05:06:40  okubata_ryoma
  Made corrections.

  Revision 1.6  09/26/2005 04:54:50  okubata_ryoma
  Added STD_CompareLString

  Revision 1.5  2005/09/06 01:25:23  yasu
  Changed the model of STD_GetStringLength to int

  Revision 1.4  09/05/2005 01:20:11  yada
  add some functions

  Revision 1.3  08/25/2005 00:17:17  yada
  fix SDK_ASSERT

  Revision 1.2  08/24/2005 02:50:50  yada
  add STD_SearchStringInString()

  Revision 1.1  08/19/2005 11:04:28  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

/*---------------------------------------------------------------------------*
  Name:         STD_CopyString

  Description:  same to strcpy

  Arguments:    destp : destination pointer
                srcp : src pointer

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
char   *STD_CopyString(char *destp, const char *srcp)
{
    char   *retval = destp;

    SDK_ASSERT(destp && srcp);

    while (*srcp)
    {
        *destp++ = (char)*srcp++;
    }
    *destp = '\0';

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         STD_CopyLString

  Description:  same to strlcpy

  Arguments:    destp : destination pointer
                srcp : src pointer
                n : max length

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
char   *STD_CopyLString(char *destp, const char *srcp, int n)
{
    int     i;

    SDK_ASSERT(destp && srcp && n >= 0);

    for (i = 0; i < n - 1; i++)
    {
        destp[i] = (char)*srcp;
        if (*srcp)
        {
            srcp++;
        }
    }
    destp[i] = '\0';

    return destp;
}

/*---------------------------------------------------------------------------*
  Name:         STD_SearchString

  Description:  same to strstr

  Arguments:    srcp : src string
                str     : string to search from src pointer

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
char   *STD_SearchString(const char *srcp, const char *str)
{
    int     i;
    int     n;

    SDK_ASSERT(srcp && str);

    for (i = 0; srcp[i]; i++)
    {
        n = 0;
        while (str[n] && srcp[i + n] == str[n])
        {
            n++;
        }

        if (str[n] == '\0')
        {
            return (char *)&srcp[i];
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         STD_GetStringLength

  Description:  get string length. same to strlen

  Arguments:    str     : string

  Returns:      string length
 *---------------------------------------------------------------------------*/
int STD_GetStringLength(const char *str)
{
    int     n = 0;
    while (str[n])
    {
        n++;
    }
    return n;
}

/*---------------------------------------------------------------------------*
  Name:         STD_ConcatenateString

  Description:  concatenate strings. same to strcat

  Arguments:    str1 : original string
                str2 : string to concatenate

  Returns:      concatenated string
 *---------------------------------------------------------------------------*/
char   *STD_ConcatenateString(char *str1, const char *str2)
{
    int     n = STD_GetStringLength(str1);
    (void)STD_CopyString(&str1[n], str2);
    return str1;
}

/*---------------------------------------------------------------------------*
  Name:         STD_CompareString

  Description:  compare strings. same to strcmp

  Arguments:    str1, str2 : strings

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
int STD_CompareString(const char *str1, const char *str2)
{
    while (*str1 == *str2 && *str1)
    {
        str1++;
        str2++;
    }
    return (*str1 - *str2);
}

/*---------------------------------------------------------------------------*
  Name:         STD_CompareNString

  Description:  same to strncmp

  Arguments:    str1, str2 : strings
                len    : max length

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
int STD_CompareNString(const char *str1, const char *str2, int len)
{
    if (len)
    {
        int     i;
        for (i = 0; i < len; ++i)
        {
            int     c = (int)(MI_ReadByte(str1 + i));
            int     d = (int)(MI_ReadByte(str2 + i));
            if (c != d)
            {
                return (int)(c - d);
            }
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         STD_CompareLString

  Description:  same to strlcmp

  Arguments:    str1, str2 : strings

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
int STD_CompareLString(const char *str1, const char *str2)
{
    return STD_CompareNString(str1, str2, STD_GetStringLength(str2));
}

/*---------------------------------------------------------------------------*
  Name:         STD_TSScanf

  Description:  sscanf with size reduced.
                Supports basic format specifications "%(*?)([lh]{,2})([diouxXpn])".

  Arguments:    src           Input string
                fmt           Format control string

  Returns:      Total number of substituted values.
                Returns -1 if the function ends without a substitution or an error is detected.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int STD_TSScanf(const char *src, const char *fmt, ...)
{
    int     ret;
    va_list va;
    va_start(va, fmt);
    ret = STD_TVSScanf(src, fmt, va);
    va_end(va);
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         STDi_IsSpace

  Description:  Determines if the specified character is a whitespace character.

  Arguments:    c                Character to determine.

  Returns:      TRUE if whitespace.
 *---------------------------------------------------------------------------*/
static BOOL STDi_IsSpace(int c)
{
    return (c == '\f') || (c == '\n') || (c == '\r') || (c == '\t') || (c == '\v') || (c == ' ');
}

/*---------------------------------------------------------------------------*
  Name:         STD_TVSScanf

  Description:  This version supports va_List used with STD_TSScanf.
                Supports basic format specifications.

  Arguments:    src           Input string
                fmt           Format control string
                vlist         Parameters

  Returns:      Total number of substituted values.
                Returns -1 if the function ends without a substitution or an error is detected.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL int STD_TVSScanf(const char *src, const char *fmt, va_list vlist)
{
    /* Start of verification string (required for %n)*/
    const char *src_base = src;
    /* Has the format been matched even once? (return -1 if not)*/
    BOOL    matched = FALSE;
    /* Number of substitutions made. (return 0 in this case)*/
    int     stored = 0;

    /* Analyze format string one character at a time*/
    while (*fmt)
    {
        char    c = *fmt;
        /* Skip both formatting/verification if whitespace*/
        if (STDi_IsSpace(c))
        {
            while (STDi_IsSpace(*fmt))
            {
                ++fmt;
            }
            while (STDi_IsSpace(*src))
            {
                ++src;
            }
            continue;
        }
        /* Conventional characters must match exactly*/
        else if (c != '%')
        {
            if (c != *src)
            {
                break;
            }
            /* Check next byte if SHIFT-JIS*/
            if ((unsigned int)(((unsigned char)c ^ 0x20) - 0xA1) < 0x3C)
            {
                if (c != *++src)
                {
                    break;
                }
            }
            ++src;
            ++fmt;
        }
        /* Simply compare if the escape character '%' is detected*/
        else if (fmt[1] == '%')
        {
            if (c != *src)
            {
                break;
            }
            fmt += 2;
        }
        /* Start format analysis if this is a conversion specification*/
        else
        {
            enum
            {
                flag_plus = 000002,    /* '+' */
                flag_minus = 000010,   /* '-' */
                flag_l1 = 000040,      /* "l"*/
                flag_h1 = 000100,      /* "h"*/
                flag_l2 = 000200,      /* "ll"*/
                flag_h2 = 000400,      /* "hh"*/
                flag_unsigned = 010000, /* 'o', 'u', ...*/
                flag_ignored = 020000, /* '*' */
                flag_end
            };
            u64     val = 0;
            int     flag = 0, width = 0, radix = 10;
            int     digit;

            /* Prohibit substitution flag*/
            c = *++fmt;
            if (c == '*')
            {
                flag |= flag_ignored;
                c = *++fmt;
            }
            /* Specify width*/
            while ((c >= '0') && (c <= '9'))
            {
                width = (width * 10) + c - '0';
                c = *++fmt;
            }
            /* Modify conversion*/
            switch (c)
            {
            case 'h':
                c = *++fmt;
                if (c != 'h')
                {
                    flag |= flag_h1;
                }
                else
                    flag |= flag_h2, c = *++fmt;
                break;
            case 'l':
                c = *++fmt;
                if (c != 'l')
                    flag |= flag_l1;
                else
                    flag |= flag_l2, c = *++fmt;
                break;
            }
            /* Conversion type*/
            switch (c)
            {
            case 'd':                 /* Signed decimal number*/
                radix = 10;
                goto get_integer;
            case 'i':                 /* Signed base-8/10/16 number*/
                radix = -1;
                goto get_integer;
            case 'o':                 /* Signed octal number*/
                radix = 8;
                goto get_integer;
            case 'u':                 /* Unsigned decimal number*/
                radix = 10;
                flag |= flag_unsigned;
                goto get_integer;
            case 'X':                 /* Signed hexadecimal number*/
            case 'x':                 /* Signed hexadecimal number*/
            case 'p':                 /* Pointer conversion (unsigned hexadecimal)*/
                radix = 16;
                flag |= flag_unsigned;
                goto get_integer;
            case 's':                 /* Character string up to non-whitespace*/
            case 'c':                 /* Character string of specified width only*/
                goto store_string;

            case 'n':                 /* Store match location*/
                /* Since this doesn't contribute to the substitution count, subtract it here.*/
                if (!(flag & flag_ignored))
                {
                    --stored;
                }
                val = src - src_base;
                c = *++fmt;
                goto store_integer;
            }

          invalid:
            /* Analysis ended in an unfortunate result*/
            break;

          get_integer:
            /* Integer input processing*/
            ++fmt;
            c = *src;
            /* Skip whitespace*/
            while (STDi_IsSpace(c))
            {
                c = *++src;
            }
            /* Get sign*/
            for (;; c = *++src)
            {
                if (c == '+')
                {
                    flag |= flag_plus;
                }
                else if (c == '-')
                {
                    /* '-' in an unsigned type is an error*/
                    if ((flag & flag_unsigned) != 0)
                    {
                        /*
                         * ...would be a good way to do things, but
                         * since this is ignored with gcc and CW,
                         * the SDK currently follows their lead.
                         */
                        //goto invalid;
                    }
                    flag |= flag_minus;
                }
                else
                {
                    break;
                }
            }
            /* Automatic detection of integers (%i)*/
            if (radix == -1)
            {
                if (c != '0')
                {
                    radix = 10;
                }
                else if ((src[1] == 'x') || (src[1] == 'X'))
                {
                    radix = 16;
                }
                else
                {
                    radix = 8;
                }
            }
            /* Skip "^(0[xX])?" if hexadecimal  */
            if ((radix == 16) && (c == '0') && ((src[1] == 'x') || (src[1] == 'X')))
            {
                src += 2;
                c = *src;
            }
            /* Get integer*/
            if (width == 0)
            {
                width = 0x7FFFFFFF;
            }
            for (digit = 0; digit < width; ++digit)
            {
                u32     d = (u32)(c - '0');
                if (d >= 10)
                {
                    d -= (u32)('a' - '0');
                    if (d < 6)
                    {
                        d += 10;
                    }
                    else
                    {
                        d -= (u32)('A' - 'a');
                        if (d < 6)
                        {
                            d += 10;
                        }
                    }
                }
                if (d >= radix)
                {
                    break;
                }
                c = *++src;
                val = val * radix + d;
            }
            /* No input is an error*/
            if (digit == 0)
            {
                break;
            }
            /* Adjust sign*/
            if (flag & flag_minus)
            {
                val = (u64)(val * -1);
            }
            /* At least conversion succeeded*/
            matched = TRUE;
          store_integer:
            /* Substitute*/
            if (!(flag & flag_ignored))
            {
                ++stored;
                if (flag & flag_h2)
                {
                    *va_arg(vlist, u8 *) = (u8)val;
                }
                else if (flag & flag_h1)
                {
                    *va_arg(vlist, u16 *) = (u16)val;
                }
                else if (flag & flag_l2)
                {
                    *va_arg(vlist, u64 *) = (u64)val;
                }
                else
                {
                    *va_arg(vlist, u32 *) = (u32)val;
                }
            }
            continue;

          store_string:
            /* Substitute character string*/
            {
                char   *dst = NULL;
                ++fmt;
                /* At least conversion has been successful to this point*/
                matched = TRUE;
                if (!(flag & flag_ignored))
                {
                    ++stored;
                    dst = va_arg(vlist, char *);
                }
                /*
                 * If s, search for whitespace and add '\'
                 * If c, search for the specified width
                 */
                if (c == 's')
                {
                    for (c = *src; c && STDi_IsSpace(c); c = *++src)
                    {
                    }
                    for (; !STDi_IsSpace(c); c = *++src)
                    {
                        if (dst)
                        {
                            *dst++ = c;
                        }
                    }
                    width = 1;
                }
                else
                {
                    if (width == 0)
                    {
                        width = 1;
                    }
                    for (c = *src; c && (width > 0); --width, c = *++src)
                    {
                        if (dst)
                        {
                            *dst++ = c;
                        }
                    }
                }
                if (dst)
                {
                    for (; width > 0; --width)
                    {
                        *dst++ = '\0';
                    }
                }
            }
            continue;
        }
    }

    return (*src || matched) ? stored : -1;

}
