/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WVR - libraries
  File:     wvr.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wvr.c,v $
  Revision 1.9  01/18/2006 02:12:40  kitase_hirotake
  do-indent

  Revision 1.8  05/09/2005 08:30:20  terui
  Fix comment.
  Corrected so the PXI callback destination is cleared to NULL when forced termination is successful.

  Revision 1.7  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.6  02/17/2005 00:01:51  terui
  Revised it so PXI reception callback settings limited to only first time.

  Revision 1.5  02/16/2005 10:36:18  terui
  Fixed indents.

  Revision 1.4  02/09/2005 09:21:47  terui
  Added a VRAM bank exclusive feature.

  Revision 1.3  01/11/2005 07:46:42  takano_makoto
  fix copyright header.

  Revision 1.2  01/05/2005 07:30:37  terui
  Revised the wvr library's include path to be clear.    

  Revision 1.1  12/20/2004 00:52:43  terui
  This is a provisional version, but newly registered with operating conditions.    

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wvr/common/wvr_common.h>
#include    <nitro/wvr/ARM9/wvr.h>
#include    <nitro/pxi.h>
#include    <nitro/os/ARM9/vramExclusive.h>
#include    <nitro/gx/gx_vramcnt.h>
#include    <nitro/wm.h>


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void WvrReceiveCallback(PXIFifoTag tag, u32 data, BOOL err);
static void WvrDummyAsyncCallback(void *arg, WVRResult result);


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static WVRCallbackFunc wvrCallback = NULL;
static void *wvrArg = NULL;
static vu16 wvrVRam = 0;
static vu16 wvrLockId = 0;


/*---------------------------------------------------------------------------*
  Name:         WVR_StartUpAsync

  Description:  Starts operations of the wireless library.
                Until forcedly stopped, access to the specified VRAM (C or D) is prohibited.

  Arguments:    vram        -   Specifies VRAM bank allocated to ARM7.    
                callback    -   Specifies callback function at time process ends.
                arg         -   Specifies argument passed to callback.    

  Returns:      The result of the process.
 *---------------------------------------------------------------------------*/
WVRResult WVR_StartUpAsync(GXVRamARM7 vram, WVRCallbackFunc callback, void *arg)
{
    OSIntrMode e;

    // Check whether component supports WVR
    PXI_Init();
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM7))
    {
        return WVR_RESULT_DISABLE;
    }

    // Get exclusive-use ID if not already obtained    
    while (wvrLockId == 0)
    {
        s32     result = OS_GetLockID();

        if (result == OS_LOCK_ID_ERROR)
        {
            return WVR_RESULT_FATAL_ERROR;
        }
        wvrLockId = (u16)result;
    }

    // Prohibit interrupts
    e = OS_DisableInterrupts();

    // Check whether WVR asynchronous function is running
    if (wvrCallback != NULL)
    {
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_ILLEGAL_STATUS;
    }

    // Check the VRAM exclusive state
    if (wvrVRam)
    {
        // Already made exclusive
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_ILLEGAL_STATUS;
    }
    switch (vram)
    {
    case GX_VRAM_ARM7_128_C:
        // Exclusive lock VRAM-C
        if (!OSi_TryLockVram(OS_VRAM_BANK_ID_C, wvrLockId))
        {
            // Lock on VRAM-C fails
            (void)OS_RestoreInterrupts(e);
            return WVR_RESULT_VRAM_LOCKED;
        }
        wvrVRam = OS_VRAM_BANK_ID_C;
        // Register-setting process corresponding to GX_SetBankForARM7( GX_VRAM_ARM7_128_C )     
        reg_GX_VRAMCNT_C = ((2 << REG_GX_VRAMCNT_C_MST_SHIFT) |
                            (0 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT));
        break;
    case GX_VRAM_ARM7_128_D:
        // Exclusive lock VRAM-D
        if (!OSi_TryLockVram(OS_VRAM_BANK_ID_D, wvrLockId))
        {
            // Lock on VRAM-D fails
            (void)OS_RestoreInterrupts(e);
            return WVR_RESULT_VRAM_LOCKED;
        }
        wvrVRam = OS_VRAM_BANK_ID_D;
        // Register-setting process corresponding to GX_SetBankForARM7( GX_VRAM_ARM7_128_D )    
        reg_GX_VRAMCNT_D = ((2 << REG_GX_VRAMCNT_D_MST_SHIFT) |
                            (0 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 << REG_GX_VRAMCNT_D_E_SHIFT));
        break;
    case GX_VRAM_ARM7_256_CD:
        // Exclusive lock VRAM-C/D
        if (!OSi_TryLockVram((OS_VRAM_BANK_ID_C | OS_VRAM_BANK_ID_D), wvrLockId))
        {
            // Locks on VRAM-C/D fail
            (void)OS_RestoreInterrupts(e);
            return WVR_RESULT_VRAM_LOCKED;
        }
        wvrVRam = OS_VRAM_BANK_ID_C | OS_VRAM_BANK_ID_D;
        // Register settings process that  corresponds to GX_SetBankForARM7( GX_VRAM_ARM7_256_CD )    
        reg_GX_VRAMCNT_C = ((2 << REG_GX_VRAMCNT_C_MST_SHIFT) |
                            (0 << REG_GX_VRAMCNT_C_OFS_SHIFT) | (1 << REG_GX_VRAMCNT_C_E_SHIFT));
        reg_GX_VRAMCNT_D = ((2 << REG_GX_VRAMCNT_D_MST_SHIFT) |
                            (1 << REG_GX_VRAMCNT_D_OFS_SHIFT) | (1 << REG_GX_VRAMCNT_D_E_SHIFT));
        break;
    default:
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_INVALID_PARAM;
    }

    // Set the PXI callback function
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM9))
    {
        PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WVR, WvrReceiveCallback);
    }

    // Save the callback function
    if (callback == NULL)
    {
        wvrCallback = WvrDummyAsyncCallback;
    }
    else
    {
        wvrCallback = callback;
    }
    wvrArg = arg;

    // Notify of StartUp via PXI
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_WVR, WVR_PXI_COMMAND_STARTUP, FALSE))
    {
        // Exclusive unlock VRAM that is exclusive locked    
        OSi_UnlockVram(wvrVRam, wvrLockId);
        wvrVRam = 0;
        wvrCallback = NULL;
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_FIFO_ERROR;
    }

    // Normal start and stop of asynchronous process    
    (void)OS_RestoreInterrupts(e);
    return WVR_RESULT_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WVR_TerminateAsync

  Description:  Forcedly stops operations of the wireless library.
                Access to VRAM (C or D) is permitted after this  asynchronous process ends.    

  Arguments:    callback    -   Specifies callback function at time process ends.
                arg         -   Specifies argument passed to callback.    

  Returns:      The result of the process.
 *---------------------------------------------------------------------------*/
WVRResult WVR_TerminateAsync(WVRCallbackFunc callback, void *arg)
{
    OSIntrMode e;

    // Check component compatibility
    PXI_Init();
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM7))
    {
        return WVR_RESULT_DISABLE;
    }

    // Prohibit interrupts
    e = OS_DisableInterrupts();

    // Check whether WVR asynchronous function is running
    if (wvrCallback != NULL)
    {
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_ILLEGAL_STATUS;
    }

    // Set the PXI callback function
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WVR, PXI_PROC_ARM9))
    {
        PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WVR, WvrReceiveCallback);
    }

    // Save the callback function
    if (callback == NULL)
    {
        wvrCallback = WvrDummyAsyncCallback;
    }
    else
    {
        wvrCallback = callback;
    }
    wvrArg = arg;

    // Notify of Terminate via PXI
    if (0 > PXI_SendWordByFifo(PXI_FIFO_TAG_WVR, WVR_PXI_COMMAND_TERMINATE, FALSE))
    {
        wvrCallback = NULL;
        (void)OS_RestoreInterrupts(e);
        return WVR_RESULT_FIFO_ERROR;
    }

    // Normal start and stop of asynchronous process    
    (void)OS_RestoreInterrupts(e);
    return WVR_RESULT_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WvrReceiveCallback

  Description:  Receives callback from ARM7 via FIFO.

  Arguments:    tag          - Not used.
                data         - Data received from ARM7.
                err          - Not used.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WvrReceiveCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused( tag , err )

    WVRCallbackFunc cb = wvrCallback;
    void   *cbArg = wvrArg;
    WVRResult result = (WVRResult)(data & 0x000000ff);

    // Direct process according to command type    
    switch (data & 0xffff0000)
    {
    case WVR_PXI_COMMAND_STARTUP:
        switch (result)
        {
        case WVR_RESULT_FATAL_ERROR:
            // Exclusive release VRAM    
            if ((wvrVRam != 0) && (wvrLockId != 0))
            {
                OSi_UnlockVram(wvrVRam, wvrLockId);
                wvrVRam = 0;
            }
            break;
        }
        break;
    case WVR_PXI_COMMAND_TERMINATE:
        switch (result)
        {
        case WVR_RESULT_SUCCESS:
            // Exclusive release VRAM    
            if ((wvrVRam != 0) && (wvrLockId != 0))
            {
                OSi_UnlockVram(wvrVRam, wvrLockId);
                wvrVRam = 0;
            }
            break;
        }
        // PXI callback destination settings cleared to NULL.
        PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WVR, NULL);
        break;
    }

    // Call to user callback
    if (cb != NULL)
    {
        wvrCallback = NULL;
        wvrArg = NULL;
        cb(cbArg, result);
    }
}

/*---------------------------------------------------------------------------*
  Name:         WvrDummyAsyncCallback

  Description:  Dummy callback function set to asynchronous function.

  Arguments:    arg     -   Unused.
                result  -   Result of asynchronous process.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WvrDummyAsyncCallback(void *arg, WVRResult result)
{
#pragma unused( arg , result )
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
