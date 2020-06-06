/*---------------------------------------------------------------------------*
  Project:  NNS - 2D
  File:     g2d_.h

  Copyright 2004, 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Common_data.h,v $
  Revision 1.11  07/19/2005 12:25:10 AM  kitani_toshikazu
  Revised copyright notation

  Revision 1.10  07/07/2005 06:59:21 AM  kitani_toshikazu
  Expansion of user-extended attributes 

  Revision 1.9  12/21/2004 04:02:11 AM  kitani_toshikazu
  Added NNS_G2D_MINMAX_ASSERT

  Revision 1.8  12/17/2004 07:49:59 AM  kitani_toshikazu
  Defined assert when referencing tools

  Revision 1.7  10/28/2004 12:08:43 AM  kitani_toshikazu
  (none)

  Revision 1.6  08/25/2004 06:57:22 AM  kitani_toshikazu
  Defined bit mark alias

  Revision 1.5  2004/06/23 06:47:52 AM  nishida_kenji
  Fix header comments.

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
#ifndef NNS_G2D_COMMON_DATA_H_
#define NNS_G2D_COMMON_DATA_H_

#include <nitro/types.h>

#ifdef NNS_FROM_TOOL
#ifndef NNS_G2D_ASSERT
    #include <assert.h>
    #define NNS_G2D_ASSERT(x) assert((x))
    #define NNS_G2D_ASSERTMSG(x, y) assert((x))
    #define NNS_G2D_NULL_ASSERT(x) assert(NULL != (x))
    #define NNS_G2D_MINMAX_ASSERT(x,min,max) assert( ( (x) >= (min) )&&( (x) <= (max) ) )
#endif // NNS_G2D_ASSERT
#else  // NNS_FROM_TOOL
    #include "nnsys/g2d/g2d_config.h"
#endif // NNS_FROM_TOOL


#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
#define NNS_G2D_DUMMY_DATA8         (u8)0xCC
#define NNS_G2D_DUMMY_DATA16        (u16)0xBEEF
#define NNS_G2D_DUMMY_DATA32        (u32)0xDEADBEEF
#define NNS_G2D_DUMMY_PTR           (u32)0x00000000

//------------------------------------------------------------------------------
#define NNS_G2D_BLKSIG_NAMELABEL        (u32)('LABL')
#define NNS_G2D_BLKSIG_USEREXT          (u32)('UEXT')

#define NNS_G2D_BIGENDIAN_BITMARK          0xfffe
#define NNS_G2D_LITTLEENDIAN_BITMARK       0xfeff



//------------------------------------------------------------------------------
typedef struct NNSG2dBinaryFileHeader
{
    u32        signature;
    u16        byteOrder;
    u16        version;
    u32        fileSize;
    u16        headerSize;
    u16        dataBlocks;

}NNSG2dBinaryFileHeader;

//------------------------------------------------------------------------------
typedef struct NNSG2dBinaryBlockHeader
{
    u32        kind;
    u32        size;

}NNSG2dBinaryBlockHeader;

//------------------------------------------------------------------------------
// User-extended region block header. 
typedef struct NNSG2dUserExDataBlock
{
    u32        blkTypeID;
    u32        blkSize;

}NNSG2dUserExDataBlock;




//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static inline BOOL NNS_G2dIsPad16Valid( u16 pad16 )
{
    return ( NNS_G2D_DUMMY_DATA16 == pad16 ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
static inline BOOL NNS_G2dIsPad32Valid( u32 pad32 )
{
    return ( NNS_G2D_DUMMY_DATA32 == pad32 ) ? TRUE : FALSE;
}

//------------------------------------------------------------------------------
static inline u16 NNS_G2dMakeVersionData( u8 majorVer, u8 minorVer )
{
    return (u16)((u16)majorVer << 8 | ((u16)minorVer & 0x00FF ));
}

//------------------------------------------------------------------------------
// Gets the user-extended block having the specified ID. 
// pExtendData        Pointer that points to start of user-extended data 
// blkTypeID          user-extended data block ID
//
static inline const NNSG2dUserExDataBlock* NNSi_G2dGetUserExDataBlkByID
(
    const void* pExtendData, 
    u32         blkTypeID 
)
{
    if( pExtendData != NULL )
    {
        const NNSG2dUserExDataBlock* pBlk = (const NNSG2dUserExDataBlock*)pExtendData;    
        
        // Found block with specified ID. 
        if( pBlk->blkTypeID == blkTypeID )
        {
            return pBlk;
        }
        // Go to next block...
        pBlk = (const NNSG2dUserExDataBlock*)((const u8*)pBlk + pBlk->blkSize);        
    }
    return NULL;
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_COMMON_DATA_H_


