/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - util_base64
  File:     util_base64.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util_base64.h,v $
  Revision 1.6  03/03/2006 05:49:40  nakata
  Added functionality to get NAS time.

  Revision 1.5  03/02/2006 06:21:58  nakata
  Added the DWC_Svc module.
  Changed the names of the DWC_Util_Alloc/DWC_Util_Free functions to DWC_Auth_Alloc/DWC_Auth_Free.

  Revision 1.4  03/01/2006 03:44:08  nakata
  removed CAL get functionality from dwc_auth.c

  Revision 1.3  08/20/2005 08:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_UTIL_BASE64_H_
#define DWC_UTIL_BASE64_H_
/*****************************************************************************/
/*                                                                           */
/*  BASE64 encode/decode*/
/*      util_base64.h*/
/*          ver. 0.5*/
/*                                                                           */
/*****************************************************************************/

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] Functions
//=============================================================================

//-----------------------------------------------------------------------------
// Function      : Encode received data using customized Base64
// Arguments     : src		- Pointer to raw data
//               : srclen	- Raw data length
//               : dst		- pointer to conversion output buffer
//               : dstlen	- Length of conversion output buffer
// Return values : int -  -1           : fails
//                        Other status : Length of conversion result
//-----------------------------------------------------------------------------
int DWC_Auth_Base64Encode(const char *src, const unsigned long srclen, char *dst, const unsigned long dstlen);

//-----------------------------------------------------------------------------
// Function      : Decode received data using customized Base64
// Arguments     : src		- Pointer to Base64 data
//               : srclen	- Raw Base64 data length
//               : dst		- pointer to conversion output buffer
//               : dstlen	- Length of conversion output buffer
// Return values : int -  -1           : fails
//                        Other status : Length of conversion result
//-----------------------------------------------------------------------------
int DWC_Auth_Base64Decode(const char *src, const unsigned long srclen, char *dst, const unsigned long dstlen);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus


#endif // DWC_UTIL_BASE64_H_
