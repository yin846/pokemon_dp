/*---------------------------------------------------------------------------*
  Project:  NNS - 2D
  File:     g2d_Character.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Character_data.h,v $
  Revision 1.12  06/21/2005 02:28:40 AM  kitani_toshikazu
  Revised copyright notation

  Revision 1.11  06/13/2005 08:24:35 AM  kitani_toshikazu
  File format version update

  Revision 1.10  12/24/2004 02:11:51 AM  kitani_toshikazu
  (none)

  Revision 1.9  09/17/2004 06:34:33 AM  kitani_toshikazu
  (none)

  Revision 1.8  09/17/2004 01:49:59 AM  kitani_toshikazu
  Added palette compression information

  Revision 1.7  09/15/2004 05:53:30 AM  kitani_toshikazu
  Prepared support for loading palette sections

  Revision 1.6  09/02/2004 01:49:39 AM  kitani_toshikazu
  Added NNS_G2D_BINFILE_SIG_PALETTEDATA_OLD

  Revision 1.5  08/25/2004 06:57:49 AM  kitani_toshikazu
  Corrected value of NNS_G2D_BINFILE_SIG_PALETTEDATA

  Revision 1.4  07/14/2004 12:30:10 PM  kitani_toshikazu
  Changed version information
  Support for Vram transfer animation.

  Revision 1.3  2004/06/23 06:47:52 AM  nishida_kenji
  Fix header comments.

  Revision 1.2  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.1  05/10/2004 04:42:28 AM  kitani_toshikazu
  Check-in for PR1.

  

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_CHARACTER_DATA_H_
#define NNS_G2D_CHARACTER_DATA_H_

#include <nitro/types.h>

#ifndef SDK_ARM9
#define SDK_ARM9
    #include <nitro_win32.h>
#undef  SDK_ARM9
#endif //SDK_ARM9

#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifdef __cplusplus
extern "C" {
#endif

// Binary file signature
#define NNS_G2D_BINFILE_SIG_CHARACTERDATA          (u32)'NCGR'

#define NNS_G2D_BINFILE_SIG_PALETTEDATA            (u32)'NCLR'
#define NNS_G2D_BINFILE_SIG_PALETTEDATA_OLD        (u32)'NCPR'

// Binary block signature
#define NNS_G2D_BINBLK_SIG_CHARACTERDATA          (u32)'CHAR'
#define NNS_G2D_BINBLK_SIG_CHAR_POSITION          (u32)'CPOS'

#define NNS_G2D_BINBLK_SIG_PALETTEDATA            (u32)'PLTT'
#define NNS_G2D_BINBLK_SIG_PALETTECOMPINFO        (u32)'PCMP'



#define NNS_G2D_BINFILE_EXT_CHARACTERDATA          "NCGR"
#define NNS_G2D_BINFILE_EXT_CHARACTERDATA_BMP      "NCBR"
#define NNS_G2D_BINFILE_EXT_PALETTEDATA            "NCLR"

//
// Version information
// Ver         Changed
// -------------------------------------
// 1.0         Initial
// 1.1         Added NNSG2dCharacterPosInfoBlock 
//
#define NNS_G2D_NCGR_MAJOR_VER             (u8)1
#define NNS_G2D_NCGR_MINOR_VER             (u8)1

//
// Version information
// Ver         Changed
// -------------------------------------
// 1.0         Initial
//
#define NNS_G2D_NCLR_MAJOR_VER             (u8)1
#define NNS_G2D_NCLR_MINOR_VER             (u8)0

#define NNS_G2D_1D_MAPPING_CHAR_SIZE      (u16)0xFFFF

#define NNS_G2D_CHARACTER_FMT_MASK              0xFF    // 8 bit
#define NNS_G2D_VRAMTRANSFERDATA_FLAG_MASK      0x1     // 1 bit
#define NNS_G2D_CHARACTER_FMT_SHIFT             0       
#define NNS_G2D_VRAMTRANSFERDATA_FLAG_SHIFT     8

//------------------------------------------------------------------------------
typedef enum NNSG2dCharacterPixelFmt
{
    NNS_G2D_CHARACTER_PIXELFMT_16,
    NNS_G2D_CHARACTER_PIXELFMT_256,
    NNS_G2D_CHARACTER_PIXELFMT_MAX
    
}NNSG2dCharacterPixelFmt;

// Data arrangement method of character data
typedef enum NNSG2dCharacterFmt
{
    NNS_G2D_CHARACTER_FMT_CHAR,  // Arranged every 8*8 texel
    NNS_G2D_CHARACTER_FMT_BMP,   // General bit map method
    NNS_G2D_CHARACTER_FMT_MAX

}NNSG2dCharacterFmt;

//------------------------------------------------------------------------------
typedef enum NNSG2dCharacterDataMapingType
{
    NNS_G2D_CHARACTERMAPING_1D_32,
    NNS_G2D_CHARACTERMAPING_1D_64,
    NNS_G2D_CHARACTERMAPING_1D_128,
    NNS_G2D_CHARACTERMAPING_1D_256,
    NNS_G2D_CHARACTERMAPING_2D,
    NNS_G2D_CHARACTERMAPING_MAX
 
}NNSG2dCharacterDataMapingType;

//------------------------------------------------------------------------------
typedef enum NNSG2dPaletteFmt
{
    NNS_G2D_NNSG2DPALETTEFMT_16X16,
    NNS_G2D_NNSG2DPALETTEFMT_256X1,
    NNS_G2D_NNSG2DPALETTEFMT_256X16,
    NNS_G2D_NNSG2DPALETTEFMT_MAX
    
}NNSG2dPaletteFmt;

//------------------------------------------------------------------------------
typedef struct NNSG2dPaletteCompressInfo
{
    u16               numPalette;
    u16               pad16;
    void*             pPlttIdxTbl;        // offset addr.
    
}NNSG2dPaletteCompressInfo;

//------------------------------------------------------------------------------
typedef struct NNSG2dPaletteData
{
    
    GXTexFmt            fmt;
    BOOL                bExtendedPlt;
    
    u32                 szByte;
    void*               pRawData;          // offset addr of the content.
    
}NNSG2dPaletteData;

//------------------------------------------------------------------------------
typedef struct NNSG2dPaletteCompressDataBlock
{
    
    NNSG2dBinaryBlockHeader             blockHeader;
    NNSG2dPaletteCompressInfo           plttCmpInfo;
    
}NNSG2dPaletteCompressDataBlock;

//------------------------------------------------------------------------------
typedef struct NNSG2dPaletteDataBlock
{
    
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dPaletteData           paletteData;
    
}NNSG2dPaletteDataBlock;

//------------------------------------------------------------------------------
typedef struct NNSG2dCharacterData
{
    
    u16                             H;           
    u16                             W;           
    
    GXTexFmt                        pixelFmt;    // GXTexFmt
    
    GXOBJVRamModeChar               mapingType;  // GXOBJVRamModeChar
    u32                             characterFmt;// 31 ..... 10 ...........9 8 ................ 0
                                                 //     Reserved  VramTransfer   NNSG2dCharacterFmt
    u32                             szByte;
    void*                           pRawData;  // offset addr of the content.
    
}NNSG2dCharacterData;


//------------------------------------------------------------------------------
typedef struct NNSG2dCharacterDataBlock
{
    
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dCharacterData         characterData;
    
}NNSG2dCharacterDataBlock;


//------------------------------------------------------------------------------
typedef struct NNSG2dCharacterPosInfo
{
    
    u16                   srcPosX;  
    u16                   srcPosY;
    u16                   srcW;
    u16                   srcH;
    
}NNSG2dCharacterPosInfo;

//------------------------------------------------------------------------------
typedef struct NNSG2dCharacterPosInfoBlock
{
    
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dCharacterPosInfo         posInfo;
    
}NNSG2dCharacterPosInfoBlock;


//------------------------------------------------------------------------------
// << This function is used by the converter.
static inline u32 NNSi_G2dPackCharacterFmtData
( 
    NNSG2dCharacterFmt          charFmt,
    BOOL                        bVramTransferData 
)
{
    return (u32)( (charFmt << NNS_G2D_CHARACTER_FMT_SHIFT) | 
                  ( bVramTransferData << NNS_G2D_VRAMTRANSFERDATA_FLAG_SHIFT ) );
}

//------------------------------------------------------------------------------
// Obtains the arrangement method of the character
static inline NNSG2dCharacterFmt NNSi_G2dGetCharacterFmtType( u32 characterFmt )
{
    return (NNSG2dCharacterFmt)
    (NNS_G2D_CHARACTER_FMT_MASK & (characterFmt >> NNS_G2D_CHARACTER_FMT_SHIFT));
}

//------------------------------------------------------------------------------
static inline BOOL NNSi_G2dIsCharacterVramTransfered( u32 characterFmt )
{
    return (BOOL)
    (NNS_G2D_VRAMTRANSFERDATA_FLAG_MASK & ( characterFmt >> NNS_G2D_VRAMTRANSFERDATA_FLAG_SHIFT ) );
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_CHARACTER_DATA_H_


