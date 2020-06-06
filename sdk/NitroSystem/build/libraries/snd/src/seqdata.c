/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Snd - libraries
  File:     seqarc.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: seqdata.c,v $
  Revision 1.8  03/09/2005 00:25:01  ida
  Updated copyright year notation

  Revision 1.7  03/08/2005 05:39:01  ida
  Support for NNS_FROM_TOOL

  Revision 1.6  09/07/2004 01:16:56  ida
  use nnsys/misc.h

  Revision 1.5  07/16/2004 06:42:49  ida
  fix Project name

  Revision 1.4  06/16/2004 04:30:25  ida
  fix wrong charactor code

  Revision 1.3  06/09/2004 08:45:34  ida
  fixed comments

  Revision 1.2  06/08/2004 00:51:00  ida
  change to private function

  Revision 1.1  05/13/2004 01:01:14  ida
  merge seqarc.c and data.c to seqdata.c

  Revision 1.10  05/12/2004 06:39:58  ida
  modify data header structure

  Revision 1.9  04/30/2004 05:24:50  ida
  add comments

  Revision 1.8  04/26/2004 02:57:02  ida
  NNSi_ -> NNS_

  Revision 1.7  04/06/2004 23:54:22  ida
  NNS_Snd -> NNSi_Snd

  Revision 1.6  04/05/2004 02:33:44  ida
  add comments

  Revision 1.5  03/11/2004 01:53:31  ida
  SeqArc count : u16 -> u32

  Revision 1.4  03/10/2004 08:40:10  ida
  reconstruct data structure

  Revision 1.3  02/20/2004 05:02:59  ida
  change directory

  Revision 1.1  02/13/2004 01:42:05  ida
  initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/snd/seqdata.h>

/* if include from Other Environment for exsample VC or BCB,*/
/* please define NNS_FROM_TOOL*/
#ifndef NNS_FROM_TOOL

#include <nnsys/misc.h>

#else

#define NNS_NULL_ASSERT(exp)           ((void) 0)

#ifdef _MSC_VER
#pragma warning( disable : 4018 ) // warning: signed/unsigned mismatch
#pragma warning( disable : 4311 ) // warning: pointer truncation from 'type' to 'type'
#pragma warning( disable : 4312 ) // warning: conversion from 'type' to 'type' of greater size
#endif

#endif // NNS_FROM_TOOL

/******************************************************************************
	private functions
 ******************************************************************************/

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndSeqArcGetSeqCount

  Description:  Gets the number of sequences inside the sequence archive

  Arguments:    seqArc - The sequence archive

  Returns:      The number of sequences
 *---------------------------------------------------------------------------*/
u32 NNSi_SndSeqArcGetSeqCount( const NNSSndSeqArc* seqArc )
{
    NNS_NULL_ASSERT( seqArc );
    return seqArc->count;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_SndSeqArcGetSeqInfo

  Description:  Gets the sequence information inside the sequence archive

  Arguments:    seqArc - The sequence archive
                seqNo  - The index number

  Returns:      The sequence information structure
 *---------------------------------------------------------------------------*/
const NNSSndSeqArcSeqInfo* NNSi_SndSeqArcGetSeqInfo(
    const NNSSndSeqArc* seqArc,
    int index
)
{
    NNS_NULL_ASSERT( seqArc );
    
	if ( index < 0 ) return NULL;
	if ( index >= seqArc->count ) return NULL;
    if ( seqArc->info[ index ].offset == NNS_SND_SEQ_ARC_INVALID_OFFSET ) return NULL;
	
	return & seqArc->info[ index ];
}

/*====== End of seqarc.c ======*/


