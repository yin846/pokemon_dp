/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_tick.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_tick.c,v $
  Revision 1.15  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.14  06/08/2005 00:53:21  yada
  add assert (check if the library is initialized)

  Revision 1.13  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.12  10/21/2004 05:48:34  yada
  fix OS_SetTick()

  Revision 1.11  09/02/2004 06:21:18  yada
  only change comment

  Revision 1.10  08/20/2004 10:48:40  yada
  fix OS_GetTick*(). avoid optimization

  Revision 1.9  07/10/2004 07:29:00  yada
  add OSi_GetTick_noProfile

  Revision 1.8  06/03/2004 04:30:37  yada
  change ifdef about OSi_GetTickLo_noProfile

  Revision 1.7  05/24/2004 04:50:24  yada
  fix profile pragma

  Revision 1.6  05/12/2004 05:10:42  yada
  SDK_PROFILE -> OS_PROFILE_AVAILABLE

  Revision 1.5  05/10/2004 12:14:23  yada
  add prototype OSi_GetTickLo_noProfile()

  Revision 1.4  05/10/2004 12:02:23  yada
  add getTick function for PROFILE

  Revision 1.3  03/02/2004 09:04:00  yada
  Fixed count up routine of tick

  Revision 1.2  03/01/2004 12:05:00  yada
  Creation of function comments in English.

  Revision 1.1  02/25/2004 11:38:17  yada
  Changes related to switch from systemClock to Tick

  Revision 1.8  02/24/2004 10:49:49  yada
  Added OS_GetTick()

  Revision 1.7  02/23/2004 07:36:47  yada
  System clock becomes 64-bit value

  Revision 1.6  02/20/2004 04:40:33  yada
  Fixed problems with timer setting

  Revision 1.5  02/12/2004 09:57:58  yada
  Changed to something that considers interrupting GetTime, SetTime

  Revision 1.4  2004/02/05 07:09:02  yasu
  change SDK prefix iris -> nitro

  Revision 1.3  02/04/2004 07:36:43  yada
  Added OS_IsSystemClockAvailable()
  Changed names from OS_GetSystemClock(), OS_SetSystemClock()
  Changed names to OS_GetTime(), OS_SetTime()

  Revision 1.2  02/03/2004 11:22:12  yada
  Changed the systemClock value from u32 to OSTime(u64).

  Revision 1.1  02/02/2004 05:29:18  yada
  System clock firstRelease

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>

//----------------------------------------------------------------------
//---- timer control setting for tick
#define OSi_TICK_TIMERCONTROL  ( REG_OS_TM0CNT_H_E_MASK | REG_OS_TM0CNT_H_I_MASK | OS_TIMER_PRESCALER_64 )

//---- timer number tick uses
#define OSi_TICK_TIMER         OS_TIMER_0

//---- timer interrupt mask (must be same number with OSi_TICK_TIMER)
#define OSi_TICK_IE_TIMER      OS_IE_TIMER0

//---- flag for initialization tick
static u16 OSi_UseTick = FALSE;

//---- tick counter
vu64    OSi_TickCounter;

//---- flag for need to re-set timer
BOOL    OSi_NeedResetTimer = FALSE;


extern u16 OSi_IsTimerReserved(int timerNum);
extern void OSi_SetTimerReserved(int timerNum);

static void OSi_CountUpTick(void);
u32     OSi_GetTick_noProfile(void);


/*---------------------------------------------------------------------------*
  Name:         OS_InitTick

  Description:  initialize 'tick system'

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_InitTick(void)
{
    if (!OSi_UseTick)
    {
        OSi_UseTick = TRUE;

        //---- OS reserves OSi_TICK_TIMER timer
        SDK_ASSERT(!OSi_IsTimerReserved(OSi_TICK_TIMER));
        OSi_SetTimerReserved(OSi_TICK_TIMER);

        //---- setting timer
        OSi_TickCounter = 0;
        OS_SetTimerControl(OSi_TICK_TIMER, 0);
        OS_SetTimerCount((OSTimer)OSi_TICK_TIMER, (u16)0);
        OS_SetTimerControl(OSi_TICK_TIMER, (u16)OSi_TICK_TIMERCONTROL);

        //---- set interrupt callback
        OS_SetIrqFunction(OSi_TICK_IE_TIMER, OSi_CountUpTick);

        //---- enable timer interrupt
        (void)OS_EnableIrqMask(OSi_TICK_IE_TIMER);

        //---- need to reset
        OSi_NeedResetTimer = FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OS_IsTickAvailable

  Description:  check tick system is available

  Arguments:    None

  Returns:      if available, TRUE.
 *---------------------------------------------------------------------------*/
BOOL OS_IsTickAvailable(void)
{
    return OSi_UseTick;
}


/*---------------------------------------------------------------------------*
  Name:         OSi_CountUpTick

  Description:  timer interrupt handle.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
static void OSi_CountUpTick(void)
{
    OSi_TickCounter++;

    //---- setting for timer
    if (OSi_NeedResetTimer)
    {
        OS_SetTimerControl(OSi_TICK_TIMER, 0);
        OS_SetTimerCount((OSTimer)OSi_TICK_TIMER, (u16)0);
        OS_SetTimerControl(OSi_TICK_TIMER, (u16)OSi_TICK_TIMERCONTROL);

        OSi_NeedResetTimer = FALSE;
    }

    //---- reset callback
    OSi_EnterTimerCallback(OSi_TICK_TIMER, (void (*)(void *))OSi_CountUpTick, 0);
}

/*---------------------------------------------------------------------------*
  Name:         OS_GetTick

  Description:  get tick value

  Arguments:    None

  Returns:      tick value
 *---------------------------------------------------------------------------*/
OSTick OS_GetTick(void)
{
    vu16    countL;
    vu64    countH;

    OSIntrMode prev = OS_DisableInterrupts();
    SDK_ASSERT(OSi_UseTick);

    countL = *(REGType16 *)((u32)REG_TM0CNT_L_ADDR + OSi_TICK_TIMER * 4);
    countH = OSi_TickCounter & 0xffffffffffffULL;

    //---- check if timer interrupt bit is on
    if (reg_OS_IF & OSi_TICK_IE_TIMER && !(countL & 0x8000))
    {
        countH++;
    }

    (void)OS_RestoreInterrupts(prev);

    return (countH << 16) | countL;
}

//---- for profiler
#pragma profile off
u32 OSi_GetTick_noProfile(void)
{
    vu16    countL;
    vu16    countH;

    OSIntrMode prev = OS_DisableInterrupts();
    SDK_ASSERT(OSi_UseTick);

    countL = *(REGType16 *)((u32)REG_TM0CNT_L_ADDR + OSi_TICK_TIMER * 4);
    countH = (u16)OSi_TickCounter;

    //---- check if timer interrupt bit is on
    if (reg_OS_IF & OSi_TICK_IE_TIMER && !(countL & 0x8000))
    {
        countH++;
    }

    (void)OS_RestoreInterrupts(prev);

    return (u32)(countH << 16) | countL;
}

#pragma profile reset

/*---------------------------------------------------------------------------*
  Name:         OS_GetTickLo

  Description:  get tick value (only u16 part)

  Arguments:    None

  Returns:      tick value (only u16 part)
 *---------------------------------------------------------------------------*/
u16 OS_GetTickLo(void)
{
    SDK_ASSERT(OSi_UseTick);
    return *(REGType16 *)((u32)REG_TM0CNT_L_ADDR + OSi_TICK_TIMER * 4);
}

/*---------------------------------------------------------------------------*
  Name:         OS_SetTick

  Description:  set tick value

  Arguments:    count     value of tick to be set

  Returns:      None
 *---------------------------------------------------------------------------*/
void OS_SetTick(OSTick count)
{
    OSIntrMode prev;

    SDK_ASSERT(OSi_UseTick);
    prev = OS_DisableInterrupts();

    reg_OS_IF = OSi_TICK_IE_TIMER;

    OSi_NeedResetTimer = TRUE;

    OSi_TickCounter = (u64)(count >> 16);

    OS_SetTimerControl(OSi_TICK_TIMER, 0);
    OS_SetTimerCount((OSTimer)OSi_TICK_TIMER, (u16)(count & 0xffff));
    OS_SetTimerControl(OSi_TICK_TIMER, (u16)OSi_TICK_TIMERCONTROL);

    (void)OS_RestoreInterrupts(prev);
}
