/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - 
  File:     prc.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: prc.c,v $
  Revision 1.7  01/18/2006 02:12:27  kitase_hirotake
  do-indent

  Revision 1.6  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.5  07/19/2004 06:52:06  seiki_masashi
  Arrangement of identifier names

  Revision 1.4  06/25/2004 13:32:11  seiki_masashi
  Addition of recognition algorithm "Standard"

  Revision 1.3  06/24/2004 08:07:34  seiki_masashi
  Support for double calling of PRCi_Init

  Revision 1.2  06/24/2004 07:03:39  seiki_masashi
  Partitioned and arranged header file

  Revision 1.1  06/23/2004 11:59:30  seiki_masashi
  add 'PRC*' to SDK

  Revision 1.3  06/17/2004 02:30:29  seiki_masashi
  To facilitate recognition algorithm changes, split into prc_algo_*    

  Revision 1.2  06/17/2004 00:52:36  seiki_masashi
  PRC_InitPrototypeDBEX, PRC_InitInputPatternEX

  Revision 1.1  06/16/2004 23:56:09  seiki_masashi
  Renamed ptrcg to prc.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/prc/common.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/

/*===========================================================================*
  Variable Definitions
 *===========================================================================*/
static BOOL PRCi_Initialized = FALSE;

/*===========================================================================*
  Functions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRCi_Init

  Description:  Initializes pattern recognition API.
                Called initially from initialization routines in all recognition algorithms.     

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void PRCi_Init(void)
{
    if (PRCi_Initialized == TRUE)
        return;

    // Initialize Something...

    PRCi_Initialized = TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetEntryStrokes

  Description:  Obtains dot sequence data from sample DB and sample DB entry.

  Arguments:    strokes         dot sequence data obtained
                                cannot be rewritten     
                prototypeList   sample pattern list
                entry       sample DB entry

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
PRC_GetEntryStrokes(PRCStrokes *strokes,
                    const PRCPrototypeList *prototypeList, const PRCPrototypeEntry *entry)
{
    if (entry != NULL)
    {
        strokes->points = (PRCPoint *)&prototypeList->pointArray[entry->pointIndex];
        strokes->size = entry->pointCount;
        strokes->capacity = entry->pointCount;
    }
    else
    {
        strokes->points = NULL;
        strokes->size = 0;
        strokes->capacity = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         PRC_CopyStrokes

  Description:  Deep copies dot sequence data.

  Arguments:    srcstrokes         PRC_Strokes structure copied from
                dststrokes              PRC_Strokes structure copied to

  Returns:      If can copy, TRUE.
 *---------------------------------------------------------------------------*/
BOOL PRC_CopyStrokes(const PRCStrokes *srcStrokes, PRCStrokes *dstStrokes)
{
    int     iPoint, size;
    PRCPoint *dstPoint;
    const PRCPoint *srcPoint;

    SDK_ASSERT(dstStrokes);
    SDK_ASSERT(srcStrokes);

    if (srcStrokes->size > dstStrokes->capacity)
    {
        return FALSE;
    }

    size = dstStrokes->size = srcStrokes->size;
    srcPoint = srcStrokes->points;
    dstPoint = dstStrokes->points;
    for (iPoint = 0; iPoint < size; iPoint++)
    {
        *dstPoint = *srcPoint;
        dstPoint++;
        srcPoint++;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
