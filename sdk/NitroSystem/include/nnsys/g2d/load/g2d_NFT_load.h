/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics 2D
  File:     g2d_NFT_load.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2d_NFT_load.h,v $
  Revision 1.1  05/20/2005 04:34:35 AM  hatamoto_minoru
  Registered in NitroSystem.

  Revision 1.5  03/18/2005 09:21:20 AM  hatamoto_minoru
  Made GetUnpackedFont() non-public.

  Revision 1.4  03/14/2005 11:54:35 PM  hatamoto_minoru
  tab -> sp

  Revision 1.3  02/18/2005 09:26:21 AM  hatamoto_minoru
  Revised the include file names.

  Revision 1.2  02/08/2005 04:06:59 AM  hatamoto_minoru
  Revised file headers.

  Revision 1.1  01/21/2005 01:03:09 AM  hatamoto_minoru
  Registered to TextDraw.

  Revision 1.2  12/06/2004 01:02:03 PM  hatamoto_minoru
  Organized the source and added comments.

  Revision 1.1  11/30/2004 09:17:04 AM  hatamoto_minoru
  Registered the initial version.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_G2D_NFT_LOAD_H_
#define NNS_G2D_NFT_LOAD_H_

#include <nitro.h>
#include <nnsys/g2d/g2d_config.h>
#include <nnsys/g2d/g2d_Data.h>
#include <nnsys/g2d/g2d_Font.h>

#ifdef __cplusplus
extern "C" {
#endif



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dGetUnpackedFont

  Description:  Expands the NFTR file to a NITRO font.

  Arguments:    pNftrFile:  Pointer to the NFTR file data.
                ppFont:     Pointer to the buffer that stores the
                            pointer to the NITRO font.

  Returns:      Returns true if the expansion was a success.
 *---------------------------------------------------------------------------*/
BOOL NNSi_G2dGetUnpackedFont( void* pNftrFile, NNSG2dFontInformation** ppFont );



/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dUnpackNFT

  Description:  Expands the NFTR file data into a format that can be used by the library.
                This only needs to be executed once on each file data
                before that data is used in the library.

  Arguments:    pHeader:    Pointer to the binary file header of the NFTR file.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_G2dUnpackNFT(NNSG2dBinaryFileHeader* pHeader);



/*---------------------------------------------------------------------------*
  Name:         NNS_G2dPrintFont

  Description:  Outputs the NITRO font information to debug print.

  Arguments:    pFont:  NITRO Pointer to the font.

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef SDK_FINALROM
    static inline void NNS_G2dPrintFont( const NNSG2dFont* /*pFont*/ )
    {
    }
#else // SDK_FINALROM
    void NNS_G2dPrintFont( const NNSG2dFont* pFont );
#endif // SDK_FINALROM



#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_G2D_NFT_LOAD_H_


