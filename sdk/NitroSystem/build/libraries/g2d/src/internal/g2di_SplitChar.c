/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  File:     g2di_GetChar.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_SplitChar.c,v $
  Revision 1.3  05/30/2005 10:18:53 AM  hatamoto_minoru
  Changed from ISO8859 to CP1252.

  Revision 1.2  05/23/2005 06:16:21 AM  hatamoto_minoru
  Added ISO-8859-1 to the encodings.

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.2  03/18/2005 09:28:11 AM  hatamoto_minoru
  Made everything non-public.

  Revision 1.1  03/10/2005 08:21:43 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2di_SplitChar.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2di_AssertUtil.h>


//****************************************************************************
// static functions
//****************************************************************************


/*---------------------------------------------------------------------------*
  Name:         IsSjisLeadByte

  Description:  Determines whether or not this is an SJIS lead byte.

  Arguments:    c:  The byte data to be assessed.

  Returns:      Returns TRUE if c is an SJIS lead byte.
 *---------------------------------------------------------------------------*/
#define SJIS_LOW_WIDTH  0xBC
#define SJIS_LOW_BASE   0x40
#define SJIS_HIGH_BASE  0x81
#define SHIGH_AREA0_ST  0x81
#define SHIGH_AREA0_ED  0x85
#define SHIGH_AREA1     0x87
#define SHIGH_AREA2_ST  0x88
#define SHIGH_AREA2_ED  0xA0
#define SHIGH_AREA3_ST  0xE0
#define SHIGH_AREA3_ED  0x100
#define ASCII_ST    0x20
#define ASCII_ED    0x80
#define HANKANA_ST  0xA0
#define HANKANA_ED  0xE0

static inline BOOL IsSjisLeadByte( u8 c )
{
    return ( ((SJIS_HIGH_BASE <= c) && (c < SHIGH_AREA2_ED))
                || (SHIGH_AREA3_ST <= c)  )
                ? TRUE: FALSE;
}






//****************************************************************************
// global functions
//****************************************************************************


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSplitChar*

  Description:  This is a function for extracting an NNSiG2dGetCharCallback type character code.
                Gets the character code of the first character from the byte stream,
                and at the same time moves the stream pointer to the next character.

  Arguments:    ppChar: The pointer to the buffer where the pointer to the byte array is stored.
                        If returning from a function, the pointer to the top of the next character is stored
                        in the buffer at which this pointer is directed.

  Returns:      The character code of the first character of *ppChar.
 *---------------------------------------------------------------------------*/

// UTF-16
u16 NNSi_G2dSplitCharUTF16(const void** ppChar)
{
    u16 c;

    NNS_G2D_POINTER_ASSERT( ppChar );
    NNS_G2D_POINTER_ASSERT( *ppChar );
    NNS_G2D_ALIGN_ASSERT( *ppChar, 2 );

    c = *(const wchar_t*)*ppChar;
    ((const wchar_t*)*ppChar)++;

    return c;
}

// UTF-8
u16 NNSi_G2dSplitCharUTF8(const void** ppChar)
{
    const u8* pChar;
    u16 c;

    NNS_G2D_POINTER_ASSERT( ppChar );
    NNS_G2D_POINTER_ASSERT( *ppChar );
    NNS_G2D_ASSERT( (*(const u8*)*ppChar & 0xC0) != 0x80 );    // The 1st UTF-8 byte fulfills the conditions described at the left

    pChar = (const u8*)*ppChar;

    if( (*pChar & 0x80) == 0x00 )
        // 1 byte character
    {
        c = *pChar;
        *(u32*)ppChar += 1;
    }
    else if( (*pChar & 0xE0) == 0xC0 )
        // 2 byte character
    {
        c = (u16)( ((*(pChar + 0) & 0x1F) << 6) |
                   ((*(pChar + 1) & 0x3F) << 0) );
        *(u32*)ppChar += 2;
    }
    else
        // 3 byte character
    {
    	// Not supported in 4 bytes or higher
        NNS_G2D_ASSERT( (*pChar & 0xF0) == 0xE0 );

        c = (u16)(  ((*(pChar + 0) & 0x1F) << 12) |
                    ((*(pChar + 1) & 0x3F) <<  6) |
                    ((*(pChar + 2) & 0x3F) <<  0) );
        *(u32*)ppChar += 3;
    }

    return c;
}

// Shift_JIS
u16 NNSi_G2dSplitCharShiftJIS(const void** ppChar)
{
    const u8* pChar;
    u16 c;

    NNS_G2D_POINTER_ASSERT( ppChar );
    NNS_G2D_POINTER_ASSERT( *ppChar );

    pChar = (const u8*)*ppChar;

    if( IsSjisLeadByte(*pChar) )
    {
        c = (u16)( ((*pChar) << 8) | (*(pChar + 1)) );
        *(u32*)ppChar += 2;
    }
    else
    {
        c = *pChar;
        *(u32*)ppChar += 1;
    }

    return c;
}

// CP1252, etc.
u16 NNSi_G2dSplitChar1Byte(const void** ppChar)
{
    const u8* pChar;
    u16 c;

    NNS_G2D_POINTER_ASSERT( ppChar );
    NNS_G2D_POINTER_ASSERT( *ppChar );

    pChar = (const u8*)*ppChar;

    c = *pChar;
    *(u32*)ppChar += 1;

    return c;
}

