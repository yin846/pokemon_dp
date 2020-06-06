/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     fader.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: fader.c,v $
  Revision 1.4  09/07/2004 01:16:56 AM  ida
  use nnsys/misc.h

  Revision 1.3  09/06/2004 07:23:23 AM  ida
  around NNSi_SndFaderSet

  Revision 1.2  09/03/2004 10:04:11 AM  ida
  fix NNSi_SndFaderSet bug when frame == 0

  Revision 1.1  08/02/2004 05:27:57 AM  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/fader.h>

#include <nnsys/misc.h>

/******************************************************************************
	private functions
 ******************************************************************************/

void NNSi_SndFaderInit( NNSSndFader* fader )
{    
    NNS_NULL_ASSERT( fader );
    
    fader->origin = fader->target = 0;
    fader->counter = fader->frame = 0;
}

void NNSi_SndFaderSet( NNSSndFader* fader, int target, int frame )
{
    NNS_NULL_ASSERT( fader );
    
    fader->origin = NNSi_SndFaderGet( fader );
    fader->target = target;
    fader->frame  = frame;
    fader->counter = 0;
    
}

int NNSi_SndFaderGet( const NNSSndFader* fader )
{
    s64 value;
    
    NNS_NULL_ASSERT( fader );
    
    if ( fader->counter >= fader->frame ) {
        return fader->target;
    }
    
    value = ( fader->target - fader->origin )
            * fader->counter / fader->frame
            + fader->origin;
    
    return (int)value;
}

void NNSi_SndFaderUpdate( NNSSndFader* fader )
{
    NNS_NULL_ASSERT( fader );
    
    if ( fader->counter < fader->frame ) {
        fader->counter ++;
    }
}

BOOL NNSi_SndFaderIsFinished( const NNSSndFader* fader )
{
    NNS_NULL_ASSERT( fader );
    
    return fader->counter >= fader->frame ? TRUE : FALSE;
}

/*====== End of fader.c ======*/


