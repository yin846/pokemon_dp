/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2 D
  
  File:     g2d_CellTransferManager.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_CellTransferManager.c,v $
  Revision 1.13  06/29/2005 08:30:39 AM  kitani_toshikazu
  Corrected comment

  Revision 1.12  10/28/2004 12:05:37 AM  kitani_toshikazu
  Corrected comment

  Revision 1.11  10/19/2004 11:57:53 AM  kitani_toshikazu
  Changed internal functions to inline functions

  Revision 1.10  10/19/2004 01:41:39 AM  kitani_toshikazu
  Revised comments, changed parameter names.

  Revision 1.9  2004/10/18 09:47:54 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.8  08/30/2004 12:11:06 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.7  07/28/2004 08:12:42 AM  kitani_toshikazu
  Corrected comment

  Revision 1.6  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.5  07/16/2004 10:59:38 AM  kitani_toshikazu
  Made revisions regarding processing the NNSi_G2dInitCellTransferState() address argument.

  Revision 1.4  07/16/2004 10:02:11 AM  kitani_toshikazu
  Changed name from NNS_G2dRequestVramTransfer() to NNS_G2dSetCellTransferStateRequested().

  Revision 1.3  07/16/2004 07:23:13 AM  kitani_toshikazu
  Added
  NNS_G2dGetNewCellTransferStateHandle()
  Add

  Revision 1.2  07/16/2004 02:34:25 AM  kitani_toshikazu
  Fixed bug in conversion part of transfer destination type.

  Revision 1.1  07/14/2004 11:48:18 PM  kitani_toshikazu
  Registered initial issue

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_CellTransferManager.h>
#include <nnsys/gfd/VramTransferMan/gfd_VramTransferManager.h>

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
static NNSG2dCellTransferState*                s_pCellStateArray = NULL;
static u32                                     s_numCellState = 0;

VramTransferTaskRegisterFuncPtr                s_pTaskRegisterFunc = NULL;


//------------------------------------------------------------------------------
// Is the state of the manager a valid state?
NNS_G2D_INLINE BOOL IsCellTransferStateManagerValid_()
{
    return (BOOL)( ( s_pCellStateArray != NULL) &&
                   ( s_numCellState != 0 ) &&
                   ( s_pTaskRegisterFunc != NULL ) ) ;
}

//------------------------------------------------------------------------------
// Is the handle valid?
NNS_G2D_INLINE BOOL IsValidHandle_( u32 handle )
{
    NNS_G2D_ASSERT( IsCellTransferStateManagerValid_() );
    
    if( handle != NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE )
    {    
        if( handle < s_numCellState )
        {
            if( s_pCellStateArray[handle].bActive )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE NNSG2dCellTransferState*
GetValidCellTransferState_( u32 validHandle )
{
    NNS_G2D_ASSERT( IsValidHandle_( validHandle ) );
    return &s_pCellStateArray[validHandle];
}



//------------------------------------------------------------------------------
// Does this need to be registered as a VRAM transfer task?
NNS_G2D_INLINE BOOL ShouldRegisterAsVramTransferTask_
(
    const NNSG2dCellTransferState*    pState,
    NNS_G2D_VRAM_TYPE                   type
)
{
    NNS_G2D_NULL_ASSERT( pState );
    
    return (BOOL)(  NNSi_G2dGetCellTransferStateRequestFlag( pState, type ) &&
                    NNSi_G2dGetCellTransferStateCellDrawnFlag( pState, type ) );
}

//------------------------------------------------------------------------------
// Reset the draw attributes
NNS_G2D_INLINE void ResetCellTransferStateDrawnFlag_
(
    NNSG2dCellTransferState*  pState 
)
{
    pState->bDrawn = 0x0;
}



//------------------------------------------------------------------------------
// Get the transfer source data
NNS_G2D_INLINE const void* GetVramTransferSrc_
(
    const NNSG2dCellTransferState*    pState,
    NNS_G2D_VRAM_TYPE                   type
)
{
    NNS_G2D_NULL_ASSERT( pState );
    
    if( type == NNS_G2D_VRAM_TYPE_3DMAIN )
    {
        return pState->pSrcNCBR;
    }else{
        return pState->pSrcNCGR;
    }
}

//------------------------------------------------------------------------------
// Is the transfer source data valid data?
NNS_G2D_INLINE BOOL IsVramTransferSrcDataValid_
(
    const NNSG2dCellTransferState*    pState,
    NNS_G2D_VRAM_TYPE                   type
)
{
    return (BOOL)( GetVramTransferSrc_( pState, type ) != NULL );
}



//------------------------------------------------------------------------------
// Are preparations for transfer ready?
NNS_G2D_INLINE BOOL IsCellTransferStateValid_
(
    const NNSG2dCellTransferState*  pState, 
    NNS_G2D_VRAM_TYPE               type
)
{
    // Transfer source data is set
    // Transfer source region is set
    return (BOOL)( NNSi_G2dIsVramLocationReadyToUse( &pState->dstVramLocation, type ) &&
                   IsVramTransferSrcDataValid_( pState, type ) );
}

//------------------------------------------------------------------------------
// Conversion of enumerator
// At present time, simply a cast
NNS_G2D_INLINE NNS_GFD_DST_TYPE
ConvertVramType_( NNS_G2D_VRAM_TYPE type )
{
    NNS_G2D_MINMAX_ASSERT( type, NNS_G2D_VRAM_TYPE_3DMAIN, NNS_G2D_VRAM_TYPE_2DSUB );
    {
        const static NNS_GFD_DST_TYPE cvtTbl []=
        {
            NNS_GFD_DST_3D_TEX_VRAM,      // NNS_G2D_VRAM_TYPE_3DMAIN
            NNS_GFD_DST_2D_OBJ_CHAR_MAIN, // NNS_G2D_VRAM_TYPE_2DMAIN
            NNS_GFD_DST_2D_OBJ_CHAR_SUB   // NNS_G2D_VRAM_TYPE_2DSUB
        };
        return cvtTbl[type];
    }
}

//------------------------------------------------------------------------------
// 
// Creation of task
// Input NNSG2dCellTransferState to create a task to register to manager
//
NNS_G2D_INLINE BOOL MakeVramTransferTask_
(
    const NNSG2dCellTransferState*    pState,
    NNS_G2D_VRAM_TYPE                   type
)
{
    NNS_G2D_NULL_ASSERT( pState );
    
    NNS_G2D_ASSERT( IsCellTransferStateValid_( pState, type ) );
    
    //
    // Assign registration of VRAM transfer task to external module
    //
    return (*s_pTaskRegisterFunc )( ConvertVramType_( type ),
                                    NNSi_G2dGetVramLocation( &pState->dstVramLocation, type ),
                                    (u8*)GetVramTransferSrc_( pState, type ) + pState->srcOffset,
                                    pState->szByte );
}
//------------------------------------------------------------------------------
// Reset NNSG2dCellTransferState
NNS_G2D_INLINE void ResetCellTransferState_( NNSG2dCellTransferState* pState )
{
    NNS_G2D_NULL_ASSERT( pState );
    {
        NNSi_G2dInitializeVRamLocation( &pState->dstVramLocation );
        pState->szDst = 0;
        pState->pSrcNCGR = NULL;
        pState->pSrcNCBR = NULL;
        pState->szSrcData = 0;
        pState->bActive = FALSE;
        
        
        pState->bDrawn              = 0x0;
        pState->bTransferRequested  = 0x0;
        pState->srcOffset           = 0x0;
        pState->szByte              = 0x0;
    }
}
//------------------------------------------------------------------------------
// library internal release
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
NNSG2dCellTransferState*
NNSi_G2dGetCellTransferState
(
    u32 handle
)
{
    return GetValidCellTransferState_( handle );
}

//------------------------------------------------------------------------------
// Set the transfer request to the transfer state manager object of the specified handler
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
)
{
    NNS_G2D_ASSERT( IsValidHandle_( handle ) );
    NNS_G2D_NON_ZERO_ASSERT( szDst );
    NNS_G2D_NON_ZERO_ASSERT( szSrcData );
    NNS_G2D_NULL_ASSERT( szDst );
    
    {
        NNSG2dCellTransferState* pState
            = GetValidCellTransferState_( handle );
        
        NNS_G2D_NULL_ASSERT( pState );
        
        // One of them should be valid
        NNS_G2D_ASSERT( dstAddr3D       != NNS_G2D_VRAM_ADDR_NONE   ||
                        dstAddr2DMain   != NNS_G2D_VRAM_ADDR_NONE   ||
                        dstAddr2DSub    != NNS_G2D_VRAM_ADDR_NONE    );
        {
            NNSG2dVRamLocation* pImg = &pState->dstVramLocation;
            
            NNSi_G2dInitializeVRamLocation( pImg );
            
            if( dstAddr3D != NNS_G2D_VRAM_ADDR_NONE )
            {
                NNSi_G2dSetVramLocation( pImg, NNS_G2D_VRAM_TYPE_3DMAIN, dstAddr3D );
            }
            
            if( dstAddr2DMain != NNS_G2D_VRAM_ADDR_NONE )
            {
                NNSi_G2dSetVramLocation( pImg, NNS_G2D_VRAM_TYPE_2DMAIN, dstAddr2DMain );
            }
            
            if( dstAddr2DSub != NNS_G2D_VRAM_ADDR_NONE )
            {
                NNSi_G2dSetVramLocation( pImg, NNS_G2D_VRAM_TYPE_2DSUB , dstAddr2DSub );
            }
        }
        
        // Both NULL does not clearly mean valid
        NNS_G2D_ASSERT( pSrcNCGR != NULL || pSrcNCBR != NULL );
        
        pState->szDst     = szDst;
        pState->pSrcNCGR  = pSrcNCGR;
        pState->pSrcNCBR  = pSrcNCBR;
        pState->szSrcData = szSrcData;
    }
}




//------------------------------------------------------------------------------
// initialization related
//------------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitCellTransferStateManager

  Description:  Initializes the cell VRAM transfer state object manager.
                Cell VRAM transfer state object buffer is passed as argument.
                pTaskRegisterFunc is the pointer to the function that requests registration of VRAM transfer task.
                
                
  Arguments:    pCellStateArray        [OUT]  Cell VRAM transfer state object buffer
                numCellState            [IN]  Cell VRAM transfer state object buffer length
                pTaskRegisterFunc     [IN]  Pointer to VRAM transfer register function
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G2dInitCellTransferStateManager
(
    NNSG2dCellTransferState*            pCellStateArray, 
    u32                                 numCellState,
    VramTransferTaskRegisterFuncPtr     pTaskRegisterFunc
)
{
    NNS_G2D_NULL_ASSERT( pCellStateArray );
    NNS_G2D_NON_ZERO_ASSERT( numCellState );
    NNS_G2D_NULL_ASSERT( pTaskRegisterFunc );
    
    
    s_pTaskRegisterFunc     = pTaskRegisterFunc;
    
    s_pCellStateArray       = pCellStateArray;
    s_numCellState          = numCellState;
    
    //
    // Reset all NNSG2dCellTransferState 
    //
    {
        u32     i;
        for( i = 0; i < numCellState; i++ )
        {
            ResetCellTransferState_( &pCellStateArray[i] );
        }
    }
}


//------------------------------------------------------------------------------
// Processes relating to transfers
//------------------------------------------------------------------------------

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dUpdateCellTransferStateManager

  Description:  Updates the transfer state manager.
                
                Call this at every frame animation update, after the rendering  request has been completed and before VRAM transfers.
                (The position of the call is critical.))
                
                Regarding all transfer state objects registered internally, 
                determine if it is necessary to generate VRAM transfer. If so, it generates task.  
                
                
                
  Arguments:    None
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dUpdateCellTransferStateManager()
{
    u32 i;
    // For all VRAM transfer objects being used....
    for( i = 0; i < s_numCellState; i++ )
    {
        NNS_G2D_VRAM_TYPE           type;
        NNSG2dCellTransferState*  pState                = &s_pCellStateArray[i];
        
        // Is state active?
        if( pState->bActive )
        {
            //
            // For all types of drawing HW....
            //
            for( type = NNS_G2D_VRAM_TYPE_3DMAIN; type < NNS_G2D_VRAM_TYPE_MAX; type++ )
            {
                //
                // If transfer is necessary....
                //
                
                if( ShouldRegisterAsVramTransferTask_( pState, type ) )
                {
                    
                    // Get from manager
                    // Create
                    if( MakeVramTransferTask_( pState, type ) )
                    {
                        // OS_Printf( "VRAM transfer task is registered ! surface_Id = %d \n", type ); Reset registration request state
                        // (Only for VRAM types that have been transferred)
                        NNSi_G2dSetCellTransferStateRequestFlag( pState, type, FALSE );
                    }else{
                        // Registration failed
                        // TODO : Warning message? Assert
                    }
                }
            }
            
            //
            // Reset drawing state (for all VRAM types)
            //
            ResetCellTransferStateDrawnFlag_( pState );
        }
    }
}





//------------------------------------------------------------------------------
// Processes relating to registration
//------------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetCellTransferStateRequested

  Description:  Set the transfer request to the transfer state manager object of the  specified handler
                
                
  Arguments:    handle:         [IN] Handle of transfer state manager object
             srcOffset:         [IN] Value of offset from start of transfer source data
                szByte:         [IN]  Transfer size
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetCellTransferStateRequested
(
    u32                                 handle,
    u32                                 srcOffset,
    u32                                 szByte
)
{
    NNS_G2D_ASSERT( IsValidHandle_( handle ) );
    
    {
        NNSG2dCellTransferState* pState
            = NNSi_G2dGetCellTransferState( handle );
        
        NNS_G2D_NULL_ASSERT( pState );
        NNS_G2D_ASSERT( szByte <= pState->szDst );
        //
        // Transition to transfer request state
        //
        pState->bTransferRequested    = 0xFFFFFFFF;// memo: When turning ON, they are all turned ON at once
        pState->srcOffset             = srcOffset;
        pState->szByte                = szByte;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetNewCellTransferStateHandle

  Description:  Gets the handle to the cell VRAM transfer state object.
                This handle is used to control the cell VRAM transfer state object.
                The handle is stored as a member of the cell animation itself.
                If getting the handle fails, NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE  is returned.
                Internally, the function performs a linear search for the cell VRAM transfer state object.
                (Avoid calling this function in performance-critical places.)

                
                
  Arguments:    None
                    
  Returns:      Cell VRAM transfer state object handle
                If getting the handle fails, NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE  is returned.
  
 *---------------------------------------------------------------------------*/
u32
NNS_G2dGetNewCellTransferStateHandle()
{
    NNS_G2D_ASSERT( IsCellTransferStateManagerValid_() );
    
    
    //
    // Search from the start of the array of transfer state objects not being used.
    // 
    {
        u32 i = 0;
        for( i = 0;i < s_numCellState; i++ )
        {
            if( s_pCellStateArray[ i ].bActive != TRUE )
            {
                s_pCellStateArray[ i ].bActive = TRUE;
                return i;
            }
        }
    }
    
    // could not be found
    return NNS_G2D_INVALID_CELL_TRANSFER_STATE_HANDLE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dFreeCellTransferStateHandle

  Description:  Returns the handle of the cell VRAM transfer state object.
                Execute this function for handles no longer being used.


                
                
  Arguments:    handle:  [IN] Handle of the cell VRAM transfer state object. 
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G2dFreeCellTransferStateHandle( u32 handle )
{
    NNS_G2D_ASSERT( IsValidHandle_( handle ) );
    NNS_G2D_ASSERT( IsCellTransferStateManagerValid_() );
    
    ResetCellTransferState_( GetValidCellTransferState_( handle ) );
}

   
    

