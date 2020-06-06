#include <nitro.h>
#include <base/dwc_base_gamespy.h>
#include <gp/gpi.h>
#include <gstats/gpersist.h>

#include <auth/util_base64.h>
#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>
#include <base/dwc_common.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// Insert a keep-alive into the GP server.
//#define DWC_ENABLE_KEEPALIVE_TO_GPSERVER


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
// Pointer to friend management object
static DWCFriendControl* stpFriendCnt = NULL;
static int stPersState = DWC_PERS_STATE_INIT;


//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static GPResult DWCi_GPProcess(void);
static void DWCi_CloseFriendProcess(void);
static void  DWCi_UpdateFriendReq(DWCFriendData friendList[], int friendListLen);
static void DWCi_EndUpdateServers(void);

static void DWCi_DeleteFriendFromList(DWCFriendData friendList[], int index, int otherIndex);
static BOOL DWCi_RefreshFriendListForth(DWCFriendData friendList[], int index, int profileID);
static int  DWCi_RefreshFriendListAll(DWCFriendData friendList[], int friendListLen, int profileID);

static GPResult DWCi_GPSendBuddyRequest(int profileID);

static BOOL DWCi_GetFriendBuddyStatus(const DWCFriendData* friendData, GPBuddyStatus* status);

static GPResult DWCi_HandleGPError(GPResult result);
static int DWCi_HandlePersError(int error);

static void DWCi_GPProfileSearchCallback(GPConnection* connection,
                                         GPProfileSearchResponseArg* arg,
                                         void* param);
static void DWCi_GPGetInfoCallback_RecvBuddyRequest(GPConnection* connection,
                                                    GPGetInfoResponseArg* arg,
                                                    void* param);
static void DWCi_GPGetInfoCallback_RecvAuthMessage(GPConnection* connection,
                                                   GPGetInfoResponseArg* arg,
                                                   void* param);

static void DWCi_StopPersLogin(DWCError error, int errorCode);
static void DWCi_SetPersistDataValuesAsync(int profileID,
                                           persisttype_t type,
                                           gsi_char* keyvalues,
                                           void* param);
static void DWCi_GetPersistDataValuesAsync(int profileID,
                                           persisttype_t type,
                                           gsi_char* keys,
                                           void* param);
static void DWCi_PersAuthCallback(int localid, int profileid,
                                  int authenticated, gsi_char* errmsg,
                                  void* instance);
static void DWCi_PersDataCallback(int localid, int profileid,
                                  persisttype_t type, int index, int success,
                                  time_t modified, char* data, int len,
                                  void* instance);
static void DWCi_PersDataSaveCallback(int localid, int profileid,
                                      persisttype_t type, int index, int success,
                                      time_t modified, void* instance);
static void DWCi_AddPersCallbackLevel(void);
static void DWCi_SubPersCallbackLevel(void);
static u32  DWCi_GetPersCallbackLevel(void);


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for getting friend communication status
  arguments: friendData: pointer to friend data
          statusString : Pointer to the start address of memory used to store the GameSpy status string defined by the game.
                         Strings cannot be copied if NULL has been passed.
                         Maximum string length is 255 character + NULL termination.
  Return value: Communication status enum value （DWC_STATUS_* in dwc_friend.h）.
                If called when this user is offline, only DWC_STATUS_OFFLINE will be returned.
                Even if the other party is not yet registered in the GP friend roster,
                DWC_STATUS_OFFLINE is returned.
  Use : Gets the communication status of the specified friend from the local cache.
        Communications are not performed.
        Notified by GP server each time the friend communication state changes.
        However, if power is cut unexpectedly during communication, the previous state should persist for several minutes.
 *---------------------------------------------------------------------------*/
u8 DWC_GetFriendStatus(const DWCFriendData* friendData, char* statusString)
{

    return DWC_GetFriendStatusSC(friendData, NULL, NULL, statusString);
}


/*---------------------------------------------------------------------------*
  friend communication state getting function (for server-client matchmaking games)
  arguments: friendData: pointer to friend data
             maxEntry  : the maximum number of connections when the friend is on the
                         server-client matchmaking server.
                         Pointer to start address of storage. Gets nothing if NULL is passed.
          numEntry: the current number of connections when the friend is on the
                    server-client matchmaking server.
                    Pointer to start address of storage. Gets nothing if NULL is passed.
          statusString : Pointer to the start address of memory used to store the GameSpy status string defined by the game.
                         Strings cannot be copied if NULL has been passed.
                         Maximum string length is 255 character + NULL termination.
  Return value: Communication status enum value （DWC_STATUS_* in dwc_friend.h）.
                If called when this user is offline, only DWC_STATUS_OFFLINE will be returned.
                Even if the other party is not yet registered in the GP friend roster,
                DWC_STATUS_OFFLINE is returned.
  Use : Gets the communication status of the specified friend from the local cache.
        Communications are not performed.
        Notified by GP server each time the friend communication state changes.
        However, if power is cut unexpectedly during communication, the previous state should persist for several minutes.
 *---------------------------------------------------------------------------*/
u8 DWC_GetFriendStatusSC(const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusString)
{
    char valueStr[4];
    int  len;
    GPBuddyStatus status;

    // Gets GPBudyStatus
    if (DWCi_GetFriendBuddyStatus(friendData, &status)){
        if (status.status == DWC_STATUS_MATCH_SC_SV){
            // when the friend's server-client matchmaking server is up
            if (maxEntry){
                // Reads the setting for maximum number of connections
                len = DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_MAX,
                                               valueStr,
                                               status.statusString, '/');
                if (len > 0) *maxEntry = (u8)strtoul(valueStr, NULL, 10);
                else *maxEntry = 0;
            }

            if (numEntry){
                // Reads the setting for current number of connections
                len = DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_NUM,
                                               valueStr,
                                               status.statusString, '/');
                if (len > 0) *numEntry = (u8)strtoul(valueStr, NULL, 10);
                else *numEntry = 0;
            }
        }
        else {
            // when the friend is not the server-client matchmaking server
            if (maxEntry) *maxEntry = 0;
            if (numEntry) *numEntry = 0;
        }

        if (statusString){
            // Set statusString to locationString
            strcpy(statusString, status.locationString);
        }

        return status.status;
    }
    else {
        // Errors are treated the same as OFFLINE
        if (maxEntry) *maxEntry = 0;
        if (numEntry) *numEntry = 0;
        return DWC_STATUS_OFFLINE;
    }
}


/*---------------------------------------------------------------------------*
  Function for getting the friend communication status (statusData version)
  arguments: friendData: pointer to friend data
             statusData: Pointer to the start address used to store GameSpy status data defined by the game.
                         Data cannot be copied if NULL has been passed.
             statusData: Pointer to the start address used to store GameSpy status data length defined by the game.
                         -1 is stored here in the case of invalid data.
  Return value: Communication status enum value （DWC_STATUS_* in dwc_friend.h）.
                If called when this user is offline, only DWC_STATUS_OFFLINE will be returned.
                Even if the other party is not yet registered in the GP friend roster,
                DWC_STATUS_OFFLINE is returned.
  Use : Gets the communication status of the specified friend from the local cache.
        Communications are not performed.
        Notified by GP server each time the friend communication state changes.
        However, if power is cut unexpectedly during communication, the previous state should persist for several minutes.
        If the return value is DWC_STATUS_OFFLINE, the function returns without setting any values, other than setting *size = -1.
           
 *---------------------------------------------------------------------------*/
u8 DWC_GetFriendStatusData(const DWCFriendData* friendData, char* statusData, int* size)
{

    return DWC_GetFriendStatusDataSC(friendData, NULL, NULL, statusData, size);
}


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
  Return value: Communication status enum value （DWC_STATUS_* in dwc_friend.h）.
          If called when this user is offline, only DWC_STATUS_OFFLINE will be returned.
          Even if the other party is not yet registered in the GP friend roster,
          DWC_STATUS_OFFLINE is returned.
  Use : Gets the communication status of the specified friend from the local cache.
        Communications are not performed.
        Notified by GP server each time the friend communication state changes.
        However, if power is cut unexpectedly during communication, the previous state should persist for several minutes.
        If the return value is DWC_STATUS_OFFLINE, the function returns without setting any values, other than setting *size = -1.
           
 *---------------------------------------------------------------------------*/
u8 DWC_GetFriendStatusDataSC(const DWCFriendData* friendData, u8* maxEntry, u8* numEntry, char* statusData, int* size)
{
    u8   ret;
    char statusString[DWC_GS_STATUS_STRING_LEN];

    ret = DWC_GetFriendStatusSC(friendData, maxEntry, numEntry, statusString);

    if (ret == DWC_STATUS_OFFLINE){
        *size = -1;
        return ret;
    }

    // First check only the size of binary data
    *size = DWC_Auth_Base64Decode(statusString, strlen(statusString), NULL, 0);

    if (!statusData || (*size == -1)) return ret;

    // Convert text data to binary data and store
    (void)DWC_Auth_Base64Decode(statusString, strlen(statusString), statusData, (u32)*size);

    return ret;
}


/*---------------------------------------------------------------------------*
  Function for getting number of friends
  Arguments: friendlist : Friend roster
          friendListLen : Maximum length of the friend list (number of entries)
  Return value: The number of friends registered on the friend roster
  Use : Accesses the local friend roster and returns the number of registered friends Communications are not performed.
 *---------------------------------------------------------------------------*/
int  DWC_GetNumFriend(const DWCFriendData friendList[], int friendListLen)
{
    int count = 0;
    int i;

    if (!friendList) return 0;

    for (i = 0; i < friendListLen; i++){
        // [note]
        // With this determination, the friend data of friends having different game codes (which should be disabled)
        // ends up being counted
        if (DWCi_Acc_IsValidFriendData(&friendList[i])) count++;
    }

    return count;
}


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
BOOL DWC_SetOwnStatusString(const char* statusString)
{
    
    if ((stpFriendCnt == NULL) || !DWCi_CheckLogin()) return FALSE;
    
    if (DWCi_SetGPStatus(DWC_GP_STATUS_NO_CHANGE, NULL, statusString)) return FALSE;
    else return TRUE;
}


/*---------------------------------------------------------------------------*
  Function for getting the GameSpy Status string
  Argument : statusString: Pointer to the start address of memory used to store the GameSpy Status string. 255 characters + NULL termination.
  Return values: TRUE: Retrieval succeeded, FALSE: Get failed such as prior to login
  Use: Gets the current GameSpy Status string for this user.
 *---------------------------------------------------------------------------*/
BOOL DWC_GetOwnStatusString(char* statusString)
{

    if ((stpFriendCnt == NULL) || (*stpFriendCnt->pGpObj == NULL) || !statusString) return FALSE;

    // [note]
    // The GPIConnection structure listed in gpi.h is used.
    strcpy(statusString, ((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString);

    return TRUE;
}


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
BOOL DWC_SetOwnStatusData(const char* statusData, u32 size)
{
    char statusString[DWC_GS_STATUS_STRING_LEN];
    int  len;
    
    if (!stpFriendCnt || !DWCi_CheckLogin()) return FALSE;

    // Performs text conversion
    len = DWC_Auth_Base64Encode(statusData, size,
                                statusString, DWC_GS_STATUS_STRING_LEN-1);

    if (len == -1) return FALSE;  // Fails if the number of characters exceeds the limit

    statusString[len] = '\0';     // Guarantees NULL termination
    
    if (DWCi_SetGPStatus(DWC_GP_STATUS_NO_CHANGE, NULL, statusString)) return FALSE;
    else return TRUE;
}


/*---------------------------------------------------------------------------*
  Function for getting GameSpy Status data
  Argument: statusData : Pointer to start address of memory used to store GameSpy Status data
  Return values: Get failed due to invalid data or prior to login
          Non-negative integer: Size of binary data retrieved
  Use: Gets the current GameSpy Status data for this user.
          Only the binary data size is retrieved if NULL is passed for the argument statusData.
 *---------------------------------------------------------------------------*/
int DWC_GetOwnStatusData(char* statusData)
{
    int size;

    if (!stpFriendCnt || !DWCi_CheckLogin()) return FALSE;

    // [note]
    // The GPIConnection structure listed in gpi.h is used.

    // First check only the size of binary data
    size = DWC_Auth_Base64Decode(((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString,
                                 strlen(((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString),
                                 NULL, 0);

    if (!statusData || (size == -1)) return size;

    // Convert text data to binary data and store
    return DWC_Auth_Base64Decode(((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString,
                                 strlen(((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString),
                                 statusData, (u32)size);
}


/*---------------------------------------------------------------------------*
  Function for getting friend roster editable status
  Arguments : None
  Return values: TRUE: Friend roster may be edited, FALSE: Friend roster may not be edited.
  Use : Check whether or not the time is good to edit the friend list.
          The return values used here are enabled until DWC_ProcessFriendsMatch() is called again.
 *---------------------------------------------------------------------------*/
BOOL DWC_CanChangeFriendList(void)
{

    if ((stpFriendCnt != NULL) &&
        ((stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_CHECK) ||
         (stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_PSEARCH))){
        // Editing is prohibited
        // Since friend data which cannot be checked due to movement of data in
        // editing is prohibited during this time.
        return FALSE;
    }
    else {
        return TRUE;
    }
}


/*---------------------------------------------------------------------------*
  friend data complete deletion function
  arguments: friendData: pointer to friend data
  Return value: None
  Use : Friend data is cleared to zero and all friend relationships are removed from the GP server.
 *---------------------------------------------------------------------------*/
void DWC_DeleteBuddyFriendData(DWCFriendData* friendData)
{
    int  profileID;

    if ((stpFriendCnt != NULL)          &&
        DWCi_CheckLogin()               &&
        (DWCi_GetUserData() != NULL)    &&
        ((profileID = DWC_GetGsProfileId(DWCi_GetUserData(), friendData)) != 0) &&
        (profileID != -1) &&
        gpIsBuddy(stpFriendCnt->pGpObj, profileID)){
        // If a valid profile ID is included in friend data,
        // and that profile ID is a friend on the GP,
        // the friend is removed.
        (void)gpDeleteBuddy(stpFriendCnt->pGpObj, profileID);

        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "DWC_DeleteBuddyFriendData : Deleted buddy.\n");
    }
    else {
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "DWC_DeleteBuddyFriendData : Only clear data.\n");
    }

    // zero-clears the friend data
    MI_CpuClear8(friendData, sizeof(DWCFriendData));
}


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
BOOL DWC_SetBuddyFriendCallback(DWCBuddyFriendCallback callback, void* param)
{

    if (!stpFriendCnt) return FALSE;

    stpFriendCnt->buddyCallback = callback;
    stpFriendCnt->buddyParam    = param;

    return TRUE;
}


/*---------------------------------------------------------------------------*
  friend status change notification callback registration function
  arguments: callback: friend status change notification callback
          param: callback parameters if NULL is passed, nothing is set
                   the default value for callback parameters is NULL.
  Return values: TRUE: registration succeeded
          FALSE: registration failed when FriendsMatch Library was inoperable
  use: Registers the callback that sends a notification when friend communication status changes.
          Although normally registration occurs on DWC_UpdateServersAsync(),
          this function can be used to get friend status without using
          DWC_UpdateServersAsync().
 *---------------------------------------------------------------------------*/
BOOL DWC_SetFriendStatusCallback(DWCFriendStatusCallback callback, void* param)
{

    if (!stpFriendCnt) return FALSE;

    stpFriendCnt->statusCallback  = callback;
    if (param != NULL){
        stpFriendCnt->statusParam = param;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Base64 encoding function
  Arguments: src: pointer to conversion data
          srclen: length of conversion data
          dst: pointer to buffer storing converted data.
                 when NULL is specified, only the largest needed buffer size is returned.
          dstlen: the size of the buffer for the converted data (string length)
  Return value: -1: conversion failed the needed converted buffer size exceeded that of dstlen.
          other values: length of converted string
  use: encodes the provided data in Base64, and outputs a char-type string
 *---------------------------------------------------------------------------*/
int DWC_Base64Encode(const char *src, const u32 srclen, char *dst, const u32 dstlen)
{
    return DWC_Auth_Base64Encode(src, srclen, dst, dstlen);
}


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
int DWC_Base64Decode(const char *src, const u32 srclen, char *dst, const u32 dstlen)
{
    return DWC_Auth_Base64Decode(src, srclen, dst, dstlen);
}

/*---------------------------------------------------------------------------*
  Function to login to data storage server
  Argument: Login completed notification callback
          param : Parameters for the callback
  Return values:  TRUE: Login has started, Returns callback based on success or failure.
          FALSE: Did not start login because it is before DWC_LoginAsync() is complete and after login was completed.
                and after login was completed. Callback does not return.
  Application: For login to GameSpy data storage server.
          If not logged in, cannot save to or load from the data storage server.
 *---------------------------------------------------------------------------*/
BOOL DWC_LoginToStorageServerAsync(DWCStorageLoginCallback callback, void* param)
{
    char response[33];
    char *authToken, *partnerChallenge;
    int  persResult;
    int  i;

    // If not yet logged-in to the GP server or already logged-in to the Persistent server,
    // return without doing anything.
    if (!DWCi_GetAuthInfo(&authToken, &partnerChallenge) || IsStatsConnected())
        return FALSE;

    stpFriendCnt->persLoginCallback = callback;
    stpFriendCnt->persLoginParam    = param;

    for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
		stPersState = DWC_PERS_STATE_LOGIN;
        persResult = InitStatsConnection(0);  // Shared initialization of stats and persistent SDK
    
        if (persResult == GE_NOERROR) break;
    
        // Retries a set number of times if a DNS error occurs
        if ((persResult != GE_NODNS) || (i == DWC_DNS_ERROR_RETRY_MAX-1)){
            DWCi_HandlePersError(persResult);
            return TRUE;  // Returns an error using a callback
        }
    }

    // Authentication is performed for the Persistent server
    (void)GenerateAuth(GetChallenge(NULL), partnerChallenge, response);
    PreAuthenticatePlayerPartner(0, authToken, response,
                                 DWCi_PersAuthCallback, NULL);

    DWCi_AddPersCallbackLevel();

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Function to log out of data storage server
  Arguments : None
  Return value: None
  Application: For logging out of GameSpy data storage server and
          releasing control objects.
          Even if this function is not called, if DWC_ShutdownFriendsMatch() is called,
          the logout process is performed.
 *---------------------------------------------------------------------------*/
void DWC_LogoutFromStorageServer(void)
{

    CloseStatsConnection();  // Releases persistent objects

    stPersState = DWC_PERS_STATE_INIT;

    if (stpFriendCnt != NULL) stpFriendCnt->persCallbackLevel = 0;
}


/*---------------------------------------------------------------------------*
  Function for registering the callback functions to use to notify that data storage server save/load operations are complete
  Arguments: saveCallback : Callback for notifying that save is complete
          loadCallback : Callback for notifying that load is complete
  Return values: TRUE: registration succeeded
          FALSE: registration failed when FriendsMatch Library was inoperable
  Use : Registers the callback functions to be called when save/load operations to data storage have been completed
          Register the callback
 *---------------------------------------------------------------------------*/
BOOL DWC_SetStorageServerCallback(DWCSaveToServerCallback saveCallback,
                                  DWCLoadFromServerCallback loadCallback)
{

    if (!stpFriendCnt) return FALSE;

    stpFriendCnt->saveCallback = saveCallback;
    stpFriendCnt->loadCallback = loadCallback;

    return TRUE;
}


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
BOOL DWC_SavePublicDataAsync(char* keyvalues, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt) return FALSE;

    DWCi_SetPersistDataValuesAsync(stpFriendCnt->profileID, pd_public_rw,
                                   keyvalues, param);

    return TRUE;
}


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
BOOL DWC_SavePrivateDataAsync(char* keyvalues, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt) return FALSE;

    DWCi_SetPersistDataValuesAsync(stpFriendCnt->profileID, pd_private_rw,
                                   keyvalues, param);

    return TRUE;
}


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
BOOL DWC_LoadOwnPublicDataAsync(char* keys, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt) return FALSE;

    DWCi_GetPersistDataValuesAsync(stpFriendCnt->profileID, pd_public_rw, keys, param);

    return TRUE;
}


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
BOOL DWC_LoadOwnPrivateDataAsync(char* keys, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt) return FALSE;

    DWCi_GetPersistDataValuesAsync(stpFriendCnt->profileID, pd_private_rw, keys, param);

    return TRUE;
}


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
BOOL DWC_LoadOthersDataAsync(char* keys, int index, void* param)
{

    if ((stPersState != DWC_PERS_STATE_CONNECTED) || DWCi_IsError() || !stpFriendCnt ||
        !DWCi_GetProfileIDFromList(index))
        return FALSE;

    DWCi_GetPersistDataValuesAsync(DWCi_GetProfileIDFromList(index), pd_public_rw, keys, param);

    return TRUE;
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for initializing the friend management structure
  Arguments: friendcnt : Pointer to the friend management object
          pGpObj  :	Pointer to the gp connection structure pointer
          playerName : Pointer to this user's player name as also accessed by other users
          friendList : Pointer to the friend roster
          friendListLen : Maximum length of the friend list (number of entries)
  Return value: None
  Use : Initializes the friend management structure
 *---------------------------------------------------------------------------*/
void DWCi_FriendInit(DWCFriendControl* friendcnt,
                     GPConnection* pGpObj,
                     const u16* playerName,
                     DWCFriendData* friendList,
                     int friendListLen)
{

    SDK_ASSERT(friendcnt);

    stpFriendCnt = friendcnt;

    stpFriendCnt->state              = DWC_FRIEND_STATE_INIT;
    stpFriendCnt->pGpObj             = pGpObj;
    stpFriendCnt->gpProcessCount     = 0;
    stpFriendCnt->lastGpProcess      = 0;
    stpFriendCnt->friendListLen      = friendListLen;
    stpFriendCnt->friendList         = friendList;
    stpFriendCnt->buddyUpdateIdx     = 0;
    stpFriendCnt->friendListChanged  = 0;
    stpFriendCnt->buddyUpdateState   = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete   = 0;
    stpFriendCnt->persCallbackLevel  = 0;
    stpFriendCnt->profileID          = 0;
    stpFriendCnt->playerName         = playerName;
    stpFriendCnt->updateCallback     = NULL;
    stpFriendCnt->updateParam        = NULL;
    stpFriendCnt->statusCallback     = NULL;
    stpFriendCnt->statusParam        = NULL;
    stpFriendCnt->deleteCallback     = NULL;
    stpFriendCnt->deleteParam        = NULL;
    stpFriendCnt->buddyCallback      = NULL;
    stpFriendCnt->buddyParam         = NULL;
    stpFriendCnt->persLoginCallback  = NULL;
    stpFriendCnt->persLoginParam     = NULL;
    stpFriendCnt->saveCallback       = NULL;
    stpFriendCnt->loadCallback       = NULL;
}


/*---------------------------------------------------------------------------*
  Function for updating friend management
  Arguments : None
  Return value: None
  Use : Called every game frame, this function updates communication processing
 *---------------------------------------------------------------------------*/
void DWCi_FriendProcess(void)
{
    GPResult gpResult;

    if (!stpFriendCnt || !stpFriendCnt->friendList || DWCi_IsError())
        return;

    if (DWCi_GetPersCallbackLevel() || IsStatsConnected()){
        // If currently connected the Persistent server, or waiting for a Persistent callback,
        // update the Persistent communications operation.
        if (!PersistThink()){
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                       "Connection to the stats server was lost\n");

            // [note]
            // Just as with the ServerBrowsing SDK, this return value operation is not necessary
#if 0
            DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                                   DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_NETWORK);
            return;
#endif
        }
    }

    // GP communication processing update
    if (stpFriendCnt->pGpObj && *stpFriendCnt->pGpObj){
        gpResult = DWCi_GPProcess();

        // As GP error processing occurs with the error callback called from gpProcess(),
        // if an error is detected here, it is acceptable to exit without doing anything
        if (gpResult != GP_NO_ERROR) return;

        // After GP server login, the local friend roster is not updated unless gpProcess() is called once.
        // Therefore, functions for adding/removing friends from the roster are called here.
        // Calls the register and cancel operations.
        if (stpFriendCnt->friendList &&
            (stpFriendCnt->buddyUpdateState != DWC_BUDDY_UPDATE_STATE_COMPLETE) &&
            (stpFriendCnt->gpProcessCount > DWC_FRIEND_UPDATE_WAIT_COUNT)){
            if ((stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_WAIT) ||
                (stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_CHECK)){
                // Synchronous processing of the friend roster and local friend roster
                DWCi_UpdateFriendReq(stpFriendCnt->friendList,
                                     stpFriendCnt->friendListLen);
            }

            if (stpFriendCnt->buddyUpdateIdx >= stpFriendCnt->friendListLen){
                // Update complete
                stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_COMPLETE;
                stpFriendCnt->svUpdateComplete++;
            }
        }
    }

    if (stpFriendCnt->svUpdateComplete >= 2){
        // Once friend list synchronization is complete, an exit processing function is called
        stpFriendCnt->svUpdateComplete = 0;
        DWCi_EndUpdateServers();
    }
}


/*---------------------------------------------------------------------------*
  friend list synchronization function
  Argument: authToken : Pointer to the token used for authentication
          partnerChallenge : Pointer to the challenge text string used for authentication
          updateCallback: friend synchronization complete callback
          updateParam : Parameter for the above callback
          statusCallback     Callback for notification of changes in friend status
          statusParam : Parameter for the above callback
          deleteCallback     Friend roster delete callback
          deleteParam : Parameter for the above callback
  Return value: None
  use: after login completes, performs synchronization of friend list (buddy list)
          on GP server with local friend list
 *---------------------------------------------------------------------------*/
void DWCi_UpdateServersAsync(const char* authToken,
                             const char* partnerChallenge,
                             DWCUpdateServersCallback updateCallback,
                             void* updateParam,
                             DWCFriendStatusCallback statusCallback,
                             void* statusParam,
                             DWCDeleteFriendListCallback deleteCallback,
                             void* deleteParam)
{
#if 0  // Make it so the 051007 Persistent server is never accessed
    char response[33];
    int  persResult;
    int  i;
#else
#pragma unused(authToken)
#pragma unused(partnerChallenge)
#endif // Make it so the 051007 Persistent server is never accessed

    SDK_ASSERT(updateCallback);

    stpFriendCnt->updateCallback = updateCallback;
    stpFriendCnt->updateParam    = updateParam;
    stpFriendCnt->statusCallback = statusCallback;
    stpFriendCnt->statusParam    = statusParam;
    stpFriendCnt->deleteCallback = deleteCallback;
    stpFriendCnt->deleteParam    = deleteParam;

    stpFriendCnt->friendListChanged = 0;  // Initializes the modification flag of the friend roster
    // After calling the next gpProcess(), requests to add/remove data to/from the friend roster are processed
    stpFriendCnt->buddyUpdateState  = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete  = 0;  // friend list synchronization complete flag clear
    stpFriendCnt->buddyUpdateIdx    = 0;
    
    // Transition to Persistent server login mode
    stpFriendCnt->state = DWC_FRIEND_STATE_PERS_LOGIN;

    // [arakit] main 051020
    if (stpFriendCnt->friendList == NULL){
        // When the friend list is not used, its set so that the friend list synchronous operation has finished
        stpFriendCnt->svUpdateComplete++; 
    }
    // [arakit] main 051020

#if 0  // Make it so the 051007 Persistent server is never accessed
    if (stpFriendCnt->playerName[0] != '\0'){
        // If a player name has been set, it is set for the Persistent server
        for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
            persResult = InitStatsConnection(0);  // Shared initialization of stats and persistent SDK
    
            if (!persResult) break;
    
            // Retries a set number of times if a DNS error occurs
            if ((persResult != GE_NODNS) || (i == DWC_DNS_ERROR_RETRY_MAX-1)){
                DWCi_HandlePersError(persResult);
                return;  // Returns an error using a callback
            }
        }

        // Authentication is performed for the Persistent server
        (void)GenerateAuth(GetChallenge(NULL), partnerChallenge, response);
        PreAuthenticatePlayerPartner(0, authToken, response,
                                     DWCi_PersAuthCallback, NULL);

        DWCi_AddPersCallbackLevel();
    }
    else {
#endif // Make it so the 051007 Persistent server is never accessed
        // As long as a player name has been set,
        // the fact that Persistent processing has completed is recorded and processing ends
        stpFriendCnt->svUpdateComplete++;
#if 0  // Make it so the 051007 Persistent server is never accessed
    }
#endif // Make it so the 051007 Persistent server is never accessed
}


/*---------------------------------------------------------------------------*
  friend management process error end function
  Arguments: error: DWC error type
          errorCode : Error code
  Return value: None
  Use : Processing is halted if an error occurs during friend management process
 *---------------------------------------------------------------------------*/
void DWCi_StopFriendProcess(DWCError error, int errorCode)
{

    if (!stpFriendCnt || (error == DWC_ERROR_NONE)) return;

    DWCi_SetError(error, errorCode);

    if ((stpFriendCnt->state != DWC_FRIEND_STATE_INIT) &&
        (stpFriendCnt->state != DWC_FRIEND_STATE_LOGON)){
        // A callback function is called on an error if it occurs during friend list synchronization
        stpFriendCnt->updateCallback(error, stpFriendCnt->friendListChanged,
                                     stpFriendCnt->updateParam);
    }

    DWCi_CloseFriendProcess();
}


/*---------------------------------------------------------------------------*
  GP friend roster received callback function
  Arguments: connection : Pointer to the GP connection object
          arg : Pointer to a GPRecvBuddyRequestArg type object
          param : Parameters for the callback
  Return value: None
  Use : GP friend roster received callback.
          Since DWC_GetUserData() is used,
          code must not be changed so that it is called before DWC_InitFriendsMatch().
 *---------------------------------------------------------------------------*/
void DWCi_GPRecvBuddyRequestCallback(GPConnection* connection,
                                     GPRecvBuddyRequestArg* arg,
                                     void* param)
{
#pragma unused(param)

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Received buddy request from %u\n",
               arg->profile);

    if (!stpFriendCnt->friendList) return;

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Begin to search gpInfo.\n");

    // Even if the friend already has a profile ID,
    // you must get the lastname from the profile ID using gpGetInfo
    gpGetInfo(connection, arg->profile, GP_DONT_CHECK_CACHE,
              GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback_RecvBuddyRequest, NULL );
}

/*---------------------------------------------------------------------------*
  GP friend response received callback function
  Arguments: connection : Pointer to the GP connection object
          arg : Pointer to a GPRecvBuddyMessageArg type object
          param : Parameters for the callback
  Return values: True if response is a GP friend request
  Use : GP friend response received callback. Can be called using
          DWCi_GPRecvBuddyMessageCallback.
 *---------------------------------------------------------------------------*/
BOOL DWCi_GPRecvBuddyAuthCallback( GPConnection* connection,
                                   GPRecvBuddyMessageArg* arg,
                                   void* param )
{
#pragma unused(param)
    char* message = arg->message;

    if (!strcmp(message, "I have authorized your request to add me to your list"))
    {
        // Even if the friend already has a profile ID,
        // you must get the lastname from the profile ID using gpGetInfo
        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                   "Received buddy authenticated message from %u.\n",
                   arg->profile);
        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Begin to search gpInfo.\n");
            
        gpGetInfo(connection, arg->profile, GP_DONT_CHECK_CACHE,
                  GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback_RecvAuthMessage, NULL );
        return TRUE;
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  GP friend status change receive callback function
  Arguments: connection : Pointer to the GP connection object
          arg : Pointer to a GPRecvBuddyStatusArg type object
          param : Parameters for the callback
  Return value: None
  Use : Since DWC_GetUserData() is used,
          Since DWC_GetUserData() is used,
          code must not be changed so that it is called before DWC_InitFriendsMatch().
 *---------------------------------------------------------------------------*/
void DWCi_GPRecvBuddyStatusCallback(GPConnection* connection,
                                    GPRecvBuddyStatusArg* arg,
                                    void* param)
{
#pragma unused(param)
    int friendIdx;
    GPBuddyStatus status;

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "RECV update friend status. p:%d\n", arg->profile);

    if (stpFriendCnt->statusCallback){
        friendIdx = DWCi_GetFriendListIndex(arg->profile);

        if (friendIdx == -1){
            // If the local friend roster is removed at just the right time,
            return;
        }

        // no error should result
        (void)gpGetBuddyStatus(connection, arg->index, &status);

        // Calls the callback registered by the user
        stpFriendCnt->statusCallback(friendIdx,
                                     (u8)status.status,
                                     status.locationString,
                                     stpFriendCnt->statusParam);
    }
}


/*---------------------------------------------------------------------------*
  Function for getting the friend roster
  Arguments : None
  Return value: Pointer to the friend roster. NULL as long as there are no friend management objects.
  Use : Gets a pointer to the friend roster used to save friend management objects
 *---------------------------------------------------------------------------*/
DWCFriendData* DWCi_GetFriendList(void)
{

    if (stpFriendCnt){
        return stpFriendCnt->friendList;
    }
    else {
        return NULL;
    }
}


/*---------------------------------------------------------------------------*
  Function for getting the friend roster length
  Arguments : None
  Return value: Friend roster length. 0 as long as there are no friend management objects.
  Use : Gets the length of the friend roster used to save friend management objects
 *---------------------------------------------------------------------------*/
int DWCi_GetFriendListLen(void)
{

    if (stpFriendCnt){
        return stpFriendCnt->friendListLen;
    }
    else {
        return 0;
    }
}


/*---------------------------------------------------------------------------*
  Function for getting the profile ID from the friend roster
  Argument: index : Friend roster index of the friend who Profile ID is to be gotten
  Return values: 0 : Invalid data (including empty data) in the roster or it only contains an login ID
                     or if there is no friend list
          Non-0: Profile ID of friend
  Use : Gets the profile ID of the friend data in the specified index
          from the local friend roster.
          Since this function cannot distinguish whether a roster is empty, or has a different game code, or contains a login ID
          for the time being, use this function when you wish to get
          a profile ID.
          Since DWC_GetUserData() is used,
          code must not be changed so that it is called before DWC_InitFriendsMatch().
 *---------------------------------------------------------------------------*/
int  DWCi_GetProfileIDFromList(int index)
{
    int profileID;

    if (stpFriendCnt->friendList == NULL) return 0;

    profileID = DWC_GetGsProfileId(DWCi_GetUserData(), &stpFriendCnt->friendList[index]);

    if (!profileID || (profileID == -1)) return 0;
    else return profileID;
}


/*---------------------------------------------------------------------------*
  Function for getting the index from a friend roster
  Argument: profileID : The profile ID
  Return value: The index in the friend roster having the profile ID in question. Otherwise -1 is returned.
  Use : Returns the index in the friend roster (if it exists) based on the profile ID.
          Since DWC_GetUserData() is used,
          code must not be changed so that it is called before DWC_InitFriendsMatch().
 *---------------------------------------------------------------------------*/
int DWCi_GetFriendListIndex(int profileID)
{
    int i;

    if (!stpFriendCnt || !profileID) return -1;

    for (i = 0; i < stpFriendCnt->friendListLen; i++){
        if (DWCi_GetProfileIDFromList(i) == profileID){
            return i;
        }
    }

    return -1;
}


/*---------------------------------------------------------------------------*
  Function for initializing the gpProcess() call counter
  Arguments : None
  Return value: None
  Use : Initializes the counter used when calling gpProcess()
 *---------------------------------------------------------------------------*/
void DWCi_InitGPProcessCount(void)
{

    if (stpFriendCnt){
        stpFriendCnt->gpProcessCount = 0;
        stpFriendCnt->lastGpProcess  = OS_GetTick();
    }
}


/*---------------------------------------------------------------------------*
  gpStatus set function
  Arguments: statusString: String to be set as gpStatus status.
                         Returns DWC_GP_STATUS_NO_CHANGE as long as the current value is not to be changed.
          statusString : String to be set as the gpStatus statusString.
                         Returns NULL as long as the current string is not to be changed.
          locationString : String to be set as the gpStatus locationString.
                         Returns NULL as long as the current string is not to be changed.
                         Since this is used here as a game definition string,
                         it is usually not overwritten by the library.
  Return value: GP processing result type. Although GP_NO_ERROR is returned if this function is called prior to login,
          no processing is performed.
  Use : Sets gpStatus for this user
 *---------------------------------------------------------------------------*/
GPResult DWCi_SetGPStatus(int status, const char* statusString, const char* locationString)
{

    // If statusString or locationString contain too many characters,
    // they are copied only up to the maximum number of characters allowed without generating an error

#if 1
    // [note]
    // Although code really should be written this way, if it is not written using an 'else'
    // a GP error will result for some reason after waiting a bit when DWC_SetOwnStatusData() has been called from inside the login callback.
    //  The error code is GP_PARSE.
    // In either case, this determination is escaped and code is executed up to gpSetStatus().
    // -> resolved
    if ((stpFriendCnt == NULL) || !DWCi_CheckLogin()) return GP_NO_ERROR;
#else
    if ((stpFriendCnt == NULL) || (stpFriendCnt->pGpObj == NULL)) return GP_NO_ERROR;
#endif

    // [note]
    // The GPIConnection structure listed in gpi.h is used.
    if (status == DWC_GP_STATUS_NO_CHANGE){
        status = ((GPIConnection *)*stpFriendCnt->pGpObj)->lastStatus;
    }
    else {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Change GP status->status %d\n", status);
    }

    if (!statusString){
        statusString = ((GPIConnection *)*stpFriendCnt->pGpObj)->lastStatusString;
    }
    else {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Change GP status->statusString %s\n", statusString);
    }
        
    if (!locationString){
        locationString = ((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString;
    }
    else {
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Change GP status->locationString %s\n", locationString);
    }

    return gpSetStatus(stpFriendCnt->pGpObj, (GPEnum)status, statusString, locationString);
}


/*---------------------------------------------------------------------------*
  Function for getting gpStatus
  Argument : status : gpStatus status
          statusString : gpStatus statusString
          locationString : gpStatus locationString
  Return values: TRUE: Retrieval succeeded, FALSE: Get failed such as prior to login
  Use: Gets the current gpStatus for this user.
 *---------------------------------------------------------------------------*/
BOOL DWCi_GetGPStatus(int* status, char* statusString, char* locationString)
{

    if ((stpFriendCnt == NULL) || (*stpFriendCnt->pGpObj == NULL)) return FALSE;

    // [note]
    // The GPIConnection structure listed in gpi.h is used.
    *status = ((GPIConnection *)*stpFriendCnt->pGpObj)->lastStatus;
    strcpy(statusString, ((GPIConnection *)*stpFriendCnt->pGpObj)->lastStatusString);
    strcpy(locationString, ((GPIConnection *)*stpFriendCnt->pGpObj)->lastLocationString);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Function for calling the callback used to establish friends
  Argument: index : Friend roster index for the friend with whom a friend relationship has been established
  Return value: None
  Use : The established friend callback function is called except during friend list synchronization
 *---------------------------------------------------------------------------*/
void DWCi_CallBuddyFriendCallback(int index)
{

    if (stpFriendCnt->buddyCallback &&
        (stpFriendCnt->state != DWC_FRIEND_STATE_PERS_LOGIN)){
        stpFriendCnt->buddyCallback(index, stpFriendCnt->buddyParam);
    }

    // The status update callback is explicitly called when establishing a friend.
    if (stpFriendCnt->statusCallback){
        u8 st;
        GPBuddyStatus status;

        st = DWC_GetFriendStatus( &stpFriendCnt->friendList[index], status.locationString );
        
        // Calls the callback registered by the user
        stpFriendCnt->statusCallback(index,
                                     st,
                                     status.locationString,
                                     stpFriendCnt->statusParam);
    }
}


/*---------------------------------------------------------------------------*
  friend management object release function
  Arguments : None
  Return value: None
  Use : Clears to NULL a pointer to a friend management object
 *---------------------------------------------------------------------------*/
void DWCi_ShutdownFriend(void)
{

    stpFriendCnt = NULL;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for calling gpProcess()
  Arguments : None
  Return value: GP processing result type.
  Use : Calls gpProcess(). The number of calls to gpProcess are also counted.
 *---------------------------------------------------------------------------*/
static GPResult DWCi_GPProcess(void)
{
    GPResult result = GP_NO_ERROR;

    if (OS_TicksToMilliSeconds(OS_GetTick()-stpFriendCnt->lastGpProcess) >= DWC_GP_PROCESS_INTERVAL)
    {
        stpFriendCnt->gpProcessCount++; // No measures taken since this does not loop for about 40 years.

#if defined(DWC_ENABLE_KEEPALIVE_TO_GPSERVER)
        if ((stpFriendCnt->gpProcessCount % DWC_GP_KEEP_ALIVE_COUNT) == 0)
        {
            // Call a function in place of keepalive periodically so that GP sessions are not cut off
            //  
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "GP:keep alive\n");
                
            (void)gpSetInfoi(stpFriendCnt->pGpObj, GP_INTERESTS1, 0);
        }
#endif
        result = gpProcess(stpFriendCnt->pGpObj);

        stpFriendCnt->lastGpProcess = OS_GetTick();
    }

    return result;
}


/*---------------------------------------------------------------------------*
  friend management process end function
  Arguments : None
  Return value: None
  application: initializes a friend management process object, and ends the friend management process
 *---------------------------------------------------------------------------*/
static void DWCi_CloseFriendProcess(void)
{

    if (!stpFriendCnt) return;

#if 0
    CloseStatsConnection();
#endif
    
    // Friend management object initialization
    stpFriendCnt->state = DWC_FRIEND_STATE_INIT;

    // [arakit] main 051027
    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_WAIT;
    stpFriendCnt->svUpdateComplete = 0;
    // [arakit] main 051027
}


/*---------------------------------------------------------------------------*
  Friend registration update function
  Arguments: friendlist : Friend roster
          friendListLen : Number of entries in the friend roster to be updated
  Return value: None
  Use : Adds/removes friends by accessing the friend roster.
          Since DWC_GetUserData() is used,
          code must not be changed so that it is called before DWC_InitFriendsMatch().
 *---------------------------------------------------------------------------*/
static void DWCi_UpdateFriendReq(DWCFriendData friendList[], int friendListLen)
{
    char userName[21];
    int numBuddy;
    int profileID;
    int i, j;
    GPBuddyStatus status;
    GPResult gpResult;

    // [todo]
    // What should I do with the error processing here?
    // Functions which only look at the friend roster only have parameter errors

    if (stpFriendCnt->buddyUpdateState == DWC_BUDDY_UPDATE_STATE_WAIT){
        // This is performed only once when processing the removal of a friend from the GP friend roster
        // Gets the number of friends
        gpResult = gpGetNumBuddies(stpFriendCnt->pGpObj, &numBuddy);
        (void)DWCi_HandleGPError(gpResult);

        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "gpGetNumBuddies -> %d\n", numBuddy);
    
        // Removes friends
        for (i = 0; i < numBuddy; i++){
            gpResult = gpGetBuddyStatus(stpFriendCnt->pGpObj, i, &status);
            (void)DWCi_HandleGPError(gpResult);
    
            for (j = 0; j < friendListLen; j++){
                if (DWCi_GetProfileIDFromList(j) == status.profile){
                    // If the same profile ID is in both rosters, this is already a friend
                    if (!DWC_IsBuddyFriendData(&friendList[j])){
                        // If the friendship established flag has not been set,
                        // the flag is set
                        // because the previous flag may not have been saved due to a power cut-off before it could be saved.
                        DWC_SetGsProfileId(&friendList[j], status.profile);
                        DWCi_SetBuddyFriendData(&friendList[j]);

                        stpFriendCnt->friendListChanged = 1;  // Friend roster has changed
                    }
                    break;
                }
            }
    
            if (j == friendListLen){
                // Friendship is cancelled when profile IDs
                // are found on the buddy roster but not on the friend roster.
                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Deleted buddy %u\n", status.profile);
                           
                gpResult = gpDeleteBuddy(stpFriendCnt->pGpObj, status.profile);
                (void)DWCi_HandleGPError(gpResult);
    
                numBuddy--;
                i--;
            }
        }

        stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
    }

    
    // Adds friends
    for ( ; stpFriendCnt->buddyUpdateIdx < friendListLen; stpFriendCnt->buddyUpdateIdx++){
        if ((profileID = DWCi_GetProfileIDFromList(stpFriendCnt->buddyUpdateIdx))){
            // When the profile ID for the friend roster is enabled

            // If the same profile ID already exists and has been removed, the function does nothing.
            if (DWCi_RefreshFriendListForth(friendList, stpFriendCnt->buddyUpdateIdx, profileID))
                continue;
            
            // If the establish friend flag has already been set, and there are no problems being friends,
            // the function does nothing because if not present on the GP friend roster
            // it's probably because the friend roster has not been downloaded.
            //if (DWC_IsBuddyFriendData(&friendList[stpFriendCnt->buddyUpdateIdx]))
            //    continue;
            
            gpResult = gpGetBuddyIndex(stpFriendCnt->pGpObj, profileID, &i);
            (void)DWCi_HandleGPError(gpResult);
        
            if (i == -1){
                // If the profile ID has not yet been registered on the friend roster
                // a request to add the friend will be issued.
                (void)DWCi_GPSendBuddyRequest(profileID);

#ifndef SDK_FINALROM    
                if (DWC_IsBuddyFriendData(&friendList[stpFriendCnt->buddyUpdateIdx])){
                    DWC_Printf(DWC_REPORTFLAG_DEBUG, "Local only friend\n");
                }
#endif
            }
            // If the friend roster is already registered and the friend code remains unchanged,
            // this is processed during the first friend deletion loop
        }
        else if (DWC_GetGsProfileId(DWCi_GetUserData(), &friendList[stpFriendCnt->buddyUpdateIdx]) == -1){
            // If the login ID has been set on the friend roster,
            // because it is necessary to get the profile ID by querying the GP,
            // the necessary data is passed to param and an asynchronous function is called.
            // Processing to follow is handled within callback functions
            // and the last name of the GP profile is retrieved from the login ID.
            DWC_LoginIdToUserName(DWCi_GetUserData(),
                                  &friendList[stpFriendCnt->buddyUpdateIdx],
                                  userName);
            // Searches for friends by last name
            gpProfileSearch(stpFriendCnt->pGpObj,
                            NULL,
                            NULL,
                            NULL,
                            NULL,
                            userName,
                            NULL,
                            GP_NON_BLOCKING,
                            (GPCallback)DWCi_GPProfileSearchCallback,
                            (void *)stpFriendCnt->buddyUpdateIdx);

            DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Called gpProfileSearch().\n");

            stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_PSEARCH;
            break;
        }
    }

    return;
}


/*---------------------------------------------------------------------------*
  friend list synchronization end function
  Arguments : None
  Return value: None
  application: This function is used for ending friend list synchronization
 *---------------------------------------------------------------------------*/
static void DWCi_EndUpdateServers(void)
{

    // Calls the callback
    stpFriendCnt->updateCallback(DWC_ERROR_NONE,
                                 stpFriendCnt->friendListChanged,
                                 stpFriendCnt->updateParam);

    stpFriendCnt->state = DWC_FRIEND_STATE_LOGON;
}


/*---------------------------------------------------------------------------*
  Function for removing friend data from the friend roster (includes callback function)
  Arguments: friendlist : Friend roster
          deleteIndex: the index for the friend data to be deleted
          otherIndex : Friend roster index to also be passed to the callback
  Return value: None
  application: Removes friend data from the local friend roster and
          calls the registered callback function for deleting that data from the game.
          Always call this function when making deletions on the game side.
 *---------------------------------------------------------------------------*/
static void DWCi_DeleteFriendFromList(DWCFriendData friendData[], int deleteIndex, int otherIndex)
{

    if (stpFriendCnt){
        // friend data cleared
        MI_CpuClear8(&friendData[deleteIndex], sizeof(DWCFriendData));
        
        if (stpFriendCnt->deleteCallback){
            // Calls the callback function for deleting friend data as registered on the game side.
            stpFriendCnt->deleteCallback(deleteIndex, otherIndex,
                                         stpFriendCnt->deleteParam);
        }
    }
}


/*---------------------------------------------------------------------------*
  Function for organizing the friend roster
  Arguments: friendList : Pointer to the friend roster
          index : Friend roster index to check for redundant friend registrations
          profileID : The profile ID to check for redundant friend registrations
  Return value: TRUE : Friend roster contents have changed, FALSE: Friend roster contents have not changed
  Use : For the friend having the specified profile ID on the specified friend roster index,
          investigates whether there is a duplicate profile ID in an index
          before the specified index, and if there is, deletes its own friend data.
          However, friendship has been established with the specified friend data,
          friend data in the previous index is removed
          only when the person associated with the friend data has not already been established as a friend.
 *---------------------------------------------------------------------------*/
static BOOL DWCi_RefreshFriendListForth(DWCFriendData friendList[], int index, int profileID)
{
    int pid_i;
    int i;

    for (i = 0; i < index; i++){
        if ((pid_i = DWCi_GetProfileIDFromList(i)) && (pid_i == profileID)){
            // deletes if a duplicate is found
            DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                       "Found same friend in the list [%d] & [%d], %d\n",
                       i, index, DWC_GetFriendDataType(&friendList[i]));

            if (DWC_IsBuddyFriendData(&friendList[index]) &&
                !DWC_IsBuddyFriendData(&friendList[i])){
                // The established friend flag has been set for the friend data to be removed,
                // however only original data for which the established friend flag
                // has not been set can be removed.
                DWCi_DeleteFriendFromList(friendList, i, index);
            }
            else {
                // Usually original data is left alone, and only data with a specified index is deleted.
                DWCi_DeleteFriendFromList(friendList, index, i);
            }

            stpFriendCnt->friendListChanged = 1;  // Friend roster has changed
            return TRUE;
        }
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  Function for organizing the friend roster (overall survey version)
  Arguments: friendList : Pointer to the friend roster
          friendListLen : Maximum length of the friend list (number of entries)
          profileID : Profile ID whose index is to be looked up
  Return value : Index of the argument Profile ID. Returns -1 when the Profile ID is not on the friend roster.
  application: Looks through the entire friend roster and removes a duplicate Profile ID.
          Returns which index location the argument Profile ID ultimately remains in.
 *---------------------------------------------------------------------------*/
static int DWCi_RefreshFriendListAll(DWCFriendData friendList[], int friendListLen, int profileID)
{
    int  index = -1;
    int  pid_i;
    int  i, j;

    for (i = 0; i < friendListLen; i++){
        pid_i = DWCi_GetProfileIDFromList(i);
        if (pid_i == 0) continue;

        // The indices of ultimately remaining Profile IDs are backed up
        if (pid_i == profileID) index = i;

        for (j = i+1; j < friendListLen; j++){
            if (pid_i == DWCi_GetProfileIDFromList(j)){
                // What to do if a duplicate Profile ID is found
                if ((DWC_GetFriendDataType(&friendList[i]) == DWC_FRIENDDATA_FRIEND_KEY) &&
                    (DWC_GetFriendDataType(&friendList[j]) == DWC_FRIENDDATA_GS_PROFILE_ID)){
                    // When i is the friend registration key and j is the Profile ID
                    // first make the friend data match the status of the Profile ID
                    DWC_SetGsProfileId(&friendList[i], pid_i);
                }

                if (DWC_IsBuddyFriendData(&friendList[j])){
                    // If j is not already established as friend, then friend data i is also made already established as friend
                    DWCi_SetBuddyFriendData(&friendList[i]);
                }

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Found same friend in the list [%d] & [%d], %d\n",
                           i, j, DWC_GetFriendDataType(&friendList[i]));

                // delete j's friend data
                DWCi_DeleteFriendFromList(friendList, j, i);

                stpFriendCnt->friendListChanged = 1;  // Friend roster has changed
            }
        }
    }

    return index;
}


/*---------------------------------------------------------------------------*
  Function for sending a friend roster
  Argument: profileID : The profile ID for which a friend request is to be sent
  Return value: GP processing result type.
  Use : Sends a friend request. Error processing is handled internally.
 *---------------------------------------------------------------------------*/
static GPResult DWCi_GPSendBuddyRequest(int profileID)
{
    GPResult gpResult;

    gpResult = gpSendBuddyRequest(stpFriendCnt->pGpObj,
                                  profileID,
                                  ""/*(gsi_char *)stpFriendCnt->playerName*/);
    (void)DWCi_HandleGPError(gpResult);

    DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
               "Send buddy request to %u\n",
               profileID);

    return gpResult;
}


/*---------------------------------------------------------------------------*
  Function for getting a friend's BuddyStatus
  arguments: friendData: pointer to friend data
          status : Pointer to the location storing BuddyStatus
  Return value: TRUE: Succeeded in retrieving buddy status, FALSE: Failed to retrieve buddy status
  Use : Gets the BuddyStatus of a friend by specifying that friend's index on the friend roster
 *---------------------------------------------------------------------------*/
static BOOL DWCi_GetFriendBuddyStatus(const DWCFriendData* friendData, GPBuddyStatus* status)
{
    u8  ret;
    int buddyIdx = 0;
    int profileid;

    SDK_ASSERT(friendData);
    SDK_ASSERT(status);

    if (!stpFriendCnt || !DWCi_CheckLogin()) return FALSE;

    profileid = DWC_GetGsProfileId(DWCi_GetUserData(), friendData);
    if (profileid > 0 && gpGetBuddyIndex(stpFriendCnt->pGpObj,
                                         profileid,
                                         &buddyIdx)){
        // Since only parameter errors result, errors can be handled offline
        ret = FALSE;
    }
    else if (profileid <= 0 || buddyIdx == -1){
        // Cases where a friend is not registered on the friend roster can also be handled offline.
        // Control arrives here if this function is called inside the login callback function or if not all friend rosters can be retrieved yet.
        //   
        ret = FALSE;
    }
    else {
        // Gets buddy data
        if (gpGetBuddyStatus(stpFriendCnt->pGpObj, buddyIdx, status)){
            // Since only parameter errors result, errors can be handled offline
            ret = FALSE;
        }
        else {
            ret = TRUE;
        }
    }

    return ret;
}


/*---------------------------------------------------------------------------*
  Error processing function for GPResult
  Argument : result : The type of result due to GP processing
  Return value: The type of result due to GP processing (the argument is returned unchanged)
  Use : Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) corresponding to the value of GPResult
           
 *---------------------------------------------------------------------------*/
static GPResult DWCi_HandleGPError(GPResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GP_NO_ERROR) return GP_NO_ERROR;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Friend, GP error %d\n", result);

    switch (result){
    case GP_MEMORY_ERROR:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GP_PARAMETER_ERROR:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    case GP_NETWORK_ERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GP_SERVER_ERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    // Base code added
    errorCode += DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_GP;

    // A function that halts friendship management is called
    DWCi_StopFriendProcess(dwcError, errorCode);

    return result;
}


/*---------------------------------------------------------------------------*
  Persistent error processing function
  Argument: error : Persistent error type
  Return value: Persistent error type (the argument is returned unchanged)
  Use : Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) corresponding to the value of the Persistent error
           
 *---------------------------------------------------------------------------*/
static int DWCi_HandlePersError(int error)
{
    int errorCode;
    DWCError dwcError;

    if (error == GE_NOERROR) return GE_NOERROR;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Friend, persistent error %d\n", error);

    switch (error){
    case GE_NOSOCKET:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case GE_NODNS:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case GE_NOCONNECT:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    case GE_DATAERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DATA;
        break;
    case GE_TIMEDOUT:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_TIMEOUT;
        break;
    }
    // GE_CONNECTING probably will not result since InitStatsAsync() is not being used

#if 0
    // Base code added
    errorCode += DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS;

    // A function that halts friendship management is called
    DWCi_StopFriendProcess(dwcError, errorCode);

#else
    // Base code added
    errorCode += DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_PERS;

    if (stPersState == DWC_PERS_STATE_LOGIN){
        DWCi_StopPersLogin(dwcError, errorCode);
    }
#endif
        
    return error;
}


/*---------------------------------------------------------------------------*
  GP profile search complete callback function
  Arguments: connection : Pointer to the GP connection object
          arg : Pointer to a GPProfileSearchResponseArg type object
          param : Parameters for the callback
  Return value: None
  Use : GP profile search complete callback
 *---------------------------------------------------------------------------*/
static void DWCi_GPProfileSearchCallback(GPConnection* connection,
                                         GPProfileSearchResponseArg* arg,
                                         void* param)
{
    int idx;
    GPResult gpResult;
    int friendIdx = (int)param;

    DWC_Printf( DWC_REPORTFLAG_DEBUG, "ProfileSearch: num:%d m:%x\n", arg->numMatches, arg->more );

    if ((arg->result == GP_NO_ERROR) && arg->numMatches &&
        (DWC_GetFriendDataType(&stpFriendCnt->friendList[friendIdx]) != DWC_FRIENDDATA_NODATA)){
        int match_index;
        // Profile found
        if (arg->numMatches > 1){
            // If two or more matching profiles are somehow found,
            // no special measures are taken because this is exceedingly rare
            DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Happned to find %d profiles.\n",
                       arg->numMatches);
        }

        if (stpFriendCnt->state == DWC_FRIEND_STATE_PERS_LOGIN){
            stpFriendCnt->friendListChanged = 1;  // Friend roster has changed

            // All of the found profiles are checked.
            for ( match_index = 0 ; match_index < arg->numMatches ; ++match_index )
            {
                // If an already existing Profile ID is found it is quickly returned.
                // Check for Profile ID duplicate
                if (DWCi_RefreshFriendListForth(stpFriendCnt->friendList,
                                                friendIdx,
                                                arg->matches[match_index].profile)){
                    // Since newly retrieved Profile IDs are already registered,
                    // the code handles what to do when they have been deleted.
                    // Proceeds through the friend roster index being processed and synchronous processing continues
                    stpFriendCnt->buddyUpdateIdx++;
                    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
                    arg->more = GP_DONE; // End
                    return;
                }
            }

            // All of the found profiles are checked.
            for ( match_index = 0 ; match_index < arg->numMatches ; ++match_index )
            {
                gpResult = gpGetBuddyIndex(connection, arg->matches[match_index].profile, &idx);
                (void)DWCi_HandleGPError(gpResult);

                if (idx == -1){
                    // A friend request is issued if the profile ID has not yet been registered on the friend roster
                    //  
                    (void)DWCi_GPSendBuddyRequest(arg->matches[match_index].profile);
                }
                else {
                    // If the Profile ID has already been registered on the friend roster,
                    // the Profile ID is registered and the friend established flag is set
                    //  
                    //  
                    DWC_SetGsProfileId(&stpFriendCnt->friendList[friendIdx],
                                       arg->matches->profile);
                    DWCi_SetBuddyFriendData(&stpFriendCnt->friendList[friendIdx]);
                    // Calls the callback for establishing friends
                    DWCi_CallBuddyFriendCallback(friendIdx);

                    stpFriendCnt->buddyUpdateIdx++;
                    stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
                    arg->more = GP_DONE; // End
                    return;
                }
            }

            if ( arg->more != GP_MORE )
            {
                // Since friend roster data has been received, flow returns one time to receive the rest
                // Proceeds through the friend roster index being processed and synchronous processing continues
                stpFriendCnt->buddyUpdateIdx++;
                stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
            }
            else
            {
                DWC_Printf(DWC_REPORTFLAG_DEBUG, "more profiles will come...\n");
            }
        }
    }
    else if (arg->result){
        // Error occurred
        if (DWCi_HandleGPError(arg->result)) return;
    }
    else if ((stpFriendCnt->state == DWC_FRIEND_STATE_PERS_LOGIN) ||
             (DWC_GetFriendDataType(&stpFriendCnt->friendList[friendIdx]) == DWC_FRIENDDATA_NODATA)){
        // Even if the profile in question could not be found,
        // flow proceeds through the friend roster index being processed and synchronous processing continues.
        // Control flow arrives here before ProfileSearchCallback is called, even if friend data has been deleted within the GetInfo callback.
        // even if friend data has been deleted within the GetInfo callback.
        stpFriendCnt->buddyUpdateIdx++;
        stpFriendCnt->buddyUpdateState = DWC_BUDDY_UPDATE_STATE_CHECK;
    }
}


/*---------------------------------------------------------------------------*
  Callback function for receiving GPInfo (used when receiving a friend roster)
  Arguments: connection : Pointer to the GP connection object
          arg : Pointer to a GPGetInfoResponseArg type object
          param : Parameters for the callback
  Return value: None
  Use : Callback for receiving GP info, used when receiving a friend roster
          Since DWC_GetUserData() is used,
          code must not be changed so that it is called before DWC_InitFriendsMatch().
 *---------------------------------------------------------------------------*/
static void DWCi_GPGetInfoCallback_RecvBuddyRequest(GPConnection* connection,
                                                    GPGetInfoResponseArg* arg,
                                                    void* param)
{
#pragma unused(param)
    char userName[21];
    GPResult gpResult;
    int i;
    BOOL confirm = FALSE;

    // [arakit] main 051013
    if (arg->result != GP_NO_ERROR)
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, " ERROR: GetInfo Req. why??? : %d\n", arg->result );
        return;
    }
    // [arakit] main 051013

    // [arakit] main 051008
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "GetInfo Req: profileID %u, lastname '%s'.\n",
               arg->profile, arg->lastname);

    for (i = 0; i < stpFriendCnt->friendListLen; i++)
    {
        if ( DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_LOGIN_ID )
        {
            DWC_LoginIdToUserName(DWCi_GetUserData(),
                                  &stpFriendCnt->friendList[i],
                                  userName);

            if (strcmp(userName, arg->lastname) == 0)
            {
                // Authenticates the friend roster
                gpResult = gpAuthBuddyRequest(connection, arg->profile);
                // The friend data is moved from the Login ID to the Profile ID
                DWC_SetGsProfileId(&stpFriendCnt->friendList[i], arg->profile);

                confirm = TRUE;

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Auth buddy request from %u, friend[%d]mp.\n",
                           arg->profile, i);
            }
        }
        else if ( DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_GS_PROFILE_ID ||
                  DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_FRIEND_KEY )
        {
            // When using a Profile ID base
            if ( DWC_GetGsProfileId( DWCi_GetUserData(), &stpFriendCnt->friendList[i] ) == arg->profile )
            {
                // Authenticates the friend roster
                gpResult = gpAuthBuddyRequest(connection, arg->profile);

                confirm = TRUE;

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Auth buddy request from %u, friend[%d]gs.\n",
                           arg->profile, i);
            }
        }
    }

    if ( confirm )
    {
        // Send a friend request even from here.
        // Since it's a problem if the other party does not receive the friend roster when online,
        // it must be sent even if it's excessive.
        (void)DWCi_GPSendBuddyRequest(arg->profile);
    }
    else {
        // Deny requests if the friend request is from a friend not on the friend list
        gpDenyBuddyRequest(connection, arg->profile);

        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Denied buddy request from %u.\n",
                   arg->profile);
    }
}


/*---------------------------------------------------------------------------*
  Callback function for receiving GPInfo (used when receiving a friend authentication callback)
  Arguments: connection : Pointer to the GP connection object
          arg : Pointer to a GPGetInfoResponseArg type object
          param : Parameters for the callback
  Return value: None
  Use : Callback for receiving GPInfo, used when receiving a friend authentication callback
          Since DWC_GetUserData() is used,
          code must not be changed so that it is called before DWC_InitFriendsMatch().
 *---------------------------------------------------------------------------*/
static void DWCi_GPGetInfoCallback_RecvAuthMessage(GPConnection* connection,
                                                   GPGetInfoResponseArg* arg,
                                                   void* param)
{
#pragma unused(connection)
#pragma unused(param)
    char userName[21];
    int index;
    int i;
    BOOL confirm = FALSE;
	BOOL shdcallback = TRUE;

    // [arakit] main 051013
    if (arg->result != GP_NO_ERROR)
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, " ERROR: GetInfo Auth. why??? : %d\n", arg->result );
        return;
    }
    // [arakit] main 051013

    // [arakit] main 051008
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "GetInfo Auth: profileID %u, lastname '%s'.\n",
               arg->profile, arg->lastname);

    // Searches through the friend roster and compares with temporary Login IDs
    for (i = 0; i < stpFriendCnt->friendListLen; i++)
    {
        if ( DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_LOGIN_ID )
        {
            // What to do with a temporary ID
            DWC_LoginIdToUserName(DWCi_GetUserData(),
                                  &stpFriendCnt->friendList[i],
                                  userName);

            if (strcmp(userName, arg->lastname) == 0)
            {
                // The friend roster is moved from the Login ID to the Profile ID
                DWC_SetGsProfileId(&stpFriendCnt->friendList[i], arg->profile);
                DWCi_SetBuddyFriendData(&stpFriendCnt->friendList[i]);

                // Establish friendship
                confirm = TRUE;

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Established buddy with %u, friend[%d]mp.\n",
                           arg->profile, i);

            }
        }
        else if ( DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_GS_PROFILE_ID ||
                  DWC_GetFriendDataType( &stpFriendCnt->friendList[i] ) == DWC_FRIENDDATA_FRIEND_KEY )
        {
			if( DWC_IsBuddyFriendData( &stpFriendCnt->friendList[i] ) == TRUE ) {
				DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "This profile is already my buddy.\n");
				shdcallback = FALSE;
				continue;
			}
			
            // When using a Profile ID base
            if ( DWC_GetGsProfileId( DWCi_GetUserData(), &stpFriendCnt->friendList[i] ) == arg->profile )
            {
                // The friend roster is moved from the Login ID to the Profile ID
                DWC_SetGsProfileId(&stpFriendCnt->friendList[i], arg->profile);
                DWCi_SetBuddyFriendData(&stpFriendCnt->friendList[i]);

                // Establish friendship
                confirm = TRUE;

                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Established buddy with %u, friend[%d]gs.\n",
                           arg->profile, i);
            }
        }
    }

    if ( confirm )
    {
        // Find and remove duplicate Profile IDs
        index = DWCi_RefreshFriendListAll(stpFriendCnt->friendList,
                                          stpFriendCnt->friendListLen,
                                          arg->profile);

        // Calls the callback for establishing friends
        if( shdcallback )
			DWCi_CallBuddyFriendCallback(index);

        stpFriendCnt->friendListChanged = 1;  // Friend roster has changed
    }
    else
    {
        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Not Established buddy with %u.\n",
                   arg->profile);
    }
}


/*---------------------------------------------------------------------------*
  Function to end Persistent server login.
  Arguments: error: DWC error type
          errorCode : Error code
  Return value: None
  Use : Processing is halted if an error occurs during Persistent server login processing
 *---------------------------------------------------------------------------*/
static void DWCi_StopPersLogin(DWCError error, int errorCode)
{

    if (!stpFriendCnt || (error == DWC_ERROR_NONE)) return;

    DWCi_SetError(error, errorCode);

    // Logs out from the Persistent server
    DWC_LogoutFromStorageServer();

    if (stpFriendCnt->persLoginCallback != NULL){
        stpFriendCnt->persLoginCallback(error, stpFriendCnt->persLoginParam);
    }
}


/*---------------------------------------------------------------------------*
  Persistent server save function
  Arguments : profileID : Profile ID to be saved
          type : Data access type
          Arguments : keyvalues : Pointer to the data base string (key/value value)
          param : Parameters for the callback
  Return value: None
  Use : Saves data on the Persistent server.
 *---------------------------------------------------------------------------*/
static void DWCi_SetPersistDataValuesAsync(int profileID,
                                           persisttype_t type,
                                           gsi_char* keyvalues,
                                           void* param)
{

    DWCi_AddPersCallbackLevel();

    SetPersistDataValues(0, profileID, type, 0, keyvalues,
                         DWCi_PersDataSaveCallback, param);
}


/*---------------------------------------------------------------------------*
  Persistent server load function
  Arguments : profileID : Profile ID to be loaded
          type : Data access type
          Arguments : keys : Pointer to the data base string (key value)
          param : Parameters for the callback
  Return value: None
  Use : Loads data from the Persistent server.
 *---------------------------------------------------------------------------*/
static void DWCi_GetPersistDataValuesAsync(int profileID,
                                           persisttype_t type,
                                           gsi_char* keys,
                                           void* param)
{

    DWCi_AddPersCallbackLevel();

    GetPersistDataValues(0, profileID, type, 0, keys,
                         DWCi_PersDataCallback, param);
}


/*---------------------------------------------------------------------------*
  Persistent Server Authentication Callback Function
  Arguments: localid : Local ID of the user for whom authentication is being performed
          profileid : ProfileID of the user for whom authentication is being performed
          authenticated: TRUE : Authentication succeeded, FALSE : Authentication failed
          errmsg : Error message used when authentication fails
          instance : Parameter for the callback
  Return value: None
  Use : Persistent server authentication callback
 *---------------------------------------------------------------------------*/
static void DWCi_PersAuthCallback(int localid, int profileid,
                                  int authenticated, gsi_char* errmsg,
                                  void* instance)
{
#ifdef SDK_FINALROM
#pragma unused(errmsg)
#endif
#pragma unused(instance)
#if 0
    char tmpStr[DWC_MAX_PLAYER_NAME*2];
#endif

    DWCi_SubPersCallbackLevel();
    
    if (!authenticated || (localid != 0)){
        // Authentication failed
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Stats server authentication failed.\n");
        DWC_Printf(DWC_REPORTFLAG_ERROR, "%s\n", errmsg);

#if 0
        // A function that halts friendship management is called
        DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_AUTH);
#else
        DWCi_StopPersLogin(DWC_ERROR_NETWORK,
                           DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_AUTH);
#endif
        return;
    }
    else {
        // Authentication succeeded
        DWC_Printf(DWC_REPORTFLAG_UPDATE_SV, "Stats server authentication succeeded.\n");

        stpFriendCnt->profileID = profileid;

#if 0
        // Create a key for the player name
        (void)OS_SNPrintf(tmpStr, sizeof(tmpStr),
                          "\\dwc_name\\%s",
                          stpFriendCnt->playerName);
        // Save the player name on the Persistent server
        DWCi_SetPersistDataValuesAsync(stpFriendCnt->profileID, pd_public_rw,
                                       tmpStr,
                                       (void *)DWC_PERS_KEY_ID_PLAYER_NAME);

        // Keep the Persistent server login status unchanged until registration of the player name key is finished
        // Keep login status unchanged

#else
        stPersState = DWC_PERS_STATE_CONNECTED;

        // Calls the callback
        if (stpFriendCnt->persLoginCallback != NULL){
            stpFriendCnt->persLoginCallback(DWC_ERROR_NONE, stpFriendCnt->persLoginParam);
        }
#endif
    }
}


/*---------------------------------------------------------------------------*
  Callback for retrieving Persistent data
  Arguments: localid : Local ID of the user for whom data is stored
          profileid : Profile ID of the user for whom data is stored
          type : Access type
          index : Index within key
          success : TRUE : Retrieval succeeded, FALSE : Retrieval failed
          modified : Time of last modification to data
          data : Retrieved data
          len : Length of retrieved data
          instance : Parameter for the callback
  Return value: None
  Use : Callback for getting persistent data
 *---------------------------------------------------------------------------*/
static void DWCi_PersDataCallback(int localid, int profileid,
                                  persisttype_t type, int index, int success,
                                  time_t modified, char* data, int len,
                                  void* instance)
{
#pragma unused(localid)
#pragma unused(type)
#pragma unused(index)
#pragma unused(modified)
    BOOL result;

    DWCi_SubPersCallbackLevel();

    if (success){
        if (len == 0){
            // What to do when the key does not exist
            // or when trying to load another person's Private key.
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                       "Persitent, Specified key is not exist.\n");
            // Return a failure callback
            result = FALSE;
        }
        else {
            // What to do when the key does exist
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Loaded data from persistent server. -> %s\n", data);
            result = TRUE;
        }

        if (stpFriendCnt->loadCallback){
            // Calls the callback registered by the user
            stpFriendCnt->loadCallback(result, DWCi_GetFriendListIndex(profileid), data, len, instance);
        }
    }
    else {
        // Error occurred
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Failed to load persistent data.\n");

#if 0
        // A function that halts friendship management is called
        DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_LOAD);
        return;
#endif

        // Calls the callback registered by the user
        if (stpFriendCnt->loadCallback){
            stpFriendCnt->loadCallback(FALSE, DWCi_GetFriendListIndex(profileid), data, len, instance);
        }
    }
}


/*---------------------------------------------------------------------------*
  Persistent data save callback function
  Arguments: localid : Local ID of the user for whom data has been saved
          profileid : Profile ID of the user for whom data has been saved
          type : Access type
          index : Index within key
          success : TRUE : Save succeeded, FALSE : Save failed
          modified : Time data was last saved
          instance : Parameter for the callback
  Return value: None
  Use : Persistent data save callback
 *---------------------------------------------------------------------------*/
static void DWCi_PersDataSaveCallback(int localid, int profileid,
                                      persisttype_t type, int index, int success,
                                      time_t modified, void* instance)
{
#pragma unused(localid)
#pragma unused(profileid)
#pragma unused(index)
#pragma unused(modified)
    u32  flag = (u32)instance;
    BOOL isPublic;

    DWCi_SubPersCallbackLevel();

#if 0
    if (flag == DWC_PERS_KEY_ID_PLAYER_NAME){
        if (success){
            // Checks if this really is the dwc_name save call back
            if (stpFriendCnt->state == DWC_FRIEND_STATE_PERS_LOGIN){
                // Player name saved
                DWC_Printf(DWC_REPORTFLAG_UPDATE_SV,
                           "Saved player name to persistent server.\n");

                // Records that Persistent processing has ended
                stpFriendCnt->svUpdateComplete++;
            }
            else {
                // Sometimes parameters only came together
                flag = 0;
            }
        }
        else {
            // Error occurred
            DWC_Printf(DWC_REPORTFLAG_ERROR, "Failed to save persistent data.\n");

            // A function that halts friendship management is called
            DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                                   DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_SAVE);
        }
    }

    // When saving data under DWC control, processing ends here
    if (flag & DWC_PERS_KEY_ID_MASK) return;
#endif

    // The part to follow is when saving from the game
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "Saved data to persistent server.\n");

    if ((type == pd_public_ro) || (type == pd_public_rw)){
        isPublic = TRUE;
    }
    else {
        isPublic = FALSE;
    }
        
    if (success){
        if (stpFriendCnt->saveCallback){
            // Calls the callback registered by the user
            stpFriendCnt->saveCallback(TRUE, isPublic, instance);
        }
    }
    else {
        // Error occurred
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Failed to save persistent data.\n");

#if 0
        // A function that halts friendship management is called
        DWCi_StopFriendProcess(DWC_ERROR_NETWORK,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_PERS+DWC_ECODE_TYPE_STATS_SAVE);
#endif

        // Calls the callback registered by the user
        if (stpFriendCnt->saveCallback){
            stpFriendCnt->saveCallback(FALSE, isPublic, instance);
        }
    }
}


/*---------------------------------------------------------------------------*
  Persistent callback level increment function
  Arguments : None
  Return value: None
  Use : Increments the callback level by 1 when asynchronous Persistent functions are called
 *---------------------------------------------------------------------------*/
static void DWCi_AddPersCallbackLevel(void)
{

    if (stpFriendCnt->persCallbackLevel == 0xffffffff){
        DWCi_StopFriendProcess(DWC_ERROR_FATAL,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_TYPE_UNEXPECTED);
        return;
    }
    stpFriendCnt->persCallbackLevel++;
}


/*---------------------------------------------------------------------------*
  Persistent callback level decrement function
  Arguments : None
  Return value: None
  Use : Decrements the callback level by 1 when asynchronous Persistent functions are called
           
 *---------------------------------------------------------------------------*/
static void DWCi_SubPersCallbackLevel(void)
{

    if (stpFriendCnt->persCallbackLevel == 0){
        DWCi_StopFriendProcess(DWC_ERROR_FATAL,
                               DWC_ECODE_SEQ_FRIEND+DWC_ECODE_TYPE_UNEXPECTED);
        return;
    }
    stpFriendCnt->persCallbackLevel--;
}


/*---------------------------------------------------------------------------*
  Function for getting the Persistent callback level
  Arguments : None
  Return value : Persistent callback level
  Use : Gets the number of pending Persistent callback functions that have been called
 *---------------------------------------------------------------------------*/
static u32 DWCi_GetPersCallbackLevel(void)
{

    return stpFriendCnt->persCallbackLevel;
}
