/*---------------------------------------------------------------------------*
  Project:  NitroSDK - RTC - libraries
  File:     convert.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: convert.c,v $
  Revision 1.6  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.5  10/04/2005 00:29:01  yada
  suppressed warning

  Revision 1.4  09/30/2005 00:26:08  yada
  add  RTC_GetDayOfWeek()

  Revision 1.3  05/18/2005 02:01:37  seiki_masashi
  Changed RTC_ConvertDateToDay and DayToDate to public functions

  Revision 1.2  05/18/2005 00:40:07  seiki_masashi
  Support for warning.

  Revision 1.1  05/17/2005 12:53:54  seiki_masashi
  Added conversion function

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/types.h>
#include    <nitro/rtc.h>


/*---------------------------------------------------------------------------*
    Static Variable Definitions
 *---------------------------------------------------------------------------*/
static s32 sDayOfYear[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };


/*---------------------------------------------------------------------------*
    Internal function declarations
 *---------------------------------------------------------------------------*/

// Simple leap year determination (If leap year, TRUE)
// Note: Range of years acceptable to RTC is limited to 2000-2099.
static inline BOOL RTCi_IsLeapYear(u32 year)
{
    // Leap year is a year divisible by four, and not divisible by 100, or a year divisible by 400
    return !((year & 0x03));
}

/*---------------------------------------------------------------------------*
    function definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateToDay

  Description:  Converts date data using RTCDate type to the total number of days since the year 2000

  Arguments:    date      - Pointer to date data.

  Returns:      The total number of days since Jan. 1 2000
 *---------------------------------------------------------------------------*/
s32 RTC_ConvertDateToDay(const RTCDate *date)
{
    s32     dayNum;

    if (date->year >= 100
        || (date->month < 1) || (date->month > 12)
        || (date->day < 1) || (date->day > 31)
        || (date->week >= RTC_WEEK_MAX) || (date->month < 1) || (date->month > 12))
    {
        return -1;
    }

    // Convert the month and day to number of days
    dayNum = (s32)(date->day - 1);
    dayNum += sDayOfYear[date->month - 1];
    if ((date->month >= 3) && RTCi_IsLeapYear(date->year))
    {
        // In leap year, increment days by one starting in March
        dayNum++;
    }

    // Convert year to number of days
    dayNum += date->year * 365;
    // Calculate the number of leap years up to the specified year -1, and add that number of days
    // Because date->year is only up to 0-99, simple version of leap year calculation
    dayNum += (date->year + 3) / 4;

    return dayNum;
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ConvertTimeToSecond

  Description:  Converts time data in RTCTime type to the total number of seconds since 0:00

  Arguments:    time      - Pointer to the time data.

  Returns:      Total number of seconds since 0:00.
 *---------------------------------------------------------------------------*/
s32 RTCi_ConvertTimeToSecond(const RTCTime *time)
{
    return (s32)((time->hour * 60 + time->minute) * 60 + time->second);
}

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateTimeToSecond

  Description:  Converts date and time data in RTCDate and RTCTime types to total number of seconds.

  Arguments:    date      - Pointer to date data.
                time      - Pointer to the time data.

  Returns:      Total number of seconds since 0:00, Jan. 1, 2000, local time.
 *---------------------------------------------------------------------------*/
s64 RTC_ConvertDateTimeToSecond(const RTCDate *date, const RTCTime *time)
{
    s32     day, sec;
    day = RTC_ConvertDateToDay(date);
    if (day == -1)
    {
        return -1;
    }
    sec = RTCi_ConvertTimeToSecond(time);
    if (sec == -1)
    {
        return -1;
    }
    return ((s64)day) * (60 * 60 * 24) + sec;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDayToDate

  Description:  Converts the total number of days since the year 2000 to date data in RTCDate type

  Arguments:    date      - Pointer to save destination date data
                day       - The total number of days since Jan 1, 2000.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTC_ConvertDayToDate(RTCDate *date, s32 day)
{
    u32     year;
    s32     month;

    if (day < 0)
    {
        day = 0;
    }
    if (day > 36524)
    {
        day = 36524;
    }

    // Jan 1, 2000 was a Saturday
    date->week = (RTCWeek)((day + 6) % 7);

    for (year = 0; year < 99; year++)
    {
        s32     prev = day;
        day -= (RTCi_IsLeapYear(year)) ? 366 : 365;
        if (day < 0)
        {
            day = prev;
            break;
        }
    }
    if (day > 365)
    {
        day = 365;
    }

    date->year = year;

    if (RTCi_IsLeapYear(year))
    {
        if (day < 31 + 29)
        {
            if (day < 31)
            {
                month = 1;
            }
            else
            {
                month = 2;
                day -= 31;
            }
            date->month = (u32)month;
            date->day = (u32)(day + 1);
            return;
        }
        else
        {
            day--;
        }
    }

    for (month = 11; month >= 0; month--)
    {
        if (day >= sDayOfYear[month])
        {
            date->month = (u32)(month + 1);
            date->day = (u32)(day - sDayOfYear[month] + 1);
            return;
        }
    }

    SDK_ASSERT("Internal Error.");
}

/*---------------------------------------------------------------------------*
  Name:         RTCi_ConvertSecondToTime

  Description:  Converts the total number of seconds since 0:00 to time data in RTCTime type

  Arguments:    time      - Pointer to save destination date data
                sec       -Total number of seconds since 0:00.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTCi_ConvertSecondToTime(RTCTime *time, s32 sec)
{
    if (sec < 0)
    {
        sec = 0;
    }
    if (sec > 86399)
    {
        sec = 86399;
    }

    time->second = (u32)(sec % 60);
    sec /= 60;
    time->minute = (u32)(sec % 60);
    sec /= 60;
    time->hour = (u32)sec;
}

/*---------------------------------------------------------------------------*
  Name:         RTC_ConvertDateTimeToSecond

  Description:  Converts total number of seconds to date and time data in RTCDate and RTCTime types.

  Arguments:    date      - Pointer to save destination date data
                time      - Pointer to the save destination time data.
                sec       - Total number of seconds since 0:00 Jan 1, 2000.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void RTC_ConvertSecondToDateTime(RTCDate *date, RTCTime *time, s64 sec)
{
    if (sec < 0)
    {
        sec = 0;
    }
    else if (sec > 3155759999)
    {
        sec = 3155759999;
    }
    RTCi_ConvertSecondToTime(time, (s32)(sec % 86400));
    RTC_ConvertDayToDate(date, (s32)(sec / 86400));
}

/*---------------------------------------------------------------------------*
  Name:         RTC_GetDayOfWeek

  Description:  Returns the day of the week based on RTCDate type data.
                (This is found based on a calculation rather than using the day of the week inside RTCDate.)

  Arguments:    date      - Pointer to date data.

  Returns:      Day of week      RTC_WEEK_xxxx
 *---------------------------------------------------------------------------*/
RTCWeek RTC_GetDayOfWeek(RTCDate *date)
{
    int     cent;
    int     year = (int)(2000 + date->year);
    int     month = (int)date->month;
    int     day = (int)date->day;

    month -= 2;
    if (month < 1)
    {
        month += 12;
        --year;
    }

    cent = year / 100;
    year %= 100;
    return (RTCWeek)(((26 * month - 2) / 10 + day + year + year / 4 + cent / 4 + 5 * cent) % 7);
    // Sunday, Monday, ...Saturday are defined as 0 through 6 for RTC_WEEK_xxxx
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
