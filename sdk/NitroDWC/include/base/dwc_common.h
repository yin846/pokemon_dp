/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_common
  File:     dwc_common.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_common.h,v $
  Revision 1.11  12/26/2005 07:43:20  nakata
  Moved ingamesn check functions from dwc_common.c to dwc_login.c.

  Revision 1.10  12/26/2005 05:23:18  nakata
  Added NAS return code 40 related functions to Auth.
  Added DWC_GetIngamesnCheckResult() to dwc_common.c/dwc_common.h.
  Fixed the bug that incorrect ingamesn is sent when auth is called in netcheck.

  Revision 1.9  12/21/2005 03:08:19  arakit
  Created the function, DWCi_WStrLen(), to return the string length of wide character strings.

  Revision 1.8  10/28/2005 09:11:20  arakit
  Deleted DWCi_GetCurrentTime().

  Revision 1.7  09/16/2005 07:27:13  arakit
  The maximum length of the general-purpose key/value string has been changed to 4096

  Revision 1.6  09/05/2005 11:42:02  arakit
  The upper limit on the number of characters that can be handled by functions which manipulate the key/value string has been abolished.

  Revision 1.5  08/29/2005 12:25:31  arakit
  Added the function DWCi_GetCurrentTime() for getting the current time.

  Revision 1.4  08/29/2005 06:34:15  arakit
  Prepared a function for writing the general-purpose key/value string

  Revision 1.3  08/23/2005 13:53:05  arakit
  Changed code to use MATH_Rand* as the random function

  Revision 1.2  08/20/2005 08:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_COMMON_H_
#define DWC_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif
    

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// Maximum character string lengths of common key/value character strings
#define DWC_COMMONSTR_MAX_KEY_VALUE_LEN 4096
    

//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for creating common key/value type character strings
  Arguments    : key        key character string to be set
                 value      value character string to be set
                 string     Pointer to head of storage for key/value type character string
                 separator  Character used for separating character strings
  Return value : Length of the key/value character string that is set (not including NULL terminator)
  Use          : Designates key/value character string, and creates DWC common key/value character string.
 *---------------------------------------------------------------------------*/
extern int  DWC_SetCommonKeyValueString(const char* key, const char* value, char* string, char separator);


/*---------------------------------------------------------------------------*
  Function for adding common key / value type character string
  Arguments    : key        key character string to be set
                 value      value character string to be set
                 string     Pointer to head of storage for key/value type character string
                 separator  Character used for separating character strings
  Return value : Length of the key/value character string after a key/value character string has been added (not including NULL terminator)
  Use          : Adds a key/value character string to an existing character string.
 *---------------------------------------------------------------------------*/
extern int  DWC_AddCommonKeyValueString(const char* key, const char* value, char* string, char separator);


/*---------------------------------------------------------------------------*
  Function for acquiring the value of a common key/value type character string
  Arguments    : key        key character string to be extracted
                 value      Pointer to the head of storage for the extracted value character string
                 string     key/value type character string
                 separator  Character used for separating character strings
  Return value : Length of the value character string (not including NULL terminator) Returns -1 if non existent key is designated
  Use          : From DWC common key/value type character string that is separated using the designated separator,
                 obtains the value character string that corresponds to the designated key character string.
 *---------------------------------------------------------------------------*/
extern int  DWC_GetCommonValueString(const char* key, char* value, const char* string, char separator);


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
extern u32  DWCi_GetMathRand32(u32 max);


extern u32  DWCi_WStrLen( const u16* str );


#ifdef __cplusplus
}
#endif


#endif // DWC_COMMON_H_
