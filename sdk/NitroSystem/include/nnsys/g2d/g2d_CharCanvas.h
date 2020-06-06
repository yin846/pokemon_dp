/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  File:     g2d_CharCanvas.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_CharCanvas.h,v $
  Revision 1.2  07/12/2005 01:17:24 PM  hatamoto_minoru
  Added cell build functions.

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.8  03/18/2005 09:26:29 AM  hatamoto_minoru
  Revised the function type names.

  Revision 1.7  03/10/2005 07:31:34 AM  hatamoto_minoru
  Added and changed comments.

  Revision 1.6  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.5  02/01/2005 11:42:48 AM  hatamoto_minoru
  Deleted unnecessary macros.

  Revision 1.4  01/27/2005 12:42:43 AM  hatamoto_minoru
  Deleted the OBJ1D substitute.

  Revision 1.3  01/25/2005 01:09:25 PM  hatamoto_minoru
  Revised where NNS_G2dArrangeOBJ1D() did not support VRAM modes other then 32K.

  Revision 1.2  01/25/2005 01:47:02 AM  hatamoto_minoru
  Added functions for OBJ placement.

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.4  12/10/2004 10:15:02 AM  hatamoto_minoru
  Added comments and ASSERT.

  Revision 1.3  12/06/2004 01:12:48 PM  hatamoto_minoru
  NNS_G2dPutString -> NNS_G2dTextCanvasDrawString

  Revision 1.2  12/06/2004 01:02:03 PM  hatamoto_minoru
  Organized the source and added comments.

  Revision 1.1  11/30/2004 09:19:28 AM  hatamoto_minoru
  Separated the character render routine from g2d_CharCanvas, and
  separated the render character string routine it used from g2d_TextScreen.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef G2D_CHARCANVAS_H_
#define G2D_CHARCANVAS_H_

#include <nnsys/g2d/g2d_Font.h>
#include <nnsys/g2d/g2di_AssertUtil.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------

//---------------------------------------------------------------------
// CharCanvas macros
//---------------------------------------------------------------------

#define NNS_G2D_CHARCANVAS_ASSERT( pCC )                                \
    NNS_G2D_ASSERTMSG(                                                  \
        NNS_G2D_IS_VALID_POINTER(pCC)                                   \
        && (0 < (pCC)->areaWidth)                                       \
        && (0 < (pCC)->areaHeight)                                      \
        && ( ((pCC)->dstBpp == 4) || ((pCC)->dstBpp == 8) )             \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->charBase )                  \
        && NNS_G2D_IS_ALIGNED((pCC)->charBase, 4)                       \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->pDrawGlyph )                \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->pClear )                    \
        && NNS_G2D_IS_VALID_POINTER( (pCC)->pClearArea )                \
        , "Illegal NNSG2dCharCanvas.")

#define NNS_G2D_COLORMODE_ASSERT( cmode )                               \
    NNS_G2D_ASSERTMSG(                                                  \
        (cmode) == NNS_G2D_CHARA_COLORMODE_16                           \
        || (cmode) == NNS_G2D_CHARA_COLORMODE_256                       \
        , "Illegal Color Mode(=%d)", (cmode) )

#define NNS_G2D_OBJVRAMMODE_ASSERT( vmode )                             \
    NNS_G2D_ASSERTMSG(                                                  \
        (vmode) == NNS_G2D_OBJVRAMMODE_32K                              \
        || (vmode) == NNS_G2D_OBJVRAMMODE_64K                           \
        || (vmode) == NNS_G2D_OBJVRAMMODE_128K                          \
        || (vmode) == NNS_G2D_OBJVRAMMODE_256K                          \
        , "Illegal VRAM Mode(=%d)", (vmode) )

#define NNS_G2D_TEXT_BG_WIDTH_ASSERT( width )                           \
    NNS_G2D_ASSERTMSG(                                                  \
        (width) == NNS_G2D_TEXT_BG_WIDTH_256                            \
        || (width) == NNS_G2D_TEXT_BG_WIDTH_512                         \
        , "Illegal Text BG Width(=%d).", (width) )

#define NNS_G2D_AFFINE_BG_WIDTH_ASSERT( width )                         \
    NNS_G2D_ASSERTMSG(                                                  \
        (width) == NNS_G2D_AFFINE_BG_WIDTH_128                          \
        || (width) == NNS_G2D_AFFINE_BG_WIDTH_256                       \
        || (width) == NNS_G2D_AFFINE_BG_WIDTH_512                       \
        || (width) == NNS_G2D_AFFINE_BG_WIDTH_1024                      \
        , "Illegal Affine BG Width(=%d).", (width) )

#define NNS_G2D_256x16PLTT_BG_WIDTH_ASSERT( width )                     \
    NNS_G2D_ASSERTMSG(                                                  \
        (width) == NNS_G2D_256x16PLTT_BG_WIDTH_128                      \
        || (width) == NNS_G2D_256x16PLTT_BG_WIDTH_256                   \
        || (width) == NNS_G2D_256x16PLTT_BG_WIDTH_512                   \
        || (width) == NNS_G2D_256x16PLTT_BG_WIDTH_1024                  \
        , "Illegal 256x16Pltt BG Width(=%d).", (width) )




// TEXT BG width
typedef enum NNSG2dTextBGWidth
{
    NNS_G2D_TEXT_BG_WIDTH_256   = 32,
    NNS_G2D_TEXT_BG_WIDTH_512   = 64
}
NNSG2dTextBGWidth;

// Affine BG width
typedef enum NNSG2dAffineBGWidth
{
    NNS_G2D_AFFINE_BG_WIDTH_128     = 16,
    NNS_G2D_AFFINE_BG_WIDTH_256     = 32,
    NNS_G2D_AFFINE_BG_WIDTH_512     = 64,
    NNS_G2D_AFFINE_BG_WIDTH_1024    = 128
}
NNSG2dAffineBGWidth;

// affine expansion 256x16 palette type BG width
typedef enum NNSG2d256x16PlttBGWidth
{
    NNS_G2D_256x16PLTT_BG_WIDTH_128     = 16,
    NNS_G2D_256x16PLTT_BG_WIDTH_256     = 32,
    NNS_G2D_256x16PLTT_BG_WIDTH_512     = 64,
    NNS_G2D_256x16PLTT_BG_WIDTH_1024    = 128
}
NNSG2d256x16PlttBGWidth;

// Character color mode
typedef enum NNSG2dCharaColorMode
{
    NNS_G2D_CHARA_COLORMODE_16 = 4,
    NNS_G2D_CHARA_COLORMODE_256 = 8
}
NNSG2dCharaColorMode;

// Size that the OBJ character region can reference
typedef enum NNSG2dOBJVramMode
{
    NNS_G2D_OBJVRAMMODE_32K     = 0,
    NNS_G2D_OBJVRAMMODE_64K     = 1,
    NNS_G2D_OBJVRAMMODE_128K    = 2,
    NNS_G2D_OBJVRAMMODE_256K    = 3
}
NNSG2dOBJVramMode;

//---------------------------------------------------------------------
// CharCanvas definitions
//---------------------------------------------------------------------


struct NNSG2dCharCanvas;

// CharCanvas character rendering function definitions
typedef void (*NNSiG2dDrawGlyphFunc)(
    const struct NNSG2dCharCanvas* pCC,
    const NNSG2dFont* pFont,
    int x,
    int y,
    int cl,
    const NNSG2dGlyph* pGlyph
);

// CharClear canvasfunction definitions
typedef void (*NNSiG2dClearFunc)(
    const struct NNSG2dCharCanvas* pCC,
    int cl
);

// CharCanvas sectional clearing function definitions
typedef void (*NNSiG2dClearAreaFunc)(
    const struct NNSG2dCharCanvas* pCC,
    int cl,
    int x,
    int y,
    int w,
    int h
);

// CharCanvas structure
typedef struct NNSG2dCharCanvas
{
    u8* charBase;
    int areaWidth;
    int areaHeight;
    u8 dstBpp;
    u8 reserved[3];
    u32 param;
    NNSiG2dDrawGlyphFunc pDrawGlyph;
    NNSiG2dClearFunc pClear;
    NNSiG2dClearAreaFunc pClearArea;
}
NNSG2dCharCanvas;





//****************************************************************************
// BG screen structure functions
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMapScrToCharText

  Description:  Matches up the characters from the screen on a one-to-one basis for CharCanvas.
                This is for the text BG.

  Arguments:    scnBase:    Pointer to the screen base of the
                            BG screen the CharCanvas is using.
                areaWidth:  CharCanvas width in character units.
                areaHeight: CharCanvas height in character units.
                areaLeft:   CharCanvas upper left x coordinate when BG screen top left is (0,0)
                            
                areaTop:    CharCanvas upper left y coordinate when BG screen's top left is (0,0)
                             
                scnWidth:   BG screen width
                charNo:     Pointer to the top character of the character string CharCanvas
                            string allocated to CharCanvas.
                cplt:       Color palette number designated in the screen.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dMapScrToCharText(
    void* scnBase,
    int areaWidth,
    int areaHeight,
    int areaLeft,
    int areaTop,
    NNSG2dTextBGWidth scnWidth,
    int charNo,
    int cplt
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMapScrToCharAffine

  Description:  Matches up the characters from the screen on a one-to-one basis for CharCanvas.
                This is for the affine BG.

  Arguments:    areaBase:   Pointer to the screen that corresponds to the CharCanvas upper left.
                areaWidth:  CharCanvas width in character units.
                areaHeight: CharCanvas height in character units.
                scnWidth:   BG screen width
                charNo:     Pointer to the top character of the character string CharCanvas
                            string allocated to CharCanvas.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dMapScrToCharAffine(
    void* areaBase,
    int areaWidth,
    int areaHeight,
    NNSG2dAffineBGWidth scnWidth,
    int charNo
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMapScrToChar256x16Pltt

  Description:  Matches up the characters from the screen on a one-to-one basis for CharCanvas.
                This is for the affine expanded BG 256 x 16 palette type.

  Arguments:    areaBase:   Pointer to the screen that corresponds to the CharCanvas upper left.
                areaWidth:  CharCanvas width in character units.
                areaHeight: CharCanvas height in character units.
                scnWidth:   BG screen width
                charNo:     Pointer to the top character of the character string CharCanvas
                            string allocated to CharCanvas.
                cplt:       Color palette number designated in the screen.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dMapScrToChar256x16Pltt(
    void* areaBase,
    int areaWidth,
    int areaHeight,
    NNSG2d256x16PlttBGWidth scnWidth,
    int charNo,
    int cplt
);





//****************************************************************************
// OBJ array functions
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dCalcRequiredOBJ
                NNS_G2dCalcRequiredOBJ1D
                NNS_G2dCalcRequiredOBJ2DRect

  Description:  Calculates the number of OBJ needed by NNS_G2dArrangeOBJ*.
                If this is the same size as CharCanvas, NNS_G2dArrangeOBJ1D and
                NNS_G2dArrangeOBJ2DRect will arrange the OBJ in the same way.
                Therefore,  NNS_G2dCalcRequireOBJ1D and NNS_G2dCalcRequireOBJ2DRect
                are internally identical.

  Arguments:    areaWidth:  CharCanvas width in characters that calculates the number of OBJ.
                areaHeight: CharCanvas height in characters that calculates the number of OBJ.

  Returns:      The number of OBJ needed.
 *---------------------------------------------------------------------------*/
int NNSi_G2dCalcRequiredOBJ(
    int areaWidth,
    int areaHeight
);

static inline int NNS_G2dCalcRequiredOBJ1D(
    int areaWidth,
    int areaHeight
)
{
    return NNSi_G2dCalcRequiredOBJ(areaWidth, areaHeight);
}

static inline int NNS_G2dCalcRequiredOBJ2DRect(
    int areaWidth,
    int areaHeight
)
{
    return NNSi_G2dCalcRequiredOBJ(areaWidth, areaHeight);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dArrangeOBJ1D

  Description:  Arranges the OBJ so that the OBJ rendered with
                NNS_G2dCharCanvasInitForOBJ1D can be displayed appropriately.
                This uses a number of OBJ equal to the oam
                starting point NNS_G2dCalcRequireOBJ1D(areaWidth, areaHeight).

  Arguments:    oam:        Pointer to the starting point of the OAM string to be used.
                areaWidth:  CharCanvas width in character units.
                areaHeight: CharCanvas height in character units.
                x:          CharCanvas upper left corner display position.
                y:          CharCanvas upper left corner display position.
                color:      CharCanvas color mode.
                charName:   Character name at the top of the character string that the OBJ string uses in display.
                vramMode:   OBJ VRAM capacity

  Returns:
 *---------------------------------------------------------------------------*/
int NNS_G2dArrangeOBJ1D(
    GXOamAttr * oam,
    int areaWidth,
    int areaHeight,
    int x,
    int y,
    GXOamColorMode color,
    int charName,
    NNSG2dOBJVramMode vramMode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dArrangeOBJ2DRect

  Description:  Arranges the OBJ so that the OBJ rendered with
                NNS_G2dCharCanvasInitForOBJ1D can be displayed appropriately.
                This uses a number of OBJ equal to the oam
                starting point NNS_G2dCalcRequireOBJ1D(areaWidth, areaHeight).

  Arguments:    oam:        Pointer to the starting point of the OAM string to be used.
                areaWidth:  CharCanvas width in character units.
                areaHeight: CharCanvas height in character units.
                x:          CharCanvas upper left corner display position.
                y:          CharCanvas upper left corner display position.
                color:      CharCanvas color mode.
                charName:   Character name at the top of the character string that the OBJ string uses in display.

  Returns:
 *---------------------------------------------------------------------------*/
int NNS_G2dArrangeOBJ2DRect(
    GXOamAttr * oam,
    int areaWidth,
    int areaHeight,
    int x,
    int y,
    GXOamColorMode color,
    int charName
);





//****************************************************************************
// CharCanvas operations
//****************************************************************************

// Drawing

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasDrawChar

  Description:  Renders 1 character specified by the character code on the CharCanvas.

  Arguments:    pCC:    Pointer to CharCanvas.
                pFont:  Pointer to the font used in rendering.
                x:      Upper left character coordinate
                y:      Upper left character coordinate
                cl:     Color number of character color.
                pGlyph: Character code of the character to render.

  Returns:      Character render width.
 *---------------------------------------------------------------------------*/
int NNS_G2dCharCanvasDrawChar(
    const NNSG2dCharCanvas* pCC,
    const NNSG2dFont* pFont,
    int x,
    int y,
    int cl,
    u16 ccode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasDrawGlyph

  Description:  Renders a designated glyph to CharCanvas.

  Arguments:    pCC:    Pointer to CharCanvas.
                pFont:  Pointer to the font used in rendering.
                x:      Upper left character coordinate.
                y:      Upper left character coordinate.
                cl:     Color number of character color.
                pGlyph: Pointer to the glyph to render.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dCharCanvasDrawGlyph(
    const NNSG2dCharCanvas* pCC,
    const NNSG2dFont* pFont,
    int x,
    int y,
    int cl,
    const NNSG2dGlyph* pGlyph
)
{
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    pCC->pDrawGlyph(pCC, pFont, x, y, cl, pGlyph);
}


// Erase

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasClear

  Description:  Fills all the characters associated with CharCanvas
                with a designated color.

  Arguments:    pCC:    Pointer to CharCanvas.
                cl:     Color number of the fill color.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dCharCanvasClear(
    const NNSG2dCharCanvas* pCC,
    int cl
)
{
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    pCC->pClear(pCC, cl);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasClearArea

  Description:  Fills a designated region of the characters associated with CharCanvas CharCanvas
                with a designated color.

  Arguments:    pCC:    Pointer to CharCanvas.
                cl:     Color number of the fill color.
                x:      X coordinate of the upper left of the fill region.
                y:      Y coordinate of the upper left of the fill region.
                w:      Width of the fill region.
                h:      Height of the fill region.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dCharCanvasClearArea(
    const NNSG2dCharCanvas* pCC,
    int cl,
    int x,
    int y,
    int w,
    int h
)
{
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    pCC->pClearArea(pCC, cl, x, y, w, h);
}



//----------------------------------------------------------------------------
// Build
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasInitForBG

  Description:  Initializes CharCanvas.
                Directly specifies and initializes the parameters for the BG.

  Arguments:    pCC:        Pointer to CharCanvas.
                charBase:   Pointer to the top character of the character
                            string allocated to CharCanvas.
                areaWidth:  CharCanvas width in character units.
                areaHeight: CharCanvas height in character units.
                colorMode:  Color mode of the render target character.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasInitForBG(
    NNSG2dCharCanvas* pCC,
    void* charBase,
    int areaWidth,
    int areaHeight,
    NNSG2dCharaColorMode colorMode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasInitForOBJ1D

  Description:  Initializes the CharCanvas for use by the 1D mapping OBJ.

  Arguments:    pCC:        Pointer to the CharCanvas to initialize.
                charBase:   Pointer to the starting point of the character allocated to CharCanvas.
                areaWidth:  CharCanvas width in character units.
                areaHeight: CharCanvas height in character units.
                colorMode:  Color mode of the render target character.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasInitForOBJ1D(
    NNSG2dCharCanvas* pCC,
    void* charBase,
    int areaWidth,
    int areaHeight,
    NNSG2dCharaColorMode colorMode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasInitForOBJ2DRect

  Description:  Initializes CharCanvas.
                This is used when the rectangle in character memory
                for the 2-dimensional mapping OBJ is used as the CharCanvas.

  Arguments:    pCC:        Pointer to CharCanvas.
                charBase:   Pointer to CharCanvas upper left character.
                areaWidth:  CharCanvas width in character units.
                areaHeight: CharCanvas height in character units.
                colorMode:  Color mode of the render target character.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasInitForOBJ2DRect(
    NNSG2dCharCanvas* pCC,
    void* charBase,
    int areaWidth,
    int areaHeight,
    NNSG2dCharaColorMode colorMode
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasMakeCell1D

  Description:  Creates a cell to display CharCanvas initialized with NNS_G2dCharCanvasInitForOBJ1D(). 
                Creates a cell for display. 

  Arguments:    pCell:      Buffer that stores generated cell data. 
                pCC:        Pointer to CharCanvas
                x:          Cell's center coordinates (based on CharCanvas coordinates)
                y:          Cell's center coordinates (based on CharCanvas coordinates)
                priority:   Cell priority
                mode:       Cell mode
                mosaic:     Cell mosaic
                effect:     Cell effects
                color:      CharCanvas color mode.
                charName:   Starting character name
                cParam:     Color palette number
                vramMode:   Specifies OBJ VRAM capacity setting.
                makeBR:     Specifies whether to add bounding rectangle information. 

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasMakeCell1D(
    NNSG2dCellData* pCell,
    const NNSG2dCharCanvas* pCC,
    int x,
    int y,
    int priority,
    GXOamMode mode,
    BOOL mosaic,
    GXOamEffect effect,
    GXOamColorMode color,
    int charName,
    int cParam,
    NNSG2dOBJVramMode vramMode,
    BOOL makeBR
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasMakeCell2DRect

  Description:  Creates a cell to display CharCanvas initialized with NNS_G2dCharCanvasInitForOBJ2DRect(). 
                Creates a cell for display. 

  Arguments:    pCell:      Buffer that stores generated cell data. 
                pCC:        Pointer to CharCanvas
                x:          Cell's center coordinates (based on CharCanvas coordinates)
                y:          Cell's center coordinates (based on CharCanvas coordinates)
                priority:   Cell priority
                mode:       Cell mode
                mosaic:     Cell mosaic
                effect:     Cell effects
                color:      CharCanvas color mode.
                charName:   Starting character name
                cParam:     Color palette number
                makeBR:     Specifies whether to add bounding rectangle information. 

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dCharCanvasMakeCell2DRect(
    NNSG2dCellData* pCell,
    const NNSG2dCharCanvas* pCC,
    int x,
    int y,
    int priority,
    GXOamMode mode,
    BOOL mosaic,
    GXOamEffect effect,
    GXOamColorMode color,
    int charName,
    int cParam,
    BOOL makeBR
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCharCanvasCalcCellDataSize*

  Description:  Calculates the buffer size to pass for the first argument of NNS_G2dCharCanvasMakeCell*().
                

  Arguments:    pCC:        Pointer to CharCanvas
                makeBR:     Specifies whether to add bounding rectangle information. 

  Returns:      None.
 *---------------------------------------------------------------------------*/
static size_t inline NNSi_G2dCharCanvasCalcCellDataSize(
    const NNSG2dCharCanvas* pCC,
    BOOL makeBR
)
{
    const int numObj        = NNSi_G2dCalcRequiredOBJ(pCC->areaWidth, pCC->areaHeight);
    const size_t oamSize    = sizeof(NNSG2dCellOAMAttrData) * numObj;
    const size_t brSize     = makeBR ? sizeof(NNSG2dCellBoundingRectS16): 0;

    return sizeof(NNSG2dCellData) + brSize + oamSize;
}

static inline size_t NNS_G2dCharCanvasCalcCellDataSize1D(
    const NNSG2dCharCanvas* pCC,
    BOOL makeBR
)
{
    return NNSi_G2dCharCanvasCalcCellDataSize(pCC, makeBR);
}

static inline size_t NNS_G2dCharCanvasCalcCellDataSize2DRect(
    const NNSG2dCharCanvas* pCC,
    BOOL makeBR
)
{
    return NNSi_G2dCharCanvasCalcCellDataSize(pCC, makeBR);
}






//---------------------------------------------------------------------

#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // G2D_CHARCANVAS_H_


