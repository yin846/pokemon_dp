/*---------------------------------------------------------------------------*
  Project:  NitroSystem - Graphics 2D
  File:     g2d_Screen.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Screen.c,v $
  Revision 1.12  08/09/2005 06:44:50 AM  hatamoto_minoru
  When in BG mode 3, transitioned to wrong BG mode when attempt was made to change BG3 to affine extended BG.  This was fixed. 
  Fixed transition to wrong BG mode. 

  Revision 1.11  06/27/2005 11:41:12 PM  hatamoto_minoru
  Warning measures. 

  Revision 1.10  06/27/2005 05:57:39 AM  hatamoto_minoru
  Some internal functions were divided and put into g2di_BGManipulate. 
  In some processes, accesses registers directly without using SDK functions. 
  Added Ex functions that support partial characters and compressed palettes. 

  Revision 1.9  06/02/2005 08:47:05 AM  hatamoto_minoru
  Error message for non-standard screen size was changed from invalid to unsupported.

  Revision 1.8  02/22/2005 12:51:14 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.7  02/21/2005 02:17:57 AM  hatamoto_minoru
  Response for warnings about implicit sign/unsign conversion.

  Revision 1.6  09/15/2004 07:55:03 AM  hatamoto_minoru
  Moved NNS_G2D_BG_ASSERT from .c to .h

  Revision 1.5  09/07/2004 12:59:05 AM  hatamoto_minoru
  Corrected function comments.
  Added ASSERT.

  Revision 1.4  09/03/2004 09:31:50 AM  hatamoto_minoru
  Corrected a file name in the comments.
  Chaned the function internal definition table to static.
  Speeded things up a bit.

  Revision 1.3  09/03/2004 04:44:28 AM  hatamoto_minoru
  Changed the randomly output message to English.

  Revision 1.2  09/03/2004 02:42:51 AM  hatamoto_minoru
  Corrected items NNSi_G2dBGAreaOver is permanently set as static.

  Revision 1.1  09/02/2004 07:07:16 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_Screen.h>
#include "g2di_BGManipulator.h"


#define BG_MODE_WARNING 8

// Size of the extended palette slot
#define NNS_G2D_BGEXTPLTT_SLOTSIZE  0x2000

//---- Unit plane size in Text BG (in characters)
#define PLANE_WIDTH     32
#define PLANE_HEIGHT    32


// Enumerator for showing the background extended palette slot
typedef enum NNSG2dBGExtPlttSlot
{
    NNS_G2D_BGEXTPLTTSLOT_MAIN0,
    NNS_G2D_BGEXTPLTTSLOT_MAIN1,
    NNS_G2D_BGEXTPLTTSLOT_MAIN2,
    NNS_G2D_BGEXTPLTTSLOT_MAIN3,
    NNS_G2D_BGEXTPLTTSLOT_SUB0,
    NNS_G2D_BGEXTPLTTSLOT_SUB1,
    NNS_G2D_BGEXTPLTTSLOT_SUB2,
    NNS_G2D_BGEXTPLTTSLOT_SUB3

} NNSG2dBGExtPlttSlot;


//---- Screen size value -> for conversion to register value 
typedef struct ScreenSizeMap
{
    u16 width;
    u16 height;
    u16 scnSize;
}
ScreenSizeMap;


// A screen size table for the text background
static const ScreenSizeMap sTextScnSize[4] =
{
    {256, 256, GX_BG_SCRSIZE_TEXT_256x256},
    {256, 512, GX_BG_SCRSIZE_TEXT_256x512},
    {512, 256, GX_BG_SCRSIZE_TEXT_512x256},
    {512, 512, GX_BG_SCRSIZE_TEXT_512x512},
};

// A screen size table for the affine background
static const ScreenSizeMap sAffineScnSize[4] =
{
    {128, 128, GX_BG_SCRSIZE_AFFINE_128x128},
    {256, 256, GX_BG_SCRSIZE_AFFINE_256x256},
    {512, 512, GX_BG_SCRSIZE_AFFINE_512x512},
    {1024, 1024, GX_BG_SCRSIZE_AFFINE_1024x1024},
};

// A screen size table for the affine extended background
static const ScreenSizeMap sAffineExtScnSize[4] =
{
    {128, 128, GX_BG_SCRSIZE_256x16PLTT_128x128},
    {256, 256, GX_BG_SCRSIZE_256x16PLTT_256x256},
    {512, 512, GX_BG_SCRSIZE_256x16PLTT_512x512},
    {1024, 1024, GX_BG_SCRSIZE_256x16PLTT_1024x1024},
};


static const u8 sBGTextModeTable[4][8] =
{
    {
        /* 0 -> */ GX_BGMODE_0,
        /* 1 -> */ GX_BGMODE_1,
        /* 2 -> */ GX_BGMODE_2,
        /* 3 -> */ GX_BGMODE_3,
        /* 4 -> */ GX_BGMODE_4,
        /* 5 -> */ GX_BGMODE_5,
        /* 6 -> */ GX_BGMODE_6,
        /* 7 -> */ BG_MODE_WARNING,                             // No configuration permitted
    },
    {
        /* 0 -> */ GX_BGMODE_0,
        /* 1 -> */ GX_BGMODE_1,
        /* 2 -> */ GX_BGMODE_2,
        /* 3 -> */ GX_BGMODE_3,
        /* 4 -> */ GX_BGMODE_4,
        /* 5 -> */ GX_BGMODE_5,
        /* 6 -> */ GX_BGMODE_6,
        /* 7 -> */ BG_MODE_WARNING,                             // No configuration permitted
    },
    {
        /* 0 -> */ GX_BGMODE_0,
        /* 1 -> */ GX_BGMODE_1,
        /* 2 -> */ GX_BGMODE_1,
        /* 3 -> */ GX_BGMODE_3,
        /* 4 -> */ GX_BGMODE_3,
        /* 5 -> */ GX_BGMODE_3,
        /* 6 -> */ GX_BGMODE_0,
        /* 7 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // No configuration permitted
    },
    {
        /* 0 -> */ GX_BGMODE_0,
        /* 1 -> */ GX_BGMODE_0,
        /* 2 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // BG2: Affine      -> Text
        /* 3 -> */ GX_BGMODE_0,
        /* 4 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // BG2: Affine      -> Text
        /* 5 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // BG2: Affine extension     -> Text
        /* 6 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // BG2: Large screen BMP    -> Text
        /* 7 -> */ (GXBGMode)(GX_BGMODE_0 + BG_MODE_WARNING),   // No configuration permitted
    }
};


// A background mode conversion table for when changing BG2 to affine BG
static const u8 sBGAffineModeTable[2][8] =
{
    {
        /* 0 -> */ (GXBGMode)(GX_BGMODE_2 + BG_MODE_WARNING),   // BG2: Text      ->Affine
        /* 1 -> */ GX_BGMODE_2,
        /* 2 -> */ GX_BGMODE_2,
        /* 3 -> */ GX_BGMODE_4,
        /* 4 -> */ GX_BGMODE_4,
        /* 5 -> */ GX_BGMODE_4,
        /* 6 -> */ GX_BGMODE_4,
        /* 7 -> */ (GXBGMode)(GX_BGMODE_2 + BG_MODE_WARNING),   // No configuration permitted
    },
    {
        /* 0 -> */ GX_BGMODE_1,
        /* 1 -> */ GX_BGMODE_1,
        /* 2 -> */ GX_BGMODE_2,
        /* 3 -> */ GX_BGMODE_1,
        /* 4 -> */ GX_BGMODE_2,
        /* 5 -> */ (GXBGMode)(GX_BGMODE_1 + BG_MODE_WARNING),   // BG2: Affine extension     -> Text
        /* 6 -> */ (GXBGMode)(GX_BGMODE_1 + BG_MODE_WARNING),   // BG2: Large screen      -> Text
        /* 7 -> */ (GXBGMode)(GX_BGMODE_1 + BG_MODE_WARNING),   // No configuration permitted
    }
};
static const u8 sBG256x16PlttModeTable[2][8] =
{
    {
        /* 0 -> */ (GXBGMode)(GX_BGMODE_5 + BG_MODE_WARNING),   // BG3: Text ->Affine extended
        /* 1 -> */ (GXBGMode)(GX_BGMODE_5 + BG_MODE_WARNING),   // BG3: Affine >Affine extended
        /* 2 -> */ (GXBGMode)(GX_BGMODE_5 + BG_MODE_WARNING),   // BG3: Affine >Affine extended
        /* 3 -> */ GX_BGMODE_5,
        /* 4 -> */ GX_BGMODE_5,
        /* 5 -> */ GX_BGMODE_5,
        /* 6 -> */ GX_BGMODE_5,
        /* 7 -> */ (GXBGMode)(GX_BGMODE_5 + BG_MODE_WARNING),   // No configuration permitted
    },
    {
        /* 0 -> */ GX_BGMODE_3,
        /* 1 -> */ GX_BGMODE_3,
        /* 2 -> */ GX_BGMODE_4,
        /* 3 -> */ GX_BGMODE_3,
        /* 4 -> */ GX_BGMODE_4,
        /* 5 -> */ GX_BGMODE_5,
        /* 6 -> */ (GXBGMode)(GX_BGMODE_3 + BG_MODE_WARNING),   // BG2: Large screen      -> Text
        /* 7 -> */ (GXBGMode)(GX_BGMODE_3 + BG_MODE_WARNING),   // No configuration permitted
    }
};



GXBGAreaOver NNSi_G2dBGAreaOver = GX_BG_AREAOVER_XLU;



//****************************************************************************
// static functions
//****************************************************************************

//----------------------------------------------------------------------------
// Inline functions
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         IsMainBGExtPlttSlot

  Description:  Determines whether the NNSG2dBGExtPlttSlot type value
                shows the main screen extended pallet slot

  Arguments:    slot:   The target

  Returns:      TRUE if the main screen extended slot is shown.
 *---------------------------------------------------------------------------*/
static inline BOOL IsMainBGExtPlttSlot(NNSG2dBGExtPlttSlot slot)
{
    return (slot <= NNS_G2D_BGEXTPLTTSLOT_MAIN3);
}



/*---------------------------------------------------------------------------*
  Name:         CalcTextScreenOffset

  Description:  Gets the offset from text BG screen's specified screen base position. 
                

  Arguments:    x:  Specified position x (in characters)
                y:  Specified position x (in characters)
                w:  Target BG screen width (in characters)

  Returns:      Specified position offset (in characters)
 *---------------------------------------------------------------------------*/
static inline int CalcTextScreenOffset(int x, int y, int w)
{
    const int x_blk = x / PLANE_WIDTH;
    const int y_blk = y / PLANE_HEIGHT;
    const int x_char = x % PLANE_WIDTH;
    const int y_char = y % PLANE_HEIGHT;

    return
        w * PLANE_HEIGHT * y_blk
        + PLANE_WIDTH * PLANE_HEIGHT * x_blk
        + PLANE_WIDTH * y_char
        + x_char;
}



/*---------------------------------------------------------------------------*
  Name:         GetCompressedPlttOriginalIndex

  Description:  Gets the offset of the specified palette before compression.  

  Arguments:    pCmpInfo:   A pointer to the palette compression data.
                idx:        palette number

  Returns:      Palette offset (in bytes)
 *---------------------------------------------------------------------------*/
static inline u32 GetCompressedPlttOriginalIndex(
    const NNSG2dPaletteCompressInfo* pCmpInfo,
    int idx
)
{
    NNS_G2D_POINTER_ASSERT( pCmpInfo );

    return ( (u16*)(pCmpInfo->pPlttIdxTbl) )[idx];
}



/*---------------------------------------------------------------------------*
  Name:         GetPlttSize

  Description:  Gets the size per palette of the palette data. 

  Arguments:    pPltData:   A pointer to the palette data.

  Returns:      Palette size (in bytes)
 *---------------------------------------------------------------------------*/
static inline u32 GetPlttSize(const NNSG2dPaletteData* pPltData)
{
    NNS_G2D_POINTER_ASSERT( pPltData );

    switch( pPltData->fmt )
    {
    case GX_TEXFMT_PLTT16:     return sizeof(GXBGPltt16);
    case GX_TEXFMT_PLTT256:    return sizeof(GXBGPltt256);
    default:                    SDK_ASSERTMSG(FALSE, "invalid NNSG2dPaletteData");
    }

    return 0;
}






//----------------------------------------------------------------------------
// Ordinary Function
//----------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         SelectScnSize

  Description:  Converts the screen size by accessing the table received.
                Returns the smallest BG size larger than the specified screen size. 
                If there is no BG larger than the specified screen size, 
                returns the largest BG size. 

  Arguments:    tbl:    The screen size conversion table
                w:      The screen width in pixels
                h:      The screen height in pixels

  Returns:      Pointer to the supported screen size data. 
 *---------------------------------------------------------------------------*/
static const ScreenSizeMap* SelectScnSize(const ScreenSizeMap tbl[4], int w, int h)
{
    int i;
    SDK_NULL_ASSERT(tbl);

    for( i = 0; i < 4; i++ )
    {
        if( w <= tbl[i].width && h <= tbl[i].height )
        {
            return &tbl[i];
        }
    }
    return &tbl[3];
}



/*---------------------------------------------------------------------------*
  Name:         ChangeBGModeByTable*

  Description:  Switches the BG mode according to the table.

  Arguments:    modeTable:  Mode switching table

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ChangeBGModeByTableMain(const u8 modeTable[])
{
    GXBGMode mode = (GXBGMode)modeTable[GetBGModeMain()];
    GXBG0As bg0as = IsBG03D() ? GX_BG0_AS_3D: GX_BG0_AS_2D;
    SDK_NULL_ASSERT(modeTable);

    // Warns of mode changes that could negatively affect other backgrounds
    if( mode >= BG_MODE_WARNING )
    {
        mode -= BG_MODE_WARNING;
        OS_Warning("Dangerous Main BG mode change: %d => %d", GetBGModeMain(), mode);
    }

    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, mode, bg0as);
}

static void ChangeBGModeByTableSub(const u8 modeTable[])
{
    GXBGMode mode = (GXBGMode)modeTable[GetBGModeSub()];
    SDK_NULL_ASSERT(modeTable);

    // Warns of mode changes that could negatively affect other backgrounds
    if( mode >= BG_MODE_WARNING )
    {
        mode -= BG_MODE_WARNING;
        OS_Warning("Dangerous Sub BG mode change: %d => %d", GetBGModeSub(), mode);
    }

    GXS_SetGraphicsMode(mode);
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGPlttToExtendedPltt

  Description:  Reads palette data into the specified extended palette slot.

  Arguments:    slot:       The extended palette slot where the palette data gets read to
                pPltData:   A pointer to the palette data

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void LoadBGPlttToExtendedPltt(
    NNSG2dBGExtPlttSlot slot,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
    void (*prepairLoad)(void);
    void (*cleanupLoad)(void);
    void (*loader)(const void*, u32, u32);
    u32 offset = (u32)( slot * NNS_G2D_BGEXTPLTT_SLOTSIZE );
    SDK_NULL_ASSERT(pPltData);

    // Cache flush
    DC_FlushRange(pPltData->pRawData, pPltData->szByte);

    if( IsMainBGExtPlttSlot(slot) )
    {
        prepairLoad = GX_BeginLoadBGExtPltt;
        cleanupLoad = GX_EndLoadBGExtPltt;
        loader      = GX_LoadBGExtPltt;
    }
    else
    {
        offset -= NNS_G2D_BGEXTPLTT_SLOTSIZE * NNS_G2D_BGEXTPLTTSLOT_SUB0;

        prepairLoad = GXS_BeginLoadBGExtPltt;
        cleanupLoad = GXS_EndLoadBGExtPltt;
        loader      = GXS_LoadBGExtPltt;
    }

    if( pCmpInfo != NULL )
    {
        const u32 szOnePltt = GetPlttSize( pPltData );
        const int numIdx = pCmpInfo->numPalette;
        int i;

        for( i = 0; i < numIdx; i++ )
        {
        	const u32 offsetAddr = GetCompressedPlttOriginalIndex(pCmpInfo, i) * szOnePltt;
            const void* pSrc = (u8*)pPltData->pRawData + szOnePltt * i;

            prepairLoad();
            loader(pSrc, offset + offsetAddr, szOnePltt);
            cleanupLoad();
        }
    }
    else
    {
        prepairLoad();
        loader(pPltData->pRawData, offset, pPltData->szByte);
        cleanupLoad();
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGPlttToNormalPltt

  Description:  Reads palette data into the standard palette.

  Arguments:    bMainDisplay:   The background screen to be read into.
                pPltData:       A pointer to the palette data

  Returns:
 *---------------------------------------------------------------------------*/
static void LoadBGPlttToNormalPltt(
    NNSG2dBGSelect bg,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
    u8* pDst;
    SDK_NULL_ASSERT(pPltData);

    // Cache flush
    DC_FlushRange(pPltData->pRawData, pPltData->szByte);

    pDst = (u8*)( IsMainBG(bg) ? HW_BG_PLTT: HW_DB_BG_PLTT );

    if( pCmpInfo != NULL )
    {
        const u32 szOnePltt = GetPlttSize( pPltData );
        const int numIdx = pCmpInfo->numPalette;
        int i;

        for( i = 0; i < numIdx; i++ )
        {
        	const u32 offsetAddr = GetCompressedPlttOriginalIndex(pCmpInfo, i) * szOnePltt;
            const void* pSrc = (u8*)pPltData->pRawData + szOnePltt * i;

            MI_DmaCopy16(GXi_DmaId, pSrc, pDst + offsetAddr, szOnePltt);
        }
    }
    else
    {
        MI_DmaCopy16(GXi_DmaId, pPltData->pRawData, pDst, pPltData->szByte);
    }
}



/*---------------------------------------------------------------------------*
  Name:         GetBGExtPlttSlot

  Description:  Gets the extended palette slot used by the specified background.

  Arguments:    bg: The background that uses the extended palette slot number to be fetched

  Returns:      The extended palette slot number used by the background
 *---------------------------------------------------------------------------*/
static NNSG2dBGExtPlttSlot GetBGExtPlttSlot(NNSG2dBGSelect bg)
{
    // NNSG2dBGSelect => BGxCNT register offset conversion table
    static const u16 addrTable[] =
    {
        REG_BG0CNT_OFFSET,
        REG_BG1CNT_OFFSET,
        0,                      // The slot to be used is fixed
        0,                      // The slot to be used is fixed
        REG_DB_BG0CNT_OFFSET,
        REG_DB_BG1CNT_OFFSET,
        0,                      // The slot to be used is fixed
        0                       // The slot to be used is fixed
    };
    u32 addr;
    NNSG2dBGExtPlttSlot slot = (NNSG2dBGExtPlttSlot)bg;
        // Generally, (int)slot == (int)bg

    NNS_G2D_BG_ASSERT(bg);
    addr = addrTable[bg];

    if( addr != 0 )
    {
        addr += HW_REG_BASE;

        if( (*(u16*)addr & REG_G2_BG0CNT_BGPLTTSLOT_MASK) != 0 )
        {
            // If the BGPLTTSLOT bit is up
            // (int)slot == (int)bg + 2
            slot += 2;
        }
    }

    return slot;
}



/*---------------------------------------------------------------------------*
  Name:         SetBGnControlTo*

  Description:  Sets the BGxCNT register and changes the BG mode.

  Arguments:    n:          The target BG screen.
                size:       The screen size.
                cmode:      The color mode.
                areaOver:   Area over processing. 
                scnBase:    The BG screen base block.
                chrBase:    The character base block.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetBGnControlToText(
    NNSG2dBGSelect n,
    GXBGScrSizeText size,
    GXBGColorMode cmode,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    const int bgNo = GetBGNo(n);
    GXBGExtPltt extPltt = GX_BG_EXTPLTT_01;
    if( IsMainBG(n) )
    {
        if( ! IsMainBGExtPltt01Available() ) extPltt = GX_BG_EXTPLTT_23;
        ChangeBGModeByTableMain(sBGTextModeTable[bgNo]);
    }
    else
    {
        ChangeBGModeByTableSub(sBGTextModeTable[bgNo]);
    }
    SetBGnControlText(n, size, cmode, scnBase, chrBase, extPltt);
}
static void SetBGnControlToAffine(
    NNSG2dBGSelect n,
    GXBGScrSizeAffine size,
    GXBGAreaOver areaOver,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    if( IsMainBG(n) )
    {
        ChangeBGModeByTableMain(sBGAffineModeTable[n-2]);
    }
    else
    {
        ChangeBGModeByTableSub(sBGAffineModeTable[n-6]);
    }
    SetBGnControlAffine(n, size, areaOver, scnBase, chrBase);
}
static void SetBGnControlTo256x16Pltt(
    NNSG2dBGSelect n,
    GXBGScrSize256x16Pltt size,
    GXBGAreaOver areaOver,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    if( IsMainBG(n) )
    {
        ChangeBGModeByTableMain(sBG256x16PlttModeTable[n-2]);
    }
    else
    {
        ChangeBGModeByTableSub(sBG256x16PlttModeTable[n-6]);
    }
    SetBGnControl256x16Pltt(n, size, areaOver, scnBase, chrBase);
}



/*---------------------------------------------------------------------------*
  Name:         BGAutoControlText

  Description:  Sets the BGxCNT register as text background
                in the rendering object. Changes the BG mode at the same time.

  Arguments:    bg:         The control target BG.
                pScnData:   A pointer to the screen data.
                scnBase:    The background screen base
                chrBase:    The background character base

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetBGControlText(
    NNSG2dBGSelect bg,
    GXBGColorMode colorMode,
    int screenWidth,
    int screenHeight,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    const ScreenSizeMap* pSizeMap;

    NNS_G2D_BG_ASSERT(bg);

    pSizeMap = SelectScnSize(sTextScnSize, screenWidth, screenHeight);
    SDK_ASSERTMSG( pSizeMap != NULL,
        "Unsupported screen size(%dx%d) in input screen data",
        screenWidth,
        screenHeight
    );

    SetBGnControlToText(bg, (GXBGScrSizeText)pSizeMap->scnSize, colorMode, scnBase, chrBase);
}



/*---------------------------------------------------------------------------*
  Name:         BGAutoControlAffine

  Description:  Sets the BGxCNT register as affine background
                in the rendering object. Changes the BG mode at the same time.

  Arguments:    bg:         The control target BG.
                pScnData:   A pointer to the screen data.
                scnBase:    The background screen base
                chrBase:    The background character base

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetBGControlAffine(
    NNSG2dBGSelect bg,
    int screenWidth,
    int screenHeight,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    const ScreenSizeMap* pSizeMap;

    NNS_G2D_BG_ASSERT(bg);
    SDK_ASSERTMSG( (bg % 4) >= 2,
        "You can not show affine BG on %s BG %d",
        (IsMainBG(bg) ? "Main": "Sub"),
        (IsMainBG(bg) ? bg: bg - NNS_G2D_BGSELECT_SUB0) );

    pSizeMap = SelectScnSize(sAffineScnSize, screenWidth, screenHeight);
    SDK_ASSERTMSG( pSizeMap != NULL,
        "Unsupported screen size(%dx%d) in input screen data",
        screenWidth,
        screenHeight
    );

    SetBGnControlToAffine(bg, (GXBGScrSizeAffine)pSizeMap->scnSize, NNSi_G2dBGAreaOver, scnBase, chrBase);
}



/*---------------------------------------------------------------------------*
  Name:         BGAutoControlAffineExt

  Description:  Sets the BGxCNT register as affine extended background
                in the rendering object. Also changes the BG mode at the same time.

  Arguments:    bg:         The control target BG.
                pScnData:   A pointer to the screen data.
                scnBase:    The background screen base
                chrBase:    The background character base

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetBGControl256x16Pltt(
    NNSG2dBGSelect bg,
    int screenWidth,
    int screenHeight,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    const ScreenSizeMap* pSizeMap;

    NNS_G2D_BG_ASSERT(bg);
    SDK_ASSERTMSG( (bg % 4) >= 2,
        "You can not show affine BG on %s BG %d",
        (IsMainBG(bg) ? "Main": "Sub"),
        (IsMainBG(bg) ? bg: bg - NNS_G2D_BGSELECT_SUB0) );

    pSizeMap = SelectScnSize(sAffineExtScnSize, screenWidth, screenHeight);
    SDK_ASSERTMSG( pSizeMap != NULL,
        "Unsupported screen size(%dx%d) in input screen data",
        screenWidth,
        screenHeight
    );

    SetBGnControlTo256x16Pltt(bg, (GXBGScrSize256x16Pltt)pSizeMap->scnSize, NNSi_G2dBGAreaOver, scnBase, chrBase);
}



/*---------------------------------------------------------------------------*
  Name:         IsBGExtPlttSlotAssigned

  Description:  Determines whether VRAM is allocated to the specified
                palette slot.

  Arguments:    slot:   The target extended palette slot

  Returns:      TRUE if VRAM is allocated.
 *---------------------------------------------------------------------------*/
static BOOL IsBGExtPlttSlotAssigned(NNSG2dBGExtPlttSlot slot)
{
    if( IsMainBGExtPlttSlot(slot) )
    {
        if( slot <= NNS_G2D_BGEXTPLTTSLOT_MAIN1 )
        {
            return IsMainBGExtPltt01Available();
        }
        else
        {
            return IsMainBGExtPltt23Available();
        }
    }
    else
    {
        return IsSubBGExtPlttAvailable();
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGPaletteSelect

  Description:  Reads the palette data into VRAM.
                Specifies the background using the palette and where to read
                depending on whether the extended palette is used.

  Arguments:    bg:         The BG using the read palette data
                bToExtPltt: If TRUE, reads to the extended palette,
                            otherwise, reads to the standard palette.
                pPltData:   A pointer to the palette data.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void LoadBGPaletteSelect(
    NNSG2dBGSelect bg,
    BOOL bToExtPltt,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
    NNS_G2D_BG_ASSERT(bg);
    SDK_NULL_ASSERT(pPltData);

    if( bToExtPltt )
    {
        NNSG2dBGExtPlttSlot slot = GetBGExtPlttSlot(bg);

        SDK_ASSERT( IsBGExtPlttSlotAssigned(slot) );
        LoadBGPlttToExtendedPltt(slot, pPltData, pCmpInfo);
    }
    else
    {
        LoadBGPlttToNormalPltt(bg, pPltData, pCmpInfo);
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGPalette

  Description:  Reads the palette data into VRAM.
                Automatically determines the read destination from the G2D screen data and
                the background using the palette.

  Arguments:    bg:         The BG using the read palette data
                pPltData:   A pointer to the palette data.
                pScnData:   A pointer to the screen data using the
                            A pointer to the screen data.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void LoadBGPalette(
    NNSG2dBGSelect bg,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dScreenData* pScnData,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
/*
    Extended palette | Enabled Disabled
      Corresponding slot | Enabled Disabled
    -------------------------------------
    text    16x16   |Standard Standard Standard
            256x1   |Extended (Extended) Standard
            256x16  |Extended (Extended) [Standard]
    affine  256x1   |Standard Standard Standard
    extend  256x16  |Extended (Extended) [Standard]

    Unless transparent, items in parentheses are black
    The palette specification is ignored for items in square brackets
    Both are stopped with ASSERT
*/
    // TODO : Even when the extended palette is being used, the standard palette is used for the backdrop colors
    //*(GXRgb*)(HW_BG_PLTT) = pPltData->pRawData[0];

    NNS_G2D_BG_ASSERT(bg);
    SDK_NULL_ASSERT(pPltData);
    SDK_NULL_ASSERT(pScnData);

    if( (pScnData->screenFormat == NNS_G2D_SCREENFORMAT_TEXT)
        && (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_256x1) )
    {
        LoadBGPaletteSelect(bg, IsBGUseExtPltt(bg), pPltData, pCmpInfo);
    }
    else
    {
        BOOL bExtPlttData = ! (
            (pPltData->fmt == GX_TEXFMT_PLTT16)
            || (pScnData->screenFormat == NNS_G2D_SCREENFORMAT_AFFINE)
        );

        SDK_ASSERTMSG( ! bExtPlttData || IsBGUseExtPltt(bg),
            "Input screen data requires extended BG palette, but unavailable");
        LoadBGPaletteSelect(bg, bExtPlttData, pPltData, pCmpInfo);
    }
}




/*---------------------------------------------------------------------------*
  Name:         LoadBGCharacter

  Description:  Loads the character data for use by the target BG. 

  Arguments:    bg:         The target BG screen.
                pChrData:   Pointer to the character data. 
                pPosInfo:   Pointer to the character position data. 
                            When pChrData is not a partial character, NULL is specified. 

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void LoadBGCharacter(
    NNSG2dBGSelect bg,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dCharacterPosInfo* pPosInfo
)
{
    u32 offset = 0;

    NNS_G2D_BG_ASSERT(bg);
    SDK_NULL_ASSERT(pChrData);
    SDK_ASSERT( pPosInfo == NULL || pPosInfo->srcPosX == 0 );

    //---- When a partial character, calculate offset 
    if( pPosInfo != NULL )
    {
        int offsetChars = pPosInfo->srcPosY * pPosInfo->srcW;
        u32 szChar = (pChrData->pixelFmt == GX_TEXFMT_PLTT256) ?
                        sizeof(GXCharFmt256): sizeof(GXCharFmt16);

        offset = offsetChars * szChar;
    }

    DC_FlushRange(pChrData->pRawData, pChrData->szByte);
    LoadBGnChar(bg, pChrData->pRawData, offset, pChrData->szByte);
}



/*---------------------------------------------------------------------------*
  Name:         LoadBGScreen

  Description:  Loads the screen data for use by the target BG. 

  Arguments:    bg:         The target BG screen.
                pScnData:   Screen data to load. 

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void LoadBGScreen(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData
)
{
    GXScrFmtText* pDstBase;
    int plane_cwidth;
    int plane_cheight;
    int load_cwidth;
    int load_cheight;

    NNS_G2D_BG_ASSERT(bg);
    SDK_NULL_ASSERT(pScnData);

    //---- Gest the screen base 
    pDstBase = (GXScrFmtText*)GetBGnScrPtr(bg);

    //---- Outputs the transferrable size 
    {
        const int scn_cwidth  = pScnData->screenWidth / 8;
        const int scn_cheight = pScnData->screenHeight / 8;

        NNSi_G2dBGGetCharSize(&plane_cwidth, &plane_cheight, bg);
        load_cwidth = (plane_cwidth > scn_cwidth) ? scn_cwidth: plane_cwidth;
        load_cheight = (plane_cheight > scn_cheight) ? scn_cheight: plane_cheight;
    }


    //---- Transfer
    DC_FlushRange( (void*)pScnData->rawData, pScnData->szByte );
    NNS_G2dBGLoadScreenRect(
        pDstBase,
        pScnData,
        0, 0,
        0, 0,
        plane_cwidth,
        load_cwidth, load_cheight
    );
}






/*---------------------------------------------------------------------------*
  Name:         SetBGControlAuto

  Description:  Performs BG control of the target BG screen. 

  Arguments:    bg:             The target BG screen.
                screenFormat:   BG type. 
                colorMode:      Color mode
                screenWidth:    Screen width (in pixels)
                screenHeight:   Screen height (in pixels)
                schBase:        Screen base block. 
                chrBase:        Character base block.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SetBGControlAuto(
    NNSG2dBGSelect bg,
    NNSG2dScreenFormat screenFormat,
    GXBGColorMode colorMode,
    int screenWidth,
    int screenHeight,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    // BG Control
    switch( screenFormat )
    {
    case NNS_G2D_SCREENFORMAT_TEXT:
        SetBGControlText(bg, colorMode, screenWidth, screenHeight, scnBase, chrBase);
        break;

    case NNS_G2D_SCREENFORMAT_AFFINE:
        SetBGControlAffine(bg, screenWidth, screenHeight, scnBase, chrBase);
        break;

    case NNS_G2D_SCREENFORMAT_AFFINEEXT:
        SetBGControl256x16Pltt(bg, screenWidth, screenHeight, scnBase, chrBase);
        break;

    default:
        SDK_ASSERTMSG(FALSE, "TEXT, AFFINE, and 256x16 format support only");
        break;
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadScreenPartText

  Description:  Copies the specified rectangle in the screen data to the specified position in the buffer. 
                

  Arguments:    pScreenDst: Pointer the transfer destination base point. 
                pScnData:   Pointer to the screen data to be transferred. 
                srcX       :       X coordinate of the upper left corner of the transfer source. (in characters)
                srcY       :       Y coordinate of the upper left corner of the transfer source. (in characters)
                dstX:        X coordinate of the upper left corner of the transfer destination. (in characters)
                dstY:       Y coordinate of the upper left corner of the transfer destination. (in characters)
                dstW:       Width of the transfer destination area. (in characters)
                width:      Width of area to transfer. (in characters)
                height:     Height of area to transfer. (in characters)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void LoadScreenPartText(
    void* pScreenDst,
    const NNSG2dScreenData* pScnData,
    int srcX,
    int srcY,
    int dstX,
    int dstY,
    int dstW,
    int width,
    int height
)
{
    NNS_G2D_POINTER_ASSERT( pScreenDst );
    NNS_G2D_POINTER_ASSERT( pScnData );

    //---- Mapping for each character 
    {
        const int src_x_end             = srcX + width;
        const int src_y_end             = srcY + height;
        const int src_next_offset       = pScnData->screenWidth - width;
        const int dst_next_offset       = dstW - width;
        const u32 szLine                = sizeof(GXScrFmtText) * width;
        const int srcW                  = pScnData->screenWidth / 8;
        const GXScrFmtText* pSrcBase    = (const GXScrFmtText*)pScnData->rawData;
        GXScrFmtText* pDstBase          = (GXScrFmtText*)pScreenDst;
        int sx, sy;
        int dx, dy;

        for( sy = srcY, dy = dstY; sy < src_y_end; sy++, dy++ )
        {
            for( sx = srcX, dx = dstX; sx < src_x_end; sx++, dx++ )
            {
                const GXScrFmtText* pSrc = pSrcBase + CalcTextScreenOffset(sx, sy, srcW);
                GXScrFmtText* pDst = pDstBase + CalcTextScreenOffset(dx, dy, dstW);

                *pDst = *pSrc;
            }
        }
    }
}




/*---------------------------------------------------------------------------*
  Name:         LoadScreenPartAffine

  Description:  Copies the specified rectangle in the screen data to the specified position in the buffer. 
                  

  Arguments:    pScreenDst: Pointer the transfer destination base point. 
                pScnData:   Pointer to the screen data to be transferred. 
                srcX       :       X coordinate of the upper left corner of the transfer source. (in characters)
                srcY       :       Y coordinate of the upper left corner of the transfer source. (in characters)
                dstX:        X coordinate of the upper left corner of the transfer destination. (in characters)
                dstY:       Y coordinate of the upper left corner of the transfer destination. (in characters)
                dstW:       Width of the transfer destination area. (in characters)
                width:      Width of area to transfer. (in characters)
                height:     Height of area to transfer. (in characters)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void LoadScreenPartAffine(
    void* pScreenDst,
    const NNSG2dScreenData* pScnData,
    int srcX,
    int srcY,
    int dstX,
    int dstY,
    int dstW,
    int width,
    int height
)
{
    NNS_G2D_POINTER_ASSERT( pScreenDst );
    NNS_G2D_POINTER_ASSERT( pScnData );

    //---- Simply transfers rectangle 
    {
        const int src_y_end = srcY + height;
        const int srcW = pScnData->screenWidth / 8;
        const u32 szLine = sizeof(GXScrFmtAffine) * width;
        const GXScrFmtAffine* pSrc = (const GXScrFmtAffine*)pScnData->rawData;
        GXScrFmtAffine* pDst = (GXScrFmtAffine*)pScreenDst;
        int y;

        pSrc += srcY * srcW + srcX;
        pDst += dstY * dstW + dstX;

        for( y = srcY; y < src_y_end; y++ )
        {
            MI_CpuCopy8(pSrc, pDst, szLine);
            pSrc += srcW;
            pDst += dstW;
        }
    }
}



/*---------------------------------------------------------------------------*
  Name:         LoadScreenPart256x16Pltt

  Description:  Copies the specifies rectangle in the screen data to the specified position in the buffer. 
                 

  Arguments:    pScreenDst: Pointer the transfer destination base point. 
                pScnData:   Pointer to the screen data to be transferred. 
                srcX       :       X coordinate of the upper left corner of the transfer source. (in characters)
                srcY       :       Y coordinate of the upper left corner of the transfer source. (in characters)
                dstX:        X coordinate of the upper left corner of the transfer destination. (in characters)
                dstY:       Y coordinate of the upper left corner of the transfer destination. (in characters)
                dstW:       Width of the transfer destination area. (in characters)
                width:      Width of area to transfer. (in characters)
                height:     Height of area to transfer. (in characters)

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void LoadScreenPart256x16Pltt(
    void* pScreenDst,
    const NNSG2dScreenData* pScnData,
    int srcX,
    int srcY,
    int dstX,
    int dstY,
    int dstW,
    int width,
    int height
)
{
    NNS_G2D_POINTER_ASSERT( pScreenDst );
    NNS_G2D_POINTER_ASSERT( pScnData );

    //---- Simply transfers rectangle 
    {
        const int src_y_end = srcY + height;
        const int srcW = pScnData->screenWidth / 8;
        const u32 szLine = sizeof(GXScrFmtText) * width;
        const GXScrFmtText* pSrc = (const GXScrFmtText*)pScnData->rawData;
        GXScrFmtText* pDst = (GXScrFmtText*)pScreenDst;
        int y;


        pSrc += srcY * srcW + srcX;
        pDst += dstY * dstW + dstX;

        for( y = srcY; y < src_y_end; y++ )
        {
            MI_CpuCopy16(pSrc, pDst, szLine);
            pSrc += srcW;
            pDst += dstW;
        }
    }
}












//****************************************************************************
// Public Functions
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGLoadElements

  Description:  The graphics data used by BG is read into the appropriate VRAM.
                The BGControl for the target background needs to be set appropriately.
                The BG0-1 extended palette load target is automatically determined using the VRAM allocation state for the extended palettes. 
                
                Switches the load destination based on whether the extended palette is enabled for the Text BG's 256x1 palette. 
                

  Arguments:    bg:         The background using the data to be loaded
                pScnData:   A pointer to the screen data to be loaded into VRAM.
                            Can be NULL, but if pPltData is not NULL, 
                            it cannot be null. 
                pChrData:   A pointer to the character data to be loaded into VRAM.
                            VRAM is not read when NULL is used.
                pPltData:   A pointer to the palette data to be loaded into VRAM.
                            VRAM is not read when NULL is used.
                pPosInfo:   Pointer to character extraction area information. 
                pCmpInfo:   A pointer to the palette compression data.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dBGLoadElementsEx(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dCharacterPosInfo* pPosInfo,
    const NNSG2dPaletteCompressInfo* pCmpInfo
)
{
    NNS_G2D_BG_ASSERT(bg);
    NNS_G2D_ASSERT( pPltData == NULL || pScnData != NULL );
    NNS_G2D_ASSERT( pPosInfo == NULL || pChrData != NULL );
    NNS_G2D_ASSERT( pCmpInfo == NULL || pPltData != NULL );

    NNS_G2D_ASSERTMSG( pChrData == NULL || pScnData == NULL
                || ( (pChrData->pixelFmt == GX_TEXFMT_PLTT16)
                    == (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ),
                "Color mode mismatch between character data and screen data" );
    NNS_G2D_ASSERTMSG( pPltData == NULL
                || ( (pPltData->fmt == GX_TEXFMT_PLTT16)
                    == (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ),
                "Color mode mismatch between palette data and screen data" );
    NNS_G2D_ASSERT( pChrData == NULL
                || ( NNSi_G2dGetCharacterFmtType( pChrData->characterFmt )
                    == NNS_G2D_CHARACTER_FMT_CHAR ) );

    if( pPltData != NULL && pScnData != NULL )
    {
        LoadBGPalette(bg, pPltData, pScnData, pCmpInfo);
    }
    if( pChrData != NULL )
    {
        LoadBGCharacter(bg, pChrData, pPosInfo);
    }
    if( pScnData != NULL )
    {
        LoadBGScreen(bg, pScnData);
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGSetupEx

  Description:  Sets the background and reads its data into VRAM.

  Arguments:    bg:         The target BG
                pScnData:   A pointer to the screen data used by BG.
                            NULL may not be used.
                pChrData:   A pointer to the character data used by BG.
                            VRAM is not read when NULL is used.
                pPltData:   A pointer to the palette data used by BG.
                            VRAM is not read when NULL is used.
                pPosInfo:   Pointer to character extraction area information. 
                pCmpInfo:   A pointer to the palette compression data.
                scnBase:    The background screen base.
                chrBase:    The background character base.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dBGSetupEx(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dPaletteData* pPltData,
    const NNSG2dCharacterPosInfo* pPosInfo,
    const NNSG2dPaletteCompressInfo* pCmpInfo,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
    NNS_G2D_BG_ASSERT(bg);
    NNS_G2D_NULL_ASSERT( pScnData );
    NNS_G2D_ASSERT( pPosInfo == NULL || pChrData != NULL );
    NNS_G2D_ASSERT( pCmpInfo == NULL || pPltData != NULL );

    NNS_G2D_ASSERTMSG( pChrData == NULL
                || ( (pChrData->pixelFmt == GX_TEXFMT_PLTT16)
                    == (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ),
                "Color mode mismatch between character data and screen data" );
    NNS_G2D_ASSERTMSG( pPltData == NULL
                || ( (pPltData->fmt == GX_TEXFMT_PLTT16)
                    == (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ),
                "Color mode mismatch between palette data and screen data" );
    NNS_G2D_ASSERT( pChrData == NULL
                || ( NNSi_G2dGetCharacterFmtType( pChrData->characterFmt )
                    == NNS_G2D_CHARACTER_FMT_CHAR ) );

    SetBGControlAuto(
        bg,
        NNSi_G2dBGGetScreenFormat(pScnData),
        NNSi_G2dBGGetScreenColorMode(pScnData),
        pScnData->screenWidth,
        pScnData->screenHeight,
        scnBase,
        chrBase );
    NNS_G2dBGLoadElementsEx(bg, pScnData, pChrData, pPltData, pPosInfo, pCmpInfo);
}




/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGLoadScreenRect

  Description:  Copies the specifies rectangle in the screen data to the specified position in the buffer. 
                  

  Arguments:    pScreenDst: Pointer the transfer destination base point. 
                pScnData:   Pointer to the screen data to be transferred. 
                srcX       :       X coordinate of the upper left corner of the transfer source. (in characters)
                srcY       :       Y coordinate of the upper left corner of the transfer source. (in characters)
                dstX:        X coordinate of the upper left corner of the transfer destination. (in characters)
                dstY:       Y coordinate of the upper left corner of the transfer destination. (in characters)
                dstW:       Width of the transfer destination area. (in characters)
                width:      Width of area to transfer. (in characters)
                height:     Height of area to transfer. (in characters)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNS_G2dBGLoadScreenRect(
    void* pScreenDst,
    const NNSG2dScreenData* pScnData,
    int srcX,
    int srcY,
    int dstX,
    int dstY,
    int dstW,
    int width,
    int height
)
{
    NNS_G2D_POINTER_ASSERT( pScreenDst );
    NNS_G2D_POINTER_ASSERT( pScnData );

    switch( pScnData->screenFormat )
    {
    case NNS_G2D_SCREENFORMAT_TEXT:
        LoadScreenPartText(pScreenDst, pScnData, srcX, srcY, dstX, dstY, dstW, width, height);
        break;

    case NNS_G2D_SCREENFORMAT_AFFINE:
        LoadScreenPartAffine(pScreenDst, pScnData, srcX, srcY, dstX, dstY, dstW, width, height);
        break;

    case NNS_G2D_SCREENFORMAT_AFFINEEXT:
        LoadScreenPart256x16Pltt(pScreenDst, pScnData, srcX, srcY, dstX, dstY, dstW, width, height);
        break;

    default:
        NNS_G2D_ASSERTMSG(FALSE, "Unknown screen format(=%d)", pScnData->screenFormat );
    }
}



