/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_PlttVramMan_Types.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_PlttVramMan_Types.h,v $
  Revision 1.13  01/31/2005 12:08:27 AM  nishida
  Corrected the year of the copyright date.

  Revision 1.12  01/28/2005 05:38:39 AM  kitani_toshikazu
  Added the round up process of the zero value size.

  Revision 1.11  12/22/2004 04:19:59 AM  kitani_toshikazu
  Added the assert condition when a key was created illegally with large size

  Revision 1.10  10/18/2004 12:28:23 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.9  09/22/2004 08:34:16 AM  kitani_toshikazu
  Added NNSi_GfdGetPlttKeyRoundupSize()

  Revision 1.8  09/10/2004 10:46:49 AM  kitani_toshikazu
  Moved the NNS_GFD_4PLTT_MAX_ADDR definition

  Revision 1.7  08/30/2004 12:17:03 AM  kitani_toshikazu
  Corrected a spelling error in the comments

  Revision 1.6  07/15/2004 02:45:30 AM  kitani_toshikazu
  Definition of the key that expresses the allocation error

  Revision 1.5  07/10/2004 04:33:03 AM  nishida
  Revision of names

  Revision 1.4  07/10/2004 02:01:01 AM  nishida
  Changed the API name

  Revision 1.3  06/23/2004 08:19:23 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.2  06/23/2004 08:14:05 AM  kitani_toshikazu
  Renamed the global variables

  Revision 1.1  06/16/2004 05:15:25 AM  kitani_toshikazu
  Palette-related additions. Arrangement of sources.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_PLTTVRAMMAN_TYPES_H_
#define NNS_GFD_PLTTVRAMMAN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

#define NNS_GFD_PLTTMASK_16BIT              0xFFFF


#define NNS_GFD_PLTTKEY_SIZE_SHIFT 3
#define NNS_GFD_PLTTKEY_ADDR_SHIFT 3

#define NNS_GFD_PLTTSIZE_MIN ( 0x1 << NNS_GFD_PLTTKEY_SIZE_SHIFT )
#define NNS_GFD_PLTTSIZE_MAX ( NNS_GFD_PLTTMASK_16BIT << NNS_GFD_PLTTKEY_SIZE_SHIFT )


#define NNS_GFD_ALLOC_ERROR_PLTTKEY         (u32)0x0
#define NNS_GFD_4PLTT_MAX_ADDR              0x10000



//------------------------------------------------------------------------------
//
// NNSGfdPlttKey:
// A 32-bit value that can designate the region of the texture palette slot in 8-byte units.
// Values 0-0xffff can be used as the error value (because its size is 0)
//
// 31                    16  15                         0
// 3 bit right-shifted size  3 bit right-shifted offset
//------------------------------------------------------------------------------
typedef u32 NNSGfdPlttKey;


//------------------------------------------------------------------------------
// NNSGfdFuncAllocPlttVram
// szByte:  Specifies the size to allocate in bytes
// is4pltt: Whether the storage of 4-color palette needs to be possible
// opt:     Argument that depends on implementation (ex Allocation from the front, allocation from the back, etc.)
//------------------------------------------------------------------------------
typedef NNSGfdPlttKey (*NNSGfdFuncAllocPlttVram)(u32 szByte, BOOL is4pltt, u32 opt);




//------------------------------------------------------------------------------
// NNSGfdFuncFreePlttVram
// Designates the key and frees the texture image slot region.
// It needs to be implemented in a way it is not going to be vague even without specifying is4pltt.
// Normal return if the return value is 0. Any other value will result in an implementation dependency error.
//------------------------------------------------------------------------------
typedef int (*NNSGfdFuncFreePlttVram)(NNSGfdPlttKey plttKey);


//------------------------------------------------------------------------------
//
// This may also be changed by the user
//
//------------------------------------------------------------------------------
extern NNSGfdFuncAllocPlttVram  NNS_GfdDefaultFuncAllocPlttVram; 
extern NNSGfdFuncFreePlttVram   NNS_GfdDefaultFuncFreePlttVram;  


//------------------------------------------------------------------------------
//
// The library code will only be able to be accessed via this function
//
//------------------------------------------------------------------------------
static inline NNSGfdPlttKey
NNS_GfdAllocPlttVram(u32 szByte, BOOL is4pltt, u32 opt)
{
    return (*NNS_GfdDefaultFuncAllocPlttVram)( szByte, is4pltt, opt );
}

static inline int
NNS_GfdFreePlttVram(NNSGfdPlttKey plttKey)
{
    return (*NNS_GfdDefaultFuncFreePlttVram)(plttKey);
}


//------------------------------------------------------------------------------
//
// Association of the NNSGfdPlttKey operations
//
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Obtains the rounded size so that NNSGfdPlttKey can be expressed.
static inline u32
NNSi_GfdGetPlttKeyRoundupSize( u32 size )
{
    if( size == 0 )
    {
        return NNS_GFD_PLTTSIZE_MIN;
    }else{
        return (((u32)(size) + ( NNS_GFD_PLTTSIZE_MIN-1 )) & ~( NNS_GFD_PLTTSIZE_MIN-1 ) );
    }
}

//------------------------------------------------------------------------------
static inline NNSGfdPlttKey
NNS_GfdMakePlttKey( u32 addr, u32 size )
{
    // Whether the rounding error is generated
    SDK_ASSERT( (addr & (u32)((0x1 << NNS_GFD_PLTTKEY_ADDR_SHIFT) - 1 )) == 0 );
    SDK_ASSERT( (size & (u32)((0x1 << NNS_GFD_PLTTKEY_SIZE_SHIFT) - 1 )) == 0 );
    
    // Whether the overflow is generated
    SDK_ASSERT( ( (size >> NNS_GFD_PLTTKEY_SIZE_SHIFT) & ~NNS_GFD_PLTTMASK_16BIT ) == 0 );
    SDK_ASSERT( ( (addr >> NNS_GFD_PLTTKEY_ADDR_SHIFT) & ~NNS_GFD_PLTTMASK_16BIT ) == 0 );
    
    return    ( ( size >> NNS_GFD_PLTTKEY_SIZE_SHIFT ) << 16 ) 
            | ( 0xFFFF & ( addr >> NNS_GFD_PLTTKEY_ADDR_SHIFT ) );
}

//------------------------------------------------------------------------------
static inline u32
NNS_GfdGetPlttKeyAddr( NNSGfdPlttKey plttKey )
{
    return (u32)( ( 0x0000FFFF & plttKey )  << NNS_GFD_PLTTKEY_ADDR_SHIFT );
}

//------------------------------------------------------------------------------
static inline u32
NNS_GfdGetPlttKeySize( NNSGfdPlttKey plttKey )
{
    return (u32)( ( ( 0xFFFF0000 & plttKey ) >> 16 ) << NNS_GFD_PLTTKEY_SIZE_SHIFT );
}


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_PLTTVRAMMAN_TYPES_H_

