/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - libraries
  File:     external.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: external.c,v $
  Revision 1.14  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.13  09/30/2005 00:27:04  yada
  calculate day of week instead of reading rtc

  Revision 1.12  02/28/2005 05:26:07  yosizaki
  do-indent.

  Revision 1.11  11/02/2004 04:13:31  terui
  Comments revised.

  Revision 1.10  08/19/2004 13:43:01  terui
  Removed HourFormat related definitions.

  Revision 1.9  08/19/2004 12:50:36  terui
  Fixed to stabilize to 24 hours display    

  Revision 1.8  08/19/2004 07:49:37  terui
  Revised overwrite editing in "Adjust" in the RTC_GetAlarmStatusAsync function to the original state.

  Revision 1.7  08/04/2004 05:31:48  miya
  Added functionality for use in factory settings.    

  Revision 1.6  07/01/2004 03:16:52  yosizaki
  change around RTCHourFormat to internal use.

  Revision 1.5  05/31/2004 08:44:45  terui
  Changed PXI callback functions logic

  Revision 1.4  05/18/2004 01:37:33  terui
  Support for ARM7 with PXI synchronized mechanism

  Revision 1.3  05/18/2004 01:15:12  terui
  Support for ARM7 with PXI synchronized mechanism

  Revision 1.2  05/17/2004 11:05:20  terui
  Improved PXI processing result to API only processing result
  Changed to return processing result type from BOOL for asynchronized functions

  Revision 1.1  05/12/2004 02:36:08  terui
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/rtc.h>


/*---------------------------------------------------------------------------*
    structure definitions
 *---------------------------------------------------------------------------*/
// Asynchronized Function Exclusive Processing Lock Definition
typedef enum RTCLock
{
    RTC_LOCK_OFF = 0,                  // Unlock status
    RTC_LOCK_ON,                       // Lock status
    RTC_LOCK_MAX
}
RTCLock;

// Sequence definition for processing including continuous command send
typedef enum RTCSequence
{
    RTC_SEQ_GET_DATE = 0,              // date obtain sequence
    RTC_SEQ_GET_TIME,                  // time obtain sequence
    RTC_SEQ_GET_DATETIME,              // date-time obtain sequence
    RTC_SEQ_SET_DATE,                  // date setup sequence
    RTC_SEQ_SET_TIME,                  // time setup sequence
    RTC_SEQ_SET_DATETIME,              // date-time setup sequence
    RTC_SEQ_GET_ALARM1_STATUS,         // alarm 1 status obtain sequence
    RTC_SEQ_GET_ALARM2_STATUS,         // alarm 2 status obtain sequence
    RTC_SEQ_GET_ALARM_PARAM,           // alarm setup value obtain sequence
    RTC_SEQ_SET_ALARM1_STATUS,         // alarm 1 status change sequence
    RTC_SEQ_SET_ALARM2_STATUS,         // alarm 2 status change sequence
    RTC_SEQ_SET_ALARM1_PARAM,          // alarm 1 setup value change sequence
    RTC_SEQ_SET_ALARM2_PARAM,          // alarm 2 setup value change sequence
    RTC_SEQ_SET_HOUR_FORMAT,           // time display format change sequence    
    RTC_SEQ_SET_REG_STATUS2,           // status 2 register write sequence
    RTC_SEQ_SET_REG_ADJUST,            // adjust register write sequence
    RTC_SEQ_MAX
}
RTCSequence;

// Work Structure
typedef struct RTCWork
{
    u32     lock;                      // exception lock
    RTCCallback callback;              // for temporary storage of asynchronized function callback    
    void   *buffer[2];                 // for temporary storage of asynchronized function parameters    
    void   *callbackArg;               // for saving callback function arguments
    u32     sequence;                  // for controlling continuous processing mode
    u32     index;                     // for controlling continuous processing status
    RTCInterrupt interrupt;            // for temporary storage of call functions during alarm notification
    RTCResult commonResult;            // for temporary storage of asynchronized function processing result     

}
RTCWork;


/*---------------------------------------------------------------------------*
    Static Variable Definitions
 *---------------------------------------------------------------------------*/
static u16 rtcInitialized;             // initialization verify flag
static RTCWork rtcWork;                // structure with work variables combined


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void RtcCommonCallback(PXIFifoTag tag, u32 data, BOOL err);
static u32 RtcBCD2HEX(u32 bcd);
static u32 RtcHEX2BCD(u32 hex);
static BOOL RtcCheckAlarmParam(const RTCAlarmParam *param);
static RTCRawAlarm RtcMakeAlarmParam(const RTCAlarmParam *param);
static BOOL RtcCheckDate(const RTCDate *date, RTCRawDate *raw);
static BOOL RtcCheckTime(const RTCTime *time, RTCRawTime *raw);
static void RtcGetResultCallback(RTCResult result, void *arg);
static void RtcWaitBusy(void);

/*---------------------------------------------------------------------------*
  Name:         RTC_Init

  Description:  Initializes the RTC library.
       Notice:  Does a power-up check by initializing the component side.
                At this time if power supply to RTC is stopped for a moment,    
                the internal status of RTC is reset and goes to 2000/01/01/00:00:00(12-hour format)0 day(Sunday).    
                The alarm setup value is 0 cleared.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTC_Init(void)
{
    // non-initialization verification
    if (rtcInitialized)
    {
        return;
    }
    rtcInitialized = 1;

    // work variable initialization
    rtcWork.lock = RTC_LOCK_OFF;
    rtcWork.callback = NULL;
    rtcWork.interrupt = NULL;
    rtcWork.buffer[0] = NULL;
    rtcWork.buffer[1] = NULL;

    // Wait until ARM7 RTC library is started.
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_RTC, PXI_PROC_ARM7))
    {
    }

    // Set the PXI callback function
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_RTC, RtcCommonCallback);
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateAsync

  Description:  Asynchronously reads date data from the RTC.

  Arguments:    date      - Specifies the buffer for storing date data.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateAsync(RTCDate *date, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_ASSERT(date != NULL);
    SDK_ASSERT(callback != NULL);

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send date read command
    rtcWork.sequence = RTC_SEQ_GET_DATE;
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)date;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawDateAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDate

  Description:  Reads date data from the RTC.

  Arguments:    date      - Specifies the buffer for storing date data.

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDate(RTCDate *date)
{
    rtcWork.commonResult = RTC_GetDateAsync(date, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetTimeAsync

  Description:  Asynchronously reads time data from the RTC.

  Arguments:    time      - Specifies the buffer for storing time data.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetTimeAsync(RTCTime *time, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(time);
    SDK_NULL_ASSERT(callback);

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send time read command
    rtcWork.sequence = RTC_SEQ_GET_TIME;
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)time;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawTimeAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetTime

  Description:  Reads time data from the RTC.

  Arguments:    time      - Specifies the buffer for storing time data.

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetTime(RTCTime *time)
{
    rtcWork.commonResult = RTC_GetTimeAsync(time, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTimeAsync

  Description:  Asynchronously reads date and time data from the RTC.

  Arguments:    date      - Specifies the buffer for storing date data.
                time      - Specifies the buffer for storing time data.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTimeAsync(RTCDate *date, RTCTime *time, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(date);
    SDK_NULL_ASSERT(time);
    SDK_NULL_ASSERT(callback);

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send time read command
    rtcWork.sequence = RTC_SEQ_GET_DATETIME;
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)date;
    rtcWork.buffer[1] = (void *)time;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawDateTimeAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDateTime

  Description:  Read date, time data from RTC.

  Arguments:    date      - Specifies the buffer for storing date data.
                time      - Specifies the buffer for storing time data.

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetDateTime(RTCDate *date, RTCTime *time)
{
    rtcWork.commonResult = RTC_GetDateTimeAsync(date, time, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateAsync

  Description:  Asynchronously writes date data to the RTC.

  Arguments:    date      - Specifies the buffer where date data is stored.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateAsync(const RTCDate *date, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(date);
    SDK_NULL_ASSERT(callback);

    // Verify and edit set date
    if (!RtcCheckDate(date, &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.date)))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send date write command
    rtcWork.sequence = RTC_SEQ_SET_DATE;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawDateAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDate

  Description:  Writes date data to the RTC.

  Arguments:    date      - Specifies the buffer where date data is stored.

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDate(const RTCDate *date)
{
    rtcWork.commonResult = RTC_SetDateAsync(date, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetTimeAsync

  Description:  Asynchronously writes time data to the RTC.

  Arguments:    time      - Specifies the buffer where time data is stored.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetTimeAsync(const RTCTime *time, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(time);
    SDK_NULL_ASSERT(callback);

    // Verify and edit set time
    if (!RtcCheckTime(time, &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.time)))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send time write command
    rtcWork.sequence = RTC_SEQ_SET_TIME;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawTimeAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetTime

  Description:  Writes time data to the RTC.

  Arguments:    time      - Specifies the buffer where time data is stored.

  Returns:      RTCResult - return result of device operation
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetTime(const RTCTime *time)
{
    rtcWork.commonResult = RTC_SetTimeAsync(time, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateTimeAsync

  Description:  Asynchronously writes date and time data to the RTC.

  Arguments:    date      - Specifies the buffer where date data is stored.
                time      - Specifies the buffer where time data is stored.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult
RTC_SetDateTimeAsync(const RTCDate *date, const RTCTime *time, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(date);
    SDK_NULL_ASSERT(time);
    SDK_NULL_ASSERT(callback);

    // Verify and edit set date/time
    if (!RtcCheckDate(date, &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.date)))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }
    if (!RtcCheckTime(time, &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.time)))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send date/time write command
    rtcWork.sequence = RTC_SEQ_SET_DATETIME;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawDateTimeAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetDateTime

  Description:  Writes date, time data to the RTC.

  Arguments:    date      - Specifies the buffer where date data is stored.
                time      - Specifies the buffer where time data is stored.

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetDateTime(const RTCDate *date, const RTCTime *time)
{
    rtcWork.commonResult = RTC_SetDateTimeAsync(date, time, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}




/*---------------------------------------------------------------------------*
  Name:         RTC_SetRegStatus2Async

  Description:  Writes data to RTC status 2 register.

  Arguments:    status2   - Specifies the buffer that holds status 2 register content.    
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult RTCi_SetRegStatus2Async(const RTCRawStatus2 *status2, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(status2);
    SDK_NULL_ASSERT(callback);

    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2.intr_mode = status2->intr_mode;
    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2.intr2_mode =
        status2->intr2_mode;
    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2.test = status2->test;

    /* No parameter check*/
    // return RTC_RESULT_ILLEGAL_PARAMETER;


    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // Send the status 2 register write command.
    rtcWork.sequence = RTC_SEQ_SET_REG_STATUS2;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawStatus2Async())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetRegStatus2

  Description:  Writes data to RTC status 2 register.

  Arguments:    status2   - Specifies the buffer that holds status 2 register content.    

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTCi_SetRegStatus2(const RTCRawStatus2 *status2)
{
    rtcWork.commonResult = RTCi_SetRegStatus2Async(status2, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}


/*---------------------------------------------------------------------------*
  Name:         RTC_SetRegAdjustAsync

  Description:  Writes data to the RTC adjust register.

  Arguments:    adjust    - Specifies the buffer that holds adjust register content.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult RTCi_SetRegAdjustAsync(const RTCRawAdjust *adjust, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(adjust);
    SDK_NULL_ASSERT(callback);

    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.adjust.adjust = adjust->adjust;

    /* No parameter check*/
    // return RTC_RESULT_ILLEGAL_PARAMETER;

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // Send the status 2 register write command.
    rtcWork.sequence = RTC_SEQ_SET_REG_ADJUST;
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_WriteRawAdjustAsync())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}


/*---------------------------------------------------------------------------*
  Name:         RTC_SetRegAdjust

  Description:  Writes data to the RTC adjust register.

  Arguments:    status2   - Specifies the buffer that holds adjust register content.

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTCi_SetRegAdjust(const RTCRawAdjust *Adjust)
{
    rtcWork.commonResult = RTCi_SetRegAdjustAsync(Adjust, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}



/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmStatusAsync

  Description:  Asynchronously reads alarm ON/OFF status from the RTC.

  Arguments:    chan      - Specifies an alarm channel
                status    - Specifies a buffer for storing alarm status.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult
RTC_GetAlarmStatusAsync(RTCAlarmChan chan, RTCAlarmStatus *status, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(status);
    SDK_NULL_ASSERT(callback);

    // confirms parameters
    if (chan >= RTC_ALARM_CHAN_MAX)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }
    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send status 2 register read command
    switch (chan)
    {
    case RTC_ALARM_CHAN_1:
        rtcWork.sequence = RTC_SEQ_GET_ALARM1_STATUS;
        break;
    case RTC_ALARM_CHAN_2:
        rtcWork.sequence = RTC_SEQ_GET_ALARM2_STATUS;
        break;
    }
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)status;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawStatus2Async())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmStatus

  Description:  Reads alarm ON/OFF status from the RTC.

  Arguments:    chan      - Specifies an alarm channel
                status    - Specifies a buffer for storing alarm status.

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmStatus(RTCAlarmChan chan, RTCAlarmStatus *status)
{
    rtcWork.commonResult = RTC_GetAlarmStatusAsync(chan, status, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmParamAsync

  Description:  Asynchronously reads alarm parameter values from the RTC.    

  Arguments:    chan      - Specifies an alarm channel
                param     - Specifies a buffer for storing alarm parameter values.    
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult
RTC_GetAlarmParamAsync(RTCAlarmChan chan, RTCAlarmParam *param, RTCCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(param);
    SDK_NULL_ASSERT(callback);

    // confirms parameters
    if (chan >= RTC_ALARM_CHAN_MAX)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send alarm 1 or 2 setup value read command
    rtcWork.sequence = RTC_SEQ_GET_ALARM_PARAM;
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)param;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (chan == RTC_ALARM_CHAN_1)
    {
        if (RTCi_ReadRawAlarm1Async())
        {
            return RTC_RESULT_SUCCESS;
        }
        else
        {
            return RTC_RESULT_SEND_ERROR;
        }
    }
    if (RTCi_ReadRawAlarm2Async())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetAlarmParam

  Description:  Reads alarm parameter values from the RTC.    

  Arguments:    chan      - Specifies an alarm channel
                param     - Specifies a buffer for storing alarm parameter values.    

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_GetAlarmParam(RTCAlarmChan chan, RTCAlarmParam *param)
{
    rtcWork.commonResult = RTC_GetAlarmParamAsync(chan, param, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmInterrupt

  Description:  Sets the callback function for when an alarm interrupt is generated.

  Arguments:    interrupt - Specifies a callback function.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTC_SetAlarmInterrupt(RTCInterrupt interrupt)
{
    rtcWork.interrupt = interrupt;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmStatusAsync

  Description:  Asynchronously writes alarm status to the RTC.

  Arguments:    chan      - Specifies an alarm channel
                status    - Specifies the buffer where alarm status is stored.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult
RTC_SetAlarmStatusAsync(RTCAlarmChan chan, const RTCAlarmStatus *status, RTCCallback callback,
                        void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(status);
    SDK_NULL_ASSERT(callback);

    // confirms parameters
    if (chan >= RTC_ALARM_CHAN_MAX)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }
    if (*status > RTC_ALARM_STATUS_ON)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send status 2 register read command
    switch (chan)
    {
    case RTC_ALARM_CHAN_1:
        rtcWork.sequence = RTC_SEQ_SET_ALARM1_STATUS;
        break;
    case RTC_ALARM_CHAN_2:
        rtcWork.sequence = RTC_SEQ_SET_ALARM2_STATUS;
        break;
    }
    rtcWork.index = 0;
    rtcWork.buffer[0] = (void *)status;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    if (RTCi_ReadRawStatus2Async())
    {
        return RTC_RESULT_SUCCESS;
    }
    else
    {
        return RTC_RESULT_SEND_ERROR;
    }
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmStatus

  Description:  Writes alarm status to the RTC.

  Arguments:    chan      - Specifies an alarm channel
                status    - Specifies the buffer where alarm status is stored.

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmStatus(RTCAlarmChan chan, const RTCAlarmStatus *status)
{
    rtcWork.commonResult = RTC_SetAlarmStatusAsync(chan, status, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmParamAsync

  Description:  Asynchronously writes alarm parameters to the RTC.   
       Notice:  Write will fail if the RTC alarm status is not ON,
                because the device side will not accept write.

  Arguments:    chan      - Specifies an alarm channel
                param     - Specifies the buffer where alarm parameter values are stored.    
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      RTCResult - Returns the result of the process that starts the asynchronous device operation.
 *---------------------------------------------------------------------------*/
RTCResult
RTC_SetAlarmParamAsync(RTCAlarmChan chan, const RTCAlarmParam *param, RTCCallback callback,
                       void *arg)
{
    OSIntrMode enabled;
    RTCRawAlarm *pAlarm = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.alarm);
    BOOL    result = FALSE;

    SDK_NULL_ASSERT(param);
    SDK_NULL_ASSERT(callback);

    // confirms parameters
    if (chan >= RTC_ALARM_CHAN_MAX)
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }
    if (!RtcCheckAlarmParam(param))
    {
        return RTC_RESULT_ILLEGAL_PARAMETER;
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (rtcWork.lock != RTC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return RTC_RESULT_BUSY;
    }
    rtcWork.lock = RTC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // edit the set data
    rtcWork.index = 0;
    rtcWork.callback = callback;
    rtcWork.callbackArg = arg;
    ((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.alarm = RtcMakeAlarmParam(param);
    // divide send command by alarm number
    switch (chan)
    {
    case RTC_ALARM_CHAN_1:
        // send alarm 1 register write command
        rtcWork.sequence = RTC_SEQ_SET_ALARM1_PARAM;
        result = RTCi_WriteRawAlarm1Async();
        break;
    case RTC_ALARM_CHAN_2:
        // send alarm 2 register write command
        rtcWork.sequence = RTC_SEQ_SET_ALARM2_PARAM;
        result = RTCi_WriteRawAlarm2Async();
        break;
    }
    if (result)
    {
        return RTC_RESULT_SUCCESS;
    }
    return RTC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_SetAlarmParam

  Description:  Writes alarm parameter values to the RTC.   

  Arguments:    chan      - Specifies an alarm channel
                param     - Specifies the buffer where alarm parameter values are stored.    

  Returns:      RTCResult - Returns the device operation process result.
 *---------------------------------------------------------------------------*/
RTCResult RTC_SetAlarmParam(RTCAlarmChan chan, const RTCAlarmParam *param)
{
    rtcWork.commonResult = RTC_SetAlarmParamAsync(chan, param, RtcGetResultCallback, NULL);
    if (rtcWork.commonResult == RTC_RESULT_SUCCESS)
    {
        RtcWaitBusy();
    }
    return rtcWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         RtcCommonCallback

  Description:  common callback function for asynchronized RTC functions

  Arguments:    tag -  PXI tag which show message type.
                data - message from ARM7.
                err -  PXI transfer error flag.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void RtcCommonCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag )

    RTCResult result;
    RTCPxiResult pxiresult;
    u8      command;
    RTCCallback cb;

    // verify PXI communication error
    if (err)
    {
        // forcibly end sequence
        if (rtcWork.index)
        {
            rtcWork.index = 0;
        }
        if (rtcWork.lock != RTC_LOCK_OFF)
        {
            rtcWork.lock = RTC_LOCK_OFF;
        }
        if (rtcWork.callback)
        {
            cb = rtcWork.callback;
            rtcWork.callback = NULL;
            cb(RTC_RESULT_FATAL_ERROR, rtcWork.callbackArg);
        }
        return;
    }

    // analyze received data
    command = (u8)((data & RTC_PXI_COMMAND_MASK) >> RTC_PXI_COMMAND_SHIFT);
    pxiresult = (RTCPxiResult)((data & RTC_PXI_RESULT_MASK) >> RTC_PXI_RESULT_SHIFT);

    // verify alarm interrupt
    if (command == RTC_PXI_COMMAND_INTERRUPT)
    {
        // It is possible to determine if alarm 1 or 2 with pxiresult, but     
        // callback is same so ignores distinction.    
        if (rtcWork.interrupt)
        {
            // callback alarm interrupt notification
            rtcWork.interrupt();
        }
        return;
    }

    // If processing success response, process each type later based on internal status.    
    if (pxiresult == RTC_PXI_RESULT_SUCCESS)
    {
        result = RTC_RESULT_SUCCESS;
        switch (rtcWork.sequence)
        {
            // date obtain sequence
        case RTC_SEQ_GET_DATE:
            {
                RTCDate *pDst = (RTCDate *)(rtcWork.buffer[0]);
                RTCRawDate *pSrc = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.date);

                pDst->year = RtcBCD2HEX(pSrc->year);
                pDst->month = RtcBCD2HEX(pSrc->month);
                pDst->day = RtcBCD2HEX(pSrc->day);
                pDst->week = RTC_GetDayOfWeek(pDst);
            }
            break;
            // time obtain sequence
        case RTC_SEQ_GET_TIME:
            {
                RTCTime *pDst = (RTCTime *)(rtcWork.buffer[0]);
                RTCRawTime *pSrc = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.time);

                pDst->hour = RtcBCD2HEX(pSrc->hour);
                pDst->minute = RtcBCD2HEX(pSrc->minute);
                pDst->second = RtcBCD2HEX(pSrc->second);
            }
            break;
            // date-time obtain sequence
        case RTC_SEQ_GET_DATETIME:
            {
                RTCDate *pDst = (RTCDate *)(rtcWork.buffer[0]);
                RTCRawDate *pSrc = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.date);

                //pDst->year =  RtcBCD2HEX( pSrc->year );   // Changed to the following code because the value is not passed for some reason
                pDst->year = RtcBCD2HEX(*(u32 *)pSrc & 0x000000ff);
                pDst->month = RtcBCD2HEX(pSrc->month);
                pDst->day = RtcBCD2HEX(pSrc->day);
                pDst->week = RTC_GetDayOfWeek(pDst);
            }
            {
                RTCTime *pDst = (RTCTime *)(rtcWork.buffer[1]);
                RTCRawTime *pSrc = &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->t.time);

                pDst->hour = RtcBCD2HEX(pSrc->hour);
                pDst->minute = RtcBCD2HEX(pSrc->minute);
                pDst->second = RtcBCD2HEX(pSrc->second);
            }
            break;
            // date change sequence
        case RTC_SEQ_SET_DATE:
        case RTC_SEQ_SET_TIME:
        case RTC_SEQ_SET_DATETIME:
            // no special processing
            break;
            // alarm 1 status obtain sequence
        case RTC_SEQ_GET_ALARM1_STATUS:
            {
                RTCAlarmStatus *pDst = (RTCAlarmStatus *)(rtcWork.buffer[0]);
                RTCRawStatus2 *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2);

                switch (pSrc->intr_mode)
                {
                case RTC_INTERRUPT_MODE_ALARM:
                    *pDst = RTC_ALARM_STATUS_ON;
                    break;
                default:
                    *pDst = RTC_ALARM_STATUS_OFF;
                }
            }
            break;
            // alarm 2 status obtain sequence
        case RTC_SEQ_GET_ALARM2_STATUS:
            {
                RTCAlarmStatus *pDst = (RTCAlarmStatus *)(rtcWork.buffer[0]);
                RTCRawStatus2 *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2);

                if (pSrc->intr2_mode)
                {
                    *pDst = RTC_ALARM_STATUS_ON;
                }
                else
                {
                    *pDst = RTC_ALARM_STATUS_OFF;
                }
            }
            break;
            // alarm 1 or 2 setup value obtain sequence
        case RTC_SEQ_GET_ALARM_PARAM:
            {
                RTCAlarmParam *pDst = (RTCAlarmParam *)(rtcWork.buffer[0]);
                RTCRawAlarm *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.alarm);

                pDst->week = (RTCWeek)(pSrc->week);
                pDst->hour = RtcBCD2HEX(pSrc->hour);
                pDst->minute = RtcBCD2HEX(pSrc->minute);
                pDst->enable = RTC_ALARM_ENABLE_NONE;
                if (pSrc->we)
                    pDst->enable += RTC_ALARM_ENABLE_WEEK;
                if (pSrc->he)
                    pDst->enable += RTC_ALARM_ENABLE_HOUR;
                if (pSrc->me)
                    pDst->enable += RTC_ALARM_ENABLE_MINUTE;
            }
            break;
            // alarm 1 status setup sequence
        case RTC_SEQ_SET_ALARM1_STATUS:
            if (rtcWork.index == 0)
            {
                RTCRawStatus2 *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2);

                // status 2 register read result
                if (*(RTCAlarmStatus *)(rtcWork.buffer[0]) == RTC_ALARM_STATUS_ON)
                {
                    // If interrupts permitted
                    if (pSrc->intr_mode != RTC_INTERRUPT_MODE_ALARM)
                    {
                        // Write to the Status 2 register
                        rtcWork.index++;        // Go to next sequence    
                        pSrc->intr_mode = RTC_INTERRUPT_MODE_ALARM;
                        if (!RTCi_WriteRawStatus2Async())
                        {
                            rtcWork.index = 0;  // halt sequence    
                            result = RTC_RESULT_SEND_ERROR;
                        }
                    }
                }
                else
                {
                    // If interrupts prohibited
                    if (pSrc->intr_mode != RTC_INTERRUPT_MODE_NONE)
                    {
                        // Write to the Status 2 register
                        rtcWork.index++;        // Go to next sequence    
                        pSrc->intr_mode = RTC_INTERRUPT_MODE_NONE;
                        if (!RTCi_WriteRawStatus2Async())
                        {
                            rtcWork.index = 0;  // halt sequence    
                            result = RTC_RESULT_SEND_ERROR;
                        }
                    }
                }
            }
            else
            {
                // status 2 register write result
                rtcWork.index = 0;     // sequence end
            }
            break;
            // alarm 2 status setup sequence
        case RTC_SEQ_SET_ALARM2_STATUS:
            if (rtcWork.index == 0)
            {
                RTCRawStatus2 *pSrc =
                    &(((RTCRawData *)(OS_GetSystemWork()->real_time_clock))->a.status2);

                // status 2 register read result
                if (*(RTCAlarmStatus *)(rtcWork.buffer[0]) == RTC_ALARM_STATUS_ON)
                {
                    // If interrupts permitted
                    if (!pSrc->intr2_mode)
                    {
                        // Write to the Status 2 register
                        rtcWork.index++;        // Go to next sequence    
                        pSrc->intr2_mode = 1;
                        if (!RTCi_WriteRawStatus2Async())
                        {
                            rtcWork.index = 0;  // halt sequence    
                            result = RTC_RESULT_SEND_ERROR;
                        }
                    }
                }
                else
                {
                    // If interrupts prohibited
                    if (pSrc->intr2_mode)
                    {
                        // Write to the Status 2 register
                        rtcWork.index++;        // Go to next sequence    
                        pSrc->intr2_mode = 0;
                        if (!RTCi_WriteRawStatus2Async())
                        {
                            rtcWork.index = 0;  // halt sequence    
                            result = RTC_RESULT_SEND_ERROR;
                        }
                    }
                }
            }
            else
            {
                // status 2 register write result
                rtcWork.index = 0;     // sequence end
            }
            break;
            // alarm 1 parameters setup sequence
        case RTC_SEQ_SET_ALARM1_PARAM:
            // alarm 2 parameters setup sequence
        case RTC_SEQ_SET_ALARM2_PARAM:
            // time notation change sequence
        case RTC_SEQ_SET_HOUR_FORMAT:
            // status 2 register write sequence
        case RTC_SEQ_SET_REG_STATUS2:
            // adjust register write sequence
        case RTC_SEQ_SET_REG_ADJUST:
            // no special processing
            break;

            // other mystery sequences
        default:
            result = RTC_RESULT_INVALID_COMMAND;
            rtcWork.index = 0;
        }
    }
    else
    {
        // failure response is sent to the command so the sequence halts    
        rtcWork.index = 0;
        // determine process result from PXI communication response classification
        switch (pxiresult)
        {
        case RTC_PXI_RESULT_INVALID_COMMAND:
            result = RTC_RESULT_INVALID_COMMAND;
            break;
        case RTC_PXI_RESULT_ILLEGAL_STATUS:
            result = RTC_RESULT_ILLEGAL_STATUS;
            break;
        case RTC_PXI_RESULT_BUSY:
            result = RTC_RESULT_BUSY;
            break;
        case RTC_PXI_RESULT_FATAL_ERROR:
        default:
            result = RTC_RESULT_FATAL_ERROR;
        }
    }

    // sequence termination process if consecutive sequences complete
    if (rtcWork.index == 0)
    {
        // exclusive lock open
        if (rtcWork.lock != RTC_LOCK_OFF)
        {
            rtcWork.lock = RTC_LOCK_OFF;
        }
        // call callback function
        if (rtcWork.callback)
        {
            cb = rtcWork.callback;
            rtcWork.callback = NULL;
            cb(result, rtcWork.callbackArg);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         RtcBCD2HEX

  Description:  Convert numeral expressed with BCD type to a numeral expressed with a general u32.

  Arguments:    bcd  - numeral expressed with BCD type

  Returns:      u32  - numeral expressed with a general u32
                       Return 0 if input parameter cannot be expressed with BCD    
 *---------------------------------------------------------------------------*/
static u32 RtcBCD2HEX(u32 bcd)
{
    u32     hex = 0;
    s32     i;
    s32     w;

    // Verify that 0xA ~ 0xF is not included in any digits
    for (i = 0; i < 8; i++)
    {
        if (((bcd >> (i * 4)) & 0x0000000f) >= 0x0a)
        {
            return hex;                // Stop conversion and return 0 forcibly
        }
    }

    // conversion loop
    for (i = 0, w = 1; i < 8; i++, w *= 10)
    {
        hex += (((bcd >> (i * 4)) & 0x0000000f) * w);
    }
    return hex;
}

/*---------------------------------------------------------------------------*
  Name:         RtcHEX2BCD

  Description:  Convert numeral expressed with a general u32 to a numeral expressed with BCD type.

  Arguments:    hex  - numeral expressed with a general u32

  Returns:      u32  - numeral expressed with BCD type
                       Return 0 if input parameter cannot be expressed with BCD
 *---------------------------------------------------------------------------*/
static u32 RtcHEX2BCD(u32 hex)
{
    u32     bcd = 0;
    s32     i;
    u32     w;

    // Make sure not over 99999999
    if (hex > 99999999)
    {
        return 0;
    }

    // conversion loop
    for (i = 0, w = hex; i < 8; i++)
    {
        bcd += ((w % 10) << (i * 4));
        w /= 10;
    }
    return bcd;
}

/*---------------------------------------------------------------------------*
  Name:         RtcCheckAlarmParam

  Description:  The alarm setup value is set in RTC and this checks if there is

  Arguments:    param  - alarm setup value checked

  Returns:      BOOL   - If no problem as alarm setup value return TRUE.
                         If a problem return FALSE.
 *---------------------------------------------------------------------------*/
static BOOL RtcCheckAlarmParam(const RTCAlarmParam *param)
{
    if (param->week >= RTC_WEEK_MAX)
        return FALSE;
    if (param->hour >= 24)
        return FALSE;
    if (param->minute >= 60)
        return FALSE;
    if (param->enable & ~RTC_ALARM_ENABLE_ALL)
        return FALSE;
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         RtcMakeAlarmParam

  Description:  Convert to format so alarm setup value can be set in RTC

  Arguments:    param  - converted alarm setup value

  Returns:      RTCRawAlarm - data converted to format can set in RTC
 *---------------------------------------------------------------------------*/
static RTCRawAlarm RtcMakeAlarmParam(const RTCAlarmParam *param)
{
    RTCRawAlarm dst;

    // 0 clear return value
    *((u32 *)(&dst)) = 0;

    // Check setup value consistency to make sure
    if (!RtcCheckAlarmParam(param))
    {
        return dst;
    }

    // day data
    dst.week = (u32)(param->week);
    // time data, and am-pm flag
    if (param->hour >= 12)
    {
        dst.afternoon = 1;
    }
    dst.hour = RtcHEX2BCD(param->hour);
    // minute data
    dst.minute = RtcHEX2BCD(param->minute);
    // enable flag
    if (param->enable & RTC_ALARM_ENABLE_WEEK)
    {
        dst.we = 1;
    }
    if (param->enable & RTC_ALARM_ENABLE_HOUR)
    {
        dst.he = 1;
    }
    if (param->enable & RTC_ALARM_ENABLE_MINUTE)
    {
        dst.me = 1;
    }

    return dst;
}

/*---------------------------------------------------------------------------*
  Name:         RtcCheckDate

  Description:  The date is set in RTC and this checks if there is a problem with the value.    
                If not, edits the format to one that can be set in RTC.

  Arguments:    date - input date to check
                raw  - output data edited to format that can set in RTC

  Returns:      BOOL - Check and return FALSE if problem.
 *---------------------------------------------------------------------------*/
static BOOL RtcCheckDate(const RTCDate *date, RTCRawDate *raw)
{
    // Check if each member is in allowed area
    if (date->year >= 100)
        return FALSE;
    if ((date->month < 1) || (date->month > 12))
        return FALSE;
    if ((date->day < 1) || (date->day > 31))
        return FALSE;
    if (date->week >= RTC_WEEK_MAX)
        return FALSE;

    // edit to raw data type
    //raw->year  = RtcHEX2BCD( date->year );    // Change to the following code because the value is not stored for some reason
    *(u32 *)raw = RtcHEX2BCD(date->year);
    raw->month = RtcHEX2BCD(date->month);
    raw->day = RtcHEX2BCD(date->day);
    raw->week = (u32)(date->week);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         RtcCheckTime

  Description:  The time is set in RTC and this checks if there is a problem with the value.    
                If not, edits the format to one that can be set in RTC.

  Arguments:    date - input time to check
                raw  - output data edited to format that can set in RTC

  Returns:      BOOL - Check and return FALSE if problem.
 *---------------------------------------------------------------------------*/
static BOOL RtcCheckTime(const RTCTime *time, RTCRawTime *raw)
{
    // Check if each member is in allowed area
    if (time->hour >= 24)
        return FALSE;
    if (time->minute >= 60)
        return FALSE;
    if (time->second >= 60)
        return FALSE;

    // edit to raw data type
    if (time->hour >= 12)
    {
        raw->afternoon = 1;
    }
    else
    {
        raw->afternoon = 0;
    }
    raw->hour = RtcHEX2BCD(time->hour);
    raw->minute = RtcHEX2BCD(time->minute);
    raw->second = RtcHEX2BCD(time->second);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         RtcGetResultCallback

  Description:  Called when asynchronized processing completes and updates internal functions processing results.

  Arguments:    result  -   The processing results from async function.
                arg     -   Not used.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void RtcGetResultCallback(RTCResult result, void *arg)
{
#pragma unused( arg )

    rtcWork.commonResult = result;
}

#include    <nitro/code32.h>
/*---------------------------------------------------------------------------*
  Name:         RtcWaitBusy

  Description:  Wait while RTC asynchronized processing is locked.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static asm void
RtcWaitBusy( void )
{
    ldr     r12,    =rtcWork.lock
loop:
    ldr     r0,     [ r12,  #0 ]
    cmp     r0,     #RTC_LOCK_ON
    beq     loop
    bx      lr
}
#include    <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
