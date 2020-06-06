/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_NEN_load.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NEN_load.c,v $
  Revision 1.10  02/22/2005 12:48:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.9  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.8  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.7  09/03/2004 10:03:20 AM  hatamoto_minoru
  Debug output string table no longer compiled in FinalRom build.

  Revision 1.6  09/02/2004 12:13:09 AM  kitani_toshikazu
  Changed ASSERT message

  Revision 1.5  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.4  07/20/2004 05:57:50 AM  kitani_toshikazu
  fix comments.

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
#include <nnsys/g2d/load/g2d_NEN_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


#ifndef SDK_FINALROM
static const char* s_entityTypeStrTbl[] = 
{
    "NNS_G2D_ENTITYTYPE_INVALID",
    "NNS_G2D_ENTITYTYPE_CELL",
    "NNS_G2D_ENTITYTYPE_MULTICELL",
    "NNS_G2D_ENTITYTYPE_MAX",
};
#endif // SDK_FINALROM

//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedEntityBank( void* pNenrFile, NNSG2dEntityDataBank** ppEntityBank )
{
    NNS_G2D_NULL_ASSERT( pNenrFile );
    NNS_G2D_NULL_ASSERT( ppEntityBank );
     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNenrFile, 
                                                        NNS_G2D_BINFILE_SIG_ENTITY ),
                       "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNenrFile, 
                                                      BIN_FILE_VERSION( NENR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );                                        
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNenrFile;
        {
            NNSG2dEntityDataBankBlock* pBinBlk   =
                (NNSG2dEntityDataBankBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                    NNS_G2D_BLKSIG_ENTITYBANK );
            if( pBinBlk )
            {
                NNS_G2dUnpackNEN( (void*)&pBinBlk->entityDataBank );
                //
                // OK
                //
                *ppEntityBank = &pBinBlk->entityDataBank;
                return TRUE;
            }else{
                //
                // NG
                //
                *ppEntityBank = NULL;
                return FALSE;
                
            }
        }
    }
}

//------------------------------------------------------------------------------
void NNS_G2dUnpackNEN( NNSG2dEntityDataBank* pDataBank )
{
    NNS_G2D_NULL_ASSERT( pDataBank );
    {
        //
        // Unpack offset pointer to the NNSG2dEntityData data.
        //
        pDataBank->pEntityDataArray 
            = NNS_G2D_UNPACK_OFFSET_PTR( pDataBank->pEntityDataArray, pDataBank );
            
        pDataBank->pAnimSequenceIdxArray 
            = NNS_G2D_UNPACK_OFFSET_PTR( pDataBank->pAnimSequenceIdxArray, pDataBank );
        
        
        {
            u16 i;
            NNSG2dEntityData* pEntity = pDataBank->pEntityDataArray;
            for( i = 0; i < pDataBank->numEntityDatas; i++ )
            {
                pEntity[i].animData.pAnimSequenceIdxArray = 
                    NNS_G2D_UNPACK_OFFSET_PTR( pEntity[i].animData.pAnimSequenceIdxArray, 
                                               pDataBank->pAnimSequenceIdxArray );
            }
        }
    }
    NNSI_G2D_DEBUGMSG0("Unpacking NENR file is successful.\n" );
}

//------------------------------------------------------------------------------
const NNSG2dEntityData* NNS_G2dGetEntityDataByIdx
( 
    const NNSG2dEntityDataBank* pDataBank, 
    u16 idx 
)
{
    NNS_G2D_NULL_ASSERT( pDataBank );
        
    if( idx < pDataBank->numEntityDatas )
    {
        return &pDataBank->pEntityDataArray[idx];
    }else{
        return NULL;
    }
}


#ifndef SDK_FINALROM 

//------------------------------------------------------------------------------
void NNS_G2dPrintEntityData        ( const NNSG2dEntityData* pEntity )
{
    u16 i;
    NNS_G2D_NULL_ASSERT( pEntity );
    
    OS_Printf( "type            = %s\n", s_entityTypeStrTbl[pEntity->type] );
    OS_Printf( "numAnimSequence = %d\n", pEntity->animData.numAnimSequence );
    for( i  = 0; i < pEntity->animData.numAnimSequence; i ++ )
    {
        OS_Printf( "anim %d ==> %d \n", i, pEntity->animData.pAnimSequenceIdxArray[i] );
    }
}

//------------------------------------------------------------------------------
void NNS_G2dPrintEntityDataBank    ( const NNSG2dEntityDataBank* pDataBank )
{
    u16         i;
    const NNSG2dEntityData*   pE;
    NNS_G2D_NULL_ASSERT( pDataBank );
    
    OS_Printf( "---------------------------------------------\n" );
    OS_Printf( "numEntityDatas = %d\n", pDataBank->numEntityDatas );
    for( i = 0; i < pDataBank->numEntityDatas; i++ )
    {
        OS_Printf( "Entity Idx = %d\n", i );
        pE = NNS_G2dGetEntityDataByIdx( pDataBank, i );
        NNS_G2dPrintEntityData( pE );
    }
    OS_Printf( "---------------------------------------------\n" );
}

#endif // SDK_FINALROM

