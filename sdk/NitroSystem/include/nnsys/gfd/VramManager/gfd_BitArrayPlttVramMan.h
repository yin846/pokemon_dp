/*---------------------------------------------------------------------------*
  Project:  NNS - tests - Gfd
  File:     gfd_BitArrayTexPlttMan.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_BitArrayPlttVramMan.h,v $
  Revision 1.3  10/18/2004 12:28:23 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.2  09/17/2004 11:35:16 AM  kitani_toshikazu
  Erased the header include for internal module

  Revision 1.1  09/01/2004 05:10:49 AM  kitani_toshikazu
  Added a bit array palette manager and separated out the shared portions.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_GFD_BITARRAYPLTTMAN_H_
#define NNS_GFD_BITARRAYPLTTMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nnsys/gfd/VramManager/gfd_PlttVramMan_Types.h>



//------------------------------------------------------------------------------
// Palette Manager
//------------------------------------------------------------------------------


void NNS_GfdInitBarPlttVramManager
( 
    u32     szByte, 
    u32     bytePerOneBlock, 
    u32*    pBitTbl, 
    u32     lengthOfBitTbl,
    BOOL    useAsDefault 
);

NNSGfdPlttKey NNS_GfdAllocBarPlttVram
( 
    u32         szByte, 
    BOOL        b4Pltt, 
    u32         bAllocFromLo
);

int NNS_GfdFreeBarPlttVram( NNSGfdPlttKey plttKey );

void NNS_GfdResetBarPlttVramState ( void );


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_BITARRAYPLTTMAN_H_

