/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - include
  File:     prc/types.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: types.h,v $
  Revision 1.9  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.8  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.7  11/10/2004 01:00:25  seiki_masashi
  Added a union so that access is possible with normalizeSize and regularizeSize

  Revision 1.6  11/04/2004 07:59:53  seiki_masashi
  Revised regularize to normalize

  Revision 1.5  07/19/2004 06:52:06  seiki_masashi
  Arrangement of identifier names

  Revision 1.4  07/01/2004 09:05:31  seiki_masashi
  Minor fixes

  Revision 1.3  06/28/2004 11:10:41  seiki_masashi
  Added regularizeSize to prototype patterns list    
  Adjusted parameter types    

  Revision 1.2  06/25/2004 03:03:39  seiki_masashi
  Deleted bufferSize specification from various functions
  Addition of GetRecognitionBufferSize*
  Changed functions that only call other functions to inline

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

#ifndef NITRO_PRC_TYPES_H_
#define NITRO_PRC_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>

/*===========================================================================*
  Constant Definitions
 *===========================================================================*/

//--- Value of marking showing pen raised
#define PRC_PEN_UP_MARKER_X (-1)
#define PRC_PEN_UP_MARKER_Y (-1)

//--- Code showing not recognized or not recognizable
#define PRC_CODE_UNKNOWN (-1)

//--- kindMask that expresses all types
#define PRC_KIND_ALL      (0xffffffffU)

//--- watchdog coordinate value is sufficiently big/small
#define PRC_LARGE_ENOUGH_X (512)
#define PRC_SMALL_ENOUGH_X (-512)
#define PRC_LARGE_ENOUGH_Y (512)
#define PRC_SMALL_ENOUGH_Y (-512)

//--- small value with fx32 value (avoid division by 0)
#define PRC_TINY_LENGTH (1)

/*===========================================================================*
  Type Definitions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Two-Dimensional Coordinate
 *---------------------------------------------------------------------------*/
typedef struct PRCPoint
{
    s16     x;
    s16     y;
}
PRCPoint;

/*---------------------------------------------------------------------------*
  bounding box
 *---------------------------------------------------------------------------*/
typedef struct PRCBoundingBox
{
    s16     x1, y1;                    // upper left coordinate of bounding box
    s16     x2, y2;                    // lower right coordinate of bounding box
}
PRCBoundingBox;

/*---------------------------------------------------------------------------*
  Combination of Strokes
  Needs to terminated with PenUpMarker
 *---------------------------------------------------------------------------*/
typedef struct PRCStrokes
{
    PRCPoint *points;
    int     size;
    u32     capacity;
}
PRCStrokes;

/*---------------------------------------------------------------------------*
  Recognition Pattern
 *---------------------------------------------------------------------------*/
typedef struct PRCPrototypeEntry
{
    BOOL    enabled;
    u32     kind;
    u16     code;
    fx16    correction;
    void   *data;
    int     pointIndex;
    u16     pointCount;
    u16     strokeCount;
}
PRCPrototypeEntry;

/*---------------------------------------------------------------------------*
  Source Data for Sample DB
 *---------------------------------------------------------------------------*/
typedef struct PRCPrototypeList
{
    const PRCPrototypeEntry *entries;
    int     entrySize;
    const PRCPoint *pointArray;
    int     pointArraySize;

    union
    {
        int     normalizeSize;         // Normalize size of coordinate values in the sample DB
        int     regularizeSize;        // Old name
    };
}
PRCPrototypeList;

/*---------------------------------------------------------------------------*
  Resampling method
 *---------------------------------------------------------------------------*/
typedef enum PRCResampleMethod
{
    PRC_RESAMPLE_METHOD_NONE = 0,
    PRC_RESAMPLE_METHOD_DISTANCE,
    PRC_RESAMPLE_METHOD_ANGLE,
    PRC_RESAMPLE_METHOD_RECURSIVE,
    PRC_RESAMPLE_METHOD_USER = 256
}
PRCResampleMethod;


#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_PRC_TYPES_H_ */
#endif
