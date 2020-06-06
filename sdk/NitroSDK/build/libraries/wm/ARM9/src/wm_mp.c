/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_mp.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_mp.c,v $
  Revision 1.20  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.19  12/09/2005 11:09:42  seiki_masashi
  Corrected ignoreSizePrecheckMode determination

  Revision 1.18  12/01/2005 13:58:14  seiki_masashi
  Added ignoreSizePrecheckMode
  Added maxFrequency
  Sorted the operations related to send / receive size.

  Revision 1.17  11/25/2005 00:45:19  seiki_masashi
  Moved mask to within the WMMPParam structure

  Revision 1.16  11/24/2005 03:05:00  seiki_masashi
  Added functions related to the WM_SetMPParameter function
  Changed the WM_StartMP(Ex) function to use the WMMPParam structure.

  Revision 1.15  11/22/2005 09:02:06  seiki_masashi
  Changed to use the MATH_CountPopulation function

  Revision 1.14  04/11/2005 02:06:54  seiki_masashi
  small fix

  Revision 1.13  04/11/2005 01:25:25  seiki_masashi
  Changed to check whether the device is in power-save mode only when it is a child when WM_StartMP is run.

  Revision 1.12  03/08/2005 06:45:26  terui
  Added check to restrict start of MP mode if not in power save mode

  Revision 1.11  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.10  02/14/2005 06:40:04  terui
  Changed the timing for the check of data transmission time based on minPollBmpMode state

  Revision 1.9  01/11/2005 07:46:10  takano_makoto
  fix copyright header.

  Revision 1.8  01/06/2005 02:04:44  seiki_masashi
  Made WM_StartMP, WM_SetMPData, WM_SetMPDataToPort to inline
  Optimized some codes

  Revision 1.7  11/29/2004 04:55:35  seiki_masashi
  fix typo

  Revision 1.6  11/29/2004 04:40:57  seiki_masashi
  Error when sendData in SetMPDataToPort and sendBuf in StartMP are the same.

  Revision 1.5  10/22/2004 04:37:28  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.4  10/05/2004 09:25:45  seiki_masashi
  Added the argument check code of WM_SetMPData*

  Revision 1.3  10/03/2004 07:49:37  ooe
  Merged with ipl_branch. Added support for ignoreFatalError flag.

  Revision 1.2  09/13/2004 04:10:20  seiki_masashi
  Added WM_SetMPFrequency.

  Revision 1.1  09/10/2004 11:03:30  terui
  New upload reflecting the breakup of wm.c.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    function definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         WMi_StartMP

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
                param       -   MP communications parameter structure specified when MP starts.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_StartMP(WMCallbackFunc callback,
                      u16 *recvBuf,
                      u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, WMMPParam * param)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();
    WMStatus *status = p->status;

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_PARENT, WM_STATE_CHILD);
    WM_CHECK_RESULT(result);

    // If the child device is not in power save mode, do not start MP
    DC_InvalidateRange(&(status->aid), 2);
    DC_InvalidateRange(&(status->pwrMgtMode), 2);
    if (status->aid != 0 && status->pwrMgtMode != 1)
    {
        WM_WARNING("Could not start MP. Power save mode is illegal.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // MP state confirmation
    DC_InvalidateRange(&(status->mp_flag), 4);  // Invalidates the ARM7 status region cache
    if (status->mp_flag == TRUE)
    {
        WM_WARNING("Already started MP protocol. So can't execute request.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // Check the parameter
    if ((recvBufSize & 0x3f) != 0)     // recvBufSize/2 must be a multiple of 32 bytes
    {
        WM_WARNING("Parameter \"recvBufSize\" is not a multiple of 64.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((sendBufSize & 0x1f) != 0)     // sendBufSize must be a multiple of 32 bytes
    {
        WM_WARNING("Parameter \"sendBufSize\" is not a multiple of 32.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)recvBuf & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"recvBuf\" is not 32-byte aligned.\n");
    }
    if ((u32)sendBuf & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"sendBuf\" is not 32-byte aligned.\n");
    }

    DC_InvalidateRange(&(status->mp_ignoreSizePrecheckMode),
                       sizeof(status->mp_ignoreSizePrecheckMode));
    if (status->mp_ignoreSizePrecheckMode == FALSE)
    {
        // Runs a preliminary check of the send / receive size.
        if (recvBufSize < WM_GetMPReceiveBufferSize())
        {
            WM_WARNING("Parameter \"recvBufSize\" is not enough size.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
        if (sendBufSize < WM_GetMPSendBufferSize())
        {
            WM_WARNING("Parameter \"sendBufSize\" is not enough size.\n");
            return WM_ERRCODE_INVALID_PARAM;
        }
#ifndef SDK_FINALROM
        // Check time needed to send data
        DC_InvalidateRange(&(status->state), 2);
        if (status->state == WM_STATE_PARENT)
        {
            DC_InvalidateRange(&(status->pparam), sizeof(WMParentParam));
            (void)WMi_CheckMpPacketTimeRequired(status->pparam.parentMaxSize,
                                                status->pparam.childMaxSize,
                                                (u8)(status->pparam.maxEntry));
        }
#endif
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_START_MP, callback);

    // notify ARM7 with FIFO
    {
        WMStartMPReq Req;

        MI_CpuClear32(&Req, sizeof(Req));

        Req.apiid = WM_APIID_START_MP;
        Req.recvBuf = (u32 *)recvBuf;
        Req.recvBufSize = (u32)(recvBufSize / 2);       // Size of one buffer
        Req.sendBuf = (u32 *)sendBuf;
        Req.sendBufSize = (u32)sendBufSize;

        MI_CpuCopy32(param, &Req.param, sizeof(Req.param));

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_StartMPEx

  Description:  Starts the MP communication. Detailed operation mode can be specified. Function that is common to parent and child.

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
WMErrCode
WM_StartMPEx(WMCallbackFunc callback,
             u16 *recvBuf,
             u16 recvBufSize,
             u16 *sendBuf,
             u16 sendBufSize,
             u16 mpFreq,
             u16 defaultRetryCount,
             BOOL minPollBmpMode, BOOL singlePacketMode, BOOL fixFreqMode, BOOL ignoreFatalError)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));

    param.mask = WM_MP_PARAM_MIN_FREQUENCY | WM_MP_PARAM_FREQUENCY | WM_MP_PARAM_DEFAULT_RETRY_COUNT
        | WM_MP_PARAM_MIN_POLL_BMP_MODE | WM_MP_PARAM_SINGLE_PACKET_MODE |
        WM_MP_PARAM_IGNORE_FATAL_ERROR_MODE;
    param.minFrequency = mpFreq;
    param.frequency = mpFreq;
    param.defaultRetryCount = defaultRetryCount;
    param.minPollBmpMode = (u8)minPollBmpMode;
    param.singlePacketMode = (u8)singlePacketMode;
    param.ignoreFatalErrorMode = (u8)ignoreFatalError;

    if (fixFreqMode)
    {
        param.mask |= WM_MP_PARAM_MAX_FREQUENCY;
        param.maxFrequency = mpFreq;
    }

    return WMi_StartMP(callback, recvBuf, recvBufSize, sendBuf, sendBufSize, &param);
}

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
                     u16 *recvBuf, u16 recvBufSize, u16 *sendBuf, u16 sendBufSize, u16 mpFreq)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));

    param.mask = WM_MP_PARAM_FREQUENCY | WM_MP_PARAM_MIN_FREQUENCY;
    param.minFrequency = mpFreq;
    param.frequency = mpFreq;

    return WMi_StartMP(callback, recvBuf, recvBufSize, sendBuf, sendBufSize, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParameter

  Description:  Configures the different parameters for MP communications

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                param       -   Pointer to the structure where the parameters for MP communications are stored

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParameter(WMCallbackFunc callback, const WMMPParam * param)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Check the state
    result = WMi_CheckInitialized();
    WM_CHECK_RESULT(result);

    // register callback function
    WMi_SetCallbackTable(WM_APIID_SET_MP_PARAMETER, callback);

    // notify ARM7 with FIFO
    {
        WMSetMPParameterReq Req;

        MI_CpuClear32(&Req, sizeof(Req));

        Req.apiid = WM_APIID_SET_MP_PARAMETER;
        MI_CpuCopy32(param, &Req.param, sizeof(Req.param));

        result = WMi_SendCommandDirect(&Req, sizeof(Req));
        WM_CHECK_RESULT(result);
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPChildSize

  Description:  Sets the number of bytes a child can send in one MP communication

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                childSize   -   Number of send bytes for a child

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPChildSize(WMCallbackFunc callback, u16 childSize)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_CHILD_SIZE;
    param.childSize = childSize;

    return WM_SetMPParameter(callback, &param);
}

/*---------------------------------------------------------------------------*
  Name:         WM_SetMPParentSize

  Description:  Sets the number of bytes a parent can send in one MP communication

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.
                childSize   -   Number of send bytes for a parent

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_SetMPParentSize(WMCallbackFunc callback, u16 parentSize)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_SIZE;
    param.parentSize = parentSize;

    return WM_SetMPParameter(callback, &param);
}

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
WMErrCode WM_SetMPFrequency(WMCallbackFunc callback, u16 mpFreq)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_FREQUENCY | WM_MP_PARAM_MIN_FREQUENCY;
    param.minFrequency = mpFreq;
    param.frequency = mpFreq;

    return WM_SetMPParameter(callback, &param);
}

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
WMErrCode WM_SetMPInterval(WMCallbackFunc callback, u16 parentInterval, u16 childInterval)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_INTERVAL | WM_MP_PARAM_CHILD_INTERVAL;
    param.parentInterval = parentInterval;
    param.childInterval = childInterval;

    return WM_SetMPParameter(callback, &param);
}

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
WMErrCode WM_SetMPTiming(WMCallbackFunc callback, u16 parentVCount, u16 childVCount)
{
    WMMPParam param;

    MI_CpuClear32(&param, sizeof(param));
    param.mask = WM_MP_PARAM_PARENT_VCOUNT | WM_MP_PARAM_CHILD_VCOUNT;
    param.parentVCount = parentVCount;
    param.childVCount = childVCount;

    return WM_SetMPParameter(callback, &param);
}

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
WMErrCode
WM_SetMPDataToPortEx(WMCallbackFunc callback, void *arg, const u16 *sendData, u16 sendDataSize,
                     u16 destBitmap, u16 port, u16 prio)
{
    WMErrCode result;
    int     maxSendSize;
    BOOL    isParent;
    u16     mpReadyBitmap = 0x0001;
    u16     childBitmap = 0x0001;
    WMArm9Buf *p = WMi_GetSystemWork();
    WMStatus *status = p->status;

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // Get the required data from the WMStatus structure
    DC_InvalidateRange(&(status->mp_maxSendSize), 2);
    maxSendSize = status->mp_maxSendSize;
    DC_InvalidateRange(&(status->aid), 2);
    isParent = (status->aid == 0) ? TRUE : FALSE;
    if (isParent == TRUE)
    {
        DC_InvalidateRange(&(status->child_bitmap), 2);
        childBitmap = status->child_bitmap;
        DC_InvalidateRange(&(status->mp_readyBitmap), 2);
        mpReadyBitmap = status->mp_readyBitmap;
    }

    // Check the parameter
    if (sendData == NULL)
    {
        WM_WARNING("Parameter \"sendData\" must not be NULL.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (childBitmap == 0)
    {
        WM_WARNING("There is no child connected.\n");
        return WM_ERRCODE_NO_CHILD;
    }
    if ((u32)sendData & 0x01f)
    {
        // align check is a warning only, not an error
        WM_WARNING("Parameter \"sendData\" is not 32-byte aligned.\n");
    }
    DC_InvalidateRange(&(status->mp_sendBuf), 2);
    if ((void *)sendData == (void *)status->mp_sendBuf)
    {
        WM_WARNING
            ("Parameter \"sendData\" must not be equal to the WM_StartMP argument \"sendBuf\".\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    // Check the transmission size.
    if (sendDataSize + (isParent ? WM_HEADER_PARENT_MAX_SIZE : WM_HEADER_CHILD_MAX_SIZE) >
        maxSendSize)
    {
        WM_WARNING("Parameter \"sendDataSize\" is over limit.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

    if (sendDataSize == 0)
    {
        WM_WARNING("Parameter \"sendDataSize\" must not be 0.\n");
        return WM_ERRCODE_INVALID_PARAM;
    }

#ifndef SDK_FINALROM
    // Check time needed to send data
    DC_InvalidateRange(&(status->mp_minPollBmpMode), 2);
    if (isParent && status->mp_minPollBmpMode)
    {
        DC_InvalidateRange(&(status->pparam), sizeof(WMParentParam));
        (void)WMi_CheckMpPacketTimeRequired(status->pparam.parentMaxSize,
                                            status->pparam.childMaxSize,
                                            (u8)MATH_CountPopulation((u32)destBitmap));
    }
#endif

    // write specified buffer cache
    DC_StoreRange((void *)sendData, sendDataSize);

    // No need to register the callback function.

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_SET_MP_DATA, 7,
                             (u32)sendData,
                             (u32)sendDataSize,
                             (u32)destBitmap, (u32)port, (u32)prio, (u32)callback, (u32)arg);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WM_EndMP

  Description:  Stops MP communication. Function that is common to parent and child.

  Arguments:    callback    -   Callback function that is called when the asynchronous process completes.

  Returns:      WMErrCode   -   Returns the process result. If the asynchronous process started normally,
                                WM_ERRCODE_OPERATING is returned, and then the result
                                of the asynchronous process is passed to the callback again.
 *---------------------------------------------------------------------------*/
WMErrCode WM_EndMP(WMCallbackFunc callback)
{
    WMErrCode result;
    WMArm9Buf *p = WMi_GetSystemWork();

    // Check the state
    result = WMi_CheckStateEx(2, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD);
    WM_CHECK_RESULT(result);

    // MP state confirmation
    DC_InvalidateRange(&(p->status->mp_flag), 4);
    if (p->status->mp_flag == FALSE)
    {
        WM_WARNING("It is not MP mode now.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // register callback function
    WMi_SetCallbackTable(WM_APIID_END_MP, callback);

    // notify ARM7 with FIFO
    result = WMi_SendCommand(WM_APIID_END_MP, 0);
    WM_CHECK_RESULT(result);

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
