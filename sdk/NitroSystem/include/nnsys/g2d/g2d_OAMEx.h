/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_OAMEx.h

  Copyright 2004, 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_OAMEx.h,v $
  Revision 1.17  03/09/2005 09:01:04 AM  nishida
  Corrected the year of the copyright date.

  Revision 1.16  03/08/2005 07:03:45 AM  kitani_toshikazu
  Added NNSG2d_SetOamManExDrawOrderType()

  Revision 1.15  12/01/2004 08:39:07 AM  kitani_toshikazu
  Changed the rendering method (method of registering OAM to an external module)

  Revision 1.14  09/15/2004 05:53:06 AM  kitani_toshikazu
  Transferred the affine transformation mode setting process out of the manager

  Revision 1.13  07/28/2004 12:11:37 AM  kitani_toshikazu
  Correspondence with the padding warning

  Revision 1.12  07/26/2004 04:44:53 AM  kitani_toshikazu
  Changed function names.

  Revision 1.11  07/24/2004 02:46:46 AM  kitani_toshikazu
  Deleted NNS_G2dInitAffineManagerEx() declaration.

  Revision 1.10  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.9  06/23/2004 08:52:56 AM  kitani_toshikazu
  Erased tabs

  Revision 1.8  06/23/2004 08:00:34 AM  hatamoto_minoru
  Added numLastFrameDrawn initialization processing.
  Handle OBJs not displayed with time division display.

  Revision 1.7  06/22/2004 06:30:27 AM  kitani_toshikazu
  NNS_G2dSetOamManExDoubleAffineFlag()
  Added NNS_G2dGetOamManExDoubleAffineFlag().

  Revision 1.6  06/21/2004 02:32:03 AM  kitani_toshikazu
  Combined OAM managers Ex2 and Ex.

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
#ifndef NNS_G2D_OAMEX_H_
#define NNS_G2D_OAMEX_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>

#ifdef __cplusplus
extern "C" {
#endif


//
// aliases of functions with names changed
// Previous functions declared as aliases to preserve compatibility.
// 
#define NNS_G2dEntryNewOamEx                   NNS_G2dEntryOamManExOam
#define NNS_G2dEntryNewOamWithAffineIdxEx      NNS_G2dEntryOamManExOamWithAffineIdx
#define NNS_G2dEntryNewAffineParamsEx          NNS_G2dEntryOamManExAffine
#define NNS_G2dApplyToBaseModuleEx             NNS_G2dApplyOamManExToBaseModule

#define NNS_G2dResetOamInstanceEx              NNS_G2dResetOamManExBuffer
#define NNS_G2dGetNewOamInstanceEx             NNS_G2dGetOamManExInstance

#define NNS_G2dSetOamEntryFunctionsEx          NNS_G2dSetOamManExEntryFunctions



//
// HW OAM Affine table index was not initialized 
// NNSG2dAffineParamProxy.affineHWIndex initial value. 
//
#define NNS_G2D_OAMEX_HW_ID_NOT_INIT       0xFFFF


//------------------------------------------------------------------------------
//
// Type of drawing registration order
//
typedef enum NNSG2dOamExDrawOrder
{
    NNSG2D_OAMEX_DRAWORDER_BACKWARD = 0x0, // Register to external module, reversing the order used in registration
    NNSG2D_OAMEX_DRAWORDER_FORWARD  = 0x1  // Register to external module, using the same order used for registration

}NNSG2dOamExDrawOrder;


//------------------------------------------------------------------------------
// Types of Functions Registered
typedef u16     (*NNSG2dGetOamCpacityFuncPtr)(); 
typedef u16     (*NNSG2dGetOamAffineCpacityFuncPtr)(); 
typedef BOOL    (*NNSG2dEntryNewOamFuncPtr)( const GXOamAttr* pOam, u16 index ); 
typedef u16     (*NNSG2dEntryNewOamAffineFuncPtr)( const MtxFx22* mtx, u16 index ); 

typedef struct NNSG2dOamExEntryFunctions
{
    NNSG2dGetOamCpacityFuncPtr        getOamCapacity;
    NNSG2dGetOamCpacityFuncPtr        getAffineCapacity;
    NNSG2dEntryNewOamFuncPtr          entryNewOam;
    NNSG2dEntryNewOamAffineFuncPtr    entryNewAffine;
    
}NNSG2dOamExEntryFunctions;



//------------------------------------------------------------------------------
// Used internally
// OAM attribute control structure
typedef struct NNSG2dOamChunk
{
    
    GXOamAttr               oam;             // OAM attributes
    u16                     affineProxyIdx;  // NNSG2dAffineParamProxy Index value referenced by Oam
    u16                     pad16_;          // The padding
    struct NNSG2dOamChunk*  pNext;           // Link for building list structure.
    
}NNSG2dOamChunk; 

//------------------------------------------------------------------------------
// Concept of combining NNSG2dOamChunk
// The array of this structure is lined up in drawing priority order and becomes the ordering table.
//
// There are two internal NNSG2dOamChunk lists.
// One is for normal OBJ. One is for OBJ that is affine converted.
//
typedef struct NNSG2dOamChunkList
{
    u16                 numChunks;         // Total number of NNSG2dOamChunk(s) in the pChunks list
    u16                 numLastFrameDrawn; // Number of the NNSG2dOamChunk that was registered for rendering in the previous frame.
    
    u16                 numDrawn;          // Number of chunks to render
    u16                 bDrawn;            // Should this ChunkList be registered for rendering?
    
    NNSG2dOamChunk*     pChunks;           // NNSG2dOamChunk list
    NNSG2dOamChunk*     pAffinedChunks;    // Affine transformed NNSG2dOamChunk list
    
    NNSG2dOamChunk*     pLastChunk;        // NNSG2dOamChunk list end
    NNSG2dOamChunk*     pLastAffinedChunk; // affine-transformed NNSG2dOamChunk list end
    
}NNSG2dOamChunkList;


/*---------------------------------------------------------------------------*
  Name:         NNSG2dAffineParamProxy

  Description:  Structure that expresses affine parameters with OAMEx internal buffer.
                
                The initial value of NNSG2dAffineParamProxy.affineHWIndex is NNS_G2D_OAMEX_HW_ID_NOT_INIT.
                After NNSG2dAffineParamProxy.affineHWIndex is loaded in HW with CopyAffineToOamManager_()
                it is overwritten in the index actually written to HW.
                
 *---------------------------------------------------------------------------*/
typedef struct NNSG2dAffineParamProxy
{
    MtxFx22             mtxAffine;          // Affine Parameters
    u16                 affineHWIndex;      // AffineIndex value when loaded in HW
    u16                 pad16_;             // The padding
}
NNSG2dAffineParamProxy;

//------------------------------------------------------------------------------
// extended manager 2
typedef struct NNSG2dOamManagerInstanceEx
{
    
    NNSG2dOamChunkList*             pOamOrderingTbl;     // Pointer to the beginning of the drawing priority table
    
    u16                             numPooledOam;        // Total number of OamChunks
    u16                             numUsedOam;          // Number of OamChunks already used
    NNSG2dOamChunk*                 pPoolOamChunks;      // Pointer to the common OamChunks array
    
    u16                             lengthOfOrderingTbl; // Drawing priority table length: ( 0 - 255: u8 initialized as a u8 value) 
    u16                             lengthAffineBuffer;  // Total number of NNSG2dAffineParamProxy instances
    u16                             numAffineBufferUsed; // Number of AffineParamProxy instances already used
    u16                             lastFrameAffineIdx;  // Number for NNSG2dAffineParamProxy registered in a previous frame
    NNSG2dAffineParamProxy*         pAffineBuffer;       // Common AffineParamProxy array
    
    NNSG2dOamExEntryFunctions       oamEntryFuncs;       // Registration function
    
    
    
    u16                             lastRenderedOrderingTblIdx;
    u16                             lastRenderedChunkIdx;
    NNSG2dOamExDrawOrder            drawOrderType;       // Drawing order type
                                                         // (To maintain backward compatibility, the default order is reverse)

}NNSG2dOamManagerInstanceEx;




//------------------------------------------------------------------------------
void NNS_G2dResetOamManExBuffer  ( NNSG2dOamManagerInstanceEx* pOam );
//------------------------------------------------------------------------------
BOOL NNS_G2dGetOamManExInstance( 
    NNSG2dOamManagerInstanceEx*    pOam, 
    NNSG2dOamChunkList*             pOamOrderingTbl, 
    u8                              lengthOfOrderingTbl,
    u16                             numPooledOam,
    NNSG2dOamChunk*                 pPooledOam,
    u16                             lengthAffineBuffer,
    NNSG2dAffineParamProxy*         pAffineBuffer );
//------------------------------------------------------------------------------
BOOL NNS_G2dEntryOamManExOam
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const GXOamAttr*                pOam, 
    u8                              priority 
);
//------------------------------------------------------------------------------
BOOL NNS_G2dEntryOamManExOamWithAffineIdx
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const GXOamAttr*                pOam, 
    u8                              priority, 
    u16                             affineIdx 
);
//------------------------------------------------------------------------------
u16  NNS_G2dEntryOamManExAffine
( 
    NNSG2dOamManagerInstanceEx*    pMan, 
    const MtxFx22*                  mtx 
);
//------------------------------------------------------------------------------
void NNS_G2dApplyOamManExToBaseModule 
( 
    NNSG2dOamManagerInstanceEx* pMan 
);

//------------------------------------------------------------------------------
void NNSG2d_SetOamManExDrawOrderType
( 
    NNSG2dOamManagerInstanceEx*    pOam, 
    NNSG2dOamExDrawOrder           drawOrderType
);

//------------------------------------------------------------------------------
void NNS_G2dSetOamManExEntryFunctions
( 
    NNSG2dOamManagerInstanceEx*        pMan, 
    const NNSG2dOamExEntryFunctions*   pSrc 
);





#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_OAMEX_H_

