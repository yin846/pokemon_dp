/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     mcs.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mcs.h,v $
  Revision 1.5  01/21/2005 05:00:13  kitamurs
  Changed year

  Revision 1.4  01/17/2005 00:26:07  kitamurs
  Added win32 header

  Revision 1.3  12/15/2004 06:07:30  kitamurs
  Print features

  Revision 1.2  06/14/2004 00:26:20  kitamurs
  Added file I/O header

  Revision 1.1  05/25/2004 07:00:37  kitamurs
  Moved directories

  Revision 1.1  05/24/2004 02:17:22  kitamurs
  Moved directories

  Revision 1.1  04/14/2004 07:02:51  kitamurs
  Registration of the initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_H_
#define NNS_MCS_H_

#if defined(_WIN32)

#include <nnsys/mcs/base_win32.h>

/* _WIN32*/
#else

#include <nnsys/mcs/base.h>
#include <nnsys/mcs/fileIOcommon.h>
#include <nnsys/mcs/fileIObase.h>
#include <nnsys/mcs/print.h>

/* _WIN32*/
#endif

/* NNS_MCS_H_*/
#endif

