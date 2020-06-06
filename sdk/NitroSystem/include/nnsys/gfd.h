/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics Fundation library
  File:     gfd.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd.h,v $
  Revision 1.6  09/29/2004 02:03:51 AM  kitani_toshikazu
  Added comments

  Revision 1.5  07/10/2004 01:52:16 AM  nishida
  Arranged VRAM Manager

  Revision 1.4  07/08/2004 07:18:18 AM  kitani_toshikazu
  Added the VRAM transmission manager

  Revision 1.3  06/23/2004 08:13:09 AM  kitani_toshikazu
  Added file header

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_GFD_H_
#define NNS_GFD_H_

#include <nnsys/gfd/gfd_common.h>

#include <nnsys/gfd/VramManager/gfd_TexVramMan_Types.h>
#include <nnsys/gfd/VramManager/gfd_FrameTexVramMan.h>
#include <nnsys/gfd/VramManager/gfd_LinkedListTexVramMan.h>

#include <nnsys/gfd/VramManager/gfd_PlttVramMan_Types.h>
#include <nnsys/gfd/VramManager/gfd_FramePlttVramMan.h>
#include <nnsys/gfd/VramManager/gfd_LinkedListPlttVramMan.h>

#include <nnsys/gfd/VramTransferMan/gfd_VramTransferManager.h>

#endif // NNS_GFD_H_

