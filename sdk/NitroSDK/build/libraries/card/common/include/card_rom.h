/*---------------------------------------------------------------------------*
  Project:  NitroSDK - CARD - libraries
  File:     card_rom.h

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/

#if !defined(NITRO_LIBRARIES_CARD_ROM_H__)
#define NITRO_LIBRARIES_CARD_ROM_H__


#include <nitro.h>
#include "../include/card_common.h"

#if defined(SDK_TEG)
#define SDK_ARM7_READROM_SUPPORT
#endif

/*****************************************************************************/
/* macro*/


/*****************************************************************************/
/* constant*/

/* Card control register*/
#define REG_CARD_MASTER_CNT (HW_REG_BASE + 0x1A1)
#define REG_CARDCNT         (HW_REG_BASE + 0x1A4)
#define REG_CARD_CMD        (HW_REG_BASE + 0x1A8)
#define REG_CARD_DATA       (HW_REG_BASE + 0x100010)

/* REG_CARD_MASTER_CNT setting bit*/
#define CARDMST_SEL_ROM     0x00
#define CARDMST_IF_ENABLE   0x40
#define CARDMST_ENABLE      0x80

/* Card command format (Hardware does not support anything other than 1 page)*/
#define CARD_DATA_READY     0x00800000
#define CARD_COMMAND_PAGE   0x01000000
#define CARD_COMMAND_ID     0x07000000
#define CARD_COMMAND_MASK   0x07000000
#define CARD_RESET_HI       0x20000000
#define CARD_RESET_LO       0x00000000
#define CARD_ACCESS_MODE    0x40000000
#define CARD_READ_MODE      0x00000000
#define CARD_WRITE_MODE     0x40000000
#define CARD_START          0x80000000
#define CARD_LATENCY1_MASK  0x00001FFF

#define MROMOP_G_READ_ID    0xB8000000
#define MROMOP_G_READ_PAGE  0xB7000000


/*****************************************************************************/
/* declaration*/

/* Structure for managing states common to CARD / CARTRIDGE / PXI*/
typedef struct CARDRomStat
{
    /* Parameters for ROM access*/
    void    (*read_func) (struct CARDRomStat *);
    u32     ctrl;                      /* Unique to CARD: Control flag*/
    u8     *cache_page;                /* The current cache page*/

    /* Wait for PXI reception*/
    u32     dummy[5];
    /* 32 BYTE alignment*/

    /* Cache of page sent last (PXI / CARD)*/
    u8      cache_buf[CARD_ROM_PAGE_SIZE];

}
CARDRomStat;


SDK_COMPILER_ASSERT(sizeof(CARDRomStat) % 32 == 0);


/*****************************************************************************/
/* variable*/

extern CARDRomStat rom_stat;


/*****************************************************************************/
/* function*/

/*---------------------------------------------------------------------------*
  Name:         CARDi_IsInTcm

  Description:  Determines if specified memory area corresponds to TCM range, even partially.

  Arguments:    buf        Starting memory address
                len        Memory size

  Returns:      If it corresponds to TCM range, even partially, TRUE. Otherwise, FALSE.
 *---------------------------------------------------------------------------*/
static inline BOOL CARDi_IsInTcm(u32 buf, u32 len)
{
#if defined(SDK_ARM9)
    const u32 i = OS_GetITCMAddress();
    const u32 d = OS_GetDTCMAddress();
    return
        ((i < buf + len) && (i + HW_ITCM_SIZE > buf)) ||
        ((d < buf + len) && (d + HW_DTCM_SIZE > buf));
#else
#pragma unused(buf)
#pragma unused(len)
    return FALSE;
#endif
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_GetRomFlag

  Description:  Gets card command control parameter.

  Arguments:    flag       Command type to issue to card device
                           (CARD_COMMAND_PAGE or CARD_COMMAND_ID)

  Returns:      Card command control parameter.
 *---------------------------------------------------------------------------*/
static inline u32 CARDi_GetRomFlag(u32 flag)
{
    const u32 rom_ctrl = *(vu32 *)(HW_ROM_HEADER_BUF + 0x60);
    return (u32)((rom_ctrl & ~CARD_COMMAND_MASK) | flag |
                 CARD_READ_MODE | CARD_START | CARD_RESET_HI);
}

/*---------------------------------------------------------------------------*
  Name:         CARDi_CheckPulledOutCore

  Description:  Main processing of the function for detecting a pulled out card.
                The card bus must be locked.

  Arguments:    id            ROM-ID read from the card

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_CheckPulledOutCore(u32 id);


#if	defined(SDK_TS) || defined(SDK_ARM7)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomIDCore

  Description:  Read the Card ID

  Arguments:    None.

  Returns:      Card ID
 *---------------------------------------------------------------------------*/
u32     CARDi_ReadRomIDCore(void);

#endif

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadEnd

  Description:  Process ending ROM access

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void CARDi_ReadEnd(void)
{
    CARDiCommon *const p = &cardi_common;
#ifdef SDK_ARM9
    /* Check here for a pulled out card. (Prior exclusive processing is not required.)*/
    CARDi_CheckPulledOutCore(CARDi_ReadRomIDCore());
#endif
    p->cmd->result = CARD_RESULT_SUCCESS;
    CARDi_EndTask(p, TRUE);
}


#if	defined(SDK_ARM7)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomCore

  Description:  Accesses the card from the ARM7.

  Arguments:    src        Transfer source offset
                src        Transfer source memory address
                src        Transfer size

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadRomCore(const void *src, void *dst, u32 len);

#endif


/*****************************************************************************
 * Section dependent on the environment
 *
 *   Card access in the TEG environment is implemented under complex conditions.
 *    Firstly, if the execution environment is not CARD, it uses CARTRIDGE instead.
 *   Even if it supports CARD, the TEG specific specifications allow access only the ARM7,
 *   therefore, to access from the ARM9, send the request to the ARM7 via the PXI.
 *
 *****************************************************************************/


/*---------------------------------------------------------------------------*
  Name:         CARDi_GetRomAccessor

  Description:  Returns a pointer to a card access function that will vary according to the operating environment.

  Arguments:    None.

  Returns:      One of the following:
                CARDi_ReadCard(), CARDi_ReadPxi(), CARDi_ReadCartridge().
 *---------------------------------------------------------------------------*/
void    (*CARDi_GetRomAccessor(void)) (CARDRomStat *);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadCard

  Description:  Directly executes the card access based on the settings.

  Arguments:    p          Structure in which the access content is set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadCard(CARDRomStat * p);

/*---------------------------------------------------------------------------*
  Name:         CARDi_TryReadCardDma

  Description:  If the set contents meet the DMA card access requirements,
                starts asynchronous DMA.

  Arguments:    p          Structure in which the access content is set.

  Returns:      If async DMA starts, TRUE. Otherwise, FALSE.
 *---------------------------------------------------------------------------*/
BOOL    CARDi_TryReadCardDma(CARDRomStat * p);


#if defined(SDK_TEG)

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadCartridge

  Description:  Accesses the cartridge based on the set content.

  Arguments:    p          Structure in which the access content is set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadCartridge(CARDRomStat * p);

/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadPxi

  Description:  Accesses the card via PXI based on the set content

  Arguments:    p          Structure in which the access content is set.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    CARDi_ReadPxi(CARDRomStat * p);

#endif /* defined(SDK_TEG)*/


#endif /* NITRO_LIBRARIES_CARD_ROM_H__*/


/*---------------------------------------------------------------------------*
  $Log: card_rom.h,v $
  Revision 1.6  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  11/09/2005 07:38:50  adachi_hiroaki
  Invalidates ROM read on the ARM7 side

  Revision 1.4  11/07/2005 01:09:47  okubata_ryoma
  Switched from SDK_ASSERT_ON_COMPILE to SDK_COMPILER_ASSERT

  Revision 1.3  10/25/2005 01:14:19  yosizaki
  small fix for ARM7.

  Revision 1.2  10/24/2005 10:49:59  yosizaki
  add pulledout-check in CARDi_ReadEnd().

  Revision 1.1  05/12/2005 11:14:59  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
