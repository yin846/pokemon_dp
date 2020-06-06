#include <nitro.h>

#include <dwc_error.h>
#include <dwc_nasfunc.h>
#include <dwc_memfunc.h>

/* =======================================================================
        define
   ======================================================================== */

/* =======================================================================
        typedef
   ======================================================================== */

/* [nakata] NAL control structure*/
typedef struct {
	DWCHttp			http;		/* [nakata] HTTP Structures*/
	DWCAuthParam	authparam;	/* [nakata] Parameters for DWCAuth*/
	DWCAuthResult	result;		/* [nakata] Structures for storage of authentication results*/
} DWCNalCnt;

/* =======================================================================
        external variable
   ======================================================================== */
extern s64	DWCnastimediff;
extern BOOL	DWCnastimediffvalid;

/* =======================================================================
        static variable
   ======================================================================== */
static DWCNalCnt *nalcnt = NULL;

/* =======================================================================
        prototype
   ======================================================================== */

/* =======================================================================
        external functions(ingamesn -related)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_GetIngamesnCheckResult

  Description:	Gets the result of ingamesn determination by NAS

  Arguments:	None

  Returns:		DWC_INGAMESN_NOTCHECKED	Currently checking (login process commencing)
                DWC_INGAMESN_VALID		ingamesn of the immediately preceding login was accepted by NAS
				DWC_INGAMESN_INVALID	ingamesn of the immediately preceding login was denied by NAS
 *---------------------------------------------------------------------------*/
DWCIngamesnCheckResult DWC_GetIngamesnCheckResult(void)
{
	return (DWCIngamesnCheckResult)DWCauthingamesncheckresult;
}

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
BOOL DWC_GetDateTime(RTCDate *date, RTCTime *time)
{
	s64 localsec;

	if(DWCnastimediffvalid == FALSE)
		return FALSE;
	
	// [nakata] gets the console's time data
	if(RTC_GetDate(date) != 0 || RTC_GetTime(time) != 0)
		return FALSE;
	
	// [nakata] converts the console time value into seconds
	localsec = RTC_ConvertDateTimeToSecond(date, time);
	if(localsec == -1)
		return FALSE;
	
	// [nakata] adds the difference with the server time to the seconds
	localsec -= DWCnastimediff;
	if(localsec < 0 || localsec > 3155759999)
		return FALSE;
	
	// [nakata] generates date/time
	RTC_ConvertSecondToDateTime(date, time, localsec);
	return TRUE;
}

/* =======================================================================
        external function(SVL -related)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLGetTokenAsync

  Description:	Function for getting tokens.

  Arguments:	svl		A 4-character ASCII character string that designates service type.
				result	Pointer to the region where the obtained results are stored.

  Returns:		TRUE	Start getting
				FALSE	Cannot start getting
 *---------------------------------------------------------------------------*/
BOOL DWC_SVLGetTokenAsync(char *svl, DWCSvlResult *result)
{
	BOOL flag;
	
	SDK_ASSERT(svl);
	SDK_ASSERT(result);

	/* [nakata] Initialization process*/
	flag = DWC_Svl_Init((DWCAuthAlloc)DWC_Alloc, (DWCAuthFree)DWC_Free);
	if(flag == FALSE) {
		/* [nakata] DWC error configuration complete internally*/
		return FALSE;
	}
	
	/* [nakata] Start token get process*/
	if(strlen(svl) == 0)
		flag = DWC_Svl_GetTokenAsync("0000", result);
	else
		flag = DWC_Svl_GetTokenAsync(svl, result);
	
	if(flag == FALSE) {
		/* [nakata] Deallocate DWC_SVL work area*/
		DWC_Svl_Cleanup();
		/* [nakata] DWC error configuration complete internally*/
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLProcess

  Description:	Function that progresses the SVL token get process.

  Arguments:	None

  Returns:		DWCSvcState
 *---------------------------------------------------------------------------*/
DWCSvlState DWC_SVLProcess(void)
{
	DWCSvlState state;
	state = DWC_Svl_Process();
	if(state == DWC_SVL_STATE_ERROR || state == DWC_SVL_STATE_SUCCESS || state == DWC_SVL_STATE_CANCELED)
		/* [nakata] Deallocate DWC_SVL work area*/
		/* [nakata] DWC error configuration complete internally*/
		DWC_Svl_Cleanup();
	return state;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_SVLAbort

  Description:	Function that progresses the SVL token get process.

  Arguments:	None

  Returns:		None
 *---------------------------------------------------------------------------*/
void DWC_SVLAbort(void)
{
	DWC_Svl_Abort();
}

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
BOOL DWC_NASLoginAsync(void)
{
	if(nalcnt != NULL)
		OS_Panic("You can't request more than one NAS login request at once.\n");
	
	/* [nakata] Allocate/initialize internal work area*/
	nalcnt = DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCNalCnt));
	if(nalcnt == NULL) {
		DWCi_SetError(DWC_ERROR_FATAL, -20100);
		return FALSE;
	}
	SDK_ASSERTMSG(nalcnt, "Memory shortage");
	MI_CpuClear8(nalcnt, sizeof(DWCNalCnt));
	
	/* [nakata] IAuth parameter initialization*/
	/* [nakata] nalcnt->authparam.ingamesn is an empty set*/
	/* [nakata] nalcnt->authparam.gsbrcd is an empty set*/
	MI_CpuCopy8("", nalcnt->authparam.gsbrcd, 9);
	nalcnt->authparam.alloc	= (DWCAuthAlloc)DWC_Alloc;
	nalcnt->authparam.free	= (DWCAuthFree)DWC_Free;
	
	/* [nakata] Auth start*/
	if(DWC_Auth_Create(&nalcnt->authparam, &nalcnt->http) != DWCAUTH_E_NOERR) {
		DWCi_SetError(DWC_ERROR_FATAL, -20100);
		return FALSE;
	}
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginProcess

  Description:	Progresses the NAS login process

  Arguments:	None

  Returns:		DWCNasLoginState
 *---------------------------------------------------------------------------*/
DWCNasLoginState DWC_NASLoginProcess(void)
{
	DWCAuthResult result;
	
	if(nalcnt == NULL)
		return DWC_NASLOGIN_STATE_DIRTY;
	
	/* [nakata] Check HTTP networking state*/
	if(nalcnt->http.thread.id == 0)
		return DWC_NASLOGIN_STATE_IDLE;
	
	// [nakata] A thread ID exists, so branch by operation
	switch(DWC_Auth_GetError()) {
	case DWCAUTH_E_NOERR:
		/* [nakata] During HTTP networking*/
		return DWC_NASLOGIN_STATE_HTTP;
	case DWCAUTH_E_FINISH:
		DWC_Auth_Destroy();
		DWC_Free(DWC_ALLOCTYPE_BASE, nalcnt, sizeof(DWCNalCnt));
		nalcnt = NULL;
		return DWC_NASLOGIN_STATE_SUCCESS;
	case DWCAUTH_E_ABORT:
		/* [nakata] Do not set an error for a disconnect*/
		DWC_Auth_Destroy();
		DWC_Free(DWC_ALLOCTYPE_BASE, nalcnt, sizeof(DWCNalCnt));
		nalcnt = NULL;
		return DWC_NASLOGIN_STATE_CANCELED;
	default:
		DWC_Auth_GetResult(&result);
		DWC_Auth_Destroy();
		DWC_Free(DWC_ALLOCTYPE_BASE, nalcnt, sizeof(DWCNalCnt));
		nalcnt = NULL;
		DWCi_SetError(DWC_ERROR_AUTH_ANY, result.returncode);
		return DWC_NASLOGIN_STATE_ERROR;
	}
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NASLoginAbort

  Description:	Function for disconnecting from the Nintendo authentication server login process.

  Arguments:	None

  Returns:		None
 *---------------------------------------------------------------------------*/
void DWC_NASLoginAbort(void)
{
	DWC_Auth_Abort();
}
