/*---------------------------------------------------------------------------*
  Project:  NNS - 2D
  File:     g2d_Cell_data.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Cell_data.h,v $
  Revision 1.24  07/07/2005 06:59:21 AM  kitani_toshikazu
  Expansion of user-extended attributes 

  Revision 1.23  06/30/2005 04:22:26 AM  kitani_toshikazu
  Moved position of NNS_G2D_LIMIT_CELL_X_128 note 

  Revision 1.22  06/29/2005 01:01:03 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.21  05/20/2005 08:48:43 AM  hatamoto_minoru
  Revised copyright year

  Revision 1.20  04/28/2005 02:34:18 AM  kitani_toshikazu
  Revised NNS_G2dRepeatXinCellSpace return range

  Revision 1.19  12/17/2004 07:49:59 AM  kitani_toshikazu
  Defined assert when referencing tools

  Revision 1.18  10/19/2004 01:41:29 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.17  10/18/2004 12:29:06 PM  kitani_toshikazu
  Name change: NNS_G2dGetSizeReqiredVramTransferCellDataBank =>
         NNS_G2dGetSizeRequiredVramTransferCellDataBank

  Revision 1.16  10/18/2004 09:47:44 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.15  10/08/2004 04:41:28 AM  kitani_toshikazu
  Changed function names.

  Revision 1.14  10/07/2004 12:58:49 PM  kitani_toshikazu
  Supported cell rectangle region information.
  Added NNS_G2dRepeatXinCellSpace(), etc.

  Revision 1.13  07/29/2004 12:34:20 AM  kitani_toshikazu
  Changed function names.

  Revision 1.12  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.11  07/17/2004 06:54:29 AM  kitani_toshikazu
  Renamed Functions

  Revision 1.10  07/16/2004 07:25:26 AM  kitani_toshikazu
  Changed part of externally opened function to open in library.

  Revision 1.9  07/14/2004 12:30:24 PM  kitani_toshikazu
  Changed version information
  Support for Vram transfer animation.

  Revision 1.8  07/02/2004 12:02:28 AM  kitani_toshikazu
  Added structure definition for VRAM transfer information.

  Revision 1.7  06/23/2004 12:19:26 AM  kitani_toshikazu
  Changed method of calculating boundary sphere radius information.

  Revision 1.6  06/22/2004 11:21:52 AM  kitani_toshikazu
  Reflected format change.

  Revision 1.5  06/22/2004 06:32:49 AM  kitani_toshikazu
  Added boundary sphere information.

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
#ifndef NNS_G2D_CELL_DATA_H_
#define NNS_G2D_CELL_DATA_H_

#include <nitro/types.h>
#include <nnsys/g2d/fmt/g2d_Character_data.h>// Cell definition depends on the character mapping data
#include <nnsys/g2d/fmt/g2d_Common_data.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Binary File Identifier Related
#define NNS_G2D_BINFILE_SIG_CELL          (u32)'NCER'
#define NNS_G2D_BLKSIG_CELLBANK           (u32)'CEBK'

#define NNS_G2D_USEREXBLK_CELLATTR        (u32)'UCAT'


//------------------------------------------------------------------------------
// Binary File Extensions
#define NNS_G2D_BINFILE_EXT_CELL          "NCER"

//------------------------------------------------------------------------------



// Shift amount for referencing inside NNSG2dCellData.bUseFlipHV.
#define NNS_G2D_CELL_FLIP_H_SHIFT           (0 + 8) 
#define NNS_G2D_CELL_FLIP_V_SHIFT           (1 + 8)
#define NNS_G2D_CELL_FLIP_HV_SHIFT          (2 + 8)

#define NNS_G2D_CELL_HAS_BR_SHIFT           (3 + 8)

#define NNS_G2D_CELL_BS_R_MASK              0x3F        // 6 bit
#define NNS_G2D_CELL_BS_R_OFFSET            (2)
#define NNS_G2D_CELL_BS_R_SHIFT             (0)

#ifndef NNS_G2D_LIMIT_CELL_X_128
    #define NNS_G2D_CELL_MAX_X                  255
#else // NNS_G2D_LIMIT_CELL_X_128
    #define NNS_G2D_CELL_MAX_X                  127
#endif // NNS_G2D_LIMIT_CELL_X_128

#define NNS_G2D_CELL_MAX_Y                  127

//------------------------------------------------------------------------------
// Bit data definitions inside NNSG2dCellDataBank.cellBankAttr
#define NNS_G2D_CELLBK_ATTR_CELLWITHBR       0x1 // & with NNSG2dCellDataBank.cellBankAttr
                                                 // If cell data is
                                                 // NNSG2dCellDataWithBR, 1
                                                 // If it is NNSG2dCellDataWithBR, 0

//
// Version information
// Ver         Changed
// -------------------------------------
// 1.0         Extend Vram transfer functionality
//
#define NNS_G2D_NCER_MAJOR_VER              1
#define NNS_G2D_NCER_MINOR_VER              0       



//------------------------------------------------------------------------------
// Changed function name: Defined old name as a separate name for compatibility.
#define NNS_G2dGetSizeReqiredVramTransferCellDataBank   NNS_G2dGetSizeRequiredVramTransferCellDataBank   

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellVramTransferData

  Description:  Information related to the Vram transfer required to display 1 cell.
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellVramTransferData
{
    u32        srcDataOffset;
    u32        szByte;
    
}NNSG2dCellVramTransferData;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dVramTransferData

  Description:  Information related to Vram transfer required to display cells.
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dVramTransferData
{
    u32                                 szByteMax;             // Maximum byte count during all VRAM transfers 
    NNSG2dCellVramTransferData*         pCellTransferDataArray;// NNSG2dCellDataBank that is saved
                                                               // Length of numCells in NNSG2dCellDataBank. 
    
}NNSG2dVramTransferData;




/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellOAMAttrData

  Description:  OAM attribute that defines cells
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellOAMAttrData
{
    u16         attr0;
    u16         attr1;
    u16         attr2;

}NNSG2dCellOAMAttrData;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellData

  Description:  Defines cell bounding information (rectangle)
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellBoundingRectS16
{
    s16                       maxX;
    s16                       maxY;
    s16                       minX;
    s16                       minY;
    
}NNSG2dCellBoundingRectS16; 

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellData

  Description:  Structure that defines cells
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellData
{
    u16                       numOAMAttrs;      // Number of OAM attributes
    u16                       cellAttr;         // Information related to cell type
                                                // Information related to flips (upper 8 bits)
                                                // Information related to bounding information
                                                // 
                                                // Information related to rendering optimization (lower 8 bits)
                                                //      (6Bit => Radius of bounding sphere  >> 2 )
                                                
    
    NNSG2dCellOAMAttrData*    pOamAttrArray;    // Pointer to the NNSG2dCellOAMAttrData array

}NNSG2dCellData;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellDataWithBR
  
  Description:  Structure that defines cell (holding NNSG2dCellBoundingRect as bounding information)
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellDataWithBR
{
    NNSG2dCellData              cellData;
    NNSG2dCellBoundingRectS16   boundingRect;
    
}NNSG2dCellDataWithBR;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellDataBank

  Description:  Structure for collecting multiple cell data
                Normally NNSG2dCellDataBank == binary file (NCER)
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellDataBank
{
    u16                              numCells;               // Total number of cells
    u16                              cellBankAttr;           // Attribute
    NNSG2dCellData*                  pCellDataArrayHead;     // offset address
    NNSG2dCharacterDataMapingType    mappingMode;            // Mapping mode of the referenced character
    
    NNSG2dVramTransferData*          pVramTransferData;      // Information related to Vram transfer animation
    
    void*                            pStringBank;            // Character string bank
    void*                            pExtendedData;          // offset addr (if it exists.)
    
    
}NNSG2dCellDataBank;

/*---------------------------------------------------------------------------*
  Name:         NNSG2dCellDataBankBlock

  Description:  Binary block where NNSG2dCellDataBank is stored
  
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dCellDataBankBlock
{
    NNSG2dBinaryBlockHeader     blockHeader;
    NNSG2dCellDataBank          cellDataBank;
    
}NNSG2dCellDataBankBlock;


//------------------------------------------------------------------------------
typedef struct NNSG2dUserExCellAttr
{
    u32*           pAttr;
    
}NNSG2dUserExCellAttr;

typedef struct NNSG2dUserExCellAttrBank
{
    u16                      numCells; // 
    u16                      numAttribute; // Attribute count: Currently fixed at 1 
    NNSG2dUserExCellAttr*    pCellAttrArray;   
    
}NNSG2dUserExCellAttrBank;



//------------------------------------------------------------------------------
// Inline functions
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSetCellBankHasBR

  Description:  Acquires whether the cell bank has cells that have a cell-bounding rectangle
                << << This function is used by the converter. >>
                
  Arguments:    pCellBank:           Cell bank information
                
                
  Returns:      Whether the cell bank has a bounding rectangle
  
 *---------------------------------------------------------------------------*/
static inline void NNSi_G2dSetCellBankHasBR(  NNSG2dCellDataBank* pCellBank )
{
    NNS_G2D_NULL_ASSERT( pCellBank );
    pCellBank->cellBankAttr |= NNS_G2D_CELLBK_ATTR_CELLWITHBR;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dCellBankHasBR

  Description:  Acquires whether the cell bank has cells that have a cell-bounding rectangle
                
  Arguments:    pCellBank:           Cell bank information
                
                
  Returns:      Whether the cell bank has a bounding rectangle
  
 *---------------------------------------------------------------------------*/
static inline BOOL NNSi_G2dCellBankHasBR(  const NNSG2dCellDataBank* pCellBank )
{
    return (BOOL)( pCellBank->cellBankAttr & NNS_G2D_CELLBK_ATTR_CELLWITHBR );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSetCellAttrFlipFlag

  Description:  Sets and returns a u16 variable that holds flip utilization setting information.
                
  Arguments:    bH:         H flip
                bV:         V flip
                bHV:        HV flip
                
  Returns:      The u16 variable that holds information
  
 *---------------------------------------------------------------------------*/
static inline u16 NNSi_G2dSetCellAttrFlipFlag( BOOL bH, BOOL bV, BOOL bHV )
{
    return (u16)((bH  << NNS_G2D_CELL_FLIP_H_SHIFT) |
                 (bV  << NNS_G2D_CELL_FLIP_V_SHIFT) |
                 (bHV << NNS_G2D_CELL_FLIP_HV_SHIFT)  );
}
/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSetCellAttrHasBR

  Description:  Sets and returns a u16 variable that sets whether a cell has a bounding rectangle.
                << << This function is used by the converter. >>
                
  Arguments:    bHasBR:         Whether the cell has a bounding rectangle
                
                
  Returns:      The u16 variable that holds information
  
 *---------------------------------------------------------------------------*/
static inline u16 NNSi_G2dSetCellAttrHasBR( BOOL bHasBR )
{
    return (u16)( bHasBR << NNS_G2D_CELL_HAS_BR_SHIFT );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dIsCellFlipH

  Description:  Tests whether H flip is being used.
                
  Arguments:    pCell:           Cell data
                
  Returns:      Whether or not it is used
  
 *---------------------------------------------------------------------------*/
static inline BOOL NNSi_G2dIsCellFlipH( const NNSG2dCellData* pCell )
{
    
    return (BOOL)( ( pCell->cellAttr >> NNS_G2D_CELL_FLIP_H_SHIFT) & 0x1);
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dIsCellFlipV

  Description:  Tests whether V flip is being used.
                
  Arguments:    pCell:           Cell data
                
  Returns:      Whether or not it is used
  
 *---------------------------------------------------------------------------*/
static inline BOOL NNSi_G2dIsCellFlipV( const NNSG2dCellData* pCell )
{
    return (BOOL)( (pCell->cellAttr >> NNS_G2D_CELL_FLIP_V_SHIFT) & 0x1);
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dIsCellFlipHV

  Description:  Tests whether HV flip is being used.
                
  Arguments:    pCell:           Cell data
                
  Returns:      Whether or not it is used
  
 *---------------------------------------------------------------------------*/
static inline BOOL NNSi_G2dIsCellFlipHV( const NNSG2dCellData* pCell )
{
    return (BOOL)( (pCell->cellAttr >> NNS_G2D_CELL_FLIP_HV_SHIFT) & 0x1);
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dCellHasBR

  Description:  Acquires whether a cell has a bounding rectangle.
                
  Arguments:    pCell:           Cell data
                
                
  Returns:      Whether the cell has a bounding rectangle
  
 *---------------------------------------------------------------------------*/
static inline BOOL NNS_G2dCellHasBR(  const NNSG2dCellData* pCell )
{
    return (BOOL)( (pCell->cellAttr >> NNS_G2D_CELL_HAS_BR_SHIFT) & 0x1);
}

static inline BOOL NNSi_G2dCellHasBR(  const NNSG2dCellData* pCell )
{
    return NNS_G2dCellHasBR( pCell );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCellBoundingRect

  Description:  Acquires a cells bounding rectangle.
                There is no internal determination of whether or not a cell has a bounding rectangle.
                Use user code to determine this.
                
  Arguments:    pCell:           Cell data
                
                
  Returns:      Cell bounding rectangle
  
 *---------------------------------------------------------------------------*/
static inline const NNSG2dCellBoundingRectS16* 
NNS_G2dGetCellBoundingRect( const NNSG2dCellData* pCell )
{
    // NNS_G2D_ASSERT( NNSi_G2dCellHasBR( pCell ) )
    const NNSG2dCellDataWithBR* pBR = ( const NNSG2dCellDataWithBR* )pCell;
    return &pBR->boundingRect;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSetCellBoundingSphereR

  Description:  Sets a cell's bounding sphere information. (This function is used by a converter.)
                Shift 8-bit bounding sphere radius information 2 bits to the right. Then store in pCell->cellAttr.

                Round up before truncating the lower 2 bits of radius information.
                
                << << This function is used by the converter. >>
                
                
  Arguments:    pCell:           Cell data
                R    :           Bounding sphere radius
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
static inline void NNSi_G2dSetCellBoundingSphereR( NNSG2dCellData* pCell, u8 R )
{
    //
    // Rounding up process
    //
    if( (R & 0x3) != 0 )
    {
        R += 0x1 << NNS_G2D_CELL_BS_R_OFFSET;
        R &= ~0x3;
    }

    R = (u8)(R >> NNS_G2D_CELL_BS_R_OFFSET);
    pCell->cellAttr |= (( R & NNS_G2D_CELL_BS_R_MASK ) << NNS_G2D_CELL_BS_R_SHIFT);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetCellBoundingSphereR
                NNSi_G2dGetCellBoundingSphereR

  Description:  Acquires a cell's bounding sphere information.
                
  Arguments:    pCell:           Cell data
                
                
  Returns:      Bounding sphere radius
  
 *---------------------------------------------------------------------------*/
static inline u8 NNS_G2dGetCellBoundingSphereR( const NNSG2dCellData* pCell )
{
    u8 R =  (u8)( (pCell->cellAttr >> NNS_G2D_CELL_BS_R_SHIFT) & NNS_G2D_CELL_BS_R_MASK );
    return (u8)(R << NNS_G2D_CELL_BS_R_OFFSET);
}

static inline u8 NNSi_G2dGetCellBoundingSphereR( const NNSG2dCellData* pCell )
{
    // TODO: Display a warning.
    return NNS_G2dGetCellBoundingSphereR( pCell );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCellDataBankHasVramTransferData

  Description:  Checks whether a cell bank has VRAM transfer information.
                
  Arguments:    pCellBank:           Cell data bank
                
                
  Returns:      Returns TRUE if it has information.
  
 *---------------------------------------------------------------------------*/
static inline BOOL NNS_G2dCellDataBankHasVramTransferData( const NNSG2dCellDataBank* pCellBank )
{
    // NNS_G2D_NULL_ASSERT( pCellBank );
    return (BOOL)( pCellBank->pVramTransferData != NULL );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetSizeRequiredVramTransferCellDataBank

  Description:  Acquires the VRAM area (bytes) required by a VRAM transfer cell bank.
                Returns 0 if the cell bank has no VRAM transfer information.
                
  Arguments:    pCellBank:           Cell data bank
                
                
  Returns:      Size of the required VRAM area (bytes).
  
 *---------------------------------------------------------------------------*/
static inline u32 NNS_G2dGetSizeRequiredVramTransferCellDataBank( const NNSG2dCellDataBank* pCellBank )
{
    // NNS_G2D_NULL_ASSERT( pCellBank );
    if( NNS_G2dCellDataBankHasVramTransferData( pCellBank ) )
    {
        const NNSG2dVramTransferData* pData = pCellBank->pVramTransferData;
        
        return pData->szByteMax;
    }
    
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetCellVramTransferData

  Description:  Acquires VRAM transfer cell data for a specified cell number.
                
                Will fail with assert if the specified number is greater than or equal to the number of cells.
                Will fail with assert if a cell data bank is specified that does not have VRAM transfer information.
                
  Arguments:    pCellBank:           Cell data bank
                cellIdx     :           Cell number
                
  Returns:      Size of the required VRAM area (bytes).
  
 *---------------------------------------------------------------------------*/
static inline const NNSG2dCellVramTransferData* 
NNSi_G2dGetCellVramTransferData( const NNSG2dCellDataBank* pCellBank, u16 cellIdx )
{
    //( pCellBank );
    //( cellIdx < pCellBank->numCells );
    // ( NNS_G2dCellDataBankHasVramTransferData( pCellBank )
    const NNSG2dVramTransferData*       pTransferData = pCellBank->pVramTransferData;
        
    return &pTransferData->pCellTransferDataArray[cellIdx];
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUserExCellAttrBankFromCellBank

  Description:  Gets the cell extended attribute bank from the cell bank. 
                
                
  Arguments:    pCellBank:           Cell data bank
                
                
  Returns:      Cell extended attribute bank
  
 *---------------------------------------------------------------------------*/
static inline const NNSG2dUserExCellAttrBank* 
NNS_G2dGetUserExCellAttrBankFromCellBank( const NNSG2dCellDataBank* pCellBank )
{
    // Gets the block
    const NNSG2dUserExDataBlock* pBlk 
        = NNSi_G2dGetUserExDataBlkByID( pCellBank->pExtendedData,
                                        NNS_G2D_USEREXBLK_CELLATTR );
    // If successful in obtaining the block... 
    if( pBlk != NULL )
    {
        return (const NNSG2dUserExCellAttrBank*)(pBlk + 1);
    }else{
        return NULL;                                
    }
}
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUserExCellAttr

  Description:  Obtains the cell extended attributes from the cell extended attribute bank, based on the cell number. 
                
                
                
  Arguments:    pCellAttrBank   :           Cell extended attribute bank
                idx         :           Cell number
                
  Returns:      Cell extended attribute 
  
 *---------------------------------------------------------------------------*/
static inline const NNSG2dUserExCellAttr* 
NNS_G2dGetUserExCellAttr
( 
    const NNSG2dUserExCellAttrBank* pCellAttrBank, 
    u16                             idx
)
{
    if( idx < pCellAttrBank->numCells )
    {
        return &pCellAttrBank->pCellAttrArray[idx];
    }else{
        return NULL;    
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUserExCellAttrValue

  Description:  Gets the attribute value from the cell extended attribute. 
                
                
  Arguments:    pCellAttrBank   :           Cell extended attribute 
                
                
  Returns:      Attribute value
  
 *---------------------------------------------------------------------------*/
static inline const u32
NNS_G2dGetUserExCellAttrValue
( 
    const NNSG2dUserExCellAttr* pCellAttr 
)
{
    return pCellAttr->pAttr[0];
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dCopyCellAsOamAttr

  Description:  Copies the Oam attribute information in the cell data to GXOamAttr.
                
  Arguments:    pCell:       Cell data
                idx         :       OAM attribute number
                pDst        :       Copy destination GXOamAttr
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
static inline void NNS_G2dCopyCellAsOamAttr( const NNSG2dCellData* pCell, u16 idx, GXOamAttr* pDst )
{
    //NNS_G2D_NULL_ASSERT( pCell );
    //NNS_G2D_NULL_ASSERT( pDst );
    //NNS_G2D_MINMAX_ASSERT( idx, 0, pCell->numOAMAttrs );
    {
        const NNSG2dCellOAMAttrData* pSrc = pCell->pOamAttrArray + idx;
        
        pDst->attr0 = pSrc->attr0;
        pDst->attr1 = pSrc->attr1;
        pDst->attr2 = pSrc->attr2;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRepeatXinCellSpace

  Description:  Repeats an OBJ in a cell (coordinate wrap).
                The resultant s16 value is transformed to -128 - 127.
                
  Arguments:    srcX       :       Source value
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
static inline  s16 NNS_G2dRepeatXinCellSpace( s16 srcX )
{
    if( srcX > NNS_G2D_CELL_MAX_X )
    { 
        srcX |= 0xFF00; 
    }
    return srcX;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dRepeatYinCellSpace

  Description:  Repeats an OBJ in a cell (coordinate wrap).
                The resultant s16 value is transformed to -128 - 127.
                
  Arguments:    srcY       :       Source value
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
static inline  s16 NNS_G2dRepeatYinCellSpace( s16 srcY )
{   
    if( srcY > NNS_G2D_CELL_MAX_Y )
    { 
        srcY |= 0xFF00; 
    }
    return srcY;
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_CELL_DATA_H_


