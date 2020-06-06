/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     g2d_Entity.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Entity.h,v $
  Revision 1.10  07/29/2004 12:34:20 AM  kitani_toshikazu
  Changed function names.

  Revision 1.9  07/28/2004 12:11:37 AM  kitani_toshikazu
  Correspondence with the padding warning

  Revision 1.8  07/24/2004 07:09:59 AM  kitani_toshikazu
  Renamed Entity related function

  Revision 1.7  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.6  05/28/2004 10:50:52 AM  kitani_toshikazu
  Added accessor to NNSG2dAnimController

  Revision 1.5  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.4  05/10/2004 04:42:28 AM  kitani_toshikazu
  Check-in for PR1.

  Revision 1.3  04/23/2004 12:26:50 AM  kitani_toshikazu
  Added workaround for color palette swap table.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:54 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
#ifndef NNS_G2D_ENTITY2_H_
#define NNS_G2D_ENTITY2_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Animation.h>
#include <nnsys/g2d/g2d_PaletteTable.h>


#ifdef __cplusplus
extern "C" {
#endif

//
// aliases of functions with names changed
// Previous functions declared as aliases to preserve compatibility.
// 
#define NNS_G2dInitializeEntity        NNS_G2dInitEntity
#define NNS_G2dSetCurrentAnimation     NNS_G2dSetEntityCurrentAnimation



#define NNS_G2D_ASSERT_ENTITY_VALID( entity )           \
           NNS_G2D_ASSERTMSG( NNS_G2dIsEntityValid( ( entity ) ),"A Invalid Entity instance was detected." );


/*---------------------------------------------------------------------------*
  Name:         NNSG2dEntity

  Description:  Concept of grouping drawing data and animation data.
                Basic structure of game characters.
                
 *---------------------------------------------------------------------------*/
 
typedef struct NNSG2dEntity
{
    void*                         pDrawStuff;                 // drawing data
    const NNSG2dEntityData*       pEntityData;                // reference to static data
    const NNSG2dAnimBankData*     pAnimDataBank;              // associated bank
    u16                           currentSequenceIdx;         // sequence number currently playing back
    u16                           pad16_;                     // The padding
    //
    // If NULL, regarded as bPaletteChangeEnable = FALSE.
    // Size of table type is large and is probably shared by multiple characters.
    // Therefore, made so pointer maintained.
    // 
    NNSG2dPaletteSwapTable*         pPaletteTbl;
    
}NNSG2dEntity;



//------------------------------------------------------------------------------
void    NNS_G2dInitEntity      
( 
    NNSG2dEntity*               pEntity, 
    void*                       pDrawStuff, 
    const NNSG2dEntityData*     pEntityData, 
    const NNSG2dAnimBankData*   pAnimDataBank 
);
void    NNS_G2dSetEntityCurrentAnimation  ( NNSG2dEntity* pEntity, u16 idx );



//------------------------------------------------------------------------------
void NNS_G2dSetEntityPaletteTable   ( NNSG2dEntity* pEntity, NNSG2dPaletteSwapTable* pPlttTbl );
void NNS_G2dResetEntityPaletteTable ( NNSG2dEntity* pEntity );
BOOL NNS_G2dIsEntityPaletteTblEnable ( const NNSG2dEntity* pEntity );



//------------------------------------------------------------------------------
void NNS_G2dTickEntity( NNSG2dEntity*    pEntity, fx32 dt );
void NNS_G2dSetEntityCurrentFrame
( 
    NNSG2dEntity*    pEntity,
    u16              frameIndex  
);

//------------------------------------------------------------------------------
void NNS_G2dSetEntitySpeed
(
    NNSG2dEntity*     pEntity,
    fx32              speed 
);

//------------------------------------------------------------------------------
BOOL            NNS_G2dIsEntityValid( NNSG2dEntity*    pEntity );

//------------------------------------------------------------------------------
NNSG2dAnimController* NNS_G2dGetEntityAnimCtrl( NNSG2dEntity*    pEntity );


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_ENTITY_H_

