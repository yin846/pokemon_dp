/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_FrameTexVramMan.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_FrameTexVramMan.h,v $
  Revision 1.7  08/17/2005 11:53:48 PM  kitani_toshikazu
  Revised copyright notation

  Revision 1.6  08/17/2005 08:08:55 AM  kitani_toshikazu
  Added a function for debug display

  Revision 1.5  10/18/2004 12:28:23 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.4  09/22/2004 09:51:04 AM  kitani_toshikazu
  Added function that changes the search order of normal textures.

  Revision 1.3  08/30/2004 12:17:03 AM  kitani_toshikazu
  Corrected a spelling error in the comments

  Revision 1.2  07/10/2004 04:32:53 AM  nishida
  Made revisions for the configuration of whether or not to use as the default at initialization

  Revision 1.1  07/10/2004 01:59:42 AM  nishida
  New registration when streamlining the VRAM manager.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_FRAMETEXVRAMMAN_H_
#define NNS_GFD_FRAMETEXVRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_TexVramMan_Types.h>

/*---------------------------------------------------------------------------*
  Name:         NNSGfdFrmTexVramState

  Description:  This is a structure that records the current state of the manager
                (In fact, it is the pointer location of 10.)
                                            
 *---------------------------------------------------------------------------*/
typedef struct NNSGfdFrmTexVramState
{
    u32         address[10];

} NNSGfdFrmTexVramState;


//------------------------------------------------------------------------------
// Function Declaration
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Internal limitation
void NNSi_GfdSetTexNrmSearchArray
( 
    int idx1st, 
    int idx2nd,
    int idx3rd,
    int idx4th,
    int idx5th
);

void         NNS_GfdDumpFrmTexVramManager();
void		 NNS_GfdInitFrmTexVramManager( u16 numSlot, BOOL useAsDefault );

NNSGfdTexKey NNS_GfdAllocFrmTexVram( u32 szByte, BOOL is4x4comp, u32 opt );
int			 NNS_GfdFreeFrmTexVram( NNSGfdTexKey memKey );

void		 NNS_GfdGetFrmTexVramState( NNSGfdFrmTexVramState* pState );
void		 NNS_GfdSetFrmTexVramState( const NNSGfdFrmTexVramState* pState );
void		 NNS_GfdResetFrmTexVramState();

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_FRAMETEXVRAMMAN_H_

