/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_error
  File:     dwc_error.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_error.h,v $
  Revision 1.36  06/22/2006 02:03:17  nakata
  Removed the insufficient buffer error in the Download Library. (We don't want to increase the types of DWCErrors.)

  Revision 1.35  06/22/2006 00:49:17  nakata
  Added an insufficient buffer error in the Download Library.

  Revision 1.34  06/01/2006 04:48:49  takayama
  Fixed the problem where a warning was appearing when compiling

  Revision 1.33  05/29/2006 01:00:42  nakata
  Deleted an unnecessary error code related to DWC_Nd

  Revision 1.32  05/25/2006 09:06:53  nakata
  Added DWC_ERROR_SVL_HTTP

  Revision 1.31  05/25/2006 09:05:36  takayama
  Split up the GHTTP library errors with the FriendsMatch library errors,
  making it more independent

  Revision 1.30  05/25/2006 05:09:50  nakata
  Made changes in accordance with the addition to DWC_Nd/DWC_SVL of the feature for returning a DWC error

  Revision 1.29  05/17/2006 00:50:34  nakata
  Modified the DWC_Nd error constant names

  Revision 1.28  04/28/2006 12:25:13  nakata
  Made revisions in accordance with the changes to the constant name

  Revision 1.27  04/28/2006 08:16:51  nakata
  Deleted the Nd server error number (it was unnecessary)

  Revision 1.26  04/27/2006 07:33:04  nakata
  Changed the Nd Library-oriented UNKNOWN error code

  Revision 1.25  04/27/2006 05:15:25  nakata
  Added Nd error codes.

  Revision 1.24  04/26/2006 10:52:17  nakata
  Added error codes for the expanded features

  Revision 1.23  01/23/2006 04:50:43  arakit
  revised comments to match standardized terminology

  Revision 1.22  12/26/2005 23:59:55  arakit
  Corrected an error in a comment

  Revision 1.21  10/24/2005 12:14:49  arakit
  Added DWC_ERROR_SERVER_FULL as error when server is over its capacity in client-server matching.
  Added DWC_ERROR_SERVER_FULL

  Revision 1.20  10/20/2005 09:59:45  sasakit
  Added GT2-related errors.

  Revision 1.19  10/14/2005 07:43:23  onozawa
  Added ghttp error codes.

  Revision 1.18  10/14/2005 06:58:31  arakit
  Finally added ghttp error codes.

  Revision 1.17  10/14/2005 06:41:37  arakit
  Added ghttp error codes.

  Revision 1.16  10/11/2005 04:08:18  arakit
  Added the function DWC_GetLastErrorEx() to allow the additional retrieval of an error processing type.

  Revision 1.15  10/05/2005 08:29:43  arakit
  Added a matching option for returning a dedicated error to
  blocked clients when server-client matching has been used and matching is blocked after matching is complete.

  Revision 1.14  10/03/2005 05:06:49  arakit
  Changed DWC_ECODE_SEQ_TRANSPORT to DWC_ECODE_SEQ_ETC.

  Revision 1.13  09/30/2005 02:49:04  arakit
  Added error code for failed connections to server/client type.

  Revision 1.12  09/28/2005 12:40:54  arakit
  Added error for specified number of failures of NAT negotiation.

  Revision 1.11  09/24/2005 12:57:10  arakit
  Added error for exceeding max. number of connections.

  Revision 1.10  09/13/2005 07:35:19  sasakit
  Added DWC_ERROR_DISCONNECTED to error state.

  Revision 1.9  09/06/2005 09:12:38  sasakit
  Added processing of errors used during ConnectInet

  Revision 1.8  09/01/2005 12:15:16  arakit
  Corrected comments regarding error.

  Revision 1.7  08/29/2005 06:34:51  arakit
  Created socketErrorCallback function for gt2.

  Revision 1.6  08/26/2005 08:15:22  arakit
  -Organized ASSERT and check of module control variables.
  -Changed code so that arguments of the external function DWC_ShutdownFriendsMatch() are not taken.
  Also, made code so that control variables for each module cannot be cleared to NULL by the same function.
  -Changed the error processing function and inserted error setting and error checking where necessary.

  Revision 1.5  08/20/2005 07:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_ERROR_H_
#define DWC_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// Error enumerator
typedef enum {
    DWC_ERROR_NONE = 0,            // No error
    DWC_ERROR_DS_MEMORY_ANY,       // Errors related to save data for this console
    DWC_ERROR_AUTH_ANY,            // Errors related to authentication
    DWC_ERROR_AUTH_OUT_OF_SERVICE, // Exit service
    DWC_ERROR_AUTH_STOP_SERVICE,   // temporary service interruption
    DWC_ERROR_AC_ANY,              // Errors related to auto-connection
    DWC_ERROR_NETWORK,             // Other network errors
    DWC_ERROR_GHTTP_ANY,           // GHTTP (HTTP communication)-related errors
    DWC_ERROR_DISCONNECTED,        // Disconnected.
    DWC_ERROR_FATAL,               // fatal error

    // [arakit] main 051011
    // Mild error on sequence (DWCErrorType = DWC_ETYPE_LIGHT)
    DWC_ERROR_FRIENDS_SHORTAGE,    // An attempt was made to start specified friend peer matchmaking even though the specified number of active friends are not present
    DWC_ERROR_NOT_FRIEND_SERVER,   // During server-client matchmaking, friends specified to the server are not mutually friends or the server is not up
    DWC_ERROR_MO_SC_CONNECT_BLOCK, // connection from the server is denied as a matchmaking option for server-client matchmaking
    // [arakit] main 051011
    // [arakit] main 051024
    DWC_ERROR_SERVER_FULL,         // number of members exceed on server during server-client matchmaking
	
	// [nakata] Added error types for the Nd library
	DWC_ERROR_ND_ANY,
	DWC_ERROR_ND_HTTP,
	
	// [nakata] Added error types for the SVL library
	DWC_ERROR_SVL_ANY,
	DWC_ERROR_SVL_HTTP,
        
    DWC_ERROR_NUM
} DWCError;

// [arakit] main 051010
// Error type indicating processing required when an error is detected
typedef enum {
    DWC_ETYPE_NO_ERROR = 0,   // No error
    DWC_ETYPE_LIGHT,          // With a game-specific display only, no error displayed is required
    DWC_ETYPE_SHOW_ERROR,     // Only display error code
    DWC_ETYPE_SHUTDOWN_FM,    // It is necessary to call DWC_ShutdownFriendsMatch() to terminate FriendsMatch Library.
                              // An error code display is also necessary.
    DWC_ETYPE_SHUTDOWN_GHTTP, // It is necessary to call DWC_ShutdownGHTTP() to terminate the GHTTP Library.
                              // An error code display is also necessary.
    DWC_ETYPE_SHUTDOWN_ND,    // It is necessary to call DWC_NdCleanupAsync() to terminate DWC_Nd Library.
                              // An error code display is also necessary.
    DWC_ETYPE_DISCONNECT,     // If DWC_ShutdownFriendsMatch() is called during FriendsMatch processing, it's necessary to have already disconnected communications using DWC_CleanupInet().
                              //An error code display is also necessary.
    DWC_ETYPE_FATAL,          // Since this is equivalent to a Fatal Error it's necessary to prompt the user to turn power OFF.
                              // An error code display is also necessary.
    DWC_ETYPE_NUM
} DWCErrorType;
// [arakit] main 051010

// Separate dwc_base error codes for debugging
enum {
    // Which sequence did this error occur in?
    DWC_ECODE_SEQ_LOGIN          = (-60000),  // Error during login processing
    DWC_ECODE_SEQ_FRIEND         = (-70000),  // Error during friend management processing
    DWC_ECODE_SEQ_MATCH          = (-80000),  // Error during matchmaking
    DWC_ECODE_SEQ_ETC            = (-90000),  // Error during processing other than the above

    // When a GameSpy error occurs, which SDK generated the error?
    DWC_ECODE_GS_GP              =  (-1000),  // GameSpy GP error
    DWC_ECODE_GS_PERS            =  (-2000),  // GameSpy Persistent error
    DWC_ECODE_GS_STATS           =  (-3000),  // GameSpy Stats error
    DWC_ECODE_GS_QR2             =  (-4000),  // GameSpy QR2 error
    DWC_ECODE_GS_SB              =  (-5000),  // GameSpy Server Browsing error
    DWC_ECODE_GS_NN              =  (-6000),  // GameSpy Nat Negotiation error
    DWC_ECODE_GS_GT2             =  (-7000),  // GameSpy gt2 error
    DWC_ECODE_GS_HTTP            =  (-8000),  // GameSpy HTTP error
    DWC_ECODE_GS_ETC             =  (-9000),  // GameSpy miscellaneous SDK error

    // Normal error type
    DWC_ECODE_TYPE_NETWORK       =   ( -10),  // Network down
    DWC_ECODE_TYPE_SERVER        =   ( -20),  // GameSpy server down
    DWC_ECODE_TYPE_DNS           =   ( -30),  // DNS down
    DWC_ECODE_TYPE_DATA          =   ( -40),  // Received invalid data
    DWC_ECODE_TYPE_SOCKET        =   ( -50),  // Socket communication error
    DWC_ECODE_TYPE_BIND          =   ( -60),  // Socket bind error
    DWC_ECODE_TYPE_TIMEOUT       =   ( -70),  // Timeout occurred
    DWC_ECODE_TYPE_PEER          =   ( -80),  // Problem with 1-to-1 communications
    DWC_ECODE_TYPE_CONN_OVER     =   (-100),  // Number of max. connections exceeded
    DWC_ECODE_TYPE_STATS_AUTH    =   (-200),  // STATS server login error
    DWC_ECODE_TYPE_STATS_LOAD    =   (-210),  // STATS server data load error
    DWC_ECODE_TYPE_STATS_SAVE    =   (-220),  // STATS server data save error
    DWC_ECODE_TYPE_NOT_FRIEND    =   (-400),  // The other party specified is not mutually registered as a friend
    DWC_ECODE_TYPE_OTHER         =   (-410),  // Communications with other part disconnected (mainly occurs during server/client matchmaking)
    DWC_ECODE_TYPE_MUCH_FAILURE  =   (-420),  // NAT negotiations have failed for the designated number of times or more
    DWC_ECODE_TYPE_SC_CL_FAIL    =   (-430),  // client connection failure during server-client matchmaking
    DWC_ECODE_TYPE_CLOSE         =   (-600),  // Error when connection closed
    DWC_ECODE_TYPE_TRANS_HEADER  =   (-610),  // Reliable data received in an invalid receive state
    DWC_ECODE_TYPE_TRANS_BODY    =   (-620),  // Receive buffer overflow
    DWC_ECODE_TYPE_AC_FATAL      =   (-700),  // Error group used by AC during AC processing. In this case, the low order digits represent the dedicated state value.
    DWC_ECODE_TYPE_OPEN_FILE     =   (-800),  // GHTTP file open failed
    DWC_ECODE_TYPE_INVALID_POST  =   (-810),  // Invalid GHTTP post
    DWC_ECODE_TYPE_REQ_INVALID   =   (-820),  // Invalid GHTTP file name or other information
    DWC_ECODE_TYPE_UNSPECIFIED   =   (-830),  // Unspecified GHTTP error
    DWC_ECODE_TYPE_BUFF_OVER     =   (-840),  // GHTTP buffer overflow
    DWC_ECODE_TYPE_PARSE_URL     =   (-850),  // GHTTP URL parsing error
    DWC_ECODE_TYPE_BAD_RESPONSE  =   (-860),  // Bad response from GHTTP server error
    DWC_ECODE_TYPE_REJECTED      =   (-870),  // Deny requests and other commands from the GHTTP server
    DWC_ECODE_TYPE_FILE_RW       =   (-880),  // GHTTP local file read/write error
    DWC_ECODE_TYPE_INCOMPLETE    =   (-890),  // GHTTP download has halted
    DWC_ECODE_TYPE_TO_BIG        =   (-900),  // The GHTTP file size is too big and cannot be downloaded
    DWC_ECODE_TYPE_ENCRYPTION    =   (-910),  // GHTTP encryption error
	
    // Fatal Error special type
    DWC_ECODE_TYPE_ALLOC         =     (-1),  // Memory allocation failure
    DWC_ECODE_TYPE_PARAM         =     (-2),  // Parameter error
    DWC_ECODE_TYPE_SO_SOCKET     =     (-3),  // Gamespy gt2 socket error caused by SO error
    DWC_ECODE_TYPE_NOT_INIT      =     (-4),  // Library not initialized.
    DWC_ECODE_TYPE_DUP_INIT      =     (-5),  // Library initialized twice.
    DWC_ECODE_TYPE_WM_INIT       =     (-6),  // WM initialization failed.
    DWC_ECODE_TYPE_UNEXPECTED    =     (-9),  // Unexpected status or unknown GameSpy error occurred

	// 30000s error code definition (for expanded features)
	DWC_ECODE_SEQ_ADDINS        =   (-30000), // Expanded feature error
	
	DWC_ECODE_FUNC_ND            =   (-1000), // Region for DWC_Nd
	
	DWC_ECODE_TYPE_ND_ALLOC      =   ( -1), // Memory allocation failure
	DWC_ECODE_TYPE_ND_FATAL      =   ( -9), // Fatal error
	DWC_ECODE_TYPE_ND_BUSY       =   (-10), // A new request was made while downloading
	DWC_ECODE_TYPE_ND_HTTP       =   (-20), // HTTP communications fail
	DWC_ECODE_TYPE_ND_BUFFULL    =   (-30), // Insufficient HTTP communications buffer
	DWC_ECODE_TYPE_ND_SERVER     =   (  0) // The server has returned an error code
};


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Get error function
  Argument : Pointer to start address of memory storing errorCode. Pass NULL unless otherwise required.
  Return value: Error type
  Use : Gets the last communication error type and error code
 *---------------------------------------------------------------------------*/
extern DWCError DWC_GetLastError(int* errorCode);


// [arakit] main 051011
/*---------------------------------------------------------------------------*
  Function for getting the expanded error
  Argument : Pointer to start address of memory storing errorCode. Pass NULL unless otherwise required.
          errorType : Pointer to the address storing the error processing type. Pass NULL unless otherwise required.
  Return value: Error type
  Use : Gets the last communication error type, error code, and error processing type
 *---------------------------------------------------------------------------*/
extern DWCError DWC_GetLastErrorEx(int* errorCode, DWCErrorType* errorType);
// [arakit] main 051011


/*---------------------------------------------------------------------------*
  Clear error function
  Arguments : None
  Return value: None
  Use : Clears records of communications errors. Fatal errors cannot be cleared.
 *---------------------------------------------------------------------------*/
extern void DWC_ClearError(void);


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Error check function
  Arguments : None
  Return value: TRUE: Error, FALSE: No error
  Use: Checks whether or not a communications error has occurred
 *---------------------------------------------------------------------------*/
extern BOOL DWCi_IsError(void);


/*---------------------------------------------------------------------------*
  Error set function
  Arguments: error: Error type
          errorCode: Error code representing detailed error information
  Return value: None
  Use : Records occurrence of communications errors. Fatal errors cannot be overwritten.
 *---------------------------------------------------------------------------*/
extern void DWCi_SetError(DWCError error, int errorCode);


#ifdef __cplusplus
}
#endif


#endif // DWC_ERROR_H_
