/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2 D 
  
  File:     g2d_OamSoftwareSpriteDraw.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_OamSoftwareSpriteDraw.c,v $
  Revision 1.23  2005/06/29 08:30:39  kitani_toshikazu
  Revised comments

  Revision 1.22  06/29/2005 01:01:36 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.21  2005/06/01 05:08:15 AM  kitani_toshikazu
  Support for drawing OBJs with double-size affine flag that was set

  Revision 1.20  05/27/2005 07:44:47 AM  kitani_toshikazu
  Changed direction of square polygon planes. (To display correctly in CULL_BACK mode)

  Revision 1.19  04/28/2005 02:31:09 AM  kitani_toshikazu
  Replaced NNSi_G2dRepeatXinCellSpace()
  with NNS_G2dRepeatXinCellSpace

  Revision 1.18  02/22/2005 12:51:14 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.17  02/21/2005 02:17:57 AM  hatamoto_minoru
  Response for warnings about implicit sign/unsign conversion.

  Revision 1.16  12/15/2004 12:59:10 PM  kitani_toshikazu
  Fixed a problem with the UV value calculation method when in 256 color character, 1D mapping mode.

  Revision 1.15  10/28/2004 02:01:50 AM  kitani_toshikazu
  Deleted unnecessary file include statement

  Revision 1.14  10/28/2004 12:27:52 AM  kitani_toshikazu
  (none)

  Revision 1.13  10/28/2004 12:04:27 AM  kitani_toshikazu
  Optimized performance

  Revision 1.12  10/18/2004 07:41:19 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.11  10/14/2004 12:58:53 AM  kitani_toshikazu
  Cleaned up the code

  Revision 1.10  10/08/2004 04:39:44 AM  kitani_toshikazu
  Changed function names.

  Revision 1.9  10/07/2004 12:47:29 PM  kitani_toshikazu
  Optimized performance

  Revision 1.8  08/26/2004 07:20:48 AM  kitani_toshikazu
  Implementation of the Z offset features

  Revision 1.7  08/23/2004 08:46:06 AM  kitani_toshikazu
  Changes to the UV value and the argument type

  Revision 1.6  08/17/2004 06:02:21 AM  kitani_toshikazu
  Implementation of the automatic Z value offset features

  Revision 1.5  07/29/2004 02:49:31 AM  kitani_toshikazu
  Arranged source.

  Revision 1.4  07/28/2004 08:12:42 AM  kitani_toshikazu
  Corrected comment

  Revision 1.3  07/23/2004 07:49:13 AM  kitani_toshikazu
  NNS_G2dDrawOneOam3DDirectWithPosAffineFast()
  did not work properly for anything but double affine mode. Fixed.

  Revision 1.2  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.1  07/17/2004 07:23:46 AM  kitani_toshikazu
  Changed file name from g2d_OamSoftwareSpriteEmu to g2d_OamSoftwareSpriteDraw.

  Revision 1.1  07/17/2004 06:55:16 AM  kitani_toshikazu
  Separated OAM software emulation drawing relation as separate module.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#include <nnsys/g2d/g2d_OAM.h>
#include <nnsys/g2d/g2d_Softsprite.h>

#include <nnsys/g2d/g2d_OamSoftwareSpriteDraw.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>


#include "g2d_Internal.h"

#define NNSI_G2D_SHIFT_SIZEOF_256PLTT   9   // means ( sizeof( u16 ) * 256 )
#define NNSI_G2D_SHIFT_SIZEOF_16PLTT    5   // means ( sizeof( u16 ) * 16 )

typedef struct SoftwareSpriteParamCache
{
    fx32       u0;
    fx32       v0;
    fx32       u1;
    fx32       v1;
    
}SoftwareSpriteParamCache;

typedef struct SpriteParams
{
    fx32   u0;
    fx32   v0;
    fx32   u1;
    fx32   v1;
    
    int    sx;
    int    sy;
    
}SpriteParams;



//
// Parameter cache
// Use when drawing a large volume of sprites that have the exact same texture settings.
static SoftwareSpriteParamCache         softwareSpreiteParamCache_ =
{
    0,
    0,  
    0,
    0
};




// UV value correction function pointer used in Oam software sprite emulation.
static NNS_G2dOamSoftEmuUVFlipCorrectFunc        s_pUVFlipCorrectFunc   = NULL;



// GXTexFmt character size table
static const u32 characterShiftSize_[] = 
{
    0,// GX_TEXFMT_NONE   
    0,// GX_TEXFMT_A3I5   
    0,// GX_TEXFMT_PLTT4  
    5,// GX_TEXFMT_PLTT16 
    6,// GX_TEXFMT_PLTT256
    0,// GX_TEXFMT_COMP4x4
    0,// GX_TEXFMT_A5I3  
    0 // GX_TEXFMT_DIRECT
};
// Table subtraction: GXTexSizeS from GXOamShape
static const GXTexSizeS gxTexSizeSTbl [3][4] = 
{
           {
           GX_TEXSIZE_S8,
           GX_TEXSIZE_S16,
           GX_TEXSIZE_S32,
           GX_TEXSIZE_S64
           },
           {
           GX_TEXSIZE_S16,
           GX_TEXSIZE_S32,
           GX_TEXSIZE_S32,
           GX_TEXSIZE_S64
           },
           {
           GX_TEXSIZE_S8,
           GX_TEXSIZE_S8,
           GX_TEXSIZE_S16,
           GX_TEXSIZE_S32
           }
};    

// Table subtraction: GXTexSizeT from GXOamShape
static const GXTexSizeT gxTexSizeTTbl [3][4] = 
{
           {
           GX_TEXSIZE_T8,
           GX_TEXSIZE_T16,
           GX_TEXSIZE_T32,
           GX_TEXSIZE_T64
           },
           {
           GX_TEXSIZE_T8,
           GX_TEXSIZE_T8,
           GX_TEXSIZE_T16,
           GX_TEXSIZE_T32
           },
           {
           GX_TEXSIZE_T16,
           GX_TEXSIZE_T32,
           GX_TEXSIZE_T32,
           GX_TEXSIZE_T64        
           }
};

static BOOL         bAutoZOffsetAdd_    = FALSE;    // Whether or not to automatically calculate Z offset when sprites drawn.
static fx32         zOffset_            = 0;        // Z offset imposed on sprite.
static fx32         zOffsetStep_        = -FX32_ONE; // The amount of increase of Z offset imposed on sprite






//------------------------------------------------------------------------------
// Get texture size as U32 value from GXTexSizeS
NNS_G2D_INLINE u32 GetNumTexChar_( GXTexSizeS texSize )
{
    static const u32 texSize_ [] = 
    {
        1,
        2,
        4,
        8,
        16,
        32,
        64,
        128   
    };
    
    GX_TEXSIZE_S_ASSERT( texSize );
    
    return texSize_[ texSize ];
}

//------------------------------------------------------------------------------
// Update the offset value for the auto-Z offset
NNS_G2D_INLINE void IncreaseAutoZOffset_()
{
    if( bAutoZOffsetAdd_ )
    {
        zOffset_ += zOffsetStep_;
    }
}

//------------------------------------------------------------------------------
// Take into consideration the fx32-type Z value that the auto-Z value gets
NNS_G2D_INLINE fx32 GetFx32DepthValue_( int z )
{
    if( bAutoZOffsetAdd_ )
    {
        return ( z << FX32_SHIFT ) + zOffset_;
    }else{
        return ( z << FX32_SHIFT );
    }
}

//------------------------------------------------------------------------------
// Perform the UV value calculations inside NNS_G2dDrawOneOam3DDirect()
NNS_G2D_INLINE void DoFlip_
(
    BOOL bFlipH, BOOL bFlipV,
    fx32*   pRetU0,  fx32* pRetU1,
    fx32*   pRetV0,  fx32* pRetV1
)
{
    NNS_G2D_NULL_ASSERT( pRetU0 );
    NNS_G2D_NULL_ASSERT( pRetU1 );
    NNS_G2D_NULL_ASSERT( pRetV0 );
    NNS_G2D_NULL_ASSERT( pRetV1 );
    
    // UV flip process
    {
        fx32 temp;
        if( bFlipH )
        {
            temp = *pRetU0;
            *pRetU0 = *pRetU1;
            *pRetU1 = temp;
        }
        
        if( bFlipV )
        {
            temp = *pRetV0;
            *pRetV0 = *pRetV1;
            *pRetV1 = temp;
        }
       
        // Call if the UV value correction function has been set
        if( s_pUVFlipCorrectFunc )
        {
            (*s_pUVFlipCorrectFunc)( pRetU0, pRetV0, pRetU1, pRetV1, bFlipH, bFlipV );
        }
    }
}

//------------------------------------------------------------------------------
// Perform the UV value calculations inside NNS_G2dDrawOneOam3DDirect()
NNS_G2D_INLINE void CalcUVFor3DDirect2DMap_
( 
    const NNSG2dImageAttr*          pTexImageAttr, 
    u16                             charName,
    fx32*   pRetU0,  
    fx32*   pRetV0
)
{
    NNS_G2D_NULL_ASSERT( pRetU0 );
    NNS_G2D_NULL_ASSERT( pRetV0 );
    
    GX_OBJVRAMMODE_CHAR_ASSERT( pTexImageAttr->mappingType );
    
    //
    // Warning: 2D graphics engine specification. 
    // The actual number of 8*8 texel characters is charName/2.
    //
    if( pTexImageAttr->fmt == GX_TEXFMT_PLTT256 )
    {
        // charName /= 2;
        charName >>= 1;
    }
    
    {
        // In the case of GX_OBJVRAMMODE_CHAR_2D
        {
            const u32 numCharPerOneLine = GetNumTexChar_( pTexImageAttr->sizeS );
            
            //U = ((charName) % numCharPerOneLine ) * 8;
        	*pRetU0 = (fx32)( (( charName & ( numCharPerOneLine - 1 ) ) << 3) << FX32_SHIFT ); 
            
            //
            // Note: This is for acceleration. (Depends on value of GXTexSizeS)
            //
            //V = ((charName) / numCharPerOneLine ) * 8;
            *pRetV0 = (( charName >> pTexImageAttr->sizeS ) << 3) << FX32_SHIFT;
        }
    }
}

//------------------------------------------------------------------------------
// Perform the UV value calculations inside NNS_G2dDrawOneOam3DDirect()
NNS_G2D_INLINE void CalcUVFor3DDirect1DMap_
( 
    fx32*   pRetU0,  
    fx32*   pRetV0
)
{
    NNS_G2D_NULL_ASSERT( pRetU0 );
    NNS_G2D_NULL_ASSERT( pRetV0 );
    
    
    {
        // GX_OBJVRAMMODE_CHAR_1D_32K:
        // GX_OBJVRAMMODE_CHAR_1D_64K:
        // GX_OBJVRAMMODE_CHAR_1D_128K:
        // GX_OBJVRAMMODE_CHAR_1D_256K:
        //
        // corresponds to baseOffset
        // Table subtraction:
        //
        *pRetU0 = 0; 
        *pRetV0 = 0;
    }
}


                                          


//------------------------------------------------------------------------------
NNS_G2D_INLINE GXTexSizeS GetTexS_( GXOamShape shape )
{
    GX_OAM_SHAPE_ASSERT( shape );
    {

    
    const u16 shapeBit = (u16)(( shape & GX_OAM_ATTR01_SHAPE_MASK ) >> GX_OAM_ATTR01_SHAPE_SHIFT);
    const u16 sizeBit = (u16)(( shape & GX_OAM_ATTR01_SIZE_MASK ) >> GX_OAM_ATTR01_SIZE_SHIFT);
                              
    return gxTexSizeSTbl[shapeBit][sizeBit];
    }
}

//------------------------------------------------------------------------------
// Table subtraction: GXTexSizeT from GXOamShape
NNS_G2D_INLINE GXTexSizeT GetTexT_( GXOamShape shape )
{
    GX_OAM_SHAPE_ASSERT( shape );
    {

    const u16 shapeBit = (u16)(( shape & GX_OAM_ATTR01_SHAPE_MASK ) >> GX_OAM_ATTR01_SHAPE_SHIFT);
    const u16 sizeBit = (u16)(( shape & GX_OAM_ATTR01_SIZE_MASK ) >> GX_OAM_ATTR01_SIZE_SHIFT);
                              
    return gxTexSizeTTbl[shapeBit][sizeBit];
    }
}    



//------------------------------------------------------------------------------
// Calculate the byte size of the offset for part that represents the palette number
NNS_G2D_INLINE u32 GetOffsetByteSizeOfPlt_( GXTexFmt pltFmt, BOOL bExtendedPlt, u16 pltNo )
{
    NNS_G2D_ASSERT( pltFmt == GX_TEXFMT_PLTT16 || pltFmt == GX_TEXFMT_PLTT256 );
    
    if( bExtendedPlt )
    {
        // Expanded palette 256 * 16
        NNS_G2D_ASSERT( pltFmt == GX_TEXFMT_PLTT256 );
        
        //return pltNo * (sizeof( u16 ) * 256 );
    	return (u32)( pltNo << NNSI_G2D_SHIFT_SIZEOF_256PLTT );
    }else{
        
        if( pltFmt == GX_TEXFMT_PLTT256 )
        {
            // For GX_TEXFMT_PLTT256 the palette number must be ignored.
            //return pltNo * 0;
            return 0;
        }else{
            // return pltNo * (sizeof( u16 ) * 16 );
        	return (u32)( pltNo << NNSI_G2D_SHIFT_SIZEOF_16PLTT );
        }
    }
    
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE int GetCharacterNameShiftBit_( GXOBJVRamModeChar objMappingMode )
{
    
    /*
    switch( objMappingMode)
    {
    case GX_OBJVRAMMODE_CHAR_1D_64K:
        return 1;
    case GX_OBJVRAMMODE_CHAR_1D_128K:
        return 2;
    case GX_OBJVRAMMODE_CHAR_1D_256K:
        return 3;
    
    default:
    case GX_OBJVRAMMODE_CHAR_2D:
    case GX_OBJVRAMMODE_CHAR_1D_32K:
        return 0;
    }
    */
    //
    // Note: This process depends on the enum definitions
    //
    /*
    GX_OBJVRAMMODE_CHAR_2D      = (0 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (0 << REG_GX_DISPCNT_EXOBJ_SHIFT),
    GX_OBJVRAMMODE_CHAR_1D_32K  = (1 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (0 << REG_GX_DISPCNT_EXOBJ_SHIFT),
    GX_OBJVRAMMODE_CHAR_1D_64K  = (1 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (1 << REG_GX_DISPCNT_EXOBJ_SHIFT),
    GX_OBJVRAMMODE_CHAR_1D_128K = (1 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (2 << REG_GX_DISPCNT_EXOBJ_SHIFT),
    GX_OBJVRAMMODE_CHAR_1D_256K = (1 << REG_GX_DISPCNT_OBJMAP_SHIFT) | (3 << REG_GX_DISPCNT_EXOBJ_SHIFT)
    */
    return (objMappingMode & REG_GX_DISPCNT_EXOBJ_MASK ) >> REG_GX_DISPCNT_EXOBJ_SHIFT;
}


//------------------------------------------------------------------------------
// Configure the Texture settings inside NNS_G2dDrawOneOam3DDirect()
NNS_G2D_INLINE void SetTextureParamsFor3DDirect1DMap_
(
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    GXOamShape                      shape,
    u16                             charName
)
{
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    // 
    // Set the texture size
    // 
    {
        const u16 shapeBit   = (u16)(( shape & GX_OAM_ATTR01_SHAPE_MASK ) >> GX_OAM_ATTR01_SHAPE_SHIFT);
        const u16 sizeBit    = (u16)(( shape & GX_OAM_ATTR01_SIZE_MASK ) >> GX_OAM_ATTR01_SIZE_SHIFT);
        //
        // Depending on the format, the original number of character blocks is obtained by a shift calculation.
        //
        // 5 indicates * 32 (8*8 texel byte size)
        // Originally, in the case of pTexImageAttr->fmt == GX_TEXFMT_PLTT256,
        //     you should shiftBit by an amount given by ( (charName / 2) * ( 32 * 2 ) )<<.
        //     However, altogether this amounts to a shiftBit of charName * 32<<.
        //
        const int shiftBit   = ( 5 + GetCharacterNameShiftBit_( pTexImageAttr->mappingType ) );
                          
        NNS_G2D_ASSERT( pTexImageAttr->mappingType != GX_OBJVRAMMODE_CHAR_2D );

        
        // If mode is 1D mapping, texture size = OBJ size
        //
        // If using G3_TexImageParam, compiler will fail at inline
        // expansion of SetTextureParamsFor3DDirect_, so....
        reg_G3_TEXIMAGE_PARAM 
           = GX_PACK_TEXIMAGE_PARAM( pTexImageAttr->fmt,   
                                     GX_TEXGEN_TEXCOORD, 
                                     gxTexSizeSTbl[shapeBit][sizeBit],         
                                     gxTexSizeTTbl[shapeBit][sizeBit],         
                                     GX_TEXREPEAT_NONE,  
                                     GX_TEXFLIP_NONE,    
                                     pTexImageAttr->plttUse, 
                                     texBaseAddr + ( charName << shiftBit ) ); 
        
    }
}

//------------------------------------------------------------------------------
// Configure the Texture settings inside NNS_G2dDrawOneOam3DDirect()
NNS_G2D_INLINE void SetTextureParamsFor3DDirect2DMap_
(
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr
)
{
    NNS_G2D_NULL_ASSERT( pTexImageAttr );        
    {        
        // If mode is 2D mapping, texture size is the character data's size
        reg_G3_TEXIMAGE_PARAM 
           = GX_PACK_TEXIMAGE_PARAM( pTexImageAttr->fmt,   
                                     GX_TEXGEN_TEXCOORD, 
                                     pTexImageAttr->sizeS,         
                                     pTexImageAttr->sizeT,         
                                     GX_TEXREPEAT_NONE,  
                                     GX_TEXFLIP_NONE,    
                                     pTexImageAttr->plttUse, 
                                     texBaseAddr );
    }
}


//------------------------------------------------------------------------------
// Configure the Texture settings inside NNS_G2dDrawOneOam3DDirect()
NNS_G2D_INLINE void SetPaletteParamsFor3DDirect_
(
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             pltBaseAddr,
    const GXOamAttr*                pOam
)
{
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    NNS_G2D_NULL_ASSERT( pOam );
    
    //
    // Configure palette settings
    //
    {
        // const GXTexFmt pltFmt = ( pOam->colorMode ) ? GX_TEXFMT_PLTT256 : GX_TEXFMT_PLTT16;
        const static GXTexFmt pltFmtTbl[] = 
        {
           GX_TEXFMT_PLTT16,
           GX_TEXFMT_PLTT256
        };
        const GXTexFmt pltFmt = pltFmtTbl[pOam->colorMode];
        
        const u32 pltOffs 
               = GetOffsetByteSizeOfPlt_( pltFmt, 
                                          pTexImageAttr->bExtendedPlt, 
                                          (u16)pOam->cParam );
                                        
        G3_TexPlttBase( pltBaseAddr + pltOffs, pltFmt );
    }    
}


//------------------------------------------------------------------------------
// Sets the sprite's parallel translation value
// In the function, determines if it is a double-size affine OBJ and modifies the display position depending on the result. 
// 
NNS_G2D_INLINE void SetQuadTranslation_
( 
    const GXOamAttr* pOam, 
    const int posX, 
    const int posY, 
    const int posZ 
)
{
    #pragma inline_max_size(20000)
    // determines if OBJ is double-size affine OBJ
    if( G2_GetOBJEffect( pOam ) == GX_OAM_EFFECT_AFFINE_DOUBLE )
    {        
        const GXOamShape     oamShape = NNS_G2dGetOAMSize( pOam );
        const int           halfW = NNS_G2dGetOamSizeX( &oamShape ) >> 1; // - 1 means / 2
        const int           halfH = NNS_G2dGetOamSizeY( &oamShape ) >> 1; 
           
        G3_Translate
        ( 
           (posX + halfW )<< FX32_SHIFT, 
           (posY + halfH )<< FX32_SHIFT, 
           GetFx32DepthValue_( posZ ) 
        );    
    }else{
        G3_Translate
        ( 
           posX << FX32_SHIFT, 
           posY << FX32_SHIFT, 
           GetFx32DepthValue_( posZ ) 
        );    
    }
}

//------------------------------------------------------------------------------
// Calculate sprite parameters.
static void CalcSpriteParams_
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr,
    SpriteParams                    *pResult
)
{
    
    const GXOamShape  shapeOam = NNS_G2dGetOAMSize( pOam );
    const u16 charName = (u16)pOam->charNo;
    
    pResult->sx       = NNS_G2dGetOamSizeX( &shapeOam );
    pResult->sy       = NNS_G2dGetOamSizeY( &shapeOam );
   
    if( pTexImageAttr->mappingType == GX_OBJVRAMMODE_CHAR_2D )
    {
        // Texture
        SetTextureParamsFor3DDirect2DMap_( pTexImageAttr, texBaseAddr );
        // Calculate UV 0 values
        CalcUVFor3DDirect2DMap_( pTexImageAttr, charName, &pResult->u0, &pResult->v0 );          
    }else{
        // Texture
        SetTextureParamsFor3DDirect1DMap_( pTexImageAttr, texBaseAddr, shapeOam, charName );
        // Calculate UV 0 values
        CalcUVFor3DDirect1DMap_( &pResult->u0, &pResult->v0 );
    }
       
    // Calculate UV 1 values
    pResult->u1 = pResult->u0 + ( pResult->sx << FX32_SHIFT );
    pResult->v1 = pResult->v0 + ( pResult->sy << FX32_SHIFT );
       
    DoFlip_( NNSi_G2dGetOamFlipH( pOam ), 
             NNSi_G2dGetOamFlipV( pOam ), 
             &pResult->u0, &pResult->u1 , 
             &pResult->v0, &pResult->v1 );
   
    // Palette
    SetPaletteParamsFor3DDirect_( pTexImageAttr, pltBaseAddr, pOam );
}

//------------------------------------------------------------------------------
// Set the flag that enables the auto-Z offset feature.
void NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( BOOL flag )
{
    bAutoZOffsetAdd_ = flag;
}

//------------------------------------------------------------------------------
fx32 NNSi_G2dGetOamSoftEmuAutoZOffset( void )
{
    return zOffset_;
}

//------------------------------------------------------------------------------
void NNSi_G2dResetOamSoftEmuAutoZOffset( void )
{
    zOffset_ = 0;
}

//------------------------------------------------------------------------------
void NNSi_G2dSetOamSoftEmuAutoZOffsetStep( fx32 step )
{
    NNS_G2D_WARNING( step <=  0, "AutoZOffsetStep should be smaller than zero." );
    zOffsetStep_ = step;
}

//------------------------------------------------------------------------------
fx32 NNSi_G2dGetOamSoftEmuAutoZOffsetStep( void )
{
    return zOffsetStep_;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawOneOam3DDirectFast

  Description:  Directly draws the Oam attribute contents using the 3D Graphics Engine
                
  Arguments:    pOam :     [IN]  OAM (GXOamAttr)
                pTexImageAttr:     [IN]  Texture attributes
                texBaseAddr:     [IN]  Texture base address
                pltBaseAddr:     [IN]  Palette base address

  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dDrawOneOam3DDirectFast
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    {
        SpriteParams             spriteParams;
        const s16 posX = NNSi_G2dRepeatXinScreenArea( NNSi_G2dGetOamX( pOam ) );
        const s16 posY = NNSi_G2dRepeatYinScreenArea( NNSi_G2dGetOamY( pOam ) );
        
        CalcSpriteParams_( pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams );
        
        NNSi_G2dDrawSpriteFast   ( posX, posY, GetFx32DepthValue_( -1 ), 
                               spriteParams.sx, spriteParams.sy,       
                               spriteParams.u0, spriteParams.v0, 
                               spriteParams.u1, spriteParams.v1 );
        //
        // Update the auto-Z offset value
        //
        IncreaseAutoZOffset_();   
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawOneOam3DDirectWithPosFast

  Description:  Specifies the position and directly draws the Oam attribute contents using the 3D Graphics Engine
                
  Arguments:    
                posX          :        [IN]  Position X
                posY          :        [IN]  Position Y
                posZ          :        [IN]  Position Z
                pOam :        [IN]  OAM (GXOamAttr)
                pTexImageAttr:        [IN]  Texture attributes
                texBaseAddr:        [IN]  VRAM  base address 
                pltBaseAddr:        [IN]  Palette base address 
                
                       
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dDrawOneOam3DDirectWithPosFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    {
        SpriteParams             spriteParams;
        CalcSpriteParams_( pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams );
        //
        // Drawing
        //
        //
        // To optimize Renderer drawing, expand the processes of the software sprite module 
        //
        // T
        SetQuadTranslation_( pOam, posX, posY, posZ );
        // S
        G3_Scale( spriteParams.sx << FX32_SHIFT, spriteParams.sy << FX32_SHIFT, FX32_ONE );        
        {
           const fx32      size = FX32_ONE;
           //
           // Draw a quad polygon.
           //
           G3_Begin( GX_BEGIN_QUADS );
                
               G3_TexCoord( spriteParams.u0, spriteParams.v1 );
               G3_Vtx10( 0 ,size, 0 ); 
                 
               G3_TexCoord( spriteParams.u1, spriteParams.v1 );
               G3_Vtx10( size, size, 0 ); 
                
               G3_TexCoord( spriteParams.u1, spriteParams.v0 );
               G3_Vtx10( size, 0 , 0 ); 
               
               G3_TexCoord( spriteParams.u0, spriteParams.v0 );
               G3_Vtx10( 0 , 0, 0 );
               
                
           G3_End( );
        }
        //
        // Update the auto-Z offset value
        //
        IncreaseAutoZOffset_();   
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawOneOam3DDirectWithPosAffine

  Description:  Specifies the position and directly draws the Oam attribute contents using the 3D Graphics Engine
                
  Arguments:    
                posX          :        [IN]  Position X
                posY          :        [IN]  Position Y
                posZ          :        [IN]  Position Z
                pOam :        [IN]  OAM (GXOamAttr)
                pTexImageAttr:        [IN]  Texture attributes
                texBaseAddr:        [IN]  VRAM  base address 
                pltBaseAddr:        [IN]  Palette base address 
                pMtx    :        [IN]  Affine transform matrix
                       
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dDrawOneOam3DDirectWithPosAffineFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr,
    const MtxFx22*                  pMtx
)
{
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    {
        SpriteParams             spriteParams;
        CalcSpriteParams_( pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams );
        
        //
        // Drawing
        //
        if( G2_GetOBJEffect( pOam ) == GX_OAM_EFFECT_AFFINE )
        {
            NNSi_G2dDrawSpriteWithMtxFast               
                ( posX, posY, GetFx32DepthValue_( posZ ), 
                  spriteParams.sx, spriteParams.sy, pMtx, 
                  spriteParams.u0, spriteParams.v0, 
                  spriteParams.u1, spriteParams.v1 );
        }else{
            NNSi_G2dDrawSpriteWithMtxDoubleAffineFast   
                ( posX, posY, GetFx32DepthValue_( posZ ), 
                spriteParams.sx, spriteParams.sy, pMtx, 
                spriteParams.u0, spriteParams.v0, 
                spriteParams.u1, spriteParams.v1 );
        }
        
        //
        // Update the auto-Z offset value
        //
        IncreaseAutoZOffset_();   
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetOamSoftEmuSpriteParamCache

  Description:  Sets the parameter cache used in drawing software sprites. 
                
                
                Drawing using the cache 
                is performed with NNS_G2dDrawOneOam3DDirectUsingParamCacheFast(). 
                
                The parameters here are the UV parameters, so when drawing
                a large number of sprites that reference the same texture, you can perform the process more efficiently by calculating and setting the UV values all at once.
                
                
                
                Drawing with NNS_G2dDrawOneOam3DDirectUsingParamCache():
                  
                  Process = Set UV parameters x 1 + draw x N
                  
                Normal drawing:
                  Process = (Set UV parameters + draw) x N
                  
                
                
  Arguments:    
                pOam :        [IN]  OBJ attributes 
                pTexImageAttr:        [IN]  Texture attributes
                texBaseAddr:        [IN]  VRAM  base address 
                pltBaseAddr:        [IN]  Palette base address 
                       
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dSetOamSoftEmuSpriteParamCache
( 
    const GXOamAttr*                pOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    {
        SpriteParams             spriteParams;
        CalcSpriteParams_( pOam, pTexImageAttr, texBaseAddr, pltBaseAddr, &spriteParams );
    
        //
        // Store in cache
        //
        softwareSpreiteParamCache_.u0 = spriteParams.u0;
        softwareSpreiteParamCache_.v0 = spriteParams.v0;
        softwareSpreiteParamCache_.u1 = spriteParams.u1;
        softwareSpreiteParamCache_.v1 = spriteParams.v1;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dDrawOneOam3DDirectUsingParamCacheFast

  Description:  Uses parameter cache memory for OAM to draw sprites.
                Set the cache parameters before executing this function.
                Set them using NNS_G2dSetOamSoftEmuSpriteParamCache().
                The current matrix of the 3D Graphics Engine is not stored
                
                at the time when the function is called.
                 
                
                
                The parameters here are the UV parameters, so when drawing
                a large number of sprites that reference the same texture, you can perform the process more efficiently by calculating and setting the UV values all at once.
                Changing the flip flag is not supported when clone drawing
                
                
                because this has such a large impact on performance.
                Accordingly,
                this function ignores the OAM flip flag.
                (The UV parameters are
                determined when NNS_G2dSetOamSoftEmuSpriteParamCache() executes.)
                When drawing a flipped sprite, 
                as an argument of NNS_G2dSetOamSoftEmuSpriteParamCache(), you must pass 
                the OAM (GXOamAttr) set to flip and update the cache parameters. 
                
                
                Drawing with NNS_G2dDrawOneOam3DDirectUsingParamCache():
                  
                  Process = Set UV parameters x 1 + draw x N
                  
                Normal drawing:
                  Process = (Set UV parameters + draw) x N
                
  Arguments:    
                posX          :        [IN]  Position X
                posY          :        [IN]  Position Y
                posZ          :        [IN]  Position Z
                pOam :        [IN]  OAM(GXOamAttr)
                       
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dDrawOneOam3DDirectUsingParamCacheFast
( 
    s16                             posX,
    s16                             posY,
    s16                             posZ,
    const GXOamAttr*                pOam
)
{
    {
        const GXOamShape  shapeOam = NNS_G2dGetOAMSize( pOam );
        const int sx       = NNS_G2dGetOamSizeX( &shapeOam );
        const int sy       = NNS_G2dGetOamSizeY( &shapeOam );        
        
        //
        // Use the parameter cache values.
        //
        const fx32   u0 = softwareSpreiteParamCache_.u0, 
                     u1 = softwareSpreiteParamCache_.u1, 
                     v0 = softwareSpreiteParamCache_.v0, 
                     v1 = softwareSpreiteParamCache_.v1;
        //
        // Drawing
        //
        //
        // To optimize drawing, expand the processes of the software sprite module. 
        
        //
        // T
        //G3_Translate( posX << FX32_SHIFT, posY << FX32_SHIFT, GetFx32DepthValue_( posZ ) );    
        SetQuadTranslation_( pOam, posX, posY, posZ );
        
        // S
        G3_Scale( sx << FX32_SHIFT, sy << FX32_SHIFT, FX32_ONE );        
        {
           const fx32      size = FX32_ONE;
           //
           // Draw a quad polygon.
           //
           G3_Begin( GX_BEGIN_QUADS );
               
               G3_TexCoord( u0, v1 );
               G3_Vtx10( 0 ,size, 0 ); 
               
               
               G3_TexCoord( u1, v1 );
               G3_Vtx10( size, size, 0 ); 
               
               G3_TexCoord( u1, v0 );
               G3_Vtx10( size, 0 , 0 ); 
               
               G3_TexCoord( u0, v0 );
               G3_Vtx10( 0 , 0, 0 );
               
                    
           G3_End( );
        }
        //
        // Update the auto-Z offset value
        //
        IncreaseAutoZOffset_();
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetOamSoftEmuUVFlipCorrectFunc

  Description:  Sets the pointer to the UV value correction function
                used in Oam software sprite emulation
                
  Arguments:    pFunc:        [IN] UV value correction function pointer
                       
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dSetOamSoftEmuUVFlipCorrectFunc( NNS_G2dOamSoftEmuUVFlipCorrectFunc pFunc )
{
    NNS_G2D_NULL_ASSERT( pFunc );
    s_pUVFlipCorrectFunc = pFunc;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetOamSoftEmuUVFlipCorrectFunc

  Description:  Sets the pointer to the UV value correction function
                used in Oam software sprite emulation
                
  Arguments:    None
                       
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void    NNS_G2dResetOamSoftEmuUVFlipCorrectFunc()
{
    s_pUVFlipCorrectFunc = NULL;
}



