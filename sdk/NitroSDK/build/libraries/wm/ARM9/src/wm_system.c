/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_system.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_system.c,v $
  Revision 1.38  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.37  12/01/2005 15:05:15  seiki_masashi
  Added the maxSend/RecvDataSize structure field to the WMPortSend/RecvCallback structure.

  Revision 1.36  11/24/2005 03:05:33  seiki_masashi
  Added a function declaration to remove the warning

  Revision 1.35  11/22/2005 08:58:10  seiki_masashi
  added WM_SendCommandDirect function

  Revision 1.34  11/07/2005 01:10:33  okubata_ryoma
  Switched from SDK_ASSERT_ON_COMPILE to SDK_COMPILER_ASSERT

  Revision 1.33  11/01/2005 11:47:21  seiki_masashi
  Moved SDK_ASSERT_ON_COMPILE definition to header file.
  Added debug code
  Changed so that notification of connectedAidBitmap is made even if WM_STATECODE_PORT_RECV is set.

  Revision 1.32  11/01/2005 08:54:28  seiki_masashi
  Made WmReceiveFifo faster.
  Added wm9buf->myAid, connectedAidBitmap.
  Delected WM_INDBUF_SIZE.

  Revision 1.31  10/28/2005 11:22:41  seiki_masashi
  Changed so that notification of reason is made when notifying regarding disconnection from port.

  Revision 1.30  10/28/2005 11:13:54  seiki_masashi
  Implemented WM_STATECODE_DISCONNECTED_FROM_MYSELF.
  Implemented accessor to WMStatus structure.
  Corrected problem with port connected/disconnected notifications on child side.

  Revision 1.29  09/16/2005 10:14:21  terui
  Deleted NULL assert used with the WMi_GetSystemWork function.

  Revision 1.28  06/07/2005 05:45:26  seiki_masashi
  Revised to reduce special processing related to key sharing

  Revision 1.27  03/03/2005 14:45:27  seiki_masashi
  Added WM_DISABLE_KEYSHARING switch

  Revision 1.26  02/28/2005 05:26:35  yosizaki
  do-indent.

  Revision 1.25  02/23/2005 12:48:14  seiki_masashi
  Cleaned up debug display

  Revision 1.24  02/18/2005 12:27:45  seiki_masashi
  Debug switch statement changed from #if to #ifdef

  Revision 1.23  02/14/2005 06:37:47  terui
  Added WMi_CheckMpPacketTimeRequired function.

  Revision 1.22  01/27/2005 02:23:08  takano_makoto
  fix comments.

  Revision 1.21  01/26/2005 05:36:52  takano_makoto
  Added WMi_InitForScan

  Revision 1.19  01/26/2005 01:35:28  seiki_masashi
  Changed to look at ARM9Buf->scanOnlyFlag in PXI interrupt handler
  and invalidate WMStatus cache

  Revision 1.18  01/26/2005 01:12:11  seiki_masashi
  Changed so that WMStatus cache is not invalidated in PXI interrupt handler.

  Revision 1.17  2005/01/25 01:57:25  yasu
  Revised Copyright notation

  Revision 1.16  01/17/2005 02:34:08  terui
  In WmReceiveFifo, added check to see if WM_Finish was called after calling user callback.

  Revision 1.15  12/27/2004 05:31:55  terui
  Added exclusive operation of ring buffer for transmitting request

  Revision 1.14  10/22/2004 08:52:52  terui
  Restored specification that queued consecutive requests, which was accidentally deleted.

  Revision 1.13  10/22/2004 04:37:28  terui
  Made changed with the function that returned int type as WMErrCode to return WMErrCode enumerated type

  Revision 1.12  10/21/2004 12:08:25  terui
  Changed location that does post-processing for WM_End in PXI callback to be before user callback is called.

  Revision 1.11  10/05/2004 09:45:28  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.10  10/05/2004 01:04:17  terui
  Added the structure size check when compiling

  Revision 1.9  10/03/2004 07:47:13  ooe
  Merged with ipl_branch. Added the freeze process when Flash error

  Revision 1.8  09/29/2004 09:48:25  terui
  Corrected for cases where WMi_SendCommand might be called from an interrupt.

  Revision 1.7  09/24/2004 04:02:39  terui
  Changed the error check sequence in WM_Init.

  Revision 1.6  09/23/2004 09:24:17  seiki_masashi
  Corrected the exclusive control of WMi_SendCommand

  Revision 1.5  09/17/2004 06:36:08  seiki_masashi
  Added ssid and myAid fields to WMPortRecvCallback.

  Revision 1.4  09/14/2004 02:47:58  terui
  Added a feature to delete the buffer cache provided during initialization.

  Revision 1.3  2004/09/11 06:09:12  yasu
  Use OS_TPrintf instead of WMi_Printf

  Revision 1.2  09/11/2004 04:13:00  terui
  Changed print for displaying warnings so not much stack is consumed.

  Revision 1.1  09/10/2004 11:03:30  terui
  New upload reflecting the breakup of wm.c.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include    <nitro/wm.h>
#include    "wm_arm9_private.h"


/*---------------------------------------------------------------------------*
    constant definitions
 *---------------------------------------------------------------------------*/
#define     WM_BUF_MSG_NUM                  10


/*---------------------------------------------------------------------------*
    internal variable definitions
 *---------------------------------------------------------------------------*/
static u16 wmInitialized = 0;          // WM initialization flag
static WMArm9Buf *wm9buf;
static u32 fifoBuf[WM_BUF_MSG_NUM][WM_FIFO_BUF_SIZE / sizeof(u32)] ATTRIBUTE_ALIGN(32);
static OSMessageQueue bufMsgQ;         // Request queue for WM7
static OSMessage bufMsg[WM_BUF_MSG_NUM];        // Request queue buffer for WM7


/*---------------------------------------------------------------------------*
    Internal function definitions
 *---------------------------------------------------------------------------*/
static void WmReceiveFifo(PXIFifoTag tag, u32 fifo_buf_adr, BOOL err);
static void WmClearFifoRecvFlag(void);
static WMErrCode WmInitCore(void *wmSysBuf, u16 dmaNo, u32 bufSize);
static u32 *WmGetCommandBuffer4Arm7(void);


/*---------------------------------------------------------------------------*
  Name:         WM_Init

  Description:  Performs the Initialization process of WM library.
                Synchronous function that only initializes ARM9.

  Arguments:    wmSysBuf    -   Pointer to the buffer allocated by the caller.
                                As much as WM_SYSTEM_BUF_SIZE is required for buffer size.
                dmaNo       -   DMA number used by WM

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Init(void *wmSysBuf, u16 dmaNo)
{
    WMErrCode result;
    result = WmInitCore(wmSysBuf, dmaNo, WM_SYSTEM_BUF_SIZE);
    if (result != WM_ERRCODE_SUCCESS)
    {
        return result;
    }
    wm9buf->scanOnlyFlag = FALSE;
    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_InitForScan

  Description:  Performs initialization processing for WM library when using only scan.
                Synchronous function that only initializes ARM9.

  Arguments:    wmSysBuf    -   Pointer to the buffer allocated by the caller.
                                Buffer size must be WM_SYSTEM_BUF_SIZE_FOR_SCAN.
                dmaNo       -   DMA number used by WM

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_InitForScan(void *wmSysBuf, u16 dmaNo);
WMErrCode WMi_InitForScan(void *wmSysBuf, u16 dmaNo)
{
#define WM_STATUS_BUF_SIZE_FOR_SCAN      768
#define WM_ARM9WM_BUF_SIZE_FOR_SCAN      320
#define WM_SYSTEM_BUF_SIZE_FOR_SCAN      (WM_ARM9WM_BUF_SIZE_FOR_SCAN + WM_ARM7WM_BUF_SIZE + WM_STATUS_BUF_SIZE_FOR_SCAN + WM_FIFO_BUF_SIZE + WM_FIFO_BUF_SIZE)

    WMErrCode result;
    result = WmInitCore(wmSysBuf, dmaNo, WM_SYSTEM_BUF_SIZE_FOR_SCAN);
    if (result != WM_ERRCODE_SUCCESS)
    {
        return result;
    }
    wm9buf = (WMArm9Buf *)wmSysBuf;
    wm9buf->WM7 = (WMArm7Buf *)((u32)wm9buf + WM_ARM9WM_BUF_SIZE_FOR_SCAN);
    wm9buf->status = (WMStatus *)((u32)(wm9buf->WM7) + WM_ARM7WM_BUF_SIZE);
    wm9buf->fifo9to7 = (u32 *)((u32)(wm9buf->status) + WM_STATUS_BUF_SIZE_FOR_SCAN);
    wm9buf->fifo7to9 = (u32 *)((u32)(wm9buf->fifo9to7) + WM_FIFO_BUF_SIZE);
    wm9buf->dmaNo = dmaNo;
    wm9buf->scanOnlyFlag = TRUE;

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WmInitCore

  Description:  Library internal use function that performs WM library initialization process.
                Synchronous function that only initializes ARM9.

  Arguments:    wmSysBuf    -   Pointer to the buffer allocated by the caller.
                dmaNo       -   DMA number used by WM
                bufSize     -   Buffer size allocated to WM library

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
static WMErrCode WmInitCore(void *wmSysBuf, u16 dmaNo, u32 bufSize)
{
    // Checks if the structure size exceeds the expected value.
    SDK_COMPILER_ASSERT(sizeof(WMArm9Buf) <= WM_ARM9WM_BUF_SIZE);
    SDK_COMPILER_ASSERT(sizeof(WMArm7Buf) <= WM_ARM7WM_BUF_SIZE);
    SDK_COMPILER_ASSERT(sizeof(WMStatus) <= WM_STATUS_BUF_SIZE);

    // Confirms that the WM library initialization is complete.
    if (wmInitialized)
    {
        return WM_ERRCODE_ILLEGAL_STATE;        // Initialization complete
    }
    wmInitialized = 1;

    // Check the parameter
    if (wmSysBuf == NULL)
    {
        WM_WARNING("Parameter \"wmSysBuf\" must not be NULL.\n");
        wmInitialized = 0;
        return WM_ERRCODE_INVALID_PARAM;
    }
    if (dmaNo > MI_DMA_MAX_NUM)
    {
        WM_WARNING("Parameter \"dmaNo\" is over %d.\n", MI_DMA_MAX_NUM);
        wmInitialized = 0;
        return WM_ERRCODE_INVALID_PARAM;
    }
    if ((u32)wmSysBuf & 0x01f)
    {
        WM_WARNING("Parameter \"wmSysBuf\" must be 32-byte aligned.\n");
        wmInitialized = 0;
        return WM_ERRCODE_INVALID_PARAM;
    }

    // Confirms whether the WM library has started in ARM7
    PXI_Init();
    if (!PXI_IsCallbackReady(PXI_FIFO_TAG_WM, PXI_PROC_ARM7))
    {
        WM_WARNING("WM library on ARM7 side is not ready yet.\n");
        wmInitialized = 0;
        return WM_ERRCODE_WM_DISABLE;
    }

    /*  It there are excess data remained in cache, there is a possibility of write back occurring at unexpected timing to cause
       interference with the status control of ARM7.
       As a countermeasure, delete the cache of all work buffer at this point. */
    DC_InvalidateRange(wmSysBuf, bufSize);
    // Initialization for each buffer
    MI_DmaClear32(dmaNo, wmSysBuf, bufSize);
    wm9buf = (WMArm9Buf *)wmSysBuf;
    wm9buf->WM7 = (WMArm7Buf *)((u32)wm9buf + WM_ARM9WM_BUF_SIZE);
    wm9buf->status = (WMStatus *)((u32)(wm9buf->WM7) + WM_ARM7WM_BUF_SIZE);
    wm9buf->fifo9to7 = (u32 *)((u32)(wm9buf->status) + WM_STATUS_BUF_SIZE);
    wm9buf->fifo7to9 = (u32 *)((u32)(wm9buf->fifo9to7) + WM_FIFO_BUF_SIZE);

    // Clears the FIFO buffer writable flag
    WmClearFifoRecvFlag();

    // Initializes variables
    wm9buf->dmaNo = dmaNo;
    wm9buf->connectedAidBitmap = 0x0000;
    wm9buf->myAid = 0;

    // Initializes the callback table for each port
    {
        s32     i;

        for (i = 0; i < WM_NUM_OF_PORT; i++)
        {
            (void)WM_SetPortCallback((u16)i, NULL, NULL);
        }
    }

    // Initializes the queue for entry registration
    {
        s32     i;

        OS_InitMessageQueue(&bufMsgQ, bufMsg, WM_BUF_MSG_NUM);
        for (i = 0; i < WM_BUF_MSG_NUM; i++)
        {
            // Clears the ring buffer to the usable state.
            *((u16 *)(fifoBuf[i])) = WM_API_REQUEST_ACCEPTED;
            DC_StoreRange(fifoBuf[i], 2);
            (void)OS_SendMessage(&bufMsgQ, fifoBuf[i], OS_MESSAGE_BLOCK);
        }
    }

    // Sets a FIFO receiving function
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WM, WmReceiveFifo);

    return WM_ERRCODE_SUCCESS;
}





/*---------------------------------------------------------------------------*
  Name:         WM_Finish

  Description:  Performs the termination process of WM library. Synchronous function.
                Restores the state before WM_Init function was called.

  Arguments:    None.

  Returns:      WMErrCode   -   Returns the process result.
 *---------------------------------------------------------------------------*/
WMErrCode WM_Finish(void)
{
    WMErrCode result;

    // Cannot execute if initialization is complete
    result = WMi_CheckInitialized();
    if (result != WM_ERRCODE_SUCCESS)
    {
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    // Cannot execute if the state is not READY
    result = WMi_CheckState(WM_STATE_READY);
    WM_CHECK_RESULT(result);

    // finish WM library
    WmClearFifoRecvFlag();
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_WM, NULL);
    wm9buf = 0;
    wmInitialized = 0;

    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_SetCallbackTable

  Description:  Registers the callback function for each asynchronous function.

  Arguments:    id          -   Asynchronous function API ID.
                callback    -   The callback function to be registered.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WMi_SetCallbackTable(WMApiid id, WMCallbackFunc callback)
{
    SDK_NULL_ASSERT(wm9buf);

    wm9buf->CallbackTable[id] = callback;
}

/*---------------------------------------------------------------------------*
  Name:         WmGetCommandBuffer4Arm7

  Description:  Reserves a buffer for ARM7 commands from the pool

  Arguments:    None.

  Returns:      If it can be reserved, it's that value; otherwise: NULL
 *---------------------------------------------------------------------------*/
u32    *WmGetCommandBuffer4Arm7(void)
{
    u32    *tmpAddr;

    if (FALSE == OS_ReceiveMessage(&bufMsgQ, (OSMessage *)&tmpAddr, OS_MESSAGE_NOBLOCK))
    {
        return NULL;
    }

    // Check if the ring buffer is available (queue is not full)
    DC_InvalidateRange(tmpAddr, 2);
    if ((*((u16 *)tmpAddr) & WM_API_REQUEST_ACCEPTED) == 0)
    {
        // Reserve again at the beginning of the queue (because it should be available first)
        (void)OS_JamMessage(&bufMsgQ, tmpAddr, OS_MESSAGE_BLOCK);
        // End as an error
        return NULL;
    }

    return tmpAddr;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_SendCommand

  Description:  Transmits a request to ARM7 via FIFO.
                Specify by enumerating parameters
                for commands accompanying some u32-type parameters.

  Arguments:    id          -   API ID that corresponds to the request.
                paramNum    -   Number of virtual arguments.
                ...     -   Virtual argument.

  Returns:      int      - Returns WM_ERRCODE_* type process results.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_SendCommand(WMApiid id, u16 paramNum, ...)
{
    va_list vlist;
    s32     i;
    int     result;
    u32    *tmpAddr;

    SDK_NULL_ASSERT(wm9buf);

    // Reserves a buffer for command send
    tmpAddr = WmGetCommandBuffer4Arm7();
    if (tmpAddr == NULL)
    {
        return WM_ERRCODE_FIFO_ERROR;
    }

    // API ID
    *(u16 *)tmpAddr = (u16)id;

    // Adds the specified number of arguments
    va_start(vlist, paramNum);
    for (i = 0; i < paramNum; i++)
    {
        tmpAddr[i + 1] = va_arg(vlist, u32);
    }
    va_end(vlist);

    DC_StoreRange(tmpAddr, WM_FIFO_BUF_SIZE);

    // Notification with FIFO
    result = PXI_SendWordByFifo(PXI_FIFO_TAG_WM, (u32)tmpAddr, FALSE);

    (void)OS_SendMessage(&bufMsgQ, tmpAddr, OS_MESSAGE_BLOCK);

    if (result < 0)
    {
        WM_WARNING("Failed to send command through FIFO.\n");
        return WM_ERRCODE_FIFO_ERROR;
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_SendCommandDirect

  Description:  Transmits a request to ARM7 via FIFO.
                Directly specifies the command sent to ARM7.

  Arguments:    data        -   Command sent to ARM7
                length      -   Size of the command sent to ARM7

  Returns:      int      - Returns WM_ERRCODE_* type process results.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_SendCommandDirect(void *data, u32 length)
{
    int     result;
    u32    *tmpAddr;

    SDK_NULL_ASSERT(wm9buf);
    SDK_ASSERT(length <= WM_FIFO_BUF_SIZE);

    // Reserves a buffer for command send
    tmpAddr = WmGetCommandBuffer4Arm7();
    if (tmpAddr == NULL)
    {
        return WM_ERRCODE_FIFO_ERROR;
    }

    // Copies to a buffer specifically for commands sent to ARM7.
    MI_CpuCopy8(data, tmpAddr, length);

    DC_StoreRange(tmpAddr, length);

    // Notification with FIFO
    result = PXI_SendWordByFifo(PXI_FIFO_TAG_WM, (u32)tmpAddr, FALSE);

    (void)OS_SendMessage(&bufMsgQ, tmpAddr, OS_MESSAGE_BLOCK);

    if (result < 0)
    {
        WM_WARNING("Failed to send command through FIFO.\n");
        return WM_ERRCODE_FIFO_ERROR;
    }

    return WM_ERRCODE_OPERATING;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_GetSystemWork

  Description:  Gets the pointer to the top of the buffer used internally by the WM library.

  Arguments:    None.

  Returns:      WMArm9Buf*  -   Returns a pointer to the internal work buffer.
 *---------------------------------------------------------------------------*/
WMArm9Buf *WMi_GetSystemWork(void)
{
//    SDK_NULL_ASSERT(wm9buf);
    return wm9buf;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_CheckInitialized

  Description:  Confirms that the WM library initialization is complete.

  Arguments:    None.

  Returns:      int      - Returns WM_ERRCODE_* type process results.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_CheckInitialized(void)
{
    // confirms if initialized
    if (!wmInitialized)
    {
        WM_WARNING("WM library is not initialized yet.\n");
        return WM_ERRCODE_ILLEGAL_STATE;
    }
    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_CheckIdle

  Description:  Confirms the internal state of the WM library, and confirms that the wireless hardware
                is already started.

  Arguments:    None.

  Returns:      int      - Returns WM_ERRCODE_* type process results.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_CheckIdle(void)
{
    WMErrCode result;

    // confirms if initialized
    result = WMi_CheckInitialized();
    WM_CHECK_RESULT(result);

    // Confirms the current state
    DC_InvalidateRange(&(wm9buf->status->state), 2);
    if ((wm9buf->status->state == WM_STATE_READY) || (wm9buf->status->state == WM_STATE_STOP))
    {
        WM_WARNING("WM state is \"%d\" now. So can't execute request.\n", wm9buf->status->state);
        return WM_ERRCODE_ILLEGAL_STATE;
    }

    return WM_ERRCODE_SUCCESS;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_CheckStateEx

  Description:  Confirms the internal state of the WM library.
                Enumerates the WMState type parameters showing the permitted state to specify.

  Arguments:    paramNum    -   Number of virtual arguments.
                ...     -   Virtual argument.

  Returns:      int      - Returns WM_ERRCODE_* type process results.
 *---------------------------------------------------------------------------*/
WMErrCode WMi_CheckStateEx(s32 paramNum, ...)
{
    WMErrCode result;
    u16     now;
    u32     temp;
    va_list vlist;

    // confirms if initialized
    result = WMi_CheckInitialized();
    WM_CHECK_RESULT(result);

    // Gets the current state
    DC_InvalidateRange(&(wm9buf->status->state), 2);
    now = wm9buf->status->state;

    // Match confirmation
    result = WM_ERRCODE_ILLEGAL_STATE;
    va_start(vlist, paramNum);
    for (; paramNum; paramNum--)
    {
        temp = va_arg(vlist, u32);
        if (temp == now)
        {
            result = WM_ERRCODE_SUCCESS;
        }
    }
    va_end(vlist);

    if (result == WM_ERRCODE_ILLEGAL_STATE)
    {
        WM_WARNING("WM state is \"%d\" now. So can't execute request.\n", now);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         WmReceiveFifo

  Description:  Receives a callback from WM7 via FIFO.

  Arguments:    tag          - Unused
                fifo_buf_adr - A pointer to the callback parameter group
                err          - Unused

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmReceiveFifo(PXIFifoTag tag, u32 fifo_buf_adr, BOOL err)
{
#pragma unused( tag )

    WMCallback *callback_p = (WMCallback *)fifo_buf_adr;
    WMArm9Buf *w9b = wm9buf;           // Notify compiler that buffer is not volatile.
#ifdef WM_DEBUG_CALLBACK
    int     beginVcount = GX_GetVCount();
#endif

    if (err)
        return;                        // The WM_sp_init PXI handler is not ready

    // Invalidates the FIFO command buffer (9 <- 7) cache
    DC_InvalidateRange(w9b->fifo7to9, WM_FIFO_BUF_SIZE);
    if (!w9b->scanOnlyFlag)
    {
        DC_InvalidateRange(w9b->status, WM_STATUS_BUF_SIZE);
    }

    // Checks if the response buffer is a different buffer from fifo7to9
    if ((u32)callback_p != (u32)(w9b->fifo7to9))
    {
        DC_InvalidateRange(callback_p, WM_FIFO_BUF_SIZE);
    }


    if (callback_p->apiid >= WM_NUM_OF_CALLBACK)
    {
        if (callback_p->apiid == WM_APIID_INDICATION)
        {
#if SDK_VERSION_WL >= 21000
            if (callback_p->errcode == WM_ERRCODE_FLASH_ERROR)
            {
                // ARM9 freeze
                OS_Terminate();        // It does not return.
            }
#endif
            if (w9b->indCallback != NULL)
            {
                w9b->indCallback((void *)callback_p);
            }
        }
        else if (callback_p->apiid == WM_APIID_PORT_RECV)
        {
            // Processes the callback according to the port number
            WMPortRecvCallback *cb_Port = (WMPortRecvCallback *)callback_p;
            SDK_ASSERT(cb_Port->port < 16);
            // Make notification if a callback has been registered.
            if (w9b->portCallbackTable[cb_Port->port] != NULL)
            {
                cb_Port->arg = w9b->portCallbackArgument[cb_Port->port];
                cb_Port->connectedAidBitmap = (u16)w9b->connectedAidBitmap;
                DC_InvalidateRange(cb_Port->recvBuf, w9b->status->mp_recvBufSize);      // To Do: Too much invalidation
                (w9b->portCallbackTable[cb_Port->port]) ((void *)cb_Port);
            }
#ifdef WM_DEBUG
            else
            {
                WM_DPRINTF("Warning: no callback function on port %d { %04x %04x ... }\n",
                           cb_Port->port, cb_Port->data[0], cb_Port->data[1]);
            }
#endif
        }
        else if (callback_p->apiid == WM_APIID_PORT_SEND)
        {
            // Does the callback specified when the data is set
            WMPortSendCallback *cb_Port = (WMPortSendCallback *)callback_p;
            callback_p->apiid = WM_APIID_SET_MP_DATA;   // Camouflage
            if (cb_Port->callback != NULL)
            {
                (cb_Port->callback) ((void *)cb_Port);
            }
        }
        else
        {
            OS_Printf("ARM9: no callback function\n");

        }
    }
    else
    {
        // Invalidates the receiving buffer cache
        // When PORT_RECV is called after MPEND_IND and MP_IND,
        // the InvalidateRange is already set, so there is no need to be concerned with the cache.
        if (callback_p->apiid == WM_APIID_START_MP)
        {
            WMStartMPCallback *cb_StartMP = (WMStartMPCallback *)callback_p;
            if (cb_StartMP->state == WM_STATECODE_MPEND_IND
                || cb_StartMP->state == WM_STATECODE_MP_IND)
            {
                if (cb_StartMP->errcode == WM_ERRCODE_SUCCESS)
                {
                    DC_InvalidateRange(cb_StartMP->recvBuf, w9b->status->mp_recvBufSize);
                }
            }
        }

        // WM_End post-processing
        if (callback_p->apiid == WM_APIID_END)
        {
            if (callback_p->errcode == WM_ERRCODE_SUCCESS)
            {
                WMCallbackFunc cb = w9b->CallbackTable[callback_p->apiid];
                // finish WM library
                (void)WM_Finish();
                if (cb != NULL)
                {
                    cb((void *)callback_p);
                }

                WM_DLOGF_CALLBACK("Cb[%x]", callback_p->apiid);

                return;
            }
        }

#if 0
        // Post-processing for WM_Reset, WM_Disconnect, and WM_EndParent.
        if (callback_p->apiid == WM_APIID_RESET
            || callback_p->apiid == WM_APIID_DISCONNECT || callback_p->apiid == WM_APIID_END_PARENT)
        {
            if (w9b->connectedAidBitmap != 0)
            {
                WM_WARNING("connectedAidBitmap should be 0, but %04x", w9b->connectedAidBitmap);
            }
            w9b->myAid = 0;
            w9b->connectedAidBitmap = 0;
        }
#endif

        // callback process according to apiid (does nothing if callback not set (NULL))
        if (NULL != w9b->CallbackTable[callback_p->apiid])
        {
            (w9b->CallbackTable[callback_p->apiid]) ((void *)callback_p);
            /* If WM_Finish was called in user callback and working memory is clear, end here.*/
            if (!wmInitialized)
            {
                return;
            }
        }
        // Post-processing
        // Process messages so that the callback for the port can also be notified regarding connections/disconnections.
        if (callback_p->apiid == WM_APIID_START_PARENT
            || callback_p->apiid == WM_APIID_START_CONNECT)
        {
            u16     state, aid, myAid, reason;
            u8     *macAddress;
            u8     *ssid;
            u16     parentSize, childSize;

            if (callback_p->apiid == WM_APIID_START_PARENT)
            {
                WMStartParentCallback *cb = (WMStartParentCallback *)callback_p;
                state = cb->state;
                aid = cb->aid;
                myAid = 0;
                macAddress = cb->macAddress;
                ssid = cb->ssid;
                reason = cb->reason;
                parentSize = cb->parentSize;
                childSize = cb->childSize;
            }
            else if (callback_p->apiid == WM_APIID_START_CONNECT)
            {
                WMStartConnectCallback *cb = (WMStartConnectCallback *)callback_p;
                state = cb->state;
                aid = 0;
                myAid = cb->aid;
                macAddress = cb->macAddress;
                ssid = NULL;
                reason = cb->reason;
                parentSize = cb->parentSize;
                childSize = cb->childSize;
            }
            if (state == WM_STATECODE_CONNECTED ||
                state == WM_STATECODE_DISCONNECTED
                || state == WM_STATECODE_DISCONNECTED_FROM_MYSELF)
            {
                // Since this processing is inside the interrupt handler, there's no problem if the type is static.
                static WMPortRecvCallback cb_Port;
                u16     iPort;

                // Change the connection status being managed on the WM9 side.
                if (state == WM_STATECODE_CONNECTED)
                {
#ifdef WM_DEBUG
                    if (w9b->connectedAidBitmap & (1 << aid))
                    {
                        WM_DPRINTF("Warning: someone is connecting to connected aid: %d (%04x)",
                                   aid, w9b->connectedAidBitmap);
                    }
#endif
                    WM_DLOGF_AIDBITMAP("aid(%d) connected: %04x -> %04x", aid,
                                       w9b->connectedAidBitmap,
                                       w9b->connectedAidBitmap | (1 << aid));
                    w9b->connectedAidBitmap |= (1 << aid);
                }
                else                   // WM_STATECODE_DISCONNECTED || WM_STATECODE_DISCONNECTED_FROM_MYSELF
                {
#ifdef WM_DEBUG
                    if (!(w9b->connectedAidBitmap & (1 << aid)))
                    {
                        WM_DPRINTF
                            ("Warning: someone is disconnecting to disconnected aid: %d (%04x)",
                             aid, w9b->connectedAidBitmap);
                    }
#endif
                    WM_DLOGF_AIDBITMAP("aid(%d) disconnected: %04x -> %04x", aid,
                                       w9b->connectedAidBitmap,
                                       w9b->connectedAidBitmap & ~(1 << aid));
                    w9b->connectedAidBitmap &= ~(1 << aid);
                }
                w9b->myAid = myAid;

                MI_CpuClear8(&cb_Port, sizeof(WMPortRecvCallback));
                cb_Port.apiid = WM_APIID_PORT_RECV;
                cb_Port.errcode = WM_ERRCODE_SUCCESS;
                cb_Port.state = state;
                cb_Port.recvBuf = NULL;
                cb_Port.data = NULL;
                cb_Port.length = 0;
                cb_Port.aid = aid;
                cb_Port.myAid = myAid;
                cb_Port.connectedAidBitmap = (u16)w9b->connectedAidBitmap;
                cb_Port.seqNo = 0xffff;
                cb_Port.reason = reason;
                MI_CpuCopy8(macAddress, cb_Port.macAddress, WM_SIZE_MACADDR);
                if (ssid != NULL)
                {
                    MI_CpuCopy16(ssid, cb_Port.ssid, WM_SIZE_CHILD_SSID);
                }
                else
                {
                    MI_CpuClear16(cb_Port.ssid, WM_SIZE_CHILD_SSID);
                }
                cb_Port.maxSendDataSize = (myAid == 0) ? parentSize : childSize;
                cb_Port.maxRecvDataSize = (myAid == 0) ? childSize : parentSize;

                // Notify all ports of connections/disconnections.
                for (iPort = 0; iPort < WM_NUM_OF_PORT; iPort++)
                {
                    cb_Port.port = iPort;
                    if (w9b->portCallbackTable[iPort] != NULL)
                    {
                        cb_Port.arg = w9b->portCallbackArgument[iPort];
                        (w9b->portCallbackTable[iPort]) ((void *)&cb_Port);
                    }
                }
            }
        }
    }
    // The fifo7to9 region has been written with PORT_SEND and PORT_RECV, so
    // the cache is invalidated so as to not be written back.
    DC_InvalidateRange(w9b->fifo7to9, WM_FIFO_BUF_SIZE);
    WmClearFifoRecvFlag();

    // If the response buffer is a different buffer from fifo7to9, camouflage the request reception completion flag.
    if ((u32)callback_p != (u32)(w9b->fifo7to9))
    {
        callback_p->apiid |= WM_API_REQUEST_ACCEPTED;
        DC_StoreRange(callback_p, WM_FIFO_BUF_SIZE);
    }

    WM_DLOGF2_CALLBACK(beginVcount, "[CB](%x)", callback_p->apiid);

    return;
}

/*---------------------------------------------------------------------------*
  Name:         WmClearFifoRecvFlag

  Description:  Notifies WM7 that reference to the FIFO data used in
                the WM7 callback is complete.
                When using FIFO for callback in WM7, edit the next callback after
                waiting for this flag to unlock.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void WmClearFifoRecvFlag(void)
{
    if (OS_GetSystemWork()->wm_callback_control & WM_EXCEPTION_CB_MASK)
    {
        // Lowers the CB exclusion flag
        OS_GetSystemWork()->wm_callback_control &= ~WM_EXCEPTION_CB_MASK;
    }
}

/*---------------------------------------------------------------------------*
  Name:         WMi_DebugPrintSendQueue

  Description:  Print outputs the contents of the port transmission queue.

  Arguments:    queue - Specifies the pointer to the port transmission queue.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WMi_DebugPrintSendQueue(WMPortSendQueue *queue)
{
    WMstatus *status = wm9buf->status;
    WMPortSendQueueData *queueData;
    u16     index;

    DC_InvalidateRange(wm9buf->status, WM_STATUS_BUF_SIZE);     // Invalidates the ARM7 status region cache
    queueData = status->sendQueueData;

    OS_Printf("head = %d, tail = %d, ", queue->head, queue->tail);
    if (queue->tail != WM_SEND_QUEUE_END)
    {
        OS_Printf("%s", (queueData[queue->tail].next == WM_SEND_QUEUE_END) ? "valid" : "invalid");
    }
    OS_Printf("\n");
    for (index = queue->head; index != WM_SEND_QUEUE_END; index = queueData[index].next)
    {
        WMPortSendQueueData *data = &(queueData[index]);

        OS_Printf("queueData[%d] -> %d { port=%d, destBitmap=%x, size=%d } \n", index, data->next,
                  data->port, data->destBitmap, data->size);
    }

}

/*---------------------------------------------------------------------------*
  Name:         WMi_DebugPrintAllSendQueue

  Description:  Print outputs the contents of all the port transmission queues.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WMi_DebugPrintAllSendQueue(void)
{
    WMstatus *status = wm9buf->status;
#if 0
    int     iPrio;

    DC_InvalidateRange(wm9buf->status, WM_STATUS_BUF_SIZE);     // Invalidates the ARM7 status region cache
    for (iPrio = 0; iPrio < WM_PRIORITY_LEVEL; iPrio++)
    {
        OS_Printf("== send queue [%d]\n", iPrio);
        WMi_DebugPrintSendQueue(&status->sendQueue[iPrio]);
    }
    for (iPrio = 0; iPrio < WM_PRIORITY_LEVEL; iPrio++)
    {
        OS_Printf("== ready queue [%d]\n", iPrio);
        WMi_DebugPrintSendQueue(&status->readyQueue[iPrio]);
    }
    OS_Printf("== free queue\n");
    OS_Printf(" head: %d, tail: %d\n", status->sendQueueFreeList.head,
              status->sendQueueFreeList.tail);
//    WMi_DebugPrintSendQueue( &status->sendQueueFreeList );
#else
    DC_InvalidateRange(wm9buf->status, WM_STATUS_BUF_SIZE);     // Invalidates the ARM7 status region cache
    OS_Printf("== ready queue [2]\n");
    OS_Printf(" head: %d, tail: %d\n", status->readyQueue[2].head, status->readyQueue[2].tail);
#endif

}

/*---------------------------------------------------------------------------*
  Name:         WMi_GetStatusAddress

  Description:  Gets the pointer to the status structure managed internally by WM.
                This structure is directly operated by ARM7, so writing by ARM9 is disabled.
                Also, note that the cache line containing the portion to be referenced needs
                to be deleted before referencing the contents.

  Arguments:    None.

  Returns:      const WMStatus* - Returns a pointer to the status structure.
 *---------------------------------------------------------------------------*/
const WMStatus *WMi_GetStatusAddress(void)
{
    // Initialization check
    if (WMi_CheckInitialized() != WM_ERRCODE_SUCCESS)
    {
        return NULL;
    }

    return wm9buf->status;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_CheckMpPacketTimeRequired

  Description:  Confirms that the time needed to send 1 MP communication packet is less than 5,600 microseconds.
                 

  Arguments:    parentSize  -   Size of parent transfer data
                childSize   -   Size of child transfer data
                childs      -   Number of children to communicate with

  Returns:      BOOL        -   If time is within the acceptable range, returns TRUE.
                                If it exceeds 5,600 microseconds, returns FALSE.
 *---------------------------------------------------------------------------*/
BOOL WMi_CheckMpPacketTimeRequired(u16 parentSize, u16 childSize, u8 childs)
{
    s32     mp_time;

    // The time it takes for one MP communication is calculated in units of us
    mp_time = ((                       // --- The parent transmission part ---
                   96                  // Preamble
                   + (24               // 802.11 Header
                      + 4              // TXOP + PollBitmap
                      + 2              // wmHeader
                      + parentSize + 4 // wmFooter( parent )
                      + 4              // FCS
                   ) * 4               // 4us per 1 byte
               ) + (                   // --- The child transmission part ---
                       (10             // SIFS
                        + 96           // Preamble
                        + (24          // 802.11 Header
                           + 2         // wmHeader
                           + childSize + 2      // wmFooter( child )
                           + 4         // FCS
                        ) * 4          // 4us per 1 byte
                        + 6            // time to spare
                       ) * childs) + ( // --- The MP ACK transmission part ---
                                         10     // SIFS
                                         + 96   // Preamble
                                         + (24  // 802.11 Header
                                            + 4 // ACK
                                            + 4 // FCS
                                         ) * 4  // 4us per 1 byte
               ));

    if (mp_time > WM_MAX_MP_PACKET_TIME)
    {
        OS_TWarning
            ("It is required %dus to transfer each MP packet.\nThat should not exceed %dus.\n",
             mp_time, WM_MAX_MP_PACKET_TIME);
        return FALSE;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_IsMP

  Description:  Get current MP communication state.

  Arguments:    None.

  Returns:      TRUE if MP communication state.
 *---------------------------------------------------------------------------*/
BOOL WMi_IsMP(void)
{
    WMStatus *status;
    BOOL    isMP;
    OSIntrMode e;

#ifdef SDK_DEBUG
    // Initialization check
    if (WMi_CheckInitialized() != WM_ERRCODE_SUCCESS)
    {
        return FALSE;
    }
#endif

    // Prohibit interrupts
    e = OS_DisableInterrupts();

    if (wm9buf != NULL)
    {
        status = wm9buf->status;
        DC_InvalidateRange(&(status->mp_flag), 4);
        isMP = status->mp_flag;
    }
    else
    {
        isMP = FALSE;
    }

    // End prohibition of interrupts
    (void)OS_RestoreInterrupts(e);

    return isMP;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetAID

  Description:  Gets current AID.
                Return a valid value only when the state is one of PARENT, MP_PARENT, CHILD, or MP_CHILD.
                 

  Arguments:    None.

  Returns:      AID
 *---------------------------------------------------------------------------*/
u16 WM_GetAID(void)
{
    u16     myAid;
    OSIntrMode e;

#ifdef SDK_DEBUG
    // Initialization check
    if (WMi_CheckInitialized() != WM_ERRCODE_SUCCESS)
    {
        return 0;
    }
#endif

    // Prohibit interrupts
    e = OS_DisableInterrupts();

    if (wm9buf != NULL)
    {
        myAid = wm9buf->myAid;
    }
    else
    {
        myAid = 0;
    }

    // End prohibition of interrupts
    (void)OS_RestoreInterrupts(e);

    return myAid;
}

/*---------------------------------------------------------------------------*
  Name:         WM_GetConnectedAIDs

  Description:  Get currently connected parties in bitmap format.
                Return a valid value only when the state is one of PARENT, MP_PARENT, CHILD, or MP_CHILD.
                 
                Returns 0x0001 in the case children when connected to the parent.

  Arguments:    None.

  Returns:      Bitmap of AIDs of connected parties.
 *---------------------------------------------------------------------------*/
u16 WM_GetConnectedAIDs(void)
{
    u32     connectedAidBitmap;
    OSIntrMode e;

#ifdef SDK_DEBUG
    // Initialization check
    if (WMi_CheckInitialized() != WM_ERRCODE_SUCCESS)
    {
        return 0;
    }
#endif

    // Prohibit interrupts
    e = OS_DisableInterrupts();

    if (wm9buf != NULL)
    {
        connectedAidBitmap = wm9buf->connectedAidBitmap;
    }
    else
    {
        connectedAidBitmap = 0;
    }

    // End prohibition of interrupts
    (void)OS_RestoreInterrupts(e);

#ifdef WM_DEBUG
    if (WMi_CheckStateEx(4, WM_STATE_PARENT, WM_STATE_CHILD, WM_STATE_MP_PARENT, WM_STATE_MP_CHILD)
        != WM_ERRCODE_SUCCESS && connectedAidBitmap != 0)
    {
        WM_WARNING("connectedAidBitmap should be 0, but %04x", connectedAidBitmap);
    }
#endif

    return (u16)connectedAidBitmap;
}

/*---------------------------------------------------------------------------*
  Name:         WMi_GetMPReadyAIDs

  Description:  Gets a list in bitmap format of the AIDs of other parties which can receive MP from among currently connected parties.
                 
                Return a valid value only when the state is one of PARENT, MP_PARENT, CHILD, or MP_CHILD.
                 
                Returns 0x0001 in the case children when connected to the parent.

  Arguments:    None.

  Returns:      Bitmap of the AID of another party for which MP is started.
 *---------------------------------------------------------------------------*/
u16 WMi_GetMPReadyAIDs(void)
{
    WMStatus *status;
    u16     mpReadyAidBitmap;
    OSIntrMode e;

#ifdef SDK_DEBUG
    // Initialization check
    if (WMi_CheckInitialized() != WM_ERRCODE_SUCCESS)
    {
        return FALSE;
    }
#endif

    // Prohibit interrupts
    e = OS_DisableInterrupts();

    if (wm9buf != NULL)
    {
        status = wm9buf->status;
        DC_InvalidateRange(&(status->mp_readyBitmap), 2);
        mpReadyAidBitmap = status->mp_readyBitmap;
    }
    else
    {
        mpReadyAidBitmap = FALSE;
    }

    // End prohibition of interrupts
    (void)OS_RestoreInterrupts(e);

    return mpReadyAidBitmap;
}


/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
