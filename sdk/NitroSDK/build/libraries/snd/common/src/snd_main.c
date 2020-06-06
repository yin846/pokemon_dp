/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - libraries
  File:     snd_main.c

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: snd_main.c,v $
  Revision 1.2  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.1  04/12/2005 06:16:12  ida
  Moved SND to NITRO-SDK

  Revision 1.4  02/14/2005 08:29:42  ida
  Added double initialization check

  Revision 1.3  02/02/2005 05:31:57  ida
  Updated copyright year notation

  Revision 1.2  02/02/2005 05:09:21  ida
  Changed so mutex lock doesn't occur in ARM7.

  Revision 1.1  11/22/2004 04:30:00  ida
  ARM7/src -> common/src

  Revision 1.17  10/18/2004 01:28:22  ida
  remove debug code

  Revision 1.16  10/04/2004 09:00:20  ida
  add SND_SetThreadPriority

  Revision 1.15  08/19/2004 06:44:17  ida
  for midi

  Revision 1.14  08/02/2004 08:30:01  ida
  fix wrong #include

  Revision 1.13  08/02/2004 05:30:22  ida
  workaround channel status

  Revision 1.12  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.11  06/28/2004 07:13:55  ida
  add SND_AlarmInit

  Revision 1.10  06/23/2004 06:24:23  ida
  channel -> exchannel

  Revision 1.9  06/21/2004 01:14:04  ida
  for new alarm and capture interface

  Revision 1.8  06/11/2004 01:52:24  ida
  port -> command

  Revision 1.7  05/11/2004 08:07:54  ida
  fix wrong comment

  Revision 1.6  05/07/2004 07:34:28  ida
  update random seed

  Revision 1.5  05/06/2004 01:40:47  ida
  use doPeriodicProc flag

  Revision 1.4  04/28/2004 01:26:17  ida
  add comments

  Revision 1.3  04/26/2004 02:56:10  ida
  for SND_RequestSpPortProc

  Revision 1.2  04/05/2004 02:55:08  ida
  unuse OS_Printf

  Revision 1.1  03/26/2004 06:25:52  ida
  create snd_drv

  Revision 1.5  03/22/2004 02:07:53  ida
  add alarm functions

  Revision 1.3  03/18/2004 13:01:26  yada
  Corrected include ... to go through make.

  Revision 1.2  03/10/2004 06:31:24  ida
  SND_Snd -> SND_

  Revision 1.1  03/08/2004 05:40:27  ida
  rename files

  Revision 1.4  03/05/2004 06:31:57  ida
  using thread

  Revision 1.3  02/24/2004 08:08:10  ida
  workaround recv port

  Revision 1.2  02/19/2004 07:19:44  ida
  NNS_ -> SND_

  Revision 1.1  02/19/2004 07:07:52  ida
  add NITRO System snd libraries

  Revision 1.3  02/19/2004 01:44:25  ida
  add comments

  Revision 1.2  02/16/2004 06:22:07  ida
  (none)

  Revision 1.1  02/13/2004 01:42:05  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/snd/common/main.h>

#include <nitro/os.h>
#include <nitro/snd/common/global.h>
#include <nitro/snd/common/exchannel.h>
#include <nitro/snd/common/seq.h>
#include <nitro/snd/common/alarm.h>
#include <nitro/snd/common/command.h>
#include <nitro/snd/common/util.h>
#include <nitro/snd/common/work.h>

/******************************************************************************
	macro definition
 ******************************************************************************/

#ifdef SDK_ARM7

#define SND_THREAD_STACK_SIZE      1024
#define SND_THREAD_MESSAGE_BUFSIZE 8

#define SND_ALARM_COUNT_P1  0x10000

#endif /* SDK_ARM7*/

/******************************************************************************
	static variables
 ******************************************************************************/

#ifdef SDK_ARM9

static OSMutex sSndMutex;

#else  /* SDK_ARM7*/

static OSThread sndThread;
static u64 sndStack[SND_THREAD_STACK_SIZE / sizeof(u64)];
static OSAlarm sndAlarm;
static OSMessageQueue sndMesgQueue;
static OSMessage sndMesgBuffer[SND_THREAD_MESSAGE_BUFSIZE];

#endif /* SDK_ARM9*/


/******************************************************************************
	static function declarations
 ******************************************************************************/

#ifdef SDK_ARM7

static void SndThread(void *arg);
static void SndAlarmCallback(void *arg);

#endif /* SDK_ARM7*/

/******************************************************************************
	public functions
 ******************************************************************************/

#ifdef SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         SND_Init

  Description:  initialize sound library

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_Init(void)
{
    {
        static BOOL initialized = FALSE;
        if (initialized)
            return;
        initialized = TRUE;
    }

    OS_InitMutex(&sSndMutex);
    SND_CommandInit();
    SND_AlarmInit();
}

#else  /* SDK_ARM7*/

/*---------------------------------------------------------------------------*
  Name:         SND_Init

  Description:  Initializes sound and runs the sound thread.

  Arguments:    threadPrio - thread priority

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_Init(u32 threadPrio)
{
    {
        static BOOL initialized = FALSE;
        if (initialized)
            return;
        initialized = TRUE;
    }

    SND_CommandInit();

    SND_CreateThread(threadPrio);
}

/*---------------------------------------------------------------------------*
  Name:         SND_CreateThread

  Description:  Runs the sound thread

  Arguments:    threadPrio - thread priority

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_CreateThread(u32 threadPrio)
{
    OS_CreateThread(&sndThread,
                    SndThread,
                    NULL,
                    sndStack + SND_THREAD_STACK_SIZE / sizeof(u64),
                    SND_THREAD_STACK_SIZE, threadPrio);
    OS_WakeupThreadDirect(&sndThread);
}

/*---------------------------------------------------------------------------*
  Name:         SND_SetThreadPriority

  Description:  Configures the priority of the sound thread.

  Arguments:    prio - Thread priority.

  Returns:      Whether it was successful or not
 *---------------------------------------------------------------------------*/
BOOL SND_SetThreadPriority(u32 prio)
{
    return OS_SetThreadPriority(&sndThread, prio);
}

/*---------------------------------------------------------------------------*
  Name:         SND_InitIntervalTimer

  Description:  Initializes the interval timer.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_InitIntervalTimer(void)
{
    OS_InitMessageQueue(&sndMesgQueue, sndMesgBuffer, SND_THREAD_MESSAGE_BUFSIZE);

    OS_CreateAlarm(&sndAlarm);
}

/*---------------------------------------------------------------------------*
  Name:         SND_StartIntervalTimer

  Description:  Runs the interval timer.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_StartIntervalTimer(void)
{
    OS_SetPeriodicAlarm(&sndAlarm,
                        OS_GetTick() + SND_ALARM_COUNT_P1,
                        SND_PROC_INTERVAL, &SndAlarmCallback, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         SND_StopIntervalTimer

  Description:  Stops the interval timer.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SND_StopIntervalTimer(void)
{
    OS_CancelAlarm(&sndAlarm);
}

/*---------------------------------------------------------------------------*
  Name:         SND_WaitForIntervalTimer

  Description:  Waiting for the interval timer.

  Arguments:    None.

  Returns:      Returns a message.
 *---------------------------------------------------------------------------*/
OSMessage SND_WaitForIntervalTimer(void)
{
    OSMessage message;

    (void)OS_ReceiveMessage(&sndMesgQueue, &message, OS_MESSAGE_BLOCK);

    return message;
}

/*---------------------------------------------------------------------------*
  Name:         SND_SendWakeupMessage

  Description:  Send a message to wake up the sound thread.

  Arguments:    None

  Returns:      Flag expressing whether the message send was successful.
 *---------------------------------------------------------------------------*/
BOOL SND_SendWakeupMessage(void)
{
    return OS_SendMessage(&sndMesgQueue, (OSMessage)SND_MESSAGE_WAKEUP_THREAD, OS_MESSAGE_NOBLOCK);
}

#endif /* SDK_ARM7*/

/******************************************************************************
	private function
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         SNDi_LockMutex

  Description:  Locks sound mutex.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SNDi_LockMutex(void)
{
#ifdef SDK_ARM9
    OS_LockMutex(&sSndMutex);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         SNDi_UnlockMutex

  Description:  Unlocks sound mutex.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void SNDi_UnlockMutex(void)
{
#ifdef SDK_ARM9
    OS_UnlockMutex(&sSndMutex);
#endif
}

/******************************************************************************
	static funtions
 ******************************************************************************/

#ifdef SDK_ARM7

/*---------------------------------------------------------------------------*
  Name:         SndAlarmCallback

  Description:  Callback called in the alarm cycle.

  Arguments:    arg - User data (unused)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SndAlarmCallback(void * /*arg*/ )
{
    if (!OS_SendMessage(&sndMesgQueue, (OSMessage)SND_MESSAGE_PERIODIC, OS_MESSAGE_NOBLOCK))
    {
        OS_PutString("Failed sound alarm OS_SendMessage\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         SndThread

  Description:  Sound thread function.

  Arguments:    arg - User data (unused)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SndThread(void * /*arg*/ )
{
    SND_InitIntervalTimer();
    SND_ExChannelInit();
    SND_SeqInit();
    SND_AlarmInit();
    SND_Enable();
    SND_SetOutputSelector(SND_OUTPUT_MIXER,
                          SND_OUTPUT_MIXER, SND_CHANNEL_OUT_MIXER, SND_CHANNEL_OUT_MIXER);
    SND_SetMasterVolume(SND_MASTER_VOLUME_MAX);

    SND_StartIntervalTimer();

    while (1)
    {
        OSMessage message;
        BOOL    doPeriodicProc = FALSE;

        //-----------------------------------------------------------------------------
        // wait interval timer

        message = SND_WaitForIntervalTimer();

        switch ((u32)message)
        {
        case SND_MESSAGE_PERIODIC:
            doPeriodicProc = TRUE;
            break;
        case SND_MESSAGE_WAKEUP_THREAD:
            break;
        }

        //-----------------------------------------------------------------------------
        // update registers

        SND_UpdateExChannel();

        //-----------------------------------------------------------------------------
        // ARM9 command proc

        SND_CommandProc();

        //-----------------------------------------------------------------------------
        // framework

        SND_SeqMain(doPeriodicProc);
        SND_ExChannelMain(doPeriodicProc);

        SND_UpdateSharedWork();

        (void)SND_CalcRandom();
    }

    SND_Disable();
    SND_StopIntervalTimer();
}

#endif /* SDK_ARM7*/

/*====== End of snd_main.c ======*/
