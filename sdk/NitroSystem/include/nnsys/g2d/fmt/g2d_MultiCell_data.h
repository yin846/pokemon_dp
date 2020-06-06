/*---------------------------------------------------------------------------*
  Project:  NNS - 2D
  File:     g2d_MultiCell_data.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_MultiCell_data.h,v $
  Revision 1.12  07/07/2005 06:59:16 AM  kitani_toshikazu
  Added NNSG2dUserExMultiCellAttr()
  Expansion of user-extended attributes 

  Revision 1.11  06/29/2005 01:01:03 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.10  12/21/2004 04:05:02 AM  kitani_toshikazu
  Corrected the problem that did not set the cell animation playback mode properly.
  (NNSi_G2dSetMultiCellNodeAttribute())

  Revision 1.9  12/07/2004 11:58:37 PM  kitani_toshikazu
  Eliminated numTotalOams, and made to use as numCellAnim
  Changed the contents of nodeArttr

  Revision 1.8  10/18/2004 09:47:44 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.7  07/16/2004 07:24:48 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.6  07/14/2004 12:29:15 PM  kitani_toshikazu
  Supported for play method control data.

  Revision 1.5  2004/06/23 06:47:52 AM  nishida_kenji
  Fix header comments.

  Revision 1.4  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.3  05/10/2004 04:42:28 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:55 AM  kitani_toshikazu
  append G2D Lib.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_MULTICELL_DATA_H_
#define NNS_G2D_MULTICELL_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Common_data.h>
#include <nnsys/g2d/fmt/g2d_Anim_data.h>
#include <nnsys/g2d/fmt/g2d_Cell_data.h>


#ifdef __cplusplus
extern "C" {
#endif

#define NNS_G2D_BINFILE_EXT_MULTICELL     "NMCR"

#define NNS_G2D_BINFILE_SIG_MULTICELL          (u32)'NMCR'
#define NNS_G2D_BLKSIG_MULTICELLBANK           (u32)'MCBK'

//
// Version information
// Ver         Changed
// -------------------------------------
// 1.0         Initial
//
#define NNS_G2D_NMCR_MAJOR_VER             (u8)1
#define NNS_G2D_NMCR_MINOR_VER             (u8)0



#define NNS_G2D_MCNODE_PLAYMODE_MASK        0x0F
#define NNS_G2D_MCNODE_PLAYMODE_SHIFT       0
#define NNS_G2D_MCNODE_VISIBILITY_SHIFT     5
#define NNS_G2D_MCNODE_CELLANIMIDX_SHIFT    8
#define NNS_G2D_MCNODE_CELLANIMIDX_MASK     0xFF00

typedef enum NNSG2dMCAnimationPlayMode
{
    NNS_G2D_MCANIM_PLAYMODE_RESET = 0,
    NNS_G2D_MCANIM_PLAYMODE_CONTINUE  = 1,
    NNS_G2D_MCANIM_PLAYMODE_PAUSE = 2,
    NNS_G2D_MCANIM_PLAYMODE_MAX
    
}NNSG2dMCAnimationPlayMode;

//------------------------------------------------------------------------------
typedef struct NNSG2dMultiCellHierarchyData
{
    u16         animSequenceIdx;        // NNSG2dCellAnimation is sequence number to play
    s16         posX;                   // NNSG2dCellAnimation location in MultiCell local system
    s16         posY;                   // NNSG2dCellAnimation location in MultiCell local system
                                        
                                        
    u16         nodeAttr;               // Node attribute ( NNSG2dMCAnimationPlayMode)
                                        // 16 ............ 8 ... 5 .....4 .........................0
                                        //   Cell animation number reserved      visibility   NNSG2dMCAnimationPlayMode
                                        //
    
    
}NNSG2dMultiCellHierarchyData;


//------------------------------------------------------------------------------
// NNSG2dMultiCellData definition data
// data stored in NNSG2dMultiCellAnimation definition file
// referenced by NNSG2dMultiCellInstance
//
typedef struct NNSG2dMultiCellData
{
    u16                             numNodes;
//  u16                             numTotalOams;     // total number of Oam needed to draw MultiCell (not implemented; unusable)
    u16                             numCellAnim;      // Number of cell animation entity needed
    NNSG2dMultiCellHierarchyData*   pHierDataArray;   // playback sequence, location and such (for numNodes)

}NNSG2dMultiCellData;


//------------------------------------------------------------------------------
typedef struct NNSG2dMultiCellDataBank
{
    u16                             numMultiCellData;
    u16                             pad16;
    NNSG2dMultiCellData*            pMultiCellDataArray;
    NNSG2dMultiCellHierarchyData*   pHierarchyDataArray;
    void*                           pStringBank;
    void*                           pExtendedData;        // offset addr (if it exists.)
    // NNSG2dAnimBankData is defined by NNSG2dCellAnimation making up MultiCell.
    // const NNSG2dAnimBankData*     pAnimDataBank; // related AnimBank (set during Unpack with run time)
    //
    // Change: Decided to keep this data with NNSG2dMultiCellInstance (entity side).
    // 
    // When you consider the case of using with NNSG2dMultiCellInstance (runtime entity),
    // pAnimDataBank represents data you want to maintain with NNSG2dMultiCellData, but if you maintain it with NNSG2dMultiCellData:
    //  - The increase in data volume is large. (The number of instances of NNSG2dMultiCellData may become very large.)
    //  - pAnimDataBank data will often be redundant.
    // 
    // If so, hold the data using NNSG2dMultiCellInstance which has NNSG2dMultiCellData.
    // The above defect cannot be completely avoided, but only the portion associated by runtime is highly flexible.
    // (There is a high degree of freedom particularly in the case where multiple NNSG2dMultiCellInstance share NNSG2dMultiCellData.))
    // 
    
    
}NNSG2dMultiCellDataBank;

//------------------------------------------------------------------------------
typedef struct NNSG2dMultiCellDataBankBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dMultiCellDataBank     multiCellDataBank;
    
}NNSG2dMultiCellDataBankBlock;


//------------------------------------------------------------------------------
typedef struct NNSG2dUserExMultiCellAttr
{
    u32*           pAttr;
    
}NNSG2dUserExMultiCellAttr;

typedef struct NNSG2dUserExMultiCellAttrBank
{
    u16                          numMultiCells; // 
    u16                          numAttribute;  // Attribute count: Currently fixed at 1 
    NNSG2dUserExMultiCellAttr*   pMCAttrArray;   
    
}NNSG2dUserExMultiCellAttrBank;



//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Sets multicell node attributes.
// This function is used by converter.
// 
static inline void 
NNSi_G2dSetMultiCellNodeAttribute
( 
    NNSG2dMCAnimationPlayMode    mode, 
    int                          bVisibility,
    u16*                         pDstAttr
)
{
    *pDstAttr = (u16)( ( ( mode & NNS_G2D_MCNODE_PLAYMODE_MASK ) << NNS_G2D_MCNODE_PLAYMODE_SHIFT ) | 
                       ( ( bVisibility & 0x1 ) << NNS_G2D_MCNODE_VISIBILITY_SHIFT ) );
}

//------------------------------------------------------------------------------
// Gets visibility state of node.
static inline BOOL 
NNSi_G2dIsMultiCellNodeVisible
( 
    const NNSG2dMultiCellHierarchyData* pNode 
)
{
    return (BOOL)(( pNode->nodeAttr >> NNS_G2D_MCNODE_VISIBILITY_SHIFT ) & 0x1);
}

//------------------------------------------------------------------------------
// acquires the cell animation play method that is bound to Node
static inline NNSG2dMCAnimationPlayMode 
NNSi_G2dGetMultiCellNodePlayMode
( 
    const NNSG2dMultiCellHierarchyData* pNode 
)
{
    const NNSG2dMCAnimationPlayMode mode 
        = (NNSG2dMCAnimationPlayMode) ( ( pNode->nodeAttr >> NNS_G2D_MCNODE_PLAYMODE_SHIFT ) & 
                                          NNS_G2D_MCNODE_PLAYMODE_MASK );
    // TODO:ASSERT
                                                
    return mode;
}
//------------------------------------------------------------------------------
// Sets the number of cell animation which Node references.
// Function used in the converter.
static inline void NNSi_G2dSetMC2NodeCellAinmIdx
( 
    NNSG2dMultiCellHierarchyData*  pNodeData, 
    u8                             idx 
)
{
    pNodeData->nodeAttr &= ~NNS_G2D_MCNODE_CELLANIMIDX_MASK;
    pNodeData->nodeAttr |= idx << NNS_G2D_MCNODE_CELLANIMIDX_SHIFT;
}

//------------------------------------------------------------------------------
// Obtains the number of cell animation which Node references
static inline u16 
NNSi_G2dGetMC2NodeCellAinmIdx
( 
    const NNSG2dMultiCellHierarchyData*  pNodeData
)
{
    return (u16)((NNS_G2D_MCNODE_CELLANIMIDX_MASK & pNodeData->nodeAttr ) >> NNS_G2D_MCNODE_CELLANIMIDX_SHIFT);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUserExCellAttrBankFromMCBank

  Description:  Gets a cell extended attribute bank from a multicell bank. 
                
                
  Arguments:    pMCBank   :           Multicell bank
                
                
  Returns:      Cell extended attribute bank
  
 *---------------------------------------------------------------------------*/
static inline const NNSG2dUserExCellAttrBank* 
NNS_G2dGetUserExCellAttrBankFromMCBank( const NNSG2dMultiCellDataBank* pMCBank )
{
    // Gets the block
    const NNSG2dUserExDataBlock* pBlk 
        = NNSi_G2dGetUserExDataBlkByID( pMCBank->pExtendedData,
                                        NNS_G2D_USEREXBLK_CELLATTR );
    // If successful in obtaining the block... 
    if( pBlk != NULL )
    {
        return (const NNSG2dUserExCellAttrBank*)(pBlk + 1);
    }else{
        return NULL;                                
    }
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_MULTICELL_DATA_H_


