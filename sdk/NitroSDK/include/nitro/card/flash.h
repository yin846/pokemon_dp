/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - include
  File:     flash.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: flash.h,v $
  Revision 1.6  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.4  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.3  11/02/2004 10:07:39  yosizaki
  fix comment.

  Revision 1.2  08/23/2004 10:39:07  yosizaki
  add write-command.
  delete erase-command. (FLASH)

  Revision 1.1  08/18/2004 13:40:54  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#if	!defined(NITRO_CARD_FLASH_H_)
#define NITRO_CARD_FLASH_H_


#include <nitro/types.h>
#include <nitro/card/common.h>
#include <nitro/card/backup.h>


#if	defined(__cplusplus)
extern  "C"
{
#endif


/* Determine whether the currently set device is FLASH*/
    static inline BOOL CARD_IsBackupFlash(void)
    {
        const CARDBackupType t = CARD_GetCurrentBackupType();
             
             
            return (((t >> CARD_BACKUP_TYPE_DEVICE_SHIFT) &
                     CARD_BACKUP_TYPE_DEVICE_MASK) == CARD_BACKUP_TYPE_DEVICE_FLASH);
    }

/* Data read*/
    static inline BOOL CARD_ReadFlash(u32 src, void *dst, u32 len)
    {
        return CARD_ReadBackup(src, dst, len);
    }
    static inline void CARD_ReadFlashAsync(u32 src, void *dst, u32 len, MIDmaCallback callback,
                                           void *arg)
    {
        CARD_ReadBackupAsync(src, dst, len, callback, arg);
    }

/* Data write*/
    static inline BOOL CARD_WriteFlash(u32 dst, const void *src, u32 len)
    {
        return CARD_WriteBackup(dst, src, len);
    }
    static inline void CARD_WriteFlashAsync(u32 dst, const void *src, u32 len,
                                            MIDmaCallback callback, void *arg)
    {
        CARD_WriteBackupAsync(dst, src, len, callback, arg);
    }

/* Data verify*/
    static inline BOOL CARD_VerifyFlash(u32 dst, const void *src, u32 len)
    {
        return CARD_VerifyBackup(dst, src, len);
    }
    static inline void CARD_VerifyFlashAsync(u32 dst, const void *src, u32 len,
                                             MIDmaCallback callback, void *arg)
    {
        CARD_VerifyBackupAsync(dst, src, len, callback, arg);
    }

/* Data write + verify*/
    static inline BOOL CARD_WriteAndVerifyFlash(u32 dst, const void *src, u32 len)
    {
        return CARD_WriteAndVerifyBackup(dst, src, len);
    }
    static inline void CARD_WriteAndVerifyFlashAsync(u32 dst, const void *src, u32 len,
                                                     MIDmaCallback callback, void *arg)
    {
        CARD_WriteAndVerifyBackupAsync(dst, src, len, callback, arg);
    }


#if	defined(__cplusplus)
}                                      /* extern "C"*/
#endif


#endif /* NITRO_CARD_EEPROM_H_ */
