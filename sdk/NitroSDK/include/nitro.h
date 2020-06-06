/*---------------------------------------------------------------------------*
  Project:  NitroSDK
  File:     nitro.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: nitro.h,v $
  Revision 1.87  03/02/2006 10:20:34  okubata_ryoma
  Added vib.h

  Revision 1.86  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.85  09/05/2005 01:18:58  yada
  initial release

  Revision 1.84  08/19/2005 11:01:52  yada
  add std.h

  Revision 1.83  07/05/2005 10:55:55  yasu
  Returned include position of utest.h to original location

  Revision 1.82  07/05/2005 09:36:29  yasu
  Moved misc.h to utest.h

  Revision 1.81  06/24/2005 02:20:13  yasu
  Added utest

  Revision 1.80  06/23/2005 12:16:01  yasu
  Added SDK_LITTLE_ENDIAN definition

  Revision 1.79  02/24/2005 08:29:20  yasu
  Revised format

  Revision 1.78  02/21/2005 00:33:02  yasu
  Support strict form check

  Revision 1.77  02/18/2005 08:42:35  yasu
  Move to make warnings for non existent macro variables

  Revision 1.76  02/16/2005 23:51:41  terui
  Update copyright.

  Revision 1.75  02/16/2005 10:33:41  terui
  Added .h.

  Revision 1.74  12/14/2004 13:13:12  takano_makoto
  Moved fx_rand.h to math/rand.h

  Revision 1.73  12/14/2004 10:51:38  seiki_masashi
  Added MATH genre

  Revision 1.72  12/13/2004 07:45:25  takano_makoto
  Added fx/fx_rand.h

  Revision 1.71  11/10/2004 13:14:18  takano_makoto
  Add mb.h

  Revision 1.70  10/20/2004 06:18:04  terui
  Add include <nitro/hw/common/lcd.h>

  Revision 1.69  09/13/2004 10:53:32  yada
  add ctrdg.h

  Revision 1.68  09/06/2004 13:05:10  terui
  Add #include <nitro/wm.h>

  Revision 1.67  08/27/2004 02:05:39  yosizaki
  delete wbt.h

  Revision 1.66  08/23/2004 09:43:28  yosizaki
  add WBT.

  Revision 1.65  08/09/2004 11:29:54  yasu
  Add spec.h to switch platform settings

  Revision 1.64  05/14/2004 11:56:21  takano_makoto
  Move gx include description from nitro.h to nitro/gx.h.

  Revision 1.63  05/12/2004 05:45:08  terui
  Add /nitro/rtc.h

  Revision 1.62  05/11/2004 09:35:19  yosizaki
  add /nitro/fs.h

  Revision 1.61  05/06/2004 02:57:28  takano_makoto
  Add cp_context.h

  Revision 1.60  04/21/2004 12:11:09  takano_makoto
  move spi.h to common.

  Revision 1.59  04/15/2004 03:26:15  yasu
  add SDK_ASM

  Revision 1.58  03/10/2004 05:38:14  yada
  Added spi.h to ARM7.

  Revision 1.57  02/18/2004 07:05:36  nishida_kenji
  Add gx_sp.h.

  Revision 1.56  02/18/2004 01:23:52  yada
  Moved pad, created exi 

  Revision 1.55  02/17/2004 09:22:39  yada
  Made so that ARM7/9 both include pad.h

  Revision 1.54  02/14/2004 06:43:55  yasu
  add pxi.h

  Revision 1.53  02/13/2004 09:14:27  nishida_kenji
  Add nitro/gx/struct_2d.h .

  Revision 1.52  02/13/2004 00:17:18  ida
  snd ARM7 -> common

  Revision 1.51  02/12/2004 13:12:13  yasu
  merge nitro.h and nitro_sp.h

  Revision 1.50  02/12/2004 10:56:04  yasu
  new location of include files ARM9/ARM7

  Revision 1.49  02/10/2004 01:21:33  yada
  Because mi/*.h increased, bundled in mi.h.  

  Revision 1.48  02/05/2004 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.47  02/05/2004 00:59:34  kitani_toshikazu
  added ext header.

  Revision 1.46  12/25/2003 12:13:58  nishida_kenji
  g2oam.h to g2_oam.h

  Revision 1.45  12/25/2003 12:02:58  nishida_kenji
  g3util.h -> g3_util.h
  add gx_load.h
  remove g2util.h

  Revision 1.44  12/24/2003 06:35:50  nishida_kenji
  integrate g3x.h and g3x_status.h

  Revision 1.43  12/12/2003 01:27:14  yada
  Corrected due to the name change of include (os/system.h and hw/armArch.h)

  Revision 1.42  12/10/2003 00:16:12  nishida_kenji
  fx_sincos.h -> fx_trig.h

  Revision 1.41  12/09/2003 08:35:44  yasu
  Revamped area around the memory map; added memorymap.h  

  Revision 1.40  12/08/2003 12:17:50  yada
  Changed the file to place in an include statement from MemoryMap.h to mmap_global.h

  Revision 1.39  11/29/2003 08:52:43  nishida_kenji
  g2util.h

  Revision 1.38  11/28/2003 01:55:51  yada
  Reflects REDSDK 03-11-27

  Revision 1.37  11/25/2003 07:04:43  nishida_kenji
  Added gx_capture.h

  Revision 1.36  11/25/2003 05:52:58  yada
  Updated os.h

  Revision 1.35  11/25/2003 02:22:58  nishida_kenji
  Added gx_bgcnt.h

  Revision 1.34  11/25/2003 00:58:50  nishida_kenji
  Added gx_vramcnt.h

  Revision 1.33  11/21/2003 09:54:10  nishida_kenji
  Added g3util.h

  Revision 1.32  11/21/2003 08:14:50  nishida_kenji
  Added g3x_status.h

  Revision 1.31  11/18/2003 11:17:59  nishida_kenji
  fx_cp.h, fx_vec.h fx_mtx.h

  Revision 1.30  11/17/2003 00:35:17  nishida_kenji
  Added fx

  Revision 1.29  11/14/2003 12:05:05  nishida_kenji
  Added fx_const.h

  Revision 1.28  11/14/2003 10:54:14  nishida_kenji
  Added fx.h

  Revision 1.27  11/14/2003 06:05:41  nishida_kenji
  Eliminates G3R

  Revision 1.26  11/13/2003 11:25:47  nishida_kenji
  Added gxsincos.h

  Revision 1.25  11/12/2003 12:25:25  yasu
  Change base.h -> crt0.h

  Revision 1.24  11/12/2003 08:56:13  yasu
  typo correction

  Revision 1.23  11/11/2003 12:48:00  yada
  Made it so include is performed on systemCall.h

  Revision 1.22  11/11/2003 06:16:38  yada
  Added misc.h

  Revision 1.21  11/11/2003 06:08:06  nishida_kenji
  Deleted fixed.h and vec.h

  Revision 1.20  11/10/2003 09:02:02  yada
  Made it so include is performed on base/base.h

  Revision 1.19  11/10/2003 05:53:23  nishida_kenji
  Added g2oam.h

  Revision 1.18  11/05/2003 07:20:41  yasu
  Changed hw/ -> iris/hw

  Revision 1.17  11/05/2003 01:44:14  yada
  spinlock->spinLock

  Revision 1.16  11/05/2003 01:29:00  Nishida_Kenji
  Fixed spelling errors

  Revision 1.15  11/05/2003 01:07:20  yada
  spinlock->spinLock

  Revision 1.14  2003/11/04 10:39:10  Nishida_Kenji
  Added ioreg

  Revision 1.13  11/04/2003 09:26:26  yada
  (none)

  Revision 1.12  11/04/2003 09:25:15  yada
  Added include for hw

  Revision 1.11  11/04/2003 09:23:22  yada
  Added include information for cp,mi,os,pad

  Revision 1.10  10/31/2003 10:57:36  Nishida_Kenji
  iris/gx/gx.h

  Revision 1.9  10/31/2003 04:48:47  Nishida_Kenji
  iris/gx/g2.h

  Revision 1.8  10/30/2003 12:36:50  Nishida_Kenji
  g3r.h

  Revision 1.7  10/30/2003 10:45:12  Nishida_Kenji
  Moved gx

  Revision 1.6  10/30/2003 09:53:02  Nishida_Kenji
  g3x.h

  Revision 1.5  10/30/2003 05:07:47  Nishida_Kenji
  Added g3imm.h

  Revision 1.4  10/30/2003 02:12:04  Nishida_Kenji
  iris/g3b.h iris/g3c.h

  Revision 1.3  10/30/2003 01:24:47  Nishida_Kenji
  Added file to include

  Revision 1.2  10/28/2003 11:36:51  Nishida_Kenji
  changed the header of this file

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITRO_H_
#define NITRO_H_

#ifdef  SDK_ASM
#include <nitro/spec.h>
#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/hw/common/armArch.h>
#else  //SDK_ASM

//
// Workaround for no macro warnings on CW:
//  - __embedded_cplusplus in MSL_C\MSL_ARM\Include\ansi_prefix.ARM.h
//  - __VEC__              in MSL_C\MSL_Common\Include\stdlib.h
//  - _MSL_USE_INLINE      in MSL_C\MSL_ARM\Include\math.ARM.h,
//
#ifdef        __MWERKS__
#  ifdef      __cplusplus
#    ifndef   __embedded_cplusplus
#      define __embedded_cplusplus 0
#    endif
#    ifndef   __VEC__
#      define __VEC__			0
#    endif
#  endif
#  if         !defined(_MSL_USE_INLINE) && __option(dont_inline)
#  define     _MSL_USE_INLINE		0
#  endif
#endif

#include <nitro/spec.h>
#include <nitro/misc.h>
#include <nitro/init/crt0.h>

#include <nitro/types.h>
#include <nitro/memorymap.h>
#include <nitro/hw/common/armArch.h>
#include <nitro/hw/common/lcd.h>

#include <nitro/os.h>
#include <nitro/mi.h>
#include <nitro/pxi.h>
#include <nitro/snd.h>
#include <nitro/pad.h>
#include <nitro/spi.h>
#include <nitro/card.h>
#include <nitro/fs.h>
#include <nitro/rtc.h>
#include <nitro/gx.h>
#include <nitro/wm.h>
#include <nitro/wvr.h>
#include <nitro/ctrdg.h>
#include <nitro/math.h>
#include <nitro/utest.h>
#include <nitro/std.h>

#ifdef  SDK_ARM9

#include <nitro/fx/fx.h>
#include <nitro/fx/fx_const.h>
#include <nitro/fx/fx_trig.h>
#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_vec.h>
#include <nitro/fx/fx_mtx.h>
#include <nitro/cp.h>
#include <nitro/ext.h>
#include <nitro/mb.h>
#include <nitro/env.h>
#include <nitro/vib.h>

#else  //SDK_ARM7

#include <nitro/exi.h>

#endif //SDK_ARM7
#endif //SDK_ASM

/* NITRO_H_*/
#endif

/*-----------------------------------------*
   NITRO-SDK Development Staff
       1st period (Sep03-Feb04)
         - Yoshitaka Yasumoto
         - Hiroto Yada
         - Kenji Nishida
         - Toshikazu Kitani(Jan04-Feb04)
       2nd period (Feb04- )
         - Yoshitaka Yasumoto
         - Hiroto Yada
         - Akio Terui
         - Koji Yoshizaki
         - Makoto Takano
         - Masashi Seiki(Jun04-)
 *-----------------------------------------*/
