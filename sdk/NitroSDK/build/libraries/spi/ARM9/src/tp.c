/*---------------------------------------------------------------------------*
  Project:  NitroSDK - libraries - tp
  File:     tp.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: tp.c,v $
  Revision 1.36  01/18/2006 02:12:27  kitase_hirotake
  do-indent

  Revision 1.35  04/14/2005 05:07:42  takano_makoto
  Corrected so the coordinate value 0 is returned when calibration parameters are illegal.

  Revision 1.34  04/14/2005 04:05:10  takano_makoto
  Corrected so TP_GetUserInfo usually returns TRUE

  Revision 1.33  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.32  2005/02/18 08:54:02  yasu
  Changed year of creation.

  Revision 1.31  2005/02/18 07:29:41  yasu
  Signed/Unsigned conversion warning control

  Revision 1.30  12/29/2004 06:08:57  takano_makoto
  Added measures for skipping coordinates to TP_GetRatestRawPointInAuto function

  Revision 1.29  12/29/2004 04:00:26  takano_makoto
  Added retry parameter to the arguments of TP_RequestSetStability function to maintain compatibility with older versions (not used internally)

  Revision 1.28  12/29/2004 02:04:34  takano_makoto
  Removed retry parameter in SetStability function

  Revision 1.27  12/14/2004 00:20:29  takano_makoto
  Made correction to save the tpState.index value in TP_GetLatestRawPointInAuto in advance

  Revision 1.26  12/13/2004 09:01:33  takano_makoto
  In case the application does not look at validity, set the coordinate value even when it is invalid with TP_GetLatestRawPointInAuto function.

  Revision 1.25  12/08/2004 06:16:37  takano_makoto
  Made correction to return a value with the invalid flag on when only the invalid coordinate value was found with TP_GetLatestRawPointInAuto.

  Revision 1.24  11/02/2004 10:26:33  takano_makoto
  fix typo.

  Revision 1.23  11/02/2004 10:19:34  takano_makoto
  fix typo.

  Revision 1.22  10/20/2004 06:34:45  terui
  Changed definition name of LCD line number.

  Revision 1.21  09/27/2004 10:06:37  takano_makoto
  Merged with branch.

  Revision 1.20.2.1  2004/09/27 10:02:33  takano_makoto
  Fixed the bug when error termination occurs while interrupts are prohibited with TP_CalcCalivrateParam.

  Revision 1.20  2004/09/13 05:49:56  yasu
  merged with NitroSDK_2_00pr6_branch

  Revision 1.19.2.1  2004/09/13 05:43:02  yasu
  Support SDK_NVRAM_FORMAT

  Revision 1.19  09/03/2004 04:29:15  terui
  Revised according to structure definition adjustments relating to user information in NVRAM.

  Revision 1.18  08/25/2004 09:29:17  terui
  Changed so that the readout of calibration data is separated by TS_VERSION.

  Revision 1.17  2004/07/27 07:31:10  yasu
  Add #pragma  warn_padding off

  Revision 1.16  2004/06/10 06:52:15  yasu
  add comment @ TP_WaitBusy

  Revision 1.15  2004/06/10 05:27:03  yasu
  Add TP_GetCalibratedResult

  Revision 1.14  06/10/2004 05:00:47  takano_makoto
  Fix ASSERT in TP_WaitBusy and TP_WaitAllBusy

  Revision 1.13  2004/06/10 04:41:37  yasu
  fix assert condition in TP_RequestAutoSamplingStartAsync

  Revision 1.12  06/09/2004 04:16:00  takano_makoto
  Fix so conversion is executed for invalid values also with TP_GetCalibratePoint()

  Revision 1.11  06/02/2004 06:14:37  takano_makoto
  Bug fix, change to TP_RequestAutoSamplingStartAsync() and TP_SetCallback() arguments

  Revision 1.10  05/31/2004 07:54:35  takano_makoto
  Fix of bug when interrupt occurring right after PXI sent

  Revision 1.9  05/21/2004 10:01:39  takano_makoto
  Added TP_GetUserInfo()

  Revision 1.8  05/18/2004 01:38:48  terui
  Support for PXI synchronization mechanism with ARM7

  Revision 1.7  05/14/2004 00:42:18  takano_makoto
  Modify Calibrate Parameter format.

  Revision 1.6  04/21/2004 12:13:38  takano_makoto
  Add Async function.

  Revision 1.5  04/19/2004 12:00:01  takano_makoto
  modify any functions interface.

  Revision 1.4  04/16/2004 02:13:37  takano_makoto
  Modify for STRB warning.

  Revision 1.3  04/15/2004 13:55:18  takano_makoto
  Chang any function name.

  Revision 1.2  04/15/2004 11:43:17  takano_makoto
  Change header file path.

  Revision 1.1  04/15/2004 10:56:17  takano_makoto
  Initial Update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitro/spi/ARM9/tp.h>
#include <nitro/spi/common/config.h>
#include "spi.h"

#define TP_RAW_MAX  0x1000
#define TP_CALIBRATE_DOT_INV_SCALE_SHIFT    (28 - TP_CALIBRATE_DOT_SCALE_SHIFT)
#define TP_CALIBRATE_DOT2ORIGIN_SCALE_SHIFT (TP_CALIBRATE_DOT_SCALE_SHIFT - TP_CALIBRATE_ORIGIN_SCALE_SHIFT)

/*===========================================================================*
    Static function prototype definition
 *===========================================================================*/

static void TPi_TpCallback(PXIFifoTag tag, u32 data, BOOL err);

/*---------------------------------------------------------------------------*
    Static variables definition
 *---------------------------------------------------------------------------*/
typedef struct
{
    s32     x0;                        // X coordinate intercept
    s32     xDotSize;                  // X dot width
    s32     xDotSizeInv;               // X denominator

    s32     y0;                        // Y coordinate intercept
    s32     yDotSize;                  // Y dot width
    s32     yDotSizeInv;               // Y denominator

}
TPiCalibrateParam;

#pragma  warn_padding off
static struct
{
    TPRecvCallback callback;           // User callback function called when TP value obtained
    TPData  buf;                       // TP value receive buffer when TP value obtained once
    u16     index;                     // Latest buffer index during auto-sampling
    u16     frequence;                 // Number of times for 1 frame sampling during auto-sampling    
    TPData *samplingBufs;              // TP value buffer pointer during auto-sampling
    u16     bufSize;                   // TP buffer address during auto-sampling
    // PADDING 2 BYTE
    TPiCalibrateParam calibrate;       // calibration parameters
    u16     calibrate_flg;             // calibration flag

    vu16    state;                     // Touch panel status
    vu16    err_flg;                   // error flag
    vu16    command_flg;               // request command executing flag
}
tpState;
#pragma  warn_padding reset


/*---------------------------------------------------------------------------*
    Inline sub-routine definition
    
    The response from ARM7 for these instructions are returned via the PXI library.
    To obtain the response from ARM7, specify the callback of the tag
    "PXI_FIFO_TAG_TOUCHPANEL" in PXI.
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         TPi_SamplingNow

  Description:  Sample touch panel once.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL TPi_SamplingNow(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT
                               | SPI_PXI_END_BIT
                               | (0 << SPI_PXI_INDEX_SHIFT)
                               | (SPI_PXI_COMMAND_TP_SAMPLING << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         TPi_AutoSamplingOn

  Description:  Start auto-sampling of touch panel.

  Arguments:    vCount -    VCount that carries out sampling.
                            If sampling is done multiple times in 1 frame the
                            frame is time divided using this as the origin.
                frequency - Frequency of sampling for 1 frame

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL TPi_AutoSamplingOn(u16 vCount, u8 frequency)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT
                               | (0 << SPI_PXI_INDEX_SHIFT)
                               | (SPI_PXI_COMMAND_TP_AUTO_ON << 8) | (u32)frequency, 0))
    {
        return FALSE;
    }

    // Send packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_END_BIT | (1 << SPI_PXI_INDEX_SHIFT) | (u32)vCount, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         TPi_AutoSamplingOff

  Description:  Stop auto-sampling of touch panel.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL TPi_AutoSamplingOff(void)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT
                               | SPI_PXI_END_BIT
                               | (0 << SPI_PXI_INDEX_SHIFT)
                               | (SPI_PXI_COMMAND_TP_AUTO_OFF << 8), 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         TPi_SetupStability

  Description:  Setup stability determination parameters for sampling

  Arguments:    range - For continuous sampling, error for which detected voltage is considered stable.    
                        The detection value is 12 bits, 0 - 4095.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static inline BOOL TPi_SetupStability(u16 range)
{
    // Send packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_TOUCHPANEL,
                               SPI_PXI_START_BIT
                               | SPI_PXI_END_BIT
                               | (0 << SPI_PXI_INDEX_SHIFT)
                               | (SPI_PXI_COMMAND_TP_SETUP_STABILITY << 8) | (u32)range, 0))
    {
        return FALSE;
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         TPi_CopyTpFromSystemWork

  Description:  Read touch panel value data written from ARM7 to shared memory
                to separate area.

  Returns:      result - Store latest touch panel value read
 *---------------------------------------------------------------------------*/
static inline void TPi_CopyTpFromSystemWork(TPData *result)
{
    SPITpData spi_tp;

    spi_tp.halfs[0] = *(u16 *)(&(OS_GetSystemWork()->touch_panel[0]));
    spi_tp.halfs[1] = *(u16 *)(&(OS_GetSystemWork()->touch_panel[2]));

    // Read from system area (2 byte access)
    result->x = (u16)spi_tp.e.x;
    result->y = (u16)spi_tp.e.y;
    result->touch = (u8)spi_tp.e.touch;
    result->validity = (u8)spi_tp.e.validity;
}


/*---------------------------------------------------------------------------*
  Name:         TPi_ErrorAtPxi

  Description:  Processing for error with PXI communication with ARM7.    
                If callback has been set, TP_RESULT_PXI_BUSY callback
                is returned.
                
  Arguments:    command - request type
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void TPi_ErrorAtPxi(TPRequestCommand command)
{

    tpState.err_flg |= (1 << command);

    if (tpState.callback)
    {
        (tpState.callback) (command, TP_RESULT_PXI_BUSY, 0);
    }
}


/*===========================================================================*
    Function definition
 *===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         TPi_TpCallback

  Description:  Function called when PXI receives messages about touch panel
                from ARM7.
                Temporarily store touch panel value from ARM7, and call user callback if     
                callback function is setup.    
                

  Arguments:    tag -  tag so PXI can distinguish type
                data - message data from ARM7
                err -  PXI transfer error flag
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void TPi_TpCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(tag)

    u16     result;
    u16     command;

    result = (u16)(data & SPI_PXI_DATA_MASK);
    command = (u16)((result & 0x7f00) >> 8);

    // PXI transfer error
    if (err)
    {
        TPi_ErrorAtPxi((TPRequestCommand)command);
        return;
    }

    if (command == SPI_PXI_COMMAND_TP_AUTO_SAMPLING)
    {
        // auto-sampling execution completed notification
//        tpState.index = (u16) (result & 0x00FF);
        tpState.index++;
        if (tpState.index >= tpState.bufSize)
        {
            tpState.index = 0;
        }

        // Temporarily store touch panel value from shared area    
        TPi_CopyTpFromSystemWork(&tpState.samplingBufs[tpState.index]);

        if (tpState.callback)
        {
            (tpState.callback) ((TPRequestCommand)command, TP_RESULT_SUCCESS, (u8)tpState.index);
        }
        return;
    }

    if (!(data & SPI_PXI_END_BIT))
    {
        return;
    }

    SDK_ASSERT(result & 0x8000);

    switch ((u8)(result & 0x00ff))
    {
    case SPI_PXI_RESULT_SUCCESS:
        // Processing when success
        switch (command)
        {
        case SPI_PXI_COMMAND_TP_SAMPLING:
            // Obtain sampling result
            TPi_CopyTpFromSystemWork(&tpState.buf);
            tpState.state = TP_STATE_READY;
            break;

        case SPI_PXI_COMMAND_TP_AUTO_ON:
            tpState.state = TP_STATE_AUTO_SAMPLING;
            break;

        case SPI_PXI_COMMAND_TP_AUTO_OFF:
            tpState.state = TP_STATE_READY;
            break;
        }

        // turn off busy flag
        tpState.command_flg &= ~(1 << command);

        // Callback call
        if (tpState.callback)
        {
            (tpState.callback) ((TPRequestCommand)command, TP_RESULT_SUCCESS, 0);
        }
        break;

    case SPI_PXI_RESULT_EXCLUSIVE:
        result = TP_RESULT_EXCLUSIVE;
        goto common;

    case SPI_PXI_RESULT_INVALID_PARAMETER:
        result = TP_RESULT_INVALID_PARAMETER;
        goto common;

    case SPI_PXI_RESULT_ILLEGAL_STATUS:
        result = TP_RESULT_ILLEGAL_STATUS;

      common:
        // Error processing
        // Turn on error flag
        tpState.err_flg |= (1 << command);
        tpState.command_flg &= ~(1 << command);

        if (tpState.callback)
        {
            (tpState.callback) ((TPRequestCommand)command, (TPRequestResult)(result & 0xff), 0);
        }
        break;

    case SPI_PXI_RESULT_INVALID_COMMAND:
    default:
        // abnormal end
//        OS_Printf("result=%x\n",result);
        OS_Panic("Get illegal TP command from ARM7!\n");
        return;
    }
}




/*---------------------------------------------------------------------------*
  Name:         TP_Init

  Description:  Touch panel library initialization

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_Init(void)
{
    static u16 initial = FALSE;

    if (initial)
    {
        return;
    }
    initial = TRUE;

    //****************************************************************
    // Initialize TP.
    //****************************************************************
    PXI_Init();

    tpState.index = 0;
    tpState.callback = NULL;
    tpState.samplingBufs = NULL;
    tpState.state = TP_STATE_READY;
    tpState.calibrate_flg = FALSE;
    tpState.command_flg = 0;
    tpState.err_flg = 0;

    // 2003/05/18 Add by terui.
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_TOUCHPANEL, PXI_PROC_ARM7))
    {
    }

    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_TOUCHPANEL, TPi_TpCallback);
}


/*---------------------------------------------------------------------------*
  Name:         TP_GetUserInfo

  Description:  Function for obtaining parameters for calibration from hardware     
                flash memory.    

  Returns:      param - pointer to area for obtaining parameters
                BOOL  - If succeed in obtaining value, TRUE.
                        If valid value is not found, return FALSE.
 *---------------------------------------------------------------------------*/
BOOL TP_GetUserInfo(TPCalibrateParam *calibrate)
{
    NVRAMConfig *info = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);
//    s16 x0, y0, xdot, ydot;
    u16     x1, y1, x2, y2, dx1, dy1, dx2, dy2;

    SDK_NULL_ASSERT(calibrate);

#if ( defined( SDK_TS ) && ( SDK_TS_VERSION >= 200 || SDK_NVRAM_FORMAT >= 100 ) )
    x1 = info->ncd.tp.raw_x1;
    y1 = info->ncd.tp.raw_y1;
    dx1 = (u16)(info->ncd.tp.dx1);
    dy1 = (u16)(info->ncd.tp.dy1);
    x2 = info->ncd.tp.raw_x2;
    y2 = info->ncd.tp.raw_y2;
    dx2 = (u16)(info->ncd.tp.dx2);
    dy2 = (u16)(info->ncd.tp.dy2);
#else
    x1 = *(u16 *)(&(info->ncd.tp.calib_data[0]));
    y1 = *(u16 *)(&(info->ncd.tp.calib_data[1]));
    dy1 = (u16)(*(u16 *)(&(info->ncd.tp.calib_data[2])) >> 8);
    dx1 = MI_ReadByte((u8 *)(&(info->ncd.tp.calib_data[2])));
    x2 = *(u16 *)(&(info->ncd.tp.calib_data[3]));
    y2 = *(u16 *)(&(info->ncd.tp.calib_data[4]));
    dy2 = (u16)(*(u16 *)(&(info->ncd.tp.calib_data[5])) >> 8);
    dx2 = MI_ReadByte((u8 *)(&(info->ncd.tp.calib_data[5])));
#endif

    /* For now, since there is no enable flag for the calibration value's enable flag*/
    if ((x1 == 0 && x2 == 0 && y1 == 0 && y2 == 0) ||
        (TP_CalcCalibrateParam(calibrate, x1, y1, dx1, dy1, x2, y2, dx2, dy2) != 0))
    {
        // Data CRC check is not preformed in the IPL, so when there is an unusual value, all parameters
        // are cleared to 0 and TRUE is returned. 
        // In this case, the TP value is usually (0,0) after calibration.
        calibrate->x0 = 0;
        calibrate->y0 = 0;
        calibrate->xDotSize = 0;
        calibrate->yDotSize = 0;
        return TRUE;
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         TP_SetCalibrateParam

  Description:  Set parameters for calibration.
                If the parameters are set up with this function, calibration is
                carried out from that point on using this value with
                TP_WaitCalibratedResult(), TP_GetLatestCalibratedPointInAuto(),
                TP_GetCalibratedPoint(), and TP_GetUnCalibratedPoint(). The  reciprocal of the dot size is calculated in advance at this point.

  Arguments:    param - Pointer to calibration parameters set up.
                        If NULL is set up, coordinate conversion due to calibration
                        is not carried out from that point on. (default value: NULL)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_SetCalibrateParam(const TPCalibrateParam *param)
{
    OSIntrMode enabled;

    if (param == NULL)
    {
        tpState.calibrate_flg = FALSE;
        return;
    }

    // Reciprocal of xDotSize and yDotSIze is calculated in advance
    enabled = OS_DisableInterrupts();

    if (param->xDotSize != 0)
    {
        CP_SetDiv32_32(0x10000000, (u32)param->xDotSize);

        tpState.calibrate.x0 = param->x0;
        tpState.calibrate.xDotSize = param->xDotSize;
        tpState.calibrate.xDotSizeInv = (s32)CP_GetDivResult32();
    }
    else
    {
        tpState.calibrate.x0 = 0;
        tpState.calibrate.xDotSize = 0;
        tpState.calibrate.xDotSizeInv = 0;
    }

    if (param->yDotSize != 0)
    {
        CP_SetDiv32_32(0x10000000, (u32)param->yDotSize);

        tpState.calibrate.y0 = param->y0;
        tpState.calibrate.yDotSize = param->yDotSize;
        tpState.calibrate.yDotSizeInv = (s32)CP_GetDivResult32();
    }
    else
    {
        tpState.calibrate.y0 = 0;
        tpState.calibrate.yDotSize = 0;
        tpState.calibrate.yDotSizeInv = 0;
    }

    (void)OS_RestoreInterrupts(enabled);

    tpState.calibrate_flg = TRUE;

}


/*---------------------------------------------------------------------------*
  Name:         TP_SetCallback

  Description:  Set up user callback function that is called when processing
                result from touch panel returns.

  Arguments:    callback - user callback function pointer
                           If NULL callback is not called (default:NULL)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_SetCallback(TPRecvCallback callback)
{
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();
    tpState.callback = callback;
    (void)OS_RestoreInterrupts(enabled);
}



/*---------------------------------------------------------------------------*
  Name:         TP_RequestSamplingAsync

  Description:  Request touch panel value for ARM7.
                After calling this function, the touch panel value can be read
                with TP_WaitRawResult() or TP_WaitCalibratedResult().
                Cannot be used while auto sampling.
                
  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_RequestSamplingAsync(void)
{
    OSIntrMode enabled;

    SDK_ASSERT(!(tpState.command_flg & TP_REQUEST_COMMAND_FLAG_SAMPLING));

    enabled = OS_DisableInterrupts();

    if (TPi_SamplingNow() == FALSE)
    {
        // PXI send failure
        (void)OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_SAMPLING);
        return;
    }
    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_SAMPLING;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_SAMPLING;

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         TP_WaitRawResult

  Description:  Wait until touch panel values from ARM7 are set and read values
                values that were not calibrated.
                Chattering measures have been taken for the data that is acquired here.

  Arguments:    None.

  Returns:      result - Pointer to a variable that is used to acquire touch panel values.
                         Takes uncalibrated values (0 - 4095) as x,y coordinates.
                u32    - If succeed in obtaining data return TRUE.   
                         If fail return non-zero.    
 *---------------------------------------------------------------------------*/
u32 TP_WaitRawResult(TPData *result)
{
    SDK_NULL_ASSERT(result);

    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_SAMPLING);

    if (tpState.err_flg & TP_REQUEST_COMMAND_FLAG_SAMPLING)
    {
        return 1;
    }

    *result = tpState.buf;
    return 0;
}



/*---------------------------------------------------------------------------*
  Name:         TP_GetCalibratedResult

  Description:  Views touch panel values from ARM7 as set and reads values
                corresponding to calibrated screen coordinates.
                Chattering measures have been taken for the data that is acquired here.

  Arguments:    result - Pointer to a variable that is used to acquire touch panel values.
                         Takes values that have been mapped to screen coordinates as x, y values.    
                         If no calibration parameter has been set, touch panel values will be
                           
                         
  Returns:      u32    - If succeed in obtaining data return TRUE.   
                         If fail return non-zero.    
 *---------------------------------------------------------------------------*/
u32 TP_GetCalibratedResult(TPData *result)
{
    SDK_NULL_ASSERT(result);

    if (tpState.err_flg & TP_REQUEST_COMMAND_FLAG_SAMPLING)
    {
        return 1;
    }

    *result = tpState.buf;
    TP_GetCalibratedPoint(result, result);
    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         TP_WaitCalibratedResult

  Description:  Wait until touch panel values from ARM7 are set and read values
                corresponding to calibrated screen coordinates.
                Chattering measures have been taken for the data that is acquired here.

  Arguments:    result - Pointer to a variable that is used to acquire touch panel values.
                         Takes values that have been mapped to screen coordinates as x, y values.    
                         If no calibration parameter has been set, touch panel values will be
                           
                         
  Returns:      u32    - If succeed in obtaining data return TRUE.   
                         If fail return non-zero.    
 *---------------------------------------------------------------------------*/
u32 TP_WaitCalibratedResult(TPData *result)
{
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_SAMPLING);
    return TP_GetCalibratedResult(result);
}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestAutoSamplingStartAsync

  Description:  Sends a request to ARM7 to start auto sampling touch panel values.
                In one frame, data is sampled "frequence" times at equal intervals.
                results are stored to array specified by samplingBufs.

  Arguments:    vcount       - Sets the VCOUNT value, which is the base for auto sampling.
                frequence    - Setting for the number of times sampling will be done in one frame.
                samplingBufs - Sets the area where auto sampling data will be stored.
                bufSize      - Sets buffer size.
                               samplingBufs array is used as busSize ring buffer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_RequestAutoSamplingStartAsync(u16 vcount, u16 frequence, TPData samplingBufs[], u16 bufSize)
{
    u32     i;
    OSIntrMode enabled;

    SDK_NULL_ASSERT(samplingBufs);
    SDK_ASSERT(vcount < HW_LCD_LINES);
    SDK_ASSERT(frequence != 0 && frequence <= SPI_TP_SAMPLING_FREQUENCY_MAX);
    SDK_ASSERT(tpState.state == TP_STATE_READY);
    SDK_ASSERT(!(tpState.command_flg & TP_REQUEST_COMMAND_FLAG_AUTO_ON));
    SDK_ASSERT(bufSize != 0);

    tpState.samplingBufs = samplingBufs;
    tpState.index = 0;
    tpState.frequence = frequence;
    tpState.bufSize = bufSize;

    for (i = 0; i < bufSize; i++)
    {
        tpState.samplingBufs[i].touch = TP_TOUCH_OFF;
    }

    enabled = OS_DisableInterrupts();

    if ((u8)TPi_AutoSamplingOn(vcount, (u8)frequence) == FALSE)
    {
        // PXI send failure
        (void)OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_AUTO_ON);
        return;
    }
    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_AUTO_ON;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_AUTO_ON;

    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         TP_RequestAutoSamplingStopAsync

  Description:  Sends a request to ARM7 to stop auto sampling touch panel values.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_RequestAutoSamplingStopAsync(void)
{
    OSIntrMode enabled;

    SDK_ASSERT(tpState.state == TP_STATE_AUTO_SAMPLING);
    SDK_ASSERT(!(tpState.command_flg & TP_REQUEST_COMMAND_FLAG_AUTO_OFF));

    enabled = OS_DisableInterrupts();

    if (TPi_AutoSamplingOff() == FALSE)
    {
        // PXI send failure
        (void)OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_AUTO_OFF);
        return;
    }

    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_AUTO_OFF;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_AUTO_OFF;

    (void)OS_RestoreInterrupts(enabled);

}


/*---------------------------------------------------------------------------*
  Name:         TP_RequestSetStabilityAsync

  Description:  Sets touch panel chattering parameters.
                Sets the number of times to retry sampling until values stabilize
                and a range for determining whether values have stabilized.

  Arguments:    retry -  This argument is not used internally.
                range -  Range for determining whether the values have stabilized.
                         It becomes invalid if the coordinate value was not in this range.
                         (Range: 0 to 4095, Default value:

  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_RequestSetStabilityAsync(u8 retry, u16 range)
{
#pragma unused( retry )
    OSIntrMode enabled;

    SDK_ASSERT(range != 0);
    SDK_ASSERT(range < 255);

    enabled = OS_DisableInterrupts();

    if (TPi_SetupStability(range) == FALSE)
    {
        // PXI send failure
        (void)OS_RestoreInterrupts(enabled);
        TPi_ErrorAtPxi(TP_REQUEST_COMMAND_SET_STABILITY);
        return;
    }
    tpState.command_flg |= TP_REQUEST_COMMAND_FLAG_SET_STABILITY;
    tpState.err_flg &= ~TP_REQUEST_COMMAND_FLAG_SET_STABILITY;

    (void)OS_RestoreInterrupts(enabled);
}



/*---------------------------------------------------------------------------*
  Name:         TP_GetLatestRawPointInAuto

  Description:  Obtain latest valid touch panel values from auto-sampling results.
                The x,y coordinates of the values obtain the range not  calibrated (0 - 4095).    

  Arguments:    None.

  Returns:      result - Pointer for obtaining latest valid touch panel values.
 *---------------------------------------------------------------------------*/
void TP_GetLatestRawPointInAuto(TPData *result)
{
    s32     i, curr_index;
    TPData *tp;

    SDK_NULL_ASSERT(result);
    SDK_NULL_ASSERT(tpState.samplingBufs);

    result->validity = TP_VALIDITY_INVALID_XY;

    curr_index = tpState.index;

    // If sampling buffer size is 1, return current coordinate values
    if (tpState.frequence == 1 || tpState.bufSize == 1)
    {
        *result = tpState.samplingBufs[curr_index];
        return;
    }

    // Search for latest valid sampling values
    for (i = 0; i < tpState.frequence && i < tpState.bufSize - 1; i++)
    {
        s32     index;

        index = curr_index - i;
        if (index < 0)
        {
            index += tpState.bufSize;
        }

        tp = &tpState.samplingBufs[index];

        if (!tp->touch)
        {
            *result = *tp;
            return;
        }

        // If invalid data is present refer to the previous sampling value
        if (result->validity & TP_VALIDITY_INVALID_X)
        {
            /* x-coordinate*/
            if (!(tp->validity & TP_VALIDITY_INVALID_X))
            {
                result->x = tp->x;
                if (i != 0)
                {
                    result->validity &= ~TP_VALIDITY_INVALID_X;
                }
            }
        }

        if (result->validity & TP_VALIDITY_INVALID_Y)
        {
            /* y-coordinate*/
            if (!(tp->validity & TP_VALIDITY_INVALID_Y))
            {
                result->y = tp->y;
                if (i != 0)
                {
                    result->validity &= ~TP_VALIDITY_INVALID_Y;
                }
            }
        }

        if (result->validity == TP_VALIDITY_VALID)
        {
            result->touch = TP_TOUCH_ON;
            return;
        }
    }

    // Returns a value that could be obtained if an invalid value remained.
    result->touch = TP_TOUCH_ON;
    return;
}



/*---------------------------------------------------------------------------*
  Name:         TP_GetLatestCalibratedPointInAuto

  Description:  Obtain latest valid touch panel values from auto-sampling results.
                The x,y coordinates of the values obtain the range of the screen coordinates.      

  Arguments:    None.

  Returns:      result - Pointer for obtaining latest valid touch panel values.
 *---------------------------------------------------------------------------*/
void TP_GetLatestCalibratedPointInAuto(TPData *result)
{
    SDK_NULL_ASSERT(result);

    TP_GetLatestRawPointInAuto(result);

    TP_GetCalibratedPoint(result, result);

}


/*---------------------------------------------------------------------------*
  Name:         TP_GetLatestIndexInAuto

  Description:  Return index of latest data sampling in loop buffer where values    
                are stored due to auto-sampling.    

  Arguments:    None.

  Returns:      u16 - index of latest data sampling    
 *---------------------------------------------------------------------------*/
u16 TP_GetLatestIndexInAuto(void)
{
    return tpState.index;
}




/*---------------------------------------------------------------------------*
  Name:         TP_CalcCalibrateParam

  Description:  Calculate calibration parameters from two points of touch panel
                values and screen coordinates.

  Arguments:    raw_x1, raw_y1 - Point 1 touch panel values
                dx1, dy1       - Point 1 screen coordinates
                raw_x2, raw_y2 - Point 2 touch panel values
                dx2, dy2,      - Point 2 screen coordinates
                
  Returns:      calibrate      - calibration parameters
                u32            - If a valid parameter return 0.
                                 If an invalid parameter return a non-zero.
                                 
 *---------------------------------------------------------------------------*/
u32 TP_CalcCalibrateParam(TPCalibrateParam *calibrate,
                          u16 raw_x1, u16 raw_y1,
                          u16 dx1, u16 dy1, u16 raw_x2, u16 raw_y2, u16 dx2, u16 dy2)
{
    s32     rx_width, dx_width, ry_width, dy_width;
    s32     tmp32;
    OSIntrMode enabled;

#define IN_S16_RANGE(x) ((x) < 0x8000 && (x) >= -0x8000)

    /*                                                                  */
    /* xDotSize = ((raw_x1 - raw_x2) << SCALE_SHIFT) / (dx1 - dx2)*/
    /* x0 = ((raw_x1 + raw_x2) - (dx1 + dx2) * xDotSize) / 2*/
    /* xDotSize = ((raw_x1 - raw_x2) << SCALE_SHIFT) / (dx1 - dx2)*/
    /* x0 = ((raw_x1 + raw_x2) - (dx1 + dx2) * xDotSize) / 2*/
    /*                                                                  */

    SDK_NULL_ASSERT(calibrate);
    // check of coordinate range
    if (raw_x1 >= TP_RAW_MAX || raw_y1 >= TP_RAW_MAX || raw_x2 >= TP_RAW_MAX
        || raw_y2 >= TP_RAW_MAX)
    {
        return 1;
    }
    if (dx1 >= GX_LCD_SIZE_X || dx2 >= GX_LCD_SIZE_X || dy1 >= GX_LCD_SIZE_Y
        || dy2 >= GX_LCD_SIZE_Y)
    {
        return 1;
    }
    if (dx1 == dx2 || dy1 == dy2 || raw_x1 == raw_x2 || raw_y1 == raw_y2)
    {
        return 1;
    }

    rx_width = raw_x1 - raw_x2;
    dx_width = dx1 - dx2;

    enabled = OS_DisableInterrupts();

    // calculate xDotSize
    CP_SetDiv32_32(((u32)rx_width) << TP_CALIBRATE_DOT_SCALE_SHIFT, (u32)dx_width);

    ry_width = raw_y1 - raw_y2;
    dy_width = dy1 - dy2;

    tmp32 = CP_GetDivResult32();
    CP_SetDiv32_32(((u32)ry_width) << TP_CALIBRATE_DOT_SCALE_SHIFT, (u32)dy_width);

    if (!IN_S16_RANGE(tmp32))
    {
        (void)OS_RestoreInterrupts(enabled);
        return 1;
    }
    calibrate->xDotSize = (s16)tmp32;
    tmp32 = (s16)((((s32)(raw_x1 + raw_x2) << TP_CALIBRATE_DOT_SCALE_SHIFT)
                   - ((s32)(dx1 + dx2) * calibrate->xDotSize)) >> (TP_CALIBRATE_DOT_SCALE_SHIFT -
                                                                   TP_CALIBRATE_ORIGIN_SCALE_SHIFT +
                                                                   1));
    if (!IN_S16_RANGE(tmp32))
    {
        (void)OS_RestoreInterrupts(enabled);
        return 1;
    }
    calibrate->x0 = (s16)tmp32;

    tmp32 = CP_GetDivResult32();
    (void)OS_RestoreInterrupts(enabled);

    if (!IN_S16_RANGE(tmp32))
    {
        return 1;
    }
    calibrate->yDotSize = (s16)tmp32;
    tmp32 = (s16)((((s32)(raw_y1 + raw_y2) << TP_CALIBRATE_DOT_SCALE_SHIFT)
                   - ((s32)(dy1 + dy2) * calibrate->yDotSize)) >> (TP_CALIBRATE_DOT_SCALE_SHIFT -
                                                                   TP_CALIBRATE_ORIGIN_SCALE_SHIFT +
                                                                   1));
    if (!IN_S16_RANGE(tmp32))
    {
        return 1;
    }
    calibrate->y0 = (s16)tmp32;

    return 0;
}


/*---------------------------------------------------------------------------*
  Name:         TP_GetCalibratedPoint
  
  Description:  Converts touch panel value to screen coordinate.
                When calibration parameter is not set,
                touch panel value is returned directly.    
                It is OK to pass the same pointer for/to/in the arguments disp and raw.     

  Arguments:    raw      - Pointer to the conversion source touch panel value.
                
  ReturnValue:  disp     - Pointer to the variable returned by the value converted to a screen coordinate.
 *---------------------------------------------------------------------------*/
void TP_GetCalibratedPoint(TPData *disp, const TPData *raw)
{
    TPiCalibrateParam *cal;

    enum
    { MAX_X = GX_LCD_SIZE_X - 1, MAX_Y = GX_LCD_SIZE_Y - 1 };

    // ----------------------------------------
    // dx = (raw_x * x0) / xDotSize
    // dy = (raw_y * y0) / yDotSize
    // ----------------------------------------

    SDK_NULL_ASSERT(disp);
    SDK_NULL_ASSERT(raw);
    SDK_NULL_ASSERT(tpState.calibrate_flg);

    if (!tpState.calibrate_flg)
    {
        *disp = *raw;
        return;
    }

    cal = &tpState.calibrate;

    disp->touch = raw->touch;
    disp->validity = raw->validity;

    if (raw->touch == 0)
    {
        disp->x = 0;
        disp->y = 0;
        return;
    }

    // X coordinate conversion
    // disp->x = (x - x0) / xDotSize;
    disp->x =
        (u16)((((u64)(raw->x << TP_CALIBRATE_ORIGIN_SCALE_SHIFT) -
                cal->x0) * cal->xDotSizeInv) >> (TP_CALIBRATE_DOT_INV_SCALE_SHIFT +
                                                 TP_CALIBRATE_ORIGIN_SCALE_SHIFT));

    if ((s16)disp->x < 0)
    {
        disp->x = 0;
    }
    else if ((s16)disp->x > MAX_X)
    {
        disp->x = MAX_X;
    }
    // Y coordinate conversion
    // disp->y = (y - y0) / yDotSize;
    disp->y =
        (u16)((((u64)(raw->y << TP_CALIBRATE_ORIGIN_SCALE_SHIFT) -
                cal->y0) * cal->yDotSizeInv) >> (TP_CALIBRATE_DOT_INV_SCALE_SHIFT +
                                                 TP_CALIBRATE_ORIGIN_SCALE_SHIFT));

    if ((s16)disp->y < 0)
    {
        disp->y = 0;
    }
    else if ((s16)disp->y > MAX_Y)
    {
        disp->y = MAX_Y;
    }
}

/*---------------------------------------------------------------------------*
  Name:         TP_GetUnCalibratedPoint

  Description:  Gets reverse conversion results of calibration.    
                Converts from screen coordinate to touch panel value.
                When calibration parameter is not set,
                screen coordinate is output directly.

  Arguments:    dx, dy       - Screen XY coordinates.
                
  ReturnValue:  raw_x, raw_y - Pointer for returning corresponding touch panel value.
 *---------------------------------------------------------------------------*/
void TP_GetUnCalibratedPoint(u16 *raw_x, u16 *raw_y, u16 dx, u16 dy)
{
    TPiCalibrateParam *cal;

    // ----------------------------------------
    // raw_x = dx * xDotSize + x0;
    // raw_y = dy * yDotSize + y0;
    // ----------------------------------------

    SDK_NULL_ASSERT(raw_x);
    SDK_NULL_ASSERT(raw_y);
    SDK_ASSERT(tpState.calibrate_flg);

    if (!tpState.calibrate_flg)
    {
        *raw_x = dx;
        *raw_y = dy;
        return;
    }

    cal = &tpState.calibrate;

    *raw_x =
        (u16)((((s32)(dx * cal->xDotSize) >> TP_CALIBRATE_DOT2ORIGIN_SCALE_SHIFT) +
               (s32)cal->x0) >> TP_CALIBRATE_ORIGIN_SCALE_SHIFT);
    *raw_y =
        (u16)((((s32)(dy * cal->yDotSize) >> TP_CALIBRATE_DOT2ORIGIN_SCALE_SHIFT) +
               (s32)cal->y0) >> TP_CALIBRATE_ORIGIN_SCALE_SHIFT);
}


/*---------------------------------------------------------------------------*
  Name:         TP_WaitBusy

  Description:  Waits until ARM7 completes touch panel request.

  Arguments:    command_flgs  - Type of request to touch panel.
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_WaitBusy(TPRequestCommandFlag command_flgs)
{
    // Goes to infinite loop if interrupt is OFF
    SDK_ASSERT(OS_GetCpsrIrq() == OS_INTRMODE_IRQ_ENABLE);
    SDK_ASSERT(reg_OS_IME == 1);
    SDK_ASSERT(OS_GetIrqMask() & OS_IE_SPFIFO_RECV);

    // Insert ASSERT before loop starts. The call of this function should not
    // be allowed in interrupt prohibited status. The tpState flag already being
    // cleared or not may depend on the situation, so

    while (tpState.command_flg & command_flgs)
    {
        // Do nothing
    }

    return;
}

/*---------------------------------------------------------------------------*
  Name:         TP_WaitAllBusy

  Description:  Waits until ARM7 completes all of the touch panel requests.

  Arguments:    None.
                
  Returns:      None.
 *---------------------------------------------------------------------------*/
void TP_WaitAllBusy(void)
{
    // Goes to infinite loop if interrupt is OFF
    SDK_ASSERT(OS_GetCpsrIrq() == OS_INTRMODE_IRQ_ENABLE);
    SDK_ASSERT(reg_OS_IME == 1);
    SDK_ASSERT(OS_GetIrqMask() & OS_IE_SPFIFO_RECV);

    while (tpState.command_flg)
    {
        // Do nothing
    }

    return;
}


/*---------------------------------------------------------------------------*
  Name:         TP_CheckBusy

  Description:  Checks whether touch panel request to ARM7 is busy.

  Arguments:    command_flgs  - Type of request to touch panel.
                
  Returns:      u32          - Returns 0 if not busy.
                               Returns non-zero if busy.
 *---------------------------------------------------------------------------*/
u32 TP_CheckBusy(TPRequestCommandFlag command_flgs)
{
    return (u32)(tpState.command_flg & command_flgs);
}



/*---------------------------------------------------------------------------*
  Name:         TP_CheckError

  Description:  Checks if touch panel request to ARM7 isn't error terminating.

  Arguments:    command_flgs  - Type of request to touch panel.
                
  Returns:      u32           - Returns 0 if error is not occurring.
                                Returns non-zero if error is occurring.
 *---------------------------------------------------------------------------*/
u32 TP_CheckError(TPRequestCommandFlag command_flgs)
{
    return (u32)(tpState.err_flg & command_flgs);
}
