/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_rom.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: card_rom.c,v $
  Revision 1.36  04/24/2006 05:51:53  yosizaki
  fix about CARDi_TryReadCardDma.

  Revision 1.35  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.34  11/30/2005 23:51:57  yosizaki
  fix warning.

  Revision 1.33  11/29/2005 01:20:29  yosizaki
  fix comments.

  Revision 1.32  11/09/2005 07:39:13  adachi_hiroaki
  Invalidates ROM read on the ARM7 side

  Revision 1.31  11/07/2005 01:10:01  okubata_ryoma
  Switched from SDK_ASSERT_ON_COMPILE to SDK_COMPILER_ASSERT

  Revision 1.30  06/27/2005 23:51:25  yosizaki
  small fix. (remove 'const')

  Revision 1.29  06/27/2005 00:26:48  yosizaki
  fix CARDi_TryReadCardDma().

  Revision 1.28  05/12/2005 11:19:42  yosizaki
  optimize some functions and calling interfaces.
  add comment.

  Revision 1.27  04/28/2005 08:12:38  yosizaki
  add assertions about DMA-0 limitation.

  Revision 1.26  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.25  02/16/2005 01:00:31  yosizaki
  fix around CARD-DMA.

  Revision 1.24  02/15/2005 09:41:38  yosizaki
  delete unnecessary flush-command.

  Revision 1.23  01/24/2005 06:35:13  yosizaki
  Revised copyright year notation

  Revision 1.22  01/19/2005 12:18:00  yosizaki
  delete pragma `pack'.

  Revision 1.21  12/15/2004 09:44:45  yasu
  Added CARD access enabler mechanism

  Revision 1.20  12/08/2004 08:53:49  yosizaki
  delete CARDi_ReadRomID()

  Revision 1.19  11/22/2004 07:38:29  yosizaki
  add cardi_rom_base.

  Revision 1.18  11/12/2004 05:45:10  yosizaki
  add CARDi_CheckValidity().

  Revision 1.17  10/18/2004 00:22:19  yosizaki
  change latency1 of READ-ID.

  Revision 1.16  10/08/2004 11:58:20  yosizaki
  move CARD-DMA setting to mi/mi_dma_card.c.
  delete unused functions.

  Revision 1.15  10/04/2004 04:39:13  yada
  avoid to call CARD_InitPulledOutCallback() twice
  even if CARD_Init() and FS_Init() are called.

  Revision 1.14  09/27/2004 13:45:08  yada
  consider for SMALL_BUILD

  Revision 1.13  09/15/2004 05:54:30  yada
  add card pulled off callback setting

  Revision 1.12  09/02/2004 11:54:42  yosizaki
  fix CARD include directives.

  Revision 1.11  08/31/2004 10:59:28  yosizaki
  add errorcode CARD_RESULT_SUCCESS in some functions.

  Revision 1.10  08/30/2004 04:33:22  yada
  remove MI_XXX definitions because those are already defined in dma.h

  Revision 1.9  08/23/2004 10:38:10  yosizaki
  add write-command.
  add retry count on writing timeout.

  Revision 1.8  07/28/2004 13:18:06  yosizaki
  asynchronous mode support.

  Revision 1.7  07/23/2004 08:29:06  yosizaki
  fix CARD_ReadRomID.

  Revision 1.6  07/19/2004 13:15:15  yosizaki
  move thread to card_common.c

  Revision 1.5  07/13/2004 07:59:25  yosizaki
  fix CARD-asynchronous operation.

  Revision 1.4  07/10/2004 10:19:50  yosizaki
  add card_common.c

  Revision 1.3  07/09/2004 08:40:27  yosizaki
  change implement of synchronous mode.

  Revision 1.2  07/08/2004 13:43:56  yosizaki
  change ROM-access (support all case)

  Revision 1.1  06/28/2004 01:52:48  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>


#include "../include/card_common.h"
#include "../include/card_rom.h"


/*****************************************************************************/
/* variable*/

CARDRomStat rom_stat ATTRIBUTE_ALIGN(32);

/* (Normally 0)*/
extern u32 cardi_rom_base;
u32     cardi_rom_base;

SDK_COMPILER_ASSERT(sizeof(rom_stat) % 32 == 0);


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         CARDi_OnReadPageDirect

  Description:  Callback after distributing 1 page directly to the receiving party

  Arguments:    arg        CARDRomStat structure

  Returns:      If distribution is still necessary: TRUE
 *---------------------------------------------------------------------------*/
static inline BOOL CARDi_OnReadPageDirect(CARDRomStat * arg)
{
    CARDiCommon *p = &cardi_common;
    (void)arg;
    p->src += CARD_ROM_PAGE_SIZE;
    p->dst += CARD_ROM_PAGE_SIZE;
    p->len -= CARD_ROM_PAGE_SIZE;
    return (p->len > 0);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadFromCache

  Description:  If possible, ROM data is distributed from a recent read cache

  Arguments:    p          CARDRomStat structure

  Returns:      If distribution is still necessary: TRUE
 *---------------------------------------------------------------------------*/
static BOOL CARDi_ReadFromCache(CARDRomStat * p)
{
    CARDiCommon *c = &cardi_common;
    const u32 cur_page = CARD_ALIGN_HI_BIT(c->src);
    if (cur_page == (u32)p->cache_page)
    {
        const u32 mod = c->src - cur_page;
        u32     len = CARD_ROM_PAGE_SIZE - mod;
        if (len > c->len)
            len = c->len;
        MI_CpuCopy8(p->cache_buf + mod, (void *)c->dst, len);
        c->src += len;
        c->dst += len;
        c->len -= len;
    }
    return (c->len > 0);
}


#if defined(SDK_TS) || defined(SDK_ARM7)


/*---------------------------------------------------------------------------*
  Name:         CARDi_SetRomOp

  Description:  Set the Card command

  Arguments:    cmd1       command word 1
                cmd2       command word 2

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_SetRomOp(u32 cmd1, u32 cmd2)
{
    /* Wait for CARD ROM completion (just to be safe)*/
    while ((*(vu32 *)REG_CARDCNT & CARD_START) != 0) ;
    /* Master enable*/
    *(vu8 *)REG_CARD_MASTER_CNT = CARDMST_SEL_ROM | CARDMST_ENABLE | CARDMST_IF_ENABLE;
    {                                  /* Command settings*/
        vu8    *const p_cmd = (vu8 *)REG_CARD_CMD;
        p_cmd[0] = (u8)(cmd1 >> (8 * 3));
        p_cmd[1] = (u8)(cmd1 >> (8 * 2));
        p_cmd[2] = (u8)(cmd1 >> (8 * 1));
        p_cmd[3] = (u8)(cmd1 >> (8 * 0));
        p_cmd[4] = (u8)(cmd2 >> (8 * 3));
        p_cmd[5] = (u8)(cmd2 >> (8 * 2));
        p_cmd[6] = (u8)(cmd2 >> (8 * 1));
        p_cmd[7] = (u8)(cmd2 >> (8 * 0));
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetRomOpReadPage1

  Description:  One-page read command

  Arguments:    src        ROM offset from the read source

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_SetRomOpReadPage1(u32 src)
{
    CARDi_SetRomOp((u32)(MROMOP_G_READ_PAGE | (src >> 8)), (u32)(src << 24));
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_SetCardDma

  Description:  Start one page worth of CARD-DMA

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_SetCardDma(void)
{
    CARDiCommon *const c = &cardi_common;
    CARDRomStat *const p = &rom_stat;
    /* First, set the CARD-DMA settings*/
    MIi_CardDmaCopy32(c->dma, (const void *)REG_CARD_DATA, (void *)c->dst, CARD_ROM_PAGE_SIZE);
    /* Next, set the CARD-ROM settings*/
    CARDi_SetRomOpReadPage1(c->src);
    *(vu32 *)REG_CARDCNT = p->ctrl;
    /* From here, processes run asynchronously*/
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_OnReadCard

  Description:  CARD-DMA completion handler

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_OnReadCard(void)
{
    CARDRomStat *const p = &rom_stat;
    CARDiCommon *const c = &cardi_common;
    MI_StopDma(c->dma);
    if (!CARDi_OnReadPageDirect(p))
    {
        (void)OS_DisableIrqMask(OS_IE_CARD_DATA);
        (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
        CARDi_ReadEnd();
    }
    else
    {
        CARDi_SetCardDma();
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_TryReadCardDma

  Description:  Set CARD-DMA settings if possible and return TRUE

  Arguments:    p          CARDRomStat structure

  Returns:      If the CARD-DMA is actually started, TRUE
 *---------------------------------------------------------------------------*/
BOOL CARDi_TryReadCardDma(CARDRomStat * p)
#if defined(SDK_TS)
{
    CARDiCommon *const c = &cardi_common;
    const u32 dst = c->dst;
    u32     len = c->len;
    /* If all the following conditions are met, the next specified range is capable of DMA transfer*/
    const BOOL is_async = !(dst & 31) &&    /* Transfer destination boundary alignment (32)*/
        (c->dma <= MI_DMA_MAX_NUM) &&       /* Valid DMA channel*/
        !CARDi_IsInTcm(dst, len) &&         /* Transfer destination is not TCM area*/
        !CARD_ALIGN_LO_BIT(c->src | len) && /* Transfer source and size boundary alignment (512)*/
        (len > 0);
    p->ctrl = CARDi_GetRomFlag(CARD_COMMAND_PAGE);
    if (is_async)
    {
        OSIntrMode bak_psr = OS_DisableInterrupts();
#if defined(SDK_ARM9)
        IC_InvalidateRange((void *)dst, len);
        /*
         * Even with a large capacity buffer that is not boundary-aligned,
         * to guarantee efficiency and safety,
         * limit the alignment boundary range only to invalidate,
         * and clean/invalidate the fractional portion before and after the buffer.
         */
        {
            u32     pos = dst;
            u32     mod = (dst & (HW_CACHE_LINE_SIZE - 1));
            if (mod)
            {
                pos -= mod;
                DC_StoreRange((void *)(pos), HW_CACHE_LINE_SIZE);
                DC_StoreRange((void *)(pos + len), HW_CACHE_LINE_SIZE);
                len += HW_CACHE_LINE_SIZE;
            }
            DC_InvalidateRange((void *)pos, len);
            DC_WaitWriteBufferEmpty();
        }
#endif
        (void)OS_SetIrqFunction(OS_IE_CARD_DATA, CARDi_OnReadCard);
        (void)OS_ResetRequestIrqMask(OS_IE_CARD_DATA);
        (void)OS_EnableIrqMask(OS_IE_CARD_DATA);
        (void)OS_RestoreInterrupts(bak_psr);
        CARDi_SetCardDma();
    }
    return is_async;
}
#else
{
    (void)p;
    return FALSE;
}
#endif /* defined(SDK_TS)*/


/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadCard

  Description:  Directly executes the card access based on the settings.

  Arguments:    p          Structure in which the access content is set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadCard(CARDRomStat * p)
{
#if defined(SDK_ARM9) \
    || (defined(SDK_ARM7) && defined(SDK_ARM7_READROM_SUPPORT))

    /*
     * Synchronous transfer with CPU (and DMA, if possible).
     *  If aligned to 4 bytes
     *  If misaligned by 1 bytes
     *  If misaligned by 2 bytes
     *  If misaligned by 3 bytes
     * Speed up as much as possible using misalignment correction transfer for each case.
     * For now, move to temporary memory, and rely on MI_CpuCopy8.
     *
     * If the card's busy time per word is long enough,
     * performance will not be affected even if only the last page was made to be always loaded into
     * temporary memory.
     */
    CARDiCommon *const c = &cardi_common;
    for (;;)
    {
        const u32 len = CARD_ROM_PAGE_SIZE;
        u32     src = CARD_ALIGN_HI_BIT(c->src);
        u32     dst;
        /* Determine whether need to go via cache*/
        if ((src != c->src) || ((c->dst & 3) != 0) || (c->len < len))
        {
            dst = (u32)p->cache_buf;
            p->cache_page = (void *)src;
        }
        else
        {
            dst = c->dst;
        }
        /* Perform this time's settings*/
        CARDi_SetRomOpReadPage1(src);
        {                              /* CPU transfer (repeated loop)*/
            u32     pos = 0;
            *(vu32 *)REG_CARDCNT = p->ctrl;
            for (;;)
            {
                const u32 ctrl = *(vu32 *)REG_CARDCNT;
                if ((ctrl & CARD_DATA_READY) != 0)
                {
                    /* Store as much as the specified size*/
                    u32     data = *(vu32 *)REG_CARD_DATA;
                    if (pos < len)
                        ((u32 *)dst)[pos++] = data;
                }
                if (!(ctrl & CARD_START))
                    break;
            }
        }
        /* Proceed to this time's transfer end part*/
        if (dst == c->dst)
        {
            if (!CARDi_OnReadPageDirect(p))
                break;
        }
        else
        {
            if (!CARDi_ReadFromCache(p))
                break;
        }
    }
#else
#pragma unused(p)
#endif
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomIDCore

  Description:  Read the Card ID

  Arguments:    None.

  Returns:      Card ID
 *---------------------------------------------------------------------------*/
u32 CARDi_ReadRomIDCore(void)
{
    CARDi_SetRomOp(MROMOP_G_READ_ID, 0);
    *(vu32 *)REG_CARDCNT = (u32)(CARDi_GetRomFlag(CARD_COMMAND_ID) & ~CARD_LATENCY1_MASK);
    while (!(*(vu32 *)REG_CARDCNT & CARD_DATA_READY))
    {
    }
    return *(vu32 *)REG_CARD_DATA;
}

#endif /* defined(SDK_TS) || defined(SDK_ARM7)*/


/********************************************************************/


#if defined(SDK_ARM7) && defined(SDK_ARM7_READROM_SUPPORT)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomCore

  Description:  Card transfer resulting from request.
                Always process synchronously.

  Arguments:    src        distribution source
                dst        distribution destination
                len        Transfer size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadRomCore(const void *src, void *dst, u32 len)
{
    CARDRomStat *const p = &rom_stat;
    CARDiCommon *const c = &cardi_common;

    /* Set this time's transfer parameters*/
    c->dma = (MI_DMA_MAX_NUM + 1);
    c->src = (u32)src;
    c->dst = (u32)dst;
    c->len = (u32)len;
    c->callback = NULL;
    c->callback_arg = NULL;
    p->ctrl = CARDi_GetRomFlag(CARD_COMMAND_PAGE);
    cardi_common.cur_th = OS_GetCurrentThread();
    CARDi_ReadCard(p);
}

#endif

#if defined(SDK_TS)
/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomID

  Description:  Read the Card ID

  Arguments:    None.

  Returns:      Card ID
 *---------------------------------------------------------------------------*/
u32 CARDi_ReadRomID(void)
{
    CARDRomStat *const p = &rom_stat;
    CARDiCommon *const c = &cardi_common;

    u32     ret = 0;

    SDK_ASSERT(CARD_IsAvailable());
    SDK_ASSERTMSG(CARDi_GetTargetMode() == CARD_TARGET_ROM, "must be locked by CARD_LockRom()");

    CARDi_WaitTask(c, NULL, NULL);

    /* Direct access is possible, so execute here*/
    ret = CARDi_ReadRomIDCore();
    CARDi_ReadEnd();
    return ret;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomSyncCore

  Description:  Common process for synchronous Card reads

  Arguments:    c          CARDiCommon structure

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_ReadRomSyncCore(CARDiCommon * c)
{
    CARDRomStat *const p = &rom_stat;
    (void)c;
    /*
     * If it does not work with the cache accessed previously,
     * this time, perform actual ROM access (CARD / CARTRIDGE / PXI)
     */
    if (CARDi_ReadFromCache(p))
    {
        (*p->read_func) (p);
    }
    CARDi_ReadEnd();
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRom

  Description:  basic function of ROM read

  Arguments:    dma        DMA channel to use
                src        Transfer source offset
                dst        Transfer destination memory address.
                len        Transfer size
                callback   Completion callback (NULL if not used)
                arg        Argument of completion callback (ignored if not used)
                is_async   If set to asynchronous mode: TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARDi_ReadRom(u32 dma,
                   const void *src, void *dst, u32 len,
                   MIDmaCallback callback, void *arg, BOOL is_async)
{
    CARDRomStat *const p = &rom_stat;
    CARDiCommon *const c = &cardi_common;

    SDK_ASSERT(CARD_IsAvailable());    /* Determine whether CARD_Init has been called*/
    SDK_ASSERT(CARDi_GetTargetMode() == CARD_TARGET_ROM);
    SDK_ASSERTMSG((dma != 0), "cannot specify DMA channel 0");

    /* Validity check of CARD access
       If access is prohibited, use OS_Panic in the function*/
    CARD_CheckEnabled();

    /* Exclusive wait on ARM9 side*/
    CARDi_WaitTask(c, callback, arg);

    /* Set this time's transfer parameters*/
    c->dma = dma;
    c->src = (u32)((u32)src + cardi_rom_base);
    c->dst = (u32)dst;
    c->len = (u32)len;
    if (dma <= MI_DMA_MAX_NUM)
        MI_StopDma(dma);

    if (CARDi_TryReadCardDma(p))
    {
        if (!is_async)
            CARD_WaitRomAsync();
    }
    else if (is_async)
    {
        CARDi_SetTask(CARDi_ReadRomSyncCore);
    }
    else
    {
        c->cur_th = OS_GetCurrentThread();
        CARDi_ReadRomSyncCore(c);
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_Init

  Description:  initialize CARD library.
                this function is called in OS_Init().

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void CARD_Init(void)
{
    CARDiCommon *const p = &cardi_common;

    if (!p->flag)
    {
        p->flag = CARD_STAT_INIT;
        p->src = p->dst = p->len = 0;
        p->dma = (u32)~0;
        p->callback = NULL;
        p->callback_arg = NULL;

        cardi_rom_base = 0;

        CARDi_InitCommon();

        rom_stat.read_func = CARDi_GetRomAccessor();

#if !defined(SDK_SMALL_BUILD)
        //---- for detect pulled out card
        CARD_InitPulledOutCallback();
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         CARD_WaitRomAsync

  Description:  wait for completion of current asynchronous ROM operation.

  Arguments:    None.

  Returns:      if the newest operation is successful, TRUE.
 *---------------------------------------------------------------------------*/
void CARD_WaitRomAsync(void)
{
    (void)CARDi_WaitAsync();
}

/*---------------------------------------------------------------------------*
  Name:         CARD_TryWaitRomAsync

  Description:  check if asynchronous ROM operation has been completed.

  Arguments:    None.

  Returns:      if operation has been completed, TRUE.
 *---------------------------------------------------------------------------*/
BOOL CARD_TryWaitRomAsync(void)
{
    return CARDi_TryWaitAsync();
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetRomAccessor

  Description:  Returns a pointer to a card access function that will vary according to the operating environment.

  Arguments:    None.

  Returns:      In this implementation, which is a TS version, always returns CARDi_ReadCard().
 *---------------------------------------------------------------------------*/
void    (*CARDi_GetRomAccessor(void)) (CARDRomStat *)
{
    return CARDi_ReadCard;
}
