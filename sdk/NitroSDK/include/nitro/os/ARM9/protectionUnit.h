/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS - include
  File:     protectionUnit.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: protectionUnit.h,v $
  Revision 1.11  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.10  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.9  02/28/2005 05:26:02  yosizaki
  do-indent.

  Revision 1.8  05/28/2004 11:03:37  yada
  only fix comment

  Revision 1.7  04/07/2004 02:03:17  yada
  fix header comment

  Revision 1.6  02/05/2004 07:27:19  yada
  historic IRIS character string returned from NITRO to IRIS.   

  Revision 1.5  2004/02/05 07:09:03  yasu
  change SDK prefix iris -> nitro

  Revision 1.4  2004/01/16 01:17:55  yasu
  Support SDK_BB

  Revision 1.3  01/15/2004 02:40:01  yada
  changed IRIS_BB  to -> SDK_BB

  Revision 1.2  01/14/2004 12:06:35  yada
  changed protectUnit to -> protectionUnit

  Revision 1.1  12/08/2003 12:22:34  yada
  Initial release.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_OS_PROTECTIONUNIT_H_
#define NITRO_OS_PROTECTIONUNIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>


//======================================================================
//          protection unit
//======================================================================
/*---------------------------------------------------------------------------*
  Name:         OS_EnableProtectionUnit

  Description:  enable protection unit

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_EnableProtectionUnit(void);

/*---------------------------------------------------------------------------*
  Name:         OS_DisableProtectionUnit

  Description:  disable protection unit

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void    OS_DisableProtectionUnit(void);

#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_OS_PROTECTIONUNIT_H_ */
#endif
