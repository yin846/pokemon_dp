/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  File:     g2di_BGManipulator.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_BGManipulator.c,v $
  Revision 1.1  06/27/2005 05:55:01 AM  hatamoto_minoru
  Separated from g2d_Screen.c 

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "g2di_BGManipulator.h"



//---- BGnCNT register table
REGType16v* const NNSiG2dBGCNTTable[] =
{
    &reg_G2_BG0CNT, &reg_G2_BG1CNT, &reg_G2_BG2CNT, &reg_G2_BG3CNT,
    &reg_G2S_DB_BG0CNT, &reg_G2S_DB_BG1CNT, &reg_G2S_DB_BG2CNT, &reg_G2S_DB_BG3CNT
};


/*---------------------------------------------------------------------------*
  Name:         NNSi_G2dBGGetCharSize

  Description:  Obtains the size of the target BG screen in characters. 

  Arguments:    pWidth:     Pointer to buffer that holds BG screen width. 
                pHeight:    Pointer to buffer that holds BG screen height. 
                n:          The target BG screen.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NNSi_G2dBGGetCharSize(int* pWidth, int* pHeight, NNSG2dBGSelect n)
{
    const int bgNo = GetBGNo(n);
    const int scnSize = (*GetBGnCNT(n) & REG_G2_BG0CNT_SCREENSIZE_MASK)
                            >> REG_G2_BG0CNT_SCREENSIZE_SHIFT;
    const int bgMode = IsMainBG(n) ? GetBGModeMain(): GetBGModeSub();
    const BOOL bAffine = ( ((bgNo == 2) && ((bgMode == 2) || (bgMode >= 4)))
                            ||  ((bgNo == 3) && (bgMode >= 1)) );

    if( bAffine )
    {
        const int size = (16 << scnSize);
        *pWidth  = size;
        *pHeight = size;
    }
    else
    {
        *pWidth  = (scnSize & 1) ? 64: 32;
        *pHeight = (scnSize & 2) ? 64: 32;
    }
}

