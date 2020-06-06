/*---------------------------------------------------------------------------*
Project: NitroWiFi - WCM - libraries
File: system.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: system.c,v $
Revision 1.17  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.16  11/08/2005 01:08:02  seiki_masashi
Support for changes in NITRO-SDK 3.0RC

Revision 1.15  11/01/2005 12:08:26  seiki_masashi
Measures for when SDK_ASSERT_ON_COMPILE is defined

Revision 1.14  10/05/2005 08:49:50  terui
Added function for backing up electromagnetic signal intensity and conditions of DCF data reception.

Revision 1.13  09/17/2005 10:09:08  terui
Add functionality to time-out in CAM lifetime.

Revision 1.12  09/16/2005 04:50:10  terui
strategy to fool the WCM_ConnectAsync function with WEP key data of all zeros if there is no encryption in WEP mode

Revision 1.11  09/16/2005 04:23:14  terui
fixed an issue where the state reverted to a temporary idle during a retry after a failed connection resulting from a rate set mismatch

Revision 1.10  09/12/2005 10:02:13  terui
added measures for dealing with a failed connection from a rate set mismatch

Revision 1.9  09/10/2005 03:08:31  terui
Added a check for a main unit blocking wireless communication.
Removed utility functions to a separate file.

Revision 1.8  09/01/2005 13:08:14  terui
added function to get timing from an alarm and send the Keep Alive packet
added functionality to communicate the wlStatus field returned from WM after a failed connect to the top

Revision 1.7  08/09/2005 07:58:30  terui
adjusted the internal buffer that empties the WEP key for a 32-byte alignment

Revision 1.6  08/08/2005 06:13:42  terui
revised to add fixed values of 1M/2M for the supported rate set when connecting to AP

Revision 1.5  07/18/2005 01:34:49  terui
changing the setting location for the types of notifications when DCF communication is terminated

Revision 1.4  07/15/2005 11:32:10  terui
Broke the WCM_SearchAsync function into start and stop functions.

Revision 1.3  07/12/2005 06:25:30  terui
added WCM_SetChannelScanTime function

Revision 1.2  07/11/2005 23:44:29  terui
Fix comment.

Revision 1.1  07/07/2005 10:45:52  terui
new additions

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"
#include "wcm_cpsif.h"

/*---------------------------------------------------------------------------*
constant definitions
 *---------------------------------------------------------------------------*/

// all 0xff ESSIDs
const u8    WCM_Essid_Any[WCM_ESSID_SIZE] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// all 0xff BSSIDs
const u8    WCM_Bssid_Any[WCM_BSSID_SIZE] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

#if WCM_DEBUG

// warning text template
static const char   wcmWarningText_AlreadyInit[] = { "WCM library is already initialized.\n" };
static const char   wcmWarningText_IllegalParam[] = { "Illegal parameter ( %s )\n" };
static const char   wcmWarningText_NotInit[] = { "WCM library is not initialized yet.\n" };
static const char   wcmWarningText_IllegalPhase[] = { "Could not accept request now. ( PHASE: %d )\n" };
static const char   wcmWarningText_InvalidWmState[] = { "Invalid state of WM library. Don't use WM or WVR library while using WCM library.\n" };
static const char   wcmWarningText_UnsuitableArm7[] = { "Unsuitable ARM7 component. Could not drive wireless module.\n" };
static const char   wcmWarningText_WirelessForbidden[] = { "Forbidden to use wireless module." };

// alert text template
static const char   wcmReportText_WmSyncError[] = { "%s failed syncronously. ( ERRCODE: 0x%02x )\n" };
static const char   wcmReportText_WmAsyncError[] = { "%s failed asyncronously. ( ERRCODE: 0x%02x - 0x%04x - 0x%04x )\n" };
static const char   wcmReportText_WmDisconnected[] = { "%s succeeded asyncronously , but already disconnected from AP.\n" };
static const char   wcmReportText_InvalidAid[] = { "%s succeeded asyncronously , but given Auth-ID is invalid.\n" };
static const char   wcmReportText_SupportRateset[] = { "Failed association to \"%s\" because of illegal support rate set.\n  So retry with camouflage 5.5M and 11M automatically.\n" };
#endif

/*---------------------------------------------------------------------------*
internal variable definitions
 *---------------------------------------------------------------------------*/
static WCMWork*     wcmw = NULL;

/*---------------------------------------------------------------------------*
internal function prototype
 *---------------------------------------------------------------------------*/
static void         WcmConfigure(WCMConfig* config, WCMNotify notify);
static void         WcmEditScanExParam(void* bssid, void* essid, u32 option);
static void         WcmInitOption(void);
static u16          WcmGetNextScanChannel(u16 channel);
static void         WcmNotify(s16 result, void* para0, void* para1, s32 para2);
static void         WcmNotifyEx(s16 notify, s16 result, void* para0, void* para1, s32 para2);
static void         WcmSetPhase(u32 phase);
static void         WcmKeepAliveAlarm(void* arg);
static u32          WcmCountBits(u32 arg);
static u32          WcmCountLeadingZero(u32 arg);

static void         WcmWmReset(void);

static void         WcmWmcbIndication(void* arg);
static void         WcmWmcbCommon(void* arg);
static void         WcmWmcbScanEx(void* arg);
static void         WcmWmcbEndScan(void* arg);
static void         WcmWmcbConnect(void* arg);
static void         WcmWmcbDisconnect(void* arg);
static void         WcmWmcbStartDCF(void* arg);
static void         WcmWmcbEndDCF(void* arg);
static void         WcmWmcbReset(void* arg);

/* compile assertion */
#ifndef SDK_ASSERT_ON_COMPILE
#define SDK_ASSERT_ON_COMPILE(expr) extern assert_on_compile ## #__LINE__ (char a[(expr) ? 1 : -1])
#endif

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
s32 WCM_Init(void* buf, s32 len)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms the validity of the WCM_WORK_SIZE constants at compile
    SDK_ASSERT_ON_COMPILE(sizeof(WCMWork) <= WCM_WORK_SIZE);

// confirms uninitialized
    if (wcmw != NULL)
    {
        WCMi_Warning(wcmWarningText_AlreadyInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // already initialized
    }

// confirms parameters
    if (buf == NULL)
    {
        WCMi_Warning(wcmWarningText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // specified buffer is NULL
    }

    if ((u32) buf & 0x01f)
    {
        WCMi_Warning(wcmWarningText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;          // specified buffer is not a 32-byte alignment
    }

    if (len < sizeof(WCMWork))
    {
        WCMi_Warning(wcmWarningText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_NOT_ENOUGH_MEM;   // insufficient size for specified buffer
    }

// initializes each of the work variable
    wcmw = (WCMWork*)buf;
    wcmw->phase = WCM_PHASE_WAIT;           // initializes internal phases
    wcmw->notifyId = WCM_NOTIFY_COMMON;     // initializes asynchronous API notification IDs
    wcmw->maxScanTime = 0;                  // initializes fixed scan time
    wcmw->apListLock = WCM_APLIST_UNLOCK;   // Initializes the lock status of the AP data storage list.
    wcmw->resetting = WCM_RESETTING_OFF;    // initializes reset duplicate call management flags
    wcmw->authId = 0;                       // initializes Auth-ID
    wcmw->wlStatusOnConnectFail = 0x0000;
    WcmInitOption();                        // initializes options settings
    WCMi_InitCpsif();                       // initializes CPS interface

// initializes alarms
    if (!OS_IsTickAvailable())
    {
        OS_InitTick();
    }

    if (!OS_IsAlarmAvailable())
    {
        OS_InitAlarm();
    }

    OS_CreateAlarm(&(wcmw->keepAliveAlarm));

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
Name: WCM_Finish

Description: Closes the WCM library. Synchronous function. The work buffer specified by the
WCM_Init function is released.

Arguments: None.

Returns: s32     -   Returns the process results from one of the following:
WCM_RESULT_SUCCESS , WCM_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32 WCM_Finish(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms if initialized
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // not initialized
    }

// confirms internal phase
    if (wcmw->phase != WCM_PHASE_WAIT)
    {
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // not the WAIT phase
    }

// releases the work buffer
    wcmw = NULL;

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_SUCCESS;
}

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
s32 WCM_StartupAsync(WCMConfig* config, WCMNotify notify)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms if initialized
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // not initialized
    }

// confirms internal phase
    switch (wcmw->phase)
    {
    case WCM_PHASE_WAIT:
        WcmConfigure(config, notify);
        break;

    case WCM_PHASE_WAIT_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;                 // already in asynchronous process for same request

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;                  // transitioned to state transition target phase

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // phase that cannot accept requests
    }

// begins the wireless functionality launch sequence
    {
        WMErrCode   wmResult;

// initializes the WM library
        wmResult = WM_Init(wcmw->wmWork, (u16) wcmw->config.dmano);
        switch (wmResult)
        {
        case WM_ERRCODE_SUCCESS:
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // WM library is already initialized

        case WM_ERRCODE_WM_DISABLE:
            WCMi_Warning(wcmWarningText_UnsuitableArm7);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_WMDISABLE;            // WM library not running on ARM7

        case WM_ERRCODE_INVALID_PARAM:
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // error outside of anticipated range
        }

// checks if main unit is blocking wireless communication
        if (0 == WM_GetAllowedChannel())
        {
            if (WM_ERRCODE_SUCCESS != WM_Finish())
            {
                WcmSetPhase(WCM_PHASE_FATAL_ERROR);
                (void)OS_RestoreInterrupts(e);
                return WCM_RESULT_FATAL_ERROR;      // error outside of anticipated range
            }

            WCMi_Warning(wcmWarningText_WirelessForbidden);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_WMDISABLE;            // wireless communication is not allowed
        }

// sets a vector for receiving notifications from an irregular ARM7
        wmResult = WM_SetIndCallback(WcmWmcbIndication);
        if (wmResult != WM_ERRCODE_SUCCESS)
        {
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // error outside of anticipated range
        }

// issues a wireless hardware use permission request
        wmResult = WM_Enable(WcmWmcbCommon);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_WAIT_TO_IDLE);    // asynchronous sequence start successful
            wcmw->notifyId = WCM_NOTIFY_STARTUP;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_Enable", wmResult);
            WcmSetPhase(WCM_PHASE_IRREGULAR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;              // request to ARM7 failed

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // error outside of anticipated range
        }
    }

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

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
s32 WCM_CleanupAsync(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms if initialized
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // not initialized
    }

// confirms internal phase
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
        break;

    case WCM_PHASE_IDLE_TO_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;                 // already in asynchronous process for same request

    case WCM_PHASE_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;                  // transitioned to state transition target phase

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // phase that cannot accept requests
    }

// begins the wireless functionality halt sequence
    {
        WMErrCode   wmResult;

// issues a shutdown request to the wireless hardware
        wmResult = WM_PowerOff(WcmWmcbCommon);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_IDLE_TO_WAIT);    // asynchronous sequence start successful
            wcmw->notifyId = WCM_NOTIFY_CLEANUP;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_PowerOff", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;               // request to ARM7 failed (retry possible)

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // error outside of anticipated range
        }
    }

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

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
s32 WCM_SearchAsync(void* bssid, void* essid, u32 option)
{
// confirms parameters
    if ((bssid == NULL) || (essid == NULL))
    {
        return WCM_EndSearchAsync();
    }

    return WCM_BeginSearchAsync(bssid, essid, option);
}

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
s32 WCM_BeginSearchAsync(void* bssid, void* essid, u32 option)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms if initialized
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;                  // not initialized
    }

// begins the AP auto-search start sequence or changes search conditions
    {
        WMErrCode   wmResult;

        switch (wcmw->phase)
        {
        case WCM_PHASE_IDLE:
            break;

        case WCM_PHASE_IDLE_TO_SEARCH:
            WcmEditScanExParam(bssid, essid, option);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_PROGRESS;             // already in asynchronous process for same request

        case WCM_PHASE_SEARCH:
            WcmEditScanExParam(bssid, essid, option);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_SUCCESS;              // transitioned to state transition target phase

        default:
            WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;              // phase that cannot accept requests
        }

// issues a scan start request
        WcmEditScanExParam(bssid, essid, option);
        DC_InvalidateRange(wcmw->scanExParam.scanBuf, wcmw->scanExParam.scanBufSize);
        wcmw->scanCount++;
        wmResult = WM_StartScanEx(WcmWmcbScanEx, &(wcmw->scanExParam));
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_IDLE_TO_SEARCH);  // asynchronous sequence start successful
            wcmw->notifyId = WCM_NOTIFY_BEGIN_SEARCH;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_StartScanEx", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;               // request to ARM7 failed (retry possible)

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;          // error outside of anticipated range
        }
    }

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

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
s32 WCM_EndSearchAsync(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms if initialized
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // not initialized
    }

// confirms internal phase
    switch (wcmw->phase)
    {
    case WCM_PHASE_SEARCH:
        WcmSetPhase(WCM_PHASE_SEARCH_TO_IDLE);
        wcmw->notifyId = WCM_NOTIFY_END_SEARCH;
        break;

    case WCM_PHASE_SEARCH_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS; // already in asynchronous process for same request

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;  // transitioned to state transition target phase

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;  // phase that cannot accept requests
    }

    /* as a scan end request is issued within the StartScanEx callback, it is not performed here */

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

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
s32 WCM_ConnectAsync(void* bssDesc, void* wepDesc, u32 option)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms if initialized
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // not initialized
    }

// confirms internal phase
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
// confirms parameters
        if (bssDesc == NULL)
        {
            WCMi_Warning(wcmWarningText_IllegalParam, "bssDesc");
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;          // bssDesc argument is NULL
        }

        if (((WMBssDesc*)bssDesc)->gameInfoLength > 0)
        {
            WCMi_Warning(wcmWarningText_IllegalParam, "bssDesc");
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FAILURE;          // the bssDesc argument is for the DS parent device, not the AP data
        }

        if (wepDesc != NULL)
        {
            if ((((WCMWepDesc*)wepDesc)->mode >= 4) || (((WCMWepDesc*)wepDesc)->keyId >= 4))
            {
                WCMi_Warning(wcmWarningText_IllegalParam, "wepDesc");
                (void)OS_RestoreInterrupts(e);
                return WCM_RESULT_FAILURE;      // the wepDesc argument has unexpected content
            }

            wcmw->wepDesc.mode = ((WCMWepDesc*)wepDesc)->mode;
            wcmw->wepDesc.keyId = ((WCMWepDesc*)wepDesc)->keyId;
            if (wcmw->wepDesc.mode == WCM_WEPMODE_NONE)
            {
                MI_CpuClear8(wcmw->wepDesc.key, WM_SIZE_WEPKEY);
            }
            else
            {
                MI_CpuCopy8(((WCMWepDesc*)wepDesc)->key, wcmw->wepDesc.key, WM_SIZE_WEPKEY);
            }
        }
        else
        {
            MI_CpuClear8(&(wcmw->wepDesc), sizeof(WCMWepDesc));
        }

        MI_CpuCopy8(bssDesc, &(wcmw->bssDesc), sizeof(WMBssDesc));
#if 0
        /* debug */
        {
            int     i;
            u8*     pDump = (u8 *) (&(wcmw->bssDesc));

            for (i = 0; i < 192; i++)
            {
                if (i % 16)
                {
                    OS_TPrintf(" ");
                }

                OS_TPrintf("%02X", pDump[i]);
                if ((i % 16) == 15)
                {
                    OS_TPrintf("\n");
                }
            }
        }
#endif

// the supported data transfer rate is revised for NITRO
        wcmw->bssDesc.rateSet.support = (u16) (WCM_ADDITIONAL_RATESET | wcmw->bssDesc.rateSet.basic);
        (void)WCM_UpdateOption(option);
        break;

    case WCM_PHASE_IDLE_TO_DCF:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // already in asynchronous process for same request

    case WCM_PHASE_DCF:
        (void)OS_RestoreInterrupts(e);          // transitioned to state transition target phase
        return WCM_RESULT_SUCCESS;

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // phase that cannot accept requests
    }

// begins the AP wireless connection sequence
    {
        WMErrCode   wmResult;

// issues a timeout setting request to the wireless firmware
        wmResult = WM_SetLifeTime(WcmWmcbCommon, 0xffff, WCM_CAM_LIFETIME, 0xffff, 0xffff);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_IDLE_TO_DCF); // asynchronous sequence start successful
            wcmw->notifyId = WCM_NOTIFY_CONNECT;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_SetLifeTime", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;           // request to ARM7 failed (retry possible)

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;      // error outside of anticipated range
        }
    }

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

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
s32 WCM_DisconnectAsync(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms if initialized
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // not initialized
    }

// confirms internal phase
    switch (wcmw->phase)
    {
    case WCM_PHASE_DCF:
        break;

    case WCM_PHASE_DCF_TO_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // already in asynchronous process for same request

    case WCM_PHASE_IDLE:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;              // transitioned to state transition target phase

    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // phase that cannot accept requests
    }

// begins the AP wireless termination sequence
    if (wcmw->resetting == WCM_RESETTING_ON)
    {
        /* because the reset comes after receiving a termination notification, it indicates that a termination request has been run */
        WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
        wcmw->notifyId = WCM_NOTIFY_DISCONNECT;
    }
    else
    {
        WMErrCode   wmResult;

// issues a DCF communication mode end request
        wmResult = WM_EndDCF(WcmWmcbEndDCF);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_DCF_TO_IDLE); // asynchronous sequence start successful
            wcmw->notifyId = WCM_NOTIFY_DISCONNECT;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_EndDCF", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;           // request to ARM7 failed (retry possible)

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;      // error outside of anticipated range
        }
    }

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

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
s32 WCM_TerminateAsync(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

// confirms if initialized
    if (wcmw == NULL)
    {
        WCMi_Warning(wcmWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // not initialized
    }

// confirms internal phase
    switch (wcmw->phase)
    {
    case WCM_PHASE_IDLE:
    case WCM_PHASE_DCF:
    case WCM_PHASE_IRREGULAR:
        break;

    case WCM_PHASE_TERMINATING:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_PROGRESS;             // already in asynchronous process for same request

    case WCM_PHASE_WAIT:
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_SUCCESS;              // transitioned to state transition target phase

    case WCM_PHASE_SEARCH:
        WcmSetPhase(WCM_PHASE_TERMINATING);
        wcmw->notifyId = WCM_NOTIFY_TERMINATE;
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_ACCEPT;               // accepts an asynchronous process request

    /* as a reset request is issued within the StartScanEx callback, it is not performed here */
    case WCM_PHASE_WAIT_TO_IDLE:
    case WCM_PHASE_IDLE_TO_WAIT:
    case WCM_PHASE_IDLE_TO_SEARCH:
    case WCM_PHASE_SEARCH_TO_IDLE:
    case WCM_PHASE_IDLE_TO_DCF:
    case WCM_PHASE_DCF_TO_IDLE:
    case WCM_PHASE_FATAL_ERROR:
    default:
        WCMi_Warning(wcmWarningText_IllegalPhase, wcmw->phase);
        (void)OS_RestoreInterrupts(e);
        return WCM_RESULT_FAILURE;              // phase that cannot accept requests
    }

// begins the wireless functionality termination sequence
    if (wcmw->resetting == WCM_RESETTING_ON)
    {
        /* the reset occurs after terminating DCF communication, so this reset is seen as if a forced termination request has run */
        WcmSetPhase(WCM_PHASE_TERMINATING);
        wcmw->notifyId = WCM_NOTIFY_TERMINATE;
    }
    else
    {
        WMStatus*   ws;
        u16         wmState;
        WMErrCode   wmResult;

// checks the internal state of the WM library
        ws = (WMStatus*)WMi_GetStatusAddress();
        DC_InvalidateRange(ws, 2);
        wmState = ws->state;

// begins chaining to transition to a halt state based on the WM internal state
        switch (wmState)
        {
        case WM_STATE_READY:
// WM library ends
            wmResult = WM_Finish();
            if (wmResult == WM_ERRCODE_SUCCESS)
            {
                WcmSetPhase(WCM_PHASE_WAIT);
                wcmw->notifyId = WCM_NOTIFY_COMMON;
                (void)OS_RestoreInterrupts(e);
                return WCM_RESULT_SUCCESS;      // synchronous transition to WAIT phase completed
            }
            break;

        case WM_STATE_STOP:
// issues a wireless hardware use blocked request
            wmResult = WM_Disable(WcmWmcbCommon);
            break;

        case WM_STATE_IDLE:
// issues a shutdown request to the wireless hardware
            wmResult = WM_PowerOff(WcmWmcbCommon);
            break;

        default:
// sets reset duplicate call management flags
            wcmw->resetting = WCM_RESETTING_ON;

// issues the wireless connection state reset request
            wmResult = WM_Reset(WcmWmcbReset);
        }

// confirms the synchronous process results for issued requests
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            WcmSetPhase(WCM_PHASE_TERMINATING); // asynchronous sequence start successful
            wcmw->notifyId = WCM_NOTIFY_TERMINATE;
            break;

        case WM_ERRCODE_FIFO_ERROR:
            WCMi_Printf(wcmReportText_WmSyncError, "WM_***", wmResult);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_REJECT;           // request to ARM7 failed (retry possible)

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            (void)OS_RestoreInterrupts(e);
            return WCM_RESULT_FATAL_ERROR;      // error outside of anticipated range
        }
    }

// normal end
    (void)OS_RestoreInterrupts(e);
    return WCM_RESULT_ACCEPT;
}

/*---------------------------------------------------------------------------*
Name: WCM_GetPhase

Description: Obtains the WCM library's internal state (phase). Synchronous function.

Arguments: None.

Returns: s32     -   Returns the current WCM library phase.
 *---------------------------------------------------------------------------*/
s32 WCM_GetPhase(void)
{
    OSIntrMode  e = OS_DisableInterrupts();
    s32         phase = WCM_PHASE_NULL;

// confirms if initialized
    if (wcmw != NULL)
    {
        phase = (s32) (wcmw->phase);
    }

    (void)OS_RestoreInterrupts(e);
    return phase;
}

/*---------------------------------------------------------------------------*
Name: WCM_UpdateOption

Description: Updates the WCM library options settings.

Arguments: option  -   Specifies the 32-bit variable for changing options
in conjunction with the options setting format.
No updates are performed when 0 is specified.

Returns: u32     -   Returns the options variable prior to changing.
 *---------------------------------------------------------------------------*/
u32 WCM_UpdateOption(u32 option)
{
    OSIntrMode  e = OS_DisableInterrupts();
    u32         filter = 0;
    u32         old_option = wcmw->option;

// initialization confirmation
    if (wcmw == NULL)
    {
        (void)OS_RestoreInterrupts(e);
        return 0;
    }

// extracts the options category to be updated, edits the bits to be temporarily cleared
    if (option & WCM_OPTION_TEST_CHANNEL)
    {
        filter |= WCM_OPTION_FILTER_CHANNEL;
        if (!(option & WCM_OPTION_FILTER_CHANNEL))
        {
            option |= WCM_OPTION_CHANNEL_RDC;
        }
    }

    if (option & WCM_OPTION_TEST_POWER)
    {
        filter |= WCM_OPTION_FILTER_POWER;
    }

    if (option & WCM_OPTION_TEST_AUTH)
    {
        filter |= WCM_OPTION_FILTER_AUTH;
    }

    if (option & WCM_OPTION_TEST_SCANTYPE)
    {
        filter |= WCM_OPTION_FILTER_SCANTYPE;
    }

    if (option & WCM_OPTION_TEST_ROUNDSCAN)
    {
        filter |= WCM_OPTION_FILTER_ROUNDSCAN;
    }

// updates each bit in the options variables
    wcmw->option = (u32) ((old_option &~filter) | option);

    (void)OS_RestoreInterrupts(e);
    return old_option;
}

/*---------------------------------------------------------------------------*
Name: WCM_SetChannelScanTime

Description: Sets the time for a scan in a single channel during an AP auto-search.

Arguments: msec    -   Specifies the time to scan a single in milliseconds.
Values between 10 and 1000 are accepted; if other values
are specified, the default search time for auto-search
will be used.

Returns: None.
 *---------------------------------------------------------------------------*/
void WCM_SetChannelScanTime(u16 msec)
{
// initialization confirmation
    if (wcmw != NULL)
    {
        if ((msec >= 10) && (msec <= 1000))
        {
            wcmw->maxScanTime = msec;
        }
        else
        {
            wcmw->maxScanTime = 0;
        }
    }
}

/*---------------------------------------------------------------------------*
Name: WCMi_GetSystemWork

Description: An internal function to get a pointer to the work buffer used by
the WCM library internally.

Arguments: None.

Returns: WCMWork*    -   Returns a pointer to the work buffer. NULL if there is no
buffer, as before initialization.
 *---------------------------------------------------------------------------*/
WCMWork* WCMi_GetSystemWork(void)
{
    return wcmw;
}

/*---------------------------------------------------------------------------*
Name: WcmConfigure

Description: Configures the WCM internal operations settings.

Arguments: config  -   Specifies a pointer to the structure for WCM operation settings.
notify  -   Specifies callback function that notifies asynchronous process results.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmConfigure(WCMConfig* config, WCMNotify notify)
{
    if (config == NULL)
    {
// if settings are not specified, the default values are used
        wcmw->config.dmano = 3;
        wcmw->config.pbdbuffer = NULL;
        wcmw->config.nbdbuffer = 0;
        wcmw->config.nbdmode = 0;
    }
    else
    {
// backs up the DMA number
        if (config->dmano &~(0x03))
        {
// the DMA number has to be between 0 and 3
            WCMi_Warning(wcmWarningText_IllegalParam, "config->dmano");
        }

        wcmw->config.dmano = (config->dmano & 0x03);

// backs up the AP data storage list area settings
        if ((((4 - ((u32) (config->pbdbuffer) & 0x03)) % 4) + sizeof(WCMApListHeader)) > config->nbdbuffer)
        {
// as the size is not enough to even store the list management header area, it is the same as if there were no buffer
            wcmw->config.pbdbuffer = NULL;
            wcmw->config.nbdbuffer = 0;
        }
        else
        {
            /*
* As it is possible that the given buffer is not 4-byte aligned,
* it is adjusted for 4-byte alignment and its size reduced by that amount.
             */
            wcmw->config.pbdbuffer = (void*)WCM_ROUNDUP4((u32) (config->pbdbuffer));
            wcmw->config.nbdbuffer = config->nbdbuffer - (s32) ((4 - ((u32) (config->pbdbuffer) & 0x03)) % 4);
            MI_CpuClear8(wcmw->config.pbdbuffer, (u32) (wcmw->config.nbdbuffer));
        }

        wcmw->config.nbdmode = config->nbdmode;
    }

// notification vector back-up
    wcmw->notify = notify;
}

/*---------------------------------------------------------------------------*
Name: WcmEditScanExParam

Description: Edits the scan settings structure for indicating a scan of the WM library.

Arguments: bssid   -   The BSSID filter during search. As the BSSID for WCM_BSSID_ANY
are all 0xff, the settings are unfiltered.
essid   -   The ESSID filter during search. If WCM_ESSID_ANY is specified
it is treated as an ESSID with a length of 0, and the settings are unfiltered.
option  -   Variable for options changes. If changes are made, the options
used jointly within the WCM library are changed and cannot be restored.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmEditScanExParam(void* bssid, void* essid, u32 option)
{
    (void)WCM_UpdateOption(option);
    wcmw->scanExParam.scanBuf = (WMBssDesc*)wcmw->recvBuf;
    wcmw->scanExParam.scanBufSize = WM_SIZE_SCAN_EX_BUF;
    wcmw->scanExParam.channelList = (u16) ((0x0001 << WcmGetNextScanChannel(0)) >> 1);
    wcmw->scanExParam.maxChannelTime = ((wcmw->maxScanTime != 0) ? wcmw->maxScanTime : WM_GetDispersionScanPeriod());
    wcmw->scanExParam.scanType = (u16) (((wcmw->option & WCM_OPTION_MASK_SCANTYPE) == WCM_OPTION_SCANTYPE_ACTIVE) ? WM_SCANTYPE_ACTIVE : WM_SCANTYPE_PASSIVE);
    if (bssid == NULL)
    {
        MI_CpuCopy8(WCM_Bssid_Any, wcmw->scanExParam.bssid, WCM_BSSID_SIZE);
    }
    else
    {
        MI_CpuCopy8(bssid, wcmw->scanExParam.bssid, WCM_BSSID_SIZE);
    }

//MI_CpuCopy8( bssid , wcmw->scanExParam.bssid , WCM_BSSID_SIZE );
    if ((essid == NULL) || (essid == (void*)WCM_Essid_Any))
    {
        MI_CpuCopy8(WCM_Essid_Any, wcmw->scanExParam.ssid, WCM_ESSID_SIZE);
        wcmw->scanExParam.ssidLength = 0;
    }
    else
    {
        s32 len;

        MI_CpuCopy8(essid, wcmw->scanExParam.ssid, WCM_ESSID_SIZE);
        for (len = 0; len < WCM_ESSID_SIZE; len++)
        {
            if (((u8*)essid)[len] == '\0')
            {
                break;
            }
        }

        wcmw->scanExParam.ssidLength = (u16) len;
    }

    wcmw->scanCount = 0;
}

/*---------------------------------------------------------------------------*
Name: WcmInitOption

Description: Initializes the WCM library options settings values.

Arguments: None.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmInitOption(void)
{
    /*
* The following default settings are use to match IW and I/O, but
* it would be OK if CHANNEL were ALL and SCANTYPE were ACTIVE.
     */
    wcmw->option = WCM_OPTION_CHANNEL_RDC |
        WCM_OPTION_POWER_SAVE |
        WCM_OPTION_AUTH_OPENSYSTEM |
        WCM_OPTION_SCANTYPE_PASSIVE |
        WCM_OPTION_ROUNDSCAN_IGNORE;
}

/*---------------------------------------------------------------------------*
Name: WcmGetNextScanChannel

Description: Determines the next channel to be scanned based on the
channel list set in the options during search.

Arguments: channel -   Passes the next channel to be scanned.

Returns: u16     -   Returns the next channel to be scanned next.
 *---------------------------------------------------------------------------*/
static u16 WcmGetNextScanChannel(u16 channel)
{
    s32 i;

    for (i = 0; i < 13; i++)
    {
        if (wcmw->option & (0x0001 << (((channel + i) % 13) + 1)))
        {
            break;
        }
    }

    return(u16) (((channel + i) % 13) + 1);
}

/*---------------------------------------------------------------------------*
Name: WcmNotify

Description: Calls back the process results for an asynchronous process.
Irregularly issued notifications are also called back through this function.
Notification types are automatically set from WCM internal common variables;
to avoid duplicate notifications, they are cleared at each notification.

Arguments: result  -   Specifies process results.
para0   -   parameter that passes the notification function [0]
para1   -   parameter that passes the notification function [1]
para2   -   parameter that passes the notification function [2]

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmNotify(s16 result, void* para0, void* para1, s32 para2)
{
    s16 notifyId = wcmw->notifyId;

    wcmw->notifyId = WCM_NOTIFY_COMMON;
    WcmNotifyEx(notifyId, result, para0, para1, para2);
}

/*---------------------------------------------------------------------------*
Name: WcmNotifyEx

Description: Performs a notification via a callback to the application.

Arguments: result  -   Specifies process results.
para0   -   parameter that passes the notification function [0]
para1   -   parameter that passes the notification function [1]
para2   -   parameter that passes the notification function [2]

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmNotifyEx(s16 notify, s16 result, void* para0, void* para1, s32 para2)
{
    if (wcmw->notify)
    {
        WCMNotice   notice;

        notice.notify = notify;
        notice.result = result;
        notice.parameter[0].p = para0;
        notice.parameter[1].p = para1;
        notice.parameter[2].n = para2;
        wcmw->notify(&notice);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmSetPhase

Description: Changes the WCM library's internal state (phase).
A change cannot be made to a separate phase from a FATAL_ERROR state.
In addition, phase changes to and from a DCF communication state
are monitored, and the keep alive packet send alarm
operations are controlled.

Arguments: phase   -   Specifies the phase to be changed.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmSetPhase(u32 phase)
{
    OSIntrMode  e = OS_DisableInterrupts();

// halts the alarm when moving from a DCF communication state to another state
    if ((wcmw->phase == WCM_PHASE_DCF) && (phase != WCM_PHASE_DCF))
    {
// halts the Keep Alive alarm
        OS_CancelAlarm(&(wcmw->keepAliveAlarm));
    }

// changes to the specified state as long as the current state is not FATAL ERROR
    if (wcmw->phase != WCM_PHASE_FATAL_ERROR)
    {
        wcmw->phase = phase;
    }

// begins the alarm when moving to a DCF communication state
    if (phase == WCM_PHASE_DCF)
    {
// begins the Keep Alive alarm
        OS_SetAlarm(&(wcmw->keepAliveAlarm), OS_SecondsToTicks(WCM_KEEP_ALIVE_SPAN), WcmKeepAliveAlarm, NULL);
    }
    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
Name: WCMi_ResetKeepAliveAlarm

Description: Resets the Keep Alive NULL packet send alarm, and resets
an alarm for the indicated time.

Arguments: None.

Returns: None.
 *---------------------------------------------------------------------------*/
void WCMi_ResetKeepAliveAlarm(void)
{
    OSIntrMode  e = OS_DisableInterrupts();

// sets the alarm anew
    OS_CancelAlarm(&(wcmw->keepAliveAlarm));
    if (wcmw->phase == WCM_PHASE_DCF)
    {
        OS_SetAlarm(&(wcmw->keepAliveAlarm), OS_SecondsToTicks(WCM_KEEP_ALIVE_SPAN), WcmKeepAliveAlarm, NULL);
    }
    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
Name: WcmKeepAliveAlarm

Description: Alarm handler for the Keep Alive NULL packet send alarm.

Arguments: arg     -   Unused.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmKeepAliveAlarm(void* arg)
{
#pragma unused(arg)

    WCMi_CpsifSendNullPacket();
    WCMi_ResetKeepAliveAlarm();
}

#include <nitro/code32.h>

/*---------------------------------------------------------------------------*
Name: WcmCountBits

Description: Gets the number of bits that are 1 and included in u32-type values.

Arguments: arg     -   The u32-type value to be checked.

Returns: u32     -   Returns number of 1 bits included in the argument. A value between 0 and 32.
 *---------------------------------------------------------------------------*/

static asm u32
WcmCountBits( u32 arg )
{
    mov     r1, r0
    mov     r0, #0
    mov     r3, #1
@loop:
    clz     r2, r1
    rsbs    r2, r2, #31
    bxcc    lr
    bic     r1, r1, r3, LSL r2
    add     r0, r0, #1
    b       @loop
}

/*---------------------------------------------------------------------------*
Name: WcmCountLeadingZero

Description: Gets the count of leading zeroes (how many of the first bits
are consecutively zeros) in a u32-type value.

Arguments: arg     -   The u32-type value to be checked.

Returns: u32     -   Returns a count of leading zeros. A value between 0 and 32.
 *---------------------------------------------------------------------------*/

static asm u32
WcmCountLeadingZero( u32 arg )
{
    clz     r0, r0
    bx      lr
}
#include <nitro/codereset.h>

/*---------------------------------------------------------------------------*
Name: WcmWmReset

Description: Begins reset process for WM library via the WM_Reset function.

Arguments: None.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmReset(void)
{
    WMErrCode   wmResult;

    if (wcmw->resetting == WCM_RESETTING_OFF)
    {
// sets reset duplicate call management flags
        wcmw->resetting = WCM_RESETTING_ON;

// issues the wireless connection state reset request
        wmResult = WM_Reset(WcmWmcbReset);
        if (wmResult != WM_ERRCODE_OPERATING)
        {
            /* no restore is possible when reset fails */
            WCMi_Printf(wcmReportText_WmSyncError, "WM_Reset", wmResult);
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);
            WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
        }
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbIndication

Description: A handler to accept irregular notifications from the WM library.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbIndication(void* arg)
{
    WMIndCallback*  cb = (WMIndCallback*)arg;

    /* notifications not related to WCM internal processes are ignored */
    if (cb->errcode == WM_ERRCODE_FIFO_ERROR)
    {
        if ((cb->state == WM_STATECODE_FIFO_ERROR) && (cb->reason == WM_APIID_AUTO_DISCONNECT))
        {
            /*
* when a fatal hardware error resulting from a loss of communication integrity is detected by ARM7,
* and auto-termination was begun but a termination request cannot be reserved because the request queue is full
             */
            switch (wcmw->phase)
            {
            case WCM_PHASE_IDLE_TO_DCF:
                WcmSetPhase(WCM_PHASE_IRREGULAR);
                break;

            case WCM_PHASE_DCF:
            case WCM_PHASE_IRREGULAR:
                WcmWmReset();
                break;

            case WCM_PHASE_DCF_TO_IDLE:
                WcmSetPhase(WCM_PHASE_IRREGULAR);
                break;

                /* notifications from unexpected phases are ignored */
            }
        }
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbCommon

Description: A handler to accept notifications from the WM library.
WM_Enable , WM_Disable , WM_PowerOn , WM_PowerOff ,
WM_SetLifeTime , WM_SetBeaconIndication , WM_SetWEPKeyEx
Receives result responses for the above.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbCommon(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
// determines the process to be performed in the next step
        switch (cb->apiid)
        {
        case WM_APIID_ENABLE:
// issues a power supply request to the wireless hardware
            wmResult = WM_PowerOn(WcmWmcbCommon);
            break;

        case WM_APIID_DISABLE:
// WM library ends
            wmResult = WM_Finish();
            switch (wmResult)
            {
            case WM_ERRCODE_SUCCESS:
                WcmSetPhase(WCM_PHASE_WAIT);        // normal end to an asynchronous sequence
                WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
                break;

            case WM_ERRCODE_WM_DISABLE:
                WCMi_Warning(wcmWarningText_InvalidWmState);

            /* Don't break here */
            default:
                WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
                WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
            }

            return; // if there is no next step, end here

        case WM_APIID_POWER_ON:
            WcmSetPhase(WCM_PHASE_IDLE);    // normal end to an asynchronous sequence
            WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
            return; // if there is no next step, end here

        case WM_APIID_POWER_OFF:
// issues a wireless hardware use blocked request
            wmResult = WM_Disable(WcmWmcbCommon);
            break;

        case WM_APIID_SET_LIFETIME:
// issues a beacon send/receive notification OFF request
            wmResult = WM_SetBeaconIndication(WcmWmcbCommon, 0);
            break;

        case WM_APIID_SET_BEACON_IND:
// issues a WEP encryption settings request
            wmResult = WM_SetWEPKeyEx(WcmWmcbCommon, (u16) (wcmw->wepDesc.mode), (u16) (wcmw->wepDesc.keyId),
                                      (const u8*)(wcmw->wepDesc.key));
            break;

        case WM_APIID_SET_WEPKEY_EX:
// issues an AP wireless connection request
            wmResult = WM_StartConnectEx(WcmWmcbConnect, &(wcmw->bssDesc), NULL,
                                         ((wcmw->option & WCM_OPTION_MASK_POWER) == WCM_OPTION_POWER_ACTIVE ? FALSE : TRUE),
                                         ((wcmw->option & WCM_OPTION_MASK_AUTH) == WCM_OPTION_AUTH_SHAREDKEY ? (u16) WM_AUTHMODE_SHARED_KEY :
                                                     (u16) WM_AUTHMODE_OPEN_SYSTEM));
            break;
        }

// confirms synchronous process results
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_FIFO_ERROR:
#if WCM_DEBUG
            switch (cb->apiid)
            {
            case WM_APIID_ENABLE:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_PowerOn", wmResult);
                break;

            case WM_APIID_POWER_OFF:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_Disable", wmResult);
                break;

            case WM_APIID_SET_LIFETIME:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_SetBeaconIndication", wmResult);
                break;

            case WM_APIID_SET_BEACON_IND:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_SetWEPKeyEx", wmResult);
                break;

            case WM_APIID_SET_WEPKEY_EX:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_StartConnectEx", wmResult);
                break;
            }
#endif
            WcmSetPhase(WCM_PHASE_IRREGULAR);   // failed to issue request to ARM7 (retry failed during sequence)
            WcmNotify(WCM_RESULT_FAILURE, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
            WcmNotify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
#if WCM_DEBUG
        switch (cb->apiid)
        {
        case WM_APIID_ENABLE:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_Enable", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_DISABLE:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_Disable", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_POWER_ON:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_PowerOn", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_POWER_OFF:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_PowerOff", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_LIFETIME:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_SetLifeTime", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_BEACON_IND:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_SetBeaconIndication", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;

        case WM_APIID_SET_WEPKEY_EX:
            WCMi_Printf(wcmReportText_WmAsyncError, "WM_SetWEPKeyEx", cb->errcode, cb->wlCmdID, cb->wlResult);
            break;
        }
#endif
        WcmSetPhase(WCM_PHASE_IRREGULAR);       // an unspecified ARM7 error
        WcmNotify(WCM_RESULT_FAILURE, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // error outside of anticipated range
        WcmNotify(WCM_RESULT_FATAL_ERROR, (wcmw->notifyId == WCM_NOTIFY_CONNECT ? &(wcmw->bssDesc) : 0), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbScanEx

Description: A handler to accept notifications from the WM library.
Receives result responses for WM_StartScanEx.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbScanEx(void* arg)
{
    WMStartScanExCallback*  cb = (WMStartScanExCallback*)arg;
    WMErrCode   wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
// if auto-search is starting up, notifies that auto-search mode was entered normally
        if (wcmw->phase == WCM_PHASE_IDLE_TO_SEARCH)
        {
            WcmSetPhase(WCM_PHASE_SEARCH);

            /* it is anticipated that an auto-search halt request can come within this notification */
            WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
        }

// determines the process to be performed in the next step
        switch (wcmw->phase)
        {
        case WCM_PHASE_SEARCH:
            wcmw->notifyId = WCM_NOTIFY_FOUND_AP;
            if (cb->state == WM_STATECODE_PARENT_FOUND)
            {
// notifies the application continuing to edit the internal list when AP data is discovered
                s32 i;

                DC_InvalidateRange(wcmw->scanExParam.scanBuf, wcmw->scanExParam.scanBufSize);
                for (i = 0; i < cb->bssDescCount; i++)
                {
                    WCMi_EntryApList(cb->bssDesc[i], cb->linkLevel[i]);

                    /*
* The following parameter configurations are used to match IW and I/O, but
* it is OK for parameters to hold cb and i.
                     */
                    WcmNotifyEx(WCM_NOTIFY_FOUND_AP, WCM_RESULT_SUCCESS, cb->bssDesc[i], (void*)cb, __LINE__);
                }
            }

// confirms whether the specified channel has looped
            if ((wcmw->option & WCM_OPTION_MASK_ROUNDSCAN) == WCM_OPTION_ROUNDSCAN_NOTIFY)
            {
                u32 channels = WcmCountBits(wcmw->option & WCM_OPTION_FILTER_CHANNEL);

                if (channels > 0)
                {
                    if ((wcmw->scanCount % channels) == 0)
                    {
// notifies the loop status for the scene for each channel
                        WcmNotifyEx(WCM_NOTIFY_SEARCH_AROUND, WCM_RESULT_SUCCESS, (void*)(wcmw->scanCount), 0, __LINE__);
                    }
                }
            }

// issues a scan begin request for the next channel
            wcmw->scanExParam.channelList = (u16) ((0x0001 << WcmGetNextScanChannel((u16) (32 - WcmCountLeadingZero(cb->channelList)))) >> 1);
            DC_InvalidateRange(wcmw->scanExParam.scanBuf, wcmw->scanExParam.scanBufSize);
            wcmw->scanCount++;
            wmResult = WM_StartScanEx(WcmWmcbScanEx, &(wcmw->scanExParam));
            break;

        case WCM_PHASE_SEARCH_TO_IDLE:
// issues a scan halt request
            wmResult = WM_EndScan(WcmWmcbEndScan);
            break;

        case WCM_PHASE_TERMINATING:
// reset here if in a forced termination sequence
            WcmWmReset();
            return;
        }

// confirms synchronous process results
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_FIFO_ERROR:
#if WCM_DEBUG
            switch (wcmw->phase)
            {
            case WCM_PHASE_SEARCH:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_StartScanEx", wmResult);
                break;

            case WCM_PHASE_SEARCH_TO_IDLE:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_EndScan", wmResult);
                break;
            }
#endif
            WcmSetPhase(WCM_PHASE_IRREGULAR);   // failed to issue request to ARM7 (retry failed during sequence)
            WcmNotify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
            WCMi_Warning(wcmWarningText_InvalidWmState);

        /* Don't break here */
        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
            WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
// reset when scan request fails
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_StartScanEx", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // error outside of anticipated range
        WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbEndScan

Description: A handler to accept notifications from the WM library.
Receives result responses for WM_EndScan.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbEndScan(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        WcmSetPhase(WCM_PHASE_IDLE);        // normal end to an asynchronous sequence
        WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
        break;

    case WM_ERRCODE_FAILED:
// reset when scan halt request fails
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_EndScan", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
        WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbConnect

Description: A handler to accept notifications from the WM library.
Accepts results responses for WM_StartConnectEx and terminated notifications
in DCF mode.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbConnect(void* arg)
{
    WMStartConnectCallback*     cb = (WMStartConnectCallback*)arg;
    WMErrCode   wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        switch (cb->state)
        {
        case WM_STATECODE_DISCONNECTED:                     // terminated notifications
        case WM_STATECODE_BEACON_LOST:                      // performs process similar to terminated one
            switch (wcmw->phase)
            {
            case WCM_PHASE_DCF_TO_IDLE:
// clears Auth-ID
                wcmw->authId = 0;

            /* Don't break here */
            case WCM_PHASE_IDLE_TO_DCF:
                WcmSetPhase(WCM_PHASE_IRREGULAR);
                break;

            case WCM_PHASE_DCF:
// clears Auth-ID
                wcmw->authId = 0;

// sets the notification type
                wcmw->notifyId = WCM_NOTIFY_DISCONNECT;

            /* Don't break here */
            case WCM_PHASE_IRREGULAR:
                WcmWmReset();
                break;

                /* termination notifications from unexpected phases are ignored */
            }
            break;

#if SDK_VERSION_MAJOR > 3 || (SDK_VERSION_MAJOR == 3 && SDK_VERSION_MINOR > 0) || \
        (SDK_VERSION_MAJOR == 3 && SDK_VERSION_MINOR == 0 && SDK_VERSION_RELSTEP >= 20100)

        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:         // If disconnection was self-initiated, processing occurs in disconnection function, so do nothing.
            break;
#endif

        case WM_STATECODE_CONNECT_START:                    // not processed as connection is underway
            break;

        case WM_STATECODE_CONNECTED:                        // connection complete notification
            if (wcmw->phase == WCM_PHASE_IRREGULAR)
            {
// if a connection complete notification comes after termination, considered a failure and reset
                WCMi_Printf(wcmReportText_WmDisconnected, "WM_StartConnectEx");
                WcmSetPhase(WCM_PHASE_IDLE_TO_DCF);         // overwrites the phase
                WcmWmReset();
            }
            else
            {
// AID should derive a range between 1 and 2007
                if ((WCM_AID_MIN <= cb->aid) && (cb->aid <= WCM_AID_MAX))
                {
// backs up Auth-ID
                    wcmw->authId = cb->aid;

// issues a DCF communication mode begin request
                    wmResult = WM_StartDCF(WcmWmcbStartDCF, (WMDcfRecvBuf *) (wcmw->recvBuf), WCM_DCF_RECV_BUF_SIZE);
                    switch (wmResult)
                    {
                    case WM_ERRCODE_OPERATING:
                        break;

                    case WM_ERRCODE_FIFO_ERROR:
                        WCMi_Printf(wcmReportText_WmSyncError, "WM_StartDCF", wmResult);
                        WcmSetPhase(WCM_PHASE_IRREGULAR);   // failed to issue request to ARM7 (retry failed during sequence)
                        WcmNotify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                        break;

                    case WM_ERRCODE_ILLEGAL_STATE:
                        WCMi_Warning(wcmWarningText_InvalidWmState);

                    /* Don't break here */
                    default:
                        WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
                        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
                    }
                }
                else
                {
// if an unexpected AID is received, seen as a failure and reset
                    WCMi_Printf(wcmReportText_InvalidAid, "WM_StartConnectEx");
                    WcmWmReset();
                }
            }
            break;

        default:    // unexpected state code
            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
            WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)(cb->state), __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
        wcmw->wlStatusOnConnectFail = cb->wlStatus;

    case WM_ERRCODE_NO_ENTRY:
    case WM_ERRCODE_INVALID_PARAM:
    case WM_ERRCODE_OVER_MAX_ENTRY:
// reset when wireless connection fails
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_StartConnectEx", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmSetPhase(WCM_PHASE_IDLE_TO_DCF);     // may be IRREGULAR so the phase is overwritten
        WcmWmReset();
        break;

    case WM_ERRCODE_ILLEGAL_STATE:
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // error outside of anticipated range
        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbDisconnect

Description: A handler to accept notifications from the WM library.
Receives result responses for WM_Disconnect.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbDisconnect(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
// when a clear termination request completion notification comes after termination, reset just in case
            WCMi_Printf(wcmReportText_WmDisconnected, "WM_Disconnect");
            WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
            WcmWmReset();
        }
        else
        {
// clears Auth-ID
            wcmw->authId = 0;

            WcmSetPhase(WCM_PHASE_IDLE);    // normal end to an asynchronous sequence
            WcmNotify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
        }
        break;

    case WM_ERRCODE_FAILED:
    case WM_ERRCODE_ILLEGAL_STATE:          // can come when communication is terminated at critical timing
// reset if wireless connection termination request fails
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_Disconnect", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
        WcmWmReset();
        break;

    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbStartDCF

Description: A handler to accept notifications from the WM library.
Accepts result responses for WM_StartDCF and data receipt notifications
in DCF mode.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbStartDCF(void* arg)
{
    WMStartDCFCallback*     cb = (WMStartDCFCallback*)arg;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        switch (cb->state)
        {
        case WM_STATECODE_DCF_START:
            if (wcmw->phase == WCM_PHASE_IRREGULAR)
            {
// if a DCF start complete notification comes after termination, considered a failure and reset
                WCMi_Printf(wcmReportText_WmDisconnected, "WM_StartDCF");
                WcmSetPhase(WCM_PHASE_IDLE_TO_DCF); // overwrites the phase
                WcmWmReset();
            }
            else
            {
                WcmSetPhase(WCM_PHASE_DCF);         // normal end to an asynchronous sequence
                WcmNotify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
            }
            break;

        case WM_STATECODE_DCF_IND:
// Backs up signal strength
            WCMi_ShelterRssi((u8) (cb->recvBuf->rate_rssi >> 8));

// notifies the DCF receipt to the interface with CPS
            DC_InvalidateRange(cb->recvBuf, WCM_DCF_RECV_BUF_SIZE);
            WCMi_CpsifRecvCallback(cb->recvBuf);
            break;

        default:    // unexpected state code
            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
            WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)(cb->state), __LINE__);
        }
        break;

    /* WM_ERRCODE_FAILED will not return from StartDCF, so omitted */
    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // error outside of anticipated range
        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbEndDCF

Description: A handler to accept notifications from the WM library.
Receives result responses for WM_EndDCF.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbEndDCF(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
        if (wcmw->phase == WCM_PHASE_IRREGULAR)
        {
// if a DCF end complete notification comes after termination, considered a failure and reset
            WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
            WcmWmReset();
        }
        else
        {
// issues an AP wireless connection termination request
            wmResult = WM_Disconnect(WcmWmcbDisconnect, 0);
            switch (wmResult)
            {
            case WM_ERRCODE_OPERATING:
                break;

            case WM_ERRCODE_FIFO_ERROR:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_Disconnect", wmResult);
                WcmSetPhase(WCM_PHASE_IRREGULAR);   // failed to issue request to ARM7 (retry failed during sequence)
                WcmNotify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), 0, __LINE__);
                break;

            case WM_ERRCODE_ILLEGAL_STATE:          // if communication was cut off at critical timing
// if termination from the AP end right before termination takes place, seen as a failure and reset
                WCMi_Printf(wcmReportText_WmSyncError, "WM_Disconnect", wmResult);
                WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);
                WcmWmReset();
                break;

            default:
                WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
                WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
            }
        }
        break;

    case WM_ERRCODE_FAILED:
    case WM_ERRCODE_ILLEGAL_STATE:                  // if communication was cut off at critical timing
// reset if DCF communication mode end fails
        WCMi_Printf(wcmReportText_WmAsyncError, "WM_EndDCF", cb->errcode, cb->wlCmdID, cb->wlResult);
        WcmSetPhase(WCM_PHASE_DCF_TO_IDLE);         // may be IRREGULAR so the phase is overwritten
        WcmWmReset();
        break;

    case WM_ERRCODE_WM_DISABLE:
        WCMi_Warning(wcmWarningText_InvalidWmState);

    /* Don't break here */
    default:
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);         // error outside of anticipated range
        WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
Name: WcmWmcbReset

Description: A handler to accept notifications from the WM library.
Receives result responses for WM_Result.

Arguments: arg     -   A pointer to notification parameters passed from the WM library.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmWmcbReset(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;
    WMErrCode       wmResult = WM_ERRCODE_MAX;

    switch (cb->errcode)
    {
    case WM_ERRCODE_SUCCESS:
// clears reset duplicate call management flags
        wcmw->resetting = WCM_RESETTING_OFF;

// clears Auth-ID
        wcmw->authId = 0;

        switch (wcmw->phase)
        {
        case WCM_PHASE_IDLE_TO_SEARCH:  // AP auto-search is beginning
        case WCM_PHASE_SEARCH:          // AP auto-search running
// notification that a failure in scan processing occur and there was a reset
            WcmSetPhase(WCM_PHASE_IDLE);
            WcmNotify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
            break;

        case WCM_PHASE_SEARCH_TO_IDLE:  // AP auto-search ending
// notification that the request AP auto-search end process completed
            WcmSetPhase(WCM_PHASE_IDLE);
            WcmNotify(WCM_RESULT_SUCCESS, 0, 0, __LINE__);
            break;

        case WCM_PHASE_IDLE_TO_DCF:     // connecting
// notification that a failure in connection processing occurred and there was a reset
            {
                u16 wlStatus = wcmw->wlStatusOnConnectFail;

                wcmw->wlStatusOnConnectFail = 0x0000;
#ifdef WCM_CAMOUFLAGE_RATESET
                /* measures for dealing with a failed connection from a rate set inconsistency */
                if (wlStatus == WCM_CONNECT_STATUSCODE_ILLEGAL_RATESET)
                {
                    if ((wcmw->bssDesc.rateSet.support & WCM_CAMOUFLAGE_RATESET) != WCM_CAMOUFLAGE_RATESET)
                    {
                        WCMi_Printf(wcmReportText_SupportRateset, wcmw->bssDesc.ssid);
                        wcmw->bssDesc.rateSet.support |= WCM_CAMOUFLAGE_RATESET;

// connection auto-retry
                        wmResult = WM_StartConnectEx(WcmWmcbConnect, &(wcmw->bssDesc), NULL,
                                                     ((wcmw->option & WCM_OPTION_MASK_POWER) == WCM_OPTION_POWER_ACTIVE ? FALSE : TRUE),
                                                             ((wcmw->option & WCM_OPTION_MASK_AUTH) == WCM_OPTION_AUTH_SHAREDKEY ?
                                                                     (u16) WM_AUTHMODE_SHARED_KEY : (u16) WM_AUTHMODE_OPEN_SYSTEM
                                                                     ));

// confirms synchronous process results
                        switch (wmResult)
                        {
                        case WM_ERRCODE_OPERATING:
                            break;

                        case WM_ERRCODE_FIFO_ERROR:
                            WcmSetPhase(WCM_PHASE_IRREGULAR);   // failed to issue request to ARM7 (retry failed during sequence)
                            WcmNotify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
                            break;

                        case WM_ERRCODE_ILLEGAL_STATE:
                            WCMi_Warning(wcmWarningText_InvalidWmState);

                        /* Don't break here */
                        default:
                            WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
                            WcmNotify(WCM_RESULT_FATAL_ERROR, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
                        }

                        return;
                    }
                }
#endif
                WcmSetPhase(WCM_PHASE_IDLE);
                WcmNotify(WCM_RESULT_FAILURE, &(wcmw->bssDesc), (void*)wlStatus, __LINE__);
            }
            break;

        case WCM_PHASE_DCF:         // DCF communication underway
        case WCM_PHASE_IRREGULAR:
// notification of AP disconnection
            WcmSetPhase(WCM_PHASE_IDLE);
            WcmNotify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), (void*)1, __LINE__);

            break;

        case WCM_PHASE_DCF_TO_IDLE: // disconnecting
// notification that the requested disconnect process completed
            WcmSetPhase(WCM_PHASE_IDLE);
            WcmNotify(WCM_RESULT_SUCCESS, &(wcmw->bssDesc), 0, __LINE__);
            break;

        case WCM_PHASE_TERMINATING: // forced termination underway
// after reset completes, continue processing to chain to a return to PHASE_WAIT
            wmResult = WM_PowerOff(WcmWmcbCommon);
            switch (wmResult)
            {
            case WM_ERRCODE_OPERATING:
                break;

            case WM_ERRCODE_FIFO_ERROR:
                WCMi_Printf(wcmReportText_WmSyncError, "WM_Reset", wmResult);
                WcmSetPhase(WCM_PHASE_IRREGULAR);   // failed to issue request to ARM7 (retry failed during sequence)
                WcmNotify(WCM_RESULT_FAILURE, 0, 0, __LINE__);
                break;

            case WM_ERRCODE_ILLEGAL_STATE:
                WCMi_Warning(wcmWarningText_InvalidWmState);

            /* Don't break here */
            default:
                WcmSetPhase(WCM_PHASE_FATAL_ERROR); // error outside of anticipated range
                WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
            }
            break;

        default:
            WcmSetPhase(WCM_PHASE_FATAL_ERROR);     // error outside of anticipated range
            WcmNotify(WCM_RESULT_FATAL_ERROR, 0, (void*)(wcmw->phase), __LINE__);
        }
        break;

    default:
        /* no restore is possible when reset fails */
        WcmSetPhase(WCM_PHASE_FATAL_ERROR);         // error outside of anticipated range
        WcmNotify(WCM_RESULT_FATAL_ERROR, 0, 0, __LINE__);
    }
}

/*---------------------------------------------------------------------------*
End of file
 *---------------------------------------------------------------------------*/
