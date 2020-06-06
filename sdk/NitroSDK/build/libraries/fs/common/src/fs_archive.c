/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_archive.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro/mi.h>
#include <nitro/os.h>

#include <nitro/fs.h>

#include "../include/util.h"
#include "../include/command.h"


/* Basically speaking, archive is not included in ARM7*/
#if defined(FS_IMPLEMENT)


/*****************************************************************************/
/* variable*/

/* Archive list.*/
static FSArchive *arc_list = NULL;

/* Current directory position*/
FSDirPos current_dir_pos;


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         FSi_GetPackedName

  Description:  Internal function.
                Set specified number of chars to lowercase and pack into u32
                If length is greater than FS_ARCHIVE_NAME_LEN_MAX, return 0.

  Arguments:    name             Character string of name to pack
                name_len         Length of name string

  Returns:      The name that was packed.
 *---------------------------------------------------------------------------*/
static u32 FSi_GetPackedName(const char *name, int name_len)
{
    u32     ret = 0;
    if (name_len <= FS_ARCHIVE_NAME_LEN_MAX)
    {
        int     i = 0;
        for (; i < name_len; ++i)
        {
            u32     c = (u32)MI_ReadByte(name + i);
            if (!c)
                break;
            c = (u32)(c - 'A');
            if (c <= (u32)('Z' - 'A'))
                c = (u32)(c + 'a');
            else
                c = (u32)(c + 'A');
            ret |= (u32)(c << (i * 8));
        }
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadMemCallback

  Description:  Internal function.
                Default memory read callback.

  Arguments:    p_arc          Archive on which to operate.
                dst              Storage destination of memory to read.
                pos              Position to read.
                size             Size to read.

  Returns:      Always returns FS_RESULT_SUCCESS.
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadMemCallback(FSArchive *p_arc, void *dst, u32 pos, u32 size)
{
    MI_CpuCopy8((const void *)FS_GetArchiveOffset(p_arc, pos), dst, size);
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_WriteMemCallback

  Description:  Internal function.
                Default memory write callback.

  Arguments:    p_arc          Archive on which to operate.
                dst              Reference destination of memory to write
                pos              Location to write
                size             Size to write.

  Returns:      Always returns FS_RESULT_SUCCESS.
 *---------------------------------------------------------------------------*/
static FSResult FSi_WriteMemCallback(FSArchive *p_arc, const void *src, u32 pos, u32 size)
{
    MI_CpuCopy8(src, (void *)FS_GetArchiveOffset(p_arc, pos), size);
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadMemoryCore

  Description:  Internal function.
                Read from preload table.

  Arguments:    p_arc          Archive on which to operate.
                dst              Storage destination of memory to read.
                pos              Position to read.
                size             Size to read.

  Returns:      Always returns FS_RESULT_SUCCESS.
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadMemoryCore(FSArchive *p_arc, void *dst, u32 pos, u32 size)
{
    (void)p_arc;
    MI_CpuCopy8((const void *)pos, dst, size);
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_NextCommand

  Description:  Internal function.
                Selects the next command to process.
                If async command is selected, return the pointer.
                If non-NULL is returned, it must be processed there.

  Arguments:    p_arc            Archive that gets the next command.

  Returns:      Next command that must be processed here.
 *---------------------------------------------------------------------------*/
FSFile *FSi_NextCommand(FSArchive *p_arc)
{
    /* Prohibit interrupts to cancel and select commands*/
    OSIntrMode bak_psr = OS_DisableInterrupts();

    /* First, check all cancel requests*/
    if (FSi_IsArchiveCanceling(p_arc))
    {
        FSFile *p, *q;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_CANCELING;
        for (p = p_arc->list.next; p; p = q)
        {
            q = p->link.next;
            if (FS_IsCanceling(p))
            {
                if (p_arc->list.next == p)
                    p_arc->list.next = q;
                FSi_ReleaseCommand(p, FS_RESULT_CANCELED);
                if (!q)
                    q = p_arc->list.next;
            }
        }
    }

    /* If not suspended, next select command*/
    if (!FSi_IsArchiveSuspending(p_arc) && !FS_IsArchiveSuspended(p_arc) && p_arc->list.next)
    {
        FSFile *p_file = p_arc->list.next;
        const BOOL is_start = !FSi_IsArchiveRunning(p_arc);
        if (is_start)
            p_arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
        (void)OS_RestoreInterrupts(bak_psr);
        if (is_start)
        {
            /*
             * Activate message notification.
             * (return value has no meaning)
             */
            if ((p_arc->proc_flag & FS_ARCHIVE_PROC_ACTIVATE) != 0)
                (void)(*p_arc->proc) (p_file, FS_COMMAND_ACTIVATE);
        }
        bak_psr = OS_DisableInterrupts();
        p_file->stat |= FS_FILE_STATUS_OPERATING;
        /* If synchronous command, transfer to idle thread*/
        if (FS_IsFileSyncMode(p_file))
        {
            OS_WakeupThread(p_file->queue);
            (void)OS_RestoreInterrupts(bak_psr);
            return NULL;
        }
        /* If asynchronous command, let the invoker process it*/
        else
        {
            (void)OS_RestoreInterrupts(bak_psr);
            return p_file;
        }
    }

    /* If command does not execute, move to idle state*/
    if (FSi_IsArchiveRunning(p_arc))
    {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_RUNNING;
        if ((p_arc->proc_flag & FS_ARCHIVE_PROC_IDLE) != 0)
        {
            FSFile  tmp;
            FS_InitFile(&tmp);
            tmp.arc = p_arc;
            /*
             * IDLE message notification.
             * (return value has no meaning)
             */
            (void)(*p_arc->proc) (&tmp, FS_COMMAND_IDLE);
        }
    }

    /* If idle state is result of suspend transition, wake the executer*/
    if (FSi_IsArchiveSuspending(p_arc))
    {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPENDING;
        p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
        OS_WakeupThread(&p_arc->stat_q);
    }
    (void)OS_RestoreInterrupts(bak_psr);

    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteAsyncCommand

  Description:  Internal function.
                Executes the async command.
                The first call is made from the user thread with interrupts enabled.
                As long as the archive is performing sync. operations, command processing will repeat here.
                If a single async process occurs, the rest is performed with NotifyAsyncEnd().

                When switching archives synchronously/asynchronously,
                be aware of the NotifyAsyncEnd() calling environment.

  Arguments:    p_file           FSFile structure that stores the async command to execute.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_ExecuteAsyncCommand(FSFile *p_file)
{
    FSArchive *const p_arc = p_file->arc;
    while (p_file)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        /* If waiting at call origin for synchrony, wake and pass the process*/
        p_file->stat |= FS_FILE_STATUS_OPERATING;
        if (FS_IsFileSyncMode(p_file))
        {
            OS_WakeupThread(p_file->queue);
            (void)OS_RestoreInterrupts(bak_psr);
            break;
        }
        /* If not, then determine as asynchronous mode here*/
        else
        {
            p_file->stat |= FS_FILE_STATUS_ASYNC;
        }
        (void)OS_RestoreInterrupts(bak_psr);
        /* If process is asynchronous, quit for a time*/
        if (FSi_TranslateCommand(p_file, p_file->command) == FS_RESULT_PROC_ASYNC)
            break;
        /* If result is synchronous end, select to continue here*/
        p_file = FSi_NextCommand(p_arc);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteSyncCommand

  Description:  Internal function.
                Executes commands in synchronous mode

  Arguments:    p_file           FSFile structure that stores the sync command to execute.

  Returns:      If the command is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL FSi_ExecuteSyncCommand(FSFile *p_file)
{
    FSFile *p_target;
    FSResult ret = FSi_TranslateCommand(p_file, p_file->command);
    FSi_ReleaseCommand(p_file, ret);
    p_target = FSi_NextCommand(p_file->arc);
    if (p_target)
        FSi_ExecuteAsyncCommand(p_target);
    return FS_IsSucceeded(p_file);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SendCommand

  Description:  Internal function.
                Issues a command to the archive.
                Adjust start timing and block here if sync command.

  Arguments:    p_file           FSFile structure in which the command argument has been specified.
                command          Command ID.

  Returns:      If the command is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL FSi_SendCommand(FSFile *p_file, FSCommandType command)
{
    FSArchive *const p_arc = p_file->arc;
    const int bit = (1 << command);

    /*
     * SDK does not guarantee thread safety of file itself.
     * Also, status has already been determined by interface of call source.
     */
    p_file->command = command;
    p_file->error = FS_RESULT_BUSY;
    p_file->stat |= FS_FILE_STATUS_BUSY;

    {
        /* Prohibit interrupts in order to add to list and determine startup*/
        OSIntrMode bak_psr = OS_DisableInterrupts();

        /* Cancel if archive is unloading*/
        if (p_arc->flag & FS_ARCHIVE_FLAG_UNLOADING)
        {
            FSi_ReleaseCommand(p_file, FS_RESULT_CANCELED);
            (void)OS_RestoreInterrupts(bak_psr);
            return FALSE;
        }

        /* Add to the list*/
        if ((bit & FS_ARCHIVE_PROC_SYNC) != 0)
            p_file->stat |= FS_FILE_STATUS_SYNC;
        FSi_AppendToList(p_file, (FSFile *)&p_arc->list);

        /* Start archive if idling*/
        if (!FS_IsArchiveSuspended(p_arc) && !FSi_IsArchiveRunning(p_arc))
        {
            p_arc->flag |= FS_ARCHIVE_FLAG_RUNNING;
            (void)OS_RestoreInterrupts(bak_psr);
            /*
             * Activate message notification.
             * (return value has no meaning)
             */
            if ((p_arc->proc_flag & FS_ARCHIVE_PROC_ACTIVATE) != 0)
                (void)(*p_arc->proc) (p_file, FS_COMMAND_ACTIVATE);
            /* If asynchronous mode, execute command and immediately return*/
            bak_psr = OS_DisableInterrupts();
            p_file->stat |= FS_FILE_STATUS_OPERATING;
            if (!FS_IsFileSyncMode(p_file))
            {
                (void)OS_RestoreInterrupts(bak_psr);
                FSi_ExecuteAsyncCommand(p_file);
                return TRUE;
            }
            (void)OS_RestoreInterrupts(bak_psr);
        }

        /* If running and in asynchronous mode, just let things go*/
        else if (!FS_IsFileSyncMode(p_file))
        {
            (void)OS_RestoreInterrupts(bak_psr);
            return TRUE;
        }

        /* If running and in asynchronous mode, wait here for turn*/
        else
        {
            do
            {
                OS_SleepThread(p_file->queue);
            }
            while (!(p_file->stat & FS_FILE_STATUS_OPERATING));
            (void)OS_RestoreInterrupts(bak_psr);
        }

    }

    /*
     * All sync-mode commands come here and complete.
     * Including ReadFileAsync + WaitAsync.
     */
    return FSi_ExecuteSyncCommand(p_file);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_EndArchive

  Description:  End all archives and release.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_EndArchive(void)
{
    OSIntrMode bak_psr = OS_DisableInterrupts();
    FSArchive *volatile *p_list = &arc_list;
    for (;;)
    {
        FSArchive *p_arc = arc_list;
        if (!p_arc)
            break;
        arc_list = arc_list->next;
        (void)FS_UnloadArchive(p_arc);
        FS_ReleaseArchiveName(p_arc);
    }
    (void)OS_RestoreInterrupts(bak_psr);
}

/*---------------------------------------------------------------------------*
  Name:         FS_InitArchive

  Description:  Initializes the archive structure.

  Arguments:    p_arc          Archive to initialize.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_InitArchive(FSArchive *p_arc)
{
    FS_ASSERT_ARG(p_arc, FALSE);
    MI_CpuClear8(p_arc, sizeof(FSArchive));
#if !defined(SDK_NO_THREAD)
    OS_InitThreadQueue(&p_arc->sync_q);
    OS_InitThreadQueue(&p_arc->stat_q);
#endif
}

/*---------------------------------------------------------------------------*
  Name:         FS_FindArchive

  Description:  Searches for archive name.
                If there is no matching name, returns NULL.

  Arguments:    name             Character string of the archive name to search for
                name_len         Length of name string

  Returns:      Pointer to the archive that was found, or NULL.
 *---------------------------------------------------------------------------*/
FSArchive *FS_FindArchive(const char *name, int name_len)
{
    u32     pack = FSi_GetPackedName(name, name_len);
    OSIntrMode bak_psr = OS_DisableInterrupts();
    FSArchive *p_arc = arc_list;
    while (p_arc && (p_arc->name.pack != pack))
        p_arc = p_arc->next;
    (void)OS_RestoreInterrupts(bak_psr);

    return p_arc;
}

/*---------------------------------------------------------------------------*
  Name:         FS_RegisterArchiveName

  Description:  Registers the archive name in the file system and associates it.
                The archive itself is not loaded into the file system.
                The archive name  "rom" is reserved by the file system.

  Arguments:    p_arc            Archive to associate with the name.
                name             String with name to register
                name_len         Length of name string

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL FS_RegisterArchiveName(FSArchive *p_arc, const char *name, u32 name_len)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_arc && name, FALSE);
    FS_ASSERT_ARC_FREE(p_arc, FALSE);

    {
        BOOL    ret = FALSE;
        OSIntrMode bak_intr = OS_DisableInterrupts();
        if (!FS_FindArchive(name, (s32)name_len))
        {
            FSArchive *p_tail = arc_list;
            if (!p_tail)
            {
                arc_list = p_arc;
                current_dir_pos.arc = p_arc;
                current_dir_pos.pos = 0;
                current_dir_pos.index = 0;
                current_dir_pos.own_id = 0;
            }
            else
            {
                while (p_tail->next)
                    p_tail = p_tail->next;
                p_tail->next = p_arc;
                p_arc->prev = p_tail;
            }
            p_arc->name.pack = FSi_GetPackedName(name, (s32)name_len);
            p_arc->flag |= FS_ARCHIVE_FLAG_REGISTER;
            ret = TRUE;
        }
        (void)OS_RestoreInterrupts(bak_intr);
        return ret;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_ReleaseArchiveName

  Description:  Release registered archive names.
                Must be unloaded from the file system.

  Arguments:    p_arc            Archive with name to release.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_ReleaseArchiveName(FSArchive *p_arc)
{
    FS_ASSERT_INIT(void);
    FS_ASSERT_ARG(p_arc, void);
    FS_ASSERT_ARC_NOT_ROM(p_arc, void);
    if (p_arc->name.pack)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        // Cut from list.
        if (p_arc->next)
            p_arc->next->prev = p_arc->prev;
        if (p_arc->prev)
            p_arc->prev->next = p_arc->next;
        p_arc->name.pack = 0;
        p_arc->next = p_arc->prev = NULL;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_REGISTER;
        // If freed archive name is in the current directory,
        //  shift automatically to "rom" root.
        if (current_dir_pos.arc == p_arc)
        {
            current_dir_pos.arc = arc_list;
            current_dir_pos.pos = 0;
            current_dir_pos.index = 0;
            current_dir_pos.own_id = 0;
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadArchive

  Description:  Load the archive into the file system.
                It must already be registered on the archive list.

  Arguments:    p_arc            Archive to load
                base             Any u32 value that can be individually used.
                fat              Starting offset of the FAT table
                fat_size         Size of FAT table
                fat              Starting offset of the FNT table
                fat_size         Size of FNT table
                read_func        Read access callback.
                write_func       Write access callback

  Returns:      If archive is loaded correctly, TRUE.
 *---------------------------------------------------------------------------*/
BOOL FS_LoadArchive(FSArchive *p_arc, u32 base,
                    u32 fat, u32 fat_size,
                    u32 fnt, u32 fnt_size,
                    FS_ARCHIVE_READ_FUNC read_func, FS_ARCHIVE_WRITE_FUNC write_func)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_arc, FALSE);
    FS_ASSERT_ARC_UNLOADED(p_arc, FALSE);

    // Set the base offset.
    p_arc->base = base;
    p_arc->fat_size = fat_size;
    p_arc->fat = p_arc->fat_bak = fat;
    p_arc->fnt_size = fnt_size;
    p_arc->fnt = p_arc->fnt_bak = fnt;
    // Access callback. (If NULL, handled with memory)
    p_arc->read_func = read_func ? read_func : FSi_ReadMemCallback;
    p_arc->write_func = write_func ? write_func : FSi_WriteMemCallback;
    // Initially, preload disabled
    p_arc->table_func = p_arc->read_func;
    p_arc->load_mem = NULL;
    // 
    p_arc->flag |= FS_ARCHIVE_FLAG_LOADED;
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadArchive

  Description:  Unload the archive from the file system.
                Block until all currently running tasks are completed.

  Arguments:    p_arc          Archive to unload.

  Returns:      If archive is unloaded correctly, TRUE.
 *---------------------------------------------------------------------------*/
BOOL FS_UnloadArchive(FSArchive *p_arc)
{
    FS_ASSERT_INIT(FALSE);
    FS_ASSERT_ARG(p_arc, FALSE);

    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        /* Ignore if it is not loaded.*/
        if (FS_IsArchiveLoaded(p_arc))
        {
            /*
             * If table is still not unloaded,
             * warn that memory leak is possible.
             */
            if (FS_IsArchiveTableLoaded(p_arc))
            {
                OS_TWarning("memory may leak. preloaded-table of archive \"%s\" (0x%08X)",
                            p_arc->name.ptr, p_arc->load_mem);
            }
            {
                FSFile *p, *q;
                /* First, suspend*/
                BOOL    bak_state = FS_SuspendArchive(p_arc);
                /* Enable the unloading flag and cancel all commands*/
                p_arc->flag |= FS_ARCHIVE_FLAG_UNLOADING;
                for (p = p_arc->list.next; p; p = q)
                {
                    q = p->link.next;
                    FSi_ReleaseCommand(p, FS_RESULT_CANCELED);
                }
                p_arc->list.next = NULL;
                /* Maintain the previous state*/
                if (bak_state)
                    (void)FS_ResumeArchive(p_arc);
            }
            /* The actual unload process*/
            p_arc->base = 0;
            p_arc->fat = 0;
            p_arc->fat_size = 0;
            p_arc->fnt = 0;
            p_arc->fnt_size = 0;
            p_arc->fat_bak = p_arc->fnt_bak = 0;
            p_arc->flag &= ~(FS_ARCHIVE_FLAG_CANCELING |
                             FS_ARCHIVE_FLAG_LOADED | FS_ARCHIVE_FLAG_UNLOADING);
        }
        (void)OS_RestoreInterrupts(bak_psr);
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         FS_LoadArchiveTables

  Description:  Preload archive FAT + FNT in memory.
                Execute the load and return the required size, only if within specified size.

  Arguments:    p_arc            Archive that will preload table.
                p_mem            Storage destination buffer for table data
                max_size         p_mem size

  Returns:      Always returns the total table size.
 *---------------------------------------------------------------------------*/
u32 FS_LoadArchiveTables(FSArchive *p_arc, void *p_mem, u32 max_size)
{
    FS_ASSERT_INIT(0);
    FS_ASSERT_ARG(p_arc, 0);

    {
        // Preload size is 32-byte aligned.
        u32     total_size = ALIGN_BYTE(p_arc->fat_size + p_arc->fnt_size + 32, 32);
        if (total_size <= max_size)
        {
            // If size is sufficient, load into memory.
            u8     *p_cache = (u8 *)ALIGN_BYTE((u32)p_mem, 32);
            FSFile  tmp;
            FS_InitFile(&tmp);
            /*
             * Sometimes the table cannot be read.
             * In that case, the table could not be accessed from the beginning so nothing happens.
             */
            // Load FAT.
            if (FS_OpenFileDirect(&tmp, p_arc, p_arc->fat, p_arc->fat + p_arc->fat_size, (u32)~0))
            {
                if (FS_ReadFile(&tmp, p_cache, (s32)p_arc->fat_size) < 0)
                {
                    MI_CpuFill8(p_cache, 0x00, p_arc->fat_size);
                }
                (void)FS_CloseFile(&tmp);
            }
            p_arc->fat = (u32)p_cache;
            p_cache += p_arc->fat_size;
            // Load FNT.
            if (FS_OpenFileDirect(&tmp, p_arc, p_arc->fnt, p_arc->fnt + p_arc->fnt_size, (u32)~0))
            {
                if (FS_ReadFile(&tmp, p_cache, (s32)p_arc->fnt_size) < 0)
                {
                    MI_CpuFill8(p_cache, 0x00, p_arc->fnt_size);
                }
                (void)FS_CloseFile(&tmp);
            }
            p_arc->fnt = (u32)p_cache;
            // Save the allocated memory.
            p_arc->load_mem = p_mem;
            // Thereafter, preload memory will work with table-read.
            p_arc->table_func = FSi_ReadMemoryCore;
            p_arc->flag |= FS_ARCHIVE_FLAG_TABLE_LOAD;
        }
        return total_size;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_UnloadArchiveTables

  Description:  Releases the archive's preloaded memory.

  Arguments:    p_arc            Archive with preloaded memory to release.

  Returns:      Buffer given by the user as preloaded memory.
 *---------------------------------------------------------------------------*/
void   *FS_UnloadArchiveTables(FSArchive *p_arc)
{
    FS_ASSERT_INIT(0);
    FS_ASSERT_ARG(p_arc, 0);

    {
        void   *ret = NULL;
        if (FS_IsArchiveLoaded(p_arc))
        {
            BOOL    bak_stat = FS_SuspendArchive(p_arc);
            if (FS_IsArchiveTableLoaded(p_arc))
            {
                p_arc->flag &= ~FS_ARCHIVE_FLAG_TABLE_LOAD;
                ret = p_arc->load_mem;
                p_arc->load_mem = NULL;
                p_arc->fat = p_arc->fat_bak;
                p_arc->fnt = p_arc->fnt_bak;
                p_arc->table_func = p_arc->read_func;
            }
            if (bak_stat)
                (void)FS_ResumeArchive(p_arc);
        }
        return ret;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_SuspendArchive

  Description:  Stops archive processing mechanism itself.
                If a process is currently executing, waits for it to complete.

  Arguments:    p_arc          Archive to stop.

  Returns:      If it was not in a suspended state before the call, TRUE.
 *---------------------------------------------------------------------------*/
BOOL FS_SuspendArchive(FSArchive *p_arc)
{
    FS_ASSERT_INIT(0);
    FS_ASSERT_ARG(p_arc, 0);

    {
        /* Ignore is already suspended*/
        OSIntrMode bak_psr = OS_DisableInterrupts();
        const BOOL bak_stat = !FS_IsArchiveSuspended(p_arc);
        if (bak_stat)
        {
            /* If executing, raise flag and wait for suspension*/
            if (FSi_IsArchiveRunning(p_arc))
            {
                p_arc->flag |= FS_ARCHIVE_FLAG_SUSPENDING;
                do
                    OS_SleepThread(&p_arc->stat_q);
                while (FSi_IsArchiveSuspending(p_arc));
            }
            /* If idling, directly suspend*/
            else
            {
                p_arc->flag |= FS_ARCHIVE_FLAG_SUSPEND;
            }
        }
        (void)OS_RestoreInterrupts(bak_psr);
        return bak_stat;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_ResumeArchive

  Description:  Restart the suspended archive processing.

  Arguments:    p_arc            Archive to reopen.

  Returns:      If it was not in a suspended state before the call, TRUE.
 *---------------------------------------------------------------------------*/
BOOL FS_ResumeArchive(FSArchive *p_arc)
{
    FS_ASSERT_INIT(0);
    FS_ASSERT_ARG(p_arc, 0);

    {
        FSFile *p_target = NULL;
        /* Lower the suspend flag*/
        OSIntrMode bak_irq = OS_DisableInterrupts();
        const BOOL bak_stat = !FS_IsArchiveSuspended(p_arc);
        if (!bak_stat)
        {
            p_arc->flag &= ~FS_ARCHIVE_FLAG_SUSPEND;
            /* If time to start, commence command*/
            p_target = FSi_NextCommand(p_arc);
        }
        (void)OS_RestoreInterrupts(bak_irq);
        if (p_target)
            FSi_ExecuteAsyncCommand(p_target);
        return bak_stat;
    }
}

/*---------------------------------------------------------------------------*
  Name:         FS_SetArchiveProc

  Description:  Sets the archive's user procedure.
                If proc == NULL or flags = 0,
                merely invalidates the user procedure.

  Arguments:    p_arc 			Archive for which to set the user procedure.
                proc             User procedure.
                flags            Bit collection of commands that hook procedures.

  Returns:      Always returns the total table size.
 *---------------------------------------------------------------------------*/
void FS_SetArchiveProc(struct FSArchive *p_arc, FS_ARCHIVE_PROC_FUNC proc, u32 flags)
{
    if (!flags)
        proc = NULL;
    else if (!proc)
        flags = 0;
    p_arc->proc = proc;
    p_arc->proc_flag = flags;
}

/*---------------------------------------------------------------------------*
  Name:         FS_NotifyArchiveAsyncEnd

  Description:  To notify that asynchronous archive processing is complete,
                it calls from the archive implementation side.

  Arguments:    p_arc           Archive for which to notify completion.
                ret              Processing result.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FS_NotifyArchiveAsyncEnd(FSArchive *p_arc, FSResult ret)
{
    /* If during asynchronous command, this signifies the end*/
    if (FSi_IsArchiveAsync(p_arc))
    {
        FSFile *p_file = p_arc->list.next;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_ASYNC;
        FSi_ReleaseCommand(p_file, ret);
        /* If the next is an asynchronous command, execute it here*/
        p_file = FSi_NextCommand(p_arc);
        if (p_file)
            FSi_ExecuteAsyncCommand(p_file);
    }
    /* If a synchronous command, return to the thread that is being blocked*/
    else
    {
        FSFile *p_file = p_arc->list.next;
        OSIntrMode bak_psr = OS_DisableInterrupts();
        p_file->error = ret;
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_SYNC;
#if !defined(SDK_NO_THREAD)
        OS_WakeupThread(&p_arc->sync_q);
#endif
        (void)OS_RestoreInterrupts(bak_psr);
    }
}


#endif /* FS_IMPLEMENT*/

/*---------------------------------------------------------------------------*
  $Log: fs_archive.c,v $
  Revision 1.34  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.33  06/28/2005 02:11:39  yosizaki
  remove some assertions.

  Revision 1.32  06/17/2005 13:07:41  yosizaki
  fix FS_WaitAsync. (add FS_FILE_STATUS_OPERATING)

  Revision 1.31  05/30/2005 04:17:17  yosizaki
  add FSi_EndArchive().
  add comments.

  Revision 1.30  04/15/2005 08:42:58  yosizaki
  fix assertion macros format. (remove ';' )

  Revision 1.29  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.28  2005/02/18 07:26:49  yasu
  Signed/Unsigned conversion warning control

  Revision 1.27  2005/01/26 02:58:51  yasu
  Revised Copyright notation

  Revision 1.26  01/25/2005 11:22:23  yosizaki
  fix around Suspend-state.

  Revision 1.25  11/12/2004 13:36:42  yosizaki
  fix around LoadTable (ignore if empty archive)

  Revision 1.24  11/02/2004 10:06:15  yosizaki
  fix typo.

  Revision 1.23  09/02/2004 11:03:00  yosizaki
  fix FS include directives.

  Revision 1.22  07/23/2004 08:29:52  yosizaki
  add cast to OS_LockCard.

  Revision 1.21  07/19/2004 13:17:36  yosizaki
  add all commands.

  Revision 1.20  07/13/2004 08:00:14  yosizaki
  fix FS_ResumeArchive

  Revision 1.19  07/13/2004 02:54:05  yosizaki
  add argument of FS_OpenFileDirect.

  Revision 1.18  07/12/2004 13:04:45  yosizaki
  change FS_COMMAND_READFILE & FS_COMMAND_WRITEFILE (add argument)

  Revision 1.17  07/09/2004 00:56:32  yosizaki
  fix argument of FS_COMMAND_IDLE procedure-message.

  Revision 1.16  07/08/2004 13:42:00  yosizaki
  change archive commands.

  Revision 1.15  06/30/2004 04:32:30  yosizaki
  implement user-procedure system.

  Revision 1.14  06/22/2004 01:48:48  yosizaki
  fix archive command operation.

  Revision 1.13  06/07/2004 10:26:33  yosizaki
  fix around archive asynchronous operation.

  Revision 1.12  06/03/2004 13:33:07  yosizaki
  fix around archive asynchronous operation.

  Revision 1.11  06/01/2004 10:17:56  yosizaki
  change around assertion code.

  Revision 1.10  05/31/2004 02:56:13  yosizaki
  change FSi_ASSERT_ARG() to FS_ASSERT_ARG().

  Revision 1.9  05/21/2004 10:55:27  yosizaki
  change OS_IsThreadAvailable() to SDK_NO_THREAD.

  Revision 1.8  05/20/2004 09:59:22  yosizaki
  fix asynchronous operation.

  Revision 1.7  05/20/2004 06:31:16  yosizaki
  add FS_ConvertPathToFileID.

  Revision 1.6  05/19/2004 02:30:14  yosizaki
  add FS_GetPathName().

  Revision 1.5  05/17/2004 08:32:26  yosizaki
  add comment.
  add assertion of each interface function.

  Revision 1.4  05/12/2004 05:22:55  yosizaki
  prepare a option "SDK_ARM7FS".

  Revision 1.3  05/11/2004 09:36:36  yosizaki
  fix FS_SeekFile().

  Revision 1.2  05/11/2004 04:30:57  yosizaki
  change archive system.

  Revision 1.1  05/10/2004 06:27:56  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
