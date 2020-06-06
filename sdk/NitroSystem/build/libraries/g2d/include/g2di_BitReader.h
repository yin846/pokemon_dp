/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  File:     g2di_BitReader.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_BitReader.h,v $
  Revision 1.2  06/27/2005 04:47:27 AM  hatamoto_minoru
  Revised invalid global function names/type names. 

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.4  03/10/2005 08:19:25 AM  hatamoto_minoru
  Added comments.

  Revision 1.3  02/18/2005 09:37:10 AM  hatamoto_minoru
  Dealt with the sign/unsign warnings.

  Revision 1.2  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.2  12/06/2004 01:02:03 PM  hatamoto_minoru
  Organized the source and added comments.

  Revision 1.1  11/30/2004 09:17:04 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef G2DI_BITREADER_H_
#define G2DI_BITREADER_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct NNSiG2dBitReader
{
    const u8* src;          // Pointer to the read position
    s8 availableBits;       // Number of unread bits in bits
    u8 bits;                // Byte cache
    u8 padding_[2];         //
}
NNSiG2dBitReader;

//----------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBitReaderInit

  Description:  Initializes BitReader.

  Arguments:    reader: Pointer to the BitReader to initialize.
                src:    Pointer to the byte string that reads the bit.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNSi_G2dBitReaderInit(NNSiG2dBitReader* reader, const void* src)
{
    reader->availableBits   = 0;
    reader->src             = (const u8*)src;
    reader->bits            = 0;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBitReaderRead

  Description:  Reads the bit string.

  Arguments:    reader: Pointer to BitReader.
                nBits:  The number of bits to read. Must be 8 or lower.

  Returns:      The bit string that was read.
 *---------------------------------------------------------------------------*/
u32 NNSi_G2dBitReaderRead(NNSiG2dBitReader* reader, int nBits);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBitReaderAlignByte

  Description:  Gets rid of the remaining bits in the byte buffer.
                Subsequent bits read from this point become
                the top of each byte of the load source.

  Arguments:    reader: Pointer to BitReader.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNSi_G2dBitReaderAlignByte(NNSiG2dBitReader* reader)
{
    // Set to 0 if availableBits are not 8.
    // This assumes that availableBits is 8 or less.
    reader->availableBits &= 8;
}



#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // G2DI_BITREADER_H_


