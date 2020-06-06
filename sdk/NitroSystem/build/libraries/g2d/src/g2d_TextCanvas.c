/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  File:     g2d_TextCanvas.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_TextCanvas.c,v $
  Revision 1.3  07/27/2005 11:27:08 PM  hatamoto_minoru
  DrawTaggedText used original TextCanvas for drawing characters. This was fixed. 

  Revision 1.2  07/11/2005 12:42:57 PM  hatamoto_minoru
  Added BPP check for font and target screen before color number. 

  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.6  03/18/2005 09:29:28 AM  hatamoto_minoru
  Added support for the changes to the holder of the encoding process callback function.

  Revision 1.5  03/10/2005 08:23:23 AM  hatamoto_minoru
  Added comments.
  Moved the render character string size calculation function from TextCanvas to Font.

  Revision 1.4  02/23/2005 05:50:39 AM  hatamoto_minoru
  Changed the character string centering so it is rounded up when calculated.

  Revision 1.3  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.2  01/27/2005 11:23:49 PM  hatamoto_minoru
  Moved towards stricter ASSERTS for the color number

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.1  12/10/2004 10:27:39 AM  hatamoto_minoru
  Changed TextScreen to TextCanvas

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <string.h>

#undef NNS_G2D_UNICODE
#include <nnsys/g2d/g2d_TextCanvas.h>






//****************************************************************************
// global functions
//****************************************************************************

//----------------------------------------------------------------------------
// Drawing
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawString

  Description:  Renders up to the first line feed character or the terminal character.

  Arguments:    pTxn:   Pointer to the TextCanvas render target.
                x:      Render starting position x coordinate.
                y:      Render starting position y coordinate.
                cl:     Color number of character color.
                str:    Character string to render.
                pPos:   Pointer to the buffer that stores the pointer to the next
                        character when rendering up to the line feed character, or
                        a NULL when rendering up to the terminal character.
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
)
{
    const void* pos;      // Pointer to the character to be rendered.
    int linefeed;       // line feed (the amount of space between the upper edge of the 1st line and the upper edge of the 2nd line)
    int charSpace;      // Space between characters (the amount of space between the right edge of the 1st character and the left edge of the 2nd character)
    const NNSG2dFont* pFont;
    u16 c;
    NNSiG2dSplitCharCallback getNextChar;

    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_POINTER_ASSERT( str );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );

    charSpace = pTxn->hSpace;
    linefeed = NNS_G2dFontGetLineFeed(pTxn->pFont) + pTxn->vSpace;
    pFont = pTxn->pFont;
    pos = str;
    getNextChar = NNSi_G2dFontGetSpliter(pFont);


    while( (c = getNextChar((const void**)&pos)) != 0 )
    {
        if( c == '\n' )
        {
            break;
        }

        // 1character rendering
        x += NNS_G2dCharCanvasDrawChar(pTxn->pCanvas, pFont, x, y, cl, c);
        x += charSpace;
    }

    if( pPos != NULL )
    {
        *pPos = (c == '\n') ? pos: NULL;
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dTextCanvasDrawTextAlign

  Description:  Renders left-, right-, and center-aligned inside the width of areaWidth.

  Arguments:    pTxn:       Pointer to the TextCanvas render target.
                x:          Render starting position x coordinate.
                y:          Render starting position y coordinate.
                cl:         Color number of character color.
                flags:      Flag that designates the render position.
                txt:        Character string to render.
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
)
{
    const void* str;    // Pointer to the character string to render
    int linefeed;       // line feed (the amount of space between the upper edge of the 1st line and the upper edge of the 2nd line)
    int charSpace;      // Space between characters (the amount of space between the right edge of the 1st character and the left edge of the 2nd character)
    const NNSG2dFont* pFont;

    int py;

    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_POINTER_ASSERT( txt );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );

    charSpace = pTxn->hSpace;
    linefeed = NNS_G2dFontGetLineFeed(pTxn->pFont) + pTxn->vSpace;
    pFont = pTxn->pFont;
    str = txt;

    py = y;

    while( str != NULL )
    {
        int px = x;

        if( flags & NNS_G2D_HORIZONTALALIGN_RIGHT )
        {
            int width = NNS_G2dTextCanvasGetStringWidth(pTxn, str, NULL);
            px += areaWidth - width;
        }
        else if( flags & NNS_G2D_HORIZONTALALIGN_CENTER )
        {
            int width = NNS_G2dTextCanvasGetStringWidth(pTxn, str, NULL);
            px += (areaWidth + 1) / 2 - (width + 1) / 2;
        }

        NNSi_G2dTextCanvasDrawString(pTxn, px, py, cl, str, &str);
        py += linefeed;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawText

  Description:  Renders a character string at the position designated with flags for (x,y).

  Arguments:    pTxn:   Pointer to the TextCanvas.
                x:      Render baseline coordinates.
                y:      Render baseline coordinates.
                cl:     Character render color number.
                flags:  Position designation flag.
                txt:    Character string to render.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_G2dTextCanvasDrawText(
    const NNSG2dTextCanvas* pTxn,
    int x,
    int y,
    int cl,
    u32 flags,
    const void* txt
)
{
    NNSG2dTextRect area;

    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_POINTER_ASSERT( txt );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );

    // Look for the upper left coordinate
    {
        area = NNS_G2dTextCanvasGetTextRect(pTxn, txt);

        if( flags & NNS_G2D_HORIZONTALORIGIN_CENTER )
        {
            x -= (area.width + 1) / 2;
        }
        else if( flags & NNS_G2D_HORIZONTALORIGIN_RIGHT )
        {
            x -= area.width;
        }

        if( flags & NNS_G2D_VERTICALORIGIN_MIDDLE )
        {
            y -= (area.height + 1) / 2;
        }
        else if( flags & NNS_G2D_VERTICALORIGIN_BOTTOM )
        {
            y -= area.height;
        }
    }

    NNSi_G2dTextCanvasDrawTextAlign(pTxn, x, y, area.width, cl, flags, txt);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawTextRect

  Description:  Positions and renders a character string with a rectangle as the base.
                This does not try to render within the bounds of the rectangle.

  Arguments:    pTxn:   Pointer to the TextCanvas.
                x:      rectangle upper left coordinate.
                y:      rectangle upper left coordinate.
                cl:     Character render color number.
                flags:  Position designation flag.
                txt:    Character string to render.
                w:      rectangle width.
                h:      rectangle height .

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
)
{
    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_POINTER_ASSERT( txt );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );

    // Compensate the y coordinate according to the flag
    {
        if( flags & NNS_G2D_VERTICALALIGN_BOTTOM )
        {
            int height = NNS_G2dTextCanvasGetTextHeight(pTxn, txt);
            y += h - height;
        }
        else if( flags & NNS_G2D_VERTICALALIGN_MIDDLE )
        {
            int height = NNS_G2dTextCanvasGetTextHeight(pTxn, txt);
            y += (h + 1)/2 - (height + 1) / 2;
        }
    }

    NNSi_G2dTextCanvasDrawTextAlign(pTxn, x, y, w, cl, flags, txt);
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTextCanvasDrawTaggedText

  Description:  Renders multiple chracters string lines that have embedded tags.
                This calls the callback if a control character (0x00-0x09,0x0B-0x1F)
                is found in the character string.
                The tag processing leaves everything to the callback.

  Arguments:    pTxn:       Pointer to the TextCanvas render target.
                x:          Render base position x coordinate.
                y:          Render base position y coordinate.
                cl:         Color number of character color.
                str:        Character string to render.
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
)
{
    const void* pos;      // Pointer to the character to be rendered.
    int linefeed;       // line feed (the amount of space between the upper edge of the 1st line and the upper edge of the 2nd line)
    int charSpace;      // Space between characters (the amount of space between the right edge of the 1st character and the left edge of the 2nd character)
    const NNSG2dFont* pFont;
    NNSG2dTagCallbackInfo cbInfo;
    u16 c;
    NNSiG2dSplitCharCallback getNextChar;

    int px = x;
    int py = y;

    NNS_G2D_TEXTCANVAS_ASSERT( pTxn );
    NNS_G2D_FONT_ASSERT( pTxn->pFont );
    NNS_G2D_POINTER_ASSERT( txt );
    NNS_G2D_ASSERT( NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))
                    <= NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp );
    NNS_G2D_MINMAX_ASSERT( cl, 0,
        (1 << NNS_G2dTextCanvasGetCharCanvas(pTxn)->dstBpp)
        - (1 << NNS_G2dFontGetBpp(NNS_G2dTextCanvasGetFont(pTxn))) + 1 );
    NNS_G2D_POINTER_ASSERT( cbFunc );

    cbInfo.txn      = *pTxn;
    cbInfo.cbParam  = cbParam;

    charSpace   = cbInfo.txn.hSpace;
    pFont       = cbInfo.txn.pFont;
    linefeed    = NNS_G2dFontGetLineFeed(pFont) + cbInfo.txn.vSpace;
    pos         = txt;
    getNextChar = NNSi_G2dFontGetSpliter(pFont);

    while( (c = getNextChar((const void**)&pos)) != 0 )
    {
        if( c < ' ' )
        {
            if( c == '\n' )
                // line feed process
            {
                px = x;
                py += linefeed;
            }
            else
                // Callback call
            {
                cbInfo.str = (const NNSG2dChar*)pos;
                cbInfo.x = px;
                cbInfo.y = py;
                cbInfo.clr = cl;

                cbFunc(c, &cbInfo);
                NNS_G2D_TEXTCANVAS_ASSERT( &(cbInfo.txn) );
                NNS_G2D_FONT_ASSERT( cbInfo.txn.pFont );
                NNS_G2D_POINTER_ASSERT( cbInfo.str );
                NNS_G2D_MINMAX_ASSERT( cl, 0, 255 );

                pos = (const void*)cbInfo.str;
                px = cbInfo.x;
                py = cbInfo.y;
                cl = cbInfo.clr;

                pFont       = cbInfo.txn.pFont;
                charSpace   = cbInfo.txn.hSpace;
                linefeed    = NNS_G2dFontGetLineFeed(pFont) + cbInfo.txn.vSpace;
            }

            continue;
        }
        else
            // 1character rendering
        {
            px += NNS_G2dCharCanvasDrawChar(cbInfo.txn.pCanvas, cbInfo.txn.pFont, px, py, cl, c);
            px += charSpace;
        }
    }
}

//--------------------------------------------------------------------

