/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  -
  File:     dwc_friend.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_friend.h,v $
  Revision 1.32  02/27/2006 03:19:17  nakata
  Revised argument types for Base64-related wrapper functions.

  Revision 1.31  01/25/2006 07:50:50  arakit
  Added the data storage server/load functions, DWC_LoadOwnPublicDataAsync()
  and DWC_LoadOwnPrivateDataAsync().

  Revision 1.30  01/23/2006 04:50:43  arakit
  revised comments to match standardized terminology

  Revision 1.29  01/17/2006 07:17:25  arakit
  - Created DWC_SetFriendStatusCallback() function that only performs settings for
    DWCFriendStatusCallback.
  - Changed to return values for DWC_SetBuddyFriendCallback() and DWC_SetStorageServerCallback()
    from void to BOOL and changed to return FALSE without doing anything before allocating control variables.
  - Created the Base64 encoding/decoding functions (Auth wrapping functions):
    DWC_Base64Encode() and DWC_Base64Decode().

  Revision 1.28  12/21/2005 03:08:32  arakit
  Added support for the change in the format of the member playerName in the control structure.

  Revision 1.27  11/07/2005 03:54:59  sasakit
  Changed GsStatus to OwnStatus.

  Revision 1.26  11/04/2005 11:36:44  arakit
  Changed 'profileID' argument of DWCLoadFromServerCallback to friend list index ('index').

  Revision 1.25  11/04/2005 05:44:59  arakit
  Corrected spelling errors.

  Revision 1.24  11/04/2005 05:07:18  arakit
  Added functions to log in and out of the Persistent server, enabled access of Persistent server
  

  Revision 1.23  11/02/2005 02:49:33  arakit
  Adjusted the file's include relationships.

  Revision 1.22  10/28/2005 09:11:05  arakit
  Corrected inaccurate comment: : "When power is cut unexpectedly during communication,
  the state nearly 10 seconds prior to this action will remain."

  Revision 1.21  10/06/2005 07:00:54  arakit
  Made slight corrections to descriptions of functions

  Revision 1.20  10/03/2005 04:05:04  arakit
  Added a call to a function that replaces the keep-alive so that TCP connections of gp are not cut off.

  Revision 1.19  10/01/2005 07:14:07  arakit
  Added BOOL DWC_CanChangeFriendList(void) -- A function for getting the editable status of the friend roster.

  Revision 1.18  09/28/2005 05:17:28  arakit
  Created DWC_DeleteBuddyFriendData() function for removing friend data and canceling friendships.

  Revision 1.17  2005/09/27 14:01:42  arakit
  Deleted constants that were not being used.

  Revision 1.16  09/20/2005 11:03:40  sasakit
  Moved the return processing for gpSendBuddyRequest() to dwc_friend.c.

  Revision 1.15  09/16/2005 08:26:40  arakit
  Changed the argument names for callbacks.

  Revision 1.14  09/13/2005 12:12:22  arakit
  ・Corrected a problem where the other party could not be found when performing a ProfileSearch using MP friend data.
    Fixed bugs.
  ・Made it possible to set callback functions which make their return once friendship is established even at times other than when DWC_UpdateServersAsync() is processing.

  Revision 1.13  09/12/2005 08:50:25  arakit
  - Made code so that ProfileSearch is performed one person at a time to handle Alloc for gp.
  - Revised code so that establishment of friendships is completed while online.
  - Made changes so that the friend roster index can be passed as an argument to the remove from friend roster callback in the case of friend data determined to be the same as deleted data.
    Changed so that the friend list index is also passed.

  Revision 1.12  09/09/2005 09:54:09  arakit
  Made changes so that matching is not performed between different version libraries.

  Revision 1.11  09/05/2005 11:43:31  arakit
  -Changed the Profile ID argument to the DWC_LoadOthersDataAsync() function used for Persistent server load to the friend roster index.
    Changed to friend list index.
  -Of the gpStatus strings, the design used to be such that libraries would use locationString while the game would use statusString.
    Since statusString contains the string "Offline" by default, specifications have been switched so that the
    library uses statusString and the game uses locationString.
     
  -Added the DWC_SetGsStatusData() function for setting data, 
    the DWC_GetGsStatusData() function for encoding and reading that data,
    the DWC_GetFriendStatusData() function for reading binary Status
    data for the friend, and DWC_GetFriendStatusDataSC().

  Revision 1.10  09/03/2005 13:03:35  arakit
  - Deleted the external functions used to manipulate the friend roster: DWC_SetFriendDataToList(), DWC_SetGsProfileIDToList(), and DWC_DeleteFriendFromList().
    Erased DWC_DeleteFriendFromList().
  - Made internal functions out of DWC_GetProfileIDFromList() and DWC_GetFriendListIndex(), used to directly handle the GameSpy Profile ID,
    Converted DWC_GetFriendListIndex() to an internal function.
  - Changed the argument passed to DWC_GetFriendStatus() and DWC_GetFriendStatusSC() from "a 
    pointer to the friend roster + the length of the roster" to just "a pointer to friend data."
  -Changed specifications so that when the friend roster is being updated synchronously, empty lines above are not filled in
    even if friend data has been removed. To reflect this change, we revised all processes that expected fill-in to occur.
  -Corrected a problem that occurs if a friend has not logged on even once when getting
    the GameSpy Profile ID of a friend from MP exchange friend data.
  -Code for authentication processing when receiving a friend request has been made to match final specifications.

  Revision 1.9  08/29/2005 06:35:56  arakit
  -Changes were made so that the statusString of gpStatus can be read from and written to the game side
  and locationString from and to the library side.
  -Code has been made so that the maximum number of connected players and the current number of connected players can be set to the locationString of gpStatus using server-client matching.
  A function has been prepared for accessing this value.

  Revision 1.8  08/26/2005 09:15:22  arakit
  -Organized ASSERT and check of module control variables.
  -Changed code so that arguments of the external function DWC_ShutdownFriendsMatch() are not taken.
  Also, made code so that control variables for each module cannot be cleared to NULL by the same function.
  -Changed the error processing function and inserted error setting and error checking where necessary.

  Revision 1.7  08/23/2005 13:39:20  arakit
  -The external functions DWC_HasProflieIDInList() and DWC_HasOnlyMPFriendKeyInList() have been removed.
  - The external function DWC_GetProfileIDFromList() and some internal functions have been changed to
  handle the user data as arguments.

  Revision 1.6  08/20/2005 08:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_FRIEND_H_
#define DWC_FRIEND_H_


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// Maximum string length for GameSpy Status (including NULL termination). 256 characters.
#define DWC_GS_STATUS_STRING_LEN GP_LOCATION_STRING_LEN

// This flag indicates save/load of Persistent data under DWC control
#define DWC_PERS_KEY_ID_MASK         0x80000000
#define DWC_PERS_KEY_ID_PLAYER_NAME  (DWC_PERS_KEY_ID_MASK | 0x01)

// Calling interval for gpProcess() in msec
#define DWC_GP_PROCESS_INTERVAL      300

// Number of calls to make to gpProcess() until friend roster synchronization starts
#define DWC_FRIEND_UPDATE_WAIT_COUNT 7    // 300×7 = 2.1sec

// Number of times to call gpProcess() to send the GP keep-alive
#define DWC_GP_KEEP_ALIVE_COUNT      256  // 300×256 = 76.8sec

// Key string to be set for the statusString of gpStatus (must be three characters)
#define DWC_GP_SSTR_KEY_MATCH_VERSION    "VER"  // Matchmaking protocol version
#define DWC_GP_SSTR_KEY_MATCH_FRIEND_NUM "FME"  // Desired number of connections when performing friend specified peer matchmaking
#define DWC_GP_SSTR_KEY_DISTANT_FRIEND   "MDF"  // Should friends of friends be allowed during friend specified peer matchmaking?
#define DWC_GP_SSTR_KEY_MATCH_SC_MAX     "SCM"  // the maximum number of connections for server-client matchmaking
#define DWC_GP_SSTR_KEY_MATCH_SC_NUM     "SCN"  // the current number of connections for server-client matchmaking

// The status value represents that status of gpStatus is not changed
#define DWC_GP_STATUS_NO_CHANGE -1

    
//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// Friend's communication status (Re-defined and used as GPEnum Status + α)
enum {
    DWC_STATUS_OFFLINE = 0,    // Offline
    DWC_STATUS_ONLINE,         // Online (already logged onto GP server)
    DWC_STATUS_PLAYING,        // During game play
    DWC_STATUS_MATCH_ANYBODY,  // friend unspecified peer matchmaking in progress
    DWC_STATUS_MATCH_FRIEND,   // friend specified peer matchmaking in progress
    DWC_STATUS_MATCH_SC_CL,    // client during server-client matchmaking
    DWC_STATUS_MATCH_SC_SV,    // server during server-client matchmaking
    DWC_STATUS_NUM
};
    
// Friend management processing status
typedef enum {
    DWC_FRIEND_STATE_INIT = 0,      // Initial state
    DWC_FRIEND_STATE_PERS_LOGIN,    // during persistent server login and friend list synchronization
    DWC_FRIEND_STATE_LOGON,         // After Persistent server login
    DWC_FRIEND_STATE_NUM
} DWCFriendState;

// Synchronous processing status of the local friend roster and GP friend roster
enum {
    DWC_BUDDY_UPDATE_STATE_WAIT = 0,  // Wait for friend roster to complete download
    DWC_BUDDY_UPDATE_STATE_CHECK,     // Checking the friend roster
    DWC_BUDDY_UPDATE_STATE_PSEARCH,   // During profile search
    DWC_BUDDY_UPDATE_STATE_COMPLETE,  // Synchronous processing of friend roster complete
    DWC_BUDDY_UPDATE_STATE_NUM
};

// Persistent server login state
enum {
    DWC_PERS_STATE_INIT = 0,   // Initial state
    DWC_PERS_STATE_LOGIN,      // During login
    DWC_PERS_STATE_CONNECTED,  // Connection state
    DWC_PERS_STATE_NUM
};


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  friend list synchronization callback type
  Arguments: error: DWC error type
          isChanged : TRUE : The friend roster has been re-written during synchronization,
                    FALSE : The friend roster has not been re-written
          param : Parameters for the callback
  Return value: None
 *---------------------------------------------------------------------------*/
typedef void (*DWCUpdateServersCallback)(DWCError error, BOOL isChanged, void* param);

/*---------------------------------------------------------------------------*
  Callback type for notification of changes in friend status
  Argument: index : Friend roster index for the friend whose status has changed
          status : Friend status. Enumerator for DWC_STATUS_*.
          statusString : GameSpy status string defined by the game.
                       Maximum string length is 255 character + NULL termination.
          param : Parameters for the callback
  Return value: None
 *---------------------------------------------------------------------------*/
typedef void (*DWCFriendStatusCallback)(int index, u8 status, const char* statusString, void* param);

/*---------------------------------------------------------------------------*
  Friend roster delete callback type
  Arguments : deletedIndex : Friend roster index of the friend that was deleted
          srcIndex : Friend roster index determined to have the same friend data
          param : Parameters for the callback
  Return value: None
  If the same friend data is found on the friend roster during processing by DWC_UpdateServersAsync(),
  call it after removing data on the library side.
 *---------------------------------------------------------------------------*/
typedef void (*DWCDeleteFriendListCallback)(int deletedIndex, int srcIndex, void* param);

/*---------------------------------------------------------------------------*
  Callback for establishing friends
  Argument: index : Friend roster index for a friend with whom a friendship has been established on the GP server
          param : Parameters for the callback
  Return value: None
 *---------------------------------------------------------------------------*/
typedef void (*DWCBuddyFriendCallback)(int index, void* param);

/*---------------------------------------------------------------------------*
  data storage server/login callback type
  Arguments: error: DWC error type
          param : Parameters for the callback
  Return value: None
 *---------------------------------------------------------------------------*/
typedef void (*DWCStorageLoginCallback)(DWCError error, void* param);

/*---------------------------------------------------------------------------*
  Data storage server save complete callback type
  Arguments : success : TRUE : Save succeeded, FALSE : Save failed
          isPublic : TRUE : Saved data is public data, FALSE : Saved data is private data
          param : Parameters for the callback
  Return value: None
 *---------------------------------------------------------------------------*/
typedef void (*DWCSaveToServerCallback)(BOOL success, BOOL isPublic, void* param);

/*---------------------------------------------------------------------------*
  Data storage server load complete callback type
  Arguments : success : TRUE : Load succeeded, FALSE : Load failed
          index     Friend list index of holder of load source data.
                    If self, or if friend was no longer friend by the time the load was finished, -1.
          data : Data that was loaded (string representing the key/value pair)
          len : Length of load data
          param : Parameters for the callback
  Return value: None
  If key specified by the load function does not exist, or if another person tries to read key
  saved with DWC_SavePrivateDataAsync(), success = FALSE.
 *---------------------------------------------------------------------------*/
typedef void (*DWCLoadFromServerCallback)(BOOL success, int index, char* data, int len, void* param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
// Union for friend roster
#define DWC_FRIEND_DATA_SIZE 16  // Friend roster size (units: bytes)(currently not used)
typedef union DWCunFriendDataOld
{
    u8  mpFriendKey[DWC_FRIEND_DATA_SIZE];  // MP friend exchange key
    struct {
        int id;                             // Profile ID (non-encoded state)
        u8  pad[DWC_FRIEND_DATA_SIZE-sizeof(int)];  // Padding
    } profile;
    u8  flags[DWC_FRIEND_DATA_SIZE];  // For accessing flags (the upper 2 bits of [15] are flags)
} DWCFriendDataOld;

typedef struct DWCstFriendControl
{
    DWCFriendState state;    // Friend management processing status
        
    GPConnection* pGpObj;    // Pointer to the gp connection structure pointer
    u32 gpProcessCount;      // Function for calling gpProcess()
    OSTick lastGpProcess;    // The time at which the last gpProcess was called
        
    int friendListLen;       // Maximum length of friend roster
    DWCFriendData* friendList;  // Friend roster

    u8  buddyUpdateIdx;      // Index during synchronous processing of the friend roster
    u8  friendListChanged;   // 1: There has been a change to the friend roster, 0: No change
    u8  buddyUpdateState;    // Synchronous processing status of friend roster. Defined using DWC_BUDDY_UPDATE_STATE_* enumerator.
    vu8 svUpdateComplete;    // friend list synchronization complete flag synchronization complete with 2
    u32 persCallbackLevel;   // Number of callback waits for Persistent data save/load
    int profileID;           // This players Profile ID
    const u16* playerName;   // Pointer to this user's player name as also accessed by other users

    DWCUpdateServersCallback updateCallback;     // friend list synchronization complete callback
    void* updateParam;       // Parameter for the above callback
    DWCFriendStatusCallback statusCallback;      // Callback for notification of changes to friend status
    void* statusParam;       // Parameter for the above callback
    DWCDeleteFriendListCallback deleteCallback;  // Friend roster delete callback
    void* deleteParam;       // Parameter for the above callback
    DWCBuddyFriendCallback buddyCallback;        // Callback for establishing friendship
    void* buddyParam;        // Parameter for the above callback
    DWCStorageLoginCallback persLoginCallback;   // Persistent server login completion notification callback
    void* persLoginParam;    // Parameter for the above callback
    DWCSaveToServerCallback saveCallback;        // Persistent server save callback
    DWCLoadFromServerCallback loadCallback;      // Persistent server load callback
} DWCFriendControl;


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for getting friend communication status
  arguments: friendData: pointer to friend data
          statusString : Pointer to the start address of memory used to store the GameSpy status string defined by the game.
                       Strings cannot be copied if NULL has been passed.
                       Maximum string length is 255 character + NULL termination.
  Return value: Communication status enum value （dwc_friend.h のDWC_STATUS_*）.
          If called when this user is offline, only DWC_STATUS_OFFLINE will be returned.
          Even if the other party is not yet registered in the GP friend roster,
          DWC_STATUS_OFFLINE is returned.
  Use : Gets the communication status of the specified friend from the local cache.
          Communications are not performed.
          Notified by GP server each time the friend communication state changes.
          However, if power is cut unexpectedly during communication, the previous state should persist for several minutes.
 *---------------------------------------------------------------------------*/
extern u8   DWC_GetFriendStatus(const DWCFriendData* friendData, char* statusString);


/*---------------------------------------------------------------------------*
  friend communication state getting function (for server-client matchmaking games)
  arguments: friendData: pointer to friend data
          maxEntry: the maximum number of connections when the friend is on the
                       server-client matchmaking server.
                       Pointer to start address of storage. Gets nothing if NULL is passed.
          numEntry: the current number of connections when the friend is on the
                       server-client matchmaking server.
                       Pointer to start address of storage. Gets nothing if NULL is passed.
          statusString : Pointer to the start address of memory used to store the GameSpy status string defined by the game.
                       Strings cannot be copied if NULL has been passed.
                       Maximum string length is 255 character + NULL termination.
  Return value: Communication status enum value （dwc_friend.h のDWC_STATUS_*）.
          If called when this user is offline, only DWC_STATUS_OFFLINE will be returned.
          Even if the other party is not yet registered in the GP friend roster,
          DWC_STATUS_OFFLINE is returned.
  Use : Gets the communication status of the specified friend from the local cache.
          Communications are not performed.
          Notified by GP server each time the friend communication state changes.
          However, if power is cut unexpectedly during communication, the previous state should persist for several minutes.
 *---------------------------------------------------------------------------*/
extern u8   DWC_GetFriendStatusSC(const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusString);


/*---------------------------------------------------------------------------*
  Function for getting the friend communication status (statusData version)
  arguments: friendData: pointer to friend data
          statusData : Pointer to the start address used to store GameSpy status data defined by the game.
                     Data cannot be copied if NULL has been passed.
          statusData : Pointer to the start address used to store GameSpy status data length defined by the game.
                     -1 is stored here in the case of invalid data.
  Return value: Communication status enum value （dwc_friend.h のDWC_STATUS_*）.
          If called when this user is offline, only DWC_STATUS_OFFLINE will be returned.
          Even if the other party is not yet registered in the GP friend roster,
          DWC_STATUS_OFFLINE is returned.
  Use : Gets the communication status of the specified friend from the local cache.
          Communications are not performed.
          Notified by GP server each time the friend communication state changes.
          However, if power is cut unexpectedly during communication, the previous state should persist for several minutes.
          If the return value is DWC_STATUS_OFFLINE, the function returns without setting any values,
          other than setting *size = -1.
 *---------------------------------------------------------------------------*/
extern u8 DWC_GetFriendStatusData(const DWCFriendData* friendData, char* statusData, int* size);


/*---------------------------------------------------------------------------*
  function for getting the friend communication status (server-client matchmaking game statusData version)
  arguments: friendData: pointer to friend data
          maxEntry: the maximum number of connections when the friend is on the
                     server-client matchmaking server.
                     Pointer to start address of storage. Gets nothing if NULL is passed.
          numEntry: the current number of connections when the friend is on the
                     server-client matchmaking server.
                     Pointer to start address of storage. Gets nothing if NULL is passed.
          statusData : Pointer to the start address of memory used to store the GameSpy status string defined by the game.
                     Strings cannot be copied if NULL has been passed.
          statusData : Pointer to the start address used to store GameSpy status data length defined by the game.
                     -1 is stored here in the case of invalid data.
  Return value: Communication status enum value （dwc_friend.h のDWC_STATUS_*）.
          If called when this user is offline, only DWC_STATUS_OFFLINE will be returned.
          Even if the other party is not yet registered in the GP friend roster,
          DWC_STATUS_OFFLINE is returned.
  Use : Gets the communication status of the specified friend from the local cache.
          Communications are not performed.
          Notified by GP server each time the friend communication state changes.
          However, if power is cut unexpectedly during communication, the previous state should persist for several minutes.
          If the return value is DWC_STATUS_OFFLINE, the function returns without setting any values, other than setting *size = -1.
          other than setting *size = -1.
 *---------------------------------------------------------------------------*/
extern u8 DWC_GetFriendStatusDataSC(const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusData, int* size);


/*---------------------------------------------------------------------------*
  Function for getting number of friends
  Arguments: friendList : Friend roster
          friendListLen : Maximum length of the friend list (number of entries)
  Return value: The number of friends registered on the friend roster
  Use : Accesses the local friend roster and returns the number of registered friends Communications are not performed.
 *---------------------------------------------------------------------------*/
extern int  DWC_GetNumFriend(const DWCFriendData friendList[], int friendListLen);


/*---------------------------------------------------------------------------*
  Function for resetting the GameSpy Status string
  Arguments: statusString: String to be set as GameSpy Status. 255 characters + NULL termination.
  Return values:  TRUE: Set successful
          FALSE: Set failed such as happens prior to login
  Use: Sets the string to use as GameSpy Status for this player.
          The string that has been set can be gotten
          using DWC_GetFriendStatus() even from another host.
          '/' and '\\' are used by the library as special identifier strings
          and must not be used inside character strings.
          The set string is cleared
          when DWC_ShutdownFriendsMatch() is called.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetOwnStatusString(const char* statusString);


/*---------------------------------------------------------------------------*
  Function for getting the GameSpy Status string
  Argument : statusString: Pointer to the start address of memory used to store the GameSpy Status string. 255 characters + NULL termination.
  Return values: TRUE: Retrieval succeeded, FALSE: Get failed such as prior to login
  Use: Gets the current GameSpy Status string for this user.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_GetOwnStatusString(char* statusString);


/*---------------------------------------------------------------------------*
  Function for setting GameSpy Status data
  Arguments: statusData: Pointer to the binary data set as GameSpy Status.
          size :  Size of binary data Up to 189 bytes.
  Return values:  TRUE: Set successful
          FALSE: Set failed such as happens prior to login and when capacity is exceeded
  Use : Converts binary data to text representing base-64
          and sets it as GameSpy Status for this user.
          The string that has been set can be gotten
          using DWC_GetFriendStatus() even from another host.
          The set string is cleared
          when DWC_ShutdownFriendsMatch() is called.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetOwnStatusData(const char* statusData, u32 size);


/*---------------------------------------------------------------------------*
  Function for getting GameSpy Status data
  Argument: statusData : Pointer to start address of memory used to store GameSpy Status data
  Return values: -1 data retrieval failed such as prior to login,
          Non-negative integer: Size of binary data retrieved
  Use: Gets the current GameSpy Status data for this user.
          Only the binary data size is retrieved if NULL is passed for the argument statusData.
 *---------------------------------------------------------------------------*/
extern int DWC_GetOwnStatusData(char* statusData);


/*---------------------------------------------------------------------------*
  [obsolete functions]
  To maintain compatibility, the old function was kept as an in-line function.
 *---------------------------------------------------------------------------*/
inline BOOL DWC_SetGsStatusString(const char* statusString)
{
    return DWC_SetOwnStatusString( statusString );
}
inline BOOL DWC_GetGsStatusString(char* statusString)
{
    return DWC_GetOwnStatusString( statusString );
}
inline BOOL DWC_SetGsStatusData(const char* statusData, u32 size)
{
    return DWC_SetOwnStatusData( statusData, size );
}
inline int DWC_GetGsStatusData(char* statusData)
{
    return DWC_GetOwnStatusData( statusData );
}

/*---------------------------------------------------------------------------*
  Function for getting friend roster editable status
  Arguments : None
  Return values: TRUE: Friend roster may be edited, FALSE: Friend roster may not be edited.
  Use : Check whether or not the time is good to edit the friend list.
          The return values used here are enabled until DWC_ProcessFriendsMatch() is called again.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_CanChangeFriendList(void);


/*---------------------------------------------------------------------------*
  friend data complete deletion function
  arguments: friendData: pointer to friend data
  Return value: None
  Use : Friend data is cleared to zero and all friend relationships are removed from the GP server.
 *---------------------------------------------------------------------------*/
extern void DWC_DeleteBuddyFriendData(DWCFriendData* friendData);


/*---------------------------------------------------------------------------*
  Function for registering a callback for establishing friends
  Arguments: callback : Callback for establishing friends
          param : Parameters for the callback
  Return values: TRUE: registration succeeded
          FALSE: registration failed when FriendsMatch Library was inoperable
  Use : This function registers the callback function to be used
          when friends registered on the local friend roster are also recognized on the GP server as friends
          at times other than when DWC_UpdateServersAsync() is being called.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetBuddyFriendCallback(DWCBuddyFriendCallback callback, void* param);


/*---------------------------------------------------------------------------*
  friend status change notification callback registration function
  arguments: callback: friend status change notification callback
          param: callback parameters. if NULL is passed, nothing is set.
                   the default value for callback parameters is NULL.
  Return values: TRUE: registration succeeded
          FALSE: registration failed when FriendsMatch Library was inoperable
  use: Registers the callback that sends a notification when friend communication status changes.
          Although normally registration occurs on DWC_UpdateServersAsync(),
          this function can be used to get friend status without using
          DWC_UpdateServersAsync().
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetFriendStatusCallback(DWCFriendStatusCallback callback, void* param);


/*---------------------------------------------------------------------------*
  Base64 encoding function
  Arguments: src: pointer to conversion data
          srclen: length of conversion data
          dst: pointer to buffer storing converted data.
                 when NULL is specified, only the largest needed buffer size is returned.
          dstlen: the size of the buffer for the converted data (string length)
  Return value: -1: conversion failed. the needed converted buffer size exceeded that of dstlen.
          other values: length of converted string
  use: encodes the provided data in Base64, and outputs a char-type string
 *---------------------------------------------------------------------------*/
extern int DWC_Base64Encode(const char *src, const u32 srclen, char *dst, const u32 dstlen);


/*---------------------------------------------------------------------------*
  Base64 decoding function
  Arguments: src: pointer to Base64 data
          srclen: Base64 data length (string length)
          dst: pointer to buffer storing converted data.
                 when NULL is specified, only the largest needed buffer size is returned.
          dstlen: the size of the buffer for the converted data
  Return value: -1: conversion failed the needed converted buffer size exceeded that of dstlen.
          other values: size of converted data
  use: decodes the provided data from Base64, and outputs char-type data
 *---------------------------------------------------------------------------*/
extern int DWC_Base64Decode(const char *src, const u32 srclen, char *dst, const u32 dstlen);


/*---------------------------------------------------------------------------*
  Function to login to data storage server
  Argument: Login completed notification callback
          param : Parameters for the callback
  Return values:  TRUE: Login has started, Returns callback based on success or failure.
          FALSE: Did not start login because it is before DWC_LoginAsync() is complete
                and after login was completed. Callback does not return.
  Application: For login to GameSpy data storage server.
          If not logged in, cannot save to or load from the data storage server.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_LoginToStorageServerAsync(DWCStorageLoginCallback callback, void* param);


/*---------------------------------------------------------------------------*
  Function to log out of data storage server
  Arguments : None
  Return value: None
  Application: For logging out of GameSpy data storage server and
          releasing control objects.
          Even if this function is not called, if DWC_ShutdownFriendsMatch() is called,
          the logout process is performed.
 *---------------------------------------------------------------------------*/
extern void DWC_LogoutFromStorageServer(void);


/*---------------------------------------------------------------------------*
  Function for registering the callback functions to use to notify that data storage server save/load operations are complete
  Arguments: saveCallback : Callback for notifying that save is complete
          loadCallback : Callback for notifying that load is complete
  Return values: TRUE: registration succeeded
          FALSE: registration failed when FriendsMatch Library was inoperable
  Use : Registers the callback functions to be called when save/load operations to data storage have been completed
          Register the callback
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetStorageServerCallback(DWCSaveToServerCallback saveCallback,
                                         DWCLoadFromServerCallback loadCallback);


/*---------------------------------------------------------------------------*
  Data storage server Public data save function
  Arguments : keyvalues : Pointer to the data save text string (key/value value)
          param : Parameters for the callback
  Return values: TRUE: Save has started, FALSE: Either offline or save not possible during communication error
  Use : Anybody can save accessible data to the data storage server.
          Data to be saved must be represented by a string consisting of a key/value pair with NULL termination.
          Written using '\\' as a delimiter such as the example "\\name\\mario\\stage\\3".
          It is possible to save multiple key/value pairs at once such as with this example.)
          The callback function is called once the data has been set.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SavePublicDataAsync(char* keyvalues, void* param);


/*---------------------------------------------------------------------------*
  Data storage server Private data save function
  Arguments : keyvalues : Pointer to the data save text string (key/value value) to be saved
          param : Parameters for the callback
  Return values: TRUE: Save has started, FALSE: Either offline or save not possible during communication error
  Use : Only the user of this console can save accessible data to the data storage server.
          Data to be saved must be represented by a string consisting of a key/value pair with NULL termination.
          Written using '\\' as a delimiter such as the example "\\name\\mario\\stage\\3".
          It is possible to save multiple key/value pairs at once such as with this example.)
          The callback function is called once the data has been set.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SavePrivateDataAsync(char* keyvalues, void* param);


/*---------------------------------------------------------------------------*
  Function for loading one's own Public data from the data storage server
  Arguments: keys : Pointer to the key text string of data to be loaded
          param : Parameters for the callback
  Return values: TRUE: Load has started, FALSE: Either offline or load not possible during communication error
  Use : Loads Public data from the data storage server.
          Data to be loaded must be a text string specified by key and have NULL termination.
          Written using '\\' as a delimiter such as the example "\\name\\mario\\stage\\3".
          It is possible to load multiple keys at once such as with this example.)
          The callback function is called once the data has been loaded.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_LoadOwnPublicDataAsync(char* keys, void* param);


/*---------------------------------------------------------------------------*
  Function for loading one's own Private data from the data storage server
  Arguments: keys : Pointer to the key text string of data to be loaded
          param : Parameters for the callback
  Return values: TRUE: Load has started, FALSE: Either offline or load not possible during communication error
  Use : Loads Private data from the data storage server.
          Data to be loaded must be a text string specified by key and have NULL termination.
          Written using '\\' as a delimiter such as the example "\\name\\mario\\stage\\3".
          It is possible to load multiple keys at once such as with this example.)
          The callback function is called once the data has been loaded.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_LoadOwnPrivateDataAsync(char* keys, void* param);


/*---------------------------------------------------------------------------*
  Function for loading one's own data from the data storage server (obsolete function)
  Arguments: keys : Pointer to the key text string of data to be loaded
          param : Parameters for the callback
  Return values: TRUE: Load has started, FALSE: Either offline or load not possible during communication error
  Use : Loads data from the data storage server.
          Data to be loaded must be a text string specified by key and have NULL termination.
          Written using '\\' as a delimiter such as the example "\\name\\mario\\stage\\3".
          It is possible to load multiple keys at once such as with this example.)
          The callback function is called once the data has been loaded.
          -> a function with the same content has been renamed as DWC_LoadOwnPrivateDataAsync().
 *---------------------------------------------------------------------------*/
#define DWC_LoadOwnDataAsync DWC_LoadOwnPrivateDataAsync


/*---------------------------------------------------------------------------*
  Function for loading individual Public data from the data storage server
  Arguments: keys : Pointer to the key text string of data to be loaded
          index : Friend roster index for the user to be loaded
          param : Parameters for the callback
  Return values:  TRUE: load has started,
          FALSE: Load impossible because this user is offline or during a communication error due to the other party not being a friend
  Use : Loads Public data from the data storage server.
          Data to be loaded must be a text string specified by key and have NULL termination.
          Written using '\\' as a delimiter such as the example "\\name\\mario\\stage\\3".
          It is possible to load multiple keys at once such as with this example.)
          The callback function is called once the data has been loaded.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_LoadOthersDataAsync(char* keys, int index, void* param);


//----------------------------------------------------------------------------
// function - internal
//---------------------------------------------------------------------------
extern void DWCi_FriendInit(DWCFriendControl* friendcnt,
                            GPConnection* pGpObj,
                            const u16* playerName,
                            DWCFriendData* friendList,
                            int friendListLen);


extern void DWCi_FriendProcess(void);


extern void DWCi_UpdateServersAsync(const char* authToken,
                                    const char* partnerChallenge,
                                    DWCUpdateServersCallback updateCallback,
                                    void* updateParam,
                                    DWCFriendStatusCallback statusCallback,
                                    void* statusParam,
                                    DWCDeleteFriendListCallback deleteCallback,
                                    void* deleteParam);


extern void DWCi_StopFriendProcess(DWCError error, int errorCode);


extern void DWCi_GPGetInfoCallback(GPConnection* connection,
                                   GPGetInfoResponseArg* arg,
                                   void* param);

extern void DWCi_GPRecvBuddyRequestCallback(GPConnection* connection,
                                            GPRecvBuddyRequestArg* arg,
                                            void* param);

extern BOOL DWCi_GPRecvBuddyAuthCallback(GPConnection* connection,
                                         GPRecvBuddyMessageArg* arg,
                                         void* param );


extern void DWCi_GPRecvBuddyStatusCallback(GPConnection* connection,
                                           GPRecvBuddyStatusArg* arg,
                                           void* param);


extern DWCFriendData* DWCi_GetFriendList(void);


extern int  DWCi_GetFriendListLen(void);


extern int  DWCi_GetProfileIDFromList(int index);


extern int DWCi_GetFriendListIndex(int profileID);


extern void DWCi_InitGPProcessCount(void);


extern GPResult DWCi_SetGPStatus(int status, const char* statusString, const char* locationString);


extern BOOL DWCi_GetGPStatus(int* status, char* statusString, char* locationString);


extern void DWCi_CallBuddyFriendCallback(int index);


extern void DWCi_ShutdownFriend(void);


#ifdef __cplusplus
}
#endif


#endif // DWC_FRIEND_H_
