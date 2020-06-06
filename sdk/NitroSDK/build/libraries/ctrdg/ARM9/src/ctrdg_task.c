/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CTRDG - libraries - ARM9
  File:     ctrdg_task.c

  Copyright 2003,2004,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ctrdg_task.c,v $
  Revision 1.2  04/07/2006 03:26:35  okubata_ryoma
  released the CTRDG_SetTaskThreadPriority function

  Revision 1.1  04/05/2006 10:48:30  okubata_ryoma
  changes made for AGB backup library's SDK collection


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

/*******************************************************

    function's description

********************************************************/
static CTRDGiTaskWork *ctrdgi_task_work = NULL;
static CTRDGTaskInfo ctrdgi_task_list;

static void CTRDGi_TaskThread(void *arg);

u64 ctrdg_task_stack[CTRDG_TASK_STACK_SIZE / sizeof(u64)];
/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitTaskThread

  Description:  Starts task thread.
                
  Arguments:    p_work     Buffer for internal work.
                           Used internally until CTRDGi_EndTaskThread() completes.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_InitTaskThread(void *p_work)
{
    // IRQ interrupt prohibition
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    // create a thread if the structure is NULL
    if (!ctrdgi_task_work)
    {
        CTRDGiTaskWork *const p = (CTRDGiTaskWork *) p_work;
        
        // determines whether thread has been initialized and can be used
        SDK_ASSERT(OS_IsThreadAvailable());

        /* Prepare work structure, stack buffer and task thread*/
        // At this point, the structure will no longer be NULL, so task threads are not created anew
        ctrdgi_task_work = p;
        // initializes the end_task structure
        CTRDGi_InitTaskInfo(&p->end_task);
        // initializes the ctrdgi_task_list structure
        CTRDGi_InitTaskInfo(&ctrdgi_task_list);
        // there should be no waiting task lists at this point so insert NULL
        p->list = NULL;

        OS_CreateThread(p->th, CTRDGi_TaskThread, p, ctrdg_task_stack + CTRDG_TASK_STACK_SIZE / sizeof(u64), CTRDG_TASK_STACK_SIZE, CTRDG_TASK_PRIORITY_DEFAULT);
        OS_WakeupThreadDirect(p->th);
    }
    // restore IRQ interrupt permission
    (void)OS_RestoreInterrupts(bak_cpsr);
}
/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsTaskAvailable

  Description:  Checks if task thread is currently available.
                
  Arguments:    None.

  Returns:      Returns TRUE if currently available. Otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_IsTaskAvailable(void)
{
    return (ctrdgi_task_work != NULL);
}
/*---------------------------------------------------------------------------*
  Name:         CTRDGi_InitTaskInfo

  Description:  Initializes task information structure.
                Must be called once before using.

  Arguments:    pt         uninitialized task information structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_InitTaskInfo(CTRDGTaskInfo * pt)
{
    SDK_ASSERT(pt != NULL);
    MI_CpuClear8(pt, sizeof(*pt));
}
/*---------------------------------------------------------------------------*
  Name:         CTRDGi_IsTaskBusy

  Description:  Checks if task information is currently being used.
                
  Arguments:    pt         task information

  Returns:      Returns TRUE if currently being used. Otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL CTRDGi_IsTaskBusy(volatile const CTRDGTaskInfo * pt)
{
    return pt->busy != FALSE;
}

static void CTRDGi_TaskThread(void *arg)
{
    CTRDGiTaskWork *const p = (CTRDGiTaskWork *) arg;
    // loop until the thread's end command comes
    for (;;)
    {
        // initializes the structure
        CTRDGTaskInfo trg;
        MI_CpuClear8(&trg, sizeof(CTRDGTaskInfo));
        /* Get the next task*/
        {
            // IRQ interrupts prohibited
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            /* Sleep if state of Idle*/
            // loop and wait until a task comes to the waiting task list
            while(!p->list)
            {
                OS_SleepThread(NULL);
            }
            // as a task has come to the waiting task list, copy that task data structure to trg
            trg = *p->list;
            // restore IRQ interrupt permission
            (void)OS_RestoreInterrupts(bak_cpsr);
        }
        /* Execute task*/
        if (trg.task)
            // run the task with the function pointer, with trg as an argument
            trg.result = (u32)(*trg.task) (&trg);
        /* Execute task-complete callback*/
        // if you've come here, the task is over, so use the task callback
        {
            // IRQ interrupts prohibited
            OSIntrMode bak_cpsr = OS_DisableInterrupts();
            // set the callback function
            CTRDG_TASK_FUNC callback = trg.callback;

            // FALSE, as the task shouldn't be running at this point
            ctrdgi_task_list.busy = FALSE;
            // if there is a callback function
            if (callback)
                // call the callback function with the function pointer, and trg as an argument
                (void)(*callback) (&trg);
             /*
             * If end request, thread ends while interrupts prohibited.
             * (This prohibition setting is valid up to the moment of context switching.)
             */
            //if (p->list == &p->end_task)
            if(ctrdgi_task_work == NULL)
                break;
                
            // initializes the list structure
            p->list = NULL;

            (void)OS_RestoreInterrupts(bak_cpsr);
        }
    }
    OS_TPrintf("task-thread end.\n");
    OS_ExitThread();
    return;
}
/*---------------------------------------------------------------------------*
  Name:         CTRDGi_SetTask

  Description:  Adds task to internal thread.
                
  Arguments:    pt         currently unused task information
                task       task function
                callback   callback when task completes (ignored if NULL)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_SetTask(CTRDGTaskInfo * pt, CTRDG_TASK_FUNC task, CTRDG_TASK_FUNC callback)
{
    // enter the structure where the current thread pointer and waiting task list are
    CTRDGiTaskWork *const p_work = ctrdgi_task_work;

    SDK_ASSERT(pt != NULL);
    SDK_ASSERT(CTRDGi_IsTaskAvailable());

    if (!CTRDGi_IsTaskAvailable())
    {
        OS_TPanic("CTRDGi_SetTask() failed! (task-thread is not available now)");
    }
    
    // bad if there is a running task in the thread
    if (ctrdgi_task_list.busy)
    {
        OS_TPanic("CTRDGi_SetTask() failed! (specified structure is busy)");
    }

    /* Add task*/
    {
        // sets the structure's parameters
        OSIntrMode bak_cpsr = OS_DisableInterrupts();
        pt->busy = TRUE;
        pt->task = task;
        pt->callback = callback;
        /* Activate thread if idle-state parent task*/

        // if this is the command that ends that task
        if (pt == &p_work->end_task)
        {
            /* Prohibit use of task thread from here*/
            ctrdgi_task_work = NULL;
        }
        // insert this task's structure into the waiting task list and launch the task thread
        ctrdgi_task_list = *pt;
        // Stores the actual address.
        p_work->list = &ctrdgi_task_list;
        OS_WakeupThreadDirect(p_work->th);

        (void)OS_RestoreInterrupts(bak_cpsr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CTRDGi_EndTaskThread

  Description:  Ends task thread.
                
  Arguments:    callback   callback when task thread ends (ignored if NULL)
                           This callback is called in the state just before the task thread ends, while
                           interrupts are prohibited.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDGi_EndTaskThread(CTRDG_TASK_FUNC callback)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();
    if (CTRDGi_IsTaskAvailable())
    {
        (void)CTRDGi_SetTask(&ctrdgi_task_work->end_task, NULL, callback);
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         CTRDG_SetTaskThreadPriority

  Description:  Changes the task thread's priority.
                
  Arguments:    priority -- the task thread's priority
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void CTRDG_SetTaskThreadPriority(u32 priority)
{
    if(ctrdgi_task_work)
    {
        CTRDGiTaskWork *const p = ctrdgi_task_work;
        (void)OS_SetThreadPriority(p->th, priority);
    }
}
