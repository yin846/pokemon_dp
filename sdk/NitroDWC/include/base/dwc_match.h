/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_common
  File:     dwc_match.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_match.h,v $
  Revision 1.59  02/07/2006 00:16:36  arakit
  Created DWC_StopGameServer() as a function to stop the server-client matchmaking server.

  Revision 1.58  01/26/2006 05:33:15  arakit
  Created DWC_StopGameServer() as a function to stop the server-client matchmaking server.

  Revision 1.57  01/25/2006 07:52:14  arakit
  - fixed an issue where a child device would hang if a parent device was disconnected during matchmaking completion synchronization
  - fixed so that, for friend specified peer matching, when distantFriend =  FALSE,
    only the specified friend can connect.

  Revision 1.56  01/23/2006 04:50:43  arakit
  revised comments to match standardized terminology

  Revision 1.55  11/29/2005 09:14:19  arakit
  Reduced the maximum number of servers gotten by ServerBrowserLimitUpdate() to 6.

  Revision 1.54  11/04/2005 11:37:00  arakit
  Corrected function description comments.

  Revision 1.53  11/02/2005 02:49:32  arakit
  Adjusted the file's include relationships.

  Revision 1.52  10/28/2005 09:09:58  arakit
  ・To reflect the deletion of DWCi_GetCurrentTime(), changed so that all time is held by OSTick.
  ・Deleted or corrected [todo] comment sections.

  Revision 1.51  10/05/2005 08:28:50  arakit
  Added a matching option for returning a dedicated error to blocked clients when server-client matching has been used and matching is blocked after matching is complete.
  Added a matching option for returning dedicated errors to clients.

  Revision 1.50  10/05/2005 06:50:52  arakit
  When using matching with a friend specification, even if there is no established friend,
  an insufficient friend error is no longer output when those conditions are found.

  Revision 1.49  10/04/2005 09:02:14  arakit
  Changed code to retain integrity even if connections are closed and power turned OFF during matching complete sync adjustment.

  Revision 1.48  10/04/2005 07:42:33  sasakit
  Tuned SBUpdate parameters

  Revision 1.47  10/04/2005 07:28:00  sasakit
  Adjusted the timing used for SBUpdate.

  Revision 1.46  10/03/2005 12:21:19  arakit
  Made it so that only a set number of retries are attempted if a timeout from the server occurs during
  a client reservation when using server-client type matching.

  Revision 1.45  10/03/2005 05:06:11  arakit
  The name of the QR2 startup function has been changed to DWC_RegisterMatchingStatus.

  Revision 1.43  10/01/2005 14:25:45  arakit
  Fixed the problem with the timeout process.

  Revision 1.42  10/01/2005 11:32:24  sasakit
  Revised most of the Timeouts to the msec order.

  Revision 1.41  10/01/2005 07:16:09  arakit
  Created a cancel synchronization adjustment process during server-client matching.

  Revision 1.40  09/30/2005 02:50:07  arakit
  Deleted the switch constant DWC_CLOSE_CONNECTION_NEW.

  Revision 1.39  09/28/2005 12:42:20  arakit
  - Changed so that an error is generated if NAT negotiation fails five times during a single match.
  - Changed the NAT negotiation cookies so that the upper 16 bits are a random number
    and the lower 16 bits are the profile ID.

  Revision 1.38  2005/09/27 13:02:15  arakit
  Expanded the wait time for connection completion between children so that it waits at least until the NN retry completes.

  Revision 1.37  2005/09/27 05:14:25  arakit
  Made the data size for matching completion synchronized communication into a constant.

  Revision 1.36  09/26/2005 16:16:09  sasakit
  Decreased the number of NAT Negotiation retries.

  Revision 1.35  09/26/2005 11:06:26  arakit
  Fixed the problem that caused numplayers for QR2 key to be an irregular value such as 253.

  Revision 1.34  09/26/2005 05:06:08  arakit
  - Revised so the addFilter string for the SB update is copied into the library.
  - Revised so the key name string for WC_AddMatchKey*() is also copied into the library.

  Revision 1.33  09/24/2005 12:56:30  arakit
  Fixed so the access functions for AID and the number of connections so that
  only hosts that have completely matching are handled as valid AIDs.

  Revision 1.32  09/23/2005 04:30:09  arakit
  Added and revised timeout procedures for unresponsive hosts.

  Revision 1.31  09/21/2005 07:13:14  arakit
  Set the timeout for the gt2Connect wait.

  Revision 1.30  09/21/2005 06:06:45  arakit
  Made gt2Connect restart matching when exceeding the number of retries.

  Revision 1.29  09/21/2005 05:21:26  arakit
  - Made major changes to the close process system, which includes canceling.
  - Created retry and timeout processing for sending commands.
  - Added a matching restart process when NAT negotiation fails.
  - Switched all gt2 soft closes to hard closes.
    In addition, made DWCi_CloseConnectionsAsync() available for use.
  (Note) Sever-client matching does not operate properly.

  Revision 1.28  09/19/2005 14:51:24  sasakit
  Changed the resend process when receiving RESV_WAIT.

  Revision 1.27  09/15/2005 13:02:06  arakit
  Prevented a message from being send when old client QR2 data for specified friend matching was found.

  Revision 1.26  09/15/2005 09:44:32  arakit
  - Included a portion of the SB message command resend process.
  - Made changes so QR2 data loaded to the server can be more accurately judged.

  Revision 1.25  09/15/2005 06:07:41  arakit
  Added the DWC_GetMOMinCompState(u64* time) function
  that can obtain the status of the matching completion option when the numbers are lower than specified.

  Revision 1.24  09/14/2005 10:07:49  arakit
  Added a configuration function for matching completion option when the numbers are lower than specified.

  Revision 1.23  09/13/2005 11:11:30  arakit
  Revised program.

  Revision 1.22  09/10/2005 09:02:57  arakit
  Added the DWC_GetMatchingState() function that indicates the matching internal state for debug purposes.

  Revision 1.21  09/09/2005 09:54:09  arakit
  Made changes so that matching is not performed between different version libraries.

  Revision 1.20  09/08/2005 14:41:26  arakit
  - Shorten the resend interval for reserved commands when not even a single
    friend matching has started during friend-specific matching.
  - Fixed the problem that caused a response to a reserved command that was received late.

  Revision 1.19  09/08/2005 08:50:24  arakit
  - Changed so that gt2Socket and QR2 start during login.
  - Fixed the bug that issued duplicate aids.

  Revision 1.18  09/08/2005 02:48:34  arakit
  Changed so that servers with connected children can wait for connections from other clients
  by delaying the start of the search (or reservations) for other servers.

  Revision 1.17  09/07/2005 12:43:34  arakit
  Changed the specifications for non-specific friend matching to decrease the time for matching.

  Revision 1.16  09/05/2005 11:44:29  arakit
  - Originally changed the isServer argument for DWCConnectionClosedCallback so that when a DS is the client
    during server-client matching, TRUE is returned only when the server closes the connection; however,
    this process was confusing, so TRUE is returned even if the DS is the server and has closed
    the connection itself.
  - Fixed the problem with server processing when the client disappeared
    with server-client matching.
  - Fixed the problem that prevented qr2_shutdown() from being called when
    closing or cancelling during server-client operations.
  - Added DWC_GetLastMatchingType() to obtain the final matching type.

  Revision 1.15  09/03/2005 13:03:53  arakit
  Changed the callback functions that take the GameSpy Profile ID as an argument, i.e.,
  DWCMatchedSCCallback, DWCNewClientCallback, and DWCConnectionClosedCallback,
  so that they return the friend roster index.

  Revision 1.14  09/01/2005 12:15:00  arakit
  Added a synchronization adjustment process during matching completion.

  Revision 1.13  08/31/2005 02:10:07  arakit
  When matching by specifying friends, can now select whether to allow friends of friends to be matched.
  - When matching by not specifying a friend, made the client status maintain random time.

  Revision 1.12  08/29/2005 12:28:15  arakit
  - Changed so that during matching by not specifying a friend, hosts that have become servers
  revert to clients after timing out and start to search for servers again.
  - Fixed the problem that sent a NN reservation to clients that were already connected
  to a different server during matching without specifying friends.

  Revision 1.11  08/29/2005 06:37:17  arakit
  - Fixed the problem that caused a SB timeout to be issued during matching with specified friends.
  - Fixed so that the specified number of connections is applied during matching with specified friends.
  -Code has been made so that the maximum number of connected players and the current number of connected players can be set to the locationString of gpStatus using server-client matching.
  A function has been prepared for accessing this value.

  Revision 1.10  08/26/2005 08:15:23  arakit
  ・Organized ASSERT and check of module control variables.
  ・Changed code so that arguments of the external function DWC_ShutdownFriendsMatch() are not taken.
  Also, made code so that control variables for each module cannot be cleared to NULL by the same function.
  ・Changed the error processing function and inserted error setting and error checking where necessary.

  Revision 1.9  08/24/2005 07:12:07  arakit
  Changed so that qr2_shutdown() is not called from the qr2 callback.

  Revision 1.8  08/23/2005 13:54:52  arakit
  - Changed the communication command resend interval due to GP buddy messages or SB messages
  from 30 game frames to 60 game frames.
  - Fixed the bug that caused a QR2 errors when matching completed, after cancelling, or when matching was retried.

  Revision 1.7  08/20/2005 11:28:07  sasakit
  Took care of lines that ended with \.

  Revision 1.6  08/20/2005 08:01:20  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_MATCH_H_
#define DWC_MATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
// Specifications call for terminating QR2 once matching has ended when using matchmaking other than server-client matchmaking
#define DWC_QR2_ALIVE_DURING_MATCHING

// Depending on the situation, it's okay to start matchmaking even when there are no friends on the friend roster
#define DWC_MATCH_ACCEPT_NO_FRIEND

// activates friend-limited friend specified peer matchmaking
#define DWC_LIMIT_FRIENDS_MATCH_VALID

// activates interim stops for server-client matchmaking
#define DWC_STOP_SC_SERVER
    
// Matchmaking protocol version. Used for matchmaking commands for different versions or
// to prevent communication with the server.
#define DWC_MATCHING_VERSION 3U
    
// Based on the gt2 standard. (cannot be made higher than 32 because of its relation to AID bitmapping)
#define DWC_MAX_CONNECTIONS 32

// QR2 custom key index (NUM_RESERVED_KEYS = 50)
// 50 -99 are the DWC reserved keys, and 100 - 253 are the keys that can be used in the game.
#define DWC_QR2_PID_KEY            NUM_RESERVED_KEYS
#define DWC_QR2_MATCH_TYPE_KEY     (NUM_RESERVED_KEYS+1)
#define DWC_QR2_MATCH_RESV_KEY     (NUM_RESERVED_KEYS+2)
#define DWC_QR2_MATCH_VER_KEY      (NUM_RESERVED_KEYS+3)
#define DWC_QR2_MATCH_EVAL_KEY     (NUM_RESERVED_KEYS+4)

// Character strings for each key
#define DWC_QR2_PID_KEY_STR        "dwc_pid"
#define DWC_QR2_MATCH_TYPE_KEY_STR "dwc_mtype"
#define DWC_QR2_MATCH_RESV_KEY_STR "dwc_mresv"
#define DWC_QR2_MATCH_VER_KEY_STR  "dwc_mver"
#define DWC_QR2_MATCH_EVAL_KEY_STR "dwc_eval"

// DWC reserved key (50)
#define DWC_QR2_RESERVED_KEYS  (100-NUM_RESERVED_KEYS)
    
// The maximum number of keys to use in the game (153)
#define DWC_QR2_GAME_RESERVED_KEYS (MAX_REGISTERED_KEYS-NUM_RESERVED_KEYS-DWC_QR2_RESERVED_KEYS)
    
// Start value for the game definition key
#define DWC_QR2_GAME_KEY_START 100


// Matchmaking command for SB messages or GP buddy messages
#define DWC_SB_COMMAND_STRING "SBCM"  // SB message command identifying string
    
#define DWC_MATCH_COMMAND_RESERVATION    0x01  // NAT negotiation reservation request
#define DWC_MATCH_COMMAND_RESV_OK        0x02  // NAT negotiation reservation completion
#define DWC_MATCH_COMMAND_RESV_DENY      0x03  // NAT negotiation reservation refusal
#define DWC_MATCH_COMMAND_RESV_WAIT      0x04  // NAT negotiation reservation wait request
#define DWC_MATCH_COMMAND_RESV_CANCEL    0x05  // NAT negotiation reservation cancellation request
#define DWC_MATCH_COMMAND_TELL_ADDR      0x06  // NN starting address notification (NAT negotiation unnecessary)
#define DWC_MATCH_COMMAND_NEW_PID_AID    0x07  // New connection client profile ID and AID notification
#define DWC_MATCH_COMMAND_LINK_CLS_REQ   0x08  // Request for connection between clients
#define DWC_MATCH_COMMAND_LINK_CLS_SUC   0x09  // Notification of successful connection between clients
#define DWC_MATCH_COMMAND_CLOSE_LINK     0x0a  // Notification of closed connection when connection is established
#define DWC_MATCH_COMMAND_RESV_PRIOR     0x0b  // NAT negotiation priority reservation request
#define DWC_MATCH_COMMAND_CANCEL         0x0c  // matchmaking cancel command
#define DWC_MATCH_COMMAND_CANCEL_SYN     0x0d  // SYN for matchmaking cancellation synchronization
#define DWC_MATCH_COMMAND_CANCEL_SYN_ACK 0x0e  // SYN-ACK for matchmaking cancellation synchronization
#define DWC_MATCH_COMMAND_CANCEL_ACK     0x0f  // ACK for matchmaking cancellation synchronization
#define DWC_MATCH_COMMAND_SC_CLOSE_CL    0x10  // Notification of unintentional disconnect for already connected clients
#define DWC_MATCH_COMMAND_POLL_TIMEOUT   0x11  // OPTION_MIN_COMPLETE timeout polling
#define DWC_MATCH_COMMAND_POLL_TO_ACK    0x12  // ACK to OPTION_MIN_COMPLETE timeout polling
#define DWC_MATCH_COMMAND_SC_CONN_BLOCK  0x13  // Reservation denied due to block by OPTION_SC_CONNECT_BLOCK
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
#define DWC_MATCH_COMMAND_FRIEND_ACCEPT  0x20  // new connection friend permission from client
#endif
#define DWC_MATCH_COMMAND_CL_WAIT_POLL   0x40  // client standby timeout polling
#define DWC_MATCH_COMMAND_SV_KA_TO_CL    0x41  // server keep-alive for the above polling
#define DWC_MATCH_COMMAND_DUMMY          0xff  // Dummy command

#if 0
#define DWC_MATCH_CMD_RESEND_INTERVAL     60    // interval for resending matchmaking command
#define DWC_MATCH_CMD_RESEND_INTERVAL_ADD 150   // added time base for interval for resending matchmaking command
#define DWC_MATCH_CMD_RESV_TIMEOUT        480   // NN reserve command response timeout
#define DWC_SB_UPDATE_INTERVAL            60    // SB server update interval
#define DWC_SB_UPDATE_INTERVAL_ADD        180   // SB server update interval additional time base
#define DWC_QR2_KEEP_RESV_TIMEOUT         600   // QR2 reservation hold timeout time
#else
#define DWC_MATCH_CMD_RESEND_INTERVAL_MSEC     3000   // interval for resending matchmaking command
#define DWC_MATCH_CMD_RESEND_INTERVAL_ADD_MSEC 3000   // added time base for interval for resending matchmaking command
#define DWC_MATCH_CMD_RESV_TIMEOUT_MSEC        6000   // NN reserve command response timeout
#define DWC_SB_UPDATE_INTERVAL_MSEC            3000   // SB server update interval
#define DWC_SB_UPDATE_INTERVAL_SHORT_MSEC      1000   // SB server update interval
#define DWC_SB_UPDATE_INTERVAL_ADD_MSEC        3000   // SB server update interval additional time base
#define DWC_SB_UPDATE_INTERVAL_SHORT           1
#define DWC_SB_UPDATE_INTERVAL_LONG            2
#endif

// In the case when processing occurs soon after sending a command or when commands return processes,
// the general-purpose timeout time for command round-trips
#define DWC_MATCH_CMD_RTT_TIMEOUT          6000

// The general purpose timeout time (assuming two searches) for
// the process from the SB server update until the targeted server data is found
#define DWC_MATCH_SB_UPDATE_TIMEOUT        13000

// Standard maximum resends for matchmaking commands
#define DWC_MATCH_CMD_RETRY_MAX            5

// Timeout time for synchronization adjustments after matchmaking is complete
#define DWC_MATCH_SYN_ACK_WAIT_TIME        DWC_MATCH_CMD_RTT_TIMEOUT

// timeout time for synchronization adjustments after server-client matchmaking is complete
#define DWC_MATCH_CANCEL_SYN_ACK_WAIT_TIME DWC_MATCH_CMD_RTT_TIMEOUT

// timeout time for reservation hold for peer matchmaking without specifying friends
#define DWC_MATCH_RESV_KEEP_TIME_ANYBODY   DWC_MATCH_CMD_RTT_TIMEOUT
// timeout time for reservation hold for server-client matchmaking with friends specified
// (considering the Reservation OK command send + SB server update + NN start time)
#define DWC_MATCH_RESV_KEEP_TIME_FRIEND    (DWC_MATCH_CMD_RTT_TIMEOUT+DWC_MATCH_SB_UPDATE_TIMEOUT)

#define DWC_MAX_MATCH_NN_RETRY  1   // Maximum number of retries for NAT negotiation
#define DWC_MATCH_NN_FAILURE_MAX 5  // Maximum number of NAT negotiation failures (exceeding this number causes termination with an error)

// Timeout time for waiting to retry after NAT negotiation failure
#define DWC_WAIT_NN_RETRY_TIMEOUT          10000

// Timeout time for gt2Connect
#define DWC_GT2_CONNECT_TIMEOUT            5000

// Timeout time for waiting for gt2Connect reception after NAT negotiation completes
#define DWC_WAIT_GT2_CONNECT_TIMEOUT       (DWC_GT2_CONNECT_TIMEOUT*DWC_MATCH_CMD_RETRY_MAX)

// Because the timeout time for completion for 1-1 client connections (using the server)
// has become an immediate timeout process when the timeout time is reached without resending
// the CLS_REQ command
// it is necessary to wait for a time of PARTNER_WAIT_TIME（10msec）×DWC_WAIT_NN_RETRY_TIMEOUT＋alpha
// and the wait for the NN retry.
#define DWC_MATCH_LINK_CLS_TIMEOUT         30000

// if there is not the following time for receiving data from the server during client standby,
// consider it a timeout and perform server polling
#define DWC_MATCH_CL_WAIT_TIMEOUT          30000

// Maximum number for retries for reserved commands after RESV_WAIT reception (number of retries excludes the original attempt)
#define DWC_RESV_COMMAND_RETRY_MAX 16

// Number of timeout polling retries with OPTION_MIN_COMPLETE
// (number of retries excludes the original attempt)
#define DWC_MATCH_OPT_MIN_COMP_POLL_RETRY_MAX 5

// Maximum number of search servers to pass to ServerBrowserLimitUpdate()
#define DWC_SB_UPDATE_MAX_SERVERS 6

// the size of the Reliable send data for the matchmaking completion synchronization packet
#define DWC_MATCH_SYN_DATA_BODY_SIZE 4

// [todo]
// Matchmaking request friend index list length.
// Temporarily set to a constant, but will use the size specified in the game
#define DWC_MAX_MATCH_IDX_LIST 64

    
//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// matchmaking option type
typedef enum {
    DWC_MATCH_OPTION_MIN_COMPLETE = 0,  // with friend specified/unspecified peer matchmaking,
                                        // matchmaking is completed for the specified number of people or less
    DWC_MATCH_OPTION_SC_CONNECT_BLOCK,  // with server-client matchmaking,
                                        // after matchmaking for one host is complete, other connections cannot be accepted.
    DWC_MATCH_OPTION_NUM
} DWCMatchOptType;

// DWC_SetMatchingOption() result type
enum {
    DWC_SET_MATCH_OPT_RESULT_SUCCESS = 0,  // No error
    DWC_SET_MATCH_OPT_RESULT_E_BAD_STATE,  // FriendsMatch Library non-operational
    DWC_SET_MATCH_OPT_RESULT_E_INVALID,    // Invalid option specification
    DWC_SET_MATCH_OPT_RESULT_E_PARAM,      // Parameter error
    DWC_SET_MATCH_OPT_RESULT_E_ALLOC,      // Alloc error
    DWC_SET_MATCH_OPT_RESULT_NUM
};


// Matchmaking status enumerator
// [todo]
// enum not referencing the game or other libraries are moved to dwc_match.c (the above define, too)
typedef enum {
    DWC_MATCH_STATE_INIT = 0,           // Initial state

    // Client Status
    DWC_MATCH_STATE_CL_WAITING,         // Wait status
    DWC_MATCH_STATE_CL_SEARCH_OWN,      // Searching for information on own host
    DWC_MATCH_STATE_CL_SEARCH_HOST,     // Open host search status (only when not specifying friends)
    DWC_MATCH_STATE_CL_WAIT_RESV,       // Waiting for response from server to be reserved
    DWC_MATCH_STATE_CL_SEARCH_NN_HOST,  // Searching for partner host that has been reserved
    DWC_MATCH_STATE_CL_NN,              // NAT negotiation in progress
    DWC_MATCH_STATE_CL_GT2,             // Establishing GT2 connection
    DWC_MATCH_STATE_CL_CANCEL_SYN,      // matchmaking cancellation synchronization adjustment underway for the server-client
    DWC_MATCH_STATE_CL_SYN,             // matchmaking completion synchronization adjustment underway
        
    // Client Status
    DWC_MATCH_STATE_SV_WAITING,         // Wait status
    DWC_MATCH_STATE_SV_OWN_NN,          // NAT negotiation with client in progress
    DWC_MATCH_STATE_SV_OWN_GT2,         // Establishing GT2 connection with client
    DWC_MATCH_STATE_SV_WAIT_CL_LINK,    // Waiting for connection between clients to complete
    DWC_MATCH_STATE_SV_CANCEL_SYN,      // Waiting for SYN-ACK for server-client matchmaking cancellation synchronization adjustment
    DWC_MATCH_STATE_SV_CANCEL_SYN_WAIT, // waiting on end to matchmaking cancellation synchronization adjustment for the server-client
    DWC_MATCH_STATE_SV_SYN,             // Waiting for matchmaking completion synchronization adjustment SYN-ACK
    DWC_MATCH_STATE_SV_SYN_WAIT,        // Waiting for matchmaking completion synchronization adjustment to complete

    // Common Status
    DWC_MATCH_STATE_WAIT_CLOSE,         // Waiting for connection close to complete

    // Status only while using matchmaking option
    DWC_MATCH_STATE_SV_POLL_TIMEOUT,    // Server is currently polling for OPTION_MIN_COMPLETE timeouts

    DWC_MATCH_STATE_NUM
} DWCMatchState;

// Matchmaking type enumerator
enum {
    DWC_MATCH_TYPE_ANYBODY = 0,  // friend unspecified peer matchmaking
    DWC_MATCH_TYPE_FRIEND,       // friend specified peer matchmaking
    DWC_MATCH_TYPE_SC_SV,        // server-client matchmaking server-side
    DWC_MATCH_TYPE_SC_CL,        // server-client matchmaking client-side
    DWC_MATCH_TYPE_NUM
};


//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  friend specified/unspecified peer matchmaking completion callback type
  Arguments: error: DWC error type
          cancel TRUE: Matchmaking ended with cancellation, FALSE: No cancellation
          param : Parameters for the callback
  Return value: None
  Called once when the set number of connections are made
 *---------------------------------------------------------------------------*/
typedef void (*DWCMatchedCallback)(DWCError error, BOOL cancel, void* param);

/*---------------------------------------------------------------------------*
  server-client matchmaking complete callback type
  Arguments: error: DWC error type
          cancel TRUE: Matchmaking ended with cancellation, FALSE: No cancellation
          self     TRUE: Either connection of host to server was successful or
                         the connection was cancelled.
                   FALSE: Either another host connected to the server successfully, or
                         the connection was cancelled.
          isServer TRUE: When self = FALSE, the host that completed the above operation
                         is the server.
                   When self = FALSE, the host that completed the above operation
                         is the client or self = TRUE
          index    When self = FALSE, the friend roster index for
                   host that completed the above operation.
                   If the host is not a friend or if self = TRUE, then it is -1.
          param : Parameters for the callback
  Return value: None
  Called each time a single connection is established (fails or is cancelled).
  When an error is issued, all argument values except param are indeterminate.
 *---------------------------------------------------------------------------*/
typedef void (*DWCMatchedSCCallback)(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int index, void* param);

/*---------------------------------------------------------------------------*
  Callback type for new connection client connection start notification
  Argument: index  Friend roster index for the new connection client
                If the new connection client does not have friends, this is -1.
          param : Parameters for the callback
  Return value: None
  Called when another client begins connection anew
  to an established network during server-client matchmaking
 *---------------------------------------------------------------------------*/
typedef void (*DWCNewClientCallback)(int index, void* param);

/*---------------------------------------------------------------------------*
  Callback type for player evaluation
  Argument: index  Index for the player being evaluated Need to pass the matchmaking key value
                to the function that gets it.
          param : Parameters for the callback
  Return value: Player evaluation value If the value is zero or less, is not considered for matchmaking.
  Called each time a matchmaking target player is found during friend specified/
  unspecified peer matchmaking.
  The player with the larger return value set here will be increasingly likely to be selected as a connection target.
 *---------------------------------------------------------------------------*/
typedef int (*DWCEvalPlayerCallback)(int index, void* param);

/*---------------------------------------------------------------------------*
  server-client matchmaking cancellation complete callback type
  arguments: param: Parameters for the callback
  Return value: None
  When a matchmaking stop has occurred on the server-client matchmaking server,
  this is called when the stop process has completed.
 *---------------------------------------------------------------------------*/
typedef void (*DWCStopSCCallback)(void* param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
// matchmaking option value used during DWC_MATCH_OPTION_MIN_COMPLETE
// matching options
typedef struct DWCstMatchOptMinComplete {
    u8  valid;       // 1: Enable settings, 0: Disable settings
    u8  minEntry;    // Minimum number of matchmaking completions (including self)
    u8  pad[2];      // Padding
    u32 timeout;     // Timeout time to consider matchmaking complete (msec)
} DWCMatchOptMinComplete;


// NAT negotiation callback parameter data structure
typedef struct DWCstNNInfo
{
    u8  isQR2;       // 1: Retry unnecessary because QR2, 0: Retry required because SB
    u8  retryCount;  // Counter to measure retry attempts
    u16 port;        // NAT Negotiation destination port number
    u32 ip;          // NAT Negotiation destination IP
    int cookie;      // NAT Negotiation cookie value. 0 indicates no NN in progress
} DWCNNInfo;

// Matchmaking command control structure
// [todo]
// Both size for data and DWCSBMessage need to be reconsidered
typedef struct DWCstMatchCommandControl {
    u8  command;      // Send command
    u8  count;        // Number of retries
    u16 port;         // Send destination QR2 public port number
    u32 ip;           // Send destination QR2 IP
    u32 data[32];     // Added send data
    int profileID;    // Send destination profile ID
    int len;          // Added send data request count
    OSTick sendTime;  // Send time
} DWCMatchCommandControl;

// Matchmaking control structure
typedef struct DWCstMatchControl
{
    GPConnection* pGpObj;  // Pointer to the gp connection structure pointer
        
    GT2Socket* pGt2Socket; // Pointer to GT2 object pointer
    GT2ConnectionCallbacks* gt2Callbacks;  // Pointer to GT2 callback group
    u8  gt2ConnectCount;   // GT2 retry counter
    u8  gt2NumConnection;  // Number of established GT2 connections
    u8  gt2NumValidConn;   // Number of valid GT2 connections that have completed connection to all members
    u8  pad1;
        
    qr2_t qr2Obj;          // Pointer to QR2 object
    vu8  qr2NNFinishCount; // NAT negotiation completion counter
    vu8  qr2MatchType;     // Matchmaking type DWC_MATCH_TYPE_* enumerator
    vu8  qr2NumEntry;      // The maximum number to be gathered, excluding self
    vu8  qr2IsReserved;    // 1: Completed NN Reservation to self, 0: No reservation
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
    u8   qr2ShutdownFlag;  // 1: Currently reserving QR2 shutdown, 0: No reservation
    u8   pad2;
#else
    u16  pad2;
#endif
    u16  qr2Port;          // Own QR2 public port number
    u32  qr2IP;            // Own QR2 public IP
    volatile int qr2Reservation;    // Profile ID of NN reservation host for self
    u32 qr2IPList[DWC_MAX_CONNECTIONS];    // Connection host QR2 IP address list
    u16 qr2PortList[DWC_MAX_CONNECTIONS];  // Connection host QR2 port number list

    ServerBrowser sbObj;   // Pointer to SB object
//    u32 sbUpdateCount;     // SB server update call interval counter
    BOOL   sbUpdateFlag;   // SB server update call flag
    OSTick sbUpdateTick;   // SB server update call interval time
    int sbPidList[DWC_MAX_CONNECTIONS];  // Connection host profile ID list

    u8  nnRecvCount;       // Count for receiving the same NAT negotiation
    u8  nnFailureCount;    // NN failure count
    u16 nnCookieRand;      // Random number for cookies used in NAT negotiation
    int nnLastCookie;      // Cookie for NN  received the time before
    OSTick nnFailedTime;   // Time when NAT negotiation failed
    OSTick nnFinishTime;   // Time when NAT negotiation terminated
    DWCNNInfo nnInfo;      // NAT negotiation data structure

    DWCMatchState state;   // Matchmaking progress status
    u8  clLinkProgress;    // Frequency of established connections between clients
    u8  friendCount;       // Friend roster check counter for friend specified peer matching
    u8  distantFriend;     // 1: Allows connection by friends of a friend, 2: Such connections are not allowed
    u8  resvWaitCount;     // Reserved command retry count
    u8  closeState;        // Connection close status Specify with DWC_MATCH_CLOSE_STATE_* enumerator
    u8  cancelState;       // Matchmaking cancellation status. Specify with DWC_MATCH_CLOSE_STATE_* enumerator
    u8  scResvRetryCount;  // Reservation retry count for server-client matchmaking clients
    u8  synResendCount;    // SYN packet resend count during matchmaking complete synchronization
    u8  cancelSynResendCount;  // SYN packet resend count during matchmaking complete synchronization
    u8  clWaitTimeoutCount;    // client standby status timeout polling
#ifdef DWC_STOP_SC_SERVER
    u8  stopSCFlag;        // server-client matchmaking stop flag
    u8  pad3;
#endif
    u16 baseLatency;       // Latency with server for client, maximum latency among the clients for the server
    u16 cancelBaseLatency; // Minimum latency between hosts via the GP server using server-client matchmaking cancellation synchronization
    u16 searchPort;        // Search server QR2 public port number
#ifdef DWC_STOP_SC_SERVER
    u16 pad4;
#endif
    u32 searchIP;          // Search server QR2 public IP
#if 0
    u32 cmdResendCount;    // matchmaking command resend counter
    u32 cmdTimeoutCount;   // matchmaking command response timeout counter
#else
    BOOL   cmdResendFlag;  // matchmaking command resend flag
    OSTick cmdResendTick;  // matchmaking command resend time
    u32 cmdTimeoutTime;    // matchmaking command response timeout counter
    OSTick cmdTimeoutStartTick;  // matchmaking command response timeout counter
#endif
    u32 synAckBit;         // SYN-ACK receive AID bitmap for matchmaking complete synchronization
    u32 cancelSynAckBit;   // SYN-ACK receive AID bitmap for matchmaking cancellation synchronization
#ifdef DWC_LIMIT_FRIENDS_MATCH_VALID
    u32 friendAcceptBit;   // new connection friend permission command receive bitmap
#endif
    OSTick lastSynSent;          // Time when final matchmaking completion synchronization signal was sent
    OSTick lastCancelSynSent;    // Time when final matchmaking cancellation synchronization signal was sent
    OSTick closedTime;           // Time when closed by another host
    OSTick clWaitTime;           // time when client standby status begun
    volatile int profileID;      // This players Profile ID
    int reqProfileID;      // Profile ID of partner that send NN request for friend specified peer matching
    int priorProfileID;    // Profile ID of the partner server that sent the priority NN request
    int cbEventPid;        // primarily server-client peer matchmaking completed
                           // Friend's profile ID passed to callback.
                           // Cancel and save in the host profile ID.
                               
    u32 ipList[DWC_MAX_CONNECTIONS];    // Connection host game IP address list
    u16 portList[DWC_MAX_CONNECTIONS];  // Connection host game port number list
    u8  aidList[DWC_MAX_CONNECTIONS];   // Connection host list of unique IDs
    u32 validAidBitmap;                 // Bitmap of valid AIDs for connections completed to all members Used to identify those still matchmaking.
    
    const char* gameName;  // Game name assigned by GameSpy
    const char* secretKey; // Secret key assigned by GameSpy

    const DWCFriendData* friendList;  // Pointer to Friend roster
    int friendListLen;                // Friend roster length
    u8  friendIdxList[DWC_MAX_MATCH_IDX_LIST];  // Matchmaking friend index list
    int friendIdxListLen;  // Matchmaking friend index list length
    u32 svDataBak[DWC_MAX_CONNECTIONS+1];    // Server connection host data backup or data backup when cancelling
                                             // [todo]
                                             // This data does not have many uses, so it will be taken dynamically

    DWCMatchCommandControl cmdCnt;           // Matchmaking command control structure

    DWCMatchedSCCallback matchedCallback;    // dwc_main.c matchmaking completion callback
    void* matchedParam;                      // Parameter for the above callback
    DWCNewClientCallback newClientCallback;  // New connection client notification callback
    void* newClientParam;                    // Parameter for the above callback
    DWCEvalPlayerCallback evalCallback;      // Player evaluation callback
    void* evalParam;                         // Parameter for the above callback
#ifdef DWC_STOP_SC_SERVER
    DWCStopSCCallback stopSCCallback;        // server-client matchmaking cancellation complete callback
    void* stopSCParam;                       // Parameter for the above callback
#endif
} DWCMatchControl;

// SB message header structure
typedef struct DWCstSBMessageHeader {
    char identifier[4];  // Identifier
    u32 version;         // matchmaking protocol version
    u8  command;         // Command type (constant DWC_SB_COMMAND_*)
    u8  size;            // Data size for commands (not including header size)
    u16 qr2Port;         // Own QR2 public port number
    u32 qr2IP;           // Own QR2 public IP
    int profileID;       // This players Profile ID
} DWCSBMessageHeader;

// SB message structure
// Multiple messages can be connected and sent out as one piece of data.
// [todo]
// The size of data must have some limit set along with
// the GP buddy message command
typedef struct DWCstSBMessage {
    DWCSBMessageHeader header;      // SB message header
    u32 data[DWC_MAX_CONNECTIONS];  // Send data
} DWCSBMessage;

// Game definition matchmaking QR2 key data structure
typedef struct DWCstGameMatchKeyData {
    u8  keyID;          // Key ID
    u8  isStr;          // 0: The value is int, 1: The value is char string
    u16 pad;
    char* keyStr;       // Pointer to key character string
    const void* value;  // Pointer to the value
} DWCGameMatchKeyData;

// matchmaking option value used during DWC_MATCH_OPTION_MIN_COMPLETE
// Option value internal structure
typedef struct DWCstMatchOptMinCompleteIn {
    u8  valid;            // 1: Enable settings, 0: Disable settings
    u8  minEntry;         // Minimum number of matchmaking completions (including self)
    u8  retry;            // 1: After the initial polling, 0: Until the initial polling
    u8  pad;
    u32 timeout;          // Timeout time to consider matchmaking complete (msec)
    u32 recvBit;          // Client send status bit for polling
    u32 timeoutBit;       // Timeout status bit for the client
    OSTick startTime;     // matchmaking start time
    OSTick lastPollTime;  // Time when last polling occurred
} DWCMatchOptMinCompleteIn;

// matchmaking option value used during DWC_MATCH_OPTION_SC_CONNECT_BLOCK
// Option value internal structure
typedef struct DWCstMatchOptSCBlock {
    u8  valid;         // 1: Enable settings, 0: Disable settings
    u8  lock;          // 1: Currently denying new connections after connection has completed, 0: Normal operations
    u16 pad;
} DWCMatchOptSCBlock;


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------
#ifdef DWC_QR2_ALIVE_DURING_MATCHING
/*---------------------------------------------------------------------------*
  matchmaking status master server registration function (for friend specified/unspecified peer matchmaking)
  Arguments : None
  return values: TRUE: matchmaking status was registered to the master server,
          FALSE: Status is either pre-login or status could not be registered due to an error
  Use: Registers the matchmaking status of the local host on the master server.
          In about 15 seconds after this function is called, the status of the local host will be visible to other hosts
          and matchmaking can proceed smoothly.
          For matchmaking other than server-client matchmaking, when matchmaking ends,
          the matchmaking status is deleted from the master server.
          If the start matchmaking function has been called, this function will be called inside the library, but
          you should call this function if you want the matchmaking status
          pre-registered with the server so that matching can be ended quickly.
          Note that matchmaking status is also deleted when
          DWC_ShutdownFriendsMatch() has been called.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_RegisterMatchingStatus(void);
#endif


/*---------------------------------------------------------------------------*
  Matchmaking cancellation function
  Arguments : None
  Return value: TRUE: Cancellation executed, FALSE: Matchmaking not in progress
  Application: Cancels matchmaking processes in progress.
          The cancellation completes within this function, and the matchmaking completion callback
          is called.
 *---------------------------------------------------------------------------*/
BOOL DWC_CancelMatching(void);


/*---------------------------------------------------------------------------*
  Matchmaking cancellation function asynchronous version (obsolete function)
  Arguments : None
  Return value: TRUE: Cancellation started, FALSE: Matchmaking not in progress
  Application: Cancels matchmaking processes in progress.
          After performing the necessary cancellation, the matchmaking completion callback
          is called.
          -> Use DWC_CancelMatching()
 *---------------------------------------------------------------------------*/
extern BOOL DWC_CancelMatchingAsync(void);


/*---------------------------------------------------------------------------*
  matchmaking cancellation enabled confirmation function
  Arguments : None
  Return value: TRUE: Cancellation is possible, FALSE: Cannot abort because matchmaking is not in progress
          or not in status that allows cancellation
  Application: Checks whether or not the matchmaking in progress can be cancelled.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_IsValidCancelMatching(void);


#ifdef DWC_STOP_SC_SERVER
/*---------------------------------------------------------------------------*
  server-client matchmaking server stopping function
  arguments: callback: the stop completion callback
          param : Parameters for the callback
  return values: TRUE: matchmaking stop begun. When complete, the callback is returned.
          FALSE: stop failed from an error or because the FriendsMatch Library is non-operational.
  application: stops server-client matchmaking, and prevents any
          subsequent connections from being received.
 *---------------------------------------------------------------------------*/
extern BOOL DWC_StopSCMatchingAsync(DWCStopSCCallback callback, void* param);
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
  Return value: Key ID of the added key. Returns 0 when a key could not be added because
          of a parameter error or the number of keys exceeds the limit
          If called before calling DWC_InitFriendsMatch(), zero is returned.
  Application: Adds a key (int type) for the matchmaking index.
 *---------------------------------------------------------------------------*/
extern u8  DWC_AddMatchKeyInt(u8 keyID,
                              const char* keyString,
                              const int* valueSrc);


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
  Return value: Key ID of the added key. Returns 0 when a key could not be added because
          of a parameter error or the number of keys exceeds the limit
          If called before calling DWC_InitFriendsMatch(), zero is returned.
  Application: Adds a key (char* type) for the matchmaking index.
 *---------------------------------------------------------------------------*/
extern u8  DWC_AddMatchKeyString(u8 keyID,
                                 const char* keyString,
                                 const char* valueSrc);


/*---------------------------------------------------------------------------*
  Get matchmaking index key function int version
  Argument: index: Index for matchmaking candidate player. Passes the index argument
                    from the evaluation callback.
          keyString    Pointer to character string that identifies the key
          idefault  Default value when the player did not have the
                    specified key
  Return value: The value of the specified key possessed by a matchmaking candidate player.
          When there is no key, the value passed to the idefault argument is returned.
  Application: Obtains the value corresponding to the matchmaking index key.
          Use is guaranteed only within the player evaluation callback,
          the return value is indeterminate when call in any other situation.
 *---------------------------------------------------------------------------*/
extern int DWC_GetMatchIntValue(int index, const char* keyString, int idefault);


/*---------------------------------------------------------------------------*
  Get matchmaking index key function string version
  Argument: index: Index for matchmaking candidate player. Passes the index argument
                    from the evaluation callback.
          keyString    Pointer to character string that identifies the key
          sdefault  Default string when the player did not have the
                    specified key
  Return value: The string for the specified key possessed by the matchmaking candidate player.
          When there is no key, the string passed to the sdefault argument is returned.
  Application: Obtains the string corresponding to the matchmaking index key.
          Use is guaranteed only within the player evaluation callback,
          the return value is indeterminate when call in any other situation.
 *---------------------------------------------------------------------------*/
extern const char* DWC_GetMatchStringValue(int index, const char* keyString, const char* sdefault);


/*---------------------------------------------------------------------------*
  Get matchmaking type function
  Arguments : None
  Return value: Last performed matchmaking type. Defined with DWC_MATCH_TYPE_*.
          Returns -1 when offline.
  Application: Obtains the last performed matchmaking type.
 *---------------------------------------------------------------------------*/
extern int DWC_GetLastMatchingType(void);


/*---------------------------------------------------------------------------*
  Matchmaking option set function
  Arguments: opttype:  Matchmaking option type
          optval:  Pointer to the matchmaking option value.
                  Passes the pointer to the type corresponding to opttype.
          optlen  Only when the type is an array, the array size must be passed.
  Return value: DWC_SET_MATCH_OPT_RESULT_* type enumerator
  Application: Sets various options for matchmaking.
 *---------------------------------------------------------------------------*/
extern int DWC_SetMatchingOption(DWCMatchOptType opttype, const void* optval, int optlen);


/*---------------------------------------------------------------------------*
  matchmaking options get function
  Arguments: opttype:  Matchmaking option type
          optval:  Pointer to location where matchmaking options are stored.
                  Passes the pointer to the type corresponding to opttype.
          optlen  Location where the size of the obtained value for the option is stored
  Return value: DWC_SET_MATCH_OPT_RESULT_* type enumerator
  Application: Gets the options being set for matchmaking.
          When the specified options are not set, nothing is done
          and zero is stored in the optlen argument.
 *---------------------------------------------------------------------------*/
extern int DWC_GetMatchingOption(DWCMatchOptType opttype, void* optval, int* optlen);


/*---------------------------------------------------------------------------*
  Completion option for matchmaking less than the specified number/get status function
  Arguments: time:  Pointer to location for time elapsed from start of matchmaking (in msec).
               If the time is unnecessary, pass NULL.
  Return value: 0: Within the timeout time period
          1: Timeout period expired; possible to complete matchmaking with fewer than the specified number
          -1: This matchmaking option is not set or is OFF.
  Application: Obtains the time period for the matchmaking completion option with fewer than the specified number of participants.
 *---------------------------------------------------------------------------*/
extern int DWC_GetMOMinCompState(u64* time);


/*---------------------------------------------------------------------------*
  Function for getting the matchmaking block option status
  Arguments : None
  Return values: TRUE: Currently blocking newly connected clients,
          FALSE: Not currently blocking or the option has not been set
  Use: Gets the blocking status of the matchmaking block option
 *---------------------------------------------------------------------------*/
extern BOOL DWC_GetMOSCConnectBlockState(void);


/*---------------------------------------------------------------------------*
  Function for releasing matchmaking block
  Arguments : None
  Return value: None
  Use: Releases the blocking status of the matchmaking block option
 *---------------------------------------------------------------------------*/
extern void DWC_ClearMOSCConnectBlock(void);


/*---------------------------------------------------------------------------*
  matchmaking internal status get function
  Arguments : None
  Return value: current internal state for matchmaking
  Application: Gets the internal state of the matchmaking in progress. Used only to find out the internal state
          during matchmaking for debugging purposes.
 *---------------------------------------------------------------------------*/
extern DWCMatchState DWC_GetMatchingState(void);


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
extern void DWCi_MatchInit(DWCMatchControl* matchcnt,
                           GPConnection pGpObj,
                           GT2Socket* pGt2Socket,
                           GT2ConnectionCallbacks* gt2Callbacks,
                           const char* gameName,
                           const char* secretKey,
                           const DWCFriendData friendList[],
                           int  friendListLen);


extern qr2_error_t DWCi_QR2Startup(int profileID);


extern void DWCi_ConnectToAnybodyAsync(u8  numEntry,
                                       const char* addFilter,
                                       DWCMatchedSCCallback matchedCallback,
                                       void* matchedParam,
                                       DWCEvalPlayerCallback evalCallback,
                                       void* evalParam);


extern void DWCi_ConnectToFriendsAsync(const u8 friendIdxList[],
                                       int friendIdxListLen,
                                       u8  numEntry,
                                       BOOL distantFriend,
                                       DWCMatchedSCCallback matchedCallback,
                                       void* matchedParam,
                                       DWCEvalPlayerCallback evalCallback,
                                       void* evalParam);


// [arakit] main 051027
extern void DWCi_SetupGameServer(u8  maxEntry,
                                 DWCMatchedSCCallback matchedCallback,
                                 void* matchedParam,
                                 DWCNewClientCallback newClientCallback,
                                 void* newClientParam);


extern void DWCi_ConnectToGameServerAsync(int serverPid,
                                          DWCMatchedSCCallback matchedCallback,
                                          void* matchedParam,
                                          DWCNewClientCallback newClientCallback,
                                          void* newClientParam);


extern void DWCi_MatchProcess(BOOL fullSpec);


extern void DWCi_GT2ConnectAttemptCallback(GT2Socket socket,
                                           GT2Connection connection,
                                           unsigned int ip, unsigned
                                           short port, int latency,
                                           GT2Byte* message, int len);


extern GT2Bool DWCi_GT2UnrecognizedMessageCallback(GT2Socket socket,
                                                   unsigned int ip,
                                                   unsigned short port,
                                                   GT2Byte* message,
                                                   int len);


extern void DWCi_GT2ConnectedCallback(GT2Connection connection,
                                      GT2Result result,
                                      GT2Byte* message,
                                      int len);


extern void DWCi_MatchGPRecvBuddyMsgCallback(GPConnection* pconnection,
                                             int profileID,
                                             char* message);


extern void DWCi_StopMatching(DWCError error, int errorCode);


extern void DWCi_ClearQR2Key(void);


extern void DWCi_ProcessMatchSynPacket(u8 aid, u16 type, u8* data);


extern BOOL DWCi_ProcessMatchClosing(DWCError error, int errorCode, int profileID);


extern void DWCi_ProcessMatchSCClosing(int clientPid);


extern BOOL DWCi_DeleteHostByProfileID(int profileID, int numHost);


extern int  DWCi_DeleteHostByIndex(int index, int numHost);


extern int  DWCi_GetNumAllConnection(void);


extern int  DWCi_GetNumValidConnection(void);


extern void DWCi_SetNumValidConnection(void);


extern int  DWCi_GetAllAIDList(u8** aidList);


extern int  DWCi_GetValidAIDList(u8** aidList);


extern GPResult DWCi_GPSetServerStatus(void);


extern void DWCi_ShutdownMatch(void);


extern BOOL DWCi_IsShutdownMatch(void);


extern int  DWCi_GetNumValidConnection(void);


#ifdef __cplusplus
}
#endif

#endif // DWC_MATCH_H_
