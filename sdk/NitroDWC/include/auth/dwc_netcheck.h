/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_netcheck
  File:     dwc_netcheck.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_netcheck.h,v $
  Revision 1.18  04/06/2006 09:21:33  nakata
  Added a function for configuring the mode that ignores 302 errors.

  Revision 1.17  04/04/2006 07:47:35  nakata
  Revised line feed code.

  Revision 1.16  03/02/2006 05:21:58  nakata
  Added the DWC_Svc module.
  Changed the names of the DWC_Util_Alloc/DWC_Util_Free functions to DWC_Auth_Alloc/DWC_Auth_Free.

  Revision 1.15  12/21/2005 12:02:19  nakata
  Rollback Auth related code to versiont "2005-11-21 08:00:00"
  Fixed retry times count bug in DWC_Auth.

  Revision 1.12  09/12/2005 06:19:29  nakata
  Added DWC_Netcheck_SetCustomBlacklist function.
  Refined DWC_Http timeout/receive finish detection.
  Modified some Makefile for successful compilation.

  Revision 1.11  08/20/2005 10:28:07  sasakit
  Took care of lines that ended with \.

  Revision 1.10  08/20/2005 07:01:18  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_NETCHECK_H_
#define DWC_NETCHECK_H_
/*****************************************************************************/
/*                                                                           */
/*  DWC_Netcheck header file for connectivity assurance library*/
/*      dwc_netcheck.h*/
/*          ver. 0.5*/
/*                                                                           */
/*****************************************************************************/

#include "dwc_auth.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] definitions
//=============================================================================

// [nakata] Enum type used to indicate DWCNetcheck error status
typedef enum {
	DWCNETCHECK_E_NOERR,			// [nakata] Initialized status/currently processing

	DWCNETCHECK_E_HTTPINITERR,		// [nakata] DWCHttp cannot be initialized
	DWCNETCHECK_E_HTTPPARSEERR,		// [nakata] The received HTTP packet is invalid and cannot be passed
	DWCNETCHECK_E_HTTPERR,			// [nakata] An unclassified error related to DWCHttp has occurred
	DWCNETCHECK_E_MEMERR,			// [nakata] Failed to secure memory
	DWCNETCHECK_E_AUTHINITERR,		// [nakata] DWCAuth cannot be initialized
	DWCNETCHECK_E_AUTHERR,			// [nakata] An unclassified error related to DWCAuth has occurred
	DWCNETCHECK_E_302TWICE,			// [nakata] A 302 error was output even though Wayport certification was performed
	DWCNETCHECK_E_CANTADDPOSTITEM,	// [nakata] Cannot add POST item to HTTP packet
	DWCNETCHECK_E_NASPARSEERR,		// [nakata] Cannot pass normally as NAS packet
	
	DWCNETCHECK_E_NETUNAVAIL,		// [nakata] Netcheck connection impossible (DWCNetcheck result)
	DWCNETCHECK_E_NETAVAIL,			// [nakata] Netcheck connection possible (DWCNetcheck result)

	DWCNETCHECK_E_MAX
} DWCNetcheckError;

// [nakata] Initialization parameter for DWCNetcheck
typedef struct
{
	DWCAuthAlloc	alloc;	// [nakata] Memory allocation function
	DWCAuthFree	free;	// [nakata] Memory release function
	
	void *bmworkarea;		// [nakata] Memory used for BM (static memory area with 32-bit alignment)
} DWCNetcheckParam;

//=============================================================================
// [nakata] Functions
//=============================================================================

//-----------------------------------------------------------------------------
// Function      : Initialization of and memory allocation for the structure used by DWCNetcheck
// Arguments     : param - Initialization parameters
// Return values : DWCNetcheckError
//-----------------------------------------------------------------------------
DWCNetcheckError DWC_Netcheck_Create(DWCNetcheckParam *param);

//-----------------------------------------------------------------------------
// Function      : Frees the memory and releases the network being used by DWCNetcheck
// Arguments     : None
// Return values : None
//-----------------------------------------------------------------------------
void DWC_Netcheck_Destroy(void);

//-----------------------------------------------------------------------------
// Function      : Abort DWCNetcheck thread
// Arguments     : None
// Return values : None
//-----------------------------------------------------------------------------
void DWC_Netcheck_Abort(void);

//-----------------------------------------------------------------------------
// Function      : OS_JoinThread is used on DWCNetcheck threads that have ended and then the app waits
// Arguments     : None
// Return values : None
//-----------------------------------------------------------------------------
void DWC_Netcheck_Join(void);

//-----------------------------------------------------------------------------
// Function      : Function for polling DWC_Netcheck operations
// Arguments     : None
// Return values : BOOL - DWCNETCHECK_NOERR : Processing has not ended yet
//             Other status Netcheck processing has ended
//-----------------------------------------------------------------------------
DWCNetcheckError DWC_Netcheck_GetError(void);

//-----------------------------------------------------------------------------
// Function      : Gets the results of DWCNetcheck without using Mutex
//                 (Be sure to use after Auth thread ends and before using DWC_Netcheck_Destroy)
// Arguments     : None
// Return values : int
//-----------------------------------------------------------------------------
int DWC_Netcheck_GetReturnCode(void);

//-----------------------------------------------------------------------------
// Function      : Sets blacklisted sites for DWC_Netcheck
// Arguments     : const char * - URL of blacklisted site
// Return values : None
//-----------------------------------------------------------------------------
void DWC_Netcheck_SetCustomBlacklist(const char *url);

//-----------------------------------------------------------------------------
// Function      : DWC_Netcheck configures a flag used to determine whether or not the 302 error is connected to and recognized by the network 
// Arguments     : allow302 - TRUE  - DWC_Netcheck uses the 302 error as a connection handler
//                   FALSE - DWC_Netcheck does not use the 302 error as a connection handler
// Return values : None
//-----------------------------------------------------------------------------
void DWC_Netcheck_SetAllow302(BOOL allow302);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus

#endif // DWC_NETCHECK_H_
