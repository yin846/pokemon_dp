/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  -
  File:     dwc_main.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_main.h,v $
  Revision 1.31  2006/04/27 07:06:09  takayama
  Expanded ingamesn from 10 to 25 characters

  Revision 1.30  01/23/2006 04:50:43  arakit
  revised comments to match standardized terminology

  Revision 1.29  01/17/2006 07:29:17  arakit
  - Changed the return value for DWC_SetConnectionClosedCallback() from void to BOOL,
    and changed to return FALSE without doing anything before allocating control variables.

  Revision 1.28  12/21/2005 03:08:54  arakit
  Made the screen name from within the game transfer to the server when logging in.

  Revision 1.27  11/04/2005 05:57:17  arakit
  Added function description comments.

  Revision 1.26  2005/11/02 02:49:33  arakit
  Adjusted the file's include relationships.

  Revision 1.25  10/28/2005 09:10:20  arakit
  Corrected comments.

  Revision 1.24  10/26/2005 09:06:35  arakit
  DWC_LoginAsync(), DWC_ConnectToAnybodyAsync(), DWC_ConnectToFriendsAsync(),
  Added return values to DWC_SetupGameServerAsync(), DWC_ConnectToGameServerAsync().

  Revision 1.23  10/05/2005 05:19:27  arakit
  Corrected a problem where the State always overwrote LOGIN inside the DWC_UpdateServersAsync() callback.
  Fixed bugs.

  Revision 1.22  09/26/2005 05:06:08  arakit
  - Revised so the addFilter string for the SB update is copied into the library.
  - Revised so the key name string for WC_AddMatchKey*() is also copied into the library.

  Revision 1.21  09/24/2005 12:56:30  arakit
  Fixed so the access functions for AID and the number of connections so that
  only hosts that have completely matching are handled as valid AIDs.

  Revision 1.20  09/21/2005 05:21:26  arakit
  - Made major changes to the close process system, which includes canceling.
  - Created retry and timeout processing for sending commands.
  - Added a matching restart process when NAT negotiation fails.
  - Switched all gt2 soft closes to hard closes.
    In addition, made DWCi_CloseConnectionsAsync() available for use.
    (Note) Sever-client matching does not operate properly.

  Revision 1.19  09/15/2005 00:22:06  arakit
  Added a return value to DWC_CloseConnectionsAsync().
  Added the DWC_CloseConnectionHard(u8 aid) and int DWC_CloseConnectionHardBitmap(u8 aid) functions for forcibly closing connections.
  Added int  DWC_CloseConnectionHardBitmap(u8 aid).

  Revision 1.18  09/13/2005 11:10:56  arakit
  - Corrected a problem where processing stopped due to a data access exception when an attempt was made to send to an aid that was already closed.
    Fixed bugs causing lock-up.
  - Function for getting an AID bitmap   u32 DWC_GetAIDBitmap(void),
    Functions for reliable sending with AID specification u32 DWC_SendReliableBitmap( u32 bitmap, const void* buffer, int size )、
    and unreliable sending with AID specification u32 DWC_SendUnreliableBitmap( u32 bitmap, const void* buffer, int size )
    have been prepared.

  Revision 1.17  09/09/2005 10:43:59  arakit
  Code has been changed to retry if a DNS error occurs with SB or persistent.

  Revision 1.16  09/09/2005 09:54:30  arakit
  Made changes so that matching is not performed between different versionlibraries.

  Revision 1.15  09/08/2005 08:50:24  arakit
  - Changed so that gt2Socket and QR2 start during login.
  - Fixed the bug tht issued duplicate aids.

  Revision 1.14  09/05/2005 11:44:16  arakit
  - Originally changed the isServer argument for DWCConnectionClosedCallback so that when a DS is the client
    during server-client matching, TRUE is returned only when the server closes the connection; however,
    this process was confusing, so TRUE is returned even if the DS is the server and has closed
    the connection itself.
  - Fixed the problem with server processing when the client disappeared
    with server-client matching.
  - Fixed the problem that prevented qr2_shutdown() from being called when
    closing or cancelling during server-client operations.

  Revision 1.13  09/03/2005 13:03:53  arakit
  Changed the callback functions that take the GameSpy Profile ID as an argument, i.e.,
  DWCMatchedSCCallback, DWCNewClientCallback, and DWCConnectionClosedCallback,
  so that they return the friend roster index.

  Revision 1.12  08/31/2005 10:29:46  arakit
  Changed the type name of the DWC control structure to DWC_FriendsMatchControl.

  Revision 1.11  08/31/2005 06:04:52  arakit
  Added the DWC_GetLastSocketError() function for getting the last socket error.

  Revision 1.10  08/31/2005 02:10:35  arakit
  Changed specifications to allow the selection of whether or not friends of friends should be permitted during matching with a friend specification.

  Revision 1.9  08/26/2005 08:15:23  arakit
  ・Organized ASSERT and check of module control variables.
  ・Changed code so that arguments of the external function DWC_ShutdownFriendsMatch() are not taken.
  Also, made code so that control variables for each module cannot be cleared to NULL by the same function.
  ・Changed the error processing function and inserted error setting and error checking where necessary.

  Revision 1.8  08/23/2005 13:53:46  arakit
  Changed code to use MATH_Rand* as the random function

  Revision 1.7  08/20/2005 07:01:20  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_MAIN_H_
#define DWC_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// This switch is for ignoring friend register request errors for friends that are already registered.
#define DWC_IGNORE_GP_ERROR_ALREADY_BUDDY
    
// GP friend message command definition
//#define DWC_GP_COMMAND_STRING       "DWC_GPCM"  // GP message command identification string
#define DWC_GP_COMMAND_STRING       "GPCM"  // GP message command identification string
#define DWC_GP_COMMAND_MATCH_STRING "MAT"   // Message identification string for matchmaking

#define DWC_DNS_ERROR_RETRY_MAX 5  // Max retry limit for DNS errors

// Switch for remote authentication of dummies
//#define DWC_USE_DUMMY_REMOTE_AUTH

// Maximum length (wide char 25 char + "\0\0") of the player name (screen name within the game).
#define DWC_MAX_PLAYER_NAME 26


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// DWC network status enumerator
typedef enum {
    DWC_STATE_INIT = 0,         // Initial state
    DWC_STATE_AVAILABLE_CHECK,  // Checking game availability status
    DWC_STATE_LOGIN,            // Processing login
    DWC_STATE_ONLINE,           // Online status after completion of login
    DWC_STATE_UPDATE_SERVERS,   // friend list synchronization in progress
    DWC_STATE_MATCHING,         // matchmaking in progress
    DWC_STATE_CONNECTED,        // Mesh-type network has been configured after matchmaking has been completed
    DWC_STATE_NUM
} DWCState;


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Connection close call-back type
  Arguments: error: DWC error type
          isLocal : TRUE : Closed by local host, FALSE : Closed by someone else
                   This is set to TRUE even when an unresponsive host is disconnected during matchmaking.
          isServer TRUE: the server-client matchmaking server closed
                         also TRUE when the self is the server and isLocal is TRUE
                   FALSE: Other always FALSE for cases other than
                         it is FALSE.
          aid      aid for the player that closed
          index   Friend roster index for player that closed.
                   Set to -1 if the closed player is not a friend.
          param : Parameters for the callback
  Return value: None
 *---------------------------------------------------------------------------*/
typedef void (*DWCConnectionClosedCallback)(DWCError error, BOOL isLocal, BOOL isServer, u8 aid, int index, void* param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
// FriendsMatch control structure
typedef struct DWCstControl
{
    GT2Socket gt2Socket;   // Pointer to the GT2 socket structure of the this host
    GT2ConnectionCallbacks gt2Callbacks;  // gt2 connection call-back collection
    int  gt2SendBufSize;   // GT2 socket send buffer size
    int  gt2RecvBufSize;   // GT2 socket receive buffer size

    GPConnection gpObj;    // Pointer to gp connection structure
    DWCUserData* userdata; // Pointer to DWCUserData structure

    DWCState state;        // Network status
    DWCState lastState;    // Previous network state
    u8   aid;              // the self-s AID (fixed ID within a mesh network)
    u8   ownCloseFlag;     // TRUE: Closed by the game itself, FALSE: Closed by other means
    u16  playerName[DWC_MAX_PLAYER_NAME];  // Player name for this player that can be referenced by other players
    int  profileID;        // User profile ID
    const char* gameName;  // Pointer to the game name received from GameSpy
    const char* secretKey; // Pointer to the secret key received from GameSpy

    DWCLoginCallback         loginCallback;          // Login complete callback
    void *loginParam;          // Parameter for the above callback
    DWCUpdateServersCallback updateServersCallback;  // friend list synchronization complete callback
    void* updateServersParam;  // Parameter for the above callback
    DWCMatchedCallback       matchedCallback;        // peer matchmaking complete callback
    void* matchedParam;        // Parameter for the above callback
    DWCMatchedSCCallback     matchedSCCallback;      // server-client matchmaking complete callback
    void* matchedSCParam;      // Parameter for the above callback
    DWCConnectionClosedCallback closedCallback;      // Connection closed call-back
    void* closedParam;         // Parameter for the above callback

    DWCLoginControl logcnt;       // login control object
    DWCFriendControl friendcnt;   // friend management object
    DWCMatchControl matchcnt;     // matchmaking control object
    DWCTransportInfo transinfo;   // transport control object
} DWCFriendsMatchControl;

// GT2 connection data structure
typedef struct DWCstConnectionInfo
{
    u8  index;    // Stored connection list index
    u8  aid;      // Unique ID for host
    u16 reserve;  // Reserved
    void* param;  // user data
} DWCConnectionInfo;


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
extern void DWC_InitFriendsMatch(DWCFriendsMatchControl* dwccnt,
                                 DWCUserData* userdata,
                                 int productID,
                                 const char* gameName,
                                 const char* secretKey,
                                 int sendBufSize,
                                 int recvBufSize,
                                 DWCFriendData friendList[],
                                 int friendListLen);

/*---------------------------------------------------------------------------*
  FriendsMatch Library closing function
  Arguments : None
  Return value: None
  Use   : Exits the FriendsMatch library and frees GameSpySDK heap region
 *---------------------------------------------------------------------------*/
extern void DWC_ShutdownFriendsMatch(void);


/*---------------------------------------------------------------------------*
  FriendsMatch Library telecommunication update function
  Arguments : None
  Return value: None
  Use : Called every game frame, this function updates FriendsMatch Library communication
 *---------------------------------------------------------------------------*/
extern void DWC_ProcessFriendsMatch(void);


/*---------------------------------------------------------------------------*
  Wi-Fi connection login function
  Argument　：ingamesn  screen name within the game
          reserved past usage. returns NULL.
          callback:   callback function for notification of login completion
          param : Parameters for the callback
  Return value: If function call completed, TRUE. Returns callback based on success or failure.
          FALSE: Not a state in which this function should be called. Callback does not return.
  Use  : Checks whether the game is available, performs remote authentication, and connects to the GP server
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
extern BOOL DWC_LoginAsync(const u16*  ingamesn,
                           const char* reserved,
                           DWCLoginCallback callback,
                           void* param);


/*---------------------------------------------------------------------------*
  friend list synchronization function
  Function: playerName   Player name for this player that can be referenced by other players
                         →The past specification. Currently ignores the set value.
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
extern BOOL DWC_UpdateServersAsync(const char* playerName,
                                   DWCUpdateServersCallback updateCallback,
                                   void* updateParam,
                                   DWCFriendStatusCallback statusCallback,
                                   void* statusParam,
                                   DWCDeleteFriendListCallback deleteCallback,
                                   void* deleteParam);


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
extern BOOL DWC_ConnectToAnybodyAsync(u8  numEntry,
                                      const char* addFilter,
                                      DWCMatchedCallback matchedCallback,
                                      void* matchedParam,
                                      DWCEvalPlayerCallback evalCallback,
                                      void* evalParam);


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
extern BOOL DWC_ConnectToFriendsAsync(const u8 friendIdxList[],
                                      int friendIdxListLen,
                                      u8  numEntry,
                                      BOOL distantFriend,
                                      DWCMatchedCallback matchedCallback,
                                      void* matchedParam,
                                      DWCEvalPlayerCallback evalCallback,
                                      void* evalParam);


/*---------------------------------------------------------------------------*
  server-client matchmaking server launching function
  Argument : maxEntry    Maximum number of players to be connected (including self)
          matchedCallback   Connection completion callback Called each time a person connects.
          matchedParam   Call-back parameter for above
          newClientCallback Newly connected client notification callback
          newClientParam   Call-back parameter for above
  Return value: If function call completed, TRUE. Matchmaking result comes back in callback.
          FALSE: Not a state in which this function should be called. Callback does not return.
  Use  : Once the server is up and a request is received from a client,
          connect/disconnect processing is performed.
 *---------------------------------------------------------------------------*/
// [arakit] main 051025 051027
extern BOOL DWC_SetupGameServer(u8  maxEntry,
                                DWCMatchedSCCallback matchedCallback,
                                void* matchedParam,
                                DWCNewClientCallback newClientCallback,
                                void* newClientParam);

// We changed the name to reflect its action more accurately. But the old name  ( DWC_SetupGameServersAsync()) can still be used.
#define DWC_SetupGameServerAsync DWC_SetupGameServer
// [arakit] main 051027


/*---------------------------------------------------------------------------*
  server-client matchmaking client launching function
  Argument  : serverIndex   Friend roster index for the server to be connected to
          matchedCallback   Connection completion callback Called each time a person connects.
          matchedParam   Call-back parameter for above
          newClientCallback Newly connected client notification callback
          newClientParam   Call-back parameter for above
  Return value: If function call completed, TRUE. Matchmaking result comes back in callback.
                FALSE: Not a state in which this function should be called. Callback does not return.
  Use  :  Uses friend roster index to specify the server to be connected to,
          or, if a client has newly connected to a server,
          connection processing for that client is performed.
 *---------------------------------------------------------------------------*/
// [arakit] main 051025
extern BOOL DWC_ConnectToGameServerAsync(int serverIndex,
                                         DWCMatchedSCCallback matchedCallback,
                                         void* matchedParam,
                                         DWCNewClientCallback newClientCallback,
                                         void* newClientParam);


/*---------------------------------------------------------------------------*
  Connection close callback setup function
  Argument  : callback   This callback is called each time a connection is closed.
          param   Call-back parameter for the above
  Return values: TRUE : registration succeeded
                 FALSE: registration failed when FriendsMatch Library was inoperable
  Use  : Sets the callback for closing connections
 *---------------------------------------------------------------------------*/
extern BOOL DWC_SetConnectionClosedCallback(DWCConnectionClosedCallback callback, void* param);


/*---------------------------------------------------------------------------*
  Function for force-closing all connections
  Arguments : None
  Return values: Succeeded if zero or greater, failed if negative
          0  : Execute close.
          1  : Although close processing was performed, the callback is not called because the number of connected hosts is 0.
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
extern int  DWC_CloseAllConnectionsHard(void);


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
extern int  DWC_CloseConnectionHard(u8 aid);


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
extern int  DWC_CloseConnectionHardBitmap(u32* bitmap);


/*---------------------------------------------------------------------------*
  Function for getting the number of connected hosts
  Arguments     : None
  Return values : Number of hosts making up the network (including this one).
                   Returns 0 when offline.
  Use           : This function returns the number of hosts on a mesh-type network.
 *---------------------------------------------------------------------------*/
extern int DWC_GetNumConnectionHost(void);


/*---------------------------------------------------------------------------*
  This function gets own AID.
  Arguments    : None
  Return value : Its own AID.
  Use          : This function returns the AID of this host (the unique host ID used within the mesh network).
 *---------------------------------------------------------------------------*/
extern u8  DWC_GetMyAID(void);


/*---------------------------------------------------------------------------*
  Function for getting the AID list of currently connected hosts
  Argument     : aidList  Pointer to the AID list.
  Return value : The AID list length (including self).
  Use          : Gets a pointer to the AID list (does not copy).
                 The AID list is packed, from the front in order, with currently enabled AIDs,
                 and this number of instances represents the number of hosts currently connected, including this host.
 *---------------------------------------------------------------------------*/
extern int DWC_GetAIDList(u8** aidList);


/*---------------------------------------------------------------------------*
  Function for getting the AID bitmap of currently connected hosts
  Arguments    : None
  Return value : AID bitmap. Returns 0 when offline.
  Use          : Gets the AID bitmap for currently connected hosts.
 *---------------------------------------------------------------------------*/
extern u32 DWC_GetAIDBitmap(void);


/*---------------------------------------------------------------------------*
  Function for determining if an AID is valid
  Argument      : aid  AID
  Return values : TRUE: The AID is valid, FALSE: AID invalid
  Use           : This function checks whether or not the specified AID is valid (whether or not it is connected to this host).
                  Returns FALSE if the AID for this host was specified.
                  AID of the newly connected client (when matchmaking) is judged invalid.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_IsValidAID(u8 aid);


/*---------------------------------------------------------------------------*
  FriendsMatch Library status getting function
  Arguments    : None
  Return value : DWCState type enumerator
  Use          : gets the processing state of the FriendsMatch Library
 *---------------------------------------------------------------------------*/
extern DWCState DWC_GetState(void);


/*---------------------------------------------------------------------------*
  Get socket error function
  Arguments    : None
  Return value : GT2 socket error type
  Use          : Gets the socket error type in the GameSpy gt2 SDK
 *---------------------------------------------------------------------------*/
extern int DWC_GetLastSocketError(void);


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
extern int DWC_CloseConnectionsAsync(void);


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
extern GT2Result DWCi_GT2Startup(void);


extern GT2Connection DWCi_GetGT2Connection(u8 aid);


extern u8  DWCi_GetConnectionAID(GT2Connection connection);


extern u8  DWCi_GetConnectionIndex(GT2Connection connection);


extern void* DWCi_GetConnectionUserData(GT2Connection connection);


extern int  DWCi_GT2GetConnectionListIdx(void);


extern void DWCi_ClearGT2ConnectionList(void);


extern GT2Connection* DWCi_GetGT2ConnectionByIdx(int index);


extern GT2Connection* DWCi_GetGT2ConnectionByProfileID(int profileID, int numHost);


extern DWCConnectionInfo* DWCi_GetConnectionInfoByIdx(int index);


extern BOOL DWCi_IsValidAID(u8 aid);


#ifdef __cplusplus
}
#endif


#endif // DWC_MAIN_H_
