/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_sync.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_sync.c,v $
  Revision 1.18  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.17  12/01/2005 13:58:43  seiki_masashi
  Sorted the operations related to send / receive size.

  Revision 1.16  11/29/2005 07:51:41  seiki_masashi
  fix typo

  Revision 1.15  11/01/2005 08:55:09  seiki_masashi
  Added WMPortRecvCallback.connectedAidBitmap.

  Revision 1.14  10/28/2005 11:15:59  seiki_masashi
  Implemented WM_STATECODE_PORT_INIT

  Revision 1.13  04/11/2005 08:51:04  yosizaki
  add WM_GetNextTgid().

  Revision 1.12  03/17/2005 05:05:19  ooe
  Added WM_GetOtherElements

  Revision 1.11  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.10  02/18/2005 13:27:33  seiki_masashi
  Revised Copyright notation

  Revision 1.9  02/18/2005 07:52:31  seiki_masashi
  warning measures

  Revision 1.8  12/22/2004 02:29:53  terui
  Added WM_IsExistAllowedChannel

  Revision 1.7  10/22/2004 04:37:28  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.6  10/18/2004 04:17:46  terui
  Added a specification to randomize the beacon interval and the scan interval.

  Revision 1.5  10/15/2004 14:12:07  terui
  Changed fixed value returned when WM_GetAllowedChannel function is not initialized.

  Revision 1.4  10/01/2004 05:38:10  seiki_masashi
  Made changes to return the minimum radio wave strength when there is no child

  Revision 1.3  10/01/2004 04:04:14  terui
  Made correction due to the addition of a feature that precalculates the wireless communication permitted channel when it is started

  Revision 1.2  09/16/2004 02:57:32  seiki_masashi
  Fixed WM_GetMPReceiveBufferSize bug.

  Revision 1.1  09/10/2004 11:03:30  terui
  New upload reflecting the breakup of wm.c.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
  Name:         WM_SetIndCallback

  Description:  Sets a function that is called to the condition notification from WM7.

  Arguments:    callback    -   Specifies the callback function called during status notification
                                from wireless firmware.

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetIndCallback(WMCallbackFunc callback)
{
    WMErrCode result;
    OSIntrMode e;

    // Prohibit interrupts
    e = OS_DisableInterrupts();

    // Confirm that initialized
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        // End prohibition of interrupts
        (void)OS_RestoreInterrupts(e);
        return result;
    }

    // Set the callback for Indication
    WMi_GetSystemWork()->indCallback = callback;
    // End prohibition of interrupts
    (void)OS_RestoreInterrupts(e);

    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetPortCallback

  Description:  Sets a function that is called to the communication frame receiving notification from WN7.

  Arguments:    port        -   Port number
                callback    -   Callback function to be called when notify the receiving.
                arg         -   Argument passed to callback function as WMPortRecvCallback.arg
                                 

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetPortCallback(u16 port, WMCallbackFunc callback, void *arg)
{
    WMErrCode result;
    OSIntrMode e;
    WMPortRecvCallback cb_Port;

    // Check the parameter
#ifdef SDK_DEBUG
    if (port >= WM_NUM_OF_PORT)
    {
        WM_WARNING("Parameter \"port\" must be less than 16.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
#endif

    if (callback != NULL)
    {
        MI_CpuClear8(&cb_Port, sizeof(WMPortRecvCallback));
        cb_Port.apiid = WM_APIID_PORT_RECV;
        cb_Port.errcode = WM_ERRCODE_SUCCESS;
        cb_Port.state = WM_STATECODE_PORT_INIT;
        cb_Port.port = port;
        cb_Port.recvBuf = NULL;
        cb_Port.data = NULL;
        cb_Port.length = 0;
        cb_Port.seqNo = 0xffff;
        cb_Port.arg = arg;
        cb_Port.aid = 0;
        OS_GetMacAddress(cb_Port.macAddress);
    }

    // Prohibit interrupts
    e = OS_DisableInterrupts();
    // Confirm that initialized
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        // End prohibition of interrupts
        (void)OS_RestoreInterrupts(e);
        return result;
    }

    // Set the callback for data reception
    {
        WMArm9Buf *p = WMi_GetSystemWork();

        p->portCallbackTable[port] = callback;
        p->portCallbackArgument[port] = arg;
    }

    if (callback != NULL)
    {
        cb_Port.connectedAidBitmap = WM_GetConnectedAIDs();
        cb_Port.myAid = WM_GetAID();
        (*callback) ((void *)&cb_Port);
    }

    // End prohibition of interrupts
    (void)OS_RestoreInterrupts(e);

    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WM_ReadStatus

  Description:  Obtains the structure that indicates the condition of the wireless library.

  Arguments:    statusBuf   -   Pointer to the variable that obtains the condition.

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_ReadStatus(WMStatus *statusBuf)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Confirm that initialized
    result = WMi_CheckInitialized();
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (statusBuf == NULL)
    {
        WM_WARNING("Parameter \"statusBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // Copy the WMStatus structure to the CPU
    DC_InvalidateRange(p->status, sizeof(WMStatus));
    MI_CpuCopyFast(p->status, statusBuf, sizeof(WMStatus));

    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetMPSendBufferSize

  Description:  Calculate the size of the Send buffer for MP communications.
                At this time, StartParent and StartConnect must be finished.

  Arguments:    None.

  Returns:      int      - send buffer size that has to be passed to WM_StartMP
 *---------------------------------------------------------------------------*/
int WM_GetMPSendBufferSize(void)
{
    WMErrCode result;
    int     maxSendSize;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
    {
        return 0;
    }

    // confirm MP status
    DC_InvalidateRange(&(p->status->mp_flag), 4);       // Invalidates the ARM7 status region cache
    if (p->status->mp_flag == TRUE)
    {
        WM_WARNING("Already started MP protocol. So can't execute request.\n");
        return 0;
    }

    // Reference the MWStatus structure for information needed for the calculation
    DC_InvalidateRange(&(p->status->mp_maxSendSize), 4);
    maxSendSize = p->status->mp_maxSendSize;

    return ((maxSendSize + 31) & ~0x1f);
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetMPReceiveBufferSize

  Description:  Calculate the size of the Receive buffer for MP communications.
                At this time, StartParent and StartConnect must be finished.

  Arguments:    None.

  Returns:      int      - receive buffer size that has to be passed to WM_StartMP
 *---------------------------------------------------------------------------*/
int WM_GetMPReceiveBufferSize(void)
{
    WMErrCode result;
    BOOL    isParent;
    int     maxReceiveSize;
    int     maxEntry;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    if (result != WM_ERRCODE_SUCCESS)
    {
        return 0;
    }

    // confirm MP status
    DC_InvalidateRange(&(p->status->mp_flag), 4);
    if (p->status->mp_flag == TRUE)
    {
        WM_WARNING("Already started MP protocol. So can't execute request.\n");
        return 0;
    }

    // Reference the MWStatus structure for information needed for the calculation
    DC_InvalidateRange(&(p->status->aid), 2);
    isParent = (p->status->aid == 0) ? TRUE : FALSE;
    DC_InvalidateRange(&(p->status->mp_maxRecvSize), 2);
    maxReceiveSize = p->status->mp_maxRecvSize;
    if (isParent == TRUE)
    {
        DC_InvalidateRange(&(p->status->pparam.maxEntry), 2);
        maxEntry = p->status->pparam.maxEntry;
        return (int)((sizeof(WMmpRecvHeader) - sizeof(WMmpRecvData) +
                      ((sizeof(WMmpRecvData) + maxReceiveSize - 2 + 2 /*MACBUG*/) * maxEntry)
                      + 31) & ~0x1f) * 2;
    }
    else
    {
        return (int)((sizeof(WMMpRecvBuf) + maxReceiveSize - 4 + 31) & ~0x1f) * 2;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WM_ReadMPData

  Description:  Extracts only the data of the specified child from the entire received data.

  Arguments:    header      -   Pointer that indicates the entire received data.
                aid         -   AID of the child to extract data.

  Returns:      WMMpRecvData* - Returns a pointer to the data received from the corresponding child.
                                Returns NULL if the extraction failed.
 *---------------------------------------------------------------------------*/
WMMpRecvData *WM_ReadMPData(const WMMpRecvHeader *header, u16 aid)
{
    int     i;
    WMErrCode result;
    WMMpRecvData *recvdata_p[WM_NUM_MAX_CHILD]; // Array of pointers to data starting position for each child (up to 15 devices).
    WMArm9Buf *p = WMi_GetSystemWork();

    // confirms if initialized
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return NULL;
    }

    // Check the parameter
    if ((aid < 1) || (aid > WM_NUM_MAX_CHILD))
    {
        WM_WARNING("Parameter \"aid\" must be between 1 and %d.\n", WM_NUM_MAX_CHILD);
        return NULL;
    }

    // Does child with that AID exist?
    DC_InvalidateRange(&(p->status->child_bitmap), 2);
    if (0 == (p->status->child_bitmap & (0x0001 << aid)))
    {
        return NULL;
    }

    // Is there any received data?
    if (header->count == 0)
    {
        return NULL;
    }

    // Calculate starting position for child data
    recvdata_p[0] = (WMMpRecvData *)(header->data);

    i = 0;
    do
    {
        // If it is the specified AID's child data, it returns its pointer.
        if (recvdata_p[i]->aid == aid)
            return recvdata_p[i];

        ++i;
        recvdata_p[i] = (WMMpRecvData *)((u32)(recvdata_p[i - 1]) + header->length);
    }
    while (i < header->count);

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetAllowedChannel

  Description:  Obtains the channel that was permitted to use for the communication.

  Arguments:    None.

  Returns:      u16 -   Returns the bit field of the use permitted channel. The lowest bit indicates 1 channel, and
                        the highest channel indicates 16 channel. The channel with the bit set to 1 is the use permitted,
                        and the channel with the bit set to 0 is the use prohibited.
                        Returns the bits in channels 1-13 that have the value 1
                        . When 0x0000 is returned, there is no channel
                        permitted for use, so the wireless function cannot use it.
                        Also, in case the function failed, such as when it is not yet initialized, 0x8000 is returned.
 *---------------------------------------------------------------------------*/
u16 WM_GetAllowedChannel(void)
{
#ifdef WM_PRECALC_ALLOWEDCHANNEL
    WMErrCode result;

    // confirms if initialized
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return WM_GET_ALLOWED_CHANNEL_BEFORE_INIT;
    }

    return *((u16 *)((u32)(OS_GetSystemWork()->nvramUserInfo) +
                     ((sizeof(NVRAMConfig) + 3) & ~0x00000003) + 6));
#else
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Confirm that wireless hardware has started
    result = WMi_CheckIdle();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return WM_GET_ALLOWED_CHANNEL_BEFORE_INIT;
    }

    DC_InvalidateRange(&(p->status->allowedChannel), 2);
    return p->status->allowedChannel;
#endif
}

#ifdef  WM_PRECALC_ALLOWEDCHANNEL
/*---------------------------------------------------------------------------*
  Name:         WM_IsExistAllowedChannel

  Description:  Checks if the channel that was permitted to be used for communication actually exists.
                Possible to determine it properly even before the initialization of WM library.

  Arguments:    None.

  Returns:      BOOL    -   TRUE if the channel that was permitted for use exist.
                            FALSE, if it does not exist.
 *---------------------------------------------------------------------------*/
BOOL WM_IsExistAllowedChannel(void)
{
    u16     allowedChannel;

    allowedChannel = *(u16 *)((u32)(OS_GetSystemWork()->nvramUserInfo) +
                              ((sizeof(NVRAMConfig) + 3) & ~0x00000003));
    if (allowedChannel)
    {
        return TRUE;
    }
    return FALSE;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         WM_GetLinkLevel

  Description:  Gets link level during communication. Synchronous function.

  Arguments:    None.

  Returns:      WMLinkLevel -   Returns at link strength at one of four evaluated levels.
 *---------------------------------------------------------------------------*/
WMLinkLevel WM_GetLinkLevel(void)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // confirms if initialized
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return WM_LINK_LEVEL_0;        // lowest electric wave strength
    }

    DC_InvalidateRange(&(p->status->state), 2);
    switch (p->status->state)
    {
    case WM_STATE_MP_PARENT:
        // For parent
        DC_InvalidateRange(&(p->status->child_bitmap), 2);
        if (p->status->child_bitmap == 0)
        {
            // when there is no child
            return WM_LINK_LEVEL_0;    // lowest electric wave strength
        }
    case WM_STATE_MP_CHILD:
    case WM_STATE_DCF_CHILD:
        // For child
        DC_InvalidateRange(&(p->status->linkLevel), 2);
        return (WMLinkLevel)(p->status->linkLevel);
    }

    // when unconnected
    return WM_LINK_LEVEL_0;            // lowest electric wave strength
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetDispersionBeaconPeriod

  Description:  Obtains the beacon interval value that should be set to operate as a parent.

  Arguments:    None.

  Returns:      u16 -   Beacon interval value that should be set (ms).
 *---------------------------------------------------------------------------*/
u16 WM_GetDispersionBeaconPeriod(void)
{
    u8      mac[6];
    u16     ret;
    s32     i;

    OS_GetMacAddress(mac);
    for (i = 0, ret = 0; i < 6; i++)
    {
        ret += mac[i];
    }
    ret += OS_GetVBlankCount();
    ret *= 7;
    return (u16)(WM_DEFAULT_BEACON_PERIOD + (ret % 20));
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetDispersionScanPeriod

  Description:  Obtains the search limit time that should be set for a child to search for a parent.

  Arguments:    None.

  Returns:      u16 -   Search limit time that should be set (ms).
 *---------------------------------------------------------------------------*/
u16 WM_GetDispersionScanPeriod(void)
{
    u8      mac[6];
    u16     ret;
    s32     i;

    OS_GetMacAddress(mac);
    for (i = 0, ret = 0; i < 6; i++)
    {
        ret += mac[i];
    }
    ret += OS_GetVBlankCount();
    ret *= 13;
    return (u16)(WM_DEFAULT_SCAN_PERIOD + (ret % 10));
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetOtherElements

  Description:  Gets the extended elements in the beacon.
                Synchronous function.

  Arguments:    bssDesc - Parent information structure
                          Specifies the structure obtained with WM_StartScan(Ex).

  Returns:      WMOtherElements - Extended element structure.
 *---------------------------------------------------------------------------*/
WMOtherElements WM_GetOtherElements(WMBssDesc *bssDesc)
{
    WMOtherElements elems;
    u8     *p_elem;
    int     i;
    u8      curr_elem_len;
    u8      elems_len;                 // Length of all elements
    u8      elems_len_counter;         // Counter for checking the length of elements

    // Ends when gameInfo is included
    if (bssDesc->gameInfoLength != 0)
    {
        elems.count = 0;
        return elems;
    }

    // Get otherElement count and terminate if 0.
    elems.count = (u8)(bssDesc->otherElementCount);
    if (elems.count == 0)
        return elems;

    // The maximum allowed number of elems is limited WM_SCAN_OTHER_ELEMENT_MAX.
    if (elems.count > WM_SCAN_OTHER_ELEMENT_MAX)
        elems.count = WM_SCAN_OTHER_ELEMENT_MAX;

    // First set the start of elements into gameInfo.
    p_elem = (u8 *)&(bssDesc->gameInfo);

    // Get length of all elements and initialize counter used for checking.
    elems_len = (u8)((bssDesc->length * sizeof(u16)) - 64);
    elems_len_counter = 0;

    // Loop only for the number of elems
    for (i = 0; i < elems.count; ++i)
    {
        elems.element[i].id = p_elem[0];
        elems.element[i].length = p_elem[1];
        elems.element[i].body = (u8 *)&(p_elem[2]);

        // Calculate current element length and add to the check counter.
        curr_elem_len = (u8)(elems.element[i].length + 2);
        elems_len_counter += curr_elem_len;

        //OS_Printf("eles_len        =%d\n", elems_len);
        //OS_Printf("eles_len_counter=%d\n", elems_len_counter);

        // An error results if the length of all elements is exceeded
        // Notify that there were no elements
        if (elems_len_counter > elems_len)
        {
            WM_WARNING("Elements length error.\n");
            elems.count = 0;
            return elems;
        }

        // Calculate the lead address of the next element
        p_elem = (u8 *)(p_elem + curr_elem_len);
    }

    return elems;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetNextTgid

  Description:  Gets the automatically generated, unqiue TGID value.
                Synchronous function.

  Arguments:    None.

  Returns:      The first time, it returns a TGID that was generated based on the RTC;
                Thereafter, it returns the value returned the previous time incremented by 1.
 *---------------------------------------------------------------------------*/
u16 WM_GetNextTgid(void)
{
    enum
    { TGID_DEFAULT = (1 << 16) };
    static u32 tgid_bak = (u32)TGID_DEFAULT;
    /* Use the RTC time value the first time so as to preserve the integrity of the unit's own time*/
    if (tgid_bak == (u32)TGID_DEFAULT)
    {
        RTCTime rt[1];
        RTC_Init();
        if (RTC_GetTime(rt) == RTC_RESULT_SUCCESS)
        {
            tgid_bak = (u16)(rt->second + (rt->minute << 8));
        }
        else
        {
            OS_Warning("failed to get RTC-data to create unique TGID!\n");
        }
    }
    /* Keep getting various unique values and increment each time*/
    tgid_bak = (u16)(tgid_bak + 1);
    return (u16)tgid_bak;
}


/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
