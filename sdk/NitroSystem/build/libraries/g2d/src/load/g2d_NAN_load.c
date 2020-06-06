/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_NAN_load.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NAN_load.c,v $
  Revision 1.19  07/07/2005 06:40:43 AM  kitani_toshikazu
  Added processing for user-extended attributes

  Revision 1.18  05/27/2005 08:49:54 AM  kitani_toshikazu
  Added assert statement that checks for animation sequences
  consisting only of frames with display time of zero.

  Revision 1.17  02/22/2005 12:48:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.16  02/21/2005 02:14:22 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.15  11/15/2004 08:27:36 AM  kitani_toshikazu
  Fixed debug output

  Revision 1.14  10/27/2004 11:57:08 PM  kitani_toshikazu
  New animation type
  Support for NNS_G2D_ANIMELEMENT_INDEX_T

  Revision 1.13  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.12  09/03/2004 10:03:20 AM  hatamoto_minoru
  Debug output string table no longer compiled in FinalRom build.

  Revision 1.11  09/02/2004 12:13:09 AM  kitani_toshikazu
  Changed ASSERT message

  Revision 1.10  08/26/2004 07:22:35 AM  kitani_toshikazu
  (none)

  Revision 1.9  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.8  07/20/2004 05:57:50 AM  kitani_toshikazu
  fix comments.

  Revision 1.7  07/14/2004 12:27:07 PM  kitani_toshikazu
  Supported for play method control data.

  Revision 1.6  07/01/2004 04:24:08 AM  kitani_toshikazu
  Added NNS_G2dGetUnpackedMCAnimBank()

  Revision 1.5  06/30/2004 11:32:46 AM  kitani_toshikazu
  Support for multicell animation.

  Revision 1.4  06/22/2004 06:28:09 AM  kitani_toshikazu
  Change of debug output functions to dummy functions during FINAL ROM build

  Revision 1.3  06/09/2004 09:33:29 AM  kitani_toshikazu
  Added gfd

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
#include <nnsys/g2d/load/g2d_NAN_load.h>
#include <nnsys/g2d/g2d_Load.h>
#include "g2di_Debug.h"


#ifndef SDK_FINALROM
static const char* s_animTypeStrTbl[] = 
{
    "NNS_G2D_ANIMATIONTYPE_INVALID",
    "NNS_G2D_ANIMATIONTYPE_CELL",
    "NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION",
    "NNS_G2D_ANIMATIONTYPE_MAX",
};

static const char* s_elemTypeStrTbl[] = 
{
    "NNS_G2D_ANIMELEMENT_INDEX",
    "NNS_G2D_ANIMELEMENT_INDEX_SRT",
    "NNS_G2D_ANIMELEMENT_INDEX_T",
    "NNS_G2D_ANIMELEMENT_MAX"
};

static const char* s_playModeStrTbl[] = 
{
    "NNS_G2D_ANIMATIONPLAYMODE_INVALID",
    "NNS_G2D_ANIMATIONPLAYMODE_FORWARD",          
    "NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP",     
    "NNS_G2D_ANIMATIONPLAYMODE_REVERSE",          
    "NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP",     
    "NNS_G2D_ANIMATIONPLAYMODE_MAX"               
};
#endif // SDK_FINALROM

//------------------------------------------------------------------------------
static BOOL GetUnpackedAnimBankImpl_( void* pNanrFile, NNSG2dAnimBankData** ppAnimBank )
{
    NNS_G2D_NULL_ASSERT( pNanrFile );
    NNS_G2D_NULL_ASSERT( ppAnimBank );
    {
        NNSG2dBinaryFileHeader*     pBinFile    = (NNSG2dBinaryFileHeader*)pNanrFile;
        {
            NNSG2dAnimBankDataBlock* pAnimBankBlk   =
                (NNSG2dAnimBankDataBlock*)NNS_G2dFindBinaryBlock( pBinFile, 
                                                                  NNS_G2D_BLKSIG_ANIMBANK );
            if( pAnimBankBlk )
            {
                
                NNS_G2dUnpackNAN( (void*)&pAnimBankBlk->animBankData );
                   
                //
                // OK
                //
                *ppAnimBank = &pAnimBankBlk->animBankData;
                return TRUE;   
            }
        }                
    }
    
    //
    // NG
    //
    *ppAnimBank = NULL;
    return FALSE;
}

//------------------------------------------------------------------------------
//
// Animation sequences that contain only frames with 0 display time are invalid
//
static BOOL CheckAnimSequenceValidity_( const NNSG2dAnimSequenceData* pSeq )
{
    int        i;
    BOOL       bHasEffectiveFrame = FALSE;
    
    for( i = 0; i < pSeq->numFrames; i++ )
    {   
        if( pSeq->pAnmFrameArray[i].frames != 0 )
        {
            bHasEffectiveFrame = TRUE;
        }
    }
    
    return bHasEffectiveFrame;
}



//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedAnimBank( void* pNanrFile, NNSG2dAnimBankData** ppAnimBank )
{
    NNS_G2D_NULL_ASSERT( pNanrFile );
    NNS_G2D_NULL_ASSERT( ppAnimBank );
    
    
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNanrFile, 
                                                        NNS_G2D_BINFILE_SIG_CELLANIM ),
                     "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNanrFile, 
                                                      BIN_FILE_VERSION( NANR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
    
    return GetUnpackedAnimBankImpl_( pNanrFile, ppAnimBank );
}

//------------------------------------------------------------------------------
BOOL NNS_G2dGetUnpackedMCAnimBank( void* pNanrFile, NNSG2dAnimBankData** ppAnimBank )
{
    NNS_G2D_NULL_ASSERT( pNanrFile );
    NNS_G2D_NULL_ASSERT( ppAnimBank );
    
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileSignatureValid( pNanrFile, 
                                                        NNS_G2D_BINFILE_SIG_MULTICELLANIM ),
                     "Input file signature is invalid for this method." );
                     
    NNS_G2D_ASSERTMSG( NNSi_G2dIsBinFileVersionValid( pNanrFile, 
                                                      BIN_FILE_VERSION( NANR ) ), 
                       "Input file is obsolete. Please use the new g2dcvtr.exe." );
    
    return GetUnpackedAnimBankImpl_( pNanrFile, ppAnimBank );
}

//------------------------------------------------------------------------------
void NNS_G2dUnpackNAN( NNSG2dAnimBankData* pData )
{
    u16 i, j;
    
    NNS_G2D_NULL_ASSERT( pData );
    
    pData->pSequenceArrayHead = NNS_G2D_UNPACK_OFFSET_PTR( pData->pSequenceArrayHead, pData );
    pData->pFrameArrayHead    = NNS_G2D_UNPACK_OFFSET_PTR( pData->pFrameArrayHead, pData );
    pData->pAnimContents      = NNS_G2D_UNPACK_OFFSET_PTR( pData->pAnimContents, pData );
    
    {
        NNSG2dAnimSequenceData*   pSeq            = pData->pSequenceArrayHead;
        NNSG2dAnimFrameData*      pFrameBase      = pData->pFrameArrayHead;
        void*                     pContentsBase   = pData->pAnimContents;
        
        // unpack offset pointer.
        for( i = 0; i < pData->numSequences; i++ )
        {
            pSeq[i].pAnmFrameArray = NNS_G2D_UNPACK_OFFSET_PTR( pSeq[i].pAnmFrameArray, pFrameBase );
            
            for( j = 0; j < pSeq[i].numFrames; j++ )
            {
                pSeq[i].pAnmFrameArray[j].pContent = 
                    NNS_G2D_UNPACK_OFFSET_PTR( pSeq[i].pAnmFrameArray[j].pContent , pContentsBase );
            }
            //
            // Check for invalid animation sequences
            //
            NNS_G2D_ASSERTMSG( CheckAnimSequenceValidity_( &pSeq[i] ), "An invalid anim-sequence is detected." );
        }
    }
    
    //
    // User extended data
    //
    if( pData->pExtendedData != NULL )
    {
        pData->pExtendedData = NNS_G2D_UNPACK_OFFSET_PTR( pData->pExtendedData, pData );
        {        
            u32 i = 0;
            u32 j = 0;
            
            NNSG2dUserExDataBlock* pExBlk 
                = (NNSG2dUserExDataBlock*)pData->pExtendedData;
            NNSG2dUserExAnimAttrBank* pAnmExAttrBank 
                = (NNSG2dUserExAnimAttrBank*)(pExBlk + 1);
            pAnmExAttrBank->pAnmSeqAttrArray 
               = NNS_G2D_UNPACK_OFFSET_PTR( pAnmExAttrBank->pAnmSeqAttrArray,
                                            pAnmExAttrBank );
                                                                            
            for( i = 0; i < pAnmExAttrBank->numSequences; i++ )
            {
                NNSG2dUserExAnimSequenceAttr* pSeqAttr 
                  = &pAnmExAttrBank->pAnmSeqAttrArray[i];
                
                pSeqAttr->pAttr
                  = NNS_G2D_UNPACK_OFFSET_PTR( pSeqAttr->pAttr,
                                            pAnmExAttrBank );
                pSeqAttr->pAnmFrmAttrArray 
                  = NNS_G2D_UNPACK_OFFSET_PTR( pSeqAttr->pAnmFrmAttrArray,
                                               pAnmExAttrBank ); 
                for( j = 0; j < pSeqAttr->numFrames; j++ )
                {
                    NNSG2dUserExAnimFrameAttr* pFrm = &pSeqAttr->pAnmFrmAttrArray[j];
                    pFrm->pAttr
                      = NNS_G2D_UNPACK_OFFSET_PTR( pFrm->pAttr, pAnmExAttrBank ); 
                }                
            }
        }
    }
    
    NNSI_G2D_DEBUGMSG0("Unpacking NANR file is successful.\n" );
}

//------------------------------------------------------------------------------
const NNSG2dAnimSequenceData* NNS_G2dGetAnimSequenceByIdx( const NNSG2dAnimBankData* pAnimBank, u16 idx )
{
    NNS_G2D_NULL_ASSERT( pAnimBank );
    
    if( NNS_G2dGetNumAnimSequence( pAnimBank ) > idx )
    {
        return &pAnimBank->pSequenceArrayHead[idx];
    }else{
        return NULL;
    }
}

#ifndef SDK_FINALROM

//------------------------------------------------------------------------------
// Display user-extended attributes. 
static void PrintAnimUserExAttr_( const void* pUserEx )
{
    u32 i = 0;
    u32 j = 0;
    u32 k = 0;
    
    NNSG2dUserExDataBlock*    pExBlk         = (NNSG2dUserExDataBlock*)pUserEx;
    NNSG2dUserExAnimAttrBank* pAnmExAttrBank = (NNSG2dUserExAnimAttrBank*)(pExBlk + 1);
    
    OS_Printf( "------- NNSG2dUserExAnimAttrBank -------\n" );
    OS_Printf( "numAttribute = %d\n", pAnmExAttrBank->numAttribute );
    OS_Printf( "numSequences = %d\n", pAnmExAttrBank->numSequences );
    
    for( i = 0; i < pAnmExAttrBank->numSequences; i++ )
    {
        NNSG2dUserExAnimSequenceAttr* pSeqAttr = &pAnmExAttrBank->pAnmSeqAttrArray[i];
        OS_Printf( "------- NNSG2dUserExAnimSequenceAttr -------\n" );
        OS_Printf( "numFrames = %d\n", pSeqAttr->numFrames );
        
        for( k = 0; k < pAnmExAttrBank->numAttribute; k++ )
        {
            OS_Printf( "seq_attr( %03d, %d )   = %08x\n", i, k, pSeqAttr->pAttr[0] );
        }
        
        for( j = 0; j < pSeqAttr->numFrames; j++ )
        {
            NNSG2dUserExAnimFrameAttr* pFrm = &pSeqAttr->pAnmFrmAttrArray[j];
            for( k = 0; k < pAnmExAttrBank->numAttribute; k++ )
            {
                OS_Printf( "frame_attr( %03d, %d ) = %08x\n", j, k, pFrm->pAttr[0] );        
            }
        }                
        OS_Printf( "-------------------------------------------\n" );
    }
    OS_Printf( "---------------------------------------\n" );
}

//------------------------------------------------------------------------------
void NNS_G2dPrintAnimContents( const void* pData, NNSG2dAnimationElement animElem )
{
    NNS_G2D_NULL_ASSERT( pData );
    
    switch( animElem )
    {
    case NNS_G2D_ANIMELEMENT_INDEX:
        {
        const NNSG2dAnimData*         pAnimData = (const NNSG2dAnimData*)pData;
        OS_Printf( "index = %d\n", *pAnimData );
        break;
        }
    case NNS_G2D_ANIMELEMENT_INDEX_SRT:
        {
        const NNSG2dAnimDataSRT*         pAnimData = (const NNSG2dAnimDataSRT*)pData;
        OS_Printf( "index = %d\n", pAnimData->index );
        
        OS_Printf( "rotZ = %x\n", pAnimData->rotZ );
        
        OS_Printf( "sx = %f\n", FX_FX32_TO_F32( pAnimData->sx ) );
        OS_Printf( "sy = %f\n", FX_FX32_TO_F32( pAnimData->sy ) );
        
        OS_Printf( "px = %d\n", pAnimData->px );
        OS_Printf( "py = %d\n", pAnimData->py );
        break;
        }
    case NNS_G2D_ANIMELEMENT_INDEX_T:
        {
        const NNSG2dAnimDataT*         pAnimData = (const NNSG2dAnimDataT*)pData;
        OS_Printf( "index = %d\n", pAnimData->index );
        
        OS_Printf( "px = %d\n", pAnimData->px );
        OS_Printf( "py = %d\n", pAnimData->py );
        break;
        }
    default:
        NNS_G2D_ASSERT( FALSE );
    }
}

//------------------------------------------------------------------------------
void NNS_G2dPrintAnimFrame( const NNSG2dAnimFrameData* pFrame, NNSG2dAnimationElement animElem )
{
    NNS_G2D_NULL_ASSERT( pFrame );
    
    OS_Printf( "frames = %d\n", pFrame->frames );
    NNS_G2dPrintAnimContents( pFrame->pContent , animElem );
}


//------------------------------------------------------------------------------
void NNS_G2dPrintAnimSequence( const NNSG2dAnimSequenceData* pSeq )
{
    u16 i;
    NNS_G2D_NULL_ASSERT( pSeq );
    {
        const NNSG2dAnimationType    animType = NNS_G2dGetAnimSequenceAnimType( pSeq );
        const NNSG2dAnimationElement animElem = NNS_G2dGetAnimSequenceElementType( pSeq );
        
        NNS_G2D_MINMAX_ASSERT( animType, NNS_G2D_ANIMATIONTYPE_CELL, NNS_G2D_ANIMATIONTYPE_MAX );
        NNS_G2D_MINMAX_ASSERT( animElem, NNS_G2D_ANIMELEMENT_INDEX, NNS_G2D_ANIMELEMENT_INDEX_T );
        
        OS_Printf( "-------------Sequence---------------\n" );
        OS_Printf( "numFrames = %d\n", pSeq->numFrames );
        OS_Printf( "animType  = %s\n", s_animTypeStrTbl[ animType ] );
        OS_Printf( "animElem  = %s\n", s_elemTypeStrTbl[ animElem ] );
        
        OS_Printf( "loopStartFrameIdx  = %d\n", pSeq->loopStartFrameIdx );
        OS_Printf( "playMode           = %s\n", s_playModeStrTbl[pSeq->playMode] );
        
        for( i = 0; i < pSeq->numFrames; i++ )
        {
            NNS_G2dPrintAnimFrame( &pSeq->pAnmFrameArray[i], animElem );
        }
        OS_Printf( "------------------------------------\n" );
    }
}

//------------------------------------------------------------------------------
void NNS_G2dPrintAnimBank( const NNSG2dAnimBankData* pAnimBank )
{
    u16 i;
    
    NNS_G2D_NULL_ASSERT( pAnimBank );
    
    OS_Printf( "---------------Anim Bank---------------------\n" );
    OS_Printf( "numSequences = %d\n", pAnimBank->numSequences );
    for( i = 0; i < pAnimBank->numSequences; i++ )
    {
        NNS_G2dPrintAnimSequence( &pAnimBank->pSequenceArrayHead[i] );
    }
    //
    // User extended attributes
    //
    if( pAnimBank->pExtendedData != NULL )
    {
        OS_Printf( "---------------UserEx--------------------\n" );
           PrintAnimUserExAttr_( pAnimBank->pExtendedData );
        OS_Printf( "-----------------------------------------\n" );
    }
    OS_Printf( "---------------------------------------------\n" );
}
#endif // SDK_FINALROM


