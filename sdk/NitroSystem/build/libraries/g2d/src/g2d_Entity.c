/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     .c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Entity.c,v $
  Revision 1.13  06/29/2005 08:30:39 AM  kitani_toshikazu
  Corrected comment

  Revision 1.12  10/19/2004 11:57:53 AM  kitani_toshikazu
  Changed internal functions to inline functions

  Revision 1.11  10/18/2004 07:41:19 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.10  07/29/2004 12:11:21 AM  kitani_toshikazu
  Changed function names.

  Revision 1.9  07/28/2004 08:12:42 AM  kitani_toshikazu
  Corrected comment

  Revision 1.8  07/24/2004 07:10:30 AM  kitani_toshikazu
  Renamed Entity related function

  Revision 1.7  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.6  05/28/2004 10:50:16 AM  kitani_toshikazu
  Added accessor to NNSG2dAnimController

  Revision 1.5  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.4  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.3  04/23/2004 12:27:24 AM  kitani_toshikazu
  Added workaround for color palette swap table.

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_Entity.h>
#include <nnsys/g2d/g2d_CellAnimation.h>
#include <nnsys/g2d/g2d_MultiCellAnimation.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>
#include "g2d_Internal.h"

//------------------------------------------------------------------------------
NNS_G2D_INLINE void ResetPaletteTbl_( NNSG2dEntity* pEntity )
{
    
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    
    NNS_G2D_NULL_ASSERT( pEntity );
    
    // Reset the color palette conversion table
    NNS_G2dResetEntityPaletteTable( pEntity );
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE void SetCurrentAnimation_( NNSG2dEntity* pEntity )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    {
        const NNSG2dAnimSequence* pAnimSeq = NNS_G2dGetAnimSequenceByIdx( pEntity->pAnimDataBank,
                                                             pEntity->currentSequenceIdx );
        if( pAnimSeq )
        {
            switch( pEntity->pEntityData->type )
            {
            case NNS_G2D_ENTITYTYPE_CELL:
                NNS_G2dSetCellAnimationSequence( (NNSG2dCellAnimation*)pEntity->pDrawStuff, pAnimSeq );
                break;
            case NNS_G2D_ENTITYTYPE_MULTICELL:
                NNS_G2dSetAnimSequenceToMCAnimation( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff, pAnimSeq );
                break;
            default:
                NNS_G2D_ASSERT( FALSE );
            }
        }else{
            NNS_G2D_ASSERT( FALSE );
        }
    }   
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitEntity

  Description:  Initialize Entity itself
                
                
  Arguments:    pEntity         [OUT]  entity
                pDrawStuff     [IN]  Data for drawing  (NNSG2dCellAnimation or NNSG2dMultiCellAnimation )
                pEntityData    [IN] Entity definition data
                pAnimDataBank   [IN]  Animation data bank 

  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitEntity
( 
    NNSG2dEntity*               pEntity, 
    void*                       pDrawStuff, 
    const NNSG2dEntityData*     pEntityData, 
    const NNSG2dAnimBankData*   pAnimDataBank 
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_NULL_ASSERT( pDrawStuff );
    NNS_G2D_NULL_ASSERT( pEntityData );
    NNS_G2D_NULL_ASSERT( pAnimDataBank );
    
    
    // pEntityData->type
    pEntity->pDrawStuff             = pDrawStuff;
    pEntity->pAnimDataBank          = pAnimDataBank;
    pEntity->pEntityData            = pEntityData;
    pEntity->currentSequenceIdx     = 0;
     
    // Initialization of the color palette conversion table
    ResetPaletteTbl_( pEntity );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetEntityCurrentAnimation

  Description:  Sets the current playback animation for NNSG2dEntity
                
                
  Arguments:    pEntity:            [OUT]  Actual NNSG2dEntity 
                idx         :                [IN]  AnimationSequence Index inside NNSG2dEntity 
                
                Animation should have been loaded.
                It must be loaded.
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetEntityCurrentAnimation( NNSG2dEntity* pEntity, u16 idx )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_NULL_ASSERT( pEntity->pAnimDataBank );
    NNS_G2D_NULL_ASSERT( pEntity->pEntityData );
    
    if( pEntity->pEntityData->animData.numAnimSequence > idx )
    {
        pEntity->currentSequenceIdx = pEntity->pEntityData->animData.pAnimSequenceIdxArray[idx];
        // TODO: This process should be performed at time of initialization on  all sequence number array elements!
        NNS_G2D_ASSERT( pEntity->pAnimDataBank->numSequences > pEntity->currentSequenceIdx );
        //
        // Process for animation switching
        //
        SetCurrentAnimation_( pEntity );
    }else{
        NNSI_G2D_DEBUGMSG0( FALSE, "Failure of finding animation data in NNS_G2dSetEntityCurrentAnimation()" );
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetColorPaletteTable

  Description:  Sets the color palette conversion table in NNSG2dEntity
                
                
  Arguments:    pEntity:            [OUT]  Actual NNSG2dEntity 
                pPlttTbl:           [IN] Color palette conversion table
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetEntityPaletteTable
( 
    NNSG2dEntity*           pEntity, 
    NNSG2dPaletteSwapTable* pPlttTbl 
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_NULL_ASSERT( pPlttTbl );
    
    pEntity->pPaletteTbl = pPlttTbl;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetEntityPaletteTable

  Description:  Sets the color palette conversion table in NNSG2dEntity to invalid
                
                
  Arguments:    pEntity:            [OUT]  Actual NNSG2dEntity 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dResetEntityPaletteTable
( 
    NNSG2dEntity*           pEntity
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    pEntity->pPaletteTbl = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsEntityPaletteTblEnable

  Description:  Checks whether or not the NNSG2dEntity color palette is valid
                
                
  Arguments:    pEntity:            [IN]  Actual NNSG2dEntity 
                
                
                
  Returns:      TRUE if color palette is valid
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsEntityPaletteTblEnable( const NNSG2dEntity* pEntity )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    return ( pEntity->pPaletteTbl != NULL ) ? TRUE : FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dTickEntity

  Description:  Updates NNSG2dEntity
                
                
  Arguments:    pEntity:            [OUT]  Actual NNSG2dEntity 
                dt:                 [IN] The time to advance (in frames)
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void  NNS_G2dTickEntity( NNSG2dEntity*    pEntity, fx32 dt )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    switch( pEntity->pEntityData->type )
    {
    case NNS_G2D_ENTITYTYPE_CELL:
        NNS_G2dTickCellAnimation( (NNSG2dCellAnimation*)pEntity->pDrawStuff, dt );
        break;
    case NNS_G2D_ENTITYTYPE_MULTICELL:
        NNS_G2dTickMCAnimation( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff, dt );
        break;
    default:
        NNS_G2D_ASSERT(FALSE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetEntityCurrentFrame

  Description:  Sets the playback animation frame for NNSG2dEntity
                
                
  Arguments:    pEntity:            [OUT]  Actual NNSG2dEntity 
                frameIndex:         [IN]  Animation frame number 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void  NNS_G2dSetEntityCurrentFrame
( 
    NNSG2dEntity*    pEntity,
    u16              frameIndex  
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    switch( pEntity->pEntityData->type )
    {
    case NNS_G2D_ENTITYTYPE_CELL:
        NNS_G2dSetCellAnimationCurrentFrame( (NNSG2dCellAnimation*)pEntity->pDrawStuff, 
                                             frameIndex );
        break;
    case NNS_G2D_ENTITYTYPE_MULTICELL:
        NNS_G2dSetMCAnimationCurrentFrame( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff,
                                             frameIndex );
        break;
    default:
        NNS_G2D_ASSERT(FALSE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetEntitySpeed

  Description:  Sets the animation speed for NNSG2dEntity
                
                
  Arguments:    pEntity:            [OUT]  Actual NNSG2dEntity 
                speed       :              [IN]  Animation speed 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetEntitySpeed
(
    NNSG2dEntity*     pEntity,
    fx32              speed  
)
{
    NNS_G2D_NULL_ASSERT( pEntity );
    NNS_G2D_ASSERT_ENTITY_VALID( pEntity );
    
    switch( pEntity->pEntityData->type )
    {
    case NNS_G2D_ENTITYTYPE_CELL:
        NNS_G2dSetCellAnimationSpeed( (NNSG2dCellAnimation*)pEntity->pDrawStuff, 
                                             speed );
        break;
    case NNS_G2D_ENTITYTYPE_MULTICELL:
        NNS_G2dSetMCAnimationSpeed( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff,
                                             speed );
        break;
    default:
        NNS_G2D_ASSERT(FALSE);
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dIsEntityValid

  Description:  Checks whether or not NNSG2dEntity state is valid
                
                
  Arguments:    pEntity:            [IN]  Actual NNSG2dEntity 
                
                
  Returns:      TRUE if state is valid
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dIsEntityValid( NNSG2dEntity*    pEntity )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    
    if( ( pEntity->pEntityData     != NULL ) &&
        ( pEntity->pDrawStuff      != NULL ) &&
        ( pEntity->pAnimDataBank   != NULL )  )
        {
            return TRUE;
        }else{
            return FALSE;
        }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetEntityAnimCtrl

  Description:  Gets the Entity's animation controller.
                
                
  Arguments:    pEntity:            [IN]  Actual NNSG2dEntity 
                
                
  Returns:      Animation controller
  
 *---------------------------------------------------------------------------*/
NNSG2dAnimController* NNS_G2dGetEntityAnimCtrl( NNSG2dEntity*    pEntity )
{
    NNS_G2D_NULL_ASSERT( pEntity );
    switch( pEntity->pEntityData->type )
    {
    case NNS_G2D_ENTITYTYPE_CELL:
        return NNS_G2dGetCellAnimationAnimCtrl( (NNSG2dCellAnimation*)pEntity->pDrawStuff );
    case NNS_G2D_ENTITYTYPE_MULTICELL:
        return NNS_G2dGetMCAnimAnimCtrl( (NNSG2dMultiCellAnimation*)pEntity->pDrawStuff );
    default:
        NNS_G2D_ASSERT(FALSE);
        return NULL;
    }
}




