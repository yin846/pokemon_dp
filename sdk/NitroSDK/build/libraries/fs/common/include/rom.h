/*---------------------------------------------------------------------------*
  Project:  NitroSDK - FS - libraries
  File:     rom.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/


#if     !defined(NITRO_FS_ROM_H_)
#define NITRO_FS_ROM_H_

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/fs/file.h>
#include <nitro/fs/archive.h>


/*****************************************************************************/
/* variable*/

/* lock ID of the card/cartridge*/
extern s32 fsi_card_lock_id;

/* Information for attached overlay*/
extern CARDRomRegion fsi_ovt9;
extern CARDRomRegion fsi_ovt7;

#if     defined(FS_IMPLEMENT)
/* ROM archive*/
extern FSArchive fsi_arc_rom;
#endif /* FS_IMPLEMENT*/


/*****************************************************************************/
/* function*/

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
  Name:         FSi_InitRom

  Description:  Initialize "rom" archive

  Arguments:    default_dma_no   default DMA channel for ROM
                                 if out of range(0-3),
                                 use CpuCopy instead of DMA.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    FSi_InitRom(u32 default_dma_no);

/*---------------------------------------------------------------------------*
  Name:         FSi_LoadOverlayInfoCore

  Description:  Internal function.
                Loads overlay information from specified rom-duplicated archive.
                Unannounced release for convenience of IPL package.

  Arguments:    p_ovi            Pointer that stores information that was read.
                target           Overlay target. (ARM9/ARM7)
                id               Overlay ID to read.
                p_arc            Archive to read to.
                offset_arm9      Start of overlay info storage position in archive.
                len_arm9         Size of overlay info in archive.
                offset_arm7      Start of overlay info storage position in archive.
                len_arm7         Size of overlay info in archive.

  Returns:      If overlay is read correctly, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    FSi_LoadOverlayInfoCore(FSOverlayInfo *p_ovi, MIProcessor target, FSOverlayID id,
                                FSArchive *p_arc,
                                u32 offset_arm9, u32 len_arm9, u32 offset_arm7, u32 len_arm7);


#ifdef __cplusplus
} /* extern "C"*/
#endif


/*****************************************************************************/


#endif /* NITRO_FS_ROM_H_*/

/*---------------------------------------------------------------------------*
  $Log: rom.h,v $
  Revision 1.8  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.7  05/30/2005 04:13:56  yosizaki
  add comments.

  Revision 1.6  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.5  2005/02/18 07:45:31  yasu
  Signed/Unsigned conversion warning control

  Revision 1.4  09/04/2004 13:21:04  yosizaki
  add prototype of FSi_LoadOverlayInfoCore.

  Revision 1.3  07/08/2004 13:39:45  yosizaki
  extern rom archive.

  Revision 1.2  05/11/2004 04:27:57  yosizaki
  (none)

  Revision 1.1  05/10/2004 06:28:28  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
