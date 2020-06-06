/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     fs_mem.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "../include/util.h"


#if defined(FS_IMPLEMENT)

/*****************************************************************************/
/* variable*/

/* Memory mapped file archive*/
static FSArchive fsi_arc_mem;
static BOOL fsi_mem_init;


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         FSi_MemArchiveProc

  Description:  Memory mapped file archive procedure

  Arguments:    p_file           FSFile structure that stores the command info.
                cmd              Command type

  Returns:      Command process result
 *---------------------------------------------------------------------------*/
static FSResult FSi_MemArchiveProc(FSFile *p_file, FSCommandType cmd)
{
#pragma unused(p_file)

    switch (cmd)
    {

    case FS_COMMAND_SEEKDIR:
    case FS_COMMAND_READDIR:
    case FS_COMMAND_FINDPATH:
    case FS_COMMAND_GETPATH:
    case FS_COMMAND_OPENFILEFAST:
        return FS_RESULT_UNSUPPORTED;

    default:
        return FS_RESULT_PROC_UNKNOWN;

    }

}

/*---------------------------------------------------------------------------*
  Name:         FSi_InitMem

  Description:  Initializes memory mapped file archive

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void FSi_InitMem(void)
{
    OSIntrMode bak_cpsr = OS_DisableInterrupts();

    if (!fsi_mem_init || !FS_IsArchiveLoaded(&fsi_arc_mem))
    {
        FS_InitArchive(&fsi_arc_mem);
        FS_SetArchiveProc(&fsi_arc_mem, FSi_MemArchiveProc, (u32)FS_ARCHIVE_PROC_ALL);
        /* No FAT or FNT*/
        if (!FS_LoadArchive(&fsi_arc_mem, 0, 0, 0, 0, 0, NULL, NULL))
        {
            OS_TPanic("failed to load memory-mapping archive!");
        }
        fsi_mem_init = TRUE;
    }

    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         FS_CreateFileFromMemory

  Description:  Temporarily generates file that maps memory region

  Arguments:    p_file           FSFile structure that stores file handle
                buf              Memory that is target of READ and WRITE
                size			Byte size of buf

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL FS_CreateFileFromMemory(FSFile *p_file, void *buf, u32 size)
{
    FSi_InitMem();
    return FS_OpenFileDirect(p_file, &fsi_arc_mem, (u32)buf, (u32)buf + size, 0);
}


#endif /* FS_IMPLEMENT*/

/*---------------------------------------------------------------------------*
  $Log: fs_mem.c,v $
  Revision 1.2  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  06/28/2005 02:11:11  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
