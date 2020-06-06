#include <nitro.h>

#include <base/dwc_error.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
static DWCError stDwcLastError = DWC_ERROR_NONE;  // Last error
static int stDwcErrorCode = 0;  // Last error code


//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Get error function
  Argument : Pointer to start address of memory storing errorCode. Pass NULL unless otherwise required.
  Return value: Error type
  Use : Gets the last communication error type and error code
 *---------------------------------------------------------------------------*/
DWCError DWC_GetLastError(int* errorCode)
{

    if (errorCode != NULL) *errorCode = stDwcErrorCode;
    
    return stDwcLastError;
}


// [arakit] main 051011
/*---------------------------------------------------------------------------*
  Function for getting the expanded error
  Argument : Pointer to start address of memory storing errorCode. Pass NULL unless otherwise required.
          errorType : Pointer to the address storing the error processing type. Pass NULL unless otherwise required.
  Return value: Error type
  Use : Gets the last communication error type, error code, and error processing type
 *---------------------------------------------------------------------------*/
DWCError DWC_GetLastErrorEx(int* errorCode, DWCErrorType* errorType)
{

    if (errorCode != NULL) *errorCode = stDwcErrorCode;

    if (errorType != NULL){
        switch (stDwcLastError){
        case DWC_ERROR_AUTH_ANY:
        case DWC_ERROR_AUTH_OUT_OF_SERVICE:
        case DWC_ERROR_AUTH_STOP_SERVICE:
        case DWC_ERROR_AC_ANY:
        case DWC_ERROR_DISCONNECTED:
            *errorType = DWC_ETYPE_DISCONNECT;
            break;
        
        case DWC_ERROR_NETWORK:
            *errorType = DWC_ETYPE_SHUTDOWN_FM;
            break;
        
        case DWC_ERROR_GHTTP_ANY:
            *errorType = DWC_ETYPE_SHUTDOWN_GHTTP;
            break;
        
        case DWC_ERROR_FRIENDS_SHORTAGE:
        case DWC_ERROR_NOT_FRIEND_SERVER:
        case DWC_ERROR_MO_SC_CONNECT_BLOCK:
        case DWC_ERROR_SERVER_FULL:
            *errorType = DWC_ETYPE_LIGHT;
            break;
        
        case DWC_ERROR_DS_MEMORY_ANY:
        case DWC_ERROR_FATAL:
            *errorType = DWC_ETYPE_FATAL;
            break;
        
		case DWC_ERROR_ND_ANY:
			*errorType = DWC_ETYPE_SHUTDOWN_ND;
			break;
		
		case DWC_ERROR_ND_HTTP:
		case DWC_ERROR_SVL_HTTP:
			*errorType = DWC_ETYPE_DISCONNECT;
			break;
		
		case DWC_ERROR_SVL_ANY:
			*errorType = DWC_ETYPE_SHOW_ERROR;
			break;
        
        case DWC_ERROR_NONE:
        default:
            *errorType = DWC_ETYPE_NO_ERROR;
            break;
        }
    }

    return stDwcLastError;
}
// [arakit] main 051011


/*---------------------------------------------------------------------------*
  Clear error function
  Arguments : None
  Return value: None
  Use : Clears records of communications errors. Fatal errors cannot be cleared.
 *---------------------------------------------------------------------------*/
void DWC_ClearError(void)
{

    if (stDwcLastError != DWC_ERROR_FATAL){
        // FATAL_ERROR cannot be cleared.
        stDwcLastError = DWC_ERROR_NONE;
        stDwcErrorCode = 0;
    }
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Error check function
  Arguments : None
  Return value: TRUE: Error, FALSE: No error
  Use: Checks whether or not a communications error has occurred
 *---------------------------------------------------------------------------*/
BOOL DWCi_IsError(void)
{

    if (stDwcLastError != DWC_ERROR_NONE) return TRUE;
    else return FALSE;
}


/*---------------------------------------------------------------------------*
  Error set function
  Arguments: error: Error type
          errorCode: Error code representing detailed error information
  Return value: None
  Use : Records occurrence of communications errors. Fatal errors cannot be overwritten.
 *---------------------------------------------------------------------------*/
void DWCi_SetError(DWCError error, int errorCode)
{

    if (stDwcLastError != DWC_ERROR_FATAL){
        // FATAL_ERROR cannot be overwritten
        stDwcLastError = error;
        stDwcErrorCode = errorCode;
    }

#ifndef SDK_FINALROM
    if ( error == DWC_ERROR_FATAL )
    {
        OS_TPrintf("FATALERROR_SET\n");
    }
#endif
}
