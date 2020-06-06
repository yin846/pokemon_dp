/*---------------------------------------------------------------------------*
  Project:   NitroSystem - Graphics 2D
  File:     g2d_Screen.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Screen.h,v $
  Revision 1.5  06/29/2005 02:45:20 AM  kitani_toshikazu
  Changed copyright text 

  Revision 1.4  06/27/2005 05:57:39 AM  hatamoto_minoru
  Some internal functions were divided and put into g2di_BGManipulate. 
  In some processes, accesses registers directly without using SDK functions. 
  Added Ex functions that support partial characters and compressed palettes. 

  Revision 1.3  09/15/2004 07:55:03 AM  hatamoto_minoru
  Moved NNS_G2D_BG_ASSERT from .c to .h

  Revision 1.2  09/03/2004 09:30:24 AM  hatamoto_minoru
  Corrected a file name in the comments.
  Corrected the macro name to prevent multiple includes.

  Revision 1.1  09/02/2004 07:07:16 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_SCREEN_H_
#define NNS_G2D_SCREEN_H_

#include <nitro.h>
#include <nnsys/g2d.h>
#include <nnsys/g2d/fmt/g2d_Screen_data.h>

#ifdef __cplusplus
extern "C" {
#endif



//-------------------------------------------------------------------------
// Enumerators
//-------------------------------------------------------------------------

// For specifying the processing target background
typedef enum NNSG2dBGSelect
{
    NNS_G2D_BGSELECT_MAIN0,
    NNS_G2D_BGSELECT_MAIN1,
    NNS_G2D_BGSELECT_MAIN2,
    NNS_G2D_BGSELECT_MAIN3,
    NNS_G2D_BGSELECT_SUB0,
    NNS_G2D_BGSELECT_SUB1,
    NNS_G2D_BGSELECT_SUB2,
    NNS_G2D_BGSELECT_SUB3,
    NNS_G2D_BGSELECT_NUM
}
NNSG2dBGSelect;

#define NNS_G2D_BG_ASSERT(bg) SDK_MINMAX_ASSERT( bg, NNS_G2D_BGSELECT_MAIN0, NNS_G2D_BGSELECT_NUM-1 )


//-------------------------------------------------------------------------
// Function Declaration
//-------------------------------------------------------------------------

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
);

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
);

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
);


//-------------------------------------------------------------------------
// Global variables  (Private)
//-------------------------------------------------------------------------

extern GXBGAreaOver NNSi_G2dBGAreaOver;




//-------------------------------------------------------------------------
// inline function
//-------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBGGetScreenColorMode

  Description:  Gets the screen data's color mode. 

  Arguments:    pScnData:   A pointer to the screen data.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline GXBGColorMode NNSi_G2dBGGetScreenColorMode(const NNSG2dScreenData* pScnData)
{
	NNS_G2D_POINTER_ASSERT( pScnData );
    return (pScnData->colorMode == NNS_G2D_SCREENCOLORMODE_16x16) ?
                GX_BG_COLORMODE_16: GX_BG_COLORMODE_256;
}



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBGGetScreenFormat

  Description:  Gets the screen data's screen format. 

  Arguments:    pScnData:   A pointer to the screen data.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline NNSG2dScreenFormat NNSi_G2dBGGetScreenFormat(const NNSG2dScreenData* pScnData)
{
	NNS_G2D_POINTER_ASSERT( pScnData );
    return (NNSG2dScreenFormat)pScnData->screenFormat;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetBGAreaOver

  Description:  Specifies to use the Affine (extended) background
                area over processing specification with NNS_G2dLoadBGScreenSet() .

  Arguments:    areaOver:   Specifies the area over processing to be used with
                            subsequent NNS_G2dLoadBGScreenSet().

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dSetBGAreaOver( GXBGAreaOver areaOver )
{
    SDK_MINMAX_ASSERT( areaOver, GX_BG_AREAOVER_XLU, GX_BG_AREAOVER_REPEAT );
    NNSi_G2dBGAreaOver = areaOver;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetBGAreaOver

  Description:  Gets the area over processing used by NNS_G2dLoadBGScreenSet().
                 

  Arguments:    None.

  Returns:      The area over processing used with NNS_G2dLoadBGScreenSet().
 *---------------------------------------------------------------------------*/
static inline GXBGAreaOver NNS_G2dGetBGAreaOver( void )
{
    return NNSi_G2dBGAreaOver;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dBGSetup

  Description:  Sets the background and reads its data into VRAM.

  Arguments:    bg:         The target BG
                pScnData:   A pointer to the screen data used by BG.
                            NULL may not be used.
                pChrData:   A pointer to the character data used by BG.
                            VRAM is not read when NULL is used.
                pPltData:   A pointer to the palette data used by BG.
                            VRAM is not read when NULL is used.
                scnBase:    The background screen base.
                chrBase:    The background character base.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dBGSetup(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dPaletteData* pPltData,
    GXBGScrBase scnBase,
    GXBGCharBase chrBase
)
{
	NNS_G2dBGSetupEx(bg, pScnData, pChrData, pPltData, NULL, NULL, scnBase, chrBase);
}

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

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dBGLoadElements(
    NNSG2dBGSelect bg,
    const NNSG2dScreenData* pScnData,
    const NNSG2dCharacterData* pChrData,
    const NNSG2dPaletteData* pPltData
)
{
	NNS_G2dBGLoadElementsEx(bg, pScnData, pChrData, pPltData, NULL, NULL);
}


//-------------------------------------------------------------------------





#ifdef __cplusplus
}/* extern "C"*/
#endif

#endif // NNS_G2D_SCREEN_H_


