/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: card_common.h,v $
  Revision 1.26  04/28/2006 07:43:40  yosizaki
  add DeviceCaps.

  Revision 1.25  04/10/2006 04:14:26  yosizaki
  add cmd->spec (erase timeout)

  Revision 1.24  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.23  11/07/2005 01:09:47  okubata_ryoma
  Switched from SDK_ASSERT_ON_COMPILE to SDK_COMPILER_ASSERT

  Revision 1.22  11/01/2005 06:14:25  okubata_ryoma
  Made it so SDK_ASSERT_ON_COMPILE is shared.

  Revision 1.21  10/13/2005 01:14:40  yosizaki
  add initial_status.

  Revision 1.20  09/02/2005 07:03:35  yosizaki
  add CARD_STAT_CANCEL

  Revision 1.19  07/04/2005 01:27:22  yosizaki
  fix size of padding in SDK_THREAD_INFINITY.

  Revision 1.18  06/30/2005 00:00:58  yosizaki
  fix padding for alignment.

  Revision 1.17  2005/06/28 09:52:53  yada
  applied for SDK_THREAD_INFINITY

  Revision 1.16  05/12/2005 11:19:14  yosizaki
  optimize some functions and calling interfaces.
  add comment.

  Revision 1.15  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.14  01/24/2005 06:35:09  yosizaki
  Revised copyright year notation

  Revision 1.13  01/19/2005 12:17:55  yosizaki
  delete pragma `pack'.

  Revision 1.12  12/15/2004 09:44:45  yasu
  Added CARD access enabler mechanism

  Revision 1.11  12/08/2004 12:39:18  yosizaki
  add device-spec members.

  Revision 1.10  11/12/2004 05:44:53  yosizaki
  add CARDi_CheckValidity().

  Revision 1.9  08/23/2004 10:38:23  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.8  08/18/2004 03:24:03  yosizaki
  fix around PXI-INIT command.

  Revision 1.7  07/28/2004 13:18:14  yosizaki
  asynchronous mode support.

  Revision 1.6  07/27/2004 06:47:31  yosizaki
  small fix (warning "padding inserted")

  Revision 1.5  07/24/2004 08:23:55  yasu
  Change type of card owner ID (with error status) to s32.

  Revision 1.4  07/23/2004 08:28:23  yosizaki
  change backup-routine to ARM7 only.

  Revision 1.3  07/22/2004 11:33:28  yosizaki
  fix CARD access of ARM7.

  Revision 1.2  07/19/2004 13:14:32  yosizaki
  add task thread.

  Revision 1.1  07/10/2004 10:20:05  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef __NITRO_CARD_COMMON_H__
#define __NITRO_CARD_COMMON_H__


#include <nitro.h>
#include <nitro/pxi.h>


/*****************************************************************************/
/* macro*/

/* Get the number of elements in the array*/
#define	COUNT_OF_(array)	(sizeof(array) / sizeof(*array))

/* Bit calculation macro*/
#define	BIT_MASK(n)	((1 << (n)) - 1)
#define	ALIGN_MASK(a)	((a) - 1)
#define	ALIGN_BYTE(n, a)	(((u32)(n) + ALIGN_MASK(a)) & ~ALIGN_MASK(a))
#define CARD_ALIGN_HI_BIT(n)     (((u32)(n)) & ~ALIGN_MASK(CARD_ROM_PAGE_SIZE))
#define CARD_ALIGN_LO_BIT(n)     (((u32)(n)) & ALIGN_MASK(CARD_ROM_PAGE_SIZE))


/*****************************************************************************/
/* constant*/

/* CARD internal state*/
enum
{
    CARD_STAT_INIT = (1 << 0),         /* Initialization complete*/
    CARD_STAT_INIT_CMD = (1 << 1),     /* Command PXI initialization complete*/
    CARD_STAT_BUSY = (1 << 2),         /* The read function itself is processing*/
    CARD_STAT_TASK = (1 << 3),         /* Transferring control to task thread*/
    CARD_STAT_RECV = (1 << 4),         /* Receiving request via PXI*/
    CARD_STAT_REQ = (1 << 5),          /* Requesting via PXI inside task process*/
    CARD_STAT_CANCEL = (1 << 6)        /* Currently requesting cancellation*/
};

typedef enum
{
    CARD_TARGET_NONE,
    CARD_TARGET_ROM,
    CARD_TARGET_BACKUP
}
CARDTargetMode;

/* Backup device functions (used internally)*/
#define CARD_BACKUP_CAPS_AVAILABLE      (u32)(CARD_BACKUP_CAPS_READ - 1)
#define CARD_BACKUP_CAPS_READ           (u32)(1 << CARD_REQ_READ_BACKUP)
#define CARD_BACKUP_CAPS_WRITE          (u32)(1 << CARD_REQ_WRITE_BACKUP)
#define CARD_BACKUP_CAPS_PROGRAM        (u32)(1 << CARD_REQ_PROGRAM_BACKUP)
#define CARD_BACKUP_CAPS_VERIFY         (u32)(1 << CARD_REQ_VERIFY_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_PAGE     (u32)(1 << CARD_REQ_ERASE_PAGE_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_SECTOR   (u32)(1 << CARD_REQ_ERASE_SECTOR_BACKUP)
#define CARD_BACKUP_CAPS_ERASE_CHIP     (u32)(1 << CARD_REQ_ERASE_CHIP_BACKUP)


/*****************************************************************************/
/* declaration*/

typedef s32 CARDiOwner;                // s32 for error status


/*
 * PXI  command request send/rec parameter.
 */
typedef struct CARDiCommandArg
{
    CARDResult result;                 /* Returned value*/
    CARDBackupType type;               /* Device type*/
    u32     id;                        /* Card ID*/
    u32     src;                       /* Transfer source*/
    u32     dst;                       /* Transfer destination*/
    u32     len;                       /* Transfer length*/

    struct
    {
        /* memory property*/
        u32     total_size;            /* memory capacity (BYTE)*/
        u32     sect_size;             /* write-sector (BYTE)*/
        u32     page_size;             /* write-page (BYTE)*/
        u32     addr_width;            /* address width on command (BYTE)*/
        /* maximun wait-time (as LCD-V-COUNT tick)*/
        u32     program_page;
        u32     write_page;
        u32     write_page_total;
        u32     erase_chip;
        u32     erase_chip_total;
        u32     erase_sector;
        u32     erase_sector_total;
        u32     erase_page;
        /* initial parameter of status register*/
        u8      initial_status;
        u8      padding1[3];
        u32     caps;
        u8      padding2[16];
    }
    spec;

}
CARDiCommandArg;


/* CARD common parameters*/
typedef struct CARDiCommon
{
    /* Shared memory for command arguments*/
    CARDiCommandArg *cmd;

    int     command;                   /* The current command*/
#if	defined(SDK_ARM7)
    u32     recv_step;                 /* The number of reception steps*/
#endif

    /*
     * Card access rights management.
     * Take exclusive control of card/backup in processor.
     * Because multiple async functions (Rom&Backup) that use card access
     * could be called from the same thread, this is needed.
     *
     *  This follows a lock-ID, as OSMutex follows a thread.
     */
    volatile CARDiOwner lock_owner;    // ==s32 with Error status
    volatile int lock_ref;
#ifndef SDK_THREAD_INFINITY
    OSThreadQueue lock_queue[4 / sizeof(OSThreadQueue)];
#else
    OSThreadQueue lock_queue[1];
#endif
    CARDTargetMode lock_target;

    /* API task parameters*/
    u32     src;                       /* Transfer source*/
    u32     dst;                       /* Transfer destination*/
    u32     len;                       /* The size of the transmission*/
    u32     dma;                       /* DMA channel*/
    CARDRequest req_type;              /* Request type*/
    int     req_retry;                 /* Number of request retries*/
    CARDRequestMode req_mode;          /* Request operation mode*/
    MIDmaCallback callback;            /* Callback*/
    void   *callback_arg;              /* Callback argument*/
    void    (*task_func) (struct CARDiCommon *);        /* Task*/

    /* Task thread*/
    OSThread thread[1];                /* The thread itself*/
    OSThread *cur_th;/* Thread that is handling task.
                                          Can also be user thread*/
    u32     priority;
#ifndef SDK_THREAD_INFINITY
    OSThreadQueue busy_q[4 / sizeof(OSThreadQueue)];    /* Thread that is waiting for end of task*/
#else
    OSThreadQueue busy_q[1];           /* Thread that is waiting for end of task*/
#endif

    /* Status flag*/
    volatile u32 flag;

    /* padding*/
#ifndef SDK_THREAD_INFINITY
#if	defined(SDK_ARM9)
    u8      dummy[28];
#else
    u8      dummy[20];
#endif
#else
#if	defined(SDK_ARM9)
    u8      dummy[8];
#else
    u8      dummy[32];
#endif
#endif

    /* Cache of backup page last sent*/
    u8      backup_cache_page_buf[256] ATTRIBUTE_ALIGN(32);

}
CARDiCommon;


SDK_COMPILER_ASSERT(sizeof(CARDiCommandArg) % 32 == 0);

SDK_COMPILER_ASSERT(sizeof(CARDiCommon) % 32 == 0);


/*****************************************************************************/
/* variable*/

extern CARDiCommon cardi_common;


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         CARDi_SendPxi

  Description:  Sends PXI FIFO word.

  Arguments:    data       Data to send

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_SendPxi(u32 data)
{
    while (PXI_SendWordByFifo(PXI_FIFO_TAG_FS, data, TRUE) < 0)
        ;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_OnFifoRecv

  Description:  PXI FIFO word receive callback

  Arguments:    tag        PXI  tag (always PXI_FIFO_TAG_FS)
                data       Receive data
                err        Error bit (according to old specs)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_OnFifoRecv(PXIFifoTag tag, u32 data, BOOL err);

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetTargetMode

  Description:  Gets the current lock target of the CARD bus.

  Arguments:    None.

  Returns:      One of the three states indicated by the CARDTargetMode.
 *---------------------------------------------------------------------------*/
static inline CARDTargetMode CARDi_GetTargetMode(void)
{
    return cardi_common.lock_target;
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitAsync

  Description:  Waits for asynchronous completion.

  Arguments:    None.

  Returns:      If the latest processing result is CARD_RESULT_SUCCESS, returns TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_WaitAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARDi_TryWaitAsync

  Description:  Tries to wait for async completion and returns control immediately regardless of success or failure.

  Arguments:    None.

  Returns:      If the most recent asynchronous processing is complete, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_TryWaitAsync(void);


/*---------------------------------------------------------------------------*
  Name:         CARDi_WaitTask

  Description:  Waits until it can obtain usage rights for the task thread.
                (Locking of the specified bus is guaranteed by the caller of this function.)

  Arguments:    p            Library's work buffer (passed by argument for efficiency)
                callback     Calllback function after completion of access
                callback_arg  Argument of callback function

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_WaitTask(CARDiCommon * p, MIDmaCallback callback, void *callback_arg)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    while ((p->flag & CARD_STAT_BUSY) != 0)
    {
        OS_SleepThread(p->busy_q);
    }
    p->flag |= CARD_STAT_BUSY;
    p->callback = callback;
    p->callback_arg = callback_arg;
    (void)OS_RestoreInterrupts(bak_psr);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetTask

  Description:  Sets a task to the task thread.
                (Exclusive control of the task thread using CARDi_WaitTask()
                 is guaranteed by the caller of this function.)

  Arguments:    task       Task function to set

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_SetTask(void (*task) (CARDiCommon *));

/*---------------------------------------------------------------------------*
  Name:         CARDi_EndTask

  Description:  Notifies that the task is complete and releases the usage rights of the task thread.

  Arguments:    p            Library's work buffer (passed by argument for efficiency)
                is_own_task  If task is on the same processor, TRUE
                             (If the ARM7 completed ARM9 task processing, FALSE)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_EndTask(CARDiCommon * p, BOOL is_own_task)
{
    const MIDmaCallback func = p->callback;
    void   *const arg = p->callback_arg;

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();

        p->flag &= ~(CARD_STAT_BUSY | CARD_STAT_TASK | CARD_STAT_CANCEL);
        OS_WakeupThread(p->busy_q);
        if ((p->flag & CARD_STAT_RECV) != 0)
        {
            OS_WakeupThreadDirect(p->thread);
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }

    if (is_own_task && func)
    {
        (*func) (arg);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TaskThread

  Description:  Main function for task thread

  Arguments:    arg          Not used

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_TaskThread(void *arg);

/*---------------------------------------------------------------------------*
  Name:         CARDi_InitCommon

  Description:  Initializes shared portions of the Card library.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_InitCommon(void);


#if	defined(SDK_ARM9)

/*---------------------------------------------------------------------------*
  Name:         CARDi_Request

  Description:  Sends request from ARM9 to ARM7 and blocks completion
                If the result is not CARD_RESULT_SUCCESS, retries the specified number of times.
                (Locking of the specified bus and exclusive control of the task thread
                 is guaranteed by the function that called this function.)

  Arguments:    p            Library's work buffer (passed by argument for efficiency)
                req_type     Command request type
                retry_max    Max. number of times to retry

  Returns:      If the result is CARD_RESULT_SUCCESS, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_Request(CARDiCommon * p, int req_type, int retry_max);


#endif /* SDK_ARM9*/


#endif  /*__NITRO_CARD_SPI_H__*/
