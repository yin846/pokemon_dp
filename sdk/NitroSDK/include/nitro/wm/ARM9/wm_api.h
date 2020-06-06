/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - include
  File:     wm_api.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_api.h,v $
  Revision 1.72  2006/02/20 08:42:43  seiki_masashi
  Changed the WM_SetGameInfo function's argument names.

  Revision 1.71  2006/02/20 07:48:19  seiki_masashi
  Added WM_IsValidGameInfo and WM_IsValidGameBeacon functions

  Revision 1.70  2006/01/18 02:12:39  kitase_hirotake
  do-indent

  Revision 1.69  11/25/2005 01:46:09  seiki_masashi
  Moved mask to within the WMMPParam structure

  Revision 1.68  11/24/2005 04:02:42  seiki_masashi
  Added definitions related to the WM_SetMPParameter function

  Revision 1.67  11/02/2005 02:32:23  ooe
  Added WM_StartTestRxMode and WM_StopTestRxMode.

  Revision 1.66  11/01/2005 09:59:44  seiki_masashi
  Changed WM_IsMP and WM_GetMPReadyAIDs to non-public functions.

  Revision 1.65  10/28/2005 12:18:55  seiki_masashi
  Implemented accessor to WMStatus structure.

  Revision 1.64  07/27/2005 07:52:42  ooe
  Added WM_SetPowerSaveMode.

  Revision 1.63  06/07/2005 06:44:48  seiki_masashi
  Revised to reduce special processing related to key sharing

  Revision 1.62  04/11/2005 09:51:18  yosizaki
  add WM_GetNextTgid().

  Revision 1.61  04/11/2005 02:16:32  seiki_masashi
  Added WMi_StartParentEx function

  Revision 1.60  03/17/2005 06:09:13  ooe
  Added WM_GetOtherElements

  Revision 1.59  03/03/2005 15:44:57  seiki_masashi
  Added WM_DISABLE_KEYSHARING switch

  Revision 1.58  02/28/2005 06:26:35  yosizaki
  do-indent.

  Revision 1.57  02/14/2005 12:48:18  terui
  Fixed comments related to WM_StartScan[Ex].

  Revision 1.56  02/14/2005 07:35:08  terui
  Added WMi_CheckMpPacketTimeRequired function.

  Revision 1.55  02/08/2005 07:43:42  ooe
  Changed WM_StartScanEx argument type (WMScanExParam)

  Revision 1.54  02/03/2005 12:42:51  ooe
  Added WM_SetWEPKeyEx.

  Revision 1.53  02/02/2005 08:46:20  takano_makoto
  Added WM_CopyBssid16, WM_IsBssidEqual16.

  Revision 1.52  01/27/2005 13:03:27  takano_makoto
  typo correction

  Revision 1.51  01/27/2005 10:11:49  takano_makoto
  Revised WM_CopyBssId, WM_IsBssidEqual to process in u8 units.

  Revision 1.50  01/27/2005 09:06:21  takano_makoto
  Added ASSERT.

  Revision 1.49  01/17/2005 02:04:09  ooe
  Added WM_StartConnectEx. Made WM_StartConnect inline.
  Added WM_SIZE_DCF_CHILD_SEND_BUFFER for DFC send buffer size calculation.

  Revision 1.48  01/11/2005 08:54:48  takano_makoto
  fix copyright header.

  Revision 1.47  01/06/2005 03:05:10  seiki_masashi
  Made WM_StartMP, WM_SetMPData, WM_SetMPDataToPort to inline

  Revision 1.46  12/22/2004 03:29:53  terui
  Added WM_IsExistAllowedChannel

  Revision 1.45  11/17/2004 08:31:04  terui
  Only fix comment.

  Revision 1.44  11/10/2004 06:41:49  takano_makoto
  Revised WM_CopyBssid, WM_IsBssidEqual arguments to const pointers.

  Revision 1.43  11/10/2004 01:59:12  takano_makoto
  Added WM_IsBssidEqual, WM_CopyBssid.

  Revision 1.42  11/09/2004 01:03:59  seiki_masashi
  Implemented timeout on NULL response

  Revision 1.41  11/02/2004 08:27:38  terui
  Comments revised.

  Revision 1.40  10/29/2004 03:53:37  terui
  Changed WM_StartScanEx temporarily to be a nonpublic function

  Revision 1.39  10/26/2004 09:22:09  ooe
  Added WM_StartScanEx

  Revision 1.38  10/22/2004 05:36:47  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.37  10/18/2004 12:08:54  terui
  Changed the WM_SetBeaconPeriod temporarily to be nonpublic

  Revision 1.36  10/18/2004 05:21:00  terui
  Fix comment only.

  Revision 1.35  10/18/2004 05:17:46  terui
  Added a specification to randomize the beacon interval and the scan interval.

  Revision 1.34  10/15/2004 15:09:16  terui
  Fix comment.

  Revision 1.33  10/05/2004 10:45:28  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.32  10/04/2004 14:40:25  terui
  Standardized game group IDs as type u32.

  Revision 1.31  10/03/2004 08:51:10  ooe
  Merged with ipl_branch. Added support for ignoreFatalError flag.

  Revision 1.30  10/01/2004 05:09:26  terui
  Restricted features regarding the test mode operation with WM_ENABLE_TESTMODE switch

  Revision 1.29  09/20/2004 11:30:21  ooe
  Added WM_SetBeaconPeriod.

  Revision 1.28  09/16/2004 03:59:28  seiki_masashi
  Fixed WM_GetMPReceiveBufferSize bug.

  Revision 1.27  09/13/2004 05:09:12  seiki_masashi
  Added WM_SetMPFrequency.

  Revision 1.26  09/11/2004 05:09:38  terui
  Only fix comment.

  Revision 1.25  09/10/2004 13:04:37  seiki_masashi
  Changed it so the number of bytes sent for key sharing is concealed in the library.

  Revision 1.24  09/10/2004 05:56:16  seiki_masashi
  Changed WM_SIZE_MP_*_BUFFER to an accurate value.

  Revision 1.23  09/09/2004 08:24:11  seiki_masashi
  Added WM_SetMPDataToPortEx.

  Revision 1.22  09/09/2004 02:58:20  terui
  Added prototype declaration in accordance with segmentation of initialization and end processes.

  Revision 1.21  09/02/2004 10:26:49  seiki_masashi
  fix typo.

  Revision 1.20  09/02/2004 10:15:33  terui
  Added WMi_GetStatusAddress function.

  Revision 1.19  08/30/2004 01:53:09  terui
  Move definition about LinkLevel to wm.h

  Revision 1.18  08/27/2004 07:44:20  seiki_masashi
  Added defaultRetryCount and fixFreqMode to WM_StartMPEx arguments.

  Revision 1.17  08/27/2004 03:16:00  terui
  Only fix comment.

  Revision 1.16  08/25/2004 09:20:38  seiki_masashi
  Added WM_StatMPEx.

  Revision 1.15  08/25/2004 08:59:06  terui
  Add WM_ConvGgid*.

  Revision 1.14  08/24/2004 14:20:04  terui
  Added enumerated type showing link strength acquisition function and strength.

  Revision 1.13  08/23/2004 05:39:10  ooe
  Changed WM_StartConnect() specifications. Added ssid to argument.

  Revision 1.12  08/20/2004 12:54:21  terui
  Childs -> Children

  Revision 1.11  08/20/2004 07:47:39  terui
  Added WM_DisconnectChilds.

  Revision 1.10  08/19/2004 16:25:25  miya
  modified SetGameInfo function.

  Revision 1.9  08/18/2004 13:54:30  seiki_masashi
  Changed name to WM_SIZE_MP_*_*_BUFFER.

  Revision 1.8  08/18/2004 07:47:28  terui
  Added WM_GetAllowedChannel function.
  Added WM_SetEntry function.

  Revision 1.7  08/18/2004 01:24:47  seiki_masashi
  Small change to WM_StartMP argument.

  Revision 1.6  08/17/2004 13:49:45  seiki_masashi
  Changed to specify the send buffer in the WM_StartMP argument.

  Revision 1.5  08/11/2004 05:27:09  seiki_masashi
  Added doubleMode to StartDataSharing arguments.

  Revision 1.4  08/10/2004 14:17:04  terui
  Fix comment.

  Revision 1.1  08/10/2004 10:10:17  terui
  Initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_WM_ARM9_WM_API_H_
#define NITRO_WM_ARM9_WM_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     WM_DEFAULT_BEACON_PERIOD    200     // ms
#define     WM_DEFAULT_SCAN_PERIOD      30      // ms
#define     WM_MAX_MP_PACKET_TIME       5600    // us


/*---------------------------------------------------------------------------*
    function definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         WM_Init

  Description:  Performs the Initialization process of WM library.
                Synchronous function that only initializes ARM9.

  Arguments:    wmSysBuf    -   Pointer to the buffer allocated by the caller.
                                As much as WM_SYSTEM_BUF_SIZE is required for buffer size.
                dmaNo       -   DMA number used by WM

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Init(void *wmSysBuf, u16 dmaNo);

/*---------------------------------------------------------------------------*
  Name:         WM_Finish

  Description:  Performs the termination process of WM library. Synchronous function.
                Restores the state before WM_Init function was called.

  Arguments:    None.

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Finish(void);

/*---------------------------------------------------------------------------*
  Name:         WM_Enable

  Description:  Makes the wireless hardware to the usable state.
                Internal state changes from READY state to STOP state.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Enable(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_Disable

  Description:  Changes the wireless hardware to the use prohibited state.
                Internal state changes from STOP state to READY state.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Disable(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_PowerOn

  Description:  Starts up the wireless hardware.
                Internal state changes from STOP state to IDLE state.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_PowerOn(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_PowerOff

  Description:  Shuts down the wireless hardware.
                Internal state changes from IDLE state to STOP state.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_PowerOff(WMCallbackFunc callback);

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
WMErrCode WM_Initialize(void *wmSysBuf, WMCallbackFunc callback, u16 dmaNo);

/*---------------------------------------------------------------------------*
  Name:         WM_SetIndCallback

  Description:  Sets a function that is called to the condition notification from WM7.
                This function itself is an synchronous function.

  Arguments:    callback    -   Specifies the callback function called during status notification
                                from wireless firmware.

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetIndCallback(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_SetPortCallback

  Description:  Sets a function that is called to the communication frame receiving notification from WN7.

  Arguments:    port        -   Port number
                callback    -   Callback function to be called when notify the receiving.
                arg         -   Argument passed to callback function as WMPortRecvCallback.arg
                                Passed argument. 

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetPortCallback(u16 port, WMCallbackFunc callback, void *arg);

/*---------------------------------------------------------------------------*
  Name:         WM_Reset

  Description:  Resets the wireless library, and restores the state immediately after the initialization.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Reset(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_End

  Description:  Ends the wireless library.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_End(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_ReadStatus

  Description:  Obtains the structure that indicates the condition of the wireless library. Synchronous function.

  Arguments:    statusBuf   -   Pointer to the variable that obtains the condition.

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_ReadStatus(WMStatus *statusBuf);

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
WMErrCode WM_SetParentParameter(WMCallbackFunc callback, const WMParentParam *pparaBuf);

/*---------------------------------------------------------------------------*
  Name:         WMi_StartParent

  Description:  Starts the communication a parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                powerSave   -   When using power save mode, TRUE. When not using it, FALSE.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_StartParentEx(WMCallbackFunc callback, BOOL powerSave);

/*---------------------------------------------------------------------------*
  Name:         WM_StartParent

  Description:  Starts the communication a parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartParent(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_EndParent

  Description:  Stops the communication as a parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndParent(WMCallbackFunc callback);

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
WMErrCode WM_StartScan(WMCallbackFunc callback, const WMScanParam *param);

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
WMErrCode WM_StartScanEx(WMCallbackFunc callback, const WMScanExParam *param);

/*---------------------------------------------------------------------------*
  Name:         WM_EndScan

  Description:  Stops the scan process as a child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndScan(WMCallbackFunc callback);

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
WMErrCode WM_StartConnectEx(WMCallbackFunc callback, const WMBssDesc *pInfo, const u8 *ssid,
                            BOOL powerSave, u16 authMode);

/*---------------------------------------------------------------------------*
  Name:         WM_StartConnect

  Description:  Starts the connection to the parent as a child

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                pInfo       -   Information of the parent to be connected. The structure obtained with WM_StartScan
                                is specified. Note that the entity of this structure is forcibly
                                cache stored.
                ssid        -   Child information to notify the parent (24Byte(WM_SIZE_CHILD_SSID) fixed size)

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
static inline WMErrCode
WM_StartConnect(WMCallbackFunc callback, const WMBssDesc *pInfo, const u8 *ssid)
{
    return WM_StartConnectEx(callback, pInfo, ssid, TRUE, WM_AUTHMODE_OPEN_SYSTEM);
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
WMErrCode WM_Disconnect(WMCallbackFunc callback, u16 aid);

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
WMErrCode WM_DisconnectChildren(WMCallbackFunc callback, u16 aidBitmap);

/*---------------------------------------------------------------------------*
  Name:         WM_GetMPSendBufferSize

  Description:  Calculates MP communication send buffer size  from the current WM state.

  Arguments:    None.

  Returns:      int      - send buffer size that has to be passed to WM_StartMP
 *---------------------------------------------------------------------------*/
int     WM_GetMPSendBufferSize(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetMPReceiveBufferSize

  Description:  Calculates the MP communication receive buffer size from the current WM state.

  Arguments:    None.

  Returns:      int      - receive buffer size that has to be passed to WM_StartMP
 *---------------------------------------------------------------------------*/
int     WM_GetMPReceiveBufferSize(void);

/*---------------------------------------------------------------------------*
  Name:         WM_StartMPEx

  Description:  Starts the MP communication. Possible to specify the detailed operation mode. Function that is common to parent and child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                recvBuf     -   Receive data storage buffer
                                    Pay attention to cache because ARM7 writes out data directly.
                recvBufSize -   Size of the receive data storage buffer.
                                    It must be larger than the return value of WM_GetReceiveBufferSize().
                sendBuf         -   Buffer that stores the send data.
                                    Pay attention to cache because ARM7 writes out data directly.
                sendBufSize -   Size of the send data storage buffer
                                    It must be larger than the return value of WM_GetSendBufferSize().
                mpFreq      -   How many times MP communication is performed in one frame.
                                    0 is contiguous send mode. This carries a meaning only for the parent.
                defaultRetryCount - The standard number of times to retry when send fails
                                    during communication of 0-7 numbered ports. 0 to not retry.
                minPollBmpMode  -   Operating mode that keeps accumulation to a minimum
                                    in accordance with the address of the packet that sends the pollBitmap during MP communication. 
                singlePacketMode -  Special operating mode where only one packet is sent for
                                    one MP.
                fixFreqMode     -   Special operation mode that prohibits the increase of the MP communication by retries.
                                    Fixed to exactly mpFreq times for the number of times during the MP communication in a frame.

                ignoreFatalError -  Does not perform AutoDisconnect when FatalError was generated.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartMPEx(WMCallbackFunc callback,
                       u16 *recvBuf,
                       u16 recvBufSize,
                       u16 *sendBuf,
                       u16 sendBufSize,
                       u16 mpFreq,
                       u16 defaultRetryCount,
                       BOOL minPollBmpMode,
                       BOOL singlePacketMode, BOOL fixFreqMode, BOOL ignoreFatalError);

/*---------------------------------------------------------------------------*
  Name:         WM_StartMP

  Description:  Starts the MP communication. Function that is common to parent and child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                recvBuf     -   Receive data storage buffer
                                Pay attention to cache because ARM7 writes out data directly.
                recvBufSize -   Size of the receive data storage buffer.
                                It must be larger than the return value of WM_GetReceiveBufferSize().
                sendBuf         -   Buffer that stores the send data.
                                Pay attention to cache because ARM7 writes out data directly.
                sendBufSize -   Size of the send data storage buffer
                                It must be larger than the return value of WM_GetSendBufferSize().
                mpFreq      -   How many times MP communication is performed in one frame.
                                0 is contiguous send mode. This carries a meaning only for the parent.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartMP(WMCallbackFunc callback,
                     u16 *recvBuf, u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, u16 mpFreq);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParameter

  Description:  Configures the different parameters for MP communications

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                param       -   Pointer to the structure where the parameters for MP communications are stored

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParameter(WMCallbackFunc callback, const WMMPParam * param);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPChildSize

  Description:  Sets the number of bytes a child can send in one MP communication

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                childSize   -   Number of send bytes for a child

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPChildSize(WMCallbackFunc callback, u16 childSize);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParentSize

  Description:  Sets the number of bytes a parent can send in one MP communication

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                childSize   -   Number of send bytes for a parent

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParentSize(WMCallbackFunc callback, u16 parentSize);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPFrequency

  Description:  Switches how many times to perform the MP communication in one frame. Function exclusive for parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                mpFreq      -   How many times MP communication is performed in one frame.
                                0 is contiguous send mode.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPFrequency(WMCallbackFunc callback, u16 mpFreq);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPInterval

  Description:  Sets the interval in which a connection can be made and MP communications carried out in one frame

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                parentInterval - the interval in which a connection can be made and MP communications carried out by a parent in one frame (ms)
                childInterval  - the interval in which a connection can be made and MP communications carried out by a child in one frame (ms)

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPInterval(WMCallbackFunc callback, u16 parentInterval, u16 childInterval);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPTiming

  Description:  Sets the preparation start timing for MP communications when in frame synchronous communication mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                parentVCount -  The parent operation start V Count when in frame simultaneous communications
                childVCount -  The child operation start V Count when in frame simultaneous communications

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPTiming(WMCallbackFunc callback, u16 parentVCount, u16 childVCount);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPDataToPortEx

  Description:  Reserves data with MP communication. Function that is common to parent and child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                arg         -   Argument to be passed to callback
                sendData    -   Pointer to the data to reserve send.
                                Note that the entity of this data is forcibly stored in cache.
                sendDataSize -  Size of the data to reserve send.
                destBitmap  -   Specifies bitmap of aid that indicates the send destination child.
                port        -   Port number to send
                prio        -   Priority (0: highest -- 3: lowest)

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPDataToPortEx(WMCallbackFunc callback,
                               void *arg,
                               const u16 *sendData,
                               u16 sendDataSize, u16 destBitmap, u16 port, u16 prio);

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPDataToPort

  Description:  Reserves data with MP communication. Function that is common to parent and child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                sendData    -   Pointer to the data to reserve send.
                                Note that the entity of this data is forcibly stored in cache.
                sendDataSize -  Size of the data to reserve send.
                destBitmap  -   Specifies bitmap of aid that indicates the send destination child.
                port        -   Port number to send
                prio        -   Priority (0: highest -- 3: lowest)

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
static inline WMErrCode
WM_SetMPDataToPort(WMCallbackFunc callback, const u16 *sendData, u16 sendDataSize, u16 destBitmap,
                   u16 port, u16 prio)
{
    return WM_SetMPDataToPortEx(callback, NULL, sendData, sendDataSize, destBitmap, port, prio);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPData

  Description:  Reserves data with MP communication. Function that is common to parent and child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                sendData    -   Pointer to the data to reserve send.
                                Note that the entity of this data is forcibly stored in cache.
                sendDataSize -  Size of the data to reserve send.
                tmptt       -   In contiguous send mode, this specifies MP packet send time.
                                (Only parent valid. ) (ignored as of 2004/07/26)
                pollbmp     -   In contiguous send mode, this specifies Poll map that indicates the send destination child.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
static inline WMErrCode
WM_SetMPData(WMCallbackFunc callback, const u16 *sendData, u16 sendDataSize, u16 tmptt, u16 pollbmp)
{
#pragma unused( tmptt )
    return WM_SetMPDataToPortEx(callback, NULL, sendData, sendDataSize, pollbmp, WM_PORT_RAWDATA,
                                WM_PRIORITY_NORMAL);
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndMP

  Description:  Stops MP communication. Function that is common to parent and child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndMP(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_StartDCF

  Description:  Starts the communication in infrastructure mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                recvBuf     -   Pointer to the data receive buffer
                                Pay attention to cache because ARM7 writes out data directly.
                recvBufSize -   size of data receive buffer

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartDCF(WMCallbackFunc callback, WMDcfRecvBuf *recvBuf, u16 recvBufSize);

/*---------------------------------------------------------------------------*
  Name:         WM_SetDCFData

  Description:  Reserves the data to be transmitted by the infrastructure mode communication.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                destAdr     -   Pointer to the buffer that indicates the MAC address of the communication target.
                sendData    -   Pointer to the data to reserve send.
                                The entity for the data to be reserved for send will be forcibly cache stored.
                                cache stored.
                sendDataSize -  Size of the data to reserve send.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetDCFData(WMCallbackFunc callback,
                        const u8 *destAdr, const u16 *sendData, u16 sendDataSize);

/*---------------------------------------------------------------------------*
  Name:         WM_EndDCF

  Description:  Stops the communication in infrastructure mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDCF(WMCallbackFunc callback);

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
WMErrCode WM_SetWEPKey(WMCallbackFunc callback, u16 wepmode, const u16 *wepkey);

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
WMErrCode WM_SetWEPKeyEx(WMCallbackFunc callback, u16 wepmode, u16 wepkeyid, const u8 *wepkey);

/*---------------------------------------------------------------------------*
  Name:         WM_ReadMPData

  Description:  Extracts only the data of the specified child from the entire received data. Synchronous function.

  Arguments:    header      -   Pointer that indicates the entire received data.
                aid         -   AID of the child to extract data.

  Returns:      WMMpRecvData* - Returns a pointer to the data received from the corresponding child.
                                Returns NULL if the extraction failed.
 *---------------------------------------------------------------------------*/
WMMpRecvData *WM_ReadMPData(const WMMpRecvHeader *header, u16 aid);

/*---------------------------------------------------------------------------*
  Name:         WM_StartDataSharing

  Description:  Enables the data sharing feature. Synchronous function.
                By performing MP communication after enabling the feature, key sharing
                data sharing communications will be performed.

  Arguments:    dsInfo      - WMDataSharingInfo structure
                port        -   Number for port to use
                aidBitmap   -   AID of the partner for data sharing.
                dataLength  -   Data length of shared memory (Specify an even number of bytes.)
                doubleMode  -   Specify TRUE if using WM_StepDataSharing in every frame

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartDataSharing(WMDataSharingInfo *dsInfo,
                              u16 port, u16 aidBitmap, u16 dataLength, BOOL doubleMode);

/*---------------------------------------------------------------------------*
  Name:         WM_EndDataSharing

  Description:  Disables the data sharing feature. Synchronous function.

  Arguments:    dsInfo      - WMDataSharingInfo structure

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDataSharing(WMDataSharingInfo *dsInfo);

/*---------------------------------------------------------------------------*
  Name:         WM_StepDataSharing

  Description:  Proceed to the next step in the synchronized data sharing process. Synchronous function.

  Arguments:    dsInfo      - WMDataSharingInfo structure
                sendData    -   Send data to be shared.
                receiveData - Shared receive data

  Returns:      WMErrCode   -   Returns the process result.
                         *_SUCCESS      : Shared data send/receive successful
                         *_NO_DATASET   : Shared data not yet received
 *---------------------------------------------------------------------------*/
WMErrCode WM_StepDataSharing(WMDataSharingInfo *dsInfo,
                             const u16 *sendData, WMDataSet *receiveData);

/*---------------------------------------------------------------------------*
  Name:         WM_GetSharedDataAddress

  Description:  Acquires the address of a particular AID that is in received shared data.
                Synchronous function.

  Arguments:    dsInfo      - WMDataSharingInfo structure
                receiveData - Shared receive data
                aid         -   AID

  Returns:      u16*    - The address for the receiving data. If there is no data, returns NULL.
 *---------------------------------------------------------------------------*/
u16    *WM_GetSharedDataAddress(WMDataSharingInfo *dsInfo, WMDataSet *receiveData, u16 aid);

/*---------------------------------------------------------------------------*
  Name:         WM_StartKeySharing

  Description:  Enables the key sharing feature.
                By performing MP communication after enabling the feature, key sharing
                communication occurs accompanying MP communication.

  Arguments:    buf         -   Pointer to the buffer that stores the key information
                                Entity is a pointer to a WMDataSharingInfo structure.
                port        -   port number to use

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartKeySharing(WMKeySetBuf *buf, u16 port);

/*---------------------------------------------------------------------------*
  Name:         WM_EndKeySharing

  Description:  Disables the key sharing feature.

  Arguments:    buf         -   Pointer to the buffer that stores the key information
                                Entity is a pointer to a WMDataSharingInfo structure.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndKeySharing(WMKeySetBuf *buf);

/*---------------------------------------------------------------------------*
  Name:         WM_GetKeySet

  Description:  Load one key set data that is key shared.

  Arguments:    buf         -   Pointer to the buffer that stores the key information
                                Entity is a pointer to a WMDataSharingInfo structure.
                keySet      -   Pointer to the buffer that reads key set.
                                Specify a buffer other than the one provided by WM_StartKeySharing.
                               

  Returns:      MWErrCode   -   Returns process results.
 *---------------------------------------------------------------------------*/
WMErrCode WM_GetKeySet(WMKeySetBuf *buf, WMKeySet *keySet);

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
WMErrCode WM_SetGameInfo(WMCallbackFunc callback,
                         const u16 *userGameInfo, u16 userGameInfoSize,
                         u32 ggid, u16 tgid, u8 attr);

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
WMErrCode WM_SetBeaconIndication(WMCallbackFunc callback, u16 flag);

#ifdef  WM_ENABLE_TESTMODE
/*---------------------------------------------------------------------------*
  Name:         WM_StartTestMode

  Description:  Starts communication in test mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                signal      -   0: No modulation (data=0)
                                1: No modulation (data=1)
                                2: PN 15 steps
                                3: 01 pattern (with scramble)
                                4: 01 pattern (without scramble)
                rate        -   1: 1Mbps
                                2: 2Mbps
                channel     -   Specifies the channel to send data (1 to 14).

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartTestMode(WMCallbackFunc callback, u16 signal, u16 rate, u16 channel);

/*---------------------------------------------------------------------------*
  Name:         WM_StopTestMode

  Description:  Stops communication in test mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StopTestMode(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_StartTestRxMode

  Description:  Starts receiving data in test mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                channel     -   Specifies the channel for receiving data (1 to 14).

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StartTestRxMode(WMCallbackFunc callback, u16 channel);

/*---------------------------------------------------------------------------*
  Name:         WM_StopTestRxMode

  Description:  Stops receiving data in test mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_StopTestRxMode(WMCallbackFunc callback);

#endif

/*---------------------------------------------------------------------------*
  Name:         WM_SetLifeTime

  Description:  Sets the life time.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                tableNumber -   CAM table number that sets the life time.
                                (all tables with 0xFFFF)
                camLifeTime -   Life time of CAM (100ms units.0xFFFF for disable)
                frameLifeTime - Beacon interval of the life time in the frame to be set
                                (100ms units.0xFFFF for disable)
                camLifeTime   - MP communication lifetime (100ms units.0xFFFF for disable)

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetLifeTime(WMCallbackFunc callback,
                         u16 tableNumber, u16 camLifeTime, u16 frameLifeTime, u16 mpLifeTime);

/*---------------------------------------------------------------------------*
  Name:         WM_MeasureChannel

  Description:  Measures the status of usage of channel.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                ccaMode     -   CCA operation mode
                                0: Only carrier sense. ED threshold value is ignored.
                                1: Valid only with ED threshold value
                                2: Logical product of carrier sense and ED threshold value.
                                3: Logical sum of carrier sense and ED threshold value.
                EDThreshold -   ED threshold (0--61) -60dBm -- -80dBm
                channel     -   Channel to investigate.
                                (Only one channel with one MeasureChannel)
                measureTime -   Time to investigate.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_MeasureChannel(WMCallbackFunc callback,
                            u16 ccaMode, u16 edThreshold, u16 channel, u16 measureTime);

/*---------------------------------------------------------------------------*
  Name:         WM_InitWirelessCounter

  Description:  Initializes WirelessCounter.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_InitWirelessCounter(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_GetWirelessCounter

  Description:  Obtains the Wireless NIC send/receive frame count and send/receive error frame count.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_GetWirelessCounter(WMCallbackFunc callback);

/*---------------------------------------------------------------------------*
  Name:         WM_GetAllowedChannel

  Description:  Obtains the channel that was permitted to use for the communication. Synchronous function.

  Arguments:    None.

  Returns:      u16 -   Returns the bit field of the use permitted channel. The lowest bit indicates 1 channel, and
                        the highest channel indicates 16 channel. The channel with the bit set to 1 is the use permitted,
                        and the channel with the bit set to 0 is the use prohibited.
                        Returns the bits in channels 1-13 that have the value 1
                        . When 0x0000 is returned, there is no channel
                        permitted for use, so the wireless function cannot use it.
                        Also, in case the function failed, such as when it is not yet initialized, 0x8000 is returned.
 *---------------------------------------------------------------------------*/
u16     WM_GetAllowedChannel(void);

#ifdef  WM_PRECALC_ALLOWEDCHANNEL
/*---------------------------------------------------------------------------*
  Name:         WM_IsExistAllowedChannel

  Description:  Checks if the channel that was permitted to be used for communication actually exists.
                Possible to determine it properly even before the initialization of WM library.

  Arguments:    None.

  Returns:      BOOL    -   TRUE if the channel that was permitted for use exist.
                            FALSE, if it does not exist.
 *---------------------------------------------------------------------------*/
BOOL    WM_IsExistAllowedChannel(void);
#endif

/*---------------------------------------------------------------------------*
  Name:         WM_SetEntry

  Description:  Switches accepting/refusing of a connection from a child as a parent.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                enabled     -   Entry permitted / not permitted flag. TRUE: permitted, FALSE: not permitted.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetEntry(WMCallbackFunc callback, BOOL enabled);

/*---------------------------------------------------------------------------*
  Name:         WM_GetLinkLevel

  Description:  Gets link level during communication. Synchronous function.

  Arguments:    None.

  Returns:      WMLinkLevel -   Returns at link strength at one of four evaluated levels.
 *---------------------------------------------------------------------------*/
WMLinkLevel WM_GetLinkLevel(void);


/*---------------------------------------------------------------------------*
  Name:         WMi_SetBeaconPeriod

  Description:  Changes the beacon intervals.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                beaconPeriod -  Beacon interval (10 to1000 TU (1024us))

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_SetBeaconPeriod(WMCallbackFunc callback, u16 beaconPeriod);
/* As of 2004/10/18 a problem occurred when the beacon interval was changed while communicating.
   when the beacon interval was changed while performing communication as of 10/18/2004. */


/*---------------------------------------------------------------------------*
  Name:         WM_GetDispersionBeaconPeriod

  Description:  Obtains the beacon interval value that should be set to operate as a parent.
                Synchronous function.

  Arguments:    None.

  Returns:      u16 -   Beacon interval value that should be set (ms).
 *---------------------------------------------------------------------------*/
u16     WM_GetDispersionBeaconPeriod(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetDispersionScanPeriod

  Description:  Obtains the search limit time that should be set for a child to search for a parent.
                Synchronous function.

  Arguments:    None.

  Returns:      u16 -   Search limit time that should be set (ms).
 *---------------------------------------------------------------------------*/
u16     WM_GetDispersionScanPeriod(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetOtherElements

  Description:  Gets the extended elements in the beacon.
                Synchronous function.

  Arguments:    bssDesc - Parent information structure
                          Specifies the structure obtained with WM_StartScan(Ex).

  Returns:      WMOtherElements - Extended element structure.
 *---------------------------------------------------------------------------*/
WMOtherElements WM_GetOtherElements(WMBssDesc *bssDesc);

/*---------------------------------------------------------------------------*
  Name:         WM_GetNextTgid

  Description:  Gets the automatically generated, unqiue TGID value.
                Synchronous function.
                This function internally calls RTC_Init().

  Arguments:    None.

  Returns:      The first time, it returns a TGID that was generated based on the RTC;
                Thereafter, it returns the value returned the previous time incremented by 1.
 *---------------------------------------------------------------------------*/
u16     WM_GetNextTgid(void);

/*---------------------------------------------------------------------------*
  Name:         WM_SetPowerSaveMode()

  Description:  Sets the PowerSaveMode

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                powerSave   -   When using power save mode, TRUE. When not using it, FALSE.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetPowerSaveMode(WMCallbackFunc callback, BOOL powerSave);

/*---------------------------------------------------------------------------*
  Name:         WMi_IsMP

  Description:  Get current MP communication state.

  Arguments:    None.

  Returns:      TRUE if MP communication state.
 *---------------------------------------------------------------------------*/
BOOL    WMi_IsMP(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetAID

  Description:  Gets current AID.
                Return a valid value only when the state is one of PARENT, MP_PARENT, CHILD, or MP_CHILD.

  Arguments:    None.

  Returns:      AID
 *---------------------------------------------------------------------------*/
u16     WM_GetAID(void);

/*---------------------------------------------------------------------------*
  Name:         WM_GetConnectedAIDs

  Description:  Get currently connected parties in bitmap format.
                Return a valid value only when the state is one of PARENT, MP_PARENT, CHILD, or MP_CHILD.
                Returns 0x0001 when this machine is a child that is connected to a parent.

  Arguments:    None.

  Returns:      Bitmap of AIDs of connected parties.
 *---------------------------------------------------------------------------*/
u16     WM_GetConnectedAIDs(void);

/*---------------------------------------------------------------------------*
  Name:         WMi_GetMPReadyAIDs

  Description:  Gets a list of the AIDs of other parties which can receive MP from among currently connected parties
                in bitmap form.
                Return a valid value only when the state is one of PARENT, MP_PARENT, CHILD, or MP_CHILD.
                 
                Returns 0x0001 when this machine is a child that is connected to a parent.

  Arguments:    None.

  Returns:      Bitmap of the AID of another party for which MP is started.
 *---------------------------------------------------------------------------*/
u16     WMi_GetMPReadyAIDs(void);


// for debug
void    WMi_DebugPrintSendQueue(WMPortSendQueue *queue);
void    WMi_DebugPrintAllSendQueue(void);
const WMStatus *WMi_GetStatusAddress(void);
BOOL    WMi_CheckMpPacketTimeRequired(u16 parentSize, u16 childSize, u8 childs);


/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_MP_PARENT_RECEIVE_BUFFER

  Description:  Calculates parent receive buffer size.

  Arguments:    childMaxSize - maximum number of bytes of data you want to receive from child
                maxEntry     - number of children you want to connect
                ksFlag       - Boolean value for whether to perform key sharing

  Returns:      int      - size of the receive buffer that must be passed to the WM_StartMP
 *---------------------------------------------------------------------------*/
#define WM_SIZE_MP_PARENT_RECEIVE_BUFFER( childMaxSize, maxEntry, ksFlag ) \
            (((sizeof( WMMpRecvHeader ) - sizeof( WMMpRecvData ) + \
            ( ( sizeof( WMMpRecvData ) + (childMaxSize) + WM_HEADER_CHILD_MAX_SIZE - 2 + 2/*MACBUG*/ + ((ksFlag) ? WM_SIZE_KS_CHILD_DATA + WM_SIZE_MP_CHILD_PADDING : 0) ) * (maxEntry) ) \
             + 31) & ~0x1f) * 2)

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_MP_CHILD_RECEIVE_BUFFER

  Description:  Calculates the size of the child receive buffer.

  Arguments:    parentMaxSize - maximum byte size of data you want to receive from the parent
                ksFlag       - Boolean value for whether to perform key sharing

  Returns:      int      - size of the receive buffer that must be passed to the WM_StartMP
 *---------------------------------------------------------------------------*/
#define WM_SIZE_MP_CHILD_RECEIVE_BUFFER( parentMaxSize, ksFlag ) \
            (((sizeof( WMMpRecvBuf ) + (parentMaxSize) + WM_HEADER_PARENT_MAX_SIZE - 4 + ((ksFlag) ? WM_SIZE_KS_PARENT_DATA + WM_SIZE_MP_PARENT_PADDING : 0) + 31) & ~0x1f) * 2)

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_MP_PARENT_SEND_BUFFER

  Description:  Calculates the size of the parent send buffer.

  Arguments:    parentMaxSize - maximum byte size of data you want to send
                ksFlag       - Boolean value for whether to perform key sharing

  Returns:      int      - size of send buffer that must be passed to the WM_StartMP
 *---------------------------------------------------------------------------*/
#define WM_SIZE_MP_PARENT_SEND_BUFFER( parentMaxSize, ksFlag ) \
            (((parentMaxSize) + WM_HEADER_PARENT_MAX_SIZE + ((ksFlag) ? WM_SIZE_KS_PARENT_DATA + WM_SIZE_MP_PARENT_PADDING : 0) + 31) & ~0x1f)

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_MP_CHILD_SEND_BUFFER

  Description:  Calculates the size of the child send buffer.

  Arguments:    childMaxSize - maximum byte size of the data you want to send
                ksFlag       - Boolean value for whether to perform key sharing

  Returns:      int      - size of send buffer that must be passed to the WM_StartMP
 *---------------------------------------------------------------------------*/
#define WM_SIZE_MP_CHILD_SEND_BUFFER( childMaxSize, ksFlag ) \
            (((childMaxSize) + WM_HEADER_CHILD_MAX_SIZE + ((ksFlag) ? WM_SIZE_KS_CHILD_DATA + WM_SIZE_MP_CHILD_PADDING : 0) + 31) & ~0x1f)

/*---------------------------------------------------------------------------*
  Name:         WM_SIZE_DCF_CHILD_SEND_BUFFER

  Description:  Calculates the size of the DCF child's send buffer.

  Arguments:    childMaxSize - maximum byte size of the data you want to send

  Returns:      int      - Send buffer size that must be passed to WM_StartDCF
 *---------------------------------------------------------------------------*/
#define WM_SIZE_DCF_CHILD_SEND_BUFFER( childMaxSize ) \
            (((childMaxSize) + WM_SIZE_MADATA_HEADER + 31) & ~0x1f)

#define WM_SIZE_CHILD_SEND_BUFFER WM_SIZE_MP_CHILD_SEND_BUFFER
#define WM_SIZE_CHILD_RECEIVE_BUFFER WM_SIZE_MP_CHILD_RECEIVE_BUFFER
#define WM_SIZE_PARENT_SEND_BUFFER WM_SIZE_MP_PARENT_SEND_BUFFER
#define WM_SIZE_PARENT_RECEIVE_BUFFER WM_SIZE_MP_PARENT_RECEIVE_BUFFER


/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         WM_ConvGgid32to16

  Description:  Changes game group ID from "32 bit form" to "16 bit x 2 form."

  Arguments:    src     -   Pointer to the change source GGID shown by 32 bit.
                dest    -   Pointer to the buffer that will store the converted GGID
                            4-byte data will be written, beginning from this address.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WM_ConvGgid32to16(u32 *src, u16 *dst)
{
    dst[0] = (u16)(*src & 0x0000ffff);
    dst[1] = (u16)(*src >> 16);
}

/*---------------------------------------------------------------------------*
  Name:         WM_ConvGgid16to32

  Description:  Converted game group ID from 16-bit x 2 form to 32 bit form.

  Arguments:    src     -   Pointer to the conversion source GGID that is expressed by 16 bit arrays.
                dest    -   Pointer to the buffer that will store the converted GGID
                            4-byte data will be written, beginning from this address.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WM_ConvGgid16to32(u16 *src, u32 *dst)
{
    *dst = ((u32)src[1] << 16) | src[0];
}

/*---------------------------------------------------------------------------*
  Name:         WM_IsBssidEqual

  Description:  Checks whether 2 BSSID are equal.

  Arguments:    idp1     -   Pointer to a BSSID that will be compared
                idp2     -   Pointer to a BSSID that will be compared

  Returns:      Returns TRUE if they are equal.
                Returns FALSE if they differ.
 *---------------------------------------------------------------------------*/
static inline BOOL WM_IsBssidEqual(const u8 *idp1, const u8 *idp2)
{
    return ((*idp1 == *idp2) &&
            (*(idp1 + 1) == *(idp2 + 1)) &&
            (*(idp1 + 2) == *(idp2 + 2)) &&
            (*(idp1 + 3) == *(idp2 + 3)) &&
            (*(idp1 + 4) == *(idp2 + 4)) && (*(idp1 + 5) == *(idp2 + 5)));
}


/*---------------------------------------------------------------------------*
  Name:         WM_IsBssidEqual16

  Description:  Checks whether 2 BSSID are the same, in 2-byte chunks.

  Arguments:    idp1     -   Pointer to a BSSID that will be compared Needs two-byte alignment.
                idp2     -   Pointer to a BSSID that will be compared Needs two-byte alignment.

  Returns:      Returns TRUE if they are equal.
                Returns FALSE if they differ.
 *---------------------------------------------------------------------------*/
static inline BOOL WM_IsBssidEqual16(const u8 *idp1, const u8 *idp2)
{
    SDK_ALIGN2_ASSERT(idp1);
    SDK_ALIGN2_ASSERT(idp2);

    return ((*(u16 *)idp1 == *(u16 *)idp2) &&
            (*(u16 *)(idp1 + 2) == *(u16 *)(idp2 + 2)) &&
            (*(u16 *)(idp1 + 4) == *(u16 *)(idp2 + 4)));
}

/*---------------------------------------------------------------------------*
  Name:         WM_CopyBssid

  Description:  Copies BSSID.

  Arguments:    src     -   Pointer to the BSSID copy source
                dest     -   Pointer to the BSSID copy destination

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WM_CopyBssid(const u8 *src, u8 *dst)
{
    *dst = *src;
    *(dst + 1) = *(src + 1);
    *(dst + 2) = *(src + 2);
    *(dst + 3) = *(src + 3);
    *(dst + 4) = *(src + 4);
    *(dst + 5) = *(src + 5);
}

/*---------------------------------------------------------------------------*
  Name:         WM_CopyBssid16

  Description:  Copies BSSID in 2-byte units.

  Arguments:    src     -   Pointer to the BSSID copy source Needs two-byte alignment.
                dest     -   Pointer to the BSSID copy destination Needs two-byte alignment.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WM_CopyBssid16(const u8 *src, u8 *dst)
{
    SDK_ALIGN2_ASSERT(src);
    SDK_ALIGN2_ASSERT(dst);

    *(u16 *)dst = *(u16 *)src;
    *(u16 *)(dst + 2) = *(u16 *)(src + 2);
    *(u16 *)(dst + 4) = *(u16 *)(src + 4);
}

/*---------------------------------------------------------------------------*
  Name:         WM_IsValidGameInfo

  Description:  Checks whether GameInfo is supported. 

  Arguments:    gameInfo - Pointer to WMGameInfo structure
                gameInfoLength - Length of data at location specified by gameInfo 

  Returns:      Returns TRUE if GameInfo is supported. 
 *---------------------------------------------------------------------------*/
static inline BOOL WM_IsValidGameInfo(const WMGameInfo* gameInfo, u16 gameInfoLength)
{
    return (gameInfoLength >= WM_GAMEINFO_LENGTH_MIN
            && gameInfo->magicNumber == WM_GAMEINFO_MAGIC_NUMBER);
}

/*---------------------------------------------------------------------------*
  Name:         WM_IsValidGameBeacon

  Description:  Checks whether the beacon is from a DS wireless play parent. 

  Arguments:    bssDesc - Pointer to WMBssDesc structure that contains the beacon information

  Returns:      If the beacon is from a DS wireless play parent, TRUE. 
 *---------------------------------------------------------------------------*/
static inline BOOL WM_IsValidGameBeacon(const WMBssDesc* bssDesc)
{
    return WM_IsValidGameInfo(&bssDesc->gameInfo, bssDesc->gameInfoLength);
}

/*---------------------------------------------------------------------------*
  Name:         WMi_IsDisconnectReasonFromMyself

  Description:  Determines whether the reason for WM_STATECODE_DISCONNECTED is that the unit disconnected from itself.

  Arguments:    reason -The reasong member of WMStartParentCallback or WMPortRecvCallback.

  Returns:      True if the unit disconnected from itself by calling a WM function.
                FALSE if disconnected from the other party or disconnected due to a communication error.
 *---------------------------------------------------------------------------*/
static inline BOOL WMi_IsDisconnectReasonFromMyself(u16 reason)
{
    return (reason >= WM_DISCONNECT_REASON_FROM_MYSELF);
}


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C"*/
#endif

#endif /* NITRO_WM_ARM9_WM_API_H_*/

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
