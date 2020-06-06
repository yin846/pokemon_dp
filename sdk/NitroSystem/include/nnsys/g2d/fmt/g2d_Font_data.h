/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  File:     g2d_Font_data.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Font_data.h,v $
  Revision 1.7  06/09/2005 08:36:19 AM  hatamoto_minoru
  Revise the structure definiton for the old version. 

  Revision 1.6  05/30/2005 10:14:19 AM  hatamoto_minoru
  Added support for the older versions.

  Revision 1.5  05/26/2005 01:59:14 AM  hatamoto_minoru
  Changed the version number to 1.0.

  Revision 1.4  05/25/2005 08:10:00 AM  hatamoto_minoru
  Changed the formatting of the fonts. Added 1 element to the character width information.

  Revision 1.3  05/24/2005 12:07:10 AM  hatamoto_minoru
  Made changes so that the warning pragma works only on vcc.

  Revision 1.2  05/23/2005 06:14:49 AM  hatamoto_minoru
  Added ISO-8859-1 to the encodings.

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.6  03/18/2005 09:22:50 AM  hatamoto_minoru
  Added encoding information to FontInformation.

  Revision 1.5  03/14/2005 11:54:35 PM  hatamoto_minoru
  tab -> sp

  Revision 1.4  02/16/2005 04:16:23 AM  hatamoto_minoru
  Deleted the old MAPMETHOD and its related functions.

  Revision 1.3  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.2  01/31/2005 12:29:39 AM  hatamoto_minoru
  Changed the linefeed from u8 to s8.

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.2  12/10/2004 10:15:02 AM  hatamoto_minoru
  Added comments and ASSERT.

  Revision 1.1  11/30/2004 09:17:04 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_FONT_DATA_H_
#define NNS_G2D_FONT_DATA_H_

//#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifdef _MSC_VER // for VC
    #pragma warning( disable: 4200 )
	#pragma warning( disable: 4201 )
#endif  //_MSC_VER

#ifndef SDK_ARM9
    #include <nitro_win32.h>
#endif //!SDK_ARM9

#ifdef __cplusplus
extern "C" {
#endif



//------------------------------------------------------------------------------
// Signature definition
//------------------------------------------------------------------------------

// Binary file signature
#define NNS_G2D_BINFILE_SIG_FONTDATA          (u32)'NFTR'

// Binary block signature
#define NNS_G2D_BINBLK_SIG_FINFDATA           (u32)'FINF'
#define NNS_G2D_BINBLK_SIG_CGLPDATA           (u32)'CGLP'
#define NNS_G2D_BINBLK_SIG_TGLPDATA           (u32)'TGLP'
#define NNS_G2D_BINBLK_SIG_TCLPDATA           (u32)'TCLP'
#define NNS_G2D_BINBLK_SIG_CWDHDATA           (u32)'CWDH'
#define NNS_G2D_BINBLK_SIG_CMAPDATA           (u32)'CMAP'


#define NNS_G2D_BINFILE_EXT_FONTDATA          "NFTR"





//------------------------------------------------------------------------------
// Resource version definitions
//------------------------------------------------------------------------------

//
// Version information
// Ver         Changed
// -------------------------------------
// 1.0         Initial
//
#define NNS_G2D_NFTR_MAJOR_VER              (u8)1
#define NNS_G2D_NFTR_MINOR_VER              (u8)0

#define NNS_G2D_NFTR_VER        (u16)((NNS_G2D_NFTR_MAJOR_VER << 8) | NNS_G2D_NFTR_MINOR_VER)




//------------------------------------------------------------------------------
// enum definitions
//------------------------------------------------------------------------------

// font type
typedef enum NNSG2dFontType
{
    NNS_G2D_FONTTYPE_GLYPH,     // BMP
    NNS_G2D_NUM_OF_FONTTYPE
}
NNSG2dFontType;

// character string encoding type
typedef enum NNSG2dFontEncoding
{
    NNS_G2D_FONT_ENCODING_UTF8,			// UTF-8
    NNS_G2D_FONT_ENCODING_UTF16,		// UTF-16
    NNS_G2D_FONT_ENCODING_SJIS,			// ShiftJIS
	NNS_G2D_FONT_ENCODING_CP1252,	    // CP1252
    NNS_G2D_NUM_OF_ENCODING
}
NNSG2dFontEncoding;

// character code map type
typedef enum NNSG2dFontMappingMethod
{
    NNS_G2D_MAPMETHOD_DIRECT,       // index  = character code - offset
    NNS_G2D_MAPMETHOD_TABLE,        // index  = mapInfo[character code - offset ]
    NNS_G2D_MAPMETHOD_SCAN,         // index  = search(mapInfo, character code)
    NNS_G2D_NUM_OF_MAPMETHOD
}
NNSG2dFontMappingMethod;



//------------------------------------------------------------------------------
// structure definitions
//------------------------------------------------------------------------------
// character width information
typedef struct NNSG2dCharWidths
{
    s8 left;                            // left space width of character
    u8 glyphWidth;                      // glyph width of character
    s8 charWidth;                       // character width  = left space width  + glyph width  + right space width
}
NNSG2dCharWidths;

// character code and glyph index pair
typedef struct NNSG2dCMapScanEntry
{
    u16 ccode;                          // character code
    u16 index;                          // glyph index
}
NNSG2dCMapScanEntry;

// NNSG2dFontCodeMap.mapInfo for NNS_G2D_MAPMETHOD_SCAN
typedef struct NNSG2dCMapInfoScan
{
    u16 num;                            // number of entry elements
    NNSG2dCMapScanEntry entries[];      // mapping list to the glyph index from the character code
}
NNSG2dCMapInfoScan;





//------------------------------------------------------------------------------
// font resource
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSG2dFontGlyph

  Description:  Stores the glyph BMP of the font.
                All glyph BMPs have the same width, height, and data size,
                so they can be accessed in an array.
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dFontGlyph
{
    u8  cellWidth;          // number of pixels in 1 glyph image line
    u8  cellHeight;         // number of glyph image lines
    u16 cellSize;           // glyph image data size per 1 glyph
    s8  baselinePos;        // (*) distance from the top edge of the glyph image to the baseline
    u8  maxCharWidth;       // (*)maximum glyph width
    u8  bpp;                // number of bits per 1 glyph image pixel
    u8  reserved;           // reserved

    u8  glyphTable[];       // glyph image array
}
NNSG2dFontGlyph;



/*---------------------------------------------------------------------------*
  Name:         NNSG2dFontWidth

  Description:  Stores the character width information for each character.
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dFontWidth
{
    u16                     indexBegin;     // glyph index corresponding to the first entry in widthTable
    u16                     indexEnd;       // glyph index corresponding to the last entry in widthTable
    struct NNSG2dFontWidth* pNext;          // Pointer to the next NNSG2dFontWidth
    NNSG2dCharWidths        widthTable[];   // width information array
}
NNSG2dFontWidth;



/*---------------------------------------------------------------------------*
  Name:         NNSG2dFontCodeMap

  Description:  Provides mapping to the index in the glyph image array of the glyph
                that corresponds to the character code from the character code.
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dFontCodeMap
{
    u16                         ccodeBegin;     // first character code in the character code boundary that manages this block
    u16                         ccodeEnd;       // last character code in the character code boundary that manages this block
    u16                         mappingMethod;  // mapping method (NNSG2dFontMappingMethod type)
    u16                         reserved;       // reserved
    struct NNSG2dFontCodeMap*   pNext;          // pointer to the next NNSG2dFontCodeMap
    u16                         mapInfo[];      // the specific details of the character code mapping information depend on the mappingMethod
}
NNSG2dFontCodeMap;



/*---------------------------------------------------------------------------*
  Name:         NNSG2dFontInformation

  Description:  Stores the information passed to the overall font.
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dFontInformation
{
    u8                  fontType;       // glyph data type  (NNSG2dFontType type)
    s8                  linefeed;       // (*)== leading
    u16                 alterCharIndex; // (*) glyph index of the glyph for characters not contained in the font
    NNSG2dCharWidths    defaultWidth;   // (*)character width information for glyphs that have no character width information
    u8                  encoding;       // corresponding character string encoding  (NNSG2dFontEncoding type)
    NNSG2dFontGlyph*    pGlyph;         // Pointer to a unique NNSG2dFontGlyph
    NNSG2dFontWidth*    pWidth;         // Pointer to the first NNSG2dFontGlyph
    NNSG2dFontCodeMap*  pMap;           // Pointer to the first NNSG2dFontCodeMap
}
NNSG2dFontInformation;




//------------------------------------------------------------------------------
// NITRO-System binary file block
//------------------------------------------------------------------------------

typedef struct NNSG2dFontInformationBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontInformation       blockBody;
}
NNSG2dFontInformationBlock;

typedef struct NNSG2dFontGlyphBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontGlyph             blockBody;
}
NNSG2dFontGlyphBlock;

typedef struct NNSG2dFontWidthBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontWidth             blockBody;
}
NNSG2dFontWidthBlock;

typedef struct NNSG2dFontCodeMapBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontCodeMap           blockBody;
}
NNSG2dFontCodeMapBlock;

//------------------------------------------------------------------------------


#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE

#define NNS_G2D_NFTR_PREV_MAJOR_VER         (u8)0
#define NNS_G2D_NFTR_PREV_MINOR_VER         (u8)1

#define NNS_G2D_NFTR_PREV_VER   (u16)((NNS_G2D_NFTR_PREV_MAJOR_VER << 8) | NNS_G2D_NFTR_PREV_MINOR_VER)

typedef struct NNSG2dCharWidthsOld
{
    s8 left;                            // left space width of character
    u8 width;                           // glyph width of character
}
NNSG2dCharWidthsOld;

typedef struct NNSG2dFontWidthOld
{
    u16                         indexBegin;     // glyph index corresponding to the first entry in widthTable
    u16                         indexEnd;       // glyph index corresponding to the last entry in widthTable
    struct NNSG2dFontWidthOld*  pNext;          // Pointer to the next NNSG2dFontWidth
    NNSG2dCharWidthsOld         widthTable[];   // width information array
}
NNSG2dFontWidthOld;

typedef struct NNSG2dFontInformationOld
{
    u8                  fontType;       // glyph data type  (NNSG2dFontType type)
    s8                  linefeed;       // (*)== leading
    u16                 alterCharIndex; // (*) glyph index of the glyph for characters not contained in the font
    NNSG2dCharWidthsOld defaultWidth;   // (*)character width information for glyphs that have no character width information
    u8                  encoding;       // corresponding character string encoding  (NNSG2dFontEncoding type)
    u8                  reserved;       // (reserved)
    NNSG2dFontGlyph*    pGlyph;         // Pointer to a unique NNSG2dFontGlyph
    NNSG2dFontWidthOld* pWidth;         // Pointer to the first NNSG2dFontWidthOld
    NNSG2dFontCodeMap*  pMap;           // Pointer to the first NNSG2dFontCodeMap
}
NNSG2dFontInformationOld;

typedef struct NNSG2dFontInformationBlockOld
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontInformationOld    blockBody;
}
NNSG2dFontInformationBlockOld;

typedef struct NNSG2dFontWidthBlockOld
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dFontWidthOld          blockBody;
}
NNSG2dFontWidthBlockOld;

#endif  // NNS_G2D_FONT_USE_OLD_RESOURCE




#ifdef __cplusplus
} /* extern "C"*/
#endif

#ifdef _MSC_VER
    #pragma warning( default: 4200 )
    #pragma warning( default: 4201 )
#endif  //_MSC_VER

#endif // NNS_G2D_FONT_DATA_H_

