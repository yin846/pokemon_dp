/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_TexVramMan_Types.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_TexVramMan_Types.h,v $
  Revision 1.10  01/31/2005 12:08:27 AM  nishida
  Corrected the year of the copyright date.

  Revision 1.9  01/28/2005 05:38:39 AM  kitani_toshikazu
  Added the round up process of the zero value size.

  Revision 1.8  12/22/2004 04:19:59 AM  kitani_toshikazu
  Added the assert condition when a key was created illegally with large size

  Revision 1.7  10/18/2004 12:28:23 PM  kitani_toshikazu
  Standardized parameter names. Fixed comments.

  Revision 1.6  09/22/2004 08:34:04 AM  kitani_toshikazu
  Added NNSi_GfdGetTexKeyRoundupSize()

  Revision 1.5  08/30/2004 12:17:03 AM  kitani_toshikazu
  Corrected a spelling error in the comments

  Revision 1.4  07/15/2004 02:45:30 AM  kitani_toshikazu
  Definition of the key that expresses the allocation error

  Revision 1.3  07/14/2004 04:48:57 AM  kitani_toshikazu
  Modified the bit layout inside the texture key

  Revision 1.2  07/10/2004 04:33:03 AM  nishida
  Revision of names

  Revision 1.1  07/10/2004 01:59:42 AM  nishida
  New registration when streamlining the VRAM manager.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_TEXVRAMMAN_TYPES_H_
#define NNS_GFD_TEXVRAMMAN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

#define NNS_GFD_MASK_15BIT 0x7FFF
#define NNS_GFD_MASK_16BIT 0xFFFF

#define NNS_GFD_TEXKEY_SIZE_SHIFT 4
#define NNS_GFD_TEXKEY_ADDR_SHIFT 3


#define NNS_GFD_TEXSIZE_MIN ( 0x1 << NNS_GFD_TEXKEY_SIZE_SHIFT )
#define NNS_GFD_TEXSIZE_MAX ( 0x7FFF << NNS_GFD_TEXKEY_SIZE_SHIFT )




// The incorrect texture key that failed in allocation
#define NNS_GFD_ALLOC_ERROR_TEXKEY          (u32)0x0


//------------------------------------------------------------------------------
//
// NNSGfdTexKey:
// A 32-bit value which can designate the region for the texture image slot in an 8-byte unit.
// Values 0-0xffff can be used as the error value (because its size is 0)
//
// 31      30                    17  16                         0    
// 4x4Comp 4 bit right-shifted size, 3 bit right-shifted offset
//
//------------------------------------------------------------------------------
typedef u32 NNSGfdTexKey;

//------------------------------------------------------------------------------
// NNSGfdFuncAllocTexVram
// szByte:    Specifies the size to allocate in bytes
// is4x4comp: Whether or not this is for the 4x4comp format
// opt:       Argument that depends on implementation (ex Allocation from the front, allocation from the back, etc.)
//
// When is4x4comp is TRUE, the return value will become the texture image region.
// The texture palette index region must also be separately allocated
//------------------------------------------------------------------------------
typedef NNSGfdTexKey (*NNSGfdFuncAllocTexVram)(u32 szByte, BOOL is4x4comp, u32 opt);




//------------------------------------------------------------------------------
// NNSGfdFuncFreeTexVram
// Designates the key and deallocates the texture image slot region.
// There must be an implementation so that there is no vagueness even if the is4x4comp is not designated.
// Normal return if the return value is 0. Any other value will result in an implementation dependency error.
//------------------------------------------------------------------------------
typedef int (*NNSGfdFuncFreeTexVram)(NNSGfdTexKey key);


//------------------------------------------------------------------------------
//
// This may also be changed by the user
//
//------------------------------------------------------------------------------
extern NNSGfdFuncAllocTexVram   NNS_GfdDefaultFuncAllocTexVram; 
extern NNSGfdFuncFreeTexVram    NNS_GfdDefaultFuncFreeTexVram;  


//------------------------------------------------------------------------------
//
// This is like a macro
// The library code will be accessed only via this function
//
//------------------------------------------------------------------------------
static inline NNSGfdTexKey
NNS_GfdAllocTexVram(u32 szByte, BOOL is4x4comp, u32 opt)
{
    return (*NNS_GfdDefaultFuncAllocTexVram)(szByte, is4x4comp, opt );
}

//------------------------------------------------------------------------------
//
// This is like a macro
// The library code will be accessed only via this function
//
//------------------------------------------------------------------------------
static inline int
NNS_GfdFreeTexVram(NNSGfdTexKey memKey)
{
    return (*NNS_GfdDefaultFuncFreeTexVram)(memKey);
}



//------------------------------------------------------------------------------
//
// Association of NNSGfdTexKey operations
//
//
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Obtains the rounded size so that NNSGfdTexKey can be expressed.
static inline u32
NNSi_GfdGetTexKeyRoundupSize( u32 size )
{
    if( size == 0 )
    {
        return NNS_GFD_TEXSIZE_MIN;
    }else{
        
        return (((u32)(size) + ( NNS_GFD_TEXSIZE_MIN-1 )) & ~( NNS_GFD_TEXSIZE_MIN-1 ) );
    }
}    

//------------------------------------------------------------------------------
static inline NNSGfdTexKey
NNS_GfdMakeTexKey( u32 addr, u32 size, BOOL b4x4Comp )
{
    // Whether the rounding error is generated
    SDK_ASSERT( (addr & (u32)((0x1 << NNS_GFD_TEXKEY_ADDR_SHIFT) - 1 )) == 0 );
    SDK_ASSERT( (size & (u32)((0x1 << NNS_GFD_TEXKEY_SIZE_SHIFT) - 1 )) == 0 );
    
    // Whether the overflow is generated
    SDK_ASSERT( ( (size >> NNS_GFD_TEXKEY_SIZE_SHIFT) & ~NNS_GFD_MASK_15BIT ) == 0 );
    SDK_ASSERT( ( (addr >> NNS_GFD_TEXKEY_ADDR_SHIFT) & ~NNS_GFD_MASK_16BIT ) == 0 );
    
    return  ( ( size >> NNS_GFD_TEXKEY_SIZE_SHIFT ) << 16 ) 
            | ( ( NNS_GFD_MASK_16BIT & ( addr >> NNS_GFD_TEXKEY_ADDR_SHIFT ) ) ) 
            | b4x4Comp << 31;
}

//------------------------------------------------------------------------------
static inline u32
NNS_GfdGetTexKeyAddr( NNSGfdTexKey memKey )
{
    return (u32)( ( ( 0x0000FFFF & memKey ) ) << NNS_GFD_TEXKEY_ADDR_SHIFT );
}

//------------------------------------------------------------------------------
static inline u32
NNS_GfdGetTexKeySize( NNSGfdTexKey memKey )
{
    return (u32)( ( ( 0x7FFF0000 & memKey ) >> 16 ) << NNS_GFD_TEXKEY_SIZE_SHIFT );
}

//------------------------------------------------------------------------------
static inline BOOL
NNS_GfdGetTexKey4x4Flag( NNSGfdTexKey memKey )
{
    return (BOOL)( (0x80000000 & memKey) >> 31 );
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_TEXVRAMMAN_TYPES_H_

