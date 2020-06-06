/*---------------------------------------------------------------------------*
  Project:  NitroSDK - include - MI
  File:     mi.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi.h,v $
  Revision 1.11  04/05/2006 07:44:43  yosizaki
  add mi/endian.h

  Revision 1.10  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.9  02/10/2005 07:47:21  yada
  add mi/init.h

  Revision 1.8  01/28/2005 13:13:39  takano_makoto
  Added mi/compress.h

  Revision 1.7  11/30/2004 10:53:53  takano_makoto
  add uncomp_stream.h

  Revision 1.6  07/20/2004 07:58:42  yada
  add stream.h

  Revision 1.5  04/26/2004 11:04:25  yosizaki
  add mi/card.h

  Revision 1.4  03/30/2004 11:00:30  yada
  Changed byte access functions from ARM7 only to common.

  Revision 1.3  03/23/2004 00:54:05  yada
  Added byteAccess.h in case of ARM7.

  Revision 1.2  02/12/2004 13:37:44 AM  yasu
  change include "" -> include <>

  Revision 1.1  02/10/2004 01:21:10  yada
  Created for an increase in mi/*.h.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_MI_H_
#define NITRO_MI_H_

#include <nitro/mi/dma.h>
#include <nitro/mi/exMemory.h>
#include <nitro/mi/wram.h>
#include <nitro/mi/memory.h>
#include <nitro/mi/swap.h>
#include <nitro/mi/uncompress.h>
#include <nitro/mi/byteAccess.h>
#include <nitro/mi/card.h>
#include <nitro/mi/stream.h>
#include <nitro/mi/uncomp_stream.h>
#include <nitro/mi/compress.h>
#include <nitro/mi/init.h>
#include <nitro/mi/endian.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_MI_H_ */
#endif
