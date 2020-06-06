**********************************************************************************
*                                                                                *
*  Readme-NitroDWC-1_2-patch-plus4-060825.txt                                    *
*                                                                                *
*  Plus4 Patch for NITRO-DWC 1.2                                                 *
*                                                                                *
*  August 25, 2006                                                               *
*                                                                                *
**********************************************************************************

1. Introduction
==============================================================================
This patch is to correct problems found in NITRO-DWC 1.2 after its release. To install this patch, copy (overwrite) these files into the directory where NITRO-DWC 1.2 (06/22/2006 version) is installed.
    

2. Revised Content
==============================================================================

Revisions Implemented by NitroSDK-1_2-patch-plus4-060825
    --------------------------------------------------------------------------
    
- Corrected a problem related to freeing memory with DWC_NdCleanupAsync
A problem was corrected where the DWC_NdCleanupAsync function could not be called (an assert would occur if called) if an error occurred during asynchronous processing of the DWC_NdInitAsync function, regardless of whether the library had already allocated memory. 
Furthermore, a note has been added to the function reference in question regarding releasing the library. 
- Corrected a problem related to freeing memory for HTTP communications and general-purpose ranking
This problem is caused by the HTTP communication (GHTTP) library. The problem was that a completion callback was not being invoked and memory was not being released when the DWC_CancelGHTTPRequest function was called. This was due to the fact that memory allocated when a request is issued (by DWC_GetGHTTPData, DWC_GetGHTTPDataEx, or DWC_PostGHTTPData) is released when a completion callback executes. 
This occurs even when the DWC_ShutdownGHTTP function is called without waiting for the completion callback. 
The above problem has been corrected so that operations are carried out correctly when canceling or releasing the library. 
Finally, there was a problem with the DWC_GetGHTTPD function where the receive buffer was not released under the following conditions. 
- When called with a combination of arguments where bufferlen is greather than zero and buffer_clear is TRUE. 
- When a communication error has occurred (excluding cases where the function is called with the combination of arguments bufferlen = 0 and buffer_clear = TRUE)              
Note: The general-purpose ranking library had the same problem because it used the GHTTP library internally. 
- Corrected a problem related to the default settings of the Nintendo authentication server to be used
A problem was corrected where the authentication server for products was being used by default before DWC_InitInet or DWC_InitInetEx function was called. 
- Revised sample demos
Parts of code that used OS_Free to release the receive buffer for ghttp_sample have been revised to use DWC_Free().  
Processing performed when an error occurs with dwc_nd_sample has been revised. In addition, a description regarding temporary operations of the Download Server has been deleted from readme.txt. 
- Revised function reference
A note related to the gamecode argument of the DWC_CreateUserData function has been added.
A mistake regarding the DWCMatchedSCCallback function has been corrected.
- Standardized the character code used in all comments to SJIS
Parts of the source code that were using character codes other than SJIS have been changed to use SJIS. 
    
Issues addressed in NitroDWC-1_2-patch-plus3-060802
    --------------------------------------------------------------------------
    
- Corrected a problem in the DWC_LoginToStorageServerAsync function
A problem was corrected where a callback was not generated when an error occurred with the DWC_LoginToStorageServerAsync function. 
- Corrected a problem with the general-purpose ranking management tool (DWCRankingAdmin.exe)
A problem was corrected where user-defined data could not be obtained normally.
Version 1.00 --> Version 1.01
- Made revisions to the Programming Manual
The Nintendo Wi-Fi Connection NITRO-DWC Programming Manual located in $NITRODWC_ROOT/docs has been revised. 
For details, see the revision history of the document.
- Revised function reference
    
Issues addressed in NitroDWC-1_2-plus2-060727 (non-public)
    --------------------------------------------------------------------------
    
- Created the new error code 540XX
We added the error code 540XX (the final two digits indicate the type of access point connected to) when disconnected from an access point.
- Corrected a problem with the DWC_UpdateConnection function
A problem was corrected that was causing problems in the error code being set when using the DWC_UpdateConnection function to detect disconnection from an access point.
- Revised sample demos
We made changes to dwc_sample so that a text animation is displayed while running.
We made changes to dwc_connect_inet so that the DWC_UpdateConnection function is used to confirm connections to access points. 
- Made revisions to the Programming Manual
We revised the Nintendo Wi-Fi Connection NITRO-DWC Programming Manual - General-Purpose Ranking Edition - located in $NITRODWC_ROOT/docs.
For details, see the revision history of the document.
- Revised function reference
    
Issues addressed in NitroDWC-1_2-patch-plus-060721-1629 (non-public)
    --------------------------------------------------------------------------
    
- Improved the security of the Download feature
The security level had been lowered in order to connect to the temporary download server. Now that the actual working server is up, security has been improved. 
- Revised sample demos
We revised the product IDs used in each of the sample demos.
We deleted the file ARM9-TS-inet.lsf that was not being used with rank_sample.
- Made revisions to the Programming Manual
We revised the Nintendo Wi-Fi Connection NITRO-DWC Programming Manual - Download Edition - located in $NITRODWC_ROOT/docs.
For details, see the revision history of the document.
- Revised function reference
    
The following revisions are identical in content to those applied under NITRO-DWC-DL 1.2 RC. 
For details, see the revision history of the function reference manual.
    
- Added Nintendo Authentication Server verification process functions
- Added functions to check the connection to an access point
- Changed the game name, secret key, and product ID in each sample demo
- Corrected a problem related to initialization of Wi-Fi connection settings
- Revised the function reference
    

3. File List
==============================================================================
Source Files
/NitroDWC/build/demos/dwc_anybody_match_sample/src/main.c
/NitroDWC/build/demos/dwc_connect_inet_sample/src/main.c
/NitroDWC/build/demos/dwc_eval_callback_sample/src/main.c
/NitroDWC/build/demos/dwc_frienddata_sample/src/main.c
/NitroDWC/build/demos/dwc_friend_match_sample/src/main.c
/NitroDWC/build/demos/dwc_naslogin_sample/src/init.c
/NitroDWC/build/demos/dwc_naslogin_sample/src/main.c
/NitroDWC/build/demos/dwc_nd_sample/src/main.c
/NitroDWC/build/demos/dwc_sample/src/main.c
/NitroDWC/build/demos/dwc_sc_match_sample/src/main.c
/NitroDWC/build/demos/dwc_userdata_sample/src/main.c
/NitroDWC/build/demos/ghttp_sample/src/main.c
/NitroDWC/build/libraries/base/src/dwc_connectinet.c
/NitroDWC/build/libraries/base/src/dwc_friend.c
/NitroDWC/build/libraries/base/src/dwc_ghttp.c
/NitroDWC/build/libraries/base/src/dwc_nasfunc.c
/NitroDWC/build/libraries/base/src/dwc_nd.c
Header Files
/NitroDWC/build/demos/dwc_naslogin_sample/include/init.h
/NitroDWC/build/demos/dwc_naslogin_sample/include/main.h
/NitroDWC/build/demos/friend_match/include/main.h
/NitroDWC/include/base/dwc_connectinet.h
/NitroDWC/include/base/dwc_core.h
/NitroDWC/include/dwc_version.h
/NitroDWC/include/nd/DWCi_Nd.h
/NitroDWC/include/nhttp/NHTTP.h
Library Files
/NitroDWC/lib/ARM9-TS/Debug/libdwcaccount.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcaccount.thumb.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcauth.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcauth.thumb.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcbase.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcbase.thumb.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcnd.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcnd.thumb.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcnhttp.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcnhttp.thumb.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcranking.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcranking.thumb.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcutil.a
/NitroDWC/lib/ARM9-TS/Debug/libdwcutil.thumb.a
/NitroDWC/lib/ARM9-TS/Release/libdwcaccount.a
/NitroDWC/lib/ARM9-TS/Release/libdwcaccount.thumb.a
/NitroDWC/lib/ARM9-TS/Release/libdwcauth.a
/NitroDWC/lib/ARM9-TS/Release/libdwcauth.thumb.a
/NitroDWC/lib/ARM9-TS/Release/libdwcbase.a
/NitroDWC/lib/ARM9-TS/Release/libdwcbase.thumb.a
/NitroDWC/lib/ARM9-TS/Release/libdwcnd.a
/NitroDWC/lib/ARM9-TS/Release/libdwcnd.thumb.a
/NitroDWC/lib/ARM9-TS/Release/libdwcnhttp.a
/NitroDWC/lib/ARM9-TS/Release/libdwcnhttp.thumb.a
/NitroDWC/lib/ARM9-TS/Release/libdwcranking.a
/NitroDWC/lib/ARM9-TS/Release/libdwcranking.thumb.a
/NitroDWC/lib/ARM9-TS/Release/libdwcutil.a
/NitroDWC/lib/ARM9-TS/Release/libdwcutil.thumb.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcaccount.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcaccount.thumb.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcauth.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcauth.thumb.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcbase.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcbase.thumb.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcnd.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcnd.thumb.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcnhttp.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcnhttp.thumb.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcranking.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcranking.thumb.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcutil.a
/NitroDWC/lib/ARM9-TS/Rom/libdwcutil.thumb.a
Documentation and Other Items
/NitroDWC/build/buildtools/commondefs
/NitroDWC/build/demos/dwc_naslogin_sample/dwc_naslogin_sample.lsf
/NitroDWC/build/demos/dwc_naslogin_sample/dwc_naslogin_sample.rsf
/NitroDWC/build/demos/dwc_naslogin_sample/Makefile
/NitroDWC/build/demos/dwc_nd_sample/readme.txt
/NitroDWC/build/demos/dwc_sc_match_sample/Makefile
/NitroDWC/build/demos/friend_match/Makefile
/NitroDWC/build/demos/ghttp_sample/Makefile
/NitroDWC/build/demos/Makefile
/NitroDWC/build/libraries/base/Makefile
/NitroDWC/build/libraries/Makefile
/NitroDWC/build/Makefile
/NitroDWC/docs/Nintendo_WiFi_Connection_NITRO-DWC_ProgrammingManual.pdf
/NitroDWC/docs/Nintendo_WiFi_Connection_NITRO-DWC_ProgrammingManual_Download.pdf
/NitroDWC/docs/Nintendo_WiFi_Connection_NITRO-DWC_ProgrammingManual_Ranking.pdf
/NitroDWC/man/en_US/changelog.html
/NitroDWC/man/en_US/dwc/a-z.html
/NitroDWC/man/en_US/dwc/base/DWCMatchedSCCallback.html
/NitroDWC/man/en_US/dwc/base/DWCNasLoginState.html
/NitroDWC/man/en_US/dwc/base/DWCNdCallback.html
/NitroDWC/man/en_US/dwc/base/DWCNdCleanupCallback.html
/NitroDWC/man/en_US/dwc/base/DWCNdFileInfo.html
/NitroDWC/man/en_US/dwc/base/DWCRnkData.html
/NitroDWC/man/en_US/dwc/base/DWCRnkError.html
/NitroDWC/man/en_US/dwc/base/DWCRnkGetMode.html
/NitroDWC/man/en_US/dwc/base/DWCRnkGetParam.html
/NitroDWC/man/en_US/dwc/base/DWC_Base64Decode.html
/NitroDWC/man/en_US/dwc/base/DWC_Base64Encode.html
/NitroDWC/man/en_US/dwc/base/DWC_CheckUserData.html
/NitroDWC/man/en_US/dwc/base/DWC_CreateUserData.html
/NitroDWC/man/en_US/dwc/base/DWC_DebugConnectInetAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_Debug_DWCInitError.html
/NitroDWC/man/en_US/dwc/base/DWC_GetDateTime.html
/NitroDWC/man/en_US/dwc/base/DWC_GetGHTTPState.html
/NitroDWC/man/en_US/dwc/base/DWC_GetLastError.html
/NitroDWC/man/en_US/dwc/base/DWC_Init.html
/NitroDWC/man/en_US/dwc/base/DWC_IsEqualFriendData.html
/NitroDWC/man/en_US/dwc/base/DWC_LoadOthersDataAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_LoginAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_NASLoginAbort.html
/NitroDWC/man/en_US/dwc/base/DWC_NASLoginAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_NASLoginProcess.html
/NitroDWC/man/en_US/dwc/base/DWC_NdCancelAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_NdCleanupAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_NdGetFileAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_NdGetFileListAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_NdGetFileListNumAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_NdGetProgress.html
/NitroDWC/man/en_US/dwc/base/DWC_NdInitAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_NdSetAttr.html
/NitroDWC/man/en_US/dwc/base/DWC_ProcessFriendsMatch.html
/NitroDWC/man/en_US/dwc/base/DWC_ProcessGHTTP.html
/NitroDWC/man/en_US/dwc/base/DWC_RnkGetScoreAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_RnkInitialize.html
/NitroDWC/man/en_US/dwc/base/DWC_RnkPutScoreAsync.html
/NitroDWC/man/en_US/dwc/base/DWC_RnkResGetOrder.html
/NitroDWC/man/en_US/dwc/base/DWC_RnkResGetRow.html
/NitroDWC/man/en_US/dwc/base/DWC_RnkResGetRowCount.html
/NitroDWC/man/en_US/dwc/base/DWC_RnkResGetTotal.html
/NitroDWC/man/en_US/dwc/base/DWC_SetConnectApType.html
/NitroDWC/man/en_US/dwc/base/DWC_SetReportLevel.html
/NitroDWC/man/en_US/dwc/base/DWC_UpdateConnection.html
/NitroDWC/man/en_US/dwc/dwc.html
/NitroDWC/man/en_US/dwc/functions.html
/NitroDWC/man/en_US/dwc/list_dwc.html
/NitroDWC/man/en_US/main.html
/NitroDWC/tools/ranking/DWCRankingAdmin.exe
