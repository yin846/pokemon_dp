/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  File:     g2di_SplitChar.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_SplitChar.h,v $
  Revision 1.2  05/23/2005 06:14:49 AM  hatamoto_minoru
  Added ISO-8859-1 to the encodings.

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.2  03/18/2005 09:23:07 AM  hatamoto_minoru
  Made everything non-public.

  Revision 1.1  03/10/2005 07:29:40 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef G2DI_SPLITCHAR_H_
#define G2DI_SPLITCHAR_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif


// Character extraction callback
typedef u16 (*NNSiG2dSplitCharCallback)(const void** ppChar);


/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dGetChar*

  Description:  This is a function for extracting an NNSiG2dGetCharCallback type character code.
                Gets the character code of the first character from the byte stream,
                and at the same time moves the stream pointer to the next character.

  Arguments:    ppChar: The pointer to the buffer where the pointer to the byte array is stored.
                        If returning from a function, the pointer to the top of the next character is stored
                        in the buffer at which this pointer is directed.

  Returns:      The character code of the first character of *ppChar.
 *---------------------------------------------------------------------------*/
u16 NNSi_G2dSplitCharUTF16(const void** ppChar);
u16 NNSi_G2dSplitCharUTF8(const void** ppChar);
u16 NNSi_G2dSplitCharShiftJIS(const void** ppChar);
u16 NNSi_G2dSplitChar1Byte(const void** ppChar);






#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // G2DI_SPLITCHAR_H_


