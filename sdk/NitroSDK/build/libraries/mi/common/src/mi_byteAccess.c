/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MI
  File:     mi_byteAccess.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mi_byteAccess.c,v $
  Revision 1.6  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.5  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.4  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.3  2004/05/20 07:24:52  yasu
  MI_WriteByte/ReadByte are valid only on the TEG

  Revision 1.2  04/06/2004 11:01:48  yosizaki
  change arg-type of MI_ReadByte. (void* -> const void*)

  Revision 1.1  03/30/2004 10:59:55  yada
  Changed byte access functions from ARM7 only to common.

  Revision 1.3  03/23/2004 01:17:35  yada
  deleted extra if - def

  Revision 1.2  03/23/2004 00:53:25  yada
  changed byteAccess.h include position

  Revision 1.1  03/23/2004 00:50:36  yada
  firstRelease

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro/misc.h>
#include <nitro/mi/byteAccess.h>

/*---------------------------------------------------------------------------*
  Name:         MI_ReadByte

  Description:  read byte data

  Arguments:    address ... address to read byte data

  Returns:      byte data
 *---------------------------------------------------------------------------*/
#ifdef	SDK_TEG
u8 MI_ReadByte(const void *address)
{
    if ((u32)address & 1)
    {
        return (u8)(*(u16 *)((u32)address & ~1) >> 8);
    }
    else
    {
        return (u8)(*(u16 *)address & 0xff);
    }
}
#endif

/*---------------------------------------------------------------------------*
  Name:         MI_WriteByte

  Description:  write byte data

  Arguments:    address ... address to write byte data

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef	SDK_TEG
void MI_WriteByte(void *address, u8 value)
{
    u16     val = *(u16 *)((u32)address & ~1);

    if ((u32)address & 1)
    {
        *(u16 *)((u32)address & ~1) = (u16)(((value & 0xff) << 8) | (val & 0xff));
    }
    else
    {
        *(u16 *)((u32)address & ~1) = (u16)((val & 0xff00) | (value & 0xff));
    }
}
#endif
