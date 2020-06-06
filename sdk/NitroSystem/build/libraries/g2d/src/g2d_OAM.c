/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_OAM.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_OAM.c,v $
  Revision 1.38  07/06/2005 12:50:08 PM  kitani_toshikazu
  Changed processing for when OBJ is input in NNS_G2dEntryOamManagerOamWithAffineIdx() with the affine transform enable flag not set. 
  

  Revision 1.37  06/29/2005 08:30:39 AM  kitani_toshikazu
  Corrected comment

  Revision 1.36  06/29/2005 01:01:36 AM  kitani_toshikazu
  Fixed comments.

  Revision 1.35  04/28/2005 02:31:09 AM  kitani_toshikazu
  Replaced NNSi_G2dRepeatXinCellSpace()
  with NNS_G2dRepeatXinCellSpace

  Revision 1.34  01/24/2005 06:26:20 AM  kitani_toshikazu
  Corrected the copyright statement.

  Revision 1.33  01/19/2005 07:40:07 AM  kitani_toshikazu
  Provisionally added definitions for the table declared in g2d_Oam_data.h

  Revision 1.32  10/28/2004 12:05:06 AM  kitani_toshikazu
  Made adjustment to handle renamed functions

  Revision 1.31  10/21/2004 11:01:03 AM  kitani_toshikazu
  Revised to optimize performance

  Revision 1.30  10/19/2004 11:57:53 AM  kitani_toshikazu
  Changed internal functions to inline functions

  Revision 1.29  10/18/2004 09:47:54 AM  kitani_toshikazu
  Revised comments (standardized terminology)

  Revision 1.28  10/18/2004 05:51:53 AM  kitani_toshikazu
  Revised comments.

  Revision 1.27  10/14/2004 12:58:30 AM  kitani_toshikazu
  Dealt with the specification change regarding
  saving software sprite module and current matrix status

  Revision 1.26  10/08/2004 09:04:02 AM  kitani_toshikazu
  Fixed bugs with the NNSi_G2dIsRSEnable() determination section in DrawBy3DGraphicsEngine_.

  Revision 1.25  09/15/2004 05:51:20 AM  kitani_toshikazu
  Transferred the affine transformation mode setting process out of the manager

  Revision 1.24  08/26/2004 07:20:53 AM  kitani_toshikazu
  Implementation of the Z offset features

  Revision 1.23  08/17/2004 06:02:21 AM  kitani_toshikazu
  Implementation of the automatic Z value offset features

  Revision 1.22  08/04/2004 05:03:14 AM  kitani_toshikazu
  Modified so the affine parameters do not reset during buffer reset

  Revision 1.21  07/29/2004 02:49:54 AM  kitani_toshikazu
  Added NNSi_G2dGetOamManagerInternalBufferForDebug()

  Revision 1.20  07/27/2004 08:57:21 AM  hatamoto_minoru
  Corrected the ASSERT conditions for NNS_G2dGetNewOamManagerInstance*.

  Revision 1.19  07/26/2004 04:44:33 AM  kitani_toshikazu
  Changed function names.

  Revision 1.18  07/24/2004 02:48:14 AM  kitani_toshikazu
  Added API.

  Revision 1.17  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.16  07/17/2004 07:23:46 AM  kitani_toshikazu
  Changed file name from g2d_OamSoftwareSpriteEmu to g2d_OamSoftwareSpriteDraw.

  Revision 1.15  07/17/2004 06:55:16 AM  kitani_toshikazu
  Separated OAM software emulation drawing relation as separate module.

  Revision 1.14  07/14/2004 12:21:58 PM  kitani_toshikazu
  Added support for multiple types of 1D mapping modes

  Revision 1.13  06/22/2004 09:08:13 AM  kitani_toshikazu
  Added NNS_G2dGetNewManagerInstanceNew()

  Revision 1.12  06/22/2004 06:35:25 AM  kitani_toshikazu
  Added NNS_G2dDrawOneOam3DDirectWithPos()

  Revision 1.11  06/17/2004 08:29:25 AM  hatamoto_minoru
  Corrected a bug in GetNumOamUsed_().

  Revision 1.10  06/11/2004 04:56:43 AM  kitani_toshikazu
  NNS_G2dEntryOamManagerAffine()
  Correct NNS_G2dApplyOamManagerToHWSprite() bug
  

  Revision 1.9  06/02/2004 09:55:08 AM  kitani_toshikazu
  Corrected a bug in NNS_G2dResetBuffe()

  Revision 1.8  06/01/2004 05:20:57 AM  kitani_toshikazu
  Changes for using expansion palette.

  Revision 1.7  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.6  05/10/2004 07:25:18 AM  kitani_toshikazu
  fix some bugs.

  Revision 1.5  05/10/2004 04:42:46 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.4  04/21/2004 11:49:04 AM  kitani_toshikazu
  Renamed functions

  Revision 1.3  04/21/2004 07:11:27 AM  kitani_toshikazu
  Updated the folder structures related to the internal module

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_OAM.h>
#include <nnsys/g2d/g2d_Softsprite.h>

#include <nnsys/g2d/g2d_OamSoftwareSpriteDraw.h>

#include <nnsys/g2d/fmt/g2d_Cell_data.h>

#include "g2d_Internal.h"


//
// Defines the table instance declared in g2d_Oam_data.h
// This location may not be appropriate.
// It may be moved to another file.
//
NNS_G2D_DEFINE_NNSI_OBJSIZEWTBL;
NNS_G2D_DEFINE_NNSI_OBJSIZEHTBL;



//------------------------------------------------------------------------------
#define OAM_SETTING_INVISIBLE   192     // Invisible setting for OAM Attribute


#define NUM_HW_OAM_ATTR         128     // Number of OAM Attributes
#define NUM_HW_OAM_AFFINE       32      // Number of Affine parameters
#define NUM_OAM_TYPES           3       // OAM types

// Nobody is using the OAM area (standard values for the reservation table used)
#define OAM_NOT_USED            0xFFFF  

#define GX_AFFINE_SIZE          sizeof( GXOamAffine )
#define GX_OAMATTR_SIZE         sizeof( GXOamAttr )
#define OAMATTR_SIZE            sizeof( u16 ) * 3

//
// Function used when loading an OBJ
//
typedef void (OBJLoadFunction)( const void *pSrc, u32 offset, u32 szByte );






//------------------------------------------------------------------------------
// Operation cache to the OAM in the OAM Manager
// 
typedef struct OamAttributeCache
{
    u16             reservationTable[NUM_HW_OAM_ATTR]; // The reservation table used
    GXOamAttr       oamBuffer       [NUM_HW_OAM_ATTR]; // OAM Attribute buffer
    
}OamAttributeCache;

//------------------------------------------------------------------------------
// Operation cache to Affine parameters
// OamAttributeCache and oamBuffer are shared.
// 
typedef struct OamAffineCache
{
    u16             reservationTable[NUM_HW_OAM_AFFINE]; // The AffineParameter reservation table used
    
}OamAffineCache;

//------------------------------------------------------------------------------
// Operation cache to the OAM in the OAM Manager
// 
// Operations for the manager OAM are temporarily stopped by this structure.
// The user needs to reflect the contents of the cache in HW
// by calling NNS_G2dApplyToHWXXX().
// 
typedef struct OamCache
{
    OamAttributeCache           attributeCache; // OamAttr
    OamAffineCache              affineCache;    // Affine params
    
}OamCache;




//------------------------------------------------------------------------------
// Unique OamCache entity
// Initialized by NNS_G2dInitOamManagerModule().
// 
static OamCache                        oamCache_[NUM_OAM_TYPES];
static u16                             numRegisterdInstance_ = 0x0;











//------------------------------------------------------------------------------
// Determines whether the management area is active
NNS_G2D_INLINE BOOL IsManageAreaValid_( const NNSG2dOAMManageArea* pArea )
{
    NNS_G2D_NULL_ASSERT( pArea );
    
    return (BOOL)( ( pArea->currentIdx <= pArea->toIdx + 1 ) &&
                   ( pArea->fromIdx <= pArea->toIdx ) );
}

//------------------------------------------------------------------------------
// Gets the pointer for the specified type of OamCache.
// 
NNS_G2D_INLINE OamCache* GetOamCachePtr_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    return &oamCache_[type];
}



//------------------------------------------------------------------------------
// Gets the pointer for the specified type of management area reservation table (for OamAttr).
NNS_G2D_INLINE u16* GetOamReservationTblPtr_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    {        
        OamCache* pOamCache = GetOamCachePtr_( type );
        NNS_G2D_NULL_ASSERT( pOamCache );
    
        return pOamCache->attributeCache.reservationTable;
    }
}
//------------------------------------------------------------------------------
// Gets the pointer for the specified type of management area reservation table (for affine Parameter)
NNS_G2D_INLINE u16* GetAffineReservationTblPtr_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    {
        OamCache* pOamCache = GetOamCachePtr_( type );
        NNS_G2D_NULL_ASSERT( pOamCache );
        
        return pOamCache->affineCache.reservationTable;
    }
}

//------------------------------------------------------------------------------
// Gets the pointer to the OamAttr buffer index number.
NNS_G2D_INLINE GXOamAttr* GetOamBufferPtr_( NNSG2dOamType type, u16 index )
{
    ASSERT_OAMTYPE_VALID( type );
    {
        OamCache* pOamCache = GetOamCachePtr_( type );
        NNS_G2D_NULL_ASSERT( pOamCache );
        
        return pOamCache->attributeCache.oamBuffer + index;
    }
}

//------------------------------------------------------------------------------
// Gets the pointer to the Oam Affine parameter buffer index number.
// Internally calls GetOamBufferPtr_().
NNS_G2D_INLINE GXOamAffine* GetAffineBufferPtr_( NNSG2dOamType type, u16 index )
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_MINMAX_ASSERT   ( index, 0 , NUM_HW_OAM_AFFINE );
    
    {
        GXOamAffine* pAff =  (GXOamAffine*)GetOamBufferPtr_( type, 0 );
        NNS_G2D_NULL_ASSERT( pAff );
        
        return pAff + index;
    }
}

//------------------------------------------------------------------------------
// Gets the affine parameter
NNS_G2D_INLINE void GetAffineParams_( NNSG2dOamType type, u16 idx, MtxFx22* pMtx )
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_MINMAX_ASSERT( idx, 0, NUM_HW_OAM_AFFINE );
    NNS_G2D_NULL_ASSERT( pMtx );
    
    {    
        GXOamAffine* pAff = GetAffineBufferPtr_( type, idx );
        NNS_G2D_NULL_ASSERT( pAff );
        
        // Affine transform parmeters (PA,PB,PC,PD) are in s7.8 format.
        // Since fx32 is in s19.12 format, shift them right by 4 bits.
        pMtx->_00 = (s16)(pAff->PA << 4);
        pMtx->_01 = (s16)(pAff->PB << 4);
        pMtx->_10 = (s16)(pAff->PC << 4);
        pMtx->_11 = (s16)(pAff->PD << 4);
    }
}

//------------------------------------------------------------------------------
// Examines whether the Oam management area is used by another entity.
NNS_G2D_INLINE BOOL IsOamNotUsed_( u16* pResvTblHead, u16 from, u16 to )
{
    const u16*    pCursor   = pResvTblHead + from;
    const u16*    pEnd      = pResvTblHead + to;
    
    while( pCursor <= pEnd )
    {
        if( *pCursor != OAM_NOT_USED )
        {
            return FALSE;
        }
        pCursor++;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
// Gets a new GUID.
NNS_G2D_INLINE u16 GetNewGUID_()
{
    return numRegisterdInstance_++;
}

//------------------------------------------------------------------------------
// Reserves an area for ownerGUID management.
// 
// The target area needs to be free of use by other managers.
// Use IsOamNotUsed_() to check.
// Assert fails if the conditions are not met.
// 
NNS_G2D_INLINE void DoReserveArea_( u16* pResvTblHead, u16 from, u16 to, u16 ownerGUID )
{
    NNS_G2D_NULL_ASSERT( pResvTblHead );
    NNS_G2D_ASSERT( from <= to );// MUST BE !    
    // Check if they aren't going to Reserve others area.
    NNS_G2D_ASSERT( IsOamNotUsed_( pResvTblHead, from, to ) );
    
    
    
    
    NNSI_G2D_DEBUGMSG0( "Oam Reservation occur... from %d to %d by GUID %d \n", 
                         from, 
                         to, 
                         ownerGUID );
               
               
    
    MI_CpuFill16( pResvTblHead + from, ownerGUID, sizeof(u16)*(u32)(to - from) );
}

//------------------------------------------------------------------------------
// Forcibly sets the reservation area to an unused state.
NNS_G2D_INLINE void DoRestoreArea_( u16* pResvTblHead, u16 from, u16 to )
{
    NNS_G2D_NULL_ASSERT( pResvTblHead );
    NNS_G2D_ASSERT( from <= to );// MUST BE !    
    
    NNSI_G2D_DEBUGMSG0( "Oam Restoration occur... from %d to %d \n" , from, to );
    
    MI_CpuFill16( pResvTblHead + from, OAM_NOT_USED, sizeof(u16)*(u32)(to - from) );
}
    
//------------------------------------------------------------------------------
// Gets excess management area.
NNS_G2D_INLINE u16 GetCapacity_( const NNSG2dOAMManageArea* pArea )
{
    NNS_G2D_NULL_ASSERT( pArea );
//  NNS_G2D_ASSERT( IsManageAreaValid_( pArea ) );
    
    if( IsManageAreaValid_( pArea ) )
    {
        return (u16)( (int)pArea->toIdx - pArea->currentIdx + 1);
    }else{ 
        return 0;
    }
}

//------------------------------------------------------------------------------
// Gets the number of management area uses.
NNS_G2D_INLINE u16 GetNumOamUsed_( const NNSG2dOAMManageArea* pArea )
{
    NNS_G2D_NULL_ASSERT( pArea );
//  NNS_G2D_ASSERT( IsManageAreaValid_( pArea ) );

    if( IsManageAreaValid_( pArea ) )
    {
        return (u16)( (int)pArea->currentIdx - pArea->fromIdx);
    }else{ 
        return 0;
    }
}

//------------------------------------------------------------------------------
// Checks whether the management area has enough capacity.
NNS_G2D_INLINE BOOL HasEnoughCapacity_( const NNSG2dOAMManageArea* pArea, u16 num )
{
    NNS_G2D_NULL_ASSERT( pArea );
    NNS_G2D_ASSERT( num != 0 );
    
    return (BOOL)( GetCapacity_( pArea ) >= num );
}


//------------------------------------------------------------------------------
// Gets the pointer to the current position of the Oam buffer.
NNS_G2D_INLINE GXOamAttr* GetOamCurrentPtr_( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( IsManageAreaValid_( &pMan->managedAttrArea ) );
    NNS_G2D_ASSERT( pMan->managedAttrArea.toIdx < NUM_HW_OAM_ATTR );
    {            
            
        GXOamAttr* pret = GetOamBufferPtr_( pMan->type, 
                                            pMan->managedAttrArea.currentIdx );
                                            
        NNS_G2D_NULL_ASSERT( pret );
        
        return pret;
    }
}

//------------------------------------------------------------------------------
// Gets the pointer to the position at the top of the Oam buffer management area.
NNS_G2D_INLINE GXOamAttr* GetOamFromPtr_( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    {
        GXOamAttr* pFrom = GetOamBufferPtr_( pMan->type, 
                                             pMan->managedAttrArea.fromIdx );
        NNS_G2D_NULL_ASSERT( pFrom );
        
        
        return pFrom;
    }
}

//------------------------------------------------------------------------------
// Gets the size of the management area in bytes.
NNS_G2D_INLINE u32 GetSizeOfManageArea_( const NNSG2dOAMManageArea* pArea )
{
    NNS_G2D_NULL_ASSERT( pArea );
//  NNS_G2D_ASSERT( IsManageAreaValid_( pArea ) );
    
    if( IsManageAreaValid_( pArea ) )
    {
        return (u32)(GX_OAMATTR_SIZE * (u16)(pArea->toIdx - pArea->fromIdx + 1));
    }else{
        return 0;
    }
        
}

//------------------------------------------------------------------------------
// Sets the affine parameter
NNS_G2D_INLINE void SetAffineParams_( NNSG2dOamType type, const MtxFx22* mtx, u16 idx )
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_NULL_ASSERT( mtx );
    NNS_G2D_MINMAX_ASSERT( idx, 0, NUM_HW_OAM_AFFINE );
    
    
    {    
        GXOamAffine* pAff = GetAffineBufferPtr_( type, idx );
        NNS_G2D_NULL_ASSERT( pAff );
        
        G2_SetOBJAffine( pAff, mtx );
    }
    
}



//------------------------------------------------------------------------------
// Draws GXOamAttr using 3DGraphicsEngine_.
// 
// When using Affine Transform, care is needed in handling the Scale parameter.
// 
static void DrawBy3DGraphicsEngine_
( 
    const GXOamAttr*                pOam, 
    u16                             numOam, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    MtxFx22                 mtx;
    u16                     affineIdx;
    s16 posX;
    s16 posY;
    s16 posZ;
    
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    NNS_G2D_MINMAX_ASSERT( numOam, 0, NUM_HW_OAM_ATTR );
    
    G3_PushMtx();
    {
        int i = 0; 
        for( i = 0; i < numOam; i++ )
        {
            posX = NNSi_G2dRepeatXinScreenArea( NNSi_G2dGetOamX( &pOam[i] ) );
            posY = NNSi_G2dRepeatYinScreenArea( NNSi_G2dGetOamY( &pOam[i] ) );
            posZ = -1;
            
            // 
            // Cautions
            // Software sprite drawing function is influenced by current matrix
            // Also, the current matrix after drawing will be changed (not saved).
            // 
            G3_Identity();
            
            if( NNSi_G2dIsRSEnable( &pOam[i] ) )
            {
                //
                // Gets the affine parameter
                // 
                affineIdx = NNSi_G2dGetAffineIdx( &pOam[i] );
                NNS_G2D_MINMAX_ASSERT( affineIdx, 0, NUM_HW_OAM_AFFINE );
                GetAffineParams_( NNS_G2D_OAMTYPE_SOFTWAREEMULATION, affineIdx, &mtx );
                
                NNS_G2dDrawOneOam3DDirectWithPosAffineFast( posX, posY, posZ, &pOam[i], pTexImageAttr, texBaseAddr, pltBaseAddr, &mtx );
            }else{
                NNS_G2dDrawOneOam3DDirectWithPosFast( posX, posY, posZ, &pOam[i], pTexImageAttr, texBaseAddr, pltBaseAddr );
            }
        }
    }    
    G3_PopMtx( 1 );
}
    


//------------------------------------------------------------------------------
// Initializes the Oam buffer with the default value.
NNS_G2D_INLINE void ClearOamByDefaultValue_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    
    MI_CpuFill16( GetOamBufferPtr_( type, 0 ),                        
                  OAM_SETTING_INVISIBLE, 
                  GX_OAMATTR_SIZE * NUM_HW_OAM_ATTR );
}

//------------------------------------------------------------------------------
// Initializes the Oam management area reservation table as unused.
NNS_G2D_INLINE void SetOamReservationTblNotUsed_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    
    DoRestoreArea_( GetOamReservationTblPtr_( type ),             
                    0, 
                    NUM_HW_OAM_ATTR );
}

//------------------------------------------------------------------------------
// Initializes the Affine parameter management area reservation table as unused.
NNS_G2D_INLINE void SetAffineReservationTblNotUsed_( NNSG2dOamType type )
{
    ASSERT_OAMTYPE_VALID( type );
    
    DoRestoreArea_( GetAffineReservationTblPtr_( type ),             
                    0, 
                    NUM_HW_OAM_AFFINE );
}
        
//------------------------------------------------------------------------------
// OAM attribute transfer by CpuCopy (for 2D Graphics Engine Main)
// Created because a need arose to transfer in small blocks
NNS_G2D_INLINE void CpuLoadOAMMain_(
    const void *pSrc,
    u32 offset,
    u32 szByte)
{
    NNS_G2D_NULL_ASSERT(pSrc);
    NNS_G2D_ASSERT(offset + szByte <= HW_OAM_SIZE);
    
    MI_CpuCopy16( pSrc, (void *)(HW_OAM + offset), szByte);
}

//------------------------------------------------------------------------------
// OAM attribute transfer by CpuCopy (for 2D Graphics Engine Sub)
// Created because a need arose to transfer data in small blocks
NNS_G2D_INLINE void CpuLoadOAMSub_(
    const void *pSrc,
    u32 offset,
    u32 szByte)
{
    NNS_G2D_NULL_ASSERT(pSrc);
    NNS_G2D_ASSERT(offset + szByte <= HW_OAM_SIZE);

    MI_CpuCopy16( pSrc, (void *)(HW_DB_OAM + offset), szByte);
}

//------------------------------------------------------------------------------
// Gets the pointer to the appropriate transfer function pointer from the Oam type
NNS_G2D_INLINE OBJLoadFunction* GetOBJLoadFunction_( NNSG2dOamType type )
{
    static OBJLoadFunction*       funcTbl[] =
    {
        CpuLoadOAMMain_,    // for NNS_G2D_OAMTYPE_MAIN
        CpuLoadOAMSub_,     // for NNS_G2D_OAMTYPE_SUB
        NULL,               // for NNS_G2D_OAMTYPE_SOFTWAREEMULATION
        NULL,               // for NNS_G2D_OAMTYPE_INVALID
        NULL,               // for NNS_G2D_OAMTYPE_MAX
    };
    
    return funcTbl[type];
}

//------------------------------------------------------------------------------
// Loads the affine parameters to the 2D Graphics Engine OAM
// Divides only the affine parameters into multiple parts and then transfers
// 
NNS_G2D_INLINE void LoadOneAffine_( const GXOamAffine* pAff, u32 offset, OBJLoadFunction* pOBJLoadFunc )
{
    offset += OAMATTR_SIZE;
    
    (*pOBJLoadFunc)( &pAff->PA, offset + GX_OAMATTR_SIZE*0, sizeof(u16) );
    (*pOBJLoadFunc)( &pAff->PB, offset + GX_OAMATTR_SIZE*1, sizeof(u16) );
    (*pOBJLoadFunc)( &pAff->PC, offset + GX_OAMATTR_SIZE*2, sizeof(u16) );
    (*pOBJLoadFunc)( &pAff->PD, offset + GX_OAMATTR_SIZE*3, sizeof(u16) );
}

//------------------------------------------------------------------------------
// Transfers the buffer contents using DMA at high speed to the graphics engine
NNS_G2D_INLINE void LoadOamAndAffineFast_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{
    GXOamAttr* pFrom    = GetOamBufferPtr_( type, fromIdx );
    const u16  szByte   = (u16)(GX_OAMATTR_SIZE * (toIdx - fromIdx + 1) );
    u16        offset   = (u16)(GX_OAMATTR_SIZE * fromIdx);
    
    // Cache flush
    DC_FlushRange( pFrom, szByte );

    // DMA transfer
    switch( type )
    {
    case NNS_G2D_OAMTYPE_MAIN:
        GX_LoadOAM( pFrom, offset, szByte );
        break;
    case NNS_G2D_OAMTYPE_SUB:
        GXS_LoadOAM( pFrom, offset, szByte );
        break;
    default:
        NNS_G2D_ASSERT( FALSE );
        break;
    }
}

//------------------------------------------------------------------------------
// Loads the OAM attributes to the 2D Graphics Engine OAM
NNS_G2D_INLINE void LoadOam_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{
    GXOamAttr* pFrom    = GetOamBufferPtr_( type, fromIdx );
    const u16  numArea  = (u16)(toIdx - fromIdx + 1);
    u16        offset   = (u16)(GX_OAMATTR_SIZE * fromIdx);
    u16         i;        
    OBJLoadFunction* pOBJLoadFunc = GetOBJLoadFunction_( type );
    
    
    for( i = 0; i < numArea; i++ ) 
    {    
        //
        // Transfers only the OAM attributes portion
        // 
        (*pOBJLoadFunc)( pFrom, offset, OAMATTR_SIZE );
        
        offset += GX_OAMATTR_SIZE;
        pFrom++;
    }
}

//------------------------------------------------------------------------------
// Loads the affine parameters to the 2D Graphics Engine OAM
NNS_G2D_INLINE void LoadAffine_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{

    GXOamAffine* pFrom      = GetAffineBufferPtr_( type, fromIdx );
    const u16  numArea      = (u16)(toIdx - fromIdx + 1);
    u16        offset       = (u16)(GX_AFFINE_SIZE * fromIdx);
    u16         i;        
    OBJLoadFunction* pOBJLoadFunc = GetOBJLoadFunction_( type );
    
    for( i = 0; i < numArea; i++ ) 
    {    
        //
        // Transfers only the affine parameter portion
        // 
        LoadOneAffine_( pFrom, offset, pOBJLoadFunc );
       
        offset += GX_AFFINE_SIZE;
        pFrom++;
    }
}
        
//------------------------------------------------------------------------------
// Resets the OAM attributes buffer with the initial values
NNS_G2D_INLINE void ResetOam_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{
    GXOamAttr* pFrom    = GetOamBufferPtr_( type, fromIdx );
    const u16  numArea  = (u16)(toIdx - fromIdx + 1);
    u16         i;        
    
    for( i = 0; i < numArea; i++ ) 
    {    
        //
        // Fills only the OAM attributes portion with the default values
        //
        //MI_CpuFill16( pFrom, OAM_SETTING_INVISIBLE, sizeof( u16 ) );
        *((u16*)pFrom) = OAM_SETTING_INVISIBLE;
        pFrom++;
    }
}

//------------------------------------------------------------------------------
// Resets the affine parameter buffer with the initial values
NNS_G2D_INLINE void ResetAffine_( NNSG2dOamType type, u16 fromIdx, u16 toIdx )
{

    GXOamAffine* pFrom      = GetAffineBufferPtr_( type, fromIdx );
    const u16  numArea      = (u16)(toIdx - fromIdx + 1);
    u16         i;        
    
    for( i = 0; i < numArea; i++ ) 
    {    
        //
        // Fills only the affine parameter portion with the default values
        // 
        pFrom->PA = OAM_SETTING_INVISIBLE;
        pFrom->PB = OAM_SETTING_INVISIBLE;
        pFrom->PC = OAM_SETTING_INVISIBLE;
        pFrom->PD = OAM_SETTING_INVISIBLE;
        pFrom++;
    }
}

//------------------------------------------------------------------------------
// library internal release
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void* NNSi_G2dGetOamManagerInternalBufferForDebug( NNSG2dOamType type )
{
    return (void*)GetOamBufferPtr_( type, 0 ); 
}




//------------------------------------------------------------------------------
// external release
//------------------------------------------------------------------------------



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitializeOamExManager

  Description:  Initializes the OamManager module
                Call before executing any of the OamManager module methods.
                
                Internally,
                the HW Oam memo file and
                Oam reservation table are initialized
                
                
  Arguments:    None
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dInitOamManagerModule()
{  
    // Oam buffer
    ClearOamByDefaultValue_( NNS_G2D_OAMTYPE_MAIN );
    ClearOamByDefaultValue_( NNS_G2D_OAMTYPE_SUB );
    ClearOamByDefaultValue_( NNS_G2D_OAMTYPE_SOFTWAREEMULATION );
    
    NNSI_G2D_DEBUGMSG0("Initialize OamBuffer ... done.\n");
    
    // OamAttr reservation table
    SetOamReservationTblNotUsed_( NNS_G2D_OAMTYPE_MAIN );
    SetOamReservationTblNotUsed_( NNS_G2D_OAMTYPE_SUB );
    SetOamReservationTblNotUsed_( NNS_G2D_OAMTYPE_SOFTWAREEMULATION );
    
    NNSI_G2D_DEBUGMSG0("Initialize Oam reservation table ... done.\n");
    
    // affine parameter reservation table
    SetAffineReservationTblNotUsed_( NNS_G2D_OAMTYPE_MAIN );
    SetAffineReservationTblNotUsed_( NNS_G2D_OAMTYPE_SUB );
    SetAffineReservationTblNotUsed_( NNS_G2D_OAMTYPE_SOFTWAREEMULATION );
    
    NNSI_G2D_DEBUGMSG0("Initialize Oam affine reservation table ... done.\n");
        
    
    NNSI_G2D_DEBUGMSG0("Initialize OamManager ... done.\n");
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetNewManagerInstance

  Description:  Initializes the OamManager instance
                
                If the requested area is already being used by another instance,
                the initialization will fail, returning FALSE.
                
  
  
  Arguments:    pMan                :       [OUT]  The manager instance 
                from:       [IN] Oam used (starting number)
                to:         [IN] Oam used (Ending number)
                type    :       [IN]  Oam type
                
                from <= to needs to be satisfied.
                
  Returns:      Whether the initialization succeeds
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetNewManagerInstance
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         from, 
    u16                         to, 
    NNSG2dOamType               type 
)
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_ASSERT( from <= to );// MUST BE !    
    NNS_G2D_ASSERT( to < NUM_HW_OAM_ATTR );
    NNS_G2D_NULL_ASSERT( pMan );
    
    
    {
        u16*        pReserveTbl = GetOamReservationTblPtr_( type );
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        
        //
        // If the specified area is unused...
        //
        if( IsOamNotUsed_( pReserveTbl, from, to ) )
        {
            //
            // Substitutes the parameters in the entity
            //
            pMan->GUID                          = GetNewGUID_();
            pMan->managedAttrArea.fromIdx       = from;
            pMan->managedAttrArea.toIdx         = to;
            pMan->managedAttrArea.currentIdx    = from;
            //
            // Sets without managing the affine parameters
            //
            pMan->managedAffineArea.fromIdx    = NUM_HW_OAM_AFFINE;
            pMan->managedAffineArea.toIdx      = 0;
            
            
            pMan->type                          = type;
            pMan->bFastTransferEnable           = FALSE;
            
            DoReserveArea_( pReserveTbl, from, to, pMan->GUID );
            
            return TRUE;
        }else{
            
            OS_Warning("Failure in NNS_G2dGetNewManagerInstance().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dInitManagerInstanceAffine

  Description:  Initializes the Affine management portion of the OamManager instance
                
                If the requested area is already being used by another instance,
                the initialization will fail, returning FALSE.
                
  
  
  Arguments:    pMan                :       [OUT]  The manager instance 
                from:       [IN] Affine used (starting number)
                to:         [IN] Affine used (ending number)
                
                from <= to needs to be satisfied.
                
  Returns:      Whether the initialization succeeds
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dInitManagerInstanceAffine
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         from, 
    u16                         to 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( from <= to );
    // Initialization complete?
    
    {
        u16*        pReserveTbl = GetAffineReservationTblPtr_( pMan->type );
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        
        //
        // If the specified area is unused...
        //
        if( IsOamNotUsed_( pReserveTbl, from, to ) )
        {
            //
            // Substitutes the parameters in the instance
            //
            pMan->managedAffineArea.fromIdx    = from;
            pMan->managedAffineArea.toIdx      = to;
            pMan->managedAffineArea.currentIdx = from;
            pMan->bFastTransferEnable          = FALSE;
            
            DoReserveArea_( pReserveTbl, from, to, pMan->GUID );
            
            return TRUE;
        }else{
            OS_Warning("Failure in NNS_G2dInitManagerInstanceAffine().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetNewOamManagerInstance

  Description:  Initializes the OamManager instance
                
                This is the new API for initializing the OAM manager.
                
  Arguments:    pMan                :           [OUT]  The manager instance 
                fromOBJ:        [IN] Oam used (starting number)
                numOBJ:         [IN] Number of Oams used (non-zero value)
                fromAffine:     [IN] Affine used (starting number)
                numAffine:      [IN] Number of affines used (zero is valid)
                type    :           [IN]  Oam type
                
                
  Returns:      Whether the initialization succeeds
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetNewOamManagerInstance
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         fromOBJ, 
    u16                         numOBJ, 
    u16                         fromAffine, 
    u16                         numAffine, 
    NNSG2dOamType               type 
)
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_NON_ZERO_ASSERT( numOBJ );// MUST BE !    
    NNS_G2D_ASSERT( numOBJ <= NUM_HW_OAM_ATTR );
    NNS_G2D_NULL_ASSERT( pMan );
    //
    // Concerning OBJ
    //
    {
        u16*        pReserveTbl = GetOamReservationTblPtr_( type );
        const u16   toOBJ       = (u16)(fromOBJ + (numOBJ - 1));
        
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        //
        // If the specified area is unused...
        //
        if( IsOamNotUsed_( pReserveTbl, fromOBJ, toOBJ ) )
        {
            //
            // Substitutes the parameters in the instance
            //
            pMan->GUID                          = GetNewGUID_();
            pMan->managedAttrArea.fromIdx       = fromOBJ;
            pMan->managedAttrArea.toIdx         = toOBJ;
            pMan->managedAttrArea.currentIdx    = fromOBJ;
            
            DoReserveArea_( pReserveTbl, fromOBJ, toOBJ, pMan->GUID );
            
        }else{
            
            OS_Warning("Failure in NNS_G2dGetNewManagerInstance().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
    
    //
    // Concerning the affine parameters
    //
    {
        u16*        pReserveTbl = GetAffineReservationTblPtr_( type );
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        
        
        if( numAffine == 0 )
        {
            //
            // If affine parameters are not used, inserts invalid data
            // Inserting invalid data is important. (Internally, the module identifies the incorrect management area and processes.)
            //
            pMan->managedAffineArea.fromIdx    = NUM_HW_OAM_AFFINE;
            pMan->managedAffineArea.toIdx      = 0;
            pMan->managedAffineArea.currentIdx = pMan->managedAffineArea.fromIdx;
            
        }else{
            //
            // For using affine parameters
            //
            const u16   toAffine       = (u16)(fromAffine + (numAffine - 1));
            //
            // If the specified area is unused...
            //
            if( IsOamNotUsed_( pReserveTbl, fromAffine, toAffine ) )
            {
                pMan->managedAffineArea.fromIdx    = fromAffine;
                pMan->managedAffineArea.toIdx      = toAffine;
                pMan->managedAffineArea.currentIdx = fromAffine;
                
                DoReserveArea_( pReserveTbl, fromAffine, toAffine, pMan->GUID );
                
            }else{
                OS_Warning("Failure in NNS_G2dGetNewManagerInstanceNew().\n The manageArea that you specified has been used by someone.");
                return FALSE;
            }
        }
    }
    
    pMan->bFastTransferEnable           = FALSE;
    pMan->type                          = type;
    
    //
    // Initialization completed without bogging down
    //
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetNewOamManagerInstanceAsFastTransferMode

  Description:  Initializes the OamManager instance.
                
                This is the new API for initializing the OAM manager.
                
                There are limitations on the reserved area.
                The number of management affine parameter areas that are allocated is equivalent to the number included in the management OAM attribute memory. 
                (Example: OAM numbers 0 to 32 => affine parameter numbers 0 to 8)
                
                High speed transfer is possible when
                transferring data from the buffer to the graphics engine.
                
  Arguments:    pMan                :           [OUT]  The manager instance 
                fromOBJ:        [IN] Oam used (starting number: multiple of 4 required)
                numOBJ:         [IN] Number of Oams used (non-zero value: multiple of 4 required)
                type    :           [IN]  Oam type
                
                
  Returns:      Whether the initialization succeeds
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dGetNewOamManagerInstanceAsFastTransferMode
( 
    NNSG2dOamManagerInstance*   pMan, 
    u16                         fromOBJ, 
    u16                         numOBJ,
    NNSG2dOamType               type 
)
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_NON_ZERO_ASSERT( numOBJ );// MUST BE !    
    NNS_G2D_ASSERT( numOBJ <= NUM_HW_OAM_ATTR );
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERT( fromOBJ % 4 == 0 );
    NNS_G2D_ASSERT( numOBJ % 4 == 0 );
    //
    // Concerning OBJ
    //
    {
        u16*        pReserveTbl = GetOamReservationTblPtr_( type );
        const u16   toOBJ       = (u16)(fromOBJ + (numOBJ - 1));
        
        NNS_G2D_NULL_ASSERT( pReserveTbl );
        //
        // If the specified area is unused...
        //
        if( IsOamNotUsed_( pReserveTbl, fromOBJ, toOBJ ) )
        {
            //
            // Substitutes the parameters in the instance
            //
            pMan->GUID                          = GetNewGUID_();
            pMan->managedAttrArea.fromIdx       = fromOBJ;
            pMan->managedAttrArea.toIdx         = toOBJ;
            pMan->managedAttrArea.currentIdx    = fromOBJ;
            
            DoReserveArea_( pReserveTbl, fromOBJ, toOBJ, pMan->GUID );
            
        }else{
            
            OS_Warning("Failure in NNS_G2dGetNewOamManagerInstanceAsFastTransferMode().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
    
    //
    // Concerning the affine parameters
    //
    {
        const u16   fromAffine     = (u16)(fromOBJ / 4);
        const u16   numAffine      = (u16)(numOBJ / 4);
        const u16   toAffine       = (u16)(fromAffine + (numAffine - 1));
        
        u16*        pReserveTbl = GetAffineReservationTblPtr_( type );
        NNS_G2D_NULL_ASSERT( pReserveTbl );        
        //
        // If the specified area is unused...
        //
        if( IsOamNotUsed_( pReserveTbl, fromAffine, toAffine ) )
        {
            pMan->managedAffineArea.fromIdx    = fromAffine;
            pMan->managedAffineArea.toIdx      = toAffine;
            pMan->managedAffineArea.currentIdx = fromAffine;
            
            DoReserveArea_( pReserveTbl, fromAffine, toAffine, pMan->GUID );
            
        }else{
            OS_Warning("Failure in NNS_G2dGetNewManagerInstanceNew().\n The manageArea that you specified has been used by someone.");
            return FALSE;
        }
    }
    
    //
    // Sets a high speed transfer enable flag
    //
    pMan->bFastTransferEnable           = TRUE;
    pMan->type                          = type;
    
    //
    // Initialization completed without bogging down
    //
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManagerOam

  Description:  Registers Oam.
                If there are not adequate 0ams for a requested registration,
                nothing happens and FALSE is returned.
                
  Arguments:    pMan                :       [OUT]  The manager instance 
                pOam :       [IN] Pointer to the start of the 0am to be registered
                num:        [IN] The number of 0ams to be registered
                
  Returns:      registration success or failure
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dEntryOamManagerOam
( 
    NNSG2dOamManagerInstance*   pMan, 
    const GXOamAttr*            pOam, 
    u16                         num 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
    NNS_G2D_ASSERT( num != 0 );
    
    //
    // If there is not enough capacity...
    //
    if( HasEnoughCapacity_( &pMan->managedAttrArea, num ) )
    {
        //
        // Copies the data to the buffer
        // Divides the memory copy so affine parameters are not overwritten.
        //     
        int i = 0;
        GXOamAttr* pOamAttr = GetOamCurrentPtr_( pMan );
        for( i = 0; i < num; i ++ )
        {    
            //MI_CpuCopy16( pOam, pOamAttr, OAMATTR_SIZE );
            pOamAttr[i].attr0 = pOam->attr0;
            pOamAttr[i].attr1 = pOam->attr1;
            pOamAttr[i].attr2 = pOam->attr2;
            
            pMan->managedAttrArea.currentIdx++;
            pOam++;
        }
        
        //
        // Debug output
        //
        NNSI_G2D_DEBUGMSG1( "New Oam entry occur...  from %d to %d for GUID %d\n",
                             pMan->currentPos, 
                             pMan->currentPos + num, 
                             pMan->GUID );
        
        return TRUE;
    }else{
        NNSI_G2D_DEBUGMSG0("The OamBuffer has no capacity enough to store new Oam.");
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManagerOamWithAffineIdx

  Description:  Specifies the affine index and registers OAM attribute parameters.
                If there is not enough area for the requested registration,
                nothing happens and FALSE is returned.
                
  Arguments:    pMan                :       [OUT]  The manager instance 
                pOam :       [IN] Affine Matrix to be registered
                affineIdx:  [IN] The Affine index that references the OBJ
                
  Returns:      registration success or failure
  
 *---------------------------------------------------------------------------*/
BOOL NNS_G2dEntryOamManagerOamWithAffineIdx
( 
    NNSG2dOamManagerInstance*   pMan, 
    const GXOamAttr*            pOam, 
    u16                         affineIdx 
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pOam );
   
    
    //
    // If there is not enough capacity...
    //
    if( HasEnoughCapacity_( &pMan->managedAttrArea, 1 ) )
    {
        GXOamAttr* pOamAttr = GetOamCurrentPtr_( pMan );
        
        
        // MI_CpuCopy16( pOam, pOamAttr, OAMATTR_SIZE );
        pOamAttr->attr0 = pOam->attr0;
        pOamAttr->attr1 = pOam->attr1;
        pOamAttr->attr2 = pOam->attr2;
        
        //
        // If affine Index is specified...
        //
        if( NNS_G2D_OAM_AFFINE_IDX_NONE != affineIdx )
        {
            
            // G2_SetOBJEffect( pOamAttr, GX_OAM_EFFECT_AFFINE_DOUBLE, affineIdx );
            {
                
                // Check the rotate/scale enable flags.
                // If affine transform enable flag is not enabled, 
                // oamAffinIdx is not set
                // Previously, forced to terminate by Assert. 
                //
                // NNS_G2D_ASSERT( pOamAttr->rsMode & 0x1 );
                //
                if( pOamAttr->rsMode & 0x1 )                
                {
                    pOamAttr->rsParam = affineIdx;
                }
            }
        }
        
        pMan->managedAttrArea.currentIdx++;
        return TRUE;
    }else{
        NNSI_G2D_DEBUGMSG0("The OamBuffer has no capacity enough to store new Oam.");
        return FALSE;
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G2dSetOamManagerAffine

  Description:  Specifies the index and registers the Oam Affine parameters.
                If invalid OAM type or index was specified, assert will fail.
                
                If the affine parameter management area has been written to, a warning is displayed.
                
  Arguments:    type    :      [OUT] OAM type
                mtx :       [IN] Affine Matrix to be registered
                idx         :       [IN] The index for registering Affines
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNS_G2dSetOamManagerAffine
( 
    NNSG2dOamType               type, 
    const MtxFx22*              mtx, 
    u16                         idx 
)
{
    ASSERT_OAMTYPE_VALID( type );
    NNS_G2D_NULL_ASSERT( mtx );
    NNS_G2D_MINMAX_ASSERT( idx, 0, NUM_HW_OAM_AFFINE );
    
    
    SDK_WARNING( IsOamNotUsed_( GetAffineReservationTblPtr_( type ), idx, idx ),
                "An invalid affine param setting to the managed area is detected. index = %d", idx );

    
    SetAffineParams_( type, mtx, idx );
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dEntryOamManagerAffine

  Description:  Registers the Oam Affine parameters.
                After registration, the Affine Parameter Index is returned.
                If there is not enough area for the requested registration,
                nothing happens and NNS_G2D_OAM_AFFINE_IDX_NONE is returned.
                
                If the affine parameters are not allocated during initialization, the assert fails.
                
  Arguments:    pMan                :      [OUT]  The manager instance 
                mtx :       [IN] Affine Matrix to be registered
                
  Returns:      The Affine parameter Index
  
 *---------------------------------------------------------------------------*/
u16 NNS_G2dEntryOamManagerAffine( NNSG2dOamManagerInstance* pMan, const MtxFx22* mtx )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( mtx );
    NNS_G2D_ASSERT( IsManageAreaValid_( &pMan->managedAffineArea ) );
    
    //
    // If there is not enough capacity...
    //
    if( HasEnoughCapacity_( &pMan->managedAffineArea, 1 ) )
    {
        const u16 currentAffineIdx = pMan->managedAffineArea.currentIdx;

        SetAffineParams_( pMan->type, mtx, currentAffineIdx );
        
        pMan->managedAffineArea.currentIdx++;
        
        return currentAffineIdx;
    }else{
        NNSI_G2D_DEBUGMSG0("The OamBuffer has no capacity enough to store new Affine Paramater.");
        return NNS_G2D_OAM_AFFINE_IDX_NONE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dApplyOamManagerToHW

  Description:  Applies the OamManager internal buffer to HW OAM
                To complete application without affecting the screen while drawing,
                execution needs to be made in the drawing blank.
                
  Arguments:    pMan                :                [OUT]  The manager instance 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
     
void NNS_G2dApplyOamManagerToHW( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_ASSERTMSG( pMan->type != NNS_G2D_OAMTYPE_SOFTWAREEMULATION, 
                    " For the NNS_G2D_OAMTYPE_SOFTWAREEMULATION type Manager, Use NNS_G2dApplyOamManagerToHWSprite() instead." );
    {
        //
        // Is high speed transfer (batch OAMAttr and affine parameter transfer) possible?
        //
        if( pMan->bFastTransferEnable )
        {
            LoadOamAndAffineFast_( pMan->type,
                                   pMan->managedAttrArea.fromIdx, 
                                   pMan->managedAttrArea.toIdx );
        }else{
            // OAM Attr
            LoadOam_    ( pMan->type, 
                          pMan->managedAttrArea.fromIdx, 
                          pMan->managedAttrArea.toIdx );
            // affine params
            if( IsManageAreaValid_( &pMan->managedAffineArea ) )
            {    
                LoadAffine_ ( pMan->type, 
                              pMan->managedAffineArea.fromIdx, 
                              pMan->managedAffineArea.toIdx );
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dApplyOamManagerToHWSprite

  Description:  Draws the contents of the manager using the 3D Graphics Engine
                
  Arguments:    pMan                :      [OUT]  The manager instance 
                pTexImageAttr:      [IN] VRAM texture image attributes
                texBaseAddr:      [IN]  VRAM  base address 
                pltBaseAddr:      [IN]  Palette base address 
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
     
void NNS_G2dApplyOamManagerToHWSprite
( 
    NNSG2dOamManagerInstance*       pMan, 
    const NNSG2dImageAttr*          pTexImageAttr,
    u32                             texBaseAddr,
    u32                             pltBaseAddr
)
{
    NNS_G2D_NULL_ASSERT( pMan );
    NNS_G2D_NULL_ASSERT( pTexImageAttr );
    
    
    NNS_G2D_ASSERTMSG( pMan->type == NNS_G2D_OAMTYPE_SOFTWAREEMULATION, 
        " For the NNS_G2D_OAMTYPE_MAIN SUB type Manager, Use NNS_G2dApplyOamManagerToHW() instead." );
    
    if( pMan->spriteZoffsetStep != 0 )
    {
        fx32 step = NNSi_G2dGetOamSoftEmuAutoZOffsetStep();
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( TRUE );
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( pMan->spriteZoffsetStep );
        
        {
            void*       pFrom   = GetOamFromPtr_( pMan );
            const u16   numOam  = GetNumOamUsed_( &pMan->managedAttrArea );
            NNS_G2D_NULL_ASSERT( pFrom );
            
            
            DrawBy3DGraphicsEngine_( pFrom, 
                                     numOam, 
                                     pTexImageAttr,
                                     texBaseAddr,
                                     pltBaseAddr );
        }
        
        NNSi_G2dSetOamSoftEmuAutoZOffsetStep( step );
        NNSi_G2dSetOamSoftEmuAutoZOffsetFlag( FALSE );
        NNSi_G2dResetOamSoftEmuAutoZOffset();
    }else{
        {
           void*       pFrom   = GetOamFromPtr_( pMan );
           const u16   numOam  = GetNumOamUsed_( &pMan->managedAttrArea );
           NNS_G2D_NULL_ASSERT( pFrom );
           
           
           DrawBy3DGraphicsEngine_( pFrom, 
                                    numOam, 
                                    pTexImageAttr,
                                    texBaseAddr,
                                    pltBaseAddr );
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G2dResetOamManagerBuffer

  Description:  Resets the OamManager internal buffer.
                
  Arguments:    pMan                :                [OUT]  The manager instance 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
 
void NNS_G2dResetOamManagerBuffer( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );

    // Resets with the default values
    {
        //
        // If high speed transfer is possible
        //
        if( pMan->bFastTransferEnable )
        {    
            //
            // Memory fill carried out in one step by DMA
            //            
            const u32 szByte = GetSizeOfManageArea_( &pMan->managedAttrArea );
            void* pData = GetOamFromPtr_( pMan );
            NNS_G2D_ASSERT( szByte != 0 );
            
            DC_InvalidateRange( pData, szByte );
            MI_DmaFill32( NNS_G2D_DMA_NO, pData, OAM_SETTING_INVISIBLE, szByte );
            
        }else{    
            //
            // Broken up memory fill carried out by CPU copy
            //
            // OAM Attr
            ResetOam_( pMan->type, 
                       pMan->managedAttrArea.fromIdx, 
                       pMan->managedAttrArea.toIdx );
            
            //
            // It is unlikely an affine parameter reset is necessary, so changed to not reset.
            //
            /*
            // affine params
            if( IsManageAreaValid_( &pMan->managedAffineArea ) )
            {
                ResetAffine_( pMan->type,
                              pMan->managedAffineArea.fromIdx,
                              pMan->managedAffineArea.toIdx );
            }
            */
        }
    }
    
    // Resets the counter
    {
        pMan->managedAttrArea.currentIdx    = pMan->managedAttrArea.fromIdx;
        pMan->managedAffineArea.currentIdx  = pMan->managedAffineArea.fromIdx;
    }
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dApplyAndResetOamManagerBuffer

  Description:  After applying the OamManager internal buffer to HW OAM,
                Resets the OamManager internal buffer.
                
  Arguments:    pMan                :                [OUT]  The manager instance 
                
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
    
void NNS_G2dApplyAndResetOamManagerBuffer( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    
    NNS_G2dApplyOamManagerToHW( pMan );
    NNS_G2dResetOamManagerBuffer( pMan );
}



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetOamManagerOamCapacity, NNS_G2dGetOamManagerAffineCapacity

  Description:  Gets the number of usable resources
                
  Arguments:    pMan                :                [IN]  The manager instance 
                
                
  Returns:      The number of usable resources
  
 *---------------------------------------------------------------------------*/
u16     NNS_G2dGetOamManagerOamCapacity( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    return GetCapacity_( &pMan->managedAttrArea );
}

//------------------------------------------------------------------------------
u16     NNS_G2dGetOamManagerAffineCapacity( NNSG2dOamManagerInstance* pMan )
{
    NNS_G2D_NULL_ASSERT( pMan );
    return GetCapacity_( &pMan->managedAffineArea );
}





