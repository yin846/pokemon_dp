#include <nitro.h>

// the original header
//---------------------------------------------------------
#include <base/dwc_report.h>
#include <base/dwc_error.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_ghttp.h>

// Structure declarations
//---------------------------------------------------------
typedef struct DWCGHTTPParamEntry {
	DWCGHTTPParam param;
	char *buf;
	int req;
	struct DWCGHTTPParamEntry *nextentry;
}DWCGHTTPParamEntry;

// Declare internal variables
//---------------------------------------------------------
static DWCGHTTPParamEntry *paramhead = NULL;
static int ghttpinitcount = 0;

// Prototype Declaration
//---------------------------------------------------------
static GHTTPBool GHTTPCompletedCallback( GHTTPRequest   request,
										 GHTTPResult	result,
										 char *			buffer,
										 GHTTPByteCount	bufferLen,
										 void *			param);

static void GHTTPProgressCallback(GHTTPRequest   request, 
                                  GHTTPState     state, 
                                  const char *   buffer, 
                                  GHTTPByteCount bufferLen, 
                                  GHTTPByteCount bytesReceived, 
                                  GHTTPByteCount totalSize, 
                                  void *         param );

static DWCGHTTPResult DWCi_HandleGHTTPError(DWCGHTTPResult result);

static DWCGHTTPParamEntry *DWCi_AppendDWCGHTTPParam(const DWCGHTTPParam *param);
static void DWCi_RemoveDWCGHTTPParamEntry(DWCGHTTPParamEntry *entry);
static void DWCi_RemoveAllDWCGHTTPParamEntry(void);
static DWCGHTTPParamEntry *DWCi_FindDWCGHTTPParamEntryByReq(int req);

//=============================================================================
/*!
 *	@brief	library initialization
 *
 *	@param	gamename	[in] gamename
 *
 *	@retval	TRUE		Succeeded
 *	@retval FALSE   	Failed
 */
//=============================================================================
BOOL	DWC_InitGHTTP( const char* gamename ){
#pragma unused(gamename)

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_InitGHTTP\n");

    //
	// Open GHTTP
	//
	ghttpStartup();
	
	ghttpinitcount++;
	
	return TRUE;

}

//=============================================================================
/*!
 *	@brief	Shutdown libraries
 *
 *	@retval	TRUE		Succeeded
 *	@retval	FALSE   	Failed
 */
//=============================================================================
BOOL	DWC_ShutdownGHTTP( void )
{
	DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_ShutdownGHTTP\n");
	if(ghttpinitcount <= 0)
		return TRUE;
	
	//
	// Exit GHTTP
	//
	ghttpCleanup();
	
	//
	// Release parameters
	//
	ghttpinitcount--;
	if(ghttpinitcount == 0)
		DWCi_RemoveAllDWCGHTTPParamEntry();

	return TRUE;
}


//=============================================================================
/*!
 *	@brief	Send data
 *
 *	@retval	TRUE		Succeeded
 *	@retval	FALSE   	Failed
 */
//=============================================================================
BOOL	DWC_ProcessGHTTP( void ){

    if (DWCi_IsError()) return FALSE;

	//
	// Wait for GHTTP response
	//
	ghttpThink();

	return TRUE;

}

//=============================================================================
/*!
 *	@brief	Callback for HTTP request
 *
 *	@retval	GHTTPTrue		Free buffer
 *	@retval	GHTTPFalse   	Buffer not freed
 */
//=============================================================================
static GHTTPBool GHTTPCompletedCallback( GHTTPRequest   request,
										 GHTTPResult	result,
										 char *			buffer,
										 GHTTPByteCount	bufferLen,
										 void *			param)
{
#pragma unused(request)

    DWCGHTTPParamEntry *entry = (DWCGHTTPParamEntry *)param;
	DWCGHTTPParam *parameter = &entry->param;

	// Gets pointer to callback function
	DWCGHTTPCompletedCallback callback = (DWCGHTTPCompletedCallback)parameter->completedCallback;

    BOOL buffer_clear = parameter->buffer_clear;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "GHTTPCompleteCallback result : %d\n", result);

    if(callback){
        if(result == GHTTPSuccess){
            callback( buffer, (int)bufferLen, (DWCGHTTPResult)result, parameter->param );
        }else{
            DWCi_HandleGHTTPError((DWCGHTTPResult)result);
            callback( NULL, 0, (DWCGHTTPResult)result, parameter->param );
        }
    }
    else{
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Callback is NULL\n");
    }
	
	// Release buffers allocated by the wrapper
	if(result != GHTTPSuccess || // Automatically release the buffer unconditionally if GHTTP fails (because the callback buf is NULL)
	   buffer_clear == TRUE // Automatically release the buffer if the buffer clear flag is TRUE
	  ) {
		if(entry->buf)
			DWC_Free(DWC_ALLOCTYPE_BASE, (void*)entry->buf, 0);
		else
			buffer_clear = TRUE;
	}

	//DWC_Free(DWC_ALLOCTYPE_BASE, (void*)parameter, 0);
    DWCi_RemoveDWCGHTTPParamEntry(entry);
	
    if(!buffer_clear){
        return GHTTPFalse;
    }

	return GHTTPTrue;
}

//=============================================================================
/*!
 *	@brief	Callback during HTTP request
 */
//=============================================================================
static void GHTTPProgressCallback(GHTTPRequest   request, 
                                  GHTTPState     state, 
                                  const char *   buffer, 
                                  GHTTPByteCount bufferLen, 
                                  GHTTPByteCount bytesReceived, 
                                  GHTTPByteCount totalSize, 
                                  void *         param )
{
#pragma unused(request)

    DWCGHTTPParam* parameter = &((DWCGHTTPParamEntry *)param)->param;

	// Gets pointer to callback function
	DWCGHTTPProgressCallback callback = (DWCGHTTPProgressCallback)parameter->progressCallback;

    if(callback){
        callback( (DWCGHTTPState)state, buffer, (int)bufferLen, (int)bytesReceived, (int)totalSize, parameter->param );
    }

}

//=============================================================================
/*!
 *	@brief	Create object to be posted
 *
 *	@param	post	  [in] Object to be created
 *
 */
//=============================================================================
void    DWC_GHTTPNewPost( DWCGHTTPPost* post ){

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_GHTTPNewPost\n");
    
    *post = ghttpNewPost();

    if (post == NULL){
        DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
    }
}

//=============================================================================
/*!
 *	@brief	For Addpost
 *
 *	@param	post	  [in] Object to be added
 *	@param	key 	  [in] key name of data to be posted
 *	@param	buf 	  [in] contents of data to be posted
 *
 *	@retval	TRUE		Succeeded
 *	@retval	FALSE   	Failed
 */
//=============================================================================
BOOL    DWC_GHTTPPostAddString( DWCGHTTPPost* post, const char* key, const char* value ){
    
    if(!ghttpPostAddString(*post, key, value)){
        DWC_Printf(DWC_REPORTFLAG_WARNING, "DWC_GHTTPPostAddString FALSE\n");
        return FALSE;
    }
    else {
        return TRUE;
    }
}

//=============================================================================
/*!
 *	@brief	Send data
 *
 *	@param	url		[in] URL for location where list can be retrieved
 *	@param	post	[in] Object to be posted
 *	@param	proc	[in] Callback used to process response
 *	@param	param	[in] Parameter for the above callback
 *
 *	@retval	req		 0 or higher request identifier (success)
 *                  -1 or less, the request error value
 */
//=============================================================================
int 	DWC_PostGHTTPData( const char* url, GHTTPPost* post, DWCGHTTPCompletedCallback completedCallback, void* param){

   	GHTTPRequest req;
	DWCGHTTPParamEntry *entry = NULL;
	DWCGHTTPParam parameter_instance;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_PostGHTTPData\n");

    if (DWCi_IsError()) return DWC_GHTTP_IN_ERROR;
    
    parameter_instance.param = param;
    parameter_instance.completedCallback = completedCallback;
    parameter_instance.progressCallback = NULL;
    parameter_instance.buffer_clear = TRUE;

	//parameter = DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCGHTTPParam));
    entry = DWCi_AppendDWCGHTTPParam(&parameter_instance);
    
    if(!entry){
        DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
        completedCallback( NULL, 0, (DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY, param );
        return (int)DWC_GHTTP_INSUFFICIENT_MEMORY;
    }
    
    //parameter->param = param;
    //parameter->completedCallback = completedCallback;
    //parameter->progressCallback = NULL;
    //parameter->buffer_clear = TRUE;

	req = ghttpPost( url, *post, GHTTPFalse, 
                      GHTTPCompletedCallback, (void *)entry );
   
    if(req < 0){
        DWCi_HandleGHTTPError((DWCGHTTPResult)req);
        completedCallback( NULL, 0, (DWCGHTTPResult)req, param );
		DWCi_RemoveDWCGHTTPParamEntry(entry);
    }
	
	entry->req = req;

	return (int)req;

}

//=============================================================================
/*!
 *	@brief	Receive data
 *
 *	@param	url		[in] URL for location where list can be retrieved
 *	@param	proc	[in] Callback used to process response
 *	@param	param	[in] Parameter for the above callback
 *
 *	@retval	req		 0 or higher request identifier (success)
 *                  -1 or less, the request error value
 */
//=============================================================================
int 	DWC_GetGHTTPData( const char* url, DWCGHTTPCompletedCallback completedCallback, void* param ){
    
   	GHTTPRequest req;
    DWCGHTTPParamEntry *entry = NULL;
	DWCGHTTPParam parameter_instance;
	
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_GetGHTTPData\n");

    if (DWCi_IsError()) return DWC_GHTTP_IN_ERROR;
    
    parameter_instance.param = param;
    parameter_instance.completedCallback = completedCallback;
    parameter_instance.progressCallback  = NULL;
    parameter_instance.buffer_clear = TRUE;

	entry = DWCi_AppendDWCGHTTPParam(&parameter_instance);
    
    if(!entry){
        DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
        completedCallback( NULL, 0, (DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY, param );
        return (int)DWC_GHTTP_INSUFFICIENT_MEMORY;
    }

    req = ghttpGet( url, GHTTPFalse, GHTTPCompletedCallback, (void *)entry );

    if(req < 0){
        DWCi_HandleGHTTPError((DWCGHTTPResult)req);
        completedCallback( NULL, 0, (DWCGHTTPResult)req, param );
        DWCi_RemoveDWCGHTTPParamEntry(entry);
    }
    
	entry->req = req;
	
	return (int)req;

}

//=============================================================================
/*!
 *	@brief	Receive data (specify buffer size and ProgressCallback)
 *
 *	@param	url		            [in] URL for location where list can be retrieved
 *	@param	bufferlen           [in] Receive buffer size
 *	@param	progressCallback	[in] Callback called during processing
 *  @param  completedCallback  [in] Callback used to process response
 *	@param	param	            [in] Parameter for the above callback
 *
 *	@retval	req		 0 or higher request identifier (success)
 *                  -1 or less, the request error value
 */
//=============================================================================
int 	DWC_GetGHTTPDataEx( const char* url, int bufferlen, BOOL buffer_clear, DWCGHTTPProgressCallback progressCallback, DWCGHTTPCompletedCallback completedCallback, void* param ){

    char* buffer = NULL;
   	GHTTPRequest req;
    DWCGHTTPParamEntry *entry = NULL;
	DWCGHTTPParam parameter_instance;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_GetGHTTPDataEx\n");

    if (DWCi_IsError()) return DWC_GHTTP_IN_ERROR;

	parameter_instance.param = param;
    parameter_instance.completedCallback = completedCallback;
    parameter_instance.progressCallback  = progressCallback;
    parameter_instance.buffer_clear = buffer_clear;

    //parameter = DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCGHTTPParam));
   	entry = DWCi_AppendDWCGHTTPParam(&parameter_instance);

    if(!entry){
        DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
        completedCallback( NULL, 0, (DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY, param );
        return (int)DWC_GHTTP_INSUFFICIENT_MEMORY;
    }

    if(bufferlen > 0){
        buffer = (char*)DWC_Alloc(DWC_ALLOCTYPE_BASE, (u32)bufferlen);

        if(!buffer){
            DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
            completedCallback( NULL, 0, (DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY, param );
			DWCi_RemoveDWCGHTTPParamEntry(entry);
            return (int)DWC_GHTTP_INSUFFICIENT_MEMORY;
        }
		
		// Record the fact taht the buffer was allocated by the wrapper at entry
		entry->buf = buffer;
    }

    //parameter->param = param;
    //parameter->completedCallback = completedCallback;
    //parameter->progressCallback  = progressCallback;
    //parameter->buffer_clear = buffer_clear;

    if(progressCallback)
        req = ghttpGetEx( url, NULL, buffer, bufferlen, NULL, GHTTPFalse, GHTTPFalse,
                          GHTTPProgressCallback, GHTTPCompletedCallback, (void *)entry );
    else
        req = ghttpGetEx( url, NULL, buffer, bufferlen, NULL, GHTTPFalse,
                          GHTTPFalse, NULL, GHTTPCompletedCallback, (void *)entry );
        
    if(req < 0){
        DWCi_HandleGHTTPError((DWCGHTTPResult)req);
        completedCallback( NULL, 0, (DWCGHTTPResult)req, param );
		if(entry->buf != NULL)
            DWC_Free(DWC_ALLOCTYPE_BASE, (void*)entry->buf, 0);
        DWCi_RemoveDWCGHTTPParamEntry(entry);
    }
    
	entry->req = req;
	
	return (int)req;

}

//=============================================================================
/*!
 *	@brief	Cancel HTTP request
 *
 *	@param	req		[in] the ID of the HTTP request being halted (the return value from DWC_GetGHTTPDataEx)
 *
 *	@retval	None
 */
//=============================================================================
void DWC_CancelGHTTPRequest(int req)
{
	DWCGHTTPParamEntry *entry;
	ghttpCancelRequest((GHTTPRequest)req); 
	
	entry = DWCi_FindDWCGHTTPParamEntryByReq(req);
	if(entry == NULL)
		return;
	if(entry->buf != NULL)
		DWC_Free(DWC_ALLOCTYPE_BASE, (void*)entry->buf, 0);
	DWCi_RemoveDWCGHTTPParamEntry(entry);
}

//=============================================================================
/*!
 *	@brief	Check communication status
 *
 *	@param	req		[in] Request identifier
 *
 *	@retval	state	Communication status
 *  @retval False   Failed
 */
//=============================================================================
DWCGHTTPState DWC_GetGHTTPState( int req )
{
    if(req < 0){
        return DWCGHTTPFalse;
    }
    else{
        return (DWCGHTTPState)(ghttpGetState(req));
    }
}

//=============================================================================
/*!
 *	@brief	Function for handling GHTTP error processing
 *
 *	@param	result	[in] GHTTP processing result
 *
 *	@retval	state	GHTTP processing result type (parameters are returned unchanged)
 */
//=============================================================================
static DWCGHTTPResult DWCi_HandleGHTTPError(DWCGHTTPResult result)
{
    int errorCode = DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_HTTP;
    DWCError dwcError = DWC_ERROR_GHTTP_ANY;

    if (result == DWC_GHTTP_SUCCESS) return DWC_GHTTP_SUCCESS;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Main, DWCGHTTP error %d\n", result);

    // Possible Error values returned from GHTTP functions.
    switch (result){
	case DWC_GHTTP_FAILED_TO_OPEN_FILE:
        errorCode += DWC_ECODE_TYPE_OPEN_FILE;
        break;
	case DWC_GHTTP_INVALID_POST:
        errorCode += DWC_ECODE_TYPE_INVALID_POST;
        break;
	case DWC_GHTTP_INSUFFICIENT_MEMORY:
        errorCode += DWC_ECODE_TYPE_BUFF_OVER;
        break;
	case DWC_GHTTP_INVALID_FILE_NAME:
	case DWC_GHTTP_INVALID_BUFFER_SIZE:
	case DWC_GHTTP_INVALID_URL:
        errorCode += DWC_ECODE_TYPE_REQ_INVALID;
        break;
	case DWC_GHTTP_UNSPECIFIED_ERROR:
        errorCode += DWC_ECODE_TYPE_UNSPECIFIED;
        break;
        
    // The result of an HTTP request.
	case DWC_GHTTP_OUT_OF_MEMORY:
    case DWC_GHTTP_MEMORY_ERROR:
        dwcError = DWC_ERROR_FATAL;
        errorCode += DWC_ECODE_TYPE_ALLOC;
        break;
	case DWC_GHTTP_BUFFER_OVERFLOW:
        errorCode += DWC_ECODE_TYPE_BUFF_OVER;
        break;
	case DWC_GHTTP_PARSE_URL_FAILED:
        errorCode += DWC_ECODE_TYPE_PARSE_URL;
        break;
	case DWC_GHTTP_HOST_LOOKUP_FAILED:
        errorCode += DWC_ECODE_TYPE_DNS;
        break;
    case DWC_GHTTP_SOCKET_FAILED:
        errorCode += DWC_ECODE_TYPE_SOCKET;
        break;
	case DWC_GHTTP_CONNECT_FAILED:
	case DWC_GHTTP_FILE_NOT_FOUND:
	case DWC_GHTTP_SERVER_ERROR:
        errorCode += DWC_ECODE_TYPE_SERVER;
        break;
	case DWC_GHTTP_BAD_RESPONSE:
        errorCode += DWC_ECODE_TYPE_BAD_RESPONSE;
        break;
	case DWC_GHTTP_REQUEST_REJECTED:
	case DWC_GHTTP_UNAUTHORIZED:
	case DWC_GHTTP_FORBIDDEN:
        errorCode += DWC_ECODE_TYPE_REJECTED;
        break;
	case DWC_GHTTP_FILE_WRITE_FAILED:
	case DWC_GHTTP_FILE_READ_FAILED:
        errorCode += DWC_ECODE_TYPE_FILE_RW;
        break;
	case DWC_GHTTP_FILE_INCOMPLETE:
        errorCode += DWC_ECODE_TYPE_INCOMPLETE;
        break;
	case DWC_GHTTP_FILE_TOO_BIG:
        errorCode += DWC_ECODE_TYPE_TO_BIG;
        break;
	case DWC_GHTTP_ENCRYPTION_ERROR:
        errorCode += DWC_ECODE_TYPE_ENCRYPTION;
        break;

    default:
        break;
    }

    DWCi_SetError(dwcError, errorCode);
    return result;
}


//=============================================================================
/*!
 *	@brief	Add GHTTP parameter list entry
 *
 *	@param	param	[in] Pointer to the DWCGHTTPParam to be added
 *
 *	@retval	Other than NULL	Pointer to the allocated DWCGHTTPParamEntry
 *	@retval	NULL   		Failed (be sure to perform error processing to handle memory allocation failure)
 */
//=============================================================================
static DWCGHTTPParamEntry *DWCi_AppendDWCGHTTPParam(const DWCGHTTPParam *param)
{
	DWCGHTTPParamEntry *entry;
	
	// Create entry to be added to the link list
	entry = DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCGHTTPParamEntry));
	if(entry == NULL)
		return NULL;
	
	// Copy param given as an argument
	entry->param = *param;
	
	// Initialize ParamEntry-specific members
	entry->nextentry = NULL;
	entry->buf = NULL;
	
	// If there is no head
	if(paramhead == NULL) {
		paramhead = entry;
		return entry;
	}
	
	// Attach to head if tehre is one
	entry->nextentry = paramhead;
	paramhead = entry;
	
	return entry;
}

//=============================================================================
/*!
 *	@brief	Delete GHTTP parameter list entry
 *
 *	@param	param	[in] Pointer to the DWCGHTTPParam to be deleted
 *
 *	@retval	None
 */
//=============================================================================
static void DWCi_RemoveDWCGHTTPParamEntry(DWCGHTTPParamEntry *entry)
{
	DWCGHTTPParamEntry *cursor, *target;
	
	// Does nothing if there is no head
	if(paramhead == NULL)
		return;
	
	// If the head is to be deleted, a special deletion method must be used
	if(paramhead == entry) {
		cursor = paramhead->nextentry;
		DWC_Free(DWC_ALLOCTYPE_BASE, (void*)paramhead, 0);
		paramhead = cursor;
		return;
	}
	
	// If the next entry after the cursor is to be deleted...
	cursor = paramhead;
	while(cursor->nextentry != NULL) {
		if(cursor->nextentry != entry) {
			cursor = cursor->nextentry;
			continue;
		}
		
		target = cursor->nextentry;
		cursor->nextentry = cursor->nextentry->nextentry;
		DWC_Free(DWC_ALLOCTYPE_BASE, (void*)target, 0);
		return;
	}
	
	return;
}

//=============================================================================
/*!
 *	@brief	Search for GHTTP parameter list entry based on value of req
 *
 *	@param	req	[in] reqid held by the DWCGHTTPParam being search for
 *
 *	@retval	None
 */
//=============================================================================
static DWCGHTTPParamEntry *DWCi_FindDWCGHTTPParamEntryByReq(int req)
{
	DWCGHTTPParamEntry *cursor;
	
	// If the next entry after the cursor is to be searched for...
	cursor = paramhead;
	while(cursor != NULL && cursor->req != req)
		cursor = cursor->nextentry;
	
	return cursor;
}

//=============================================================================
/*!
 *	@brief	Delete all GHTTP parameter list entries
 *
 *	@param	None
 *
 *	@retval	None
 */
//=============================================================================
static void DWCi_RemoveAllDWCGHTTPParamEntry(void)
{
	DWCGHTTPParamEntry *cursor, *target;
	
	cursor = paramhead;
	while(cursor != NULL) {
		target = cursor;
		cursor = cursor->nextentry;
		
		// Release if the wrapper allocated the buffer
		if(target->buf != NULL)
			DWC_Free(DWC_ALLOCTYPE_BASE, (void*)target->buf, 0);
		// Release the entry itself
		DWC_Free(DWC_ALLOCTYPE_BASE, (void*)target, 0);
	}
	
	paramhead = NULL;
	return;
}
