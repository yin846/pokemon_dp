/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_command_default.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/util.h"
#include "../include/command.h"


/* Basically speaking, archive is not included in ARM7*/
#if defined(FS_IMPLEMENT)


/*****************************************************************************/
/* declaration*/

/* Blocking Read structure for synchronous-type command*/
typedef struct
{
    FSArchive *arc;                    /* Target archive*/
    u32     pos;                       /* Starting position*/
}
FSiSyncReadParam;

static FSResult FSi_ReadFileCommand(FSFile *p_file);
static FSResult FSi_WriteFileCommand(FSFile *p_file);
static FSResult FSi_SeekDirCommand(FSFile *p_dir);
static FSResult FSi_ReadDirCommand(FSFile *p_dir);
static FSResult FSi_FindPathCommand(FSFile *p_dir);
static FSResult FSi_GetPathCommand(FSFile *p_file);
static FSResult FSi_OpenFileFastCommand(FSFile *p_file);
static FSResult FSi_OpenFileDirectCommand(FSFile *p_file);
static FSResult FSi_CloseFileCommand(FSFile *p_file);


/*****************************************************************************/
/* constant*/

FSResult (*const (fsi_default_command[])) (FSFile *) =
{
FSi_ReadFileCommand,
        FSi_WriteFileCommand,
        FSi_SeekDirCommand,
        FSi_ReadDirCommand,
        FSi_FindPathCommand,
        FSi_GetPathCommand,
        FSi_OpenFileFastCommand, FSi_OpenFileDirectCommand, FSi_CloseFileCommand,};


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         FSi_StrNICmp

  Description:  Internal function.
                Compares character strings for the specified number of bytes; not case-sensitive

  Arguments:    str1             Comparison source character string
                str2             Comparison destination character string
                len              No. of bytes to compare

  Returns:      (str1 - str2) comparison result
 *---------------------------------------------------------------------------*/
int FSi_StrNICmp(const char *str1, const char *str2, u32 len)
{
    int     i;
    for (i = 0; i < len; ++i)
    {
        u32     c = (u32)(MI_ReadByte(str1 + i) - 'A');
        u32     d = (u32)(MI_ReadByte(str2 + i) - 'A');
        if (c <= 'Z' - 'A')
            c += 'a' - 'A';
        if (d <= 'Z' - 'A')
            d += 'a' - 'A';
        if (c != d)
            return (int)(c - d);
    }
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadTable

  Description:  Internal function.
                Execute synchronous Read inside synchronous-type command

  Arguments:    p                Synchronous read structure
                dst              Read data storage destination buffer.
                len              No. of bytes to read

  Returns:      Result of synchronous read.
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadTable(FSiSyncReadParam * p, void *dst, u32 len)
{
    FSResult result;
    FSArchive *const p_arc = p->arc;

    /* Start command's synchronous process*/
    p_arc->flag |= FS_ARCHIVE_FLAG_IS_SYNC;
    result = (*p_arc->table_func) (p_arc, dst, p->pos, len);
    switch (result)
    {
    case FS_RESULT_SUCCESS:
    case FS_RESULT_FAILURE:
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_SYNC;
        break;
    case FS_RESULT_PROC_ASYNC:
#if !defined(SDK_NO_THREAD)
        {
            /* Sleep if asynchronous*/
            OSIntrMode bak_psr = OS_DisableInterrupts();
            while (FSi_IsArchiveSync(p_arc))
                OS_SleepThread(&p_arc->sync_q);
            (void)OS_RestoreInterrupts(bak_psr);
        }
#else
        while (FSi_IsArchiveSync(p_arc))
            ;
#endif
        result = p_arc->list.next->error;
        break;
    }
    p->pos += len;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SeekDirDirect

  Description:  Internal function.
                Directly moves to beginning of specified directory.

  Arguments:    p_dir            Pointer that stores directory list
                id               Directory ID.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_SeekDirDirect(FSFile *p_dir, u32 id)
{
    p_dir->stat |= FS_FILE_STATUS_SYNC;
    p_dir->arg.seekdir.pos.arc = p_dir->arc;
    p_dir->arg.seekdir.pos.pos = 0;
    p_dir->arg.seekdir.pos.index = 0;
    p_dir->arg.seekdir.pos.own_id = (u16)id;
    (void)FSi_TranslateCommand(p_dir, FS_COMMAND_SEEKDIR);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadFileCommand

  Description:  Internal function.
                Default implementation of READFILE command.

  Arguments:    p_file           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadFileCommand(FSFile *p_file)
{
    FSArchive *const p_arc = p_file->arc;
    const u32 pos = p_file->prop.file.pos;
    const u32 len = p_file->arg.readfile.len;
    void   *const dst = p_file->arg.readfile.dst;
    p_file->prop.file.pos += len;
    return (*p_arc->read_func) (p_arc, dst, pos, len);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_WriteFileCommand

  Description:  Internal function.
                Default implementation of WRITEFILE command.

  Arguments:    p_file           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_WriteFileCommand(FSFile *p_file)
{
    FSArchive *const p_arc = p_file->arc;
    const u32 pos = p_file->prop.file.pos;
    const u32 len = p_file->arg.writefile.len;
    const void *const src = p_file->arg.writefile.src;
    p_file->prop.file.pos += len;
    return (*p_arc->write_func) (p_arc, src, pos, len);
}

/*---------------------------------------------------------------------------*
  Name:         FSi_SeekDirCommand

  Description:  Internal function.
                Default implementation of SEEKDIR command.

  Arguments:    p_dir           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_SeekDirCommand(FSFile *p_dir)
{
    FSResult result;
    FSArchive *const p_arc = p_dir->arc;
    const FSDirPos *const arg = &p_dir->arg.seekdir.pos;

    /* access to specified index of FNT*/
    FSArchiveFNT fnt_entry;
    FSiSyncReadParam param;
    param.arc = p_arc;
    param.pos = p_arc->fnt + arg->own_id * sizeof(fnt_entry);
    result = FSi_ReadTable(&param, &fnt_entry, sizeof(fnt_entry));
    if (result == FS_RESULT_SUCCESS)
    {
        p_dir->prop.dir.pos = *arg;
        /* if !index && !pos, seek to first entry*/
        if ((arg->index == 0) && (arg->pos == 0))
        {
            p_dir->prop.dir.pos.index = fnt_entry.index;
            p_dir->prop.dir.pos.pos = p_arc->fnt + fnt_entry.start;
        }
        p_dir->prop.dir.parent = (u32)(fnt_entry.parent & BIT_MASK(12));
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_ReadDirCommand

  Description:  Internal function.
                Default implementation of READDIR command.

  Arguments:    p_dir           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_ReadDirCommand(FSFile *p_dir)
{
    FSResult result;
    FSDirEntry *p_entry = p_dir->arg.readdir.p_entry;
    u8      len;

    /* pos directs next entry of directory*/
    FSiSyncReadParam param;
    param.arc = p_dir->arc;
    param.pos = p_dir->prop.dir.pos.pos;

    /* read length and property*/
    result = FSi_ReadTable(&param, &len, sizeof(len));
    if (result != FS_RESULT_SUCCESS)
    {
        return result;
    }
    p_entry->name_len = (u32)(len & BIT_MASK(7));
    p_entry->is_directory = (u32)((len >> 7) & 1);
    /* if end of entry, return FAILURE*/
    if (p_entry->name_len == 0)
    {
        result = FS_RESULT_FAILURE;
        return result;
    }

    /* read or skip filename section*/
    if (!p_dir->arg.readdir.skip_string)
    {
        result = FSi_ReadTable(&param, p_entry->name, p_entry->name_len);
        if (result != FS_RESULT_SUCCESS)
        {
            return result;
        }
        MI_WriteByte((u8 *)p_entry->name + p_entry->name_len, (u8)'\0');
    }
    else
    {
        param.pos += p_entry->name_len;
    }

    if (p_entry->is_directory)
    {
        /* if directory information, read index*/
        u16     id;
        result = FSi_ReadTable(&param, &id, sizeof(id));
        if (result != FS_RESULT_SUCCESS)
        {
            return result;
        }
        p_entry->dir_id.arc = p_dir->arc;
        p_entry->dir_id.own_id = (u16)(id & BIT_MASK(12));
        p_entry->dir_id.index = 0;
        p_entry->dir_id.pos = 0;
    }
    else
    {
        /* if file information, use index incrementally*/
        p_entry->file_id.arc = p_dir->arc;
        p_entry->file_id.file_id = p_dir->prop.dir.pos.index;
        ++p_dir->prop.dir.pos.index;
    }
    /* if succeeded, increment position to next entry*/
    p_dir->prop.dir.pos.pos = param.pos;

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_FindPathCommand

  Description:  Internal function.
                Default implementation of FINDPATH command.
                (Uses SEEKDIR, READDIR  commands)

  Arguments:    p_dir           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_FindPathCommand(FSFile *p_dir)
{
    const char *path = p_dir->arg.findpath.path;
    const BOOL is_dir = p_dir->arg.findpath.find_directory;

    /* search path in order from specified position*/
    p_dir->arg.seekdir.pos = p_dir->arg.findpath.pos;
    (void)FSi_TranslateCommand(p_dir, FS_COMMAND_SEEKDIR);
    for (; MI_ReadByte(path); path += (MI_ReadByte(path) ? 1 : 0))
    {
        u32     is_directory;
        int     name_len = 0;
        while ((is_directory = MI_ReadByte(path + name_len)),
               (is_directory && !FSi_IsSlash(is_directory)))
            ++name_len;
        if (is_directory || is_dir)
            is_directory = 1;

        /* invalid path such as "//"*/
        if (name_len == 0)
        {
            return FS_RESULT_FAILURE;
        }
        /* special directory*/
        else if (MI_ReadByte(path) == '.')
        {
            /* "." means current directory*/
            if (name_len == 1)
            {
                path += 1;
                continue;
            }
            /* ".."  means parent directory*/
            else if ((name_len == 2) & (MI_ReadByte(path + 1) == '.'))
            {
                if (p_dir->prop.dir.pos.own_id != 0)
                    FSi_SeekDirDirect(p_dir, p_dir->prop.dir.parent);
                path += 2;
                continue;
            }
        }
        /* too long filename (error of archive)*/
        if (name_len > FS_FILE_NAME_MAX)
        {
            return FS_RESULT_FAILURE;
        }
        /* search the coincidence name in a directory in order*/
        else
        {
            FSDirEntry etr;
            p_dir->arg.readdir.p_entry = &etr;
            p_dir->arg.readdir.skip_string = FALSE;
            for (;;)
            {
                /* if end of entry or error, not found*/
                if (FSi_TranslateCommand(p_dir, FS_COMMAND_READDIR) != FS_RESULT_SUCCESS)
                    return FS_RESULT_FAILURE;
                /* if don't match, ignore*/
                if ((is_directory != etr.is_directory) ||
                    (name_len != etr.name_len) || FSi_StrNICmp(path, etr.name, (u32)name_len))
                    continue;
                /* if match to directory, move to its first position*/
                if (is_directory)
                {
                    path += name_len;
                    p_dir->arg.seekdir.pos = etr.dir_id;
                    (void)FSi_TranslateCommand(p_dir, FS_COMMAND_SEEKDIR);
                    break;
                }
                /* if match to file with expecting directory, FAILURE*/
                else if (is_dir)
                {
                    return FS_RESULT_FAILURE;
                }
                /* file is found, return its index*/
                else
                {
                    *p_dir->arg.findpath.result.file = etr.file_id;
                    return FS_RESULT_SUCCESS;
                }

            }
        }
    }
    /* if reach end of path without matching file, FAILURE*/
    if (!is_dir)
        return FS_RESULT_FAILURE;
    /* directory is found, return its first position*/
    *p_dir->arg.findpath.result.dir = p_dir->prop.dir.pos;
    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_GetPathCommand

  Description:  Internal function.
                Default implementation of GETPATH command.
                (Uses SEEKDIR, READDIR  commands)

  Arguments:    p_dir           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_GetPathCommand(FSFile *p_file)
{
    FSArchive *const p_arc = p_file->arc;

    FSGetPathInfo *p_info = &p_file->arg.getpath;

    FSDirEntry entry;
    FSFile  tmp;
    u32     dir_id;
    u32     file_id;
    u32     id;
    u32     len;

    enum
    { INVALID_ID = 0x10000 };

    FS_InitFile(&tmp);
    tmp.arc = p_file->arc;

    if (FS_IsDir(p_file))
    {
        /* If directory, there is no special process.*/
        dir_id = p_file->prop.dir.pos.own_id;
        file_id = INVALID_ID;
    }
    else
    {
        /* If file, then directory is unclear*/
        file_id = p_file->prop.file.own_id;
        if (p_info->total_len != 0)
        {
            /* If search already completed, there is no special process*/
            dir_id = p_info->dir_id;
        }
        else
        {
            /* If not searched, then search directory from FNT*/
            u32     pos = 0;
            u32     num_dir = 0;
            dir_id = INVALID_ID;
            do
            {
                /* Thoroughly search all directories*/
                FSi_SeekDirDirect(&tmp, pos);
                if (!pos)
                    num_dir = tmp.prop.dir.parent;
                /*
                 * Eliminate index-based search conditions so that
                 * customized low-level commands can be used
                 */
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = TRUE;
                while (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) == FS_RESULT_SUCCESS)
                {
                    if (!entry.is_directory && (entry.file_id.file_id == file_id))
                    {
                        dir_id = tmp.prop.dir.pos.own_id;
                        break;
                    }
                }
            }
            while ((dir_id == INVALID_ID) && (++pos < num_dir));
        }
    }
    /* If cannot find directory, FALSE*/
    if (dir_id == INVALID_ID)
    {
        p_info->total_len = 0;
        return FS_RESULT_FAILURE;
    }
    /* If path length not calculated, measure once this time*/
    if (p_info->total_len == 0)
    {
        len = 0;
        /* First, add "archive name:/"*/
        if (p_arc->name.pack <= 0x000000FF)
            len += 1;
        else if (p_arc->name.pack <= 0x0000FF00)
            len += 2;
        else
            len += 3;
        len += 1 + 1;                  /* ":/" */
        /* If necessary, add file name. (Already obtained)*/
        if (file_id != INVALID_ID)
            len += entry.name_len;
        /* Next, trace back in order and add own name*/
        id = dir_id;
        if (id != 0)
        {
            FSi_SeekDirDirect(&tmp, id);
            do
            {
                /* Move to parent and search self*/
                FSi_SeekDirDirect(&tmp, tmp.prop.dir.parent);
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = TRUE;
                while (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) == FS_RESULT_SUCCESS)
                {
                    if (entry.is_directory && (entry.dir_id.own_id == id))
                    {
                        len += entry.name_len + 1;
                        break;
                    }
                }
                id = tmp.prop.dir.pos.own_id;
            }
            while (id != 0);
        }
        /* Save the calculated data*/
        p_info->total_len = (u16)(len + 1);
        p_info->dir_id = (u16)dir_id;
    }
    /* Success if measuring path length is the objective*/
    if (!p_info->buf)
        return FS_RESULT_SUCCESS;
    /* Failure if buffer length is insufficient*/
    if (p_info->buf_len < p_info->total_len)
        return FS_RESULT_FAILURE;
    /* Store the character string from the end*/
    else
    {
        u8     *dst = p_info->buf;
        u32     total = p_info->total_len;
        u32     pos = 0;
        /* First, add "archive name:/"*/
        if (p_arc->name.pack <= 0x000000FF)
            len = 1;
        else if (p_arc->name.pack <= 0x0000FF00)
            len = 2;
        else
            len = 3;
        MI_CpuCopy8(p_arc->name.ptr, dst + pos, len);
        pos += len;
        MI_CpuCopy8(":/", dst + pos, 2);
        pos += 2;
        /* Get the standard directory*/
        id = dir_id;
        FSi_SeekDirDirect(&tmp, id);
        if (file_id != INVALID_ID)
        {
            /* If necessary, add file name.*/
            tmp.arg.readdir.p_entry = &entry;
            tmp.arg.readdir.skip_string = FALSE;
            while (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) == FS_RESULT_SUCCESS)
            {
                if (!entry.is_directory && (entry.file_id.file_id == file_id))
                    break;
            }
            len = entry.name_len + 1;
            MI_CpuCopy8(entry.name, dst + total - len, len);
            total -= len;
        }
        else
        {
            /* If directory, append only the end*/
            MI_WriteByte(dst + total - 1, '\0');
            total -= 1;
        }
        /* Next, trace back in order and add own name*/
        if (id != 0)
        {
            do
            {
                /* Move to parent*/
                FSi_SeekDirDirect(&tmp, tmp.prop.dir.parent);
                tmp.arg.readdir.p_entry = &entry;
                tmp.arg.readdir.skip_string = FALSE;
                /* Add "/"*/
                MI_WriteByte(dst + total - 1, '/');
                total -= 1;
                /* Search parent's children (previous self)*/
                while (FSi_TranslateCommand(&tmp, FS_COMMAND_READDIR) == FS_RESULT_SUCCESS)
                {
                    if (entry.is_directory && (entry.dir_id.own_id == id))
                    {
                        len = entry.name_len;
                        MI_CpuCopy8(entry.name, dst + total - len, len);
                        total -= len;
                        break;
                    }
                }
                id = tmp.prop.dir.pos.own_id;
            }
            while (id != 0);
        }
        SDK_ASSERT(pos == total);
    }

    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_OpenFileFastCommand

  Description:  Internal function.
                Default implementation of OPENFILEFAST command.
                (Uses OPENFILEDIRECT command)

  Arguments:    p_file           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_OpenFileFastCommand(FSFile *p_file)
{
    FSResult result;
    FSArchive *const p_arc = p_file->arc;
    const FSFileID *p_id = &p_file->arg.openfilefast.id;
    const u32 index = p_id->file_id;

    {
        /* Determine correctness of FAT access region*/
        u32     pos = (u32)(index * sizeof(FSArchiveFAT));
        if (pos >= p_arc->fat_size)
        {
            result = FS_RESULT_FAILURE;
        }
        else
        {
            // Obtain the image area of the specified file from FAT.
            FSArchiveFAT fat;
            FSiSyncReadParam param;
            param.arc = p_arc;
            param.pos = p_arc->fat + pos;
            result = FSi_ReadTable(&param, &fat, sizeof(fat));
            if (result == FS_RESULT_SUCCESS)
            {
                /* Directly open*/
                p_file->arg.openfiledirect.top = fat.top;
                p_file->arg.openfiledirect.bottom = fat.bottom;
                p_file->arg.openfiledirect.index = index;
                result = FSi_TranslateCommand(p_file, FS_COMMAND_OPENFILEDIRECT);
            }
        }
    }
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_OpenFileDirectCommand

  Description:  Internal function.
                Default implementation of OPENFILEDIRECT command.

  Arguments:    p_file           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_OpenFileDirectCommand(FSFile *p_file)
{
    /* Initialize the file information*/
    p_file->prop.file.top = p_file->arg.openfiledirect.top;
    p_file->prop.file.pos = p_file->arg.openfiledirect.top;
    p_file->prop.file.bottom = p_file->arg.openfiledirect.bottom;
    p_file->prop.file.own_id = p_file->arg.openfiledirect.index;

    return FS_RESULT_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         FSi_CloseFileCommand

  Description:  Internal function.
                Default implementation of CLOSEFILE command.

  Arguments:    p_file           Handle to process command.

  Returns:      Command result value.
 *---------------------------------------------------------------------------*/
static FSResult FSi_CloseFileCommand(FSFile *p_file)
{
    (void)p_file;
    return FS_RESULT_SUCCESS;
}


#endif /* FS_IMPLEMENT*/

/*---------------------------------------------------------------------------*
  $Log: fs_command_default.c,v $
  Revision 1.4  05/08/2006 01:31:19  yosizaki
  small fix.

  Revision 1.3  04/27/2006 09:36:31  yosizaki
  fix about FSi_ReadTable().

  Revision 1.2  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  05/30/2005 04:15:59  yosizaki
  initial upload. (moved from fs_command.c)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
