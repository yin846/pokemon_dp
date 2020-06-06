/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_etc.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_etc.c,v $
  Revision 1.19  02/21/2006 01:10:23  okubata_ryoma
  Copyright fix

  Revision 1.18  2006/02/20 08:43:10  seiki_masashi
  Changed the WM_SetGameInfo function's argument names.

  Revision 1.17  11/22/2005 10:02:26  seiki_masashi
  changes to use the WM_SendCommandDirect function

  Revision 1.16  11/02/2005 02:21:32  ooe
  Added WM_StartTestRxMode and WM_StopTestRxMode.

  Revision 1.15  07/27/2005 08:01:18  ooe
  Added WM_SetPowerSaveMode.

  Revision 1.14  03/07/2005 01:00:36  terui
  Revised parameter range checking for WM_SetWEPKeyEx.

  Revision 1.13  02/28/2005 06:26:35  yosizaki
  do-indent.

  Revision 1.12  02/03/2005 00:40:36  ooe
  Revised copyright notice

  Revision 1.11  02/03/2005 12:33:23  ooe
  Added WM_SetWEPKeyEx.

  Revision 1.10  12/20/2004 03:49:59  seiki_masashi
  Made correction because the cache store of wepkey was missing in WM_SetWEPKey.

  Revision 1.9  11/17/2004 08:30:24  terui
  In WM_SetGameInfo, revised MI_CpuCopy32to MI_CpuCopy16

  Revision 1.8  11/09/2004 01:03:44  seiki_masashi
  Implemented timeout on NULL response

  Revision 1.7  11/02/2004 08:27:14  terui
  Comments revised.

  Revision 1.6  10/22/2004 05:37:28  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.5  10/18/2004 12:09:16  terui
  Changed the WM_SetBeaconPeriod temporarily to be nonpublic

  Revision 1.4  10/04/2004 14:40:25  terui
  Standardized game group IDs as type u32.

  Revision 1.3  10/01/2004 05:03:16  terui
  Modified so that the features can be restricted with the WM_ENABLE_TESTMODE switch

  Revision 1.2  09/20/2004 11:27:36  ooe
  Added WMSP_SetBeaconPeriod.

  Revision 1.1  09/10/2004 12:03:30  terui
  New upload reflecting the breakup of wm.c.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     WM_SIZE_TEMP_USR_GAME_INFO_BUF 128


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static u32 tmpUserGameInfoBuf[WM_SIZE_TEMP_USR_GAME_INFO_BUF / sizeof(u32)] ATTRIBUTE_ALIGN(32);


#ifdef  WM_ENABLE_TESTMODE
/*---------------------------------------------------------------------------*
  Name:         WM_StartTestMode

  Description:  Starts communication in test mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                signal      -   0: no modulation (data=0), 1: no modulation (data=1), 2:PN15 grade,
                                3:01 pattern (with scrambling), 4:01 pattern (without scrambling)
                rate        -   1:1Mbps, 2:2Mbps
                channel     -   Specifies the channel to send data (1 to 14).

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartTestMode(WMCallbackFunc callback, u16 signal, u16 rate, u16 channel)
{
    WMErrCode result;

    // Cannot execute if not in IDLE state.
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_START_TESTMODE, callback);

    // notify ARM7 with FIFO
    {
        WMStartTestModeReq Req;

        Req.apiid = WM_APIID_START_TESTMODE;
        Req.signal = signal;
        Req.rate = rate;
        Req.channel = channel;

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        if (result != WM_ERRCODE_SUCCESS)
        {
            return result;
        }
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StopTestMode

  Description:  Stops communication in test mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StopTestMode(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot run unless state is TESTMODE
    result = WMi_CheckState(WM_STATE_TESTMODE);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_STOP_TESTMODE, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_STOP_TESTMODE, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartTestRxMode

  Description:  Starts receiving data in test mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                channel     -   Specifies the channel for receiving data (1 to 14).

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartTestRxMode(WMCallbackFunc callback, u16 channel)
{
    WMErrCode result;

    // Cannot execute if not in IDLE state.
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_START_TESTRXMODE, callback);

    // notify ARM7 with FIFO
    {
        WMStartTestRxModeReq Req;

        Req.apiid = WM_APIID_START_TESTRXMODE;
        Req.channel = channel;

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        if (result != WM_ERRCODE_SUCCESS)
        {
            return result;
        }
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StopTestRxMode

  Description:  Stops receiving data in test mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StopTestRxMode(WMCallbackFunc callback)
{
    WMErrCode result;

    // Cannot run unless state is TESTMODE
    result = WMi_CheckState(WM_STATE_TESTMODE_RX);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_STOP_TESTRXMODE, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_STOP_TESTRXMODE, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         WM_SetWEPKey

  Description:  Sets the encryption feature and encryption key.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                wepmode     -   0: no encryption feature
                                1: RC4(40bit) encyrption mode
                                2: RC4(104bit) encryption mode
                                3: RC4(128bit) encryption mode
                wepkey      -   Pointer to the encryption key data (80 bytes).
                                Key data consists of 4 sets of data. Each of them is 20 bytes.
                                Of each 20 bytes,
                                 5 bytes in 40-bit mode
                                13 bytes in 104-bit mode
                                16 bytes in 128-bit mode
                                are used.
                                Also the entity of this data is forcibly stored in cache.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetWEPKey(WMCallbackFunc callback, u16 wepmode, const u16 *wepkey)
{
    WMErrCode result;

    // Confirm that wireless hardware has started
    result = WMi_CheckIdle();
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (wepmode > 3)
    {
        WM_WARNING("Parameter \"wepmode\" must be less than %d.\n", 3);
        return WM_ERRCODE_INVALID_PARAM;
    }

    if (wepmode != WM_WEPMODE_NO)
    {
        // Check the parameter
        if (wepkey == NULL)
        {
            WM_WARNING("Parameter \"wepkey\" must not be NULL.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
        if ((u32)wepkey & 0x01f)
        {
            // align check is a warning only, not an error
            WM_WARNING("Parameter \"wepkey\" is not 32-byte aligned.\n");
        }

        // write specified buffer cache
        DC_StoreRange((void *)wepkey, WM_SIZE_WEPKEY);
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_WEPKEY, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_WEPKEY, 2, (u32)wepmode, (u32)wepkey);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetWEPKeyEx

  Description:  Sets encryption functionality, encryption key, encryption key ID.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                wepmode     -   0: no encryption feature
                                1: RC4(40bit) encyrption mode
                                2: RC4(104bit) encryption mode
                                3: RC4(128bit) encryption mode
                wepkeyid    -   Selects which of 4 specified wepkey to use.
                                Specifies as 0-3.
                wepkey      -   Pointer to the encryption key data (80 bytes).
                                Key data consists of 4 sets of data. Each of them is 20 bytes.
                                Of each 20 bytes,
                                 5 bytes in 40-bit mode
                                13 bytes in 104-bit mode
                                16 bytes in 128-bit mode
                                are used.
                                Also the entity of this data is forcibly stored in cache.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetWEPKeyEx(WMCallbackFunc callback, u16 wepmode, u16 wepkeyid, const u8 *wepkey)
{
    WMErrCode result;

    // Confirm that wireless hardware has started
    result = WMi_CheckIdle();
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (wepmode > 3)
    {
        WM_WARNING("Parameter \"wepmode\" must be less than %d.\n", 3);
        return WM_ERRCODE_INVALID_PARAM;
    }

    if (wepmode != WM_WEPMODE_NO)
    {
        // Check the parameter
        if (wepkey == NULL)
        {
            WM_WARNING("Parameter \"wepkey\" must not be NULL.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
        if (wepkeyid > 3)
        {
            WM_WARNING("Parameter \"wepkeyid\" must be less than %d.\n", 3);
        }
        if ((u32)wepkey & 0x01f)
        {
            // align check is a warning only, not an error
            WM_WARNING("Parameter \"wepkey\" is not 32-byte aligned.\n");
        }

        // write specified buffer cache
        DC_StoreRange((void *)wepkey, WM_SIZE_WEPKEY);
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_WEPKEY_EX, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_WEPKEY_EX, 3, (u32)wepmode, (u32)wepkey, (u32)wepkeyid);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetGameInfo

  Description:  Sets the game information. Initial value is set by WM_SetParentParameter, so
                use when changing this.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                userGameInfo    -   Pointer to the user game information.
                userGameInfoSize -  Size of the user game information.
                ggid        -   Game group ID
                tgid        -   Temporary group ID
                attr        -   Flag group. Sets the logical sum of the following flags.
                                    WM_ATTR_FLAG_ENTRY - Entry permitted
                                    WM_ATTR_FLAG_MB    - Accepting multiboot
                                    WM_ATTR_FLAG_KS    - Key sharing
                                    WM_ATTR_FLAG_CS    - Continuous transfer mode
  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode
WM_SetGameInfo(WMCallbackFunc callback, const u16 *userGameInfo, u16 userGameInfoSize,
               u32 ggid, u16 tgid, u8 attr)
{
    WMErrCode result;

    // Confirm status as active parent
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_MP_PARENT);
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (userGameInfo == NULL)
    {
        WM_WARNING("Parameter \"userGameInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (userGameInfoSize > WM_SIZE_USER_GAMEINFO)
    {
        WM_WARNING("Parameter \"userGameInfoSize\" must be less than %d.\n", WM_SIZE_USER_GAMEINFO);
        return WM_ERRCODE_INVALID_PARAM;
    }

    // Temporarily save the specified buffer
    MI_CpuCopy16((void *)userGameInfo, (void *)tmpUserGameInfoBuf, userGameInfoSize);
    DC_StoreRange((void *)tmpUserGameInfoBuf, userGameInfoSize);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_GAMEINFO, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_GAMEINFO, 5,
                             (u32)tmpUserGameInfoBuf,
                             (u32)userGameInfoSize, (u32)ggid, (u32)tgid, (u32)attr);
    if (result != WM_ERRCODE_SUCCESS)
    {
        return result;
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetBeaconIndication

  Description:  Switches enable/disable of the beacon send/receive indicate.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                flag        -   0: Invalid
                                1: Valid

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetBeaconIndication(WMCallbackFunc callback, u16 flag)
{
    WMErrCode result;

    // Confirm that wireless hardware has started
    result = WMi_CheckIdle();
    WM_CHECK_RESULT(result);

    // Check the parameter
    if ((0 != flag) && (1 != flag))
    {
        WM_WARNING("Parameter \"flag\" must be \"0\" of \"1\".\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_BEACON_IND, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_BEACON_IND, 1, (u32)flag);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetLifeTime

  Description:  Sets lifetime.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                tableNumber -   CAM table number that sets lifetime (0xFFFF and all tables)
                camLifeTime -   CAM lifetime (100ms units: invalid when 0xFFFF)
                frameLifeTime - lifetime beacon period of set frame (100ms units: invalid when 0xFFFF)
                mpLifeTime  -    MP communication lifetime (100ms units: 0xFFFF to disable)

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode
WM_SetLifeTime(WMCallbackFunc callback, u16 tableNumber, u16 camLifeTime, u16 frameLifeTime,
               u16 mpLifeTime)
{
    WMErrCode result;

    // Confirm that wireless hardware has started
    result = WMi_CheckIdle();
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_LIFETIME, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_LIFETIME, 4,
                             (u32)tableNumber,
                             (u32)camLifeTime, (u32)frameLifeTime, (u32)mpLifeTime);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_MeasureChannel

  Description:  Measure channel usage.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                ccaMode     -   CCA operation mode
                                0: Only carrier sense. ED threshold value is ignored.
                                1: Valid only with ED threshold value
                                2: Logical product of carrier sense and ED threshold value.
                                3: Logical sum of carrier sense and ED threshold value.
                EDThreshold -   ED threshold (0 - 61) -60dBm to -80dBm
                channel     -   channel to check (only one channel per one MeasureChannel)
                measureTime -   time to check

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode
WM_MeasureChannel(WMCallbackFunc callback, u16 ccaMode, u16 edThreshold, u16 channel,
                  u16 measureTime)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Cannot execute if not in IDLE state.
    result = WMi_CheckState(WM_STATE_IDLE);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_MEASURE_CHANNEL, callback);

    // notify ARM7 with FIFO
    {
        WMMeasureChannelReq Req;

        Req.apiid = WM_APIID_MEASURE_CHANNEL;
        Req.ccaMode = ccaMode;
        Req.edThreshold = edThreshold;
        Req.channel = channel;
        Req.measureTime = measureTime;

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }
    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_InitWirelessCounter

  Description:  Initializes WirelessCounter.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_InitWirelessCounter(WMCallbackFunc callback)
{
    WMErrCode result;

    // Confirm that wireless hardware has started
    result = WMi_CheckIdle();
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_INIT_W_COUNTER, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_INIT_W_COUNTER, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetWirelessCounter

  Description:  Gets send/receive frame count and send/receive error frame count of Wireless NIC.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_GetWirelessCounter(WMCallbackFunc callback)
{
    WMErrCode result;

    // Confirm that wireless hardware has started
    result = WMi_CheckIdle();
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_GET_W_COUNTER, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_GET_W_COUNTER, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetEntry

  Description:  Switches accepting/refusing of a connection from a child as a parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                enabled     -   Entry permitted / not permitted flag. TRUE: permitted, FALSE: not permitted.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetEntry(WMCallbackFunc callback, BOOL enabled)
{
    WMErrCode result;

    // not executable unless parent
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_MP_PARENT);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_ENTRY, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_ENTRY, 1, (u32)enabled);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_SetBeaconPeriod

  Description:  Changes the beacon intervals.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                beaconPeriod -  Beacon interval (10 to1000 TU (1024us))

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_SetBeaconPeriod(WMCallbackFunc callback, u16 beaconPeriod)
{
    WMErrCode result;

    // not executable unless parent
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_MP_PARENT);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_BEACON_PERIOD, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_BEACON_PERIOD, 1, (u32)beaconPeriod);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetPowerSaveMode()

  Description:  Sets the PowerSaveMode

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                powerSave   -   When using power save mode, TRUE. When not using it, FALSE.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetPowerSaveMode(WMCallbackFunc callback, BOOL powerSave)
{
    WMErrCode result;

    // This function cannot be executed except when in the DCF child state
    result = WMi_CheckState(WM_STATE_DCF_CHILD);
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_PS_MODE, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_PS_MODE, 1, (u32)powerSave);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
