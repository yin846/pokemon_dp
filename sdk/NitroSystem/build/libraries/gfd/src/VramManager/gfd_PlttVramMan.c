/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_PlttVramMan.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_PlttVramMan.c,v $
  Revision 1.4  10/18/2004 12:27:53 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.3  08/30/2004 12:16:17 AM  kitani_toshikazu
  Corrected a spelling error in the comments

  Revision 1.2  2004/07/15 12:06:25 PM  nishida
  Corrected so that it returns key that expresses error. 

  Revision 1.1  07/10/2004 04:30:49 AM  nishida
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_PlttVramMan_Types.h>

static NNSGfdPlttKey	AllocPlttVram_( u32 szByte, BOOL b4Pltt, u32 bAllocFromLo );
static int				FreePlttVram_( NNSGfdPlttKey key );


NNSGfdFuncAllocPlttVram NNS_GfdDefaultFuncAllocPlttVram = AllocPlttVram_;
NNSGfdFuncFreePlttVram  NNS_GfdDefaultFuncFreePlttVram  = FreePlttVram_;

/*---------------------------------------------------------------------------*
  Name:         AllocPlttVram_

  Description:  Memory allocation dummy function. When the default memory allocation function is not registered, 
  				this function is called. 

  Arguments:   szByte:  The size
               bPltt4           :  4 color texture?
               bAllocFromHead   :  Allocate from the top of the region (lower position)?

  Returns:     Texture palette key
 *---------------------------------------------------------------------------*/
static NNSGfdPlttKey
AllocPlttVram_( u32 /* szByte */, BOOL /* b4Pltt */, u32 /* bAllocFromLo */ )
{
    NNS_GFD_WARNING("no default AllocPlttVram function.");

    // Error: Returns a PlttKey that expresses an error.
    return NNS_GFD_ALLOC_ERROR_PLTTKEY;
}

/*---------------------------------------------------------------------------*
  Name:         FreePlttTexVram_

  Description:  Memory release dummy function. When the default memory release function is not registered, 
  				this function is called. 

  Arguments:   plttKey          :   Texture palette key

  Returns:     Success or failure value (if successful, 0)
 *---------------------------------------------------------------------------*/
static int
FreePlttVram_( NNSGfdPlttKey /* plttKey */ )
{
    NNS_GFD_WARNING("no default FreePlttVram function.");

    return -1;
}

