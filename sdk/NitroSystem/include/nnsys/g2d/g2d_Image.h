/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_Image.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Image.h,v $
  Revision 1.15  12/24/2004 02:11:43 AM  kitani_toshikazu
  Added the function group to execute the load process and proxy initialization at different timings.

  Revision 1.14  10/18/2004 07:54:06 AM  kitani_toshikazu
  Revised misspelling

  Revision 1.13  09/17/2004 05:28:09 AM  kitani_toshikazu
  NNS_G2dLoadPaletteCopmressed => Changed name to NNS_G2dLoadPaletteEx

  Revision 1.12  09/15/2004 05:53:15 AM  kitani_toshikazu
  Prepared support for loading palette sections

  Revision 1.11  07/29/2004 12:34:20 AM  kitani_toshikazu
  Changed function names.

  Revision 1.10  07/28/2004 08:13:01 AM  kitani_toshikazu
  Corrected comment

  Revision 1.9  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.8  07/16/2004 10:57:48 AM  kitani_toshikazu
  Added NNS_G2D_VRAM_ADDR_NONE definition

  Revision 1.7  07/16/2004 07:24:12 AM  kitani_toshikazu
  Added NNS_G2dSetVramTransferImageProxy().
  Changed part of externally opened function to open in library.

  Revision 1.6  07/14/2004 12:28:10 PM  kitani_toshikazu
  Support for Vram transfer animation.

  Revision 1.5  06/01/2004 05:20:33 AM  kitani_toshikazu
  Changes for using expansion palette.

  Revision 1.4  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.3  05/10/2004 04:42:28 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:54 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_G2D_IMAGE_H_
#define NNS_G2D_IMAGE_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_Character_data.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// aliases of functions with names changed
// Previous functions declared as aliases to preserve compatibility.
// 
#define NNS_G2dInitializeImageProxy         NNS_G2dInitImageProxy
#define NNS_G2dInitializeImagePaletteProxy  NNS_G2dInitImagePaletteProxy 

/*---------------------------------------------------------------------------*
  Name:         NNS_G2D_VRAM_TYPE

  Description:  VRAM type
 *---------------------------------------------------------------------------*/
typedef enum NNS_G2D_VRAM_TYPE
{
    NNS_G2D_VRAM_TYPE_3DMAIN = 0,
    NNS_G2D_VRAM_TYPE_2DMAIN = 1,
    NNS_G2D_VRAM_TYPE_2DSUB  = 2,
    NNS_G2D_VRAM_TYPE_MAX    = 3

}NNS_G2D_VRAM_TYPE;

#define NNS_G2D_VRAM_TYPE_VALID( val )                   \
    NNS_G2D_ASSERTMSG( (val) == NNS_G2D_VRAM_TYPE_3DMAIN ||  \
                   (val) == NNS_G2D_VRAM_TYPE_2DMAIN ||  \
                   (val) == NNS_G2D_VRAM_TYPE_2DSUB  ,   \
                    "Invalid NNS_G2D_VRAM_TYPE type" )   \

#define NNS_G2D_VRAM_ADDR_NOT_INITIALIZED       0xFFFFFFFF
#define NNS_G2D_VRAM_ADDR_NONE                  NNS_G2D_VRAM_ADDR_NOT_INITIALIZED

/*---------------------------------------------------------------------------*
  Name:         NNSG2dImageAttr

  Description:  Image Attributes
                NNSG2dImageProxy has this function.
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dImageAttr
{
    GXTexSizeS                      sizeS;          // image size (Corrupt data is set during 1D mapping.)
    GXTexSizeT                      sizeT;          // image size (Corrupt data is set during 1D mapping.)
    
    GXTexFmt                        fmt;            // image format
    BOOL                            bExtendedPlt;   // whether or not to use the extended palette
    
    GXTexPlttColor0                 plttUse;        // how to use palette number 0
    GXOBJVRamModeChar               mappingType;    // mapping mode
}
NNSG2dImageAttr;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dVRamLocation

  Description:  Actual address in VRAM.
                Contains the VRAM type actual address.
                NNSG2dImageProxy has this function.
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dVRamLocation
{
    u32                 baseAddrOfVram[ NNS_G2D_VRAM_TYPE_MAX ];
}
NNSG2dVRamLocation;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dImageProxy

  Description:  Structure that expresses image.
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dImageProxy
{
    NNSG2dVRamLocation        vramLocation;           
    NNSG2dImageAttr           attr;
}
NNSG2dImageProxy;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dImagePaletteProxy

  Description:  Structure that expresses image palette.
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dImagePaletteProxy
{
    
    GXTexFmt                  fmt;          // palette format
                                            // (values can obtain are limited to GX_TEXFMT_PLTT16 and GX_TEXFMT_PLTT256)
                                            
    BOOL                      bExtendedPlt; // use expansion palette or not
    
    NNSG2dVRamLocation        vramLocation;           
}
NNSG2dImagePaletteProxy;


// For Image

void NNS_G2dInitImageProxy   ( NNSG2dImageProxy* pImg );


void NNS_G2dSetImageLocation       ( NNSG2dImageProxy* pImg, NNS_G2D_VRAM_TYPE type, u32 addr );



u32  NNS_G2dGetImageLocation       ( const NNSG2dImageProxy* pImg, NNS_G2D_VRAM_TYPE type );
BOOL NNS_G2dIsImageReadyToUse      ( const NNSG2dImageProxy* pImg, NNS_G2D_VRAM_TYPE type );


void NNS_G2dLoadImage1DMapping
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy 
);

void NNS_G2dLoadImage2DMapping
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy 
);


void NNS_G2dLoadImageVramTransfer
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy 
);

void NNS_G2dLoadPalette
(
    const NNSG2dPaletteData*    pSrcData,
    u32                         addr,
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImagePaletteProxy*    pPltProxy
);

void NNS_G2dLoadPaletteEx
(
    const NNSG2dPaletteData*            pSrcData,
    const NNSG2dPaletteCompressInfo*    pCmpInfo,
    u32                                 addr,
    NNS_G2D_VRAM_TYPE                   type, 
    NNSG2dImagePaletteProxy*            pPltProxy
);

// For ImagePalette
void           NNS_G2dInitImagePaletteProxy( NNSG2dImagePaletteProxy* pImg );
void           NNS_G2dSetImagePaletteLocation( NNSG2dImagePaletteProxy* pImg, NNS_G2D_VRAM_TYPE type, u32 addr );
u32            NNS_G2dGetImagePaletteLocation( const NNSG2dImagePaletteProxy* pImg, NNS_G2D_VRAM_TYPE type );
BOOL           NNS_G2dIsImagePaletteReadyToUse( const NNSG2dImagePaletteProxy* pImg, NNS_G2D_VRAM_TYPE type );
static BOOL    NNS_G2dIsPaletteImageFmt( const NNSG2dImageAttr* imgAttr );


//------------------------------------------------------------------------------
void NNSi_G2dInitializeVRamLocation( NNSG2dVRamLocation* pVramLocation );
void NNSi_G2dSetVramLocation( NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type, u32 addr );
u32  NNSi_G2dGetVramLocation( const NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type );
BOOL NNSi_G2dIsVramLocationReadyToUse( const NNSG2dVRamLocation* pVramLocation, NNS_G2D_VRAM_TYPE type );

void NNSi_G2dDoImageLoadingToVram
( 
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr,
    NNS_G2D_VRAM_TYPE           type
);
void NNSi_G2dSetupImageProxyPrams
(
    const NNSG2dCharacterData*  pSrcData, 
    u32                         baseAddr, 
    NNS_G2D_VRAM_TYPE           type, 
    NNSG2dImageProxy*           pImgProxy
);

void NNSi_G2dDoLoadingPaletteToVram
(
    const NNSG2dPaletteData*    pSrcData,
    u32                         addr,
    NNS_G2D_VRAM_TYPE           type
);

void NNSi_G2dDoLoadingPaletteToVramEx
(
    const NNSG2dPaletteData*            pSrcData,
    const NNSG2dPaletteCompressInfo*    pCmpInfo,
    u32                                 addr,
    NNS_G2D_VRAM_TYPE                   type   
);

void NNSi_G2dSetupPaletteProxyPrams
(
    const NNSG2dPaletteData*            pSrcData,
    u32                                 addr,
    NNS_G2D_VRAM_TYPE                   type, 
    NNSG2dImagePaletteProxy*            pPltProxy
);

//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

NNS_G2D_INLINE BOOL NNS_G2dIsPaletteImageFmt( const NNSG2dImageAttr* imgAttr )
{
    NNS_G2D_NULL_ASSERT( imgAttr );
    return ( imgAttr->fmt == GX_TEXFMT_PLTT4   ||
             imgAttr->fmt == GX_TEXFMT_PLTT16  ||
             imgAttr->fmt == GX_TEXFMT_PLTT256 ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNS_G2dSetImageExtPaletteFlag( NNSG2dImageProxy* pImgProxy, BOOL bUseExtPlt )
{
    NNS_G2D_NULL_ASSERT( pImgProxy );
    pImgProxy->attr.bExtendedPlt = bUseExtPlt;
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_IMAGE_H_

