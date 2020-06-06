/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_auth
  File:     dwc_auth.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_auth.h,v $
  Revision 1.42  06/13/2006 00:00:22  nakata
  Added a browser-oriented error item

  Revision 1.41  2006/04/27 07:06:09  takayama
  Expanded ingamesn from 10 to 25 characters

  Revision 1.40  03/09/2006 04:06:36  nakata
  repaired places where newline code was not working properly

  Revision 1.39  03/02/2006 05:21:58  nakata
  Added the DWC_Svc module.
  Changed the names of the DWC_Util_Alloc/DWC_Util_Free functions to DWC_Auth_Alloc/DWC_Auth_Free.

  Revision 1.38  03/01/2006 02:43:53  nakata
  removed CAL get functionality from dwc_auth.c

  Revision 1.37  02/24/2006 05:31:08  nakata
  deleted unnecessary comments

  Revision 1.36  01/17/2006 07:15:38  nakata
  Fixed the bug related to determining ingamesn length in DWCi_RemoteLoginProcess().

  Revision 1.35  12/26/2005 06:57:43  nakata
  Changed DWC_AUTH_INGAMESN_NOTCHECKED to DWC_AUTH_INGAMESN_NOT_CHECKED.
  Changed DWC_INGAMESN_NOTCHECKED to DWC_INGAMESN_NOT_CHECKED.

  Revision 1.34  12/26/2005 06:43:19  nakata
  Moved ingamesn check functions from dwc_common.c to dwc_login.c.

  Revision 1.33  12/26/2005 04:23:17  nakata
  Added NAS return code 40 related functions to Auth.
  Added DWC_GetIngamesnCheckResult() to dwc_common.c/dwc_common.h.
  Fixed the bug that incorrect ingamesn is sent when auth is called in netcheck.

  Revision 1.32  12/21/2005 12:02:19  nakata
  Rollback Auth related code to versiont "2005-11-21 08:00:00"
  Fixed retry times count bug in DWC_Auth.

  Revision 1.21  11/17/2005 07:52:34  nakata
  Now DWC_Auth/DWC_Netcheck/DWC_Http don't use DWC_Alloc/DWC_Free.
  DWC_Netcheck sends whole HTTP packet including HTTP header to NAS
  when receives 302 message from conntest.

  Revision 1.20  10/07/2005 13:10:53  nakata
  Added SSL priority setting code to DWC_Http. But disabled by DWC_HTTP_SETSSLPRIO switch.
  Fixed ingamesn length detection part. It used strlen and now it uses wcslen for ingamesn.

  Revision 1.19  2005/09/07 08:01:02  nakata
  Modified DWC_Auth_GetError and DWC_Auth_GetResult to work when DWC_Auth
  is not initialized

  Revision 1.18  08/22/2005 11:48:02  nakata
  Add runtime custom nas setting function to DWC_Auth.

  Revision 1.17  08/20/2005 07:01:18  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_AUTH_H_
#define DWC_AUTH_H_

#include <nitro.h>

#include "dwc_http.h"

#include "ac/dwc_ac.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] definitions
//=============================================================================

// [nakata] Constant Definition
#define DWCAUTH_INGAMESN_LEN	(25+1)
#define DWCAUTH_GSBRCDBYTES		(11+1)

// [nakata] Structure used to store the result of DWCAuth

// [nakata] NAS error code list
// 0 or positive value	Succeeded
//
// -20100	Authentication cannot be completed due to an error on the DS side (due to insufficient memory, failed library initialization, or other cause)
// -20101	Cannot complete authentication due to error on the server side other than defined below
// -20102	The sent MAC address is on the NAS BAN list
// -20103	The MAC address extracted from the user name used to login/create new users is not a DS address
// -20104	Failed to create new address even in a case where the ID was already in use, the ID was changed, and a retry was attempted
// -20105	Login disallowed because ID does not exist
// -20106	An attempt was made to create more IDs than the upper limit on the number of IDs that can be created from this DS
// -20107	The sent game code is not registered in NAS
// -20108	The ID has expired
// -20109	Information required by the DS has not been sent, suspicion of invalid access
// -20110	WiFi connection service has ended for the sent game code
//
// -20200	NAS internal process error
// -20301	NAS function not implemented
//
// -20403	No reading privileges (HTTP server error)
// -20404	No file to be read (HTTP server error)
// -20500	Internal server error (HTTP server error)
// -20503	HTTP server processing error/RADIUS error (HTTP server error)

typedef struct
{
	int returncode;			// [nakata] NAS error code
	
	// [nakata] Various data obtained as a result of authentication (text string)
	char retcode[3+1];		// [nakata] HTTP result
	char datetime[14+1];	// [nakata] Server time in YYYYMMDDHHMMSS format
	char locator[50+1];		// [nakata] GameSpy server member name
	char token[300+1];		// [nakata] Token text string
	char challenge[8+1];	// [nakata] Challenge code
	char cookie[64+1];		// [nakata] cookies
} DWCAuthResult;

// [nakata] Enum type used to indicate DWCAuth error status
typedef enum
{
	DWCAUTH_E_NOERR,				// [nakata] Initialized status/currently processing
	DWCAUTH_E_BMINITERR,			// [nakata] Failed to initialize library for flash access of DS
	DWCAUTH_E_MEMERR,				// [nakata] Failed to secure memory
	DWCAUTH_E_ACERR,				// [nakata] Cannot get connection status from automatically connecting library
	DWCAUTH_E_HTTPINITERR,			// [nakata] DWCHttp cannot be initialized
	DWCAUTH_E_CALERR,				// [nakata] failed to get CAL data
	DWCAUTH_E_INVALIDPARAM,			// [nakata] Invalid parameter given
	DWCAUTH_E_CANTADDHTTPHEADER,	// [nakata] Cannot add header item to HTTP packet
	DWCAUTH_E_CANTADDPOSTITEM,		// [nakata] Cannot add POST item to HTTP packet
	DWCAUTH_E_DNSERR,				// [nakata] DNS error
	DWCAUTH_E_CANTSTARTHTTPTHREAD,	// [nakata] Cannot start HTTP thread
	DWCAUTH_E_HTTPCONERR,			// [nakata] Cannot establish TCP connection
	DWCAUTH_E_HTTPPARSEERR,			// [nakata] The received HTTP packet is invalid and cannot be passed
	DWCAUTH_E_HTTPERR,				// [nakata] An unclassified error related to DWCHttp has occurred
	DWCAUTH_E_NASPARSEERR,			// [nakata] The received NAS response is invalid and cannot be passed
	DWCAUTH_E_BMERR,				// [nakata] DS flash operation failed
	DWCAUTH_E_NASALREADYEXIST,		// [nakata] The account software attempted to create on NAS was found to already exist
	DWCAUTH_E_NASACCTEXPIRE,		// [nakata] The account for which login has been attempted has expired on NAS
	DWCAUTH_E_NASERR,				// [nakata] Error returned from NAS
	DWCAUTH_E_TOOMANYRETRY,			// [nakata] Error due to too many libraries
	DWCAUTH_E_ABORT,
	DWCAUTH_E_FINISH,				// [nakata] Authentication succeeded
	DWCAUTH_E_NOTINITIALIZED,		// [nakata] Uninitialized
	DWCAUTH_E_MAX
} DWCAuthError;

// [nakata] Structure used to store information required to initialize DWC_Auth
typedef struct
{
	// [nakata] These parameters must all be NULL terminated
	unsigned short ingamesn[DWCAUTH_INGAMESN_LEN];	// [nakata] Screen name within game
	char           gsbrcd[DWCAUTH_GSBRCDBYTES];		// [nakata] Gamespy ID Branch Code
	
	// [nakata] Memory management function
	DWCAuthAlloc	alloc;
	DWCAuthFree 	free;
} DWCAuthParam;

// [nakata] enumerated list of ingamesn check results
typedef enum
{
	DWC_AUTH_INGAMESN_NOT_CHECKED	= 0,
	DWC_AUTH_INGAMESN_VALID			= 1,
	DWC_AUTH_INGAMESN_INVALID		= 2
} DWCAuthIngamesnCheckResult;

extern DWCAuthIngamesnCheckResult DWCauthingamesncheckresult;

//=============================================================================
// [nakata] Functions
//=============================================================================

//-----------------------------------------------------------------------------
// Function : Sets a custom NAS address to be used by DWC_Auth
// Arguments : nasaddr - URL text for NAS to be used
// Return values: None
//-----------------------------------------------------------------------------
void DWC_Auth_SetCustomNas(const char *nasaddr);

//-----------------------------------------------------------------------------
// Function : Initialization of and memory allocation for the structure used by DWCAuth
// Arguments : param - Initialization parameters
//      : http - Pointer to the DWCHttp structure used by DWCAuth
// Return values: DWCAuthError
//-----------------------------------------------------------------------------
DWCAuthError DWC_Auth_Create(DWCAuthParam *param, void *http);
DWCAuthError DWC_Auth_Create_WFCUtil(DWCAuthParam *param, void *http);

//-----------------------------------------------------------------------------
// Function : DWCAuth abort
//        
// Arguments : None
// Return values: None
//-----------------------------------------------------------------------------
void DWC_Auth_Abort(void);

//-----------------------------------------------------------------------------
// Function : Frees the memory and releases the network being used by DWCAuth
// Arguments : None
// Return values: None
//-----------------------------------------------------------------------------
void DWC_Auth_Destroy(void);

//-----------------------------------------------------------------------------
// Function : OS_JoinThread is used on DWCAuth threads that have ended and then the app waits
// Arguments : None
// Return values: None
//-----------------------------------------------------------------------------
void DWC_Auth_Join(void);

//-----------------------------------------------------------------------------
// Function : Exclusively gets the DWCAuth error code
//        (This can also be used when polling the status of DWCAuth)
// Arguments : None
// Return values: DWCAuthError
//-----------------------------------------------------------------------------
DWCAuthError DWC_Auth_GetError(void);

//-----------------------------------------------------------------------------
// Function : Gets the results of DWCAuth without using Mutex
//        (Be sure to use after Auth thread ends and before using DWC_Auth_Destroy)
// Arguments : None
// Return values: DWCAuthResult
//-----------------------------------------------------------------------------
void DWC_Auth_GetResult(DWCAuthResult *result);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus


#endif // DWC_AUTH_H_
