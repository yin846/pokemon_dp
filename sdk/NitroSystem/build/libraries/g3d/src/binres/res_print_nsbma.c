/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_print_nsbma.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_print_nsbma.c,v $
  Revision 1.7  09/17/2004 01:58:43  nishida_kenji
  REVISED

  Revision 1.6  07/20/2004 01:37:06  nishida_kenji
  fixed comments

  Revision 1.5  07/07/2004 11:54:21  kitani_toshikazu
  Fixed bug in access part of rgb value

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


static void
printColData_(u32 info)
{
    if (info & NNS_G3D_MATCANM_ELEM_CONST)
    {
        u32 r = info & 0x1f;
        u32 g = (info >> 5) & 0x1f;
        u32 b = (info >> 10) & 0x1f;
        RES_PRINTF(", constant, %d, %d, %d\n", r, g, b);
    }
    else
    {
        u32 stepType = info & NNS_G3D_MATCANM_ELEM_STEP_MASK;
        u32 step;
        if (stepType == NNS_G3D_MATCANM_ELEM_STEP_4)
            step = 4;
        else if (stepType == NNS_G3D_MATCANM_ELEM_STEP_2)
            step = 2;
        else
            step = 1;
        RES_PRINTF(", frame step, %d\n", step);
    }
}


static void
printAlphaData_(u32 info)
{
    if (info & NNS_G3D_MATCANM_ELEM_CONST)
    {
        RES_PRINTF(", constant, %d\n", info & NNS_G3D_MATCANM_ELEM_OFFSET_CONSTANT_MASK);
    }
    else
    {
        u32 stepType = info & NNS_G3D_MATCANM_ELEM_STEP_MASK;
        u32 step;
        if (stepType == NNS_G3D_MATCANM_ELEM_STEP_4)
            step = 4;
        else if (stepType == NNS_G3D_MATCANM_ELEM_STEP_2)
            step = 2;
        else
            step = 1;
        RES_PRINTF(", frame step, %d\n", step);
    }
}


void
NNS_G3dPrintMatColAnm(const NNSG3dResMatCAnm* pMatColAnm)
{
    u16 j;
    tabPlus_();
    for( j = 0; j < pMatColAnm->dict.numEntry; j++ )
    {
        const NNSG3dResDictMatCAnmData* pAnmData;
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&pMatColAnm->dict, j);
        chkDict_(&pMatColAnm->dict, name, j);
        tabPrint_(); RES_PRINTF("Target material name, ");
        NNS_G3dPrintResName(name); RES_PRINTF("\n");
        pAnmData = (const NNSG3dResDictMatCAnmData*)NNS_G3dGetResDataByIdx( &pMatColAnm->dict, j );

        tabPlus_();
        tabPrint_(); RES_PRINTF("diffuse"); printColData_(pAnmData->diffuse);
        tabPrint_(); RES_PRINTF("ambient"); printColData_(pAnmData->ambient);
        tabPrint_(); RES_PRINTF("specular"); printColData_(pAnmData->specular);
        tabPrint_(); RES_PRINTF("emission"); printColData_(pAnmData->emission);
        tabPrint_(); RES_PRINTF("alpha"); printAlphaData_(pAnmData->polygon_alpha);
        tabMinus_();
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintMatColAnmSet

  Description:  Displays the contents of NNSG3dResMatCAnmSet in the debug output.
                
  Arguments:    pAnmSet:     A structure that gathers the sets of the material color animation.
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintMatColAnmSet( const NNSG3dResMatCAnmSet* pAnmSet )
{
    u8    i;
    NNS_G3D_NULL_ASSERT( pAnmSet );
    NNS_G3D_ASSERT(pAnmSet->header.kind == NNS_G3D_DATABLK_MATC_ANM);
 
    tabPrint_(); RES_PRINTF("# of animations, %d\n", pAnmSet->dict.numEntry);
    tabPlus_();
    for( i = 0; i < pAnmSet->dict.numEntry; i ++ )
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&pAnmSet->dict, i);
        const NNSG3dResMatCAnm* pMatColAnm = NNS_G3dGetMatCAnmByIdx( pAnmSet, i );
        chkDict_(&pAnmSet->dict, name, i);
        tabPrint_(); RES_PRINTF("Material color animation, ");
        NNS_G3dPrintResName(name); RES_PRINTF("\n");
        NNS_G3dPrintMatColAnm(pMatColAnm);
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintNSBMA

  Description:  Displays the contents of NSBMA file in the debug output.
                
  Arguments:    binFile:     Pointer to the beginning of the NSBMA file data
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNSBMA(const u8* binFile)
{
    u32                          i;
    const NNSG3dResFileHeader*   header;
    u32                          numBlocks;
    
    NNS_G3D_NULL_ASSERT(binFile);
   
    header = (const NNSG3dResFileHeader*)&binFile[0];
    NNS_G3D_ASSERT( header->sigVal == NNS_G3D_SIGNATURE_NSBMA );
    NNS_G3dPrintFileHeader(header);

    //
    // Regarding all blocks...
    //
    numBlocks  = header->dataBlocks;
    for ( i = 0; i < numBlocks; ++i )
    {
        const NNSG3dResDataBlockHeader* blk 
            = NNS_G3dGetDataBlockHeaderByIdx( header, i );

        NNS_G3dPrintDataBlockHeader(blk);
        tabPlus_();
        switch( blk->kind )
        {
        case NNS_G3D_DATABLK_MATC_ANM:
            NNS_G3dPrintMatColAnmSet((const NNSG3dResMatCAnmSet*)blk);
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

