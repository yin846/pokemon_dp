/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_NCL_load.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NCL_load.c,v $
  Revision 1.15  02/22/2005 12:48:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.14  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.13  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.12  09/17/2004 06:44:43 AM  kitani_toshikazu
  (none)

  Revision 1.11  09/17/2004 06:34:03 AM  kitani_toshikazu
  (none)

  Revision 1.10  09/17/2004 01:48:59 AM  kitani_toshikazu
  Added the palette compression information load function

  Revision 1.9  09/03/2004 10:03:20 AM  hatamoto_minoru
  Debug output string table no longer compiled in FinalRom build.

  Revision 1.8  09/02/2004 12:12:57 AM  kitani_toshikazu
  Changed ASSART conditions

  Revision 1.7  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.6  07/23/2004 01:35:40 AM  kitani_toshikazu
  Fixed Palette Debug Display

  Revision 1.5  07/20/2004 05:57:50 AM  kitani_toshikazu
  fix comments.

  Revision 1.4  06/22/2004 06:28:09 AM  kitani_toshikazu
  Change of debug output functions to dummy functions during FINAL ROM build

  Revision 1.3  06/01/2004 05:20:57 AM  kitani_toshikazu
  Changes for using expansion palette.

  Revision 1.2  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.1  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/load/g2d_NCL_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


#ifndef SDK_FINALROM
static const char* StrNNSG2dPaletteFmt_ [] =
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
#endif // SDK_FINALROM

//------------------------------------------------------------------------------
static void DumpPalette_( const NNSG2dPaletteData* pPlttData )
{
    NNS_G2D_NULL_ASSERT( pPlttData );
    NNS_G2D_NULL_ASSERT( pPlttData->pRawData );
    {
        u16 i;
        const u16*    pColor = (const u16*)pPlttData->pRawData;
        OS_Printf( "\n ---------------- palette ---------------- \n" );
        for( i = 0; i < 16 * 16; i++ )
        {
            if( (i & 0xF ) == 0 )
            {
                OS_Printf("\n");
            }
            
            OS_Printf(" %04X ", pColor[i] );
        }
        OS_Printf( "\n ---------------- palette ---------------- \n" );
    }
}



//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedPaletteData( void* pNclrFile, NNSG2dPaletteData** ppPltData )
{
    NNS_G2D_NULL_ASSERT( pNclrFile );
    NNS_G2D_NULL_ASSERT( ppPltData );
    
    // Is file identifier correct?
    {
        const NNSG2dBinaryFileHeader*   pBinFile = pNclrFile;
        // To support previous identifiers.
        NNS_G2D_ASSERTMSG( pBinFile->signature == NNS_G2D_BINFILE_SIG_PALETTEDATA ||
                           pBinFile->signature == NNS_G2D_BINFILE_SIG_PALETTEDATA_OLD, 
                           "Input file signature is invalid for this method." );
    }
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNclrFile, 
                                                      BIN_FILE_VERSION( NCLR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
    
                                                    
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNclrFile;
        {
            NNSG2dPaletteDataBlock* pBinBlk   =
                (NNSG2dPaletteDataBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                   NNS_G2D_BINBLK_SIG_PALETTEDATA );
            if( pBinBlk )
            {
                NNS_G2dUnpackNCL( (void*)&pBinBlk->paletteData );
                //
                // OK
                //
                *ppPltData = &pBinBlk->paletteData;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppPltData = NULL;
                return FALSE;
                
            }
        }
    }
}

//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedPaletteCompressInfo( void* pNclrFile, NNSG2dPaletteCompressInfo** ppPltCmpInfo )
{
    NNS_G2D_NULL_ASSERT( pNclrFile );
    NNS_G2D_NULL_ASSERT( ppPltCmpInfo );
    
    // Is file identifier correct?
    {
        const NNSG2dBinaryFileHeader*   pBinFile = pNclrFile;
        // To support previous identifiers.
        NNS_G2D_ASSERTMSG( pBinFile->signature == NNS_G2D_BINFILE_SIG_PALETTEDATA ||
                           pBinFile->signature == NNS_G2D_BINFILE_SIG_PALETTEDATA_OLD, 
                           "Input file signature is invalid for this method." );
    }
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNclrFile, 
                                                      BIN_FILE_VERSION( NCLR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
    
                                                    
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNclrFile;
        {
            NNSG2dPaletteCompressDataBlock* pBinBlk   =
                (NNSG2dPaletteCompressDataBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                   NNS_G2D_BINBLK_SIG_PALETTECOMPINFO );
            if( pBinBlk )
            {
                NNSi_G2dUnpackNCLCmpInfo( (void*)&pBinBlk->plttCmpInfo );
                //
                // OK
                //
                *ppPltCmpInfo = &pBinBlk->plttCmpInfo;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppPltCmpInfo = NULL;
                return FALSE;   
            }
        }
    }
}

//------------------------------------------------------------------------------
void NNSi_G2dUnpackNCLCmpInfo( NNSG2dPaletteCompressInfo* pPlttCmpInfo )
{
    NNS_G2D_NULL_ASSERT( pPlttCmpInfo );
    
    pPlttCmpInfo->pPlttIdxTbl
        = NNS_G2D_UNPACK_OFFSET_PTR( pPlttCmpInfo->pPlttIdxTbl, pPlttCmpInfo );
        
    NNSI_G2D_DEBUGMSG0("Unpacking NCLR(Compressed) file is successful.\n" );
}

//------------------------------------------------------------------------------
void NNS_G2dUnpackNCL( NNSG2dPaletteData* pPlttData )
{
    NNS_G2D_NULL_ASSERT( pPlttData );
    
    pPlttData->pRawData
        = NNS_G2D_UNPACK_OFFSET_PTR( pPlttData->pRawData, pPlttData );
        
    
//    NNS_G2D_MINMAX_ASSERT( pPlttData->fmt,
//        NNS_G2D_NNSG2DPALETTEFMT_16X16, NNS_G2D_NNSG2DPALETTEFMT_MAX );
  
    NNS_G2D_MINMAX_ASSERT( pPlttData->fmt, GX_TEXFMT_PLTT16, GX_TEXFMT_PLTT256 );  
    
    NNSI_G2D_DEBUGMSG0("Unpacking NCLR file is successful.\n" );
}

#ifndef SDK_FINALROM 

//------------------------------------------------------------------------------
void NNS_G2dPrintPaletteData( const NNSG2dPaletteData* pPlttData )
{
    NNS_G2D_NULL_ASSERT( pPlttData );
    {
        OS_Printf( "---------------------------------------------\n" );
        OS_Printf( " Palette Data (NCL)" );
        OS_Printf( " fmt          = %s \n", StrNNSG2dPaletteFmt_[ pPlttData->fmt ] );
        OS_Printf( " bExtendedPlt = %d \n", pPlttData->bExtendedPlt );
        DumpPalette_( pPlttData );
        OS_Printf( "---------------------------------------------\n" );
    }
}

#endif // SDK_FINALROM


