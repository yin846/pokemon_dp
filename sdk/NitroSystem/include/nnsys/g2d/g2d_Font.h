/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  File:     g2d_Font.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Font.h,v $
  Revision 1.4  05/30/2005 10:14:19 AM  hatamoto_minoru
  Added support for the older versions.

  Revision 1.3  05/25/2005 08:10:00 AM  hatamoto_minoru
  Changed the formatting of the fonts. Added 1 element to the character width information.

  Revision 1.2  05/23/2005 06:14:49 AM  hatamoto_minoru
  Added ISO-8859-1 to the encodings.

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.7  03/18/2005 09:25:42 AM  hatamoto_minoru
  Changed the definition of NNSG2dFont.

  Revision 1.6  03/14/2005 11:54:34 PM  hatamoto_minoru
  tab -> sp

  Revision 1.5  03/10/2005 07:30:58 AM  hatamoto_minoru
  Added functions for obtaining the size of the character string.

  Revision 1.4  02/18/2005 09:26:35 AM  hatamoto_minoru
  Revised where the linefeed was u8.

  Revision 1.3  02/16/2005 04:16:23 AM  hatamoto_minoru
  Deleted the old MAPMETHOD and its related functions.

  Revision 1.2  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

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
#ifndef G2D_FONT_H_
#define G2D_FONT_H_

#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2di_AssertUtil.h>
#include <nnsys/g2d/fmt/g2d_Font_data.h>
#include <nnsys/g2d/g2di_SplitChar.h>
#include <nnsys/g2d/g2di_Char.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------

//*****************************************************************************
// Font macro
//*****************************************************************************

#define NNS_G2D_FONT_ASSERT( pFont )                                            \
    NNS_G2D_ASSERTMSG(                                                          \
        ((pFont) != NULL)                                                       \
            && (*(((u32*)((pFont)->pRes)) - 2) == NNS_G2D_BINBLK_SIG_FINFDATA)  \
            && ((pFont)->pRes->pGlyph != NULL)                                  \
            && NNS_G2D_IS_VALID_POINTER((pFont)->cbCharSpliter)                 \
        , "invalid NNSG2dFont data" )

#define NNS_G2D_GLYPH_ASSERT( pGlyph )                                          \
    NNS_G2D_ASSERTMSG(                                                          \
        ((pGlyph) != NULL)                                                      \
            && ((pGlyph)->image != NULL)                                        \
        , "invalid NNSG2dGlyph data")                                           \

#define NNS_G2D_FONT_MAX_GLYPH_INDEX( pFont )                                   \
    ((*((u32*)(pFont)->pRes->pGlyph - 1) - sizeof(*((pFont)->pRes->pGlyph)))    \
        / (pFont)->pRes->pGlyph->cellSize)                                      \


#define NNS_G2D_GLYPH_INDEX_NOT_FOUND   0xFFFF





//*****************************************************************************
// Font definitions
//*****************************************************************************

// font
#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE
typedef struct NNSG2dFont
{
    NNSG2dFontInformation* pRes;            // Pointer to the expanded font resource
    NNSiG2dSplitCharCallback cbCharSpliter; // Pointer to the character string encoding process callback
    u16 isOldVer;
    u16 widthsSize;
}
NNSG2dFont;
#else // NNS_G2D_FONT_USE_OLD_RESOURCE
typedef struct NNSG2dFont
{
    NNSG2dFontInformation* pRes;            // Pointer to the expanded font resource
    NNSiG2dSplitCharCallback cbCharSpliter; // Pointer to the character string encoding process callback
}
NNSG2dFont;
#endif // NNS_G2D_FONT_USE_OLD_RESOURCE


// glyph
typedef struct NNSG2dGlyph
{
    const NNSG2dCharWidths* pWidths;    // Pointer to the width information
    const u8* image;                    // Pointer to the glyph image
}
NNSG2dGlyph;



// character string rectangle
typedef struct NNSG2dTextRect
{
    int width;      // rectangle width
    int height;     // rectangle height
}
NNSG2dTextRect;





//*****************************************************************************
// Font build
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontInit*

  Description:  Builds the font from the font resource.

  Arguments:    pFont:      Pointer to the font.
                pNftrFile:  Pointer to the font resource.

  Returns:      Returns a value other than FALSE if the font build is successful.
 *---------------------------------------------------------------------------*/
void NNS_G2dFontInitAuto(NNSG2dFont* pFont, void* pNftrFile);
void NNS_G2dFontInitUTF8(NNSG2dFont* pFont, void* pNftrFile);
void NNS_G2dFontInitUTF16(NNSG2dFont* pFont, void* pNftrFile);
void NNS_G2dFontInitShiftJIS(NNSG2dFont* pFont, void* pNftrFile);
void NNS_G2dFontInitCP1252(NNSG2dFont* pFont, void* pNftrFile);





//*****************************************************************************
// Font operations
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontFindGlyphIndex

  Description:  Gets the index of the corresponding glyph from the character code.

  Arguments:    pFont:  Pointer to the font.
                c:      Character code that gets the glyph index.

  Returns:      If a glyph corresponding to c exists, returns its index.
                Returns NNS_G2D_GLYPH_INDEX_NOT_FOUND if one does not exist.
 *---------------------------------------------------------------------------*/
u16 NNS_G2dFontFindGlyphIndex( const NNSG2dFont* pFont, u16 c );



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidthsFromIndex

  Description:  Gets the character width information from the glyph index.

  Arguments:    pFont:  Pointer to the font.
                idx         :    glyph index .

  Returns:      Pointer to the character width information.
 *---------------------------------------------------------------------------*/
const NNSG2dCharWidths* NNS_G2dFontGetCharWidthsFromIndex(
                                        const NNSG2dFont* pFont, u16 idx );





//*****************************************************************************
// Font accessor
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetType

  Description:  Gets the font type.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Font type.
 *---------------------------------------------------------------------------*/
static inline NNSG2dFontType NNS_G2dFontGetType( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return (NNSG2dFontType)pFont->pRes->fontType;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetLineFeed

  Description:  Gets the line feed width of the font.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Font line feed width.
 *---------------------------------------------------------------------------*/
static inline s8 NNS_G2dFontGetLineFeed( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->linefeed;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetAlternateGlyphIndex

  Description:  Gets the alternate character glyph index of the font.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Alternate character glyph index of the font.
 *---------------------------------------------------------------------------*/
static inline u16 NNS_G2dFontGetAlternateGlyphIndex( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->alterCharIndex;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetDefaultCharWidths

  Description:  Gets the default character width information.
                This is used if the default character width information does not contain
                character width information unique to the character.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Pointer to the default character width information.
 *---------------------------------------------------------------------------*/
static inline NNSG2dCharWidths* NNS_G2dFontGetDefaultCharWidths( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return &pFont->pRes->defaultWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetHeight

  Description:  Gets the font height.
                Normally, the height of the glyph image is the height of the font.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Font height.
 *---------------------------------------------------------------------------*/
static inline u8 NNS_G2dFontGetHeight( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->cellHeight;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCellHeight

  Description:  Gets the height of the glyph image.
                The size of the glyph image is shared among all characters in the font.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Glyph image height.
 *---------------------------------------------------------------------------*/
static inline u8 NNS_G2dFontGetCellHeight( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->cellHeight;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCellWidth

  Description:  Gets the glyph image width.
                The size of the glyph image is shared among all characters in the font.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Width of the glyph image.
 *---------------------------------------------------------------------------*/
static inline u8 NNS_G2dFontGetCellWidth( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->cellWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetBaselinePos

  Description:  Gets the baseline position as seen from the upper edge of the glyph image.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Baseline position with the upper edge of the glyph image as 0.
 *---------------------------------------------------------------------------*/
static inline int NNS_G2dFontGetBaselinePos( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT( pFont );
    return pFont->pRes->pGlyph->baselinePos;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetGlyphDataSize

  Description:  Gets the byte size per 1 glyph image character.
                The size of the glyph image is shared among all characters in the font.

                This will be equal to (CellWidth * CellHeight * bpp + 7) / 8

  Arguments:    pFont:  Pointer to the font.

  Returns:      Byte size per 1 glyph image character.
 *---------------------------------------------------------------------------*/
static inline int NNSi_G2dFontGetGlyphDataSize( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->cellSize;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetMaxCharWidth

  Description:  Gets the maximum character width in the font.

  Arguments:    pFont:  Pointer to the font.

  Returns:      The maximum character width in the font.
 *---------------------------------------------------------------------------*/
static inline u8 NNS_G2dFontGetMaxCharWidth( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->maxCharWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetBpp

  Description:  Gets the number of bits in 1 dot of the glyph image.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Number of bits in 1 dot of the glyph image.
 *---------------------------------------------------------------------------*/
static inline u8 NNS_G2dFontGetBpp( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->pRes->pGlyph->bpp;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetEncoding

  Description:  Gets the character string encoding that corresponds to the font.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Character string encoding that corresponds to the font.
 *---------------------------------------------------------------------------*/
static inline NNSG2dFontEncoding NNSi_G2dFontGetEncoding( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return (NNSG2dFontEncoding)pFont->pRes->encoding;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetSpliter

  Description:  Gets the pointer to the character string encoding
                process callback function.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Pointer to the character string encoding process callback function.
 *---------------------------------------------------------------------------*/
static inline NNSiG2dSplitCharCallback NNSi_G2dFontGetSpliter( const NNSG2dFont* pFont )
{
    NNS_G2D_FONT_ASSERT(pFont);
    return pFont->cbCharSpliter;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyphImageFromIndex

  Description:  Gets the glyph image from the glyph index.

  Arguments:    pFont:  Pointer to the font.
                idx         :    glyph index .

  Returns:      Pointer to the glyph image.
 *---------------------------------------------------------------------------*/
static inline const u8* NNS_G2dFontGetGlyphImageFromIndex(
                                        const NNSG2dFont* pFont, u16 idx )
{
    NNS_G2D_FONT_ASSERT(pFont);
    NNS_G2D_ASSERT( idx < NNS_G2D_FONT_MAX_GLYPH_INDEX(pFont) );
    return pFont->pRes->pGlyph->glyphTable + idx * NNSi_G2dFontGetGlyphDataSize(pFont);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontSetLineFeed

  Description:  Configures the line feed width of the font.

  Arguments:    pFont:      Pointer to the font.
                linefeed:   New line feed width.

  Returns:      Font line feed width.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dFontSetLineFeed( NNSG2dFont* pFont, s8 linefeed )
{
    NNS_G2D_FONT_ASSERT(pFont);
    pFont->pRes->linefeed = linefeed;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontSetDefaultCharWidths

  Description:  Configures the default character width information.

  Arguments:    pFont:  Pointer to the font.
                gw:     New default character width information

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dFontSetDefaultCharWidths(
                                NNSG2dFont* pFont, NNSG2dCharWidths cw)
{
    NNS_G2D_FONT_ASSERT(pFont);
    pFont->pRes->defaultWidth = cw;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontSetAlternateGlyphIndex

  Description:  Configures the alternate character glyph index for the font.

  Arguments:    pFont:  Pointer to the font.
                idx         :    New alternate character glyph index.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dFontSetAlternateGlyphIndex( NNSG2dFont* pFont, u16 idx )
{
    NNS_G2D_FONT_ASSERT(pFont);
    NNS_G2D_ASSERT( idx < NNS_G2D_FONT_MAX_GLYPH_INDEX(pFont) );
    pFont->pRes->alterCharIndex = idx;
}





//*****************************************************************************
// inline function
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyphIndex

  Description:  Gets the glyph index from the character code.

  Arguments:    pFont:  Pointer to the font.
                c:      character code.

  Returns:      glyph index .
 *---------------------------------------------------------------------------*/
static inline u16 NNS_G2dFontGetGlyphIndex( const NNSG2dFont* pFont, u16 c )
{
    const u16 idx = NNS_G2dFontFindGlyphIndex(pFont, c);

    return (idx != NNS_G2D_GLYPH_INDEX_NOT_FOUND) ?
                idx :
                pFont->pRes->alterCharIndex ;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyphFromIndex

  Description:  Gets the glyph from the glyph index.

  Arguments:    pGlyph: Pointer to the buffer that stores the glyph.
                pFont:  Pointer to the font.
                idx         :    Glyph index of the glyph to obtain.

  Returns:      Pointer to the glyph.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dFontGetGlyphFromIndex(
                        NNSG2dGlyph* pGlyph, const NNSG2dFont* pFont, u16 idx )
{
    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( pGlyph );
    pGlyph->pWidths = NNS_G2dFontGetCharWidthsFromIndex(pFont, idx);
    pGlyph->image = NNS_G2dFontGetGlyphImageFromIndex(pFont, idx);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidths

  Description:  Gets the character width information from the character code.

  Arguments:    pFont:  Pointer to the font.
                c:      Character's character code that gets the character width information.

  Returns:      Character width information
 *---------------------------------------------------------------------------*/
static inline const NNSG2dCharWidths* NNS_G2dFontGetCharWidths(
                                            const NNSG2dFont* pFont, u16 c )
{
    u16 iGlyph;

    NNS_G2D_FONT_ASSERT( pFont );

    iGlyph = NNS_G2dFontGetGlyphIndex( pFont, c );
    return NNS_G2dFontGetCharWidthsFromIndex( pFont, iGlyph );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidth

  Description:  Gets the character width from the character code.

  Arguments:    pFont:  Pointer to the font.
                c:      Character's character code that gets the character width.

  Returns:      Character width in pixel units.
 *---------------------------------------------------------------------------*/
static inline int NNS_G2dFontGetCharWidth( const NNSG2dFont* pFont, u16 c )
{
    const NNSG2dCharWidths* pWidths;

    NNS_G2D_FONT_ASSERT( pFont );

    pWidths = NNS_G2dFontGetCharWidths(pFont, c);
#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE
    if( pFont->isOldVer )
    {
        return pWidths->left + pWidths->glyphWidth;
    }
    else
#endif
    return pWidths->charWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidthFromIndex

  Description:  Gets the character width from the glyph index.

  Arguments:    pFont:  Pointer to the font.
                idx         :    Character glyph index that gets the character width.

  Returns:      Character width in pixel units.
 *---------------------------------------------------------------------------*/
static inline int NNS_G2dFontGetCharWidthFromIndex( const NNSG2dFont* pFont, u16 idx )
{
    const NNSG2dCharWidths* pWidths;

    NNS_G2D_FONT_ASSERT( pFont );

    pWidths = NNS_G2dFontGetCharWidthsFromIndex(pFont, idx);
#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE
    if( pFont->isOldVer )
    {
        return pWidths->left + pWidths->glyphWidth;
    }
    else
#endif
    return pWidths->charWidth;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyphImage

  Description:  Gets the glyph image from the character code.

  Arguments:    pFont:  Pointer to the font.

  Returns:      Pointer to the glyph image.
 *---------------------------------------------------------------------------*/
static inline const u8* NNS_G2dFontGetGlyphImage( const NNSG2dFont* pFont, u16 c )
{
    u16 iGlyph;

    NNS_G2D_FONT_ASSERT( pFont );

    iGlyph = NNS_G2dFontGetGlyphIndex( pFont, c );
    return NNS_G2dFontGetGlyphImageFromIndex( pFont, iGlyph );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetGlyph

  Description:  Gets the glyph from the character code.

  Arguments:    pGlyph: Pointer to the buffer that stores the glyph.
                pFont:  Pointer to the font.
                ccode:  Character's character code that gets the glyph.

  Returns:      Pointer to the glyph.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dFontGetGlyph(
                    NNSG2dGlyph* pGlyph, const NNSG2dFont* pFont, u16 ccode )
{
    u16 iGlyph;

    NNS_G2D_FONT_ASSERT( pFont );
    NNS_G2D_POINTER_ASSERT( pGlyph );

    iGlyph = NNS_G2dFontGetGlyphIndex(pFont, ccode);
    NNS_G2dFontGetGlyphFromIndex(pGlyph, pFont, iGlyph);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontSetAlternateChar

  Description:  Puts an alternate character in place of a designated character code character.

  Arguments:    pFont:  Pointer to the font.
                c:      New alternate character code of the character.

  Returns:      TRUE if the alternate character replacement succeeds.
                Fails if there is no character corresponding to c inside the font.
 *---------------------------------------------------------------------------*/
static inline BOOL NNS_G2dFontSetAlternateChar( NNSG2dFont* pFont, u16 c )
{
    u16 iGlyph;

    NNS_G2D_FONT_ASSERT(pFont);

    iGlyph = NNS_G2dFontFindGlyphIndex(pFont, c);

    if( iGlyph == NNS_G2D_GLYPH_INDEX_NOT_FOUND )
    {
        return FALSE;
    }

    pFont->pRes->alterCharIndex = iGlyph;
    return TRUE;
}






/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFontGetCharWidthFromIndex

  Description:  Puts an alternate character in place of a designated character code character.

  Arguments:    pFont:  Pointer to the font.
                c:      New alternate character code of the character.

  Returns:      TRUE if the alternate character replacement succeeds.
                Fails if there is no character corresponding to c inside the font.
 *---------------------------------------------------------------------------*/





//*****************************************************************************
// inline function
//*****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetStringWidth

  Description:  Looks for the width when 1 line of a character string is rendered under specified conditions.
                At the same time, this gets a pointer to the terminal position of the line.

  Arguments:    pFont:      Pointer to the font used in the calcluation of the character string width.
                hSpace:     Space between characters in pixels.
                str:        Pointer to the character string that looks for the character string width.
                pPos:       Pointer to the buffer that receives the pointer to the line end point position.
                            A NULL can be designated here.

  Returns:      Width in pixels when one line worth of character string is rendered from str.
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetStringWidth(
    const NNSG2dFont* pFont,
    int hSpace,
    const void* str,
    const void** pPos
);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetTextHeight

  Description:  Looks for the height when a character string is rendered under specified conditions.

  Arguments:    pFont:  Pointer to the font used in the calcluation of the character string width.
                vSpace: Space between lines in pixels.
                txt:    Pointer to the character string that looks for the character string width.

  Returns:      The height in pixels when a character string is rendered.
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetTextHeight(
    const NNSG2dFont* pFont,
    int vSpace,
    const void* txt
);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetTextWidth

  Description:  Looks for the width when a character string is rendered under designated conditions.

  Arguments:    pFont:  Pointer to the font used in the calcluation of the character string width.
                hSpace: Space between characters in pixels.
                txt:    Pointer to the character string that looks for the character string width.

  Returns:      Width in pixels when the character string is rendered.
                This means the maximum width from the widths of each line.
 *---------------------------------------------------------------------------*/
int NNSi_G2dFontGetTextWidth(
    const NNSG2dFont* pFont,
    int hSpace,
    const void* txt
);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dFontGetTextRect

  Description:  Looks for the width and height when a character string is rendered under specified conditions.

  Arguments:    pFont:  Pointer to the font used in the calcluation of the character string width.
                hSpace: Space between characters in pixels.
                vSpace: Space between lines in pixels.
                txt:    Pointer to the character string that looks for the character string width.

  Returns:      Structure that stores the width and height in pixels, when the text string is drawn. 
                
 *---------------------------------------------------------------------------*/
NNSG2dTextRect NNSi_G2dFontGetTextRect(
    const NNSG2dFont* pFont,
    int hSpace,
    int vSpace,
    const void* txt
);



//---------------------------------------------------------------------

static inline int NNS_G2dFontGetStringWidth( const NNSG2dFont* pFont, int hSpace, const NNSG2dChar* str, const NNSG2dChar** pPos )
    { return NNSi_G2dFontGetStringWidth(pFont, hSpace, str, (const void**)pPos); }
static inline int NNS_G2dFontGetTextHeight( const NNSG2dFont* pFont, int vSpace, const NNSG2dChar* txt )
    { return NNSi_G2dFontGetTextHeight(pFont, vSpace, txt); }
static inline int NNS_G2dFontGetTextWidth( const NNSG2dFont* pFont, int hSpace, const NNSG2dChar* txt )
    { return NNSi_G2dFontGetTextWidth(pFont, hSpace, txt); }
static inline NNSG2dTextRect NNS_G2dFontGetTextRect( const NNSG2dFont* pFont, int hSpace, int vSpace, const NNSG2dChar* txt )
    { return NNSi_G2dFontGetTextRect(pFont, hSpace, vSpace, txt); }

//---------------------------------------------------------------------




//---------------------------------------------------------------------

#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // G2D_FONT_H_


