/*---------------------------------------------------------------------------*
  Project:  NitroSDK - SPI - libraries
  File:     mic.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mic.c,v $
  Revision 1.16  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.15  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.14  01/11/2005 07:44:00  takano_makoto
  fix copyright header.

  Revision 1.13  01/07/2005 12:32:39  terui
  Added MIC_AdjustAutoSampling[Async] function

  Revision 1.12  12/03/2004 05:40:03  terui
  Complied with the added feature that explicitly indicates the data that was left out during the MIC auto sampling.

  Revision 1.11  11/04/2004 07:11:54  terui
  Comments revised.

  Revision 1.10  11/02/2004 04:31:13  terui
  Fixed comments.

  Revision 1.9  09/28/2004 06:12:29  terui
  Added a 32-byte multiple check of the buffer size to the MIC_StartAutoSampling(Async) function.

  Revision 1.8  09/17/2004 09:36:03  terui
  Revised in connection with changes to MIC_StartAutoSampling(Async) parameters    

  Revision 1.7  07/13/2004 09:20:07  terui
  Changes reflecting addition of signed sampling types

  Revision 1.6  06/10/2004 06:50:23  terui
  Clear of variables in shared area during initialization

  Revision 1.5  06/03/2004 04:09:29  terui
  Changed definition of shortest sampling rate.

  Revision 1.4  06/01/2004 04:47:58  terui
  Support for data storage location being fixed to the shared area during mic stand-alone sampling    

  Revision 1.3  06/01/2004 02:03:38  terui
  Fix accompanying change to MICAutoParam member name

  Revision 1.2  06/01/2004 00:57:11  terui
  Changed so that sampling period can be specified directly in ARM7 clock units.

  Revision 1.1  05/31/2004 08:52:56  terui
  Initial upload.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/spi.h>


/*---------------------------------------------------------------------------*
    structure definitions
 *---------------------------------------------------------------------------*/
// Asynchronized Function Exclusive Processing Lock Definition
typedef enum MICLock
{
    MIC_LOCK_OFF = 0,                  // Unlock status
    MIC_LOCK_ON,                       // Lock status
    MIC_LOCK_MAX
}
MICLock;

// Work Structure
typedef struct MICWork
{
    MICLock lock;                      // exception lock
    MICCallback callback;              // for temporary storage of asynchronized function callback    
    void   *callbackArg;               // for saving callback function arguments
    MICResult commonResult;            // for temporary storage of asynchronized function processing result     
    MICCallback full;                  // for saving sampling complete callback
    void   *fullArg;                   // for saving complete callback function argument
    void   *dst_buf;                   // for saving simple sampling results storage area    

}
MICWork;

/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static u16 micInitialized;             // initialization verify flag
static MICWork micWork;                // structure with work variables combined


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void MicCommonCallback(PXIFifoTag tag, u32 data, BOOL err);
static BOOL MicDoSampling(u16 type);
static BOOL MicStartAutoSampling(void *buf, u32 size, u32 span, u8 flags);
static BOOL MicStopAutoSampling(void);
static BOOL MicAdjustAutoSampling(u32 span);
static void MicGetResultCallback(MICResult result, void *arg);
static void MicWaitBusy(void);


/*---------------------------------------------------------------------------*
  Name:         MIC_Init

  Description:  Initializes microphone library.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MIC_Init(void)
{
    // non-initialization verification
    if (micInitialized)
    {
        return;
    }
    micInitialized = 1;

    // work variable initialization
    micWork.lock = MIC_LOCK_OFF;
    micWork.callback = NULL;

    // Wait until ARM7 MIC library started
    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_MIC, PXI_PROC_ARM7))
    {
    }

    // Clear latest sampling result storage address for shared area
    OS_GetSystemWork()->mic_last_address = 0;

    // Set the PXI callback function
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_MIC, MicCommonCallback);
}

/*---------------------------------------------------------------------------*
  Name:         MIC_DoSamplingAsync

  Description:  Performs a single async sample of the mic.

  Arguments:    type      - The sampling type.
                buf       - Buffer that stores the sampling data.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      MICResult    - Returns the result of the start of the asynchronous operation.
 *---------------------------------------------------------------------------*/
MICResult MIC_DoSamplingAsync(MICSamplingType type, void *buf, MICCallback callback, void *arg)
{
    OSIntrMode enabled;
    u16     wtype;

    SDK_NULL_ASSERT(buf);
    SDK_NULL_ASSERT(callback);

    // confirms parameters
    if (type >= MIC_SAMPLING_TYPE_MAX)
    {
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }
    switch (type)
    {
    case MIC_SAMPLING_TYPE_8BIT:
        wtype = SPI_MIC_SAMPLING_TYPE_8BIT;
        break;
    case MIC_SAMPLING_TYPE_12BIT:
        wtype = SPI_MIC_SAMPLING_TYPE_12BIT;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_8BIT:
        wtype = SPI_MIC_SAMPLING_TYPE_S8BIT;
        break;
    case MIC_SAMPLING_TYPE_SIGNED_12BIT:
        wtype = SPI_MIC_SAMPLING_TYPE_S12BIT;
        break;
    default:
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (micWork.lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    micWork.lock = MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // Send execute sampling command     
    micWork.callback = callback;
    micWork.callbackArg = arg;
    micWork.dst_buf = buf;
    if (MicDoSampling(wtype))
    {
        return MIC_RESULT_SUCCESS;
    }
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_DoSampling

  Description:  Samples the mic once.

  Arguments:    type      - The sampling type.
                buf       - Buffer that stores the sampling data.

  Returns:      MICResult - Returns the result of device operation process.
 *---------------------------------------------------------------------------*/
MICResult MIC_DoSampling(MICSamplingType type, void *buf)
{
    micWork.commonResult = MIC_DoSamplingAsync(type, buf, MicGetResultCallback, NULL);
    if (micWork.commonResult == MIC_RESULT_SUCCESS)
    {
        MicWaitBusy();
    }
    return micWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StartAutoSamplingAsync

  Description:  Starts mic auto sampling asynchronously.

  Arguments:    param     - Specifies the auto sampling setting with the pointer to the structure.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      MICResult    - Returns the result of the start of the asynchronous operation.
 *---------------------------------------------------------------------------*/
MICResult MIC_StartAutoSamplingAsync(const MICAutoParam *param, MICCallback callback, void *arg)
{
    OSIntrMode enabled;
    u8      flags;

    SDK_NULL_ASSERT(callback);
    SDK_NULL_ASSERT(param->buffer);

    // confirms parameters
    {
        // buffer 32 byte align
        if ((u32)(param->buffer) & 0x01f)
        {
#ifdef  SDK_DEBUG
            OS_TWarning("Parameter param->buffer must be 32-byte aligned.\n");
#endif
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // Buffer size alignment
        if (param->size & 0x01f)
        {
#ifdef  SDK_DEBUG
            OS_TWarning("Parameter param->size must be a multiple of 32-byte.\n");
#endif
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // Buffer size
        if (param->size <= 0)
        {
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // sampling rate    
        if (param->rate < MIC_SAMPLING_RATE_LIMIT)
        {
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // AD conversion bit width
        switch (param->type)
        {
        case MIC_SAMPLING_TYPE_8BIT:
            flags = SPI_MIC_SAMPLING_TYPE_8BIT;
            break;
        case MIC_SAMPLING_TYPE_12BIT:
            flags = SPI_MIC_SAMPLING_TYPE_12BIT;
            break;
        case MIC_SAMPLING_TYPE_SIGNED_8BIT:
            flags = SPI_MIC_SAMPLING_TYPE_S8BIT;
            break;
        case MIC_SAMPLING_TYPE_SIGNED_12BIT:
            flags = SPI_MIC_SAMPLING_TYPE_S12BIT;
            break;
        case MIC_SAMPLING_TYPE_12BIT_FILTER_OFF:
            flags = (SPI_MIC_SAMPLING_TYPE_12BIT | SPI_MIC_SAMPLING_TYPE_FILTER_OFF);
            break;
        case MIC_SAMPLING_TYPE_SIGNED_12BIT_FILTER_OFF:
            flags = (SPI_MIC_SAMPLING_TYPE_S12BIT | SPI_MIC_SAMPLING_TYPE_FILTER_OFF);
            break;
        default:
            return MIC_RESULT_ILLEGAL_PARAMETER;
        }
        // loop determination    
        if (param->loop_enable)
        {
            flags = (u8)(flags | SPI_MIC_SAMPLING_TYPE_LOOP_ON);
        }
        else
        {
            flags = (u8)(flags | SPI_MIC_SAMPLING_TYPE_LOOP_OFF);
        }
        // Correction flag for expansion is currently fixed in OFF.
        flags = (u8)(flags | SPI_MIC_SAMPLING_TYPE_CORRECT_OFF);
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (micWork.lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    micWork.lock = MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // Send auto-sampling start command
    micWork.callback = callback;
    micWork.callbackArg = arg;
    micWork.full = param->full_callback;
    micWork.fullArg = param->full_arg;
    if (MicStartAutoSampling(param->buffer, param->size, param->rate, flags))
    {
        return MIC_RESULT_SUCCESS;
    }
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StartAutoSampling

  Description:  Starts auto-sampling with the mic.

  Arguments:    param     - Specifies the auto sampling setting with the pointer to the structure.

  Returns:      MICResult - Returns the result of device operation process.
 *---------------------------------------------------------------------------*/
MICResult MIC_StartAutoSampling(const MICAutoParam *param)
{
    micWork.commonResult = MIC_StartAutoSamplingAsync(param, MicGetResultCallback, NULL);
    if (micWork.commonResult == MIC_RESULT_SUCCESS)
    {
        MicWaitBusy();
    }
    return micWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StopAutoSamplingAsync

  Description:  Stops auto-sampling with the mic asynchronously.

  Arguments:    callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      MICResult    - Returns the result of the start of the asynchronous operation.
 *---------------------------------------------------------------------------*/
MICResult MIC_StopAutoSamplingAsync(MICCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(callback);

    // Lock check
    enabled = OS_DisableInterrupts();
    if (micWork.lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    micWork.lock = MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // send auto-sampling stop command
    micWork.callback = callback;
    micWork.callbackArg = arg;
    if (MicStopAutoSampling())
    {
        return MIC_RESULT_SUCCESS;
    }
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_StopAutoSampling

  Description:  Stops auto-sampling with the mic.
                If a loop was not specified at the start of auto-sampling,
                it stops sampling automatically at the point where the buffer is full.

  Arguments:    None.

  Returns:      MICResult - Returns the result of device operation process.
 *---------------------------------------------------------------------------*/
MICResult MIC_StopAutoSampling(void)
{
    micWork.commonResult = MIC_StopAutoSamplingAsync(MicGetResultCallback, NULL);
    if (micWork.commonResult == MIC_RESULT_SUCCESS)
    {
        MicWaitBusy();
    }
    return micWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_AdjustAutoSamplingAsync

  Description:  Asynchronously adjusts the sampling rate used in microphone auto sampling.
                microphone's auto-sampling process

  Arguments:    rate      - Specifies the sampling rate.
                callback     - Specifies the function to be called when the asynchronous process completed.
                arg       - Specifies the argument used when calling the callback function.

  Returns:      MICResult    - Returns the result of the start of the asynchronous operation.
 *---------------------------------------------------------------------------*/
MICResult MIC_AdjustAutoSamplingAsync(u32 rate, MICCallback callback, void *arg)
{
    OSIntrMode enabled;

    SDK_NULL_ASSERT(callback);

    // confirms parameters
    if (rate < MIC_SAMPLING_RATE_LIMIT)
    {
        return MIC_RESULT_ILLEGAL_PARAMETER;
    }

    // Lock check
    enabled = OS_DisableInterrupts();
    if (micWork.lock != MIC_LOCK_OFF)
    {
        (void)OS_RestoreInterrupts(enabled);
        return MIC_RESULT_BUSY;
    }
    micWork.lock = MIC_LOCK_ON;
    (void)OS_RestoreInterrupts(enabled);

    // Send the auto sampling adjustment command
    micWork.callback = callback;
    micWork.callbackArg = arg;
    if (MicAdjustAutoSampling(rate))
    {
        return MIC_RESULT_SUCCESS;
    }
    return MIC_RESULT_SEND_ERROR;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_AdjustAutoSampling

  Description:  Adjusts the sampling rate of the microphone automatic sampling.

  Arguments:    rate      - Specifies the sampling rate.

  Returns:      MICResult - Returns the result of device operation process.
 *---------------------------------------------------------------------------*/
MICResult MIC_AdjustAutoSampling(u32 rate)
{
    micWork.commonResult = MIC_AdjustAutoSamplingAsync(rate, MicGetResultCallback, NULL);
    if (micWork.commonResult == MIC_RESULT_SUCCESS)
    {
        MicWaitBusy();
    }
    return micWork.commonResult;
}

/*---------------------------------------------------------------------------*
  Name:         MIC_GetLastSamplingAddress

  Description:  Gets the storage address of the most recent mic sample.     

  Arguments:    None.

  Returns:      void* - Returns the storage address of the sampling result.
                        Returns NULL if not yet sampling.
 *---------------------------------------------------------------------------*/
void   *MIC_GetLastSamplingAddress(void)
{
    return (void *)(OS_GetSystemWork()->mic_last_address);
}

/*---------------------------------------------------------------------------*
  Name:         MicCommonCallback

  Description:  Common callback function for asynchronous MIC function

  Arguments:    tag -  PXI tag which show message type.
                data - message from ARM7.
                err -  PXI transfer error flag.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MicCommonCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag )

    u16     command;
    u16     pxiresult;
    MICResult result;
    MICCallback cb;

    // verify PXI communication error
    if (err)
    {
        if (micWork.lock != MIC_LOCK_OFF)
        {
            micWork.lock = MIC_LOCK_OFF;
        }
        if (micWork.callback)
        {
            cb = micWork.callback;
            micWork.callback = NULL;
            cb(MIC_RESULT_FATAL_ERROR, micWork.callbackArg);
        }
    }

    // analyze received data
    command = (u16)((data & SPI_PXI_RESULT_COMMAND_MASK) >> SPI_PXI_RESULT_COMMAND_SHIFT);
    pxiresult = (u16)((data & SPI_PXI_RESULT_DATA_MASK) >> SPI_PXI_RESULT_DATA_SHIFT);

    // Verify processing result
    switch (pxiresult)
    {
    case SPI_PXI_RESULT_SUCCESS:
        result = MIC_RESULT_SUCCESS;
        break;
    case SPI_PXI_RESULT_INVALID_COMMAND:
        result = MIC_RESULT_INVALID_COMMAND;
        break;
    case SPI_PXI_RESULT_INVALID_PARAMETER:
        result = MIC_RESULT_ILLEGAL_PARAMETER;
        break;
    case SPI_PXI_RESULT_ILLEGAL_STATUS:
        result = MIC_RESULT_ILLEGAL_STATUS;
        break;
    case SPI_PXI_RESULT_EXCLUSIVE:
        result = MIC_RESULT_BUSY;
        break;
    default:
        result = MIC_RESULT_FATAL_ERROR;
    }

    // Verify processing target command
    if (command == SPI_PXI_COMMAND_MIC_BUFFER_FULL)
    {
        // Callback buffer full notification
        if (micWork.full)
        {
            micWork.full(result, micWork.fullArg);
        }
    }
    else
    {
        if (command == SPI_PXI_COMMAND_MIC_SAMPLING)
        {
            // Store sampling result in buffer
            if (micWork.dst_buf)
            {
                *(u16 *)(micWork.dst_buf) = OS_GetSystemWork()->mic_sampling_data;
            }
        }
        // exclusive lock open
        if (micWork.lock != MIC_LOCK_OFF)
        {
            micWork.lock = MIC_LOCK_OFF;
        }
        // Callback processing result
        if (micWork.callback)
        {
            cb = micWork.callback;
            micWork.callback = NULL;
            cb(result, micWork.callbackArg);
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MicDoSampling

  Description:  Send command for sampling mic once to ARM7.

  Arguments:    type - Sampling type
                ( 0: 8 bits , 1: 12 bits , 2: signed 8-bit, 3: signed 12 bit )

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL MicDoSampling(u16 type)
{
    // PXI Packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_SAMPLING << 8) | (u32)type, 0))
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicStartAutoSampling

  Description:  Send start command for mic auto-sampling to ARM7.

  Arguments:    buf   - Buffer address for storing data sampled
                size  - Buffer size. Specified in byte units.
                span     - sampling interval (specify with ARM7 CPU clock)
                        Due to timer characteristics, only numbers of 16 bits    
                        x 1or64or256or1024 can be accurately set up.     Fractional bits are rounded off.
                flags - Specify AD conversion bit width, loop determination, and correction determination.    

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL MicStartAutoSampling(void *buf, u32 size, u32 span, u8 flags)
{
    // PXI Packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_AUTO_ON << 8) | (u32)flags, 0))
    {
        return FALSE;
    }

    // PXI Packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (1 << SPI_PXI_INDEX_SHIFT) | ((u32)buf >> 16), 0))
    {
        return FALSE;
    }

    // PXI Packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (2 << SPI_PXI_INDEX_SHIFT) | ((u32)buf & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // PXI Packet [3]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (3 << SPI_PXI_INDEX_SHIFT) | (size >> 16), 0))
    {
        return FALSE;
    }

    // PXI Packet [4]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               (4 << SPI_PXI_INDEX_SHIFT) | (size & 0x0000ffff), 0))
    {
        return FALSE;
    }

    // PXI Packet [5]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (5 << SPI_PXI_INDEX_SHIFT) | (span >> 16), 0))
    {
        return FALSE;
    }

    // PXI Packet [6]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_END_BIT |
                               (6 << SPI_PXI_INDEX_SHIFT) | (span & 0x0000ffff), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicStopAutoSampling

  Description:  Send stop command for mic auto-sampling to ARM7.

  Arguments:    None.

  Returns:      BOOL - Returns TRUE when able to normally send command via PXI.
                       Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
static BOOL MicStopAutoSampling(void)
{
    // PXI Packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               SPI_PXI_END_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) | (SPI_PXI_COMMAND_MIC_AUTO_OFF << 8), 0))
    {
        return FALSE;
    }
    return TRUE;
}

static BOOL MicAdjustAutoSampling(u32 span)
{
    // PXI Packet [0]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_START_BIT |
                               (0 << SPI_PXI_INDEX_SHIFT) |
                               (SPI_PXI_COMMAND_MIC_AUTO_ADJUST << 8), 0))
    {
        return FALSE;
    }

    // PXI Packet [1]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC, (1 << SPI_PXI_INDEX_SHIFT) | (span >> 16), 0))
    {
        return FALSE;
    }

    // PXI Packet [2]
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_MIC,
                               SPI_PXI_END_BIT |
                               (2 << SPI_PXI_INDEX_SHIFT) | (span & 0x0000ffff), 0))
    {
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         MicGetResultCallback

  Description:  Called when asynchronized processing completes and updates internal functions processing results.

  Arguments:    result  -   The processing results from async function.
                arg     -   Not used.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MicGetResultCallback(MICResult result, void *arg)
{
#pragma unused( arg )

    micWork.commonResult = result;
}

#include    <nitro/code32.h>
/*---------------------------------------------------------------------------*
  Name:         MicWaitBusy

  Description:  Wait while asynchronous processing for MIC is locked

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static asm void
MicWaitBusy( void )
{
    ldr     r12,    =micWork.lock
loop:
    ldr     r0,     [ r12,  #0 ]
    cmp     r0,     #MIC_LOCK_ON
    beq     loop
    bx      lr
}
#include    <nitro/codereset.h>


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
