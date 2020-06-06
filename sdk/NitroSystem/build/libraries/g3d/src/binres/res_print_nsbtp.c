/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_print_nsbtp.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_print_nsbtp.c,v $
  Revision 1.9  09/17/2004 01:58:43  nishida_kenji
  REVISED

  Revision 1.8  07/22/2004 09:16:34  kitani_toshikazu
  Renamed NNS_G3dGetTexPatAnm() to NNS_G3dGetTexPatAnmByIdx()

  Revision 1.7  07/22/2004 06:02:59  kitani_toshikazu
  Renamed Functions
  NNS_G3dGetTexPatAnmDataByIdx()
      => NNSi_G3dGetTexPatAnmDataByIdx()
  NNSi_G3dGetTexNameByIdx()
      => NNSi_G3dGetTexPatAnmTexNameByIdx()
  NNSi_G3dGetPlttNameByIdx()
      => NNSi_G3dGetTexPatAnmPlttNameByIdx()

  Revision 1.6  07/20/2004 01:37:06  nishida_kenji
  fixed comments

  Revision 1.5  07/06/2004 04:20:57  kitani_toshikazu
  Changed to make use of NNSi_G3dGetTexPatAnmFVByFVIndex()

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


//---------------------------------------------------------------------
void 
NNS_G3dPrintTexPatAnm( const NNSG3dResTexPatAnm* pPatAnm )
{
    NNS_G3D_NULL_ASSERT( pPatAnm );
    
    {
        u32 i, j;
        const u32 numEntries = pPatAnm->dict.numEntry;
        tabPlus_();
        for( i = 0; i < numEntries; i++ )
        {
            const NNSG3dResDictTexPatAnmData* pAnmData = 
                NNSi_G3dGetTexPatAnmDataByIdx( pPatAnm, i );
            {
                const NNSG3dResName* name =
                    NNS_G3dGetResNameByIdx(&pPatAnm->dict, i);
                chkDict_(&pPatAnm->dict, name, i);
                tabPrint_(); RES_PRINTF("Target material name, ");
                NNS_G3dPrintResName(name); RES_PRINTF("\n");
            }

            tabPrint_(); RES_PRINTF("# of FV keys, %d\n", pAnmData->numFV);

            {
                const NNSG3dResTexPatAnmFV* pfvArray =
                    (const NNSG3dResTexPatAnmFV*)((u8*)pPatAnm + pAnmData->offset);
                
                tabPlus_();
                for( j = 0; j < pAnmData->numFV; j++ )
                {
                    const NNSG3dResTexPatAnmFV* pTexPatAnm =
                        NNSi_G3dGetTexPatAnmFVByFVIndex( pPatAnm, i, j );
                    tabPrint_();
                    RES_PRINTF("frame, %d, ", pTexPatAnm->idxFrame);
                    {
                        const NNSG3dResName* name =
                            (const NNSG3dResName*)((u8*)pPatAnm + pPatAnm->ofsTexName);
                        RES_PRINTF("texture, ");
                        NNS_G3dPrintResName(&name[pTexPatAnm->idTex]);
                    }
                    {
                        const NNSG3dResName* name =
                            (const NNSG3dResName*)((u8*)pPatAnm + pPatAnm->ofsPlttName);
                        RES_PRINTF(", palette, ");
                        NNS_G3dPrintResName(&name[pTexPatAnm->idPltt]);
                    }
                    RES_PRINTF("\n");
                }
                tabMinus_();
            }
        }
        tabMinus_();
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintTexPatAnmSet

  Description:  Displays the contents of NNSG3dResTexPatAnmSet in the debug display.
                
  Arguments:    pAnmSet:     The structure that gathers the sets of the texture pattern animations.
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintTexPatAnmSet( const NNSG3dResTexPatAnmSet* pAnmSet )
{
    NNS_G3D_NULL_ASSERT( pAnmSet );
    NNS_G3D_ASSERT( pAnmSet->header.kind == NNS_G3D_DATABLK_TEXPAT_ANM );
    
    {
        u8 i;
        const u8 numEntries = pAnmSet->dict.numEntry;

        tabPrint_(); RES_PRINTF("# of animations, %d\n", numEntries);
        tabPlus_();
        for( i = 0; i < numEntries; i++ )
        {
            const NNSG3dResName* name =
                NNS_G3dGetResNameByIdx(&pAnmSet->dict, i);
            chkDict_(&pAnmSet->dict, name, i);
            tabPrint_(); RES_PRINTF("Texture pattern animation, ");
            NNS_G3dPrintResName(name); RES_PRINTF("\n");

            NNS_G3dPrintTexPatAnm( NNS_G3dGetTexPatAnmByIdx( pAnmSet, i ) );
        }
        tabMinus_();
    }
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintNSBTP

  Description:  Displays the contents of NSBTP file in the debug display.
                
  Arguments:    binFile:     Pointer to the beginning of the NSBTP file data
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNSBTP(const u8* binFile)
{
    u32                          i;
    const NNSG3dResFileHeader*   header;
    u32                          numBlocks;
    NNS_G3D_NULL_ASSERT(binFile);
    
    header     = (const NNSG3dResFileHeader*)&binFile[0];
    NNS_G3D_ASSERT( header->sigVal == NNS_G3D_SIGNATURE_NSBTP );
    NNS_G3dPrintFileHeader(header);

    //
    // Regarding all blocks...
    //
    numBlocks  = header->dataBlocks;
    for ( i = 0; i < numBlocks; ++i )
    {
        const NNSG3dResDataBlockHeader* blk;
        blk = NNS_G3dGetDataBlockHeaderByIdx(header, i);
        NNS_G3D_NULL_ASSERT( blk );
        NNS_G3dPrintDataBlockHeader(blk);

        //
        // Generate summary debug output for each type
        //
        tabPlus_();
        {
            switch( blk->kind )
            {
            case NNS_G3D_DATABLK_TEXPAT_ANM:
                NNS_G3dPrintTexPatAnmSet((NNSG3dResTexPatAnmSet*)blk);
                break;
            default:
                tabPlus_();
                tabPrint_(); RES_PRINTF("cannot display this block\n");
                tabMinus_();
            break;
            }
        }
        tabMinus_();
    }    
}

#endif

