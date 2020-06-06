/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - 
  File:     prc_algo_light.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: prc_algo_light.c,v $
  Revision 1.6  01/18/2006 02:12:27  kitase_hirotake
  do-indent

  Revision 1.5  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.4  07/19/2004 06:52:06  seiki_masashi
  Arrangement of identifier names

  Revision 1.3  07/05/2004 02:37:54  seiki_masashi
  Removed prevRatio

  Revision 1.2  06/30/2004 12:53:44  seiki_masashi
  Deleted ASSERT that could be harmful

  Revision 1.1  06/30/2004 08:49:24  seiki_masashi
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
  Operable version (light only)

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
#include <nitro/prc/algo_light.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/

/*===========================================================================*
  Variable Definitions
 *===========================================================================*/

/*===========================================================================*
  Functions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognitionBufferSizeEx_Light

  Description:  Calculates the work area size required for recognition algorithm.
                comparison.

  Arguments:    maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count
                protoDB     sample DB
                param       parameters for recognition processing

  Returns:      required memory capacity for recognition algorithm
 *---------------------------------------------------------------------------*/
u32
PRC_GetRecognitionBufferSizeEx_Light(int maxPointCount,
                                     int maxStrokeCount,
                                     const PRCPrototypeDB_Light *protoDB,
                                     const PRCRecognizeParam_Light *param)
{
    (void)maxPointCount;
    (void)maxStrokeCount;
    (void)protoDB;
    (void)param;

    return 1;                          // Because OS_Alloc(0) causes an error
}

/*---------------------------------------------------------------------------*
  Name:         PRC_GetRecognizedEntriesEx_Light

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
PRC_GetRecognizedEntriesEx_Light(PRCPrototypeEntry **resultEntries,
                                 fx32 *resultScores,
                                 int numRanking,
                                 void *buffer,
                                 const PRCInputPattern_Light *input,
                                 const PRCPrototypeDB_Light *protoDB,
                                 u32 kindMask, const PRCRecognizeParam_Light *param)
{
    int     i;
    const PRCiPatternData_Common *inputData;
    int     numCompared;

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
        resultScores[i] = FX32_ONE * 32768;
    }

    inputData = &input->data;
    numCompared = 0;

    {
        const PRCiPrototypeEntry_Common *proto;
        int     iPattern;
        fx32    border;

        border = FX32_ONE;             // Cutoff level: If this value is set to FX32_ONE/2,  then those with a score below 0.5 will not be evaluated at all.    
        border *= 32768;               // Internal score factor    

        proto = protoDB->patterns;

        for (iPattern = 0; iPattern < protoDB->patternCount; iPattern++, proto++)
        {
            const PRCiPatternData_Common *protoData;
            int     iStroke;
            fx32    wholeScore;
            fx32    patternBorder;

            if (!proto->entry->enabled || !(proto->entry->kind & kindMask))
                continue;

            protoData = &proto->data;

            if (inputData->strokeCount != protoData->strokeCount)
                continue;

            wholeScore = 0;

            // Set the cutoff point for reversed correction     
            patternBorder = border;
            if (proto->entry->correction != 0)
            {
                // Approximate patternBorder *= 1 / (FX32_ONE - correction) by setting a larger cutoff     
                // with a straight line    
                if (proto->entry->correction < -FX32_ONE)
                {
                    patternBorder >>= 1;        // Less than 1/2 is a match     
                }
                else if (proto->entry->correction < 0)
                {
                    // -1 < correction < 0  is approximated by correction/2+1
                    patternBorder = FX_Mul(patternBorder, FX32_ONE + proto->entry->correction / 2);
                }
                else if (proto->entry->correction < FX32_ONE / 2)
                {
                    // 0 < correction < 1/2 is approximated by correction*2+1
                    patternBorder = FX_Mul(patternBorder, FX32_ONE + proto->entry->correction * 2);
                }
                else
                {
                    patternBorder = FX32_ONE * 32768;
                }                      // No elimination    
            }

            for (iStroke = 0; iStroke < inputData->strokeCount; iStroke++)
            {
                int     iProto, iInput;
                int     protoStrokeIndex, inputStrokeIndex;
                int     protoSize, inputSize;
                const u16 *protoAngle;
                const u16 *inputAngle;
                const fx16 *protoRatio;
                const fx16 *inputRatio;
                fx16    protoNextRatio, inputNextRatio;
                fx32    score;
                fx32    localBorder;
                fx16    strokeRatio;
                int     loopEnd;

                score = 0;

                protoStrokeIndex = protoData->strokes[iStroke];
                inputStrokeIndex = inputData->strokes[iStroke];
                protoSize = protoData->strokeSizes[iStroke];
                inputSize = inputData->strokeSizes[iStroke];
                protoAngle = &protoData->lineSegmentAngleArray[protoStrokeIndex];
                inputAngle = &inputData->lineSegmentAngleArray[inputStrokeIndex];
                protoRatio = &protoData->lineSegmentRatioToStrokeArray[protoStrokeIndex];
                inputRatio = &inputData->lineSegmentRatioToStrokeArray[inputStrokeIndex];

                strokeRatio = protoData->strokeRatios[iStroke]; // If here do inputData->, then the relative weight of the input image takes on great importance

                // Set the elimination level    
                if (strokeRatio == FX32_ONE)
                {
                    localBorder = patternBorder;
                }
                else if (strokeRatio >= FX32_ONE / 2)
                {
                    localBorder = patternBorder * 2;    // No problem with taking excess
                }
                else if (strokeRatio >= FX32_ONE / 3)
                {
                    localBorder = patternBorder * 3;    // No problem with taking excess
                }
                else
                {
                    localBorder = FX32_ONE * 32768;     // No elimination    
                }

                SDK_ASSERT(protoSize >= 2);
                SDK_ASSERT(inputSize >= 2);

                iProto = iInput = 1;
                protoNextRatio = protoRatio[iProto];
                inputNextRatio = inputRatio[iInput];
                loopEnd = protoSize + inputSize - 3;
                for (i = 0; i < loopEnd; i++)
                {
                    int     diff;
                    SDK_ASSERT(iProto < protoSize);
                    SDK_ASSERT(iInput < inputSize);
                    diff = (s16)(protoAngle[iProto] - inputAngle[iInput]);
                    if (diff < 0)
                    {
                        diff = -diff;
                    }
                    if (protoNextRatio <= inputNextRatio)
                    {
                        score += protoNextRatio * diff;
                        iProto++;
                        inputNextRatio -= protoNextRatio;
                        protoNextRatio = protoRatio[iProto];
                    }
                    else
                    {
                        score += inputNextRatio * diff;
                        iInput++;
                        protoNextRatio -= inputNextRatio;
                        inputNextRatio = inputRatio[iInput];
                    }
                    // Check the cutoff    
                    if (score > localBorder)
                    {
                        // At this point, no hope of even a poor match    
                        wholeScore = FX32_ONE * 32768;
                        goto quit_compare;
                    }
                }

                wholeScore += FX_Mul(score, strokeRatio);
            }

            if (proto->entry->correction != 0)
            {
                wholeScore = FX_Mul(wholeScore, FX32_ONE - proto->entry->correction);
            }

//                wholeScore = FX_Mul(wholeScore, FX32_ONE - proto->entry->correction)
//                            + proto->entry->correction;

            // Note that low scores are better at this stage     
          quit_compare:
            numCompared++;
            if (resultScores[numRanking - 1] > wholeScore)
            {
                resultScores[numRanking - 1] = wholeScore;
                resultEntries[numRanking - 1] = (PRCPrototypeEntry *)proto->entry;
                for (i = numRanking - 2; i >= 0; i--)
                {
                    if (resultScores[i] > resultScores[i + 1])
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
                // Set the cutoff score     
                border = resultScores[numRanking - 1];
            }
        }
    }
    // Normalize the score
    {
        int     iRank;
        fx32    score;
        for (iRank = 0; iRank < numRanking; iRank++)
        {
            if (resultEntries[iRank] == NULL)
                break;

            score = resultScores[iRank];
            score = FX32_ONE - (score / 32768);
            if (score < 0)
                score = 0;
            if (score >= FX32_ONE)
                score = FX32_ONE;

            resultScores[iRank] = score;
        }
    }

    return numCompared;
}

/*===========================================================================*
  Static Functions
 *===========================================================================*/


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
