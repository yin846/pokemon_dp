/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_MultiCellAnimaiton.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_MultiCellAnimation.h,v $
  Revision 1.20  08/17/2005 10:21:28 AM  kitani_toshikazu
  Changed the argument of the function that calls the multicell callback. 

  Revision 1.19  07/11/2005 01:58:39 AM  kitani_toshikazu
  Added G2dTraverseMCNodes().

  Revision 1.18  07/07/2005 06:41:42 AM  kitani_toshikazu
  Added NNS_G2dTraverseMCCellAnims().

  Revision 1.17  06/23/2005 02:03:27 AM  kitani_toshikazu
  Fixed updating problem in animation with data that specifies NNS_G2D_MCANIM_PLAYMODE_CONTINUE for the node. 
  Fixed problem with incorrect updating of animation. 

  Revision 1.16  05/30/2005 09:22:37 AM  kitani_toshikazu
  Added NNS_G2dSetMCAnimationCellAnimFrame().

  Revision 1.15  02/22/2005 12:39:36 AM  hatamoto_minoru
  Added copyright year.

  Revision 1.14  02/21/2005 02:15:10 AM  hatamoto_minoru
  Revised include file names.

  Revision 1.13  12/14/2004 07:27:21 AM  kitani_toshikazu
  Added NNS_G2dResetMCCellAnimationAll()

  Revision 1.12  12/13/2004 08:24:46 AM  kitani_toshikazu
  Merged branch g2d_multiCell

  Revision 1.11.4.2  2004/12/13 05:33:40  kitani_toshikazu
  (none)

  Revision 1.11.4.1  2004/12/13 02:53:29  kitani_toshikazu
  Merged the new multicell module

  Revision 1.11  11/15/2004 08:32:09 AM  kitani_toshikazu
  Added NNS_G2dGetMCBankNumNodesRequired()

  Revision 1.10  10/18/2004 09:47:44 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.9  07/29/2004 12:34:20 AM  kitani_toshikazu
  Changed function names.

  Revision 1.8  07/28/2004 12:11:37 AM  kitani_toshikazu
  Correspondence with the padding warning

  Revision 1.7  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.6  07/17/2004 06:50:58 AM  kitani_toshikazu
  Changed name from NNS_G2dSetMultiCellDataToMCInstance()
  to NNS_G2dSetMCDataToMCInstance().
  Changed name from NNS_G2dGetNumNodesRequired()
  to NNS_G2dGetMCNumNodesRequired().

  Revision 1.5  05/28/2004 10:50:52 AM  kitani_toshikazu
  Added accessor to NNSG2dAnimController

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
#ifndef NNS_G2D_MULTICELLANIMATION_H_
#define NNS_G2D_MULTICELLANIMATION_H_

#include <nitro.h>
#include <nnsys/g2d/fmt/g2d_MultiCell_data.h>

#include <nnsys/g2d/g2d_Node.h>
#include <nnsys/g2d/g2d_Animation.h>
#include <nnsys/g2d/g2d_CellAnimation.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
// Exactly the same format, but defined with separate name for ease of understanding.
typedef NNSG2dAnimSequence            NNSG2dMultiCellAnimSequence;
typedef NNSG2dAnimBankData            NNSG2dMultiCellAnimBankData;


//
// This function is called while scanning all cell animations in a multicell. 
// If the callback invocation is cancelled, returns FALSE. 
//
typedef BOOL (*NNSG2dMCTraverseCellAnimCallBack)
( 
    u32                   userParamater,
    NNSG2dCellAnimation*  pCellAnim, 
    u16                   cellAnimIdx 
);    
//
// This function is called while scanning all cell animations in a multicell.
// If the callback invocation is cancelled, returns FALSE. 
//
typedef BOOL (*NNSG2dMCTraverseNodeCallBack)
( 
    u32                                 userParamater,
    const NNSG2dMultiCellHierarchyData* pNodeData,
    NNSG2dCellAnimation*                pCellAnim, 
    u16                                 nodeIdx 
);    


//
// aliases of functions with names changed
// Previous functions declared as aliases to preserve compatibility.
// 
#define NNS_G2dInitializeMCAnimation       NNS_G2dInitMCAnimation
#define NNS_G2dInitializeMCInstance        NNS_G2dInitMCInstance
#define NNS_G2dSetMCAnimSpeed              NNS_G2dSetMCAnimationSpeed

typedef enum 
{
    NNS_G2D_MCTYPE_DONOT_SHARE_CELLANIM,
    NNS_G2D_MCTYPE_SHARE_CELLANIM
    
}NNSG2dMCType;

typedef struct NNSG2dMCNodeArray
{
    NNSG2dNode*                         pNodeArray;         // Node array
    u16                                 numNode;            // Number of Node
    u16                                 pad16_;             // The padding
    
}NNSG2dMCNodeArray;

typedef struct NNSG2dMCCellAnimation
{
    NNSG2dCellAnimation     cellAnim;
    BOOL                    bInited;
    
}NNSG2dMCCellAnimation;


typedef struct NNSG2dMCNodeCellAnimArray
{
    NNSG2dMCCellAnimation*       cellAnimArray;
    
}NNSG2dMCNodeCellAnimArray;

//------------------------------------------------------------------------------
// Note: pCurrentMultiCell must satisfy 
//       ( numNode > pCurrentMultiCell->numNodes ).
//       However, in most cases, numNode == pCurrentMultiCell->numNodes.
//
//       Alternate concept of the previous version of ComposedObj.
typedef struct NNSG2dMultiCellInstance 
{
    const NNSG2dMultiCellData*          pCurrentMultiCell;  // Multi cell data
    
    const NNSG2dCellAnimBankData*       pAnimDataBank;      // Make MultiCell.
                                                            // NNSG2dCellAnimation is defined.
                                                            // NNSG2dAnimBankData
    
    //
    // The structure of the internal data entity is different depending on the initialization condition.
    // It is either NNSG2dMCNodeArray or NNSG2dMCNodeCellAnimArray
    //
    //
    NNSG2dMCType                        mcType;
    void*                               pCellAnimInstasnces;
    
    /*
    NNSG2dNode*                         pNodeArray;         // Node array
    u16                                 numNode;            // Node number
    u16                                 pad16_;             // padding
    */
    
}NNSG2dMultiCellInstance;



//------------------------------------------------------------------------------
// animCtrl results hold the Index information to the NNSG2dMultiCellData.
// The library gets the pointer to NNSG2dMultiCellData from the database with that Index as the key.
// After that, sets the acquired NNSG2dMultiCellData in NNSG2dMultiCellInstance.
typedef struct NNSG2dMultiCellAnimation
{
    NNSG2dAnimController              animCtrl;             // Animation controller
    
    u16                               totalVideoFrame;           // Total video frame
    u16                               pad16;                // The padding
    
    NNSG2dMultiCellInstance           multiCellInstance;    // multicell instance
    
    const NNSG2dMultiCellDataBank*    pMultiCellDataBank;   // Multi cell data bank
    
    NNSG2dSRTControl                  srtCtrl;              // SRT animation results
    
    
    // TODO:Bounding volume
}NNSG2dMultiCellAnimation;




//------------------------------------------------------------------------------
// initialization related
//------------------------------------------------------------------------------


//------------------ NEW ------------------------------
void NNS_G2dInitMCAnimationInstance
( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim,  
    void*                              pWork, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank,
    NNSG2dMCType                       mcType
);




//------------------------------------------------------------------------------
void NNS_G2dSetAnimSequenceToMCAnimation
( 
    NNSG2dMultiCellAnimation*             pMultiCellAnim, 
    const NNSG2dMultiCellAnimSequence*    pAnimSeq 
);
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
u16 NNS_G2dGetMCNumNodesRequired
( 
    const NNSG2dMultiCellAnimSequence*    pMultiCellSeq, 
    const NNSG2dMultiCellDataBank*        pMultiCellDataBank 
);
u16 NNS_G2dGetMCBankNumNodesRequired
( 
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank 
);
u32 NNS_G2dGetMCWorkAreaSize
(
    const NNSG2dMultiCellDataBank*       pMultiCellDataBank,
    NNSG2dMCType                         mcType
);

//------------------------------------------------------------------------------
// animation related
//------------------------------------------------------------------------------
void NNS_G2dTickMCInstance( NNSG2dMultiCellInstance* pMultiCellAnim, fx32 frames );
void NNS_G2dTickMCAnimation( NNSG2dMultiCellAnimation* pMultiCellAnim, fx32 frames );
//------------------------------------------------------------------------------
void NNS_G2dSetMCAnimationCurrentFrame
( 
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    u16                         frameIndex 
);

//------------------------------------------------------------------------------
// Sets the animation frame of cell animations in multicell
void NNS_G2dSetMCAnimationCellAnimFrame
( 
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    u16                         caFrameIndex     
);

//------------------------------------------------------------------------------
void NNS_G2dSetMCAnimationSpeed
(
    NNSG2dMultiCellAnimation*   pMultiCellAnim, 
    fx32                        speed  
);

void NNS_G2dResetMCCellAnimationAll
(
    NNSG2dMultiCellInstance*    pMCInst 
);


//------------------------------------------------------------------------------
// create OAM information
u16 NNS_G2dMakeSimpleMultiCellToOams
( 
    GXOamAttr*                      pDstOams, 
    u16                             numDstOams,
    const NNSG2dMultiCellInstance*  pMCellInst, 
    const MtxFx22*                  pMtxSR, 
    const NNSG2dFVec2*              pBaseTrans,
    u16                             affineIndex,
    BOOL                            bDoubleAffine 
);

//------------------------------------------------------------------------------
void NNS_G2dTraverseMCCellAnims
( 
    NNSG2dMultiCellInstance*         pMCellInst,
    NNSG2dMCTraverseCellAnimCallBack pCBFunc,
    u32                              userParamater
);
//------------------------------------------------------------------------------
void NNS_G2dTraverseMCNodes
( 
    NNSG2dMultiCellInstance*        pMCellInst,
    NNSG2dMCTraverseNodeCallBack    pCBFunc,
    u32                             userParamater
);


//------------------ OLD ------------------------------
// Old API
// It was kept to maintain compatibility with older versions.
void NNS_G2dInitMCAnimation( 
    NNSG2dMultiCellAnimation*          pMultiCellAnim, 
    NNSG2dNode*                        pNodeArray, 
    NNSG2dCellAnimation*               pCellAnim, 
    u16                                numNode, 
    const NNSG2dCellAnimBankData*      pAnimBank,  
    const NNSG2dCellDataBank*          pCellDataBank,
    const NNSG2dMultiCellDataBank*     pMultiCellDataBank 
);
void NNS_G2dInitMCInstance
( 
    NNSG2dMultiCellInstance*      pMultiCell, 
    NNSG2dNode*                   pNodeArray, 
    NNSG2dCellAnimation*          pCellAnim, 
    u16                           numNode, 
    const NNSG2dCellAnimBankData* pAnimBank,  
    const NNSG2dCellDataBank*     pCellDataBank 
);
BOOL NNS_G2dSetMCDataToMCInstance( 
    NNSG2dMultiCellInstance*       pMCInst, 
    const NNSG2dMultiCellData*     pMcData
);
//------------------ OLD ------------------------------


//------------------------------------------------------------------------------
// inline funcs.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dAnimController* NNS_G2dGetMCAnimAnimCtrl
( 
    NNSG2dMultiCellAnimation* pMultiCellAnim 
)
{
    NNS_G2D_NULL_ASSERT( pMultiCellAnim );
    return &pMultiCellAnim->animCtrl;
}

//
// Obtains the pointer to the internal work memory
//
NNS_G2D_INLINE void* NNSi_G2dGetMCInstanceWorkMemory
(
    NNSG2dMultiCellInstance*      pMultiCell
)
{
    return pMultiCell->pCellAnimInstasnces;
}


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_MULTICELLANIMATION_H_


