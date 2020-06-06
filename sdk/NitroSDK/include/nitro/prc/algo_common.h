/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - include
  File:     prc/algo_common.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: algo_common.h,v $
  Revision 1.12  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.11  03/04/2005 12:45:07  seiki_masashi
  Changed so that PRC_InitPrototypeDB and PRC_InitInputPattern return whether create succeeded or failed.

  Revision 1.10  02/28/2005 05:26:01  yosizaki
  do-indent.

  Revision 1.9  11/10/2004 01:00:25  seiki_masashi
  Added a union so that access is possible with normalizeSize and regularizeSize

  Revision 1.8  11/04/2004 07:59:53  seiki_masashi
  Revised regularize to normalize

  Revision 1.7  07/19/2004 06:52:06  seiki_masashi
  Arrangement of identifier names

  Revision 1.6  07/01/2004 09:05:31  seiki_masashi
  Minor fixes

  Revision 1.5  06/29/2004 02:13:28  seiki_masashi
  remove lineSegmentRatioToWholeArray

  Revision 1.4  06/28/2004 13:18:22  seiki_masashi
  Addition of PRC_GetInputPatternStrokes

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

  Revision 1.3  06/22/2004 11:19:00  seiki_masashi
  Operable Version (simple only)

  Revision 1.2  06/17/2004 08:14:52  seiki_masashi
  PRC_GetPrototypeDBBufferSizeEx

  Revision 1.1  06/17/2004 02:30:29  seiki_masashi
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

#ifndef NITRO_PRC_ALGO_COMMON_H_
#define NITRO_PRC_ALGO_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/prc/types.h>
#include <nitro/prc/common.h>


/*===========================================================================*
  Macros
 *===========================================================================*/

//--- For Get*BufferSize
#define PRCi_ARRAY_SIZE(type, size, dummy) \
    sizeof(type) * (size)

//--- For Set*BufferInfo
#define PRCi_ALLOC_ARRAY(dst, type, size, base, offset) \
    (dst) = (type*)((u8*)(base) + (offset)); \
    (offset) += sizeof(type) * (size)


/*===========================================================================*
  Constant Definitions
 *===========================================================================*/

#define PRC_RESAMPLE_DEFAULT_METHOD_COMMON PRC_RESAMPLE_METHOD_RECURSIVE
#define PRC_RESAMPLE_DEFAULT_THRESHOLD_COMMON 5

/*===========================================================================*
  Type Definitions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Parameters for Sample DB Expansion
 *---------------------------------------------------------------------------*/
typedef struct PRCPrototypeDBParam_Common
{
    int     dummy;
}
PRCPrototypeDBParam_Common;

/*---------------------------------------------------------------------------*
  Parameters for Input Pattern Interpretation
    normalizeSize       Normalize Size
    resampleMethod      type of algorithm used for normalization
    resampleThreshold   post-normalization resample threshold
 *---------------------------------------------------------------------------*/
typedef struct PRCInputPatternParam_Common
{
    union
    {
        int     normalizeSize;
        int     regularizeSize;        // Old name
    };
    PRCResampleMethod resampleMethod;
    int     resampleThreshold;
}
PRCInputPatternParam_Common;

/*---------------------------------------------------------------------------*
  Common Parts of Sample DB Origin and Input Origin Recognition Pattern
 *---------------------------------------------------------------------------*/
typedef struct PRCiPatternData_Common
{
    u16     strokeCount;               // entry and duplicate information    
    u16     pointCount;                // entry and duplicate information    
    const PRCPoint *pointArray;
    int    *strokes;
    int    *strokeSizes;
    fx32   *lineSegmentLengthArray;
    fx16   *lineSegmentRatioToStrokeArray;
//    fx16                        *lineSegmentRatioToWholeArray;
    u16    *lineSegmentAngleArray;
    fx32   *strokeLengths;
    fx16   *strokeRatios;
    fx32    wholeLength;
    PRCBoundingBox *strokeBoundingBoxes;
    PRCBoundingBox wholeBoundingBox;
}
PRCiPatternData_Common;

/*---------------------------------------------------------------------------*
  Buffer Information for forming pattern dynamically during input.
  It is possible each recognition algorithm is expanding based on this so
  when changing you need to verify the -BufferInfo_* for each algorithm.
 *---------------------------------------------------------------------------*/
typedef struct PRCiPatternBufferInfo_Common
{
    PRCPoint *pointArray;
    int    *strokes;
    int    *strokeSizes;
    fx32   *lineSegmentLengthArray;
    fx16   *lineSegmentRatioToStrokeArray;
//    fx16                        *lineSegmentRatioToWholeArray;
    u16    *lineSegmentAngleArray;
    fx32   *strokeLengths;
    fx16   *strokeRatios;
    PRCBoundingBox *strokeBoundingBoxes;
}
PRCiPatternBufferInfo_Common;

/*---------------------------------------------------------------------------*
  Sample DB Origin Recognition Pattern
 *---------------------------------------------------------------------------*/
typedef struct PRCiPrototypeEntry_Common
{
    PRCiPatternData_Common data;
    const PRCPrototypeEntry *entry;
}
PRCiPrototypeEntry_Common;

/*---------------------------------------------------------------------------*
  Recognition Pattern of Input Origin
 *---------------------------------------------------------------------------*/
typedef struct PRCInputPattern_Common
{
    PRCiPatternData_Common data;
    union
    {
        int     normalizeSize;
        int     regularizeSize;        // Old name
    };

    void   *buffer;
    u32     bufferSize;
}
PRCInputPattern_Common;

/*---------------------------------------------------------------------------*
  Buffer Information for forming sample DB dynamically during input.
  It is possible each recognition algorithm is expanding based on this so
  when changing you need to verify the -BufferInfo_* for each algorithm.
 *---------------------------------------------------------------------------*/
typedef struct PRCiPrototypeListBufferInfo_Common
{
    PRCiPrototypeEntry_Common *patterns;
    fx32   *lineSegmentLengthArray;
    fx16   *lineSegmentRatioToStrokeArray;
//    fx16                        *lineSegmentRatioToWholeArray;
    u16    *lineSegmentAngleArray;
    int    *strokeArray;
    int    *strokeSizeArray;
    fx32   *strokeLengthArray;
    fx16   *strokeRatioArray;
    PRCBoundingBox *strokeBoundingBoxArray;
}
PRCiPrototypeListBufferInfo_Common;

/*---------------------------------------------------------------------------*
  Sample DB Expanded in Memory
 *---------------------------------------------------------------------------*/
typedef struct PRCPrototypeDB_Common
{
    PRCiPrototypeEntry_Common *patterns;
    int     patternCount;

    fx32   *lineSegmentLengthArray;
    fx16   *lineSegmentRatioToStrokeArray;
//    fx16                        *lineSegmentRatioToWholeArray;
    u16    *lineSegmentAngleArray;
    int     wholePointCount;

    int    *strokeArray;
    int    *strokeSizeArray;
    fx32   *strokeLengthArray;
    fx16   *strokeRatioArray;
    PRCBoundingBox *strokeBoundingBoxArray;
    int     wholeStrokeCount;

    int     maxPointCount;
    int     maxStrokeCount;

    union
    {
        int     normalizeSize;
        int     regularizeSize;        // Old name
    };

    const PRCPrototypeList *prototypeList;
    void   *buffer;
    u32     bufferSize;
}
PRCPrototypeDB_Common;


/*---------------------------------------------------------------------------*
  Name:         PRC_Init_Common

  Description:  Initializes pattern recognition API.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void PRC_Init_Common(void)
{
    PRCi_Init();
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSizeEx_Common

  Description:  Calculates the work area size required for expanding sample DB.

  Arguments:    prototypeList   sample pattern list
                kindMask        mask for type specification
                ignoreDisabledEntries   expand sample DB entries with enabled    
                                        flag of FALSE or not expand     
                param           parameters for sample DB expansion

  Returns:      required memory capacity for expanding sample DB
 *---------------------------------------------------------------------------*/
u32
 
 
PRC_GetPrototypeDBBufferSizeEx_Common(const PRCPrototypeList *prototypeList,
                                      u32 kindMask,
                                      BOOL ignoreDisabledEntries,
                                      const PRCPrototypeDBParam_Common *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_GetPrototypeDBBufferSize_Common

  Description:  Calculates the work area size required for expanding sample DB.

  Arguments:    prototypeList   sample pattern list

  Returns:      required memory capacity for expanding sample DB
 *---------------------------------------------------------------------------*/
u32 static inline PRC_GetPrototypeDBBufferSize_Common(const PRCPrototypeList *prototypeList)
{
    return PRC_GetPrototypeDBBufferSizeEx_Common(prototypeList, PRC_KIND_ALL, FALSE, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDBEx_Common

  Description:  Creates the PRCPrototypeDB structure.
                A buffer region larger than the size that PRC_GetPrototypeDBBufferSize
                returns needs to be set up in the buffer.
                Parameters during expansion can be specified with param.

  Arguments:    protoDB         initialized sample DB structure    
                buffer          pointer to memory region used for sample DB expansion
                                (Return value of memory area size >=PRC_GetPrototypeDBBufferSize)
                prototypeList   sample pattern list
                kindMask        mask for type specification
                ignoreDisabledEntries   expand sample DB entries with enabled    
                                        flag of FALSE or not expand     
                param           parameters for sample DB expansion

  Returns:      TRUE if create succeeded.
 *---------------------------------------------------------------------------*/
BOOL
 
 PRC_InitPrototypeDBEx_Common(PRCPrototypeDB_Common *protoDB,
                              void *buffer,
                              const PRCPrototypeList *prototypeList,
                              u32 kindMask,
                              BOOL ignoreDisabledEntries, const PRCPrototypeDBParam_Common *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_InitPrototypeDB_Common

  Description:  Creates the PRCPrototypeDB structure.
                A buffer region larger than the size that PRC_GetPrototypeDBBufferSize
                returns needs to be set up in the buffer.

  Arguments:    protoDB         initialized sample DB structure    
                buffer          pointer to memory region used for sample DB expansion
                                (Return value of memory area size >=PRC_GetPrototypeDBBufferSize)
                prototypeList   sample pattern list

  Returns:      TRUE if create succeeded.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitPrototypeDB_Common(PRCPrototypeDB_Common *protoDB,
                           void *buffer, const PRCPrototypeList *prototypeList)
{
    return PRC_InitPrototypeDBEx_Common(protoDB, buffer, prototypeList, PRC_KIND_ALL, FALSE, NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternBufferSize_Common

  Description:  Calculates work area required for expanding pattern data for
                comparison.

  Arguments:    maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count

  Returns:      memory capacity required for expanding pattern
 *---------------------------------------------------------------------------*/
u32     PRC_GetInputPatternBufferSize_Common(int maxPointCount, int maxStrokeCount);

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPatternEx_Common

  Description:  Creates PRCInputPattern structure.
                Specification of parameters for input pattern interpretation can be done with param.

  Arguments:    pattern             initialized pattern structure    
                buffer              pointer to memory region used for pattern expansion
                                    (Return value of area size >=PRC_GetInputPatternBufferSize)
                strokes             raw input coordinate value before forming
                maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count
                param               parameters for input pattern interpretation

  Returns:      TRUE if create succeeded.
 *---------------------------------------------------------------------------*/
BOOL
 
 PRC_InitInputPatternEx_Common(PRCInputPattern_Common *pattern,
                               void *buffer,
                               const PRCStrokes *strokes,
                               int maxPointCount,
                               int maxStrokeCount, const PRCInputPatternParam_Common *param);

/*---------------------------------------------------------------------------*
  Name:         PRC_InitInputPattern_Common

  Description:  Creates PRCInputPattern structure.

  Arguments:    pattern             initialized pattern structure    
                buffer              pointer to memory region used for pattern expansion
                                    (Return value of area size >=PRC_GetInputPatternBufferSize)
                strokes             raw input coordinate value before forming
                maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count

  Returns:      TRUE if create succeeded.
 *---------------------------------------------------------------------------*/
static inline BOOL
PRC_InitInputPattern_Common(PRCInputPattern_Common *pattern,
                            void *buffer,
                            const PRCStrokes *strokes, int maxPointCount, int maxStrokeCount)
{
    return PRC_InitInputPatternEx_Common(pattern, buffer, strokes, maxPointCount, maxStrokeCount,
                                         NULL);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetInputPatternStrokes_Common

  Description:  Obtains dot sequence data from PRCInputPattern structure.

  Arguments:    strokes         dot sequence data obtained
                                cannot be rewritten     
                input           input pattern

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    PRC_GetInputPatternStrokes_Common(PRCStrokes *strokes, const PRCInputPattern_Common *input);


// for debug
void    PRCi_PrintPatternData_Common(PRCiPatternData_Common *data);

/*===========================================================================*
  Inline Functions
 *===========================================================================*/

#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NITRO_PRC_ALGO_COMMON_H_ */
#endif
