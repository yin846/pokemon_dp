/*---------------------------------------------------------------------------*
  Project:  NNS - 2D
  File:     g2d_Anim_data.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Anim_data.h,v $
  Revision 1.15  08/31/2005 08:04:48 AM  kitani_toshikazu
  Fixed support for warning displays in VC build environment. 

  Revision 1.14  07/07/2005 06:59:21 AM  kitani_toshikazu
  Expansion of user-extended attributes 

  Revision 1.13  01/25/2005 01:56:02 AM  kitani_toshikazu
  Corrected comment

  Revision 1.12  01/24/2005 07:04:31 AM  kitani_toshikazu
  Added NNS_G2dCalcAnimSequenceTotalVideoFrames()

  Revision 1.11  12/17/2004 07:49:59 AM  kitani_toshikazu
  Defined assert when referencing tools

  Revision 1.10  10/28/2004 12:09:38 AM  kitani_toshikazu
  Added NNSG2dAnimDataT

  Revision 1.9  10/18/2004 09:47:44 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.8  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.7  07/14/2004 12:30:35 PM  kitani_toshikazu
  Changed version information

  Revision 1.6  2004/06/23 06:47:52 AM  nishida_kenji
  Fix header comments.

  Revision 1.5  06/01/2004 05:20:33 AM  kitani_toshikazu
  Changes for using expansion palette.

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
#ifndef NNS_G2D_ANIM_DATA_H_
#define NNS_G2D_ANIM_DATA_H_





#include <nitro/types.h>
#include <nitro/fx/fx.h>


#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Binary File Identifier Related
#define NNS_G2D_BINFILE_SIG_CELLANIM          (u32)'NANR'
#define NNS_G2D_BINFILE_SIG_MULTICELLANIM     (u32)'NMAR'

#define NNS_G2D_BLKSIG_ANIMBANK               (u32)'ABNK'
#define NNS_G2D_USEREXBLK_ANMATTR             (u32)'UAAT'

//------------------------------------------------------------------------------
// Binary File Extensions
#define NNS_G2D_BINFILE_EXT_CELLANIM          "NANR"
#define NNS_G2D_BINFILE_EXT_MULTICELLANIM     "NMAR"

//
// Version information
// Ver         Changed
// -------------------------------------
// 1.0         Initial
//
#define NNS_G2D_NANR_MAJOR_VER   1
#define NNS_G2D_NANR_MINOR_VER   0


// Use for NNSG2dAnimSequenceData member access
#define NNS_G2D_ANIMTYPE_SHIFT      16
#define NNS_G2D_ANIMTYPE_MASK       0xFF00
#define NNS_G2D_ANIMELEM_MASK       0x00FF


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimationType

  Description:  Animation Type
                NNSG2dAnimSequence maintains.
                (In other words, in one sequence the same NNSG2dAnimationType is guaranteed.)
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dAnimationType
{
    NNS_G2D_ANIMATIONTYPE_INVALID           = 0x0, // Invalid type
    NNS_G2D_ANIMATIONTYPE_CELL                   , // Cell
    NNS_G2D_ANIMATIONTYPE_MULTICELLLOCATION      , // MultiCell
    NNS_G2D_ANIMATIONTYPE_MAX
}
NNSG2dAnimationType;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimationElement

  Description:  Animation Element Type.
                NNSG2dAnimSequence maintains.
                (In other words, in one sequence the same NNSG2dAnimationElement is guaranteed.)
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dAnimationElement
{
    NNS_G2D_ANIMELEMENT_INDEX           = 0x0, // Index only
    NNS_G2D_ANIMELEMENT_INDEX_SRT            , // Index + SRT
    NNS_G2D_ANIMELEMENT_INDEX_T              , // Index + T
    NNS_G2D_ANIMELEMENT_MAX
}
NNSG2dAnimationElement;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimationPlayMode

  Description:  Method for Animation Sequence Playback.
                NNSG2dAnimSequence maintains.
                
                
 *---------------------------------------------------------------------------*/
typedef enum NNSG2dAnimationPlayMode
{
    NNS_G2D_ANIMATIONPLAYMODE_INVALID = 0x0,    // invalid
    NNS_G2D_ANIMATIONPLAYMODE_FORWARD,          // one time playback (forward)
    NNS_G2D_ANIMATIONPLAYMODE_FORWARD_LOOP,     // repeat playback (forward loop)
    NNS_G2D_ANIMATIONPLAYMODE_REVERSE,          // reverse playback (reverse (forward + reverse)
    NNS_G2D_ANIMATIONPLAYMODE_REVERSE_LOOP,     // reverse playback repeat (reverse (forward + reverse) loop)
    NNS_G2D_ANIMATIONPLAYMODE_MAX               
}
NNSG2dAnimationPlayMode;


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimDataSRT

  Description:  Animation Result
                This data is after NNSG2dAnimFrameData.pContent.
                In future, we plan to support multiple formats in addition to animation result NNSG2dAnimDataSRT.  
                
                                
 *---------------------------------------------------------------------------*/
typedef       u16       NNSG2dAnimData; // index only

typedef struct NNSG2dAnimDataSRT        // index + SRT information
{
    u16         index;      // index
    
    u16         rotZ;       // rotation
    
    fx32        sx;         // scale X
    fx32        sy;         // scale Y
    
    s16         px;         // position X
    s16         py;         // position Y
    
}NNSG2dAnimDataSRT;

typedef struct NNSG2dAnimDataT        // index + T information
{
    u16         index;      // index
    u16         pad_;       // rotation

    s16         px;         // position X
    s16         py;         // position Y
    
}NNSG2dAnimDataT;



/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimFrameData

  Description:  Animation Frame
                Unit elements that form animation
                Forms an animation sequence from multiple animation frames. 
                
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimFrameData
{
    void*           pContent;   // pointer to animation result
    u16             frames;     // animation frame continuation time (units: video frames)
    u16             pad16;      // padding
    
}NNSG2dAnimFrameData;





/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimFrameData

  Description:  Animation Sequence
                Combination of continuous animation frames
                It supports animations.
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimSequenceData
{
    u16                     numFrames;         // number of animation frames forming the sequence
    u16                     loopStartFrameIdx; // loop start animation frame number
    
    u32                     animType;          // animation type (upper 16 bits)|
                                               //  animation elements (lower 16 bits)
                                               // (changes after NNSG2dAnimFrameData.pContent)
                                                      
    NNSG2dAnimationPlayMode playMode;          // Method for Animation Sequence Playback.
    NNSG2dAnimFrameData*    pAnmFrameArray;    // offset form the head of pFrameArray.
    
}NNSG2dAnimSequenceData;





/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimBankData

  Description:  Animation Bank
                This is a combination of multiple animation sequences.
                Usually animation bank == animation file.
                
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimBankData
{
    
    u16                       numSequences;         // number of animation sequences
    u16                       numTotalFrames;       // total number of animation frames
    NNSG2dAnimSequenceData*   pSequenceArrayHead;   // pointer to animation sequence array
    NNSG2dAnimFrameData*      pFrameArrayHead;      // pointer to animation frame array
    void*                     pAnimContents;        // pointer to animation result array
    void*                     pStringBank;          // pointer to string bank (set during execution)
    void*                     pExtendedData;        // pointer to library expansion area (not used)

}NNSG2dAnimBankData;






/*---------------------------------------------------------------------------*
  Name:         NNSG2dAnimBankDataBlock

  Description:  Animation Bank Block
                A binary block that includes animation banks.
                
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAnimBankDataBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;    // binary header
    NNSG2dAnimBankData          animBankData;   // Animation Bank
    
}NNSG2dAnimBankDataBlock;

//------------------------------------------------------------------------------
typedef struct NNSG2dUserExAnimFrameAttr
{
    u32*           pAttr;
    
}NNSG2dUserExAnimFrameAttr;

typedef struct NNSG2dUserExAnimSequenceAttr
{
    u16                          numFrames;
    u16                          pad16;
    u32*                         pAttr;    
    NNSG2dUserExAnimFrameAttr*   pAnmFrmAttrArray;
        
}NNSG2dUserExAnimSequenceAttr;

typedef struct NNSG2dUserExAnimAttrBank
{
    u16                           numSequences; // number of animation sequences
    u16                           numAttribute; // Attribute count: Currently fixed at 1 
    NNSG2dUserExAnimSequenceAttr* pAnmSeqAttrArray;   
    
}NNSG2dUserExAnimAttrBank;





//------------------------------------------------------------------------------
// inline functions.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// related to obtaining animation type
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static inline u32 NNSi_G2dMakeAnimType( NNSG2dAnimationType animType, NNSG2dAnimationElement elmType )
{
    return (u32)animType << NNS_G2D_ANIMTYPE_SHIFT | ((u32)elmType & NNS_G2D_ANIMELEM_MASK );
}
//------------------------------------------------------------------------------
static inline NNSG2dAnimationType 
NNSi_G2dGetAnimSequenceAnimType( u32 animType )
{
    return  (NNSG2dAnimationType)( animType >> NNS_G2D_ANIMTYPE_SHIFT );
}
//------------------------------------------------------------------------------
static inline NNSG2dAnimationElement
NNSi_G2dGetAnimSequenceElementType( u32 animType )
{
    return  (NNSG2dAnimationElement)( animType & NNS_G2D_ANIMELEM_MASK );
}

//------------------------------------------------------------------------------
static inline NNSG2dAnimationType 
NNS_G2dGetAnimSequenceAnimType( const NNSG2dAnimSequenceData* pAnimSeq )
{
//    NNS_G2D_NULL_ASSERT( pAnimSeq );
    
    return  NNSi_G2dGetAnimSequenceAnimType( pAnimSeq->animType );
}

//------------------------------------------------------------------------------
static inline NNSG2dAnimationElement
NNS_G2dGetAnimSequenceElementType( const NNSG2dAnimSequenceData* pAnimSeq )
{
//    NNS_G2D_NULL_ASSERT( pAnimSeq );
    
    return  NNSi_G2dGetAnimSequenceElementType( pAnimSeq->animType );
}

//------------------------------------------------------------------------------
// Calculates the total length of video frames in the sequence.
static inline u32 NNS_G2dCalcAnimSequenceTotalVideoFrames
( 
     const NNSG2dAnimSequenceData* pAnimSeq 
)
{
    u32    i;
    u32    total = 0;
    
    for( i = 0; i < pAnimSeq->numFrames; i++ )
    {
        total += pAnimSeq->pAnmFrameArray[i].frames;
    }
    return total;
}
//------------------------------------------------------------------------------
// Gets the animation sequence number from the animation bank. 
// Expects the animation sequence to be included in the animation bank. 
// 
#ifndef NNS_FROM_TOOL
static inline u16 NNS_G2dGetAnimSequenceIndex
( 
    const NNSG2dAnimBankData*     pAnimBank,
    const NNSG2dAnimSequenceData* pAnimSeq 
)
{
    return (u16)(
        ((u32)pAnimSeq - (u32)pAnimBank->pSequenceArrayHead) 
        / (u32)sizeof( NNSG2dAnimSequenceData ) );
}
#else
// Because NNS_G2dGetAnimSequenceIndex() generates a warning in the VC environment, it is not defined. 
#endif

//------------------------------------------------------------------------------
// Gets NNSG2dUserExAnimAttrBank from NNSG2dAnimBankData. 
// If it fails, NULL is returned.
// To output data that includes user extended attributes, you must use g2dcvtr.exe 2.8 or later,
// and output the data specifying the -oua option.  
// 
static inline const NNSG2dUserExAnimAttrBank* 
NNS_G2dGetUserExAnimAttrBank( const NNSG2dAnimBankData* pAnimBank )
{
    // Gets the block
    const NNSG2dUserExDataBlock* pBlk 
        = NNSi_G2dGetUserExDataBlkByID( pAnimBank->pExtendedData,
                                        NNS_G2D_USEREXBLK_ANMATTR );
    // If successful in obtaining the block... 
    if( pBlk != NULL )
    {
        return (const NNSG2dUserExAnimAttrBank*)(pBlk + 1);
    }else{
        return NULL;                                
    }
}

//------------------------------------------------------------------------------
// Gets the extended attribute data of an animation sequence based on the number. 
// If the number is invalid, return NULL. 
//
static inline const NNSG2dUserExAnimSequenceAttr* 
NNS_G2dGetUserExAnimSequenceAttr
( 
    const NNSG2dUserExAnimAttrBank* pAnimAttrBank,
    u16                             idx 
)
{
    NNS_G2D_NULL_ASSERT( pAnimAttrBank );
    
    if( idx < pAnimAttrBank->numSequences )
    {
        return &pAnimAttrBank->pAnmSeqAttrArray[idx];
    }else{
        return NULL;
    }
}

//------------------------------------------------------------------------------
// Gets the extended attribute data of an animation sequence based on the number. 
// If the number is invalid, return NULL. 
//
static inline const NNSG2dUserExAnimFrameAttr* 
NNS_G2dGetUserExAnimFrameAttr
( 
    const NNSG2dUserExAnimSequenceAttr* pAnimSeqAttr,
    u16                                 idx 
)
{
    NNS_G2D_NULL_ASSERT( pAnimSeqAttr );
    
    if( pAnimSeqAttr->numFrames )
    {
        return &pAnimSeqAttr->pAnmFrmAttrArray[idx];            
    }else{
        return NULL;
    }   
}
//------------------------------------------------------------------------------
// Gets the animation sequence's attribute value. 
// 
static inline u32
NNS_G2dGetUserExAnimSeqAttrValue
( 
    const NNSG2dUserExAnimSequenceAttr* pAnimSeqAttr
)
{
    NNS_G2D_NULL_ASSERT( pAnimSeqAttr );
    return pAnimSeqAttr->pAttr[0];
}

//------------------------------------------------------------------------------
// Gets the attribute value of the animation frame. 
// 
static inline u32
NNS_G2dGetUserExAnimFrmAttrValue
( 
    const NNSG2dUserExAnimFrameAttr* pFrmAttr
)
{
    NNS_G2D_NULL_ASSERT( pFrmAttr );
    return pFrmAttr->pAttr[0];
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_ANIM_DATA_H_


