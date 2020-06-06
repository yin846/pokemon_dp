/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_ds.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_ds.c,v $
  Revision 1.23  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.22  11/01/2005 08:56:18  seiki_masashi
  Removed unnecessary status checks.

  Revision 1.21  10/28/2005 11:57:24  seiki_masashi
  small fix

  Revision 1.20  10/28/2005 11:17:33  seiki_masashi
  Support for disconnection from parent.
  Support for additions to port specifications.
  Revised so that structures internal to WM are touched as little as possible.

  Revision 1.19  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.18  02/23/2005 12:48:14  seiki_masashi
  Cleaned up debug display

  Revision 1.17  02/18/2005 12:27:45  seiki_masashi
  Debug switch statement changed from #if to #ifdef

  Revision 1.16  02/18/2005 07:52:31  seiki_masashi
  warning measures

  Revision 1.15  01/14/2005 07:46:55  seiki_masashi
  Previously, child machines picked up unrelated DataSharing packets. This was fixed.

  Revision 1.14  01/14/2005 07:45:12  seiki_masashi
  Cleaned up WmGetSharedDataAddress

  Revision 1.13  01/11/2005 07:46:10  takano_makoto
  fix copyright header.

  Revision 1.12  01/07/2005 11:43:41  seiki_masashi
  Changed OS_Printf  in debug output to OS_TPrintf.

  Revision 1.11  01/06/2005 06:42:56  seiki_masashi
  Made changes to use MATH_CountPopulation.

  Revision 1.10  01/06/2005 01:54:17  seiki_masashi
  Fixed comments and optimized codes.

  Revision 1.9  10/29/2004 03:36:03  seiki_masashi
  Fixed debug messages

  Revision 1.8  10/29/2004 01:41:11  seiki_masashi
  Supported for send completion callback after WM_EndDataSharing.

  Revision 1.7  10/22/2004 04:37:28  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.6  09/23/2004 10:02:10  seiki_masashi
  small fix.

  Revision 1.5  09/23/2004 09:47:31  seiki_masashi
  Added a DataSharing internal error processing routine

  Revision 1.4  09/22/2004 04:06:43  seiki_masashi
  Corrected an error related to DataSharing exclusion

  Revision 1.3  2004/09/21 10:50:38  takano_makoto
  remove warning in WM_GetSharedDataAddress()

  Revision 1.2  09/10/2004 12:02:46  terui
  Bug fix ( port number check ).

  Revision 1.1  09/10/2004 11:03:30  terui
  New upload reflecting the breakup of wm.c.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void WmDataSharingSetDataCallback(void *callback);
static void WmDataSharingReceiveCallback_Parent(void *callback);
static void WmDataSharingReceiveCallback_Child(void *callback);
static void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, u16 aid, u16 *data);
static void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, BOOL delayed);
static u16 *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, u32 aidBitmap, u16 *receiveBuf,
                                   u32 aid);

//// List of port number currently using data-sharing
//static u16 WmDataSharingPortBitmap = 0;

static inline u16 WmDataSharingGetNextIndex(u32 index)
{
    return (u16)((index + 1) % WM_DS_DATASET_NUM);
}

static inline u16 WmDataSharingGetPrevIndex(u32 index)
{
    return (u16)((index + WM_DS_DATASET_NUM - 1) % WM_DS_DATASET_NUM);
}


/*---------------------------------------------------------------------------*
  Name:         WM_StartDataSharing

  Description:  Enables the data sharing feature.

  Arguments:    dsInfo - WMDataSharingInfo structure
                port        -   port number to use
                aidBitmap   -   AID bitmap of the other party for performing data sharing
                dataLength  -   shared memory data length (specify an even of bytes)
                doubleMode  -   Specify TRUE if using WM_StepDataSharing in every frame

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode
WM_StartDataSharing(WMDataSharingInfo *dsInfo, u16 port, u16 aidBitmap, u16 dataLength,
                    BOOL doubleMode)
{
    WMErrCode result;
    int     aid;
    u16     mpReadyBitmap = 0x0001;

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (port >= WM_NUM_OF_PORT)
    {
        WM_WARNING("Parameter \"port\" must be less than %d.\n", WM_NUM_OF_PORT);
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)dsInfo & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"dsInfo\" is not 32-byte aligned.\n");
    }
    if (aidBitmap == 0)
    {
        WM_WARNING("Parameter \"aidBitmap\" must not be 0.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // Get necessary information
    aid = WM_GetAID();
    if (aid == 0)
    {
        mpReadyBitmap = WMi_GetMPReadyAIDs();
    }

    // Initialize data sharing control variables
    MI_CpuClearFast(dsInfo, sizeof(WMDataSharingInfo));
    dsInfo->writeIndex = 0;
    dsInfo->sendIndex = 0;
    dsInfo->readIndex = 0;
    dsInfo->dataLength = dataLength;
    dsInfo->port = port;
    dsInfo->aidBitmap = 0;
    dsInfo->doubleMode = (u16)((doubleMode) ? TRUE : FALSE);

    aidBitmap |= (1 << aid);           // Adds its own portion
    dsInfo->aidBitmap = aidBitmap;

    {
        u16     count = MATH_CountPopulation(aidBitmap);
        dsInfo->stationNumber = count;
        dsInfo->dataSetLength = (u16)(dataLength * count);

        if (dsInfo->dataSetLength > WM_DS_DATA_SIZE)
        {
            // Total data amount exceeds limit
            dsInfo->aidBitmap = 0;
            WM_WARNING("Total size of sharing data must be less than or equal to %d bytes.\n",
                       WM_DS_DATA_SIZE);
            return WM_ERRCODE_INVALID_PARAM;
        }
        dsInfo->dataSetLength += 4;    // aidBitmap, receivedBitmap
    }

    dsInfo->state = WM_DS_STATE_START;

    if (aid == 0)
    {
        // Parent start processing
        int     i;

        for (i = 0; i < WM_DS_DATASET_NUM; i++)
        {
            dsInfo->ds[i].aidBitmap = (u16)(dsInfo->aidBitmap & (mpReadyBitmap | 0x0001));
        }

        (void)WM_SetPortCallback(port, WmDataSharingReceiveCallback_Parent, (void *)dsInfo);

        // Sends empty data for start-up
        for (i = 0; i < ((dsInfo->doubleMode == TRUE) ? 2 : 1); i++)
        {
            int     res;

            dsInfo->writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, (u16 *)&dsInfo->ds[i],
                                     dsInfo->dataSetLength,
                                     (u16)(dsInfo->aidBitmap & mpReadyBitmap), dsInfo->port,
                                     WM_PRIORITY_HIGH);
            if (res == WM_ERRCODE_NO_CHILD)
            {
                dsInfo->seqNum[i] = 0xffff;
                dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            }
            else
            {
                if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
                {
                    WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                    dsInfo->state = WM_DS_STATE_ERROR;
                    return WM_ERRCODE_FAILED;
                }
            }
        }
    }
    else
    {
        // Child start processing
        dsInfo->sendIndex = (u16)(WM_DS_DATASET_NUM - 1);
        (void)WM_SetPortCallback(port, WmDataSharingReceiveCallback_Child, (void *)dsInfo);
    }

    return WM_ERRCODE_SUCCESS;         // Successful completion
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndDataSharing

  Description:  Disables the data sharing feature.

  Arguments:    dsInfo - WMDataSharingInfo structure

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndDataSharing(WMDataSharingInfo *dsInfo)
{
    WMErrCode result;

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // Checks the data sharing state
    if (dsInfo->aidBitmap == 0)
    {
        WM_WARNING("It is not data-sharing mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    (void)WM_SetPortCallback(dsInfo->port, NULL, NULL);
    dsInfo->aidBitmap = 0;
    dsInfo->state = WM_DS_STATE_READY;

    return WM_ERRCODE_SUCCESS;         // Successful completion
}

/*---------------------------------------------------------------------------*
  Name:         WM_StepDataSharing

  Description:  advances synchronicity of data sharing by one

  Arguments:    dsInfo - WMDataSharingInfo structure
                sendData    -   send data you want to share
                receiveData -   received shared data

  Returns:      WMErrCode   -   Returns the process result.
                    *_SUCCESS      : Shared data send/receive successful
                    *_NO_DATASET   : Next shared data has not come yet
 *---------------------------------------------------------------------------*/
WMErrCode WM_StepDataSharing(WMDataSharingInfo *dsInfo, const u16 *sendData, WMDataSet *receiveData)
{
    WMErrCode result;
    u16     aid;
    u16     mpReadyBitmap;
    u16     state;

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // Check the parameter
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (sendData == NULL)
    {
        WM_WARNING("Parameter \"sendData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (receiveData == NULL)
    {
        WM_WARNING("Parameter \"receiveData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // Get necessary information
    aid = WM_GetAID();
    if (aid == 0)
    {
        mpReadyBitmap = WMi_GetMPReadyAIDs();
    }

    // Checks the data sharing state
    state = dsInfo->state;
    if (state == WM_DS_STATE_ERROR)
    {
        WM_WARNING("An error occurred during data-sharing.\n");
        return WM_ERRCODE_FAILED;
    }
    if (state != WM_DS_STATE_START && state != WM_DS_STATE_RETRY_SEND)
    {
        WM_WARNING("It is not data-sharing mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    result = WM_ERRCODE_NO_DATASET;

    if (aid == 0)
    {
        // If a parent
        BOOL    sendFlag = FALSE;
        BOOL    delayed = FALSE;

        if (state == WM_DS_STATE_RETRY_SEND)
        {
            // Resends in case previous time was SEND_QUEUE_FULL
            int     res;
            int     oldWI;
            dsInfo->state = WM_DS_STATE_START;

            WM_DLOG_DATASHARING("send queue was full. do retry.");

            oldWI = WmDataSharingGetPrevIndex(dsInfo->writeIndex);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo,
                                     (u16 *)&dsInfo->ds[oldWI], dsInfo->dataSetLength,
                                     (u16)(dsInfo->aidBitmap & mpReadyBitmap), dsInfo->port,
                                     WM_PRIORITY_HIGH);
            if (res == WM_ERRCODE_NO_CHILD)
            {
                WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d\n",
                                     dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex);
                dsInfo->seqNum[oldWI] = 0xffff;
                dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            }
            else
            {
                if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
                {
                    WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                    dsInfo->state = WM_DS_STATE_ERROR;
                    return WM_ERRCODE_FAILED;
                }
            }
        }

        // After the parent device has finished sending DataSet, it will be available for use, so the limit where sendIndex can be read is decided.
        if (dsInfo->readIndex != dsInfo->sendIndex)
        {
            // Read processing
            WM_DLOGF_DATASHARING("read ds   : write: %d, read: %d, send: %d, seq#: %d",
                                 dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex,
                                 dsInfo->seqNum[dsInfo->readIndex]);

            dsInfo->ds[dsInfo->readIndex].aidBitmap |= 0x0001;  // The lowest bit in the aidBitmap in the transmission data is a delay flag
            MI_CpuCopy16(&dsInfo->ds[dsInfo->readIndex], receiveData, sizeof(WMDataSet));
            dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
            dsInfo->readIndex = WmDataSharingGetNextIndex(dsInfo->readIndex);

            sendFlag = TRUE;
            result = WM_ERRCODE_SUCCESS;
            if (dsInfo->doubleMode == FALSE && mpReadyBitmap != 0
                && dsInfo->ds[dsInfo->writeIndex].aidBitmap == 0x0001)
            {
                // If the parent sets data, it can be sent immediately = frame shifts only with the parent
                delayed = TRUE;
            }
            else
            {
                delayed = FALSE;
            }
        }

        // Sends the DataSet to the send buffer when the data for all the machines is ready
        WmDataSharingSendDataSet(dsInfo, FALSE);

        if (sendFlag)
        {
            // The parent inserts into its own buffer instead of sending
            WM_DLOGF_DATASHARING("send data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            WmDataSharingReceiveData(dsInfo, 0, (u16 *)sendData);       // Remove const because chained rewrite beyond this place is bothersome.

            if (dsInfo->doubleMode == FALSE)
            {
                // Only when not a Step in each frame,
                // the DataSet needs to be prepared for sending with this GF at this point in time.
                // Sends the DataSet to the send buffer when the data for all the machines is ready
                WmDataSharingSendDataSet(dsInfo, delayed);
            }
        }
    }
    else
    {
        // If a child
        BOOL    sendFlag = FALSE;

        if (state == WM_DS_STATE_RETRY_SEND)
        {
            // Resends in case previous time was SEND_QUEUE_FULL
            sendFlag = TRUE;
            dsInfo->state = WM_DS_STATE_START;
            WM_DLOG_DATASHARING("send queue was full. do retry.");
        }
        else
        {
            // After the child device has finished receiving DataSet, it will be available for use, so the limit where writeIndex can be read is decided.
            if (dsInfo->readIndex != dsInfo->writeIndex)
            {
                // If the delay flag is down, one frame is forcibly delayed
                // The lowest bit in the aidBitmap in the transmission data is a delay flag
                if (!(dsInfo->ds[dsInfo->readIndex].aidBitmap & 0x0001))
                {
                    dsInfo->ds[dsInfo->readIndex].aidBitmap |= 0x0001;
                }
                else
                {
                    // Read processing
                    WM_DLOGF_DATASHARING("read ds   : write: %d, read: %d, send: %d, seq#: %d",
                                         dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex,
                                         dsInfo->seqNum[dsInfo->readIndex]);
                    MI_CpuCopy16(&dsInfo->ds[dsInfo->readIndex], receiveData, sizeof(WMDataSet));
                    dsInfo->currentSeqNum = dsInfo->seqNum[dsInfo->readIndex];
                    dsInfo->readIndex = WmDataSharingGetNextIndex(dsInfo->readIndex);

                    sendFlag = TRUE;
                    result = WM_ERRCODE_SUCCESS;
                }
            }
        }

        if (sendFlag)
        {
            // The child transmits as is
            int     res;
            // Use part of dsInfo->ds as xmit buffer
            u16    *buf = (u16 *)(((u8 *)&dsInfo->ds[dsInfo->sendIndex]) + 32); // 32-byte alignment is required

            WM_DLOGF_DATASHARING("send data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            MI_CpuCopy16(sendData, buf, dsInfo->dataLength);
            res =
                WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, buf, dsInfo->dataLength,
                                     dsInfo->aidBitmap, dsInfo->port, WM_PRIORITY_HIGH);
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
            if (res != WM_ERRCODE_OPERATING && res != WM_ERRCODE_SUCCESS)
            {
                WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                dsInfo->state = WM_DS_STATE_ERROR;
                result = WM_ERRCODE_FAILED;
            }
        }
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingSetDataCallback

  Description:  A transmission completion callback.

  Arguments:    callback - A pointer to the callback structure.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingSetDataCallback(void *callback)
{
    WMArm9Buf *p = WMi_GetSystemWork();
    WMPortSendCallback *cb_Port = (WMPortSendCallback *)callback;
    WMDataSharingInfo *dsInfo;
    u16     aid;

    // Checks if WM_EndDataSharing() has been called while DSInfo is in a invalid state.
    dsInfo = (WMDataSharingInfo *)(p->portCallbackArgument[cb_Port->port]);
    if ((p->portCallbackTable[cb_Port->port] != WmDataSharingReceiveCallback_Parent
         && p->portCallbackTable[cb_Port->port] != WmDataSharingReceiveCallback_Child)
        || dsInfo == NULL || dsInfo != (WMDataSharingInfo *)(cb_Port->arg))
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    aid = WM_GetAID();

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        // Send completed
        if (aid == 0)
        {
            // Parent processing
            WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            dsInfo->seqNum[dsInfo->sendIndex] = (u16)(cb_Port->seqNo >> 1);
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
        }
        else
        {
            // Child processing
            WM_DLOGF_DATASHARING("sent data : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);
        }
    }
    else
    {
        if (cb_Port->errcode == WM_ERRCODE_SEND_QUEUE_FULL)
        {
            // Resend as the transmission queue was full
            if (aid != 0)
            {
                // For the child, put the sendIndex back one
                dsInfo->sendIndex = WmDataSharingGetPrevIndex(dsInfo->sendIndex);
            }
            dsInfo->state = WM_DS_STATE_RETRY_SEND;
            WM_DLOG_DATASHARING("send queue is full. will retry.");
        }
        else
        {
            WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n",
                       cb_Port->errcode);
            dsInfo->state = WM_DS_STATE_ERROR;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveCallback_Parent

  Description:  A parent port receiving callback

  Arguments:    callback - A pointer to the callback structure.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveCallback_Parent(void *callback)
{
    WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback;
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)cb_Port->arg;

    if (dsInfo == NULL)
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (cb_Port->state)
        {
        case WM_STATECODE_PORT_RECV:
            // Store the received child data in a buffer
            WmDataSharingReceiveData(dsInfo, cb_Port->aid, cb_Port->data);
            WmDataSharingSendDataSet(dsInfo, FALSE);
            break;

        case WM_STATECODE_CONNECTED:
            WmDataSharingSendDataSet(dsInfo, FALSE);    // Should not be required
            break;

        case WM_STATECODE_DISCONNECTED:
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            {
                u32     aidBit;
                u32     writeIndex;
                OSIntrMode enabled;
                aidBit = 1U << cb_Port->aid;
                enabled = OS_DisableInterrupts();
                writeIndex = dsInfo->writeIndex;
                dsInfo->ds[writeIndex].aidBitmap &= ~aidBit;
                if (dsInfo->doubleMode == TRUE)
                {
                    dsInfo->ds[WmDataSharingGetNextIndex(writeIndex)].aidBitmap &= ~aidBit;
                }
                (void)OS_RestoreInterrupts(enabled);
                WmDataSharingSendDataSet(dsInfo, FALSE);
                if (dsInfo->doubleMode == TRUE)
                {
                    WmDataSharingSendDataSet(dsInfo, FALSE);
                }
            }
            break;

        case WM_STATECODE_PORT_INIT:
            break;
        }
    }
    else
    {
        WM_WARNING("An unknown receiving error occured during data-sharing. errcode=%x\n",
                   cb_Port->errcode);
        dsInfo->state = WM_DS_STATE_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveCallback_Child

  Description:  A child port receiving callback.

  Arguments:    callback - A pointer to the callback structure.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveCallback_Child(void *callback)
{
    WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback;
    WMDataSharingInfo *dsInfo = (WMDataSharingInfo *)cb_Port->arg;

    if (dsInfo == NULL)
    {
        WM_WARNING("data-sharing has already terminated.");
        return;
    }

    if (cb_Port->errcode == WM_ERRCODE_SUCCESS)
    {
        switch (cb_Port->state)
        {
        case WM_STATECODE_PORT_RECV:
            {
                u32     length;
                u32     aid;
                u32     aidBitmap;
                WMDataSet *dataSet;

                WM_DLOGF_DATASHARING("recv ds   : write: %d, read: %d, send: %d",
                                     dsInfo->writeIndex, dsInfo->readIndex, dsInfo->sendIndex);

                // Saves the DataSet
                dataSet = (WMDataSet *)(cb_Port->data);
                aidBitmap = dataSet->aidBitmap;
                length = cb_Port->length;
                aid = WM_GetAID();

                if (length != dsInfo->dataSetLength)
                {
                    WM_WARNING("received DataSharing data size(%d) != dsInfo->dataSetLength(%d)\n",
                               length, dsInfo->dataSetLength);
                    if (length > sizeof(WMDataSet))
                    {
                        // If incoming data is longer than the buffer, correct it.
                        WM_WARNING("received DataSharing data exceeds sizeof(WMDataSet)\n");
                        length = sizeof(WMDataSet);
                    }
                }
#ifdef SDK_DEBUG
                if (aidBitmap & ~(dsInfo->aidBitmap))
                {
                    // Larger than aidBitmap expected by child
                    WM_WARNING("received aidBitmap(%x) has too many members.\n", aidBitmap);
                }
#endif
                if (length >= 4 && (aidBitmap & (1 << aid)))
                {
                    MI_CpuCopy16(dataSet, &dsInfo->ds[dsInfo->writeIndex], length);
                    dsInfo->seqNum[dsInfo->writeIndex] = (u16)(cb_Port->seqNo >> 1);
                    dsInfo->writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
                }
            }
            break;
        case WM_STATECODE_PORT_INIT:
        case WM_STATECODE_CONNECTED:
        case WM_STATECODE_DISCONNECTED:
        case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
            break;
        }
    }
    else
    {
        WM_WARNING("An unknown receiving error occured during data-sharing. errcode=%x\n",
                   cb_Port->errcode);
        dsInfo->state = WM_DS_STATE_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingReceiveData

  Description:  Stores the data as the parent received data from each machine.

  Arguments:    dsInfo      - WMDataSharingInfo structure
                aid    - AID for the terminal that received data.
                data   - A pointer to the received data.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmDataSharingReceiveData(WMDataSharingInfo *dsInfo, u16 aid, u16 *data)
{
    u16     aidBit = (u16)(1 << aid);

    WM_DLOGF_DATASHARING("recv data%d: write: %d, read: %d, send: %d", aid, dsInfo->writeIndex,
                         dsInfo->readIndex, dsInfo->sendIndex);

    // Confirms whether a processing target
    if (dsInfo->aidBitmap & aidBit)
    {
        u16    *buf;
        u16     writeIndex;
        OSIntrMode enabled;

        if (!(dsInfo->ds[dsInfo->writeIndex].aidBitmap & aidBit))
        {
            if (dsInfo->doubleMode == TRUE)
            {
                WM_DLOGF_DATASHARING("[DS] received two DS packets from aid %d", aid);

                writeIndex = WmDataSharingGetNextIndex(dsInfo->writeIndex);
                if (!(dsInfo->ds[writeIndex].aidBitmap & aidBit))
                {
                    // Up to two are stored in the buffer. Anything beyond that is discarded.
                    OS_Warning("received too many DataSharing packets from aid %d. discarded.\n",
                               aid);
                    return;
                }
            }
            else
            {
                // If not in doubleMode, the buffer can hold up to one
                OS_Warning("received too many DataSharing packets from aid %d. discarded.\n", aid);
                return;
            }
        }
        else
        {
            writeIndex = dsInfo->writeIndex;
        }

        buf = WmGetSharedDataAddress(dsInfo, dsInfo->aidBitmap, dsInfo->ds[writeIndex].data, aid);
        if (data != NULL)
        {
            MI_CpuCopy16(data, buf, dsInfo->dataLength);
        }
        else
        {
            MI_CpuClear16(buf, dsInfo->dataLength);
        }

        enabled = OS_DisableInterrupts();
        // Puts down the unreceived flag
        dsInfo->ds[writeIndex].aidBitmap &= ~aidBit;
        // Puts up the received flag
        dsInfo->ds[writeIndex].receivedBitmap |= aidBit;
        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WmDataSharingSendDataSet

  Description:  After the parent confirms reception of each machine's data, the data set is sent.

  Arguments:    dsInfo      - WMDataSharingInfo structure
                delayed - TRUE if in frame delay state

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WmDataSharingSendDataSet(WMDataSharingInfo *dsInfo, BOOL delayed)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();
    // If receiving is finished for all the machines,
    // (The ds[].aidBitmap acts as unreceived flag during the receiving buffer)
    if (dsInfo->ds[dsInfo->writeIndex].aidBitmap == 0)
    {
        u16     newWI, oldWI, resetWI;
        WMErrCode res;
        u16     mpReadyBitmap;

        WM_DLOGF_DATASHARING("send ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                             dsInfo->readIndex, dsInfo->sendIndex);

        mpReadyBitmap = WMi_GetMPReadyAIDs();
        oldWI = dsInfo->writeIndex;
        newWI = WmDataSharingGetNextIndex(oldWI);
        if (dsInfo->doubleMode == TRUE)
        {
            resetWI = WmDataSharingGetNextIndex(newWI);
        }
        else
        {
            resetWI = newWI;
        }
        SDK_ASSERT(newWI != dsInfo->readIndex && resetWI != dsInfo->readIndex);
//        SDK_ASSERT( dsInfo->sendIndex == dsInfo->writeIndex);
        MI_CpuClear16(&dsInfo->ds[resetWI], sizeof(WMDataSet));
        dsInfo->ds[resetWI].aidBitmap = (u16)(dsInfo->aidBitmap & (mpReadyBitmap | 0x0001));
        dsInfo->writeIndex = newWI;
        dsInfo->ds[oldWI].aidBitmap = dsInfo->aidBitmap;        // enter an original value in aidBitmap before sending
        if (delayed == TRUE)
        {
            // The lowest bit in the aidBitmap is a delay flag
            dsInfo->ds[oldWI].aidBitmap &= ~0x0001;
        }
        (void)OS_RestoreInterrupts(enabled);
        res =
            WM_SetMPDataToPortEx(WmDataSharingSetDataCallback, dsInfo, (u16 *)&dsInfo->ds[oldWI],
                                 dsInfo->dataSetLength, (u16)(dsInfo->aidBitmap & mpReadyBitmap),
                                 dsInfo->port, WM_PRIORITY_HIGH);
        if (res == WM_ERRCODE_NO_CHILD)
        {
            WM_DLOGF_DATASHARING("sent ds   : write: %d, read: %d, send: %d", dsInfo->writeIndex,
                                 dsInfo->readIndex, dsInfo->sendIndex);

            dsInfo->seqNum[oldWI] = 0xffff;
            dsInfo->sendIndex = WmDataSharingGetNextIndex(dsInfo->sendIndex);
        }
        else
        {
            if (res != WM_ERRCODE_SUCCESS && res != WM_ERRCODE_OPERATING)
            {
                WM_WARNING("WM_SetMPDataToPort failed during data-sharing. errcode=%x\n", res);
                dsInfo->state = WM_DS_STATE_ERROR;
            }
        }
    }
    else
    {
        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetSharedDataAddress

  Description:  Gets specific AID address in data sharing receive data.

  Arguments:    dsInfo - WMDataSharingInfo structure
                receiveData -   received shared data
                aid         -   AID

  Returns:      u16*    - The address for the receiving data. If there is no data, returns NULL.
 *---------------------------------------------------------------------------*/
u16    *WM_GetSharedDataAddress(WMDataSharingInfo *dsInfo, WMDataSet *receiveData, u16 aid)
{
    u32     aidBitmap = receiveData->aidBitmap;
    u32     receivedBitmap = receiveData->receivedBitmap;
    u32     aidBit = (1U << aid);

    // Check the parameter
    if (dsInfo == NULL)
    {
        WM_WARNING("Parameter \"dsInfo\" must not be NULL.\n");
        return NULL;
    }

    if (receiveData == NULL)
    {
        WM_WARNING("Parameter \"receiveData\" must not be NULL.\n");
        return NULL;
    }

    if (!(aidBitmap & aidBit))
    {
        WM_WARNING("Parameter \"aid\" must be a member of \"receiveData->aidBitmap\".\n");
        return NULL;
    }

    if (!(receivedBitmap & aidBit))
    {
        // cannot receive data
        return NULL;
    }

    return WmGetSharedDataAddress(dsInfo, aidBitmap, receiveData->data, aid);
}

/*---------------------------------------------------------------------------*
  Name:         WmGetSharedDataAddress

  Description:  Gets the specific AID address in the receiving buffer

  Arguments:    dsInfo - WMDataSharingInfo structure
                aidBitmap - Other communication party included in the data
                receiveBuf - Receiving buffer
                aid         -   AID
                                Confirm aidBitmap & (1<<aid) before calling

  Returns:      u16*    - The address for the receiving data.
 *---------------------------------------------------------------------------*/
u16    *WmGetSharedDataAddress(WMDataSharingInfo *dsInfo, u32 aidBitmap, u16 *receiveBuf, u32 aid)
{
    u32     mask;
    u32     count;
    u32     offset;

    // Count the 1s in the aidBitmap from the bottom
    mask = (0x0001U << aid) - 1U;
    aidBitmap &= mask;
    count = MATH_CountPopulation(aidBitmap);
    offset = dsInfo->dataLength * count;

    return (u16 *)(((u8 *)receiveBuf) + offset);
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
