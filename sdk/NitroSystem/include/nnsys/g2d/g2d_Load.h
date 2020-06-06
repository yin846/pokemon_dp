/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  File:     g2d_Load.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_Load.h,v $
  Revision 1.11  07/07/2005 06:59:21 AM  kitani_toshikazu
  Expansion of user-extended attributes 

  Revision 1.10  06/13/2005 08:23:29 AM  kitani_toshikazu
  Changed determination conditions in NNSi_G2dIsBinFileVersionValid() 

  Revision 1.9  05/20/2005 05:07:11 AM  hatamoto_minoru
  Added g2d_NFT_load.

  Revision 1.8  09/02/2004 07:08:10 AM  hatamoto_minoru
  Added BG related.

  Revision 1.7  09/02/2004 01:43:19 AM  kitani_toshikazu
  Addition of verification functions used internal to the assert statement

  Revision 1.6  08/25/2004 06:56:54 AM  kitani_toshikazu
  (none)

  Revision 1.5  07/23/2004 02:49:17 AM  kitani_toshikazu
  Replaced ASSERT and static inline

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
#ifndef NNS_G2D_LOAD_H_
#define NNS_G2D_LOAD_H_

#include <nitro.h>


#ifdef __cplusplus
extern "C" {
#endif



//------------------------------------------------------------------------------
// for loading binary file.
#include <nnsys/g2d/load/g2d_NCE_load.h>
#include <nnsys/g2d/load/g2d_NAN_load.h>
#include <nnsys/g2d/load/g2d_NEN_load.h>
#include <nnsys/g2d/load/g2d_NMC_load.h>
#include <nnsys/g2d/load/g2d_NCG_load.h>
#include <nnsys/g2d/load/g2d_NCL_load.h>
#include <nnsys/g2d/load/g2d_NSC_load.h>
#include <nnsys/g2d/load/g2d_NFT_load.h>

//------------------------------------------------------------------------------
#define BIN_FILE_VERSION( EXT ) NNS_G2dMakeVersionData( NNS_G2D_##EXT##_MAJOR_VER, NNS_G2D_##EXT##_MINOR_VER )




NNSG2dBinaryBlockHeader* NNS_G2dFindBinaryBlock
( 
    NNSG2dBinaryFileHeader* pBinFileHeader, 
    u32                     signature 
);

//------------------------------------------------------------------------------
void NNSi_G2dUnpackUserExCellAttrBank( NNSG2dUserExCellAttrBank* pCellAttrBank );

//------------------------------------------------------------------------------
// Debug output function
// Does not generate code in FINAL_ROM 
#ifdef __SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNSi_G2dPrintUserExCellAttrBank( const NNSG2dUserExCellAttrBank* pCellAttrBank ) NNS_G2D_DEBUG_FUNC_DECL_END
#else//__SNC__
NNS_G2D_DEBUG_FUNC_DECL_BEGIN void NNSi_G2dPrintUserExCellAttrBank( const NNSG2dUserExCellAttrBank*  ) NNS_G2D_DEBUG_FUNC_DECL_END
#endif//__SNC__

//
// Inline functions
//
//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dIsBinFileSignatureValid
( 
    const NNSG2dBinaryFileHeader*   pBinFile, 
    u32                             binFileSig 
)
{
    if( pBinFile != NULL )
    {
        // Is file identifier correct?
        if( ( pBinFile->signature == binFileSig  ) )
        {
            return TRUE;                
        }        
    }
    return FALSE;
}

//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNSi_G2dIsBinFileVersionValid
( 
    const NNSG2dBinaryFileHeader*   pBinFile, 
    u16                             version 
)
{
    if( pBinFile != NULL )
    {
        // Is the binary file version newer than the specified version?
        if( pBinFile->version >= version )
        {
            return TRUE;                
        }        
    }
    return FALSE;
}





//------------------------------------------------------------------------------
NNS_G2D_INLINE BOOL NNS_G2dIsBinFileValid
( 
    const NNSG2dBinaryFileHeader*   pBinFile, 
    u32                             binFileSig, 
    u16                             version 
)
{
    if( pBinFile != NULL )
    {
        // Is the byte order correct?
        //
        // This has not been correctly configured by the output data of the old version converter (A zero value is substituted).
        // Since this is not substantially used, no check is performed.
        //
        // if( pBinFile->byteOrder == NNS_G2D_LITTLEENDIAN_BITMARK )
        {            
            return NNSi_G2dIsBinFileSignatureValid( pBinFile, binFileSig ) && 
                   NNSi_G2dIsBinFileVersionValid( pBinFile, version );   
        }    
    }
    return FALSE;
}







#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_LOAD_H_

