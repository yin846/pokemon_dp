/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_MultiCellAnimation.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_MultiCellAnimation.c,v $
  Revision 1.40  08/17/2005 10:21:19 AM  kitani_toshikazu
  Changed the argument of the function that calls the multicell callback. 

  Revision 1.39  07/11/2005 01:58:17 AM  kitani_toshikazu
  Added G2dTraverseMCNodes().

  Revision 1.38  07/07/2005 06:41:17 AM  kitani_toshikazu
  Added NNS_G2dTraverseMCCellAnims().

  Revision 1.37  06/30/2005 11:28:56 AM  kitani_toshikazu
  In frames that cause switching of multicell animation frames inside NNS_G2dTickMCAnimation(),  
  we stopped the updating of cell animations inside the multicell. 

  Revision 1.36  06/29/2005 08:30:39 AM  kitani_toshikazu
  Corrected comment

  Revision 1.35  06/29/2005 12:43:49 AM  kitani_toshikazu
  Revisions related to updating of multicell animations that include non-looping animations. 

  
  Revision 1.31  06/23/2005 02:03:15 AM  kitani_toshikazu
  Fixed updating problem in animation with data that specifies NNS_G2D_MCANIM_PLAYMODE_CONTINUE for the node. 
  Fixed problem with incorrect updating of animation. 

  Revision 1.30  05/30/2005 09:22:37 AM  kitani_toshikazu
  Added NNS_G2dSetMCAnimationCellAnimFrame().

  Revision 1.29  2005/02/23 12:02:18 AM  takano_makoto
  fix copyright header.

  Revision 1.28  02/07/2005 12:42:12 PM  kitani_toshikazu
  NNS_G2dSetMCAnimationSpeed() and
  NNS_G2dResetMCCellAnimationAll().
  Fixed bug in function

  Revision 1.27  12/14/2004 07:27:13 AM  kitani_toshikazu
  Added NNS_G2dResetMCCellAnimationAll()

  Revision 1.26  12/14/2004 02:52:29 AM  kitani_toshikazu
  Cleaned up the code

  Revision 1.25  12/13/2004 11:53:58 PM  kitani_toshikazu
  (none)

  Revision 1.24  12/13/2004 08:24:28 AM  kitani_toshikazu
  Merged branch g2d_multiCell

  Revision 1.23.2.3  2004/12/13 08:07:51  kitani_toshikazu
  (none)

  Revision 1.23.2.2  2004/12/13 05:33:25  kitani_toshikazu
  (none)

  Revision 1.23.2.1  2004/12/13 02:54:14  kitani_toshikazu
  Merged the new multicell module

  Revision 1.23  12/07/2004 11:56:23 PM  kitani_toshikazu
  (none)

  Revision 1.22  11/15/2004 08:27:15 AM  kitani_toshikazu
  Added NNS_G2dGetMCBankNumNodesRequired()

  Revision 1.21  10/27/2004 11:56:25 PM  kitani_toshikazu
  New animation type
  Support for NNS_G2D_ANIMELEMENT_INDEX_T

  Revision 1.20  10/19/2004 11:57:53 AM  kitani_toshikazu
  Changed internal functions to inline functions

  Revision 1.19  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.18  10/18/2004 09:47:54 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.17  09/01/2004 04:29:52 AM  kitani_toshikazu
  Corrected a bug where the cell animation playback state was not reset when switching multi-cells

  Revision 1.16  08/26/2004 07:21:49 AM  kitani_toshikazu
  Updated to support SRT animation

  Revision 1.15  07/29/2004 12:11:21 AM  kitani_toshikazu
  Changed function names.

  Revision 1.14  07/28/2004 08:12:42 AM  kitani_toshikazu
  Corrected comment

  Revision 1.13  07/24/2004 06:58:45 AM  kitani_toshikazu
  Changed function names related to animation controller.

  Revision 1.12  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.11  07/17/2004 06:55:28 AM  kitani_toshikazu
  Changed name from NNS_G2dSetMultiCellDataToMCInstance()
  to NNS_G2dSetMCDataToMCInstance().
  Changed name from NNS_G2dGetNumNodesRequired()
  to NNS_G2dGetMCNumNodesRequired().

  Revision 1.10  07/14/2004 12:22:24 PM  kitani_toshikazu
  Supported for play method control data.

  Revision 1.9  06/30/2004 11:33:46 AM  kitani_toshikazu
  Changed to avoid updates for invalid notes

  Revision 1.8  06/22/2004 06:36:00 AM  kitani_toshikazu
  Deleted the comment in the debug output line

  Revision 1.7  05/28/2004 07:22:01 AM  kitani_toshikazu
  Changed MultiCell animation update method.

  Revision 1.6  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.5  05/10/2004 07:25:18 AM  kitani_toshikazu
  fix some bugs.

  Revision 1.4  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.3  04/26/2004 04:34:00 AM  kitani_toshikazu
  Process to change interface names

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_MultiCellAnimation.h>
#include <nnsys/g2d/g2d_SRTControl.h>

#include <nnsys/g2d/load/g2d_NMC_load.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>


//------------------------------------------------------------------------------
// Sets node information in a cell animation. 
NNS_G2D_INLINE void SetNodeDataToCellAnim_( 
    NNSG2dCellAnimation*                  pCellAnim,
    const NNSG2dMultiCellHierarchyData*   pNodeData, 
    const NNSG2dAnimBankData*             pAnimBank,
    u16                                   mcTotalFrame 
)
{
    const NNSG2dAnimSequenceData*  pAnimSeq     = NULL;
    
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_NULL_ASSERT( pNodeData );
    NNS_G2D_NULL_ASSERT( pAnimBank );
    
    
    // Gets the animation entity
    pAnimSeq = NNS_G2dGetAnimSequenceByIdx( pAnimBank, pNodeData->animSequenceIdx );
    
    //
    // Reflects the NNSG2dNode setup data
    // The currently displayed animation frame is reset to the sequence start. 
    //
    NNS_G2dSetCellAnimationSequence( pCellAnim, pAnimSeq );            
    // Starts the playback
    NNS_G2dStartAnimCtrl( NNS_G2dGetCellAnimationAnimCtrl( pCellAnim ) );
    
                
    //
    // Is the cell animation playback method set to continuous playback?
    // 
    //
    if( NNSi_G2dGetMultiCellNodePlayMode( pNodeData ) == NNS_G2D_MCANIM_PLAYMODE_CONTINUE )
    {
        // The animation's playback position is reset to the start of the animation sequence
        // by the execution of NNS_G2dSetCellAnimationSequence(). 
        // 
        // At this point, updates the playback position of the cell animation to the proper position. 
        const u32 animSeqLength = NNS_G2dCalcAnimSequenceTotalVideoFrames( pAnimSeq );
        
        // Was looping playback specified?
        // If looping was not specified, after the animation sequence is played once initially, it must be stopped at its end. 
        // 
        // 
        if( NNSi_G2dIsAnimCtrlLoopAnim( NNS_G2dGetCellAnimationAnimCtrl( pCellAnim ) ) )
        {
           const u32 frameToMove   = ( mcTotalFrame % animSeqLength );
           NNS_G2dTickCellAnimation( pCellAnim, (fx32)( frameToMove  << FX32_SHIFT ) );                            
        }else{
           const u32 frameToMove   = ( mcTotalFrame >= animSeqLength ) ? animSeqLength : mcTotalFrame;
           NNS_G2dTickCellAnimation( pCellAnim, (fx32)( frameToMove  << FX32_SHIFT ) );        
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         SetMCDataToMCInstanceImpl_

  Description:  Sets multi-cell data in a multi-cell instance.
                This needs to be called after initialization.
                Enough nodes must be maintained for the actual multicell to reflect the multicell data. 
                
                
                Caution:
                Previous versions compared pMCInst->numNode and pMcData->numNodes,
                 and if conditions were not satisfied returned FALSE without processing.
                 However, we determined that such a specification had little practical meaning,
                 and now processing occurs based on the assumption that the multicell animation instance meets the previous conditions. 
                 
                 Therefore the user must assure that conditions are met.
                
                The internal structure will vary greatly depending on the NNSG2dMultiCellInstance.mcType member specified when the multicell instance was initialized. 
                 
                 Accordingly, processing even in this function can be divided into two main sections. 
                
                
  Arguments:    pMultiCellAnim:        Multi-cell animation entity
                pMcData         :        Multi cell data
                mcTotalFrame     :        Animation playback total video frame length
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
 
//
// This function has been changed to a private function.
// The old API is left here for compatibility with previous versions.
//
static void SetMCDataToMCInstanceImpl_
( 
    NNSG2dMultiCellInstance*       pMCInst, 
    const NNSG2dMultiCellData*     pMcData,
    u16                            mcTotalFrame 
)
{
    u16 i;
    
    NNS_G2D_NULL_ASSERT( pMCInst );
    NNS_G2D_NULL_ASSERT( pMCInst->pAnimDataBank );
    NNS_G2D_NULL_ASSERT( pMcData );
    
    pMCInst->pCurrentMultiCell = pMcData;
    
    //
    // Multicell instance type?
    //  The internal structure will differ greatly depending on the NNSG2dMultiCellInstance.mcType member specified when the multicell instance was initialized. 
    //  
    //  The method used to obtain pCellAnim is quite different. 
    //
    if( pMCInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
        
        NNSG2dMCCellAnimation* pCellAnimArray = (NNSG2dMCCellAnimation*)pMCInst->pCellAnimInstasnces;
        
        //
        // Resets the initialized flag.
        //
        for( i = 0; i < pMcData->numCellAnim; i++ )
        {
            pCellAnimArray[i].bInited = FALSE;
        }
        
        // for each NNSG2Node...
        for( i = 0; i < pMcData->numNodes; i++ )
        {
            // 
            // TODO: Get from nodeAttr. Use converter to insert data.
            //
            const NNSG2dMultiCellHierarchyData* pNodeData    = &pMcData->pHierDataArray[i];
            const u16                           cellAnimIdx  = NNSi_G2dGetMC2NodeCellAinmIdx( pNodeData );
            
            // Initialized? 
            // If there are cell animations that are shared by multiple nodes, the condition is considered valid. 
            // 
            if( !pCellAnimArray[cellAnimIdx].bInited )
            {
                NNSG2dCellAnimation*          pCellAnim = &pCellAnimArray[cellAnimIdx].cellAnim;
                
                SetNodeDataToCellAnim_( pCellAnim, 
                                        pNodeData, 
                                        pMCInst->pAnimDataBank,
                                        mcTotalFrame );
                  
                pCellAnimArray[cellAnimIdx].bInited = TRUE;
            }
        }
        
    }else{
       
       NNSG2dNode*   pNodeArray   = (NNSG2dNode*)(pMCInst->pCellAnimInstasnces);
       
       
       // for each NNSG2Node...
       for( i = 0; i < pMcData->numNodes; i++ )
       {
           const NNSG2dMultiCellHierarchyData* pNodeData    = &pMcData->pHierDataArray[i];
           NNSG2dCellAnimation*  pCellAnim    = (NNSG2dCellAnimation*)pNodeArray[i].pContent;
                  
           SetNodeDataToCellAnim_( pCellAnim, 
                                   pNodeData, 
                                   pMCInst->pAnimDataBank,
                                   mcTotalFrame );
               
           pNodeArray[i].bVisible = TRUE;                          
           NNSi_G2dSrtcSetTrans( &pNodeArray[i].srtCtrl, pNodeData->posX, pNodeData->posY );             
       }
    }
}


//------------------------------------------------------------------------------
NNS_G2D_INLINE void ApplyCurrentAnimResult_( NNSG2dMultiCellAnimation* pMultiCellAnim )
{
    //
    // This is a bit sloppy, but regardless of the animation format, it will be accessed by NNSG2dAnimDataSRT.
    // (If the animation format is NNS_G2D_ANIMELEMENT_INDEX, it will cause incorrect data to be put into the SRT section.)
    // 
    const NNSG2dAnimDataSRT*      pAnimResult  = NULL;
    const NNSG2dMultiCellData*    pData        = NULL;
    
    
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    NNS_G2D_NULL_ASSERT( pMultiCellAnim->pMultiCellDataBank );
    
    pAnimResult 
        = (const NNSG2dAnimDataSRT*)NNS_G2dGetAnimCtrlCurrentElement( &pMultiCellAnim->animCtrl );
    
    pData  
        = NNS_G2dGetMultiCellDataByIdx( pMultiCellAnim->pMultiCellDataBank, 
                                        pAnimResult->index );
    NNS_G2D_NULL_ASSERT( pData );
    
    //
    // Use SRT
    //
    {
       const NNSG2dAnimationElement elemType 
           = NNSi_G2dGetAnimSequenceElementType( pMultiCellAnim->animCtrl.pAnimSequence->animType );
       NNSi_G2dSrtcInitControl  ( &pMultiCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
       
       if( elemType != NNS_G2D_ANIMELEMENT_INDEX )
       {
           if( elemType == NNS_G2D_ANIMELEMENT_INDEX_T )
           {
              const NNSG2dAnimDataT*  pAnmResT = (const NNSG2dAnimDataT*)pAnimResult;
              NNSi_G2dSrtcSetTrans     ( &pMultiCellAnim->srtCtrl, pAnmResT->px, pAnmResT->py );// T
           }else{
              NNSi_G2dSrtcSetSRTScale  ( &pMultiCellAnim->srtCtrl, pAnimResult->sx, pAnimResult->sy );// S    
              NNSi_G2dSrtcSetSRTRotZ   ( &pMultiCellAnim->srtCtrl, pAnimResult->rotZ );// R    
              NNSi_G2dSrtcSetTrans     ( &pMultiCellAnim->srtCtrl, pAnimResult->px, pAnimResult->py );// T    
           }
       }
       
    }
    
    //
    // multicell configuration
    //    
    SetMCDataToMCInstanceImpl_( &pMultiCellAnim->multiCellInstance, 
                                pData, 
                                pMultiCellAnim->totalVideoFrame );
    
    NNS_G2D_NULL_ASSERT( pMultiCellAnim->multiCellInstance.pCurrentMultiCell );
}



//------------------------------------------------------------------------------
NNS_G2D_INLINE void FVecToSVec( const NNSG2dFVec2* pvSrc, NNSG2dSVec2* pvDst )
{
    pvDst->x = (s16)(pvSrc->x >> FX32_SHIFT);
    pvDst->y = (s16)(pvSrc->y >> FX32_SHIFT);
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void SVecToFVec( const NNSG2dSVec2* pvSrc, NNSG2dFVec2* pvDst )
{
    pvDst->x = pvSrc->x << FX32_SHIFT;
    pvDst->y = pvSrc->y << FX32_SHIFT;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void AddSVec_( const NNSG2dSVec2* pv1, const NNSG2dSVec2* pv2, NNSG2dSVec2* pvDst )
{
    pvDst->x = (s16)(pv1->x + pv2->x);
    pvDst->y = (s16)(pv1->y + pv2->y);
}

//------------------------------------------------------------------------------
//
static u16 GetMCBankNumCellAnimRequired_
( 
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );    
    {   
        const NNSG2dMultiCellData*  pMCell    = NULL;
        u16                         maxNum    = 0;
        u16                         i;
        
        for( i = 0; i < pMultiCellDataBank->numMultiCellData; i++ )
        {
            pMCell = NNS_G2dGetMultiCellDataByIdx( pMultiCellDataBank, i );
            NNS_G2D_NULL_ASSERT( pMCell );
            
            if( pMCell->numCellAnim > maxNum )
            {
                maxNum = pMCell->numCellAnim;
            }
        }
        return maxNum;
    }
}

//------------------------------------------------------------------------------
static u16 MakeCellAnimToOams_
(
    GXOamAttr*                   pDstOams, 
    u16                          numDstOams,
    const NNSG2dCellAnimation*   pCellAnim,
    const NNSG2dSVec2*           pNodeTrans,
    const MtxFx22*               pMtxSR, 
    const NNSG2dFVec2*           pBaseTrans,
    u16                          affineIndex,
    BOOL                         bDoubleAffine
)
{
    NNSG2dFVec2                       vTransF;
    NNSG2dSVec2                       vTransS;
    const NNSG2dSRTControl*           pContentsSRT;
    
    pContentsSRT = &pCellAnim->srtCtrl;
   
    // trans
    {
        // vTransS = pContentsSRT->srtData.trans + pNode->srtCtrl.srtData.trans
        AddSVec_( &pContentsSRT->srtData.trans, 
                   pNodeTrans, 
                  &vTransS );
        
        
        // If pMtxSR exists, transforms Trans for Node .
        if( pMtxSR != NULL )
        {
            vTransF.x = pMtxSR->_00 * vTransS.x + pMtxSR->_10 * vTransS.y;
            vTransF.y = pMtxSR->_01 * vTransS.x + pMtxSR->_11 * vTransS.y;  
            
        }else{
            SVecToFVec( &vTransS, &vTransF );
        }
        
        // Adds to vNodeTrans if pBaseTrans exists
        if( pBaseTrans != NULL )
        {
            vTransF.x += pBaseTrans->x;
            vTransF.y += pBaseTrans->y;
        }
    }
    
    //
    // Do the constituent elements have an Affine conversion?
    // 
    // TODO: Make this into NNS_G2D_WARNING.
    if( NNSi_G2dSrtcIsAffineEnable_SR( pContentsSRT ) )
    {
        // Warning!
        OS_Warning("invalid affine transformation is found in NNS_G2dInitMCAnimation()");
    }
    
    //
    // Converts Cell to Oam
    // 
    return NNS_G2dMakeCellToOams( pDstOams,
                                  numDstOams,
                                  NNS_G2dGetCellAnimationCurrentCell( pCellAnim ), 
                                  pMtxSR, 
                                  &vTransF, 
                                  affineIndex,
                                  bDoubleAffine );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 MakeSimpleMultiCellToOams_
( 
    GXOamAttr*                      pDstOams, 
    u16                             numDstOams,
    const NNSG2dMultiCellInstance*  pMCellInst, 
    const MtxFx22*                  pMtxSR, 
    const NNSG2dFVec2*              pBaseTrans,
    u16                             affineIndex,
    BOOL                            bDoubleAffine 
)
{
    u16     i           = 0;
    u16     numOamUsed  = 0;
    int     numRestOams = numDstOams;
    NNSG2dNode* pNode   = NULL;
    
    NNS_G2D_NULL_ASSERT( pDstOams );
    NNS_G2D_NULL_ASSERT( pMCellInst );
    NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );
    NNS_G2D_ASSERT( pMCellInst->mcType == NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM );
    
    
    //
    // For each Node
    //
    pNode = (NNSG2dNode*)pMCellInst->pCellAnimInstasnces;
    for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
    {
        // If there is sufficient buffer capacity...
        if( numRestOams > 0 )
        {
            //
            // transform Node to Oam Attrs.
            //
            
            //
            // Caution:
            // It is assumed that pNode->type is NNS_G2D_NODETYPE_CELL
            // Does not support rendering multi-cells whose node SR parameters have changed.
            //
            NNS_G2D_WARNING( pNode->type == NNS_G2D_NODETYPE_CELL &&
                            !NNSi_G2dSrtcIsAffineEnable_SR( &pNode->srtCtrl ),
                            "A SR-Transformation of a multicell-node was ignored." );
                            
            numOamUsed = MakeCellAnimToOams_( pDstOams,
                                              (u16)numRestOams,
                                              pNode[i].pContent,
                                              &pNode[i].srtCtrl.srtData.trans,
                                              pMtxSR,
                                              pBaseTrans,
                                              affineIndex,
                                              bDoubleAffine );
            
            // Updates the write buffer position
            numRestOams -= numOamUsed;
            pDstOams    += numOamUsed;    
        }else{
            // End of loop.
            break;
        }
    }
    
    NNS_G2D_ASSERT( numDstOams >= numRestOams );
    return (u16)( numDstOams - numRestOams );
}

// 
// Sequence of events leading to NNS_G2dMakeSimpleMultiCellToOams()
// 
// Problem
// 
// If a MultiCell Node or NodeContents contain Affine transformation,
// during transformation multiple affine transformation matrices may be generated.
// 
// (1) While setting these matrices in HW,
// (2) for Oam Attrs you must configure affineIndex according to the results of (1)
// 
// In the case of both (1) and (2) it is expected that people will want to customize the implementation with client code.
// Normally, in such a case as this, a function pointer passes the processes in (1) and (2) to an external function.
// But this would lose sight of the purpose of this function, which is provided as a simple rendering interface.
// 



//------------------------------------------------------------------------------
NNS_G2D_INLINE u16 MakeSimpleMultiCellToOams_ShareCellAnims_
( 
    GXOamAttr*                      pDstOams, 
    u16                             numDstOams,
    const NNSG2dMultiCellInstance*  pMCellInst, 
    const MtxFx22*                  pMtxSR, 
    const NNSG2dFVec2*              pBaseTrans,
    u16                             affineIndex,
    BOOL                            bDoubleAffine 
)
{
    u16     i           = 0;
    u16     numOamUsed  = 0;
    int     numRestOams = numDstOams;
    const NNSG2dMCCellAnimation* pMCCellAnimArray = NULL;
    NNSG2dSVec2       nodeTrans;
    
    NNS_G2D_NULL_ASSERT( pDstOams );
    NNS_G2D_NULL_ASSERT( pMCellInst );
    NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );
    NNS_G2D_ASSERT( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM );
    
    pMCCellAnimArray = (const NNSG2dMCCellAnimation*)pMCellInst->pCellAnimInstasnces;
    
    //
    // For each Node
    //
    for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
    {
        const NNSG2dMultiCellHierarchyData* pNode 
           = &pMCellInst->pCurrentMultiCell->pHierDataArray[i];
        
        //
        // TODO :
        //
        // pNode->animSequenceIdx
        // Cell animation numbers associated with a node are stored in pNode->nodeAttr
        //
        //
        const u16 cellAnimIdx  = NNSi_G2dGetMC2NodeCellAinmIdx( pNode );
        
        
           
           
        // If there is sufficient buffer capacity...
        if( numRestOams > 0 )
        {
            //
            // transform Node to Oam Attrs.
            //
            nodeTrans.x = pNode->posX;
            nodeTrans.y = pNode->posY;
            
            numOamUsed = MakeCellAnimToOams_( pDstOams,
                                              (u16)numRestOams,
                                              &pMCCellAnimArray[cellAnimIdx].cellAnim,
                                              &nodeTrans,
                                              pMtxSR,
                                              pBaseTrans,
                                              affineIndex,
                                              bDoubleAffine );
            
            // Updates the write buffer position
            numRestOams -= numOamUsed;
            pDstOams    += numOamUsed;   
        }else{
            // End of loop.
            break;
        }
    }
    
    NNS_G2D_ASSERT( numDstOams >= numRestOams );
    return (u16)( numDstOams - numRestOams );
}

/*---------------------------------------------------------------------------*
  Name:         InitMCInstance_

  Description:  Initializes instance
                Left here for compatibility with previous versions.
                
                
  Arguments:    pMultiCell      :         multicell instance
                pNodeArray      :         Node instance array ( x numNode )
                pCellAnim             :         Cell animation instance array ( x numNode )
                numNode         :         Number of nodes
                pAnimBank       :         Cell animation definition bank
                pCellDataBank:         Cell data bank
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
NNS_G2D_INLINE void InitMCInstance_
( 
    NNSG2dMultiCellInstance*           pMultiCell, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCell );
    NNS_G2D_NULL_ASSERT( pNodeArray );
    NNS_G2D_NULL_ASSERT( pAnimBank );
    NNS_G2D_ASSERTMSG( numNode != 0, "Non zero value is expected." );
    
    
    //
    // When using this function, initialized as instance of type NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM.  
    // 
    // The NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM type instance's pCellAnimInstasnces member
    // Indicates the start of the node array. 
    //
    pMultiCell->mcType              = NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM;
    pMultiCell->pCurrentMultiCell   = NULL;
    pMultiCell->pAnimDataBank       = pAnimBank;
    pMultiCell->pCellAnimInstasnces = (void*)pNodeArray;
    
    {
        u16 i;
        for( i = 0; i < numNode; i++ ) 
        {
            NNSi_G2dInitializeNode( &pNodeArray[i], NNS_G2D_NODETYPE_CELL );
            // TODO: Make bind into a function.
            // bind
            pNodeArray[i].pContent  = &pCellAnim[i];
            
            
            NNS_G2dInitCellAnimation( 
                &pCellAnim[i], 
                NNS_G2dGetAnimSequenceByIdx( pAnimBank, 0 ) , 
                pCellDataBank );
        }
    }
}

//------------------------------------------------------------------------------
// Initializes multi-cell animation.
// This will be run when NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM is set as the mcType.
//
// When NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM is specified as mcType,
// the instance will be initialized with the same data structure as the previous version, and processed the same.
// As long as the programmer does not access a multi-cell structure node and do something like overwrite data, etc,
// specifying NNS_G2D_MCTYPE_SHARE_CELLANIM and initialing the instance is advantageous from the point of view of memory consumption and processing efficiency. 
//
NNS_G2D_INLINE void InitMCAnimation_( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank 
)
{
#pragma unused( pMultiCellDataBank )
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    
    NNS_G2D_NULL_ASSERT( pNodeArray );
    NNS_G2D_NULL_ASSERT( pCellAnim );
    NNS_G2D_ASSERTMSG( numNode != 0, "TODO: msg" );
    
    NNS_G2D_NULL_ASSERT( pAnimBank );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );
    
    //
    // Initializes the multi-cell entity
    //
    InitMCInstance_( &pMultiCellAnim->multiCellInstance, 
                            pNodeArray, 
                            pCellAnim, 
                            numNode, 
                            pAnimBank,  
                            pCellDataBank );
    
    NNS_G2dInitAnimCtrl( &pMultiCellAnim->animCtrl );
    pMultiCellAnim->pMultiCellDataBank = pMultiCellDataBank;
    NNSi_G2dSrtcInitControl( &pMultiCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
    pMultiCellAnim->totalVideoFrame = 0;
}

//------------------------------------------------------------------------------
// Initializes multi-cell animation.
// This will be run when NNS_G2D_MCTYPE_SHARE_CELLANIM is set as mcType.
// 
// If NNS_G2D_MCTYPE_SHARE_CELLANIM is specified as mcType,
// and if data is input that could cause multiple multi-cell nodes to play the same animation sequences,
// multiple nodes will share cell animation instances. This will decrease memory consumption and animation update processing overhead.
//
// If NNS_G2D_MCTYPE_SHARE_CELLANIM is specified, the multi-cell runtime instance will not hold information that corresponds to nodes.
// Therefore it will not support position changes or affine transformations at node level.
// However, due to this limitation nodes that play the same cell animation always reference the same affine parameters,
// (in particular when using the 2D graphics engine to render) compared to specifying NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM,
// this method can conserve hardware resources (= the affine parameters that are used). 
//
NNS_G2D_INLINE void InitMCAnimation_SharingCellAnim_( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim,  
    void*                              pWork, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    NNS_G2D_NULL_ASSERT( pWork );    
    NNS_G2D_NULL_ASSERT( pAnimBank );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );
    
    //
    // Initializes the multi-cell entity
    //
    pMultiCellAnim->multiCellInstance.mcType = NNS_G2D_MCTYPE_SHARE_CELLANIM;
    {
       NNSG2dMultiCellInstance* pMCInst = &pMultiCellAnim->multiCellInstance;
       
       pMCInst->pAnimDataBank = pAnimBank;
       // Initializes work memory.
       // Uses work memory as NNSG2dMCCellAnimation.
       pMCInst->pCellAnimInstasnces = (void*)pWork;

       {
           u16 i;
           const u16 numCellAnim 
              = GetMCBankNumCellAnimRequired_( pMultiCellDataBank );
           NNSG2dMCCellAnimation* pMCCellAnim 
              = (NNSG2dMCCellAnimation*)pMCInst->pCellAnimInstasnces;
            
           //
           // Concerning all cell animation,
           // this first of all allocates appropriate animation.
           //   
           for( i = 0; i < numCellAnim; i++ ) 
           {
              NNS_G2dInitCellAnimation( 
                  &pMCCellAnim[i].cellAnim, 
                  NNS_G2dGetAnimSequenceByIdx( pAnimBank, 0 ) , 
                  pCellDataBank );
                    
              pMCCellAnim[i].bInited = TRUE;
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetAnimSequenceToMCAnimation

  Description:  Sets the playback animation to the multi-cell animation entity
                This needs to be called after initialization
                
  Arguments:    pMultiCellAnim:         [OUT]  The cell animation instance 
                pAnimSeq:         [IN]  Playback animation 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
// TODO: Executes the function group for checking whether each type of release is correct in ASSERT
void NNS_G2dSetAnimSequenceToMCAnimation
( 
    NNSG2dMultiCellAnimation*         pMultiCellAnim, 
    const NNSG2dAnimSequence*         pAnimSeq
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    NNS_G2D_NULL_ASSERT( pMultiCellAnim->pMultiCellDataBank );
    
    NNS_G2D_NULL_ASSERT( pAnimSeq );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pAnimSeq ) 
                   == NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION, 
                    "NNSG2dAnimationType must be MultiCellLocation." );
    
    NNS_G2dBindAnimCtrl( &pMultiCellAnim->animCtrl, pAnimSeq );
    
    // Resets the multicell playback total frame time.     
    pMultiCellAnim->totalVideoFrame = 0;
    
    ApplyCurrentAnimResult_( pMultiCellAnim );
}




/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitMCInstance

  Description:  Initializes the multi-cell entity
  
                This function has been prepared to replace
                         NNS_G2dInitMCInstance(),
                         NNS_G2dSetMCDataToMCInstance().
                These function remain with separate names to preserve compatibility with previous functions.

                Please use this function for new usages. 
                
  Arguments:    pMultiCellAnim      [OUT] Multicell animation
                pWork            [IN]    Work space used by a multi-cell instance.
                pAnimBank      [IN] Animation bank that defines the cell animations that constitute a multi-cell.
                pCellDataBank       [IN]  Cell data bank 
                pCellDataBank       [IN]  Multicell data bank 
                mcType        [IN]   Multi-cell instance type (NNSG2dMCType type)

  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitMCAnimationInstance
( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim,  
    void*                              pWork, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank,
    NNSG2dMCType                       mcType
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    NNS_G2D_NULL_ASSERT( pWork );    
    NNS_G2D_NULL_ASSERT( pAnimBank );
    NNS_G2D_NULL_ASSERT( pCellDataBank );
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );
    
    
    //
    // Initialization by the instance type
    //
    if( mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       InitMCAnimation_SharingCellAnim_( pMultiCellAnim,
                                         pWork,
                                         pAnimBank,
                                         pCellDataBank,
                                         pMultiCellDataBank );
    }else{
       const u16 numNode 
           = NNS_G2dGetMCBankNumNodesRequired( pMultiCellDataBank );
       NNSG2dNode* pNodeArray 
           = pWork;
       NNSG2dCellAnimation* pCellAnimArray 
           = (NNSG2dCellAnimation*)(pNodeArray + numNode);
                 
       InitMCAnimation_( pMultiCellAnim, 
                         pNodeArray, 
                         pCellAnimArray, 
                         numNode, 
                         pAnimBank,  
                         pCellDataBank,
                         pMultiCellDataBank );
    }
    
    //
    // Initialize common area
    //
    NNS_G2dInitAnimCtrl( &pMultiCellAnim->animCtrl );
    pMultiCellAnim->pMultiCellDataBank = pMultiCellDataBank;
    NNSi_G2dSrtcInitControl( &pMultiCellAnim->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
    pMultiCellAnim->totalVideoFrame = 0;
    
}






/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetMCNumNodesRequired

  Description:  Gets the maximum NNSG2dNode number needed in NNSG2dMultiCellAnimSequence
                
                Because the new initialization API NNS_G2dInitMCAnimationInstance() was added, 
                there is no need to use this function any longer. 
                
  Arguments:    pMultiCellSeq:          [IN]  Multicell animation
                pMultiCellDataBank:     [IN]   Multicell databank 
                
                
  Returns:      Maximum NNSG2dNode number needed in the multi-cell animation
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dGetMCNumNodesRequired
( 
    const NNSG2dMultiCellAnimSequence*   pMultiCellSeq, 
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellSeq );
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );
    NNS_G2D_ASSERTMSG( NNS_G2dGetAnimSequenceAnimType( pMultiCellSeq )
                   == NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION, 
                    "NNSG2dAnimationType must be MultiCellLocation." );
    
    {   
        const NNSG2dMultiCellData*  pMCell        = NULL;
        u16                         maxNumNode    = 0;
        
        //
        // Searches for the maximum NNSG2dNode number in the multi-cell series constituting the animation
        // 
        u16     i;
        const NNSG2dAnimDataSRT*    pAnmFrm;
        for( i = 0; i < pMultiCellSeq->numFrames; i++ )
        {
            pAnmFrm = (const NNSG2dAnimDataSRT*)
               pMultiCellSeq->pAnmFrameArray[i].pContent;
            
            
            pMCell = NNS_G2dGetMultiCellDataByIdx( pMultiCellDataBank, 
                                                   pAnmFrm->index );
            NNS_G2D_NULL_ASSERT( pMCell );
            
            
            if( pMCell->numNodes > maxNumNode )
            {
                maxNumNode = pMCell->numNodes;
            }
        }
        return maxNumNode;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetMCWorkAreaSize

  Description:  Acquires the size of the work memory that is required to initialize an instance of a multi-cell.
                NNS_G2dGetMCNumNodesRequired
                NNS_G2dGetMCBankNumNodesRequired
                NNS_G2dGetMCBankNumCellAnimsRequired
                In place of these functions, it acquires the required work size in bytes.
                
  Arguments:    pMultiCellDataBank:     [IN]   Multicell databank 
                mcType:                 [IN] Type of multicell instance
                
  Returns:      Maximum number of NNSG2dNode required in a multi-cell data bank
  
 *---------------------------------------------------------------------------*/
u32 NNS_G2dGetMCWorkAreaSize
(
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank,
    NNSG2dMCType                         mcType
)
{
    if( mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       return sizeof( NNSG2dMCCellAnimation ) 
           * GetMCBankNumCellAnimRequired_( pMultiCellDataBank );
    }else{
       return ( sizeof( NNSG2dNode ) + sizeof( NNSG2dCellAnimation ) ) 
           * NNS_G2dGetMCBankNumNodesRequired( pMultiCellDataBank );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetMCBankNumNodesRequired

  Description:  Gets the maximum number of NNSG2dNode needed in NNSG2dMultiCellDataBank
                
                Because the new initialization API NNS_G2dInitMCAnimationInstance() was added, 
                there is no need to use this function any longer. 
                
  Arguments:    pMultiCellDataBank:     [IN]   Multicell databank 
                
                
  Returns:      Maximum number of NNSG2dNode required in a multi-cell data bank
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dGetMCBankNumNodesRequired
( 
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellDataBank );    
    {   
        const NNSG2dMultiCellData*  pMCell        = NULL;
        u16                         maxNumNode    = 0;
        u16                         i;
        
        for( i = 0; i < pMultiCellDataBank->numMultiCellData; i++ )
        {
            pMCell = NNS_G2dGetMultiCellDataByIdx( pMultiCellDataBank, i );
            NNS_G2D_NULL_ASSERT( pMCell );
            
            if( pMCell->numNodes > maxNumNode )
            {
                maxNumNode = pMCell->numNodes;
            }
        }
        return maxNumNode;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickMCInstance

  Description:  Advances the time for the elements (such as cell animation) constituting the multi-cell
                
  Arguments:    pMultiCellAnim:     [OUT]  The multicell instance 
                frames:             [IN] The time to advance (in frames)
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dTickMCInstance( NNSG2dMultiCellInstance* pMCellInst, fx32 frames )
{

    if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       u16 i;
       NNSG2dMCCellAnimation* pCellAnimArray = pMCellInst->pCellAnimInstasnces;
       
       for( i = 0; i < pMCellInst->pCurrentMultiCell->numCellAnim; i++ )
       {
           NNS_G2dTickCellAnimation( &pCellAnimArray[i].cellAnim, frames );
       }
    }else{    
       u16 i;
       NNSG2dNode*   pNodeArray = NULL;
        
       NNS_G2D_NULL_ASSERT( pMCellInst );
       NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );
        
       pNodeArray = (NNSG2dNode*)(pMCellInst->pCellAnimInstasnces);
        
       for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
       {
            
           // Currently only NNS_G2D_NODETYPE_CELL is supported.
           NNS_G2D_ASSERT( pNodeArray[i].type == NNS_G2D_NODETYPE_CELL );
           NNS_G2dTickCellAnimation( (NNSG2dCellAnimation*)pNodeArray[i].pContent, frames );
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickMCAnimation

  Description:  Advances the time of NNSG2dMultiCellAnimation
                
  Arguments:    pMultiCellAnim:     [OUT]  The multicell instance 
                frames:             [IN] The time to advance (in frames)
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dTickMCAnimation( NNSG2dMultiCellAnimation* pMultiCellAnim, fx32 frames )
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    
    {
        const u16 currentAnimFrameFrames = pMultiCellAnim->animCtrl.pCurrent->frames;
        
        if( NNS_G2dTickAnimCtrl( &pMultiCellAnim->animCtrl, frames ) )
        {
           pMultiCellAnim->totalVideoFrame += currentAnimFrameFrames;
           //
           // Reflects the animation results if there has been a Track update
           //
           ApplyCurrentAnimResult_( pMultiCellAnim );
        }else{
           // Updates the cell animation in the multicell.  
           // Note that this is not executed in the frame where a switch of the multicell animation frame occurs. 
           // 
           NNS_G2dTickMCInstance( &pMultiCellAnim->multiCellInstance, frames );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetMCAnimationCurrentFrame

  Description:  Sets the playback animation frame for NNS_G2dSetMCAnimation
                
  Arguments:    pMultiCellAnim:     [OUT]  The multicell instance 
                frameIndex:         [IN] Set frame number
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetMCAnimationCurrentFrame
( 
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    u16                         frameIndex 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    
    if( NNS_G2dSetAnimCtrlCurrentFrame( &pMultiCellAnim->animCtrl, frameIndex ) )
    {
        //
        // Reflects the animation results if there has been a Track update
        //
        ApplyCurrentAnimResult_( pMultiCellAnim );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetMCAnimationCellAnimFrame

  Description:  
                Configures animation frames for each cell animation that comprises a multicell. 
                
                
                
  Arguments:    pMultiCellAnim:         [OUT]  The multicell instance 
                caFrameIndex:           [IN]  Cell animation frame number 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetMCAnimationCellAnimFrame
( 
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    u16                         caFrameIndex     
)
{
    u16 i;
    NNSG2dMultiCellInstance* pMCInst = NULL;
        
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );            
    //
    // Sets the animation frames for all cell animations that make up a multicell
    // 
    pMCInst = &pMultiCellAnim->multiCellInstance;
    
    // The data structure will differ depending on the parameters at the time of initialization
    if( pMCInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
        NNSG2dMCCellAnimation* pCellAnimArray 
           = (NNSG2dMCCellAnimation*)pMCInst->pCellAnimInstasnces;
        const int numCellAnm = pMCInst->pCurrentMultiCell->numCellAnim;
                      
        for( i = 0; i < numCellAnm; i++ )
        {
           NNSG2dCellAnimation* pCell 
           = &pCellAnimArray[i].cellAnim;
           NNS_G2dSetCellAnimationCurrentFrame( pCell, caFrameIndex );
        }
    }else{
        NNSG2dNode*   pNodeArray   
           = (NNSG2dNode*)(pMCInst->pCellAnimInstasnces);
        const int numCellAnm = pMCInst->pCurrentMultiCell->numNodes;
           
        for( i = 0; i < numCellAnm; i++ )
        {
           NNSG2dCellAnimation* pCell 
           = (NNSG2dCellAnimation*)pNodeArray[i].pContent;
           NNS_G2dSetCellAnimationCurrentFrame( pCell, caFrameIndex );
        }
    }
}




/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetMCAnimationSpeed

  Description:  Sets the playback animation speed for NNS_G2dSetMCAnimation
                
  Arguments:    pMultiCellAnim:     [OUT]  The multicell instance 
                speed       :              [IN]  Animation speed 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetMCAnimationSpeed
(
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    fx32                        speed  
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    {
       NNSG2dMultiCellInstance* pMCellInst     = &pMultiCellAnim->multiCellInstance;
        
       if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
       {
           u16 i;
           
           NNSG2dMCCellAnimation*     pCellAnimArray = pMCellInst->pCellAnimInstasnces;
           NNS_G2dSetAnimCtrlSpeed( &pMultiCellAnim->animCtrl, speed );
           
           for( i = 0; i < pMCellInst->pCurrentMultiCell->numCellAnim; i++ )
           {
              NNS_G2dSetCellAnimationSpeed( &pCellAnimArray[i].cellAnim, speed );
           }
           
       }else{    
           u16 i = 0;
           NNSG2dNode*              pNode      = (NNSG2dNode*)pMultiCellAnim->multiCellInstance.pCellAnimInstasnces;
           
           NNS_G2D_NULL_ASSERT( pMultiCellAnim );
            
            
           NNS_G2dSetAnimCtrlSpeed( &pMultiCellAnim->animCtrl, speed );
            
           // Cell animation speed also changes.
           for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
           {
              // Currently only NNS_G2D_NODETYPE_CELL is supported.
              NNS_G2D_ASSERT( pNode[i].type == NNS_G2D_NODETYPE_CELL );
              NNS_G2dSetCellAnimationSpeed( (NNSG2dCellAnimation*)pNode[i].pContent, speed );
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMakeSimpleMultiCellToOams

  Description:  Sets the cell animation play sequence in a multi-cell animation to zero.
                
  Arguments:    pMultiCellAnim:      [OUT]  The multicell instance 
                speed       :               [IN]  Animation speed 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetMCCellAnimationAll
(
    NNSG2dMultiCellInstance*    pMCInst 
)
{
    u16 i = 0;
    NNS_G2D_NULL_ASSERT( pMCInst );
    {
       if( pMCInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
       {
           NNSG2dMCCellAnimation*     pCellAnimArray = pMCInst->pCellAnimInstasnces;
           
           for( i = 0; i < pMCInst->pCurrentMultiCell->numCellAnim; i++ )
           {
              NNS_G2dSetCellAnimationCurrentFrame( &pCellAnimArray[i].cellAnim, 0 );
           }
           
       }else{    
           
           NNSG2dNode*              pNode      = (NNSG2dNode*)pMCInst->pCellAnimInstasnces;
                 
           
           // Cell animation speed also changes.
           for( i = 0; i < pMCInst->pCurrentMultiCell->numNodes; i++ )
           {
              // Currently only NNS_G2D_NODETYPE_CELL is supported.
              NNS_G2D_ASSERT( pNode[i].type == NNS_G2D_NODETYPE_CELL );
              NNS_G2dSetCellAnimationCurrentFrame( (NNSG2dCellAnimation*)pNode[i].pContent, 0 );
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMakeSimpleMultiCellToOams

  Description:  Generates OAM information.
                The CellAnimation ( MultiCellAnimation ) that constitutes a MultiCell
                can only run in a simple MultiCell instance that does not contain affine transformation.
  
                If an illegal affine transformation is found in a MultiCell, a warning will be displayed,
                the transformation will be ignored, and processing will continue.
                (Therefore, rendering results will not reflect it.)
                 
                The entire MultiCell can be Affine converted.              
                If that is done, though, OBJ with flip effects will not be drawn correctly.  
                
                
                
  Arguments:    pDstOams:             [IN]  Result buffer
                numDstOams:             [IN]  Result buffer length
                pMCellInst:             [OUT]  The multicell instance 
                pMtxSR:             [IN] affine matrix (3D format)
                pBaseTrans:             [IN]  Parallel translation component 
                affineIndex :             [IN] Affine Index referenced by the multi-cell constituent Oam
                bDoubleAffine :            [IN]  Double affine?
                
                
  Returns:      Number of result buffers actually used
  
 *---------------------------------------------------------------------------*/
 
u16 NNS_G2dMakeSimpleMultiCellToOams
( 
    GXOamAttr*                      pDstOams, 
    u16                             numDstOams,
    const NNSG2dMultiCellInstance*  pMCellInst, 
    const MtxFx22*                  pMtxSR, 
    const NNSG2dFVec2*              pBaseTrans,
    u16                             affineIndex,
    BOOL                            bDoubleAffine 
)
{
    if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
        return MakeSimpleMultiCellToOams_ShareCellAnims_( pDstOams, 
                              numDstOams, 
                              pMCellInst, 
                              pMtxSR, 
                              pBaseTrans, 
                              affineIndex, 
                              bDoubleAffine );
    }else{
        return MakeSimpleMultiCellToOams_( pDstOams, 
                                    numDstOams, 
                                    pMCellInst, 
                                    pMtxSR, 
                                    pBaseTrans, 
                                    affineIndex, 
                                    bDoubleAffine );
    }    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dMakeSimpleMultiCellToOams

  Description:  Cycles through the cell animations that make up a multicell and invokes callback functions using each cell animation as an argument. 


                
                
  Arguments:    pMCellInst:             [IN]  The multicell instance 
                pCBFunc     :             [IN] Callback functions called for each cell animation in the multicell 

                userParamater :           [IN] Parameter used freely by the user that is passed as the first argument of the callback function. 

                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
 
void NNS_G2dTraverseMCCellAnims
( 
    NNSG2dMultiCellInstance*         pMCellInst,
    NNSG2dMCTraverseCellAnimCallBack pCBFunc,
    u32                              userParamater
)
{
    BOOL                         bContinue = TRUE;
    if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
    {
       u16 i;
       NNSG2dMCCellAnimation* pCellAnimArray = pMCellInst->pCellAnimInstasnces;
       
       for( i = 0; i < pMCellInst->pCurrentMultiCell->numCellAnim; i++ )
       {
           bContinue = (*pCBFunc)( userParamater, &pCellAnimArray[i].cellAnim, i );
           
           if( !bContinue )
           {
              break;
           }
       }
    }else{    
       u16 i;
       NNSG2dNode*   pNodeArray = NULL;
        
       NNS_G2D_NULL_ASSERT( pMCellInst );
       NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );
        
       pNodeArray = (NNSG2dNode*)(pMCellInst->pCellAnimInstasnces);
        
       for( i = 0; i < pMCellInst->pCurrentMultiCell->numNodes; i++ )
       { 
           // Currently only NNS_G2D_NODETYPE_CELL is supported.
           NNS_G2D_ASSERT( pNodeArray[i].type == NNS_G2D_NODETYPE_CELL );
           bContinue = (*pCBFunc)( userParamater, (NNSG2dCellAnimation*)pNodeArray[i].pContent, i );           
           
           if( !bContinue )
           {
              break;
           }
       }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTraverseMCNodes

  Description:  Cycles through the nodes that make up a multicell and invokes callback functions using each node, cell animation, and node number as an argument. 


                
                
  Arguments:    pMCellInst:             [IN]  The multicell instance 
                pCBFunc     :             [IN] Callback functions called for each cell animation in the multicell 
                userParamater :             [IN] Parameter used freely by the user that is passed as the first argument of the callback function. 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
 
void NNS_G2dTraverseMCNodes
( 
    NNSG2dMultiCellInstance*        pMCellInst,
    NNSG2dMCTraverseNodeCallBack    pCBFunc,
    u32                             userParamater
)
{
    NNS_G2D_NULL_ASSERT( pMCellInst );
    NNS_G2D_NULL_ASSERT( pMCellInst->pCurrentMultiCell );    
    {
        u16 i;
        const NNSG2dMultiCellData*   pMCData = pMCellInst->pCurrentMultiCell;
        const u16                    numNode = pMCData->numNodes;
        BOOL                         bContinue = TRUE;
        
        if( pMCellInst->mcType == NNS_G2D_MCTYPE_SHARE_CELLANIM )
        {
           NNSG2dMCCellAnimation* pMCCellAnimArray
               = (NNSG2dMCCellAnimation*)pMCellInst->pCellAnimInstasnces;
           //
           // For each Node
           //
           for( i = 0; i < numNode; i++ )
           {
               const NNSG2dMultiCellHierarchyData* pNodeData = &pMCData->pHierDataArray[i];
               // Cell animation numbers associated with a node are stored in pNode->nodeAttr
               const u16 cellAnimIdx  = NNSi_G2dGetMC2NodeCellAinmIdx( pNodeData );
                
               // Callback call
               bContinue = (*pCBFunc)( userParamater,
                                       pNodeData,                                 // Node data
                                       &pMCCellAnimArray[cellAnimIdx].cellAnim,   // Cell animation
                                       i );                                       // node number
               if( !bContinue )
               {
                  break;
               }
           }
        }else{ 
           const NNSG2dNode* pNode 
               = (const NNSG2dNode*)pMCellInst->pCellAnimInstasnces;
           //
           // For each Node
           //
           for( i = 0; i < numNode; i++ )
           {
               const NNSG2dMultiCellHierarchyData* pNodeData = &pMCData->pHierDataArray[i];
               // pNode->animSequenceIdx
               // Cell animation numbers associated with a node are stored in pNode->nodeAttr
               //
               //
               // Callback call
               bContinue = (*pCBFunc)( userParamater,
                                       pNodeData,           // Node data
                                       pNode[i].pContent,   // Cell animation
                                       i );                 // node number
               
               if( !bContinue )
               {
                  break;
               }
           }
        }
    }
}
//------------------------------------------------------------------------------
//
// This function has been changed to a private function.
// The old API is left here for compatibility with previous versions.
//
void NNS_G2dInitMCAnimation( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank 
)
{
    InitMCAnimation_( pMultiCellAnim, 
                      pNodeArray, 
                      pCellAnim, 
                      numNode, 
                      pAnimBank, 
                      pCellDataBank, 
                      pMultiCellDataBank );
}
//
// This function has been changed to a private function.
// The old API is left here for compatibility with previous versions.
//
void NNS_G2dInitMCInstance
( 
    NNSG2dMultiCellInstance*           pMultiCell, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank 
)
{
    InitMCInstance_
    ( 
       pMultiCell, 
       pNodeArray, 
       pCellAnim, 
       numNode, 
       pAnimBank,  
       pCellDataBank 
    );
}


//------------------------------------------------------------------------------
//
// This function has been changed to a private function.
// The old API is left here for compatibility with previous versions.
//
BOOL NNS_G2dSetMCDataToMCInstance
( 
    NNSG2dMultiCellInstance*       pMCInst, 
    const NNSG2dMultiCellData*     pMcData 
)
{
    SetMCDataToMCInstanceImpl_( pMCInst, pMcData, 0 );

    // To maintain API compatibility
    // this has no substantial meaning.     
    return TRUE;
}



//------------------------------------------------------------------------------



