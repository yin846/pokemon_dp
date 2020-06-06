/*---------------------------------------------------------------------------*
  Project:  NitroSDK - PRC - 
  File:     prc_algo_common.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: prc_resample.c,v $
  Revision 1.12  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.11  03/04/2005 14:07:53  seiki_masashi
  Changed return value to indicate success/failure of PRC_ResampleStrokes_*

  Revision 1.10  02/28/2005 05:26:29  yosizaki
  do-indent.

  Revision 1.9  08/31/2004 00:47:47  seiki_masashi
  fix a comment.

  Revision 1.8  07/19/2004 06:52:06  seiki_masashi
  Arrangement of identifier names

  Revision 1.7  07/09/2004 00:12:59  seiki_masashi
  PRCi_Resample* -> PRC_Resample*

  Revision 1.6  06/29/2004 07:23:30  seiki_masashi
  Revised PRC_RESAMPLE_METHOD_ANGLE

  Revision 1.5  06/28/2004 11:11:36  seiki_masashi
  Added regularizeSize to prototype patterns list    
  Adjusted parameter types    

  Revision 1.4  06/25/2004 13:32:11  seiki_masashi
  Addition of recognition algorithm "Standard"

  Revision 1.3  06/24/2004 08:08:33  seiki_masashi
  fix typo

  Revision 1.2  06/24/2004 07:03:39  seiki_masashi
  Partitioned and arranged header file

  Revision 1.1  06/23/2004 11:59:30  seiki_masashi
  add 'PRC*' to SDK

  Revision 1.1  06/22/2004 11:19:00  seiki_masashi
  Operable Version (simple only)


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/prc/resample.h>

/*===========================================================================*
  Prototype Declarations
 *===========================================================================*/


/*===========================================================================*
  Variable Definitions
 *===========================================================================*/

/*===========================================================================*
  Functions
 *===========================================================================*/

static BOOL
PRCi_TerminateStrokes(u16 *selectedPoints,
                      int *pSelectedPointNum, int maxPointCount, const PRCStrokes *strokes)
{
    int     selectedPointNum = *pSelectedPointNum;
    const PRCPoint *inputPoints;

    inputPoints = strokes->points;
    if (selectedPointNum < 2)
    {
        // Ignore if length is 0 or 1
        *pSelectedPointNum = 0;
        return FALSE;
    }
    if (!PRC_IsPenUpMarker(&inputPoints[selectedPoints[selectedPointNum - 1]]))
    {
        // Doesn't finish with Pen Up Marker
        if (!PRC_IsPenUpMarker(&inputPoints[selectedPoints[selectedPointNum - 2]]))
        {
            // Next to last is not Pen Up Marker either
            if (selectedPointNum < maxPointCount)
            {
                // If there is room at the end, append Pen Up Marker
                selectedPoints[selectedPointNum] = (u16)-1;
                selectedPointNum++;
            }
            else
            {
                // If no space at the end
                if (selectedPointNum >= 3
                    && !PRC_IsPenUpMarker(&inputPoints[selectedPoints[selectedPointNum - 3]]))
                {
                    // If third from last is not a Pen Up Marker, add Pen Up at the end    
                    selectedPoints[selectedPointNum - 1] = (u16)-1;
                }
                else
                {
                    // Otherwise delete two
                    selectedPointNum -= 2;
                }
            }
        }
        else
        {
            // If the next to last is Pen  Up Marker, always cut the last one    
            selectedPointNum--;
        }
    }

    *pSelectedPointNum = selectedPointNum;
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         PRC_ResampleStrokes_None

  Description:  Does not resample.

  Arguments:    selectedPoints, pSelectedPointNum pointer with result return
                maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count
                strokes             raw input coordinate value before forming
                threshold           resample threshold
                buffer              work area (sizeof(int)*maxPointCount required)

  Returns:      TRUE if resampling succeeded.
 *---------------------------------------------------------------------------*/
BOOL
PRC_ResampleStrokes_None(u16 *selectedPoints,
                         int *pSelectedPointNum,
                         int maxPointCount,
                         int maxStrokeCount, const PRCStrokes *strokes, int threshold, void *buffer)
{
    // Use the raw stroke data without resampling    
    u16     iPoint;
    int     size = strokes->size;

    (void)maxStrokeCount;
    (void)threshold;
    (void)buffer;

    if (size > maxPointCount)
    {
        size = maxPointCount;
    }
    if (size < 2)
    {
        // Ignore if length is 0 or 1
        *pSelectedPointNum = 0;
    }
    else
    {
        // Select point automatically
        for (iPoint = 0; iPoint < size; iPoint++)
        {
            selectedPoints[iPoint] = iPoint;
        }
        *pSelectedPointNum = iPoint;

        if (!PRC_IsPenUpMarker(&strokes->points[size - 1]))
        {
            // Not terminated with Pen Up Marker
            (void)PRCi_TerminateStrokes(selectedPoints, pSelectedPointNum, maxPointCount, strokes);
        }
    }

    return (*pSelectedPointNum > 0);
}

#define PRCi_ABS(x) (((x)>=0)?(x):-(x))

/*---------------------------------------------------------------------------*
  Name:         PRC_ResampleStrokes_Distance

  Description:  Resamples based on city block distance

  Arguments:    selectedPoints, pSelectedPointNum pointer with result return
                maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count
                strokes             raw input coordinate value before forming
                threshold           resample threshold
                buffer              work area (sizeof(int)*maxPointCount required)

  Returns:      TRUE if resampling succeeded.
 *---------------------------------------------------------------------------*/
BOOL
PRC_ResampleStrokes_Distance(u16 *selectedPoints,
                             int *pSelectedPointNum,
                             int maxPointCount,
                             int maxStrokeCount,
                             const PRCStrokes *strokes, int threshold, void *buffer)
{
    int     selectedPointNum;
    int     strokeCount;
    int     iPoint;
    int     size;
    PRCPoint prevPoint;
    PRCPoint *point;
    BOOL    newFlag;
    int     length;

    SDK_ASSERT(maxPointCount > 0);
    SDK_ASSERT(maxStrokeCount > 0);

    (void)buffer;

    selectedPointNum = 0;
    strokeCount = 0;

    size = strokes->size;
    point = strokes->points;

    newFlag = TRUE;
    for (iPoint = 0; iPoint < size && selectedPointNum < maxPointCount; iPoint++, point++)
    {
        if (!PRC_IsPenUpMarker(point))
        {
            if (newFlag)
            {
                // Always select endpoint     
                selectedPoints[selectedPointNum] = (u16)iPoint;
                selectedPointNum++;
                length = 0;
                newFlag = FALSE;
            }
            else
            {
                length += PRCi_ABS(point->x - prevPoint.x) + PRCi_ABS(point->y - prevPoint.y);
                if (length >= threshold)
                {
                    selectedPoints[selectedPointNum] = (u16)iPoint;
                    selectedPointNum++;
                    length = 0;
                }
            }
            prevPoint = *point;
        }
        else
        {
            if (newFlag)
            {
                // Ignore series of Pen Up Markers
                continue;
            }
            else
            {
                if (selectedPoints[selectedPointNum - 1] != iPoint - 1) // Always selectedPointNum>0 when comes to here
                {
                    // End point is always selected
                    selectedPoints[selectedPointNum] = (u16)(iPoint - 1);
                    selectedPointNum++;
                    if (selectedPointNum >= maxPointCount)
                    {
                        break;
                    }
                }
                selectedPoints[selectedPointNum] = (u16)iPoint;
                selectedPointNum++;
                newFlag = TRUE;

                strokeCount++;
                if (strokeCount >= maxStrokeCount)
                {
                    // maxStrokeCount exceeded     
                    iPoint++;          // Process for after the loop At the present time, has no meaning because newFlag == TRUE
                    break;
                }
            }
        }
    }

    *pSelectedPointNum = selectedPointNum;

    if (!newFlag)
    {
        // Not terminated with Pen Up Marker

        // Confirm that end point was selected
        if (selectedPointNum > 0 && selectedPoints[selectedPointNum - 1] != iPoint - 1
            && selectedPointNum < maxPointCount)
        {
            // End point is always selected
            selectedPoints[*pSelectedPointNum] = (u16)(iPoint - 1);
            (*pSelectedPointNum)++;
        }

        (void)PRCi_TerminateStrokes(selectedPoints, pSelectedPointNum, maxPointCount, strokes);
    }

    return (*pSelectedPointNum > 0);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_ResampleStrokes_Angle

  Description:  Resamples based on the angle difference.

  Arguments:    selectedPoints, pSelectedPointNum pointer with result return
                maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count
                strokes             raw input coordinate value before forming
                threshold           resample threshold
                buffer              work area (sizeof(int)*maxPointCount required)

  Returns:      TRUE if resampling succeeded.
 *---------------------------------------------------------------------------*/
BOOL
PRC_ResampleStrokes_Angle(u16 *selectedPoints,
                          int *pSelectedPointNum,
                          int maxPointCount,
                          int maxStrokeCount,
                          const PRCStrokes *strokes, int threshold, void *buffer)
{
#define PRC_RESAMPLE_ANGLE_LENGTH_THRESHOLD 6   // Angle is not valid unless distance is more than 6 city-blocks     
    int     selectedPointNum;
    int     strokeCount;
    int     iPoint;
    int     size;
    PRCPoint *point;
    BOOL    newFlag;
    u16     prevAngle;
    PRCPoint prevPoint;
    BOOL    firstFlag;

    SDK_ASSERT(maxPointCount > 0);
    SDK_ASSERT(maxStrokeCount > 0);

    (void)buffer;

    selectedPointNum = 0;
    strokeCount = 0;

    size = strokes->size;
    point = strokes->points;

    newFlag = TRUE;
    for (iPoint = 0; iPoint < size && selectedPointNum < maxPointCount; iPoint++, point++)
    {
        if (!PRC_IsPenUpMarker(point))
        {
            if (newFlag)
            {
                // Always select endpoint     
                selectedPoints[selectedPointNum] = (u16)iPoint;
                selectedPointNum++;
                prevPoint = *point;
                newFlag = FALSE;
                firstFlag = TRUE;
                if (iPoint + 1 < size)
                {
                    prevAngle =
                        FX_Atan2Idx(((point + 1)->y - point->y) << FX32_SHIFT,
                                    ((point + 1)->x - point->x) << FX32_SHIFT);
                }
            }
            else
            {
                int     length;
                length = PRCi_ABS(point->x - prevPoint.x) + PRCi_ABS(point->y - prevPoint.y);
                if (length >= PRC_RESAMPLE_ANGLE_LENGTH_THRESHOLD)
                {
                    if (firstFlag)
                    {
                        // Look at the angle to the next point until the second point is selected.
                        // Otherwise, the direction of the initial segment will be lost.      
                        if (iPoint + 1 < size && !PRC_IsPenUpMarker(point + 1))
                            // If point+1 is Pen Up Marker, then it is always selected as the endpoint
                            // and thus other conditions are not needed.
                        {
                            u16     currAngle, nextAngle;
                            nextAngle =
                                FX_Atan2Idx(((point + 1)->y - point->y) << FX32_SHIFT,
                                            ((point + 1)->x - point->x) << FX32_SHIFT);
                            if (PRCi_ABS((s16)(prevAngle - nextAngle)) >= threshold)
                            {
                                currAngle =
                                    FX_Atan2Idx((point->y - prevPoint.y) << FX32_SHIFT,
                                                (point->x - prevPoint.x) << FX32_SHIFT);
                                selectedPoints[selectedPointNum] = (u16)iPoint;
                                selectedPointNum++;
                                prevAngle = currAngle;
                            }
                        }
                        firstFlag = FALSE;
                    }
                    else
                    {
                        u16     currAngle;
                        currAngle =
                            FX_Atan2Idx((point->y - prevPoint.y) << FX32_SHIFT,
                                        (point->x - prevPoint.x) << FX32_SHIFT);
                        if (PRCi_ABS((s16)(prevAngle - currAngle)) >= threshold)
                        {
                            selectedPoints[selectedPointNum] = (u16)iPoint;
                            selectedPointNum++;
                            prevAngle = currAngle;
                        }
                    }
                    prevPoint = *point;
                }
            }
        }
        else
        {
            if (newFlag)
            {
                // Ignore series of Pen Up Markers
                continue;
            }
            else
            {
                if (selectedPoints[selectedPointNum - 1] != iPoint - 1) // Always selectedPointNum>0 when comes to here
                {
                    // End point is always selected
                    selectedPoints[selectedPointNum] = (u16)(iPoint - 1);
                    selectedPointNum++;
                    if (selectedPointNum >= maxPointCount)
                    {
                        break;
                    }
                }
                selectedPoints[selectedPointNum] = (u16)iPoint;
                selectedPointNum++;
                newFlag = TRUE;

                strokeCount++;
                if (strokeCount >= maxStrokeCount)
                {
                    // maxStrokeCount exceeded     
                    iPoint++;          // Process for after the loop At the present time, has no meaning because newFlag == TRUE
                    break;
                }
            }
        }
    }

    *pSelectedPointNum = selectedPointNum;

    if (!newFlag)
    {
        // Not terminated with Pen Up Marker

        // Confirm that end point was selected
        if (selectedPointNum > 0 && selectedPoints[selectedPointNum - 1] != iPoint - 1
            && selectedPointNum < maxPointCount)
        {
            // End point is always selected
            selectedPoints[*pSelectedPointNum] = (u16)(iPoint - 1);
            (*pSelectedPointNum)++;
        }

        (void)PRCi_TerminateStrokes(selectedPoints, pSelectedPointNum, maxPointCount, strokes);
    }

    return (*pSelectedPointNum > 0);
}

/*---------------------------------------------------------------------------*
  Name:         PRC_ResampleStrokes_Recursive

  Description:  Resample with recursive method.

  Arguments:    selectedPoints, pSelectedPointNum pointer with result return
                maxPointCount   upper limit of input points (includes pen up marker)
                maxStrokeCount  upper limit of stroke count
                strokes             raw input coordinate value before forming
                threshold           resample threshold
                buffer              work area (sizeof(int)*maxPointCount required)

  Returns:      TRUE if resampling succeeded.
 *---------------------------------------------------------------------------*/
BOOL
PRC_ResampleStrokes_Recursive(u16 *selectedPoints,
                              int *pSelectedPointNum,
                              int maxPointCount,
                              int maxStrokeCount,
                              const PRCStrokes *strokes, int threshold, void *buffer)
{
    u16     beginIndex, endIndex;
    int     stackSize;
    int     stackTop, stackTail;
    int     strokeCount;
    int     selectedPointNum;
    int     size;
    PRCPoint *inputPoints;
    u16    *stackP1;
    u16    *stackP2;
    int     squaredThreshold;

    stackP1 = (u16 *)buffer;
    stackP2 = (u16 *)buffer + maxPointCount;

    squaredThreshold = threshold * threshold;

    beginIndex = 0;
    endIndex = 0;
    strokeCount = 0;
    selectedPointNum = 0;

    inputPoints = strokes->points;
    size = strokes->size;

    while (1)
    {
        if (selectedPointNum + 3 > maxPointCount || strokeCount > maxStrokeCount)
        {
            // No space to store the next stroke
            // To store a single stroke you must have three spaces for
            // starting point, end point and PenUpMarker.
            break;
        }

        // Read and skip over PenUpMarker
        while (endIndex < size && PRC_IsPenUpMarker(&inputPoints[endIndex]))
        {
            endIndex++;
        }

        beginIndex = endIndex;
        if (beginIndex >= size)
        {
            // Quit
            break;
        }

        // Search for next PenUpMarker
        while (endIndex < size && !PRC_IsPenUpMarker(&inputPoints[endIndex]))
        {
            endIndex++;
        }
        if (endIndex < size)
        {
            selectedPoints[selectedPointNum] = endIndex;
            selectedPointNum++;        // Select required PenUpMarker
        }
        else
        {
            selectedPoints[selectedPointNum] = (u16)-1;
            selectedPointNum++;        // -1 especially indicates the final PenUpMarker
        }

        SDK_ASSERT(endIndex > 0);
        selectedPoints[selectedPointNum] = beginIndex;
        selectedPointNum++;
        selectedPoints[selectedPointNum] = (u16)(endIndex - 1);
        selectedPointNum++;

        strokeCount++;                 // The strokeCount should not exceed maxStrokeCount     

        if (selectedPointNum >= maxPointCount)
        {
            // Point count is maxed out     
            break;
        }

        if (endIndex - beginIndex <= 2)
            continue;

        // Using the stack, recursively extract characteristic points
        stackP1[0] = beginIndex;
        stackP2[0] = (u16)(endIndex - 1);
        stackSize = 1;
        stackTop = 0;
        stackTail = 1;
        while (stackSize > 0)
        {
            u16     p1, p2;
            int     x1, y1, x2, y2, xDir, yDir, offs;
            int     lastX, lastY;
            int     i;
            int     maxDist;
            u16     maxP;

            p1 = stackP1[stackTop];
            p2 = stackP2[stackTop];
            stackTop++;
            if (stackTop >= maxPointCount)
            {
                stackTop = 0;
            }
            stackSize--;               // pop

            if (p2 - p1 <= 1)
                continue;

            x1 = inputPoints[p1].x;    // Starting point
            y1 = inputPoints[p1].y;
            x2 = inputPoints[p2].x;    // End point
            y2 = inputPoints[p2].y;
            xDir = x2 - x1;            // Direction vector
            yDir = y2 - y1;
            offs = -(x1 * y2 - x2 * y1);        // To reduce the number of calculations     

            maxDist = -1;
            maxP = (u16)-1;
            lastX = -1;
            lastY = -1;
            for (i = p1 + 1; i < p2; i++)
            {
                int     dist;
                int     x, y;
                x = inputPoints[i].x;
                y = inputPoints[i].y;

                if (lastX == x && lastY == y)
                    continue;
                lastX = x;
                lastY = y;

                // Calculate the distance between the straight line and the point
                // Actually, the original distance is multiplied by the distance between the start and end points    
                dist = x * yDir - y * xDir + offs;
                if (dist < 0)
                {
                    dist = -dist;
                }

                if (maxDist < dist)
                {
                    maxP = (u16)i;
                    maxDist = dist;
                }
            }

            // If the start and end point coordinates are identical, then maxDist == 0,    
            // therefore, it would be desirable to separately obtain the Euclidean distance from (x, y) to the starting point.    
            // However, to avoid adding processing for an unusual case, we always used p1+1 for the case where maxDist==0, maxP==p1+1.     
            // (In that case, note that xDir*xDir+yDir*yDir == 0)     
            if (maxDist * maxDist >= (xDir * xDir + yDir * yDir) * squaredThreshold)
            {
                // Save points outside the threshold as characteristic points
                // Note that maxDist is the original distance multiplied by the distance from the start point to end point.
                selectedPoints[selectedPointNum] = maxP;
                selectedPointNum++;
                stackP1[stackTail] = maxP;
                stackP2[stackTail] = p2;
                stackTail++;
                if (stackTail >= maxPointCount)
                {
                    stackTail = 0;
                }
                stackSize++;           // push
                stackP1[stackTail] = p1;
                stackP2[stackTail] = maxP;
                stackTail++;
                if (stackTail >= maxPointCount)
                {
                    stackTail = 0;
                }
                stackSize++;           // push
                SDK_ASSERT(stackSize <= maxPointCount);
                if (selectedPointNum >= maxPointCount)
                {
                    // Point count is maxed out     
                    break;
                }
            }
        }
    }

    *pSelectedPointNum = selectedPointNum;

//{OSTick start, end; start = OS_GetTick();
    // Sort in ascending order before returning
    // To Do: quick sort  (If possible, find a way to speed this up)     
    {
        int     i, j;
        for (i = 0; i < selectedPointNum - 1; i++)
        {
            for (j = i + 1; j < selectedPointNum; j++)
            {
                if (selectedPoints[i] > selectedPoints[j])
                {
                    u16     tmp;
                    tmp = selectedPoints[i];
                    selectedPoints[i] = selectedPoints[j];
                    selectedPoints[j] = tmp;
                }
            }
        }
    }
//end = OS_GetTick(); OS_Printf("// sort in resample: %lldmicro s selectedPointNum=%d\n", OS_TicksToMicroSeconds(end-start), selectedPointNum); }
    return (*pSelectedPointNum > 0);
}























/*===========================================================================*
  Static Functions
 *===========================================================================*/


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
