/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_dcf.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_dcf.c,v $
  Revision 1.7  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.6  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.5  12/03/2004 06:35:20  ooe
  Changed 1500 in the maximum send size check section to WM_DCF_MAX_SIZE.

  Revision 1.4  10/22/2004 04:37:28  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.3  10/08/2004 08:37:37  ooe
  small fix

  Revision 1.2  10/05/2004 09:45:28  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.1  09/10/2004 11:03:30  terui
  New upload reflecting the breakup of wm.c.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


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
WMErrCode WM_StartDCF(WMCallbackFunc callback, WMDcfRecvBuf *recvBuf, u16 recvBufSize)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Check the state
    result = WMi_CheckState(WM_STATE_CHILD);
    WM_CHECK_RESULT(result);

    // Confirm DCF state
    DC_InvalidateRange(&(p->status->dcf_flag), 4);
    if (p->status->dcf_flag == TRUE)
    {
        WM_WARNING("Already DCF mode. So can't start DCF again.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // Check the parameter
    if (recvBufSize < 16)
    {
        WM_WARNING("Parameter \"recvBufSize\" must not be less than %d.\n", 16);
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (recvBuf == NULL)
    {
        WM_WARNING("Parameter \"recvBuf\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)recvBuf & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"recvBuf\" is not 32-byte aligned.\n");
    }

    // write specified buffer cache
    DC_StoreRange(recvBuf, recvBufSize);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_START_DCF, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_START_DCF, 2, (u32)recvBuf, (u32)recvBufSize);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

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
WMErrCode
WM_SetDCFData(WMCallbackFunc callback, const u8 *destAdr, const u16 *sendData, u16 sendDataSize)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();
    u32     wMac[2];

    // Check the state
    result = WMi_CheckState(WM_STATE_DCF_CHILD);
    WM_CHECK_RESULT(result);

    // Confirm DCF state
    DC_InvalidateRange(&(p->status->dcf_flag), 4);
    if (p->status->dcf_flag == FALSE)
    {
        WM_WARNING("It is not DCF mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // Check the parameter
    if (sendDataSize > WM_DCF_MAX_SIZE)
    {
        WM_WARNING("Parameter \"sendDataSize\" is over %d.\n", WM_DCF_MAX_SIZE);
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)sendData & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"sendData\" is not 32-byte aligned.\n");
    }

    // write specified buffer cache
    DC_StoreRange((void *)sendData, sendDataSize);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_DCF_DATA, callback);

    // Copy MAC address
    MI_CpuCopy8(destAdr, wMac, 6);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_DCF_DATA, 4,
                             (u32)wMac[0], (u32)wMac[1], (u32)sendData, (u32)sendDataSize);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndDCF

  Description:  Stops the communication in infrastructure mode.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDCF(WMCallbackFunc callback)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Check the state
    result = WMi_CheckState(WM_STATE_DCF_CHILD);
    WM_CHECK_RESULT(result);

    // Confirm DCF state
    DC_InvalidateRange(&(p->status->dcf_flag), 4);
    if (p->status->dcf_flag == FALSE)
    {
        WM_WARNING("It is not DCF mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_END_DCF, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_END_DCF, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
