/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_wm.c

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_wm.c,v $
  Revision 1.8  10/28/2005 11:23:41 AM  seiki_masashi
  Supported addition of WM_STATECODE_DISCONNECTED_FROM_MYSELF.
  Supported addition of WM_STATECODE_PORT_INIT.

  Revision 1.7  02/28/2005 05:26:24 AM  yosizaki
  do-indent.

  Revision 1.6  02/21/2005 12:39:34 AM  yosizaki
  replace prefix MBw to MBi.

  Revision 1.5  01/11/2005 07:41:13 AM  takano_makoto
  fix copyright header.

  Revision 1.4  01/07/2005 02:56:25 AM  takano_makoto
  Made correction to ignore the error notification that does not have any problems with callback of WM_StartMP

  Revision 1.3  11/24/2004 01:00:02 PM  takano_makoto
  Added error processing.

  Revision 1.2  11/23/2004 11:50:54 PM  takano_makoto
  Removed Warning.

  Revision 1.1  11/22/2004 12:38:30 PM  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "mb_common.h"
#include "mb_wm.h"
#include "mb_child.h"
#include "mb_wm_base.h"
#include "mb_block.h"

//===========================================================================
// Prototype Declaration
//===========================================================================

static BOOL IsSendEnabled(void);
static void MBi_WMStateOutStartConnect(void *arg);
static void ChildStateOutStartMP(void *arg);
static void ChildPortCallback(void *arg);
static void StateOutMPSendToParent(void *arg);

static void MBi_WMStateOutStartConnect(void *arg);

static void MBi_WMStateOutEndMP(void *arg);

static void MBi_WMStateOutDisconnect(void *arg);
static void MBi_WMStateInDisconnect(void);

static void MBi_WMStateOutReset(void *arg);

static void MBi_WMSendCallback(u16 type, void *arg);
static void MBi_WMErrorCallback(u16 apiid, u16 error_code);
static void MBi_WMApiErrorCallback(u16 apiid, u16 error_code);


//===========================================================================
// Variable Declaration
//===========================================================================

static MBWMWork *wmWork = NULL;


//===========================================================================
// Function Definitions
//===========================================================================

/*---------------------------------------------------------------------------*
  Name:         MBi_WMSetBuffer

  Description:  Sets the buffer that MB_WM uses for MP communication.
                You must provide the size of MBWMWork only.

  Arguments:    buf     pointer to region used as work buffer

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMSetBuffer(void *buf)
{
    SDK_NULL_ASSERT(buf);
    SDK_ASSERT(((u32)buf & 0x1f) == 0); // whether or not 32 byte aligned

    wmWork = (MBWMWork *) buf;
    wmWork->start_mp_busy = 0;         // prevent multiple calls of parent StartMP
    wmWork->mpStarted = 0;
    wmWork->child_bitmap = 0;
    wmWork->mpBusy = 0;
    wmWork->endReq = 0;
    wmWork->sendBuf = NULL;
    wmWork->recvBuf = NULL;
    wmWork->mpCallback = NULL;
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMSetCallback

  Description:  Sets callback.

  Arguments:    callback    callback function

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMSetCallback(MBWMCallbackFunc callback)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    wmWork->mpCallback = callback;

    (void)OS_RestoreInterrupts(enabled);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStartConnect

  Description:  Begins connection to parent.

  Arguments:    bssDesc   BssDesc of parent to connect

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMStartConnect(WMBssDesc *bssDesc)
{
    WMErrCode result;

    wmWork->mpStarted = 0;
    wmWork->endReq = 0;

    wmWork->sendBufSize = (u16)WM_SIZE_MP_CHILD_SEND_BUFFER(bssDesc->gameInfo.childMaxSize, FALSE);
    wmWork->recvBufSize =
        (u16)WM_SIZE_MP_CHILD_RECEIVE_BUFFER(bssDesc->gameInfo.parentMaxSize, FALSE);
    wmWork->pSendLen = bssDesc->gameInfo.parentMaxSize;
    wmWork->pRecvLen = bssDesc->gameInfo.childMaxSize;
    wmWork->blockSizeMax = (u16)MB_COMM_CALC_BLOCK_SIZE(wmWork->pSendLen);
    MBi_SetChildMPMaxSize(wmWork->pRecvLen);

    result = WM_StartConnect(MBi_WMStateOutStartConnect, bssDesc, NULL);

    if (result != WM_ERRCODE_OPERATING)
    {
        MBi_WMSendCallback(MB_CALLBACK_CONNECT_FAILED, NULL);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateOutStartConnect

  Description:  parent connection callback

  Arguments:    arg     WM_StartConnect callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateOutStartConnect(void *arg)
{
    WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        MBi_WMSendCallback(MB_CALLBACK_CONNECT_FAILED, arg);
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_BEACON_LOST:
        break;
    case WM_STATECODE_CONNECT_START:
        break;
    case WM_STATECODE_DISCONNECTED:
        MBi_WMSendCallback(MB_CALLBACK_DISCONNECTED_FROM_PARENT, NULL);
        break;
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        // Does nothing if the unit disconnects from itself
        break;
    case WM_STATECODE_CONNECTED:
        // When authentication is complete.
        MBi_WMSendCallback(MB_CALLBACK_CONNECTED_TO_PARENT, arg);
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_ChildStartMP

  Description:  start MP communication

  Arguments:    sendBuf     pointer to region to set as send buffer
                recvBuf     pointer to region to set as receive buffer

  Returns:      Error code, normally WM_ERRCODE_OPERATING.
 *---------------------------------------------------------------------------*/
void MBi_ChildStartMP(u16 *sendBuf, u16 *recvBuf)
{
    WMErrCode result;

    wmWork->sendBuf = (u32 *)sendBuf;
    wmWork->recvBuf = (u32 *)recvBuf;

    result = WM_SetPortCallback(WM_PORT_BT, ChildPortCallback, NULL);
    if (result != WM_ERRCODE_SUCCESS)
    {
        MBi_WMApiErrorCallback(WM_APIID_START_MP, result);
        return;
    }

    result = WM_StartMPEx(ChildStateOutStartMP, recvBuf, wmWork->recvBufSize, sendBuf, wmWork->sendBufSize, 1,  // mpFreq
                          0,           // defaultRetryCount 
                          FALSE,       // minPollBmpMode
                          FALSE,       // singlePacketMode
                          TRUE,        // fixFrameMode
                          TRUE);       // ignoreFatalError

    if (result != WM_ERRCODE_OPERATING)
    {
        MBi_WMApiErrorCallback(WM_APIID_START_MP, result);
    }
}

/*---------------------------------------------------------------------------*
  Name:         ChildStateOutStartMP

  Description:  child WM_StartMPEx callback function

  Arguments:    arg     WM_StartMP callback argument

  Returns:      Error code, normally WM_ERRCODE_OPERATING.
 *---------------------------------------------------------------------------*/
static void ChildStateOutStartMP(void *arg)
{
    WMStartMPCallback *cb = (WMStartMPCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        // End if it is the error notification that does not require error handling
        if (cb->errcode == WM_ERRCODE_SEND_FAILED)
        {
            return;
        }
        else if (cb->errcode == WM_ERRCODE_TIMEOUT)
        {
            return;
        }
        else if (cb->errcode == WM_ERRCODE_INVALID_POLLBITMAP)
        {
            return;
        }

        MBi_WMErrorCallback(cb->apiid, cb->errcode);
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_MP_START:
        wmWork->mpStarted = 1;         // Set flag indicating that MP has started MP
        wmWork->mpBusy = 0;
        wmWork->child_bitmap = 0;
        MBi_WMSendCallback(MB_CALLBACK_MP_STARTED, NULL);
        {
            // MP send-enabled callback
            MBi_WMSendCallback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
        }
        break;

    case WM_STATECODE_MP_IND:
        // None.
        break;

    case WM_STATECODE_MPACK_IND:
        // None.
        break;

    case WM_STATECODE_MPEND_IND:      // only occurs in parent
    default:
        MBi_WMErrorCallback(cb->apiid, WM_ERRCODE_FAILED);
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         MBi_WMDisconnect

  Description:  Child MP disconnect processing. Disconnects from parent after WM_EndMP completes.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMDisconnect(void)
{
    WMErrCode result;

    wmWork->endReq = 1;

    result = WM_EndMP(MBi_WMStateOutEndMP);

    if (result != WM_ERRCODE_OPERATING)
    {
        wmWork->endReq = 0;
        MBi_WMApiErrorCallback(WM_APIID_END_MP, result);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateOutEndMP

  Description:  The callback function for WM_EndMP

  Arguments:    arg     The callback argument for WM_EndMP

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateOutEndMP(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        wmWork->endReq = 0;
        MBi_WMErrorCallback(cb->apiid, cb->errcode);
        return;
    }

    wmWork->mpStarted = 0;
    MBi_WMStateInDisconnect();
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateInDisconnect

  Description:  Disconnects child from parent and moves to the IDLE state.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateInDisconnect(void)
{
    WMErrCode result;

    result = WM_Disconnect(MBi_WMStateOutDisconnect, 0);

    if (result != WM_ERRCODE_OPERATING)
    {
        wmWork->endReq = 0;
        MBi_WMApiErrorCallback(WM_APIID_DISCONNECT, result);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateInDisconnect

  Description:  WM_Disconnect callback argument

  Arguments:    arg  WM_Disconnect callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateOutDisconnect(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    wmWork->endReq = 0;
    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        MBi_WMErrorCallback(cb->apiid, cb->errcode);
        return;
    }
    MBi_WMSendCallback(MB_CALLBACK_DISCONNECT_COMPLETE, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMReset

  Description:  Resets that of the child, and moves it to IDLE state.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMReset(void)
{
    WMErrCode result;

    result = WM_Reset(MBi_WMStateOutReset);
    if (result != WM_ERRCODE_OPERATING)
    {
        MBi_WMApiErrorCallback(WM_APIID_RESET, result);
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMStateOutReset

  Description:  Resets that of the child, and moves it to IDLE state. 

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MBi_WMStateOutReset(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        MBi_WMErrorCallback(cb->apiid, cb->errcode);
        return;
    }
    // Reset sets to the idling (standby) state without starting the next state.
    MBi_WMSendCallback(MB_CALLBACK_DISCONNECT_COMPLETE, NULL);
}


/*
 * Check if MP transmissions are possible.
   
   In order to resetting of MP after SetMP and before the callback is returned,
   added a component to determine mpBusy flag that is on during SetMP execution.
 
 */
/*---------------------------------------------------------------------------*
  Name:         IsSendEnabled

  Description:  Function that determines whether or not it is OK to set new MP data at present.
                In order to resetting of MP after SetMP and before the callback is returned,
                added a component to determine mpBusy flag that is on during SetMP execution.

  Arguments:    None.

  Returns:      TRUE if it is OK to set new data.
                Otherwise returns FALSE.
 *---------------------------------------------------------------------------*/
static BOOL IsSendEnabled(void)
{
    return (wmWork->mpStarted == 1) && (wmWork->mpBusy == 0) && (wmWork->endReq == 0);
}


/*---------------------------------------------------------------------------*
  Name:         ChildPortCallback

  Description:  child MP port callback function

  Arguments:    arg     MP communication port callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ChildPortCallback(void *arg)
{
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS)
    {
        return;
    }

    switch (cb->state)
    {
    case WM_STATECODE_PORT_RECV:
        // Indicate reception of data
        MBi_WMSendCallback(MB_CALLBACK_MP_CHILD_RECV, cb);
        break;
    case WM_STATECODE_CONNECTED:
        // Connection notification
        break;
    case WM_STATECODE_PORT_INIT:
    case WM_STATECODE_DISCONNECTED:
    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         MBi_MPSendToParent

  Description:  Sends contents of buffer to parent.

  Arguments:    body_len  data size
                pollbmp  poll bitmap of the other party (unrelated when child)
                sendbuf  pointer to send data
  Returns:      If send processing started successfully, returns WM_ERRCODE_OPERATING.
                If fails, some other callback is returned.
 *---------------------------------------------------------------------------*/
WMErrCode MBi_MPSendToParent(u32 body_len, u16 pollbmp, u32 *sendbuf)
{
    WMErrCode result;

    // 32 byte align check
    SDK_ASSERT(((u32)sendbuf & 0x1F) == 0);

    DC_FlushRange(sendbuf, sizeof(body_len));
    DC_WaitWriteBufferEmpty();

    if (!IsSendEnabled())
    {
        return WM_ERRCODE_FAILED;
    }

    result = WM_SetMPDataToPort(StateOutMPSendToParent,
                                (u16 *)sendbuf,
                                (u16)body_len, pollbmp, WM_PORT_BT, WM_PRIORITY_LOW);
    if (result != WM_ERRCODE_OPERATING)
    {
        return result;
    }

    wmWork->mpBusy = 1;
    return WM_ERRCODE_OPERATING;
}


/*---------------------------------------------------------------------------*
  Name:         StateOutMPSendToParent

  Description:  MP send completion notification callback

  Arguments:    arg     WM_SetMPDataToPort callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void StateOutMPSendToParent(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    wmWork->mpBusy = 0;
    if (cb->errcode == WM_ERRCODE_SUCCESS)
    {
        MBi_WMSendCallback(MB_CALLBACK_MP_CHILD_SENT, arg);
    }
    else if (cb->errcode == WM_ERRCODE_TIMEOUT)
    {
        MBi_WMSendCallback(MB_CALLBACK_MP_CHILD_SENT_TIMEOUT, arg);
    }
    else
    {
        MBi_WMSendCallback(MB_CALLBACK_MP_CHILD_SENT_ERR, arg);
    }
    // Next transmission is allowed
    MBi_WMSendCallback(MB_CALLBACK_MP_SEND_ENABLE, NULL);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMSendCallback

  Description:  Does callback notification in WM layer.

  Arguments:    type        callback type
                arg      Callback argument

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBi_WMSendCallback(u16 type, void *arg)
{
    if (wmWork->mpCallback == NULL)
    {
        return;
    }
    wmWork->mpCallback(type, arg);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_WMErrorCallback

  Description:  Does error notification in WM layer.

  Arguments:    apiid       WM_APIID that was the case
                error_code  error code

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBi_WMErrorCallback(u16 apiid, u16 error_code)
{
    MBErrorCallback arg;

    if (wmWork->mpCallback == NULL)
    {
        return;
    }

    arg.apiid = apiid;
    arg.errcode = error_code;

    wmWork->mpCallback(MB_CALLBACK_ERROR, &arg);
}

/*---------------------------------------------------------------------------*
  Name:         MBi_WMApiErrorCallback

  Description:  Notifies when an error occurs with return value of WM API call.

  Arguments:    apiid       WM_APIID that was the case
                error_code  error code

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void MBi_WMApiErrorCallback(u16 apiid, u16 error_code)
{
    MBErrorCallback arg;

    if (wmWork->mpCallback == NULL)
    {
        return;
    }

    arg.apiid = apiid;
    arg.errcode = error_code;

    wmWork->mpCallback(MB_CALLBACK_API_ERROR, &arg);
}


/*---------------------------------------------------------------------------*
  Name:         MBi_WMApiErrorCallback

  Description:  Notifies when an error occurs with return value of WM API call.

  Arguments:    apiid       WM_APIID that was the case
                error_code  error code

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MBi_WMClearCallback(void)
{
    (void)WM_SetPortCallback(WM_PORT_BT, NULL, NULL);
}
