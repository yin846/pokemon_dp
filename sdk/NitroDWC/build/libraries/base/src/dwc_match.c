#include <nitro.h>
#include <base/dwc_base_gamespy.h>
#include <natneg/natneg.h>
#include <serverbrowsing/sb_internal.h>
#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>
#include <base/dwc_common.h>

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// Maximum size and maximum number of elements for supplemental matchmaking command send data
#define DWC_MATCH_COMMAND_ADD_MESSAGE_MAX 512
#define DWC_MATCH_COMMAND_ADD_MESSAGE_NUM (DWC_MATCH_COMMAND_ADD_MESSAGE_MAX >> 2)

// [arakit] main 051024
// Reason for refusing matchmaking reservation
#define DWC_MATCH_RESV_DENY_REASON_SV_FULL 0x10  // Server is over capacity


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// matchmaking cancellation status enumerator
enum {
    DWC_MATCH_CANCEL_STATE_INIT = 0,  // Initial state
    DWC_MATCH_CANCEL_STATE_EXEC,      // Executing cancel processing (cancel side)
    DWC_MATCH_CANCEL_STATE_NUM
};

// matchmaking close status enumerator
enum {
    DWC_MATCH_CLOSE_STATE_INIT = 0,          // Initial state
    DWC_MATCH_CLOSE_STATE_CHANGE_TO_CLIENT,  // Closed in order to change to client
    DWC_MATCH_CLOSE_STATE_TIMEOUT,           // Closed because no response from host was detected
    DWC_MATCH_CLOSE_STATE_FORCED,            // Remaining hosts are closed due to closure by another host
    DWC_MATCH_CLOSE_STATE_NUM
};

// Type of processing passed to DWCi_PostProcessConnection()
typedef enum {
    DWC_PP_CONNECTION_SV_CONNECT = 0,     // gt2Connect complete for server itself or connection complete between clients
    DWC_PP_CONNECTION_CL_GT2_CONNECT,     // gt2Connect complete for already connected clients
    DWC_PP_CONNECTION_CL_GT2_ACCEPT,      // gt2Accept complete for newly connected clients
    DWC_PP_CONNECTION_CL_FINISH_CONNECT,  // All connections complete for clients already connected to newly connected clients
    DWC_PP_CONNECTION_SYN_FINISH,         // matchmaking completion synchronization adjustment end
    DWC_PP_CONNECTION_NUM
} DWCMatchPpConnectionType;

// Reset level of matchmaking control parameter. Pass to DWCi_ResetMatchParam()
typedef enum {
    DWC_MATCH_RESET_ALL = 0,   // Initializes matchmaking
    DWC_MATCH_RESET_RESTART,   // Restart matchmaking from the beginning
    DWC_MATCH_RESET_CONTINUE,  // Matchmaking continues under the same conditions
    DWC_MATCH_RESET_NUM
} DWCMatchResetLevel;


//----------------------------------------------------------------------------
// variable
//----------------------------------------------------------------------------
// Pointer to matchmaking control object
// to source: static DWCMatchControl* stpMatchCnt = NULL;

// Pointer to filter used for SB server update
static char* stpAddFilter = NULL;

// Pointer to control structure with option to complete matchmaking even if less than the specified number of players
static DWCMatchOptMinCompleteIn* stpOptMinComp = NULL;

// Control structure with an option to disallow further connections after a single matchmaking has
// completed when using server-client matchmaking.
static DWCMatchOptSCBlock stOptSCBlock;

// Key data array for QR2 defined by game
// [todo]
// Change so that only the necessary amount of memory is secured by the game
static DWCGameMatchKeyData stGameMatchKeys[DWC_QR2_GAME_RESERVED_KEYS];

// Proportion of ease of selection for each server when evaluating servers.
static const stEvalRate[DWC_SB_UPDATE_MAX_SERVERS] = { 3, 3, 2, 2, 1, 1, };

#if 0
// for test
int DWCi_connect_errorcount = 0;
#endif

//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static void DWCi_ResetMatchParam(DWCMatchResetLevel level);
static void DWCi_SetMatchCommonParam(u8  matchType,
                                     u8  numEntry,
                                     DWCMatchedSCCallback callback,
                                     void* param);

static void DWCi_CloseMatching(void);

static SBError DWCi_SBUpdateAsync(int profileID);
static int DWCi_GetDefaultMatchFilter(char* filter, int profileID, u8 numEntry, u8 matchType);

static NegotiateError DWCi_NNStartupAsync(int isQR2, int cookie, SBServer server);
static NegotiateError DWCi_DoNatNegotiationAsync(DWCNNInfo* nnInfo);

static int  DWCi_SendMatchCommand(u8 command, int profileID, u32 ip, u16 port, const u32 data[], int len);
static SBError DWCi_SendSBMsgCommand(u8 command, u32 ip, u16 port, const u32 data[], int len);
static GPResult DWCi_SendGPBuddyMsgCommand(GPConnection* connection, u8 command, int profileID, const char* message);
static int  DWCi_GetGPBuddyAdditionalMsg(char* dstMsg, const char* srcMsg, int index);
static void DWCi_StopResendingMatchCommand(void);

static BOOL DWCi_ProcessRecvMatchCommand(u8  command,
                                         int srcPid,
                                         u32 srcIP,
                                         u16 srcPort,
                                         const u32 data[],
                                         int len);
static u8   DWCi_CheckResvCommand(int profileID, u32 qr2IP, u16 qr2Port, u32 matchType, BOOL priorFlag);
static int  DWCi_ProcessResvOK(int profileID, u32 ip, u16 port);
static void DWCi_MakeBackupServerData(int profileID, const u32 data[]);

static int  DWCi_HandleMatchCommandError(int error);
static int  DWCi_SendResvCommand(int profileID, BOOL delay);
static int  DWCi_SendResvCommandToFriend(BOOL delay, BOOL init, int resendPid);
static BOOL DWCi_RetryReserving(int resendPid);

static int  DWCi_CancelReservation(int profileID);
static BOOL DWCi_CancelPreConnectedServerProcess(int clientPid);
static BOOL DWCi_CancelPreConnectedClientProcess(int serverPid);

static int  DWCi_ChangeToClient(void);

static void DWCi_PostProcessConnection(DWCMatchPpConnectionType type);

static BOOL DWCi_AreAllBuddies(const u32 pidList[], u32 pidListLen);

static void DWCi_DoCancelMatching(void);
static void DWCi_FinishCancelMatching(void);
static int  DWCi_InvalidateReservation(void);
static void DWCi_RestartFromCancel(DWCMatchResetLevel level);
static void DWCi_RestartFromTimeout(void);
static int  DWCi_ResumeMatching(void);
static BOOL DWCi_CloseCancelHostAsync(int profileID);
static void DWCi_CloseAllConnectionsByTimeout(void);
static BOOL DWCi_CloseShutdownClientSC(u32 bitmap);

static void DWCi_SendMatchSynPacket(u8 aid, u16 type);
static BOOL DWCi_ProcessMatchSynTimeout(void);

static BOOL DWCi_SendCancelMatchSynCommand(int profileID, u8 command);
static BOOL DWCi_ProcessCancelMatchSynCommand(int profileID, u8 command, u32 data);
static BOOL DWCi_ProcessCancelMatchSynTimeout(void);

static u8   DWCi_GetNewMatchKey(void);
static void DWCi_ClearGameMatchKeys(void);

static u8   DWCi_GetAIDFromList(void);
static BOOL DWCi_IsFriendByIdxList(int profileID);
static int  DWCi_CheckDWCServer(SBServer server);

static int  DWCi_GetPidListIndex(int profileID, BOOL idx0);
static u8   DWCi_GetAIDFromProfileID(int profileID, BOOL idx0);
static u32  DWCi_GetAIDBitmask(BOOL valid);

static void DWCi_InitClWaitTimeout(void);

static void DWCi_InitOptMinCompParam(BOOL reset);
static void DWCi_ProcessOptMinComp(void);

static GPResult DWCi_HandleGPError(GPResult result);
static SBError DWCi_HandleSBError(SBError error);
static qr2_error_t DWCi_HandleQR2Error(qr2_error_t error);
static NegotiateError DWCi_HandleNNError(NegotiateError error);
static NegotiateResult DWCi_HandleNNResult(NegotiateResult result);
static GT2Result DWCi_HandleGT2Error(GT2Result result);

static void DWCi_SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance);
static void DWCi_SBPrintServerData(SBServer server);
static BOOL DWCi_EvaluateServers(BOOL sort);
static void DWCi_RandomizeServers(void);

static void DWCi_QR2ServerKeyCallback(int keyid, qr2_buffer_t outbuf, void *userdata);
static void DWCi_QR2PlayerKeyCallback(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
static void DWCi_QR2TeamKeyCallback(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
static void DWCi_QR2KeyListCallback(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata);
static int DWCi_QR2CountCallback(qr2_key_type keytype, void *userdata);
static void DWCi_QR2AddErrorCallback(qr2_error_t error, gsi_char *errmsg, void *userdata);
static void DWCi_QR2PublicAddrCallback(unsigned int ip, unsigned short port, void* userdata);
static void DWCi_QR2NatnegCallback(int cookie, void *userdata);
static void DWCi_QR2ClientMsgCallback(gsi_char* data, int len, void* userdata);

static void DWCi_NNProgressCallback(NegotiateState state, void* userdata);
static void DWCi_NNCompletedCallback(NegotiateResult result, SOCKET gamesocket, struct sockaddr_in* remoteaddr, void* userdata);
static BOOL DWCi_ProcessNNFailure(BOOL ignoreError);

static void DWCi_SetMatchCnt(DWCMatchControl *pCnt);
static DWCMatchControl *DWCi_GetMatchCnt(void);
static void DWCi_SetMatchStatus(DWCMatchState state);

//#define NITRODWC_DEBUG

#ifdef NITRODWC_DEBUG
static void disp_time(void);
static void disp_match_state(DWCMatchState old, DWCMatchState now);
#endif
//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
/*---------------------------------------------------------------------------*
  matchmaking status master server registration function (for friend specified/unspecified peer matchmaking)
  arguments: None
  return values: TRUE: matchmaking status was registered to the master server,
          FALSE: Status is either pre-login or status could not be registered due to an error
  Use: Registers the matchmaking status of the local host on the master server.
          In about 15 seconds after this function is called, the status of the local host will be visible to other hosts
          and matchmaking can proceed smoothly.
          For matchmaking other than server-client matchmaking, when matchmaking ends,
          the matchmaking status is deleted from the master server.
          If the start matchmaking function has been called, this function will be called inside the library, but
          you should call this function if you want the matchmaking status
          pre-registered with the server so that matchmaking can be ended quickly.
          Note that matchmaking status is also deleted when
          DWC_ShutdownFriendsMatch() has been called.
 *---------------------------------------------------------------------------*/
BOOL DWC_RegisterMatchingStatus(void)
{

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG, "!!DWC_RegisterMatchingStatus() was called!!\n");

    if ((DWCi_GetMatchCnt() == NULL) || (DWCi_GetMatchCnt()->profileID == 0) || DWCi_IsError()){
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "But ignored.\n");
        return FALSE;
    }

    if (DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID)) return FALSE;
    return TRUE;
}
#endif


/*---------------------------------------------------------------------------*
  Matchmaking cancellation function
  arguments: None
  return values: TRUE: Cancellation executed, FALSE: Matchmaking not in progress
  Application: Cancels matchmaking processes in progress.
          The cancellation completes within this function, and the matchmaking completion callback
          is called.
 *---------------------------------------------------------------------------*/
BOOL DWC_CancelMatching(void)
{

    if (DWC_IsValidCancelMatching()){
        // Perform cancel processing
        DWCi_DoCancelMatching();
        return TRUE;
    }
    else {
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Now unable to cancel.\n");
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
  Matchmaking cancellation function asynchronous version (obsolete function)
  arguments: None
  return values: TRUE: Cancellation started, FALSE: Matchmaking not in progress
  Application: Cancels matchmaking processes in progress.
          After performing the necessary cancellation, the matchmaking completion callback
          is called.
          -> Use DWC_CancelMatching()
 *---------------------------------------------------------------------------*/
BOOL DWC_CancelMatchingAsync(void)
{

    if (DWC_IsValidCancelMatching()){
        // Proceed to reserved status for cancel processing
        DWCi_DoCancelMatching();
        return TRUE;
    }
    else {
        // [arakit] main 051013
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Now unable to cancel.\n");
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
  matchmaking cancellation enabled confirmation function
  arguments: None
  return values: TRUE: Cancellation is possible, FALSE: Cannot abort because matchmaking is not in progress
          or not in status that allows cancellation
  Application: Checks whether or not the matchmaking in progress can be cancelled.
 *---------------------------------------------------------------------------*/
BOOL DWC_IsValidCancelMatching(void)
{

    if (DWCi_IsError()) return FALSE;

    if (!DWCi_GetMatchCnt() ||
        (DWC_GetState() != DWC_STATE_MATCHING) ||
        (DWCi_GetMatchCnt()->cancelState != DWC_MATCH_CANCEL_STATE_INIT)){
        return FALSE;
    }
    else {
        return TRUE;
    }
}


#ifdef DWC_STOP_SC_SERVER
/*---------------------------------------------------------------------------*
  server-client matchmaking server stopping function
  arguments: callback: the stop completion callback
          param: Parameters for the callback
  return values: TRUE: matchmaking stop begun. When complete, the callback is returned.
          FALSE: stop failed from an error or because the FriendsMatch Library is non-operational.
  application: stops server-client matchmaking, and prevents any
          subsequent connections from being received.
 *---------------------------------------------------------------------------*/
BOOL DWC_StopSCMatchingAsync(DWCStopSCCallback callback, void* param)
{

    if (DWCi_IsError() || !DWCi_GetMatchCnt() ||
        (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV) ||
        ((DWC_GetState() != DWC_STATE_MATCHING) && (DWC_GetState() != DWC_STATE_CONNECTED)))
        return FALSE;

    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_WAITING){
        // if not in matchmaking, by making the maximum number of connections equal to
        // the current number, subsequent connections can be denied
        DWCi_GetMatchCnt()->qr2NumEntry = DWCi_GetMatchCnt()->gt2NumConnection;
        (void)DWCi_GPSetServerStatus();

        if (callback) callback(param);  // Callback call
    }
    else {
        DWCi_GetMatchCnt()->stopSCCallback = callback;
        DWCi_GetMatchCnt()->stopSCParam    = param;
        DWCi_GetMatchCnt()->stopSCFlag     = 1;
    }

    return TRUE;
}
#endif


/*---------------------------------------------------------------------------*
  matchmaking index key addition function int version
  Arguments: keyID  Pass the key ID here when setting a key ID.
                       If obtaining a new keyID, set a value below
                       DWC_QR2_GAME_KEY_START (e.g., 0)
                       If an additional key ID that is already used is specified, if the passed keyString matches that in the key ID being used, valueSrc will be reset. But if a different keyString is passed, it does nothing.
                       If the same keyString is passed, valueSrc is reset but,
                       if a different keyString is passed, nothing occurs.
          keyString    Pointer to character string that identifies the key.
                       This character string is copied and stored within the library.
          valueSrc     Pointer to the value corresponding to the key, when there is polling
                       from the master server, the value is referenced using this pointer
                       and loaded to the master server.
  return values: Key ID of the added key. Returns 0 when a key could not be added because
          of a parameter error or the number of keys exceeds the limit
          If called before calling DWC_InitFriendsMatch(), zero is returned.
  Application: Adds a key (int type) for the matchmaking index.
 *---------------------------------------------------------------------------*/
u8  DWC_AddMatchKeyInt(u8 keyID,
                       const char* keyString,
                       const int* valueSrc)
{
    int index;

    if (!keyString || !valueSrc) return 0;

    if ((keyID >= DWC_QR2_GAME_KEY_START) &&
        stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyID){
        // If a game-defined key ID already defined by the game is specified
        if ((stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyStr != NULL) &&
            (strcmp(stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyStr, keyString) != 0)){
            // If the key name is different from that already defined, do nothing.
            return 0;
        }
        // If the key name matches that already defined, advance the process
        // and replace the pointer.
    }
    else {
        keyID = DWCi_GetNewMatchKey();  // Gets an available key ID
        if (!keyID) return 0;
    }

    // Registers new key data
    index = keyID-DWC_QR2_GAME_KEY_START;
    stGameMatchKeys[index].keyID  = keyID;
    stGameMatchKeys[index].isStr  = 0;
    stGameMatchKeys[index].pad    = 0;

    // Frees any old key names that may be laying around
    if (stGameMatchKeys[index].keyStr)
        DWC_Free(DWC_ALLOCTYPE_BASE, stGameMatchKeys[index].keyStr, 0);

    // Memory is allocated and a text string is set for the key name
    stGameMatchKeys[index].keyStr = DWC_Alloc(DWC_ALLOCTYPE_BASE, strlen(keyString)+1);
    if (!stGameMatchKeys[index].keyStr){
        DWCi_StopMatching(DWC_ERROR_FATAL, DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_ALLOC);
        return 0;
    }

    strcpy(stGameMatchKeys[index].keyStr, keyString);

    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "DWC_AddMatchKeyInt: key='%s', value=%d\n",
               keyString, *valueSrc);
    
    stGameMatchKeys[index].value  = valueSrc;

    // Set custom key
    qr2_register_key(keyID, stGameMatchKeys[index].keyStr);

    return keyID;
}


/*---------------------------------------------------------------------------*
  Matchmaking index key addition function string version
  Arguments: keyID  Pass the key ID here when setting a key ID.
                       If obtaining a new keyID, set a value below
                       DWC_QR2_GAME_KEY_START (e.g., 0)
                       If an additional key ID that is already used is specified, if the passed keyString matches that in the key ID being used, valueSrc will be reset. But if a different keyString is passed, it does nothing.
                       If the same keyString is passed, valueSrc is reset but,
                       if a different keyString is passed, nothing occurs.
          keyString    Pointer to character string that identifies the key.
                       This character string is copied and stored within the library.
          valueSrc     Pointer to the string corresponding to the key, when there is polling
                       from the master server, the string is referenced using this pointer
                       and loaded to the master server.
  return values: Key ID of the added key. Returns 0 when a key could not be added because
          of a parameter error or the number of keys exceeds the limit
          If called before calling DWC_InitFriendsMatch(), zero is returned.
  Application: Adds a key (char* type) for the matchmaking index.
 *---------------------------------------------------------------------------*/
u8  DWC_AddMatchKeyString(u8 keyID,
                          const char* keyString,
                          const char* valueSrc)
{
    int index;

    if (!keyString || !valueSrc) return 0;

    if ((keyID >= DWC_QR2_GAME_KEY_START) &&
        stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyID){
        // If a game-defined key ID already defined by the game is specified
        if ((stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyStr != NULL) &&
            (strcmp(stGameMatchKeys[keyID-DWC_QR2_GAME_KEY_START].keyStr, keyString) != 0)){
            // If the key name is different from that already defined, do nothing.
            return 0;
        }
        // If the key name matches that already defined, advance the process
        // and replace the pointer.
    }
    else {
        keyID = DWCi_GetNewMatchKey();  // Gets an available key ID
        if (!keyID) return 0;
    }

    // Registers new key data
    index = keyID-DWC_QR2_GAME_KEY_START;
    stGameMatchKeys[index].keyID      = keyID;
    stGameMatchKeys[index].isStr      = 1;
    stGameMatchKeys[index].pad        = 0;

    // Frees any old key names that may be laying around
    if (stGameMatchKeys[index].keyStr)
        DWC_Free(DWC_ALLOCTYPE_BASE, stGameMatchKeys[index].keyStr, 0);

    // Memory is allocated and a text string is set for the key name
    stGameMatchKeys[index].keyStr = DWC_Alloc(DWC_ALLOCTYPE_BASE, strlen(keyString)+1);
    if (!stGameMatchKeys[index].keyStr){
        DWCi_StopMatching(DWC_ERROR_FATAL, DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_ALLOC);
        return 0;
    }

    strcpy(stGameMatchKeys[index].keyStr, keyString);

    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "DWC_AddMatchKeyString: key='%s' value='%s'\n",
               keyString, valueSrc);
    
    stGameMatchKeys[index].value      = valueSrc;

    // Set custom key
    qr2_register_key(keyID, stGameMatchKeys[index].keyStr);

    return keyID;
}


/*---------------------------------------------------------------------------*
  Get matchmaking index key function int version
  arguments: index: Index for matchmaking candidate player. Passes the index argument
                    from the evaluation callback.
          keyString    Pointer to character string that identifies the key
          idefault  Default value when the player did not have the
                    specified key
  return values: The value of the specified key possessed by a matchmaking candidate player.
          When there is no key, the value passed to the idefault argument is returned.
  Application: Obtains the value corresponding to the matchmaking index key.
          Use is guaranteed only within the player evaluation callback,
          the return value is indeterminate when call in any other situation.
 *---------------------------------------------------------------------------*/
int DWC_GetMatchIntValue(int index, const char* keyString, int idefault)
{
    SBServer server;

    if (!DWCi_GetMatchCnt() || DWCi_IsError()) return idefault;

    server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, index);

    if (!server) return idefault;
    
    return SBServerGetIntValue(server, keyString, idefault);
}


/*---------------------------------------------------------------------------*
  Get matchmaking index key function string version
  arguments: index: Index for matchmaking candidate player. Passes the index argument
                    from the evaluation callback.
          keyString    Pointer to character string that identifies the key
          sdefault  Default string when the player did not have the
                    specified key
  return values: The string for the specified key possessed by the matchmaking candidate player.
          When there is no key, the string passed to the sdefault argument is returned.
  Application: Obtains the string corresponding to the matchmaking index key.
          Use is guaranteed only within the player evaluation callback,
          the return value is indeterminate when call in any other situation.
 *---------------------------------------------------------------------------*/
const char* DWC_GetMatchStringValue(int index, const char* keyString, const char* sdefault)
{
    SBServer server;

    if (!DWCi_GetMatchCnt() || DWCi_IsError()) return sdefault;

    server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, index);

    if (!server) return sdefault;
    
    return SBServerGetStringValue(server, keyString, sdefault);
}


/*---------------------------------------------------------------------------*
  Get matchmaking type function
  arguments: None
  return values: Last performed matchmaking type. Defined with DWC_MATCH_TYPE_*.
          Returns -1 when offline.
  Application: Obtains the last performed matchmaking type.
 *---------------------------------------------------------------------------*/
int DWC_GetLastMatchingType(void)
{

    if (DWCi_GetMatchCnt()) return DWCi_GetMatchCnt()->qr2MatchType;
    else return -1;
}


/*---------------------------------------------------------------------------*
  Matchmaking option set function
  Arguments: opttype:  Matchmaking option type
          optval:  Pointer to the matchmaking option value.
                  Passes the pointer to the type corresponding to opttype.
          optlen  Only when the type is an array, the array size must be passed.
  return values: DWC_SET_MATCH_OPT_RESULT_* type enumerator
  Application: Sets various options for matchmaking.
 *---------------------------------------------------------------------------*/
int DWC_SetMatchingOption(DWCMatchOptType opttype, const void* optval, int optlen)
{
#pragma unused(optlen)
    DWCMatchOptMinComplete* pMinComp;

    // [arakit] main 051013
    DWC_Printf(DWC_REPORTFLAG_DEBUG,
               "!!DWC_SetMatchingOption() was called!! type %d\n",
               opttype);

    if (!DWCi_GetMatchCnt()) return DWC_SET_MATCH_OPT_RESULT_E_BAD_STATE;
    if (!optval) return DWC_SET_MATCH_OPT_RESULT_E_PARAM;
    
    switch (opttype){
    case DWC_MATCH_OPTION_MIN_COMPLETE:
        // Even if the specified number of players do not gather, this option allows matchmaking complete to be set for a set period of time.
        // optval is accepted by the DWCMatchOptMinComplete structure.
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_POLL_TIMEOUT){
            // What to do if called right during processing for timeout polling
            return DWC_SET_MATCH_OPT_RESULT_E_BAD_STATE;
        }
        
        pMinComp = (DWCMatchOptMinComplete *)optval;
        // A parameter error results if the minimum number of player is 1 or less and timeout is 0
        if (pMinComp->valid && (pMinComp->minEntry <= 1))
            return DWC_SET_MATCH_OPT_RESULT_E_PARAM;

        if (!stpOptMinComp){
            // Use Alloc if the structure does not exist
            stpOptMinComp =
                DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCMatchOptMinCompleteIn));
            if (!stpOptMinComp) return DWC_SET_MATCH_OPT_RESULT_E_ALLOC;
        }

        stpOptMinComp->valid        = pMinComp->valid;
        stpOptMinComp->minEntry     = pMinComp->minEntry;
        stpOptMinComp->retry        = 0;
        stpOptMinComp->pad          = 0;
        stpOptMinComp->timeout      = pMinComp->timeout;
        stpOptMinComp->recvBit      = 0;
        stpOptMinComp->timeoutBit   = 0;
        stpOptMinComp->startTime    = OS_GetTick();
        stpOptMinComp->lastPollTime = OS_GetTick();
        return DWC_SET_MATCH_OPT_RESULT_SUCCESS;

    case DWC_MATCH_OPTION_SC_CONNECT_BLOCK:
        // In server-client matchmaking, the option to not receive other
        // connections after a single matchmaking completion has occurred.
        // optval is accepted by BOOL.
        // Since this setting is required before matchmaking begins,
        // no determination is performed where non-server-client matchmaking cannot be set.
        if (*(BOOL *)optval) stOptSCBlock.valid = 1;
        else                 stOptSCBlock.valid = 0;
        stOptSCBlock.lock  = 0;
        return DWC_SET_MATCH_OPT_RESULT_SUCCESS;

    default:
        return DWC_SET_MATCH_OPT_RESULT_E_INVALID;
    }
}


/*---------------------------------------------------------------------------*
  matchmaking options get function
  Arguments: opttype:  Matchmaking option type
          optval:  Pointer to location where matchmaking options are stored.
                  Passes the pointer to the type corresponding to opttype.
          optlen  Location where the size of the obtained value for the option is stored
  return values: DWC_SET_MATCH_OPT_RESULT_* type enumerator
  Application: Gets the options being set for matchmaking.
          When the specified options are not set, nothing is done
          and zero is stored in the optlen argument.
 *---------------------------------------------------------------------------*/
int DWC_GetMatchingOption(DWCMatchOptType opttype, void* optval, int* optlen)
{

    if (!DWCi_GetMatchCnt()) return DWC_SET_MATCH_OPT_RESULT_E_BAD_STATE;
    if (!optval) return DWC_SET_MATCH_OPT_RESULT_E_PARAM;
    
    switch (opttype){
    case DWC_MATCH_OPTION_MIN_COMPLETE:
        // even if the specified number of players do not gather, this option allows matchmaking complete to be set for a set period of time
        if (!stpOptMinComp){
            if (optlen) *optlen = 0;
        }
        else {
            DWCMatchOptMinComplete* pVal = optval;
            pVal->valid    = stpOptMinComp->valid;
            pVal->minEntry = stpOptMinComp->minEntry;
            pVal->timeout  = stpOptMinComp->timeout;
            pVal->pad[0] = pVal->pad[1] = 0;
            if (optlen) *optlen = sizeof(DWCMatchOptMinComplete);
        }
        return DWC_SET_MATCH_OPT_RESULT_SUCCESS;

    case DWC_MATCH_OPTION_SC_CONNECT_BLOCK:
        // In server-client matchmaking, the option to not receive other
        // connections after a single matchmaking completion has occurred.
        // Returns whether or not the option is valid.
        if (stOptSCBlock.valid == 1) *(BOOL *)optval = TRUE;
        else                         *(BOOL *)optval = FALSE;
        if (optlen) *optlen = sizeof(BOOL);
        return DWC_SET_MATCH_OPT_RESULT_SUCCESS;

    default:
        return DWC_SET_MATCH_OPT_RESULT_E_INVALID;
    }
}


/*---------------------------------------------------------------------------*
  Completion option for matchmaking less than the specified number/get status function
  Arguments: time:  Pointer to location for time elapsed from start of matchmaking (in msec).
               If the time is unnecessary, pass NULL.
  return values: 0: Within the timeout time period
          1: Timeout period expired; possible to complete matchmaking with fewer than the specified number
          -1: This matchmaking option is not set or is OFF.
  Application: Obtains the time period for the matchmaking completion option with fewer than the specified number of participants
 *---------------------------------------------------------------------------*/
int DWC_GetMOMinCompState(u64* time)
{
    u64 passTime;

    if (!stpOptMinComp || !stpOptMinComp->valid) return -1;

    passTime = OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime);
    if (time) *time = passTime;  // Sets the elapsed time

    if (passTime >= stpOptMinComp->timeout) return 1;
    else return 0;
}


/*---------------------------------------------------------------------------*
  Function for getting the matchmaking block option status
  arguments: None
  Return values: TRUE: Currently blocking newly connected clients,
          FALSE: Not currently blocking or the option has not been set
  Use: Gets the blocking status of the matchmaking block option
 *---------------------------------------------------------------------------*/
BOOL DWC_GetMOSCConnectBlockState(void)
{

    if ((stOptSCBlock.valid == 0) || (stOptSCBlock.lock == 0)){
        // Returns false if the option has not been set and is not currently blocked
        return FALSE;
    }
    else {
        return TRUE;
    }
}


/*---------------------------------------------------------------------------*
  Function for releasing matchmaking block
  arguments: None
  return values: None
  Use: Releases the blocking status of the matchmaking block option
 *---------------------------------------------------------------------------*/
void DWC_ClearMOSCConnectBlock(void)
{

    // Release the block on new connections
    stOptSCBlock.lock = 0;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "[OPT_SC_BLOCK] ClearMOSCConnectBlock\n");
}


/*---------------------------------------------------------------------------*
  matchmaking internal status get function
  arguments: None
  return values: current internal state for matchmaking
  Application: Gets the internal state of the matchmaking in progress. Used only to find out the internal state
          during matchmaking for debugging purposes.
 *---------------------------------------------------------------------------*/
DWCMatchState DWC_GetMatchingState(void)
{

    if (DWCi_GetMatchCnt()){
        return DWCi_GetMatchCnt()->state;
    }
    else {
        return DWC_MATCH_STATE_INIT;
    }
}


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Matchmaking control structure initialization function
  Arguments: matchcnt: Pointer to matchmaking control object
          pGpObj: Pointer to the GP connection structure
          pGt2Socket: Pointer to the gt2 socket structure
          gt2Callbacks: Pointer to the GT2 callbacks
          gameName: Game name received from GameSpy
          secretKey: Secret key received from GameSpy
          friendList    Friend roster (NULL is OK if not used by the game)
          friendListLen: Length of friend roster
  return values: None
  application: Initializes the matchmaking control structure
 *---------------------------------------------------------------------------*/
void DWCi_MatchInit(DWCMatchControl* matchcnt,
                    GPConnection pGpObj,
                    GT2Socket* pGt2Socket,
                    GT2ConnectionCallbacks* gt2Callbacks,
                    const char* gameName,
                    const char* secretKey,
                    const DWCFriendData friendList[],
                    int  friendListLen)
{

    SDK_ASSERT(matchcnt);

	DWCi_SetMatchCnt(matchcnt); //stpMatchCnt = matchcnt;

    DWCi_GetMatchCnt()->pGpObj            = pGpObj;
    DWCi_GetMatchCnt()->pGt2Socket        = pGt2Socket;
    DWCi_GetMatchCnt()->gt2Callbacks      = gt2Callbacks;
    DWCi_GetMatchCnt()->qr2Obj            = NULL;
    DWCi_GetMatchCnt()->qr2IP             = 0;
    DWCi_GetMatchCnt()->qr2Port           = 0;
    DWCi_GetMatchCnt()->sbObj             = NULL;
    
    DWCi_SetMatchStatus(DWC_MATCH_STATE_INIT);
    DWCi_GetMatchCnt()->pad1              = 0;
    DWCi_GetMatchCnt()->pad2              = 0;
#ifdef DWC_STOP_SC_SERVER
    DWCi_GetMatchCnt()->pad3              = 0;
    DWCi_GetMatchCnt()->pad4              = 0;
#endif
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
    DWCi_GetMatchCnt()->friendAcceptBit   = 0;
#endif
    DWCi_GetMatchCnt()->profileID         = 0;
    DWCi_GetMatchCnt()->gameName          = gameName;
    DWCi_GetMatchCnt()->secretKey         = secretKey;

    DWCi_GetMatchCnt()->friendList        = friendList;
    DWCi_GetMatchCnt()->friendListLen     = friendListLen;
    MI_CpuClear8(DWCi_GetMatchCnt()->friendIdxList, DWC_MAX_MATCH_IDX_LIST);
    DWCi_GetMatchCnt()->friendIdxListLen  = 0;

    DWCi_GetMatchCnt()->matchedCallback   = NULL;
    DWCi_GetMatchCnt()->matchedParam      = NULL;
    DWCi_GetMatchCnt()->evalCallback      = NULL;
    DWCi_GetMatchCnt()->evalParam         = NULL;
#ifdef DWC_STOP_SC_SERVER
    DWCi_GetMatchCnt()->stopSCCallback    = NULL;
    DWCi_GetMatchCnt()->stopSCParam       = NULL;
#endif

    // Initialize key data array for the QR2 defined by the game
    DWCi_ClearGameMatchKeys();

    stOptSCBlock.valid = 0;
    stOptSCBlock.lock  = 0;
    stOptSCBlock.pad   = 0;

    // Parameters requiring initialization for each matchmaking process are all initialized together here
    DWCi_ResetMatchParam(DWC_MATCH_RESET_ALL);
}


/*---------------------------------------------------------------------------*
  Function for starting QR2
  arguments: profileID: The profile ID of this host
  return values: QR2 error type
  application: Starts QR2. Retries even after one failure.
 *---------------------------------------------------------------------------*/
qr2_error_t DWCi_QR2Startup(int profileID)
{
    int i;
    qr2_error_t qr2Error = e_qrnoerror;

    if (DWCi_GetMatchCnt()->qr2Obj){
        // Does nothing if control arrives here even though QR2 has already been started
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "QR2 is already set up.\n");
        return e_qrnoerror;
    }

    DWCi_GetMatchCnt()->profileID = profileID;

    for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
        // QR2 initialization
        qr2Error = qr2_init_socket(&DWCi_GetMatchCnt()->qr2Obj,
                                   gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
                                   gt2GetLocalPort(*DWCi_GetMatchCnt()->pGt2Socket),
                                   DWCi_GetMatchCnt()->gameName, DWCi_GetMatchCnt()->secretKey, 1, 1,
                                   DWCi_QR2ServerKeyCallback,
                                   DWCi_QR2PlayerKeyCallback,
                                   DWCi_QR2TeamKeyCallback,
                                   DWCi_QR2KeyListCallback,
                                   DWCi_QR2CountCallback,
                                   DWCi_QR2AddErrorCallback,
                                   NULL);
        if (!qr2Error) break;

        // Retries a set number of times if a DNS error occurs
        if ((qr2Error != e_qrdnserror) || (i == DWC_DNS_ERROR_RETRY_MAX-1)){
            DWCi_HandleQR2Error(qr2Error);
            return qr2Error;
        }
    }

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    // Initialize the public IP and port number for QR2 of the local host
    DWCi_GetMatchCnt()->qr2IP   = 0;
    DWCi_GetMatchCnt()->qr2Port = 0;
#endif

    // Register function for public address notification callback
    qr2_register_publicaddress_callback(DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2PublicAddrCallback);
        
    // Register function for NAT negotiation callback
    qr2_register_natneg_callback(DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2NatnegCallback);

    // Register function for client message received callback
    qr2_register_clientmessage_callback(DWCi_GetMatchCnt()->qr2Obj, DWCi_QR2ClientMsgCallback);

    // Notifies the master server of changes in status
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

    return qr2Error;
}


/*---------------------------------------------------------------------------*
  no friend specification peer matchmaking beginning function
  arguments: numEntry: Number of connection requests (not including self)
          addFilter: Filter for SB server update to be added by game.
          matchedCallback  Callback when matchmaking is complete
          matchedParam   Call-back parameter for above
          evalCallback   Player evaluation callback
          evalParam   Call-back parameter for above
  return values: None
  application: Starts friend specified peer matchmaking
 *---------------------------------------------------------------------------*/
void DWCi_ConnectToAnybodyAsync(u8  numEntry,
                                const char* addFilter,
                                DWCMatchedSCCallback matchedCallback,
                                void* matchedParam,
                                DWCEvalPlayerCallback evalCallback,
                                void* evalParam)
{
    char filter[MAX_FILTER_LEN];
    u32  addFilterLenMax;
    SBError sbError;

    // Frees any old filter names that may be laying around
    if (stpAddFilter){
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    // Checks whether or not the additional filter length has exceeded the limit on the number of characters allowed
    if (addFilter){
        // Calculates the maximum number of characters allowed (including NULL termination) for the additional filter
        addFilterLenMax = MAX_FILTER_LEN-DWCi_GetDefaultMatchFilter(filter, 0xffffffff, DWC_MAX_CONNECTIONS, DWC_MATCH_TYPE_SC_CL)-strlen(" and ()");

        SDK_ASSERT(strlen(addFilter) < addFilterLenMax);

        stpAddFilter = DWC_Alloc(DWC_ALLOCTYPE_BASE, addFilterLenMax);
        if (!stpAddFilter){
            DWCi_StopMatching(DWC_ERROR_FATAL, DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_ALLOC);
            return;
        }

        MI_CpuCopy8(addFilter, stpAddFilter, addFilterLenMax);
        stpAddFilter[addFilterLenMax-1] = '\0';  // Guarantees NULL termination
    }

    // Common parameter set used for matchmaking
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_ANYBODY,
                             numEntry, matchedCallback, matchedParam);

    DWCi_GetMatchCnt()->evalCallback = evalCallback;
    DWCi_GetMatchCnt()->evalParam    = evalParam;

    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_OWN);

    // SB initialization
    if ( !DWCi_GetMatchCnt()->sbObj )
            DWCi_GetMatchCnt()->sbObj = ServerBrowserNew(DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->secretKey, 0, 20,
                                                  QVERSION_QR2, SBFalse,
                                                  DWCi_SBCallback, NULL);
    if (!DWCi_GetMatchCnt()->sbObj){
        // Probably a memory allocation error
        if (DWCi_HandleSBError(sbe_allocerror)) return;
    }

    // Sets GP status during matchmaking without friend specification (errors are ignored)
    (void)DWCi_SetGPStatus(DWC_STATUS_MATCH_ANYBODY, "", NULL);

    // Performs an SB server update and checks that changes are reflected in data for the local host
    sbError = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->profileID);
    if (DWCi_HandleSBError(sbError)) return;

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (DWCi_GetMatchCnt()->qr2Obj == NULL){
        // If QR2 hasn't been started, start it here
        DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID);
    }

#else
    // Notifies the master server of changes in status
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
#endif

    // Initialization for matchmaking complete options for the number of specified players or less
    DWCi_InitOptMinCompParam(FALSE);
}


/*---------------------------------------------------------------------------*
  friend specification peer matchmaking beginning function
  arguments: friendIdxList: Pointer to friend index list used during connection requests
          friendIdxListLen  Connection request friend index list length
          numEntry: Number of connection requests (not including self)
          distantFriend    TRUE: Allows connection by friends of a friend, FALSE: Such connections are not allowed
          matchedCallback  Callback when matchmaking is complete
          matchedParam   Call-back parameter for above
          evalCallback   Player evaluation callback
          evalParam   Call-back parameter for above
  return values: None
  application: Starts friend specified peer matchmaking
 *---------------------------------------------------------------------------*/
void DWCi_ConnectToFriendsAsync(const u8 friendIdxList[],
                                int friendIdxListLen,
                                u8  numEntry,
                                BOOL distantFriend,
                                DWCMatchedSCCallback matchedCallback,
                                void* matchedParam,
                                DWCEvalPlayerCallback evalCallback,
                                void* evalParam)
{
    char valueStr[12], keyValueStr[32];
#ifndef DWC_MATCH_ACCEPT_NO_FRIEND
    int  checkpid;
    int  friendCount;
    int  i;
#endif
    int  result;
    GPResult gpResult;

    // Common parameter set used for matchmaking
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_FRIEND,
                             numEntry, matchedCallback, matchedParam);

    DWCi_GetMatchCnt()->distantFriend = (u8)(distantFriend ? 1 : 0);
    DWCi_GetMatchCnt()->evalCallback  = evalCallback;
    DWCi_GetMatchCnt()->evalParam     = evalParam;

    // Copy friend index list
#ifdef DWC_MATCH_ACCEPT_NO_FRIEND
    MI_CpuCopy8(friendIdxList, DWCi_GetMatchCnt()->friendIdxList, (u32)friendIdxListLen);
    DWCi_GetMatchCnt()->friendIdxListLen = friendIdxListLen;

    if (!friendIdxListLen ||
        ((friendIdxListLen < numEntry) &&
         !distantFriend &&
         (!stpOptMinComp || !stpOptMinComp->valid || (friendIdxListLen < stpOptMinComp->minEntry-1)))){
        // An insufficient number of friends error results, if the number of friends is fewer
        // than the number of connected players requested, and the number of friends
        // is fewer than the minimum number of players with a matchmaking complete option
        // for the specified number of players or less under a setting where friends of friends are not allowed. Also an error if number of valid friends is 0.
        DWCi_StopMatching(DWC_ERROR_FRIENDS_SHORTAGE, 0);
        return;
    }

#else
    for (i = 0, friendCount = 0; i < friendIdxListLen; i++){
        if (!(checkpid = DWCi_GetProfileIDFromList(friendIdxList[i])) ||
            !gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, checkpid)){
            // The player is removed from the list because the index specified does not have
            // a Profile ID or the other party is not a friend.
            continue;
        }

        DWCi_GetMatchCnt()->friendIdxList[friendCount] = friendIdxList[i];
        friendCount++;
    }

    if (!friendCount ||
        ((friendCount < numEntry) &&
         !distantFriend &&
         (!stpOptMinComp || !stpOptMinComp->valid || (friendCount < stpOptMinComp->minEntry-1)))){
        // An insufficient number of friends error results, if the number of friends is fewer
        // than the number of connected players requested, and the number of friends
        // is fewer than the minimum number of players with a matchmaking complete option
        // for the specified number of players or less under a setting where friends of friends are not allowed. Also an error if number of valid friends is 0.
        // [todo]
        // This error will no longer be used once code is changed so that
        // friends established during matchmaking are added to the index list.
        // -> abandoned
        DWCi_StopMatching(DWC_ERROR_FRIENDS_SHORTAGE, 0);
        return;
    }

    DWCi_GetMatchCnt()->friendIdxListLen = friendCount;
#endif
    
    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

    // SB initialization
    if ( !DWCi_GetMatchCnt()->sbObj )
            DWCi_GetMatchCnt()->sbObj = ServerBrowserNew(DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->secretKey, 0, 20,
                                                  QVERSION_QR2, SBFalse,
                                                  DWCi_SBCallback, NULL);
    if (!DWCi_GetMatchCnt()->sbObj){
        // Probably a memory allocation error
        if (DWCi_HandleSBError(sbe_allocerror)) return;
    }

    // Creates a GP status string that indicates whether or not the desired number of friends are connected and if friends of friends are allowed
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", numEntry);
    (void)DWC_SetCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_FRIEND_NUM,
                                      valueStr, keyValueStr, '/');
    (void)DWC_AddCommonKeyValueString(DWC_GP_SSTR_KEY_DISTANT_FRIEND,
                                      distantFriend ? "Y" : "N",
                                      keyValueStr, '/');

    // Set the matchmaking version.
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", DWC_MATCHING_VERSION);
    (void)DWC_AddCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_VERSION,
                                      valueStr, keyValueStr, '/');

    // GP status is handled during matchmaking with a friend specification and the GP status string created
    // is also set.
    gpResult = DWCi_SetGPStatus(DWC_STATUS_MATCH_FRIEND, keyValueStr, NULL);
    if (DWCi_HandleGPError(gpResult)) return;

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (DWCi_GetMatchCnt()->qr2Obj == NULL){
        // If QR2 hasn't been started, start it here
        // The qr2IP and qr2Port must be initialized before the reservation command is sent.
        if (DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID)) return;
    }

#else
    // Notifies the master server of changes in status
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
#endif

    // Make NAT negotiation reservation for first server using a GP message.
    // This part of code ensures that the Profile ID definitely corresponds to a friend.
    result = DWCi_SendResvCommandToFriend(FALSE, TRUE, 0);
    if (DWCi_HandleMatchCommandError(result)) return;

    // Initialization for matchmaking complete options for the number of specified players or less
    DWCi_InitOptMinCompParam(FALSE);
}


/*---------------------------------------------------------------------------*
  server-client matchmaking server starting function
  arguments: MaxEntry: Maximum number of connected players (not including self)
          matchedCallback  Callback when matchmaking is complete
          matchedParam   Call-back parameter for above
          newClientCallback Newly connected client notification callback
          newClientParam   Call-back parameter for above
  return values: None
  application: begins the server-client matchmaking server and the matchmaking process
 *---------------------------------------------------------------------------*/
// [arakit] main 051027
void DWCi_SetupGameServer(u8   maxEntry,
                          DWCMatchedSCCallback matchedCallback,
                          void* matchedParam,
                          DWCNewClientCallback newClientCallback,
                          void* newClientParam)
{
    GPResult gpResult;

    // Common parameter set used for matchmaking
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_SC_SV,
                             maxEntry, matchedCallback, matchedParam);

    // Set the callback to use for notification regarding newly connected clients
    DWCi_GetMatchCnt()->newClientCallback = newClientCallback;
    DWCi_GetMatchCnt()->newClientParam    = newClientParam;

    // Put the Profile ID for the local host (self) first on the Profile ID list
    DWCi_GetMatchCnt()->sbPidList[0] = DWCi_GetMatchCnt()->profileID;

    // The AID for the local host is always valid from the beginning
    DWCi_GetMatchCnt()->validAidBitmap  = 1;
    DWCi_GetMatchCnt()->gt2NumValidConn = 0;

    // Always release any block on new connections due to the matchmaking option before starting matchmaking
    stOptSCBlock.lock  = 0;

    DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAITING);

    // Set GP status
    gpResult = DWCi_GPSetServerStatus();
    if (DWCi_HandleGPError(gpResult)) return;

    // [arakit] main 051008
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (DWCi_GetMatchCnt()->qr2Obj == NULL){
        // If QR2 hasn't been started, start it here
        DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID);
    }

#else
    // Notifies the master server of changes in status
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
#endif
    // [arakit] main 051008
}


/*---------------------------------------------------------------------------*
  server-client matchmaking server connection starting function
  arguments: serverPid: the server's profile ID
          matchedCallback  Callback when matchmaking is complete
          matchedParam   Call-back parameter for above
          newClientCallback Newly connected client notification callback
          newClientParam   Call-back parameter for above
  return values: None
  application: begins matchmaking from the server-client
          matchmaking client
 *---------------------------------------------------------------------------*/
void DWCi_ConnectToGameServerAsync(int serverPid,
                                   DWCMatchedSCCallback matchedCallback,
                                   void* matchedParam,
                                   DWCNewClientCallback newClientCallback,
                                   void* newClientParam)
{
    int result;
    GPResult gpResult;

    // Common parameter set used for matchmaking
    DWCi_SetMatchCommonParam(DWC_MATCH_TYPE_SC_CL,
                             0, matchedCallback, matchedParam);

    // Set the callback to use for notification regarding newly connected clients
    DWCi_GetMatchCnt()->newClientCallback = newClientCallback;
    DWCi_GetMatchCnt()->newClientParam    = newClientParam;

    // Record that this is a client
    DWCi_GetMatchCnt()->qr2IsReserved  = 1;
    DWCi_GetMatchCnt()->qr2Reservation = DWCi_GetMatchCnt()->profileID;

    // Set the Profile ID of the server on the list
    DWCi_GetMatchCnt()->sbPidList[0] = serverPid;

    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

    // SB initialization
    if ( !DWCi_GetMatchCnt()->sbObj )
            DWCi_GetMatchCnt()->sbObj = ServerBrowserNew(DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->gameName,
                                                  DWCi_GetMatchCnt()->secretKey,
                                                  0, 20,
                                                  QVERSION_QR2, SBFalse,
                                                  DWCi_SBCallback, NULL);
    if (!DWCi_GetMatchCnt()->sbObj){
        // Probably a memory allocation error
        if (DWCi_HandleSBError(sbe_allocerror)) return;
    }

    // Set the GP status for a client that uses server-client matchmaking
    gpResult = DWCi_SetGPStatus(DWC_STATUS_MATCH_SC_CL, "", NULL);
    if (DWCi_HandleGPError(gpResult)) return;

    // [arakit] main 051008
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    if (DWCi_GetMatchCnt()->qr2Obj == NULL){
        // If QR2 hasn't been started, start it here
        // The qr2IP and qr2Port must be initialized before the reservation command is sent.
        if (DWCi_QR2Startup(DWCi_GetMatchCnt()->profileID)) return;
    }

#else
    // Notifies the master server of changes in status
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
#endif
    // [arakit] main 051008

    // Make NAT negotiation reservation for the target server using a GP message.
    // This part of code ensures that the Profile ID definitely corresponds to a friend.
    // Also guarantees that the other party has launched the server-client
    // matchmaking server.
    result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->sbPidList[0], FALSE);
    if (DWCi_HandleMatchCommandError(result)) return;
}

/*---------------------------------------------------------------------------*
  subcontracting function for the matchmaking update function. when necessary, calls qr2_send_statechanged
  arguments: qr2obj 
  return values: None
  application: notifies the master server of state changes resulting from QR2 data reflection
 *---------------------------------------------------------------------------*/
static void DWCi_SendStateChanged(qr2_t qr2Obj)
{
  if(qr2Obj == NULL){ return; }

  qr2_think(qr2Obj);
  
  if(qr2Obj->userstatechangerequested){ return; }
  
  // The master server is notified of changes in status so that QR2 data can be updated in a timely manner as clients are searched for
  // 
  // (With this timing, actually a flag is just set)
  switch(DWCi_GetMatchCnt()->qr2MatchType){
  case DWC_MATCH_TYPE_ANYBODY:
  case DWC_MATCH_TYPE_FRIEND:
	switch(DWCi_GetMatchCnt()->state){
	case DWC_MATCH_STATE_CL_WAITING:
	case DWC_MATCH_STATE_CL_SEARCH_OWN:
	case DWC_MATCH_STATE_CL_SEARCH_HOST:
	case DWC_MATCH_STATE_CL_WAIT_RESV:
	case DWC_MATCH_STATE_CL_NN:
	case DWC_MATCH_STATE_SV_OWN_NN:
	  qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
	  break;
	}
	break;
  case DWC_MATCH_TYPE_SC_SV:
	if(DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN){
	  qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
	}
	break;
  }
}

/*---------------------------------------------------------------------------*
  matchmaking update function
  arguments: fullSpec: TRUE: Perform all processing for matchmaking
                   FALSE: Only update GT2 and QR2
  return values: None
  application: Called each game frame, this function updates matchmaking communication
 *---------------------------------------------------------------------------*/
void DWCi_MatchProcess(BOOL fullSpec)
{
  u32  resendInterval;
  u32  sbInterval;
  int  result;
  SBError sbError;
    
  if(!DWCi_GetMatchCnt() || DWCi_IsError()){ return; }
  
  if(!fullSpec){ // Only update GT2 and QR2
	if(DWCi_GetMatchCnt()->qr2Obj){ qr2_think(DWCi_GetMatchCnt()->qr2Obj); }
	if (DWCi_GetMatchCnt()->pGt2Socket){ gt2Think(*DWCi_GetMatchCnt()->pGt2Socket); }
	return;
  }

  if(DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_INIT){	return; } // Does nothing even if control arrives here during initialized status

    /*** « timeout processing  ***/
    switch (DWCi_GetMatchCnt()->state){
    case DWC_MATCH_STATE_CL_WAIT_RESV:  // NN reservation response wait status
	  //        if (DWCi_GetMatchCnt()->cmdTimeoutCount){
	  if (DWCi_GetMatchCnt()->cmdTimeoutTime > 0 ){
		// Waiting for response to NN reservation command
		//            DWCi_GetMatchCnt()->cmdTimeoutCount++;
		//            if (DWCi_GetMatchCnt()->cmdTimeoutCount >= DWC_MATCH_CMD_RESV_TIMEOUT){
		if (OS_TicksToMilliSeconds( OS_GetTick() - DWCi_GetMatchCnt()->cmdTimeoutStartTick ) > DWCi_GetMatchCnt()->cmdTimeoutTime){
		  // It's likely the server disconnected and there is no response
		  //                DWCi_GetMatchCnt()->cmdTimeoutCount = 0;
		  DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
		  
		  if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
			// for server-client matchmaking
			DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
					   "Timeout: wait server response %d/%d.\n",
					   DWCi_GetMatchCnt()->scResvRetryCount,
					   DWC_MATCH_CMD_RETRY_MAX );
			
			DWCi_GetMatchCnt()->scResvRetryCount++;
			if (DWCi_GetMatchCnt()->scResvRetryCount > DWC_MATCH_CMD_RETRY_MAX){
			  // Matchmaking ends on an error if a set number of failures occurs
			  DWCi_StopMatching(DWC_ERROR_NETWORK,
								DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_SC_CL_FAIL);
			  return;
			}
			else {
			  // Resend the reservation command
			  result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->sbPidList[0], FALSE);
			  if (DWCi_HandleMatchCommandError(result)) return;
			}
		  }
		  else {
			// for friend specified/unspecified peer matchmaking, search for the next server
			// Error processing is handled inside the function
			DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN resv(with %u) timed out. Try next server.\n", DWCi_GetMatchCnt()->sbPidList[0]);
			if (!DWCi_RetryReserving(0)) return;
		  }
		}
	  }
	  
	  //        if (DWCi_GetMatchCnt()->cmdResendCount){
	  if (DWCi_GetMatchCnt()->cmdResendFlag){
		// Waiting to resend the NN reservation command
		// If the client is already connected, the start of searching again is delayed and
		// a somewhat long wait is made for other host reservations
		resendInterval =
		  (u32)DWC_MATCH_CMD_RESEND_INTERVAL_MSEC+DWC_MATCH_CMD_RESEND_INTERVAL_ADD_MSEC*DWCi_GetMatchCnt()->gt2NumConnection;
		
		//            DWCi_GetMatchCnt()->cmdResendCount++;
		//            if (DWCi_GetMatchCnt()->cmdResendCount >= resendInterval){
		if (OS_TicksToMilliSeconds( OS_GetTick() - DWCi_GetMatchCnt()->cmdResendTick ) >= resendInterval){
		  // Resends an NN reservation command to the server
		  result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->sbPidList[0], FALSE);
		  if (DWCi_HandleMatchCommandError(result)) return;
		}
	  }
	  break;

    case DWC_MATCH_STATE_CL_SEARCH_OWN:      // Searching for information on own host
    case DWC_MATCH_STATE_CL_SEARCH_HOST:     // Searching open hosts (when using friend unspecified peer matchmaking)
    case DWC_MATCH_STATE_CL_SEARCH_NN_HOST:  // Searching for hosts for whom NN reservation is complete
	  if(DWCi_GetMatchCnt()->sbUpdateFlag <= 0){ break; }

	  if(DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_HOST){
		// If the client is already connected in a case where the server uses 
		// friend unspecified matchmaking, the start of searching again is delayed
		// and a somewhat long wait is made for the local host reservation
		sbInterval = (u32)DWC_SB_UPDATE_INTERVAL_MSEC+DWC_SB_UPDATE_INTERVAL_ADD_MSEC*DWCi_GetMatchCnt()->gt2NumConnection;
	  }else{
		if (DWCi_GetMatchCnt()->sbUpdateFlag == DWC_SB_UPDATE_INTERVAL_SHORT){
		  sbInterval = DWC_SB_UPDATE_INTERVAL_SHORT_MSEC;
		}else{
		  sbInterval = DWC_SB_UPDATE_INTERVAL_MSEC;
		}
	  }
	  // DWCi_GetMatchCnt()->sbUpdateCount++;
	  if (OS_TicksToMilliSeconds( OS_GetTick() - DWCi_GetMatchCnt()->sbUpdateTick ) > sbInterval){
		// Update the SB server
		sbError = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->reqProfileID);
		if(DWCi_HandleSBError(sbError)){ return; }
		DWCi_GetMatchCnt()->sbUpdateFlag = 0;
	  }
	  break;
    case DWC_MATCH_STATE_CL_GT2:  // Establishing GT2 connection
        if (DWCi_GetMatchCnt()->nnFinishTime){
            // (nnFinishTime > 0) results only on the NN start side
            if (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->nnFinishTime) > DWC_WAIT_GT2_CONNECT_TIMEOUT){
                // If gt2Connect is not returned from a party outside NAT for some time after 
                // NAT negotiations with that party have completed
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout: wait gt2Connect().\n");

                DWCi_GetMatchCnt()->nnFinishTime = 0;
                // all client processing is terminated and matchmaking is restarted.
                // ends in an error for server-client matchmaking.
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[0]))
                    return;
            }
        }
        else if ((DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_TELL_ADDR) &&
                 (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->cmdCnt.sendTime) > DWC_MATCH_CMD_RTT_TIMEOUT)){
                DWC_Printf(DWC_REPORTFLAG_DEBUG,
                           "RTT Timeout with DWC_MATCH_STATE_CL_GT2.\n" );
            // When gt2Connect is not returned for some time from a party inside the same NAT
            DWCi_GetMatchCnt()->cmdCnt.count++;
            if (DWCi_GetMatchCnt()->cmdCnt.count > DWC_MATCH_CMD_RETRY_MAX){
                // When the number of resend retries overflows
                DWCi_StopResendingMatchCommand();  // Stop resending command

                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Stop resending command %d.\n",
                           DWC_MATCH_COMMAND_TELL_ADDR);
                    
                // all client processing is terminated and matchmaking is restarted.
                // ends in an error for server-client matchmaking.
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[0]))
                    return;
                break;
            }

            // Resend the address notification command
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_TELL_ADDR,
                                      DWCi_GetMatchCnt()->cmdCnt.profileID,
                                      DWCi_GetMatchCnt()->cmdCnt.ip,
                                      DWCi_GetMatchCnt()->cmdCnt.port,
                                      DWCi_GetMatchCnt()->cmdCnt.data,
                                      DWCi_GetMatchCnt()->cmdCnt.len);
            if (DWCi_HandleMatchCommandError(result)) return;
        }
        break;

    case DWC_MATCH_STATE_SV_OWN_NN:  // NAT negotiation in progress
        if ((DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_RESV_OK) &&
            (((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) &&
              (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->cmdCnt.sendTime) > DWC_MATCH_RESV_KEEP_TIME_ANYBODY)) ||
             ((DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_ANYBODY) &&
              (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->cmdCnt.sendTime) > DWC_MATCH_RESV_KEEP_TIME_FRIEND)))){
                // If a client with an approved reservation still hasn't come for NN connection after all this time, the reservation is cleared and initial status restored.
                // 
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Reservation timeout. Cancel reservation.\n");

                DWCi_StopResendingMatchCommand();  // Stop resending command
                
                // accepting new connections is ended and matchmaking is restarted.
                // In the case of server-client types, this is processed by 
                // cancelling the connection of the newly connected client.
                if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1]))
                    return;
        }
        break;

    case DWC_MATCH_STATE_SV_WAIT_CL_LINK:  // Waiting for connection between clients to complete
        if ((DWCi_GetMatchCnt()->cmdCnt.command == DWC_MATCH_COMMAND_LINK_CLS_REQ) &&
            (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->cmdCnt.sendTime) > DWC_MATCH_LINK_CLS_TIMEOUT)){
            // When connection between clients just doesn't seem to end
            DWCi_GetMatchCnt()->cmdCnt.count++;
            if (DWCi_GetMatchCnt()->cmdCnt.count > 0){ // [sasakit] No retry is attempted because it would be rather meaningless.
                // When the number of resend retries overflows.
                DWCi_StopResendingMatchCommand();  // Stop resending command

                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Wait clients connecting timeout.\n");

                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                    // for the server-client matchmaking server
                    // processing continues by closing only newly connected clients
                    // since it's necessary to continue connected status
                    if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection]))
                        return;
                }
                else {
                    DWCi_RestartFromTimeout();  // matchmaking redone from the beginning
                }
                break;
            }

            // The command is resent since the connection request command between clients may have been dropped
            // Resend the command
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_REQ,
                                      DWCi_GetMatchCnt()->cmdCnt.profileID,
                                      DWCi_GetMatchCnt()->cmdCnt.ip,
                                      DWCi_GetMatchCnt()->cmdCnt.port,
                                      DWCi_GetMatchCnt()->cmdCnt.data,
                                      DWCi_GetMatchCnt()->cmdCnt.len);
            if (DWCi_HandleMatchCommandError(result)) return;
        }
        break;

    case DWC_MATCH_STATE_CL_WAITING:
        if ((DWC_GetState() == DWC_STATE_MATCHING) &&
            (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->clWaitTime) > DWC_MATCH_CL_WAIT_TIMEOUT)){
            // when a certain period of time has passed during client standby status
            // a polling packet is sent to the server
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "No data from server %d/%d.\n",
                       DWCi_GetMatchCnt()->clWaitTimeoutCount, DWC_MATCH_CMD_RETRY_MAX);
                
            if (DWCi_GetMatchCnt()->clWaitTimeoutCount >= DWC_MATCH_CMD_RETRY_MAX){
                // if there is no response from the server after five tries, redo matchmaking
                // all client processing is terminated and matchmaking is restarted.
                // ends in an error for server-client matchmaking.
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Timeout: Connection to server was shut down.\n");
                    
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[0]))
                    return;
            }
            else {
                result =
                    DWCi_SendMatchCommand(DWC_MATCH_COMMAND_CL_WAIT_POLL,
                                          DWCi_GetMatchCnt()->sbPidList[0],
                                          DWCi_GetMatchCnt()->qr2IPList[0],
                                          DWCi_GetMatchCnt()->qr2PortList[0],
                                          NULL, 0);
                if (DWCi_HandleMatchCommandError(result)) return;

                DWCi_GetMatchCnt()->clWaitTimeoutCount++;
                // as the next timeout is issues after a command's roundtrip timeout
                // has elapsed, adjust the time at which timeout measurement begins
                DWCi_GetMatchCnt()->clWaitTime =
                    OS_GetTick()-OS_MilliSecondsToTicks(DWC_MATCH_CL_WAIT_TIMEOUT-DWC_MATCH_CMD_RTT_TIMEOUT);
            }
        }
        break;

    default:
        break;
    }

    if (((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN) ||
         (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_NN)) &&
        DWCi_GetMatchCnt()->nnFailedTime &&
        (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->nnFailedTime) > DWC_WAIT_NN_RETRY_TIMEOUT)){
        // If no retry comes at all after an NN failure and a timeout occurs
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout : wait NN retry.\n");

        // the callback for completing NAT negotiations will be called on a "no
        // response from other party" error
        DWCi_NNCompletedCallback(nr_deadbeatpartner, NULL, NULL, &DWCi_GetMatchCnt()->nnInfo);
    }
    /*** ªtimeout processing ***/


    /*** «SDK update processing for each GameSpy ***/
    if (DWCi_GetMatchCnt()->sbObj){
#if 1
        // If there has been no communication with the master server for a long time, an error (sbe_connecterror)
        // is returned, but ignored here.
        // If disconnected during communication with the master server, the connection is automatically re-established.
        (void)ServerBrowserThink(DWCi_GetMatchCnt()->sbObj);
#else
        sbError = ServerBrowserThink(DWCi_GetMatchCnt()->sbObj);
        if (DWCi_HandleSBError(sbError)) return;
#endif
    }

	DWCi_SendStateChanged(DWCi_GetMatchCnt()->qr2Obj); // send a state change after each certain interval

    // Since the NATNegotiator corresponding to the cancelled cookie is
    // removed inside NNThink() after NNCancel, it is not desirable for
    // NNProcessData() to be called from UnrecognizedCallback inside gt2Think().
    // For this reason, call NNThink() ahead of gt2Think().
    NNThink();

    if (DWCi_GetMatchCnt()->pGt2Socket){
        gt2Think(*DWCi_GetMatchCnt()->pGt2Socket);
    }
    /*** ªSDK update processing for each GameSpy ***/


    /*** Other processing required for every game frame (functions which must be called after each SDK update) ***/
    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_WAIT_CLOSE) &&
        (OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->closedTime) > (DWC_MATCH_CMD_RTT_TIMEOUT >> 1))){
        // If a timeout occurs on a wait for priority reservation server notification wait when a connection is closed by another host,
        // redo matchmaking from the beginning.
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "RTT Timeout with DWCi_MatchProcess.\n" );
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Timeout : Wait prior profileID.\n");
        
        if (DWCi_ResumeMatching()) return;
    }

    // matchmaking completion synchronization adjustment
    if (!DWCi_ProcessMatchSynTimeout()) return;

    // server-client matchmaking cancellation synchronization adjustment
    if (!DWCi_ProcessCancelMatchSynTimeout()) return;

    // matchmaking complete options for the number of specified players or less
    DWCi_ProcessOptMinComp();

#ifdef DWC_STOP_SC_SERVER
    // for the server-client matchmaking server, if the matchmaking stop flag is
    // up, upon matchmaking completion, the maximum number of connection will be set to
    // the current number, and subsequent connections can be denied
    if (DWCi_GetMatchCnt()->stopSCFlag && (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_WAITING)){
        DWCi_GetMatchCnt()->qr2NumEntry = DWCi_GetMatchCnt()->gt2NumConnection;
        (void)DWCi_GPSetServerStatus();
        
        DWCi_GetMatchCnt()->stopSCFlag = 0;

        // Callback call
        if (DWCi_GetMatchCnt()->stopSCCallback)
            DWCi_GetMatchCnt()->stopSCCallback(DWCi_GetMatchCnt()->stopSCParam);
    }
#endif
}


/*---------------------------------------------------------------------------*
  Callback function to use when a message is received stating GT2 authentication was impossible
  arguments: socket: pointer to GT2 socket
          ip: Other hosts IP
          port: Port number for other host
          message: Received data
          len: Size of received data
  return values: GT2False is returned if there is no GT2 message
  application: Callback when a GT2 authentication impossible message is received
 *---------------------------------------------------------------------------*/
GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket,
                                            unsigned int ip,
                                            unsigned short port,
                                            GT2Byte* message,
                                            int len)
{
#pragma unused(socket)
	struct sockaddr_in saddr;

	if (!len || !message) return GT2False;

    //(void)memset(&saddr, 0, sizeof(saddr));
    MI_CpuClear32(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
   	saddr.sin_addr.s_addr = ip;
    saddr.sin_port = htons(port);

    if (((message[0] == QR_MAGIC_1) && (message[1] == QR_MAGIC_2))
        || (message[0] == '\\')){
        // When a QR2 message is received
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "GT2 Unrecognized : Received QR2 data.\n");
        
        // Must be NULL terminated
        // [note]
        // The fact that there is no need to check for NULL termination if the
        // GSI_UNICODE constant has not been defined, but there is processing that searches
        // for NULL termination at the start of qr2_parse_query() if it has been defined can lead to bugs.
        if (DWCi_GetMatchCnt()->qr2Obj){
         	qr2_parse_query(DWCi_GetMatchCnt()->qr2Obj, (gsi_char *)message, len, (struct sockaddr *)&saddr);
        }
        else {
            DWC_Printf( DWC_REPORTFLAG_MATCH_NN, "  ignore qr2 message.\n" );
        }
    }
    else if (!memcmp(message, NNMagicData, NATNEG_MAGIC_LEN)){
        // When an NN message is received
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "GT2 Unrecognized : Received NN data.\n");
        NNProcessData((char *)message, len, &saddr);
    }
    else if ((message[0] == 0xfe) && (message[1] == 0xfe))
    {
        // GT2Packet with no GT2Connection
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "GT2 Unrecognized : Not Connected gt2 data.\n");

        // Beware that flow will not proceed to calling ConnectAttemptCallback unless 
        // GT2False is returned here!!
        return GT2False;
    }
    else {
        // When an unrecognized message has been received or when data from an unconnected party 
        // has been received, control flow arrives here even if there is a magic
        // number included in the header of the GT2 packet.
        DWC_Printf(DWC_REPORTFLAG_WARNING, "GT2 Unrecognized : Received unrecognized data.\n");

        return GT2False;
    }

    return GT2True;
}


/*---------------------------------------------------------------------------*
  GT2 connection request received callback function
  arguments: socket: pointer to GT2 socket
          connection: Pointer to the structure for the accepted connection
          ip: Other hosts IP
          port: Port number for other host
          latency: Latency of the other host (round-trip time)
          message: Received data
          len: Size of received data
  return values: None
  application: GT2 connection request received callback
 *---------------------------------------------------------------------------*/
void DWCi_GT2ConnectAttemptCallback(GT2Socket socket,
                                    GT2Connection connection,
                                    unsigned int ip,
                                    unsigned short port,
                                    int latency,
                                    GT2Byte* message,
                                    int len)
{
#pragma unused(socket)
#pragma unused(message)
#pragma unused(len)
    int index;
    GT2Bool result;
    GT2Connection* pGt2Con;
    DWCConnectionInfo* pConInfo;

    if (!DWCi_GetMatchCnt() || (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_GT2) ||
        (DWCi_GetMatchCnt()->cancelState != DWC_MATCH_CANCEL_STATE_INIT)){
        // Reject if GT2Connect occurs immediately after a cancel
        gt2Reject(connection, (const GT2Byte *)"Init state", -1);
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Reject was called : Init state\n");
        return;
    }

    // Search for available spots on the connection list
    index = DWCi_GT2GetConnectionListIdx();
	if (index == -1){
        // Reject if connections are already full
        gt2Reject(connection, (const GT2Byte *)"Server full", -1);
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Reject was called : Server full\n");

        // matchmaking ends in an error
        DWCi_StopMatching(DWC_ERROR_NETWORK,
                          DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_CONN_OVER);
    }
    else {
        if ((ip != DWCi_GetMatchCnt()->ipList[DWCi_GetMatchCnt()->gt2NumConnection]) ||
            (port != DWCi_GetMatchCnt()->portList[DWCi_GetMatchCnt()->gt2NumConnection])){
            if (message[0] &&
                (strtoul((const char *)message, NULL, 10) == DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection])){
                // When gt2Connect() is received first, even though a NAT connection has not yet been completed,
                // If gt2Connect() is received first
                DWC_Printf(DWC_REPORTFLAG_WARNING,
                           "gt2Connect() came before NN complete.\n");

                // the IP and port are set in the list here
                DWCi_GetMatchCnt()->ipList[DWCi_GetMatchCnt()->gt2NumConnection]   = ip;
                DWCi_GetMatchCnt()->portList[DWCi_GetMatchCnt()->gt2NumConnection] = port;
            }
            else {
                // Does nothing when gt2Connect() was from a completely unrelated host
                gt2Reject(connection, (const GT2Byte *)"Unknown connect attempt", -1);
                DWC_Printf(DWC_REPORTFLAG_WARNING,
                           "gt2Reject was called : Unknown connect attempt from %s\n",
                           gt2AddressToString(ip, port, NULL));
                return;
            }
        }

        
        // Clears the time when NN has ended
        DWCi_GetMatchCnt()->nnFinishTime = 0;

        // Accept connection
	    result = gt2Accept(connection, DWCi_GetMatchCnt()->gt2Callbacks);
        if (result == GT2False){
            // [test]
            // After accepting Connect, this library immediately Accepts,
            // so it is impossible for 'result' to be GT2False, as long as the thread is not interrupted.
            // 
            DWC_Printf(DWC_REPORTFLAG_WARNING, "Unexpected failure to gt2Accept.\n");
                
            // matchmaking ends in an error if already disconnected
            // [todo]
            // for non-server-client matchmaking
            // there is a need to modify such that matchmaking is redone
            DWCi_StopMatching(DWC_ERROR_NETWORK,
                              DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_OTHER);
            return;
        }
        
	    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                   "Accepted connection from %s (latency %d)\n",
                   gt2AddressToString(ip, port, NULL), latency);

        // Clears the resend command so that the TELL_ADDR command does not send more than this
        DWCi_StopResendingMatchCommand();

        if (DWCi_GetMatchCnt()->gt2NumConnection == 0){
            // Records the one-way latency with the server so the server may be notified later
            DWCi_GetMatchCnt()->baseLatency =
                (u16)(((latency >> 1) < 0xffff) ? (latency >> 1) : 0xffff);
        }

        // Gets a pointer to the elements in the GT2 connection list and connection data list
        pGt2Con  = DWCi_GetGT2ConnectionByIdx(index);
        pConInfo = DWCi_GetConnectionInfoByIdx(index);

        // Add accepted connections to the list
        *pGt2Con = connection;
        DWCi_GetMatchCnt()->gt2NumConnection++;

        // Set connection data
        pConInfo->index   = (u8)index;
        pConInfo->aid     = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection-1];
        pConInfo->reserve = 0;
        pConInfo->param   = NULL;

        // Data regarding this connection is set in the data member (used to register user data)
        // of the GT2Connection structure
    	gt2SetConnectionData(connection, pConInfo);

        // The side to be connected is always a newly connected client
        DWCi_PostProcessConnection(DWC_PP_CONNECTION_CL_GT2_ACCEPT);
    }
}


/*---------------------------------------------------------------------------*
  GT2 connection complete callback function
  arguments: connection: Pointer to the structure for the completed connection
          result: GT2 result type
          message: Received data
          len: Size of received data
  return values: None
  application: GT2 connection complete callback
 *---------------------------------------------------------------------------*/
void DWCi_GT2ConnectedCallback(GT2Connection connection,
                               GT2Result result,
                               GT2Byte* message,
                               int len)
{
#pragma unused(len)
    char pidStr[12];
    int  index;
    GT2Connection* pGt2Con;
    GT2Result gt2Result;
    DWCConnectionInfo* pConInfo;

    if (!DWCi_GetMatchCnt() ||
        ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_GT2) &&
         (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_GT2))){
        // Ignored even if flow arrives here immediately after a cancel
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                   "gt2ConnectedCallback: Already cancel\n");
        return;
    }

	if (result != GT2Success){
		if (!message) message = (GT2Byte *)"";
		DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "GT2 connect failed %d: %s\n", result, message);

        if (result == GT2DuplicateAddress){
            // Does nothing since it is possible that an attempt was made to retry the connection on
            // a timeout that occurred before connect complete notification could be
            // received during "result = GT2DuplicateAddress"
            return;
        }
        else if (result == GT2TimedOut){
            // Retry in case of timeout
            DWCi_GetMatchCnt()->gt2ConnectCount++;
            if (DWCi_GetMatchCnt()->gt2ConnectCount > DWC_MATCH_CMD_RETRY_MAX){
                // When the number of resend retries overflows
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "gt2Connect() retry over.\n");
                DWCi_GetMatchCnt()->gt2ConnectCount = 0;

                // accepting new connections is ended and matchmaking is restarted.
                // In the case of server-client types, this is processed by 
                // cancelling the connection of the newly connected client.
                DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->qr2NNFinishCount]);
                return;
            }
                
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Retry to gt2Connect.\n");

            (void)OS_SNPrintf(pidStr, sizeof(pidStr), "%u", DWCi_GetMatchCnt()->profileID);
            gt2Result =
                gt2Connect(*DWCi_GetMatchCnt()->pGt2Socket,
                           NULL,
                           gt2AddressToString(
                               DWCi_GetMatchCnt()->ipList[DWCi_GetMatchCnt()->qr2NNFinishCount],
                               DWCi_GetMatchCnt()->portList[DWCi_GetMatchCnt()->qr2NNFinishCount],
                               NULL),
                           (GT2Byte *)pidStr,
                           -1,
                           DWC_GT2_CONNECT_TIMEOUT,
                           DWCi_GetMatchCnt()->gt2Callbacks,
                           GT2False);
            if ( gt2Result == GT2OutOfMemory )
            {
                DWCi_HandleGT2Error(gt2Result);
                return;
            }
            else if ( gt2Result == GT2Success )
            {
                // Wait for the callback if successful.
            }
            else if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->qr2NNFinishCount]))
            {
                // Reset and wait for the next connection if connection to gt2Connect suddenly fails.
                return;
            }
        }
        else {
            // If a connection has been rejected, processing for accepting new connections is terminated
            // and matchmaking is restarted. Same when other errors occur.
            // (Even if ignored here, there are probably other socket errors that have
            // occurred, so expect them there.)
            // In the case of server-client types, this is processed by 
            // cancelling the connection of the newly connected client.
            if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1]))
                return;
        }
	}
    else {
    	DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "GT2 connected.\n");

        // Search for available spots on the connection list
        index = DWCi_GT2GetConnectionListIdx();

        if (index == -1){
            // It is possible that the connection list becomes full when repetitive
            // matchmaking is used without connections being closed by the game.
            DWC_Printf(DWC_REPORTFLAG_WARNING,
                       "Don't continue matching without closing connections!!\n");

            // matchmaking ends in an error
            DWCi_StopMatching(DWC_ERROR_NETWORK,
                              DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GT2+DWC_ECODE_TYPE_CONN_OVER);
            return;
        }

        // Gets a pointer to the elements in the GT2 connection list and connection data list
        pGt2Con  = DWCi_GetGT2ConnectionByIdx(index);
        pConInfo = DWCi_GetConnectionInfoByIdx(index);

        // Add accepted connections to the list
        *pGt2Con = connection;
        DWCi_GetMatchCnt()->gt2NumConnection++;

        // Set connection data
        pConInfo->index   = (u8)index;
        pConInfo->reserve = 0;
        pConInfo->param   = NULL;
        pConInfo->aid     = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection];

        // Data regarding this connection is set in the data member (used to register user data) of the GT2Connection structure
        // of the GT2Connection structure
	    gt2SetConnectionData(connection, pConInfo);

        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2){
            // For the server
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SV_CONNECT);
        }
        else {
            // For already connected clients
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_CL_GT2_CONNECT);
        }
    }
}


/*---------------------------------------------------------------------------*
  GP friend message received callback function
  Arguments: pconnection: Pointer to the GP connection object
          profileID: Profile ID of the message origin
          message: Received message text
  return values: None
  application: If a GP friend message is received, this function is called within that callback,
          and specific processing for matchmaking is performed
 *---------------------------------------------------------------------------*/
void DWCi_MatchGPRecvBuddyMsgCallback(GPConnection* pconnection,
                                      int profileID,
                                      char* message)
{
#pragma unused(pconnection)
    char tmpStr[16];
    // [arakit] main 051011
    u32 data[DWC_MATCH_COMMAND_ADD_MESSAGE_NUM];
    int i;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "<GP> RECV-0x%02x <- [--------:-----] [pid=%u]\n",
               *message, profileID);

    // An additional message stored in a character string is converted to u32 and stored in an array
    for (i = 0; i < DWC_MATCH_COMMAND_ADD_MESSAGE_NUM; i++){ 
        if (DWCi_GetGPBuddyAdditionalMsg(tmpStr, message+1, i) == -1) break;
        data[i] = strtoul(tmpStr, NULL, 10);
    }

    // Process the received command.
    // Error processing is handled inside the function
    (void)DWCi_ProcessRecvMatchCommand((u8)*message, profileID, 0, 0, data, i);
}


/*---------------------------------------------------------------------------*
  matchmaking command resend stop function
  arguments: None
  return values: None
  application: Initializes some members of the matchmaking command's send control
          structure and stops further resending of this command
 *---------------------------------------------------------------------------*/
static void DWCi_StopResendingMatchCommand(void)
{

    DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;
    DWCi_GetMatchCnt()->cmdCnt.count   = 0;
}


/*---------------------------------------------------------------------------*
  matchmaking error ending function
  Arguments: error: DWC error type
          errorCode: Error code
  return values: None
  application: Processing is halted if an error occurs during matchmaking
 *---------------------------------------------------------------------------*/
void DWCi_StopMatching(DWCError error, int errorCode)
{

    if (!DWCi_GetMatchCnt() || (error == DWC_ERROR_NONE)) return;

    // All connections are closed.
    DWCi_CloseAllConnectionsByTimeout();

    DWCi_SetError(error, errorCode);

    // GP status is set to online mode.
    // [arakit] main 051007
    (void)DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);

    // Call to the matchmaking complete callback function
    DWCi_GetMatchCnt()->matchedCallback(error,
                                 FALSE,
                                 DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
                                 (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ? TRUE : FALSE, 
                                 DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                                 DWCi_GetMatchCnt()->matchedParam);

    DWCi_CloseMatching();
}


/*---------------------------------------------------------------------------*
  QR2 key initialization function
  arguments: None
  return values: None
  application: The QR2 key is initialized so that searches do not hang due to SB server
          update processing during conventional online mode after matchmaking is complete
 *---------------------------------------------------------------------------*/
void DWCi_ClearQR2Key(void)
{

    // the server-client matchmaking server, to continue matchmaking,
    // should not be made to come here!!

    if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV){
        DWCi_GetMatchCnt()->qr2NNFinishCount = 0;  // Usually 0 when control arrives here
        DWCi_GetMatchCnt()->qr2NumEntry      = 0;
        //DWCi_GetMatchCnt()->qr2Reservation   = 0;

        // Notifies the master server of changes in status
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    }
}


/*---------------------------------------------------------------------------*
  matchmaking completion synchronization adjustment packet receipt function
  arguments: aid: Originating host AID
          type: DWC transport packet type. Defined by DWC_SEND_TYPE_*.
          data: Pointer to the start of data near the header
  return values: None
  application: Performs process when receiving matchmaking complete synchronization adjustment packets
 *---------------------------------------------------------------------------*/
void DWCi_ProcessMatchSynPacket(u8 aid, u16 type, u8* data)
{
    u8  aidIdx;
    u32 bitmask;
    int i;

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Received SYN %d packet from aid %d.\n",
               type-DWC_SEND_TYPE_MATCH_SYN, aid);

    switch (type){
    case DWC_SEND_TYPE_MATCH_SYN:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING){
            // When the first SYN packet is received
            if (data[0] == 1){
                // When the local host is a newly connected client.
                // Connections for all clients have been established.
                // Profile ID for callback is 0.
                DWCi_GetMatchCnt()->cbEventPid = 0;
            }

            aidIdx = data[1];
            // Gets one's own AID
            DWCi_GetMatchCnt()->aidList[aidIdx] = data[2];
            // The Profile ID of the local host may also be registered in advance
            DWCi_GetMatchCnt()->sbPidList[aidIdx] = DWCi_GetMatchCnt()->profileID;

            // Matches the number of desired connections with the actual number of connections
            // for matchmaking complete options for the number of specified players or less
            if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
                (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
                DWCi_GetMatchCnt()->qr2NumEntry = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
            }

            // initializes the client's timeout occurrence counter
            DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;

            // Proceed to matchmaking completion synchronization status
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SYN);
        }

        // Sends a SYN-ACK packet to the server
        DWCi_SendMatchSynPacket(aid, DWC_SEND_TYPE_MATCH_SYN_ACK);
        break;

    case DWC_SEND_TYPE_MATCH_SYN_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN){
            // What to do during SYN-ACK wait status
            // Set SYN-ACK received AID bitmap
            DWCi_GetMatchCnt()->synAckBit |= 1 << aid;

            // Update if client's rating is at maximum
            if ((data[0] | (data[1] << 8)) > DWCi_GetMatchCnt()->baseLatency){
                DWCi_GetMatchCnt()->baseLatency = (u16)(data[0] | (data[1] << 8));
            }

            bitmask = DWCi_GetAIDBitmask(FALSE);  // Get the AID bit mask for determination
            if (DWCi_GetMatchCnt()->synAckBit == bitmask){
                // Send an ACK to all clients once SYN-ACK has been returned from all clients
                // Send ACK
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    DWCi_SendMatchSynPacket(DWCi_GetMatchCnt()->aidList[i],
                                            DWC_SEND_TYPE_MATCH_ACK);
                }

                // Proceed to matchmaking completion synchronization standby status
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_SYN_WAIT);

                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "Wait max latency %d msec.\n",
                           DWCi_GetMatchCnt()->baseLatency);
            }
        }
        else {
            // Reply with ACK quickly to handle SYN-ACKs which were individually
            // delayed after ACK was sent
            DWCi_SendMatchSynPacket(aid, DWC_SEND_TYPE_MATCH_ACK);
        }
        break;

    case DWC_SEND_TYPE_MATCH_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SYN){
            // Call matchmaking completion process
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SYN_FINISH);
        }
        break;
    }
}


/*---------------------------------------------------------------------------*
  matchmaking connection closing function
  Arguments: error: Value when a close processing error has been converted to a DWCError type
          errorCode: Error code
          profileID: Profile ID of a closed host
  return values: TRUE: Cancelled during matchmaking or closed by timeout processing
          FALSE: Closed for other reason
  application: Called from within the gt2ConnectionClose callback
          when connections have been closed during matchmaking, this function
          restarts matchmaking for clients
          Connections are also closed when matchmaking has been cancelled.
 *---------------------------------------------------------------------------*/
BOOL DWCi_ProcessMatchClosing(DWCError error, int errorCode, int profileID)
{
#pragma unused(profileID)

    if (DWC_GetState() != DWC_STATE_MATCHING) return FALSE;

    // [test]
    // It is necessary that code does nothing if control somehow reaches this point in the short 
    // amount of time between when the gt2Connection is established
    // and the connected callback is called.
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWCi_ProcessMatchClosing: SC_SV.\n");
        return TRUE;
    }

    if (error){
        // matchmaking end processing call
        DWCi_StopMatching(error, errorCode+DWC_ECODE_SEQ_MATCH);
        return TRUE;
    }

    // Called from within DWCi_GT2ClosedCallback() in dwc_main.c
    // The server's aid is initialized here since the aidList has been shifted due
    // to characteristics of DWCi_DeleteAID()
    DWCi_GetMatchCnt()->aidList[0] = 0;

    if ((DWCi_GetMatchCnt()->cancelState == DWC_MATCH_CANCEL_STATE_EXEC) ||
        (DWCi_GetMatchCnt()->closeState == DWC_MATCH_CLOSE_STATE_CHANGE_TO_CLIENT) ||
        (DWCi_GetMatchCnt()->closeState == DWC_MATCH_CLOSE_STATE_TIMEOUT))
        // Does nothing when cancelled or closed by local host
        return TRUE;

    
    if (DWCi_GetMatchCnt()->nnInfo.cookie){
        // Cancel during NAT negotiation
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }

    if (DWCi_GetMatchCnt()->gt2NumConnection){
        // When negotiations still remain
        if (DWCi_GetMatchCnt()->closeState == DWC_MATCH_CLOSE_STATE_INIT){
            // If the connection is closed first by another host,
            // the fact that remaining hosts are being closed
            // by the other host is recorded and all connections are closed
            DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_FORCED;
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        }
    }
    else {
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            // the server-client matchmaking client
            // ends the closed matchmaking in an error
            // [todo]
            // matchmaking should be restarted from the beginning even if disconnected from a client
            // other than the host or if there is no response while newly
            // connected clients are connecting to hosts other than the server
            DWCi_StopMatching(DWC_ERROR_NETWORK,
                              DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_SC_CL_FAIL);
        }
        else if (DWCi_GetMatchCnt()->priorProfileID){
            // When the Profile ID of servers that have been priority reserved is set
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Reserve NN to %u.\n",
                       DWCi_GetMatchCnt()->priorProfileID);
            
            (void)DWCi_ResumeMatching();
        }
        else if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING){
            // Wait a little bit since a priority reservation may be received
            // in the case of hosts already connected as clients
            // and in wait status.
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Wait prior profileID.\n");

            // If state is not changed, a special state will result if an NN
            // request is received at this time
            DWCi_SetMatchStatus(DWC_MATCH_STATE_WAIT_CLOSE);
            DWCi_GetMatchCnt()->closedTime = OS_GetTick();
        }
        else {
            // Matchmaking must be redone from the beginning, since control flow arrives here
            // if the connection was closed by another host that detected a
            // timeout or in the case of a host for which matchmaking has been cancelled
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Restart matching immediately.\n");
            
            DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
        }
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  server-client matchmaking newly connected client closing function
  arguments: clientPid: Profile ID of the closed client
  return values: None
  application: When a newly connected client is closed, this function
          is called from within the gt2ConnectionClose callback clients
          already connected are notified of the cancellation.
          This function must be called by a server.
 *---------------------------------------------------------------------------*/
void DWCi_ProcessMatchSCClosing(int clientPid)
{

    if (DWCi_GetMatchCnt()->closeState != DWC_MATCH_CLOSE_STATE_TIMEOUT){
        // As long as the connection was not closed by the local host, a cancel
        // command is sent to already connected clients, and server processing is cancelled
        DWCi_CancelPreConnectedServerProcess(clientPid);
    }
}


/*---------------------------------------------------------------------------*
  Function for deleting the host data list (version where a profile ID is specified)
  arguments: profileID: The host profile ID to be deleted
          numHost: Number of connected hosts
  return values: TRUE: Deleted, FALSE Specified Profile ID not on list
  application: Deletes data for the host specified by Profile ID from various
          host data lists
 *---------------------------------------------------------------------------*/
BOOL DWCi_DeleteHostByProfileID(int profileID, int numHost)
{
    int i;

    if (!DWCi_GetMatchCnt()) return FALSE;

    for (i = 0; i < numHost; i++){
        if (DWCi_GetMatchCnt()->sbPidList[i] == profileID){
            DWCi_DeleteHostByIndex(i, numHost);
            return TRUE;
        }
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  Function for deleting host data lists (version where an index specification is used)
  arguments: index: Index of the host to be deleted
          numHost: Number of connected hosts
  return values: Profile ID of the host to be deleted. Returns 0 if called after shutdown.
  application: Deletes data for the host specified by index from various host data lists
          host data lists
 *---------------------------------------------------------------------------*/
int DWCi_DeleteHostByIndex(int index, int numHost)
{
    int profileID;

    if (!DWCi_GetMatchCnt()) return 0;

    profileID = DWCi_GetMatchCnt()->sbPidList[index];

    // Clear active bits in the AID bitmap to zero
    DWCi_GetMatchCnt()->validAidBitmap &= ~(1 << DWCi_GetMatchCnt()->aidList[index]);
    // Update number of valid connections
    DWCi_SetNumValidConnection();

    if (index < numHost-1){
        // Shift one space to the left
        int i;
        for ( i = 0 ; i < numHost - index - 1 ; ++i )
        {
            DWCi_GetMatchCnt()->qr2IPList  [index+i] = DWCi_GetMatchCnt()->qr2IPList  [index+i+1];
            DWCi_GetMatchCnt()->qr2PortList[index+i] = DWCi_GetMatchCnt()->qr2PortList[index+i+1];
            DWCi_GetMatchCnt()->sbPidList  [index+i] = DWCi_GetMatchCnt()->sbPidList  [index+i+1];
            DWCi_GetMatchCnt()->ipList     [index+i] = DWCi_GetMatchCnt()->ipList     [index+i+1];
            DWCi_GetMatchCnt()->portList   [index+i] = DWCi_GetMatchCnt()->portList   [index+i+1];
            DWCi_GetMatchCnt()->aidList    [index+i] = DWCi_GetMatchCnt()->aidList    [index+i+1];
        }
    }

    if ( numHost > 0 )
    {
        DWCi_GetMatchCnt()->qr2IPList[numHost-1]   = 0;
        DWCi_GetMatchCnt()->qr2PortList[numHost-1] = 0;
        DWCi_GetMatchCnt()->sbPidList[numHost-1]   = 0;
        DWCi_GetMatchCnt()->ipList[numHost-1]      = 0;
        DWCi_GetMatchCnt()->portList[numHost-1]    = 0;
        DWCi_GetMatchCnt()->aidList[numHost-1]     = 0;
    }

    return profileID;
}


/*---------------------------------------------------------------------------*
  Function for getting the total number of connections
  arguments: None
  return values: Number of hosts connected to the local host (not including self).
          Returns 0 when offline.
  application: Gets the number of hosts connected to this local host.
          Returns the current number of connected hosts even during matchmaking.
 *---------------------------------------------------------------------------*/
int DWCi_GetNumAllConnection(void)
{

    if (!DWCi_GetMatchCnt()) return 0;
    return DWCi_GetMatchCnt()->gt2NumConnection;
}


/*---------------------------------------------------------------------------*
  Function for getting the number of active connections
  arguments: None
  return values: Number of hosts connected to the local host (not including self).
          Returns 0 when offline.
  application: Gets the number of active hosts (all inter-connected hosts) connected to this local host and records the
          data in a member of the control structure.
          This function is mainly used in cases where matchmaking is progressing
          simultaneously with game communications when you want to get
          a number that excludes partially connected hosts before matchmaking is complete.
 *---------------------------------------------------------------------------*/
int DWCi_GetNumValidConnection(void)
{
    
    if (!DWCi_GetMatchCnt()) return 0;
    return DWCi_GetMatchCnt()->gt2NumValidConn;
}


/*---------------------------------------------------------------------------*
  Function for setting the number of active connections
  arguments: None
  return values: None
  application: Gets the number of active hosts (all inter-connected hosts) connected to this local host
          and records the data in a member of the control structure.
          This must be set when updating validAidBitmap.
 *---------------------------------------------------------------------------*/
void DWCi_SetNumValidConnection(void)
{
    int count = -1;
    int i;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        if (DWCi_GetMatchCnt()->validAidBitmap & (1 << i))
            count++;
    }

    if (count == -1) DWCi_GetMatchCnt()->gt2NumValidConn = 0;
    else DWCi_GetMatchCnt()->gt2NumValidConn = (u8)count;
}


/*---------------------------------------------------------------------------*
  Function for getting an AID list of all connected hosts
  arguments: aidList  Pointer to the AID list.
  return values: The AID list length (including self).
  application: Gets a pointer to the AID list (does not copy).
          The AID list lists currently valid AIDs in order from the beginning.
          and this number of instances represents the number of hosts currently connected, including this host.
          Returns the current number of connected hosts and connected AIDs even during matchmaking.
 *---------------------------------------------------------------------------*/
int DWCi_GetAllAIDList(u8** aidList)
{

    if (!DWCi_GetMatchCnt()) return 0;

    *aidList = DWCi_GetMatchCnt()->aidList;
    return DWCi_GetMatchCnt()->gt2NumConnection+1;
}


/*---------------------------------------------------------------------------*
  Function for getting an AID list of active connected hosts
  arguments: aidList  Pointer to the AID list.
  return values: The AID list length (including self).
  application: Gets a pointer to the AID list (does not copy).
          The AID list is packed, from the front in order, with currently enabled AIDs,
          and this number of instances represents the number of hosts currently connected, including this host.
          This function is mainly used in cases where matchmaking is progressing
          This function is used when you want to get an AID list that excludes
          partially connected hosts before matchmaking is complete.
 *---------------------------------------------------------------------------*/
int DWCi_GetValidAIDList(u8** aidList)
{
    static u8 validAidList[DWC_MAX_CONNECTIONS];
    int i;

    if (!DWCi_GetMatchCnt()) return 0;

    // Clears the active AID list being accessed
    MI_CpuClear8(validAidList, sizeof(validAidList));
    
    for (i = 0; i <= DWCi_GetMatchCnt()->gt2NumValidConn; i++){
        if (DWCi_GetMatchCnt()->validAidBitmap & (1 << DWCi_GetMatchCnt()->aidList[i])){
            // Set active AIDs on the list
            validAidList[i] = DWCi_GetMatchCnt()->aidList[i];
        }
        else break;
        // Since data is padded toward the front, once an inactive AID is encountered, there are no active AIDs from that point on
    }

    *aidList = validAidList;

    return DWCi_GetMatchCnt()->gt2NumValidConn+1;
}


/*---------------------------------------------------------------------------*
  server-client matchmaking server GPstatus setting function
  arguments: None
  return values: GP processing result type.
  application: sets the GP status for the server during server-client matchmaking
 *---------------------------------------------------------------------------*/
GPResult DWCi_GPSetServerStatus(void)
{
    char valueStr[12], keyValueStr[32];

    if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV) return GP_NO_ERROR;

    // Takes the desired number of connections and current number of connections and creates a GP status string
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", DWCi_GetMatchCnt()->qr2NumEntry+1);
    (void)DWC_SetCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_SC_MAX,
                                      valueStr, keyValueStr, '/');
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", DWCi_GetMatchCnt()->gt2NumConnection+1);
    (void)DWC_AddCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_SC_NUM,
                                      valueStr, keyValueStr, '/');

    // Sets the matchmaking version.
    // Since indiscriminate matchmaking is not performed in the case of 
    // server-client type matchmaking, this status will not be referenced, but is merely set for confirmation purposes.
    (void)OS_SNPrintf(valueStr, sizeof(valueStr), "%u", DWC_MATCHING_VERSION);
    (void)DWC_AddCommonKeyValueString(DWC_GP_SSTR_KEY_MATCH_VERSION,
                                      valueStr, keyValueStr, '/');

    // GP status is used for the status of the server performing
    // server-client type matchmaking and the created GP status string is also set
    return DWCi_SetGPStatus(DWC_STATUS_MATCH_SC_SV, keyValueStr, NULL);
}


/*---------------------------------------------------------------------------*
  matchmaking control object deallocation function
  arguments: None
  return values: None
  application: Clears to NULL a pointer to a matchmaking control object
 *---------------------------------------------------------------------------*/
void DWCi_ShutdownMatch(void)
{

  DWCi_SetMatchCnt(NULL); //stpMatchCnt = NULL;

    // Frees the filter character string
    if (stpAddFilter){
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    // Initialize key data array for the QR2 defined by the game
    DWCi_ClearGameMatchKeys();

    // deallocates the matchmaking option control structure
    if (stpOptMinComp != NULL){
        DWC_Free(DWC_ALLOCTYPE_BASE, stpOptMinComp, 0);
        stpOptMinComp = NULL;
    }

    stOptSCBlock.valid = 0;
    stOptSCBlock.lock  = 0;
}


/*---------------------------------------------------------------------------*
  matchmaking shutdown check function
  arguments: None
  return values: TRUE: Matchmaking has been shut down, FALSE: Normal
  application: Checks whether or not DWCi_ShutdownMatch() has been called
          and matchmaking shut down
 *---------------------------------------------------------------------------*/
BOOL DWCi_IsShutdownMatch(void)
{

    if (!DWCi_GetMatchCnt()) return TRUE;
    else return FALSE;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  matchmaking based control structure initializing function
  arguments: None
  return values: None
  application: Initializes the control structure members required for initializing each matchmaking
 *---------------------------------------------------------------------------*/
static void DWCi_ResetMatchParam(DWCMatchResetLevel level)
{

    DWCi_GetMatchCnt()->gt2ConnectCount      = 0;
    DWCi_GetMatchCnt()->nnRecvCount          = 0;
    DWCi_GetMatchCnt()->nnCookieRand         = (u16)DWCi_GetMathRand32(0x10000);
    DWCi_GetMatchCnt()->nnLastCookie         = 0;
    DWCi_GetMatchCnt()->nnFailedTime         = 0;
    DWCi_GetMatchCnt()->nnFinishTime         = 0;
    DWCi_GetMatchCnt()->clLinkProgress       = 0;
    DWCi_GetMatchCnt()->cancelState          = DWC_MATCH_CANCEL_STATE_INIT;
    DWCi_GetMatchCnt()->scResvRetryCount     = 0;
    DWCi_GetMatchCnt()->synResendCount       = 0;
    DWCi_GetMatchCnt()->cancelSynResendCount = 0;
    DWCi_GetMatchCnt()->resvWaitCount        = 0;
    DWCi_GetMatchCnt()->closeState           = DWC_MATCH_CLOSE_STATE_INIT;
    DWCi_GetMatchCnt()->cancelBaseLatency    = 0;
    DWCi_GetMatchCnt()->searchPort           = 0;
    DWCi_GetMatchCnt()->searchIP             = 0;
    DWCi_GetMatchCnt()->lastSynSent          = 0;
    DWCi_GetMatchCnt()->closedTime           = 0;
    MI_CpuClear32(&DWCi_GetMatchCnt()->cmdCnt, sizeof(DWCi_GetMatchCnt()->cmdCnt));

    if (level == DWC_MATCH_RESET_CONTINUE){
        // Clears the count of hosts for which NN is finished when performing continued matchmaking
        // 
        DWCi_GetMatchCnt()->qr2NNFinishCount = (u8)DWCi_GetMatchCnt()->gt2NumConnection;

        // also sets the state during server-client matchmaking
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
            DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAITING);
        }
    }
    else {
        DWCi_GetMatchCnt()->gt2NumConnection = 0;
        DWCi_GetMatchCnt()->gt2NumValidConn  = 0;
        
        DWCi_GetMatchCnt()->qr2NNFinishCount = 0;
        DWCi_GetMatchCnt()->qr2IsReserved    = 0;
        DWCi_GetMatchCnt()->qr2Reservation   = 0;
        
//        DWCi_GetMatchCnt()->sbUpdateCount    = 0;
        DWCi_GetMatchCnt()->sbUpdateFlag     = FALSE;

        DWCi_GetMatchCnt()->friendCount      = 0;
        DWCi_GetMatchCnt()->baseLatency      = 0;
//        DWCi_GetMatchCnt()->cmdResendCount   = 0;
        DWCi_GetMatchCnt()->cmdResendFlag    = FALSE;
        DWCi_GetMatchCnt()->cmdResendTick    = 0;
//        DWCi_GetMatchCnt()->cmdTimeoutCount = 0;
        DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
        DWCi_GetMatchCnt()->cmdTimeoutStartTick = 0;
        DWCi_GetMatchCnt()->synAckBit        = 0;
        DWCi_GetMatchCnt()->reqProfileID     = 0;
        DWCi_GetMatchCnt()->priorProfileID   = 0;
        DWCi_GetMatchCnt()->validAidBitmap   = 0;

        MI_CpuClear32(DWCi_GetMatchCnt()->qr2IPList, sizeof(DWCi_GetMatchCnt()->qr2IPList));
        MI_CpuClear16(DWCi_GetMatchCnt()->qr2PortList, sizeof(DWCi_GetMatchCnt()->qr2PortList));
        MI_CpuClear32(DWCi_GetMatchCnt()->sbPidList, sizeof(DWCi_GetMatchCnt()->sbPidList));
        MI_CpuClear32(&DWCi_GetMatchCnt()->nnInfo, sizeof(DWCNNInfo));
        MI_CpuClear32(DWCi_GetMatchCnt()->ipList, sizeof(DWCi_GetMatchCnt()->ipList));
        MI_CpuClear16(DWCi_GetMatchCnt()->portList, sizeof(DWCi_GetMatchCnt()->portList));
        MI_CpuClear8(DWCi_GetMatchCnt()->aidList, sizeof(DWCi_GetMatchCnt()->aidList));
        MI_CpuClear32(DWCi_GetMatchCnt()->svDataBak, sizeof(DWCi_GetMatchCnt()->svDataBak));

        if (level == DWC_MATCH_RESET_RESTART){
            // To redo matchmaking from the beginning, state must also be returned to initial status
            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
            }
            // server-client matchmaking will not reach this level
        }
        else {
            // The following parameters are cleared only when DWC_MATCH_RESET_ALL has been set.
            DWCi_GetMatchCnt()->qr2MatchType       = 0;
            DWCi_GetMatchCnt()->qr2NumEntry        = 0;
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
            DWCi_GetMatchCnt()->qr2ShutdownFlag    = 0;
#endif
            DWCi_GetMatchCnt()->cbEventPid         = 0;
            DWCi_GetMatchCnt()->distantFriend      = 0;
#ifdef DWC_STOP_SC_SERVER
            DWCi_GetMatchCnt()->stopSCFlag         = 0;
#endif
            DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
            DWCi_GetMatchCnt()->clWaitTime         = 0;
            DWCi_GetMatchCnt()->newClientCallback  = NULL;
            DWCi_GetMatchCnt()->newClientParam     = NULL;
        }
    }
}


/*---------------------------------------------------------------------------*
  matchmaking common parameters initialization function
  arguments: matchType: the matchmaking type DWC_MATCH_TYPE_* enumerator
          numEntry: Number of connection requests (not including self)
          callback: Matchmaking complete callback
          param: Parameter for the matchmaking complete callback
  return values: None
  application: Sets common parameters when matchmaking starts
 *---------------------------------------------------------------------------*/
static void DWCi_SetMatchCommonParam(u8  matchType,
                                     u8  numEntry,
                                     DWCMatchedSCCallback callback,
                                     void* param)
{

    // Initialize parameters requiring initialization for each matchmaking process
    DWCi_ResetMatchParam(DWC_MATCH_RESET_ALL);

    DWCi_GetMatchCnt()->qr2MatchType    = matchType;
    DWCi_GetMatchCnt()->qr2NumEntry     = numEntry;
    DWCi_GetMatchCnt()->matchedCallback = callback;
    DWCi_GetMatchCnt()->matchedParam    = param;
    DWCi_GetMatchCnt()->nnFailureCount  = 0;

    // The AID of the server must be 0
    DWCi_GetMatchCnt()->aidList[0] = 0;

    // Set custom key
    qr2_register_key(DWC_QR2_PID_KEY, DWC_QR2_PID_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_TYPE_KEY, DWC_QR2_MATCH_TYPE_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_RESV_KEY, DWC_QR2_MATCH_RESV_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_VER_KEY, DWC_QR2_MATCH_VER_KEY_STR);
    qr2_register_key(DWC_QR2_MATCH_EVAL_KEY, DWC_QR2_MATCH_EVAL_KEY_STR);
}


/*---------------------------------------------------------------------------*
  matchmaking ending function
  arguments: None
  return values: None
  application: Initializes the matchmaking control object and frees objects
          secured by the GameSpySDK
 *---------------------------------------------------------------------------*/
static void DWCi_CloseMatching(void)
{
    
    DWC_Printf( DWC_REPORTFLAG_MATCH_NN, " Close Matching....\n" );

    if (!DWCi_GetMatchCnt()) return;

#ifndef DWC_BUG_WIFI_DELAY_SEND
    // Releases SB object
    if (DWCi_GetMatchCnt()->sbObj){
        ServerBrowserFree(DWCi_GetMatchCnt()->sbObj);
        DWCi_GetMatchCnt()->sbObj = NULL;  // Must be cleared to null on its own!
    }
#endif
    // Frees even the memory being used by NN
    NNFreeNegotiateList();

    // Initializes the status
    DWCi_SetMatchStatus(DWC_MATCH_STATE_INIT);

    // Frees the filter set for SB server update
    if (stpAddFilter){
        DWC_Free(DWC_ALLOCTYPE_BASE, stpAddFilter, 0);
        stpAddFilter = NULL;
    }

    // Initialize key data array for the QR2 defined by the game
    DWCi_ClearGameMatchKeys();

#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    // Raise the flag for shutting down QR2.
    // Do not shutdown until completely out of the QR2 callback
    // server-client matchmaking doesn't get here, so a shutdown does not occur
    DWCi_GetMatchCnt()->qr2ShutdownFlag = 1;
#endif
}


#define DWC_NUM_QR2_MATCH_KEY 7  // Number of QR2 keys to be used by library
/*---------------------------------------------------------------------------*
  SB server update function
  arguments: profileID: The Profile ID to use when searching for a specified Profile ID.
                    0 is passed if no specification.
  return values: SB error type
  application: The ServerBrowserLimitUpdate function is called using the method best suited for state
 *---------------------------------------------------------------------------*/
static SBError DWCi_SBUpdateAsync(int profileID)
{
    char filter[MAX_FILTER_LEN];
    char* pFilter = filter;
    u8 basicFields[DWC_NUM_QR2_MATCH_KEY+DWC_QR2_GAME_RESERVED_KEYS];
    int len;
    int numBasicFields = DWC_NUM_QR2_MATCH_KEY;
    int i;
    SBError sbError = sbe_noerror;

    // Sets the QR2 keys to be used by library
    basicFields[0] = NUMPLAYERS_KEY;
    basicFields[1] = MAXPLAYERS_KEY;
    basicFields[2] = DWC_QR2_PID_KEY;
    basicFields[3] = DWC_QR2_MATCH_TYPE_KEY;
    basicFields[4] = DWC_QR2_MATCH_RESV_KEY;
    basicFields[5] = DWC_QR2_MATCH_VER_KEY;
    basicFields[6] = DWC_QR2_MATCH_EVAL_KEY;

    // Add game-defined keys
    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
        (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
        for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
            if (stGameMatchKeys[i].keyID){
                basicFields[numBasicFields] = stGameMatchKeys[i].keyID;
                numBasicFields++;
            }
        }
    }

    switch (DWCi_GetMatchCnt()->state){
    case DWC_MATCH_STATE_CL_SEARCH_HOST:
        if (!DWCi_GetMatchCnt()->priorProfileID){
            len = DWCi_GetDefaultMatchFilter(filter, DWCi_GetMatchCnt()->profileID,
                                             DWCi_GetMatchCnt()->qr2NumEntry,
                                             DWCi_GetMatchCnt()->qr2MatchType);

            // The filter to be added for the game is set
            if (stpAddFilter){
                (void)OS_SNPrintf(filter, sizeof(filter),
                                  "%s and (%s)",
                                  filter, stpAddFilter);
            }
            break;
        }
        else {
            // If a priority reservation Profile ID is set, a search is made using that Profile ID
            // The profile ID is used to search
            profileID = DWCi_GetMatchCnt()->priorProfileID;
        }

    case DWC_MATCH_STATE_CL_SEARCH_OWN:
    case DWC_MATCH_STATE_CL_WAIT_RESV:
    case DWC_MATCH_STATE_CL_SEARCH_NN_HOST:
        // searches only on profile ID
        (void)OS_SNPrintf(filter, sizeof(filter),
                          "%s = %u",
                          DWC_QR2_PID_KEY_STR, profileID);

        // for friend unspecified peer matchmaking and server-client matchmaking
        // it needs to be set to be re-sent here without using reqProfileID.
        DWCi_GetMatchCnt()->reqProfileID = profileID;
        break;

    default:
        // [test]
        // Impossible
        DWC_Printf(DWC_REPORTFLAG_ERROR,
                   "---DWCi_SBUpdateAsync() illegal state %d.\n",
                   DWCi_GetMatchCnt()->state);
    }

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "ServerBrowserFilter : %s\n", filter);

#if 1
    // The server list is cleared since SBUpdate cannot be performed without disconnecting from the master server
    // Clear server list
    ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
#else
    if (ServerBrowserState(DWCi_GetMatchCnt()->sbObj) != sb_disconnected){
        // The server list is cleared since SBUpdate cannot be performed without disconnecting from the master server
        // Clear server list
        ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
    }
#endif

    // Updates the server list up to the maximum number of desired connections.
    for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
        sbError = ServerBrowserLimitUpdate(DWCi_GetMatchCnt()->sbObj, SBTrue, SBFalse,
                                           basicFields, numBasicFields,
                                           filter, DWC_SB_UPDATE_MAX_SERVERS);
        if (!sbError) break;
        else if (sbError != sbe_dnserror) break;
        // Retries a set number of times if a DNS error occurs
    }
#ifdef NITRODWC_DEBUG
	//disp_time();
#endif
    return sbError;
}


/*---------------------------------------------------------------------------*
  friend unspecified peer matchmaking SB server update default filtering string get function
  arguments: filter: Pointer to the start of the string
          profileID: Profile ID of the local host
          numEntry: Number of connection requests (not including self)
          matchType: matchmaking type
  return values: String length
  application: creates and gets the friend unspecified peer matchmaking SB server update
          default filtering string
 *---------------------------------------------------------------------------*/
static int DWCi_GetDefaultMatchFilter(char* filter, int profileID, u8 numEntry, u8 matchType)
{

    // Search for QR2 host at less than max. capacity using the same matchmaking conditions.
    // Also check the pid so that old local host data is not affected.
    return OS_SNPrintf(filter, MAX_FILTER_LEN,
                       "%s = %d and %s != %u and maxplayers = %d and numplayers < %d and %s = %d and %s != %s",
                       DWC_QR2_MATCH_VER_KEY_STR,
                       DWC_MATCHING_VERSION,
                       DWC_QR2_PID_KEY_STR,
                       profileID,
                       numEntry,
                       numEntry,
                       DWC_QR2_MATCH_TYPE_KEY_STR,
                       matchType,
                       DWC_QR2_MATCH_RESV_KEY_STR,
                       DWC_QR2_PID_KEY_STR);
}


/*---------------------------------------------------------------------------*
  Function for starting NAT negotiations
  arguments: isQR2: 1: QR2, 0: SB
          cookie: Cookie value required by QR2. Generated within the function for the SB side.
          server: SBServer object. Required by SB side during star-type NAT negotiations
  return values: NN error type
  application: Starts NAT negotiations according to the method corresponding to state
 *---------------------------------------------------------------------------*/
static NegotiateError DWCi_NNStartupAsync(int isQR2, int cookie, SBServer server)
{
    u32 ip;
    u32 senddata[2];
    int index = DWCi_GetMatchCnt()->qr2NNFinishCount;
    int result;
    BOOL nnValid;
    NegotiateError nnError = ne_noerror;

    if (!isQR2){
        // for NN beginning
        // Generate cookie value
        cookie = (DWCi_GetMatchCnt()->profileID & 0x0ffff) | (DWCi_GetMatchCnt()->nnCookieRand << 16);

        if (SBServerHasPrivateAddress(server)){
            // When the server is in NAT
            if (SBServerGetPublicInetAddress(server) ==
                ServerBrowserGetMyPublicIPAddr(DWCi_GetMatchCnt()->sbObj)){
                // When the server is in the same NAT
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Server[%d] is behind same NAT as me.\n", index);

                // The IP and port actually used have private values
                DWCi_GetMatchCnt()->ipList[index]   = SBServerGetPrivateInetAddress(server);
                DWCi_GetMatchCnt()->portList[index] = SBServerGetPrivateQueryPort(server);

                nnValid = FALSE;  // NAT negotiation unnecessary
            }
            else {
                // When the server is in another NAT
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Server[%d] is behind NAT.\n", index);
                
                // The IP and port actually used are determined by NAT negotiation
                nnValid = TRUE;  // NAT negotiation required
            }
        }
        else {
            // When the server is outside NAT
            ip = (u16)SO_GetHostID();
            if (((ip & 0xffff) == 0xa8c0) ||
                (((ip & 0xff) == 0xac) && ((ip & 0xff00) >= 0x1000) && ((ip & 0xff00) <= 0x1f00)) ||
                ((ip & 0xff) == 0x10)){
                // NAT negotiation is required as long as the local host is in the NAT even if the other party is outside the NAT
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Server[%d] is not behind NAT. But I'm behind NAT.\n",
                           index);

                // The IP and port actually used are determined by NAT negotiation
                nnValid = TRUE;  // NAT negotiation required
            }
            else {
                // When the local host and other party are both outside NAT
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Both I and Server[%d] are not behind NAT.\n",
                           index);

                // The IP and port actually used have public values
                DWCi_GetMatchCnt()->ipList[index]   = SBServerGetPublicInetAddress(server);
                DWCi_GetMatchCnt()->portList[index] = SBServerGetPublicQueryPort(server);
                    
                nnValid = FALSE;  // NAT negotiation unnecessary
            }
        }

        if (nnValid){
            // Generated only when NAT negotiation requests are actually performed
            DWCi_GetMatchCnt()->nnCookieRand  = (u16)DWCi_GetMathRand32(0x10000);

            // Cookies are set only when NAT negotiations are actually performed
            DWCi_GetMatchCnt()->nnInfo.cookie = cookie;
        }
        else {
            // If NAT negotiations are unneeded, consider the IP and port number of the other party
            // and have NAT negotiation complete status set
            senddata[0] = (u32)SO_GetHostID();
            senddata[1] = gt2GetLocalPort(*DWCi_GetMatchCnt()->pGt2Socket);

            // Send command
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_TELL_ADDR,
                                           DWCi_GetMatchCnt()->sbPidList[index],
                                           SBServerGetPublicInetAddress(server),
                                           SBServerGetPublicQueryPort(server),
                                           senddata, 2);

            DWCi_GetMatchCnt()->cmdCnt.count = 0;  // Start command resend count
                                               
            // [todo]
            // How should errors here be processed?
            if (result) return ne_socketerror;

            // Do not set cookies if NAT negotiations are not actually performed
            DWCi_GetMatchCnt()->nnInfo.cookie = 0;
        }

        // Always set the NN parameter. Even if NAT negotiations are unneeded,
        // access the member isQR2 only from inside the NN complete callback.
        DWCi_GetMatchCnt()->nnInfo.isQR2      = 0;
        DWCi_GetMatchCnt()->nnInfo.retryCount = 0;
        DWCi_GetMatchCnt()->nnInfo.port       = SBServerGetPublicQueryPort(server);
        DWCi_GetMatchCnt()->nnInfo.ip         = SBServerGetPublicInetAddress(server);
    }
    else {
        // Set the NN parameter here for the QR2 side
        DWCi_GetMatchCnt()->nnInfo.isQR2      = 1;
        DWCi_GetMatchCnt()->nnInfo.retryCount = 0;
        DWCi_GetMatchCnt()->nnInfo.port       = 0;
        DWCi_GetMatchCnt()->nnInfo.ip         = 0;
        DWCi_GetMatchCnt()->nnInfo.cookie     = cookie;

        nnValid = TRUE;  // NAT negotiation required
    }

    
    if (nnValid){
        // Execute NAT negotiation processing
        // Error processing is handled inside the function
        nnError = DWCi_DoNatNegotiationAsync(&DWCi_GetMatchCnt()->nnInfo);
    }
    else {
        // If NAT negotiation is unneeded, call the NAT negotiation complete callback
        // Calls the callback
        // Leave remoteaddr = userdata = NULL as a sign.
        DWCi_NNCompletedCallback(nr_success,
                                 gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
                                 NULL, &DWCi_GetMatchCnt()->nnInfo);

        // If NAT negotiation was not actually performed, clear the time when NN exits
        // 
        DWCi_GetMatchCnt()->nnFinishTime = 0;
    }

    return nnError;
}


/*---------------------------------------------------------------------------*
  Function for starting NAT negotiations
  arguments: Pointer to the NAT negotiation parameter structure nnInfo
  return values: NN error type
  application: Starts actual negotiation based on the NAT negotiation parameters given
 *---------------------------------------------------------------------------*/
static NegotiateError DWCi_DoNatNegotiationAsync(DWCNNInfo* nnInfo)
{
    SBError sbError;
    NegotiateError nnError;
    int i;

    if (!nnInfo->isQR2){
        // A cookie is sent to the server if it is not on the QR2 side
        sbError =
            ServerBrowserSendNatNegotiateCookieToServer(
                DWCi_GetMatchCnt()->sbObj,
                gt2AddressToString(nnInfo->ip, 0, NULL),
                nnInfo->port,
                nnInfo->cookie);
        // [todo]
        // How should errors here be processed?
        if (DWCi_HandleSBError(sbError)) return ne_socketerror;

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Send NN cookie = %x.\n",
                   nnInfo->cookie);
    }

    // Start NAT negotiation
    for ( i = 0 ; i < DWC_DNS_ERROR_RETRY_MAX ; ++i )
    {
        nnError =
                NNBeginNegotiationWithSocket(gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
                                             nnInfo->cookie, nnInfo->isQR2,
                                             DWCi_NNProgressCallback,
                                             DWCi_NNCompletedCallback,
                                             nnInfo);
        if ( nnError == ne_noerror || nnError != ne_dnserror) break;

        DWC_Printf(DWC_REPORTFLAG_DEBUG, " dns error occurs when NatNegotiation begin... retry\n");
    }

    return nnError;
}


/*---------------------------------------------------------------------------*
  matchmaking command sending function
  arguments: command: Command constant to be sent
          profileID: Profile ID of the host being sent to
          ip: The IP for the QR2 of the host being sent to for friend unspecified peer matchmaking
          port: The port number for the QR2 of the host being sent to for friend unspecified peer matchmaking
          data: Data to be sent appended to the command (pointer to a u32 type array).
                    If there is no data, NULL is passed
          len: Number of elements in the array data. If data = NULL, this parameter is ignored no matter what is passed.
  return values: SBError type or the value resulting from conversion from GPResult type to int type
  application: The matchmaking command is sent with either SB message or GP 
          friend message selected
 *---------------------------------------------------------------------------*/
static int  DWCi_SendMatchCommand(u8 command, int profileID, u32 ip, u16 port, const u32 data[], int len)
{
    char message[DWC_MATCH_COMMAND_ADD_MESSAGE_MAX];
    char tmpStr[16];
    int msgLen = 0, tmpLen;
    int result;
    int i;

    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
        (((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL) ||
          DWCi_GetMatchCnt()->distantFriend) &&
         (command == DWC_MATCH_COMMAND_TELL_ADDR))){
        // Send SB message command
        // even for server-client matchmaking, among clients
        // use SB messages as there is the possibility they aren't friends.
        // Similarly, stick to SB messages for the above command even if friends of 
        // friends are allowed (only possible when friend specified peer matchmaking is used).
        result = (int)DWCi_SendSBMsgCommand(command, ip, port, data, len);
    }
    else {
        if (data && len){
            // Convert u32 array data into a text string
            msgLen = OS_SNPrintf(message, sizeof(message), "%u", data[0]);

            for (i = 1; i < len; i++){
                tmpLen = OS_SNPrintf(tmpStr, sizeof(tmpStr), "/%u", data[i]);

                // Additional message text
                MI_CpuCopy8(tmpStr, &message[msgLen], (u32)tmpLen);
                msgLen += tmpLen;
            }
        }
        message[msgLen] = '\0';

        // Send GP friend message command
        result =
            (int)DWCi_SendGPBuddyMsgCommand(DWCi_GetMatchCnt()->pGpObj,
                                            command,
                                            profileID,
                                            message);
    }

    // Only place data of send commands that are being resent on the stack
    if ((command == DWC_MATCH_COMMAND_RESV_OK)      ||
        (command == DWC_MATCH_COMMAND_TELL_ADDR)    ||
        (command == DWC_MATCH_COMMAND_LINK_CLS_REQ) ||
        (command == DWC_MATCH_COMMAND_LINK_CLS_SUC)){
        DWCi_GetMatchCnt()->cmdCnt.command   = command;
        DWCi_GetMatchCnt()->cmdCnt.port      = port;
        DWCi_GetMatchCnt()->cmdCnt.ip        = ip;
        DWCi_GetMatchCnt()->cmdCnt.profileID = profileID;
        DWCi_GetMatchCnt()->cmdCnt.len       = len;
        DWCi_GetMatchCnt()->cmdCnt.sendTime  = OS_GetTick();
        if (data && len){
            MI_CpuCopy32(data, DWCi_GetMatchCnt()->cmdCnt.data, (u32)len*4);
        }
    }

    return result;
}


/*---------------------------------------------------------------------------*
  matchmaking SB message command sending function
  arguments: command: Command constant to be sent
          ip: IP for QR2 of the host being sent to
          port: Port number for QR2 of the host being sent to
          data: Data to be sent appended to the command (pointer to a u32 type array).
                  If there is no data, NULL is passed
          len: Number of elements in the array data. If data = NULL, this parameter is ignored no matter what is passed.
  return values: SBError type
  application: Sends a control command for matchmaking using an SB message.
 *---------------------------------------------------------------------------*/
static SBError DWCi_SendSBMsgCommand(u8 command, u32 ip, u16 port, const u32 data[], int len)
{
    int i;
    DWCSBMessage senddata;
    SBError sbError;

    // ASSERT when the local host has created too much data
    SDK_ASSERT(len < DWC_MATCH_COMMAND_ADD_MESSAGE_NUM);

    // Create send data
    if (data && len){
        MI_CpuCopy32(data, senddata.data, sizeof(u32)*len);
    }
    else {
        // The value assigned to len is ignored when data = NULL
        len = 0;
    }

    strcpy(senddata.header.identifier, DWC_SB_COMMAND_STRING);
    senddata.header.version   = DWC_MATCHING_VERSION;
    senddata.header.command   = command;
    senddata.header.size      = (u8)(sizeof(u32)*len);
    senddata.header.qr2Port   = DWCi_GetMatchCnt()->qr2Port;
    senddata.header.qr2IP     = DWCi_GetMatchCnt()->qr2IP;
    senddata.header.profileID = DWCi_GetMatchCnt()->profileID;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "<SB> SEND-0x%02x -> [%08x:%d] [pid=--------]\n",
               command, ip, port);

    // Send SB message
    for (i = 0; i < DWC_DNS_ERROR_RETRY_MAX; i++){
        sbError =
            ServerBrowserSendMessageToServer(DWCi_GetMatchCnt()->sbObj,
                                             gt2AddressToString(ip, 0, NULL),
                                             port,
                                             (gsi_char *)&senddata,
                                             (int)(sizeof(DWCSBMessageHeader)+senddata.header.size));

        if (!sbError) break;
        else if (sbError != sbe_dnserror) break;
        // Retries a set number of times if a DNS error occurs
    }

    return sbError;
}


/*---------------------------------------------------------------------------*
  matchmaking GP friend message command sending function
  Arguments: connection: Pointer to the GP connection object
          profileID: Profile ID of the friend being sent to
          command: Command constant to be sent
          message: Message to be appended to command and sent. If no message is necessary, NULL is passed
                     Note that even numeric values must be passed as character strings (since 0 is taken as a terminator)
  return values: GP result type.
  application: Sends a control command for matchmaking using a GP message.
 *---------------------------------------------------------------------------*/
static GPResult DWCi_SendGPBuddyMsgCommand(GPConnection* connection, u8 command, int profileID, const char* message)
{
    char tmpStr[DWC_MATCH_COMMAND_ADD_MESSAGE_MAX];
    u32  len, len2;
    GPResult result;

    len = (u32)OS_SNPrintf(tmpStr, sizeof(tmpStr),
                           "%s%dv%s",
                           DWC_GP_COMMAND_STRING,
                           DWC_MATCHING_VERSION,
                           DWC_GP_COMMAND_MATCH_STRING);
    tmpStr[len]   = (char)command;
    tmpStr[len+1] = '\0';

    if (message){
        len2 = strlen(message);

        // [test]
        // Warning to local host when the additional command message is too long
        SDK_ASSERT(len2 < (DWC_MATCH_COMMAND_ADD_MESSAGE_MAX-4-11-3-1));

        // Append additional command message to the send command
        MI_CpuCopy8(message, &tmpStr[len+1], len2);
        tmpStr[len+1+len2] = '\0';
    }

    // Send friend message
    result = gpSendBuddyMessage(connection, profileID, tmpStr);

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "<GP> SEND-0x%02x -> [--------:-----] [pid=%u]\n",
               command, profileID);

    return result;
}


/*---------------------------------------------------------------------------*
  Function for getting the additional message of the GP friend message command
  arguments: dtsMsg: Pointer to the array to which the extracted string is to be copied
          srcMsg: The additional message to be used with the GP friend message command
          index: Indicates how many lines of strings, delimited by '/', are to be retrieved
  return values: String length Returns -1 when an index that does not exist has been specified
  application: Retrieves the specified string from the additional message of the GP friend message command
          Get the string by specifying which number command
 *---------------------------------------------------------------------------*/
static int  DWCi_GetGPBuddyAdditionalMsg(char* dstMsg, const char* srcMsg, int index)
{
    const char* pSrcBegin = srcMsg;
    char* pSrcNext = NULL;
    char* pSrcEnd;
    int len;
    int i;

    pSrcEnd = strchr(pSrcBegin, '\0');  // Gets a pointer to the termination

    // Sets a pointer to the start of the string in the index specified using pSrcBegin
    for (i = 0; i < index; i++){
        pSrcNext = strchr(pSrcBegin, '/');
        if (!pSrcNext){
            // Invalid index specification
            return -1;
        }
        pSrcBegin = pSrcNext+1;
    }

    // Sets a pointer to the end of the string even if it is the last index
    //if (!pSrcNext) pSrcNext = pSrcEnd;
    pSrcNext = strchr(pSrcBegin, '/');
    if (!pSrcNext){
        // If '/' cannot be found
        pSrcNext = pSrcEnd;
    }

    // Invalid index specification when called when there is no additional message
    if (pSrcBegin == pSrcNext) return -1;

    // Copy the specified string
    len = pSrcNext-pSrcBegin;
    MI_CpuCopy8(pSrcBegin, dstMsg, (u32)len);
    dstMsg[len] = '\0';

    return len;
}


/*---------------------------------------------------------------------------*
  matchmaking command handling function
  arguments: command: Received matchmaking command
          srcPid: Profile ID of the command send source
          srcIP: Public IP for QR2 of the command send source when received with an SB message
          srcPort: Public port number for QR2 of the command send source when received with an SB message
          data: Pointer to the received data array
          len: Number of elements in the array data
  return values: TRUE: Normal end, FALSE: Error generated
  application: Accesses and processes the command and data that has been received
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessRecvMatchCommand(u8  command,
                                         int srcPid,
                                         u32 srcIP,
                                         u16 srcPort,
                                         const u32 data[],
                                         int len)
{
#pragma unused(len)
    u8  sendCommand;
    u8  aid;
    u16 port = 0;
    u32 ip = 0;
    u32 aidIdx;
    u32 senddata[DWC_MAX_MATCH_IDX_LIST+1];
    int profileID;
    int sendlen = 0;
    int result;
    int i;
    SBError sbError;
    NegotiateError nnError;
    struct sockaddr_in remoteaddr;

    if (!DWCi_GetMatchCnt() || (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_INIT)){
        // Nothing is returned when received immediately after matchmaking cancellation
        // [todo]
        // If only this condition is used, received commands may be processed when
        // matchmaking is redone immediately after cancellation using different matchmaking conditions.
        // Even so, be sure to design code so that it does not run away from you.
        return TRUE;
    }

    if ((DWC_GetState() == DWC_STATE_MATCHING) &&
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING) &&
        (srcPid == DWCi_GetMatchCnt()->sbPidList[0])){
        // for connected clients, once a command is received from the server
        // initialize the timeout counter
        DWCi_InitClWaitTimeout();
    }

    switch (command){
    case DWC_MATCH_COMMAND_RESERVATION:  // Request NN reservation
    case DWC_MATCH_COMMAND_RESV_PRIOR:   // Request priority NN reservation
        if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_ANYBODY){
            // for friend specified server-client matchmaking
            // the client's QR2 public IP and port number are within the additional
            // received data
            srcIP   = data[1];
            srcPort = (u16)data[2];
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "friend IP:%x, port:%d\n",
                       srcIP, srcPort);
        }
        
        // Gets the command to be sent back based on the sender data of the reservation command
        sendCommand =
            DWCi_CheckResvCommand(srcPid, srcIP, srcPort, data[0],
                                  command == DWC_MATCH_COMMAND_RESV_PRIOR ? TRUE : FALSE);

        if (sendCommand == DWC_MATCH_COMMAND_RESV_OK){
            // When the reservation is approved
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
            // clear the newly connected friend permission command receipt bitmap
            DWCi_GetMatchCnt()->friendAcceptBit = 0;
#endif
                
            // Makes common settings for an approved reservation and notifies already
            // connected clients of the newly connected host.
            result = DWCi_ProcessResvOK(srcPid, srcIP, srcPort);
            if (DWCi_HandleMatchCommandError(result)) return FALSE;

            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                // for the server-client matchmaking server
                // Call the new connection client notification callback
                if (DWCi_GetMatchCnt()->newClientCallback){
                    DWCi_GetMatchCnt()->newClientCallback(DWCi_GetFriendListIndex(srcPid),
                                                   DWCi_GetMatchCnt()->newClientParam);
                }
            }

            senddata[0] = DWCi_GetMatchCnt()->qr2NNFinishCount;

            // Sends the number of clients already connected and the Profile ID of each client.
            // (Currently, this data is only accessed when the receiving side
            // uses friend specified/unspecified peer matchmaking)
            for (sendlen = 1; sendlen <= DWCi_GetMatchCnt()->qr2NNFinishCount; sendlen++){
                senddata[sendlen] = (u32)DWCi_GetMatchCnt()->sbPidList[sendlen];
            }

            // The client is informed of the public IP and port number for the latest QR2 of the local host
            senddata[sendlen++] = DWCi_GetMatchCnt()->qr2IP;
            senddata[sendlen++] = DWCi_GetMatchCnt()->qr2Port;

            // Status proceeds to NAT negotiation underway
            DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_OWN_NN);
        }
        // [arakit] main 051024
        else if (sendCommand == DWC_MATCH_COMMAND_RESV_DENY){
            // If RESV_DENY, send the reason for refusal.
            // [note]
            // the reason why qr2NumEntry > 0?
            // When we have this, if DWC_StopSCMatching() is called before
            // reservation receipt when no one is connected, qr2NumEntry will equal 0;
            // the client will hit an 80410 error
            if ((DWCi_GetMatchCnt()->qr2NumEntry > 0) &&
                (DWCi_GetMatchCnt()->qr2NNFinishCount == DWCi_GetMatchCnt()->qr2NumEntry)){
                // If the server is over max. capacity
                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                    // a denial reason is only needed at this point for server-
                    // client matchmaking
                    senddata[0] = DWC_MATCH_RESV_DENY_REASON_SV_FULL;
                    sendlen     = 1;
                }
            }
        }
        // [arakit] main 051024

        if (sendCommand != DWC_MATCH_COMMAND_DUMMY){
            // Sends a command to the host originally sending the reserve command
            result =
                DWCi_SendMatchCommand(sendCommand, srcPid, srcIP, srcPort,
                                      senddata, sendlen);
            if (DWCi_HandleMatchCommandError(result)) return FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_RESV_OK:  // NN reservation complete
        // Ignored if this command arrives late even though the reservation was already approved
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV) break;
        
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Succeeded NN reservation.\n");

        if (srcPid != DWCi_GetMatchCnt()->reqProfileID){
            // Ignored even if reservation complete notification is returned from a server that was not sent the reservation request
            break;
        }

        DWCi_GetMatchCnt()->priorProfileID = 0;  // Clear priority reservation

        DWCi_GetMatchCnt()->resvWaitCount  = 0;  // Clear resend counter when WAIT is received

//        DWCi_GetMatchCnt()->cmdTimeoutCount = 0;
        DWCi_GetMatchCnt()->cmdTimeoutTime = 0;

//        DWCi_GetMatchCnt()->cmdResendCount   = 0;
        DWCi_GetMatchCnt()->cmdResendFlag  = FALSE;

        // Record the public IP and port number for the latest QR2 of the server
        DWCi_GetMatchCnt()->qr2IPList[0]   = data[data[0]+1];
        DWCi_GetMatchCnt()->qr2PortList[0] = (u16)data[data[0]+2];
        DWCi_GetMatchCnt()->searchIP       = data[data[0]+1];
        DWCi_GetMatchCnt()->searchPort     = (u16)data[data[0]+2];
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Server IP:%x, port:%d\n",
                   DWCi_GetMatchCnt()->searchIP, DWCi_GetMatchCnt()->searchPort);

        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
            // For friend specified peer matchmaking
            if (DWCi_AreAllBuddies(&data[1], data[0])){
                // When clients already connected on the server side are all friends of the local host
                if (DWCi_GetMatchCnt()->gt2NumConnection){
                    // if there are connected clients to the host, take a backup
                    // to transfer data there as well.
                    DWCi_MakeBackupServerData(srcPid, data);
                }
                // Next, be sure to first search for the host data of the other party
            }
            else {
                // if a client that is not a friend has connected to the server
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "But some clients are not friends.\n");
                
                // Performs reservation cancellation.
                result = DWCi_CancelReservation(srcPid);
                if (DWCi_HandleMatchCommandError(result)) return FALSE;

                // Send an NN reservation request to the next friend
                result = DWCi_SendResvCommandToFriend(FALSE, FALSE, srcPid);
                if (DWCi_HandleMatchCommandError(result)) return FALSE;
                break;  // Skip over the following processing
            }
        }

        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
            // For friend unspecified peer matchmaking
            if (DWCi_GetMatchCnt()->gt2NumConnection){
                // If a client already connected to the local host is present, 
                // close all connections and return completely to 1 client
                DWCi_MakeBackupServerData(srcPid, data);

                result = DWCi_ChangeToClient();
                if (DWCi_HandleMatchCommandError(result)) return FALSE;
            }

            // Set the NAT negotiation status
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
               
            // Start NAT negotiation with the server
            nnError = DWCi_NNStartupAsync(0, 0, ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, 0));
            if (DWCi_HandleNNError(nnError)) return FALSE;
        }
        else {
            // Proceed to searching for reserved hosts
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_NN_HOST);

            // Search for the host data of the other party at SB server update
            sbError = DWCi_SBUpdateAsync(srcPid);
            if (DWCi_HandleSBError(sbError)) return FALSE;
        }
        break;
            
    case DWC_MATCH_COMMAND_RESV_DENY:  // Deny NN reservation
        // The value of srcIP and srcPort are both 0 when a timeout has occurred due to
        // no response from the server
        // -> this specification has gone away
            
        // Ignored if this command arrives late when the reservation
        // has already been approved by another client
        if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV) ||
            (srcPid != DWCi_GetMatchCnt()->reqProfileID))
            break;

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Reservation was denied by %u.\n", srcPid);

        // [arakit] main 051024
        if (len > 0){
            // If there is a reason for refusal
            if (data[0] == DWC_MATCH_RESV_DENY_REASON_SV_FULL){
                // number of members exceed on server during server-client matchmaking
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Game server is fully occupied.\n");

                DWCi_StopMatching(DWC_ERROR_SERVER_FULL, 0);
                return FALSE;
            }
        }
        // [arakit] main 051024

        // Search for next reservation
        return DWCi_RetryReserving(DWCi_GetMatchCnt()->reqProfileID);
        break;

    case DWC_MATCH_COMMAND_RESV_WAIT:  // Wait for NN reservation
        // Ignored if this command arrives late even though the reservation was already approved
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV) break;
        
        // Since the server, or clients connected to it, 
        // have established connections, software waits for this to end
        if (srcPid == DWCi_GetMatchCnt()->reqProfileID){
            // Reset the start time for measuring the timeout
            DWCi_GetMatchCnt()->cmdTimeoutStartTick = OS_GetTick();
            
            if ((DWCi_GetMatchCnt()->priorProfileID &&
                 (DWCi_GetMatchCnt()->resvWaitCount < DWC_RESV_COMMAND_RETRY_MAX)) ||
                (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
                // If a priority reservation has been set, resend the reservation command after a set interval of time.
                // Resend the reservation command
                // Resend now since connecting with other servers is 
                // unnecessary even when server-client matchmaking is used.
//                DWCi_GetMatchCnt()->cmdResendCount   = 1;
                DWCi_GetMatchCnt()->cmdResendFlag = TRUE;
                DWCi_GetMatchCnt()->cmdResendTick = OS_GetTick();
                
                if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL){
                    DWCi_GetMatchCnt()->resvWaitCount++;
                }
                // In the case of server-client matchmaking, this command may be resent
                // continuously while waiting for WAIT to return because the reservation
                // command is only sent to the specified server.
                break;  // Skip over the following processing
            }

            DWCi_GetMatchCnt()->priorProfileID = 0;  // Clear priority reservation
            DWCi_GetMatchCnt()->resvWaitCount  = 0;  // Clear the send reservation until WAIT received counter

            // As long as no priority reservation has been set, close quickly and search for the next reservation
            // Search for next reservation
            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                // Return to searching for available hosts
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
//                DWCi_GetMatchCnt()->sbUpdateCount = 1;  // Search after set time interval elapses
                DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_SHORT;  // Search after set time interval elapses
                DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
                // Send an NN reservation request to the next friend
                (void)DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);
            }
            // server-client matchmaking wouldn't make it here
        }
        // Commands from hosts other than the one to which the reservation was sent are ignored.
        // The reservation wait command is also ignored if the reservation is cancelled or the local host has already approved the reservation.
        break;

    case DWC_MATCH_COMMAND_RESV_CANCEL:  // Request cancellation of NN reservation
        if (DWCi_GetMatchCnt()->qr2IsReserved &&
            (srcPid == DWCi_GetMatchCnt()->qr2Reservation)){ 
            // Respond if the cancel reservation request is from a host being reserved
            if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) &&
                (DWCi_GetMatchCnt()->gt2NumConnection == 1) &&
                (DWCi_GetMatchCnt()->sbPidList[1] == srcPid)){
                // with server-client matchmaking,
                // when a newly connected client directly cancels,
                // if that occurs immediately after establishing gt2Connect,
                // close the connection
                gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
            }
                
            // accepting new connections is ended and matchmaking is restarted
            if (!DWCi_CancelPreConnectedServerProcess(srcPid)) return FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_TELL_ADDR:  // Notification of NN beginning address
        // This command is for terminating NAT negotiation in cases where NAT negotiation
        // is unnecessary because the local host is in the same NAT as the NN parent.
        ip   = data[0];
        port = (u16)data[1];

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "NN parent is behind same NAT as me. Received IP %x & port %d\n",
                   ip, port);

        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING){
            // Status is changed out of convenience when clients are already connected
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
        }
        else if (((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN) &&
                  (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)) ||
                 (srcPid != DWCi_GetMatchCnt()->qr2Reservation)){
            // If an NN request arrives after a reservation has been cancelled because the client
            // did not come to the reservation for quite a while, the request is ignored
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "But already canceled reservation.\n");
            break;
        }

        // Terminating reservation hold timeout measurement
        DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;

        // If you still don't know the profile ID of the other party because it hasn't been
        // possible to receive a newly connected client notification, it is set here.
        // If NAT negotiation has actually been performed, there is absolutely
        // no chance of recovering a profile ID of this type.
        if (DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1] != srcPid){
            DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1] = srcPid;
        }

        // Create address data to be passed to the NAT negotiation complete callback
        remoteaddr.sin_addr.s_addr = ip;
        // The port number was converted inside the NN callback function
        // and is converted back here
        remoteaddr.sin_port = SO_HtoNs(port);

        // Call the NAT negotiation complete callback
        DWCi_GetMatchCnt()->nnInfo.isQR2 = 1;
        DWCi_NNCompletedCallback(nr_success,
                                 gt2GetSocketSOCKET(*DWCi_GetMatchCnt()->pGt2Socket),
                                 &remoteaddr, &DWCi_GetMatchCnt()->nnInfo);

        // If NAT negotiation was not actually performed, clear the time when NN exits
        // 
        DWCi_GetMatchCnt()->nnFinishTime = 0;
        break;

    case DWC_MATCH_COMMAND_NEW_PID_AID:  // Newly connected client data
        // [arakit] main 051010
        // Does nothing unless received from the correct server in an appropriate status
        if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING) ||
            (srcPid != DWCi_GetMatchCnt()->sbPidList[0])){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Ignore delayed NEW_PID_AID command.\n");
            break;
        }
        // [arakit] main 051010
            
        profileID = (int)data[0];
        aid       = (u8)data[1];

#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND) &&
            !DWCi_GetMatchCnt()->distantFriend){
            // when friend specified peer matchmaking does not permit friend-of-friend connections,
            // determine whether a friend specified in the newly connected
            // client's friend index list and send the results to the server
            senddata[0] = (u32)DWCi_IsFriendByIdxList(profileID);

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "profileID %d is acceptable? - %d.\n",
                       profileID, senddata[0]);
            
            result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_FRIEND_ACCEPT,
                                           srcPid, srcIP, srcPort,
                                           senddata, 1);
            if (DWCi_HandleMatchCommandError(result)) return FALSE;
        }
#endif

        // Set the profile ID and AID in the list
        DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1] = profileID;
        DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1]   = aid;

        // Since newly connected clients will soon search for the local host's QR2 data, that data is updated
        // Perform update
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

        if (DWCi_GetMatchCnt()->newClientCallback){
            // Call the new connection client notification callback
            DWCi_GetMatchCnt()->newClientCallback(DWCi_GetFriendListIndex(profileID),
                                           DWCi_GetMatchCnt()->newClientParam);
        }

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "Received new client's profileID = %u & aid = %d.\n",
                   profileID, aid);
        break;

    case DWC_MATCH_COMMAND_LINK_CLS_REQ:  // Request for connection between clients
        // [arakit] main 051010
        // Does nothing unless received from the correct server in an appropriate status
        if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING) ||
            (srcPid != DWCi_GetMatchCnt()->sbPidList[0])){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Ignore delayed LINK_CLS_REQ command.\n");
            break;
        }
        // [arakit] main 051010
        
        profileID = (int)data[0];
        if (!profileID){
            // Connections for all clients have been established.
            aidIdx = data[1];
            aid    = (u8)data[2];

            // Gets one's own AID
            DWCi_GetMatchCnt()->aidList[aidIdx] = aid;
            // The Profile ID of the local host may also be registered in advance
            DWCi_GetMatchCnt()->sbPidList[aidIdx] = DWCi_GetMatchCnt()->profileID;
            
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_CL_FINISH_CONNECT);
        }
        else {
            // NAT negotiation is requested for the specified profile ID.
            // No reservation necessary in this case.
            // Next, the profile ID and AID of the client to be connected are stored
            aidIdx = data[1];
            aid    = (u8)data[2];

            if ((DWCi_GetMatchCnt()->sbPidList[aidIdx] == profileID) &&
                (aidIdx == DWCi_GetMatchCnt()->gt2NumConnection-1)){
                // The connection succeeded command is resent if, due to communications passing each other,
                // a connection request command is received again despite the fact the connection has
                // already been made and the connection succeeded command has been sent.
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Resend command %d for delayed command %d.\n",
                           DWC_MATCH_COMMAND_LINK_CLS_SUC,
                           DWC_MATCH_COMMAND_LINK_CLS_REQ);
                
                result =
                    DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_SUC,
                                          srcPid,
                                          DWCi_GetMatchCnt()->qr2IPList[0],
                                          DWCi_GetMatchCnt()->qr2PortList[0],
                                          (u32 *)&profileID, 1);
                if (DWCi_HandleMatchCommandError(result)) return FALSE;
                break;
            }
            
            DWCi_GetMatchCnt()->sbPidList[aidIdx] = profileID;
            DWCi_GetMatchCnt()->aidList[aidIdx]   = aid;

            // Since a public IP and port number for QR2 are being received, they are recorded in order
            // to accurately search the QR2 data of the client to be connected next
            DWCi_GetMatchCnt()->qr2IPList[aidIdx]   = data[3];
            DWCi_GetMatchCnt()->qr2PortList[aidIdx] = (u16)data[4];
            DWCi_GetMatchCnt()->searchIP            = data[3];
            DWCi_GetMatchCnt()->searchPort          = (u16)data[4];
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Client IP:%x, port:%d\n",
                       DWCi_GetMatchCnt()->searchIP, DWCi_GetMatchCnt()->searchPort);
                    
            // Transition to status for getting server data
            // [note]
            // for friend unspecified peer matchmaking, if private addresses and ports
            // lists are to be used, NAT negotiations will begin here without searching
            // for a server using SBUpdate, so we believe there is room
            // to cut the time down on the order of a half-second
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_NN_HOST);

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Next, try to connect to %u.\n", profileID);

            // Find the other party in the NAT negotiation using SB server update
            sbError = DWCi_SBUpdateAsync(profileID);
            if (DWCi_HandleSBError(sbError)) return FALSE;

//            DWCi_GetMatchCnt()->cmdTimeoutCount = 0;
            DWCi_GetMatchCnt()->cmdTimeoutTime = 0;
//            DWCi_GetMatchCnt()->cmdResendCount   = 0;
            DWCi_GetMatchCnt()->cmdResendFlag = FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_LINK_CLS_SUC:  // Client-to-client connection succeeded
        if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_WAIT_CL_LINK) &&
            (DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->clLinkProgress+1] == data[0])){
            // data[0] is the profile ID of the other party whose client connection has completed.
            // The following connection request is only sent when a connection has completed with another party that had already output a connection request.
            // Send the next connection request.
            DWCi_GetMatchCnt()->clLinkProgress++;

            // Start establishing the next connection
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SV_CONNECT);
        }
        else {
            // Do nothing if a success command regarding an already established connection arrives having passed our outgoing communication
            // received during "result = GT2DuplicateAddress"
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Ignore delayed command %d.\n",
                       DWC_MATCH_COMMAND_LINK_CLS_SUC);
        }
        break;

    case DWC_MATCH_COMMAND_CLOSE_LINK:  // Close connection command
        // [arakit] main 051010
        // Does nothing unless received during the appropriate status
        if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAITING) &&
            (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_WAIT_CLOSE)){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Ignore delayed CLOSE_LINK command.\n");
            break;
        }
        // [arakit] main 051010
            
        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
            DWCi_AreAllBuddies(&data[1], data[0])){
            // If the hosts already connected to a newly connected server that was
            // the source of a close command are all friends, the local host will also attempt to connect there next.
            // It doesn't matter whether or not others are friends when using friend unspecified peer matchmaking.
            DWCi_GetMatchCnt()->priorProfileID = (int)data[1];  // Record next connection target

            // Prepare to resend due to receipt of WAIT
            DWCi_GetMatchCnt()->resvWaitCount = 0;

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Received close command. Next try to %u.\n",
                       data[1]);
        }
        else {
            // If the hosts already connected to a newly connected server, that was the source
            // of a close command, includes hosts who are not friends, next time search for another server to connect to.
            DWCi_GetMatchCnt()->priorProfileID = 0;

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Received close command. Server %u or its clients are not friends.\n",
                       data[1]);
        }
        
        if (DWCi_GetMatchCnt()->gt2NumConnection){
            // The local host closes the connection if it hasn't
            // been closed yet.
            // Processing to follow is handled within callback functions.
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        }
        else {
            // If the connection has already been closed, an attempt will soon be
            // made to connect to a priority reserved host.
            if (DWCi_ResumeMatching()) return FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_CANCEL:  // matchmaking cancellation
        // This command is only received by clients from the point when they have
        // reserved a server right up until a gt2Connection is established.

        // [arakit] main 051010
        // Does nothing unless received from the correct server in an appropriate status
        if (srcPid != DWCi_GetMatchCnt()->sbPidList[0]){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Ignore delayed CANCEL command.\n");
            break;
        }
        // [arakit] main 051010
            
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "Received cancel command from %u data[0] = %d.\n",
                   srcPid, data[0]);
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "numHost nn=%d gt2=%d, state %d\n",
                   DWCi_GetMatchCnt()->qr2NNFinishCount,
                   DWCi_GetMatchCnt()->gt2NumConnection, DWCi_GetMatchCnt()->state);

        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
            (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
            // all client processing is terminated and matchmaking is restarted.
            if (!DWCi_CancelPreConnectedClientProcess(srcPid)) return FALSE;
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            // Matchmaking ends if a server cancels a
            // newly connected client.
            // Record the profile ID of the cancelling host
            DWCi_GetMatchCnt()->cbEventPid = srcPid;

            // Close all remaining connections
            DWCi_CloseAllConnectionsByTimeout();
            DWCi_RestartFromCancel(DWC_MATCH_RESET_ALL);
        }
        break;

    case DWC_MATCH_COMMAND_CANCEL_SYN:
    case DWC_MATCH_COMMAND_CANCEL_SYN_ACK:
    case DWC_MATCH_COMMAND_CANCEL_ACK:
        // process the server-client matchmaking cancellation synchronization adjustment command
        if (!DWCi_ProcessCancelMatchSynCommand(srcPid, command, data[0]))
            return FALSE;
        break;

    case DWC_MATCH_COMMAND_SC_CLOSE_CL:   // Notification of disconnect for already connected clients
        // Does nothing even if this command is somehow received from a host other than the server
        if (srcPid != DWCi_GetMatchCnt()->sbPidList[0]) return TRUE;

        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Close shutdown client.\n");

        // Disconnect and close selected clients
        for (i = 0; i < len; i++){
            aid = DWCi_GetAIDFromProfileID((int)data[i], FALSE);
            if (aid != 0xff){
                DWC_CloseConnectionHard(aid);
            }
        }
        break;

    case DWC_MATCH_COMMAND_POLL_TIMEOUT:  // OPTION_MIN_COMPLETE timeout polling
        if (stpOptMinComp && stpOptMinComp->valid &&
            (OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime) >= stpOptMinComp->timeout)){
            // Even if the OPTION_MIN_COMPLETE option has been set for the local host,
            // 1 is returned once the timeout interval has been exceeded.
            senddata[0] = 1;

            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                       "[OPT_MIN_COMP] time is %lu.\n",
                       OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime));
        }
        else {
            senddata[0] = 0;
        }

        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_POLL_TO_ACK,
                                       srcPid, srcIP, srcPort,
                                       senddata, 1);
        if (DWCi_HandleMatchCommandError(result)) return FALSE;
        break;

    case DWC_MATCH_COMMAND_POLL_TO_ACK:  // OPTION_MIN_COMPLETE timeout polling ACK
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_POLL_TIMEOUT){
            // Set the receive bit from the source client (AID bitmap)
            aid = DWCi_GetAIDFromProfileID(srcPid, FALSE);
            if (aid != 0xff){
                stpOptMinComp->recvBit |= 1 << aid;
            
                if (data[0]){
                    // Set this bit if the client has also timed out
                    stpOptMinComp->timeoutBit |= 1 << aid;
                }
            }
        }
        // Does nothing if ACK is received late while in a different status
        break;
 
    case DWC_MATCH_COMMAND_SC_CONN_BLOCK:  // Reservation denied due to block by OPTION_SC_CONNECT_BLOCK
        // matchmaking ends in an error
        // [arakit] main 051011
        DWCi_StopMatching(DWC_ERROR_MO_SC_CONNECT_BLOCK, 0);
        return FALSE;

#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
    case DWC_MATCH_COMMAND_FRIEND_ACCEPT:  // new connection friend permission from client
        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND) &&
            !DWCi_GetMatchCnt()->distantFriend){
            // proceed to the following only when friend-of-friend connections are not
            // permitted in friend specified peer matchmaking
            // [todo]
            // create a function that determines whether the client is connected to the local host from the profile ID
            for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                if (DWCi_GetMatchCnt()->sbPidList[i] == srcPid){
                    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                               "New client was accepted? - %d.\n", data[0]);
                    if (!data[0]){
                        // for connected clients, when newly connected clients are not
                        // friends included the friend index list,
                        // redo matchmaking
                        if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN) ||
                            (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2)){

                            // while connected to newly connected clients
                            // send the matchmaking cancellation command
                            result = DWCi_InvalidateReservation();
                            if (DWCi_HandleMatchCommandError(result)) return FALSE;
                        }

                        if (DWCi_GetMatchCnt()->nnInfo.cookie){
                            // Cancel during NAT negotiation
                            NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
                            DWCi_GetMatchCnt()->nnInfo.cookie = 0;
                        }

                        // redo matchmaking
                        DWCi_RestartFromTimeout();
                    }
                    else {
                        // record the receipt of newly connected friend permissions
                        DWCi_GetMatchCnt()->friendAcceptBit |=
                            1 << DWCi_GetAIDFromProfileID(srcPid, FALSE);
                    }
                    break;
                }
            }
        }
        break;
#endif

    case DWC_MATCH_COMMAND_CL_WAIT_POLL:  // keep-alive request from the client
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            // if received from a client connected to the local host, 
            // return the keep-alive command
            if (DWCi_GetMatchCnt()->sbPidList[i] == srcPid){
                result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SV_KA_TO_CL,
                                               srcPid, srcIP, srcPort,
                                               NULL, 0);
                if (DWCi_HandleMatchCommandError(result)) return FALSE;
                else break;
            }
        }
        break;

    case DWC_MATCH_COMMAND_SV_KA_TO_CL:  // keep-alive from the server
        // as the first timeout counter initialization occurs in this function,
        // nothing special needs to be done here
        break;

    default:
        // [test]
        // Display only during development
        DWC_Printf(DWC_REPORTFLAG_ERROR,
                   "Received unexpected matching command 0x%02x.\n",
                   command);
        break;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  matchmaking reservation command checking function
  arguments: profileID: The profile ID of the host which sent the command
          qr2IP     Public IP for QR2 of the host sending the command
          qr2Port Public port number for QR2 of the host sending the command
          matchType: Matchmaking type of the host that sent the command
          priorFlag: Priority reservation flag
  return values: Command to be sent back next. DWC_MATCH_COMMAND_RESV_* enumerator.
  application: Determines the command to be sent back next when a NAT negotiation
          reservation command has been received
 *---------------------------------------------------------------------------*/
static u8  DWCi_CheckResvCommand(int profileID, u32 qr2IP, u16 qr2Port, u32 matchType, BOOL priorFlag)
{
    u8  sendCommand;
    int result;

    switch (DWCi_GetMatchCnt()->qr2MatchType){
    case DWC_MATCH_TYPE_FRIEND:   // friend specified peer matchmaking
        if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID)){
            // Ignore reservations from friends in one direction only.
            // Does not even return with an SB message since it's not clear
            // if the other party's QR2 is up.
            sendCommand = DWC_MATCH_COMMAND_DUMMY;  // Dummy command
            break;
        }
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
        else if (!DWCi_IsFriendByIdxList(profileID)){
            // deny reservations from friends not on the friend index list
            sendCommand = DWC_MATCH_COMMAND_RESV_DENY;  // Deny reservation command
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "This friend doesn't exist in friendIdxList.\n");
            break;
        }
#endif
        // Proceed to processing common to continued friend unspecified peer matchmaking
        
    case DWC_MATCH_TYPE_ANYBODY:  // friend unspecified peer matchmaking
        if ((matchType != DWCi_GetMatchCnt()->qr2MatchType)                    ||
            (DWCi_GetMatchCnt()->cancelState != DWC_MATCH_CANCEL_STATE_INIT)   ||
            (DWCi_GetMatchCnt()->qr2NNFinishCount == DWCi_GetMatchCnt()->qr2NumEntry) ||
            (DWCi_GetMatchCnt()->qr2IsReserved &&
             (DWCi_GetMatchCnt()->qr2Reservation == DWCi_GetMatchCnt()->profileID))){
            // The reservation is rejected if the local host is already a connected client 
            // or if the number of connections is already full.
            sendCommand = DWC_MATCH_COMMAND_RESV_DENY;  // Deny reservation command

            if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) &&
                !DWCi_GetMatchCnt()->qr2Obj->userstatechangerequested &&
                DWCi_GetMatchCnt()->qr2IsReserved &&
                (DWCi_GetMatchCnt()->qr2Reservation == DWCi_GetMatchCnt()->profileID)){
                // If the reservation command has been received even though the client
                // is already connected with friend unspecified peer matchmaking, code is designed
                // to quickly reflect information regarding established connections on the server.
                qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
            }
        }
        else if (((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_SEARCH_HOST) &&
                  (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_WAIT_RESV))     ||
                 ((DWCi_GetMatchCnt()->qr2IP == 0) && (DWCi_GetMatchCnt()->qr2Port == 0)) ||
                 ((qr2IP == 0) && (qr2Port == 0))){
            // As long as local host matchmaking is progressing, wait until that ends.
            // Same when the local host has not yet determined the public IP and port for QR2 of the other party
            sendCommand = DWC_MATCH_COMMAND_RESV_WAIT;  // Reservation wait command
        }
        else {
            if (DWCi_GetMatchCnt()->reqProfileID){
                // Wait status after an NN request is sent to someone
                if (DWCi_GetMatchCnt()->reqProfileID == profileID){
                    // When an NN request arrives from a party which the local host sent an NN request to
                    if (priorFlag ||
                        ((DWCi_GetMatchCnt()->profileID < profileID) &&
                        (profileID != DWCi_GetMatchCnt()->priorProfileID))){
                        // The reservation is approved if the profile ID of the local host has a lower value 
                        // than the other party or if the priority reservation flag is on.
                        // However, even if the above conditions are satisfied, requests from other party's to which
                        // a priority reserve request was sent by the local host are not approved.
                        sendCommand = DWC_MATCH_COMMAND_RESV_OK;  // Approve reservation command
                    }
                    else {
                        // Sets a dummy command for when a command is not to be sent, because there
                        // should be a response to the NN request from the other party.
                        sendCommand = DWC_MATCH_COMMAND_DUMMY;  // Dummy command
                    }
                }
                else {
                    // When an NN request arrives from a party which the local host has not sent an NN request to
                    if (priorFlag ||
                        ((DWCi_GetMatchCnt()->profileID < profileID) &&
                        !DWCi_GetMatchCnt()->priorProfileID)){
                        // If the profile ID of the local host has a lower value than the other party and 
                        // a priority reservation request has not been sent, or if the priority reservation
                        // flag is on, the NN request from the local host is abandoned and the received NN request is approved.
                        // Performs reservation cancellation.
                        result = DWCi_CancelReservation(DWCi_GetMatchCnt()->reqProfileID);
                        if (DWCi_HandleMatchCommandError(result)){
                            return DWC_MATCH_COMMAND_DUMMY;  // Dummy command
                        }
                        else {
                            sendCommand = DWC_MATCH_COMMAND_RESV_OK;  // Approve reservation command
                        }
                    }
                    else {
                        sendCommand = DWC_MATCH_COMMAND_RESV_DENY;  // Deny reservation command
                    }
                }
            }
            else {
                sendCommand = DWC_MATCH_COMMAND_RESV_OK;    // Approve reservation command
            }
        }
        break;

    case DWC_MATCH_TYPE_SC_SV:  // server-client matchmaking server-side
        if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, profileID)){
            // Ignore reservations from friends in one direction only.
            // Doesn't even return with an SB message since it's not clear
            // if the other party's QR2 is up.
            sendCommand = DWC_MATCH_COMMAND_DUMMY;  // Dummy command
        }
        else if ((matchType != DWC_MATCH_TYPE_SC_CL) ||
                 (DWCi_GetMatchCnt()->qr2NNFinishCount == DWCi_GetMatchCnt()->qr2NumEntry)){
            // Deny reservation when already full.
            // Also deny reservations if an NN request is received immediately
            // after matchmaking has been cancelled.
            sendCommand = DWC_MATCH_COMMAND_RESV_DENY;  // Deny reservation command
        }
        else if ((stOptSCBlock.valid == 1) && (stOptSCBlock.lock == 1)){
            // Send a special deny command when new connections have been
            // blocked by setting the OPTION_SC_CONNECT_BLOCK option
            sendCommand = DWC_MATCH_COMMAND_SC_CONN_BLOCK;
        }
        else if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_WAITING)         ||
                 ((DWCi_GetMatchCnt()->qr2IP == 0) && (DWCi_GetMatchCnt()->qr2Port == 0)) ||
                 ((qr2IP == 0) && (qr2Port == 0))){
            // As long as processing for establishing connections with other players is progressing,
            // have the software wait until the processing ends.
            // Same when the local host has not yet determined the public IP and port for QR2 of the other party
            sendCommand = DWC_MATCH_COMMAND_RESV_WAIT;  // Reservation wait command
        }
        else {
            sendCommand = DWC_MATCH_COMMAND_RESV_OK;    // Approve reservation command
        }
        break;
    }

    return sendCommand;
}


/*---------------------------------------------------------------------------*
  matchmaking reservation OK response pre-processing function
  arguments: profileID: The profile ID of the host which sent the reservation command
          ip: The public IP for the QR2 of the other party when using friend unspecified peer matchmaking
          port: The public port number for the QR2 of the other party when using friend unspecified peer matchmaking
  return values: SBError type or the value resulting from conversion from GPResult type to int type
  application: Makes several settings and notifies already connected clients regarding
          newly connected clients before returning OK for a NAT negotiation request.
 *---------------------------------------------------------------------------*/
static int DWCi_ProcessResvOK(int profileID, u32 ip, u16 port)
{
    u32  senddata[2];
    int  result;
    int  i;

    if (DWCi_GetMatchCnt()->qr2IsReserved && (DWCi_GetMatchCnt()->qr2Reservation == profileID)){
        // Does nothing when control arrives here when the reservation has already been made
        return 0;
    }

    // Establish reservation by client
    DWCi_GetMatchCnt()->qr2IsReserved   = 1;
    DWCi_GetMatchCnt()->qr2Reservation  = profileID;
//    DWCi_GetMatchCnt()->cmdResendCount  = 0;  // Clears the command resend counter
    DWCi_GetMatchCnt()->cmdResendFlag   = FALSE;  // Clear command resend counter
//    DWCi_GetMatchCnt()->cmdTimeoutCount = 0;  //clears the command response timeout counter
    DWCi_GetMatchCnt()->cmdTimeoutTime = 0;  // Clear command response timeout count

    // Notifies the master server of changes in status
    qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

    // Clear the profile ID of the source of the NN request in order to use friend specified peer matchmaking
    DWCi_GetMatchCnt()->reqProfileID = 0;

    // Store the Profile ID of the other party on the list
    DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1] = profileID;

    // Set the public IP and port number for QR2 of the other party
    DWCi_GetMatchCnt()->qr2IPList[DWCi_GetMatchCnt()->qr2NNFinishCount+1]    = ip;
    DWCi_GetMatchCnt()->qr2PortList[DWCi_GetMatchCnt()->qr2NNFinishCount+1] = port;
    DWCi_GetMatchCnt()->searchIP   = ip;
    DWCi_GetMatchCnt()->searchPort = port;

    // Register the AID on the AID list
    DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1] = DWCi_GetAIDFromList();

    // Send the profile ID and AID of the newly connected client to the already connected clients
    // Send ACK
    senddata[0] = (u32)profileID;
    senddata[1] = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->qr2NNFinishCount+1];

    for (i = 1; i <= DWCi_GetMatchCnt()->qr2NNFinishCount; i++){
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_NEW_PID_AID,
                                       DWCi_GetMatchCnt()->sbPidList[i],
                                       DWCi_GetMatchCnt()->qr2IPList[i],
                                       DWCi_GetMatchCnt()->qr2PortList[i],
                                       senddata, 2);
        if (result) return result;
    }

    // Initialize this parameter as long as the matchmaking complete option for the number of specified
    // players or less has been set in cases where it has been determined that the local host will play the role of server.
    DWCi_InitOptMinCompParam(TRUE);

    return 0;
}


/*---------------------------------------------------------------------------*
  Function for backing up the list of hosts already connected to the server
  arguments: profileID: The profile ID of the server
          data: List of hosts already connected as received from the server
  return values: None
  application: Backs up data regarding hosts already connected to the server if the reservation 
          OK command has been received from the server when using friend specified peer matchmaking
 *---------------------------------------------------------------------------*/
static void DWCi_MakeBackupServerData(int profileID, const u32 data[])
{
    u32 len;

    // svDataBak[1] represents the profile ID of the server with pidList
    // appended to the end. svDataBak[0] represents the length of the profile ID list.
    len = data[0]+2;
    if (len > 2){
        MI_CpuCopy32(&data[1], &DWCi_GetMatchCnt()->svDataBak[2], sizeof(u32)*(len-2));
    }

    DWCi_GetMatchCnt()->svDataBak[0] = len-1;
    DWCi_GetMatchCnt()->svDataBak[1] = (u32)profileID;
}


/*---------------------------------------------------------------------------*
  matchmaking command process error handling function
  arguments: int: SBError type or GPResult type
  return values: Returns data received in parameters as int type
  applications: Since both GP friend messages and SB messages are used with the 
          matchmaking command, both types of errors can be handled here, but only one error is returned.
 *---------------------------------------------------------------------------*/
static int DWCi_HandleMatchCommandError(int error)
{

    // [todo]
    // Since a command is sometimes sent using an SB message even in cases where
    // friend unspecified peer matchmaking is not being used, it's not accurate to assume this only happens in this case
    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
        return (int)DWCi_HandleSBError((SBError)error);
    }
    else {
        return (int)DWCi_HandleGPError((GPResult)error);
    }
}


/*---------------------------------------------------------------------------*
  Function for sending the NN reservation command
  arguments: profileID: The profile ID of the server to be reserved.
                    used for friend specified, server-client matchmaking.
          delay: TRUE: Sends the command after a specific frame,
                    FALSE: Sends the command immediately
  return values: SBError type or the value resulting from conversion from GPResult type to int type
  application: Sets the specified server data and sends an NN reservation command
 *---------------------------------------------------------------------------*/
static int DWCi_SendResvCommand(int profileID, BOOL delay)
{
    u8  command;
    u32 senddata[3];
    int sendlen;
    int result;
    SBServer server;

    if (delay || ((DWCi_GetMatchCnt()->qr2IP == 0) && (DWCi_GetMatchCnt()->qr2Port == 0))){
        // Set the resend counter when you wish to delay an actual transmission and the NN reservation
        // request will be sent in the form of a resend within DWCi_MatchProcess() as long as the profile
        // ID of the next host to be sent to has been set ahead of time in Profile ID List[0].
        // If a public IP and port number for QR2 have not been determined,
        // the transmission must be delayed because the server cannot be notified.
//        DWCi_GetMatchCnt()->cmdResendCount   = 1;
        DWCi_GetMatchCnt()->cmdResendFlag = TRUE;
        DWCi_GetMatchCnt()->cmdResendTick = OS_GetTick();
        DWCi_GetMatchCnt()->sbPidList[0] = profileID;

        if (!delay){
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "Delay ResvCommand - qr2IP & qr2Port = 0.\n");
        }
        return 0;
    }

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
        // for friend unspecified peer matchmaking, set the server
        // candidate profile ID, QR2 public IP and port number to the list
        server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, 0); 
        DWCi_GetMatchCnt()->sbPidList[0]   =
            SBServerGetIntValue(server, DWC_QR2_PID_KEY_STR, 0);
        DWCi_GetMatchCnt()->qr2IPList[0]   = SBServerGetPublicInetAddress(server);
        DWCi_GetMatchCnt()->qr2PortList[0] = SBServerGetPublicQueryPort(server);

        // Record the profile ID of the NN request recipient
        DWCi_GetMatchCnt()->reqProfileID = DWCi_GetMatchCnt()->sbPidList[0];
        sendlen = 1;
    }
    else {
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
            // Set the profile ID of the server candidate at the start of the profile ID list
            DWCi_GetMatchCnt()->sbPidList[0] = profileID;
        }

        // Record the profile ID of the NN request recipient
        DWCi_GetMatchCnt()->reqProfileID = profileID;

        // for friend specified server-client matchmaking
        // Send the public IP and port number for QR2 here.
        // Stored in the SB message header when using friend unspecified peer matchmaking.
        senddata[1] = DWCi_GetMatchCnt()->qr2IP;
        senddata[2] = DWCi_GetMatchCnt()->qr2Port;
        sendlen = 3;
    }
    
//    DWCi_GetMatchCnt()->cmdTimeoutCount = 1;  // Start the timeout count
    DWCi_GetMatchCnt()->cmdTimeoutTime = DWC_MATCH_CMD_RESV_TIMEOUT_MSEC;
    DWCi_GetMatchCnt()->cmdTimeoutStartTick = OS_GetTick();
//    DWCi_GetMatchCnt()->cmdResendCount   = 0;
    DWCi_GetMatchCnt()->cmdResendFlag  = FALSE;
        
    if (DWCi_GetMatchCnt()->priorProfileID){
        // Send the priority reservation command if disconnected from the server and
        // the disconnected server mediates for the next server to be connected to
        command = DWC_MATCH_COMMAND_RESV_PRIOR;
    }
    else {
        // Regular reservation command
        command = DWC_MATCH_COMMAND_RESERVATION;
    }

    // Set the matchmaking type of the local host into send data
    senddata[0] = DWCi_GetMatchCnt()->qr2MatchType;

    // NN send reservation command
    // Since the reservation command is always sent to the server, use [0] for the index
    result = DWCi_SendMatchCommand(command,
                                   profileID,
                                   DWCi_GetMatchCnt()->qr2IPList[0],
                                   DWCi_GetMatchCnt()->qr2PortList[0],
                                   senddata, sendlen);

    return result;
}


/*---------------------------------------------------------------------------*
  friend specified peer matchmaking NN reservation command sending function
  arguments: delay: TRUE: Sends the command after a specific frame,
                    FALSE: Sends the command immediately
          init: TRUE: First call. to avoid incrementing friendCount,
                    FALSE: Other
          resendPid: in friend specified peer matchmaking, even when delay = FALSE,
                    when sending a reservation command continuously for the
                    same profile ID as the prior time, the prior send target's
                    profile ID is specified. If not needed, specify 0.
  return values: SBError type or the value resulting from conversion from GPResult type to int type
          arguments: Always 0 when delay = TRUE
  application: Selects the server to attempt connection with next from the friend index list,
          sets the data for that server, and sends an NN reservation command.
          Send an NN reservation command after a set interval when delay = TRUE.
 *---------------------------------------------------------------------------*/
static int DWCi_SendResvCommandToFriend(BOOL delay, BOOL init, int resendPid)
{
    char version[12];
    char numEntry[4];
    char distantFriend[2];
    int  profileID;
    int  startIdx = init ? DWCi_GetMatchCnt()->friendCount :
                           (DWCi_GetMatchCnt()->friendCount < DWCi_GetMatchCnt()->friendIdxListLen-1) ? DWCi_GetMatchCnt()->friendCount+1 : 0;
    int  buddyIdx;
    int  initFinished = 0;
    int  versionLen, numEntryLen, distantFriendLen;
    int  i;
    GPBuddyStatus status;
    GPResult gpResult;

    while (1){
        if (!init || initFinished){
            DWCi_GetMatchCnt()->friendCount++;
            if (DWCi_GetMatchCnt()->friendCount >= DWCi_GetMatchCnt()->friendIdxListLen){
                // Return to the beginning once everything has been checked to the end.
                DWCi_GetMatchCnt()->friendCount = 0;
            }
        }

        if (initFinished){
            if (DWCi_GetMatchCnt()->friendCount == startIdx){
                // Wait for resend until timeout if no one is present for one loop
                // However, the timeout has been shortened because detection is 
                // possible when two players have started matchmaking simultaneously
                // by waiting a little bit for GPStatus to be updated.
//                DWCi_GetMatchCnt()->cmdTimeoutCount =
//                    DWC_MATCH_CMD_RESV_TIMEOUT-DWC_MATCH_CMD_RESEND_INTERVAL;
                DWCi_GetMatchCnt()->cmdTimeoutTime = DWC_MATCH_CMD_RESEND_INTERVAL_MSEC;
                DWCi_GetMatchCnt()->cmdTimeoutStartTick = OS_GetTick();

//                DWCi_GetMatchCnt()->cmdResendCount   = 0;
                DWCi_GetMatchCnt()->cmdResendFlag  = FALSE;
                return 0;
            }
        }
        initFinished = 1;

        profileID = DWC_GetGsProfileId(DWCi_GetUserData(), &DWCi_GetMatchCnt()->friendList[DWCi_GetMatchCnt()->friendIdxList[DWCi_GetMatchCnt()->friendCount]]);

#ifdef DWC_MATCH_ACCEPT_NO_FRIEND
        // Does nothing if a profile ID is not ready yet
        if ((profileID == 0) || (profileID == -1)) continue;

        // Does nothing even if the other party is not an established friend
        if (!DWCi_Acc_IsValidFriendData(&DWCi_GetMatchCnt()->friendList[DWCi_GetMatchCnt()->friendIdxList[DWCi_GetMatchCnt()->friendCount]])) continue;
#endif

        // Check whether or not the next server candidate is already connected
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            if (DWCi_GetMatchCnt()->sbPidList[i] == profileID) break;
        }

        if (i > DWCi_GetMatchCnt()->gt2NumConnection){
            // Check whether or not the other party is conducting friend specified peer matchmaking
            gpResult  = gpGetBuddyIndex(DWCi_GetMatchCnt()->pGpObj, profileID, &buddyIdx);
            gpResult |= gpGetBuddyStatus(DWCi_GetMatchCnt()->pGpObj, buddyIdx, &status);

            if (!gpResult && (status.status == DWC_STATUS_MATCH_FRIEND)){
                // Since the other party is also performing friend specified peer matchmaking, this function
                // checks if the version matches that of the local host, if the number of connection requests matches,
                // and if the "allow friend of a friend" attribute has been set the same as the local host.
                versionLen =
                    DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_VERSION,
                                             version, 
                                             status.statusString,
                                             '/');
                numEntryLen = 
                    DWC_GetCommonValueString(DWC_GP_SSTR_KEY_MATCH_FRIEND_NUM,
                                             numEntry, 
                                             status.statusString,
                                             '/');
                distantFriendLen = 
                    DWC_GetCommonValueString(DWC_GP_SSTR_KEY_DISTANT_FRIEND,
                                             distantFriend,
                                             status.statusString,
                                             '/');

                if ((versionLen > 0) && (numEntryLen > 0) && (distantFriendLen > 0) &&
                    (strtoul(version, NULL, 10) == DWC_MATCHING_VERSION) &&
                    (strtoul(numEntry, NULL, 10) == DWCi_GetMatchCnt()->qr2NumEntry)){
                    // Determined as server to be sent a reservation
                    break;
                }
            }
            // Ignore gpResult error
        }
    }

    // to prevent the continued sending of repeated profile IDs for when, e.g., there is
    // only one friend, when the prior send target's profile ID is set as resendPid,
    // and the current send target is the same, send only after a certain frame
    if (profileID == resendPid) delay = TRUE;

    // Send an NN reservation request to the next friend
    return DWCi_SendResvCommand(profileID, delay);
}


/*---------------------------------------------------------------------------*
  Reservation command resend retry function
  arguments: resendPid: in friend specified peer matchmaking, even when delay = FALSE,
                    when sending a reservation command continuously for the
                    same profile ID as the prior time, the prior send target's
                    profile ID is specified. If not needed, specify 0.
  return values: SBError type or the value resulting from conversion from GPResult type to int type
  application: The reservation command is sent when the RESV_DENY command is
          received or after a command response timeout.
          Retries are not required in the case of server-client matchmaking
          clients. Error processing is handled inside the function.
 *---------------------------------------------------------------------------*/
static BOOL DWCi_RetryReserving(int resendPid)
{
    SBError sbError;
    int result;

    DWCi_GetMatchCnt()->priorProfileID = 0;  // Clear priority reservation
    DWCi_GetMatchCnt()->reqProfileID   = 0;  // Clear the reservation

    DWCi_GetMatchCnt()->resvWaitCount  = 0;  // Clear resend counter when WAIT is received
    // Reset the start time for measuring the timeout
    DWCi_GetMatchCnt()->cmdTimeoutStartTick = OS_GetTick();

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
        // Return to searching for available hosts
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);

        // Update the SB server
        // [todo]
        // If this is so, even when the client is already connected to the server, the fact
        // that an SP update will immediately occur is not good
        sbError = DWCi_SBUpdateAsync(0);
        if (DWCi_HandleSBError(sbError)) return FALSE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
        // Send an NN reservation request to the next friend
        result = DWCi_SendResvCommandToFriend(FALSE, FALSE, resendPid);
        if (DWCi_HandleMatchCommandError(result)) return FALSE;
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
        // Matchmaking is cancelled and halted if the player
        // capacity of the server is exceeded at this instant
        DWCi_StopMatching(DWC_ERROR_NETWORK,
                          DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_OTHER);
        return FALSE;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Function for cancelling NN reservations
  arguments: profileID: The profile ID of the server for which reservation is to be cancelled
  return values: SBError type or the value resulting from conversion from GPResult type to int type
  application: Sends the NN reservation cancel command.
 *---------------------------------------------------------------------------*/
static int  DWCi_CancelReservation(int profileID)
{
    int result;

    result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_RESV_CANCEL,
                                   profileID,
                                   DWCi_GetMatchCnt()->qr2IPList[0],
                                   DWCi_GetMatchCnt()->qr2PortList[0],
                                   NULL, 0);

    // Clear the the profile ID of the NN request recipient
    DWCi_GetMatchCnt()->reqProfileID = 0;

    return result;
}


/*---------------------------------------------------------------------------*
  Function for cancelling processing for servers before connection is established
  arguments: clientPid: Profile ID of the already reserved client
  return values: TRUE: Success, FALSE: Failure
  application: Cancels all processing for servers in the middle of establishing a connection 
          and restarts matchmaking as-is for current connections.
          If there has been an NN failure with a newly connected client or a GT2 connect failure, control flow will
          sometimes arrive at this function even in the case of already connected clients.
          This functions assumes that it has been called when gt2Connect with
          newly connected clients has not been completed.
 *---------------------------------------------------------------------------*/
static BOOL DWCi_CancelPreConnectedServerProcess(int clientPid)
{
    int  i;
    BOOL isServer;

    if (DWCi_GetMatchCnt()->qr2IsReserved && (DWCi_GetMatchCnt()->qr2Reservation == DWCi_GetMatchCnt()->profileID)){
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "DWCi_CancelPreConnectedServerProcess : client\n");
        isServer = FALSE;
    }
    else {
        DWC_Printf(DWC_REPORTFLAG_DEBUG,
                   "DWCi_CancelPreConnectedServerProcess : server\n");
        isServer = TRUE;
    }

    if (isServer){
        // Clear reservations if the host is a server.
        // If the host is a client, the reservation is usually cleared inside
        // DWCi_RestartFromTimeout(), which is still to be called; however, reservations
        // with the profile ID of the local host are not cleared because matchmaking
        // continues for server-client clients already connected.
        DWCi_GetMatchCnt()->qr2IsReserved   = 0;
        DWCi_GetMatchCnt()->qr2Reservation  = 0;
    
        // Notifies the master server of changes in status
        qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);
    }

    // Remove the profile ID of the reserved client from the list.
    // It is assumed that newly connected clients are not included in gt2NumConnection.
    if (DWCi_GetMatchCnt()->gt2NumConnection < DWC_MAX_CONNECTIONS-1){
        DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1] = 0;
    }

    // Terminating reservation hold timeout measurement
    DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;

    if (DWCi_GetMatchCnt()->nnInfo.cookie){
        // Cancel during NAT negotiation
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }

    // Return parameters to status before the search was started in order to restart searching for a server.
    DWCi_GetMatchCnt()->qr2NNFinishCount = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
    DWCi_GetMatchCnt()->reqProfileID     = 0;

    if (!isServer){
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            // In server-client matchmaking, when a connected client
            // detects a non-responding newly-connected client,
            // a connection timeout among the server and clients is detected
            // and they wait for a cancel command to be sent.
            // [todo]
            // Sets the timeout interval to use until the cancel command is received.
            // It's necessary to determine that the server has shutdown if the timeout is exceeded.
        }
        else {
            // For other forms of matchmaking, all connections are closed
            // and matchmaking is redone.
            DWCi_RestartFromTimeout();
        }
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
        // Return to available host search status when using friend unspecified peer matchmaking
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
        
        // Searches for an available host after a specific frame
//        DWCi_GetMatchCnt()->sbUpdateCount    = 1;
        DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
        DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
        // Return to original status when using friend specified peer matchmaking
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

        // Send NN reservation request after a specific frame
        (void)DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);
    }
    else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
        // for the server-client matchmaking server
        // a cancel is sent to connected clients.
        // Proceed to "wait for receipt of cancel SYN-ACK" status
        DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_CANCEL_SYN);

        DWCi_GetMatchCnt()->cancelSynAckBit   = 0;  // Clear the cancel SYN-ACK receive bit
        DWCi_GetMatchCnt()->cancelBaseLatency = 0;  // Clear latency for syncing cancellation processing

        // Close newly connected clients
        DWCi_CloseCancelHostAsync(clientPid);

        // Send a SYN packet to already connected clients.
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            if (!DWCi_SendCancelMatchSynCommand(DWCi_GetMatchCnt()->sbPidList[i],
                                                DWC_MATCH_COMMAND_CANCEL_SYN))
                return FALSE;
        }

        if (DWCi_GetMatchCnt()->gt2NumConnection == 0){
            // Set to continued matchmaking status since the above SYN packet will not cause any 
            // connections to close as long as there are no connected hosts.
            DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
        }
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Function for cancelling processing for clients before connection is established
  arguments: serverPid: The profile ID of the game server to be reserved
  return values: TRUE: Success, FALSE: Failure
  application: Cancels all processes for clients during negotiations
          prior to establishing a connection.
 *---------------------------------------------------------------------------*/
static BOOL DWCi_CancelPreConnectedClientProcess(int serverPid)
{
#pragma unused(serverPid)
    BOOL result = TRUE;

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
        // if there is a newly-connected client in server-client matchmaking
        // connection processing ends in an error
        if (DWCi_GetMatchCnt()->gt2NumConnection){
            // Disconnect all connections
            DWCi_CloseAllConnectionsByTimeout();
        }

        // matchmaking error ends
        DWCi_StopMatching(DWC_ERROR_NETWORK,
                          DWC_ECODE_SEQ_MATCH+DWC_ECODE_TYPE_SC_CL_FAIL);
        return FALSE;
    }

    // Return parameters to status before the search was started in order to restart searching for a server.
    DWCi_GetMatchCnt()->qr2NNFinishCount = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
    DWCi_GetMatchCnt()->priorProfileID   = 0;

    if (DWCi_GetMatchCnt()->nnInfo.cookie){
        // Cancel during NAT negotiation
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }

    if (DWCi_GetMatchCnt()->gt2NumConnection){
        // If gt2Connect() has already been accepted when currently connected to a client
        // connected to a server or when currently connected to a server,
        // all connections are closed and matchmaking is redone.
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "Close all connection and restart matching.\n");
            
        DWCi_RestartFromTimeout();
    }
    else {
        // Restores original status if there still is no connection
        // and restarts client matchmaking.
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "Cancel and restart client process.\n");
        
        // Restore status
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Cancel and retry to reserve.\n");

        // Search for the next server using the same processing if the RSV_DENY command is received.
        result = DWCi_RetryReserving(0);
    }

    return result;
}


/*---------------------------------------------------------------------------*
  client transfer function
  arguments: None
  return values: SBError type or the value resulting from conversion from GPResult type to int type
  application: Returns completely to the client to connect with a new server
 *---------------------------------------------------------------------------*/
static int  DWCi_ChangeToClient(void)
{
    int result;
    int i;

    // Send a close command to already connected clients
    for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_CLOSE_LINK,
                                       DWCi_GetMatchCnt()->sbPidList[i],
                                       DWCi_GetMatchCnt()->qr2IPList[i],
                                       DWCi_GetMatchCnt()->qr2PortList[i],
                                       DWCi_GetMatchCnt()->svDataBak,
                                       (int)(DWCi_GetMatchCnt()->svDataBak[0]+1));
        if (result) return result;
    }

    DWCi_GetMatchCnt()->qr2IsReserved  = 0;
    DWCi_GetMatchCnt()->qr2Reservation = 0;

    // Close status is set for clients during rotation processing
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_CHANGE_TO_CLIENT;
    
    // Disconnect all connections
    gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);

    // Initialize close status
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_INIT;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "Closed all connections. Begin NN to %u\n",
               DWCi_GetMatchCnt()->reqProfileID);

    return 0;
}


/*---------------------------------------------------------------------------*
  Function for processing after connection established
  arguments: type: Processing type. DWCMatchPpConnectionType type.
  return values: None
  application: Starts establishing the next connection after an established connection has ended
          or performs connection complete processing.
 *---------------------------------------------------------------------------*/
static void DWCi_PostProcessConnection(DWCMatchPpConnectionType type)
{
    u32 senddata[5];
    int sendlen = 3;
    int result;
    int i;
    BOOL sbClear = FALSE;
    GPResult gpResult;

    switch (type){
    case DWC_PP_CONNECTION_SV_CONNECT:
        // Complete one connection as server
        if (DWCi_GetMatchCnt()->clLinkProgress < DWCi_GetMatchCnt()->gt2NumConnection-1){
            // If none of the connections to the newly connected client or
            // already connected client have ended.
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Send client-client link request.\n");

            // Transition to "wait to establish connection between clients" status
            DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAIT_CL_LINK);

            // Makes notification regarding the profile ID, AID index and AID of the
            // other client you wish would go find a client connection
            senddata[0] = (u32)DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->clLinkProgress+1];
            senddata[1] = (u32)(DWCi_GetMatchCnt()->clLinkProgress+1);
            senddata[2] = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->clLinkProgress+1];

            // Send the public IP and port number for qr2
            senddata[3] = DWCi_GetMatchCnt()->qr2IPList[DWCi_GetMatchCnt()->clLinkProgress+1];
            senddata[4] = DWCi_GetMatchCnt()->qr2PortList[DWCi_GetMatchCnt()->clLinkProgress+1];
            sendlen     = 5;
        }
        else {
            // This code is for when the local host's GT2 connection with the first client
            // has been completed or when connections between all connected clients have ended.
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "Tell new client completion of matching.\n");
            // Cancel reservation
            DWCi_GetMatchCnt()->qr2IsReserved  = 0;
            DWCi_GetMatchCnt()->qr2Reservation = 0;

            // Notifies the master server of changes in status
            qr2_send_statechanged(DWCi_GetMatchCnt()->qr2Obj);

            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                // Return to available host search status when using friend unspecified peer matchmaking
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
            }
            else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
                // Return to client initialization status when using friend specified peer matchmaking
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
            }
            else {
                // Return to standby status for a server-client matchmaking server
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAITING);
            }
            
            DWCi_GetMatchCnt()->clLinkProgress = 0;  // Clear number of connections between clients

            if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
                (DWCi_GetMatchCnt()->gt2NumConnection == DWCi_GetMatchCnt()->qr2NumEntry)){
                // specified player numbers network completed (server-client matchmaking
                // would connect per device).
                // Back up the profile ID to be passed to the callback
                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                    DWCi_GetMatchCnt()->cbEventPid =
                        DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection];
                }
                else {
                    DWCi_GetMatchCnt()->cbEventPid = 0;

                    // The profile ID of the local host is set into a list here for
                    // hosts that have been converted from temporary servers into clients.
                    // (This is required when matchmaking has been completed for a set number of connections.)
                    DWCi_GetMatchCnt()->sbPidList[0] = DWCi_GetMatchCnt()->profileID;
                }

                // Proceed to "wait for receipt of cancel SYN-ACK" status
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_SYN);

                // Clear Syn-ACK receive bit
                DWCi_GetMatchCnt()->synAckBit = 0;

                // Send a SYN packet to already connected clients.
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    DWCi_SendMatchSynPacket(DWCi_GetMatchCnt()->aidList[i],
                                            DWC_SEND_TYPE_MATCH_SYN);
                }
            }
            else {
                // Wait a bit and search for new hosts to connect to when the number of connected hosts
                // is less than the number specified when using friend specified/unspecified peer matchmaking.
                senddata[0] = 0;  // Make notification when processing ends with profile ID = 0
                // Make notification regarding the AID index and AID for the newly connected client
                senddata[1] = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
                senddata[2] = DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection];

                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                    // Return to available host search status when using friend unspecified peer matchmaking
//                    DWCi_GetMatchCnt()->sbUpdateCount    = 1;
                    DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                    DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
                }
                else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
                    // Send an NN reservation request after a specific frame
                    // when using friend-specified peer matchmaking
                    (void)DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);

#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
                    if (!DWCi_GetMatchCnt()->distantFriend &&
                        (DWCi_GetMatchCnt()->gt2NumConnection >= 2)){
                        // when any connections for clients for the second or later party,
                        // without permitting friend-of-friend connections,
                        // determine whether newly connected friend permission commands
                        // can all be received from connected clients
                        if (DWCi_GetMatchCnt()->friendAcceptBit !=
                            (DWCi_GetAIDBitmask(FALSE) & ~(1 << DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection]))){
                            // if not, redo matchmaking
                            // [todo]
                            // when a possible delay is considered, it would
                            // be better to wait a little longer
                            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                                       "FRIEND_ACCEPT command droped.\n");
                            DWCi_RestartFromTimeout();
                            break;
                        }
                    }
#endif
                }
            }

            if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV){
                // if not server-client matchmaking
                // clear server browsing
                sbClear = TRUE;
            }
        }

        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_SYN){
            // Make notification regarding connection requests between clients (end notification if all are ended)
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_REQ,
                                      DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection],
                                      DWCi_GetMatchCnt()->qr2IPList[DWCi_GetMatchCnt()->gt2NumConnection],
                                      DWCi_GetMatchCnt()->qr2PortList[DWCi_GetMatchCnt()->gt2NumConnection],
                                      senddata, sendlen);
            if (DWCi_HandleMatchCommandError(result)) return;

            DWCi_GetMatchCnt()->cmdCnt.count = 0;  // Start command resend count
        }
        break;
    
    case DWC_PP_CONNECTION_CL_GT2_CONNECT:
        // Return to wait status
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);

        // for the server-client matchmaking client
        // back up the profile ID to be passed to the callback here.
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
            DWCi_GetMatchCnt()->cbEventPid = DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection];
        }

        // clear server browsing
        sbClear = TRUE;
        break;

    case DWC_PP_CONNECTION_CL_GT2_ACCEPT:
        // Enter wait status
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);

        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
            (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
            // When using friend specified/unspecified peer matchmaking, set reservation status for the 
            // first time here using the profile ID of the local host to prevent other hosts from connecting.
            DWCi_GetMatchCnt()->qr2IsReserved  = 1;
            DWCi_GetMatchCnt()->qr2Reservation = DWCi_GetMatchCnt()->profileID;
        }

        // we're in standby mode until a DWC_MATCH_COMMAND_LINK_CLS_REQ command
        // is received next from the server, and the timeout counter is initialized
        DWCi_InitClWaitTimeout();
        
        if (DWCi_GetMatchCnt()->gt2NumConnection > 1){
            // When a newly connected client has established a connection with an already 
            // connected client, the server is notified of the completion of the connection.
            // The server is also notified of the profile IDs of the clients for which the connection was established.
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_LINK_CLS_SUC,
                                      DWCi_GetMatchCnt()->sbPidList[0],
                                      DWCi_GetMatchCnt()->qr2IPList[0],
                                      DWCi_GetMatchCnt()->qr2PortList[0],
                                      (u32 *)&DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection-1],
                                      1);
            if (DWCi_HandleMatchCommandError(result)) return;
        }
        // Wait for an indication if the connection from the server is complete or if the connection is to an already connected client.
        break;
        
    case DWC_PP_CONNECTION_CL_FINISH_CONNECT:
        // Recovery is possible since an aid is always sent with the SYN packet
        // so that everything is okay even if the CLS_REQ command is dropped;
        // however, an SB timeout error will result if matchmaking doesn't end soon,
        // since it is impossible to clear server browsing.
            
        // Enter standby status
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);

        // as standby status will continue until matchmaking is complete,
        // initialize the timeout counter
        DWCi_InitClWaitTimeout();

        // A profile ID to pass to the callback is not required
        DWCi_GetMatchCnt()->cbEventPid = 0;

        // clear server browsing
        sbClear = TRUE;
        break;

    case DWC_PP_CONNECTION_SYN_FINISH:
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Completed matching!\n");

        if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV){
            // if not the server-client matchmaking server,
            // GP status is set to game-play status.
            (void)DWCi_SetGPStatus(DWC_STATUS_PLAYING, "", NULL);
        }

        // Calls the dwc_main.c callback
        DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE,
                                     FALSE,
                                     DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
                                     FALSE,
                                     DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                                     DWCi_GetMatchCnt()->matchedParam);

        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
            (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
            // The GameSpy control structure is freed here since it's not possible
            // to join midway when using friend specified/unspecified peer matchmaking..
            DWCi_CloseMatching();
        }
        else {
#ifndef DWC_BUG_WIFI_DELAY_SEND
            // with server-client matchmaking, deallocate only the SB object
            if (DWCi_GetMatchCnt()->sbObj){
                ServerBrowserFree(DWCi_GetMatchCnt()->sbObj);
                DWCi_GetMatchCnt()->sbObj = NULL;  // Must be cleared to null on its own!
            }
#endif

            // Frees even the memory being used by NN
            NNFreeNegotiateList();

            if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV){
                // for the server-client matchmaking server
                // Sets the increase in number of connected players into GP status
                gpResult = DWCi_GPSetServerStatus();
                if (DWCi_HandleGPError(gpResult)) return;

                // Once one case of matchmaking is complete, new connections are blocked
                // here if the option for blocking new connections is active
                if (stOptSCBlock.valid == 1){
                    stOptSCBlock.lock = 1;
                    DWC_Printf(DWC_REPORTFLAG_DEBUG,
                               "[OPT_SC_BLOCK] Connect block start!\n");
                }

                // Restore status to server wait status
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_WAITING);
            }
            else {
                // Restore status to client wait status
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAITING);
            }

            // Initialize the profile ID to be passed to the callback
            DWCi_GetMatchCnt()->cbEventPid = 0;
        }

        // Initialize cancel status when cancellations have been ignored
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_INIT;
        break;
    }


#ifndef DWC_BUG_WIFI_DELAY_SEND
#if 1
    if (sbClear &&
        (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL)){
        // When using friend specified/unspecified peer matchmaking, clear the server 
        // list as a communication error will result unless there is a disconnect
        // when the master server cannot be accessed for awhile.
        ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
    }
#else
    if (sbClear &&
        (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL) &&
        (ServerBrowserState(DWCi_GetMatchCnt()->sbObj) != sb_disconnected)){
        // When using friend specified/unspecified peer matchmaking, clear the server 
        // list as a communication error will result unless there is a disconnect
        // when the master server cannot be accessed for awhile.
        ServerBrowserClear(DWCi_GetMatchCnt()->sbObj);
    }
#endif
#endif
}


/*---------------------------------------------------------------------------*
  Function for confirming multiple friends
  arguments: pidList: Pointer to the array of profile IDs whose friend status is to be checked
          pidListLen: Number of requests in the above array
  return values: TRUE: All profile IDs are friends, FALSE: Profile IDs that are not friends are included
  application: Checks the list of profile IDs and returns whether or not all entries are friends
 *---------------------------------------------------------------------------*/
static BOOL DWCi_AreAllBuddies(const u32 pidList[], u32 pidListLen)
{
    int i;

    // When allowing friends of friends, always allow clients that 
    // have found the new server to connect to that server
    if (DWCi_GetMatchCnt()->distantFriend &&
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV))
        return TRUE;

    for (i = 0; i < pidListLen; i++){
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
        if (!DWCi_IsFriendByIdxList((int)pidList[i])) return FALSE;
#else
        if (!gpIsBuddy(DWCi_GetMatchCnt()->pGpObj, (int)pidList[i])) return FALSE;
#endif

        if (DWCi_GetMatchCnt()->distantFriend &&
            (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING)){
            // When friends of friends are allowed, if the new server candidate (pidList[0])
            // is the only friend, clients closed by the current server
            // are allowed to connect to that server.
            return TRUE;
        }
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  matchmaking cancellation execution function
  arguments: None
  return values: None
  application: executes matchmaking cancellation
 *---------------------------------------------------------------------------*/
static void DWCi_DoCancelMatching(void)
{
    u32 senddata;
    int result;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "CANCEL! state %d, numHost nn=%d gt2=%d.\n",
               DWCi_GetMatchCnt()->state, DWCi_GetMatchCnt()->qr2NNFinishCount,
               DWCi_GetMatchCnt()->gt2NumConnection);

    // 0 is used for the profile ID of the cancelled host when cancelled by the local host.
    DWCi_GetMatchCnt()->cbEventPid = 0;
    
    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_OWN){
        // Quick cancel processing can be executed because the host is not connected
        DWCi_FinishCancelMatching();
        return;
    }

    // Set the cancel execution status
    DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;

    if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL){
        // server-client matchmaking's newly connected clients are cancelled
        senddata = 0;

        // Set the cancel execution status
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;

        if (DWCi_GetMatchCnt()->gt2NumConnection){
            // Disconnect all connections
            gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
        }
        else {
            // If there still is not connection, only the reservation is cancelled.
            // Even if this command is dropped, no resend is necessary since it will
            // be noticed on the server side due to the timeout.
            result = DWCi_CancelReservation(DWCi_GetMatchCnt()->sbPidList[0]);
            if (DWCi_HandleMatchCommandError(result)) return;
        }

        // End cancel processing
        DWCi_FinishCancelMatching();
        return;
    }

    // Although a gt2 connection has still not been established, perform processing for hosts that are being connected.
    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_NN_HOST) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_NN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_GT2) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_GT2)){
        if (DWCi_GetMatchCnt()->reqProfileID){
            // If status indicates a reservation command has been sent to someone, 
            // cancel the reservation for the other party to whom the reservation command was sent.
            result = DWCi_CancelReservation(DWCi_GetMatchCnt()->sbPidList[0]);
            if (DWCi_HandleMatchCommandError(result)) return;
        }

        // If the local host has been reserved by someone, send the 
        // matchmaking cancel command to the client.
        result = DWCi_InvalidateReservation();
        if (DWCi_HandleMatchCommandError(result)) return;
    }

    if (DWCi_GetMatchCnt()->nnInfo.cookie){
        // Cancel during NAT negotiation
        NNCancel(DWCi_GetMatchCnt()->nnInfo.cookie);
        DWCi_GetMatchCnt()->nnInfo.cookie = 0;
    }

    if (DWCi_GetMatchCnt()->gt2NumConnection){
        // Transition to "execute cancel processing" status
        DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_EXEC;

        // Disconnect all connections
        gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);
    }

    // End cancel processing
    DWCi_FinishCancelMatching();
}


/*---------------------------------------------------------------------------*
  matchmaking cancellation ending function
  arguments: None
  return values: None
  application: Performs end processing when cancellation is
          complete for the host that cancelled matchmaking.
 *---------------------------------------------------------------------------*/
static void DWCi_FinishCancelMatching(void)
{
    GPResult gpResult;

    // GP status is set to online.
    gpResult = DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
    if (DWCi_HandleGPError(gpResult)) return;

    // Close matchmaking processes
    DWCi_CloseMatching();

    // Call the matchmaking complete callback
    DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE,
                                 TRUE,
                                 DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
                                 DWCi_GetMatchCnt()->cbEventPid ? TRUE : ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ? TRUE : FALSE),
                                 DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                                 DWCi_GetMatchCnt()->matchedParam);

    // Initialize cancel status
    DWCi_GetMatchCnt()->cancelState = DWC_MATCH_CANCEL_STATE_INIT;
}


/*---------------------------------------------------------------------------*
  Function for disabling client reservations
  arguments: None
  return values: SBError type or the value resulting from conversion from GPResult type to int type
  application: When the local host has been reserved by a client, that reservation
          is cancelled and the client originating the reservation is notified of this fact.
 *---------------------------------------------------------------------------*/
static int  DWCi_InvalidateReservation(void)
{
    u32 senddata = 0;
    int result   = 0;

    if (DWCi_GetMatchCnt()->qr2IsReserved && DWCi_GetMatchCnt()->qr2Reservation &&
        (DWCi_GetMatchCnt()->qr2Reservation != DWCi_GetMatchCnt()->profileID)){
        result =
            DWCi_SendMatchCommand(DWC_MATCH_COMMAND_CANCEL,
                                  DWCi_GetMatchCnt()->qr2Reservation,
                                  DWCi_GetMatchCnt()->qr2IPList[DWCi_GetMatchCnt()->gt2NumConnection+1],
                                  DWCi_GetMatchCnt()->qr2PortList[DWCi_GetMatchCnt()->gt2NumConnection+1],
                                  &senddata, 1);
        
        DWCi_GetMatchCnt()->qr2IsReserved  = 0;
        DWCi_GetMatchCnt()->qr2Reservation = 0;

        // Isn't send_statechanged necessary?
    }

    return result;
}


/*---------------------------------------------------------------------------*
  restarting after matchmaking cancellation function
  arguments: level: Matchmaking restart level. Defined by the DWCMatchResetLevel enumerator.
  return values: None
  application: Restarts matchmaking (ends matchmaking depending on the situation)
          after cancellation processing has ended for the host that cancelled matchmaking
 *---------------------------------------------------------------------------*/
static void DWCi_RestartFromCancel(DWCMatchResetLevel level)
{
    SBError sbError;

    if (level == DWC_MATCH_RESET_ALL){        
        // ends matchmaking
        DWCi_FinishCancelMatching();
    }
    else {
        // initializes matchmaking control parameters
        DWCi_ResetMatchParam(level);

        if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
            (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
            // calls matchmaking callback upon cancellation.
            // The callback is set regardless of the value set for level and
            // then software enters wait status with reset parameters.
            DWCi_GetMatchCnt()->matchedCallback(DWC_ERROR_NONE,
                                         TRUE,
                                         DWCi_GetMatchCnt()->cbEventPid ? FALSE : TRUE,
                                         FALSE,
                                         DWCi_GetFriendListIndex(DWCi_GetMatchCnt()->cbEventPid),
                                         DWCi_GetMatchCnt()->matchedParam);
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
            if (level == DWC_MATCH_RESET_RESTART){
                // Update the SB server
                sbError = DWCi_SBUpdateAsync(0);
                if (DWCi_HandleSBError(sbError)) return;
            }
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
            if (level == DWC_MATCH_RESET_RESTART){
                // Make NAT negotiation reservation for first server using a GP message.
                // An infinite loop may result if TRUE is set for the argument init
                // [todo]
                // Upon investigating why an infinite loop sometimes results, it is only possible when init=TRUE
                (void)DWCi_SendResvCommandToFriend(FALSE/*TRUE*/, FALSE, 0);
            }
        }
        else {
            // [test]
            // Infinite loop probably impossible
            DWC_Printf(DWC_REPORTFLAG_ERROR,
                       "ERROR - DWCi_RestartFromCancel : matchType %d, level %d\n",
                       DWCi_GetMatchCnt()->qr2MatchType, level);
        }
    }
}


/*---------------------------------------------------------------------------*
  Function for restarting matchmaking after timeout is detected
  arguments: None
  return values: None
  application: The host that finds the timeout host closes all connections and restarts matchmaking from the beginning.
          Redo matchmaking from the beginning
          This function must not be called when using server-client matchmaking!
 *---------------------------------------------------------------------------*/
static void DWCi_RestartFromTimeout(void)
{

    // [test]
    // This function must not be called when using server-client matchmaking!
    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL)){
        DWC_Printf(DWC_REPORTFLAG_WARNING,
                   "DWCi_RestartFromTimeout() shouldn't be called.\n");
        return;
    }

    // Disconnect all connections
    DWCi_CloseAllConnectionsByTimeout();

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "Closed all connections and restart matching.\n");

    // Restart matchmaking from the beginning
    DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
}


/*---------------------------------------------------------------------------*
  Function for restarting matchmaking after connections have been closed
  arguments: None
  return values: SBError type or the value resulting from conversion from GPResult type to int type
  application: Restarts matchmaking for clients that have connected
          to the server and are now in standby mode when other hosts
          have closed connections using matchmaking cancellation or other means.
          This function must not be called when using server-client matchmaking!
 *---------------------------------------------------------------------------*/
static int DWCi_ResumeMatching(void)
{
    SBError sbError;
    int result;

    // Cancel reservation
    DWCi_GetMatchCnt()->qr2IsReserved  = 0;
    DWCi_GetMatchCnt()->qr2Reservation = 0;

    // Initializes close status
    DWCi_GetMatchCnt()->closeState     = DWC_MATCH_CLOSE_STATE_INIT;

    if (DWCi_GetMatchCnt()->priorProfileID){
        // When the next connected server for which mediation was performed by the server sending the close command is active.
        // priorProfileID is not set with server-client matchmaking
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
            // Returns to searching for a new server introduced by the original server
            // when using friend unspecified matchmaking.
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);

            sbError = DWCi_SBUpdateAsync(0);
            if (DWCi_HandleSBError(sbError)) return sbError;
        }
        else if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND){
            // Make NN reservation immediately when using matchmaking with a friend specification.
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
            
            result = DWCi_SendResvCommand(DWCi_GetMatchCnt()->priorProfileID, FALSE);
            if (DWCi_HandleMatchCommandError(result)) return result;
        }
    }
    else {
        // This code is for when the next connected server for which mediation was performed by the server
        // sending the close command, or a host connected to that server, is not a friend of the local host.
        DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
    }

    return 0;
}


/*---------------------------------------------------------------------------*
  Function for closing cancelled hosts
  arguments: profileID: The profile ID of the host to be closed
  Return values: TRUE: Close processing initiated, FALSE: Close processing 
          not performed because there is no connection with the specified host
  application: Closes the connection with the host originating the cancellation
          when cancelling matchmaking.
 *---------------------------------------------------------------------------*/
static BOOL DWCi_CloseCancelHostAsync(int profileID)
{
    BOOL ret;
    GT2Connection* connection;

    // Record the profile ID of the cancelled client
    DWCi_GetMatchCnt()->cbEventPid = profileID;

    // Get the gt2 connection with the host having the specified profile ID
    connection =
        DWCi_GetGT2ConnectionByProfileID(profileID, DWCi_GetMatchCnt()->gt2NumConnection+1);

    if (connection){
        // Close status is for connections closed due to timeout
        DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;
        
        // Close connections if there are any.
        gt2CloseConnectionHard(*connection);

        // Restore close status to original setting
        DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_INIT;
        ret = TRUE;
    }
    else {
        // If there are no connections, just delete data from the list
        (void)DWCi_DeleteHostByProfileID(profileID, DWCi_GetMatchCnt()->gt2NumConnection+1);
        
        ret = FALSE;
    }

    return ret;
}


/*---------------------------------------------------------------------------*
  Function for closing connections when a timeout is discovered
  arguments: None
  return values: None
  application: Closes all host connections when a timeout host
          is discovered.
          to close a connection without performing anything within the connection close callback,
          use this close function.
 *---------------------------------------------------------------------------*/
static void DWCi_CloseAllConnectionsByTimeout(void) 
{

    // Close status is for connections closed due to timeout
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;
    
    // Disconnect all connections
    gt2CloseAllConnectionsHard(*DWCi_GetMatchCnt()->pGt2Socket);

    // Initialize close status
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_INIT;
}


/*---------------------------------------------------------------------------*
  server-client matchmaking timeout client closing & notification function
  arguments: bitmap: The AID bitmap of host from which there was a response (not including the server)
  return values: TRUE: Success, FALSE: Error generated
  application: for the server-client matchmaking server,
          when the timeout client is found, that client is closed and
          connected clients are notified
 *---------------------------------------------------------------------------*/
static BOOL DWCi_CloseShutdownClientSC(u32 bitmap)
{
    u8  aid;
    int senddata[DWC_MAX_CONNECTIONS];
    int senddataLen = 0;
    int validPid[DWC_MAX_CONNECTIONS];
    int validPidLen = 0;
    int result;
    int i;

    // [note]
    // Under this method of doing things, the server calls the close callback and then:
    // Even though sync adjustment completes and this callback is returned,
    // since the GT2 packet arrives earlier in the case of matchmaking complete synchronization adjustment,
    // the close callback is called after the synchronization adjustment
    // complete callback is returned.
    
    // First create a profile ID list made up of unresponsive hosts
    for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
        if (bitmap & (1 << DWCi_GetMatchCnt()->aidList[i])){
            validPid[validPidLen] = DWCi_GetMatchCnt()->sbPidList[i];
            validPidLen++;
        }
        else {
            senddata[senddataLen] = DWCi_GetMatchCnt()->sbPidList[i];
            senddataLen++;
        }
    }

    // Send the command for closing unresponsive hosts to all responsive hosts
    for (i = 0; i < validPidLen; i++){
        result = DWCi_SendMatchCommand(DWC_MATCH_COMMAND_SC_CLOSE_CL,
                                       validPid[i], 0, 0,
                                       (u32 *)senddata, senddataLen);
        if (DWCi_HandleMatchCommandError(result)) return FALSE;
    }

    // Set closed status so that nothing is done even if clients
    // in the middle of matchmaking are closed.
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_TIMEOUT;

    // Close unresponsive hosts
    for (i = 0; i < senddataLen; i++){
        aid = DWCi_GetAIDFromProfileID(senddata[i], FALSE);
        if (aid != 0xff){
            // Close the connection
            DWC_CloseConnectionHard(aid);
        }
    }

    // Clear close status
    DWCi_GetMatchCnt()->closeState = DWC_MATCH_CLOSE_STATE_INIT;

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Function for sending synchronization adjustment packets when matchmaking is complete
  Arguments: aid: AID of sending host
          type: DWC transport packet type. Defined by DWC_SEND_TYPE_*.
  return values: None
  application: Sends synchronization adjustment packets when matchmaking is complete
 *---------------------------------------------------------------------------*/
static void DWCi_SendMatchSynPacket(u8 aid, u16 type)
{
    u8  i;
    u8  senddata[4];

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Sent SYN %d packet to aid %d.\n",
               type-DWC_SEND_TYPE_MATCH_SYN, aid);

    switch (type){
    case DWC_SEND_TYPE_MATCH_SYN:
        if (aid == DWCi_GetMatchCnt()->aidList[DWCi_GetMatchCnt()->gt2NumConnection]){
            // Flag set when sending to newly connected clients
            senddata[0] = 1;
        }
        else {
            senddata[0] = 0;
        }

        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            if (aid == DWCi_GetMatchCnt()->aidList[i]){
                // Always make notification regarding both the AID index and AID for when the
                // CLS_REQ command is dropped.
                // Under current specifications, AID index = AID.
                senddata[1] = i;
                senddata[2] = aid;
                break;
            }
        }
        break;

    case DWC_SEND_TYPE_MATCH_SYN_ACK:
        // Latency with the server is added and sent
        senddata[0] = (u8)(DWCi_GetMatchCnt()->baseLatency & 0xff);
        senddata[1] = (u8)((DWCi_GetMatchCnt()->baseLatency >> 8) & 0xff);
        break;
    }
    // No supplemental data when DWC_SEND_TYPE_MATCH_ACK is set

    DWCi_SendReliable(type, aid, senddata, DWC_MATCH_SYN_DATA_BODY_SIZE);

    // Record the time sent
    DWCi_GetMatchCnt()->lastSynSent = OS_GetTick();
}


/*---------------------------------------------------------------------------*
  Function for processing synchronization adjustment timeouts when matchmaking is complete
  arguments: None
  return values: TRUE: Normal operation, FALSE: Error generated
  application: Checks the synchronization adjustment timeout when
          matchmaking is complete and resends if the timeout is reached.
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessMatchSynTimeout(void)
{
    u64 passTime;
    int i;

    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SYN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_SYN_WAIT))
        passTime = OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->lastSynSent);
    else return TRUE;

    switch (DWCi_GetMatchCnt()->state){
    case DWC_MATCH_STATE_CL_SYN:
        if (passTime > DWC_MATCH_SYN_ACK_WAIT_TIME){
            // when a certain period of time has passed during client ACK standby status
            // sends a SYN-ACK packet to the server
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "[SYN] No ACK from server %d/%d.\n",
                       DWCi_GetMatchCnt()->clWaitTimeoutCount, DWC_MATCH_CMD_RETRY_MAX);
            if ((DWC_GetState() == DWC_STATE_MATCHING) &&
                (DWCi_GetMatchCnt()->clWaitTimeoutCount >= DWC_MATCH_CMD_RETRY_MAX)){
                // if there is no response from the server after five tries, redo matchmaking
                // all client processing is terminated and matchmaking is restarted.
                // for server-client matchmaking newly-connected clients
                // end in an error.
                // for server-client matchmaking connected clients
                // after connection, timeout processing is left to the game to perform
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "Timeout: [SYN] Connection to server was shut down.\n");
                    
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[0]))
                    return FALSE;
            }
            else {
                DWCi_GetMatchCnt()->clWaitTimeoutCount++;

                // sends a SYN-ACK packet to the server
                DWCi_SendMatchSynPacket(DWCi_GetMatchCnt()->aidList[0],
                                        DWC_SEND_TYPE_MATCH_SYN_ACK);
            }
        }
        break;

    case DWC_MATCH_STATE_SV_SYN:
        if (passTime > DWC_MATCH_SYN_ACK_WAIT_TIME){
            // Timed out waiting to receive SYN-ACK
            DWCi_GetMatchCnt()->synResendCount++;
            if (DWCi_GetMatchCnt()->synResendCount > DWC_MATCH_CMD_RETRY_MAX){
                // If there is no reply even after attempts to send a set number of retries, a determination is
                // made that communication with the client has been disconnected
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Timeout: wait SYN-ACK (aidbitmap 0x%x). Restart matching.\n",
                           DWCi_GetMatchCnt()->synAckBit);
                    
                if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY) ||
                    (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND)){
                    // Disconnect all connections when using friend specified/
                    // unspecified peer matchmaking and retry matchmaking from the beginning 
                    DWCi_CloseAllConnectionsByTimeout();
                    DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
                }
                else {
                    // for server-client matchmaking, disconnect only
                    // non-responding clients, communicating that to the other
                    // clients and then disconnecting them.
                    if (!DWCi_CloseShutdownClientSC(DWCi_GetMatchCnt()->synAckBit))
                        return FALSE;

                    if (DWCi_GetMatchCnt()->gt2NumConnection != 0){
                        // If connected clients remain, initialize the retry
                        // count and SYN packet send time, and continue with
                        // synchronization adjustment processing a second time
                        DWCi_GetMatchCnt()->synResendCount = 0;
                        DWCi_GetMatchCnt()->lastSynSent    = OS_GetTick();
                    }
                    else {
                        // If there are no already connected client members,
                        // end all server processing and return to matchmaking start status
                        if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->cbEventPid))
                            return FALSE;
                    }
                }
            }
            else {
                // Send a SYN packet to clients that haven't returned a SYN-ACK
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    if (!(DWCi_GetMatchCnt()->synAckBit & (1 << DWCi_GetMatchCnt()->aidList[i]))){
                        DWCi_SendMatchSynPacket(DWCi_GetMatchCnt()->aidList[i],
                                                DWC_SEND_TYPE_MATCH_SYN);
                    }
                }
            }
        }
        break;

    case DWC_MATCH_STATE_SV_SYN_WAIT:
        if (passTime > DWCi_GetMatchCnt()->baseLatency){
            // Call matchmaking complete processing if the maximum latency timeout period since an ACK was sent has elapsed
            DWCi_PostProcessConnection(DWC_PP_CONNECTION_SYN_FINISH);
        }
        break;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  server-client matchmaking cancellation synchronization adjustment command sending function
  arguments: profile ID: The host profile ID
          command: Sync adjustment command to be sent
  return values: TRUE: Normal end, FALSE: Error generated
  application: sends the server-client matchmaking cancellation synchronization adjustment command.
          Error processing is handled inside the function
 *---------------------------------------------------------------------------*/
static BOOL DWCi_SendCancelMatchSynCommand(int profileID, u8 command)
{
    u32 senddata;
    int len;
    int result;

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Sent CANCEL SYN %d command to %u.\n",
               command-DWC_MATCH_COMMAND_CANCEL_SYN, profileID);

    if (command == DWC_MATCH_COMMAND_CANCEL_SYN){
        // Send the profile ID of the client to be cancelled when sending cancel SYN
        senddata = (u32)DWCi_GetMatchCnt()->cbEventPid;
        len      = 1;
    }
    else {
        len      = 0;
    }

    result = DWCi_SendMatchCommand(command, profileID, 0, 0, &senddata, len);
    if (DWCi_HandleMatchCommandError(result)) return FALSE;

    // Record the time sent
    DWCi_GetMatchCnt()->lastCancelSynSent = OS_GetTick();

    return TRUE;
}


/*---------------------------------------------------------------------------*
  server-client matchmaking cancellation synchronization adjustment command receipt function
  arguments: profile ID: The profile ID of the sending host
          command: Received sync adjustment command
          data: u32 data included with the command
  return values: TRUE: Normal end, FALSE: Error generated
  application: performs processes upon receipt of server-client matchmaking cancellation
          synchronization adjustment command  
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessCancelMatchSynCommand(int profileID, u8 command, u32 data)
{
    u8  aid;
    u32 bitmask;
    u64 baseTime, latency;
    int i;

    DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Received CANCEL SYN %d command from %u.\n",
               command-DWC_MATCH_COMMAND_CANCEL_SYN, profileID);

    if (DWC_GetState() != DWC_STATE_CONNECTED){
        // Ignore if already received after close
        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2, "Ignore delayed CANCEL SYN.\n");
        return TRUE;
    }

    switch (command){
    case DWC_MATCH_COMMAND_CANCEL_SYN:
        if (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_CANCEL_SYN){
            // Proceed to matchmaking cancel synchronization status when received the first time
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_CANCEL_SYN);

            // Close newly connected clients
            // The profile ID of the newly connected client is stored in data.
            DWCi_CloseCancelHostAsync((int)data);
        }

        // Sends a SYN-ACK packet to the server
        if (!DWCi_SendCancelMatchSynCommand(profileID, DWC_MATCH_COMMAND_CANCEL_SYN_ACK))
            return FALSE;
        break;

    case DWC_MATCH_COMMAND_CANCEL_SYN_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN){
            // What to do during SYN-ACK wait status
            // Find the approximate one-way latency from the time SYN is
            // sent until SYN-ACK is returned and record it if it's a maximum.
            // Although the time is rather short if a SYN-ACK is returned
            // immediately after a resend, we don't care about this shortened amount.
            baseTime = OS_GetTick();
            if ((OS_TicksToMilliSeconds(baseTime-DWCi_GetMatchCnt()->lastCancelSynSent) >> 1) > DWC_GP_PROCESS_INTERVAL){
                // Subtract the maximum amount of time assumed from when
                // the client receives a command until it sends a command
                latency = (OS_TicksToMilliSeconds(baseTime-DWCi_GetMatchCnt()->lastCancelSynSent) >> 1)-DWC_GP_PROCESS_INTERVAL;
                if (latency > DWCi_GetMatchCnt()->cancelBaseLatency){
                    DWCi_GetMatchCnt()->cancelBaseLatency = (u16)latency;
                }
            }
                
            // Set SYN-ACK receive bits
            // Record using the AID bitmap
            aid = DWCi_GetAIDFromProfileID(profileID, FALSE);
            if (aid != 0xff){
                DWCi_GetMatchCnt()->cancelSynAckBit |= 1 << aid;
            }

            bitmask = DWCi_GetAIDBitmask(TRUE);  // Get the AID bit mask for determination
            if (DWCi_GetMatchCnt()->cancelSynAckBit == bitmask){
                // Send an ACK to all clients once SYN-ACK has been returned from all clients
                // Send ACK
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    if (!DWCi_SendCancelMatchSynCommand(DWCi_GetMatchCnt()->sbPidList[i],
                                                        DWC_MATCH_COMMAND_CANCEL_ACK))
                        return FALSE;
                }

                // Proceed to matchmaking cancellation synchronization complete standby status
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT);

                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "Wait max latency %d msec.\n",
                           DWCi_GetMatchCnt()->cancelBaseLatency);
            }
        }
        else {
            // Reply with ACK quickly to handle SYN-ACKs which were individually delayed after ACK was sent
            // delayed after ACK was sent
            if (!DWCi_SendCancelMatchSynCommand(profileID, DWC_MATCH_COMMAND_CANCEL_ACK))
                return FALSE;
        }
        break;

    case DWC_MATCH_COMMAND_CANCEL_ACK:
        if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_CANCEL_SYN){            
            // client matchmaking restarts
            DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
        }
        break;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  server-client matchmaking cancellation synchronization adjustment timeout process function
  arguments: None
  return values: TRUE: Normal end, FALSE: Error generated
  application: checks the timeout for server-client matchmaking cancellation
          synchronization adjustment and resends if the timeout is reached.
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessCancelMatchSynTimeout(void)
{
    u64 passTime;
    int i;

    if ((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_CANCEL_SYN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN) ||
        (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT))
        passTime = OS_TicksToMilliSeconds(OS_GetTick()-DWCi_GetMatchCnt()->lastCancelSynSent);
    else return TRUE;

    switch (DWCi_GetMatchCnt()->state){
    case DWC_MATCH_STATE_CL_CANCEL_SYN:
        if (passTime > DWC_MATCH_CANCEL_SYN_ACK_WAIT_TIME){
            // Resends a SYN-ACK packet to the server
            if (!DWCi_SendCancelMatchSynCommand(DWCi_GetMatchCnt()->sbPidList[0],
                                                DWC_MATCH_COMMAND_CANCEL_SYN_ACK))
                return FALSE;
        }
        break;

    case DWC_MATCH_STATE_SV_CANCEL_SYN:
        if (passTime > DWC_MATCH_CANCEL_SYN_ACK_WAIT_TIME){
            // Timed out waiting to receive SYN-ACK
            DWCi_GetMatchCnt()->cancelSynResendCount++;
            if (DWCi_GetMatchCnt()->cancelSynResendCount > DWC_MATCH_CMD_RETRY_MAX){
                // If there is no reply even after attempts to send a set number of retries, a determination is
                // made that communication with the client has been disconnected
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                           "Timeout: wait cancel SYN-ACK (aidbitmap 0x%x).\n",
                           DWCi_GetMatchCnt()->cancelSynAckBit);

                // for server-client matchmaking, disconnect only
                // non-responding clients, communicating that to the other
                // clients and then disconnecting them.
                if (!DWCi_CloseShutdownClientSC(DWCi_GetMatchCnt()->cancelSynAckBit))
                    return FALSE;

                if (DWCi_GetMatchCnt()->gt2NumConnection != 0){
                    // If connected clients remain, initialize the retry count and SYN packet send time, and continue with sync adjustment processing a second time
                    // count and SYN packet send time, and continue with
                    // synchronization adjustment processing a second time
                    DWCi_GetMatchCnt()->cancelSynResendCount = 0;
                    DWCi_GetMatchCnt()->lastCancelSynSent    = OS_GetTick();
                }
                else {
                    // If there are no already connected client members, end all server processing and return to matchmaking start status
                    // connections to close as long as there are no connected hosts.
                    DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
                }
            }
            else {
                // Send a SYN packet to clients that haven't returned a SYN-ACK
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    if (!(DWCi_GetMatchCnt()->cancelSynAckBit & (1 << DWCi_GetMatchCnt()->aidList[i]))){
                        if (!DWCi_SendCancelMatchSynCommand(DWCi_GetMatchCnt()->sbPidList[i],
                                                            DWC_MATCH_COMMAND_CANCEL_SYN))
                            return FALSE;
                    }
                }
            }
        }
        break;

    case DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT:
        if (passTime > DWCi_GetMatchCnt()->cancelBaseLatency){
            // Restart matchmaking if the maximum latency time since sending cancel ACK has elapsed
            DWCi_RestartFromCancel(DWC_MATCH_RESET_CONTINUE);
        }
        break;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Function for getting an available QR2 key ID
  arguments: None
  return values: The available QR2 keys defined by the game (value after DWC_QR2_GAME_KEY_START)
  application: Searches the QR2 key data array and gets the available QR2 key IDs
 *---------------------------------------------------------------------------*/
static u8  DWCi_GetNewMatchKey(void)
{
    u8 i;

    // Search for available key numbers
    for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
        if (!stGameMatchKeys[i].keyID) return (u8)(DWC_QR2_GAME_KEY_START+i);
    }

    // Number of registered keys already full
    return 0;
}


/*---------------------------------------------------------------------------*
  Function for clearing QR2 keys for the game
  arguments: None
  return values: None
  application: Clears the QR2 key data array
 *---------------------------------------------------------------------------*/
static void DWCi_ClearGameMatchKeys(void)
{
    int i;

    // First search for all keys and then free those for which memory has already been allocated
    for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
        if (stGameMatchKeys[i].keyStr)
            DWC_Free(DWC_ALLOCTYPE_BASE, stGameMatchKeys[i].keyStr, 0);
    }

    // Clear to all zero
    MI_CpuClear32(stGameMatchKeys, sizeof(DWCGameMatchKeyData)*DWC_QR2_GAME_RESERVED_KEYS);
}

/*---------------------------------------------------------------------------*
  Function for getting available AIDs
  arguments: None
  return values: Highest available AID number
  application: Searches the AID list and gets the maximum AID value available.
 *---------------------------------------------------------------------------*/
static u8   DWCi_GetAIDFromList(void)
{
    u8  i;
    int j;

    for (i = 0; i < DWC_MAX_CONNECTIONS; i++){
        for (j = 0; j <= DWCi_GetMatchCnt()->qr2NNFinishCount; j++){
            if (DWCi_GetMatchCnt()->aidList[j] == i) break;
        }
        if (j > DWCi_GetMatchCnt()->qr2NNFinishCount) break;
    }

    return i;
}


/*---------------------------------------------------------------------------*
  Function for determining friends (version that uses the friend index list)
  arguments: profileID: The profile ID
  return values: TRUE: Friend is on the index list, FALSE: Not a friend
  application: Searches the friend index list and friend roster and checks
          if the specified profile ID is registered on the friend index list
          and whether or not it represents a friend.
 *---------------------------------------------------------------------------*/
static BOOL DWCi_IsFriendByIdxList(int profileID)
{
    int listProfileID;
    int i;

    if (DWCi_GetMatchCnt()->friendList == NULL) return FALSE;

    for (i = 0; i < DWCi_GetMatchCnt()->friendIdxListLen; i++){
        if (((listProfileID = DWCi_GetProfileIDFromList(DWCi_GetMatchCnt()->friendIdxList[i])) > 0) &&
            (listProfileID == profileID)){
            // Consider as a friend if the profile ID exists on the friend list.
            // From the way this function is used, there'd be no need to make a determination with gpIsBuddy().
            return TRUE;
        }
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  Function for checking the servers using the DWC library
  arguments: server: SBServer type
  return values: Returns the value of the dwc_pid key (profile ID) of the server.
          Returns 0 if the server uses the DWC library
  application: Searches for SBServer objects, determines whether or not there is a
          key set for the DWC library, and returns the dwc_pid key value (profile ID)
          if there is, and returns 0 if there is not.
 *---------------------------------------------------------------------------*/
static int  DWCi_CheckDWCServer(SBServer server)
{

    if (SBServerGetIntValue(server, "numplayers", -1) == -1) return 0;
    if (SBServerGetIntValue(server, "maxplayers", -1) == -1) return 0;
    if (SBServerGetIntValue(server, DWC_QR2_MATCH_TYPE_KEY_STR, -1) == -1) return 0;
    if (SBServerGetIntValue(server, DWC_QR2_MATCH_RESV_KEY_STR, -1) == -1){
        if (SBServerGetIntValue(server, DWC_QR2_MATCH_RESV_KEY_STR, 0) == 0) return 0;
    }
    if (SBServerGetIntValue(server, DWC_QR2_MATCH_VER_KEY_STR, -1) == -1) return 0;
    return SBServerGetIntValue(server, DWC_QR2_PID_KEY_STR, 0);
}


/*---------------------------------------------------------------------------*
  Function for getting the sbPidList index
  arguments: profileID: The profile ID of the host whose index is being retrieved
          idx0: TRUE: Include 0 in the indices to be searched, FALSE: Treat 0 as undefined
  return values: Index of sbPidList or other list. Returns -1 if the profile ID is not on the list
          returns a -1
  application: Gets the sbPidList index of the host having the specified profile ID
 *---------------------------------------------------------------------------*/
static int  DWCi_GetPidListIndex(int profileID, BOOL idx0)
{
    int i;

    for (i = (idx0 ? 0 : 1); i <= DWCi_GetMatchCnt()->qr2NNFinishCount; i++){
        if (DWCi_GetMatchCnt()->sbPidList[i] == profileID) return i;
    }

    return -1;
}


/*---------------------------------------------------------------------------*
  Function for getting an AID from a profile ID
  arguments: profileID: The profile ID of the host whose AID is being retrieved
          idx0: TRUE: Include 0 in the indices to be searched, FALSE: Treat 0 as undefined
  return values: AID. Returns 0xff if the corresponding AID does not exist.
  application: Gets the AID of the host having the specified profile ID
 *---------------------------------------------------------------------------*/
static u8   DWCi_GetAIDFromProfileID(int profileID, BOOL idx0)
{
    int i;

    for (i = (idx0 ? 0 : 1); i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
        if (DWCi_GetMatchCnt()->sbPidList[i] == profileID)
            return DWCi_GetMatchCnt()->aidList[i];
    }

    return 0xff;
}


/*---------------------------------------------------------------------------*
  Function for getting an AID bitmap for determinations during sync adjustment
  arguments: valid: TRUE: Bitmap of already connected hosts only
                     (used for server-client matchmaking cancellation synchronization)
                FALSE: Bitmap of connected hosts included hosts in the middle of matchmaking
                      (used for matchmaking completion synchronization)
  return values: AID bitmap.
  Use: Gets a bitmap for checking which hosts have received SYN-ACK during various types of sync adjustment
 *---------------------------------------------------------------------------*/
static u32  DWCi_GetAIDBitmask(BOOL valid)
{
    u32 bitmask = 0;
    int i;

    if (valid){
        // Bitmask where the server's aid (=0) has been deleted from the AIDs for already connected hosts
        return (DWCi_GetMatchCnt()->validAidBitmap & ~1);
    }
    else {
        // Bitmask where the server's aid (=0) has been deleted from the
        // connected AIDs that included hosts in the middle of matchmaking
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            bitmask |= 1 << DWCi_GetMatchCnt()->aidList[i];
        }
        return bitmask;
    }
}


/*---------------------------------------------------------------------------*
  connected client timeout counter initialization function
  arguments: timeout: time of no communication with the server determined to be a timeout
  return values: None
  application: initializes the timeout occurrence counter from the time the last data was received
          for connected clients to gauge how much time has passed without receiving data from the server
 *---------------------------------------------------------------------------*/
static void DWCi_InitClWaitTimeout(void)
{

    DWCi_GetMatchCnt()->clWaitTimeoutCount = 0;
    DWCi_GetMatchCnt()->clWaitTime         = OS_GetTick();
}


/*---------------------------------------------------------------------------*
  Matchmaking Option: Function for initializing the matchmaking complete counter for a specified number of hosts or less
  arguments: reset: TRUE: Reset when server, FALSE: Completely initialize
  return values: None
  application: function to initialize timeout gauge counter as a matchmaking
          option when matchmaking is complete for a specified number of hosts or less
 *---------------------------------------------------------------------------*/
static void DWCi_InitOptMinCompParam(BOOL reset)
{

    if (stpOptMinComp && stpOptMinComp->valid){
        stpOptMinComp->recvBit      = 0;
        stpOptMinComp->timeoutBit   = 0;
        stpOptMinComp->retry        = 0;
        stpOptMinComp->lastPollTime = OS_GetTick();
        if (!reset) stpOptMinComp->startTime = OS_GetTick();
    }
}


/*---------------------------------------------------------------------------*
  Matchmaking Option: Function for frame-by-frame processing used with matchmaking complete with a specific number of hosts or less
  arguments: None
  return values: None
  application: function to check the timeout and perform client polling
          as a matchmaking option for matchmaking complete for a specified number of hosts or less
 *---------------------------------------------------------------------------*/
static void DWCi_ProcessOptMinComp(void)
{
    u32 bitmask;
    int result;
    int i;

    if (!stpOptMinComp || !stpOptMinComp->valid ||
        (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV) ||
        (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL))
        return;

    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_POLL_TIMEOUT){
        bitmask = DWCi_GetAIDBitmask(FALSE);  // Get the AID bit mask for determination
        if (stpOptMinComp->recvBit == bitmask){
            // When all players have replied
            if (stpOptMinComp->timeoutBit == bitmask){
                // Performs matchmaking complete processing when even the client
                // has waited longer than the total player timeout interval.
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "[OPT_MIN_COMP] Timeout occured in all hosts.\n");
                    
                // Matches the desired number of players with the currently connected number of players
                DWCi_GetMatchCnt()->qr2NumEntry = (u8)DWCi_GetMatchCnt()->gt2NumConnection;
                // Adjust parameters in order to proceed to end processing
                DWCi_GetMatchCnt()->clLinkProgress = (u8)(DWCi_GetMatchCnt()->gt2NumConnection-1);

                // matchmaking end processing call
                DWCi_PostProcessConnection(DWC_PP_CONNECTION_SV_CONNECT);
            }
            else {
                // If there are clients who have not reached the timeout yet
                DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                           "[OPT_MIN_COMP] Some clients is in time.\n");

                // Restart measurement of resend interval
                stpOptMinComp->lastPollTime = OS_GetTick();
                stpOptMinComp->recvBit      = 0;  // Clear the receive bit

                if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
                    // Return to available host search status when using friend unspecified peer matchmaking
                    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
//                    DWCi_GetMatchCnt()->sbUpdateCount    = 1;
                    DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                    DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
                }
                else {
                    // Send an NN reservation request after a specific frame
                    // Resent the NN reservation request
                    DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);
                    (void)DWCi_SendResvCommandToFriend(TRUE, FALSE, 0);
                }
            }
        }
        else if (OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->lastPollTime) >= DWC_MATCH_CMD_RTT_TIMEOUT*stpOptMinComp->retry){
            // If the reply from clients is unequal even after the timeout interval has been exceeded
            DWC_Printf(DWC_REPORTFLAG_DEBUG,
                       "[OPT_MIN_COMP] Timeout: wait poll-ACK %d/%d.\n",
                       stpOptMinComp->retry-1, DWC_MATCH_OPT_MIN_COMP_POLL_RETRY_MAX);
            
            if (stpOptMinComp->retry > DWC_MATCH_OPT_MIN_COMP_POLL_RETRY_MAX){
                // lf there is no response from client after being polled the
                // maximum number of times, assume the client was
                // disconnected and attempt matchmaking again.
                DWC_Printf(DWC_REPORTFLAG_DEBUG, 
                           "[OPT_MIN_COMP] Timeout: aidbitmap 0x%x. Restart matching.\n",
                           stpOptMinComp->recvBit);
                
                // Initialize control parameters
                DWCi_InitOptMinCompParam(TRUE);

                // Disconnects all connections and restarts matchmaking from the beginning
                DWCi_CloseAllConnectionsByTimeout();
                DWCi_RestartFromCancel(DWC_MATCH_RESET_RESTART);
            }
            else {
                // Resend command to clients who have not replied yet
                for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
                    if (!(stpOptMinComp->recvBit & (1 << DWCi_GetMatchCnt()->aidList[i]))){
                        result =
                            DWCi_SendMatchCommand(DWC_MATCH_COMMAND_POLL_TIMEOUT,
                                                  DWCi_GetMatchCnt()->sbPidList[i],
                                                  DWCi_GetMatchCnt()->qr2IPList[i],
                                                  DWCi_GetMatchCnt()->qr2PortList[i],
                                                  NULL, 0);
                        if (DWCi_HandleMatchCommandError(result)) return;
                    }
                }
                stpOptMinComp->retry++;
            }
        }
    }
    else if (((DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAIT_RESV) ||
              (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_SEARCH_HOST)) &&
             (DWCi_GetMatchCnt()->gt2NumConnection >= stpOptMinComp->minEntry-1) &&
             ((!stpOptMinComp->retry &&
               (OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime) >= stpOptMinComp->timeout)) ||
              (stpOptMinComp->retry &&
               (OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->lastPollTime) >= stpOptMinComp->timeout >> 2)))){
        // if the server has exceeded the timeout time, and the minimum number
        // for matchmaking complete have been connected, performs polling to
        // see if the client has not also exceeded the timeout time.
        // Beginning from the second time polling occurs, polling is repeated
        // periodically at an interval that is 1/4 the timeout interval

        // First cancel any reservations that have already been sent
        if (DWCi_GetMatchCnt()->reqProfileID){
            result = DWCi_CancelReservation(DWCi_GetMatchCnt()->reqProfileID);
            if (DWCi_HandleMatchCommandError(result)) return;
        }

        // Proceed to polling status
        DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_POLL_TIMEOUT);

        DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                   "[OPT_MIN_COMP] Poll timeout (my time is %lu).\n",
                   OS_TicksToMilliSeconds(OS_GetTick()-stpOptMinComp->startTime));

        // Send polling packet
        for (i = 1; i <= DWCi_GetMatchCnt()->gt2NumConnection; i++){
            result =
                DWCi_SendMatchCommand(DWC_MATCH_COMMAND_POLL_TIMEOUT,
                                      DWCi_GetMatchCnt()->sbPidList[i],
                                      DWCi_GetMatchCnt()->qr2IPList[i],
                                      DWCi_GetMatchCnt()->qr2PortList[i],
                                      NULL, 0);
            if (DWCi_HandleMatchCommandError(result)) return;
        }

        // Overwrite the lastPolltime used to count the time required
        // for data to be returned from all clients with the current time
        stpOptMinComp->lastPollTime = OS_GetTick();
        stpOptMinComp->retry        = 1;  // Record that this has been sent once
    }
}


/*---------------------------------------------------------------------------*
  Error processing function for GPResult
  arguments: result: The type of result due to GP processing
  return values: The type of result due to GP processing (the argument is returned unchanged)
  application: Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) corresponding to the value of GPResult
          while displaying errors (or stopping) corresponding to the value of GPResult
 *---------------------------------------------------------------------------*/
static GPResult DWCi_HandleGPError(GPResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GP_NO_ERROR) return GP_NO_ERROR;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, GP error %d\n", result);

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
    errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GP;

    // Call the matchmaking stop function
    DWCi_StopMatching(dwcError, errorCode);

    return result;
}


/*---------------------------------------------------------------------------*
  Error processing function for SBError
  arguments: error: SB error type
  return values: SB error type (the argument is returned unchanged)
  application: Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) according to the value of SBError
          The corresponding DWC error is passed and matchmaking procedures are terminated
 *---------------------------------------------------------------------------*/
static SBError DWCi_HandleSBError(SBError error)
{
    int errorCode;
    DWCError dwcError;

    if (error == sbe_noerror) return sbe_noerror;

#if 0
    // [sasakit] ignore connecterror
    if (error == sbe_connecterror)
    {
        DWCi_connect_errorcount++;
        DWC_Printf(DWC_REPORTFLAG_ERROR, "********************************%d\n", DWCi_connect_errorcount);
        return sbe_noerror;
    }
#endif

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, SB error %d\n", error);

    switch (error){
    case sbe_socketerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case sbe_dnserror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case sbe_connecterror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    case sbe_dataerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DATA;
        break;
    case sbe_allocerror:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case sbe_paramerror:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    }

    // Base code added
    errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_SB;

    DWCi_StopMatching(dwcError, errorCode);
    
    return error;
}


/*---------------------------------------------------------------------------*
  Error processing function for qr2_error_t
  arguments: error: QR2 error type
  return values: QR2 error type (the argument is returned unchanged)
  application: Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) according to the value of QR2_error_t
          The corresponding DWC error is passed and matchmaking procedures are terminated
 *---------------------------------------------------------------------------*/
static qr2_error_t DWCi_HandleQR2Error(qr2_error_t error)
{
    int errorCode;
    DWCError dwcError;

    if (error == e_qrnoerror) return e_qrnoerror;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, QR2 error %d\n", error);

    switch (error){
    case e_qrwsockerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case e_qrbinderror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_BIND;
        break;
    case e_qrdnserror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    case e_qrconnerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_PEER;
        break;
    case e_qrnochallengeerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    // Add a base error code and call specific error processing during various processing
    switch (DWC_GetState()){
    case DWC_STATE_LOGIN:
        errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_QR2;
        DWCi_StopLogin(dwcError, errorCode);     // Exit login processing
        break;
    case DWC_STATE_UPDATE_SERVERS:
        errorCode += DWC_ECODE_SEQ_FRIEND+DWC_ECODE_GS_QR2;
        DWCi_StopFriendProcess(dwcError, errorCode);  // End friend management
        break;
    case DWC_STATE_MATCHING:
        // If an error occurs during matchmaking
        // in a pre-existing STATE_CONNECTED state
        // it is not necessary to return a separate matchmaking callback
        errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_QR2;
        DWCi_StopMatching(dwcError, errorCode);  // matchmaking ends
        break;
    default:
        errorCode += DWC_ECODE_SEQ_ETC+DWC_ECODE_GS_QR2;
        DWCi_SetError(dwcError, errorCode);
        break;
    }
    
    return error;
}


/*---------------------------------------------------------------------------*
  Error processing function for NegotiateError
  arguments: error: NN error type
  return values: NN error type (the argument is returned unchanged)
  application: Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) according to the value of NegotiateError
          The corresponding DWC error is passed and matchmaking procedures are terminated
 *---------------------------------------------------------------------------*/
static NegotiateError DWCi_HandleNNError(NegotiateError error)
{
    int errorCode;
    DWCError dwcError;

    if (error == ne_noerror) return ne_noerror;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, NN error %d\n", error);

    switch (error){
    case ne_allocerror:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case ne_socketerror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SOCKET;
        break;
    case ne_dnserror:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_DNS;
        break;
    }

    // Base code added
    errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_NN;

    DWCi_StopMatching(dwcError, errorCode);
    
    return error;
}


/*---------------------------------------------------------------------------*
  Error processing function for NegotiateResult
  arguments: result: NN result type
  return values: NN result type (the argument is returned unchanged)
  application: Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) according to the value of NegotiateResult
          The corresponding DWC error is passed and matchmaking procedures are terminated
 *---------------------------------------------------------------------------*/
static NegotiateResult DWCi_HandleNNResult(NegotiateResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == nr_success) return nr_success;

    DWC_Printf(DWC_REPORTFLAG_WARNING, "Match, NN result %d\n", result);

    switch (result){
    case nr_deadbeatpartner:
        return nr_deadbeatpartner;
        break;
    case nr_inittimeout:
        return nr_inittimeout;
        break;
    default:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_UNEXPECTED;
        break;
    }

    if (dwcError){
        // Base code added
        errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_NN;
        DWCi_StopMatching(dwcError, errorCode);
    }
    
    return result;
}


/*---------------------------------------------------------------------------*
  Error processing function for GT2Result
  arguments: result: GT2 processing result type
  return values: GT2 processing result type (the argument is returned unchanged)
  application: Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) according to the value of GT2Result
          while displaying errors (or stopping) corresponding to the value of GPResult
 *---------------------------------------------------------------------------*/
// [note]
// Only gt2Connect can be used
static GT2Result DWCi_HandleGT2Error(GT2Result result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GT2Success) return GT2Success;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Match, GT2 error %d\n", result);

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

    if (dwcError){
        // Base code added
        errorCode += DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_GT2;
        DWCi_StopMatching(dwcError, errorCode);
    }

    return result;
}


/*---------------------------------------------------------------------------*
  SB callback function
  Arguments: sb: SB object
          reason: Reason for calling callback
          server: Updated server structure
          instance: Parameter for the callback
  return values: None
  application: SB callback
 *---------------------------------------------------------------------------*/
static void DWCi_SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
{
#pragma unused(instance)
    int profileID;
    int result;
    int i;
    NegotiateError nnError;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "SBCallback : reason %d (state = %d)\n",
               reason, DWCi_GetMatchCnt()->state);

    switch (reason){
    case sbc_serveradded:     // Add server data
        DWCi_SBPrintServerData(server);  // Display server data test
        break;
        
    case sbc_updatecomplete:  // All server data updates completed
        // The server list is checked and servers for which processing is complete are removed from the list
        // After this, processing is performed only for servers
        // where correct DWC server data exists. If a server for which
        // processing is totally complete existed, it would likely lead to other errors.
        for (i = 0; i < ServerBrowserCount(sb); i++){
            server = ServerBrowserGetServer(sb, i);

            // Checks whether or not the server uses the DWC library and gets its profile ID
            // from the local friend roster.
            if (!DWCi_CheckDWCServer(server)){
                // Servers which do not have a valid profile ID are deleted from the list
                ServerBrowserRemoveServer(sb, server);

                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                           "Deleted server [%d].\n", i);
                i--;
            }
        }

        switch (DWCi_GetMatchCnt()->state){
        case DWC_MATCH_STATE_CL_SEARCH_OWN:
            // When searching for the local host using friend unspecified peer matchmaking
            for (i = 0; i < ServerBrowserCount(sb); i++){
                server = ServerBrowserGetServer(sb, i);

                if (DWCi_GetMatchCnt()->qr2IP &&
                    (DWCi_GetMatchCnt()->qr2IP == SBServerGetPublicInetAddress(server)) &&
                    DWCi_GetMatchCnt()->qr2Port &&
                    (DWCi_GetMatchCnt()->qr2Port == SBServerGetPublicQueryPort(server)))
                    break;
            }

            if (i < ServerBrowserCount(sb)){
                SBError sbError;
                // If the current IP and port number of the local host is already known from a
                // QR2 address callback or if that data has been uploaded to a master server
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_SEARCH_HOST);
                DWCi_GetMatchCnt()->reqProfileID = 0;  // the reservation receiver is cleared

                // Start next search.
                sbError = DWCi_SBUpdateAsync(DWCi_GetMatchCnt()->reqProfileID);
                if (DWCi_HandleSBError(sbError)) return;
            }
            else
            {
                // If the local host's data has not been uploaded to the master server,
                // a reservation for SB server updating using the same conditions is made a second time

//            DWCi_GetMatchCnt()->sbUpdateCount = 1;  // Reserve SB server update
                DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
            }
            break;
            
        case DWC_MATCH_STATE_CL_SEARCH_HOST:
            // when searching for available hosts using friend unspecified peer matchmaking
            // Control flow arrives here when searching by profile ID when introducing
            // to a new server after disconnection from a server
            (void)DWCi_EvaluateServers(TRUE);  // Evaluate each server
            DWCi_RandomizeServers();           // Further, add a random element.

            if (ServerBrowserCount(sb)){
                // Send a reservation request command if any hosts whose members have split up are found
                // application: Sets the specified server data and sends a reservation command to that server
                result = DWCi_SendResvCommand(0, FALSE); 
                if (DWCi_HandleMatchCommandError(result)) return; 

                // Transition to reservation confirmed status
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

//                DWCi_GetMatchCnt()->sbUpdateCount    = 0;
                DWCi_GetMatchCnt()->sbUpdateFlag = 0;
            }
            else {
                // If not even one server could be found, reserve SB server updating a second time
//                DWCi_GetMatchCnt()->sbUpdateCount    = 1;
                DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
            }
            break;
            
        case DWC_MATCH_STATE_CL_SEARCH_NN_HOST:
            // When searching for another host using NAT negotiation
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "searchIP: %x, searchPort: %d\n",
                       DWCi_GetMatchCnt()->searchIP, DWCi_GetMatchCnt()->searchPort);

            // If not the latest data, remove from the server list
            while (ServerBrowserCount(sb)){
                server = ServerBrowserGetServer(sb, 0);
                if ((SBServerGetPublicInetAddress(server) == DWCi_GetMatchCnt()->searchIP) &&
                    (SBServerGetPublicQueryPort(server) == DWCi_GetMatchCnt()->searchPort)){
                    break;
                }
                else {
                    ServerBrowserRemoveServer(sb, server);
                }
            }
                
            if (ServerBrowserCount(sb)){
                // If a server is found
                profileID =
                    SBServerGetIntValue(ServerBrowserGetServer(sb, 0),
                                        DWC_QR2_PID_KEY_STR, 0);
                if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_FRIEND) &&
                    (profileID == DWCi_GetMatchCnt()->sbPidList[0])){
                    // only when a server candidate is found using friend specified peer matchmaking,
                    // evaluate that server
                    if (DWCi_EvaluateServers(FALSE)){
                        if (DWCi_GetMatchCnt()->gt2NumConnection){
                            // If there are hosts already connected to this local host, close all connections and return completely to single client status
                            // close all connections and return completely to 1 client
                            result = DWCi_ChangeToClient();
                            if (DWCi_HandleMatchCommandError(result)) return;

                            // The following processing is skipped in order to wait for the closing of connections
                        }
                    }
                    else {
                        // Cancel the reservation if the result of evaluation is that the server was deleted
                        result = DWCi_CancelReservation(DWCi_GetMatchCnt()->sbPidList[0]);
                        if (DWCi_HandleMatchCommandError(result)) return;

                        // Return status to reservation wait
                        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_WAIT_RESV);

                        // Send an NN reservation request to the next friend
                        result = DWCi_SendResvCommandToFriend(FALSE, FALSE, DWCi_GetMatchCnt()->sbPidList[0]);
                        if (DWCi_HandleMatchCommandError(result)) return;
                        break;  // Skip over the following processing
                    }
                }
                
                // Set the NAT negotiation status
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
                
                // Start NAT negotiation with the server
                server = ServerBrowserGetServer(sb, 0);
                nnError = DWCi_NNStartupAsync(0, 0, server);
                if (DWCi_HandleNNError(nnError)) return;
            }
            else {
                // If a server could not be found, perform SB server updating
                // a second time as this is probably due to the wait time
                // required for uploading data to the master server.
//                DWCi_GetMatchCnt()->sbUpdateCount = 1;  // begin SB server update count
                DWCi_GetMatchCnt()->sbUpdateFlag = DWC_SB_UPDATE_INTERVAL_LONG;
                DWCi_GetMatchCnt()->sbUpdateTick = OS_GetTick();
            }
            break;

        default:
            // If a reservation is accepted from another host while searching for an available host
            // using friend unspecified peer matchmaking, control flow will arrive here under status given
            // by state = DWC_MATCH_STATE_SV_OWN_NN, but code doesn't particularly do anything.
            break;
        }
        break;
        
    case sbc_queryerror:      // Query string error
        break;
        
    default:
        // Since the server list is cleared every update, reason should
        // not be set to sbc_serverupdated or sbc_serverupdatefailed.
        //  It should be sbc_serverdeleted.
        break;
    }
}


/*---------------------------------------------------------------------------*
  Function for testing the SB server data display
  arguments: server: Updated server structure
  return values: None
  application: Displays SB server data on the console
 *---------------------------------------------------------------------------*/
static void DWCi_SBPrintServerData(SBServer server)
{
#if defined(SDK_FINALROM)
#pragma unused(server)
#endif

    int i;

    // Display server data
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPrivateAddress     = %s\n",
               SBServerGetPrivateAddress(server));
  	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPrivateInetAddress = %x\n",
               SBServerGetPrivateInetAddress(server));
  	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPrivateQueryPort   = %d\n",
               SBServerGetPrivateQueryPort(server));
  	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPublicAddress      = %s\n",
               SBServerGetPublicAddress(server));
   	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPublicInetAddres   = %x\n",
               SBServerGetPublicInetAddress(server));
  	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerGetPublicQueryPort    = %d\n",
               SBServerGetPublicQueryPort(server));
   	DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
               "SBServerHasPrivateAddress     = %d\n",
               SBServerHasPrivateAddress(server));

    // Display the key data used by DWC
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "numplayers  = %d\n",
               SBServerGetIntValue(server, "numplayers", -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "maxplayers  = %d\n",
               SBServerGetIntValue(server, "maxplayers", -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s     = %u\n",
               DWC_QR2_PID_KEY_STR,
               SBServerGetIntValue(server, DWC_QR2_PID_KEY_STR, 0));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %u\n",
               DWC_QR2_MATCH_RESV_KEY_STR,
               SBServerGetIntValue(server, DWC_QR2_MATCH_RESV_KEY_STR, -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n",
               DWC_QR2_MATCH_TYPE_KEY_STR,
               SBServerGetIntValue(server, DWC_QR2_MATCH_TYPE_KEY_STR, -1));
    DWC_Printf(DWC_REPORTFLAG_SB_UPDATE, "%s   = %d\n",
               DWC_QR2_MATCH_VER_KEY_STR,
               SBServerGetIntValue(server, DWC_QR2_MATCH_VER_KEY_STR, -1));

    // Display the key data defined by the game
    for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
        if (stGameMatchKeys[i].keyID){
            if (stGameMatchKeys[i].isStr){
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                           "%s  = %s\n",
                           stGameMatchKeys[i].keyStr,
                           SBServerGetStringValue(server,
                                                  stGameMatchKeys[i].keyStr,
                                                  "NONE"));
            }
            else {
                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                           "%s  = %d\n",
                           stGameMatchKeys[i].keyStr,
                           SBServerGetIntValue(server,
                                               stGameMatchKeys[i].keyStr,
                                               -1));
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  Function for evaluating servers
  arguments: sort: TRUE: Server list sorted in order of evaluation, FALSE: Not sorted
  return values: TRUE: Normal, FALSE: Server data has been deleted and the server list has been cleared
  application: Function for evaluating servers
 *---------------------------------------------------------------------------*/
static BOOL DWCi_EvaluateServers(BOOL sort)
{
    int eval;
    int deleteFlag = 0;
    int localDelete;
    int profileID;
    int i, j;
    SBServer server;

    // Evaluate each server
    for (i = 0; i < ServerBrowserCount(DWCi_GetMatchCnt()->sbObj); i++){
        server = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, i);

        // When using friend unspecified peer matchmaking, old QR2 server data of 
        // clients already connected to the local host is made so it cannot be accessed
        if (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_ANYBODY){
            profileID   = SBServerGetIntValue(server, DWC_QR2_PID_KEY_STR, 0);
            localDelete = 0;
            for (j = 1; j <= DWCi_GetMatchCnt()->gt2NumConnection; j++){
                if (profileID == DWCi_GetMatchCnt()->sbPidList[j]){
                    // Remove data if the server is already connected to the local host
                    ServerBrowserRemoveServer(DWCi_GetMatchCnt()->sbObj, server);

                    i--;
                    localDelete = 1;
                    break;
                }
            }

            if (localDelete) continue;
        }

        if (DWCi_GetMatchCnt()->evalCallback){
            eval = DWCi_GetMatchCnt()->evalCallback(i, DWCi_GetMatchCnt()->evalParam);
            
            if (eval > 0){
                // Write the evaluated value on the server list if it is larger than 0.
                // Even given the same value, always apply a difference to shift the value, and set a random value into the low order bits
                // and sets it as GameSpy Status for this user.
                if ( eval > 0x007fffff ) eval = 0x007fffff;
                SBServerAddIntKeyValue(server,
                                       DWC_QR2_MATCH_EVAL_KEY_STR,
                                       (int)((eval << 8) | DWCi_GetMathRand32(256)));
            }
            else {
                // Remove as a matchmaking target if the evaluation value is 0 or less
                ServerBrowserRemoveServer(DWCi_GetMatchCnt()->sbObj, server);

                DWC_Printf(DWC_REPORTFLAG_SB_UPDATE,
                           "Deleted server [%d] (eval point is %d).\n",
                           i, eval);

                i--;
                deleteFlag = 1;
            }
        }
        else {
            // A random evaluation value is set, since we want to change the order of
            // servers each time searching is performed even if server evaluation is not performed.
            // (Since no evaluation has been performed at this time, an evaluation value of 0 may be used.)
            SBServerAddIntKeyValue(server, DWC_QR2_MATCH_EVAL_KEY_STR,
                                   (int)DWCi_GetMathRand32(DWC_MAX_CONNECTIONS << 2));
        }
    }

    //if (!DWCi_GetMatchCnt()->evalCallback) return TRUE;

    if (sort && ServerBrowserCount(DWCi_GetMatchCnt()->sbObj)){
        // Sort if the server list needs to be sorted.
        // Sort is necessary since we are randomly selecting a host to be
        // reserved even though no evaluation callback has been set.
        ServerBrowserSort(DWCi_GetMatchCnt()->sbObj, SBFalse, DWC_QR2_MATCH_EVAL_KEY_STR, sbcm_int);
    }

    if (deleteFlag && !ServerBrowserCount(DWCi_GetMatchCnt()->sbObj)) return FALSE;
    else return TRUE;
}


/*---------------------------------------------------------------------------*
  Function to randomize the server list order
  arguments: None
  return values: None
  application: Add a random element to the a server list that has
          already been evaluated while considering the size of the evaluation values and re-sort the list.
          This function is only to call servers after the DWCi_EvaluateServers() call value.
          [note]
          If you don't need data from servers other than the one you've selected, you don't have to sort.
 *---------------------------------------------------------------------------*/
static void DWCi_RandomizeServers(void)
{
    u32 rand;
    int tmpEval, maxEval = 0;
    int sum = 0;
    int i;
    int eval[DWC_SB_UPDATE_MAX_SERVERS];
    SBServer server;

    if (ServerBrowserCount(DWCi_GetMatchCnt()->sbObj) <= 1) return;

    // Checks all servers and seeks the highest evaluation value and evaluation rate.
    for (i = 0; i < ServerBrowserCount(DWCi_GetMatchCnt()->sbObj); i++){
        server  = ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, i);
        tmpEval = SBServerGetIntValue(server, DWC_QR2_MATCH_EVAL_KEY_STR, -1);
        if (tmpEval > maxEval) maxEval = tmpEval;
        
        sum += stEvalRate[i];
    }

    // Seeks a random value between 0 and 99.
    rand = DWCi_GetMathRand32(100);

    // Seeks the division (%) of the evaluation rate values for all servers, and chooses a server from a random number.
    for (i = 0; i < ServerBrowserCount(DWCi_GetMatchCnt()->sbObj); i++){
        if (i == ServerBrowserCount(DWCi_GetMatchCnt()->sbObj)-1){
            // Due to division errors, the total will not always equal 100%, so once the 
            // end of the process is reached, it will always choose that server.
            eval[i] = 100;
            break;
        }
        else {
            eval[i] = (100*stEvalRate[i])/sum+(i > 0 ? eval[i-1] : 0);
            if (rand < eval[i]) break;
        }
    }

    // Be aware that the index value displayed here is the value at the top of the
    // server list when it has been sorted in order from the highest evaluation value.
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "Server[%d] is selected (%d/100: rand %d)\n",
               i, eval[i], rand);

    // The highest evaluation value is given to the selected server, and servers are re-sorted.
    if (maxEval < 0x7fffffff) maxEval++;
    SBServerAddIntKeyValue(ServerBrowserGetServer(DWCi_GetMatchCnt()->sbObj, i),
                           DWC_QR2_MATCH_EVAL_KEY_STR,
                           maxEval);

    ServerBrowserSort(DWCi_GetMatchCnt()->sbObj, SBFalse, DWC_QR2_MATCH_EVAL_KEY_STR, sbcm_int);
}


/*---------------------------------------------------------------------------*
  Callback function for receiving QR2 server key requests
  Arguments: keyid: Key ID
          outbuf: Send buffer
          userdata: Callback parameters
  return values: None
  application: Callback for receiving QR2 server key requests
 *---------------------------------------------------------------------------*/
static void DWCi_QR2ServerKeyCallback(int keyid, qr2_buffer_t outbuf, void *userdata)
{
#pragma unused(userdata)
    int index;

    switch (keyid){
    case NUMPLAYERS_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2NNFinishCount);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->qr2NNFinishCount);
        break;
    case MAXPLAYERS_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2NumEntry);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->qr2NumEntry );
        break;
    case DWC_QR2_PID_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->profileID);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->profileID );
        break;
    case DWC_QR2_MATCH_TYPE_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2MatchType);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->qr2MatchType );
        break;
    case DWC_QR2_MATCH_RESV_KEY:
        qr2_buffer_add_int(outbuf, DWCi_GetMatchCnt()->qr2Reservation);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWCi_GetMatchCnt()->qr2Reservation );
        break;
    case DWC_QR2_MATCH_VER_KEY:
        qr2_buffer_add_int(outbuf, DWC_MATCHING_VERSION);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, DWC_MATCHING_VERSION);
        break;
    case DWC_QR2_MATCH_EVAL_KEY:
        qr2_buffer_add_int(outbuf, 1);
        DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received ServerKeyReq : keyID %d - %d\n", keyid, 1 );
        break;
    default:
        // Upload data if key data defined by the game exists
        index = keyid-DWC_QR2_GAME_KEY_START;
        if ( ( index >= 0 && index < DWC_QR2_GAME_RESERVED_KEYS )
             && stGameMatchKeys[index].keyID)
        {
            if (stGameMatchKeys[index].isStr){
                qr2_buffer_add(outbuf, (char *)stGameMatchKeys[index].value);
            }
            else {
                qr2_buffer_add_int(outbuf, *(int *)stGameMatchKeys[index].value);
            }
        }
        break;
    }
}


/*---------------------------------------------------------------------------*
  Callback function for receiving QR2 player key requests
  Arguments: keyid: Key ID
          index: Player index
          outbuf: Send buffer
          userdata: Callback parameters
  return values: None
  application: Callback for receiving QR2 player key requests
 *---------------------------------------------------------------------------*/
static void DWCi_QR2PlayerKeyCallback(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
#pragma unused(keyid)
#pragma unused(index)
#pragma unused(outbuf)
#pragma unused(userdata)
}


/*---------------------------------------------------------------------------*
  Callback function for receiving QR2 team key requests
  Arguments: keyid: Key ID
          index: Team index
          outbuf: Send buffer
          userdata: Callback parameters
  return values: None
  application: Callback for receiving QR2 team key requests
 *---------------------------------------------------------------------------*/
static void DWCi_QR2TeamKeyCallback(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
#pragma unused(keyid)
#pragma unused(index)
#pragma unused(outbuf)
#pragma unused(userdata)
}


/*---------------------------------------------------------------------------*
  Callback function for receiving QR2 key list requests
  Arguments: keytype: Key type
          keybuffer: Buffer used to send keys
          userdata: Callback parameters
  return values: None
  application: Callback for receiving QR2 key list key requests
 *---------------------------------------------------------------------------*/
static void DWCi_QR2KeyListCallback(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata)
{
#pragma unused(userdata)
    int i;

    // Register data in buffers used to send required key lists
    switch (keytype){
	case key_server:
        qr2_keybuffer_add(keybuffer, NUMPLAYERS_KEY);
        qr2_keybuffer_add(keybuffer, MAXPLAYERS_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_PID_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_TYPE_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_RESV_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_VER_KEY);
        qr2_keybuffer_add(keybuffer, DWC_QR2_MATCH_EVAL_KEY);

        // If there are keys defined by the game, send those too
        for (i = 0; i < DWC_QR2_GAME_RESERVED_KEYS; i++){
            if (stGameMatchKeys[i].keyID){
                qr2_keybuffer_add(keybuffer, stGameMatchKeys[i].keyID);
            }
        }
        break;
    case key_player:
        break;
    case key_team:
        break;
    }

    DWC_Printf(DWC_REPORTFLAG_QR2_REQ, "QR2, Received KeyListReq : keytype %d\n", keytype);
}


/*---------------------------------------------------------------------------*
  Callback function for getting the number of players or teams registered with QR2
  Arguments: keytype: Key type
          userdata: Callback parameters
  return values: Number of player or teams
  application: Callback function for getting the number of players or teams registered with QR2
 *---------------------------------------------------------------------------*/
static int DWCi_QR2CountCallback(qr2_key_type keytype, void *userdata)
{
#pragma unused(keytype)
#pragma unused(userdata)

    return 0;
}


/*---------------------------------------------------------------------------*
  Callback function for processing errors when registering with the QR2 master server
  Arguments: error: QR2 error type
          userdata: Callback parameters
  return values: None
  application: Callback for processing errors when registering with the QR2 master server
 *---------------------------------------------------------------------------*/
static void DWCi_QR2AddErrorCallback(qr2_error_t error, gsi_char *errmsg, void *userdata)
{
#pragma unused(userdata)
#if defined(SDK_FINALROM)
#pragma unused(errmsg)
#endif

    DWC_Printf(DWC_REPORTFLAG_ERROR, "QR2 Failed query addition to master server %d\n", error);
    DWC_Printf(DWC_REPORTFLAG_ERROR, "%s\n", errmsg);

    (void)DWCi_HandleQR2Error(error);
}


/*---------------------------------------------------------------------------*
  Callback function for notification of QR2 public addresses
  Arguments: ip: Public IP used by QR2
          port: Public port number used by QR2
          userdata: Callback parameters
  return values: None
  application: Callback used for notification of QR2 public addresses
 *---------------------------------------------------------------------------*/
static void DWCi_QR2PublicAddrCallback(unsigned int ip, unsigned short port, void* userdata)
{
#pragma unused(userdata)

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
               "Got my query IP %08x & port %d.\n", ip, port);

    DWCi_GetMatchCnt()->qr2IP   = ip;
    DWCi_GetMatchCnt()->qr2Port = port;
}


/*---------------------------------------------------------------------------*
  Callback function for receiving QR2 NAT negotiation requests
  Arguments: cookie: Received cookie value
          userdata: Callback parameters
  return values: None
  application: Callback for receiving QR2 NAT negotiation requests
 *---------------------------------------------------------------------------*/
static void DWCi_QR2NatnegCallback(int cookie, void *userdata)
{
#pragma unused(userdata)
    NegotiateError nnError;

    // Always accept NAT negotiation requests
    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Got NN request, cookie = %x.\n", cookie);

    if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_CL_WAITING){
        // Status advances if a request from a newly-connected client is accepted
        DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_NN);
    }
    else if ((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN) &&
             (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)){
        // If an NN request arrives after a reservation has been cancelled because the client did not come to the reservation for quite a while, the request is ignored
        // did not come to the reservation for quite a while, the request is ignored
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "But already canceled reservation.\n");
        return;
    }

    if (DWCi_GetMatchCnt()->nnLastCookie == cookie){
        // If the same cookie as last time is received, it is taken as an NN retry and the count is incremented
        DWCi_GetMatchCnt()->nnRecvCount++;
    }
    else {
        // If a different cookie from the last time is received, that's a new NN and the counter is cleared
        DWCi_GetMatchCnt()->nnRecvCount  = 0;
        DWCi_GetMatchCnt()->nnLastCookie = cookie;
    }

    // If a NAT negotiation cookie is received, clear the fail time
    DWCi_GetMatchCnt()->nnFailedTime = 0;

    // Start NAT negotiation
    nnError = DWCi_NNStartupAsync(1, cookie, NULL);
    if (DWCi_HandleNNError(nnError)) return;

    // Terminating reservation hold timeout measurement
    DWCi_GetMatchCnt()->cmdCnt.command = DWC_MATCH_COMMAND_DUMMY;
}


/*---------------------------------------------------------------------------*
  Callback function for receiving QR2 client messages
  Arguments: data: Received data
          len: Received data length
          userdata: Callback parameters
  return values: None
  application: Callback for receiving QR2 client messages
 *---------------------------------------------------------------------------*/
static void DWCi_QR2ClientMsgCallback(gsi_char* data, int len, void* userdata)
{
#pragma unused(userdata)
    int offset = 0;
    BOOL boolResult;
    DWCSBMessage sbMsg;

    // [arakit] main 051010
    // The command is only accepted during matchmaking
    if ((DWC_GetState() != DWC_STATE_MATCHING) &&
        ((DWC_GetState() != DWC_STATE_CONNECTED) ||
         ((DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_SV) &&
          (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL)))){
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed SB matching command.\n");
        return;
    }
    // [arakit] main 051010

    //if (len < sizeof(DWCSBMessageHeader)){
        // If an undefined command message is received
    //    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "QR2, Got undefined SBcommand\n");
    //    return;
    //}

    // Although software is designed to support packets of consecutively sent by
    // multiple commands, it does not send such packets
    while (offset+sizeof(DWCSBMessageHeader) <= len){
        // Copy the SB message
        MI_CpuCopy8(data, &sbMsg, sizeof(DWCSBMessageHeader));

        if (strncmp(sbMsg.header.identifier, DWC_SB_COMMAND_STRING, 4)){
            // Received a command having a different identifier
            DWC_Printf(DWC_REPORTFLAG_WARNING, "Got undefined SBcommand.\n");
            return;
        }
        if (sbMsg.header.version != DWC_MATCHING_VERSION){
            // Received a command with a different matchmaking protocol version
            DWC_Printf(DWC_REPORTFLAG_WARNING, "Got different version SBcommand.\n");
            return;
        }
        
        MI_CpuCopy8(data+sizeof(DWCSBMessageHeader), sbMsg.data, sbMsg.header.size);

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                   "<SB> RECV-0x%02x <- [%08x:%d] [pid=%u]\n",
                   sbMsg.header.command,
                   sbMsg.header.qr2IP, sbMsg.header.qr2Port,
                   sbMsg.header.profileID);
        
        // Process the received command.
        // Error processing is handled inside the function
        boolResult = DWCi_ProcessRecvMatchCommand(sbMsg.header.command,
                                                  sbMsg.header.profileID,
                                                  sbMsg.header.qr2IP,
                                                  sbMsg.header.qr2Port,
                                                  sbMsg.data,
                                                  sbMsg.header.size >> 2);
        if (!boolResult) return;  // Immediately terminate on an error

        offset += sizeof(DWCSBMessageHeader)+sbMsg.header.size;
    }
}


/*---------------------------------------------------------------------------*
  Callback function for processing NN negotiations
  arguments: state: NAT negotiation processing status
          userdata: Callback parameters
  return values: None
  application: Callback for processing NAT negotiations
 *---------------------------------------------------------------------------*/
static void DWCi_NNProgressCallback(NegotiateState state, void* userdata)
{
#pragma unused(userdata)
#if defined(SDK_FINALROM)
#pragma unused(state)
#endif

	DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN, Got state update: %d\n", state);
}


/*---------------------------------------------------------------------------*
  Callback function for completing NN negotiations
  arguments: result: NAT negotiation result type
          gamesocket: Communication socket descriptor
          remoteaddr: Pointer to the address structure
          userdata: Callback parameters
  return values: None
  application: Callback for processing NAT negotiations
 *---------------------------------------------------------------------------*/
static void DWCi_NNCompletedCallback(NegotiateResult result, SOCKET gamesocket, struct sockaddr_in* remoteaddr, void* userdata)
{
#pragma unused(gamesocket)
    char pidStr[12];
    int  index;
    NegotiateError nnError;
    NegotiateResult nnResult;
    GT2Result gt2Result;
    DWCNNInfo* nnInfo = (DWCNNInfo *)userdata;

    DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN, Complete NAT Negotiation. result : %d\n", result);
    if (nnInfo)
        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN cookie = %x.\n", nnInfo->cookie);

    if (((DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_CL_NN) &&
         (DWCi_GetMatchCnt()->state != DWC_MATCH_STATE_SV_OWN_NN)) ||
        !nnInfo){
        // Control flow is ignored even if it arrives here during matchmaking cancellation or after cancellation is complete.
        // Also ignored if NN cookies cannot be received and only
        // the NAT negotiation failed callback (!nnInfo) is received.
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed NN after cancel.\n");
        return;
    }

    if (result == nr_success){
        if (remoteaddr){
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN, remote address : %s\n",
                       gt2AddressToString(remoteaddr->sin_addr.s_addr, SO_NtoHs(remoteaddr->sin_port), NULL));
        }

        nnInfo->cookie = 0;  // Record NN end

        DWCi_GetMatchCnt()->qr2NNFinishCount++;        // Increment the NN complete count
        index = DWCi_GetMatchCnt()->qr2NNFinishCount;  // Back up the index

        if (nnInfo->isQR2){
            // for NN receiving-side
            // Update the IP and port list
            DWCi_GetMatchCnt()->ipList[index]   = remoteaddr->sin_addr.s_addr;
            DWCi_GetMatchCnt()->portList[index] = SO_NtoHs(remoteaddr->sin_port);

            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "NN child finished Nat Negotiation.\n");

            // Clear NN receive data
            DWCi_GetMatchCnt()->nnRecvCount  = 0;
            DWCi_GetMatchCnt()->nnLastCookie = 0;
            DWCi_GetMatchCnt()->nnFailedTime = 0;

            // Proceed to GT2 connection status
            if (DWCi_GetMatchCnt()->state == DWC_MATCH_STATE_SV_OWN_NN){
                DWCi_SetMatchStatus(DWC_MATCH_STATE_SV_OWN_GT2);
            }
            else {
                DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_GT2);
            }

            // Initialize the GT2Connect retry count
            DWCi_GetMatchCnt()->gt2ConnectCount = 0;

            // Always execute gt2Connect() from here
            DWC_Printf(DWC_REPORTFLAG_MATCH_GT2,
                       "gt2Connect() to pidList[%d] (%s)\n",
                       index,
                       gt2AddressToString(DWCi_GetMatchCnt()->ipList[index], DWCi_GetMatchCnt()->portList[index], NULL));

            // NULL can be passed here, since the connection structure
            // is to be retrieved inside the ConnectedCallback.
            // Send the profile ID of the local host as a message.
            (void)OS_SNPrintf(pidStr, sizeof(pidStr), "%u", DWCi_GetMatchCnt()->profileID);
            gt2Result =
                gt2Connect(*DWCi_GetMatchCnt()->pGt2Socket,
                           NULL,
                           gt2AddressToString(DWCi_GetMatchCnt()->ipList[index], DWCi_GetMatchCnt()->portList[index], NULL),
                           (GT2Byte *)pidStr,
                           -1,
                           DWC_GT2_CONNECT_TIMEOUT,
                           DWCi_GetMatchCnt()->gt2Callbacks,
                           GT2False);
            if ( gt2Result == GT2OutOfMemory )
            {
                DWCi_HandleGT2Error(gt2Result);
                return;
            }
            else if ( gt2Result == GT2Success )
            {
                // Wait for the callback if successful.
            }
            else if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[index]))
            {
                // Reset and wait for the next connection if connection to gt2Connect suddenly fails.
                return;
            }
        }
        else {
            // for NN beginning
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN,
                       "NN parent finished Nat Negotiation.\n");

            if (remoteaddr){
                // Update the IP and port list if a NAT negotiation has actually been performed
                // Update the IP and port list
                DWCi_GetMatchCnt()->ipList[index-1]   = remoteaddr->sin_addr.s_addr;
                DWCi_GetMatchCnt()->portList[index-1] = SO_NtoHs(remoteaddr->sin_port);
            }

            // Record the current time in order to measure the
            // time required to receive gt2Connect()
            DWCi_GetMatchCnt()->nnFinishTime = OS_GetTick();

            // Next, establish the GT2 connection (wait for gt2Connect)
            DWCi_SetMatchStatus(DWC_MATCH_STATE_CL_GT2);
        }
    }
    else {
        // If NAT negotiations fail
        if (!nnInfo->cookie){
            // Ignore if this arrives after matchmaking cancellation is complete
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore delayed NN error after cancel.\n");
            return;
        }
            
        // Software does not perform error processing for nr_inittimeout or nr_deadbeatpartner
        nnResult = DWCi_HandleNNResult(result);

        if ((nnResult != nr_inittimeout) && (nnResult != nr_deadbeatpartner)) return;

        if (!nnInfo->isQR2){
            // SB host (host that started NAT negotiation)
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Failed %d/%d NN send.\n",
                       nnInfo->retryCount, DWC_MAX_MATCH_NN_RETRY);
                
            if ((nnResult == nr_deadbeatpartner) ||
                ((nnResult == nr_inittimeout) &&
                 (nnInfo->retryCount >= DWC_MAX_MATCH_NN_RETRY))){
                // If there is no response from the other host or if the retry count has been exceeded
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Abort NN.\n");

                nnInfo->cookie = 0;  // Record NN end

                // Count the number of NN failures
                if (!DWCi_ProcessNNFailure(FALSE)) return;

                // all client processing is terminated and matchmaking is restarted.
                // ends in an error for server-client matchmaking.
                if (!DWCi_CancelPreConnectedClientProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection]))
                    return;
            }
            else {
                nnInfo->retryCount++;  // Record the timeout count
                
                // Start NAT negotiation
                nnError = DWCi_DoNatNegotiationAsync(nnInfo);
                if (DWCi_HandleNNError(nnError)) return;
            }
        }
        else {
            // QR2 host (host that received NAT negotiation)
            DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Failed %d/%d NN recv.\n",
                       DWCi_GetMatchCnt()->nnRecvCount, DWC_MAX_MATCH_NN_RETRY);

            // Record time at which NAT negotiation failed
            DWCi_GetMatchCnt()->nnFailedTime = OS_GetTick();
            
            if ((nnResult == nr_deadbeatpartner) ||
                ((nnResult == nr_inittimeout) &&
                 (DWCi_GetMatchCnt()->nnRecvCount >= DWC_MAX_MATCH_NN_RETRY))){
                // If there is no response from the other host or if the retry count has been exceeded
                DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "Abort NN.\n");

                nnInfo->cookie = 0;  // Record NN end

                // Count the number of NN failures
                if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL) ||
                    (DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_SV)){
                    if (!DWCi_ProcessNNFailure(TRUE)) return;
                }
                else {
                    if (!DWCi_ProcessNNFailure(FALSE)) return;
                }

                // Clear NN receive data
                DWCi_GetMatchCnt()->nnRecvCount  = 0;
                DWCi_GetMatchCnt()->nnLastCookie = 0;
                DWCi_GetMatchCnt()->nnFailedTime = 0;
                    
                // accepting new connections is ended and matchmaking is restarted.
                // In the case of server-client types, this is processed by 
                // cancelling the connection of the newly connected client.
                if (!DWCi_CancelPreConnectedServerProcess(DWCi_GetMatchCnt()->sbPidList[DWCi_GetMatchCnt()->gt2NumConnection+1]))
                    return;
            }
        }
    }
}


/*---------------------------------------------------------------------------*
  Function for processing related to the NN negotiation failure count
  arguments: ignoreError: TRUE: Do not count failures, FALSE: Count failures
  return values: TRUE: Can still fail, FALSE: Ends on a matchmaking error when the specified number of retries has failed
  application: Callback for processing NAT negotiations
 *---------------------------------------------------------------------------*/
static BOOL DWCi_ProcessNNFailure(BOOL ignoreError)
{

    // In order for a server-client matchmaking server and a connected
    // client to cancel the NN and continue play, a count of NN failures is not performed
    if (ignoreError) return TRUE;
    
    if (DWCi_GetMatchCnt()->qr2MatchType != DWC_MATCH_TYPE_SC_CL){
        // Count the number of NN failures
        DWCi_GetMatchCnt()->nnFailureCount++;

        DWC_Printf(DWC_REPORTFLAG_MATCH_NN, "NN failure %d/%d.\n",
                   DWCi_GetMatchCnt()->nnFailureCount, DWC_MATCH_NN_FAILURE_MAX);
    }

    if ((DWCi_GetMatchCnt()->qr2MatchType == DWC_MATCH_TYPE_SC_CL) ||
        (DWCi_GetMatchCnt()->nnFailureCount >= DWC_MATCH_NN_FAILURE_MAX)){
        // Matchmaking ends on an error if the number of NN failures reaches the specified number of retries.
        // Always end on an error server-client matchmaking clients
        DWCi_StopMatching(DWC_ERROR_NETWORK,
                          DWC_ECODE_SEQ_MATCH+DWC_ECODE_GS_NN+DWC_ECODE_TYPE_MUCH_FAILURE);
        return FALSE;
    }
    else {
        return TRUE;
    }
}
//----------------------------------------------------------------------------
// variables moved down here for encapsulated processes
//----------------------------------------------------------------------------
// Pointer to matchmaking control object
static DWCMatchControl* stpMatchCnt = NULL;
#ifdef NITRODWC_DEBUG
static OSTick sMatchStateTick;
#endif
/*---------------------------------------------------------------------------*
  access functions
 *---------------------------------------------------------------------------*/
static void DWCi_SetMatchCnt(DWCMatchControl *pCnt){ 
  stpMatchCnt = pCnt; 
#ifdef NITRODWC_DEBUG
  sMatchStateTick = OS_GetTick();
#endif
}
static DWCMatchControl *DWCi_GetMatchCnt(void){ return (stpMatchCnt); }
static void DWCi_SetMatchStatus(DWCMatchState state)
{
  SDK_ASSERT(stpMatchCnt);
#ifdef NITRODWC_DEBUG
  if(stpMatchCnt->state != state){
	disp_match_state(stpMatchCnt->state, state);
  }
#endif
  stpMatchCnt->state = state;
}

#ifdef NITRODWC_DEBUG
static char *match_st_string[DWC_MATCH_STATE_NUM] = 
{
  "DWC_MATCH_STATE_INIT",           // Initial state
  // Client Status
  "DWC_MATCH_STATE_CL_WAITING",         // Wait status
  "DWC_MATCH_STATE_CL_SEARCH_OWN",      // Searching for information on own host
  "DWC_MATCH_STATE_CL_SEARCH_HOST",     // Open host search status (only when not specifying friends)
  "DWC_MATCH_STATE_CL_WAIT_RESV",       // Waiting for response from server to be reserved
  "DWC_MATCH_STATE_CL_SEARCH_NN_HOST",  // Searching for partner host that has been reserved
  "DWC_MATCH_STATE_CL_NN",              // NAT negotiation in progress
  "DWC_MATCH_STATE_CL_GT2",             // Establishing GT2 connection
  "DWC_MATCH_STATE_CL_CANCEL_SYN",      // matchmaking cancellation synchronization adjustment for the server-client
  "DWC_MATCH_STATE_CL_SYN",             // matchmaking completion synchronization adjustment underway
        
  // Client Status
  "DWC_MATCH_STATE_SV_WAITING",         // Wait status
  "DWC_MATCH_STATE_SV_OWN_NN",          // NAT negotiation with client in progress
  "DWC_MATCH_STATE_SV_OWN_GT2",         // Establishing GT2 connection with client
  "DWC_MATCH_STATE_SV_WAIT_CL_LINK",    // Waiting for connection between clients to complete
  "DWC_MATCH_STATE_SV_CANCEL_SYN",      // Waiting for SYN-ACK for server-client matchmaking cancellation synchronization adjustment
  "DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT", // waiting on end to matchmaking cancellation synchronization adjustment for the server-client
  "DWC_MATCH_STATE_SV_SYN",             // Waiting for matchmaking completion synchronization adjustment SYN-ACK
  "DWC_MATCH_STATE_SV_SYN_WAIT",        // Waiting for matchmaking completion synchronization adjustment to complete
  
  // Common Status
  "DWC_MATCH_STATE_WAIT_CLOSE",         // Waiting for connection close to complete
  
  // Status only while using matchmaking option
  "DWC_MATCH_STATE_SV_POLL_TIMEOUT",    // Server is currently polling for OPTION_MIN_COMPLETE timeouts
};

static void disp_time(void)
{
  int m_time, sec;

  m_time = (int)OS_TicksToMilliSeconds(OS_GetTick() - sMatchStateTick);
  sec = m_time/1000;
  m_time = (m_time-sec+50)/100;
  OS_TPrintf("elapsed time %d.%d seconds\n", sec, m_time);
}
static void disp_match_state(DWCMatchState old, DWCMatchState now)
{
  int m_time, sec;
  OSTick tick;
  tick = sMatchStateTick; sMatchStateTick = OS_GetTick(); tick = sMatchStateTick - tick;
  m_time = (int)OS_TicksToMilliSeconds(tick);
  sec = m_time/1000;
  m_time = (m_time-sec+50)/100;
  OS_TPrintf("Changed Status %s(%d.%d •b) -> %s\n", match_st_string[old], sec, m_time, match_st_string[now]);
}
#endif
