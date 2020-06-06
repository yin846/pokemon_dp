/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - spi
  File:     spi.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: spi.h,v $
  Revision 1.18  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.17  02/28/2005 05:26:27  yosizaki
  do-indent.

  Revision 1.16  12/29/2004 02:05:04  takano_makoto
  Removed retry parameter in SetStability function

  Revision 1.15  10/07/2004 06:49:34  terui
  Made correction due to the addition of commands for the NVRAM made by a different manufacturer.

  Revision 1.14  07/31/2004 08:08:46  terui
  Removed unnecessary codes

  Revision 1.13  06/01/2004 04:45:59  terui
  Deleted buf specification from inline functions for stand-alone mic sampling    

  Revision 1.12  05/31/2004 08:54:50  terui
  Moved non-inline functions to type.h in global include    

  Revision 1.11  05/28/2004 08:51:44  terui
  Removed definitions to header files under nitro/include    
  Plan to delete file if inline functions can be made local functions on ARM9 side

  Revision 1.10  05/25/2004 00:58:48  terui
  Fixes accompanying segmentation of library for each SPI device

  Revision 1.8  05/12/2004 05:28:24  terui
  Change SPI_Init declaration

  Revision 1.7  04/29/2004 10:28:43  terui
  Added type to consider exclusive access from ARM7 to device type    

  Revision 1.6  04/27/2004 12:05:59  terui
  Added mic sampling latest save address obtain I/F     

  Revision 1.5  04/23/2004 02:27:35  takano_makoto
  Change Default Stability Range

  Revision 1.4  04/15/2004 13:55:41  takano_makoto
  Chang default stability parameter

  Revision 1.3  04/14/2004 06:26:46  terui
  Updates accompanying SPI library source arrangement    

  Revision 1.2  04/09/2004 02:59:15  terui
  Correct misspell.

  Revision 1.1  04/05/2004 04:46:32  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef LIBRARIES_SPI_H_
#define LIBRARIES_SPI_H_

#include    <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
    Inline function definitions
    Temporarily define for ARM9
    Should be defined in ARM9 library as static function
 *---------------------------------------------------------------------------*/
#ifdef  SDK_ARM9

/*---------------------------------------------------------------------------*
  Name:         SPI_TpSamplingNow

  Description:  Sample touch panel once.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_TpSamplingNow(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_TP_SAMPLING << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_TpAutoSamplingOn

  Description:  Start auto-sampling of touch panel.

  Arguments:    vCount -    VCount that carries out sampling.
                            If sampling is done multiple times in 1 frame the
                            frame is time divided using this as the origin.
                frequency - Frequency of sampling for 1 frame

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_TpAutoSamplingOn(u16 vCount, u8 frequency)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_TP_AUTO_ON << 8) | (u32)frequency, 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_END_BIT | (1 << SPI_PXI_INDEX_SHIFT) | (u32)vCount, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_TpAutoSamplingOff

  Description:  Stop auto-sampling of touch panel.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_TpAutoSamplingOff(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_TP_AUTO_OFF << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_TpSetupStability

  Description:  Setup stability determination parameters for sampling

  Arguments:    range - For continuous sampling, error for which detected voltage is considered stable.    
                        The detection value is 12 bits, 0 - 4095.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_TpSetupStability(u16 range)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_TP_SETUP_STABILITY << 8) | (u32)range, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramWriteEnable

  Description:  Issue "write permitted" instruction to NVRAM.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramWriteEnable(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_WREN << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramWriteDisable

  Description:  Issue "write prohibited" instruction to NVRAM.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramWriteDisable(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_WRDI << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReadStatusRegister

  Description:  Issues command "status register read" in NVRAM.    

  Arguments:    pData - Pointer to variable storing read value.
                        The ARM7 directly writes the value so be careful of the cache.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReadStatusRegister(u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_RDSR << 8) | ((u32)pData >> 24), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (((u32)pData >> 8) & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (2 << SPI_PXI_INDEX_SHIFT) | (((u32)pData << 8) & 0x0000ff00), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReadDataBytes

  Description:  Issues 'read' command in NVRAM.    

  Arguments:    address - Read start address on NVRAM. Only 24 bits valid.
                size -    Byte size to consecutively read.
                pData -   Array storing read value.
                          The ARM7 directly writes the value so be careful of the cache.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReadDataBytes(u32 address, u32 size, u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_READ << 8) |
                               ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [5]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (5 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReadHigherSpeed

  Description:  Issues 'high speed read' command to NVRAM.
                'High speed read' is a command where, after issuing command, it is possible to read
                with clock up to 25 MHz by ticking down a dummy 8-bit clock before reading data.
                However, the SPI clock maximum is up to 4 MHz so there using it is meaningless.
                The normal 'read' command supports clocks up to a maximum of 20 MHz.
                The above is for when the device is M45PE40. When the device is LE25FW203T,    
                Both 'read' and 'high speed read' are 30 MHz.    

  Arguments:    address - Read start address on NVRAM. Only 24 bits valid.
                size -    Byte size to consecutively read.
                pData -   Array storing read value.
                          The ARM7 directly writes the value so be careful of the cache.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReadHigherSpeed(u32 address, u32 size, u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_FAST_READ << 8) |
                               ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [5]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (5 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramPageWrite

  Description:  Issues 'page write' command to NVRAM.
                Inside NVRAM, "Delete Page" and "Load Page (Conditionally)" are
                performed in series.

  Arguments:    address - Write start address on NVRAM. Only 24 bits valid.
                size -    Consecutively written byte size.
                          When address + size exceeds page boundary (256 bytes), the excessive data
                          is ignored.
                pData -   Array where written value is stored.
                          ARM7 reads directly, so you must be sure to write from cache to memory    
                          beforehand.    

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramPageWrite(u32 address, u16 size, const u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_PW << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (u32)size, 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramPageWrite

  Description:  Issues "page write (conditional)" command to NVRAM.
                Writes overwhelmingly faster than normal "page write", but the only operation you
                can do is lower the bit.   

  Arguments:    address - Write start address on NVRAM. Only 24 bits valid.
                size -    Consecutively written byte size.
                          When address + size exceeds page boundary (256 bytes), the excessive data
                          is ignored.
                pData -   Array where written value is stored.
                          ARM7 reads directly, so you must be sure to write from cache to memory    
                          beforehand.    

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramPageProgram(u32 address, u16 size, const u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_PP << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM, (2 << SPI_PXI_INDEX_SHIFT) | (u32)size, 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (3 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (4 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramPageErase

  Description:  Issue "page erase" instruction to NVRAM.
                All bits for erased page are all '1'.

  Arguments:    address - Address on NVRAM of page erased. Only 24 bits valid.
                          All of the 256 bytes of the page included in this address are erased.
                          In the NVRAM the address of the lower 8 bits    
                          that were erased become the target address.    

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramPageErase(u32 address)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_PE << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramSectorErase

  Description:  Issue "sector erase" instruction to NVRAM.
                All bits for erased sector are all '1'.

  Arguments:    address - Address on NVRAM of sector erased. Only 24 bits valid.
                          All of the 64 bytes of the sector included in this address are erased.
                          In the NVRAM the address of the lower 16 bits    
                          that were erased become the target address.    

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramSectorErase(u32 address)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_SE << 8) | ((address >> 16) & 0x000000ff), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (1 << SPI_PXI_INDEX_SHIFT) | (address & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramPowerDown

  Description:  Issue "power save" instruction to NVRAM.
                Will not receive instruction other than "return from power save".

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramPowerDown(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_DP << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReleasePowerDown

  Description:  Issue "return from power save" instruction to NVRAM.
                Ignored if not in power save mode.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReleasePowerDown(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_RDP << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramChipErase

  Description:  Issues "chip erase" command to NVRAM.
                Effective when device is LE25FW203T.
                Ignored when device is M45PE40.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramChipErase(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_CE << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramReadSiliconId

  Description:  Issues "read silicon ID" command to NVRAM.
                Effective when device is LE25FW203T.
                Ignored when device is M45PE40.

  Arguments:    pData - Specifies a 2-byte buffer for storing the silicon ID that has been read.
                        of the ESS-ID. The ARM7 directly writes the value so be careful of the cache.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                        Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramReadSiliconId(u8 *pData)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_NVRAM_RSI << 8) | ((u32)pData >> 24), 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               (1 << SPI_PXI_INDEX_SHIFT) | (((u32)pData >> 8) & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_END_BIT |
                               (2 << SPI_PXI_INDEX_SHIFT) | (((u32)pData << 8) & 0x0000ff00), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_NvramSoftwareReset

  Description:  Issues "software reset" command to NVRAM.
                Effective when device is LE25FW203T.
                Ignored when device is M45PE40.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_NvramSoftwareReset(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_NVRAM,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_NVRAM_SR << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_MicSamplingNow

  Description:  Samples the mic once.

  Arguments:    type  - sampling type ( 0: 8 bits , 1: 12 bits )
                pData - memory address that stores sampling results
                   -> Delete

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_MicSamplingNow(u8 type)
{
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_SAMPLING << 8) | (u32)type, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_MicAutoSamplingOn

  Description:  Starts auto-sampling with the mic.

  Arguments:    pData    - buffer address that stores sampled data
                size  - Buffer size. Specified in byte units.
                span     - sampling interval (specify with ARM7 CPU clock)
                           Due to timer characteristics, only numbers of 16 bits    
                           x 1or64or256or1024 can be accurately set up.     Fractional bits are rounded off.
                middle   - Sampling times that returns response during process
                           -> Delete
                adMode   - specify AD conversion bit ( 8 or 12 )
                loopMode - auto-sampling loop control determination
                correct  - Tick correction determination during auto-sampling

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_MicAutoSamplingOn(void *pData, u32 size, u32 span,
//    u32     middle ,
                                         u8 adMode, BOOL loopMode, BOOL correct)
{
    u8      temp;

    // Combine flag types in "type" variable
    switch (adMode)                    // AD conversion bit width specification
    {
    case SPI_MIC_SAMPLING_TYPE_8BIT:
    case SPI_MIC_SAMPLING_TYPE_12BIT:
        temp = adMode;
        break;
    default:
        return FALSE;
    }
    if (loopMode)                      // Continuous sampling loop specification
    {
        temp |= (u8)SPI_MIC_SAMPLING_TYPE_LOOP_ON;
    }
    if (correct)                       // apply correction logic specification due to Tick error    
    {
        temp |= (u8)SPI_MIC_SAMPLING_TYPE_CORRECT_ON;
    }

    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_AUTO_ON << 8) | (u32)temp, 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (1 << SPI_PXI_INDEX_SHIFT) | ((u32)pData >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (2 << SPI_PXI_INDEX_SHIFT) | ((u32)pData & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (3 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (4 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // Send packet [5]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (5 << SPI_PXI_INDEX_SHIFT) | (span >> 16), 0))
    {
        return FALSE;
    }

    // Send packet [6]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_END_BIT |
                               (6 << SPI_PXI_INDEX_SHIFT) | (span & 0x0000ffff), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         SPI_MicAutoSamplingOff

  Description:  Stops auto-sampling with the mic.
                Stops automatically when buffer filled if loop specification
                not done during start.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL SPI_MicAutoSamplingOff(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_MIC_AUTO_OFF << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

#endif /* SDK_ARM9*/

/*===========================================================================*/

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* LIBRARIES_SPI_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
