/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_FrameTexVramMan.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_FrameTexVramMan.c,v $
  Revision 1.15  08/17/2005 11:51:48 PM  kitani_toshikazu
  Revised copyright notation

  Revision 1.14  08/17/2005 08:09:56 AM  kitani_toshikazu
  Added a function for debug display

  Revision 1.13  12/22/2004 04:18:51 AM  kitani_toshikazu
  Added a process that returns an error key for a request that would allocate an illegally large area.

  Revision 1.12  11/22/2004 08:46:00 AM  kitani_toshikazu
  Changed internal functions to inline functions

  Revision 1.11  11/17/2004 04:31:40 AM  kitani_toshikazu
  Fixed a bug in the search-order config section in the manager initialization function

  Revision 1.10  10/18/2004 12:27:53 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.9  09/29/2004 02:03:23 AM  kitani_toshikazu
  Added comments

  Revision 1.8  09/22/2004 09:50:08 AM  kitani_toshikazu
  Described the process for changing the search order of normal texture when initialized with 2 or less slots

  Revision 1.7  09/22/2004 08:27:31 AM  kitani_toshikazu
  Error process of TexKey free that has invalid size information

  Revision 1.6  09/02/2004 11:53:02 PM  kitani_toshikazu
  Revised misspelling

  Revision 1.5  08/30/2004 12:16:17 AM  kitani_toshikazu
  Corrected a spelling error in the comments

  Revision 1.4  07/28/2004 12:31:21 AM  kitani_toshikazu
  Correspondence with the padding warning

  Revision 1.3  07/15/2004 02:50:13 AM  kitani_toshikazu
  Supported key that expresses allocation error.

  Revision 1.2  07/10/2004 04:31:46 AM  nishida
  Made revisions for the configuration of whether or not to use as the default at initialization

  Revision 1.1  07/10/2004 02:02:48 AM  nishida
  New registration when streamlining the VRAM manager.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/gfd/gfd_common.h>
#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_FrameTexVramMan.h>

//------------------------------------------------------------------------------
#define NNS_GFD_TEX_SLOT_SIZE          		0x20000

#define NNS_GFD_NUM_TEX_VRAM_REGION         5
#define NNS_GFD_NUM_TEX_VRAM_REGION_4x4     2
#define NNS_GFD_INVALID_ADDR                0xFFFFFFFF

//------------------------------------------------------------------------------
typedef struct NNSGfdFrmTexRegionState
{
    u32           head;
    u32           tail;
    
    BOOL          bActive;
    
    const BOOL    bHalfSize;        // TODO: Plan to change so that it saves size directly.
    
    const u16     index;
    const u16     pad16_;
    const u32     baseAddress;
    
}NNSGfdFrmTexRegionState;

//------------------------------------------------------------------------------
typedef struct NNSGfdFrmTexVramMnager
{
    u16     numSlot;

} NNSGfdFrmTexVramMnager;


static NNSGfdFrmTexVramMnager frmExVramMan_;

//------------------------------------------------------------------------------

static NNSGfdFrmTexRegionState  vramRegions_[NNS_GFD_NUM_TEX_VRAM_REGION] = 
{
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, FALSE, 0, 0xFFFF, 0x0                                               },
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, TRUE,  1, 0xFFFF, (u32)NNS_GFD_TEX_SLOT_SIZE                             },
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, TRUE,  2, 0xFFFF, (u32)(NNS_GFD_TEX_SLOT_SIZE + NNS_GFD_TEX_SLOT_SIZE / 2) },
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, FALSE, 3, 0xFFFF, (u32)(NNS_GFD_TEX_SLOT_SIZE * 2)                         },
    { NNS_GFD_INVALID_ADDR, NNS_GFD_INVALID_ADDR, FALSE, FALSE, 4, 0xFFFF, (u32)(NNS_GFD_TEX_SLOT_SIZE * 3)                         }
};

//------------------------------------------------------------------------------
static NNSGfdFrmTexRegionState* texNrmSearchArray_[NNS_GFD_NUM_TEX_VRAM_REGION] =
{
    &vramRegions_[4],
    &vramRegions_[3],
    &vramRegions_[0],
    &vramRegions_[2],
    &vramRegions_[1]
};

static NNSGfdFrmTexRegionState* tex4x4SearchArray_[NNS_GFD_NUM_TEX_VRAM_REGION_4x4] =
{
    &vramRegions_[0],
    &vramRegions_[3]
};



//------------------------------------------------------------------------------
// Is the number of slots appropriate?
NNS_GFD_INLINE BOOL IsSlotNumValid_( u16 numSlot )
{
    if( numSlot > 0 && numSlot <= 4 )
    {
        return TRUE;
    }else{
        return FALSE;
    }
}


//------------------------------------------------------------------------------
NNS_GFD_INLINE void ResetRegionNormal_( NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    
    pRegion->head = 0x0;
    pRegion->tail = NNS_GFD_TEX_SLOT_SIZE;
}
//------------------------------------------------------------------------------
NNS_GFD_INLINE void ResetRegionHalf_( NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    
    pRegion->head = 0x0;
    pRegion->tail = NNS_GFD_TEX_SLOT_SIZE / 2;
}



//------------------------------------------------------------------------------
NNS_GFD_INLINE BOOL IsRegionStateValid_( const NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    
    if( pRegion->head != NNS_GFD_INVALID_ADDR && 
        pRegion->tail != NNS_GFD_INVALID_ADDR )
    {
        if( pRegion->head <= pRegion->tail ) 
        {
            return TRUE;
        }
    }
    
    return FALSE;
}

//------------------------------------------------------------------------------
// Checks if the manager is in a valid state.
NNS_GFD_INLINE BOOL IsVramManagerValid_( )
{
    int i;
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        if( !IsRegionStateValid_( &vramRegions_[i] ) )
        {
            return FALSE;
        }
    }
    return TRUE;
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE u32 GetReagionCapacity_( const NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    NNS_GFD_ASSERT( IsRegionStateValid_( pRegion ) );
    
    return (u32)( pRegion->tail - pRegion->head );
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE u32 AllocFromReagionHead_( NNSGfdFrmTexRegionState* pRegion, u32 szByte )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    NNS_GFD_ASSERT( IsRegionStateValid_( pRegion ) );
    NNS_GFD_ASSERT( GetReagionCapacity_( pRegion ) >= szByte );
    
    {
        const u32 result = pRegion->head;
    
        pRegion->head += szByte;
    
        return result;
    }
}

//------------------------------------------------------------------------------
NNS_GFD_INLINE u32 AllocFromReagionTail_( NNSGfdFrmTexRegionState* pRegion, u32 szByte )
{
    NNS_GFD_NULL_ASSERT( pRegion ); 
    NNS_GFD_ASSERT( IsRegionStateValid_( pRegion ) );
    NNS_GFD_ASSERT( GetReagionCapacity_( pRegion ) >= szByte );
    
    {
        pRegion->tail -= szByte;
        
        return pRegion->tail;
    }
}




//------------------------------------------------------------------------------
NNS_GFD_INLINE NNSGfdFrmTexRegionState* Get4x4IdxRegion_( const NNSGfdFrmTexRegionState* pRegion )
{
    NNS_GFD_NULL_ASSERT( pRegion );
    switch( pRegion->index )
    {
        case 0:return &vramRegions_[1];
        case 3:return &vramRegions_[2];
        // NG
        default:NNS_GFD_WARNING("Invalid region is detected. in Get4x4IdxRegion_(). ");break;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// Allocate 4x4Compressed texture region.
NNS_GFD_INLINE BOOL DoAlloc4x4_( u32 szByte, u32* addr )
{
    NNS_GFD_NULL_ASSERT( addr );
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    {
        int i;
        NNSGfdFrmTexRegionState* pRegion = NULL;
        NNSGfdFrmTexRegionState* pPltRegion = NULL;
        
        for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION_4x4; i++ )
        {        
            pRegion = tex4x4SearchArray_[i];
            
            //
            // If usable, and if there is free space...
            //
            if( pRegion->bActive && 
                GetReagionCapacity_( pRegion ) >= szByte )
            {
                pPltRegion = Get4x4IdxRegion_( pRegion );
                
                // Concerning a region used for an index table
                //
                // If usable, and if there is free space...
                //
                if( pPltRegion->bActive && 
                    GetReagionCapacity_( pPltRegion ) >= szByte / 2 )
                {
                    *addr = AllocFromReagionHead_( pRegion, szByte );
                    (void)AllocFromReagionHead_( pPltRegion, szByte / 2 );
                    
                    *addr += pRegion->baseAddress;
                    
                    return TRUE; 
                }
            }
        }
        
        //
        // NG TODO:msg
        //
        NNS_GFD_WARNING("We have no memory for 4x4 Tex.");
        return FALSE;
    }
}



//------------------------------------------------------------------------------
// Allocating a normal texture region
NNS_GFD_INLINE BOOL DoAllocNrm_( u32 szByte, u32* addr )
{
    NNS_GFD_NULL_ASSERT( addr );
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    {
        int i;
        NNSGfdFrmTexRegionState* pRegion = NULL;
        
        for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
        {        
            pRegion = texNrmSearchArray_[i];
            if( pRegion->bActive )
            {
                if( GetReagionCapacity_( pRegion ) >= szByte )
                {
                    *addr = AllocFromReagionTail_( pRegion, szByte );    
                    
                    *addr += pRegion->baseAddress;
                    
                    return TRUE; 
                }
            }
        }
        //
        // NG TODO:msg
        //
        NNS_GFD_WARNING("We have no memory for Nrm Tex.");
        return FALSE;
    }
}

//------------------------------------------------------------------------------
// Sets the search order of normal texture
void NNSi_GfdSetTexNrmSearchArray
( 
    int idx1st, 
    int idx2nd,
    int idx3rd,
    int idx4th,
    int idx5th
)
{
    
    NNS_GFD_MINMAX_ASSERT( idx1st, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    NNS_GFD_MINMAX_ASSERT( idx2nd, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    NNS_GFD_MINMAX_ASSERT( idx3rd, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    NNS_GFD_MINMAX_ASSERT( idx4th, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    NNS_GFD_MINMAX_ASSERT( idx5th, 0, NNS_GFD_NUM_TEX_VRAM_REGION - 1);
    
    texNrmSearchArray_[0] = &vramRegions_[idx1st];
    texNrmSearchArray_[1] = &vramRegions_[idx2nd];
    texNrmSearchArray_[2] = &vramRegions_[idx3rd];
    texNrmSearchArray_[3] = &vramRegions_[idx4th];
    texNrmSearchArray_[4] = &vramRegions_[idx5th];
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdDumpFrmTexVramManager

  Description:  Manager contents are output for debugging
                
                 
  Arguments:   None

                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdDumpFrmTexVramManager()
{
    
    
    int i;
    const NNSGfdFrmTexRegionState*  pState          = NULL;
    u32                             szTotalFree     = 0;
    u32                             szTotalResrved  = 0;
    
    OS_Printf("===== NNS_Gfd FrmTexVramManager Dump =====\n");
    OS_Printf("index : head-Addr   : tail-addr   : free-size \n");
    
    // Calls the callback functions for all RegionState.
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        pState = &vramRegions_[i];
        if( pState->bActive )
        {
            OS_Printf("%02d    : 0x%08x  : 0x%08x  : 0x%08x  \n", 
                pState->index, 
                pState->head + pState->baseAddress, 
                pState->tail + pState->baseAddress, 
                pState->tail - pState->head );
            
            szTotalFree     += pState->tail - pState->head;
            szTotalResrved  += (pState->bHalfSize) ? NNS_GFD_TEX_SLOT_SIZE / 2 : NNS_GFD_TEX_SLOT_SIZE;            
        }else{
            OS_Printf("%02d    : ----------  : ----------  : ----------  \n", pState->index );
        }
    }
    
    OS_Printf("    %08d / %08d bytes (%6.2f%%) used \n", 
                szTotalResrved - szTotalFree, 
                szTotalResrved, 
                100.0f * ( szTotalResrved - szTotalFree ) / szTotalResrved ); 
    OS_Printf("==========================================\n");
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdInitFrmTexVramManager

  Description:  Initializes manager.
                
                 
  Arguments:   numSlot      The number of VRAM slots that manage the frame texture VRAM manager
               useAsDefault Whether or not to use the frame texture VRAM manager by default

                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdInitFrmTexVramManager( u16 numSlot, BOOL useAsDefault )
{
    NNS_GFD_ASSERT( IsSlotNumValid_( numSlot ) );
    
    //
    // When slot 2 (Region 3) is not used,
    // Region 2 is not used as the index table for 4x4 texture.
    // In this case, Region 2 should have a priority over Region 0,
    // reset the search table that way.
    //
    if( numSlot <= 2 )
    {
        // ( 4, 3, 0, 2, 1 ) => ( 4, 3, 2, 0, 1 )
        NNSi_GfdSetTexNrmSearchArray( 4, 3, 2, 0, 1 ); 
    }else{
        // ( 4, 3, 2, 0, 1 ) => ( 4, 3, 0, 2, 1 )
        NNSi_GfdSetTexNrmSearchArray( 4, 3, 0, 2, 1 ); 
    }
    
    frmExVramMan_.numSlot = numSlot;
    NNS_GfdResetFrmTexVramState();

	if (useAsDefault)
	{
		NNS_GfdDefaultFuncAllocTexVram = NNS_GfdAllocFrmTexVram;
		NNS_GfdDefaultFuncFreeTexVram  = NNS_GfdFreeFrmTexVram;
	}
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdResetFrmTexVramState

  Description:  Resets manager's current state.
                 
  Arguments:   None
                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdResetFrmTexVramState()
{
    int i;
    u16 numSlot = frmExVramMan_.numSlot;
    //
    // The first slot is managed as two Regions
    //
    const numRegion = ( numSlot > 1 ) ? numSlot + 1 : numSlot + 0;
    
    NNS_GFD_ASSERT( IsSlotNumValid_( numSlot ) );
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        //
        //
        //
        if( i < numRegion )
        {
            vramRegions_[i].bActive = TRUE;
        }else{
            vramRegions_[i].bActive = FALSE;
        }   
        
        //
        // 
        //
        if( vramRegions_[i].bHalfSize )
        {
            ResetRegionHalf_( &vramRegions_[i] );
        }else{
            ResetRegionNormal_( &vramRegions_[i] );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdAllocFrmTexVram

  Description:  Allocates memory.
                
                 
  Arguments:   szByte:  The size
               is4x4comp:  4x4 compressed texture?
               opt:  Implementation dependent parameter (not used)
                            
  Returns:     Texture
  
 *---------------------------------------------------------------------------*/
NNSGfdTexKey NNS_GfdAllocFrmTexVram
(
    u32     szByte, 
    BOOL    is4x4comp, 
    u32     opt 
)
{
#pragma unused(opt)

    u32     addr;
    BOOL    result;
    
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    {
        //
        // If a size is allocated that is too small to be displayed with a texture key, this rounds the size up.
        //
        szByte = NNSi_GfdGetTexKeyRoundupSize( szByte );
        //
        // If a size is allocated that is too large for the texture key to display, this returns an error key.
        //
        if( szByte >= NNS_GFD_TEXSIZE_MAX )
        {
            NNS_GFD_WARNING("Allocation size is too big. : NNS_GfdAllocFrmTexVram()");
            return NNS_GFD_ALLOC_ERROR_TEXKEY;
        }
        
        NNS_GFD_MINMAX_ASSERT( szByte, NNS_GFD_TEXSIZE_MIN, NNS_GFD_TEXSIZE_MAX );
    }
        
    //
    // Tries to allocate.
    //
    if( is4x4comp )
    {
        result = DoAlloc4x4_( szByte, &addr );
    }else{
        result = DoAllocNrm_( szByte, &addr );
    }
    
    
    //
    // If allocate succeeds, converts to key.
    //
    if( result )
    {
        return NNS_GfdMakeTexKey( addr, szByte, is4x4comp );
    }else{
        // Error: Return a TexKey that expresses an error.
        NNS_GFD_WARNING("failure in Vram Allocation. : NNS_GfdAllocFrmTexVram()");
        return NNS_GFD_ALLOC_ERROR_TEXKEY;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdFreeFrmTexVram

  Description:  Frees memory.
                Actually does nothing.
                 
  Arguments:   texKey          :   Texture
                            
  Returns:     Succeed or fail (if succeed 0)
  
 *---------------------------------------------------------------------------*/
int NNS_GfdFreeFrmTexVram( NNSGfdTexKey texKey )
{
#pragma unused(texKey)
    
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    // What region is this memory allocated from?
    
    // In case of Lo Head( 4x4 )
    //        => indexTable also freed at same time.
    
    // Lo Tail( Nrm )
    // Hi Tail( Nrm )
    return 0;
}



/*---------------------------------------------------------------------------*
  Name:         NNS_GfdGetFrmTexVramState

  Description:  Acquires current manager state.
                
                 
  Arguments:   pState          :   Manager state information
                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdGetFrmTexVramState( NNSGfdFrmTexVramState* pState )
{
    int i;
    NNS_GFD_NULL_ASSERT( pState );
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        pState->address[i * 2 + 0] = vramRegions_[i].head;
        pState->address[i * 2 + 1] = vramRegions_[i].tail;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_GfdSetFrmTexVramState

  Description:  Sets specified state information in manager state.
                 
  Arguments:   pState          :   Manager state information
                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdSetFrmTexVramState( const NNSGfdFrmTexVramState* pState )
{
    int i;
    NNS_GFD_NULL_ASSERT( pState );
    NNS_GFD_ASSERTMSG( IsVramManagerValid_(), "Make sure to Initialize the manager." );
    
    for( i = 0; i < NNS_GFD_NUM_TEX_VRAM_REGION; i++ )
    {
        vramRegions_[i].head = pState->address[i * 2 + 0];
        vramRegions_[i].tail = pState->address[i * 2 + 1];
    }
}


