/*---------------------------------------------------------------------------*
Project: NitroWiFi - WCM - libraries
File: cpsif.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: cpsif.c,v $
Revision 1.7  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.6  09/01/2005 23:45:02  terui
revised to disable argument unused warning in the WcmCpsifKACallback function

Revision 1.5  09/01/2005 13:05:06  terui
added Keep Alive NULL packet transmission functionality
added functionality to reset the Keep Alive packet transmission alarm while sending data

Revision 1.4  08/10/2005 10:39:24  adachi_hiroaki
fixed error message arguments

Revision 1.3  08/08/2005 11:15:49  terui
added WCM_GetApEssid function

Revision 1.2  07/11/2005 11:42:04  yasu
added padding to eliminate the padding warning

Revision 1.1  07/07/2005 10:45:52  terui
new additions

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "wcm_private.h"
#include "wcm_cpsif.h"

/*---------------------------------------------------------------------------*
constant definitions
 *---------------------------------------------------------------------------*/
#if WCM_DEBUG

// warning text template
static const char   cpsifWarningText_NotInit[] = { "WCM library is not initialized yet.\n" };
static const char   cpsifWarningText_IllegalParam[] = { "Illegal parameter ( %s )\n" };
static const char   cpsifWarningText_InIrqMode[] = { "Processor mode is IRQ mode now.\n" };

// alert text template
static const char   cpsifReportText_WmSyncError[] = { "%s failed syncronously. ( ERRCODE: 0x%02x )\n" };
static const char   cpsifReportText_WmAsyncError[] = { "%s failed asyncronously. ( ERRCODE: 0x%02x )\n" };
#endif

#define WCMCPSIF_DUMMY_IRQ_THREAD   ((OSThread*)OS_IrqHandler)

/*---------------------------------------------------------------------------*
structure definitions
 *---------------------------------------------------------------------------*/

// internal status management structures
typedef struct WCMCpsifWork
{
    u8  initialized;                // for confirming work variable initialization
    u8  reserved[3];                // padding (when OSThreadQueue is 4 bytes or more)
    OSThreadQueue   sendQ;          // thread queue for  DCF transmission complete hold block
    OSMutex         sendMutex;      // Mutex for multiple thread send exception control
    WMErrCode       sendResult;     // DCF transmission asynchronous process results backup area
    WCMRecvInd      recvCallback;   // DCF receipt callback target backup area
} WCMCpsifWork;

/*---------------------------------------------------------------------------*
internal variable definitions
 *---------------------------------------------------------------------------*/

/* 
* If ThreadQueue and Mutex, etc., are released when the WCM library ends,
* the problematic variables will become static variables. Once these variables are initialized,
they won't be released or initialized twice.
 */
static WCMCpsifWork wcmCpsifw;

/*---------------------------------------------------------------------------*
internal function prototype
 *---------------------------------------------------------------------------*/
static void         WcmCpsifWmCallback(void* arg);
static void         WcmCpsifKACallback(void* arg);
static BOOL         WcmCpsifTryLockMutexInIRQ(OSMutex* mutex);
static void         WcmCpsifUnlockMutexInIRQ(OSMutex* mutex);

/*---------------------------------------------------------------------------*
Name: WCMi_InitCpsif

Description: Initializes the work variables for the portion that will become the
interface with the CPS library. Once initialized they won't be re-initialized.

Arguments: None.

Returns: None.
 *---------------------------------------------------------------------------*/
void WCMi_InitCpsif(void)
{
    if (wcmCpsifw.initialized == 0)
    {
        wcmCpsifw.initialized = 1;
        wcmCpsifw.sendResult = WM_ERRCODE_SUCCESS;

        /* not initialized as wcmCpsifw.recvCallback may have been changed */
        OS_InitThreadQueue(&(wcmCpsifw.sendQ));
        OS_InitMutex(&(wcmCpsifw.sendMutex));
    }
}

/*---------------------------------------------------------------------------*
Name: WCMi_CpsifRecvCallback

Description: Function called when the wireless driver receives a DCF frame.
Called within the interruption service, calls the CPS receipt
callback function.

Arguments: recv    -   A pointer to the received DCF frame structure is passed.

Returns: None.
 *---------------------------------------------------------------------------*/
void WCMi_CpsifRecvCallback(WMDcfRecvBuf* recv)
{
    if (wcmCpsifw.recvCallback != NULL)
    {
        wcmCpsifw.recvCallback((u8 *) (recv->srcAdrs), (u8 *) (recv->destAdrs), (u8 *) (recv->data), (s32) (recv->length));
    }
}

/*---------------------------------------------------------------------------*
Name: WCMi_CpsifSendNullPacket

Description: Sends the Keep Alive NULL packet.
Nothing is performed is other DCF packet is being sent.

Arguments: None.

Returns: None.
 *---------------------------------------------------------------------------*/
void WCMi_CpsifSendNullPacket(void)
{
    volatile WCMWork*   w = WCMi_GetSystemWork();

// checks the internal state of the WCM library
    if (w == NULL)
    {
        return;
    }

    if ((w->phase != WCM_PHASE_DCF) || (w->resetting == WCM_RESETTING_ON))
    {
        return;
    }

// exception lock
    if (FALSE == WcmCpsifTryLockMutexInIRQ(&(wcmCpsifw.sendMutex)))
    {
        return;
    }

// issues send request
    if (WM_ERRCODE_OPERATING != WM_SetDCFData(WcmCpsifKACallback, (const u8*)w->bssDesc.bssid, (const u16*)(w->sendBuf),
        0))
    {
        WcmCpsifUnlockMutexInIRQ(&(wcmCpsifw.sendMutex));
    }
}

/*---------------------------------------------------------------------------*
Name: WCM_GetApMacAddress

Description: Gets the MAC address for the AP to which the wireless driver is connected.
NOTICE: The contents of the buffer indicated by the pointer gotten in this function
can change as a result of interrupts, etc., and the buffer itself may
be released; be aware of these possibilities. We recommend that you call the function while interrupts are disabled
and that you back up the content to another buffer.

Arguments: None.

Returns: u8*     -   Returns a pointer to the MAC address for the connected AP.
                             Returns NULL if there is no connection.
 *---------------------------------------------------------------------------*/
u8* WCM_GetApMacAddress(void)
{
    u8*         mac = NULL;
    WCMWork*    w = WCMi_GetSystemWork();
    OSIntrMode  e = OS_DisableInterrupts();

// initialization confirmation
    if (w != NULL)
    {
// confirms the WCM library state
        if ((w->phase == WCM_PHASE_DCF) && (w->resetting == WCM_RESETTING_OFF))
        {
            /* connection established by the wireless driver */
            mac = w->bssDesc.bssid;
        }
    }

    (void)OS_RestoreInterrupts(e);
    return mac;
}

/*---------------------------------------------------------------------------*
Name: WCM_GetApEssid

Description: Gets the ESS-ID for the AP to which the wireless driver is connected.
NOTICE: The contents of the buffer indicated by the pointer gotten in this function
can change as a result of interrupts, etc., and the buffer itself may
be released; be aware of these possibilities. We recommend that you call the function while interrupts are disabled
and that you back up the content to another buffer.

Arguments: length  -   Specifies a pointer to the u16-type variable that gets the length
of the ESS-ID. 0 is stored is the connection has not been established.
If it's not necessary to get the length, this argument may
be specified as NULL.

Returns: u8*     -   Returns a pointer to the ESS-ID data row for the connected
AP. Returns NULL if there is no connection.
 *---------------------------------------------------------------------------*/
u8* WCM_GetApEssid(u16* length)
{
    u8*         essid = NULL;
    u16         len = 0;
    WCMWork*    w = WCMi_GetSystemWork();
    OSIntrMode  e = OS_DisableInterrupts();

// initialization confirmation
    if (w != NULL)
    {
// confirms the WCM library state
        if ((w->phase == WCM_PHASE_DCF) && (w->resetting == WCM_RESETTING_OFF))
        {
            /* connection established by the wireless driver */
            essid = &(w->bssDesc.ssid[0]);
            len = w->bssDesc.ssidLength;
        }
    }

    (void)OS_RestoreInterrupts(e);
    if (length != NULL)
    {
        *length = len;
    }

    return essid;
}

/*---------------------------------------------------------------------------*
Name: WCM_SetRecvDCFCallback

Description: Registers the CPS receipt callback function.
The receipt callback function registered here is called every time
the wireless driver receives a DCF frame.
NOTICE: When the callback function registered here is within the overlay,
be aware that you need to overwrite the registered callback function
with NULL and clear it before the jump target disappears from memory.

Arguments: callback    -   Specifies a DCF receipt callback function.
Specify NULL to stop receipt notifications.

Returns: None.
 *---------------------------------------------------------------------------*/
void WCM_SetRecvDCFCallback(WCMRecvInd callback)
{
    OSIntrMode  e = OS_DisableInterrupts();

    wcmCpsifw.recvCallback = callback;

    (void)OS_RestoreInterrupts(e);
}

/*---------------------------------------------------------------------------*
Name: WCM_SendDCFData

Description: Indicates the sending of a DCF frame to the wireless driver. Internally blocked and
waited on until the DCF frame is sent or fails. In addition, fails
if the wireless connection state cannot send DCF frames.
NOTICE: It is assumed that this can be called asynchronously from multiple threads, but
be aware that it is assumed it won't be called from within the interrupt service.

Arguments: dstAddr -   Specifies the MAC address the DCF frame will go to.
buf     -   Specifies a pointer to the data to be sent.
Be aware that you will need to maintain the content
of the buffer until returning from this function.
len     -   Specifies the length of the data to be sent in bytes.

Returns: s32     -   Returns the data length if the send was successful.
Returns a negative number if failed.
 *---------------------------------------------------------------------------*/
s32 WCM_SendDCFData(const u8* dstAddr, const u8* buf, s32 len)
{
    OSIntrMode          e = OS_DisableInterrupts();
    volatile WCMWork*   w = WCMi_GetSystemWork();

// confirms if initialized
    if (w == NULL)
    {
        WCMi_Warning(cpsifWarningText_NotInit);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // WCM library is not initialized
    }

#if WCM_DEBUG
// confirms parameters
    if ((dstAddr == NULL) || (buf == NULL))
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "dstAddr or buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // pointer is NULL
    }

    if (len <= 0)
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // len is 0 or less
    }

    if (len > WCM_ROUNDUP32(WM_DCF_MAX_SIZE))
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "len");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // len exceeds the size
    }

    if ((u32) buf & 0x01)
    {
        WCMi_Warning(cpsifWarningText_IllegalParam, "buf");
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_ILLEGAL_PARAM;      // buf is an odd-numbered address
    }

// confirms whether within an interrupt service
    if (OS_GetProcMode() == HW_PSR_IRQ_MODE)
    {
        WCMi_Warning(cpsifWarningText_InIrqMode);
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_IN_IRQ_MODE;        // calls in IRQ mode are not allowed
    }
#endif

// if another thread is in an exception send block, wait until cleared
    OS_LockMutex(&(wcmCpsifw.sendMutex));

    /*
* The WCM library state is unclear upon returning from the block state.
* As you can't send unless connected to AP, you need to confirm the WCM
* library state here.
     */
    w = WCMi_GetSystemWork();
    if (w == NULL)
    {
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_BEFORE_INIT;        // if the WCM library ended during sleep mode
    }

    if ((w->phase != WCM_PHASE_DCF) || (w->resetting == WCM_RESETTING_ON))
    {
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_NO_CONNECTION;      // If the AP connection is not established when the send was attempted
    }

    /*
* The data to be sent is temporarily copied to a cache-safe buffer.
* As send requests from multiple threads are processed as exceptions, it should be
* enough cache-safe buffer to copy the portion to be sent alone.
     */
    MI_CpuCopy8(buf, (void*)(w->sendBuf), (u32) len);

// issues send request
    {
        WMErrCode   wmResult;

        wmResult = WM_SetDCFData(WcmCpsifWmCallback, (const u8*)dstAddr, (const u16*)(w->sendBuf), (u16) len);
        switch (wmResult)
        {
        case WM_ERRCODE_OPERATING:
            break;

        case WM_ERRCODE_ILLEGAL_STATE:
        case WM_ERRCODE_INVALID_PARAM:
        case WM_ERRCODE_FIFO_ERROR:
        default:
// if the send request fails
            WCMi_Printf(cpsifReportText_WmSyncError, "WM_SetDCFData", wmResult);
            OS_UnlockMutex(&(wcmCpsifw.sendMutex));
            (void)OS_RestoreInterrupts(e);
            return WCM_CPSIF_RESULT_SEND_FAILURE;   // DCF send failed
        }
    }

// send complete wait block
    OS_SleepThread(&(wcmCpsifw.sendQ));

    /*
* The WCM library state is unclear upon returning from the block state.
* Send results are kept even if the WCM library ends so
* there is no need to confirm the WCM library state.
     */
    switch (wcmCpsifw.sendResult)
    {
    case WM_ERRCODE_SUCCESS:
        break;

    case WM_ERRCODE_FAILED:
    default:
// if the send fails
        WCMi_Printf(cpsifReportText_WmAsyncError, "WCM_SendDCFData", wcmCpsifw.sendResult);
        OS_UnlockMutex(&(wcmCpsifw.sendMutex));
        (void)OS_RestoreInterrupts(e);
        return WCM_CPSIF_RESULT_SEND_FAILURE;       // DCF send failed
    }

// normal end
    OS_UnlockMutex(&(wcmCpsifw.sendMutex));
    (void)OS_RestoreInterrupts(e);
    return len;
}

/*---------------------------------------------------------------------------*
Name: WcmCpsifWmCallback

Description: Callback function that accepts asynchronous responses from the wireless driver
in response to DCF frame send requests.

Arguments: arg -   The address of the buffer where the response parameters
from the wireless driver are stored is passed.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmCpsifWmCallback(void* arg)
{
    WMCallback*     cb = (WMCallback*)arg;

    switch (cb->apiid)
    {
    case WM_APIID_SET_DCF_DATA:
        wcmCpsifw.sendResult = (WMErrCode) (cb->errcode);
        if (cb->errcode == WM_ERRCODE_SUCCESS)
        {
            WCMi_ResetKeepAliveAlarm();
        }

        OS_WakeupThread(&(wcmCpsifw.sendQ));
        break;
    }
}

/*---------------------------------------------------------------------------*
Name: WcmCpsifKACallback

Description: Callback function that accepts asynchronous responses from the wireless driver
in response to DCF frame send requests. Specific to sending Keep Alive NULL
packets. As exceptional send controls are performed for normal packets,
it is possible to switch to a callback function that can only register one.

Arguments: arg -   The address of the buffer where the response parameters
from the wireless driver are stored is passed.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmCpsifKACallback(void* arg)
{
#if WCM_DEBUG
    if (((WMCallback*)arg)->errcode == WM_ERRCODE_SUCCESS)
    {
        WCMi_Printf("Send NULL packet for KeepAlive.\n");
    }

#else
#pragma unused(arg)
#endif

// clears the exception lock regardless of process outcome
    WcmCpsifUnlockMutexInIRQ(&(wcmCpsifw.sendMutex));
}

/*---------------------------------------------------------------------------*
Name: WcmCpsifTryLockMutexInIRQ

Description: Tests the lock for the Mutex for exception controls within the IRQ interrupt service.
The Mutex locked by this function can only be unlocked by the
WcmCpsifUnlockMutexInIRQ function.

Arguments: mutex   -   Specifies a pointer to the OSMutex variable for exception controls.

Returns: BOOL    -   Returns TRUE if the lock is successful. If it has been locked
somehow by another thread, returns FALSE.
 *---------------------------------------------------------------------------*/
static BOOL WcmCpsifTryLockMutexInIRQ(OSMutex* mutex)
{
#if WCM_DEBUG
    if ((mutex == NULL) || (OS_GetProcMode() != HW_PSR_IRQ_MODE))
    {
        return FALSE;
    }
#endif
    if (mutex->thread == NULL)
    {
        mutex->thread = WCMCPSIF_DUMMY_IRQ_THREAD;
        mutex->count++;
        return TRUE;
    }
    else if (mutex->thread == WCMCPSIF_DUMMY_IRQ_THREAD)
    {
        mutex->count++;
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
Name: WcmCpsifUnlockMutexInIRQ

Description: Tests the unlock for the Mutex for exception controls within
the IRQ interrupt service. Only valid for the Mutex locked by the
WcmCpsifTryLockMutexInIRQ function.
There is no way to know if the unlock didn't work.

Arguments: mutex   -   Specifies a pointer to the OSMutex variable for exception controls.

Returns: None.
 *---------------------------------------------------------------------------*/
static void WcmCpsifUnlockMutexInIRQ(OSMutex* mutex)
{
#if WCM_DEBUG
    if ((mutex == NULL) || (OS_GetProcMode() != HW_PSR_IRQ_MODE))
    {
        return;
    }
#endif
    if (mutex->thread == WCMCPSIF_DUMMY_IRQ_THREAD)
    {
        if (--(mutex->count) == 0)
        {
            mutex->thread = NULL;
            OS_WakeupThread(&(mutex->queue));
        }
    }
}

/*---------------------------------------------------------------------------*
End of file
 *---------------------------------------------------------------------------*/
