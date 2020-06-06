/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     mutex.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mutex.h,v $
  Revision 1.11  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.10  06/28/2005 09:49:44  yada
  applied for SDK_THREAD_INFINITY

  Revision 1.9  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.8  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.7  09/02/2004 06:28:30  yada
  only change comment

  Revision 1.6  2004/07/27 03:46:14  yasu
  add '#pragma  warn_padding reset"

  Revision 1.5  04/07/2004 02:03:17  yada
  fix header comment

  Revision 1.4  04/02/2004 12:49:11  yada
  Added a list to mutex

  Revision 1.3  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.2  12/25/2003 07:29:51  yada
  Changes as per unified type rules

  Revision 1.1  11/29/2003 10:12:08  yada
  Initial release.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_MUTEX_H_
#define NITRO_OS_MUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/os/common/thread.h>

//----------------------------------------------------------------
//
//
#ifndef SDK_THREAD_INFINITY
typedef struct OSMutex OSMutex;
#endif

#pragma  warn_padding off
struct OSMutex
{
    OSThreadQueue queue;
    OSThread *thread;                  // the current owner
    s32     count;                     // lock count

#ifndef SDK_THREAD_INFINITY
    OSMutex *prev;                     // link for OSThread.queueMutex
    OSMutex *next;                     // link for OSThread.queueMutex
#else
    OSMutexLink link;
#endif
};
#pragma  warn_padding reset

/*---------------------------------------------------------------------------*
  Name:         OS_InitMutex

  Description:  initialize mutex

  Arguments:    mutex       pointer to mutex structure
                            to be initialized

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_InitMutex(OSMutex *mutex);

/*---------------------------------------------------------------------------*
  Name:         OS_LockMutex

  Description:  lock mutex

  Arguments:    mutex       pointer to mutex structure

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_LockMutex(OSMutex *mutex);

/*---------------------------------------------------------------------------*
  Name:         OS_UnlockMutex

  Description:  unlock mutex

  Arguments:    mutex       pointer to mutex structure

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_UnlockMutex(OSMutex *mutex);

/*---------------------------------------------------------------------------*
  Name:         OS_TryLockMutex

  Description:  try to lock mutex

  Arguments:    mutex       pointer to mutex structure

  Returns:      True if lock
 *---------------------------------------------------------------------------*/
BOOL    OS_TryLockMutex(OSMutex *mutex);

/*---------------------------------------------------------------------------*
  Name:         OSi_UnlockAllMutex

  Description:  unlocks all the mutexes locked by the thread

  Arguments:    mutex       pointer to mutex structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    OSi_UnlockAllMutex(OSThread *thread);

#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_OS_MUTEX_H_*/
#endif
