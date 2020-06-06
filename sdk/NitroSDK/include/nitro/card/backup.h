/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - include
  File:     backup.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: backup.h,v $
  Revision 1.13  04/10/2006 04:12:42  yosizaki
  beautify source code indent.

  Revision 1.12  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.11  09/02/2005 07:35:02  yosizaki
  small fix.

  Revision 1.10  09/02/2005 07:14:55  yosizaki
  add CARD_CancelBackupAsync().

  Revision 1.9  05/12/2005 11:21:18  yosizaki
  add some internal parameters.

  Revision 1.8  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.7  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.6  12/08/2004 06:46:28  yosizaki
  add comments.

  Revision 1.5  09/06/2004 13:44:31  yosizaki
  move CARD_(Lock/Unlock)Backup

  Revision 1.4  08/23/2004 10:39:07  yosizaki
  add write-command.
  delete erase-command. (FLASH)

  Revision 1.3  08/18/2004 13:40:49  yosizaki
  add FLASH 2Mbit.

  Revision 1.2  07/28/2004 13:18:29  yosizaki
  asynchronous mode support.

  Revision 1.1  07/23/2004 08:32:18  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_CARD_BACKUP_H_)
#define NITRO_CARD_BACKUP_H_


#include <nitro/types.h>


#ifdef __cplusplus
extern  "C"
{
#endif


/*---------------------------------------------------------------------------*
  Name:         CARD_LockBackup

  Description:  lock the CARD-bus before backup operation.

  Arguments:    lock id.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_LockBackup(u16 lock_id);

/*---------------------------------------------------------------------------*
  Name:         CARD_UnlockBackup

  Description:  unlock the CARD-bus after backup operation.

  Arguments:    lock id which is used by CARD_LockBackup().

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_UnlockBackup(u16 lock_id);


/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitBackupAsync

  Description:  check if asynchronous backup operation has been completed.

  Arguments:    None.

  Returns:      if operation has been completed, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARD_TryWaitBackupAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitBackupAsync

  Description:  wait for completion of current asynchronous backup operation.

  Arguments:    None.

  Returns:      if the newest operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARD_WaitBackupAsync(void);

/*---------------------------------------------------------------------------*
  Name:         CARD_CancelBackupAsync

  Description:  request to cancel current asynchronous backup operation.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARD_CancelBackupAsync(void);

/* internal chip command as Serial-Bus-Interface*/


/*---------------------------------------------------------------------------*
  Name:         CARDi_RequestStreamCommand

  Description:  Issues request for command to transfer data.

  Arguments:    src        Transfer origin offset or memory address
                dst        Transfer destination offset or memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE
                req_type   Command request type
                req_retry  Max. no. of retries when command request fails
                req_mode   Command request operation mode

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_RequestStreamCommand(u32 src, u32 dst, u32 len,
                                   MIDmaCallback callback, void *arg, BOOL is_async,
                                   CARDRequest req_type, int req_retry, CARDRequestMode req_mode);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadBackup

  Description:  Issues chip command "read".

  Arguments:    src        Transfer origin offset or memory address
                dst        Transfer destination offset or memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ReadBackup(u32 src, void *dst, u32 len,
                                 MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len,
                                      callback, arg, is_async,
                                      CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramBackup

  Description:  Issues read command using chip command "program".

  Arguments:    src        Transfer origin offset or memory address
                dst        Transfer destination offset or memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ProgramBackup(u32 dst, const void *src, u32 len,
                                    MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteBackup

  Description:  Issues a read command that uses chip command "write".

  Arguments:    src        Transfer origin offset or memory address
                dst        Transfer destination offset or memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_WriteBackup(u32 dst, const void *src, u32 len,
                                  MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_VerifyBackup

  Description:  Issues a verify command that uses chip command "read".

  Arguments:    src        Transfer origin offset or memory address
                dst        Transfer destination offset or memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_VerifyBackup(u32 dst, const void *src, u32 len,
                                   MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_VERIFY_BACKUP, 1, CARD_REQUEST_MODE_SEND);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ProgramAndVerifyBackup

  Description:  Issues program and verify commands using chip command "program".

  Arguments:    src        Transfer origin offset or memory address
                dst        Transfer destination offset or memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_ProgramAndVerifyBackup(u32 dst, const void *src, u32 len,
                                             MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_WriteAndVerifyBackup

  Description:  Issues program and verify commands using chip command "write".

  Arguments:    src        Transfer origin offset or memory address
                dst        Transfer destination offset or memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_WriteAndVerifyBackup(u32 dst, const void *src, u32 len,
                                           MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand((u32)src, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SEND_VERIFY);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_EraseBackupSector

  Description:  Issues a sector deletion command using the chip command, erase sector.

  Arguments:    dst -- the deletion target offset
                len -- the deletion size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If async operation was specified, TRUE

  Returns:      If action is successful, TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARDi_EraseBackupSector(u32 dst, u32 len,
                                        MIDmaCallback callback, void *arg, BOOL is_async)
{
    return CARDi_RequestStreamCommand(0, (u32)dst, len, callback, arg, is_async,
                                      CARD_REQ_ERASE_SECTOR_BACKUP, CARD_RETRY_COUNT_MAX,
                                      CARD_REQUEST_MODE_SPECIAL);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadBackupAsync

  Description:  Asynchronous backup read (equivalent to the chip command "read")

  Arguments:    src        Transfer source offset
                dst        Transfer destination memory address.
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ReadBackupAsync(u32 src, void *dst, u32 len, MIDmaCallback callback, void *arg)
{
    (void)CARDi_ReadBackup(src, dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ReadBackup

  Description:  Synchronous backup read (equivalent to the chip command "read")

  Arguments:    src        Transfer source offset
                dst        Transfer destination memory address.
                len        Transfer size

  Returns:      TRUE if completed with CARD_RESULT_SUCCESS, FALSE for everything else.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ReadBackup(u32 src, void *dst, u32 len)
{
    return CARDi_ReadBackup(src, dst, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramBackupAsync

  Description:  Asynchronous backup program (equivalent to the chip command "program")

  Arguments:    dst        Transfer destination offset
                src        Transfer source memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ProgramBackupAsync(u32 dst, const void *src, u32 len,
                                        MIDmaCallback callback, void *arg)
{
    (void)CARDi_ProgramBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramBackup

  Description:  Synchronous backup program (equivalent to the chip command "program")

  Arguments:    dst        Transfer destination offset
                src        Transfer source memory address
                len        Transfer size

  Returns:      TRUE if completed with CARD_RESULT_SUCCESS, FALSE for everything else.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ProgramBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_ProgramBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackupAsync

  Description:  Asynchronous backup write (equivalent to the chip command "write")

  Arguments:    dst        Transfer destination offset
                src        Transfer source memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteBackupAsync(u32 dst, const void *src, u32 len,
                                      MIDmaCallback callback, void *arg)
{
    (void)CARDi_WriteBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteBackup

  Description:  Synchronous backup write (equivalent to the chip command "write")

  Arguments:    dst        Transfer destination offset
                src        Transfer source memory address
                len        Transfer size

  Returns:      TRUE if completed with CARD_RESULT_SUCCESS, FALSE for everything else.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_WriteBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_VerifyBackupAsync

  Description:  Asynchronous backup verify (equivalent to the chip command "read")

  Arguments:    src        Comparison source offset
                dst        Comparison destination memory address
                len        Comparison size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_VerifyBackupAsync(u32 dst, const void *src, u32 len,
                                       MIDmaCallback callback, void *arg)
{
    (void)CARDi_VerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_VerifyBackup

  Description:  Synchronous backup verify (equivalent to the chip command "read")

  Arguments:    src        Comparison source offset
                dst        Comparison destination memory address
                len        Comparison size

  Returns:      TRUE if completed with CARD_RESULT_SUCCESS, FALSE for everything else.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_VerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_VerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramAndVerifyBackupAsync

  Description:  Asynchronous backup program + verify

  Arguments:    dst        Transfer destination offset
                src        Transfer source memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_ProgramAndVerifyBackupAsync(u32 dst, const void *src, u32 len,
                                                 MIDmaCallback callback, void *arg)
{
    (void)CARDi_ProgramAndVerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_ProgramAndVerifyBackup

  Description:  Synchronous backup program + verify

  Arguments:    dst        Transfer destination offset
                src        Transfer source memory address
                len        Transfer size

  Returns:      TRUE if completed with CARD_RESULT_SUCCESS, FALSE for everything else.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_ProgramAndVerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_ProgramAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackupAsync

  Description:  Asynchronous backup write + verify

  Arguments:    dst        Transfer destination offset
                src        Transfer source memory address
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_WriteAndVerifyBackupAsync(u32 dst, const void *src, u32 len,
                                               MIDmaCallback callback, void *arg)
{
    (void)CARDi_WriteAndVerifyBackup(dst, src, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WriteAndVerifyBackup

  Description:  Synchronous backup write + Verify

  Arguments:    dst        Transfer destination offset
                src        Transfer source memory address
                len        Transfer size

  Returns:      TRUE if completed with CARD_RESULT_SUCCESS, FALSE for everything else.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_WriteAndVerifyBackup(u32 dst, const void *src, u32 len)
{
    return CARDi_WriteAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSectorAsync

  Description:  Asynchronous sector deletion.

  Arguments:    dst -- the deletion target offset
                len -- the deletion size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void CARD_EraseBackupSectorAsync(u32 dst, u32 len,
                                            MIDmaCallback callback, void *arg)
{
    (void)CARDi_EraseBackupSector(dst, len, callback, arg, TRUE);
}

/*---------------------------------------------------------------------------*
  Name:         CARD_EraseBackupSector

  Description:  Synchronous sector deletion.

  Arguments:    dst -- the deletion target offset
                len -- the deletion size

  Returns:      TRUE if completed with CARD_RESULT_SUCCESS, FALSE for everything else.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL CARD_EraseBackupSector(u32 dst, u32 len)
{
    return CARDi_EraseBackupSector(dst, len, NULL, NULL, FALSE);
}


#ifdef __cplusplus
} /* extern "C"*/
#endif


#endif /* NITRO_CARD_BACKUP_H_*/
