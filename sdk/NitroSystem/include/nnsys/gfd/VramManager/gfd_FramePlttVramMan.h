/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_FramePlttVramMan.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_FramePlttVramMan.h,v $
  Revision 1.6  08/17/2005 11:53:48 PM  kitani_toshikazu
  Revised copyright notation

  Revision 1.5  08/17/2005 08:08:55 AM  kitani_toshikazu
  Added a function for debug display

  Revision 1.4  10/18/2004 12:28:23 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.3  08/30/2004 12:17:03 AM  kitani_toshikazu
  Corrected a spelling error in the comments

  Revision 1.2  07/10/2004 04:32:53 AM  nishida
  Made revisions for the configuration of whether or not to use as the default at initialization

  Revision 1.1  07/10/2004 01:59:42 AM  nishida
  New registration when streamlining the VRAM manager.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_FRAMEPLTTVRAMMAN_H_
#define NNS_GFD_FRAMEPLTTVRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_PlttVramMan_Types.h>


typedef struct NNSGfdFrmPlttVramState
{
    u32         address[2];
    
} NNSGfdFrmPlttVramState;



void            NNS_GfdDumpFrmPlttVramManager();
void			NNS_GfdInitFrmPlttVramManager( u32 szByte, BOOL useAsDefault );

NNSGfdPlttKey	NNS_GfdAllocFrmPlttVram( u32 szByte, BOOL b4Pltt, u32 bAllocFromLo );
int				NNS_GfdFreeFrmPlttVram( NNSGfdPlttKey plttKey );

void			NNS_GfdGetFrmPlttVramState( NNSGfdFrmPlttVramState* pState );
void			NNS_GfdSetFrmPlttVramState( const NNSGfdFrmPlttVramState* pState );
void			NNS_GfdResetFrmPlttVramState();

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_FRAMEPLTTVRAMMAN_H_

