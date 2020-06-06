/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  File:     g2di_BitReader.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_BitReader.c,v $
  Revision 1.2  06/27/2005 04:47:27 AM  hatamoto_minoru
  Revised invalid global function names/type names. 

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.4  03/14/2005 11:54:34 PM  hatamoto_minoru
  tab -> sp

  Revision 1.3  02/18/2005 09:37:10 AM  hatamoto_minoru
  Dealt with the sign/unsign warnings.

  Revision 1.2  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.1  12/06/2004 01:16:47 PM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "g2di_BitReader.h"


/*---------------------------------------------------------------------------*
  Name:         BitReaderReload

  Description:  Gets rid of the current buffer inside BitReader, and loads the next byte.

  Arguments:    reader: Pointer to BitReader.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void BitReaderReload(NNSiG2dBitReader* reader)
{
    reader->bits = *(reader->src)++;
    reader->availableBits = 8;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBitReaderRead

  Description:  Reads the bit string.

  Arguments:    reader: Pointer to BitReader.
                nBits:  The number of bits to read. Must be 8 or lower.

  Returns:      The bit string that was read.
 *---------------------------------------------------------------------------*/
u32 NNSi_G2dBitReaderRead(NNSiG2dBitReader* reader, int nBits)
{
    u32 val = reader->bits;
    int nAvlBits = reader->availableBits;

    SDK_ASSERT(nBits <= 8);

    if( nAvlBits < nBits )
    // If the the bits inside the byte buffer are insufficient.
    {
        int lack = nBits - nAvlBits;
        val <<= lack;
        BitReaderReload(reader);
        val |= NNSi_G2dBitReaderRead(reader, lack);
    }
    else
    // If the the bits inside the byte buffer are sufficient.
    {
        val >>= (nAvlBits - nBits);

        reader->availableBits = (s8)(nAvlBits - nBits);
    }

    val &= 0xFF >> (8 - nBits);
    return val;
}


