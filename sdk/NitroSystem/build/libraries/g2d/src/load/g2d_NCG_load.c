/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_NCG_load.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NCG_load.c,v $
  Revision 1.23  08/17/2005 01:57:24 AM  kitani_toshikazu
  Added comments

  Revision 1.22  06/30/2005 04:21:35 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.21  06/13/2005 08:33:40 AM  kitani_toshikazu
  Added comments

  Revision 1.20  06/13/2005 08:22:23 AM  kitani_toshikazu
  Support for new versions of NCGR, NCBR-load related functions

  Revision 1.19  02/22/2005 12:48:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.18  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.17  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.16  09/07/2004 01:02:17 AM  hatamoto_minoru
  Added to ASSERT condition that it is in character format for BG

  Revision 1.15  09/03/2004 10:03:20 AM  hatamoto_minoru
  Debug output string table no longer compiled in FinalRom build.

  Revision 1.14  09/02/2004 07:08:10 AM  hatamoto_minoru
  Added BG related.

  Revision 1.13  09/02/2004 12:13:09 AM  kitani_toshikazu
  Changed ASSERT message

  Revision 1.12  08/26/2004 07:22:35 AM  kitani_toshikazu
  (none)

  Revision 1.11  08/18/2004 07:49:19 AM  kitani_toshikazu
  Removed the size limits on the height during 2D mapping from the assert conditions

  Revision 1.10  08/10/2004 07:50:21 AM  hatamoto_minoru
  Changed judgment condition of 2D mapping data size condition attribute.

  Revision 1.9  08/05/2004 11:09:20 AM  kitani_toshikazu
  Added 2D mapping data size condition judgment attribute.

  Revision 1.8  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.7  07/23/2004 01:47:31 AM  kitani_toshikazu
  Improved debug display when 1D mapping.

  Revision 1.6  07/23/2004 01:35:53 AM  kitani_toshikazu
  Improved debug display when 1D mapping.

  Revision 1.5  07/20/2004 05:57:50 AM  kitani_toshikazu
  fix comments.

  Revision 1.4  07/14/2004 12:26:42 PM  kitani_toshikazu
  Support for Vram transfer animation.

  Revision 1.3  06/22/2004 06:28:09 AM  kitani_toshikazu
  Change of debug output functions to dummy functions during FINAL ROM build

  Revision 1.2  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.1  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/load/g2d_NCG_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


#define NNS_G2D_NCGR_VER_FOR_CHARDATA_BLK_LOADING   NNS_G2dMakeVersionData( (u8)1, (u8)0 )

#ifndef SDK_FINALROM
static const char* StrNNSG2dPixelFmt_ [] =
{
    "INVALID_PixelFmt !! ", //  GX_TEXFMT_NONE
    "INVALID_PixelFmt !! ", //  GX_TEXFMT_A3I5
    "INVALID_PixelFmt !! ", //  GX_TEXFMT_PLTT4
    "GX_TEXFMT_PLTT16"    , //  GX_TEXFMT_PLTT16
    "GX_TEXFMT_PLTT256"   , //  GX_TEXFMT_PLTT256
    "INVALID_PixelFmt !! ", //  GX_TEXFMT_COMP4x4
    "INVALID_PixelFmt !! ", //  GX_TEXFMT_A5I3
    "INVALID_PixelFmt !! "  //  GX_TEXFMT_DIRECT
};

static const char* StrNNSG2dCharacterOrder_ [] =
{
    "NNS_G2D_CHARACTER_FMT_CHAR",
    "NNS_G2D_CHARACTER_FMT_BMP",
    "NNS_G2D_CHARACTER_FMT_MAX"
};

static const char*  GetStrNNSG2dCharacterDataMapingType_ ( GXOBJVRamModeChar type )
{
    switch( type )
    {
    case GX_OBJVRAMMODE_CHAR_2D:        return "GX_OBJVRAMMODE_CHAR_2D           ";
    case GX_OBJVRAMMODE_CHAR_1D_32K:    return "GX_OBJVRAMMODE_CHAR_1D_32K       ";
    case GX_OBJVRAMMODE_CHAR_1D_64K:    return "GX_OBJVRAMMODE_CHAR_1D_64K       ";
    case GX_OBJVRAMMODE_CHAR_1D_128K:   return "GX_OBJVRAMMODE_CHAR_1D_128K      ";
    case GX_OBJVRAMMODE_CHAR_1D_256K:   return "GX_OBJVRAMMODE_CHAR_1D_256K      ";
    default: NNS_G2D_ASSERT(FALSE); return "INVALID_TYPE";
    }
}
#endif // SDK_FINALROM



 
//------------------------------------------------------------------------------
// For OBJ (For the BG character data, please use NNS_G2dGetUnpackedBGCharacterData()). )
//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedCharacterData( void* pNcgrFile, NNSG2dCharacterData** ppCharData )
{
    NNS_G2D_NULL_ASSERT( pNcgrFile );
    NNS_G2D_NULL_ASSERT( ppCharData );
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNcgrFile, 
                                                        NNS_G2D_BINFILE_SIG_CHARACTERDATA ),
                     "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNcgrFile, 
                                                      NNS_G2D_NCGR_VER_FOR_CHARDATA_BLK_LOADING ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
                       
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNcgrFile;
        {
            NNSG2dCharacterDataBlock* pBinBlk   =
                (NNSG2dCharacterDataBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                   NNS_G2D_BINBLK_SIG_CHARACTERDATA );
            if( pBinBlk )
            {
                
                NNS_G2dUnpackNCG( (void*)&pBinBlk->characterData );
                //
                // OK
                //
                *ppCharData = &pBinBlk->characterData;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppCharData = NULL;
                return FALSE;
            }
        }
    }
}

//------------------------------------------------------------------------------
void NNS_G2dUnpackNCG( NNSG2dCharacterData* pCharData )
{
    NNS_G2D_NULL_ASSERT( pCharData );
    
    pCharData->pRawData
        = NNS_G2D_UNPACK_OFFSET_PTR( pCharData->pRawData, pCharData );
    
    //
    // Validity of parameter group
    //
    NNS_G2D_MINMAX_ASSERT( pCharData->pixelFmt
        , GX_TEXFMT_PLTT16, GX_TEXFMT_PLTT256 );
        
    NNS_G2D_MINMAX_ASSERT( pCharData->mapingType
        , GX_OBJVRAMMODE_CHAR_2D, GX_OBJVRAMMODE_CHAR_1D_256K );
        
    NNS_G2D_MINMAX_ASSERT( NNSi_G2dGetCharacterFmtType( pCharData->characterFmt )
        , NNS_G2D_CHARACTER_FMT_CHAR, NNS_G2D_CHARACTER_FMT_MAX );
    
    //
    // Checks whether it is the valid size when in 2D mapping mode
    //
    if( (pCharData->characterFmt == NNS_G2D_CHARACTER_FMT_CHAR)
    	&& (pCharData->mapingType == GX_OBJVRAMMODE_CHAR_2D) )
    {
        NNS_G2D_ASSERTMSG( ( pCharData->pixelFmt == GX_TEXFMT_PLTT16 && 
                             pCharData->W == 32 ) || 
                           ( pCharData->pixelFmt == GX_TEXFMT_PLTT256 && 
                             pCharData->W == 16 ) 
                             , "Invalid OBJ character data size for 2D mapping mode." );
    }
    
    NNSI_G2D_DEBUGMSG0("Unpacking NCGR file is successful.\n" );
}



//------------------------------------------------------------------------------
// For BG (For the OBJ character data, please use NNS_G2dGetUnpackedBGCharacterData()). )
//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedBGCharacterData( void* pNcgrFile, NNSG2dCharacterData** ppCharData )
{
    NNS_G2D_NULL_ASSERT( pNcgrFile );
    NNS_G2D_NULL_ASSERT( ppCharData );
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNcgrFile, 
                                                        NNS_G2D_BINFILE_SIG_CHARACTERDATA ),
                     "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNcgrFile, 
                                                      NNS_G2D_NCGR_VER_FOR_CHARDATA_BLK_LOADING ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
                       
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNcgrFile;
        {
            NNSG2dCharacterDataBlock* pBinBlk   =
                (NNSG2dCharacterDataBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                   NNS_G2D_BINBLK_SIG_CHARACTERDATA );
            if( pBinBlk )
            {
                
                NNS_G2dUnpackBGNCG( (void*)&pBinBlk->characterData );
                //
                // OK
                //
                *ppCharData = &pBinBlk->characterData;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppCharData = NULL;
                return FALSE;
            }
        }
    }
}

//------------------------------------------------------------------------------
// Obtains position information block
//------------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUnpackedCharacterPosInfo

  Description:  Gets the character data load position info block from the NCBR and NCGR files.
                
                When reading character data to VRAM, the character data load position info block indicates
                what position the data should be loaded to. 
                
                The character data load position info block can only be obtained with 
                NCGR and NCBR files stored with 2D mapping mode. 
                
  Arguments:    pNcgrFile         Pointer to NCG file 
                ppCharPosInfo     character data load position info block
                                    
  Returns:      If obtained successfully, TRUE
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetUnpackedCharacterPosInfo
( 
    void*                        pNcgrFile, 
    NNSG2dCharacterPosInfo**     ppCharPosInfo 
)
{
    NNS_G2D_NULL_ASSERT( pNcgrFile );
    NNS_G2D_NULL_ASSERT( ppCharPosInfo );
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNcgrFile, 
                                                        NNS_G2D_BINFILE_SIG_CHARACTERDATA ),
                     "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNcgrFile, 
                                                      BIN_FILE_VERSION( NCGR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
                       
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNcgrFile;
        {
            NNSG2dCharacterPosInfoBlock* pBinBlk   =
                (NNSG2dCharacterPosInfoBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                      NNS_G2D_BINBLK_SIG_CHAR_POSITION );
            if( pBinBlk )
            {
                *ppCharPosInfo = &pBinBlk->posInfo;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppCharPosInfo = NULL;
                return FALSE;
            }
        }
    }
}

//------------------------------------------------------------------------------
void NNS_G2dUnpackBGNCG( NNSG2dCharacterData* pCharData )
{
    NNS_G2D_NULL_ASSERT( pCharData );
    
    pCharData->pRawData
        = NNS_G2D_UNPACK_OFFSET_PTR( pCharData->pRawData, pCharData );
    
    //
    // Validity of parameter group
    //
    NNS_G2D_MINMAX_ASSERT( pCharData->pixelFmt
        , GX_TEXFMT_PLTT16, GX_TEXFMT_PLTT256 );
        
    NNS_G2D_MINMAX_ASSERT( pCharData->mapingType
        , GX_OBJVRAMMODE_CHAR_2D, GX_OBJVRAMMODE_CHAR_1D_256K );
        
	// Only the character format is OK. BMP format is not OK.
    NNS_G2D_ASSERT( NNSi_G2dGetCharacterFmtType( pCharData->characterFmt )
    	== NNS_G2D_CHARACTER_FMT_CHAR );

    NNSI_G2D_DEBUGMSG0("Unpacking NCGR file is successful.\n" );
}



//------------------------------------------------------------------------------
// Common
//------------------------------------------------------------------------------
#ifndef SDK_FINALROM 

//------------------------------------------------------------------------------
// Debug output data (character data)
void NNS_G2dPrintCharacterData( const NNSG2dCharacterData* pCharData )
{
    NNS_G2D_NULL_ASSERT( pCharData );
    {
        const NNSG2dCharacterFmt charFmt = NNSi_G2dGetCharacterFmtType( pCharData->characterFmt );
        
        OS_Printf( "---------------------------------------------\n" );
        OS_Printf( " Character Data (NCG) \n" );
        OS_Printf( " szByte         = %d \n", pCharData->szByte );
        
        //
        // When 1D mapping,  NNS_G2D_1D_MAPPING_CHAR_SIZE is set to H and W with a converter tool
        //
        {
            if( pCharData->W == NNS_G2D_1D_MAPPING_CHAR_SIZE )
            {
                OS_Printf( " W              = %s \n", "Not used( 1D mapping data.)" );
            }else{
                OS_Printf( " W              = %d \n", pCharData->W );
            }
            
            if( pCharData->H == NNS_G2D_1D_MAPPING_CHAR_SIZE )
            {
                OS_Printf( " H              = %s \n", "Not used( 1D mapping data.)" );
            }else{
                OS_Printf( " H              = %d \n", pCharData->H );
            }
        }
        
        OS_Printf( " pixelFmt       = %s \n", StrNNSG2dPixelFmt_[ pCharData->pixelFmt ] );
        OS_Printf( " mapingType     = %s \n", GetStrNNSG2dCharacterDataMapingType_( pCharData->mapingType ) );
        OS_Printf( " characterFmt   = %s \n", StrNNSG2dCharacterOrder_[ charFmt ] );
        OS_Printf( " isVramTransfer = %d \n", NNSi_G2dIsCharacterVramTransfered( pCharData->characterFmt ) );
        OS_Printf( "---------------------------------------------\n" );
    }
}

//------------------------------------------------------------------------------
// Debug output data (character position info)
void NNS_G2dPrintCharacterPosInfo( const NNSG2dCharacterPosInfo* pPosInfo )
{
    NNS_G2D_NULL_ASSERT( pPosInfo );
    {
        OS_Printf( "---------------------------------------------\n" );
        OS_Printf( " Character PosInfo (NCG) \n" );
        OS_Printf( " srcPosX    = %d \n", pPosInfo->srcPosX );
        OS_Printf( " srcPosY    = %d \n", pPosInfo->srcPosY );
        OS_Printf( " srcW       = %d \n", pPosInfo->srcW );
        OS_Printf( " srcH       = %d \n", pPosInfo->srcH );
        OS_Printf( "---------------------------------------------\n" );
    }
}

#endif // SDK_FINALROM


