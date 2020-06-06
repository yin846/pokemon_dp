/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_task.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_task.c,v $
  Revision 1.6  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.4  02/24/2005 07:34:25  yosizaki
  Deleted unnecessary #ifdef __cplusplus.

  Revision 1.3  01/27/2005 11:27:32  yosizaki
  remove debug-output...

  Revision 1.2  12/08/2004 09:32:46  yosizaki
  fix around end of task-thread.

  Revision 1.1  11/11/2004 11:45:57  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include "mb_task.h"


/* struct ------------------------------------------------------------------*/

typedef struct
{
    OSThread th[1];                    /* thread context */
    MBiTaskInfo *volatile list;        /* waiting task list */
    MBiTaskInfo end_task;              /* task structure for end-command */
}
MBiTaskWork;

/* variable ---------------------------------------------------------------- */

static MBiTaskWork *mbi_task_work = NULL;


/* function ----------------------------------------------------------------*/

static void MBi_TaskThread(void *arg)
{
    MBiTaskWork *const p = (MBiTaskWork *) arg;
    for (;;)
    {
        MBiTaskInfo *trg = NULL;
        /* Get the next task*/
        {
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            /* Sleep if state of Idle*/
            while (!p->list)
            {
                (void)OS_SetThreadPriority(p->th, OS_THREAD_PRIORITY_MIN);
                OS_SleepThread(NULL);
            }
            trg = p->list;
            p->list = p->list->next;
            (void)OS_SetThreadPriority(p->th, trg->priority);
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
        /* Execute task*/
        if (trg->task)
            (*trg->task) (trg);
        /* Execute task-complete callback*/
        {
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            MB_TASK_FUNC callback = trg->callback;
            /*
             * Here, we operate cautiously regarding thread priority levels.
             If there is no next task, specify highest. (wait sleep)
             If there is a next task that is higher than current, change to that.
             If there is a next task that is lower than current, leave as is.
             * Priority level will never be lower than current state.
             */
            const u32 cur_priority = OS_GetThreadPriority(p->th);
            u32     new_priority;
            if (!p->list)
                new_priority = OS_THREAD_PRIORITY_MIN;
            else if (cur_priority < p->list->priority)
                new_priority = p->list->priority;
            else
                new_priority = cur_priority;
            if (new_priority != cur_priority)
                (void)OS_SetThreadPriority(p->th, new_priority);
            trg->next = NULL;
            trg->busy = FALSE;
            if (callback)
                (*callback) (trg);
            /*
             * If end requst, thread ends while interrupts prohibited.
             * (This prohibition setting is valid up to the moment of context switching.)
             */
            if (trg == &p->end_task)
                break;
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
    }
    OS_TPrintf("task-thread end.\n");
    OS_ExitThread();
    return;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_InitTaskThread

  Description:  Starts task thread.
                
  Arguments:    p_work     Buffer for internal work.
                           Used internally until MBi_EndTaskThread() completes.
                size       Byte size of p_work.
                           Must be greater than MB_TASK_WORK_MIN, and
                           size - MB_TASK_WORK_MIN is used by stack.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_InitTaskThread(void *p_work, u32 size)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (!mbi_task_work)
    {
        MBiTaskWork *const p = (MBiTaskWork *) p_work;

        SDK_ASSERT(size >= MB_TASK_WORK_MIN);
        SDK_ASSERT(OS_IsThreadAvailable());

        /* Prepare work structure, stack buffer and task thread*/
        mbi_task_work = p;
        MBi_InitTaskInfo(&p->end_task);
        p->list = NULL;
        size = (u32)((size - sizeof(MBiTaskWork)) & ~3);
        OS_CreateThread(p->th, MBi_TaskThread, p,
                        (u8 *)(p + 1) + size, size, OS_THREAD_PRIORITY_MIN);
        OS_WakeupThreadDirect(p->th);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_IsTaskAvailable

  Description:  Checks if task thread is currently available.
                
  Arguments:    None.

  Returns:      Returns TRUE if currently available. Otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL MBi_IsTaskAvailable(void)
{
    return (mbi_task_work != NULL);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_InitTaskInfo

  Description:  Initializes task information structure.
                Must be called once before using.

  Arguments:    pt         uninitialized task information structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_InitTaskInfo(MBiTaskInfo * pt)
{
    SDK_ASSERT(pt != NULL);
    MI_CpuClear8(pt, sizeof(*pt));
}

/*---------------------------------------------------------------------------*
  Name:         MBi_IsTaskBusy

  Description:  Checks if task information is currently being used.
                
  Arguments:    pt         task information

  Returns:      Returns TRUE if currently being used. Otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL MBi_IsTaskBusy(volatile const MBiTaskInfo * pt)
{
    return pt->busy != FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         MBi_SetTask

  Description:  Adds task to internal thread.
                
  Arguments:    pt         currently unused task information
                task       task function
                callback   callback when task completes (ignored if NULL)
                priority   thread priority of task running    

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_SetTask(MBiTaskInfo * pt, MB_TASK_FUNC task, MB_TASK_FUNC callback, u32 priority)
{
    MBiTaskWork *const p_work = mbi_task_work;

    SDK_ASSERT(pt != NULL);
    SDK_ASSERT(MBi_IsTaskAvailable());

    if (!MBi_IsTaskAvailable())
    {
        OS_TPanic("MBi_SetTask() failed! (task-thread is not available now)");
    }

    if (pt->busy)
    {
        OS_TPanic("MBi_SetTask() failed! (specified structure is busy)");
    }

    /* Support for expanded definitions of priority levels*/
    if (priority > OS_THREAD_PRIORITY_MAX)
    {
        const u32 cur_priority = OS_GetThreadPriority(p_work->th);
        if (priority == MB_TASK_PRIORITY_ABOVE)
        {
            /* Only one priority level higher than caller*/
            priority = (u32)((cur_priority > OS_THREAD_PRIORITY_MIN) ?
                             (cur_priority - 1) : OS_THREAD_PRIORITY_MIN);
        }
        else if (priority == MB_TASK_PRIORITY_BELOW)
        {
            /* Only one priority level lower than caller*/
            priority = (u32)((cur_priority < OS_THREAD_PRIORITY_MAX) ?
                             (cur_priority + 1) : OS_THREAD_PRIORITY_MAX);
        }
        else if (priority == MB_TASK_PRIORITY_NORMAL)
        {
            /* Same priority level as caller*/
            priority = cur_priority;
        }
        else
        {
            /* Simple invalid specification*/
            priority = OS_THREAD_PRIORITY_MAX;
        }
    }
    /* Add task*/
    {
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        pt->busy = TRUE;
        pt->priority = priority;
        pt->task = task;
        pt->callback = callback;
        /* Activate thread if idle-state parent task*/
        if (!p_work->list)
        {

            if (pt == &p_work->end_task)
            {
                /* Prohibit use of task thread from here*/
                mbi_task_work = NULL;
            }

            p_work->list = pt;
            OS_WakeupThreadDirect(p_work->th);
        }
        else
        {
            /* Insert if list is not empty*/
            MBiTaskInfo *pos = p_work->list;
            /* Always add to termination if End command*/
            if (pt == &p_work->end_task)
            {
                while (pos->next)
                    pos = pos->next;
                pos->next = pt;
                /* Prohibit use of task thread from here*/
                mbi_task_work = NULL;
            }
            /* Insert in order of priority if normal command*/
            else
            {
                if (pos->priority > priority)
                {
                    p_work->list = pt;
                    pt->next = pos;
                }
                else
                {
                    while (pos->next && (pos->next->priority <= priority))
                        pos = pos->next;
                    pt->next = pos->next;
                    pos->next = pt;
                }
            }
        }
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_EndTaskThread

  Description:  Ends task thread.
                
  Arguments:    callback   callback when task thread ends (ignored if NULL)
                           This callback is called in the state just before the task thread ends, while
                           interrupts are prohibited.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_EndTaskThread(MB_TASK_FUNC callback)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (MBi_IsTaskAvailable())
    {
        MBi_SetTask(&mbi_task_work->end_task, NULL, callback, OS_THREAD_PRIORITY_MIN);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}
