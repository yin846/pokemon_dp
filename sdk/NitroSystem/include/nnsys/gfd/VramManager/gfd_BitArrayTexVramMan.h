/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_BitArrayTexVramMan.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_BitArrayTexVramMan.h,v $
  Revision 1.7  10/18/2004 12:28:23 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.6  09/17/2004 11:35:16 AM  kitani_toshikazu
  Erased the header include for internal module

  Revision 1.5  09/10/2004 10:47:00 AM  kitani_toshikazu
  (none)

  Revision 1.4  09/01/2004 05:10:49 AM  kitani_toshikazu
  Added a bit array palette manager and separated out the shared portions.

  Revision 1.3  08/30/2004 05:28:54 AM  nishida
  Corrected a spelling error in the comments

  Revision 1.2  07/28/2004 12:40:38 AM  kitani_toshikazu
  Correspondence with the padding warning

  Revision 1.1  07/12/2004 04:31:56 AM  nishida
  New registration when streamlining the VRAM manager.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_GFD_BITARRAYVRAMMAN_H_
#define NNS_GFD_BITARRAYVRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_TexVramMan_Types.h>



//------------------------------------------------------------------------------
// Function Declaration
//------------------------------------------------------------------------------

void NNS_GfdInitBarTexVramManager
( 
    u32     numSlot, 
    u32     bytePerOneBlock, 
    u32*    pBitTbl, 
    u32     lengthOfBitTbl,
    BOOL	useAsDefault
);

NNSGfdTexKey    NNS_GfdAllocBarTexVram( u32 szByte, BOOL is4x4comp, u32 opt );
int             NNS_GfdFreeBarTexVram( NNSGfdTexKey memKey );
void            NNS_GfdResetBarTexVramState( void );




#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_BITARRAYVRAMMAN_H_


