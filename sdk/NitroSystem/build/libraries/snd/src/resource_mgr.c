/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     resource_mgr.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: resource_mgr.c,v $
  Revision 1.8  01/06/2005 02:51:20  ida
  Updated copyright year notation

  Revision 1.7  01/05/2005 06:46:41  ida
  Added NNSi_GetLockedChannel function

  Revision 1.6  10/25/2004 02:12:15  ida
  change argument name

  Revision 1.5  10/21/2004 02:10:50  ida
  NNS_SndInitResourceMgr become private

  Revision 1.4  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.3  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.2  07/09/2004 06:54:03  ida
  when chBitMask == 0, do nothing

  Revision 1.1  06/28/2004 07:10:49  ida
  lock.c -> resource_mgr.c

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/resource_mgr.h>

#include <nitro/snd.h>
#include <nnsys/misc.h>

/******************************************************************************
	static variable
 ******************************************************************************/

static u32 sChannelLock;
static u32 sCaptureLock;
static u32 sAlarmLock;

/******************************************************************************
	public functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNS_SndLockChannel

  Description:  Locks channels.

  Arguments:    chBitFlag - Channel bit flag

  Returns:      Whether or not lock succeeded.
 *---------------------------------------------------------------------------*/
BOOL NNS_SndLockChannel( u32 chBitFlag )
{
    if ( chBitFlag == 0 ) return TRUE;
    
    if ( chBitFlag & sChannelLock ) return FALSE;
    
    SND_LockChannel( chBitFlag, 0 );
    
    sChannelLock |= chBitFlag;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndUnlockChannel

  Description:  Unlocks channels.

  Arguments:    chBitFlag - Channel bit flag

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndUnlockChannel( u32 chBitFlag )
{
    NNS_ASSERT( ( chBitFlag & sChannelLock ) == chBitFlag );

    if ( chBitFlag == 0 ) return;
    
    SND_UnlockChannel( chBitFlag, 0 );
    
    sChannelLock &= ~chBitFlag;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndLockCapture

  Description:  Locks capture.

  Arguments:    capBitFlag - Capture bit flag

  Returns:      Whether or not lock succeeded.
 *---------------------------------------------------------------------------*/
BOOL NNS_SndLockCapture( u32 capBitFlag )
{
    if ( capBitFlag & sCaptureLock ) return FALSE;
    
    sCaptureLock |= capBitFlag;
    
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndUnlockCapture

  Description:  Unlocks capture.

  Arguments:    capBitFlag - Capture bit flag

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndUnlockCapture( u32 capBitFlag )
{
    NNS_ASSERT( ( capBitFlag & sCaptureLock ) == capBitFlag );
    
    sCaptureLock &= ~capBitFlag;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndAllocAlarm

  Description:  Allocates alarm.

  Arguments:    None.

  Returns:      If allocation succeeds, returns alarm number.
                If allocation fails, returns -1.
 *---------------------------------------------------------------------------*/
int NNS_SndAllocAlarm( void )
{
    int alarmNo;
    u32 mask = 1;
    
    for( alarmNo = 0; alarmNo < SND_ALARM_NUM ; alarmNo++, mask <<= 1 )
    {
        if ( ( sAlarmLock & mask ) == 0 ) {
            sAlarmLock |= mask;
            return alarmNo;
        }
    }
    
    return -1;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_SndFreeAlarm

  Description:  Frees alarm.

  Arguments:    alarmNo - Alarm number

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_SndFreeAlarm( int alarmNo )
{
    NNS_MINMAX_ASSERT( alarmNo, SND_ALARM_MIN, SND_ALARM_MAX );
    NNS_ASSERT( sAlarmLock & ( 1 << alarmNo ) );
    
    sAlarmLock &= ~( 1 << alarmNo );
}

/******************************************************************************
	private functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndInitResourceMgr

  Description:  Initialize resource manager

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_SndInitResourceMgr( void )
{
    sChannelLock = 0;
    sCaptureLock = 0;
    sAlarmLock   = 0;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_GetLockedChannel

  Description:  Obtains the number of the channel that is already locked.

  Arguments:    None.

  Returns:      Bit flag of the channel that is already locked.
 *---------------------------------------------------------------------------*/
u32 NNSi_GetLockedChannel( void )
{
    return sChannelLock;
}

/*====== End of resource_mgr.c ======*/


