/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_http
  File:     dwc_http.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_http.h,v $
  Revision 1.48  03/10/2006 05:07:15  nakata
  fixed issue where DWC_Http froze if the URL was 128 characters or longer

  Revision 1.47  03/02/2006 06:21:58  nakata
  Added the DWC_Svc module.
  Changed the names of the DWC_Util_Alloc/DWC_Util_Free functions to DWC_Auth_Alloc/DWC_Auth_Free.

  Revision 1.46  12/21/2005 13:02:19  nakata
  Rollback Auth related code to versiont "2005-11-21 08:00:00"
  Fixed retry times count bug in DWC_Auth.

  Revision 1.31  11/17/2005 13:17:16  nakata
  Resurrected old memory management functions.
  DWC_Netcheck sends entire HTTP packet from Wayport router to NAS.

  Revision 1.30  11/17/2005 08:52:34  nakata
  Now DWC_Auth/DWC_Netcheck/DWC_Http don't use DWC_Alloc/DWC_Free.
  DWC_Netcheck sends whole HTTP packet including HTTP header to NAS
  when receives 302 message from conntest.

  Revision 1.29  11/15/2005 07:29:47  nakata
  Now, DWC_Auth sends ESSID when the NDS connects to Hotspot AP.
  Removed unnecessary file "util_alloc.c".

  Revision 1.28  11/15/2005 02:43:26  nakata
  DWC_Http now uses DWC_Alloc/DWC_Free in base library for memory management.
  For this modification, Makefiles for AcSample/UtilSample were modified.

  Revision 1.27  11/15/2005 02:05:47  nakata
  DWC_Http now recognizes "http://xxx:yyy/zzz" style URL that has custom port number.

  Revision 1.26  10/18/2005 01:09:11  sasakit
  Gave up on a function to lower thread priority during DWC auth connections.

  Revision 1.25  10/17/2005 07:33:14  sasakit
  Activated a function for adjusting thread priority during SSL execution.

  Revision 1.24  10/07/2005 14:10:53  nakata
  Added SSL priority setting code to DWC_Http. But disabled by DWC_HTTP_SETSSLPRIO switch.
  Fixed ingamesn length detection part. It used strlen and now it uses wcslen for ingamesn.

  Revision 1.23  09/14/2005 11:36:47  nakata
  To fix stuck bug that can be occurred when DWC_Netcheck is aborted, modified DWC_Http.
  Modified DWC_Http not to use OS_DisableIrq/RestoreIrq.
  Modified DWC_Http to use CPS_SocGetLength().
  Refine debug outputs.

  Revision 1.22  09/12/2005 07:19:29  nakata
  Added DWC_Netcheck_SetCustomBlacklist function.
  Refined DWC_Http timeout/receive finish detection.
  Modified some Makefile for successful compilation.

  Revision 1.21  2005/09/07 03:47:00  nakata
  Added progress notification function to DWC_Http.

  Revision 1.20  09/05/2005 00:34:51  nakata
  Added interface function for getting loading progress percentage to DWC_Http.

  Revision 1.19  09/05/2005 03:11:29  nakata
  Now, DWC_Http can download data to pre-allocated receive buffer.

  Revision 1.18  08/22/2005 12:48:02  nakata
  Add runtime custom nas setting function to DWC_Auth.

  Revision 1.17  08/20/2005 08:01:18  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_HTTP_H_
#define DWC_HTTP_H_

#include <nitroWiFi.h>
#include <nitrowifi/version.h>

#include "util_alloc.h"
#include "util_base64.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] definitions
//=============================================================================

// [nakata] Maximum length of URL
#define DWCHTTP_MAXURL			256

// [nakata] Stack size
#define DWCHTTP_STACKSIZE		4096

#define DWCHTTP_MAX_LABELVALUE	32

// [nakata] Be sure to use define when setting the SSL priority to ON
// [nakata] However, be sure to check that an inconvenient SVC_WaitVBlankIntr() is not pending!
//#define DWC_HTTP_SETSSLPRIO // [sasakit] Blocked more connections than anticipated, so gave up.

typedef enum
{
	DWCHTTP_E_NOERR,
	
	DWCHTTP_E_MEMERR,
	DWCHTTP_E_DNSERR,
	DWCHTTP_E_CONERR,
	DWCHTTP_E_SENDTOUT,
	DWCHTTP_E_SENDERR,
	DWCHTTP_E_RECVTOUT,
	DWCHTTP_E_ABORT,
	
	DWCHTTP_E_FINISH,
	DWCHTTP_E_MAX
} DWCHttpError;

// [nakata] Flag for indicating whether action is POST or GET
typedef enum
{
	DWCHTTP_POST,
	DWCHTTP_GET
} DWCHttpAction;

// [nakata] Structure for managing buffers all together
typedef struct {
	char *buffer;
	char *write_index;
	char *buffer_tail;
	unsigned long length;
} DWCHttpBuffer;

// [nakata] Structure for managing label and result strings
typedef struct
{
	char *label;
	char *value;
} DWCHttpLabelValue;

// [nakata] Structure for managing groups of labels
typedef struct
{
	DWCHttpLabelValue *entry;
	int len;
	int index;
} DWCHttpParseResult;

// [nakata] DWCHttp initialization parameter
typedef struct {
	const char		*url;
	DWCHttpAction	action;
	unsigned long	len_recvbuf;
	
	DWCAuthAlloc	alloc;
	DWCAuthFree		free;
	
	BOOL			ignoreca;
	
	int				timeout;
} DWCHttpParam;

// [nakata] Structure for storing internal information to be used by DWCHttp

typedef struct {
	unsigned char	stack[DWCHTTP_STACKSIZE];
	
	// [nakata] Initialization flag
	u8				initflag;
	
	// [nakata] Initialization parameter
	DWCHttpParam	param;
	DWCHttpError	error;					// [nakata] Last error code generated
	
	// [nakata] Host/SSL related
	char			url[DWCHTTP_MAXURL];	// [nakata] Array for saving URLs
	char			*hostname;				// [nakata] Pointer to hostname (points to somewhere inside the url)
	char			*filepath;				// [nakata] Pointer to file path (points to somewhere inside the url)
	BOOL			ssl_enabled;			// [nakata] Flag indicating whether or not to use SSL
	unsigned short		port;
	
	// [nakata] CPS socket related
	CPSSoc				soc;
	CPSSslConnection	con;
	unsigned char		*lowrecvbuf;//[1460*2];
	unsigned char		*lowsendbuf;//[1514];
	u32					lowentropydata[OS_LOW_ENTROPY_DATA_SIZE/sizeof(u32)];
	
	// [nakata] Pointers to send/receive buffers, etc.
	int				num_postitem;
	DWCHttpBuffer	req;
	DWCHttpBuffer	rep;
	
	// [nakata] an environment to store values that are used often
	OSMutex			content_len_mutex;
	int				content_len;
	int				receivedbody_len;
	
	// [nakata] Environment for storing the receive data base results
	DWCHttpLabelValue	labelvalue[DWCHTTP_MAX_LABELVALUE];

	// [nakata] Send/receive thread related
	OSThread		thread;
	OSMutex			mutex;
	BOOL			abort;
} DWCHttp;

//=============================================================================
// [nakata] Functions
//=============================================================================

//-----------------------------------------------------------------------------
// Function     : Initialization of and memory allocation for the structure used by DWC_Http
// Arguments    : http              - Pointer to the DWC_Http structure to be processed
//              : max_recv_bodysize - Maximum packet length to be received
//              : alloc             - Pointer to the memory allocation function used internally
//              : free              - Pointer to the memory free function used internally
// Return values: BOOL   - TRUE  : Succeeded
//                         FALSE : Fails
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_Create(DWCHttp *http, DWCHttpParam *param);

//-----------------------------------------------------------------------------
// Function      : Free memory used by DWC_Http, close sockets and free SSL
// Arguments     : http   - Pointer to the DWC_Http structure to be processed
// Return values : BOOL   - TRUE  : Succeeded
//                          FALSE : Fails
//-----------------------------------------------------------------------------
void DWC_Http_Destroy(DWCHttp *http);

//-----------------------------------------------------------------------------
// Function      : Function for creating DWCHttp thread
//        
// Arguments     : http - Pointer to the DWC_Http structure to be processed
//               : prio - Priority of the HTTP thread
// Return values : DWCHttpError
//-----------------------------------------------------------------------------
void DWC_Http_StartThread(DWCHttp *http, u32 prio);

//-----------------------------------------------------------------------------
// Function      : Abort DWCHttp
//        
// Arguments     : http - Pointer to the DWC_Http structure to be processed
// Return values : None
//-----------------------------------------------------------------------------
void DWC_Http_Abort(DWCHttp *http);

//-----------------------------------------------------------------------------
// Function      : Get DWCHttp receive data
//        
// Arguments     : http - Pointer to the DWC_Http structure to be processed
// Return values : int  - Receive percentage from 0 to 255
//                      - -1  Not receiving
//-----------------------------------------------------------------------------
int DWC_Http_GetRecvProgress(DWCHttp *http);

//-----------------------------------------------------------------------------
// Function      : Add items to the receive header
// Arguments     : http  - Pointer to the DWC_Http structure to be processed
//               : label - Label (NULL termination required)
//               : data  - Data (NULL termination required)
// Return values : DWCHttpError
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_Add_HeaderItem(DWCHttp *http, char *label, char *data);

//-----------------------------------------------------------------------------
// Function      : Adds base-64 encoded send POST data
// Arguments     : http      - Pointer to the DWC_Http structure to be processed
//               : label     - Label (NULL termination required)
//               : data      - Data (NULL termination not required)
//               : data_len  - Data length
// Return values : DWCHttpError
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_Add_PostBase64Item(DWCHttp *http, const char *label, const char *data, unsigned long data_len);

//-----------------------------------------------------------------------------
// Function      : Adds the specified string to BODY of the send packet
// Arguments     : http      - Pointer to the DWC_Http structure to be processed
//               : data      - Data (NULL termination required)
//               : data_len  - Data length
// Return values : DWCHttpError
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_Add_Body(DWCHttp *http, const char *data);

//-----------------------------------------------------------------------------
// Function      : Adds Content-Length to the send packet inside DWCHttp and enables sending
//        
// Arguments     : http      - Pointer to the DWC_Http structure to be processed
// Return values : DWCHttpError
//-----------------------------------------------------------------------------
DWCHttpError DWC_Http_FinishHeader(DWCHttp *http);

//-----------------------------------------------------------------------------
// Function      : Header entry for the receive packet and FORM data strings are stored in array_entry as labeled strings
//                 Receive packet contents destroyed by NULL characters
//        
// Arguments     : http      - Pointer to the DWC_Http to be processed
//                 noparsebody - If TRUE, the HTTP BODY is registered as an httpbody entry without being passed
//                 buffer      - HTTP packet to be analyzed
// Return values : BOOL   - TRUE  : Analysis succeeded
//                          FALSE : HTTP cannot be analyzed
//-----------------------------------------------------------------------------
BOOL DWC_Http_ParseResult(DWCHttp *http, BOOL noparsebody);

//-----------------------------------------------------------------------------
// Function      : Changes the priority level when making TCP connections for DWCHttp
// Arguments     : prio - Priority (from 0 to 31)
// Return values : None
//-----------------------------------------------------------------------------
#ifdef DWC_HTTP_SETSSLPRIO
void DWC_Http_SetSSLPrio(u32 prio);
#endif

//-----------------------------------------------------------------------------
// Group of functions for manipulating pass results
//-----------------------------------------------------------------------------
char	*DWC_Http_GetResult(DWCHttp *http, char *label);
int		DWC_Http_GetBase64DecodedResult(DWCHttp *http, char *label, char *buffer, unsigned long buffer_len);
BOOL	DWC_Http_GetRawResult(DWCHttp *http, char *label, char *buffer, int buffer_len);

#ifdef __cplusplus
}
#endif // [nakata] __cplusplus

#endif // DWC_HTTP_H_
