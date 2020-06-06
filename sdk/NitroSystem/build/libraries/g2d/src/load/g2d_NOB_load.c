/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_NOB_load.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NOB_load.c,v $
  Revision 1.15  07/07/2005 06:40:43 AM  kitani_toshikazu
  Added processing for user-extended attributes

  Revision 1.14  02/22/2005 12:48:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.13  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.12  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.11  10/08/2004 04:39:14 AM  kitani_toshikazu
  Changed function names.

  Revision 1.10  10/07/2004 12:48:45 PM  kitani_toshikazu
  Supported the output of rectangle boundary information

  Revision 1.9  09/02/2004 12:13:09 AM  kitani_toshikazu
  Changed ASSERT message

  Revision 1.8  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.7  07/20/2004 05:57:50 AM  kitani_toshikazu
  fix comments.

  Revision 1.6  07/14/2004 12:25:38 PM  kitani_toshikazu
  Support for Vram transfer animation.

  Revision 1.5  07/05/2004 09:16:44 AM  kitani_toshikazu
  Commented out VRAM transfer related code.

  Revision 1.4  07/02/2004 12:03:20 AM  kitani_toshikazu
  Added process for VRAM transfer data member.

  Revision 1.3  06/22/2004 06:28:09 AM  kitani_toshikazu
  Change of debug output functions to dummy functions during FINAL ROM build

  Revision 1.2  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.1  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/load/g2d_NCE_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


//------------------------------------------------------------------------------
static void* GetPtrOamArrayHead_( NNSG2dCellDataBank* pCellBank )
{
    NNS_G2D_NULL_ASSERT( pCellBank );
    
    if( NNSi_G2dCellBankHasBR( pCellBank ) ) 
    {
        return (NNSG2dCellDataWithBR*)(pCellBank->pCellDataArrayHead) + pCellBank->numCells;
    }else{
        return pCellBank->pCellDataArrayHead + pCellBank->numCells;
    }   
}



//------------------------------------------------------------------------------
// Uncompress user-extended area info. 
// Currently, there is only one, but in some cases, more than one block is stored in user-extended memory. 
static void UnPackExtendedData_( void* pExData )
{
    NNS_G2D_NULL_ASSERT( pExData );
    {    
        NNSG2dUserExDataBlock*    pBlk          = (NNSG2dUserExDataBlock*)pExData;
        NNSG2dUserExCellAttrBank* pCellAttrBank = (NNSG2dUserExCellAttrBank*)(pBlk + 1);
    
        NNSi_G2dUnpackUserExCellAttrBank( pCellAttrBank );        
    }
}


//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedCellBank( void* pNcerFile, NNSG2dCellDataBank** ppCellBank )
{
    NNS_G2D_NULL_ASSERT( pNcerFile );
    NNS_G2D_NULL_ASSERT( ppCellBank );
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNcerFile, 
                                                        NNS_G2D_BINFILE_SIG_CELL ),
                       "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNcerFile, 
                                                      BIN_FILE_VERSION( NCER ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
                                                     
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNcerFile;
        {
            NNSG2dCellDataBankBlock* pBinBlk   =
                (NNSG2dCellDataBankBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                   NNS_G2D_BLKSIG_CELLBANK );
            if( pBinBlk )
            {
                NNS_G2dUnpackNCE( (void*)&pBinBlk->cellDataBank );
                //
                // OK
                //
                *ppCellBank = &pBinBlk->cellDataBank;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppCellBank = NULL;
                return FALSE;
                
            }
        }
    }
}

//------------------------------------------------------------------------------
const NNSG2dCellData* NNS_G2dGetCellDataByIdx( const NNSG2dCellDataBank* pCellData, u16 idx )
{
    NNS_G2D_NULL_ASSERT( pCellData );
    
    // 
    // Returns NULL for invalid index
    //
    if( idx >= pCellData->numCells )
    {
        return NULL;
    }
    
    //
    // It is necessary to change the access method depending on the type.
    //
    if( NNSi_G2dCellBankHasBR( pCellData ) )
    {
        const NNSG2dCellDataWithBR* pCellBR = 
            (const NNSG2dCellDataWithBR*)(pCellData->pCellDataArrayHead) + idx;
        return &pCellBR->cellData;
    }else{
        return pCellData->pCellDataArrayHead + idx;
    }
}




//------------------------------------------------------------------------------
void NNS_G2dUnpackNCE( NNSG2dCellDataBank* pCellData )
{
    NNS_G2D_NULL_ASSERT( pCellData );
    {
        //
        // Unpack offset pointer to the cell data.
        //
        pCellData->pCellDataArrayHead 
            = NNS_G2D_UNPACK_OFFSET_PTR( pCellData->pCellDataArrayHead, pCellData );
        
        //
        // Unpack offset pointer to the OBJ data.
        //
        {
            void* pHeadOfOAMData = GetPtrOamArrayHead_( pCellData );
            
            u16 i;
            NNSG2dCellData* pCell = NULL;
            for( i = 0; i < pCellData->numCells; i++ )
            {
                pCell = (NNSG2dCellData*)(NNS_G2dGetCellDataByIdx( pCellData, i ));
                pCell->pOamAttrArray 
                    = NNS_G2D_UNPACK_OFFSET_PTR( pCell->pOamAttrArray, pHeadOfOAMData );
            }
        }
        
        //
        // Unpack offset pointer to the Vram transform data.
        //
        if( pCellData->pVramTransferData != NULL )
        {
            NNSG2dVramTransferData* pVramTsfmData 
                = NNS_G2D_UNPACK_OFFSET_PTR( pCellData->pVramTransferData, pCellData );
            
            pVramTsfmData->pCellTransferDataArray 
                = NNS_G2D_UNPACK_OFFSET_PTR( pVramTsfmData->pCellTransferDataArray, pVramTsfmData );
                            
            pCellData->pVramTransferData = pVramTsfmData;
        }
        
        // 
        // Uncompress the user-extended attributes. 
        //
        if( pCellData->pExtendedData != NULL )
        {
            pCellData->pExtendedData
               = NNS_G2D_UNPACK_OFFSET_PTR( pCellData->pExtendedData, pCellData );    
            
            UnPackExtendedData_( pCellData->pExtendedData );
        }
        
    }
    NNSI_G2D_DEBUGMSG0("Unpacking NCER file is successful.\n" );
}



#ifndef SDK_FINALROM 

//------------------------------------------------------------------------------
void NNS_G2dPrintOBJAttr( const NNSG2dCellOAMAttrData* pOBJ )
{
    NNS_G2D_NULL_ASSERT( pOBJ );
    OS_Printf( "OBJ_0 = %x\n", pOBJ->attr0 );
    OS_Printf( "OBJ_1 = %x\n", pOBJ->attr1 );
    OS_Printf( "OBJ_2 = %x\n\n", pOBJ->attr2 );
}

//------------------------------------------------------------------------------
static void PrintCellBoundingRect_( const NNSG2dCellBoundingRectS16* pBR )
{
    NNS_G2D_NULL_ASSERT( pBR );
    OS_Printf( "maxX = %d\n", pBR->maxX );
    OS_Printf( "maxY = %d\n", pBR->maxY );
    OS_Printf( "minX = %d\n", pBR->minX );
    OS_Printf( "minY = %d\n", pBR->minY );
}

//------------------------------------------------------------------------------
void NNS_G2dPrintCellInfo( const NNSG2dCellData* pCell )
{
    u16         i;
    NNS_G2D_NULL_ASSERT( pCell );
    
    OS_Printf( "-------------------\n" );
    {
        const NNSG2dCellOAMAttrData*    pOBJ = pCell->pOamAttrArray;
        
        OS_Printf( "numOBJ = %d\n", pCell->numOAMAttrs );
        
        //
        // Displays if Cell has BoundingRect
        //
        if( NNSi_G2dCellHasBR( pCell ) )
        {
            const NNSG2dCellDataWithBR* pCellBR 
                = (const NNSG2dCellDataWithBR*)( pCell );
            PrintCellBoundingRect_( &pCellBR->boundingRect );
        }
        
        for( i = 0; i < pCell->numOAMAttrs; i++ )
        {
            NNS_G2dPrintOBJAttr( &pOBJ[i] );    
        }
    }
    OS_Printf( "-------------------\n" );
}

//------------------------------------------------------------------------------
static void PrintVramTransformData_
( 
    const NNSG2dVramTransferData*   pVramTsfmData, 
    u16                             numCells 
)
{
    u16 i;
    NNS_G2D_NULL_ASSERT( pVramTsfmData );
    
    for( i = 0; i < numCells; i++ )
    {
        const NNSG2dCellVramTransferData* pTsfmData 
            = &pVramTsfmData->pCellTransferDataArray[i];
        
        OS_Printf( "srcDataOffset   = %d\n", pTsfmData->srcDataOffset );
        OS_Printf( "szByte          = %d\n", pTsfmData->szByte );
    }
}

//------------------------------------------------------------------------------
// Display extended info. 
static void PrintCellExtendedData_( const void* pExData )
{
    NNS_G2D_NULL_ASSERT( pExData ); 
    {    
        NNSG2dUserExDataBlock*    pBlk          = (NNSG2dUserExDataBlock*)pExData;
        NNSG2dUserExCellAttrBank* pCellAttrBank = (NNSG2dUserExCellAttrBank*)(pBlk + 1);
        
        NNSi_G2dPrintUserExCellAttrBank( pCellAttrBank );
    }
}

//------------------------------------------------------------------------------
void NNS_G2dPrintCellBank( const NNSG2dCellDataBank*    pCellBank )
{
    u16         i;
    const NNSG2dCellData*   pCell;
    OS_Printf( "---------------------------------------------\n" );
    OS_Printf( "numCell = %d\n", pCellBank->numCells );
    for( i = 0; i < pCellBank->numCells; i++ )
    {
        OS_Printf( "Cell Idx = %d\n", i );
        pCell = NNS_G2dGetCellDataByIdx( pCellBank, i );
        NNS_G2dPrintCellInfo( pCell );
    }
    
    //
    // Vram transfer information
    //
    if( pCellBank->pVramTransferData != NULL )
    {
        OS_Printf( "--------- VramTransform Data\n" );
        {
            const NNSG2dVramTransferData* pVramTsfmData 
                = (const NNSG2dVramTransferData*)pCellBank->pVramTransferData;
            
            OS_Printf( "szByteMax   = %d\n", pVramTsfmData->szByteMax );
            PrintVramTransformData_( pVramTsfmData, pCellBank->numCells );
        }    
    }
    
    //
    // Extended info
    //
    if( pCellBank->pExtendedData != NULL )
    { 
        PrintCellExtendedData_( pCellBank->pExtendedData );
    }
    
    OS_Printf( "---------------------------------------------\n" );
}

#endif // SDK_FINALROM


