/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - include
  File:     resource_mgr.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: resource_mgr.h,v $
  Revision 1.6  01/06/2005 04:07:49  ida
  Updated copyright year notation

  Revision 1.5  01/05/2005 06:46:41  ida
  Added NNSi_GetLockedChannel function

  Revision 1.4  10/25/2004 02:12:15  ida
  change argument name

  Revision 1.3  10/21/2004 02:11:30  ida
  NNS_SndInitResourceMgr become private

  Revision 1.2  07/16/2004 06:42:58  ida
  fix Project name

  Revision 1.1  06/28/2004 07:18:01  ida
  lock.h -> resource_mgr.h

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_SND_RESOURCE_MGR_H_
#define NNS_SND_RESOURCE_MGR_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
	public function declaration
 ******************************************************************************/

BOOL NNS_SndLockChannel( u32 chBitFlag );
void NNS_SndUnlockChannel( u32 chBitFlag );

BOOL NNS_SndLockCapture( u32 capBitFlag );
void NNS_SndUnlockCapture( u32 capBitFlag );

int NNS_SndAllocAlarm( void );
void NNS_SndFreeAlarm( int alarmNo );

/******************************************************************************
	private function declaration
 ******************************************************************************/

void NNSi_SndInitResourceMgr( void );
u32 NNSi_GetLockedChannel( void );

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* NNS_SND_RESOURCE_MGR_H_*/


