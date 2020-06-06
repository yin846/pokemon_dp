/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  File:     g2d_TextCanvas.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_TextCanvas.h,v $
  Revision 1.2  07/27/2005 11:27:25 PM  hatamoto_minoru
  Revised comments

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.7  03/18/2005 09:24:52 AM  hatamoto_minoru
  Deleted the initialization functions for each encoding.

  Revision 1.6  03/10/2005 09:15:30 AM  hatamoto_minoru
  Changed the order of the arguments.

  Revision 1.5  03/10/2005 07:32:54 AM  hatamoto_minoru
  Made everything related to obtaining the character string size external.
  Changed the encoding designation method via callback.

  Revision 1.4  02/10/2005 12:09:07 PM  hatamoto_minoru
  Revised where the pointer to the font for the NNS_G2dTextCanvasSetFont() arguments was a constant.

  Revision 1.3  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.2  02/01/2005 11:42:48 AM  hatamoto_minoru
  Deleted unnecessary macros.

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.1  12/10/2004 10:27:39 AM  hatamoto_minoru
  Changed TextScreen to TextCanvas

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef G2D_TEXTAREA_H_
#define G2D_TEXTAREA_H_

#include <nnsys/g2d/g2d_Font.h>
#include <nnsys/g2d/g2d_CharCanvas.h>
#include <nnsys/g2d/g2di_AssertUtil.h>
#include <nnsys/g2d/g2di_SplitChar.h>
#include <nnsys/g2d/g2di_Char.h>

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------

//---------------------------------------------------------------------
// TextCanvas Macros
//---------------------------------------------------------------------
#define NNS_G2D_TEXTCANVAS_ASSERT( pTxn )                           \
    NNS_G2D_ASSERTMSG(                                              \
        NNS_G2D_IS_VALID_POINTER(pTxn)                              \
            && NNS_G2D_IS_VALID_POINTER((pTxn)->pCanvas)            \
            && NNS_G2D_IS_VALID_POINTER((pTxn)->pFont)              \
        , "Illegal NNSG2dTextCanvas." )

#define NNS_G2D_CHARENCODING_ASSERT( enc )                      \
    NNS_G2D_ASSERTMSG(                                          \
        (0 <= (enc)) && ((enc) < NNS_G2D_NUM_OF_CHARENCODING)   \
        , "Illegal NNSG2dCharEncoding(=%d).", (enc) )           \



//---------------------------------------------------------------------
// TextCanvas definitions
//---------------------------------------------------------------------

// Vertical origin placement
typedef enum NNSG2dVerticalOrigin
{
    NNS_G2D_VERTICALORIGIN_TOP      = 0x1,
    NNS_G2D_VERTICALORIGIN_MIDDLE   = 0x2,
    NNS_G2D_VERTICALORIGIN_BOTTOM   = 0x4
}
NNSG2dVerticalOrigin;

// Horizontal origin placement
typedef enum NNSG2dHorizontalOrigin
{
    NNS_G2D_HORIZONTALORIGIN_LEFT   = 0x8,
    NNS_G2D_HORIZONTALORIGIN_CENTER = 0x10,
    NNS_G2D_HORIZONTALORIGIN_RIGHT  = 0x20
}
NNSG2dHorizontalOrigin;

// Vertical alignment
typedef enum NNSG2dVerticalAlign
{
    NNS_G2D_VERTICALALIGN_TOP       = 0x40,
    NNS_G2D_VERTICALALIGN_MIDDLE    = 0x80,
    NNS_G2D_VERTICALALIGN_BOTTOM    = 0x100
}
NNSG2dVerticalAlign;

// Horizontal alignment
typedef enum NNSG2dHorizontalAlign
{
    NNS_G2D_HORIZONTALALIGN_LEFT    = 0x200,
    NNS_G2D_HORIZONTALALIGN_CENTER  = 0x400,
    NNS_G2D_HORIZONTALALIGN_RIGHT   = 0x800
}
NNSG2dHorizontalAlign;



// TextCanvas
typedef struct NNSG2dTextCanvas
{
    const NNSG2dCharCanvas* pCanvas;
    const NNSG2dFont* pFont;
    int hSpace;
    int vSpace;
}
NNSG2dTextCanvas;



// Callback parameters for NNS_G2dTextCanvasDrawTaggedText
typedef struct NNSG2dTagCallbackInfo
{
    NNSG2dTextCanvas txn;   // TextCanvas being used for drawing. 
    const NNSG2dChar* str;  // Pointer to the character string being rendered.
    int x;                  // X coordinate being rendered
    int y;                  // Y coordinate being rendered
    int clr;                // Color number being rendered.
    void* cbParam;          // 7th argument of NNS_G2dTextCanvasDrawTaggedText.
}
NNSG2dTagCallbackInfo;

// Callback function for NNS_G2dTextCanvasDrawTaggedText
typedef void (*NNSG2dTagCallback)(u16 c, NNSG2dTagCallbackInfo* pInfo);


//---------------------------------------------------------------------
// TextCanvas Operations
//---------------------------------------------------------------------

//--------------------------------------------
// Drawing

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawString

  Description:  Renders up to the first newline character or the terminal character.

  Arguments:    pTxn:   Pointer to the TextCanvas to be drawn.
                x:      Draw starting position x coordinate.
                y:      Draw starting position y coordinate.
                cl:     Color number of character color.
                str:    Character string to be drawn.
                pPos:   Pointer to the buffer that stores the pointer to the next
                        character when drawing up to the newline character or
                        a NULL when drawing up to the terminal character.
                        A NULL can be designated here if unnecessary.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawString(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    const void* str,
    const void** pPos
);



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dDrawTextAlign

  Description:  Draws up to the first newlnie character or the terminal character.

  Arguments:    pTxn:       Pointer to the TextCanvas render target.
                x:          Draw starting position x coordinate.
                y:          Draw starting position y coordinate.
                cl:         Color number of character color.
                flags:      Flag that designates the drawing position.
                txt:        Character string to be drawn.
                areaWidth:  Render region width referenced when right-aligning or centering.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTextAlign(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int areaWidth,
    int cl,
    u32 flags,
    const void* txt
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawText

  Description:  Draws multiple character string lines.
                The drawing position can be designated with a point as the base.

  Arguments:    pTxn:   Pointer to the TextCanvas to be drawn.
                x:      Draw base position x coordinate.
                y:      Draw base position y coordinate.
                cl:     Color number of character color.
                flags:  Flag that designates the drawing position.
                txt:    Character string to be drawn.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawText(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    u32 flags,
    const void* txt
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawTextRect

  Description:  Draws multiple character string lines.
                The drawing position can be designated with a rectangle as the base.
                Characters that stick out of the rectangle will be drawn as normal.

  Arguments:    pTxn:   Pointer to the TextCanvas to be drawn.
                x:      Upper left y coordinate of rectangle to be drawn.
                y:      Upper left y coordinate of rectangle to be drawn.
                cl:     Color number of character color.
                flags:  Flag that designates the drawing position.
                txt:    Character string to be drawn.
                w:      Width of rectangle to be drawn.
                h:      Height of rectangle to be drawn.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTextRect(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int w,
    int h,
    int cl,
    u32 flags,
    const void* txt
);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawTaggedText

  Description:  Draws multiple chracters string lines that have embedded tags.
                Drawing can be controlled with the tags.

  Arguments:    pTxn:       Pointer to the TextCanvas to be drawn.
                x:          Draw base position x coordinate.
                y:          Draw base position y coordinate.
                cl:         Color number of character color.
                txt:        Character string to be drawn.
                cbFunc:     Callback function called by the tag.
                cbParam:    User data passed to the callback function.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawTaggedText(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    const void* txt,
    NNSG2dTagCallback cbFunc,
    void* cbParam
);





//--------------------------------------------
// Build

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasInit*

  Description:  Initializes the TextCanvas.

  Arguments:    pTxn:   Pointer to the TextCanvas to initialize.
                pCC:    Pointer to the CharCanvas to be drawn on.
                pFont:  Pointer to the font used for drawing.
                hSpace: Space between characters.
                vSpace: Correction difference for the amount of space between lines in the font.
                encode: Pointer to the character cutout callback function.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dTextCanvasInit(
    NNSG2dTextCanvas* pTxn,
    const NNSG2dCharCanvas* pCC,
    const NNSG2dFont* pFont,
    int hSpace,
    int vSpace
)
{
    NNS_G2D_POINTER_ASSERT( pTxn );
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    NNS_G2D_FONT_ASSERT( pFont );

    pTxn->pCanvas       = pCC;
    pTxn->pFont         = pFont;
    pTxn->hSpace        = hSpace;
    pTxn->vSpace        = vSpace;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasInit*

  Description:  Initializes the TextCanvas.

  Arguments:    pTxn:       Pointer to the TextCanvas to initialize.
                pCC:        Pointer to the CharCanvas to be drawn on.
                pFont:      Pointer to the font used for drawing
                hSpace:     Space between characters.
                vSpace:     Correction difference for the amount of space between lines in the font.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#define NNS_G2dTextCanvasInit1Byte      NNS_G2dTextCanvasInit
#define NNS_G2dTextCanvasInitUTF8       NNS_G2dTextCanvasInit
#define NNS_G2dTextCanvasInitUTF16      NNS_G2dTextCanvasInit
#define NNS_G2dTextCanvasInitShiftJIS   NNS_G2dTextCanvasInit


//--------
// getter

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetCharCanvas

  Description:  Configures the CharCanvas on which the TextCanvas is to be drawn.

  Arguments:    pTxn:   Pointer to the TextCanvas that configures the font.
                pCC:    Pointer to the newly-configured CharCanvas.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline const NNSG2dCharCanvas* NNS_G2dTextCanvasGetCharCanvas(
    const NNSG2dTextCanvas* pTxn
)
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    return pTxn->pCanvas;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetFont

  Description:  Configures the font for the TextCanvas.

  Arguments:    pTxn:   Pointer to the TextCanvas that configures the font.
                pFont:  Pointer to the newly-configured font.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline const NNSG2dFont* NNS_G2dTextCanvasGetFont( const NNSG2dTextCanvas* pTxn )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    return pTxn->pFont;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetHSpace

  Description:  Configures the amount of space TextCanvas uses when drawing a character string.

  Arguments:    pTxn:   Pointer to the TextCanvas that configures the font.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline int NNS_G2dTextCanvasGetHSpace( const NNSG2dTextCanvas* pTxn )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    return pTxn->hSpace;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetVSpace

  Description:  Configures the amount of space TextCanvas uses when drawing a character string.

  Arguments:    pTxn:   Pointer to the TextCanvas that configures the font.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline int NNS_G2dTextCanvasGetVSpace( const NNSG2dTextCanvas* pTxn )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    return pTxn->vSpace;
}



//--------
// setter

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasSetCharCanvas

  Description:  Configures the CharCanvas on which the TextCanvas is to be drawn.

  Arguments:    pTxn:   Pointer to the TextCanvas that configures the font.
                pCC:    Pointer to the newly-configured CharCanvas.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dTextCanvasSetCharCanvas(
    NNSG2dTextCanvas* pTxn,
    const NNSG2dCharCanvas* pCC
)
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_CHARCANVAS_ASSERT( pCC );
    pTxn->pCanvas = pCC;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasSetFont

  Description:  Configures the font in TextCanvas.

  Arguments:    pTxn:   Pointer to the TextCanvas that configures the font.
                pFont:  Pointer to the newly-configured font.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dTextCanvasSetFont( NNSG2dTextCanvas* pTxn, const NNSG2dFont* pFont )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_FONT_ASSERT( pFont );
    pTxn->pFont = pFont;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasSetHSpace

  Description:  Configures the amount of space TextCanvas uses when drawing a character string.

  Arguments:    pTxn:   Pointer to the TextCanvas that configures the font.
                hSpace: The newly-configured amount of space between characters.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dTextCanvasSetHSpace( NNSG2dTextCanvas* pTxn, int hSpace )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    pTxn->hSpace = hSpace;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasSetVSpace

  Description:  Configures the amount of space TextCanvas uses when drawing a character string.

  Arguments:    pTxn:   Pointer to the TextCanvas that configures the font.
                vSpace: The newly-configured amount of space between lines.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dTextCanvasSetVSpace( NNSG2dTextCanvas* pTxn, int vSpace )
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    pTxn->vSpace = vSpace;
}



//--------------------------------------------
// Get information

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetStringWidth

  Description:  Looks for the width when drawing up to the first newline or the terminal character.

  Arguments:    pTxn:   Pointer to the TextCanvas that is the base for the calculation.
                str:    character string .
                pPos:   A pointer to the next char after the newline when processing stopped at a newline. 
                        A pointer to the buffer that stores the null if processing stopped at a terminating char. 
                        
                        NULL can be specified here if the setting is unnecessary.


  Returns:      The width when the character string is rendered.
 *---------------------------------------------------------------------------*/
static inline int NNS_G2dTextCanvasGetStringWidth(
    const NNSG2dTextCanvas* pTxn,
    const NNSG2dChar* str,
    const NNSG2dChar** pPos
)
{
    return NNS_G2dFontGetStringWidth(
                pTxn->pFont,
                pTxn->hSpace,
                str,
                pPos );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetTextHeight

  Description:  Looks for the height when a character string is drawn.

  Arguments:    pTxn:   Pointer to the TextCanvas that is the base for the calculation.
                txt:    character string .

  Returns:      Height when the character string is drawn.
 *---------------------------------------------------------------------------*/
static inline int NNS_G2dTextCanvasGetTextHeight(
    const NNSG2dTextCanvas* pTxn,
    const NNSG2dChar* txt
)
{
    return NNS_G2dFontGetTextHeight(
                pTxn->pFont,
                pTxn->vSpace,
                txt );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetTextWidth

  Description:  Looks for the width when a character string is drawn.
                This value will become the maximum width of each line.

  Arguments:    pTxn:   Pointer to the TextCanvas that is the base for the calculation.
                txt:    character string .

  Returns:      The width when the character string is drawn.
 *---------------------------------------------------------------------------*/
static inline int NNS_G2dTextCanvasGetTextWidth(
    const NNSG2dTextCanvas* pTxn,
    const NNSG2dChar* txt
)
{
    return NNS_G2dFontGetTextWidth(
                pTxn->pFont,
                pTxn->hSpace,
                txt );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasGetTextRect

  Description:  Looks for the height and width when a character string is drawn.

  Arguments:    pTxn:   Pointer to the TextCanvas that is the base for the calculation.
                txt:    character string .

  Returns:      Height and width when the character string is drawn.
 *---------------------------------------------------------------------------*/
static inline NNSG2dTextRect NNS_G2dTextCanvasGetTextRect(
    const NNSG2dTextCanvas* pTxn,
    const NNSG2dChar* txt
)
{
    return NNS_G2dFontGetTextRect(
                pTxn->pFont,
                pTxn->hSpace,
                pTxn->vSpace,
                txt );
}



//---------------------------------------------------------------------

static inline void NNS_G2dTextCanvasDrawString(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, const NNSG2dChar* str, const NNSG2dChar** pPos)
    { NNSi_G2dTextCanvasDrawString(pTxn, x, y, cl, str, (const void**)pPos); }
static inline void NNS_G2dTextCanvasDrawText(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, u32 flags, const NNSG2dChar* txt)
    { NNSi_G2dTextCanvasDrawText(pTxn, x, y, cl, flags, txt); }
static inline void NNS_G2dTextCanvasDrawTextRect(const NNSG2dTextCanvas* pTxn, int x, int y, int w, int h, int cl, u32 flags, const NNSG2dChar* txt)
    { NNSi_G2dTextCanvasDrawTextRect(pTxn, x, y, w, h, cl, flags, txt); }
static inline void NNS_G2dTextCanvasDrawTaggedText(const NNSG2dTextCanvas* pTxn, int x, int y, int cl, const NNSG2dChar* txt, NNSG2dTagCallback cbFunc, void* cbParam)
    { NNSi_G2dTextCanvasDrawTaggedText(pTxn, x, y, cl, txt, cbFunc, cbParam); }

//---------------------------------------------------------------------

#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // G2D_TEXTAREA_H_


