/*---------------------------------------------------------------------------*
  Project:  NitroSDK - include - HW
  File:     memorymap.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: memorymap.h,v $
  Revision 1.10  06/23/2005 12:16:39  yasu
  Moved SDK_LITTLE_ENDIAN definition to types.h

  Revision 1.9  06/23/2005 12:04:49  yasu
  Added SDK_LITTLE_ENDIAN 

  Revision 1.8  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.7  02/18/2004 07:51:20  yada
  mmap_shared.h to A7/9 common

  Revision 1.6  02/13/2004 01:51:58  yada
  mmap_main.h, mmap_shared added, for ARM7

  Revision 1.5  02/12/2004 13:17:44  yasu
  fix include list

  Revision 1.4  02/12/2004 10:56:04  yasu
  new location of include files ARM9/ARM7

  Revision 1.3  02/05/2004 07:09:03  yasu
  change SDK prefix "iris" to "nitro"

  Revision 1.2  12/17/2003 06:05:39  yasu
  split mmap_reserved.h into mmap_tcm.h and mmap_shared.h

  Revision 1.1  12/09/2003 08:35:26  yasu
  Revamped the area related to memory map 

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MEMORYMAP_H_
#define NITRO_MEMORYMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	SDK_ARM9
#include	<nitro/hw/ARM9/mmap_global.h>
#include	<nitro/hw/ARM9/mmap_main.h>
#include	<nitro/hw/ARM9/mmap_tcm.h>
#include	<nitro/hw/ARM9/mmap_vram.h>
#include	<nitro/hw/common/mmap_shared.h>
#include	<nitro/hw/ARM9/ioreg.h>

#else  //SDK_ARM7
#include	<nitro/hw/ARM7/mmap_global.h>
#include	<nitro/hw/ARM7/mmap_main.h>
#include	<nitro/hw/ARM7/mmap_wram.h>
#include	<nitro/hw/common/mmap_shared.h>
#include	<nitro/hw/ARM7/ioreg.h>
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* NITRO_MEMORYMAP_H_ */
#endif

