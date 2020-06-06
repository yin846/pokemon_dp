/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_FramePlttVramMan.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_FramePlttVramMan.c,v $
  Revision 1.15  08/17/2005 11:51:48 PM  kitani_toshikazu
  Revised copyright notation

  Revision 1.14  08/17/2005 08:09:56 AM  kitani_toshikazu
  Added a function for debug display

  Revision 1.13  12/22/2004 04:18:51 AM  kitani_toshikazu
  Added a process that returns an error key for a request that would allocate an illegally large area.

  Revision 1.12  11/22/2004 08:46:00 AM  kitani_toshikazu
  Changed internal functions to inline functions

  Revision 1.11  10/18/2004 12:27:53 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.10  09/29/2004 02:03:23 AM  kitani_toshikazu
  Added comments

  Revision 1.9  09/22/2004 08:28:15 AM  kitani_toshikazu
  Described the rounding process of size information when allocated

  Revision 1.8  09/10/2004 10:45:35 AM  kitani_toshikazu
  Moved the NNS_GFD_4PLTT_MAX_ADDR definition

  Revision 1.7  09/02/2004 11:53:02 PM  kitani_toshikazu
  Revised misspelling

  Revision 1.6  08/30/2004 12:16:17 AM  kitani_toshikazu
  Corrected a spelling error in the comments

  Revision 1.5  07/15/2004 08:00:17 AM  kitani_toshikazu
  Fixed alignment process bug.

  Revision 1.4  07/15/2004 02:50:13 AM  kitani_toshikazu
  Supported key that expresses allocation error.

  Revision 1.3  2004/07/15 02:19:56 AM  nishida
  Corrected bug that caused AllocFromLo_ to return incorrect address.

  Revision 1.2  07/10/2004 04:31:46 AM  nishida
  Made revisions for the configuration of whether or not to use as the default at initialization

  Revision 1.1  07/10/2004 02:02:48 AM  nishida
  New registration when streamlining the VRAM manager.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nnsys/gfd/gfd_common.h>
#include <nnsys/gfd/VramManager/gfd_VramMan.h>
#include <nnsys/gfd/VramManager/gfd_FramePlttVramMan.h>

#define NNS_GFD_TEXPLTT_BASE_ADDR	0x00000
#define NNS_GFD_TEXPLTT_MAX_ADDR    0x18000


//------------------------------------------------------------------------------
typedef struct NNSGfdFramPlttVramManager
{
    u32     loAddr;
    u32     hiAddr;
    u32     szTotal;
    
} NNSGfdFrmPlttVramManager;

//------------------------------------------------------------------------------
static NNSGfdFrmPlttVramManager       s_managerState_ = { 0, 0, 0 };
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Gets the alignment boundary size for each format.
// Alignment offset in the high order direction
NNS_GFD_INLINE u32 GetUpperSideAlignmentSize_( u32 addr, BOOL b4Pltt )
{
    if( b4Pltt ) 
    {
        return 0x07 & (0x08 - (addr & 0x07));
    }else{
        return 0x0F & (0x10 - (addr & 0x0F));
    }
}

// Alignment offset in the low order direction
NNS_GFD_INLINE u32 GetLowerSideAlignmentSize_( u32 addr, BOOL b4Pltt )
{
    if( b4Pltt ) 
    {
        return addr & 0x07;
    }else{
        return addr & 0x0F;
    }
}

//------------------------------------------------------------------------------
// Checks if the manager is in a valid state.
static BOOL IsVramManagerValid_( )
{
    if( ( s_managerState_.loAddr <= s_managerState_.hiAddr ) && 
          s_managerState_.szTotal != 0 )
    {
        return TRUE;
    }else{
        return FALSE;
    }
}

// Acquire size.
//------------------------------------------------------------------------------
// Acquires remaining capacity.
NNS_GFD_INLINE u32 GetCapacity_()
{
    NNS_GFD_ASSERT( IsVramManagerValid_( ) );
    
    return (u32)( s_managerState_.hiAddr - s_managerState_.loAddr );
}



// Acquire
//------------------------------------------------------------------------------
NNS_GFD_INLINE BOOL AllocFromLo_( u32 szByte, BOOL b4Pltt, u32* pResAddr )
{
    const u32 addr          = s_managerState_.loAddr;
    const u32 alignment     = GetUpperSideAlignmentSize_( addr, b4Pltt );// Align appropriate for the format
    const u32 increase      = szByte + alignment;
    
    
    NNS_GFD_ASSERT( szByte != 0 );
    NNS_GFD_ASSERT( IsVramManagerValid_( ) );
    
    if( GetCapacity_() >= increase )
    {   
        const u32 tail          = s_managerState_.loAddr + increase;
        //
        // 4 color palette has a region that cannot be referenced.
        // 
        if( b4Pltt )
        {
            if( tail > NNS_GFD_4PLTT_MAX_ADDR )
            {
                // Error
                return FALSE;
            }
        }
         
        *pResAddr               = s_managerState_.loAddr + alignment;
        s_managerState_.loAddr += increase;
        
        return TRUE;
    }else{
        
        return FALSE;
    }
}



//------------------------------------------------------------------------------
NNS_GFD_INLINE BOOL AllocFromHi_( u32 szByte, BOOL b4Pltt, u32* pResAddr )
{
    NNS_GFD_ASSERT( szByte != 0 );
    NNS_GFD_ASSERT( IsVramManagerValid_( ) );
    
    if( s_managerState_.hiAddr >= szByte )
    {
        const u32 addr          = (u32)(s_managerState_.hiAddr - szByte);
        const u32 alignment     = GetLowerSideAlignmentSize_( addr, b4Pltt );
        const u32 increase      = szByte + alignment;
        
        //OS_Printf("addr = %d\n", addr );
        //OS_Printf("alignment = %d\n", alignment );
        //OS_Printf("increase = %d\n", increase );
        
        if( GetCapacity_() >= increase )
        {    
            const u32 tail          = s_managerState_.hiAddr;
            
            if( b4Pltt )
            {
                if( tail > NNS_GFD_4PLTT_MAX_ADDR )
                {
                    // Error
                    return FALSE;
                }
            }
            
            s_managerState_.hiAddr -= increase;
            *pResAddr               = s_managerState_.hiAddr;
            
            return TRUE;
        }
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdDumpFrmPlttVramManager

  Description:  Manager contents are output for debugging
                
                 
  Arguments:   None

                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdDumpFrmPlttVramManager()
{
    const u32 szFree = s_managerState_.hiAddr - s_managerState_.loAddr;
    
    OS_Printf("===== NNS_Gfd FrmPlttVramManager Dump ====\n");
    OS_Printf("head-Addr   : tail-addr   : free-size \n");
    
    
    OS_Printf("0x%08x  : 0x%08x  : 0x%08x  \n", 
                s_managerState_.loAddr, 
                s_managerState_.hiAddr, 
                szFree );
    
    OS_Printf("    %08d / %08d bytes (%6.2f%%) used \n", 
                s_managerState_.szTotal - szFree, 
                s_managerState_.szTotal, 
                100.0f * ( s_managerState_.szTotal - szFree ) / s_managerState_.szTotal ); 
    OS_Printf("==========================================\n");
}


/*---------------------------------------------------------------------------*
  Name:         NNS_GfdInitFrmPlttVramManager

  Description:  Initializes manager.
                
                 
  Arguments:   szByte       The number of bytes of palette RAM managed by the frame palette VRAM manager. 
               useAsDefault: Represents whether or not to use the frame palette VRAM manager by default. 

                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdInitFrmPlttVramManager( u32 szByte, BOOL useAsDefault )
{
    NNS_GFD_MINMAX_ASSERT( szByte, 0, NNS_GFD_TEXPLTT_MAX_ADDR );
     
    s_managerState_.szTotal = szByte;
    
    NNS_GfdResetFrmPlttVramState();
    
    NNS_GFD_ASSERT( IsVramManagerValid_( ) );

	if (useAsDefault)
	{
		NNS_GfdDefaultFuncAllocPlttVram = NNS_GfdAllocFrmPlttVram;
		NNS_GfdDefaultFuncFreePlttVram  = NNS_GfdFreeFrmPlttVram;
	}
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdAllocFrmPlttVram

  Description:  Allocates memory.
                
                 
  Arguments:   szByte:  The size
               bPltt4           :  4 color texture?
               allocFrom        :  Allocate from the top of the region (lower position)?
                            
  Returns:     Texture palette key
  
 *---------------------------------------------------------------------------*/
NNSGfdPlttKey NNS_GfdAllocFrmPlttVram
( 
    u32         szByte, 
    BOOL        bPltt4, 
    u32         allocFrom
)
{
    u32     addr    = 0x0;
    BOOL    result  = FALSE;
    
    NNS_GFD_ASSERT( IsVramManagerValid_( ) );
    
    {
        //
        // If a size is allocated that is too small to be displayed with a palette key, this rounds the size up.
        //
        szByte = NNSi_GfdGetPlttKeyRoundupSize( szByte );
        //
        // If a size is allocated that is too large for the palette key to display, this returns an error key.
        //
        if( szByte >= NNS_GFD_PLTTSIZE_MAX )
        {
            NNS_GFD_WARNING("Allocation size is too big. : NNS_GfdAllocLnkPlttVram()");
            return NNS_GFD_ALLOC_ERROR_PLTTKEY;
        }
        
        NNS_GFD_MINMAX_ASSERT( szByte, NNS_GFD_PLTTSIZE_MIN, NNS_GFD_PLTTSIZE_MAX );
    }
    
    if( allocFrom == NNS_GFD_ALLOC_FROM_LOW )
    {
        result = AllocFromLo_( szByte, bPltt4, &addr );
    }else{
        result = AllocFromHi_( szByte, bPltt4, &addr );
    }
    
    
    if( result )
    {   
        //
        // Generate TexPlttKey
        //
        return NNS_GfdMakePlttKey( addr, szByte );
    }
        
NG_CASE:
    // Error
    NNS_GFD_WARNING("failure in Vram Allocation. : NNS_GfdPlttFrmTexAlloc()");
    return NNS_GFD_ALLOC_ERROR_PLTTKEY;
    
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdFreeFrmPlttVram

  Description:  Frees memory.
                Actually does nothing.
                 
  Arguments:   plttKey          :   Texture palette key
                            
  Returns:     Succeed or fail (if succeed 0)
  
 *---------------------------------------------------------------------------*/
int NNS_GfdFreeFrmPlttVram( NNSGfdPlttKey plttKey )
{
#pragma unused(plttKey)
    // What region is this memory allocated from?
    
    // In case of Lo Head( 4x4 )
    //        => indexTable also freed at same time.
    
    // Lo Tail( Nrm )
    // Hi Tail( Nrm )
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdGetPlttFrmManagerState

  Description:  Acquires current manager state.
                
                 
  Arguments:   pState          :   Manager state information
                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdGetFrmPlttVramState( NNSGfdFrmPlttVramState* pState )
{
    NNS_GFD_NULL_ASSERT( pState );
    NNS_GFD_ASSERT( IsVramManagerValid_( ) );
    
    pState->address[0] = s_managerState_.loAddr;
    pState->address[1] = s_managerState_.hiAddr;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_GfdSetFrmPlttVramState

  Description:  Sets specified state information in manager state.
                 
  Arguments:   pState          :   Manager state information
                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdSetFrmPlttVramState( const NNSGfdFrmPlttVramState* pState )
{
    NNS_GFD_NULL_ASSERT( pState );
    
    s_managerState_.loAddr = pState->address[0];
    s_managerState_.hiAddr = pState->address[1];
}

/*---------------------------------------------------------------------------*
  Name:         NNS_GfdResetPlttFrmManagerState

  Description:  Resets manager's current state.
                 
  Arguments:   None
                            
  Returns:     None
  
 *---------------------------------------------------------------------------*/
void NNS_GfdResetFrmPlttVramState()
{ 
    s_managerState_.loAddr = NNS_GFD_TEXPLTT_BASE_ADDR;
    s_managerState_.hiAddr = s_managerState_.szTotal;
}


