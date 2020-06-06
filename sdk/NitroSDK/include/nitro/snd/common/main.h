/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SND - include
  File:     main.h

  Copyright 2004-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.h,v $
  Revision 1.12  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.11  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.10  12/09/2004 02:08:47  ida
  sound patch for NITRO-System 12/06

  Revision 1.10  11/22/2004 04:29:11  ida
  merged init.h

  Revision 1.9  10/04/2004 09:00:20  ida
  add SND_SetThreadPriority

  Revision 1.8  08/19/2004 06:44:32  ida
  for midi

  Revision 1.7  06/29/2004 06:49:58  ida
  remove #include <nitro/os.h>

  Revision 1.6  06/28/2004 07:17:11  ida
  SND_TIMER_INTERVAL -> SND_PROC_INTERVAL

  Revision 1.5  05/06/2004 01:41:48  ida
  SNDi_WakeupThread -> SND_WakeupThread

  Revision 1.4  04/28/2004 01:27:22  ida
  add SNDi_WakeupThread

  Revision 1.3  04/26/2004 02:26:19  ida
  extern OSMessageQueue SNDi_MesgQueue;

  Revision 1.2  04/08/2004 07:29:02  ida
  fix Project name

  Revision 1.1  03/26/2004 06:26:43  ida
  add SND-SDK header files

  Revision 1.4  03/08/2004 05:40:33  ida
  rename files

  Revision 1.3  03/05/2004 06:33:18  ida
  using thread

  Revision 1.2  02/19/2004 07:19:44  ida
  NNS_ -> SND_

  Revision 1.1  02/19/2004 07:07:58  ida
  add NITRO System snd libraries

  Revision 1.2  02/16/2004 06:22:13  ida
  (none)

  Revision 1.1  02/13/2004 01:44:10  ida
  initial version
  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_SND_COMMON_MAIN_H_
#define NITRO_SND_COMMON_MAIN_H_

#include <nitro/types.h>
#include <nitro/os.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	macro definition
 ******************************************************************************/

#define SND_PROC_INTERVAL 0xAA8        // = 5.2095 msec

#define SND_MESSAGE_PERIODIC        1
#define SND_MESSAGE_WAKEUP_THREAD   2

/******************************************************************************
	public function declaration
 ******************************************************************************/

#ifdef SDK_ARM9

void    SND_Init(void);

#else  /* SDK_ARM7*/

void    SND_Init(u32 threadPrio);

void    SND_CreateThread(u32 threadPrio);
BOOL    SND_SetThreadPriority(u32 prio);

void    SND_InitIntervalTimer(void);
void    SND_StartIntervalTimer(void);
void    SND_StopIntervalTimer(void);
OSMessage SND_WaitForIntervalTimer(void);
BOOL    SND_SendWakeupMessage(void);

#endif /* SDK_ARM7*/

/******************************************************************************
	private function declaration
 ******************************************************************************/

void    SNDi_LockMutex(void);
void    SNDi_UnlockMutex(void);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NITRO_SND_COMMON_MAIN_H_ */
