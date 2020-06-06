/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_TexVramMan.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_TexVramMan.c,v $
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
#include <nnsys/gfd/VramManager/gfd_TexVramMan_Types.h>

static NNSGfdTexKey	AllocTexVram_( u32 szByte, BOOL is4x4comp, u32 opt );
static int			FreeTexVram_( NNSGfdTexKey memKey );


NNSGfdFuncAllocTexVram  NNS_GfdDefaultFuncAllocTexVram = AllocTexVram_;
NNSGfdFuncFreeTexVram   NNS_GfdDefaultFuncFreeTexVram  = FreeTexVram_;

/*---------------------------------------------------------------------------*
  Name:         AllocTexVram_

  Description:  Memory allocation dummy function. When the default memory allocation function is not registered, 
  				this function is called. 

  Arguments:   szByte:  The size
               is4x4comp:  4x4 compressed texture?
               opt:  Implementation-dependent parameter

  Returns:     Texture
 *---------------------------------------------------------------------------*/
static NNSGfdTexKey
AllocTexVram_( u32 /* szByte */, BOOL /* is4x4comp */, u32 /* opt */ )
{
    NNS_GFD_WARNING("no default AllocTexVram function.");

    // Error: Return a TexKey that expresses an error.
    return NNS_GFD_ALLOC_ERROR_TEXKEY;
}

/*---------------------------------------------------------------------------*
  Name:         FreeFrmTexVram_

  Description:  Memory release dummy function. When the default memory release function is not registered, 
  				this function is called. 

  Arguments:   memKey :   Texture

  Returns:     Success or failure value (if successful, 0)
 *---------------------------------------------------------------------------*/
static int
FreeTexVram_( NNSGfdTexKey /* memKey */ )
{
    NNS_GFD_WARNING("no default FreeTexVram function.");

    return -1;
}

