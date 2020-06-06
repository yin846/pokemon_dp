/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2 D 
  
  File:     g2d_NMC_load.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NMC_load.c,v $
  Revision 1.12  07/07/2005 06:40:43 AM  kitani_toshikazu
  Added processing for user-extended attributes

  Revision 1.11  02/22/2005 12:48:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.10  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.9  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.8  09/03/2004 10:03:20 AM  hatamoto_minoru
  Debug output string table no longer compiled in FinalRom build.

  Revision 1.7  09/02/2004 12:13:09 AM  kitani_toshikazu
  Changed ASSERT message

  Revision 1.6  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.5  07/20/2004 05:57:50 AM  kitani_toshikazu
  fix comments.

  Revision 1.4  07/14/2004 12:26:04 PM  kitani_toshikazu
  Supported for play method control data.

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
#include <nnsys/g2d/load/g2d_NMC_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


#ifndef SDK_FINALROM
// Character string of NNSG2dMCAnimationPlayMode
static const char* s_playModeStrTbl[] = 
{
    "NNS_G2D_MCANIM_PLAYMODE_RESET",
    "NNS_G2D_MCANIM_PLAYMODE_CONTINUE",
    "NNS_G2D_MCANIM_PLAYMODE_PAUSE",
    "NNS_G2D_MCANIM_PLAYMODE_MAX",
};
#endif // SDK_FINALROM

//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedMultiCellBank
( 
    void*                       pNmcrFile, 
    NNSG2dMultiCellDataBank**   ppMCBank 
)
{
    NNS_G2D_NULL_ASSERT( pNmcrFile );
    NNS_G2D_NULL_ASSERT( ppMCBank );
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNmcrFile, 
                                                        NNS_G2D_BINFILE_SIG_MULTICELL ),
                       "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNmcrFile, 
                                                      BIN_FILE_VERSION( NMCR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
                                                     
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNmcrFile;
        {
            NNSG2dMultiCellDataBankBlock* pBinBlk   =
                (NNSG2dMultiCellDataBankBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                       NNS_G2D_BLKSIG_MULTICELLBANK );
            if( pBinBlk )
            {
                NNS_G2dUnpackNMC( (void*)&pBinBlk->multiCellDataBank );
                //
                // OK
                //
                *ppMCBank = &pBinBlk->multiCellDataBank;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppMCBank = NULL;
                return FALSE;
                
            }
        }
    }
}

//------------------------------------------------------------------------------
// Restores the extended data.
// Extended data can include more than one block, but currently
// only one user-extended attribute block is stored.
static void UnpackExtendedData_( void* pExData )
{
    NNS_G2D_NULL_ASSERT( pExData );
    
    //------- Restore user-extended attribute block. 
    {    
        NNSG2dUserExDataBlock*    pBlk          = (NNSG2dUserExDataBlock*)pExData;
        NNSG2dUserExCellAttrBank* pCellAttrBank = (NNSG2dUserExCellAttrBank*)(pBlk + 1);
    
        NNSi_G2dUnpackUserExCellAttrBank( pCellAttrBank );
    }
}

//------------------------------------------------------------------------------
void NNS_G2dUnpackNMC( NNSG2dMultiCellDataBank* pMCellData )
{
    NNS_G2D_NULL_ASSERT( pMCellData );
    
    {
        //
        // Unpack offset pointer to the MultiCell data.
        //
        pMCellData->pMultiCellDataArray 
            = NNS_G2D_UNPACK_OFFSET_PTR( pMCellData->pMultiCellDataArray, pMCellData );
        
        pMCellData->pHierarchyDataArray 
            = NNS_G2D_UNPACK_OFFSET_PTR( pMCellData->pHierarchyDataArray, pMCellData );
        //
        // Unpack offset pointer to the NNSG2dMultiCellHierarchyData data.
        //
        {
            u16 i;
            NNSG2dMultiCellData* pCell = pMCellData->pMultiCellDataArray;
            for( i = 0; i < pMCellData->numMultiCellData; i++ )
            {
                pCell[i].pHierDataArray 
                    = NNS_G2D_UNPACK_OFFSET_PTR( pCell[i].pHierDataArray, 
                                                 pMCellData->pHierarchyDataArray );
            }
        }
        //
        // Restore the extended info pointer. (If there is one...) 
        //
        if( pMCellData->pExtendedData != NULL )
        {
            pMCellData->pExtendedData
               = NNS_G2D_UNPACK_OFFSET_PTR( pMCellData->pExtendedData, pMCellData );        
            UnpackExtendedData_( pMCellData->pExtendedData );
        }
    }
    
    NNSI_G2D_DEBUGMSG0("Unpacking NMCR file is successful.\n" );
}

//------------------------------------------------------------------------------
const NNSG2dMultiCellData* 
NNS_G2dGetMultiCellDataByIdx
( 
    const NNSG2dMultiCellDataBank* pDataBank, 
    u16 idx 
)
{
    NNS_G2D_NULL_ASSERT( pDataBank );
        
    if( idx < pDataBank->numMultiCellData )
    {
        return &pDataBank->pMultiCellDataArray[idx];
    }else{
        return NULL;
    }
}

#ifndef SDK_FINALROM 


//------------------------------------------------------------------------------
static const char* GetPlayModeStr_( playType )
{
    if( (NNS_G2D_MCANIM_PLAYMODE_RESET <= playType) &&
        (NNS_G2D_MCANIM_PLAYMODE_MAX >= playType) )
    {
        return s_playModeStrTbl[playType];
    }else{
        return "INVALID_DATA_TYPE";
    }
}

//------------------------------------------------------------------------------
static void PrintNodeAttr_( const NNSG2dMultiCellHierarchyData* pNode )
{
    NNS_G2D_NULL_ASSERT( pNode );
    
    OS_Printf( "nodeAttr--- = %x\n", pNode->nodeAttr );
    
    OS_Printf( "visibilityFlag = %d\n", 
                NNSi_G2dIsMultiCellNodeVisible( pNode ) );
    
    OS_Printf( "playMode = %s\n", 
                GetPlayModeStr_( NNSi_G2dGetMultiCellNodePlayMode( pNode) ) );
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
void NNS_G2dPrintNodeHierData( const NNSG2dMultiCellHierarchyData* pNode )
{
    NNS_G2D_NULL_ASSERT( pNode );
    
    OS_Printf( "animSequenceIdx = %d\n", pNode->animSequenceIdx);
    OS_Printf( "posX            = %d\n", pNode->posX);
    OS_Printf( "posY            = %d\n", pNode->posY);
    
    PrintNodeAttr_( pNode );
}

//------------------------------------------------------------------------------
void NNS_G2dPrintMultiCellInfo  ( const NNSG2dMultiCellData* pMCell )
{
    u16         i;
    NNS_G2D_NULL_ASSERT( pMCell );
    OS_Printf( "-------------------\n" );
    {
        const NNSG2dMultiCellHierarchyData*    pHierData = pMCell->pHierDataArray;
        
        OS_Printf( "numNodes = %d\n", pMCell->numNodes );
        
        for( i = 0; i < pMCell->numNodes; i++ )
        {
            OS_Printf( "---node Idx = %d\n", i );
            NNS_G2dPrintNodeHierData( &pHierData[i] );    
        }
    }
    OS_Printf( "-------------------\n" );
}

//------------------------------------------------------------------------------
void NNS_G2dPrintMultiCellBank  ( const NNSG2dMultiCellDataBank* pDataBank )
{
    u16         i;
    const NNSG2dMultiCellData*   pMCell;
    OS_Printf( "---------------------------------------------\n" );
    OS_Printf( "numMultiCell = %d\n", pDataBank->numMultiCellData );
    for( i = 0; i < pDataBank->numMultiCellData; i++ )
    {
        OS_Printf( "MultiCell Idx = %d\n", i );
        pMCell = NNS_G2dGetMultiCellDataByIdx( pDataBank, i );
        NNS_G2dPrintMultiCellInfo( pMCell );
    }
    //
    // Display extended info (if it exists)
    //
    if( pDataBank->pExtendedData != NULL )
    {
        PrintCellExtendedData_( pDataBank->pExtendedData );
    }
    
    OS_Printf( "---------------------------------------------\n" );
}

#endif // SDK_FINALROM

