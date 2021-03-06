/*---------------------------------------------------------------------------*
  Project:  NitroSDK - include
  File:     version_begin.h

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: version_begin.h,v $
  Revision 1.3  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.2  07/11/2005 05:00:42  yada
  add include nitro/section.h

  Revision 1.1  04/08/2005 09:32:32  yosizaki
  initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
//
// include this instead of using C99 pragma extensions for compatibility
//
#include <nitro/section.h>

#if	defined(SDK_CW) || defined(__MWERKS__)
#pragma section VERSION begin
#elif	defined(SDK_ADS)
TO BE   DEFINED
#elif	defined(SDK_GCC)
TO BE   DEFINED
#endif
