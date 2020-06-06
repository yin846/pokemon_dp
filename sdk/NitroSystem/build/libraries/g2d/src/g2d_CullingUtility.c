/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  
  File:     .c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_CullingUtility.c,v $
  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nnsys/g2d/g2d_CullingUtility.h>

//------------------------------------------------------------------------------
BOOL NNS_G2dIsInViewCircle( const NNSG2dFVec2* pTopLeft, const NNSG2dFVec2* pSize, const NNSG2dFVec2* pos, fx32 boundingR )
{
    
    if( pos->x < pTopLeft->x - boundingR )
        return FALSE;
    if( pos->y < pTopLeft->y - boundingR )
        return FALSE;
    
    if( pos->x > pTopLeft->x + pSize->x )
        return FALSE;
    
    if( pos->y > pTopLeft->y + pSize->y )
        return FALSE;
        
    return TRUE;
}


