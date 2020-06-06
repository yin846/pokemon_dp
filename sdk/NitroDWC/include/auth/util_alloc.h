/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - util_alloc
  File:     util_alloc.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util_alloc.h,v $
  Revision 1.15  04/13/2006 11:35:19  hayashi
  Change prefix Ranking->Rnk

  Revision 1.14  03/02/2006 05:21:58  nakata
  Added the DWC_Svc module.
  Changed the names of the DWC_Util_Alloc/DWC_Util_Free functions to DWC_Auth_Alloc/DWC_Auth_Free.

  Revision 1.13  12/22/2005 01:39:06  nakata
  Resurrected util_alloc.h.

  Revision 1.8  11/15/2005 10:55:40  nakata
  Resurrect util_alloc.h for successful compilation.

  Revision 1.6  09/01/2005 00:19:46  nakata
  Removed util_alloc.c related codes from DWC_Http/DWC_Auth/DWC_Netcheck.

  Revision 1.5  08/20/2005 07:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_UTIL_ALLOC_H_
#define DWC_UTIL_ALLOC_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] Structures
//=============================================================================

// [nakata] Interface type for functions that allocate/free memory
typedef void *(*DWCAuthAlloc)(unsigned long name, long size);
typedef void (*DWCAuthFree)(unsigned long name, void *ptr, long size);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus

#endif // DWC_UTIL_ALLOC_H_
