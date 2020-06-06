/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     g2d_SRTControl.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_SRTControl.c,v $
  Revision 1.7  07/06/2005 12:52:06 PM  kitani_toshikazu
  Changed copyright year

  Revision 1.6  06/29/2005 08:30:27 AM  kitani_toshikazu
  Corrected comment

  Revision 1.5  08/23/2004 08:45:04 AM  kitani_toshikazu
  Implementation of Rotate/Scale -related methods

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
#include <nnsys/g2d/g2d_SRTControl.h>


#define ASSERT_SRTControlType_VALID( val ) \
        NNS_G2D_MINMAX_ASSERT( (val), NNS_G2D_SRTCONTROLTYPE_SRT, NNS_G2D_SRTCONTROLTYPE_MTX3D )

#define ASSERT_SRT_MODE( val )  NNS_G2D_ASSERTMSG( ( val ) == NNS_G2D_SRTCONTROLTYPE_SRT, \
                  "You can use the function only if its NNSG2dSRTControlType is NNS_G2D_SRTCONTROLTYPE_SRT" );

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcSetTrans TODO:

  Description:  Sets the translation value of NNSG2dSRTControl
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                    x:         The X translation value
                    y:         The Y translation value
                    
                    The NNSG2dSRTControlType must
                    be NNS_G2D_SRTCONTROLTYPE_SRT.
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcSetTrans( NNSG2dSRTControl* pCtrl, s16 x, s16 y )
{
    
    
    if( pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT )
    {
        NNSi_G2dSrtcAffineFlagON( pCtrl, NNS_G2D_AFFINEENABLE_TRANS );
        
        pCtrl->srtData.trans.x = x;
        pCtrl->srtData.trans.y = y;
    }else{
        NNS_G2D_ASSERTMSG( FALSE, "NOT implemented, avoid calling me");
    }
}




/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcSetSRTRotZ TODO:

  Description:  Configures the rotation value of NNSG2dSRTControl
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                 rotZ:         Rotation on the Z-axis
                    
                    The NNSG2dSRTControlType must
                    be NNS_G2D_SRTCONTROLTYPE_SRT.
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcSetSRTRotZ( NNSG2dSRTControl* pCtrl, u16 rotZ )
{
    if( pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT )
    {
        NNSi_G2dSrtcAffineFlagON( pCtrl, NNS_G2D_AFFINEENABLE_ROTATE );
        
        pCtrl->srtData.rotZ = rotZ;
    }else{
        NNS_G2D_ASSERTMSG( FALSE, "NOT implemented, avoid calling me");
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcSetSRTScale TODO:

  Description:  Sets the scale value of NNSG2dSRTControl
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                    x:         The X scale value
                    y:         The Y scale value
                    
                    The NNSG2dSRTControlType must
                    be NNS_G2D_SRTCONTROLTYPE_SRT.
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcSetSRTScale( NNSG2dSRTControl* pCtrl, fx32 x, fx32 y )
{
    if( pCtrl->type == NNS_G2D_SRTCONTROLTYPE_SRT )
    {
        NNSi_G2dSrtcAffineFlagON( pCtrl, NNS_G2D_AFFINEENABLE_SCALE );
        
        pCtrl->srtData.scale.x = x;
        pCtrl->srtData.scale.y = y;
    }else{
        NNS_G2D_ASSERTMSG( FALSE, "NOT implemented, avoid calling me");
    }
    
}


/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcInitControl TODO:

  Description:  Initialize NNSG2dSRTControl
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                 type:         The data format of NNSG2dSRTControl
                    
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcInitControl( NNSG2dSRTControl* pCtrl, NNSG2dSRTControlType type )
{
    NNS_G2D_NULL_ASSERT( pCtrl );
    ASSERT_SRTControlType_VALID( type );
    
    pCtrl->type = type;
    
    NNSi_G2dSrtcSetInitialValue( pCtrl );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcSetInitialValue TODO:

  Description:  Sets the initial value of NNSG2dSRTControl
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                    
                    The NNSG2dSRTControlType must
                    be NNS_G2D_SRTCONTROLTYPE_SRT.
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcSetInitialValue    ( NNSG2dSRTControl* pCtrl )
{
    NNS_G2D_NULL_ASSERT( pCtrl );
    ASSERT_SRT_MODE( pCtrl->type ); 
    
    MI_CpuFill16( &pCtrl->srtData, 0, sizeof( NNSG2dSRTData ) );
    
    pCtrl->srtData.scale.x = FX32_ONE;
    pCtrl->srtData.scale.y = FX32_ONE;
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcBuildMatrixFromSRT_2D TODO:

  Description:  Calculates the SR information of NNSG2dSRTControl as a matrix for the 2D Graphics Engine.
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                 pDst:         The output matrix
                    
                    The NNSG2dSRTControlType must
                    be NNS_G2D_SRTCONTROLTYPE_SRT.
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcBuildMatrixFromSRT_2D( const NNSG2dSRTControl* pCtrl , MtxFx22* pDst )
{
#pragma unused( pCtrl )
#pragma unused( pDst )
    NNS_G2D_NULL_ASSERT( pCtrl );
    NNS_G2D_NULL_ASSERT( pDst );
    ASSERT_SRT_MODE( pCtrl->type ); 
    
    MTX_Identity22( pDst );
    
    // r
    MTX_Rot22( pDst, FX_SinIdx( pCtrl->srtData.rotZ ), FX_CosIdx( pCtrl->srtData.rotZ ) );
    // s
    MTX_ScaleApply22( pDst, pDst, pCtrl->srtData.scale.x, pCtrl->srtData.scale.y );
}

/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dSrtcBuildMatrixFromSRT_3D TODO:

  Description:  Calculates the SR information of NNSG2dSRTControl for use by the 3D Graphics Engine.
                
                
  Arguments:    pCtrl:         NNSG2dSRTControl
                 pDst        :         The output matrix
                    
                    The NNSG2dSRTControlType must
                    be NNS_G2D_SRTCONTROLTYPE_SRT.
                    
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void NNSi_G2dSrtcBuildMatrixFromSRT_3D( const NNSG2dSRTControl* pCtrl , MtxFx22* pDst )
{
#pragma unused( pCtrl )
#pragma unused( pDst )
    NNS_G2D_NULL_ASSERT( pCtrl );
    NNS_G2D_NULL_ASSERT( pDst );
    ASSERT_SRT_MODE( pCtrl->type );       
    
    // r
    MTX_Rot22( pDst, FX_SinIdx( pCtrl->srtData.rotZ ), FX_CosIdx( pCtrl->srtData.rotZ ) );
    // s
    MTX_ScaleApply22( pDst, pDst, pCtrl->srtData.scale.x, pCtrl->srtData.scale.y );
}


