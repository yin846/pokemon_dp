/*---------------------------------------------------------------------------*
  Project:  NitroSDK - STD - include
  File:     string.h

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: string.h,v $
  Revision 1.10  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.9  10/21/2005 08:44:16  yosizaki
  add STD_TSScanf().

  Revision 1.8  09/27/2005 05:19:39  yada
  just arrange src list

  Revision 1.7  09/26/2005 08:48:53  okubata_ryoma
  Added STD_CompareNString, STD_CompareLString

  Revision 1.6  09/26/2005 05:06:24  okubata_ryoma
  Made corrections.

  Revision 1.5  09/26/2005 04:55:42  okubata_ryoma
  Added STD_CompareLString

  Revision 1.4  2005/09/06 01:25:40  yasu
  Changed the model of STD_GetStringLength to int

  Revision 1.3  09/05/2005 01:20:47  yada
  add some functions

  Revision 1.2  08/24/2005 02:51:22  yada
  add STD_SearchStringInString()

  Revision 1.1  08/19/2005 11:01:05  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_STD_STRING_H_
#define NITRO_STD_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>

//---- aliases
#define STD_StrCpy          STD_CopyString
#define STD_StrLCpy         STD_CopyLString
#define STD_StrStr          STD_SearchString
#define STD_StrLen          STD_GetStringLength
#define STD_StrCat          STD_ConcatenateString
#define STD_StrCmp          STD_CompareString
#define STD_StrNCmp         STD_CompareNString
#define STD_StrLCmp         STD_CompareLString

/*---------------------------------------------------------------------------*
  Name:         STD_CopyString

  Description:  same to strcpy

  Arguments:    destp : destination pointer
                srcp : src pointer

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
extern char *STD_CopyString(char *destp, const char *srcp);

/*---------------------------------------------------------------------------*
  Name:         STD_CopyLString

  Description:  same to strlcpy

  Arguments:    destp : destination pointer
                srcp : src pointer
                n : max length

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
extern char *STD_CopyLString(char *destp, const char *srcp, int n);

/*---------------------------------------------------------------------------*
  Name:         STD_SearchString

  Description:  same to strstr

  Arguments:    srcp : src string
                str     : string to search from src pointer

  Returns:      pointer to destination
 *---------------------------------------------------------------------------*/
extern char *STD_SearchString(const char *srcp, const char *str);

/*---------------------------------------------------------------------------*
  Name:         STD_GetStringLength

  Description:  get string length. same to strlen

  Arguments:    str     : string

  Returns:      string length
 *---------------------------------------------------------------------------*/
extern int STD_GetStringLength(const char *str);

/*---------------------------------------------------------------------------*
  Name:         STD_ConcatenateString

  Description:  concatenate strings. same to strcat

  Arguments:    str1 : original string
                str2 : string to concatenate

  Returns:      concatenated string
 *---------------------------------------------------------------------------*/
extern char *STD_ConcatenateString(char *str1, const char *str2);

/*---------------------------------------------------------------------------*
  Name:         STD_CompareString

  Description:  compare strings. same to strcmp

  Arguments:    str1, str2 : strings

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
extern int STD_CompareString(const char *str1, const char *str2);

/*---------------------------------------------------------------------------*
  Name:         STD_CompareNString

  Description:  same to strncmp

  Arguments:    str1, str2 : strings
                len    : max length

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
extern int STD_CompareNString(const char *str1, const char *str2, int len);

/*---------------------------------------------------------------------------*
  Name:         STD_CompareLString

  Description:  same to strlcmp

  Arguments:    str1, str2 : strings

  Returns:      0 if same
 *---------------------------------------------------------------------------*/
extern int STD_CompareLString(const char *str1, const char *str2);

/*---------------------------------------------------------------------------*
  Name:         STD_TSScanf

  Description:  sscanf with size reduced.
                Supports basic format specifications "%(*?)([lh]{,2})([diouxXpn])".

  Arguments:    src           Input string
                fmt           Format control string

  Returns:      Total number of substituted values.
                Returns -1 if the function ends without a substitution or an error is detected.
 *---------------------------------------------------------------------------*/
extern int STD_TSScanf(const char *src, const char *fmt, ...);

/*---------------------------------------------------------------------------*
  Name:         STD_TVSScanf

  Description:  This version supports va_List used with STD_TSScanf.
                Supports basic format specification "%(*?)([lh]{,2})[diouxX]".

  Arguments:    src           Input string
                fmt           Format control string
                vlist         Parameters

  Returns:      Total number of substituted values.
                Returns -1 if the function ends without a substitution or an error is detected.
 *---------------------------------------------------------------------------*/
extern int STD_TVSScanf(const char *src, const char *fmt, va_list vlist);


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_STD_STRING_H_*/
#endif
