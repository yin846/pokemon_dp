#include <nitro.h>
#include <base/dwc_base_gamespy.h>
#include <available.h>
#include <natneg/natneg.h>
#include <gstats/gpersist.h>

#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_connectinet.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_common.h>
#include <base/dwc_main.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
// Pointer to FriendsMatch control object
static DWCFriendsMatchControl* stpDwcCnt = NULL;

static int stLastSocketError = 0;  // Last socket error number generated

// gt2 connection list
// [todo]
// Along with InfoList below, code will be changed so that it maintains a list only of the size specified by the game
static GT2Connection stGt2ConnectionList[DWC_MAX_CONNECTIONS];
// gt2 connection data list
static DWCConnectionInfo stConnectionInfoList[DWC_MAX_CONNECTIONS];


//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static BOOL DWCs_UpdateConnection( void );
static void DWCs_ForceShutdown( void );

static void DWCi_SetState(DWCState state);

static int  DWCi_DeleteAID(u8 aid);
static u32  DWCi_GetAIDBitmapFromList(u8* aidList, int aidListLen);

static GPResult DWCi_HandleGPError(GPResult result);
static GT2Result DWCi_HandleGT2Error(GT2Result result);

static void DWCi_LoginCallback(DWCError error, int profileID, void *param);
static void DWCi_UpdateServersCallback(DWCError error, BOOL isChanged, void* param);
static void DWCi_MatchedCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int profileID, void* param);

static void DWCi_GPErrorCallback(GPConnection* pconnection, GPErrorArg* arg, void* param);
static void DWCi_GPRecvBuddyMessageCallback(GPConnection* pconnection, GPRecvBuddyMessageArg* arg, void* param);

static void DWCi_GT2ReceivedCallback(GT2Connection connection, GT2Byte* message, int len, GT2Bool reliable);
static void DWCi_GT2ClosedCallback(GT2Connection connection, GT2CloseReason reason);
static void DWCi_GT2PingCallback(GT2Connection connection, int latency);
static void DWCi_GT2SocketErrorCallback(GT2Socket socket);


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  FriendsMatch Library initialization function
  Argument :dwccnt   Pointer to FriendsMatch control object
          userdata: pointer to the user data object
          productID     Product ID received from GameSpy
          gameName      Game name received from GameSpy (NULL termination required)
          secretKey      Secret key received from GameSpy (NULL termination required)
          sendBufSize   Send buffer size used by DWC_Transport Uses anywhere from 0 to the default 8KB
          recvBufSize   Receive buffer size used by DWC_Transport Uses anywhere from 0 to the default 8KB
          friendList    Friend roster (NULL is OK if not used by the game)
          friendListLen : Maximum length of the friend list (number of entries)
  Return value: None
  Use   : Initializes the FriendsMatch library
 *---------------------------------------------------------------------------*/
void DWC_InitFriendsMatch(DWCFriendsMatchControl* dwccnt,
              DWCUserData* userdata,
              int productID,
              const char* gameName,
              const char* secretKey,
              int sendBufSize,
              int recvBufSize,
              DWCFriendData friendList[],
              int friendListLen)
{
    u32 cpySize;

    SDK_ASSERT(dwccnt);
    SDK_ASSERT(gameName);
    SDK_ASSERT(secretKey);
    SDK_ASSERT(friendListLen <= DWC_MAX_MATCH_IDX_LIST);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_InitFriendsMatch() was called!!\n");

    stpDwcCnt = dwccnt;  // Sets a pointer to the control structure

    DWC_ClearError();  // Clears errors up to now

    stpDwcCnt->gt2Socket              = NULL;
    stpDwcCnt->gt2Callbacks.connected = DWCi_GT2ConnectedCallback;  // Defined by dwc_match.c
    stpDwcCnt->gt2Callbacks.received  = DWCi_GT2ReceivedCallback;
    stpDwcCnt->gt2Callbacks.closed    = DWCi_GT2ClosedCallback;
    stpDwcCnt->gt2Callbacks.ping      = DWCi_GT2PingCallback;
    stpDwcCnt->gt2SendBufSize         = sendBufSize ? sendBufSize : 8*1024;
    stpDwcCnt->gt2RecvBufSize         = recvBufSize ? recvBufSize : 8*1024;

    stpDwcCnt->gpObj            = NULL;
    stpDwcCnt->userdata         = userdata;

    stpDwcCnt->state            = DWC_STATE_INIT;
    stpDwcCnt->lastState        = DWC_STATE_INIT;
    stpDwcCnt->aid              = 0;
    stpDwcCnt->ownCloseFlag     = FALSE;
    stpDwcCnt->profileID        = 0;
    stpDwcCnt->gameName         = gcd_gamename;    // Stores a pointer to the variable stats
    stpDwcCnt->secretKey        = gcd_secret_key;  // Stores a pointer to the variable stats

    stpDwcCnt->loginCallback         = NULL;
    stpDwcCnt->loginParam            = NULL;
    stpDwcCnt->updateServersCallback = NULL;
    stpDwcCnt->updateServersParam    = NULL;
    stpDwcCnt->matchedCallback       = NULL;
    stpDwcCnt->matchedParam          = NULL;
    stpDwcCnt->matchedSCCallback     = NULL;
    stpDwcCnt->matchedSCParam        = NULL;
    stpDwcCnt->closedCallback        = NULL;
    stpDwcCnt->closedParam           = NULL;

    // Initializes the gt2 connection list and g2 connection data list
    DWCi_ClearGT2ConnectionList();

    // Initialization of the login control structure
    DWCi_LoginInit(&stpDwcCnt->logcnt, userdata, &stpDwcCnt->gpObj, productID, userdata->gamecode, stpDwcCnt->playerName, DWCi_LoginCallback, NULL);

    // Initialization of the friend management structure
    DWCi_FriendInit(&stpDwcCnt->friendcnt, &stpDwcCnt->gpObj, stpDwcCnt->playerName, friendList, friendListLen);

    // Matchmaking control structure initialization
    DWCi_MatchInit(&stpDwcCnt->matchcnt, &stpDwcCnt->gpObj,
                   &stpDwcCnt->gt2Socket, &stpDwcCnt->gt2Callbacks,
                   gcd_gamename, gcd_secret_key,
                   friendList, friendListLen);

    // Transport control structure initialization
    DWCi_InitTransport(&stpDwcCnt->transinfo);

    // Set global variable for GameSpy persistent SDK (gcd_gamename, gcd_secret_key)
    cpySize = strlen(gameName) < sizeof(gcd_gamename) ? strlen(gameName) : sizeof(gcd_gamename)-1;
    MI_CpuCopy8(gameName, gcd_gamename, cpySize);
    gcd_gamename[cpySize] = '\0';
    
    cpySize = strlen(secretKey) < sizeof(gcd_secret_key) ? strlen(secretKey) : sizeof(gcd_secret_key)-1;
    MI_CpuCopy8(secretKey, gcd_secret_key, cpySize);
    gcd_secret_key[cpySize] = '\0';
}


/*---------------------------------------------------------------------------*
  FriendsMatch Library closing function
  Arguments : None
  Return value: None
  Use   : Exits the FriendsMatch library and frees GameSpySDK heap region
 *---------------------------------------------------------------------------*/
void DWC_ShutdownFriendsMatch(void)
{

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "!!DWC_ShutdownFriendsMatch() was called!! stpDwcCnt = 0x%x\n",
               stpDwcCnt);

    if (!stpDwcCnt) return;

    // GP status is put offline
    // ¨Sending close inside gpDestroy() (this is sent quickly) has the same effect
    //if (stpDwcCnt->gpObj){
    //    (void)DWCi_SetGPStatus(DWC_STATUS_OFFLINE, "", "");
    //    (void)gpProcess(&stpDwcCnt->gpObj);  // up to GP server
    //}

    // Release QR2 object
    // Since qr2_shutdown() will be sent, call it before gt2CloseSocket()
    if (stpDwcCnt->matchcnt.qr2Obj){
        qr2_shutdown(stpDwcCnt->matchcnt.qr2Obj);
        stpDwcCnt->matchcnt.qr2Obj = NULL;  // Must be cleared to null on its own!
    }
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    stpDwcCnt->matchcnt.qr2ShutdownFlag = 0;  // QR2 shutdown flag clear
#endif

    // Releases SB object
    if (stpDwcCnt->matchcnt.sbObj){
        ServerBrowserFree(stpDwcCnt->matchcnt.sbObj);
        stpDwcCnt->matchcnt.sbObj = NULL;  // Must be cleared to null on its own!
    }

    // Releases negotiation list
    NNFreeNegotiateList();

    CloseStatsConnection();  // Releases persistent objects

    // Releases GP objects
    // [todo]
    // Although this also exhibits problems when called from within a callback, it must be released
    if (stpDwcCnt->gpObj){
         // When WCM disconnects, a final send error is sometimes output, indicating that there is a memory leak.
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_ERROR, (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_MESSAGE, (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_REQUEST, (GPCallback)NULL, NULL);
        (void)gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_STATUS, (GPCallback)NULL, NULL);
        (void)gpProcess(&stpDwcCnt->gpObj); // Wipes away accumulated memory..
        gpDestroy(&stpDwcCnt->gpObj);
        stpDwcCnt->gpObj = NULL;
    }

    // Shutdown each module (mainly clears control objects)
    DWCi_ShutdownLogin();
    DWCi_ShutdownFriend();
    DWCi_ShutdownMatch();
    DWCi_ShutdownTransport();

    // Releases GT2 socket
    // [todo]
    // Checks if there are no objects and processing has halted when the function is called from inside a callback
    //  
    if (stpDwcCnt->gt2Socket){
        gt2CloseSocket(stpDwcCnt->gt2Socket);
        stpDwcCnt->gt2Socket = NULL;  // Must be cleared to null on its own!
    }

#ifdef GSI_MEM_MANAGED
    gsMemMgrDestroy();  // Releases GameSpy heap memory and heap manager
#endif

    stpDwcCnt = NULL;  // FriendsMatch control object clear
    
    // Initialize parameters
    //DWC_InitFriendsMatch(dwccnt, dwccnt->userdata, dwccnt->logcnt.productID, dwccnt->gameName,
    //                     dwccnt->secretKey, dwccnt->gt2SendBufSize,
    //                     dwccnt->gt2RecvBufSize, NULL, NULL);
}


/*---------------------------------------------------------------------------*
  FriendsMatch Library telecommunication update function
  Arguments : None
  Return value: None
  Use : Called every game frame, this function updates FriendsMatch Library communication
 *---------------------------------------------------------------------------*/
void DWC_ProcessFriendsMatch(void)
{
    GSIACResult acResult;
    GPResult gpResult;

    if ( DWCs_UpdateConnection() )
    {
        // DCF has been disconnected.
        DWCs_ForceShutdown();
    }

    if (!stpDwcCnt || (stpDwcCnt->state == DWC_STATE_INIT) || DWCi_IsError())
        return;

    switch (stpDwcCnt->state){
    case DWC_STATE_AVAILABLE_CHECK:  // During game availability status check
        acResult = GSIAvailableCheckThink();  // AvailableCheck update
        switch (acResult){
        case GSIACAvailable:    // Available.
            DWC_Printf(DWC_REPORTFLAG_ACHECK, "Confirmed the backend of GameSpy server.\n");
            // GP initialization
            gpResult = gpInitialize(&stpDwcCnt->gpObj, stpDwcCnt->logcnt.productID, 0);
            if (DWCi_HandleGPError(gpResult)) return;

#if 0       // On hold because this is untested
            // The GP cache is limited only to BUDDY
            gpResult = gpDisable(&stpDwcCnt->gpObj, GP_INFO_CACHING_BUDDY_ONLY); 
            if (DWCi_HandleGPError(gpResult)) return;
#endif
            
            // Set GP callback function
            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_ERROR,
                                     (GPCallback)DWCi_GPErrorCallback,
                                     NULL);
            if (DWCi_HandleGPError(gpResult)) return;

            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_MESSAGE,
                                     (GPCallback)DWCi_GPRecvBuddyMessageCallback,
                                     NULL);
            if (DWCi_HandleGPError(gpResult)) return;

            // DWCi_GPRecvBuddyRequestCallback() is defined in dwc_friend.c
            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_REQUEST,
                                     (GPCallback)DWCi_GPRecvBuddyRequestCallback,
                                     NULL);
            if (DWCi_HandleGPError(gpResult)) return;

            // DWCi_GPRecvBuddyStatusCallback() is defined in dwc_friend.c
            gpResult = gpSetCallback(&stpDwcCnt->gpObj, GP_RECV_BUDDY_STATUS,
                                     (GPCallback)DWCi_GPRecvBuddyStatusCallback,
                                     NULL);
            if (DWCi_HandleGPError(gpResult)) return;

            DWCi_SetState(DWC_STATE_LOGIN);

            // Calls an internal login function
            DWCi_LoginAsync();
            break;
        case GSIACUnavailable:  // Exit service
            // Exit login processing
            DWCi_StopLogin(DWC_ERROR_AUTH_OUT_OF_SERVICE, -20110); // Server exit error code: -20110
            return;
            break;
        case GSIACTemporarilyUnavailable:  // Temporarily unavailable
            // Exit login processing
            DWCi_StopLogin(DWC_ERROR_AUTH_STOP_SERVICE, -20101); // service temporarily unavailable error code: -20101
            return;
            break;
        default:
            break;
        }
        break;
        
    case DWC_STATE_LOGIN:  // Processing login
        DWCi_LoginProcess();
        break;

    case DWC_STATE_UPDATE_SERVERS:  // friend list synchronization in progress
    case DWC_STATE_ONLINE:          // Online
        DWCi_FriendProcess();
        DWCi_MatchProcess(FALSE);  // Call qr2_think(), gt2Think() here
        break;

    case DWC_STATE_MATCHING:  // matchmaking in progress
        DWCi_MatchProcess(TRUE);
        DWCi_FriendProcess();
        break;

    case DWC_STATE_CONNECTED:  // Connection completion status
		DWCi_TransportProcess();
        DWCi_FriendProcess();

        if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
            (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
            // matchmaking processing is performed when someone joins midway
            DWCi_MatchProcess(TRUE);
        }
        else if (stpDwcCnt->gt2Socket){
            // Call qr2_think(), gt2Think() here if other than the above
            DWCi_MatchProcess(FALSE);
        }
        break;

    default:
        break;
    }

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (stpDwcCnt->matchcnt.qr2ShutdownFlag == 1){
        // End QR2 if the QR2 shutdown flag has been set
        if (stpDwcCnt->matchcnt.qr2Obj != NULL){
            qr2_shutdown(stpDwcCnt->matchcnt.qr2Obj);
            stpDwcCnt->matchcnt.qr2Obj = NULL;  // Must be cleared to null on its own!
        }
        stpDwcCnt->matchcnt.qr2ShutdownFlag = 0;  // QR2 shutdown flag clear
    }
#endif
}


/*---------------------------------------------------------------------------*
  Wi-Fi connection login function
  Argument@Fingamesn  screen name within the game
          reserved past usage. returns NULL.
          callback:   callback function for notification of login completion
          param : Parameters for the callback
  Return value: If function call completed, TRUE. Returns callback based on success or failure.
          FALSE: Not a state in which this function should be called. Callback does not return.
  Use  : Checks whether the game is available, performs remote authentication, and connects to the GP server
 *---------------------------------------------------------------------------*/
BOOL DWC_LoginAsync(const u16*  ingamesn,
                    const char* reserved,
                    DWCLoginCallback callback,
                    void* param)
{
#pragma unused(reserved)
    u32 len;

    SDK_ASSERT(stpDwcCnt);
//    SDK_ASSERT(userID);
//    SDK_ASSERT(password);
    SDK_ASSERT(callback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_LoginAsync() was called!!\n");

    if (ingamesn == NULL){
        // It is absolutely necessary to configure ingamesn.
        DWC_Printf(DWC_REPORTFLAG_WARNING, "ingamesn is NULL!!\n");
        return FALSE;
    }

    // [arakit] main 051025
    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_INIT)){
        // Do nothing when called during login or when already logged in
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }
    // [arakit] main 051025

    stpDwcCnt->loginCallback   = callback;
    stpDwcCnt->loginParam      = param;
    //stpDwcCnt->logcnt.userID   = userID;
    //stpDwcCnt->logcnt.password = password;

    // Registers the in-game screen name.
    if (!ingamesn || (ingamesn[0] == '\0')){
        len = 0;
    }
    else {
        // Only copy the portion that falls within the maximum character number.
        MI_CpuClear16( stpDwcCnt->playerName, DWC_MAX_PLAYER_NAME*2 );
        len = DWCi_WStrLen(ingamesn) <= DWC_MAX_PLAYER_NAME-1 ? DWCi_WStrLen(ingamesn) : DWC_MAX_PLAYER_NAME-1;
        MI_CpuCopy16(ingamesn, stpDwcCnt->playerName, len*2);
    }
    stpDwcCnt->playerName[len] = 0;

    // Attempted login even though not connected to the Internet yet.
    if (DWC_GetInetStatus() != DWC_CONNECTINET_STATE_CONNECTED )
    {
        DWCi_StopLogin( DWC_ERROR_AUTH_ANY, DWC_ECODE_SEQ_LOGIN + DWC_ECODE_TYPE_NETWORK );
        // [arakit] main 051025
        return TRUE;
    }

    DWCi_SetState(DWC_STATE_AVAILABLE_CHECK);

    // Checks whether or not the game specified by GAME_NAME can be processed by the GameSpy server before using the GameSpy API.
    //  
	// Start availability check
	GSIStartAvailableCheck(stpDwcCnt->gameName);

    // [arakit] main 051025
    return TRUE;
}


/*---------------------------------------------------------------------------*
  friend list synchronization function
  Function: playerName   Player name for this player that can be referenced by other players
                         ¨The past specification. Currently ignores the set value.
          updateCallback friend list synchronization complete callback
          updateParam : Parameter for the above callback
          statusCallback     Callback for notification of changes in friend status
          statusParam : Parameter for the above callback
          deleteCallback     Friend roster delete callback
          deleteParam : Parameter for the above callback
  Return value: If function call completed, TRUE. Returns callback based on success or failure.
          FALSE: Not a state in which this function should be called. Callback does not return.
  use: after login completes, performs synchronization of friend list (buddy list)
          on GP server with local friend list
 *---------------------------------------------------------------------------*/
BOOL DWC_UpdateServersAsync(const char* playerName,
                            DWCUpdateServersCallback updateCallback,
                            void* updateParam,
                            DWCFriendStatusCallback statusCallback,
                            void* statusParam,
                            DWCDeleteFriendListCallback deleteCallback,
                            void* deleteParam)
{
#pragma unused(playerName)
    //u32 len;

    // [arakit] main 051025
    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT(updateCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_UpdateServersAsync() was called!!\n");

    // [arakit] main 051025
    if (DWCi_IsError() ||
        (stpDwcCnt->state < DWC_STATE_ONLINE) || (stpDwcCnt->state == DWC_STATE_UPDATE_SERVERS)){
        // STATE_ONLINE when called during login callback
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }

#if 0
    // Gets the player name -> Player name is not set here.
    if (!playerName || (playerName[0] == '\0')){
        len = 0;
    }
    else {
        len = strlen(playerName) < DWC_MAX_PLAYER_NAME ? strlen(playerName) : DWC_MAX_PLAYER_NAME-1;
        MI_CpuCopy8(playerName, stpDwcCnt->playerName, len);
    }
    stpDwcCnt->playerName[len] = '\0';  // Guarantees NULL termination
#endif

    stpDwcCnt->updateServersCallback = updateCallback;
    stpDwcCnt->updateServersParam    = updateParam;

    // transition to friend list synchronization status
    DWCi_SetState(DWC_STATE_UPDATE_SERVERS);

    // begin friend list synchronization
    DWCi_UpdateServersAsync(stpDwcCnt->logcnt.authToken,
                            stpDwcCnt->logcnt.partnerChallenge,
                            DWCi_UpdateServersCallback, NULL,
                            statusCallback, statusParam,
                            deleteCallback, deleteParam);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  no friend specification peer matchmaking beginning function
  Argument  :numEntry  Number of people in requested network construction (including self)
          addFilter       Matchmaking condition string to be added by the game
                          NULL is returned when no condition is added.
                          Conditions can be written in standard SQL format.
                          The maximum string length currently specifiable is 127 characters
                          Debug/build checks if this number of characters has been exceeded.
                          This character string is copied and stored within the library.
          matchedCallback  Callback when matchmaking is complete
          matchedParam   Call-back parameter for above
          evalCallback   Player evaluation callback
          evalParam   Call-back parameter for above
  Return value: If function call completed, TRUE. Matchmaking result comes back in callback.
          FALSE: Not a state in which this function should be called. Callback does not return.
  Use   : Creates a mesh-type network based on a specified number of players rather than by specifying friends
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
BOOL DWC_ConnectToAnybodyAsync(u8  numEntry,
                               const char* addFilter,
                               DWCMatchedCallback matchedCallback,
                               void* matchedParam,
                               DWCEvalPlayerCallback evalCallback,
                               void* evalParam)
{

    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT((numEntry > 1) && (numEntry <= DWC_MAX_CONNECTIONS));
    SDK_ASSERT(matchedCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_ConnectToAnybodyAsync() was called!!\n");

    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_ONLINE)){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        // [arakit] main 051025
        return FALSE;
    }

    // Initializes the gt2 connection list and g2 connection data list
    DWCi_ClearGT2ConnectionList();

    stpDwcCnt->matchedCallback = matchedCallback;
    stpDwcCnt->matchedParam    = matchedParam;

    DWCi_SetState(DWC_STATE_MATCHING);

    // matchmaking function call
    // Subtract 1 for numEntry because within the matchmaking processing it represents the number of connected players not including one's self
    DWCi_ConnectToAnybodyAsync((u8)(numEntry-1),
                               addFilter,
                               DWCi_MatchedCallback, NULL,
                               evalCallback, evalParam);

    // [arakit] main 051025
    return TRUE;
}


/*---------------------------------------------------------------------------*
  friend specification peer matchmaking beginning function
  Argument : friendIdxList   Index list of friends you want to connect to
                           Connection is requested of everyone on the friend roster if this is NULL
          friendIdxListLen  Connection request friend index list length
          numEntry  Number of people in requested network construction (including self)
          distantFriend    TRUE: Allows connection by friends of a friend, FALSE: Such connections are not allowed
          matchedCallback  Callback when matchmaking is complete
          matchedParam   Call-back parameter for above
          evalCallback   Player evaluation callback
          evalParam   Call-back parameter for above
  Return value: If function call completed, TRUE. Matchmaking result comes back in callback.
          FALSE: Not a state in which this function should be called. Callback does not return.
  Use   : Creates a mesh-type network by specifying friends and connecting
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
BOOL DWC_ConnectToFriendsAsync(const u8 friendIdxList[],
                               int friendIdxListLen,
                               u8  numEntry,
                               BOOL distantFriend,
                               DWCMatchedCallback matchedCallback,
                               void* matchedParam,
                               DWCEvalPlayerCallback evalCallback,
                               void* evalParam)
{
    // Temporary friend list. Size is changed if the maximum number of friends is received from the game
    u8  tmpFriendIdxList[DWC_MAX_MATCH_IDX_LIST];
    u8  idxList[DWC_MAX_MATCH_IDX_LIST];
    u8  i;
    u32 j;

    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT(friendIdxListLen <= DWC_MAX_MATCH_IDX_LIST);
    SDK_ASSERT((numEntry > 1) && (numEntry <= DWC_MAX_CONNECTIONS));
    SDK_ASSERT(matchedCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_ConnectToFriendsAsync() was called!!\n");

    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_ONLINE)){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        // [arakit] main 051025
        return FALSE;
    }

    // Initializes the gt2 connection list and g2 connection data list
    DWCi_ClearGT2ConnectionList();

    stpDwcCnt->matchedCallback = matchedCallback;
    stpDwcCnt->matchedParam    = matchedParam;

    DWCi_SetState(DWC_STATE_MATCHING);

    // Subtract 1 for numEntry because within the matchmaking processing it represents the number of connected players not including one's self
    if (friendIdxList){
        // matchmaking function call
        DWCi_ConnectToFriendsAsync(friendIdxList, friendIdxListLen,
                                   (u8)(numEntry-1),
                                   distantFriend,
                                   DWCi_MatchedCallback, NULL,
                                   evalCallback, evalParam);
    }
    else {
        // If a friend index list has not been specified, the indices of all friends are temporarily registered on the index list and passed to the matching function
        // registered on the index list and passed to the matching function
        friendIdxListLen = 0;
        
        // creates an index list based on index extraction
        for (i = 0; i < DWCi_GetFriendListLen(); i++){
            idxList[i] = i;
        }

        // extracts random index values from the list,
        // and creates a friends index list
        for (i = 0; i < DWCi_GetFriendListLen(); i++){
            u32 randIdx = DWCi_GetMathRand32((u32)(DWCi_GetFriendListLen()-i));

#ifdef DWC_MATCH_ACCEPT_NO_FRIEND
            // Set all even though not already established friends
            tmpFriendIdxList[i] = idxList[randIdx];
            friendIdxListLen++;
            
#else
            // avoids selection of invalid friend data
            if (DWCi_Acc_IsValidFriendData(&(DWCi_GetFriendList())[idxList[randIdx]])){
                tmpFriendIdxList[i] = idxList[randIdx];
                friendIdxListLen++;
            }
#endif

            // pads the end of the index list forward from the extracted index
            for (j = randIdx; j < DWCi_GetFriendListLen()-i-1; j++){
                idxList[j] = idxList[j+1];
            }
        }
            
        // matchmaking function call
        DWCi_ConnectToFriendsAsync(tmpFriendIdxList, friendIdxListLen,
                                   (u8)(numEntry-1),
                                   distantFriend,
                                   DWCi_MatchedCallback, NULL,
                                   evalCallback, evalParam);
    }

    // [arakit] main 051025
    return TRUE;
}


/*---------------------------------------------------------------------------*
  server-client matchmaking server launching function
  Argument : maxEntry    Maximum number of players to be connected (including self)
          matchedCallback:  Callback when matchmaking is complete. Called each time a person connects.
          matchedParam   Call-back parameter for above
          newClientCallback Newly connected client notification callback
          newClientParam   Call-back parameter for above
  Return value: If function call completed, TRUE. Matchmaking result comes back in callback.
          FALSE: Not a state in which this function should be called. Callback does not return.
  Use  : Once the server is up and a request is received from a client,
          connect/disconnect processing is performed.
 *---------------------------------------------------------------------------*/
// [arakit] main 051025 051027
BOOL DWC_SetupGameServer(u8  maxEntry,
                         DWCMatchedSCCallback matchedCallback,
                         void* matchedParam,
                         DWCNewClientCallback newClientCallback,
                         void* newClientParam)
{

    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT((maxEntry > 1) && (maxEntry <= DWC_MAX_CONNECTIONS));
    SDK_ASSERT(matchedCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_SetupGameServer() was called!!\n");

    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_ONLINE)){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        // [arakit] main 051025
        return FALSE;
    }

    // Initializes the gt2 connection list and g2 connection data list
    DWCi_ClearGT2ConnectionList();

    stpDwcCnt->matchedSCCallback = matchedCallback;
    stpDwcCnt->matchedSCParam    = matchedParam;

    // Always AID = 0 for the server
    stpDwcCnt->aid = 0;

    DWCi_SetState(DWC_STATE_MATCHING);

    // matchmaking function call
    // Subtract 1 for maxEntry because within matchmaking it represents the number of connected players not including one's self
    // [arakit] main 051027
    DWCi_SetupGameServer((u8)(maxEntry-1),
                         DWCi_MatchedCallback, NULL,
                         newClientCallback, newClientParam);

    // [arakit] main 051025
    return TRUE;
}


/*---------------------------------------------------------------------------*
  server-client matchmaking client launching function
  Argument  : serverIndex   Friend roster index for the server to be connected to
          matchedCallback:  Callback when matchmaking is complete. Called each time a person connects.
          matchedParam   Call-back parameter for above
          newClientCallback Newly connected client notification callback
          newClientParam   Call-back parameter for above
  Return value: If function call completed, TRUE. Matchmaking result comes back in callback.
          FALSE: Not a state in which this function should be called. Callback does not return.
  Use  :  Uses friend roster index to specify the server to be connected to,
          or, if a client has newly connected to a server,
          perform the connection with that client.
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
BOOL DWC_ConnectToGameServerAsync(int serverIndex,
                                  DWCMatchedSCCallback matchedCallback,
                                  void* matchedParam,
                                  DWCNewClientCallback newClientCallback,
                                  void* newClientParam)
{
    int profileID;
    int buddyIdx = -1;
    GPBuddyStatus status;
    // [arakit] main 051025
    DWCError dwcError;

    SDK_ASSERT(stpDwcCnt);
    SDK_ASSERT(matchedCallback);

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_ConnectToGameServerAsync() was called!!\n");

    if (DWCi_IsError() || (stpDwcCnt->state != DWC_STATE_ONLINE)){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        // [arakit] main 051025
        return FALSE;
    }

    // Initializes the gt2 connection list and g2 connection data list
    DWCi_ClearGT2ConnectionList();

    stpDwcCnt->matchedSCCallback = matchedCallback;
    stpDwcCnt->matchedSCParam    = matchedParam;

    DWCi_SetState(DWC_STATE_MATCHING);

    if (!(profileID = DWCi_GetProfileIDFromList(serverIndex)) ||
        !gpIsBuddy(&stpDwcCnt->gpObj, profileID)){
        // This callback is called on an error when the index specified does not have a Profile ID or when the other party is not a friend.
        // the server-client matchmaking server.
        // [arakit] main 051025
        DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is not buddy.\n", profileID);
        dwcError = DWC_ERROR_NOT_FRIEND_SERVER;
        goto error;
        // [arakit] main 051025
    }

    // Error impossible
    (void)gpGetBuddyIndex(&stpDwcCnt->gpObj, profileID, &buddyIdx);
    (void)gpGetBuddyStatus(&stpDwcCnt->gpObj, buddyIdx, &status);
    
    if (status.status != DWC_STATUS_MATCH_SC_SV){
        // This callback is called on an error when the specified party has not launched
        // the server-client matchmaking server.
        // [arakit] main 051025
        DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is not game server.\n", profileID);
        dwcError = DWC_ERROR_NOT_FRIEND_SERVER;
        goto error;
        // [arakit] main 051025
    }

    // [arakit] main 051024
    {
        char valueStr[4];
        u8   maxEntry, numEntry;

        valueStr[0] = '0';  // Just in case, initialize to zero persons
            
        // Reads the setting for maximum number of connected persons for the server
        DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_MAX, valueStr,
                                 status.statusString, '/');
        maxEntry = (u8)strtoul(valueStr, NULL, 10);

        // Reads the setting for current number of connected persons for the server
        DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_SC_NUM, valueStr,
                                 status.statusString, '/');
        numEntry = (u8)strtoul(valueStr, NULL, 10);

        if (numEntry == maxEntry){
            // [arakit] main 051025
            DWC_Printf(DWC_REPORTFLAG_ERROR, "pid %d is fully occupied.\n", profileID);
            dwcError = DWC_ERROR_SERVER_FULL;
            goto error;
            // [arakit] main 051025
        }
    }
    // [arakit] main 051024

    // matchmaking function call
    DWCi_ConnectToGameServerAsync(profileID, DWCi_MatchedCallback, NULL,
                                  newClientCallback, newClientParam);

    // [arakit] main 051025
    return TRUE;

    // [arakit] main 051025
error:
    // Error processing
    DWCi_SetError(dwcError, 0);
            
    stpDwcCnt->matchedSCCallback(dwcError,
                                 FALSE, TRUE, FALSE,
                                 0, stpDwcCnt->matchedSCParam);

    if ((stpDwcCnt != NULL) && (stpDwcCnt->state == DWC_STATE_MATCHING)){
        // Return to online status if the matchmaking status remains unchanged
        DWCi_SetState(DWC_STATE_ONLINE);
        // GP status is set to online.
        (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
    }

    return TRUE;
    // [arakit] main 051025
}


/*---------------------------------------------------------------------------*
  Connection close callback setup function
  Argument  : callback   This callback is called each time a connection is closed.
          param   Call-back parameter for the above
  Return values: TRUE: registration succeeded
          FALSE: registration failed when FriendsMatch Library was inoperable
  Use  : Sets the callback for closing connections
 *---------------------------------------------------------------------------*/
BOOL DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback, void* param)
{

    if (!stpDwcCnt) return FALSE;

    stpDwcCnt->closedCallback = callback;
    stpDwcCnt->closedParam    = param;

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Function for closing all connections (obsolete function)
  Arguments : None
  Return values: Succeeded if zero or greater, failed if negative
          0: Execute close. This callback is called every time close processing is completed.
          1 : Although close processing was performed, the callback is not called because the number of connected hosts is 0.
             Only for the server-client matchmaking server.
          -1 : Connection not complete or did nothing because an error has occurred.
  Use : Closes all connected connections
          for each connection that is closed,
          the callback function set using DWC_SetConnectionClosedCallback()
          for each picture frame. Remains logged in to the Wi-Fi connection.
          Other hosts are also notified of this close.
          In the case of a server for which server-client matchmaking is used, if connected hosts
          are not present, processing is performed, but no callback is called.
          ->DWC_CloseAllConnectionsHard() should be used.
 *---------------------------------------------------------------------------*/
int  DWC_CloseConnectionsAsync(void)
{

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_CloseConnectionsAsync() was called!!\n");

    if (!stpDwcCnt || DWCi_IsError() ||
        ((stpDwcCnt->state != DWC_STATE_MATCHING) && (stpDwcCnt->state != DWC_STATE_CONNECTED))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }

    if (!stpDwcCnt->matchcnt.gt2NumConnection){
        // Perform exit processing here for servers using server-client matchmaking
        // (only the server-client matchmaking server can call a close with
        // connection host number 0)
        DWC_Printf(DWC_REPORTFLAG_TRANSPORT, "Closed 0 connection.\n");

        // GP status is set to online.
        (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);

        // Frees even the memory being used by NN
        NNFreeNegotiateList();

        DWCi_SetState(DWC_STATE_ONLINE);  // Returns to login mode
            
        return 1;
    }

    // Closes the connections of all connected hosts
    gt2CloseAllConnections(stpDwcCnt->gt2Socket);

    return 0;
}


/*---------------------------------------------------------------------------*
  Function for force-closing all connections
  Arguments : None
  Return values: Succeeded if zero or greater, failed if negative
          0: Execute close.
          1 : Although close processing was performed, the callback is not called because the number of connected hosts is 0.
             Only for the server-client matchmaking server.
          -1 : Connection not complete or did nothing because an error has occurred.
  Use : Closes all connected connections
          Close processing is completed within this function., Before leaving it,
          for each connection that is closed,
          the callback function set using DWC_SetConnectionClosedCallback()
          for each picture frame. Remains logged in to the Wi-Fi connection.
          Other hosts are also notified of this close.
          In the case of a server for which server-client matchmaking is used, if connected hosts
          are not present, processing is performed, but no callback is called.
 *---------------------------------------------------------------------------*/
int  DWC_CloseAllConnectionsHard(void)
{

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_CloseAllConnectionsHard() was called!!\n");

    if (!stpDwcCnt || DWCi_IsError() ||
        ((stpDwcCnt->state != DWC_STATE_MATCHING) && (stpDwcCnt->state != DWC_STATE_CONNECTED))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }

    if (!stpDwcCnt->matchcnt.gt2NumConnection){
        // Perform exit processing here for servers using server-client matchmaking
        // (only the server-client matchmaking server can call a close with
        // connection host number 0)
        DWC_Printf(DWC_REPORTFLAG_TRANSPORT, "Closed 0 connection.\n");

        // GP status is set to online.
        (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);

        // Frees even the memory being used by NN
        NNFreeNegotiateList();

        DWCi_SetState(DWC_STATE_ONLINE);  // Returns to login mode
            
        return 1;
    }

    stpDwcCnt->ownCloseFlag = TRUE;   // Checks that the connection was closed by the game

    // Closes the connections of all connected hosts
    gt2CloseAllConnectionsHard(stpDwcCnt->gt2Socket);

    stpDwcCnt->ownCloseFlag = FALSE;  // Lower the flag

    return 0;
}


/*---------------------------------------------------------------------------*
  Force close function for all connections
  Argument : aid  The AID of the host to be closed.
  Return values : 0 Close executed.
          -1 : Connection not complete or did nothing because an error has occurred.
          -2 : Did nothing because the specified AID is already closed.
  Use : Forcibly closes the connection with the specified AID.
          Close processing is completed within this function. Before leaving this function,
          the callback function set using DWC_SetConnectionClosedCallback()
          for each picture frame. Remains logged in to the Wi-Fi connection.
          Other hosts are also notified of this close.
          Intended only for error processing such as when closing a host with which communications have become impossible due to power off or other reason.
          which communications have become impossible due to power off or other reason.
 *---------------------------------------------------------------------------*/
int  DWC_CloseConnectionHard(u8 aid)
{
    GT2Connection connection;

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_CloseConnectionHard() was called!! aid = %d.\n", aid);

    if (!stpDwcCnt || DWCi_IsError() ||
        ((stpDwcCnt->state != DWC_STATE_MATCHING) && (stpDwcCnt->state != DWC_STATE_CONNECTED))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }
    
    if (!(connection = DWCi_GetGT2Connection(aid))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "No connection!\n");
        return -2;
    }
    
    gt2CloseConnectionHard(connection);

    return 0;
}


/*---------------------------------------------------------------------------*
  This function forcibly closes connections specified by bitmap.
  Argument : bitmap  Pointer to the AID bitmap for hosts to be closed.
                 Only bits for AIDs that were successfully closed are set to 1.
                 Even if the bit for this machine is set, it must always be lowered.
  Return values : Starts close. This callback is called every time close processing is completed.
          -1 : Connection not complete or did nothing because an error has occurred.
          -2 : Did nothing since all AIDs are already closed.
  Use : Forcibly closes the connection with the specified AID.
          Close processing is completed within this function. Before leaving this function,
          the callback function set using DWC_SetConnectionClosedCallback()
          for each picture frame. Remains logged in to the Wi-Fi connection.
          Other hosts are also notified of this close.
          Intended only for error processing such as when closing a host with which communications have become impossible due to power off or other reason.
          which communications have become impossible due to power off or other reason.
 *---------------------------------------------------------------------------*/
int  DWC_CloseConnectionHardBitmap(u32* bitmap)
{
    u8  aid;
    u32 bitmask;

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "!!DWC_CloseConnectionHardBitmap() was called!! AID bitmap = 0x%x\n",
               (bitmap == NULL) ? 0 : *bitmap);

    if (!stpDwcCnt || !bitmap || DWCi_IsError() ||
        ((stpDwcCnt->state != DWC_STATE_MATCHING) && (stpDwcCnt->state != DWC_STATE_CONNECTED))){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return -1;
    }

    for (aid = 0; aid < DWC_MAX_CONNECTIONS; aid++){
        bitmask = 1U << aid;

        if (*bitmap & bitmask){
            if (aid == DWC_GetMyAID()){
                *bitmap &= ~bitmask;       // Set the AID bit for oneself back to 0
            }
            else if (DWC_CloseConnectionHard(aid)){
                *bitmap &= ~bitmask;  // Close failed
            }
        }
    }

    if (!*bitmap) return -2;  // No connections existed
    return 0;  // Close succeeded
}


/*---------------------------------------------------------------------------*
  Function for getting the number of connected hosts
  Arguments : None
  Return values: Number of hosts making up the network (including this one).
          Returns 0 when offline.
  Use : This function returns the number of hosts on a mesh-type network.
 *---------------------------------------------------------------------------*/
int DWC_GetNumConnectionHost(void)
{

    if (!stpDwcCnt) return 0;

    if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
        // Returns only the number of valid AIDs when someone joins midway
        return DWCi_GetNumValidConnection()+1;
    }
    else {
        // Returns the current number of actual connections even during matchmaking
        return DWCi_GetNumAllConnection()+1;
    }
}


/*---------------------------------------------------------------------------*
  This function gets own AID.
  Arguments : None
  Return value: Its own AID.
  Use: This function returns the AID of this host (the unique host ID used within the mesh network).
 *---------------------------------------------------------------------------*/
u8  DWC_GetMyAID(void)
{

    if (!stpDwcCnt) return 0;

    return stpDwcCnt->aid;
}


/*---------------------------------------------------------------------------*
  Function for getting the AID list of currently connected hosts
  Argument : aidList  Pointer to the AID list.
  Return value: The AID list length (including self).
  Use : Gets a pointer to the AID list (does not copy).
          The AID list is packed, from the front in order, with currently enabled AIDs,
          and this number of instances represents the number of hosts currently connected, including this host.
 *---------------------------------------------------------------------------*/
int DWC_GetAIDList(u8** aidList)
{

    if (!stpDwcCnt) return 0;

    *aidList = stpDwcCnt->matchcnt.aidList;

    if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
        // Returns only the number of valid AIDs when someone joins midway
        return DWCi_GetValidAIDList(aidList);
    }
    else {
        return DWCi_GetAllAIDList(aidList);
    }
}


/*---------------------------------------------------------------------------*
  Function for getting the AID bitmap of currently connected hosts
  Arguments : None
  Return value: AID bitmap. Returns 0 when offline.
  Use : Gets the AID bitmap for currently connected hosts.
 *---------------------------------------------------------------------------*/
u32 DWC_GetAIDBitmap(void)
{
    u8* pAidList;
    u32 bitmap = 0;
    int numHost;

    if (!stpDwcCnt) return 0;

    numHost = DWC_GetAIDList(&pAidList);

    // Get AID bitmap
    bitmap = DWCi_GetAIDBitmapFromList(pAidList, numHost);

    return bitmap;
}


/*---------------------------------------------------------------------------*
  Function for determining if an AID is valid
  Argument : aid  AID
  Return values: TRUE: The AID is valid, FALSE: AID invalid
  Use : This function checks whether or not the specified AID is valid (whether or not it is connected to this host).
          Returns FALSE if the AID for this host was specified.
          AID of the newly connected client (when matchmaking) is judged invalid.
 *---------------------------------------------------------------------------*/
BOOL DWC_IsValidAID(u8 aid)
{

    if (!stpDwcCnt) return FALSE;

    // Matchmaking from the game is complete whether or not there is a connection
    // Makes it so that only hosts appear valid
    if (!(stpDwcCnt->matchcnt.validAidBitmap & (1 << aid))) return FALSE;

    return DWCi_IsValidAID(aid);
}


/*---------------------------------------------------------------------------*
  FriendsMatch Library status getting function
  Arguments : None
  Return value : DWCState type enumerator
  use: gets the processing state of the FriendsMatch Library
 *---------------------------------------------------------------------------*/
DWCState DWC_GetState(void)
{

    if (!stpDwcCnt){
        return DWC_STATE_INIT;
    }
    else {
        return stpDwcCnt->state;
    }
}


/*---------------------------------------------------------------------------*
  Get socket error function
  Arguments : None
  Return value: GT2 socket error type
  Use : Gets the socket error type in the GameSpy gt2 SDK
 *---------------------------------------------------------------------------*/
int DWC_GetLastSocketError(void)
{

    return stLastSocketError;
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for initializing GT2
  Arguments : None
  Return value: GT2 processing result type
  Use : Performs GT2 initialization
 *---------------------------------------------------------------------------*/
GT2Result DWCi_GT2Startup(void)
{
    u16 baseport;
    GT2Result gt2Result;

    if (stpDwcCnt->gt2Socket){
        // Do nothing if control flow somehow arrived here even though there already is a gt2 socket
        DWC_Printf(DWC_REPORTFLAG_WARNING, "gt2Socket is already made.\n");
        return GT2Success;
    }

    // Randomly generate a port to be used for the GT2 socket
    baseport = (u16)(0xc000+DWCi_GetMathRand32(0x4000));

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "--- Private port = %d ---\n", baseport);

    // Create socket
    gt2Result = gt2CreateSocket(&stpDwcCnt->gt2Socket,
                                gt2AddressToString(0, baseport, NULL),
                                stpDwcCnt->gt2SendBufSize,
                                stpDwcCnt->gt2RecvBufSize,
                                DWCi_GT2SocketErrorCallback);
    if (DWCi_HandleGT2Error(gt2Result)) return gt2Result;

    // Function set for connection request receive callbacks
    // Defined by dwc_match.c
    gt2Listen(stpDwcCnt->gt2Socket, DWCi_GT2ConnectAttemptCallback);

    // Register function to use when a message is received stating GT2 authentication was impossible
    // Defined by dwc_match.c
    gt2SetUnrecognizedMessageCallback(stpDwcCnt->gt2Socket, DWCi_GT2UnrecognizedMessageCallback);

    return gt2Result;
}


/*---------------------------------------------------------------------------*
  Function for getting a GT2 connection
  Argument: aid : Host AID
  Return value: GT2 connection. Returns NULL if no connection corresponds to the AID
  Use : Gets the GT2 connection value that corresponds to the AID
 *---------------------------------------------------------------------------*/
GT2Connection DWCi_GetGT2Connection(u8 aid)
{
    int i;

    if (!stpDwcCnt) return NULL;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        if (stGt2ConnectionList[i] &&
            (((DWCConnectionInfo *)gt2GetConnectionData(stGt2ConnectionList[i]))->aid == aid)){
            return stGt2ConnectionList[i];
        }
    }

    return NULL;
}


/*---------------------------------------------------------------------------*
  Function for getting the AID from the GT2 connection
  Argument: connection : GT2 connection type
  Return value: Connection AID
  Use : Gets the AID from the GT2 connection
 *---------------------------------------------------------------------------*/
u8  DWCi_GetConnectionAID(GT2Connection connection)
{

    return ((DWCConnectionInfo *)gt2GetConnectionData(connection))->aid;
}


/*---------------------------------------------------------------------------*
  Function for getting the index from the GT2 connection
  Argument: connection : GT2 connection type
  Return value: Index of the connection in the connection list
  Use : Get the index in the connection list from GT2 connection
 *---------------------------------------------------------------------------*/
u8  DWCi_GetConnectionIndex(GT2Connection connection)
{

    return ((DWCConnectionInfo *)gt2GetConnectionData(connection))->index;
}


/*---------------------------------------------------------------------------*
  Function for getting the user data from the GT2 connection
  Argument: connection : GT2 connection type
  Return value : a pointer to the user setting data specific to the connection
  application: gets a pointer to the user setting data specific to the connection
          from the GT2Connection
 *---------------------------------------------------------------------------*/
void* DWCi_GetConnectionUserData(GT2Connection connection)
{

    return ((DWCConnectionInfo *)gt2GetConnectionData(connection))->param;
}


/*---------------------------------------------------------------------------*
  Function for getting an empty index on the GT2 connection list
  Arguments : None
  Return value: Empty index in list. Returns -1 if no spots are empty.
  Use : Gets an empty index on the GT2 connection list.
 *---------------------------------------------------------------------------*/
int  DWCi_GT2GetConnectionListIdx(void)
{
    int i;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        if (!stGt2ConnectionList[i]){
            return i;
        }
    }

    return -1;
}


/*---------------------------------------------------------------------------*
  Function for initializing the GT2 connection list and GT2 connection data list
  Arguments : None
  Return value: None
  Use : Initializes the GT2 connection list and the GT2 connection data list
 *---------------------------------------------------------------------------*/
void DWCi_ClearGT2ConnectionList(void)
{
    
    MI_CpuClear32(stGt2ConnectionList, sizeof(GT2Connection)*DWC_MAX_CONNECTIONS);
    MI_CpuClear32(stConnectionInfoList, sizeof(DWCConnectionInfo)*DWC_MAX_CONNECTIONS);
}


/*---------------------------------------------------------------------------*
  Function for getting a pointer to the GT2 connection list
  Argument: index : Index of the GT2 connection list
  Return value: Pointer to the GT2 connection list
  Use : Gets a pointer to an element in the GT2 connection list based on an index
 *---------------------------------------------------------------------------*/
GT2Connection* DWCi_GetGT2ConnectionByIdx(int index)
{

    return &stGt2ConnectionList[index];
}


/*---------------------------------------------------------------------------*
  Function for getting a pointer to the GT2 connection list
  Argument: profileID : The host profile ID
          numHost : Number of hosts to be checked
  Return value: Pointer to the GT2 connection list
          Returns NULL if no connection corresponds to the Profile ID.
  Use : Gets a pointer to an element in the GT2 connection list based on a profile ID
 *---------------------------------------------------------------------------*/
GT2Connection* DWCi_GetGT2ConnectionByProfileID(int profileID, int numHost)
{
    u8 i;

    for (i = 0; i < numHost; i++){
        if (stpDwcCnt->matchcnt.sbPidList[i] == profileID) break;
    }

    if (i >= numHost) return NULL;  // Specified host not present

    return DWCi_GetGT2ConnectionByIdx(DWCi_GetConnectionIndex(DWCi_GetGT2Connection(stpDwcCnt->matchcnt.aidList[i])));
}


/*---------------------------------------------------------------------------*
  Function for getting a pointer to the GT2 connection data list
  Argument: index : Index of the GT2 connection list
  Return value: Pointer to the GT2 connection data list
  Use : Gets a pointer to an element of the GT2 connection data list that corresponds to (has the same index as) an index on the GT2 connection list
           
 *---------------------------------------------------------------------------*/
DWCConnectionInfo* DWCi_GetConnectionInfoByIdx(int index)
{

    return &stConnectionInfoList[index];
}


/*---------------------------------------------------------------------------*
  Function for determining AID validity (version for internal use)
  Argument : aid  AID
  Return values: TRUE: The AID is valid, FALSE: AID invalid
  Use : Checks whether or not the specified AID is valid (whether or not the AID is connected).
          Returns FALSE if the AID for this host was specified.
          This differs from externally available versions in that even AIDs for
          newly connected clients during matchmaking are deemed valid.
 *---------------------------------------------------------------------------*/
BOOL DWCi_IsValidAID(u8 aid)
{
    int i;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        if (stGt2ConnectionList[i] &&
            (((DWCConnectionInfo *)gt2GetConnectionData(stGt2ConnectionList[i]))->aid == aid)){
            return TRUE;
        }
    }

    return FALSE;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  DCF communicaton processing update function
  Arguments : None
  Return value : Disconnected when TRUE
  Use : Called every game frame to monitor DCF communications.
 *---------------------------------------------------------------------------*/
static BOOL DWCs_UpdateConnection( void )
{
    DWC_ProcessInet();

    if ( DWCi_CheckDisconnected() )
    {
        // Disconnected.
        DWCi_SetError( DWC_ERROR_DISCONNECTED, 0 );

        // [arakit] main 051013
        DWC_Printf( DWC_REPORTFLAG_DEBUG,
                    "Detected DCF shutdown and call DWC_CleanupInet().\n" );

        DWC_CleanupInet();

        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  DCF communicaton processing update function
  Arguments : None
  Return value: None
  Use : Called when DCF has been disconnected. Put code for any required processing here.
 *---------------------------------------------------------------------------*/
static void DWCs_ForceShutdown( void )
{
    // Functions which can be forcibly called without problem are called here.
}


/*---------------------------------------------------------------------------*
  FriendsMatch Library status setting function
  Argument: DWCState type enumerator
  Return value: None
  application: sets the processing state of the FriendsMatch Library
 *---------------------------------------------------------------------------*/
static void DWCi_SetState(DWCState state)
{

    stpDwcCnt->lastState = stpDwcCnt->state;
    stpDwcCnt->state     = state;
}


/*---------------------------------------------------------------------------*
  Function for removing AIDs
  Argument: aid : AID to be removed from list
  Return value : Profile ID corresponding to the deleted AID
  Use : Removes the specified AID from the AID list and shifts remaining data towards the front while also shifting data in the profile list toward the front when an index has a dependent relationship.
           
 *---------------------------------------------------------------------------*/
static int DWCi_DeleteAID(u8 aid)
{
    u8* pAidList;
    int numHost;
    int i;

    numHost = DWCi_GetAllAIDList(&pAidList);

    for (i = 0; i < numHost; i++){
        if (pAidList[i] == aid) break;
    }

    if (i == numHost) return 0;  // A non-existent AID was specified

    return DWCi_DeleteHostByIndex(i, numHost);
}


/*---------------------------------------------------------------------------*
  Function for getting an AID bitmap
  Argument: aidList : Original list for getting AID bitmap
          aidListLen : AID list length (number of connected hosts + 1)
  Return value: AID bitmap.
  Use : Creates an AID bitmap from the specified AID list
 *---------------------------------------------------------------------------*/
static u32 DWCi_GetAIDBitmapFromList(u8* aidList, int aidListLen)
{
    u32 bitmap = 0;
    int i;

    for (i = 0; i < aidListLen; i++){
        bitmap |= 1 << aidList[i];
    }

    return bitmap;
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

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Main, GP error %d\n", result);

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

    // Add a base error code and call specific error processing during various processing
    switch (stpDwcCnt->state){
    case DWC_STATE_AVAILABLE_CHECK:
        errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GP;
        DWCi_StopLogin(dwcError, errorCode);
        break;
    case DWC_STATE_LOGIN:
        errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GP;
        // [arakit] main 051027
        if (stpDwcCnt->logcnt.state < DWC_LOGIN_STATE_REMOTE_AUTH){
            // If before start of gpConnect during login, stop login and
            // call the game-side callback
            DWCi_StopLogin(dwcError, errorCode);
        }
        else {
            // If after start of gpConnect during login, the halting of the login and the calling of the game-side callback take place inside GPConnectCallback, so do nothing here. (If an error is set, processing will not advance.)
            // side callback take place inside GPConnectCallback,
            // so do nothing here. (If an error is set, processing will not advance.)
            DWC_Printf(DWC_REPORTFLAG_ERROR, "Not handle an error here.\n");
            // [arakit] main 051027
        }
        break;
    case DWC_STATE_MATCHING:
        // When there is a matchmaking error in a pre-existing STATE_CONNECTED state
        // during server-client matchmaking, there is no need to return a
        // matchmaking completion callback
        errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GP;
        DWCi_StopMatching(dwcError, errorCode);
        break;
    case DWC_STATE_UPDATE_SERVERS:
        errorCode += DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_GP;
        break;
    default:
        errorCode += DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_GP;
        break;
    }

    // Always exit friend management processing
    DWCi_StopFriendProcess(dwcError, errorCode);

    return result;
}


/*---------------------------------------------------------------------------*
  Error processing function for GT2Result
  Argument : result : GT2 processing result type
  Return value: GT2 processing result type (the argument is returned unchanged)
  Use : Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) according to the value of GT2Result
           
 *---------------------------------------------------------------------------*/
static GT2Result DWCi_HandleGT2Error(GT2Result result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GT2Success) return GT2Success;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Main, GT2 error %d\n", result);

    switch (result){
    case GT2OutOfMemory:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GT2Rejected:
    case GT2DuplicateAddress:
        dwcError  = DWC_ERROR_NONE;
        errorCode = 0;
        result    = GT2Success;
        break;
    case GT2NetworkError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GT2AddressError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case GT2TimedOut:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_TIMEOUT;
        break;
    case GT2NegotiationError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_PEER;
        break;
    }

    // Currently only called when gt2CreatSocket is called during login
    if (dwcError){
        // Base code added
        errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GT2;
        DWCi_StopLogin(dwcError, errorCode);
    }

    return result;
}


/*---------------------------------------------------------------------------*
  Login complete callback function
  Arguments: error: DWC error type
          profileID : Profile ID obtained as a result of login
          param : Parameters for the callback
  Return value: None
  Use : Login complete callback.
          Called from a function in dwc_login.c when login is complete.
 *---------------------------------------------------------------------------*/
static void DWCi_LoginCallback(DWCError error, int profileID, void *param)
{
#pragma unused(param)

    if (error == DWC_ERROR_NONE){
        // Sets status to online if login succeeds
        stpDwcCnt->profileID = profileID;
        DWCi_SetState(DWC_STATE_ONLINE);

        // Initializes the counter for the number of calls to gpProcess()
        DWCi_InitGPProcessCount();
    }
    else {
        // When login fails
        DWCi_SetState(DWC_STATE_INIT);
    }

    // Calls the callback function given by the game
    if ( stpDwcCnt->loginCallback != NULL )
    {
        stpDwcCnt->loginCallback(error, profileID, stpDwcCnt->loginParam);
    }
}


/*---------------------------------------------------------------------------*
  friend list synchronization complete callback function
  Arguments: error: DWC error type
          isChanged : TRUE: Friend roster has changed, FALSE : Friend roster has not changed
          param : Parameters for the callback
  Return value: None
  application: friend list synchronization complete callback
          Called from within dwc_friend.c when friend list synchronization completes.
 *---------------------------------------------------------------------------*/
static void DWCi_UpdateServersCallback(DWCError error, BOOL isChanged, void* param)
{
#pragma unused(param)

    // [arakit] main 051008
    if (stpDwcCnt->lastState != DWC_STATE_UPDATE_SERVERS){
        // If a transition to another state is not made after calling DWC_UpdateServersAsyn(),
        // return to the status before the calls was made.
        // [todo]
        // It is necessary to remove UPDATE_SERVERS from the main state because changes in two or more states cannot be handled
        //  
        DWCi_SetState(stpDwcCnt->lastState);
    }
    // [arakit] main 051008

    // Calls the callback given by the game
    stpDwcCnt->updateServersCallback(error, isChanged, stpDwcCnt->updateServersParam);
}


/*---------------------------------------------------------------------------*
  matchmaking complete callback function
  Arguments: error: DWC error type
          cancel : TRUE : Matchmaking has terminated due to cancellation
                 FALSE : Not cancelled
          self : TRUE : Connection processing for self has completed, FALSE: Connection processing for others has completed
          index: the friend list index for the connection host for server-
                 Friend roster index
          param : Parameters for the callback
  Return value: None
  application:  Callback when matchmaking is complete.
          Called from a function in dwc_match.c when matchmaking is complete.
 *---------------------------------------------------------------------------*/
static void DWCi_MatchedCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int index, void* param)
{
#pragma unused(param)
    int i;

    if ((error == DWC_ERROR_NONE) && cancel){
        // When matchmaking has been canceled
        if (stpDwcCnt->matchcnt.state == DWC_MATCH_STATE_INIT){
            // When matchmaking has completed with cancel by self
            // QR2 key data is cleared so that a reserved command will not arrive
            //  
            DWCi_ClearQR2Key();

            // Returns to online status
            DWCi_SetState(DWC_STATE_ONLINE);
        }
        // Processing continues in the case of a server-client matchmaking even if a cancellation is issued by another host
    }
    else if (error == DWC_ERROR_NONE){
        // Sets status to connected
        DWCi_SetState(DWC_STATE_CONNECTED);

        // Gets one's own AID
        // (allow same processes to pass when getting during matchmaking start for
        // server in server-client matchmaking)
        for (i = 0; i <= stpDwcCnt->matchcnt.gt2NumConnection; i++){
            if (stpDwcCnt->matchcnt.sbPidList[i] == stpDwcCnt->profileID){
                stpDwcCnt->aid = stpDwcCnt->matchcnt.aidList[i];
                break;
            }
        }
    }

#ifndef	SDK_FINALROM
    if (!error){
        int i;
        // [test]
        // Debug display
        for (i = 0; i < stpDwcCnt->matchcnt.gt2NumConnection+1; i++){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "aid[%d] = %d, pid[%d] = %u\n",
                       i, stpDwcCnt->matchcnt.aidList[i],
                       i, stpDwcCnt->matchcnt.sbPidList[i]);
        }
    }
#endif

    // Update valid AID bitmap
    stpDwcCnt->matchcnt.validAidBitmap =
        DWCi_GetAIDBitmapFromList(stpDwcCnt->matchcnt.aidList, stpDwcCnt->matchcnt.gt2NumConnection+1);
    // Update number of valid connections
    DWCi_SetNumValidConnection();

    // Calls the callback given by the game
    if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
        stpDwcCnt->matchedSCCallback(error, cancel, self, isServer, index,
                                     stpDwcCnt->matchedSCParam);
    }
    else {
        stpDwcCnt->matchedCallback(error, cancel, stpDwcCnt->matchedParam);
    }

    // [arakit] main 051007
    if ((error != DWC_ERROR_NONE) &&
        (stpDwcCnt != NULL) && (stpDwcCnt->state == DWC_STATE_MATCHING)){
        // Return to online status if the matchmaking status remains unchanged when an error has occurred
        DWCi_SetState(DWC_STATE_ONLINE);
        // GP status is always set inside DWCi_StopMatching()
    }
    // [arakit] main 051007
}


/*---------------------------------------------------------------------------*
  GP error callback function
  Arguments: pconnection : Pointer to the GP connection object
          arg : Pointer to GP error structure
          param : Parameters for the callback
  Return value: None
  Use : GP error callback
 *---------------------------------------------------------------------------*/
static void DWCi_GPErrorCallback(GPConnection* pconnection, GPErrorArg* arg, void* param)
{
#pragma unused(pconnection)
#pragma unused(param)
	gsi_char* errorCodeString;
	gsi_char* resultString;
    GPResult gpResult;

#ifdef DWC_IGNORE_GP_ERROR_ALREADY_BUDDY
    if (arg->errorCode == GP_ADDBUDDY_ALREADY_BUDDY ||
        arg->errorCode == GP_BM_NOT_BUDDY ||
        arg->errorCode == GP_DELBUDDY_NOT_BUDDY
	){
        // Ignore friend registration request error for unregistered friends
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "Ignore GP ALLREADY_BUDDY or NOT_BUDDY %d.\n",
                   arg->errorCode);
        return;
    }
#endif

#define RESULT(x) case x: resultString = #x; break;
	switch (arg->result){
	RESULT(GP_NO_ERROR)
	RESULT(GP_MEMORY_ERROR)
	RESULT(GP_PARAMETER_ERROR)
	RESULT(GP_NETWORK_ERROR)
	RESULT(GP_SERVER_ERROR)
	default:
		resultString = "Unknown result!\n";
	}

#define ERRORCODE(x) case x: errorCodeString = #x; break;
	switch(arg->errorCode){
	ERRORCODE(GP_GENERAL)
	ERRORCODE(GP_PARSE)
	ERRORCODE(GP_NOT_LOGGED_IN)
	ERRORCODE(GP_BAD_SESSKEY)
	ERRORCODE(GP_DATABASE)
	ERRORCODE(GP_NETWORK)
	ERRORCODE(GP_FORCED_DISCONNECT)
	ERRORCODE(GP_CONNECTION_CLOSED)
	ERRORCODE(GP_LOGIN)
	ERRORCODE(GP_LOGIN_TIMEOUT)
	ERRORCODE(GP_LOGIN_BAD_NICK)
	ERRORCODE(GP_LOGIN_BAD_EMAIL)
	ERRORCODE(GP_LOGIN_BAD_PASSWORD)
	ERRORCODE(GP_LOGIN_BAD_PROFILE)
	ERRORCODE(GP_LOGIN_PROFILE_DELETED)
	ERRORCODE(GP_LOGIN_CONNECTION_FAILED)
	ERRORCODE(GP_LOGIN_SERVER_AUTH_FAILED)
	ERRORCODE(GP_NEWUSER)
	ERRORCODE(GP_NEWUSER_BAD_NICK)
	ERRORCODE(GP_NEWUSER_BAD_PASSWORD)
	ERRORCODE(GP_UPDATEUI)
	ERRORCODE(GP_UPDATEUI_BAD_EMAIL)
	ERRORCODE(GP_NEWPROFILE)
	ERRORCODE(GP_NEWPROFILE_BAD_NICK)
	ERRORCODE(GP_NEWPROFILE_BAD_OLD_NICK)
	ERRORCODE(GP_UPDATEPRO)
	ERRORCODE(GP_UPDATEPRO_BAD_NICK)
	ERRORCODE(GP_ADDBUDDY)
	ERRORCODE(GP_ADDBUDDY_BAD_FROM)
	ERRORCODE(GP_ADDBUDDY_BAD_NEW)
	ERRORCODE(GP_ADDBUDDY_ALREADY_BUDDY)
	ERRORCODE(GP_AUTHADD)
	ERRORCODE(GP_AUTHADD_BAD_FROM)
	ERRORCODE(GP_AUTHADD_BAD_SIG)
	ERRORCODE(GP_STATUS)
	ERRORCODE(GP_BM)
	ERRORCODE(GP_BM_NOT_BUDDY)
	ERRORCODE(GP_GETPROFILE)
	ERRORCODE(GP_GETPROFILE_BAD_PROFILE)
	ERRORCODE(GP_DELBUDDY)
	ERRORCODE(GP_DELBUDDY_NOT_BUDDY)
	ERRORCODE(GP_DELPROFILE)
	ERRORCODE(GP_DELPROFILE_LAST_PROFILE)
	ERRORCODE(GP_SEARCH)
	ERRORCODE(GP_SEARCH_CONNECTION_FAILED)
	default:
		errorCodeString = "Unknown error code!\n";
	}

	if (arg->fatal){
		DWC_Printf(DWC_REPORTFLAG_ERROR, "FATAL ERROR\n");
	}
	else {
		DWC_Printf(DWC_REPORTFLAG_ERROR, "ERROR\n");
	}
    
	DWC_Printf(DWC_REPORTFLAG_ERROR, "RESULT: %s (%d)\n", resultString, arg->result);
	DWC_Printf(DWC_REPORTFLAG_ERROR, "ERROR CODE: %s (0x%X)\n", errorCodeString, arg->errorCode);
	DWC_Printf(DWC_REPORTFLAG_ERROR, "ERROR STRING: %s\n", arg->errorString);

    // All regarded as network errors
    gpResult = GP_NETWORK_ERROR;

    // CP error processing function call
    (void)DWCi_HandleGPError(gpResult);
}


/*---------------------------------------------------------------------------*
  GP friend message received callback function
  Arguments: pconnection : Pointer to the GP connection object
          arg : Pointer to a GPRecvBuddyMessageArg type object
          param : Parameters for the callback
  Return value: None
  Use : GP friend message received callback.
 *---------------------------------------------------------------------------*/
static void DWCi_GPRecvBuddyMessageCallback(GPConnection* pconnection, GPRecvBuddyMessageArg* arg, void* param)
{
#pragma unused(param)
    char version[12] = { 0, };
    char* message = arg->message;
    u32 len;

    // Skips the friend registration response callback in dwc_friend.c.
    if ( DWCi_GPRecvBuddyAuthCallback( pconnection, arg, param ) )
    {
        return;
    }

    if (memcmp(message, DWC_GP_COMMAND_STRING, strlen(DWC_GP_COMMAND_STRING))){
        // Does nothing even if a friend message was received that is not a DWC GP command
        DWC_Printf(DWC_REPORTFLAG_WARNING,
                   "Received undefined buddy message. '%s'\n",
                   message);
        return;
    }

    message += strlen(DWC_GP_COMMAND_STRING);
    len = (u32)(strchr(message, 'v')-message);
    strncpy(version, message, len);
    if ((len > 10) || (strtoul(version, NULL, 10) != DWC_MATCHING_VERSION)){
        // Received version different command
        DWC_Printf(DWC_REPORTFLAG_WARNING,
                   "Received different version buddy message command. '%s'\n",
                   message);
        return;
    }

    message += len+1;
    if (memcmp(message, DWC_GP_COMMAND_MATCH_STRING, strlen(DWC_GP_COMMAND_MATCH_STRING)) == 0){
        // [arakit] main 051010
        // As a command that uses matchmaking, a callback function that uses
        // matchmaking during matchmaking is called (even in the case of server-client matchmaking
        // CONNECTED states)
        if ((stpDwcCnt->state == DWC_STATE_MATCHING) ||
            ((stpDwcCnt->state == DWC_STATE_CONNECTED) &&
             ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
              (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)))){
            message += strlen(DWC_GP_COMMAND_MATCH_STRING);
            DWCi_MatchGPRecvBuddyMsgCallback(pconnection, arg->profile, message);
        }
        else {
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed GP matching command.\n");
        }
        // [arakit] main 051010
    }
    //else {
    //    DWC_Printf(DWC_REPORTFLAG_WARNING,
    //               "Received undefined buddy message command. '%s'\n",
    //               arg->message);
    //}
}


/*---------------------------------------------------------------------------*
  GT2 data received callback function
  Arguments : connection : Pointer to the structure for the received connection
          message : Received data
          len : Size of received data
          reliable : Indicates whether or not data is reliable
  Return value: None
  Use : GT2 data received callback
 *---------------------------------------------------------------------------*/
static void DWCi_GT2ReceivedCallback(GT2Connection connection, GT2Byte* message, int len, GT2Bool reliable)
{

	// DWC transport receive processing
    DWCi_RecvCallback( connection, message, len, reliable );
}


/*---------------------------------------------------------------------------*
  GT2 connection disconnected callback function
  Arguments : connection : Pointer to the structure for the disconnected connection
          reason : Reason for disconnection
  Return value: None
  Use : GT2 connection disconnected callback
 *---------------------------------------------------------------------------*/
static void DWCi_GT2ClosedCallback(GT2Connection connection, GT2CloseReason reason)
{
    u8  aid;
    int profileID = 0;
    int errorCode;
    int i;
    BOOL isServer = FALSE;
    BOOL aidValid;
    DWCConnectionInfo* conninfo;
    DWCError dwcError;

#if 0
    // [todo]
    // Since the return value of GOAGetLastError() is always negative, it cannot be used as a condition.
    // If control proceeds as-is to calling the disconnection callback during a socket error, strange operations may result depending on the game implementation.
    //  
    if (GOAGetLastError(gt2GetSocketSOCKET(stpDwcCnt->gt2Socket)) < 0){
        // Although control does arrive here during a socket error when reason is LocalClose,
        // since this callback was called even earlier than SocketErrorCallback,
        // control is not allowed to proceed any further once an error set by GameSpy is seen.
        DWC_Printf(DWC_REPORTFLAG_ERROR,
                   "Connection was closed by socket error.\n");
        return;
    }
#endif

    if (DWCi_IsShutdownMatch()){
        // After connection is complete, or when DWC_ShutdownFriendsMatch()
        // is called during matchmaking (it shouldn't normally be called), nothing happens here
        // in order to prevent a data access exception.
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_WARNING,
                   "Called DWC_ShutdownFriendsMatch() with unexpected status.\n");
        return;
    }
        

    switch (reason){
	case GT2LocalClose:
	case GT2RemoteClose:
        dwcError = DWC_ERROR_NONE;
        break;
	case GT2CommunicationError:
	case GT2SocketError:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_CLOSE;
        break;
	case GT2NotEnoughMemory:
		dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_CLOSE+DWC_ECODE_TYPE_ALLOC;
        break;
    }

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "Connection was closed (reason %d).\n", reason);

    if (!dwcError){
        // Gets connection data
	    conninfo = (DWCConnectionInfo *)gt2GetConnectionData(connection);

        if (!conninfo){
            // If gt2CloseSocket() is called from inside this callback for some reason, control will arrive here a second time.
            // Does nothing in this case. Does nothing.
            // Although the library will not make the call, control arrives here if a user calls DWC_ShutdownFriendsMatch() while inside the callback.
            // Control arrives here if DWC_ShutdownFriendsMatch() is called.
            return;
        }
        
        aid = conninfo->aid;  // Back up AID

        // Records whether or not the AID is for a host for which matchmaking is complete
        if (stpDwcCnt->matchcnt.validAidBitmap & (1 << aid))
             aidValid = TRUE;
        else aidValid = FALSE;

        // Clears the transport connection structure
        DWCi_ClearTransConnection(aid);

        DWC_Printf(DWC_REPORTFLAG_DEBUG, "aid = %d (validity %d).\n",
                   aid, aidValid);

        // Records the fact that the server has closed in server/client matchmaking.
        //  
        // In the case of clients, aid = 0 is always a server, so this alone can be used to make the determination
        if (((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) &&
             (reason == GT2LocalClose)) ||
            ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL) &&
             (aid == 0))){
            isServer = TRUE;
        }

        // The AID is removed from the AID list and data is padded toward the front
        // Similar removal processing is performed for the Profile ID list and other lists.
        profileID = DWCi_DeleteAID(aid);

        // [todo]
        // Memory originally freed here
    	stGt2ConnectionList[conninfo->index] = NULL;

        stpDwcCnt->matchcnt.gt2NumConnection--;
        stpDwcCnt->matchcnt.qr2NNFinishCount--;
    }

    if (!stpDwcCnt->ownCloseFlag &&
        (stpDwcCnt->state == DWC_STATE_CONNECTED) && !aidValid){
        // When a newly-connected client cancels matchmaking
        // during server-client matchmaking.
        // Or, when a newly-connected client also stops responding during the matchmaking synchronization adjustment process.
        //  
        if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) &&
            (dwcError == DWC_ERROR_NONE)){
            // If already connected clients are closed during sync adjustment,
            // the number of players including newly connected clients may be set into gpStatus,
            // so set the number of connected players into GP status here.
            // Set into GP status
            (void)DWCi_GPSetServerStatus();
            
            // The server/client matchmaking server sends out a cancellation notification to a connected client.
            //  
            DWCi_ProcessMatchSCClosing(profileID);
        }

        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Closing process by matching SC.\n");
        return;
    }
    else if (DWCi_ProcessMatchClosing(dwcError, errorCode, profileID)){
        // When a connection is closed during matchmaking, perform the matchmaking process
        // and skip over any subsequent processes.
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Closing process by matching.\n");
        return;
    }

    if (dwcError){
        // If there is an error, process it here
        DWCi_SetError(dwcError, errorCode);
        return;
    }


    // For friend-specific and friend non-specific, the following section only applies after matchmaking is complete.
    // For server/client matchmaking, you will come to here only after making a self-connection and the connected host is closed.
    //  

    // [arakit] main 051010
    if ((!stpDwcCnt->ownCloseFlag) &&
        ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
         (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_CL)) &&
        (stpDwcCnt->matchcnt.sbPidList[stpDwcCnt->matchcnt.gt2NumConnection+2] != 0)){
        // In server/client matchmaking, before a newly-connected client completes a connection
        // and a connected client closes a connection,
        // an adjustment is made so that the list doesn't become misaligned.
        // Since the active AID bitmap will be changed by the next function, temporarily set the AID so that the AID = 0 bit is not disabled.
        //  
        stpDwcCnt->matchcnt.aidList[stpDwcCnt->matchcnt.gt2NumConnection+1] =
            stpDwcCnt->matchcnt.aidList[stpDwcCnt->matchcnt.gt2NumConnection+2];

        // Shift newly connected client data toward the top of the list
        (void)DWCi_DeleteHostByIndex(stpDwcCnt->matchcnt.gt2NumConnection+1,
                                     stpDwcCnt->matchcnt.gt2NumConnection+3);
    }
    // [arakit] main 051010
        
    if (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV){
        // for the server-client matchmaking server
        if (!stpDwcCnt->ownCloseFlag){
            // set decreases in the number of connected players into GP status in cases where they were not closed using DWC_CloseAllConnectionsHard().
            // Sets the decrease in number of connected players into GP status
            (void)DWCi_GPSetServerStatus();
        }
        else if (stpDwcCnt->matchcnt.gt2NumConnection == 0){
            // Set GP status to 'online' if connections have been completely closed using DWC_CloseAllConnectionsHard()
            // GP status is set to online.
            (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
        }
    }
    else if (stpDwcCnt->matchcnt.gt2NumConnection == 0){
        // for non-server-client matchmaking servers,
        // set GP status to online if all connections are closed
        (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
    }

    for (i = 0; i < stpDwcCnt->matchcnt.gt2NumConnection+1; i++){
         DWC_Printf(DWC_REPORTFLAG_DEBUG,
                    "aid[%d] = %d, pid[%d] = %u\n",
                    i, stpDwcCnt->matchcnt.aidList[i],
                    i, stpDwcCnt->matchcnt.sbPidList[i]);
    }

    if ((stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
        // In the case of matching with or without a friend specification, reduce the number of desired connections 
        // to match the current number of connections, and set so that the client doesn't make any reservations.
        stpDwcCnt->matchcnt.qr2NumEntry = stpDwcCnt->matchcnt.qr2NNFinishCount;

        // Notifies the master server of changes in status
        qr2_send_statechanged(stpDwcCnt->matchcnt.qr2Obj);
    }

    if (stpDwcCnt->closedCallback && aidValid){
        // Calls the callback given by the game.
        // In server/client matchmaking, the callback is not called for clients for which a connection has not been completed.
        //  
        stpDwcCnt->closedCallback(dwcError,
                                  reason == GT2LocalClose ? TRUE : FALSE,
                                  isServer,
                                  aid,
                                  DWCi_GetFriendListIndex(profileID),
                                  stpDwcCnt->closedParam);
    }

    // Servers using server-client type matchmaking are closed
    // and even if the number of connected hosts becomes 0 it it unrelated, so drop out here
    if (!stpDwcCnt->ownCloseFlag &&
        (stpDwcCnt->matchcnt.qr2MatchType == DWC_MATCH_TYPE_SC_SV))
        return;

    if (!stpDwcCnt->matchcnt.gt2NumConnection){
        // When all connections are closed, even the memory used by NN is freed for server-client type matching
        // memory used by NN is freed for server-client type matching
        NNFreeNegotiateList();

        // QR2 key data is cleared so that a reserved command will not arrive
        //  
        DWCi_ClearQR2Key();

        DWCi_SetState(DWC_STATE_ONLINE);  // Returns to login mode
    }
}


/*---------------------------------------------------------------------------*
  GT2 PINT callback function
  Arguments : connection : Pointer to the structure for the connection that was successfully pinged
          latency : Latency to other host
  Return value: None
  Use : GT2 PING callback
 *---------------------------------------------------------------------------*/
static void DWCi_GT2PingCallback(GT2Connection connection, int latency)
{
#pragma unused(connection)
    
	DWC_Printf(DWC_REPORTFLAG_TRANSPORT, "Ping: %dms\n", latency);

	// Ping callback set by DWC transport
    DWCi_PingCallback( connection, latency );
}


/*---------------------------------------------------------------------------*
  GT2 socket error callback function
  Argument : socket : GT2Socket object
  Return value: None
  Use : GT2 socket error callback
 *---------------------------------------------------------------------------*/
static void DWCi_GT2SocketErrorCallback(GT2Socket socket)
{

    stLastSocketError = GOAGetLastError(gt2GetSocketSOCKET(socket));

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Socket fatal error! (%d)\n",
               stLastSocketError);

    // Although conceivable during matching, since processing as a
    // FATAL error is performed, matching exit processing is not called
    DWCi_SetError(DWC_ERROR_FATAL, DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_SO_SOCKET);

    // Must be cleared to NULL because sockets are closed by gti2SocketError() when there is a socket error!
    //  
    stpDwcCnt->gt2Socket = NULL;
}
