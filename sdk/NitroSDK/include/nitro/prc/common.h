/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - include
  File:     prc/common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: common.h,v $
  Revision 1.7  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.6  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.5  07/19/2004 06:52:06  seiki_masashi
  Arrangement of identifier names

  Revision 1.4  06/29/2004 23:34:54  seiki_masashi
  PRC_IsEmpty

  Revision 1.3  06/29/2004 07:26:31  seiki_masashi
  PRC_IsPenUpMarker set to high speed

  Revision 1.2  06/25/2004 13:32:01  seiki_masashi
  Addition of recognition algorithm "Standard"

  Revision 1.1  06/24/2004 07:03:10  seiki_masashi
  Partitioned and arranged header file

  Revision 1.1  06/23/2004 12:00:29  seiki_masashi
  add 'PRC*' to SDK

  Revision 1.5  06/22/2004 11:19:00  seiki_masashi
  Operable Version (simple only)

  Revision 1.4  06/17/2004 08:14:52  seiki_masashi
  PRC_GetPrototypeDBBufferSizeEx

  Revision 1.3  06/17/2004 02:30:29  seiki_masashi
  To facilitate recognition algorithm changes, split into prc_algo_*    

  Revision 1.2  06/17/2004 00:52:36  seiki_masashi
  PRC_InitPrototypeDBEX, PRC_InitInputPatternEX

  Revision 1.1  06/16/2004 23:56:09  seiki_masashi
  Renamed ptrcg to prc.

  Revision 1.3  06/15/2004 08:10:55  seiki_masashi
  Made prototype program operable with dummy library    

  Revision 1.2  06/15/2004 05:55:09  seiki_masashi
  Interface improvements

  Revision 1.1  06/15/2004 00:54:42  seiki_masashi
  Interface design


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NITRO_PRC_COMMON_H_
#define NITRO_PRC_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>

#include <nitro/prc/types.h>

/*===========================================================================*
  Constant Definitions
 *===========================================================================*/

/*===========================================================================*
  Type Definitions
 *===========================================================================*/


/*===========================================================================*
  Function Declaration
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRCi_Init

  Description:  Initializes pattern recognition API.
                Called at beginning from initialization routine of algorithms.    

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    PRCi_Init(void);

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
                     const PRCPrototypeList *prototypeList, const PRCPrototypeEntry *entry);

/*---------------------------------------------------------------------------*
  Name:         PRC_CopyStrokes

  Description:  Deep copies dot sequence data.

  Arguments:    srcStrokes      copy souce PRC_Strokes structure
                dstStrokes      copy destination PRC_Strokes structure

  Returns:      If can copy, TRUE.
 *---------------------------------------------------------------------------*/
BOOL    PRC_CopyStrokes(const PRCStrokes *srcStrokes, PRCStrokes *dstStrokes);

/*===========================================================================*
  Inline Functions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRC_GetEntryCode

  Description:  Returns code number that replies to sample DB entry.     

  Arguments:    entry       sample DB entry

  Returns:      code number
 *---------------------------------------------------------------------------*/
static inline int PRC_GetEntryCode(const PRCPrototypeEntry *entry)
{
    return (entry != NULL) ? entry->code : -1;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetEntryData

  Description:  Returns user data associated with the sample DB entry.

  Arguments:    entry       sample DB entry

  Returns:      user data
 *---------------------------------------------------------------------------*/
static inline void *PRC_GetEntryData(const PRCPrototypeEntry *entry)
{
    return (entry != NULL) ? entry->data : NULL;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitStrokes

  Description:  Initializes PRCStrokes structure.

  Arguments:    strokes     initialization target
                points      pointer to PRCPoint array which becomes actual memory region
                size        size of array

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_InitStrokes(PRCStrokes *strokes, PRCPoint *points, u32 size)
{
    SDK_ASSERT(strokes && points);
    strokes->points = points;
    strokes->capacity = size;
    strokes->size = 0;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_AppendPoint

  Description:  Adds dots to PRCStrokes.

  Arguments:    strokes     target
                x, y        added points

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_AppendPoint(PRCStrokes *strokes, int x, int y)
{
    SDK_ASSERT(strokes);
    SDK_ASSERT(strokes->size < strokes->capacity);
    strokes->points[strokes->size].x = (s16)x;
    strokes->points[strokes->size].y = (s16)y;
    (strokes->size)++;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_AppendPenUpMarker

  Description:  Adds marker that means pen up to PRCStrokes.

  Arguments:    strokes     target

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_AppendPenUpMarker(PRCStrokes *strokes)
{
    PRC_AppendPoint(strokes, PRC_PEN_UP_MARKER_X, PRC_PEN_UP_MARKER_Y);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_Clear

  Description:  Empties PRCStrokes.

  Arguments:    strokes     target

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_Clear(PRCStrokes *strokes)
{
    SDK_ASSERT(strokes);
    strokes->size = 0;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_IsFull

  Description:  Determines if PRCStrokes is full.

  Arguments:    strokes     target

  Returns:      If full, TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL PRC_IsFull(const PRCStrokes *strokes)
{
    SDK_ASSERT(strokes);
    return strokes->size >= strokes->capacity;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_IsEmpty

  Description:  Determines if PRCStrokes is empty or not.

  Arguments:    strokes     target

  Returns:      If empty, TRUE.
 *---------------------------------------------------------------------------*/
static inline BOOL PRC_IsEmpty(const PRCStrokes *strokes)
{
    SDK_ASSERT(strokes);
    return strokes->size == 0;
}

/*---------------------------------------------------------------------------*
  Name:         PRC_IsPenUpMarker

  Description:  Determines if marker that means pen up.

  Arguments:    point       target checked

  Returns:      If pen up marker, TRUE.
 *---------------------------------------------------------------------------*/
static inline int PRC_IsPenUpMarker(const PRCPoint *point)
{
    SDK_ASSERT(point);
    // If point->x == PRC_PEN_UP_MARKER_X, then point->y==PRC_PEN_UP_MARKER_Y
    SDK_ASSERT((point->x != PRC_PEN_UP_MARKER_X) || (point->y == PRC_PEN_UP_MARKER_Y));
    return (point->x == PRC_PEN_UP_MARKER_X);
}

/*---------------------------------------------------------------------------*
  Name:         PRCi_CalcDistance

  Description:  Calculates Euclid distance.

  Arguments:    p1, p2      target checked

  Returns:      Euclid distance (fx32).
 *---------------------------------------------------------------------------*/
static inline fx32 PRCi_CalcDistance(const PRCPoint *p1, const PRCPoint *p2)
{
    SDK_ASSERT(p1 && p2);
    return (FX_Sqrt
            (((p1->x - p2->x) * (p1->x - p2->x) +
              (p1->y - p2->y) * (p1->y - p2->y)) << FX32_SHIFT));
}

/*---------------------------------------------------------------------------*
  Name:         PRCi_CalcAngle

  Description:  Calculates angle.

  Arguments:    p1, p2      target checked

  Returns:      Angle in (2pi/65536) radian units (u16). 
 *---------------------------------------------------------------------------*/
static inline u16 PRCi_CalcAngle(const PRCPoint *from, const PRCPoint *to)
{
    SDK_ASSERT(from && to);
    return ((u16)(FX_Atan2Idx(to->y - from->y, to->x - from->x)));
}


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_PRC_COMMON_H_ */
#endif
