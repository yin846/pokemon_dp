/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_nasfunc
  File:     dwc_nasfunc.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_nasfunc.h,v $
  Revision 1.11  05/25/2006 09:07:18  nakata
  Added DWC_NASLOGIN_STATE_CANCELED

  Revision 1.10  2006/04/27 12:46:24  takayama
  svc(SVC) -> svl(SVL)

  Revision 1.9  04/05/2006 02:15:22  nakata
  Fixed the error pointed out by Ooki.

  Revision 1.8  03/27/2006 05:49:47  nakata
  Changed the prefix NAL to NasLogin

  Revision 1.7  03/23/2006 05:16:01  nakata
  Added DWC_NAL-related codes.

  Revision 1.6  03/20/2006 07:47:24  nakata
  Made changes corresponding to the SVL specification revisions.

  Revision 1.5  03/20/2006 05:02:33  nakata
  *** empty log message ***

  Revision 1.4  03/17/2006 07:58:39  nakata
  Changed the comment formatting.

  Revision 1.3  03/09/2006 04:11:44  nakata
  added DWC_GetDateTime function

  Revision 1.2  03/07/2006 01:57:40  nakata
  Manually added reference to DWC_GetNasTime function.

  Revision 1.1  03/07/2006 01:18:16  nakata
  Performed necessary changes resulting from collecting added NAS functionality into dwc_nasfunc.c/dwc_nasfunc.h.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_NASFUNC_H_
#define DWC_NASFUNC_H_

#include "auth/dwc_auth.h"
#include "auth/dwc_svl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =======================================================================
        enum
   ======================================================================== */

/* [nakata] enumerated list of ingamesn check results*/
typedef enum {
	DWC_INGAMESN_NOT_CHECKED	= DWC_AUTH_INGAMESN_NOT_CHECKED,
	DWC_INGAMESN_VALID			= DWC_AUTH_INGAMESN_VALID,
	DWC_INGAMESN_INVALID		= DWC_AUTH_INGAMESN_INVALID
} DWCIngamesnCheckResult;

// [nakata] Enum type used to indicate DWC_Nal state
typedef enum {
	DWC_NASLOGIN_STATE_DIRTY	= 0,	// not initialized
	DWC_NASLOGIN_STATE_IDLE,			// Process not running
	DWC_NASLOGIN_STATE_HTTP,			// HTTP networking active
	DWC_NASLOGIN_STATE_SUCCESS,		// Login successful
	DWC_NASLOGIN_STATE_ERROR,		// Login failed
	DWC_NASLOGIN_STATE_CANCELED,	// Login cancelled
	DWC_NASLOGIN_STATE_MAX
} DWCNasLoginState;

/* =======================================================================
        external function(ingamesn -related)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_GetIngamesnCheckResult

  Description:	Gets the result of ingamesn determination by NAS

  Arguments:	None

  Returns:		DWC_INGAMESN_NOTCHECKED	Currently checking (login process commencing)
                DWC_INGAMESN_VALID		ingamesn of the immediately preceding login was accepted by NAS
				DWC_INGAMESN_INVALID	ingamesn of the immediately preceding login was denied by NAS
 *---------------------------------------------------------------------------*/
DWCIngamesnCheckResult DWC_GetIngamesnCheckResult(void);

/* =======================================================================
        external function(NAS time-related)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_GetIngamesnCheckResult

  Description:	gets the NAS time in RTC format

  Arguments:	date	pointer to the region where the resulting RTCDate is written
				time	pointer to the region where the resulting RTCTime is written

  Returns:		TRUE	Get successful
				FALSE - failed
 *---------------------------------------------------------------------------*/
BOOL DWC_GetDateTime(RTCDate *date, RTCTime *time);

/* =======================================================================
        external function(SVL -related)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLGetToken

  Description:	Function for getting tokens.

  Arguments:	svl		A 4-character ASCII character string that designates service type.
				result	Pointer to the region where the obtained results are stored.

  Returns:		TRUE	Start getting
				FALSE	Cannot start getting
 *---------------------------------------------------------------------------*/
BOOL DWC_SVLGetTokenAsync(char *svl, DWCSvlResult *result);

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLProcess

  Description:	Function that progresses the SVL token get process.

  Arguments:	None

  Returns:		DWCSvlState
 *---------------------------------------------------------------------------*/
DWCSvlState DWC_SVLProcess(void);

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLAbort

  Description:	Function that progresses the SVL token get process.

  Arguments:	None

  Returns:		None
 *---------------------------------------------------------------------------*/
void DWC_SVLAbort(void);

/* =======================================================================
        external function(NAS authentication-related)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginAsync

  Description:	NAS login start

  Arguments:	None

  Returns:		TRUE  - Login process successfully started
				FALSE - Login process failed to start
 *---------------------------------------------------------------------------*/
BOOL DWC_NASLoginAsync(void);

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginProcess

  Description:	Progresses the NAS login process

  Arguments:	None

  Returns:		DWCNasLoginState
 *---------------------------------------------------------------------------*/
DWCNasLoginState DWC_NASLoginProcess(void);

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginAbort

  Description:	Function for disconnecting from the Nintendo authentication server login process.

  Arguments:	None

  Returns:		None
 *---------------------------------------------------------------------------*/
void DWC_NASLoginAbort(void);

#ifdef __cplusplus
}
#endif

#endif // DWC_LOGIN_H_
