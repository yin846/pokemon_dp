/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - 
  File:     prc_algo_standard.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: prc_algo_standard.c,v $
  Revision 1.11  01/18/2006 02:12:27  kitase_hirotake
  do-indent

  Revision 1.10  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.9  11/04/2004 07:57:46  seiki_masashi
  Revised regularize to normalize

  Revision 1.8  07/19/2004 06:52:06  seiki_masashi
  Arrangement of identifier names

  Revision 1.7  07/05/2004 05:44:30  seiki_masashi
  changed variable names

  Revision 1.6  07/05/2004 02:38:41  seiki_masashi
  Removed prevRatio
  Allowed selection of near points     

  Revision 1.5  06/30/2004 12:53:44  seiki_masashi
  Deleted ASSERT that could be harmful

  Revision 1.4  06/30/2004 08:49:24  seiki_masashi
  algo_simple -> algo_light
  algo_standard -> algo_fine
  algo_accurate -> algo_superfine
  Addition of new algo_standard

  Revision 1.5  06/29/2004 07:25:02  seiki_masashi
  bug fixes
  Implemented pruning

  Revision 1.4  06/25/2004 03:02:55  seiki_masashi
  Deleted bufferSize specification from various functions
  Addition of GetRecognitionBufferSize*

  Revision 1.3  06/24/2004 08:08:33  seiki_masashi
  fix typo

  Revision 1.2  06/24/2004 07:03:39  seiki_masashi
  Partitioned and arranged header file

  Revision 1.1  06/23/2004 11:59:30  seiki_masashi
  add 'PRC*' to SDK

  Revision 1.3  06/22/2004 11:19:00  seiki_masashi
  Operable version (standard only)

  Revision 1.2  06/17/2004 08:14:52  seiki_masashi
  PRC_GetPrototypeDBBufferSizeEx

  Revision 1.1  06/17/2004 02:30:29  seiki_masashi
  To facilitate recognition algorithm changes, split into prc_algo_*    

  Revision 1.2  06/17/2004 00:52:36  seiki_masashi
  PRC_InitPrototypeDBEX, PRC_InitInputPatternEX

  Revision 1.1  06/16/2004 23:56:09  seiki_masashi
  Renamed ptrcg to prc.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/prc/algo_standard.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/
static inline int CityBlockDistance(const PRCPoint *p1, const PRCPoint *p2)
{
    int     x = p1->x - p2->x;
    int     y = p1->y - p2->y;
    if (x < 0)
        x = -x;
    if (y < 0)
        y = -y;
    return (x + y);
}

static inline void GetMiddlePoint(PRCPoint *p, const PRCPoint *p1, const PRCPoint *p2)
{
    p->x = (s16)((p1->x + p2->x) / 2);
    p->y = (s16)((p1->y + p2->y) / 2);
}

/*===========================================================================*
  Variable Definitions
 *===========================================================================*/

/*===========================================================================*
  Functions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSizeEx_Standard

  Description:  Calculates the work area size required for recognition algorithm.
                comparison.

  Arguments:    maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count
                protoDB     sample DB
                param       parameters for recognition processing

  Returns:      required memory capacity for recognition algorithm
 *---------------------------------------------------------------------------*/
u32
PRC_GetRecognitionBufferSizeEx_Standard(int maxPointCount,
                                        int maxStrokeCount,
                                        const PRCPrototypeDB_Standard *protoDB,
                                        const PRCRecognizeParam_Standard *param)
{
    (void)maxPointCount;
    (void)maxStrokeCount;
    (void)protoDB;
    (void)param;

    return 1;                          // Because OS_Alloc(0) causes an error
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntriesEx_Standard

  Description:  Compares and recognizes a specific kind entry and input pattern
                the top ranking of the result numRanking.

  Arguments:    resultEntries   Pointer to array that enters pointer to recognition result.
                                If can only recognize less than requested number,    
                                fill in remainder with NULL.
                resultScores    pointer to recognition result score array
                numRanking      number return to result*
                buffer      pointer to memory region used by recognition algorithm
                                (Return value of area size >=PRC_GetRecognitionBufferSize)
                input           input pattern
                protoDB     sample DB
                kindMask    Carry out logical multiplication of kind value for each sample DB entry. 
                                If non-zero considered valid.
                param       parameters for recognition processing

  Returns:      Number of patterns on compared sample DB    
 *---------------------------------------------------------------------------*/
int
PRC_GetRecognizedEntriesEx_Standard(PRCPrototypeEntry **resultEntries,
                                    fx32 *resultScores,
                                    int numRanking,
                                    void *buffer,
                                    const PRCInputPattern_Standard *input,
                                    const PRCPrototypeDB_Standard *protoDB,
                                    u32 kindMask, const PRCRecognizeParam_Standard *param)
{
    int     i;
    const PRCiPatternData_Common *inputData;
    int     numCompared;
    int     normalizeSize;
    int     doubleWidth;

    (void)buffer;
    (void)param;

    SDK_ASSERT(resultEntries);
    SDK_ASSERT(resultScores);
    SDK_ASSERT(input);
    SDK_ASSERT(protoDB);
    SDK_ASSERT(numRanking > 0);

    for (i = 0; i < numRanking; i++)
    {
        resultEntries[i] = NULL;
        resultScores[i] = 0;
    }

    normalizeSize = protoDB->normalizeSize;
    if (normalizeSize < input->normalizeSize)
    {
        normalizeSize = input->normalizeSize;
    }
    doubleWidth = normalizeSize * 2;

    inputData = &input->data;
    numCompared = 0;

    {
        const PRCiPrototypeEntry_Common *proto;
        int     iPattern;

        proto = protoDB->patterns;

        for (iPattern = 0; iPattern < protoDB->patternCount; iPattern++, proto++)
        {
            const PRCiPatternData_Common *protoData;
            int     iStroke;
            fx32    wholeScore;
            fx32    wholeWeight;

            if (!proto->entry->enabled || !(proto->entry->kind & kindMask))
                continue;

            protoData = &proto->data;

            if (inputData->strokeCount != protoData->strokeCount)
                continue;

            wholeScore = 0;
            wholeWeight = 0;

            for (iStroke = 0; iStroke < inputData->strokeCount; iStroke++)
            {
                int     iProto, iInput;
                int     protoStrokeIndex, inputStrokeIndex;
                int     protoSize, inputSize;
                const PRCPoint *protoPoints;
                const PRCPoint *inputPoints;
                const u16 *protoAngle;
                const u16 *inputAngle;
                const fx16 *protoRatio;
                const fx16 *inputRatio;
//                PRCPoint protoMidPoint, inputMidPoint;
                fx16    protoNextRatio, inputNextRatio;
                fx32    strokeScore;
                fx16    strokeRatio;

                strokeScore = 0;

                protoStrokeIndex = protoData->strokes[iStroke];
                inputStrokeIndex = inputData->strokes[iStroke];
                protoSize = protoData->strokeSizes[iStroke];
                inputSize = inputData->strokeSizes[iStroke];
                protoPoints = &protoData->pointArray[protoStrokeIndex];
                inputPoints = &inputData->pointArray[inputStrokeIndex];
                protoAngle = &protoData->lineSegmentAngleArray[protoStrokeIndex];
                inputAngle = &inputData->lineSegmentAngleArray[inputStrokeIndex];
                protoRatio = &protoData->lineSegmentRatioToStrokeArray[protoStrokeIndex];
                inputRatio = &inputData->lineSegmentRatioToStrokeArray[inputStrokeIndex];

                SDK_ASSERT(protoSize >= 2);
                SDK_ASSERT(inputSize >= 2);

                iProto = iInput = 1;
                protoNextRatio = protoRatio[iProto];
                inputNextRatio = inputRatio[iInput];
//                GetMiddlePoint(&protoMidPoint, &protoPoints[iProto-1], &protoPoints[iProto]);
//                GetMiddlePoint(&inputMidPoint, &inputPoints[iInput-1], &inputPoints[iInput]);
                for (i = 0; i < protoSize + inputSize - 3; i++)
                {
                    int     diff, score;
                    SDK_ASSERT(iProto < protoSize);
                    SDK_ASSERT(iInput < inputSize);
                    diff = (s16)(protoAngle[iProto] - inputAngle[iInput]);
                    if (diff < 0)
                    {
                        diff = -diff;
                    }
                    score = ((32768 - diff) / 128);
                    if (protoNextRatio <= inputNextRatio)
                    {
                        // Trace one step in stroke of sample
                        // The score reflects the distance to the point of the input stroke that is closer    
                        inputNextRatio -= protoNextRatio;
                        score *= (inputNextRatio < inputRatio[iInput] / 2)      // Remainder is less than half =Next point is closer
                            ? (doubleWidth -
                               CityBlockDistance(&inputPoints[iInput],
                                                 &protoPoints[iProto])) : (doubleWidth -
                                                                           CityBlockDistance
                                                                           (&inputPoints
                                                                            [iInput - 1],
                                                                            &protoPoints[iProto]));
                        strokeScore += protoNextRatio * score;
                        iProto++;
                        protoNextRatio = protoRatio[iProto];
//                        GetMiddlePoint(&protoMidPoint, &protoPoints[iProto-1], &protoPoints[iProto]);
                    }
                    else
                    {
                        // Trace one step in stroke of sample    
                        // The score reflects the distance to the point of the input stroke that is closer    
                        protoNextRatio -= inputNextRatio;
                        score *= (protoNextRatio < protoRatio[iProto] / 2)      // Remainder is less than half =Next point is closer
                            ? (doubleWidth -
                               CityBlockDistance(&inputPoints[iInput],
                                                 &protoPoints[iProto])) : (doubleWidth -
                                                                           CityBlockDistance
                                                                           (&inputPoints[iInput],
                                                                            &protoPoints[iProto -
                                                                                         1]));
                        strokeScore += inputNextRatio * score;
                        iInput++;
                        inputNextRatio = inputRatio[iInput];
//                        GetMiddlePoint(&inputMidPoint, &inputPoints[iInput-1], &inputPoints[iInput]);
                    }
                }

                strokeRatio = protoData->strokeRatios[iStroke];
                if (strokeRatio < inputData->strokeRatios[iStroke])
                {
                    strokeRatio = inputData->strokeRatios[iStroke];
                }

                wholeScore += FX_Mul(strokeScore, strokeRatio);
                wholeWeight += strokeRatio;
            }

            wholeScore = FX_Div(wholeScore, wholeWeight * doubleWidth);
            wholeScore /= 256;

            if (proto->entry->correction != 0)
            {
                wholeScore = FX_Mul(wholeScore, FX32_ONE - proto->entry->correction)
                    + proto->entry->correction;
            }

            if (wholeScore < 0)
                wholeScore = 0;
            if (wholeScore >= FX32_ONE)
                wholeScore = FX32_ONE;

            numCompared++;
            if (resultScores[numRanking - 1] < wholeScore)
            {
                resultScores[numRanking - 1] = wholeScore;
                resultEntries[numRanking - 1] = (PRCPrototypeEntry *)proto->entry;
                for (i = numRanking - 2; i >= 0; i--)
                {
                    if (resultScores[i] < resultScores[i + 1])
                    {
                        fx32    tmpScore;
                        PRCPrototypeEntry *tmpEntry;
                        tmpScore = resultScores[i];
                        resultScores[i] = resultScores[i + 1];
                        resultScores[i + 1] = tmpScore;
                        tmpEntry = resultEntries[i];
                        resultEntries[i] = resultEntries[i + 1];
                        resultEntries[i + 1] = tmpEntry;
                    }
                }
            }
        }
    }
    // Normalize the score

    return numCompared;
}

/*===========================================================================*
  Static Functions
 *===========================================================================*/


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
