/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     basei.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: basei.h,v $
  Revision 1.6  02/09/2005 08:23:00 AM  kitamurs
  Changed so that Sleep is called when it can be called.

  Revision 1.5  01/21/2005 04:51:16 AM  kitamurs
  Changed year

  Revision 1.4  12/16/2004 07:25:03 AM  kitamurs
  Newly created the Sleep function

  Revision 1.3  12/01/2004 01:44:48 AM  kitamurs
  Message version check

  Revision 1.2  11/12/2004 02:03:04 AM  kitamurs
  Added support for divided transfer

  Revision 1.1  09/08/2004 06:00:14 AM  kitamurs
  Supported ensata

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_BASEI_H_
#define NNS_MCS_BASEI_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================
    static inline function
   ======================================================================== */

static inline void
NNSi_McsSleep(u32 milSec)
{
    if (OS_IsThreadAvailable() && OS_IsTickAvailable() && OS_IsAlarmAvailable())
    {
        OS_Sleep(milSec);
    }
    else
    {
        OS_SpinWait((u32)(1LL * HW_CPU_CLOCK * milSec /1000));
    }
}


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_MCS_BASEI_H_*/
#endif

