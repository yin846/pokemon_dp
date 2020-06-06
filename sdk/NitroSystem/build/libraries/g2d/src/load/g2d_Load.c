/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_Load.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Load.c,v $
  Revision 1.5  07/07/2005 06:40:43 AM  kitani_toshikazu
  Added processing for user-extended attributes

  Revision 1.4  02/22/2005 12:48:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.3  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.2  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.1  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"

NNSG2dBinaryBlockHeader* NNS_G2dFindBinaryBlock
( 
    NNSG2dBinaryFileHeader* pBinFileHeader, 
    u32                     signature 
)
{         
    NNS_G2D_NULL_ASSERT( pBinFileHeader );
    
    {
        NNSG2dBinaryBlockHeader*    pCursor
             = (NNSG2dBinaryBlockHeader*)((u32)pBinFileHeader + (u32)pBinFileHeader->headerSize);
        
        u16 count = 0;
        while( count < pBinFileHeader->dataBlocks )
        {
            NNS_G2D_NULL_ASSERT( pCursor );
            
            NNSI_G2D_DEBUGMSG0("pCursor->kind = %d\n", pCursor->kind );
            NNSI_G2D_DEBUGMSG0("signature = %d\n", signature );
            
            if( pCursor->kind == signature )
            {
                return pCursor;
            }
            pCursor = (NNSG2dBinaryBlockHeader*)((u32)pCursor + pCursor->size);
            count++;
        }
    }
    // Discovery failed
    return NULL;
}

//------------------------------------------------------------------------------
// Uncompress NNSG2dUserExCellAttrBank 
void NNSi_G2dUnpackUserExCellAttrBank( NNSG2dUserExCellAttrBank* pCellAttrBank )
{
    u16 i;
    
    pCellAttrBank->pCellAttrArray 
        = NNS_G2D_UNPACK_OFFSET_PTR( pCellAttrBank->pCellAttrArray, 
                                     pCellAttrBank );
    for( i = 0; i < pCellAttrBank->numCells; i++ )
    {
        pCellAttrBank->pCellAttrArray[i].pAttr
           = NNS_G2D_UNPACK_OFFSET_PTR( pCellAttrBank->pCellAttrArray[i].pAttr, 
                                        pCellAttrBank );
    }
}

//
// The following routine does not generate code in FINAL ROM builds. 
//
#ifndef SDK_FINALROM 

//------------------------------------------------------------------------------
// Displays NNSG2dUserExCellAttrBank content in debug output.
void NNSi_G2dPrintUserExCellAttrBank( const NNSG2dUserExCellAttrBank* pCellAttrBank )
{
    u16 i, j;
    OS_Printf( "------ NNSG2dUserExCellAttrBank ------\n" );
    OS_Printf( "numCells     = %d \n", pCellAttrBank->numCells );
    OS_Printf( "numAttribute = %d \n", pCellAttrBank->numAttribute );
              
    for( i = 0; i < pCellAttrBank->numCells; i++ )
    {
        NNSG2dUserExCellAttr* pCellAttr 
           = &pCellAttrBank->pCellAttrArray[i];            
           
        for( j = 0; j < pCellAttrBank->numAttribute; j++ )
        {
           OS_Printf( "attr(%03d, %d) = %08x\n", i, j, pCellAttr->pAttr[j] );
        }
    }     
    OS_Printf( "--------------------------------------\n" );
}

#endif // SDK_FINALROM

