/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     command.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_FS_COMMAND_H_)
#define NITRO_FS_COMMAND_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/fs/file.h>


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************/
/* constant*/

extern FSResult (*const (fsi_default_command[])) (FSFile *);


/*****************************************************************************/
/* variable*/

extern FSDirPos current_dir_pos;


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
int     FSi_StrNICmp(const char *str1, const char *str2, u32 len);

/*---------------------------------------------------------------------------*
  Name:         FSi_SendCommand

  Description:  Internal function.
                Issues a command to the archive.
                Adjust start timing and block here if sync command.

  Arguments:    p_file           FSFile structure in which the command argument has been specified.
                command          Command ID.

  Returns:      If the command is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FSi_SendCommand(FSFile *p_file, FSCommandType command);

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
FSResult FSi_TranslateCommand(FSFile *p_file, FSCommandType command);

/*---------------------------------------------------------------------------*
  Name:         FSi_ReleaseCommand

  Description:  Internal function.
                If command is complete and there is an idle thread, restore it.

  Arguments:    p_file           FSFile structure that stores the completed command.
                ret              Command result value.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_ReleaseCommand(FSFile *p_file, FSResult ret);

/*---------------------------------------------------------------------------*
  Name:         FSi_NextCommand

  Description:  Internal function.
                Selects the next command to process.
                If async command is selected, return the pointer.
                If non-NULL is returned, it must be processed there.

  Arguments:    p_arc            Archive that gets the next command.

  Returns:      Next command that must be processed here.
 *---------------------------------------------------------------------------*/
FSFile *FSi_NextCommand(FSArchive *p_arc);

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
void    FSi_ExecuteAsyncCommand(FSFile *p_file);

/*---------------------------------------------------------------------------*
  Name:         FSi_ExecuteSyncCommand

  Description:  Internal function.
                Executes commands in synchronous mode

  Arguments:    p_file           FSFile structure that stores the sync command to execute.

  Returns:      If the command is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FSi_ExecuteSyncCommand(FSFile *p_file);


#ifdef __cplusplus
} /* extern "C"*/
#endif


#endif /* NITRO_FS_COMMAND_H_*/

/*---------------------------------------------------------------------------*
  $Log: command.h,v $
  Revision 1.6  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  05/30/2005 04:13:26  yosizaki
  add comments.

  Revision 1.4  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.3  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.2  07/23/2004 08:30:26  yosizaki
  fix command operation.

  Revision 1.1  07/08/2004 13:40:41  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
