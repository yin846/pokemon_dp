/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - libraries
  File:     internal.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: internal.c,v $
  Revision 1.2  2005/02/28 05:26:08  yosizaki
  do-indent.

  Revision 1.1  2004/05/12 02:36:08  terui Improvement:
  initial upload

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include	<nitro/rtc/ARM9/api.h>
#include	<nitro/pxi.h>


/*---------------------------------------------------------------------------*
	Internal function definitions
 *---------------------------------------------------------------------------*/
static BOOL RtcSendPxiCommand(u32 command);


/*---------------------------------------------------------------------------*
  Name:         RTCi_ResetAsync

  Description:  Do asynchronized reset of RTC
                The notification for response from ARM7 is done by PXI callback (tag:RTC).

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ResetAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_RESET);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_SetHourFormatAsync

  Description:  Do asynchronized change of time format.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Sets argument with status1 in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_SetHourFormatAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_SET_HOUR_FORMAT);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawDateTimeAsync

  Description:  Do asynchronized obtain of nonprocessed date/time data.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawDateTimeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_DATETIME);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawDateTimeAsync

  Description:  Do asynchronized write to device of nonprocessed date/time data.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawDateTimeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_DATETIME);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawDateAsync

  Description:  Do asynchronized obtain of nonprocessed date data.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawDateAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_DATE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawDateAsync

  Description:  Do asynchronized write to device of nonprocessed date data.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawDateAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_DATE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawTimeAsync

  Description:  Do asynchronized obtain of nonprocessed time data.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawTimeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_TIME);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawTimeAsync

  Description:  Do asynchronized write to device of nonprocessed time data.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawTimeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_TIME);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawPulseAsync

  Description:  Do asynchronized obtain of non-processed frequency steady interrupt setup values.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawPulseAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_PULSE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawPulseAsync

  Description:  Do asynchronized write of non-processed frequency steady interrupt setup values to device.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawPulseAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_PULSE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawAlarm1Async

  Description:  Do asynchronized obtain of non-processed alarm 1 interrupt setup values.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawAlarm1Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_ALARM1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawAlarm1Async

  Description:  Do asynchronized write of non-processed alarm 1 interrupt setup values to device.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawAlarm1Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_ALARM1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawAlarm2Async

  Description:  Do asynchronized obtain of non-processed alarm 2 interrupt setup  values.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawAlarm2Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_ALARM2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawAlarm2Async

  Description:  Do asynchronized write of non-processed alarm 2 interrupt setup  values to device.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawAlarm2Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_ALARM2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawStatus1Async

  Description:  Do asynchronized obtain of non-processed status 1 register setup values.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawStatus1Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_STATUS1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawStatus1Async

  Description:  Do asynchronized write of non-processed status 1 register setup values to device.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawStatus1Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_STATUS1);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawStatus2Async

  Description:  Do asynchronized obtain of non-processed status 2 register setup  values.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawStatus2Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_STATUS2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawStatus2Async

  Description:  Do asynchronized write of non-processed status 2 register setup values to device.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawStatus2Async(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_STATUS2);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawAdjustAsync

  Description:  Do asynchronized obtain of non-processed clock adjustment  register setup values.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawAdjustAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_ADJUST);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawAdjustAsync

  Description:  Do asynchronized write of non-processed clock adjustment  register setup values to device.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawAdjustAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_ADJUST);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ReadRawFreeAsync

  Description:  Do asynchronized obtain of non-processed free register setup  values.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                Data will be stored in OS_GetSystemWork()->real_time_clock.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_ReadRawFreeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_READ_FREE);
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_WriteRawFreeAsync

  Description:  Do asynchronized write of non-processed free register setup  values to device.
                The notification for response from ARM7 is done by PXI callback (tag:RTC).
                The data in OS_GetSystemWork()->real_time_clock will be written

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
BOOL RTCi_WriteRawFreeAsync(void)
{
    return RtcSendPxiCommand(RTC_PXI_COMMAND_WRITE_FREE);
}

/*---------------------------------------------------------------------------*
  Name:         RtcSendPxiCommand

  Description:  Send specified command to ARM7 via PXI.

  Arguments:    None.

  Returns:      BOOL     - Return TRUE if send for PXI completed.
                           Return FALSE if send by PXI fails.
 *---------------------------------------------------------------------------*/
static BOOL RtcSendPxiCommand(u32 command)
{
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_RTC,
                               ((command << RTC_PXI_COMMAND_SHIFT) & RTC_PXI_COMMAND_MASK), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/

