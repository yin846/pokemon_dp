/*---------------------------------------------------------------------------*
  Project:  NitroSDK - screenshot test - Ext
  File:     ext_scrntest.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: ext_scrntest.h,v $
  Revision 1.8  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.7  02/28/2005 05:26:07  yosizaki
  do-indent.

  Revision 1.6  11/04/2004 02:21:59  takano_makoto
  fix typo.

  Revision 1.5  05/27/2004 09:16:31  takano_makoto
  Add EXT_Printf(), EXT_CompPrint()

  Revision 1.4  2004/02/06 12:38:55  yasu
  delete SDK_AUTOTEST off

  Revision 1.3  02/06/2004 09:16:23  kitani_toshikazu
  Add EXT_TestSetVRAMForScreenShot() definition.

  Revision 1.2  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.1  02/05/2004 00:59:34  kitani_toshikazu
  added ext header.

  
  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef EXT_SCRNTEST_H_
#define EXT_SCRNTEST_H_

#include <nitro/types.h>
#include <nitro/gx/gx_vramcnt.h>

#ifdef __cplusplus
extern "C" {
#endif

void    EXT_Printf(const char *fmt, ...);
void    EXT_CompPrint(const char *src1, const char *src2_fmt, ...);
void    EXT_TestScreenShot(u32 testFrames, u32 checkSum);
void    EXT_TestTickCounter();
void    EXT_TestResetCounter();
void    EXT_TestSetVRAMForScreenShot(GXVRamLCDC vram);

#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif /* EXT_SCRNTEST_H_ */
