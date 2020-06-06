/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_fake_child.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_fake_child.c,v $
  Revision 1.16  03/13/2006 06:33:33  yosizaki
  add MB_FAKESCAN_PARENT_BEACON.

  Revision 1.15  2006/02/20 02:31:03  seiki_masashi
  Changed name of WMGameInfo.gameNameCount_attribute to attribute 

  Revision 1.14  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.13  12/01/2005 09:15:13  takano_makoto
  Added a size check for MB_FAKE_WORK_SIZE

  Revision 1.12  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.11  02/21/2005 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.10  01/11/2005 07:41:13  takano_makoto
  fix copyright header.

  Revision 1.9  01/11/2005 02:24:33  takano_makoto
  Made correction to reset the Timeout counter when receiving data of the parent that is being locked.

  Revision 1.8  01/07/2005 02:55:30  takano_makoto
  Added debug output

  Revision 1.7  11/29/2004 05:46:01  takano_makoto
  Changes the send/receive buffer size

  Revision 1.6  11/29/2004 04:53:08  takano_makoto
  Made correction because the same region was used for sendBuffer of WM_StartMP and SendBuffer of WM_SetMPData.

  Revision 1.5  11/29/2004 02:55:30  takano_makoto
  Fixed the 32 byte alignment problem.

  Revision 1.4  11/25/2004 12:30:28  takano_makoto
  Changed print debug to OFF

  Revision 1.3  11/24/2004 13:00:35  takano_makoto
  Added error processing.

  Revision 1.2  11/23/2004 23:51:20  takano_makoto
  Removed Warning at the time of building FINALROM

  Revision 1.1  11/22/2004 12:38:30  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "mb_common.h"
#include "mb_block.h"
#include "mb_child.h"
#include "mb_private.h"
#include "mb_wm_base.h"
#include "mb_fake_child.h"
#include "mb_wm.h"

#define MB_FAKE_PRINT   (0)

#if (MB_FAKE_PRINT == 1)
#define MB_FAKE_OUTPUT    OS_TPrintf
#else
#define MB_FAKE_OUTPUT( ... ) ((void)0)
#endif

#define MY_ROUND(n, a)      (((u32) (n) + (a) - 1) & ~((a) - 1))

typedef struct
{
    u32     sendBuf[WM_SIZE_CHILD_SEND_BUFFER(MB_COMM_PARENT_RECV_MAX, FALSE) / sizeof(u32)] ATTRIBUTE_ALIGN(32);       // Send buffer  0x400 byte
    u32     recvBuf[WM_SIZE_CHILD_RECEIVE_BUFFER(MB_COMM_PARENT_SEND_MAX, FALSE) / sizeof(u32)] ATTRIBUTE_ALIGN(32);    // Receive buffer 0xF00 byte
    u32     sendDataBuf[WM_SIZE_CHILD_SEND_BUFFER(MB_COMM_PARENT_RECV_MAX, FALSE) / sizeof(u32)] ATTRIBUTE_ALIGN(32);   // Buffer for creating send data 0x400 byte
    WMBssDesc bssDescBuf ATTRIBUTE_ALIGN(32);   // Buffer for scanning parents  Needs to be 32-byte aligned   0xC0 byte
    WMScanParam scanParam ATTRIBUTE_ALIGN(32);  // Scan parameter  Needs to be 32-byte aligned  0x20 byte
    MBWMWork wmWork ATTRIBUTE_ALIGN(32);        // Work buffer for MP communication  Needs to be 32-byte aligned
    MBUserInfo userInfo;               // User information
    u8      _padding1[2];
    MBFakeScanCallbackFunc scanCallback;        // Callback for parent scan notification
    MBCommCStateCallbackFunc stateCallback;     // Callback for status notification
    u32     ggid;                      // GGID to search
    MbBeaconRecvStatus beaconRecvStatus;        // beacon receive status
    BOOL    scanning;                  // Scanning flag
    BOOL    endScanBusy;               // Flag for processing scan termination
    BOOL    locking;                   // Locking flag
    BOOL    endFlag;                   // MP termination process request
    u32     targetGgid;                // GGID of binary to request
    u16     targetFileNo;              // FileNo of binary to request
    u16     c_comm_state;              // Child status
    u8      _padding2[16];
}
MBFakeWork;

SDK_COMPILER_ASSERT(sizeof(MBFakeWork) <= MB_FAKE_WORK_SIZE);


//============================================================================
// Function Prototype Declarations
//============================================================================

static void MBFi_EndComplete(void);

static void MBFi_CommChangeChildState(u16 state, void *arg);
static void MBFi_SendCallback(u16 state, void *arg);
static void MBFi_ErrorCallback(u16 apiid, u16 errcode, BOOL isApiError);

/* For scanning*/
static void MBFi_StateInStartScan(void);
static void MBFi_StateOutStartScanParent(void *arg);
static void MBFi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index);

static void MBFi_ScanCallback(u16 state, MBGameInfoRecvList * gInfop, int index);
static void MBFi_ScanErrorCallback(u16 apiid, u16 errcode);
static void MBFi_ScanLock(u8 *macAddr);
static void MBFi_ScanUnlock(void);
static BOOL RotateChannel(void);

static void MBFi_StateInEndScan(void);
static void MBFi_StateOutEndScan(void *arg);

/* For entry processes*/
static void MBFi_WMCallback(u16 type, void *arg);
static void MBFi_CommChildRecvData(WMPortRecvCallback *cb);
static void MBFi_CommChildSendData(void);

/* For debug output*/
#if (MB_FAKE_PRINT == 1)
static void MBFi_PrintMBCallbackType(u16 type);
static void MBFi_PrintMBCommCallbacyType(u16 type);
#else
#define MBFi_PrintMBCallbackType(a)         ((void)0)
#define MBFi_PrintMBCommCallbackType(a)     ((void)0)
#endif

//============================================================================
// Local Variable Declaration
//============================================================================

static vu16 mbf_initialize;
static MBFakeWork *mbf_work;



//============================================================================
// function definitions
//============================================================================


/*---------------------------------------------------------------------------*
  Name:         MB_FakeInit

  Description:  Initialization function of the fake multiboot child

  Arguments:    buf     Pointer to the work area necessary for fake child.
                        The necessary size can be obtained with MB_FakeGetWorkSize().
                user    Pointer to the user information of a child
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeInit(void *buf, const MBUserInfo *user)
{
    SDK_NULL_ASSERT(buf);
    SDK_ASSERT(((u32)buf & 0x1F) == 0); // Checks for the 32-byte alignment

    if (mbf_initialize)
    {
        OS_TWarning("MB_FakeInit multiply called\n");
        return;
    }
    mbf_initialize = 1;

    MB_FAKE_OUTPUT("MB_Fake Initialized\n");

    // Buffer clear
    MI_CpuClear8(buf, sizeof(MBFakeWork));

    mbf_work = (MBFakeWork *) buf;
    mbf_work->c_comm_state = MB_COMM_CSTATE_NONE;

    MI_CpuCopy8(user, &mbf_work->userInfo, sizeof(MBUserInfo));
    // Sets the work buffer for receiving beacon
    MBi_SetBeaconRecvStatusBuffer(&mbf_work->beaconRecvStatus);
    // Initializes the game information receive status
    MB_InitRecvGameInfoStatus();
    // Sets the function for scan lock
    MBi_SetScanLockFunc(MBFi_ScanLock, MBFi_ScanUnlock);
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeEnd

  Description:  Termination function of the fake multiboot child.

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeEnd(void)
{
    if (mbf_work->endFlag)
    {
        // Does not do anything if the termination process has been started
        return;
    }

    mbf_work->endFlag = TRUE;

    switch (mbf_work->c_comm_state)
    {
    case MB_COMM_CSTATE_NONE:
        // End the scan if scan is being performed
        if (mbf_work->scanning)
        {
            mbf_work->scanning = FALSE;
        }
        // End without doing anything if before starting scanning or after completing EndScan
        else if (!mbf_work->endScanBusy)
        {
            MBFi_EndComplete();
        }
        else
        {
            // Wait for completion if scan termination process is being performed
        }
        break;

    case MB_COMM_CSTATE_INIT_COMPLETE:
    case MB_COMM_CSTATE_CONNECT:
    case MB_COMM_CSTATE_CONNECT_FAILED:
    case MB_COMM_CSTATE_DISCONNECTED_BY_PARENT:
    case MB_COMM_CSTATE_REQ_REFUSED:
    case MB_COMM_CSTATE_ERROR:
    case MB_COMM_CSTATE_MEMBER_FULL:
        // Reset to end.
        MBi_WMReset();
        break;

    case MB_COMM_CSTATE_REQ_ENABLE:
    case MB_COMM_CSTATE_DLINFO_ACCEPTED:
    case MB_COMM_CSTATE_RECV_PROCEED:
    case MB_COMM_CSTATE_RECV_COMPLETE:
        // End safely in order starting from MP
        MBi_WMDisconnect();
        break;

    case MB_COMM_CSTATE_BOOTREQ_ACCEPTED:
        // Wait for completion of disconnection
        break;

    case MB_COMM_CSTATE_BOOT_READY:
    case MB_COMM_CSTATE_CANCELLED:
        // End because it is already in IDLE state.
        MBFi_EndComplete();
        break;

    default:
        OS_TPanic("MB fake child is in Illegal State\n");
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_EndComplete

  Description:  Termination process of the fake child library

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_EndComplete(void)
{
    mbf_initialize = 0;
    mbf_work->endFlag = 0;
    // Clears port callback
    MBi_WMClearCallback();

    // Notify the user of termination
    MBFi_CommChangeChildState(MB_COMM_CSTATE_FAKE_END, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeGetWorkSize

  Description:  Obtains the work size necessary for the fake child library.

  Arguments:    None.

  Returns:      Size of the necessary work buffer
 *---------------------------------------------------------------------------*/
u32 MB_FakeGetWorkSize(void)
{
    return sizeof(MBFakeWork);
}



/*---------------------------------------------------------------------------*
  Name:         MB_FakeSetCStateCallback

  Description:  Sets the callback function to notify the state change of the fake child.

  Arguments:    callback    Callback function to set

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeSetCStateCallback(MBCommCStateCallbackFunc callback)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    mbf_work->stateCallback = callback;

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_CommChangeChildState

  Description:  Function that changes the child state.
                Returns the callback to the user program at the same time as the status change.

  Arguments:    state   Status to set
                arg     Argument for the callback

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_CommChangeChildState(u16 state, void *arg)
{
#pragma unused( arg )
    OSIntrMode enabled;

    static const char *const CSTATE_NAME_STRING[] = {
        "MB_COMM_CSTATE_NONE",
        "MB_COMM_CSTATE_INIT_COMPLETE",
        "MB_COMM_CSTATE_CONNECT",
        "MB_COMM_CSTATE_CONNECT_FAILED",
        "MB_COMM_CSTATE_DISCONNECTED_BY_PARENT",
        "MB_COMM_CSTATE_REQ_ENABLE",
        "MB_COMM_CSTATE_REQ_REFUSED",
        "MB_COMM_CSTATE_DLINFO_ACCEPTED",
        "MB_COMM_CSTATE_RECV_PROCEED",
        "MB_COMM_CSTATE_RECV_COMPLETE",
        "MB_COMM_CSTATE_BOOTREQ_ACCEPTED",
        "MB_COMM_CSTATE_BOOT_READY",
        "MB_COMM_CSTATE_CANCELLED",
        "MB_COMM_CSTATE_AUTHENTICATION_FAILED",
        "MB_COMM_CSTATE_MEMBER_FULL",
        "MB_COMM_CSTATE_GAMEINFO_VALIDATED",
        "MB_COMM_CSTATE_GAMEINFO_INVALIDATED",
        "MB_COMM_CSTATE_GAMEINFO_LOST",
        "MB_COMM_CSTATE_GAMEINFO_LIST_FULL",
        "MB_COMM_CSTATE_ERROR",
        "MB_COMM_CSTATE_FAKE_END",
    };

    MB_FAKE_OUTPUT("state %s -> %s\n", CSTATE_NAME_STRING[mbf_work->c_comm_state],
                   CSTATE_NAME_STRING[state]);

    enabled = OS_DisableInterrupts();
    mbf_work->c_comm_state = state;
    (void)OS_RestoreInterrupts(enabled);
    MBFi_SendCallback(state, arg);
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_SendCallback

  Description:  Sends callback.

  Arguments:    state   Status of the fake child.
                arg      Callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBFi_SendCallback(u16 state, void *arg)
{
    if (mbf_work->stateCallback == NULL)
    {
        return;
    }
    mbf_work->stateCallback(state, arg);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_ErrorCallback

  Description:  Notifies the error generation with callback.

  Arguments:    apiid   Notifies the API that caused the error with callback.
                errcode  Notifies the error cause code with callback.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBFi_ErrorCallback(u16 apiid, u16 errcode, BOOL isApiError)
{
    MBErrorStatus cb;
    u16     error_type;

    if (mbf_work->stateCallback == NULL)
    {
        return;
    }

    if (isApiError)
    {
        // Error at the time of calling API function.
        switch (errcode)
        {
        case WM_ERRCODE_INVALID_PARAM:
        case WM_ERRCODE_FAILED:
        case WM_ERRCODE_WM_DISABLE:
        case WM_ERRCODE_NO_DATASET:
        case WM_ERRCODE_FIFO_ERROR:
        case WM_ERRCODE_TIMEOUT:
            error_type = MB_ERRCODE_FATAL;
            break;
        case WM_ERRCODE_OPERATING:
        case WM_ERRCODE_ILLEGAL_STATE:
        case WM_ERRCODE_NO_CHILD:
        case WM_ERRCODE_OVER_MAX_ENTRY:
        case WM_ERRCODE_NO_ENTRY:
        case WM_ERRCODE_INVALID_POLLBITMAP:
        case WM_ERRCODE_NO_DATA:
        case WM_ERRCODE_SEND_QUEUE_FULL:
        case WM_ERRCODE_SEND_FAILED:
        default:
            error_type = MB_ERRCODE_WM_FAILURE;
            break;
        }
    }
    else
    {
        // Error at the time of callback.
        switch (apiid)
        {
        case WM_APIID_INITIALIZE:
        case WM_APIID_SET_LIFETIME:
        case WM_APIID_SET_P_PARAM:
        case WM_APIID_SET_BEACON_IND:
        case WM_APIID_START_PARENT:
        case WM_APIID_START_MP:
        case WM_APIID_SET_MP_DATA:
        case WM_APIID_START_DCF:
        case WM_APIID_SET_DCF_DATA:
        case WM_APIID_DISCONNECT:
        case WM_APIID_START_KS:
            /* The above errors are important to WM initialization*/
            error_type = MB_ERRCODE_FATAL;
            break;
        case WM_APIID_RESET:
        case WM_APIID_END:
        default:
            /* Other errors are returned as callback errors*/
            error_type = MB_ERRCODE_WM_FAILURE;
            break;
        }
    }

    MB_FAKE_OUTPUT("MBFi_ErrorCallback apiid = 0x%x, errcode = 0x%x\n", apiid, errcode);
    cb.errcode = error_type;
    mbf_work->stateCallback(MB_COMM_CSTATE_ERROR, &cb);
}


//============================================================================
// Function for scan
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         MB_FakeStartScanParent

  Description:  Starts scanning multiboot parent

  Arguments:    callback    Function that notifies callback of parent scan
                ggid        GGID of the MB parent to search. If GGID is different, it would not be found.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeStartScanParent(MBFakeScanCallbackFunc callback, u32 ggid)
{
    MB_FAKE_OUTPUT("%s\n", __func__);

    mbf_work->scanCallback = callback;
    mbf_work->ggid = ggid;
    mbf_work->scanParam.channel = 0;
    mbf_work->scanParam.scanBuf = &mbf_work->bssDescBuf;
    mbf_work->scanning = TRUE;
    mbf_work->locking = FALSE;
    mbf_work->beaconRecvStatus.nowScanTargetFlag = FALSE;
    MBFi_StateInStartScan();
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_StateInStartScan

  Description:  Issues scan command.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_StateInStartScan(void)
{
    WMErrCode result;

    // Issue WM_StartScan
    if (mbf_work->locking)
    {
        mbf_work->scanParam.maxChannelTime = MB_SCAN_TIME_LOCKING;
    }
    else
    {
        static const u8 ANY_PARENT[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        WM_CopyBssid(ANY_PARENT, mbf_work->scanParam.bssid);

        mbf_work->scanParam.maxChannelTime = MB_SCAN_TIME_NORMAL;
        // Sets a channel if it is not locked
        if (!RotateChannel())
        {
            // The state which wireless is not usable
            MBFi_ScanErrorCallback(WM_APIID_MEASURE_CHANNEL, 0);
            return;
        }
    }

    if (mbf_work->scanParam.channel == 0)
    {
        mbf_work->scanParam.channel = 1;
    }
    result = WM_StartScan(MBFi_StateOutStartScanParent, &mbf_work->scanParam);
    if (result != WM_ERRCODE_OPERATING)
    {
        MBFi_ScanErrorCallback(WM_APIID_START_SCAN, result);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_StateOutStartScanParent

  Description:  Callback function of WM_StartScan

  Arguments:    arg     Callback argument of WM_StartScan

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_StateOutStartScanParent(void *arg)
{
    WMStartScanCallback *cb = (WMStartScanCallback *)arg;

    if (WM_ERRCODE_SUCCESS != cb->errcode)
    {
        MBFi_ScanErrorCallback(WM_APIID_START_SCAN, cb->errcode);
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_SCAN_START:
        break;

    case WM_STATECODE_PARENT_FOUND:
        // Store the parent information obtained with scan
        {
            // Analyze if GGID is the same
            if (cb->gameInfo.ggid == mbf_work->ggid)
            {
                DC_InvalidateRange(&mbf_work->bssDescBuf, WM_BSS_DESC_SIZE);
                (void)MB_RecvGameInfoBeacon(MBFi_CommBeaconRecvCallback, cb->linkLevel,
                                            &mbf_work->bssDescBuf);
                MB_CountGameInfoLifetime(MBFi_CommBeaconRecvCallback, TRUE);
            }
        }
        // don't break;

    case WM_STATECODE_PARENT_NOT_FOUND:
        /* Lifetime count of parent game information*/
        MB_CountGameInfoLifetime(MBFi_CommBeaconRecvCallback, FALSE);

        if (mbf_work->scanning)
        {
            MBFi_StateInStartScan();
        }
        else
        {
            MBFi_StateInEndScan();
        }
        break;

    default:
        MBFi_ScanErrorCallback(WM_APIID_START_SCAN, WM_ERRCODE_FAILED);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_ScanCallback

  Description:  Uses a callback to notify the user program about the parent
                scan information.

  Arguments:    state   Status to notify
                gInfop  Pointer to the parent information that was found
                index   Control number of the parent managed internally

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_ScanCallback(u16 state, MBGameInfoRecvList * gInfop, int index)
{
    MBFakeScanCallback cb;

    if (mbf_work->scanCallback == NULL)
    {
        return;
    }

    cb.index = (u16)index;
    if (gInfop != NULL)
    {
        cb.gameInfo = &gInfop->gameInfo;
        cb.bssDesc = &gInfop->bssDesc;
    }
    else
    {
        cb.gameInfo = NULL;
        cb.bssDesc = NULL;
    }
    mbf_work->scanCallback(state, &cb);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_ScanErrorCallback

  Description:  Notifies the error generation at the time of scanning with callback.

  Arguments:    apiid    APIID that caused the error
                errcode  Error cause code

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_ScanErrorCallback(u16 apiid, u16 errcode)
{
    MBFakeScanErrorCallback cb;

    if (mbf_work->scanCallback == NULL)
    {
        return;
    }

    cb.apiid = apiid;
    cb.errcode = errcode;
    mbf_work->scanCallback(MB_FAKESCAN_API_ERROR, &cb);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_ScanLock

  Description:  Locks the parent to scan

  Arguments:    macAddr  : MAC address of the parent to lock

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_ScanLock(u8 *macAddr)
{
    MB_FAKE_OUTPUT("Scan Locked\n");
    mbf_work->locking = TRUE;
    WM_CopyBssid(macAddr, mbf_work->scanParam.bssid);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_ScanUnlock

  Description:  Releases the lock of the parent to scan

  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_ScanUnlock(void)
{
    MB_FAKE_OUTPUT("Scan Unlocked\n");
    mbf_work->locking = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_CommBeaconRecvCallback

  Description:  child beacon receive callback

  Arguments:    msg    : beacon receive message
                gInfop : parent game information
                index  : beacon index

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_CommBeaconRecvCallback(MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index)
{
    switch (msg)
    {
    case MB_BC_MSG_GINFO_VALIDATED:
        MBFi_ScanCallback(MB_FAKESCAN_PARENT_FOUND, (void *)gInfop, index);
        MB_FAKE_OUTPUT("Parent Info validated\n");
        break;
    case MB_BC_MSG_GINFO_INVALIDATED:
        MB_FAKE_OUTPUT("Parent Info invalidate\n");
        break;
    case MB_BC_MSG_GINFO_LOST:
        MBFi_ScanCallback(MB_FAKESCAN_PARENT_LOST, (void *)gInfop, index);
        MB_FAKE_OUTPUT("Parent Info Lost\n");
        break;
    case MB_BC_MSG_GINFO_LIST_FULL:
        MB_FAKE_OUTPUT("Parent List Full\n");
        break;
    case MB_BC_MSG_GINFO_BEACON:
        /* Acknowledge each time existing parent beacon is detected*/
        MBFi_ScanCallback(MB_FAKESCAN_PARENT_BEACON, (void *)gInfop, index);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RotateChannel

  Description:  Rotates channels

  Arguments:    None.

  Returns:      If an available channel was not found, this returns FALSE.
 *---------------------------------------------------------------------------*/
static BOOL RotateChannel(void)
{
    u16     allowedChannel = WM_GetAllowedChannel();
    if (allowedChannel == 0x8000 || allowedChannel == 0)
    {
        return FALSE;
    }

    // Rotate channels
    mbf_work->scanParam.channel++;
    while (TRUE)
    {
        mbf_work->scanParam.channel++;
        if (mbf_work->scanParam.channel > 16)
        {
            mbf_work->scanParam.channel = 1;
        }
        if (allowedChannel & (0x0001 << (mbf_work->scanParam.channel - 1)))
        {
            break;
        }
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeEndScan

  Description:  End scanning for parent

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_FakeEndScan(void)
{
    mbf_work->scanning = FALSE;
    mbf_work->endScanBusy = TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_StateInEndScan

  Description:  Issues the parent scan termination command

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_StateInEndScan(void)
{
    WMErrCode result;

    result = WM_EndScan(MBFi_StateOutEndScan);
    if (result != WM_ERRCODE_OPERATING)
    {
        MBFi_ScanErrorCallback(WM_APIID_END_SCAN, result);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_StateOutEndScan

  Description:  Parent scanning termination callback

  Arguments:    arg     Callback argument of WM_EndScan

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_StateOutEndScan(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        MBFi_ScanErrorCallback(WM_APIID_END_SCAN, cb->errcode);
        return;
    }
    mbf_work->endScanBusy = FALSE;

    if (mbf_work->endFlag)
    {
        MBFi_EndComplete();
        return;
    }

    MBFi_ScanCallback(MB_FAKESCAN_END_SCAN, NULL, 0);

}



//============================================================================
// Process for entering to parent
//============================================================================

/*---------------------------------------------------------------------------*
  Name:         MB_FakeEntryToParent

  Description:  Starts the entry to the parent

  Arguments:    index   Gives internal control number of the parent for the entry as an argument.
                        For this argument, you should pass the "index"
                        obtained with the MB_FakeStartScanParent callback.

  Returns:      TRUE if the index of the argument is valid.
                FALSE if it is not valid.
 *---------------------------------------------------------------------------*/
BOOL MB_FakeEntryToParent(u16 index)
{
    MBGameInfoRecvList *info;

    info = MB_GetGameInfoRecvList(index);

    if (info == NULL)
    {
        return FALSE;
    }
    mbf_work->targetGgid = info->gameInfo.ggid;
    mbf_work->targetFileNo = info->gameInfo.fileNo;

    MBi_WMSetBuffer(&mbf_work->wmWork);
    MBi_WMSetCallback(MBFi_WMCallback);
    MBFi_CommChangeChildState(MB_COMM_CSTATE_INIT_COMPLETE, NULL);
    MI_CpuCopy8(&info->bssDesc, &mbf_work->bssDescBuf, sizeof(WMBssDesc));
    MBi_WMStartConnect(&mbf_work->bssDescBuf);

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_WMCallback

  Description:  Callback function of MP communication

  Arguments:    type        callback type
                arg      Callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_WMCallback(u16 type, void *arg)
{
#pragma unused( arg )

    switch (type)
    {
        //--------------------------------------------
        // Notification that connection to parent failed  
    case MB_CALLBACK_CONNECT_FAILED:
        MBFi_CommChangeChildState(MB_COMM_CSTATE_CONNECT_FAILED, NULL);
        break;
        //--------------------------------------------
        // Notification that connection to parent is complete  
    case MB_CALLBACK_CONNECTED_TO_PARENT:
        MB_FAKE_OUTPUT("connect to parent\n");
        MBFi_CommChangeChildState(MB_COMM_CSTATE_CONNECT, arg);
        MBi_ChildStartMP((u16 *)mbf_work->sendBuf, (u16 *)mbf_work->recvBuf);
        break;
        //--------------------------------------------
        // Notification of disconnection from parent 
    case MB_CALLBACK_DISCONNECTED_FROM_PARENT:
        MBFi_CommChangeChildState(MB_COMM_CSTATE_DISCONNECTED_BY_PARENT, arg);
        break;
        //--------------------------------------------
        // Notify that the MP communication started
    case MB_CALLBACK_MP_STARTED:
        break;
        //-----------------------------
        // Notify that data send is possible
    case MB_CALLBACK_MP_SEND_ENABLE:
        {
            // Send data setting process
            MBFi_CommChildSendData();
        }
        break;
        //-----------------------------
        // Notify that data receive is possible
    case MB_CALLBACK_MP_CHILD_RECV:
        {
            MBFi_CommChildRecvData((WMPortRecvCallback *)arg);
        }
        break;
        //-----------------------------
        // Notify that the disconnection from the child is completed.
    case MB_CALLBACK_DISCONNECT_COMPLETE:
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED)
        {
            // If it is after the boot request is received, it changes into the boot complete status
            MBFi_CommChangeChildState(MB_COMM_CSTATE_BOOT_READY, NULL);
        }
        else
        {
            // If it is before the boot request is received, it changes into the cancel status
            MBFi_CommChangeChildState(MB_COMM_CSTATE_CANCELLED, NULL);
        }
        if (mbf_work->endFlag)
        {
            MBFi_EndComplete();
        }
        break;
        //------------------------------
        // MP communication notification
    case MB_CALLBACK_MP_CHILD_SENT:
    case MB_CALLBACK_MP_CHILD_SENT_TIMEOUT:
    case MB_CALLBACK_MP_CHILD_SENT_ERR:
        // Process is not necessary because this is only notifying the communication condition of MP level.
        break;
        //------------------------------
        // Handles error
    case MB_CALLBACK_API_ERROR:
        {
            MBErrorCallback *cb = (MBErrorCallback *) arg;

            MBFi_ErrorCallback(cb->apiid, cb->errcode, TRUE);
        }
        break;
    case MB_CALLBACK_ERROR:
        {
            MBErrorCallback *cb = (MBErrorCallback *) arg;

            MBFi_ErrorCallback(cb->apiid, cb->errcode, FALSE);
        }
        break;
    default:
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBFi_CommChildRecvData

  Description:  Process for receiving data from a parent

  Arguments:    cb  Callback argument of the port callback

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_CommChildRecvData(WMPortRecvCallback *cb)
{
    MBCommParentBlockHeader hd;

    // Header analysis
    (void)MBi_SetRecvBufferFromParent(&hd, (u8 *)cb->data);

//    MBFi_PrintMBCommCallbacyType( hd.type );

    switch (hd.type)                   // state transition depending on receive block type
    {
    case MB_COMM_TYPE_PARENT_SENDSTART:
        // send start message from parent
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_CONNECT)
        {
            MB_FAKE_OUTPUT("Allowd to request file from parent!\n");
            MBFi_CommChangeChildState(MB_COMM_CSTATE_REQ_ENABLE, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_KICKREQ: // kick message from parent
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_REQ_REFUSED, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_MEMBER_FULL:      // members from parent exceeded message   
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_MEMBER_FULL, NULL);
        }
        break;

    case MB_COMM_TYPE_PARENT_DL_FILEINFO:
        // receive MbDownloadFileInfoHeader
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_REQ_ENABLE)
        {
            /* Pass the received MbDownloadFileInfo as an argument. */
            MBFi_CommChangeChildState(MB_COMM_CSTATE_DLINFO_ACCEPTED, NULL);
        }

        break;

    case MB_COMM_TYPE_PARENT_DATA:
        /* Receive block data*/
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_DLINFO_ACCEPTED)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_RECV_PROCEED, NULL);
        }

        if (mbf_work->c_comm_state == MB_COMM_CSTATE_RECV_PROCEED)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_RECV_COMPLETE, NULL);      // receive complete
        }
        break;

    case MB_COMM_TYPE_PARENT_BOOTREQ:
        if (mbf_work->c_comm_state == MB_COMM_CSTATE_RECV_COMPLETE)
        {
            MBFi_CommChangeChildState(MB_COMM_CSTATE_BOOTREQ_ACCEPTED, NULL);
        }
        else if (mbf_work->c_comm_state == MB_COMM_CSTATE_BOOTREQ_ACCEPTED)
        {
            MBi_WMDisconnect();        // end communication
        }
        break;
    default:
        break;
    }

    return;
}



/*---------------------------------------------------------------------------*
  Name:         MBi_CommChildSendData

  Description:  Sends child data.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/

static void MBFi_CommChildSendData(void)
{
    MBCommChildBlockHeader hd;
    WMErrCode errcode = WM_ERRCODE_SUCCESS;
    u16     pollbmp = 0xffff;

    switch (mbf_work->c_comm_state)
    {
    case MB_COMM_CSTATE_REQ_ENABLE:
        {
            MBCommRequestData req_data;
            u8     *databuf;

            req_data.ggid = mbf_work->targetGgid;
            req_data.version = MB_IPL_VERSION;
            req_data.fileid = (u8)mbf_work->targetFileNo;
            MI_CpuCopy8(&mbf_work->userInfo, &req_data.userinfo, sizeof(MBUserInfo));

            hd.type = MB_COMM_TYPE_CHILD_FILEREQ;
            hd.req_data.piece = MBi_SendRequestDataPiece(hd.req_data.data, &req_data);
            databuf = MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
            if (databuf == NULL)
            {
                return;
            }
            errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        }
        break;

    case MB_COMM_CSTATE_DLINFO_ACCEPTED:
        // DownloadInfo reception message
        hd.type = MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
        errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        break;

    case MB_COMM_CSTATE_RECV_PROCEED:
        break;

    case MB_COMM_CSTATE_RECV_COMPLETE:
        // block transfer stop message (send continues until BOOTREQ comes from parent)
        hd.type = MB_COMM_TYPE_CHILD_STOPREQ;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
        errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        break;

    case MB_COMM_CSTATE_BOOTREQ_ACCEPTED:
        hd.type = MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
        errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        break;
    default:
        // send DUMMY MP to establish MP communication
        hd.type = MB_COMM_TYPE_DUMMY;
        (void)MBi_MakeChildSendBuffer(&hd, (u8 *)mbf_work->sendDataBuf);
        errcode = MBi_MPSendToParent(MB_COMM_CHILD_HEADER_SIZE, pollbmp, mbf_work->sendDataBuf);
        break;

    }
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeGetParentGameInfo

  Description:  Obtains the game information of the parent from the parent list

  Arguments:    index           Parent index
                pGameInfo Pointer to the region that obtains the parent information

  Returns:      TRUE if the index of a valid parent;
                FALSE if the index of an invalid parent.
 *---------------------------------------------------------------------------*/
BOOL MB_FakeGetParentGameInfo(u16 index, MBGameInfo *pGameInfo)
{
    MBGameInfoRecvList *parentInfo;
    OSIntrMode enabled;

    if (index >= MB_GAME_INFO_RECV_LIST_NUM)
    {
        return FALSE;
    }

    enabled = OS_DisableInterrupts();

    parentInfo = MB_GetGameInfoRecvList(index);
    if (parentInfo == NULL)
    {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }
    MI_CpuCopy8(&parentInfo->gameInfo, pGameInfo, sizeof(MBGameInfo));
    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeGetParentBssDesc

  Description:  Obtains BssDesc of the parent from the parent list.

  Arguments:    index           Parent index
                pBssDesc        Pointer to the region that obtains the parent information

  Returns:      TRUE if the index of a valid parent;
                FALSE if the index of an invalid parent.
 *---------------------------------------------------------------------------*/
BOOL MB_FakeGetParentBssDesc(u16 index, WMBssDesc *pBssDesc)
{
    MBGameInfoRecvList *parentInfo;
    OSIntrMode enabled;

    if (index >= MB_GAME_INFO_RECV_LIST_NUM)
    {
        return FALSE;
    }

    enabled = OS_DisableInterrupts();

    parentInfo = MB_GetGameInfoRecvList(index);
    if (parentInfo == NULL)
    {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }
    MI_CpuCopy8(&parentInfo->bssDesc, pBssDesc, sizeof(WMBssDesc));
    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         MB_FakeReadParentBssDesc

  Description:  Obtains the information for WM_StartConnect function based on the parent BssDesc of the parent list.

  Arguments:    index           Parent index
                pBssDesc        Pointer to the region that obtains the parent information
                parent_max_size Maximum send size of  the MP parent
                child_max_size  Maximum send size of the MP child
                ks_flag         Flag for setting key sharing
                cs_flag         Flag for setting continuous transfer

  Returns:      TRUE if the index of a valid parent;
                FALSE if the index of an invalid parent.
 *---------------------------------------------------------------------------*/
BOOL MB_FakeReadParentBssDesc(u16 index, WMBssDesc *pBssDesc, u16 parent_max_size,
                              u16 child_max_size, BOOL ks_flag, BOOL cs_flag)
{
    BOOL    result;

    SDK_NULL_ASSERT(pBssDesc);

    result = MB_FakeGetParentBssDesc(index, pBssDesc);

    if (!result)
    {
        return FALSE;
    }

    pBssDesc->gameInfoLength = 0x10;
    pBssDesc->gameInfo.userGameInfoLength = 0;
    pBssDesc->gameInfo.parentMaxSize = parent_max_size;
    pBssDesc->gameInfo.childMaxSize = child_max_size;
    pBssDesc->gameInfo.attribute = (u8)((ks_flag ? WM_ATTR_FLAG_KS : 0) |
                                        (cs_flag ? WM_ATTR_FLAG_CS : 0) |
                                        WM_ATTR_FLAG_ENTRY);
    return TRUE;
}



#if (MB_FAKE_PRINT == 1)

/*---------------------------------------------------------------------------*
  Name:         MBFi_PrintMBCallbackType

  Description:  Debug outputs the callback type from MB_WM

  Arguments:    type        callback type

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_PrintMBCallbackType(u16 type)
{
#pragma unused( type )
    static const char *const CALLBACK_NAME[] = {
        "MB_CALLBACK_CHILD_CONNECTED",
        "MB_CALLBACK_CHILD_DISCONNECTED",
        "MB_CALLBACK_MP_PARENT_SENT",
        "MB_CALLBACK_MP_PARENT_RECV",
        "MB_CALLBACK_PARENT_FOUND",
        "MB_CALLBACK_PARENT_NOT_FOUND",
        "MB_CALLBACK_CONNECTED_TO_PARENT",
        "MB_CALLBACK_DISCONNECTED",
        "MB_CALLBACK_MP_CHILD_SENT",
        "MB_CALLBACK_MP_CHILD_RECV",
        "MB_CALLBACK_DISCONNECTED_FROM_PARENT",
        "MB_CALLBACK_CONNECT_FAILED",
        "MB_CALLBACK_DCF_CHILD_SENT",
        "MB_CALLBACK_DCF_CHILD_SENT_ERR",
        "MB_CALLBACK_DCF_CHILD_RECV",
        "MB_CALLBACK_DISCONNECT_COMPLETE",
        "MB_CALLBACK_DISCONNECT_FAILED",
        "MB_CALLBACK_END_COMPLETE",
        "MB_CALLBACK_MP_CHILD_SENT_ERR",
        "MB_CALLBACK_MP_PARENT_SENT_ERR",
        "MB_CALLBACK_MP_STARTED",
        "MB_CALLBACK_INIT_COMPLETE",
        "MB_CALLBACK_END_MP_COMPLETE",
        "MB_CALLBACK_SET_GAMEINFO_COMPLETE",
        "MB_CALLBACK_SET_GAMEINFO_FAILED",
        "MB_CALLBACK_MP_SEND_ENABLE",
        "MB_CALLBACK_PARENT_STARTED",
        "MB_CALLBACK_BEACON_LOST",
        "MB_CALLBACK_BEACON_SENT",
        "MB_CALLBACK_BEACON_RECV",
        "MB_CALLBACK_MP_SEND_DISABLE",
        "MB_CALLBACK_DISASSOCIATE",
        "MB_CALLBACK_REASSOCIATE",
        "MB_CALLBACK_AUTHENTICATE",
        "MB_CALLBACK_SET_LIFETIME",
        "MB_CALLBACK_DCF_STARTED",
        "MB_CALLBACK_DCF_SENT",
        "MB_CALLBACK_DCF_SENT_ERR",
        "MB_CALLBACK_DCF_RECV",
        "MB_CALLBACK_DCF_END",
        "MB_CALLBACK_MPACK_IND",
        "MB_CALLBACK_MP_CHILD_SENT_TIMEOUT",
        "MB_CALLBACK_SEND_QUEUE_FULL_ERR",
        "MB_CALLBACK_API_ERROR",
        "MB_CALLBACK_ERROR",
    };

    MB_FAKE_OUTPUT("RecvCallback %s\n", CALLBACK_NAME[type]);
}

/*---------------------------------------------------------------------------*
  Name:         MBFi_PrintMBCommCallbacyType

  Description:  Debug outputs the message type of the packet from a parent

  Arguments:    type Message type

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBFi_PrintMBCommCallbacyType(u16 type)
{
#pragma unused( type )
    static const char *const MB_TYPE_STRING[] = {
        "MB_COMM_TYPE_DUMMY",          //      0
        "MB_COMM_TYPE_PARENT_SENDSTART",        //      1
        "MB_COMM_TYPE_PARENT_KICKREQ", //      2
        "MB_COMM_TYPE_PARENT_DL_FILEINFO",      //      3
        "MB_COMM_TYPE_PARENT_DATA",    //      4
        "MB_COMM_TYPE_PARENT_BOOTREQ", //      5
        "MB_COMM_TYPE_PARENT_MEMBER_FULL",      //      6
        "MB_COMM_TYPE_CHILD_FILEREQ",  //      7
        "MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO",   //      8
        "MB_COMM_TYPE_CHILD_CONTINUE", //      9
        "MB_COMM_TYPE_CHILD_STOPREQ",  //      10
        "MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED",  //      11
    };

    MB_FAKE_OUTPUT("RECV %s\n", MB_TYPE_STRING[type]);
}

#endif
