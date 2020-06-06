/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  File:     g2d_Font.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Font.c,v $
  Revision 1.5  05/30/2005 10:18:48 AM  hatamoto_minoru
  Added support for the older versions.

  Revision 1.4  05/25/2005 08:09:59 AM  hatamoto_minoru
  Changed the formatting of the fonts. Added 1 element to the character width information.

  Revision 1.3  05/23/2005 06:34:16 AM  hatamoto_minoru
  Fixed where NNS_G2dFontInitAuto() did not support ISO-8859-1.

  Revision 1.2  05/23/2005 06:16:21 AM  hatamoto_minoru
  Added ISO-8859-1 to the encodings.

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.8  03/28/2005 02:14:10 AM  hatamoto_minoru
  Fixed where there was an erroneous ASSERT position.

  Revision 1.7  03/18/2005 09:30:22 AM  hatamoto_minoru
  Added NNSG2dFont build functions.
  Added support for the changes to the NNSG2dFont structure.

  Revision 1.6  03/10/2005 08:43:19 AM  hatamoto_minoru
  Changed the order of the arguments.

  Revision 1.5  03/10/2005 08:23:23 AM  hatamoto_minoru
  Added comments.
  Moved the render character string size calculation function from TextCanvas to Font.

  Revision 1.4  02/16/2005 04:16:23 AM  hatamoto_minoru
  Deleted the old MAPMETHOD and its related functions.

  Revision 1.3  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.2  02/01/2005 11:58:10 AM  hatamoto_minoru
  Put the source in order.

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.3  12/10/2004 10:15:02 AM  hatamoto_minoru
  Added comments and ASSERT.

  Revision 1.2  12/06/2004 01:02:03 PM  hatamoto_minoru
  Organized the source and added comments.

  Revision 1.1  11/30/2004 09:17:04 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_Font.h>
#include <nnsys/g2d/load/g2d_NFT_load.h>
#include <nnsys/g2d/fmt/g2d_Font_data.h>



//****************************************************************************
// Local Functions
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         GetGlyphIndex

  Description:  Gets the glyph index that corresponds to c from
                process callback function.

  Arguments:    pMap:   character code map block of the font.
                c:      character code.

  Returns:      Glyph index that corresponds to c.
                Returns NNS_G2D_GLYPH_INDEX_NOT_FOUND if one does not exist.
 *---------------------------------------------------------------------------*/
static u16 GetGlyphIndex(const NNSG2dFontCodeMap* pMap, u16 c)
{
    u16 index = NNS_G2D_GLYPH_INDEX_NOT_FOUND;

    NNS_G2D_POINTER_ASSERT( pMap );
    NNS_G2D_ASSERT(pMap->ccodeBegin <= c && c <= pMap->ccodeEnd);

    switch( pMap->mappingMethod )
    {
    //-----------------------------------------------------------
    // index  = character code - offset
    case NNS_G2D_MAPMETHOD_DIRECT:
        {
            u16 offset = pMap->mapInfo[0];
            index = (u16)(c - pMap->ccodeBegin + offset);
        }
        break;

    //-----------------------------------------------------------
    // index  = table[character code - character codeoffset ]
    case NNS_G2D_MAPMETHOD_TABLE:
        {
            const int table_index = c - pMap->ccodeBegin;

            index = pMap->mapInfo[table_index];
        }
        break;

    //-----------------------------------------------------------
    // index  = binary search (character code)
    case NNS_G2D_MAPMETHOD_SCAN:
        {
            const NNSG2dCMapInfoScan* const ws = (NNSG2dCMapInfoScan*)(pMap->mapInfo);
            const NNSG2dCMapScanEntry* st = &(ws->entries[0]);
            const NNSG2dCMapScanEntry* ed = &(ws->entries[ws->num - 1]);

            while( st <= ed )
            {
                const NNSG2dCMapScanEntry* md = st + (ed - st) / 2;

                if( md->ccode < c )
                {
                    st = md + 1;
                }
                else if( c < md->ccode )
                {
                    ed = md - 1;
                }
                else
                {
                    index = md->index;
                    break;
                }
            }
        }
        break;

    //-----------------------------------------------------------
    // unknown
    default:
        NNS_G2D_ASSERTMSG(FALSE, "unknwon MAPMETHOD");
    }

    return index;
}



/*---------------------------------------------------------------------------*
  Name:         GetCharWidthsFromIndex

  Description:  Gets the character width information corresponding to glyph index
                process callback function.

  Arguments:    pWidth: Pointer to the character width block of the font.
                idx         :    glyph index .

  Returns:      Character width information corresponding to idx.
                The operations are indefinite when idx is an index not included in pWidth.
 *---------------------------------------------------------------------------*/
#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE
static inline const NNSG2dCharWidths* GetCharWidthsFromIndex(
    int widthsSize, const NNSG2dFontWidth* const pWidth, int idx )
{
    NNS_G2D_POINTER_ASSERT( pWidth );
    NNS_G2D_ASSERT(pWidth->indexBegin <= idx && idx <= pWidth->indexEnd);

    return (NNSG2dCharWidths*)( (u32)(pWidth->widthTable) + (idx - pWidth->indexBegin) * widthsSize );
}
#else
static inline const NNSG2dCharWidths* GetCharWidthsFromIndex( const NNSG2dFontWidth* const pWidth, int idx )
{
    NNS_G2D_POINTER_ASSERT( pWidth );
    NNS_G2D_ASSERT(pWidth->indexBegin <= idx && idx <= pWidth->indexEnd);

    return (NNSG2dCharWidths*)(pWidth->widthTable) + (idx - pWidth->indexBegin);
}
#endif




//*******************************************************************************
// Global Functions
//*******************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontInit*

  Description:  Builds the font from the font resource.

  Arguments:    pFont:      Pointer to the font.
                pNftrFile:  Pointer to the font resource.

  Returns:      Returns a value other than FALSE if the font build is successful.
 *---------------------------------------------------------------------------*/

#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE
    #define NNS_G2D_DEFINE_FONT_INIT_PART_FOR_OLD                           \
        pFont->isOldVer = (u16)(result - TRUE);                             \
        pFont->widthsSize = (u16)( pFont->isOldVer ?                        \
                    sizeof(NNSG2dCharWidthsOld): sizeof(NNSG2dCharWidths) );
#else // NNS_G2D_FONT_USE_OLD_RESOURCE
    #define NNS_G2D_DEFINE_FONT_INIT_PART_FOR_OLD
#endif // NNS_G2D_FONT_USE_OLD_RESOURCE


void NNS_G2dFontInitAuto(NNSG2dFont* pFont, void* pNftrFile)
{
    const static NNSiG2dSplitCharCallback spliterTable[] =
    {
        NNSi_G2dSplitCharUTF8,
        NNSi_G2dSplitCharUTF16,
        NNSi_G2dSplitCharShiftJIS,
        NNSi_G2dSplitChar1Byte
    };
    NNSG2dFontEncoding encoding;
    BOOL result;

    NNS_G2D_POINTER_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( pNftrFile );

    result = NNSi_G2dGetUnpackedFont(pNftrFile, &(pFont->pRes));
    NNS_G2D_ASSERTMSG(result, "Faild to unpack font.");
    encoding = (NNSG2dFontEncoding)pFont->pRes->encoding;
    NNS_G2D_MINMAX_ASSERT( encoding, 0, NNS_G2D_NUM_OF_ENCODING - 1 );

    NNS_G2D_DEFINE_FONT_INIT_PART_FOR_OLD
    pFont->cbCharSpliter = spliterTable[encoding];
}


#define NNS_G2D_DEFINE_FONT_INIT(name, enc, spliter)                        \
    void NNS_G2dFontInit##name(NNSG2dFont* pFont, void* pNftrFile)          \
    {                                                                       \
        BOOL result;                                                        \
        NNS_G2D_POINTER_ASSERT( pFont );                                    \
        NNS_G2D_POINTER_ASSERT( pNftrFile );                                \
                                                                            \
        result = NNSi_G2dGetUnpackedFont(pNftrFile, &(pFont->pRes));        \
        NNS_G2D_ASSERTMSG(result, "Faild to unpack font.");                 \
        NNS_G2D_ASSERT(  (NNSG2dFontEncoding)pFont->pRes->encoding          \
                                    == NNS_G2D_FONT_ENCODING_##enc );       \
                                                                            \
        NNS_G2D_DEFINE_FONT_INIT_PART_FOR_OLD                               \
        pFont->cbCharSpliter = NNSi_G2dSplitChar##spliter;                  \
    }

NNS_G2D_DEFINE_FONT_INIT(UTF8, UTF8, UTF8)
NNS_G2D_DEFINE_FONT_INIT(UTF16, UTF16, UTF16)
NNS_G2D_DEFINE_FONT_INIT(ShiftJIS, SJIS, ShiftJIS)
NNS_G2D_DEFINE_FONT_INIT(CP1252, CP1252, 1Byte)



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontFindGlyphIndex

  Description:  Gets the index of the corresponding glyph from the character code.

  Arguments:    pFont:  Pointer to the font.
                c:      Character code that gets the glyph index.

  Returns:      If a glyph corresponding to c exists, returns its index.
                Returns NNS_G2D_GLYPH_INDEX_NOT_FOUND if one does not exist.
 *---------------------------------------------------------------------------*/
u16 NNS_G2dFontFindGlyphIndex( const NNSG2dFont* pFont, u16 c )
{
    const NNSG2dFontCodeMap* pMap;

    NNS_G2D_FONT_ASSERT(pFont);

    pMap = pFont->pRes->pMap;

    // linear search for the CMAP block list
    while( pMap != NULL )
    {
        if( (pMap->ccodeBegin <= c) && (c <= pMap->ccodeEnd) )
        {
            return GetGlyphIndex(pMap, c);
        }

        pMap = pMap->pNext;
    }

    // not found
    return NNS_G2D_GLYPH_INDEX_NOT_FOUND;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidthsFromIndex

  Description:  Gets the character width information from the glyph index.

  Arguments:    pFont:  Pointer to the font.
                idx         :    glyph index .

  Returns:      Pointer to the character width information stored in the font
                if character width information corresponding to idx exists.
                Returns the pointer to the default width information if not.
 *---------------------------------------------------------------------------*/
const NNSG2dCharWidths* NNS_G2dFontGetCharWidthsFromIndex( const NNSG2dFont* pFont, u16 idx )
{
    const NNSG2dFontWidth* pWidth;

    NNS_G2D_FONT_ASSERT(pFont);

    pWidth = pFont->pRes->pWidth;

    // linear search width information block list
    while( pWidth != NULL )
    {
        if( (pWidth->indexBegin <= idx) && (idx <= pWidth->indexEnd) )
        {
#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE
            return GetCharWidthsFromIndex(pFont->widthsSize, pWidth, idx);
#else
            return GetCharWidthsFromIndex(pWidth, idx);
#endif // NNS_G2D_FONT_USE_OLD_RESOURCE

        }

        pWidth = pWidth->pNext;
    }

    // Return default if not found.
    return &(pFont->pRes->defaultWidth);
}















//----------------------------------------------------------------------------
// Acquire size.
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetStringWidth

  Description:  Obtains the width when one line of a character string is drawn. 
                Namely, it finds the character string width when drawn, from txt to the return char or terminating char.  
                

  Arguments:    pTxn:   Pointer to the text screen structure.
                txt:    Pointer to the NULL terminal character string.
                pPos:   Pointer to the buffer that stores the pointer to the next character from the first line feed
                        character when txt contains a line feed character.
                        A NULL is stored when txt does not contain a line feed character.
                        A NULL can be designated here if unnecessary.

  Returns:      txt width.
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetStringWidth(
    const NNSG2dFont* pFont,
    int hSpace,
    const void* str,
    const void** pPos
)
{
    int width = 0;
    const void* pos = str;
    u16 c;
    NNSiG2dSplitCharCallback getNextChar;

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( str );

    getNextChar = NNSi_G2dFontGetSpliter(pFont);

    while( (c = getNextChar((const void**)&pos)) != 0 )
    {
        if( c == '\n' )
        {
            break;
        }

        width += NNS_G2dFontGetCharWidth(pFont, c) + hSpace;
    }

    if( pPos != NULL )
    {
        *pPos = (c == '\n') ? pos: NULL;
    }
    if( width > 0 )
    {
        width -= hSpace;
    }
    return width;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetTextHeight

  Description:  Looks for the height when a character string containing a line feed is rendered.

  Arguments:    pTxn:   Pointer to the text screen structure.
                txt:    Pointer to the NULL terminal character string.

  Returns:      txt height.
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetTextHeight(
    const NNSG2dFont* pFont,
    int vSpace,
    const void* txt
)
{
    const void* pos = txt;
    int lines = 1;
    NNSG2dTextRect rect = {0, 0};
    u16 c;
    NNSiG2dSplitCharCallback getNextChar;

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( txt );

    getNextChar = NNSi_G2dFontGetSpliter(pFont);

    while( (c = getNextChar((const void**)&pos)) != 0 )
    {
        if( c == '\n' )
        {
            lines++;
        }
    }

    return lines * (NNS_G2dFontGetLineFeed(pFont) + vSpace) - vSpace;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetTextWidth

  Description:  Looks for the width when a character string is rendered.
                This value will become the maximum width of each line.

  Arguments:    pTxn:   Pointer to the font that is the basis for the calculation.
                txt:    character string .

  Returns:      The width when the character string is rendered.
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetTextWidth(
    const NNSG2dFont* pFont,
    int hSpace,
    const void* txt
)
{
    int width = 0;

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( txt );

    while( txt != NULL )
    {
        const int line_width = NNSi_G2dFontGetStringWidth(pFont, hSpace, txt, &txt);
        if( line_width > width )
        {
            width = line_width;
        }
    }

    return width;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetTextRect

  Description:  Looks for the maximum width and height when a character string containing a line feed is rendered.

  Arguments:    pTxn:   Pointer to the text screen structure.
                txt:    Pointer to the NULL terminal character string.

  Returns:      The width and height of the smallest rectangle that can surround the txt.
 *---------------------------------------------------------------------------*/
NNSG2dTextRect NNSi_G2dFontGetTextRect(
    const NNSG2dFont* pFont,
    int hSpace,
    int vSpace,
    const void* txt
)
{
    int lines = 1;
    NNSG2dTextRect rect = {0, 0};

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( txt );

    while( txt != NULL )
    {
        const int width = NNSi_G2dFontGetStringWidth(pFont, hSpace, txt, &txt);
        if( width > rect.width )
        {
            rect.width = width;
        }
        lines++;
    }

    // height  = number of lines x (line height + space between lines) - Space between lines
    rect.height = ((lines - 1) * (NNS_G2dFontGetLineFeed(pFont) + vSpace) - vSpace);

    return rect;
}


