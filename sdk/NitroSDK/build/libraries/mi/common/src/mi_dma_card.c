/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_dma_card.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_dma_card.c,v $
  Revision 1.6  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.4  01/12/2005 11:10:15  takano_makoto
  fix copyright header.

  Revision 1.3  01/12/2005 02:47:53  yada
  consider about multi-DMA

  Revision 1.2  10/12/2004 02:03:46  yosizaki
  change ~0 to MIi_DMA_TIMING_ANY.

  Revision 1.1  10/11/2004 23:49:01  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "../include/mi_dma.h"


/*---------------------------------------------------------------------------*
  Name:         MIi_CardDmaCopy32

  Description:  CARD DMA copy.
                32bit, sync version.
                (this sets only DMA-control. CARD register must be set after)

  Arguments:    dmaNo : DMA channel No.
                src: source address
                dest  : destination address
                size   : transfer size (byte)

  Returns:      None
 *---------------------------------------------------------------------------*/
void MIi_CardDmaCopy32(u32 dmaNo, const void *src, void *dest, u32 size)
{
    vu32   *dmaCntp;

    MIi_ASSERT_DMANO(dmaNo);
    MIi_ASSERT_DEST_ALIGN4(dest);
    /* Size is not used in this function, but specify for determining size*/
    MIi_ASSERT_SRC_ALIGN512(size);
    MIi_WARNING_ADDRINTCM(dest, size);
    (void)size;

#ifdef SDK_ARM9
    /*
     * CARD DMA differs from other DMA in that there is no  * "do same type in parallel."
     Although there is centralized  * management for the CARD library on the calling side, set
     * MIi_DMA_TIMING_ANY to also check for same types.
     */
    MIi_CheckAnotherAutoDMA(dmaNo, MIi_DMA_TIMING_ANY);
#endif
    //---- check DMA0 source address
    MIi_CheckDma0SourceAddress(dmaNo, (u32)src, size, MI_DMA_SRC_FIX);

    if (size == 0)
    {
        return;
    }

    MIi_Wait_BeforeDMA(dmaCntp, dmaNo);
    MIi_DmaSetParams(dmaNo, (u32)src, (u32)dest,
                     (u32)(MI_CNT_CARDRECV32(4) | MI_DMA_CONTINUOUS_ON));
    /*
     * Here, the automatic startup only turns ON.     
     * It will start up for the first time when commands are configured in the CARD register.
     */
}
