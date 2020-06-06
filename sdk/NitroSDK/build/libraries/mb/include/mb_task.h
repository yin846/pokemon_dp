/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_task.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_task.h,v $
  Revision 1.3  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.2  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.1  11/11/2004 11:51:41  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#if	!defined(NITRO_MB_TASK_H_)
#define NITRO_MB_TASK_H_


#if	defined(__cplusplus)
extern  "C"
{
#endif


/* constant ----------------------------------------------------------------*/

/* Expanded definitions for task thread priority levels*/
    /* Specify a priority level just 1 higher than the caller*/
#define MB_TASK_PRIORITY_ABOVE  (OS_THREAD_PRIORITY_MAX + 1)
    /* Specify a priority level just 1 lower  than the caller*/
#define MB_TASK_PRIORITY_BELOW  (OS_THREAD_PRIORITY_MAX + 2)
    /* Specify the same priority level as the caller*/
#define MB_TASK_PRIORITY_NORMAL (OS_THREAD_PRIORITY_MAX + 3)

#define MB_TASK_WORK_MIN	(sizeof(OSThread) + 256)


/* struct ------------------------------------------------------------------*/

    struct MBiTaskInfo_tag;

    typedef void (*MB_TASK_FUNC) (struct MBiTaskInfo_tag *);

/*
 * Task information structure requested in task thread.     
 */
    typedef struct MBiTaskInfo_tag
    {
/* private: */
        struct MBiTaskInfo_tag *next;  /* next element as list */
        u32     busy:1;                /* if now processing, set 1 */
        u32     priority:31;           /* thread priority*/
        MB_TASK_FUNC task;             /* task function */
        MB_TASK_FUNC callback;         /* callback*/
/* public: */
        u32     param[4];              /* user-defined argument and return-value */
    }
    MBiTaskInfo;



/* function ----------------------------------------------------------------*/


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
    void    MBi_InitTaskThread(void *p_work, u32 size);

/*---------------------------------------------------------------------------*
  Name:         MBi_IsTaskAvailable

  Description:  Checks if task thread is currently available.
                
  Arguments:    None.

  Returns:      Returns TRUE if currently available. Otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
    BOOL    MBi_IsTaskAvailable(void);

/*---------------------------------------------------------------------------*
  Name:         MBi_InitTaskInfo

  Description:  Initializes task information structure.
                Must be called once before using.

  Arguments:    pt         uninitialized task information structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_InitTaskInfo(MBiTaskInfo * pt);

/*---------------------------------------------------------------------------*
  Name:         MBi_IsTaskBusy

  Description:  Checks if task information is currently being used.
                
  Arguments:    pt         task information

  Returns:      Returns TRUE if currently being used. Otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
    BOOL    MBi_IsTaskBusy(volatile const MBiTaskInfo * pt);

/*---------------------------------------------------------------------------*
  Name:         MBi_SetTask

  Description:  Adds task to internal thread.
                
  Arguments:    pt         currently unused task information
                task       task function
                callback   callback when task completes (ignored if NULL)
                priority   thread priority of task running    

  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_SetTask(MBiTaskInfo * pt, MB_TASK_FUNC task, MB_TASK_FUNC callback, u32 priority);

/*---------------------------------------------------------------------------*
  Name:         MBi_EndTaskThread

  Description:  Ends task thread.
                
  Arguments:    callback   callback when task thread ends (ignored if NULL)
                           This callback is called in the state just before the task thread ends, while
                           interrupts are prohibited.
  Returns:      None.
 *---------------------------------------------------------------------------*/
    void    MBi_EndTaskThread(MB_TASK_FUNC callback);


#if	defined(__cplusplus)
}
#endif


#endif                                 /* NITRO_MB_TASK_H_ */
