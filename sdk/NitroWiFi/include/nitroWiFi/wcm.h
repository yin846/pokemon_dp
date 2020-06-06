/*---------------------------------------------------------------------------*
Project: NitroWiFi - WCM - include
File: wcm.h

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: wcm.h,v $
Revision 1.13  03/10/2006 09:20:22  kitase_hirotake
  INDENT SOURCE

Revision 1.12  09/17/2005 10:08:19  terui
added the WCM_CAM_LIFETIME constant

Revision 1.11  09/12/2005 10:00:27  terui
added the WCM_CAMOUFLAGE_RATESET constant

Revision 1.10  09/10/2005 04:09:21  terui
changed the utility function group from inline to actual and saved to a separate file

Revision 1.9  09/09/2005 00:55:39  terui
added an inline function to detect wireless network characteristics

Revision 1.8  09/01/2005 13:03:01  terui
expanded WCM_WORK_SIZE as part of the work to add necessary buffer for Keep Alive

Revision 1.7  08/08/2005 06:13:42  terui
revised to add fixed values of 1M/2M for the supported rate set when connecting to AP

Revision 1.6  07/19/2005 13:04:08  terui
expanded WCM_APLIST_SIZE from 64 to 192

Revision 1.5  07/18/2005 02:27:55  terui
Fix comment.

Revision 1.4  07/15/2005 11:33:04  terui
added definitions for WCM_BeginSearchAsync and WCM_EndSearchAsync functions

Revision 1.3  07/12/2005 06:25:52  terui
added WCM_SetChannelScanTime function

Revision 1.2  07/11/2005 12:04:48  terui
deleted unnecessary members from within the WCMWepDesc structure

Revision 1.1  07/07/2005 10:40:44  terui
new additions

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_WCM_H_
#define NITROWIFI_WCM_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/
#include <nitro/types.h>
#include <nitro/wm.h>

/*---------------------------------------------------------------------------*
constant definitions
 *---------------------------------------------------------------------------*/

// work buffer size needed for the WCM library
#define WCM_WORK_SIZE   8960

// process results code
#define WCM_RESULT_SUCCESS          0               // Succeeded
#define WCM_RESULT_FAILURE          1               // Failed
#define WCM_RESULT_PROGRESS         2               // asynchronous process already running
#define WCM_RESULT_ACCEPT           3               // accept asynchronous process request (always generates a callback)
#define WCM_RESULT_REJECT           4               // deny asynchronous process request (failure in communication of request to ARM7)
#define WCM_RESULT_WMDISABLE        5               // WM library not running on ARM7
#define WCM_RESULT_NOT_ENOUGH_MEM   6               // insufficient work buffer size
#define WCM_RESULT_FATAL_ERROR      7               // unrecoverable error

// library internal phase
#define WCM_PHASE_NULL              0               // pre-initialization
#define WCM_PHASE_WAIT              1               // status immediately after initialization (waiting for request)
#define WCM_PHASE_WAIT_TO_IDLE      2               // wireless functionality launch sequence underway after status immediately after initialization
#define WCM_PHASE_IDLE              3               // wireless functionality idling
#define WCM_PHASE_IDLE_TO_WAIT      4               // wireless functionality halt sequence underway after idling
#define WCM_PHASE_IDLE_TO_SEARCH    5               // transfer sequence to AP auto-detect state underway after idling
#define WCM_PHASE_SEARCH            6               // AP auto-detect state
#define WCM_PHASE_SEARCH_TO_IDLE    7               // transfer sequence to idling underway after AP auto-detect state
#define WCM_PHASE_IDLE_TO_DCF       8               // AP wireless connection sequence underway after idling
#define WCM_PHASE_DCF               9               // DCF communication possible for wireless connection with AP
#define WCM_PHASE_DCF_TO_IDLE       10              // wireless connection interrupt sequence underway after DCF communication state
#define WCM_PHASE_FATAL_ERROR       11              // state where an unrecoverable error has occurred and no processes can be accepted
#define WCM_PHASE_IRREGULAR         12              // state where problem occurred during status transition sequence
#define WCM_PHASE_TERMINATING       13              // WCM library forced termination sequence underway

// notification types
#define WCM_NOTIFY_COMMON           0               // default notification types
#define WCM_NOTIFY_STARTUP          1               // WCM_StartupAsync function results notification types
#define WCM_NOTIFY_CLEANUP          2               // WCM_CleanupAsync function results notification types
#define WCM_NOTIFY_BEGIN_SEARCH     3               // WCM_SearchAsync and WCM_BeginSearchAsync function search start process results notification types
#define WCM_NOTIFY_END_SEARCH       4               // WCM_SearchAsync and WCM_EndSearchAsync function search halt process results notification types
#define WCM_NOTIFY_CONNECT          5               // WCM_ConnectAsync function results notification types
#define WCM_NOTIFY_DISCONNECT       6               // WCM_DisconnectAsync function results notification types, AP disconnect notification types
#define WCM_NOTIFY_FOUND_AP         7               // AP discovery during auto-search process notification types
#define WCM_NOTIFY_SEARCH_AROUND    8               // completed auto-search process notification types (when setting options)
#define WCM_NOTIFY_TERMINATE        9               // WCM_TerminateAsync function results notification types

// AP auto-search results standby buffer lock types
#define WCM_APLIST_UNLOCK   0                       // opened - the found AP data was originally appended to the list
#define WCM_APLIST_LOCK     1                       // locked - the found AP data is not reflected in the list

// the types of operations when the AP auto-search results standby buffer is full
#define WCM_APLIST_MODE_IGNORE      0               // ignore - the found AP data is ignored
#define WCM_APLIST_MODE_EXCHANGE    1               // exchanged - data is exchanged with the oldest AP data

// WEP encryption types
#define WCM_WEPMODE_NONE    0                       // no encryption
#define WCM_WEPMODE_40      1                       // RC4 (40-bit) encryption mode
#define WCM_WEPMODE_104     2                       // RC4 (104-bit) encryption mode
#define WCM_WEPMODE_128     3                       // RC4 (128-bit) encryption mode

// types of control constants
#define WCM_CAM_LIFETIME            80              // CAM lifetime (8s)
#define WCM_BSSID_SIZE              WM_SIZE_BSSID   // BDD-ID size (6 bytes)
#define WCM_ESSID_SIZE              WM_SIZE_SSID    // ESS-ID maximum size (32 bytes)
#define WCM_APLIST_SIZE             192             // data volume size for the AP data block
#define WCM_APLIST_EX_SIZE          16              // control volume size for the AP data block
#define WCM_APLIST_BLOCK_SIZE       (WCM_APLIST_EX_SIZE + WCM_APLIST_SIZE)
#define WCM_WEP_SIZE                WM_SIZE_WEPKEY  // 80 bytes
#define WCM_WEP_EX_SIZE             (1 + 1 + 14)    // WCMWepDesc.mode + .keyId + .reserved
#define WCM_WEP_STRUCT_SIZE         (WCM_WEP_SIZE + WCM_WEP_EX_SIZE)    // == sizeof( WCMWepDesc )
#define WCM_DCF_RECV_EXCESS_SIZE    (sizeof(WMDcfRecvBuf) - 4)          // 44 bytes
#define WCM_DCF_SEND_EXCESS_SIZE    0   // header storage space for the send buffer is not needed
#define WCM_DCF_RECV_SIZE           (WM_DCF_MAX_SIZE + WCM_DCF_RECV_EXCESS_SIZE)
#define WCM_DCF_RECV_BUF_SIZE       (WCM_ROUNDUP32(WCM_DCF_RECV_SIZE))  // the necessary buffer size for a single DCF receipt
#define WCM_DCF_SEND_BUF_SIZE       (WCM_ROUNDUP32(WM_DCF_MAX_SIZE))    // the necessary buffer size for a single DCF send
#define WCM_ETC_BUF_SIZE            108     // sizes for the various control variable groups
#define WCM_ADDITIONAL_RATESET      0x0003  // added as fixed values for the support rate set (1M or 2M)
#define WCM_CAMOUFLAGE_RATESET      0x0024  // dummy additions to the support rate set (5.5M and 11M)

/*
* The support rate set is specified in the format of a 16-bit bit field.
* The correspondence between each bit and rate, and the actual value stored in the element is as indicated below:
                                     *
*   1M   2M 5.5M   6M   9M  11M  12M  18M  24M  36M  48M  54M
                                     * {  2,   4,  11,  12,  18,  22,  24,  36,  48,  72,  96, 108, 0, 0, 0, 0 }
                                     */
#define WCM_AID_MIN 1                       // minimum Auth-ID
#define WCM_AID_MAX 2007                    // maximum Auth-ID
extern const u8 WCM_Bssid_Any[WCM_BSSID_SIZE];
extern const u8 WCM_Essid_Any[WCM_ESSID_SIZE];

// constant indicating the optional BSS-ID
#define WCM_BSSID_ANY   ((void*)WCM_Bssid_Any)

// constant indicating the optional ESS-ID
#define WCM_ESSID_ANY   ((void*)WCM_Essid_Any)

/*-------- options bit layout definitions --------*/

// channel during scan
#define WCM_OPTION_TEST_CHANNEL     0x00008000
#define WCM_OPTION_FILTER_CHANNEL   0x00003ffe
#define WCM_OPTION_MASK_CHANNEL     (WCM_OPTION_TEST_CHANNEL | WCM_OPTION_FILTER_CHANNEL)
#define WCM_OPTION_CHANNEL_1        (WCM_OPTION_TEST_CHANNEL | 0x00000002)
#define WCM_OPTION_CHANNEL_2        (WCM_OPTION_TEST_CHANNEL | 0x00000004)
#define WCM_OPTION_CHANNEL_3        (WCM_OPTION_TEST_CHANNEL | 0x00000008)
#define WCM_OPTION_CHANNEL_4        (WCM_OPTION_TEST_CHANNEL | 0x00000010)
#define WCM_OPTION_CHANNEL_5        (WCM_OPTION_TEST_CHANNEL | 0x00000020)
#define WCM_OPTION_CHANNEL_6        (WCM_OPTION_TEST_CHANNEL | 0x00000040)
#define WCM_OPTION_CHANNEL_7        (WCM_OPTION_TEST_CHANNEL | 0x00000080)
#define WCM_OPTION_CHANNEL_8        (WCM_OPTION_TEST_CHANNEL | 0x00000100)
#define WCM_OPTION_CHANNEL_9        (WCM_OPTION_TEST_CHANNEL | 0x00000200)
#define WCM_OPTION_CHANNEL_10       (WCM_OPTION_TEST_CHANNEL | 0x00000400)
#define WCM_OPTION_CHANNEL_11       (WCM_OPTION_TEST_CHANNEL | 0x00000800)
#define WCM_OPTION_CHANNEL_12       (WCM_OPTION_TEST_CHANNEL | 0x00001000)
#define WCM_OPTION_CHANNEL_13       (WCM_OPTION_TEST_CHANNEL | 0x00002000)
#define WCM_OPTION_CHANNEL_ALL      (WCM_OPTION_TEST_CHANNEL | WCM_OPTION_FILTER_CHANNEL)
#define WCM_OPTION_CHANNEL_RDC      (WCM_OPTION_CHANNEL_1 | WCM_OPTION_CHANNEL_7 | WCM_OPTION_CHANNEL_13)

// power saving settings for DCF communications
#define WCM_OPTION_TEST_POWER   0x00020000
#define WCM_OPTION_FILTER_POWER 0x00010000
#define WCM_OPTION_MASK_POWER   (WCM_OPTION_TEST_POWER | WCM_OPTION_FILTER_POWER)
#define WCM_OPTION_POWER_SAVE   (WCM_OPTION_TEST_POWER | 0x00000000)
#define WCM_OPTION_POWER_ACTIVE (WCM_OPTION_TEST_POWER | 0x00010000)

// verified mode for AP
#define WCM_OPTION_TEST_AUTH        0x00080000
#define WCM_OPTION_FILTER_AUTH      0x00040000
#define WCM_OPTION_MASK_AUTH        (WCM_OPTION_TEST_AUTH | WCM_OPTION_FILTER_AUTH)
#define WCM_OPTION_AUTH_OPENSYSTEM  (WCM_OPTION_TEST_AUTH | 0x00000000)
#define WCM_OPTION_AUTH_SHAREDKEY   (WCM_OPTION_TEST_AUTH | 0x00040000)

// scan types
#define WCM_OPTION_TEST_SCANTYPE    0x00200000
#define WCM_OPTION_FILTER_SCANTYPE  0x00100000
#define WCM_OPTION_MASK_SCANTYPE    (WCM_OPTION_TEST_SCANTYPE | WCM_OPTION_FILTER_SCANTYPE)
#define WCM_OPTION_SCANTYPE_PASSIVE (WCM_OPTION_TEST_SCANTYPE | 0x00000000)
#define WCM_OPTION_SCANTYPE_ACTIVE  (WCM_OPTION_TEST_SCANTYPE | 0x00100000)

// notification on/off setting for scan cycling
#define WCM_OPTION_TEST_ROUNDSCAN   0x00800000
#define WCM_OPTION_FILTER_ROUNDSCAN 0x00400000
#define WCM_OPTION_MASK_ROUNDSCAN   (WCM_OPTION_TEST_ROUNDSCAN | WCM_OPTION_FILTER_ROUNDSCAN)
#define WCM_OPTION_ROUNDSCAN_IGNORE (WCM_OPTION_TEST_ROUNDSCAN | 0x00000000)
#define WCM_OPTION_ROUNDSCAN_NOTIFY (WCM_OPTION_TEST_ROUNDSCAN | 0x00400000)

/*---------------------------------------------------------------------------*
macro definitions
 *---------------------------------------------------------------------------*/
#define WCM_ROUNDUP32(_x_)      (((_x_) + 0x01f) &~(0x01f))
#define WCM_ROUNDDOWN32(_x_)    ((_x_) &~(0x01f))
#define WCM_ROUNDUP4(_x_)       (((_x_) + 0x03) &~(0x03))
#define WCM_ROUNDDOWN4(_x_)     ((_x_) &~(0x03))

/*---------------------------------------------------------------------------*
structure definitions
 *---------------------------------------------------------------------------*/

// WCM library operation settings
typedef struct WCMConfig
{
    s32     dmano;      // DMA number (0 to 3) used by WM
    void*   pbdbuffer;  // AP data standby volume address
    s32     nbdbuffer;  // AP data standby volume size (in bytes)
    s32     nbdmode;    // AP data standby method ( WCM_APLIST_MODE_* )
} WCMConfig;

// detailed parameter union within WCM library notifications
typedef union WCMNoticeParameter
{
    s32     n;          // integer type
    void*   p;          // pointer type
} WCMNoticeParameter;

// structure passed to WCM library notification callback
typedef struct WCMNotice
{
    s16 notify; // notification types ( WCM_NOTIFY_* )
    s16 result; // process results ( WCM_RESULT_* )
    WCMNoticeParameter  parameter[3];   // detailed parameter types
} WCMNotice;

// type definition, WCM notification callback function
typedef void (*WCMNotify) (WCMNotice * arg);

// WEP key setting structure
typedef struct WCMWepDesc
{
    u8  mode;                   // WEP encryption mode ( WCM_WEPMODE_* )
    u8  keyId;                  // WEP key ID (0 to 3)
    u8  key[WM_SIZE_WEPKEY];    // WEP key data string [20 * 4 bytes]
} WCMWepDesc;

/*---------------------------------------------------------------------------*
function definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
Name: WCM_Init

Description: Initializes the WCM library. Synchronous function.

Arguments: buf     -   Specifies the pointer to the work buffer used
within the WCM library. After initialization completes, the buffer should not be used
for any other purposes until the WCM_Finish function
closes the WCM library.
len     -   Specifies the size of the work buffer. Specifying a size smaller
than that in WCM_WORK_SIZE will cause this function to fail.

Returns: s32     -   Returns the process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_NOT_ENOUGH_MEM
 *---------------------------------------------------------------------------*/
s32         WCM_Init(void* buf, s32 len);

/*---------------------------------------------------------------------------*
Name: WCM_Finish

Description: Closes the WCM library. Synchronous function. The work buffer specified by the
WCM_Init function is released.

Arguments: None.

Returns: s32     -   Returns the process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32         WCM_Finish(void);

/*---------------------------------------------------------------------------*
Name: WCM_StartupAsync

Description: Begins the wireless functionality launch sequence.
Internally a phase transfer process from WAIT to IDLE is performed.
An asynchronous function that notifies asynchronous process results
from the callback function specified by notify when the synchronous
returned value is WCM_RESULT_ACCEPT.
The asynchronous process results notification type is WCM_NOTIFY_STARTUP.

Arguments: config  -   Specifies a pointer to the structure for WCM operation settings.
notify  -   Specifies callback function that notifies asynchronous process results.
This callback function is used jointly with results notifications
for subsequent asynchronous functions.

Returns: s32     -   Returns the synchronous process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_PROGRESS , WCM_RESULT_WMDISABLE ,
WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32         WCM_StartupAsync(WCMConfig* config, WCMNotify notify);

/*---------------------------------------------------------------------------*
Name: WCM_CleanupAsync

Description: Begins the wireless functionality halt sequence.
Internally a phase transfer process from IDLE to WAIT is performed.
An asynchronous function that notifies asynchronous process results
from the callback function specified by the WCM_StartupAsync function when
the synchronous returned value is WCM_RESULT_ACCEPT.
The asynchronous process results notification type is WCM_NOTIFY_CLEANUP.

Arguments: None.

Returns: s32     -   Returns the synchronous process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32         WCM_CleanupAsync(void);

/*---------------------------------------------------------------------------*
Name: WCM_SearchAsync

Description: Begins the AP auto-search start or stop sequence.
This function uses parameters to differentiate between calls to the
WCM_BeginSearchAsync and WCM_EndSearchAsync functions.

Arguments: bssid   -   Specifies the BSSID for the AP to be searched. When a NULL is specified,
it is taken to indicate a halt for the search.
essid   -   Specifies the ESSID for the AP to be searched. When a NULL is specified,
it is taken to indicate a halt for the search.
option  -   Specifies the 32-bit variable for changing options
in conjunction with the options setting format. When 0 is specified,
no changes will be made.

Returns: s32     -   Returns the synchronous process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32         WCM_SearchAsync(void* bssid, void* essid, u32 option);

/*---------------------------------------------------------------------------*
Name: WCM_BeginSearchAsync

Description: Begins the AP auto-search start sequence. When auto-search is already activated,
only changes to search parameters can be performed.
Internally a phase transfer process from IDLE to SEARCH is performed.
An asynchronous function that notifies asynchronous process results
from the callback function specified by the WCM_StartupAsync function when
the synchronous returned value is WCM_RESULT_ACCEPT.
The asynchronous process results notification type is WCM_NOTIFY_BEGIN_SEARCH.

Arguments: bssid   -   Specifies the BSSID for the AP to be searched. When NULL or
WCM_BSSID_ANY is specified, the AP with the optional
ESSID is searched.
essid   -   Specifies the ESSID for the AP to be searched. When NULL or
WCM_ESSID_ANY is specified, the AP with the optional
ESSID is searched.
option  -   Specifies the 32-bit variable for changing options
in conjunction with the options setting format. When 0 is specified,
no changes will be made.

Returns: s32     -   Returns the synchronous process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32         WCM_BeginSearchAsync(void* bssid, void* essid, u32 option);

/*---------------------------------------------------------------------------*
Name: WCM_EndSearchAsync

Description: Begins the AP auto-search end sequence.
Internally a phase transfer process from SEARCH to IDLE is performed.
An asynchronous function that notifies asynchronous process results
from the callback function specified by the WCM_StartupAsync function when
the synchronous returned value is WCM_RESULT_ACCEPT.
The asynchronous process results notification type is WCM_NOTIFY_END_SEARCH.

Arguments: None.

Returns: s32     -   Returns the synchronous process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_PROGRESS , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32         WCM_EndSearchAsync(void);

/*---------------------------------------------------------------------------*
Name: WCM_ConnectAsync

Description: Begins the AP wireless connection sequence.
Internally a phase transfer process from IDLE to DCF is performed.
An asynchronous function that notifies asynchronous process results
from the callback function specified by the WCM_StartupAsync function when
the synchronous returned value is WCM_RESULT_ACCEPT.
The asynchronous process results notification type is WCM_NOTIFY_CONNECT.

Arguments: bssDesc -   Specifies the wireless network data for the AP connection.
The assumption is that the data obtained in WCM_SearchAsync
will be specified as is.
wepDesc -   Specifies the encryption settings data for the WCMWepDesc-type
of the ESS-ID. When NULL, the setting is for no WEP encryption.
option  -   Specifies the 32-bit variable for changing options
in conjunction with the options setting format. When 0 is specified,
no changes will be made.

Returns: s32     -   Returns the synchronous process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32         WCM_ConnectAsync(void* bssDesc, void* wepDesc, u32 option);

/*---------------------------------------------------------------------------*
Name: WCM_DisconnectAsync

Description: Begins the AP wireless disconnect sequence.
Internally a phase transfer process from DCF to IDLE is performed.
An asynchronous function that notifies asynchronous process results
from the callback function specified by the WCM_StartupAsync function when
the synchronous returned value is WCM_RESULT_ACCEPT.
The asynchronous process results notification type is WCM_NOTIFY_DISCONNECT.

Arguments: None.

Returns: s32     -   Returns the synchronous process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32         WCM_DisconnectAsync(void);

/*---------------------------------------------------------------------------*
Name: WCM_TerminateAsync

Description: Begins the wireless functionality termination sequence.
May be run from any phase as long as no other asynchronous process
is running; internally a phase transfer process to WAIT is performed.
An asynchronous function that notifies asynchronous process results
from the callback function specified by the WCM_StartupAsync function when
the synchronous returned value is WCM_RESULT_ACCEPT.
The asynchronous process results notification type is WCM_NOTIFY_TERMINATE.

Arguments: None.

Returns: s32     -   Returns the synchronous process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE ,
WCM_RESULT_PROGRESS , WCM_RESULT_REJECT ,
WCM_RESULT_FATAL_ERROR , WCM_RESULT_ACCEPT
 *---------------------------------------------------------------------------*/
s32         WCM_TerminateAsync(void);

/*---------------------------------------------------------------------------*
Name: WCM_GetPhase

Description: Obtains the WCM library's internal state (phase). Synchronous function.

Arguments: None.

Returns: s32     -   Returns the current WCM library phase.
 *---------------------------------------------------------------------------*/
s32         WCM_GetPhase(void);

/*---------------------------------------------------------------------------*
Name: WCM_UpdateOption

Description: Updates the WCM library options settings.

Arguments: option  -   Specifies the 32-bit variable for changing options
in conjunction with the options setting format.
No updates are performed when 0 is specified.

Returns: u32     -   Returns the options variable prior to changing.
 *---------------------------------------------------------------------------*/
u32         WCM_UpdateOption(u32 option);

/*---------------------------------------------------------------------------*
Name: WCM_SetChannelScanTime

Description: Sets the time for a scan in a single channel during an AP auto-search.

Arguments: msec    -   Specifies the time to scan a single channel in milliseconds.
Values between 10 and 1000 are accepted; if other values
are specified, the default search time for auto-search
will be used.

Returns: None.
 *---------------------------------------------------------------------------*/
void        WCM_SetChannelScanTime(u16 msec);

/*---------------------------------------------------------------------------*
Name: WCM_LockApList

Description: Changes the lock status of the AP data storage list.
NOTICE: When locked, the contents of the AP data storage list are not changed,
even when an AP is found via an interrupt; be aware that the found AP data is abandoned.

Arguments: lock    -   WCM_APLIST_LOCK     : Locks the list.
WCM_APLIST_UNLOCK   : Unlocks the list.
other               : Recognized as WCM_APLIST_LOCK.

Returns: s32     -   Returns the lock status prior to the call.
WCM_APLIST_UNLOCK   : Locked.
WCM_APLIST_UNLOCK   : Unlocked.
 *---------------------------------------------------------------------------*/
s32         WCM_LockApList(s32 lock);

/*---------------------------------------------------------------------------*
Name: WCM_ClearApList

Description: Deletes the content of the AP data storage list, returning it to its initial state.
Clears the data storage area regardless of the lock status.

Arguments: None.

Returns: None.
 *---------------------------------------------------------------------------*/
void        WCM_ClearApList(void);

/*---------------------------------------------------------------------------*
Name: WCM_CountApList

Description: Gets the number of AP data management blocks stored in the
AP data storage list.
NOTICE: Be aware that the number of blocks can change if an interrupt occurs
after the function is called if the list is not locked with the WCM_LockApList function.

Arguments: None.

Returns: s32     -   Returns the number of AP data management blocks.
If the number of blocks is not normally available, 0 is returned.
 *---------------------------------------------------------------------------*/
s32         WCM_CountApList(void);

/*---------------------------------------------------------------------------*
Name: WCM_PointApList

Description: Extracts the frequency strength registered for the AP data
within the AP data storage list.
NOTICE: Be aware that it is possible the buffer contents for the indicated
pointer can be overwritten if an interrupt occurs after the function is called
if the list is not locked with the WCM_LockApList function.

Arguments: index       -   Specifies the index. It is necessary to specify an index smaller
than the number to be obtained via the WCM_CountApList function.

Returns: WMBssDesc*  -   Returns the pointer to the extracted AP data.
Returns NULL if extraction fails.
 *---------------------------------------------------------------------------*/
WMBssDesc*  WCM_PointApList(s32 index);

/*---------------------------------------------------------------------------*
Name: WCM_PointApListLinkLevel

Description: Extracts the frequency strength registered for the AP data
assigned to the specified index from within the AP data storage list.

Arguments: index       -   Specifies the index. It is necessary to specify an index smaller
than the number to be obtained via the WCM_CountApList function.

Returns: WMLinkLevel -   Returns the frequency strength for the extracted AP data at registration.
Returns WM_LINK_LEVEL_0 if extraction fails.
 *---------------------------------------------------------------------------*/
WMLinkLevel WCM_PointApListLinkLevel(s32 index);

/*---------------------------------------------------------------------------*
Name: WCM_CompareBssID

Description: Compares two BSSIDs. Synchronous function.

Arguments: a       -   Specifies a pointer to a BSSID for comparison.
b       -   Specifies a pointer to the other BSSID for comparison.

Returns: BOOL    -   Returns TRUE if the two BSSIDs are identical.
If the BSSIDs differ, returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL        WCM_CompareBssID(u8* a, u8* b);

/*---------------------------------------------------------------------------*
Name: WCM_GetLinkLevel

Description: Obtains the link strength at DCF communication. Synchronous function.

Arguments: None.

Returns: WMLinkLevel -   Returns at link strength at one of four evaluated levels.
 *---------------------------------------------------------------------------*/
WMLinkLevel WCM_GetLinkLevel(void);

/*---------------------------------------------------------------------------*
Name: WCM_IsInfrastructureNetwork

Description: Checks whether the data obtained as AP search results
are those for an infrastructure network.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for an infrastructure network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL        WCM_IsInfrastructureNetwork(WMBssDesc* bssDesc);

/*---------------------------------------------------------------------------*
Name: WCM_IsAdhocNetwork

Description: Checks whether the data obtained as AP search results
are those for an ad-hoc network.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for an ad-hoc network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL        WCM_IsAdhocNetwork(WMBssDesc* bssDesc);

/*---------------------------------------------------------------------------*
Name: WCM_IsPrivacyNetwork

Description: Checks whether the data obtained as AP search results are from
a network with long preamble settings applied.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for a WEP security network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL        WCM_IsPrivacyNetwork(WMBssDesc* bssDesc);

/*---------------------------------------------------------------------------*
Name: WCM_IsShortPreambleNetwork

Description: Checks whether the data obtained as AP search results are from
a network with long preamble settings applied.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for a short preamble network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL        WCM_IsShortPreambleNetwork(WMBssDesc* bssDesc);

/*---------------------------------------------------------------------------*
Name: WCM_IsLongPreambleNetwork

Description: Checks whether the data obtained as AP search results are from
a network with long preamble settings applied.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the data is for a long preamble network;
otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL        WCM_IsLongPreambleNetwork(WMBssDesc* bssDesc);

/*---------------------------------------------------------------------------*
Name: WCM_IsEssidHidden

Description: Checks whether the ESS-ID for the data obtained as AP search results
is hidden.

Arguments: bssDesc -   The AP data (network data) to be checked.

Returns: BOOL    -   Returns TRUE if the ESS-ID is hidden.
Returns FALSE if a valid ESS-ID is published.
 *---------------------------------------------------------------------------*/
BOOL        WCM_IsEssidHidden(WMBssDesc* bssDesc);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* NITROWIFI_WCM_H_ */

/*---------------------------------------------------------------------------*
End of file
 *---------------------------------------------------------------------------*/
