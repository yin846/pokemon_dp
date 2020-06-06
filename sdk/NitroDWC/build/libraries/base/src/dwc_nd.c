/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  -
  File:     dwc_nd.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_nd.c,v $
  Revision 1.35  08/25/2006 04:31:35  nakata
  Made changes so that there is no assert even if Cleanup executes when uninitialized.

  Revision 1.34  08/21/2006 07:50:01  nakata
  Corrected a memory leak occurring when initialization of the Nd library failed.

  Revision 1.33  06/22/2006 09:10:20  nakata
  restored the deleted #pragma unused

  Revision 1.32  06/22/2006 02:34:19  nakata
  Added an ASSERT when the buffer size during a file download is smaller than DWC_ND_RECVBUF_MIN.

  Revision 1.31  06/22/2006 02:03:04  nakata
  Removed the insufficient buffer error in the Download Library. (We don't want to increase the types of DWCErrors.)

  Revision 1.30  06/22/2006 00:49:50  nakata
  Changed to avoid OS_Panic when a buffer insufficiency occurs in the Download Library

  Revision 1.29  06/08/2006 06:07:33  nakata
  comments added

  Revision 1.28  06/08/2006 06:06:54  nakata
  SVL was made to return as base64 decoded, so we made changes so that it is base64 encoded before passing it to DWCi_Nd

  Revision 1.27  06/05/2006 05:49:36  nakata
  Corrected a problem where there were erroneous ASSERT conditions formatting that compared the download destination buffer size and the file size

  Revision 1.26  05/30/2006 04:33:51  nakata
  Applied a portion of the changes that set DWC_Error from Nd, leaving behind only the SVL error number

  Revision 1.25  05/29/2006 11:37:00  nakata
  When the SVL error is returned as-is, the response that deallocates the Nd library is not carried out, so we made a further revision so that the SVL error is not overwritten by the Nd library

  Revision 1.24  05/29/2006 11:13:33  nakata
  Made changes to the SVL DWCError so that Nd is not overwritten

  Revision 1.23  05/26/2006 04:09:59  nakata
  Prevented the FATAL error from being overwritten when a DWC error occurs

  Revision 1.22  05/26/2006 02:42:44  nakata
  Added around DWCError

  Revision 1.21  05/25/2006 10:42:41  nakata
  Added around DWC_Error

  Revision 1.20  05/25/2006 05:08:21  nakata
  Made changes in accordance with the addition to DWC_Nd/DWC_SVL of the feature for returning a DWC error

  Revision 1.19  05/17/2006 09:01:11  nakata
  Made revisions along with the 5/17 update of the NHTTP/DWC_Nd library

  Revision 1.18  05/17/2006 00:50:10  nakata
  Made revisions along with the changes to dwc_error.h

  Revision 1.17  05/15/2006 01:18:13  nakata
  Added support for the 5/12 vserion of the DWC_Nd library

  Revision 1.16  05/12/2006 11:24:48  nakata
  Added support for Ooki's remarks

  Revision 1.15  05/10/2006 11:50:58  nakata
  Made changes to the initialization functions in accordance with the changes to the Nd library specifications

  Revision 1.14  05/10/2006 04:45:34  nakata
  Corrected the problem where the DWC error configured during initialization was incorrect

  Revision 1.13  05/02/2006 00:57:07  nakata
  Changed to use the timing.

  Revision 1.12  05/02/2006 00:27:10  nakata
  Updated DWC_nd to the 5/2 version

  Revision 1.11  04/28/2006 13:14:03  nakata
  Changed the error when failing to get the SVL to a network error

  Revision 1.10  04/28/2006 12:23:27  nakata
  Made revisions in accordance with the changes to the constant name

  Revision 1.9  04/28/2006 10:08:43  nakata
  Added a password to DWC_NdInitAsync
  Changed functions in accordance with the changes to the DWC_NdGetFileAsync specification
  Added revised headers that go along with each of the constants in the guidelines

  Revision 1.8  04/28/2006 08:12:06  nakata
  Fixed a bug where the thread for obtaining the SVL would not end

  Revision 1.7  04/27/2006 12:46:24  takayama
  svc(SVC) -> svl(SVL)

  Revision 1.6  04/27/2006 05:27:54  nakata
  Error code-related revisions
  Made changes so that no function can be executed when a DWC error occurs

  Revision 1.5  04/26/2006 10:52:39  nakata
  Made changes to DWC_Nd so that it sets an error code

  Revision 1.4  04/26/2006 08:35:13  nakata
  Made changes so that DWC_Nd would operate even if it includes dwc.h

  Revision 1.3  04/26/2006 05:09:13  nakata
  Fixed a bug where memory was not being deallocated when initialization failed

  Revision 1.2  04/26/2006 04:19:23  nakata
  Updated Nd/NHTTP to the 4/26 version

  Revision 1.1  04/25/2006 11:46:46  nakata
  Added the DWC_Nd wrapper



  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <base/dwc_memfunc.h>
#include <base/dwc_nasfunc.h>
#include <base/dwc_error.h>
#include <base/dwc_nd.h>
#include <nd/DWCi_Nd.h>
#include <auth/dwc_svl.h>
#include <auth/util_base64.h>

/* -------------------------------------------------------------------------
        pre-processing switch
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

// [nakata] The SVL that the Nd library uses
#define DWC_ND_SVL				"9000"
#define SVLTHREADSTACKSIZE		2048
#define SVLHOSTNAMEMAX			255
#define GAMECDMAX				32
#define PASSWDMAX				32
#define TOKEN_LENGTH			(300/3*4)
/* -------------------------------------------------------------------------
        macro
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

// [nakata] Structure used to keep each of the DWCNd parameters
typedef struct {
	// [nakata] Parameters at initialization
	DWCNdCallback			callback;
	DWCNdCleanupCallback	cleanupcallback;
	char					gamecd[GAMECDMAX];
	char					passwd[PASSWDMAX];
	
	// [nakata] DWCi_Nd initialization flag
	BOOL			ndinitialized;
	
	// [nakata] SVL token storage region
	DWCSvlResult	svlresult;
	
	// [nakata] Wait thread for obtaining SVL
	u8				svlthreadstack[SVLTHREADSTACKSIZE];
	OSThread		svlthread;
} DWCNdCnt;

/* -------------------------------------------------------------------------
        variable
   ------------------------------------------------------------------------- */

// [nakata] Structure used to keep each of the parameters at initialization
static DWCNdCnt *dwcndcnt = NULL;

/* -------------------------------------------------------------------------
        prototype
   ------------------------------------------------------------------------- */

//


/* -------------------------------------------------------------------------
        function - internal
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:			DWCi_NdAlloc

  Description:	A memory management funtion used inside the Nd library

  Arguments:	size  - The size of the memory block to allocate
				align - The alignment of the memory block to allocate
  Returns:		Non-NULL - The pointer to the memory block to allocate
				NULL     - Memory allocation failed
 *---------------------------------------------------------------------------*/
static void *DWCi_NdAlloc(u32 size, int align)
{
	return DWC_AllocEx(DWC_ALLOCTYPE_ND, size, align);
}

/*---------------------------------------------------------------------------*
  Name:			DWCi_NdFree

  Description:	A memory management funtion used inside the Nd library

  Arguments:	ptr - The size of the memory block to be deallocated
  Returns:		None
 *---------------------------------------------------------------------------*/
static void DWCi_NdFree(void *ptr)
{
	DWC_Free(DWC_ALLOCTYPE_ND, ptr, 0);
}

/*---------------------------------------------------------------------------*
  Name:			DWCi_NdCallback

  Description:	A function that performs processes such as the internal processing of the Nd library callback and error code configuration.

  Arguments:	References the DWCNdCallback type
  Returns:		None
 *---------------------------------------------------------------------------*/
static void DWCi_NdCallback(DWCNdCallbackReason reason, DWCNdError error, int servererror)
{
	SDK_ASSERT(dwcndcnt);
	SDK_ASSERT(dwcndcnt->callback);
	
	// [nakata] If there is no error, then call the callback function as-is
	if(error == DWC_ND_ERROR_NONE) {
		dwcndcnt->callback(reason, error, servererror);
		return;
	}
	
	// [nakata] If an error has occurred, Ci_SetError must be called
	switch(error) {
	case DWC_ND_ERROR_ALLOC:
		DWCi_SetError(DWC_ERROR_FATAL, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND + DWC_ECODE_TYPE_ND_ALLOC);
		break;
	case DWC_ND_ERROR_BUSY:
		OS_Panic("You can't request more than one download process to DWC_Nd library.");
		break;
	case DWC_ND_ERROR_HTTP:
		DWCi_SetError(DWC_ERROR_ND_HTTP, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND + DWC_ECODE_TYPE_ND_HTTP);
		break;
	case DWC_ND_ERROR_BUFFULL:
		DWCi_SetError(DWC_ERROR_ND_ANY, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND + DWC_ECODE_TYPE_ND_BUFFULL);
		break;
	case DWC_ND_ERROR_DLSERVER:
		DWCi_SetError(DWC_ERROR_ND_ANY, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND - servererror);
		break;
	case DWC_ND_ERROR_CANCELED:
		break;
	case DWC_ND_ERROR_PARAM:
		OS_Panic("Some Parameters passed to DWC_Nd library are invalid.");
		break;
	case DWC_ND_ERROR_FATAL:
		DWCi_SetError(DWC_ERROR_FATAL, DWC_ECODE_SEQ_ADDINS + DWC_ECODE_FUNC_ND + DWC_ECODE_TYPE_ND_FATAL);
		break;
	default:
		OS_Panic("DWCi_NdCallback: Unknown error type.");
	}
	
	// [nakata] Call the callback function
	dwcndcnt->callback(reason, error, servererror);
}

/*---------------------------------------------------------------------------*
  Name:			DWCi_NdCleanupCallback

  Description:	A function that performs processes such as the internal processing of the Nd library cleanup callback and error code configuration.

  Arguments:	References the DWCNdCleanupCallback type
  Returns:		None
 *---------------------------------------------------------------------------*/
static void DWCi_NdCleanupCallback(void)
{
	DWCNdCleanupCallback callback;
	
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] Join to the thread
	OS_JoinThread(&dwcndcnt->svlthread);
	
	// [nakata] The user-specified callback is saved ahead of time
	callback = dwcndcnt->cleanupcallback;
	
	// [nakata] Free memory (since dwcndcnt is NULL here, it is not necessary to clear dwcndcnt->ndinitialized)
	DWCi_NdFree(dwcndcnt);
	dwcndcnt = NULL;
	
	// [nakata] Calls the callback configured by the user
	if(callback != NULL)
		callback();
}

/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:			DWCi_SVLTokenWaitThread

  Description:	Wait thread function for obtaining the SVL token

  Arguments:	None
  Returns:		None
 *---------------------------------------------------------------------------*/
static void DWCi_SVLTokenWaitThread(void *arg)
{
#pragma unused(arg)
	DWCSvlState	svlstate;
	char		url[SVLHOSTNAMEMAX+1];
	char		token[TOKEN_LENGTH];
	int			errorcode;

	SDK_ASSERT(dwcndcnt);
	SDK_ASSERT(dwcndcnt->callback);
	
	for(;;) {
		svlstate = DWC_SVLProcess();
		if(svlstate == DWC_SVL_STATE_SUCCESS) {
			// [nakata] SVL token successfully obtained, start Nd library initialization
			if(strlen(dwcndcnt->svlresult.svlhost)+strlen("https:///download") > SVLHOSTNAMEMAX) {
				OS_TPrintf("DWCi_SVLTokenWaitThread: DWCi_NdStartup too long svlhost.\n");
				DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_FATAL, -1);
				return;
			}
			OS_SNPrintf(url, SVLHOSTNAMEMAX+1, "https://%s/download", dwcndcnt->svlresult.svlhost);
			
			// [nakata] Since the SVL has been changed to return as base64 decoded, encode it to base64 before passing it to DWCi_Nd
			if(DWC_Auth_Base64Encode(dwcndcnt->svlresult.svltoken, strlen(dwcndcnt->svlresult.svltoken), token, TOKEN_LENGTH) < 0) {
				OS_TPrintf("DWCi_SVLTokenWaitThread: Can't encode SVL token.\n");
				DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_FATAL, -1);
				return;
			}
			
			if(DWCi_NdStartup(DWCi_NdAlloc, DWCi_NdFree, url, token, dwcndcnt->gamecd, dwcndcnt->passwd, DWCi_NdCallback) == FALSE) {
				// [nakata] Nd library initialization failure
				OS_TPrintf("DWCi_SVLTokenWaitThread: DWCi_NdStartup failed.\n");
				DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_FATAL, -1);
				return;
			}
			
			// [nakata] Nd library initialization successful
			OS_TPrintf("DWCi_SVLTokenWaitThread: DWCi_NdStartup successed.\n");
			dwcndcnt->ndinitialized = TRUE;
			DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_NONE, -1);
			return;
		}
		else if(svlstate == DWC_SVL_STATE_ERROR) {
			// [nakata] Failed to get SVL token
			// [nakata] DWC error configured
			OS_TPrintf("DWCi_SVLTokenWaitThread: Can't get SVL token.\n");
			
			// [nakata] Call the callback function
			(void)DWC_GetLastError(&errorcode);
			DWCi_SetError(DWC_ERROR_ND_ANY, errorcode);
			dwcndcnt->callback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_HTTP, -1);
			
			// [nakata] DWC error configured, but since the Nd library is handled differently than SVL and must be deallocated,
			//          the error is overwritten
			//DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_HTTP, -1);
			return;
		}
		else if(svlstate == DWC_SVL_STATE_CANCELED) {
			// [nakata] Cancel SVL token get
			//DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_HTTP, -1);
			
			// [nakata] DWC error configured, but the error is overwritten in the handling of the Nd library cancellation
			DWCi_NdCallback(DWC_ND_CBREASON_INITIALIZE, DWC_ND_ERROR_CANCELED, -1);
		}
		OS_Sleep(100);
	}
	return;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdInitAsync

  Description:	The Nd library initialization function

  Arguments:	callback - The pointer to the notification callback function that the Nd library calls.
                password - The password needed to access the download server
  Returns:		TRUE  - Initialization successful
				FALSE - Initialization failed
 *---------------------------------------------------------------------------*/
BOOL DWC_NdInitAsync(DWCNdCallback callback, char *gamecd, char *passwd)
{
	SDK_ASSERT(callback);
	SDK_ASSERT(dwcndcnt == NULL);
	SDK_ASSERT(gamecd);
	SDK_ASSERT(passwd);
	
	// [nakata] Kick if DWC error occurs
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	// [nakata] Control structure initialization
	dwcndcnt = DWCi_NdAlloc(sizeof(DWCNdCnt), 32);
	SDK_ASSERT(dwcndcnt);
	dwcndcnt->ndinitialized = FALSE;
	
	MI_CpuClear8(dwcndcnt, sizeof(DWCNdCnt));
	dwcndcnt->callback	= callback;
	strncpy(dwcndcnt->gamecd, gamecd, GAMECDMAX-1);
	strncpy(dwcndcnt->passwd, passwd, PASSWDMAX-1);
	
	// [nakata] Start getting SVL token
	if(DWC_SVLGetTokenAsync(DWC_ND_SVL, &dwcndcnt->svlresult) == FALSE) {
		// [nakata] Process for obtaining SVL token failed to start
		// [nakata] DWC error configured inside of SVL
		OS_TPrintf("DWC_NdInitAsync: DWC_SVLGetTokenAsync failed.\n");
		
		// [nakata] Memory deallocation
		DWCi_NdFree(dwcndcnt);
		dwcndcnt = NULL;
		return FALSE;
	}
	OS_TPrintf("DWC_NdInitAsync: DWC_SVLGetTokenAsync started.\n");
	
	// [nakata] Start up the wait thread for SVL token reception
	OS_CreateThread(&dwcndcnt->svlthread, DWCi_SVLTokenWaitThread, NULL, dwcndcnt->svlthreadstack+SVLTHREADSTACKSIZE, SVLTHREADSTACKSIZE, 16);
	OS_WakeupThreadDirect(&dwcndcnt->svlthread);
	
	return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdCleanupAsync

  Description:	The Nd library deallocation function

  Arguments:	callback - The pointer to the deallocation complete notification callback function that the Nd library calls.
  Returns:		None
 *---------------------------------------------------------------------------*/
void DWC_NdCleanupAsync(DWCNdCleanupCallback callback)
{
	if(dwcndcnt == NULL)
		return;
	
	dwcndcnt->cleanupcallback = callback;

	if(dwcndcnt->ndinitialized == FALSE)
		DWCi_NdCleanupCallback();
	else
		DWCi_NdCleanupAsync(DWCi_NdCleanupCallback);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdSetAttr

  Description:	The function for configuring attributes at download

  Arguments:	attr1 - Attribute character string 1
				attr2 - Attribute character string 2
				attr3 - Attribute character string 3
  Returns:		TRUE  - Attribute configuration successful
				FALSE - Attribute configuration failed
 *---------------------------------------------------------------------------*/
BOOL DWC_NdSetAttr(const char* attr1, const char* attr2, const char* attr3)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] Kick if DWC error occurs
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	// [nakata] Configure the error code based on the return value
	return DWCi_NdSetAttr((char *)attr1, (char *)attr2, (char *)attr3);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetFileListNumAsync

  Description:	A function for starting the process for getting the total number of file list entries.

  Arguments:	entrynum - The pointer to the region that stores the total number of file list entries when networking is complete
  Returns:		TRUE  - Waiting for the process that started networking to complete
				FALSE - Process failure
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileListNumAsync(int* entrynum)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] Kick if DWC error occurs
	if(DWCi_IsError()) {
		return FALSE;
	}

	return DWCi_NdGetFileListNumAsync(entrynum);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdFileListAsync

  Description:	A function that starts the process for obtaining the file list.

  Arguments:	filelist - The pointer to the array where the obtained file list is stored
				offset   - The starting position of the offset of the obtained file list
				num      - The maximum length of the file list to obtain
  Returns:		TRUE  - Waiting for the process that started networking to complete
				FALSE - Process failure
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileListAsync(DWCNdFileInfo* filelist, unsigned  offset, unsigned int num)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] Kick if DWC error occurs
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	// [nakata] 0 clear file list region
	MI_CpuClear8(filelist, sizeof(DWCNdFileInfo)*num);
	
	return DWCi_NdGetFileListAsync(filelist, (int)offset, (int)num);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetFileAsync

  Description:	A function that starts the process for obtaining the file.

  Arguments:	filename - The file name to be obtained
				buf      - The pointer to the region where the obtained file data is stored
				size     - The size of the region where the obtained file data is stored
  Returns:		TRUE  - Waiting for the process that started networking to complete
				FALSE - Process failure
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetFileAsync(DWCNdFileInfo *fileinfo, char* buf, unsigned int size)
{
#pragma unused(size)
	SDK_ASSERT(dwcndcnt);
	SDK_ASSERT(fileinfo);
	SDK_ASSERT(buf);
	SDK_ASSERT(size >= DWC_ND_RECVBUF_MIN);
	SDK_ASSERT(size >= fileinfo->size);
	
	// [nakata] Kick if DWC error occurs
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	return DWCi_NdGetFileAsync(fileinfo, buf, size);
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdCancelAsync

  Description:	A function that notifies of a disconnect in a running process.

  Arguments:	None
  Returns:		TRUE  - Either the process has been successfully interrupted or rhw process is not running
				FALSE - Process failure
 *---------------------------------------------------------------------------*/
BOOL DWC_NdCancelAsync(void)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] Kick if DWC error occurs
	if(DWCi_IsError()) {
		return FALSE;
	}
	
	return DWCi_NdCancelAsync();
}

/*---------------------------------------------------------------------------*
  Name:			DWC_NdGetProgress

  Description:	A function that gets the progress of a running process.

  Arguments:	received   - The pointer to the region where the amount of bytes of received data are stored
				contentlen - The pointer to the region where the received data content length is stored
  Returns:		TRUE  - Progress successfully obtained
				FALSE - Failed to obtain progress, or process is not running
 *---------------------------------------------------------------------------*/
BOOL DWC_NdGetProgress(u32* received, u32* contentlen)
{
	SDK_ASSERT(dwcndcnt);
	
	// [nakata] Kick if DWC error occurs
	if(DWCi_IsError()) {
		return FALSE;
	}

	return DWCi_NdGetProgress(received, contentlen);
}
