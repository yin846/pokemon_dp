/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     .c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Node.c,v $
  Revision 1.4  07/23/2004 02:49:24 AM  kitani_toshikazu
  Replaced ASSERT and static inline

  Revision 1.3  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nnsys/g2d/g2d_Node.h>
#include <nnsys/g2d/g2d_SRTControl.h>

void NNSi_G2dInitializeNode( NNSG2dNode* pNode, NNSG2dNodeType type )
{
    NNS_G2D_NULL_ASSERT( pNode );
    NNS_G2D_ASSERT_NODETYPE_VALID( type );
    
    pNode->pContent     = NULL;
    pNode->type         = type;
    pNode->bVisible     = TRUE;
    
    
    NNSi_G2dSrtcInitControl( &pNode->srtCtrl, NNS_G2D_SRTCONTROLTYPE_SRT );
    
}






