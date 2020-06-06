/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_CellTransferManager.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_CellTransferManager.h,v $
  Revision 1.8  10/18/2004 09:47:44 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.7  07/28/2004 08:13:01 AM  kitani_toshikazu
  Corrected comment

  Revision 1.6  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.5  07/16/2004 10:58:18 AM  kitani_toshikazu
  Changed name from NNS_G2dRequestVramTransfer() to NNS_G2dSetCellTransferStateRequested().

  Revision 1.4  07/16/2004 07:24:26 AM  kitani_toshikazu
  Added
  NNS_G2dGetNewCellTransferStateHandle()
  Add

  Revision 1.3  07/16/2004 02:35:35 AM  kitani_toshikazu
  (none)

  Revision 1.2  07/14/2004 11:50:58 PM  kitani_toshikazu
  Fixed comments.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_CELL_TRANSFER_MANAGER_H_
#define NNS_G2D_CELL_TRANSFER_MANAGER_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Image.h>

#ifdef __cplusplus
extern "C" {
#endif

// Will this be unnecessary later?
#include <nnsys/gfd/VramTransferMan/gfd_VramTransferManager.h>

#define NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE    (u32)0xFFFFFFFF


/*---------------------------------------------------------------------------*
  Name:         VramTransferTaskRegisterFuncPtr

  Description:  The pointer to the function that registers the Vram transfer task.
                
 *---------------------------------------------------------------------------*/
typedef BOOL (*VramTransferTaskRegisterFuncPtr)( NNS_GFD_DST_TYPE  type, 
                                              u32               dstAddr, 
                                              void*             pSrc, 
                                              u32               szByte );

//------------------------------------------------------------------------------
//
// NNSG2dCellTransferState
//
//------------------------------------------------------------------------------
typedef struct NNSG2dCellTransferState
{
    //
    // member set in initialization phase
    //
    NNSG2dVRamLocation    dstVramLocation;      // transfer destination of image address (be sure each area size is at least szDst)
    u32                   szDst;                // transfer destination area size
    
    void*                 pSrcNCGR;             // transfer source data (character method)
    void*                 pSrcNCBR;             // transfer source data  (bitmap method)
    u32                   szSrcData;            // transfer source data size  (same)
    BOOL                  bActive;              // active state?
    
    //
    // member renewed every frame
    //
    u32                   bDrawn;               // Drawn?
                                                // Stores whether or not the image was drawn for each graphics engine.
                                                // This is used to avoid Vram transfer of undrawn cells.
                                                // Set by the drawing module,
                                                // and refreshed every frame by the control module.
                                                // When the user builds his or her own drawing module, this member must be set appropriately.
                                            
    u32                   bTransferRequested;   // Transfer requested?
                                                // Stores the state of the transfer request for each graphics engine.
                                                // The cell animation control module is set.
                                                // Reset when the transfer task registration is ended by the control module.
    
    u32                   srcOffset;            // source offset (detail of transfer request)
    u32                   szByte;               // transfer size   (detail of transfer request)
    
}NNSG2dCellTransferState;



//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void 
NNS_G2dInitCellTransferStateManager
( 
    NNSG2dCellTransferState*            pCellStateArray, 
    u32                                 numCellState,
    VramTransferTaskRegisterFuncPtr     pTaskRegisterFunc
);

//------------------------------------------------------------------------------
// get and return handle
u32 
NNS_G2dGetNewCellTransferStateHandle();
void
NNS_G2dFreeCellTransferStateHandle( u32 handle );

//------------------------------------------------------------------------------
void NNS_G2dUpdateCellTransferStateManager();

//------------------------------------------------------------------------------
// transfer request related
// Called from the cell animation entity//
// 
void NNS_G2dSetCellTransferStateRequested
( 
    u32                                 handle, 
    u32                                 srcOffset,
    u32                                 szByte
);







//------------------------------------------------------------------------------
// internal functions
//------------------------------------------------------------------------------
NNSG2dCellTransferState* 
NNSi_G2dGetCellTransferState
( 
    u32 handle 
);

//------------------------------------------------------------------------------
void NNSi_G2dInitCellTransferState
( 
    u32                   handle,
    
    u32                   dstAddr3D,
    u32                   dstAddr2DMain,
    u32                   dstAddr2DSub,
    u32                   szDst,
    
    void*                 pSrcNCGR,
    void*                 pSrcNCBR,
    u32                   szSrcData
);


//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------
// Access such as flag operation: internally released function.
//------------------------------------------------------------------------------
NNS_G2D_INLINE void 
NNSi_G2dSetCellTransferStateRequestFlag( NNSG2dCellTransferState* pState, NNS_G2D_VRAM_TYPE type, BOOL flag )
{
    pState->bTransferRequested = ( pState->bTransferRequested & ~( 0x1 << type ) ) | ( flag << type );
}

NNS_G2D_INLINE void 
NNSi_G2dSetVramTransferRequestFlag( u32 handle, NNS_G2D_VRAM_TYPE type, BOOL flag )
{
    NNSG2dCellTransferState* pState = NNSi_G2dGetCellTransferState( handle );
    
    NNSi_G2dSetCellTransferStateRequestFlag( pState, type, flag );
}


//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL 
NNSi_G2dGetCellTransferStateRequestFlag( const NNSG2dCellTransferState* pState, NNS_G2D_VRAM_TYPE type )
{
    return (BOOL)( pState->bTransferRequested & ( 0x1 << type ) );
}

NNS_G2D_INLINE BOOL 
NNSi_G2dGetVramTransferRequestFlag( u32 handle, NNS_G2D_VRAM_TYPE type )
{
    const NNSG2dCellTransferState* pState = NNSi_G2dGetCellTransferState( handle );
    return NNSi_G2dGetCellTransferStateRequestFlag( pState, type );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void 
NNSi_G2dSetCellTransferStateCellDrawnFlag( NNSG2dCellTransferState* pState, NNS_G2D_VRAM_TYPE type, BOOL flag )
{
    pState->bDrawn = ( pState->bDrawn & ~( 0x1 << type ) ) | ( flag << type );
}

NNS_G2D_INLINE void 
NNSi_G2dSetVramTransferCellDrawnFlag( u32 handle, NNS_G2D_VRAM_TYPE type, BOOL flag )
{
    NNSG2dCellTransferState* pState = NNSi_G2dGetCellTransferState( handle );
    NNSi_G2dSetCellTransferStateCellDrawnFlag( pState, type, flag );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL 
NNSi_G2dGetCellTransferStateCellDrawnFlag( const NNSG2dCellTransferState* pState, NNS_G2D_VRAM_TYPE type )
{
    return (BOOL)( pState->bDrawn & ( 0x1 << type ) );
}

NNS_G2D_INLINE BOOL 
NNSi_G2dGetVramTransferCellDrawnFlag( u32 handle, NNS_G2D_VRAM_TYPE type )
{
    const NNSG2dCellTransferState* pState = NNSi_G2dGetCellTransferState( handle );
    return NNSi_G2dGetCellTransferStateCellDrawnFlag( pState, type );
}


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_CELL_TRANSFER_MANAGER_H_

