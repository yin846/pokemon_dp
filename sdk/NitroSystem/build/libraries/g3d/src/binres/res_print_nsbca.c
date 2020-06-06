/*---------------------------------------------------------------------------*
  Project:  NITRO-System - G3D(Hokotate) -
  File:     res_print_nsbca.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: res_print_nsbca.c,v $
  Revision 1.10  03/02/2005 00:40:24  takano_makoto
  fix copyright header.

  Revision 1.9  02/28/2005 00:14:37  takano_makoto
  Warning measures for signed<->unsigned casts

  Revision 1.8  09/15/2004 04:52:04  nishida_kenji
  small fix

  Revision 1.7  09/13/2004 11:01:48  nishida_kenji
  revised

  Revision 1.6  07/26/2004 01:33:14  hatamoto_minoru
  change function name.
  NNS_G3dGetNNSG3dResJntAnmByIdx -> NNS_G3dGetJntAnmByIdx

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

/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintJntAnm

  Description:  Displays NNSG3dResJntAnm content in debug output.
                
  Arguments:    pJntAnm:           joint  animation
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void 
NNS_G3dPrintJntAnm( const NNSG3dResJntAnm* pJntAnm )
{
    int i;

    NNS_G3D_NULL_ASSERT( pJntAnm );
    tabPlus_();
    {
        //
        // Node count, joint count
        //
        tabPrint_(); RES_PRINTF("# of frames, %d\n", pJntAnm->numFrame);
        tabPrint_(); RES_PRINTF("# of nodes, %d\n", pJntAnm->numNode);
        
        //
        // Display a summary of all nodes.
        //
        tabPlus_(); 
        for( i = 0; i < pJntAnm->numNode; i++ )
        {
            tabPrint_(); RES_PRINTF(" %d", i);
            {
                u32 tag = NNS_G3dGetJntAnmSRTTag(pJntAnm, i)->tag;
                
                if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY)
                    RES_PRINTF(", IdentitySRT");
                else
                {
                    if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_T)
                        RES_PRINTF(", IdentityT");
                    else
                    {
                        if (tag & NNS_G3D_JNTANM_SRTINFO_BASE_T)
                            RES_PRINTF(", UseBaseT");
                        else
                        {
                            if (tag & NNS_G3D_JNTANM_SRTINFO_CONST_TX)
                                RES_PRINTF(", ConstTX");
                            if (tag & NNS_G3D_JNTANM_SRTINFO_CONST_TY)
                                RES_PRINTF(", ConstTY");
                            if (tag & NNS_G3D_JNTANM_SRTINFO_CONST_TZ)
                                RES_PRINTF(", ConstTZ");
                        }  
                    }
                    
                    if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_R)
                        RES_PRINTF(", IdentityR");
                    else
                    {
                        if (!(tag & NNS_G3D_JNTANM_SRTINFO_BASE_R))
                        {
                            if (tag & NNS_G3D_JNTANM_SRTINFO_CONST_R)
                                RES_PRINTF(", ConstR");
                        }
                        else
                            RES_PRINTF(", UseBaseR");
                    }

                    if (tag & NNS_G3D_JNTANM_SRTINFO_IDENTITY_S)
                        RES_PRINTF(", IdentityS");
                    else
                    {
                        if (tag & NNS_G3D_JNTANM_SRTINFO_BASE_S)
                            RES_PRINTF(", UseBaseS");
                        else
                        {
                            if (tag & NNS_G3D_JNTANM_SRTINFO_CONST_SX)
                                RES_PRINTF(", ConstSX");
                            if (tag & NNS_G3D_JNTANM_SRTINFO_CONST_SY)
                                RES_PRINTF(", ConstSY");
                            if (tag & NNS_G3D_JNTANM_SRTINFO_CONST_SZ)
                                RES_PRINTF(", ConstSZ");
                        }
                    }
                }
                RES_PRINTF("\n");
            }
        }
        tabMinus_();
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintJntAnmSet

  Description:  Displays NNS_G3dPrintJntAnmSet content in debug output.
                
  Arguments:    pJntAnmSet:           Joint animation set
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void 
NNS_G3dPrintJntAnmSet( const NNSG3dResJntAnmSet* pJntAnmSet )
{
    u32 i;

    NNS_G3D_NULL_ASSERT( pJntAnmSet );
    
    tabPlus_();
    {
        //
        // For each file name entry...
        //
        for( i = 0; i < pJntAnmSet->dict.numEntry ; i++ )
        {
            const NNSG3dResJntAnm* pJntAnm;
            const NNSG3dResName* name = NNS_G3dGetResNameByIdx(&pJntAnmSet->dict, i);
            chkDict_(&pJntAnmSet->dict, name, i);
            tabPrint_(); RES_PRINTF("Joint Animation, ");
            NNS_G3dPrintResName(name); RES_PRINTF("\n");

            pJntAnm = NNS_G3dGetJntAnmByIdx( pJntAnmSet, (int)i );
            NNS_G3dPrintJntAnm( pJntAnm );
        }
    }
    tabMinus_();
}


/*---------------------------------------------------------------------------*
  Name:         NNS_G3dPrintNSBCA

  Description:  Displays content of NSBCA file in debug output.
                
  Arguments:    binFile:     Pointer to top of NSBCA file data
                
  Returns:      None
  
 *---------------------------------------------------------------------------*/
void
NNS_G3dPrintNSBCA(const u8* binFile)
{
    u32                          i;
    const NNSG3dResFileHeader*   header;
    u32                          numBlocks;
    NNS_G3D_NULL_ASSERT(binFile);
   
    header     = (const NNSG3dResFileHeader*)&binFile[0];
    NNS_G3D_ASSERT( header->sigVal == NNS_G3D_SIGNATURE_NSBCA );
    NNS_G3dPrintFileHeader(header);

    numBlocks  = header->dataBlocks;
    for ( i = 0; i < numBlocks; ++i )
    {
        const NNSG3dResDataBlockHeader* blk =
            NNS_G3dGetDataBlockHeaderByIdx( header, i );
        NNS_G3dPrintDataBlockHeader(blk);

        //
        // Generate summary debug output for each type
        //
        tabPlus_();
        {
            switch( blk->kind )
            {
            case NNS_G3D_DATABLK_JNT_ANM:
                NNS_G3dPrintJntAnmSet((const NNSG3dResJntAnmSet*)blk);
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


