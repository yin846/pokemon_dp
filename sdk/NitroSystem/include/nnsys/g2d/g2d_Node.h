/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_Node.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Node.h,v $
  Revision 1.5  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.4  06/30/2004 11:34:19 AM  kitani_toshikazu
  Added access to visual flag

  Revision 1.3  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.2  04/21/2004 06:40:03 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:25:54 AM  kitani_toshikazu
  append G2D Lib.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_G2D_NODE_H_
#define NNS_G2D_NODE_H_


#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/fmt/g2d_SRTControl_data.h>

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------------------------------
typedef enum NNSG2dNodeType
{
    NNS_G2D_NODETYPE_INVALID       = 0,
    NNS_G2D_NODETYPE_CELL          ,
    NNS_G2D_NODETYPE_MULTICELL     ,
    NNS_G2D_NODETYPE_MAX
    
}NNSG2dNodeType;


#define NNS_G2D_ASSERT_NODETYPE_VALID( val )     \
        NNS_G2D_MINMAX_ASSERT( val, NNS_G2D_NODETYPE_CELL, NNS_G2D_NODETYPE_MULTICELL )

#define NNS_G2D_NODE_AFFINE_IDX_NONE    0xFFFF

//------------------------------------------------------------------------------
typedef struct NNSG2dNode
{
    void*                   pContent;   // associated drawing entities
    NNSG2dNodeType          type;       // accociated drawing entities type
    BOOL                    bVisible;   // visible flag
    NNSG2dSRTControl        srtCtrl;    // Scale Rotate Translate
    
}NNSG2dNode;


void NNSi_G2dInitializeNode( NNSG2dNode* pNode, NNSG2dNodeType type );


//------------------------------------------------------------------------------
// inline function
//------------------------------------------------------------------------------
NNS_G2D_INLINE void NNSi_G2dSetNodeVisibleFlag( NNSG2dNode* pNode, BOOL bVisible )
{
    NNS_G2D_NULL_ASSERT( pNode );
    pNode->bVisible = bVisible;
}

NNS_G2D_INLINE BOOL NNSi_G2dGetNodeVisibleFlag( NNSG2dNode* pNode )
{
    NNS_G2D_NULL_ASSERT( pNode );
    return pNode->bVisible;
}

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_NODE_H_

