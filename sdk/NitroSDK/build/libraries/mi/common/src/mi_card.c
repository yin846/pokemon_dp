/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_card.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_card.c,v $
  Revision 1.16  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.15  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.14  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.13  07/27/2004 07:21:21  yosizaki
  small fix (warning "padding inserted")

  Revision 1.12  2004/07/24 08:24:39  yasu
  Change type of lock ID to u16.

  Revision 1.11  07/19/2004 13:17:06  yosizaki
  change MI_ReadCard.

  Revision 1.10  07/08/2004 13:44:51  yosizaki
  fix MI_ReadCard (for card/rom.h)

  Revision 1.9  06/28/2004 02:05:14  yosizaki
  change internal implement.
  (SDK recommends using CARD-API instead of this module)

  Revision 1.8  06/22/2004 01:47:41  yosizaki
  change to use OS_GetConsoleType().
  avoid cartridge access on SDK_TS.

  Revision 1.7  06/16/2004 08:57:00  yosizaki
  fix MIi_ReadCardCore.

  Revision 1.6  06/11/2004 08:57:23  yosizaki
  fix MI_ReadCardID. (add Lock/Unlock)

  Revision 1.5  06/07/2004 10:25:41  yosizaki
  fix around CARD-DMA handler.

  Revision 1.4  06/03/2004 13:32:07  yosizaki
  add MI_ReadCardID.
  fix around async-mode.

  Revision 1.3  05/21/2004 06:05:52  yosizaki
  change OS_DisableIrq to OS_DisableInterrupt.

  Revision 1.2  05/10/2004 06:23:50  yosizaki
  add MIi_ReadCardAsync() callback param.

  Revision 1.1  04/26/2004 11:05:14  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro/os.h>
#include <nitro/mi/card.h>


/********************************************************************/
/* Variables*/

// Variables for card's exclusive processing.
typedef struct
{
    BOOL    is_init;                   // Initialized.
    u16     lock_id;                   //Lock ID.
    u16     padding;
    MIDmaCallback dma_callback;
}
MIi_CardParam;

static MIi_CardParam mii_card_param;


/********************************************************************/
/* functions*/

// Initialize card access via MI.
static void MIi_InitCard(void)
{
    MIi_CardParam *const p = &mii_card_param;
    OSIntrMode bak_psr = OS_DisableInterrupts();

    if (!p->is_init)
    {
        s32     lock_id = OS_GetLockID();

#ifndef	SDK_FINALROM
        if (lock_id < 0)
        {
            OS_Panic("Invalid lock ID.");
        }
#endif
        p->is_init = TRUE;
        p->lock_id = (u16)lock_id;
    }
    (void)OS_RestoreInterrupts(bak_psr);
}

// Allocate device using exclusive control of processor .
void MIi_LockCard(void)
{
    // Initialize here if necessary for convenience.    
    MIi_InitCard();
    CARD_LockRom(mii_card_param.lock_id);
}

// Release the device using exclusive control of processor.
void MIi_UnlockCard(void)
{
    SDK_ASSERT(mii_card_param.is_init);
    CARD_UnlockRom(mii_card_param.lock_id);
}

// Unlock the MI itself before user callback.
static void MIi_OnAsyncEnd(void *arg)
{
    MIi_UnlockCard();
    {
        MIDmaCallback func = mii_card_param.dma_callback;
        mii_card_param.dma_callback = NULL;
        if (func)
            (*func) (arg);
    }
}

// Load the card. (Asynchronous)
void MIi_ReadCardAsync(u32 dmaNo, const void *src, void *dst, u32 size,
                       MIDmaCallback callback, void *arg)
{
    MIi_LockCard();
    mii_card_param.dma_callback = callback;
    (void)CARD_ReadRomAsync(dmaNo, src, dst, size, MIi_OnAsyncEnd, arg);
}
