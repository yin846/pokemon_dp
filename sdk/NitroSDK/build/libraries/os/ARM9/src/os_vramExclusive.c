/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - libraries
  File:     os_vramExclusive.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_vramExclusive.c,v $
  Revision 1.4  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.3  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.2  2005/02/18 06:18:49  yasu
  Signed/Unsigned conversion warning control

  Revision 1.1  02/08/2005 08:38:20  terui
  Initial release.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include        <nitro/os/ARM9/vramExclusive.h>
#include        <nitro/os/common/interrupt.h>


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static u32 OSi_vramExclusive;
static u16 OSi_vramLockId[OS_VRAM_BANK_KINDS];


/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         OsCountZeroBits

  Description:  Counts the string of 0 bits starting from the top inside a 32-bit value, returning the number of zeros.    

  Arguments:    bitmap  -       The value to evaluate.

  Returns:      u32     -       The number of 0 bits.
                                                        0x80000000 = 0; 0x00000000 = 32.
 *---------------------------------------------------------------------------*/
#include        <nitro/code32.h>
static asm u32
OsCountZeroBits( u32 bitmap )
{
    clz         r0,             r0
        bx              lr
        }
#include        <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  Name:         OSi_InitVramExclusive

  Description:  Initializes VRAM exclusive process.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
OSi_InitVramExclusive( void )
{
    s32         i;

    OSi_vramExclusive = 0x0000;
    for( i = 0 ; i < OS_VRAM_BANK_KINDS ; i ++ )
    {
        OSi_vramLockId[ i ] = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         OSi_TryLockVram

  Description:  Tries the VRAM exclusive lock.

  Arguments:    bank    -  ID bitmap of VRAM on which exclusive lock is being tried.
                                lockId  -  Optional ID for key when locked.    

  Returns:      BOOL    -   Returns TRUE if the lock is successful.
 *---------------------------------------------------------------------------*/
BOOL OSi_TryLockVram(u16 bank, u16 lockId)
{
    u32     workMap;
    s32     zeroBits;
    OSIntrMode enabled = OS_DisableInterrupts();

    // Check whether exclusive lock already set with different ID
    workMap = (u32)(bank & OSi_vramExclusive);
    while (TRUE)
    {
        zeroBits = (s32)(31 - OsCountZeroBits(workMap));
        if (zeroBits < 0)
        {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        if (OSi_vramLockId[zeroBits] != lockId)
        {
            (void)OS_RestoreInterrupts(enabled);
            return FALSE;
        }
    }

    // Lock all VRAM with specified ID
    workMap = (u32)(bank & OS_VRAM_BANK_ID_ALL);
    while (TRUE)
    {
        zeroBits = (s32)(31 - OsCountZeroBits(workMap));
        if (zeroBits < 0)
        {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        OSi_vramLockId[zeroBits] = lockId;
        OSi_vramExclusive |= (0x00000001 << zeroBits);
    }

    (void)OS_RestoreInterrupts(enabled);
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         OSi_UnlockVram

  Description:  Releases VRAM exclusive lock.

  Arguments:    bank    -  ID bitmap of VRAM on which exclusive lock is being released.
                                lockId  -  Arbitrary ID specified when locked.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void OSi_UnlockVram(u16 bank, u16 lockId)
{
    u32     workMap;
    s32     zeroBits;
    OSIntrMode enabled = OS_DisableInterrupts();

    workMap = (u32)(bank & OSi_vramExclusive & OS_VRAM_BANK_ID_ALL);
    while (TRUE)
    {
        zeroBits = (s32)(31 - OsCountZeroBits((u32)workMap));
        if (zeroBits < 0)
        {
            break;
        }
        workMap &= ~(0x00000001 << zeroBits);
        if (OSi_vramLockId[zeroBits] == lockId)
        {
            OSi_vramLockId[zeroBits] = 0;
            OSi_vramExclusive &= ~(0x00000001 << zeroBits);
        }
    }

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
