/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_command.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

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
/* function*/

/*---------------------------------------------------------------------------*
  Name:         FSi_ReleaseCommand

  Description:  Internal function.
                If command is complete and there is an idle thread, restore it.

  Arguments:    p_file           FSFile structure that stores the completed command.
                ret              Command result value.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void FSi_ReleaseCommand(FSFile *p_file, FSResult ret)
{
    SDK_ASSERT(FS_IsBusy(p_file));
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
        FSi_CutFromList(p_file);
        p_file->stat &= ~(FS_FILE_STATUS_CANCEL | FS_FILE_STATUS_BUSY |
                          FS_FILE_STATUS_SYNC | FS_FILE_STATUS_ASYNC | FS_FILE_STATUS_OPERATING);
        p_file->error = ret;
        OS_WakeupThread(p_file->queue);
        (void)OS_RestoreInterrupts(bak_psr);
    }
}

/*---------------------------------------------------------------------------*
  Name:         FSi_TranslateCommand

  Description:  Internal function.
                Calls user procedure or default process and returns result.
                If a synchronous command returns an asynchronous response, wait for completion internally.
                If an asynchronous command returns an asynchronous response, return it as is.

  Arguments:    p_file           FSFile structure that stores the command to execute.
                command          Command ID.

  Returns:      Command processing result.
 *---------------------------------------------------------------------------*/
FSResult FSi_TranslateCommand(FSFile *p_file, FSCommandType command)
{
    FSResult ret;

    FSArchive *const p_arc = p_file->arc;
    const int bit = (1 << command);

    /* Enable flag for synchronous/asynchronous process*/
    if (FS_IsFileSyncMode(p_file))
        p_arc->flag |= FS_ARCHIVE_FLAG_IS_SYNC;
    else
        p_arc->flag |= FS_ARCHIVE_FLAG_IS_ASYNC;

    /* If procedure supports it, call and confirm reply*/
    if ((p_arc->proc_flag & bit) != 0)
    {
        switch (ret = (*p_arc->proc) (p_file, command))
        {
        case FS_RESULT_SUCCESS:
        case FS_RESULT_FAILURE:
        case FS_RESULT_UNSUPPORTED:
            /* Complete processing of command on the procedure side*/
            p_file->error = ret;
            break;
        case FS_RESULT_PROC_ASYNC:
            /* Control handling of asynchronous process in details later*/
            break;
        case FS_RESULT_PROC_UNKNOWN:
            /* Unknown command, so switch to default from this time on*/
            ret = FS_RESULT_PROC_DEFAULT;
            p_arc->proc_flag &= ~bit;
            break;
        }
    }
    else
    {
        ret = FS_RESULT_PROC_DEFAULT;
    }
    /* If necessary call the default process*/
    if (ret == FS_RESULT_PROC_DEFAULT)
    {
        ret = (*fsi_default_command[command]) (p_file);
    }
    /* If entering asynchronous process, block if mode is synchronous*/
    if (ret == FS_RESULT_PROC_ASYNC)
    {
        if (FS_IsFileSyncMode(p_file))
        {
            OSIntrMode bak_psr = OS_DisableInterrupts();
            while (FSi_IsArchiveSync(p_arc))
                OS_SleepThread(&p_arc->sync_q);
            ret = p_file->error;
            (void)OS_RestoreInterrupts(bak_psr);
        }
    }
    /* If synchronous completed in asynchronous mode, release here*/
    else if (!FS_IsFileSyncMode(p_file))
    {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_ASYNC;
        FSi_ReleaseCommand(p_file, ret);
    }
    /* If synchronous-type command is synchronously completed, store the result as-is*/
    else
    {
        p_arc->flag &= ~FS_ARCHIVE_FLAG_IS_SYNC;
        p_file->error = ret;
    }
    /* Return the result. (Return FS_RESULT_PROC_ASYNC only for asynchronous-type asynchronous process)*/
    return ret;
}


#endif /* FS_IMPLEMENT*/

/*---------------------------------------------------------------------------*
  $Log: fs_command.c,v $
  Revision 1.14  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.13  06/17/2005 13:02:55  yosizaki
  fix FS_WaitAsync. (add FS_FILE_STATUS_OPERATING)

  Revision 1.12  05/30/2005 04:18:26  yosizaki
  move some functions to fs_command_default.c.
  add comments.

  Revision 1.11  02/28/2005 05:26:13  yosizaki
  do-indent.

  Revision 1.10  2005/02/18 07:26:49  yasu
  Signed/Unsigned conversion warning control

  Revision 1.9  10/22/2004 13:06:52  yosizaki

  Revision 1.8  09/22/2004 08:55:59  yosizaki
  fix FSi_SeekDirDirect().

  Revision 1.7  09/03/2004 06:54:20  yosizaki
  small fix.

  Revision 1.6  09/02/2004 11:03:00  yosizaki
  fix FS include directives.

  Revision 1.5  08/05/2004 03:26:28  yosizaki
  fix FS_COMMAND_READDIR (numbering of file-ID)

  Revision 1.4  07/23/2004 08:30:06  yosizaki
  fix command operation.

  Revision 1.3  07/19/2004 13:18:03  yosizaki
  change system (sync & async)

  Revision 1.2  07/12/2004 13:04:00  yosizaki
  small fix (FS_COMMAND_GETPATH)

  Revision 1.1  07/08/2004 13:41:00  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
