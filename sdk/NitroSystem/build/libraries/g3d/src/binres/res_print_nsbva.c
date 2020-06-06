/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_print_nsbva.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_print_nsbva.c,v $
  Revision 1.10  03/02/2005 00:40:24  takano_makoto
  fix copyright header.

  Revision 1.9  02/28/2005 00:14:32  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.8  10/25/2004 01:27:54  nishida_kenji
  Expand NNSi_G3dGetVisAnm.

  Revision 1.7  09/15/2004 04:51:55  nishida_kenji
  revised

  Revision 1.6  07/22/2004 06:02:59  kitani_toshikazu
  Renamed Functions
  NNS_G3dGetTexPatAnmDataByIdx()
      => NNSi_G3dGetTexPatAnmDataByIdx()
  NNSi_G3dGetTexNameByIdx()
      => NNSi_G3dGetTexPatAnmTexNameByIdx()
  NNSi_G3dGetPlttNameByIdx()
      => NNSi_G3dGetTexPatAnmPlttNameByIdx()

  Revision 1.5  07/20/2004 01:37:06  nishida_kenji
  fixed comments

  Revision 1.4  06/28/2004 08:56:50  kitani_toshikazu
  Described the animation type related to the accessor

  Revision 1.3  06/16/2004 23:59:38  nishida_kenji
  NitroSystem -> NITRO-System

  Revision 1.2  06/14/2004 06:24:18  nishida_kenji
  (none)

  Revision 1.1  06/11/2004 04:10:30  nishida_kenji
  Initial check in.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "./res_print_internal.h"


#include <nnsys/g3d/binres/res_struct.h>
#include <nnsys/g3d/binres/res_print.h>
#include <nnsys/g3d/binres/res_struct_accessor.h>


#if defined(SDK_DEBUG) || defined(_CVTR)


void
NNS_G3dPrintVisAnm(const NNSG3dResVisAnm* pResVisAnm )
{
    u16 i,j;
    NNS_G3D_NULL_ASSERT( pResVisAnm );
    
    tabPrint_(); RES_PRINTF( "# of frames, %d\n", pResVisAnm->numFrame );
    tabPrint_(); RES_PRINTF( "# of nodes, %d\n", pResVisAnm->numNode );
    tabPrint_(); RES_PRINTF( "size of animation, %d bytes\n", pResVisAnm->size );

    //
    // For each node
    //
    tabPlus_();
    tabPrint_(); RES_PRINTF("    , "
                            "0         "
                            "1         "
                            "2         "
                            "3         "
                            "4         "
                            "5         \n");
    for( i = 0; i < pResVisAnm->numNode; i++ )
    {
        tabPrint_(); RES_PRINTF(" %3d, ", i);
        for( j = 0; j < pResVisAnm->numFrame; j++ )
        {
            if (j % 60 == 0 && j != 0)
            {
                RES_PRINTF("\n");
                tabPrint_();
                RES_PRINTF("    , ");
            }
            {
                u32 pos = (u32)( j * pResVisAnm->numNode + i );
                u32 n = pos >> 5;
                u32 mask = 1UL << (pos & 0x1f);

                if (pResVisAnm->visData[n] & mask)
                    RES_PRINTF("1");
                else
                    RES_PRINTF("0");
            }
        }
        RES_PRINTF("\n");
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintVisAnmSet

  Description:  Shows NNSG3dResVisAnmSet content in debug output.
                
  Arguments:    pAnmSet:     This is a structure that gathers visibility animation sets.
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintVisAnmSet( const NNSG3dResVisAnmSet* pAnmSet )
{
    NNS_G3D_NULL_ASSERT(pAnmSet);
    NNS_G3D_ASSERT( pAnmSet->header.kind == NNS_G3D_DATABLK_VIS_ANM );
    {
        u8 i;
        tabPlus_();
        for( i = 0; i < pAnmSet->dict.numEntry; i++ )
        {
            const NNSG3dResVisAnm* pVisAnm;
            const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&pAnmSet->dict, i);
            chkDict_(&pAnmSet->dict, name, i);
            tabPrint_(); RES_PRINTF("Visibility Animation, ");
            NNS_G3dPrintResName(name); RES_PRINTF("\n");

            pVisAnm = NNS_G3dGetVisAnmByIdx( pAnmSet, i );
            NNS_G3dPrintVisAnm(pVisAnm);
        }
        tabMinus_();
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintNSBVA

  Description:  Shows NSBVA file content in debug output.
                
  Arguments:    binFile:     Pointer to top of NSBVA file data.
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNSBVA(const u8* binFile)
{
    u32                          i;
    const NNSG3dResFileHeader*   header;
    u32                          numBlocks;

    NNS_G3D_NULL_ASSERT(binFile);
   
    header     = (const NNSG3dResFileHeader*)&binFile[0];
    NNS_G3D_ASSERT( header->sigVal == NNS_G3D_SIGNATURE_NSBVA );
    NNS_G3dPrintFileHeader(header);

    //
    // Regarding all blocks...
    //
    numBlocks  = header->dataBlocks;
    for ( i = 0; i < numBlocks; ++i )
    {
        const NNSG3dResDataBlockHeader* blk;

        blk = NNS_G3dGetDataBlockHeaderByIdx( header, i );
        NNS_G3D_NULL_ASSERT( blk );
        NNS_G3dPrintDataBlockHeader(blk);

        //
        // Generate summary debug output for each type
        //
        tabPlus_();
        switch( blk->kind )
        {
        case NNS_G3D_DATABLK_VIS_ANM:
            NNS_G3dPrintVisAnmSet((const NNSG3dResVisAnmSet*)blk);
            break;
        default:
            tabPlus_();
            tabPrint_(); RES_PRINTF("cannot display this block\n");
            tabMinus_();
            break;
        }
        tabMinus_();
    }
}

#endif

