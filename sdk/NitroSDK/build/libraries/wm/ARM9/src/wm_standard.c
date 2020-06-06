/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_standard.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_standard.c,v $
  Revision 1.23  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.22  11/22/2005 09:00:52  seiki_masashi
  changes to use the WM_SendCommandDirect function

  Revision 1.21  11/01/2005 11:48:42  seiki_masashi
  Added debug code

  Revision 1.20  09/08/2005 00:49:06  seiki_masashi
  Support specification of ssidMatchLength as a parameter to the function WM_StartScanEx.

  Revision 1.19  04/11/2005 01:15:20  seiki_masashi
  Added WMi_StartParentEx.

  Revision 1.18  03/16/2005 06:18:16  terui
  Corrected registration table index of the WM_StartScanEx callback function.

  Revision 1.17  03/06/2005 23:59:31  terui
  Changes size of cache in WM_StartConnectEx to proper value.

  Revision 1.16  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.15  02/14/2005 11:43:06  terui
  In WM_StartScan[Ex], deleted caching of "param" parameter where not necessary.

  Revision 1.14  02/14/2005 06:38:45  terui
  Moved check of data transmission time to MP start.

  Revision 1.13  02/08/2005 06:50:15  ooe
  Changed WM_StartScanEx specifications

  Revision 1.12  2005/01/25 01:57:25  yasu
  Revised Copyright notation

  Revision 1.11  01/17/2005 00:59:36  ooe
  Replaced WM_StartConnect with WM_StartConnectEx

  Revision 1.10  10/29/2004 02:53:25  terui
  Changed WM_StartScanEx temporarily to be a nonpublic function

  Revision 1.9  10/27/2004 04:23:57  ooe
  Deleted unnecessary OS_Printf.

  Revision 1.8  10/26/2004 08:21:44  ooe
  Added WM_StartScanEx

  Revision 1.7  10/22/2004 04:37:28  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.6  10/05/2004 09:45:28  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.5  10/01/2004 11:05:22  terui
  Made a subtle change to the warning message

  Revision 1.4  09/15/2004 08:31:43  terui
  Corrected mistakes in method used to determine align check during debug builds.

  Revision 1.3  09/15/2004 02:19:48  terui
  Replaced the warning about MP send/receive size with the warning required when sending/receiving MP packets    

  Revision 1.2  09/10/2004 12:05:40  seiki_masashi
  Changed it so the number of bytes sent for key sharing is concealed in the library.

  Revision 1.1  09/10/2004 11:03:30  terui
  New upload reflecting the breakup of wm.c.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static BOOL WmCheckParentParameter(const WMParentParam *param);


/*---------------------------------------------------------------------------*
  Name:         WM_Enable

  Description:  Makes the wireless hardware to the usable state.
                Internal state changes from READY state to STOP state.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Enable(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot execute if not in READY state
    result = WMi_CheckState(WM_STATE_READY);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_ENABLE, callback);

    // notify ARM7 with FIFO
    {
        WMArm9Buf *p = WMi_GetSystemWork();

        result = WMi_SendCommand(WM_APIID_ENABLE, 3,
                                 (u32)(p->WM7), (u32)(p->status), (u32)(p->fifo7to9));
        WM_CHECK_RESULT(result);
    }

    // normal end
    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_Disable

  Description:  Changes the wireless hardware to the use prohibited state.
                Internal state changes from STOP state to READY state.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Disable(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot execute if not in STOP state
    result = WMi_CheckState(WM_STATE_STOP);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_DISABLE, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_DISABLE, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_PowerOn

  Description:  Starts up the wireless hardware.
                Internal state changes from STOP state to IDLE state.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_PowerOn(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot execute if not in STOP state
    result = WMi_CheckState(WM_STATE_STOP);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_POWER_ON, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_POWER_ON, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_PowerOff

  Description:  Shuts down the wireless hardware.
                Internal state changes from IDLE state to STOP state.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_PowerOff(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot execute if not in IDLE state.
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_POWER_OFF, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_POWER_OFF, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_Initialize

  Description:  Performs the WM initialization process.

  Arguments:    wmSysBuf    -   Pointer to the buffer allocated by the caller.
                                As much as WM_SYSTEM_BUF_SIZE is required for buffer size.
                callback    -   Callback function that is called when the asynchronous process completes.
                dmaNo       -   DMA number used by WM

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Initialize(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo)
{
    WMErrCode result;

    // Execute the initialization
    result = WM_Init(wmSysBuf, dmaNo);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_INITIALIZE, callback);

    // notify ARM7 with FIFO
    {
        WMArm9Buf *p = WMi_GetSystemWork();

        result = WMi_SendCommand(WM_APIID_INITIALIZE, 3,
                                 (u32)(p->WM7), (u32)(p->status), (u32)(p->fifo7to9));
        WM_CHECK_RESULT(result);
    }

    // normal end
    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_Reset

  Description:  Resets the wireless library, and restores the state immediately after the initialization.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Reset(WMCallbackFunc callback)
{
    WMErrCode result;

    // Confirm that wireless hardware has started
    result = WMi_CheckIdle();
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_RESET, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_RESET, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_End

  Description:  Ends the wireless library.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_End(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot execute if not in IDLE state.
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_END, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_END, 0);
    WM_CHECK_RESULT(result);

    // Perform the process for ending the WM library on the ARM9 side inside the callback

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetParentParameter

  Description:  Sets the parent information.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                pparaBuf    -   Pointer to the structure that indicates the parent information
                                Notice that pparaBuf and pparaBuf->userGameInfo entities are forcibly
                                cache stored.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetParentParameter(WMCallbackFunc callback, const WMParentParam *pparaBuf)
{
    WMErrCode result;

    // Cannot execute if not in IDLE state.
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (pparaBuf == NULL)
    {
        WM_WARNING("Parameter \"pparaBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)pparaBuf & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"pparaBuf\" is not 32-byte aligned.\n");
    }
    if (pparaBuf->userGameInfoLength > 0)
    {
        if (pparaBuf->userGameInfo == NULL)
        {
            WM_WARNING("Parameter \"pparaBuf->userGameInfo\" must not be NULL.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
        if ((u32)(pparaBuf->userGameInfo) & 0x01f)
        {
            // align check is a warning only, not an error
            WM_WARNING("Parameter \"pparaBuf->userGameInfo\" is not 32-byte aligned.\n");
        }
    }

    // Check maximum data transfer length
    if ((pparaBuf->parentMaxSize +
         (pparaBuf->KS_Flag ? WM_SIZE_KS_PARENT_DATA + WM_SIZE_MP_PARENT_PADDING : 0) >
         WM_SIZE_MP_DATA_MAX)
        || (pparaBuf->childMaxSize +
            (pparaBuf->KS_Flag ? WM_SIZE_KS_CHILD_DATA + WM_SIZE_MP_CHILD_PADDING : 0) >
            WM_SIZE_MP_DATA_MAX))
    {
        WM_WARNING("Transfer data size is over %d byte.\n", WM_SIZE_MP_DATA_MAX);
        return WM_ERRCODE_INVALID_PARAM;
    }
    (void)WmCheckParentParameter(pparaBuf);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_P_PARAM, callback);

    // write specified buffer cache
    DC_StoreRange((void *)pparaBuf, WM_PARENT_PARAM_SIZE);
    if (pparaBuf->userGameInfoLength > 0)
    {
        DC_StoreRange(pparaBuf->userGameInfo, pparaBuf->userGameInfoLength);
    }

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_P_PARAM, 1, (u32)pparaBuf);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WmCheckParentParameter

  Description:  This is a debug function, used to check whether the set parent
                parameter falls within the admissible range shown in the guidelines.

  Arguments:    param   -   Pointer to set parent parameter being checked.

  Returns:      BOOL    -   Returns TRUE if no problem, or FALSE if setting
                            value not admissible.
 *---------------------------------------------------------------------------*/
static BOOL WmCheckParentParameter(const WMParentParam *param)
{
    // User game information can be up to 112 bytes
    if (param->userGameInfoLength > WM_SIZE_USER_GAMEINFO)
    {
        OS_TWarning("User gameInfo length must be less than %d .\n", WM_SIZE_USER_GAMEINFO);
        return FALSE;
    }
    // Beacon transmission interval is 10 to 1000
    if ((param->beaconPeriod < 10) || (param->beaconPeriod > 1000))
    {
        OS_TWarning("Beacon send period must be between 10 and 1000 .\n");
        return FALSE;
    }
    // Connection channels are 1 to 14
    if ((param->channel < 1) || (param->channel > 14))
    {
        OS_TWarning("Channel must be between 1 and 14 .\n");
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_StartParentEx

  Description:  Starts the communication a parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                powerSave   -   When using power save mode, TRUE. When not using it, FALSE.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_StartParentEx(WMCallbackFunc callback, BOOL powerSave)
{
    WMErrCode result;

    // Cannot execute if not in IDLE state.
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    {
        WMArm9Buf *w9b = WMi_GetSystemWork();
#ifdef WM_DEBUG
        if (w9b->connectedAidBitmap != 0)
        {
            WM_DPRINTF("Warning: connectedAidBitmap should be 0, but %04x",
                       w9b->connectedAidBitmap);
        }
#endif
        w9b->myAid = 0;
        w9b->connectedAidBitmap = 0;
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_START_PARENT, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_START_PARENT, 1, (u32)powerSave);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartParent

  Description:  Starts the communication a parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartParent(WMCallbackFunc callback)
{
    return WMi_StartParentEx(callback, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndParent

  Description:  Stops the communication as a parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndParent(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot execute if not in PARENT state
    result = WMi_CheckState(WM_STATE_PARENT);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_END_PARENT, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_END_PARENT, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartScan

  Description:  Starts scanning for a parent as a child.
                Obtains one parent information with one call back.
                It can be called repeatedly without calling WM_EndScan.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                param       -   Pointer to the structure that indicates the scan information.
                                ARM7 directly writes scan result information to param->scanBuf.
                                so it must match cache line.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartScan(WMCallbackFunc callback, const WMScanParam *param)
{
    WMErrCode result;

    // Not executable outside of IDLE CALSS1 SCAN state
    result = WMi_CheckStateEx(3, WM_STATE_IDLE, WM_STATE_CLASS1, WM_STATE_SCAN);
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (param == NULL)
    {
        WM_WARNING("Parameter \"param\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (param->scanBuf == NULL)
    {
        WM_WARNING("Parameter \"param->scanBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((param->channel < 1) || (param->channel > 14))
    {
        WM_WARNING("Parameter \"param->channel\" must be between 1 and 14.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)(param->scanBuf) & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"param->scanBuf\" is not 32-byte aligned.\n");
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_START_SCAN, callback);

    // notify ARM7 with FIFO
    {
        WMStartScanReq Req;

        Req.apiid = WM_APIID_START_SCAN;
        Req.channel = param->channel;
        Req.scanBuf = param->scanBuf;
        Req.maxChannelTime = param->maxChannelTime;
        Req.bssid[0] = param->bssid[0];
        Req.bssid[1] = param->bssid[1];
        Req.bssid[2] = param->bssid[2];
        Req.bssid[3] = param->bssid[3];
        Req.bssid[4] = param->bssid[4];
        Req.bssid[5] = param->bssid[5];
        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartScanEx

  Description:  Starts scanning for a parent as a child.
                Gets multiple parent information with one call.
                It can be called repeatedly without calling WM_EndScan.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                param       -   Pointer to the structure that indicates the scan information.
                           ARM7 directly writes scan result information to param->scanBuf.
                           so it must match cache line.

  Returns:      int      - Returns WM_ERRCODE_* type process results.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartScanEx(WMCallbackFunc callback, const WMScanExParam *param)
{
    WMErrCode result;

    // Not executable outside of IDLE CALSS1 SCAN state
    result = WMi_CheckStateEx(3, WM_STATE_IDLE, WM_STATE_CLASS1, WM_STATE_SCAN);
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (param == NULL)
    {
        WM_WARNING("Parameter \"param\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (param->scanBuf == NULL)
    {
        WM_WARNING("Parameter \"param->scanBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (param->scanBufSize > WM_SIZE_SCAN_EX_BUF)
    {
        WM_WARNING
            ("Parameter \"param->scanBufSize\" must be less than or equal to WM_SIZE_SCAN_EX_BUF.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)(param->scanBuf) & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"param->scanBuf\" is not 32-byte aligned.\n");
    }
    if (param->ssidLength > WM_SIZE_SSID)
    {
        WM_WARNING("Parameter \"param->ssidLength\" must be less than or equal to WM_SIZE_SSID.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (param->scanType != WM_SCANTYPE_ACTIVE && param->scanType != WM_SCANTYPE_PASSIVE
        && param->scanType != WM_SCANTYPE_ACTIVE_CUSTOM
        && param->scanType != WM_SCANTYPE_PASSIVE_CUSTOM)
    {
        WM_WARNING
            ("Parameter \"param->scanType\" must be WM_SCANTYPE_PASSIVE or WM_SCANTYPE_ACTIVE.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((param->scanType == WM_SCANTYPE_ACTIVE_CUSTOM
         || param->scanType == WM_SCANTYPE_PASSIVE_CUSTOM) && param->ssidMatchLength > WM_SIZE_SSID)
    {
        WM_WARNING
            ("Parameter \"param->ssidMatchLength\" must be less than or equal to WM_SIZE_SSID.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_START_SCAN_EX, callback);

    // notify ARM7 with FIFO
    {
        WMStartScanExReq Req;

        Req.apiid = WM_APIID_START_SCAN_EX;
        Req.channelList = param->channelList;
        Req.scanBuf = param->scanBuf;
        Req.scanBufSize = param->scanBufSize;
        Req.maxChannelTime = param->maxChannelTime;
        MI_CpuCopy8(param->bssid, Req.bssid, WM_SIZE_MACADDR);
        Req.scanType = param->scanType;
        Req.ssidMatchLength = param->ssidMatchLength;
        Req.ssidLength = param->ssidLength;
        MI_CpuCopy8(param->ssid, Req.ssid, WM_SIZE_SSID);

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndScan

  Description:  Stops the scan process as a child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndScan(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot execute if not in SCAN state
    result = WMi_CheckState(WM_STATE_SCAN);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_END_SCAN, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_END_SCAN, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartConnectEx

  Description:  Starts the connection to the parent as a child

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                pInfo       -   Information of the parent to be connected.
                                Specify the structure obtained with WM_StartScan.
                                Note that the entity of this structure is forcibly
                                cache stored.
                ssid        -   Child information to notify the parent (24Byte(WM_SIZE_CHILD_SSID) fixed size)
                powerSave   -   When using power save mode, TRUE. When not using it, FALSE.
                authMode    -   Authentication mode selections
                                  WM_AUTHMODE_OPEN_SYSTEM : OPEN SYSTEM mode
                                  WM_AUTHMODE_SHARED_KEY  : SHARED KEY mode

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode
WM_StartConnectEx(WMCallbackFunc callback, const WMBssDesc *pInfo, const u8 *ssid,
                  BOOL powerSave, const u16 authMode)
{
    WMErrCode result;

    // Cannot execute if not in IDLE state.
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (pInfo == NULL)
    {
        WM_WARNING("Parameter \"pInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)pInfo & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"pInfo\" is not 32-byte aligned.\n");
    }
    if ((authMode != WM_AUTHMODE_OPEN_SYSTEM) && (authMode != WM_AUTHMODE_SHARED_KEY))
    {
        WM_WARNING
            ("Parameter \"authMode\" must be WM_AUTHMODE_OPEN_SYSTEM or WM_AUTHMODE_SHARED_KEY.\n");
    }

    // write specified buffer cache
    DC_StoreRange((void *)pInfo, (u32)(pInfo->length * 2));

    {
        WMArm9Buf *w9b = WMi_GetSystemWork();
#ifdef WM_DEBUG
        if (w9b->connectedAidBitmap != 0)
        {
            WM_DPRINTF("Warning: connectedAidBitmap should be 0, but %04x",
                       w9b->connectedAidBitmap);
        }
#endif
        w9b->myAid = 0;
        w9b->connectedAidBitmap = 0;
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_START_CONNECT, callback);

    // notify ARM7 with FIFO
    {
        WMStartConnectReq Req;

        Req.apiid = WM_APIID_START_CONNECT;
        Req.pInfo = (WMBssDesc *)pInfo;
        if (ssid != NULL)
        {
            MI_CpuCopy8(ssid, Req.ssid, WM_SIZE_CHILD_SSID);
        }
        else
        {
            MI_CpuClear8(Req.ssid, WM_SIZE_CHILD_SSID);
        }
        Req.powerSave = powerSave;
        Req.authMode = authMode;

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_Disconnect

  Description:  Cuts off the communication that had been established.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                aid         -   AID of the communication target to be disconnected.
                                As for parent, disconnects children with ID 1 to 15 individually.
                                As for child, cuts off the communication with the parent with ID 0.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Disconnect(WMCallbackFunc callback, u16 aid)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Check the state
    result = WMi_CheckStateEx(5,
                              WM_STATE_PARENT, WM_STATE_MP_PARENT,
                              WM_STATE_CHILD, WM_STATE_MP_CHILD, WM_STATE_DCF_CHILD);
    WM_CHECK_RESULT(result);

    if (                               // p->status->state cache is discarded
           (p->status->state == WM_STATE_PARENT) || (p->status->state == WM_STATE_MP_PARENT))
    {
        // For parent
        if ((aid < 1) || (aid > WM_NUM_MAX_CHILD))
        {
            WM_WARNING("Parameter \"aid\" must be between 1 and %d.\n", WM_NUM_MAX_CHILD);
            return WM_ERRCODE_INVALID_PARAM;
        }
        DC_InvalidateRange(&(p->status->child_bitmap), 2);
        if (!(p->status->child_bitmap & (0x0001 << aid)))
        {
            WM_WARNING("There is no child that have aid %d.\n", aid);
            return WM_ERRCODE_NO_CHILD;
        }
    }
    else
    {
        // For child
        if (aid != 0)
        {
            WM_WARNING("Now child mode , so aid must be 0.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_DISCONNECT, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_DISCONNECT, 1, (u32)(0x0001 << aid));
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_DisconnectChildren

  Description:  Disconnects each of the children which the communication has been established. Function exclusive for parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                aidBitmap   -   AID bit field of the child to be disconnected.
                                Least significant bit is ignored. Bit 1-15 indicate respectively children with AID 1-15.
                                Bits 1-15 show the children having AID 1-15
                                Bit that indicates the child that is not connected is ignored, so specify 0xFFFF for disconnecting all children
                                regardless of their connection status.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_DisconnectChildren(WMCallbackFunc callback, u16 aidBitmap)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_MP_PARENT);
    WM_CHECK_RESULT(result);

    // Check the parameter
    DC_InvalidateRange(&(p->status->child_bitmap), 2);
    if (!(p->status->child_bitmap & aidBitmap & 0xfffe))
    {
        WM_WARNING("There is no child that is included in \"aidBitmap\" %04x_.\n", aidBitmap);
        return WM_ERRCODE_NO_CHILD;
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_DISCONNECT, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_DISCONNECT, 1, (u32)aidBitmap);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
