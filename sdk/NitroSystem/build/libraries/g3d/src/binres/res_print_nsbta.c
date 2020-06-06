/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_print_nsbta.c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_print_nsbta.c,v $
  Revision 1.6  09/17/2004 01:58:43  nishida_kenji
  REVISED

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


static void
printSTData_(u32 info, u32 data)
{
    if (info & NNS_G3D_TEXSRTANM_ELEM_CONST)
    {
        RES_PRINTF("constant, %f\n", (fx32)data / 4096.f);
    }
    else
    {
        u32 stepType = info & NNS_G3D_TEXSRTANM_ELEM_STEP_MASK;
        u32 step;
        if (stepType == NNS_G3D_TEXSRTANM_ELEM_STEP_4)
            step = 4;
        else if (stepType == NNS_G3D_TEXSRTANM_ELEM_STEP_2)
            step = 2;
        else
            step = 1;
        RES_PRINTF("frame step, %d\n", step);
    }
}

static void
printRData_(u32 info, u32 data)
{
    if (info & NNS_G3D_TEXSRTANM_ELEM_CONST)
    {
        fx16 s = (fx16)(data & 0x0000ffff);
        fx16 c = (fx16)(data >> 16);
        RES_PRINTF("constant, sin, %f, cos, %f\n", s / 4096.f, c / 4096.f);
    }
    else
    {
        u32 stepType = info & NNS_G3D_TEXSRTANM_ELEM_STEP_MASK;
        u32 step;
        if (stepType == NNS_G3D_TEXSRTANM_ELEM_STEP_4)
            step = 4;
        else if (stepType == NNS_G3D_TEXSRTANM_ELEM_STEP_2)
            step = 2;
        else
            step = 1;
        RES_PRINTF("frame step, %d\n", step);
    }
}


void
NNS_G3dPrintTexSRTAnm(const NNSG3dResTexSRTAnm* pTexSRTAnm)
{
    u8 j;
    tabPlus_();
    for( j = 0; j < pTexSRTAnm->dict.numEntry; j++ )
    {
        const NNSG3dResDictTexSRTAnmData* pAnmData;
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&pTexSRTAnm->dict, j);
        chkDict_(&pTexSRTAnm->dict, name, j);
        tabPrint_(); RES_PRINTF("Target material name, ");
        NNS_G3dPrintResName(name); RES_PRINTF("\n");
        
        pAnmData = (const NNSG3dResDictTexSRTAnmData*)
            NNS_G3dGetResDataByIdx( &pTexSRTAnm->dict, j);

        tabPlus_();
        tabPrint_(); RES_PRINTF("scaleS, "); printSTData_(pAnmData->scaleS, pAnmData->scaleSEx);
        tabPrint_(); RES_PRINTF("scaleT, "); printSTData_(pAnmData->scaleT, pAnmData->scaleTEx);

        tabPrint_(); RES_PRINTF("rot, "); printRData_(pAnmData->rot, pAnmData->rotEx);

        tabPrint_(); RES_PRINTF("transS, "); printSTData_(pAnmData->transS, pAnmData->transSEx);
        tabPrint_(); RES_PRINTF("transT, "); printSTData_(pAnmData->transT, pAnmData->transTEx);
        tabMinus_();
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintTexSRTAnmSet

  Description:  Show contents of NNSG3dResTexSRTAnmSet in debug print.
                
  Arguments:    pTexSRTSet:     The structure that gathers the sets of the texture SRT animations.
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintTexSRTAnmSet( const NNSG3dResTexSRTAnmSet* pAnmSet )
{
    u8 i;
    NNS_G3D_NULL_ASSERT(pAnmSet);
    NNS_G3D_ASSERT(pAnmSet->header.kind == NNS_G3D_DATABLK_TEXSRT_ANM);

    tabPrint_(); RES_PRINTF("# of animations, %d\n", pAnmSet->dict.numEntry);
    tabPlus_();

    // TODO : block header
    for( i = 0; i < pAnmSet->dict.numEntry; i ++ )
    {
        const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&pAnmSet->dict, i);
        const NNSG3dResTexSRTAnm* pTexSRTAnm = 
           NNS_G3dGetTexSRTAnmByIdx( pAnmSet, i );
        chkDict_(&pAnmSet->dict, name, i);
        tabPrint_(); RES_PRINTF("Texture SRT animation, ");
        NNS_G3dPrintResName(name); RES_PRINTF("\n");
        
        NNS_G3dPrintTexSRTAnm(pTexSRTAnm);
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintNSBTA

  Description:  Show contents of SBTA file in debug print.
                
  Arguments:    binFile:     Pointer to the start of the SBTA file data
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNSBTA(const u8* binFile)
{
    u32                          i;
    const NNSG3dResFileHeader*   header;
    u32                          numBlocks;

    NNS_G3D_NULL_ASSERT(binFile);

    header     = (const NNSG3dResFileHeader*)&binFile[0];
    NNS_G3D_ASSERT( header->sigVal == NNS_G3D_SIGNATURE_NSBTA );
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
        case NNS_G3D_DATABLK_TEXSRT_ANM:
            NNS_G3dPrintTexSRTAnmSet((const NNSG3dResTexSRTAnmSet*)blk);
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

