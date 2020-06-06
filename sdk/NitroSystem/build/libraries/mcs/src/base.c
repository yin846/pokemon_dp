/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     base.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: base.c,v $
  Revision 1.37  06/21/2005 09:22:43 AM  kitamurs
  Merged with main program 

  Revision 1.36.2.1  2005/06/21 09:17:27  kitamurs
  Support for IS-NITRO-DEBUGGER 1.56

  Revision 1.36  03/02/2005 12:02:10 AM  kitamurs
  Inserted Lock, Unlock in external functions

  Revision 1.35  01/21/2005 04:50:10 AM  kitamurs
  Changed year

  Revision 1.34  01/13/2005 07:59:34 AM  kitamurs
  Corrected spelling error

  Revision 1.33  12/21/2004 04:24:54 AM  kitamurs
  Changed debug print

  Revision 1.32  12/20/2004 07:12:45 AM  kitamurs
  Made to call Polling

  Revision 1.31  12/20/2004 06:46:40 AM  kitamurs
  Changed the function orders

  Revision 1.30  12/20/2004 05:04:32 AM  kitamurs
  Connection confirmation process on server

  Revision 1.29  12/17/2004 01:47:45 AM  kitamurs
  Fixed bugs in thread measures for polling

  Revision 1.28  12/16/2004 07:39:05 AM  kitamurs
  Thread measures for polling

  Revision 1.27  12/16/2004 07:25:03 AM  kitamurs
  Newly created the Sleep function

  Revision 1.26  12/16/2004 02:32:20 AM  kitamurs
  Improved the wait method for recognizing device when connecting UIC

  Revision 1.25  12/13/2004 01:22:30 AM  kitamurs
  DEBUGGER startup determination

  Revision 1.24  12/09/2004 06:44:25 AM  kitamurs
  Constant value

  Revision 1.23  12/08/2004 01:25:05 AM  kitamurs
  When connecting UIC, made to wait until writing becomes possible

  Revision 1.22  12/01/2004 01:44:49 AM  kitamurs
  Message version check

  Revision 1.21  11/30/2004 12:48:47 AM  kitamurs
  Changed the send wait method

  Revision 1.20  11/18/2004 01:18:40 AM  kitamurs
  Added an error message

  Revision 1.19  11/12/2004 02:03:04 AM  kitamurs
  Added support for divided transfer

  Revision 1.18  11/10/2004 06:14:55 AM  kitamurs
  Added NNS_McsOpenEx

  Revision 1.17  10/19/2004 01:22:57 AM  kitamurs
  NNS_McsGetTotaStreamReadableSize -> NNS_McsGetTotalStreamReadableSize

  Revision 1.16  10/15/2004 09:05:23 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.15  10/15/2004 08:49:38 AM  kitamurs
  Changed SDK_*** to NNS_***

  Revision 1.14  10/13/2004 06:48:54 AM  kitamurs
  Changed so that the maskResource update is performed in an interrupt-prohibited state

  Revision 1.13  10/13/2004 02:07:28 AM  kitamurs
  Added NNS_McsUnregisterRecvResouce()

  Revision 1.12  10/04/2004 05:44:55 AM  kitamurs
  Added the buffer clear function

  Revision 1.11  09/08/2004 06:00:14 AM  kitamurs
  Supported ensata

  Revision 1.10  09/02/2004 03:01:29 AM  kitamurs
  Renamed functions once again

  Revision 1.9  08/26/2004 02:14:36 AM  kitamurs
  - Changed the channel types from u32 to u16 once again
  - Changed the name of the functions

  Revision 1.8  08/05/2004 05:51:50 AM  kitamurs
  Changed the channel type from u32 to u16

  Revision 1.7  06/14/2004 05:24:10 AM  kitamurs
  FINAL_ROM provisions

  Revision 1.6  06/14/2004 01:01:35 AM  kitamurs
  Corrected comment

  Revision 1.5  06/09/2004 07:34:11 AM  kitamurs
  Provisional port of the file I/O

  Revision 1.4  05/31/2004 06:59:09 AM  kitamurs
  Corrected comments and added NNS_McsClose()

  Revision 1.3  05/30/2004 11:55:42 PM  kitamurs
  Corrected debug text.

  Revision 1.2  05/26/2004 08:50:35 AM  kitamurs
  Added device priorities

  Revision 1.1  05/25/2004 06:59:11 AM  kitamurs
  Moved directories

  Revision 1.1  05/24/2004 02:14:08 AM  kitamurs
  Moved directories

  Revision 1.4  04/28/2004 05:11:45 AM  kitamurs
  Added load functions

  Revision 1.3  04/21/2004 09:20:06 AM  kitamurs
  Defined write functions

  Revision 1.2  04/21/2004 05:56:21 AM  kitamurs
  Corrected typos

  Revision 1.1  04/14/2004 07:03:43 AM  kitamurs
  Registration of the initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)

#include <nitro.h>
#include <nnsys/misc.h>
#include <nnsys/mcs/base.h>
#include <nnsys/mcs/ringBuffer.h>
#include <isdbglib.h>
#include "baseCommoni.h"
#include "basei.h"


/* ========================================================================
    Constants
   ======================================================================== */

static const int SEND_RETRY_MAX = 10;
static const int UIC_WAIT_TIMEOUT_FRAME = 60 * 2;


/* ========================================================================
    Type Definitions
   ======================================================================== */

typedef struct DefRecvCBInfo DefRecvCBInfo;
struct DefRecvCBInfo
{
    NNSMcsRecvCBInfo    cbInfo;
    NNSMcsRingBuffer    ringBuf;
};

typedef struct NNSiMcsEnsWork NNSiMcsEnsWork;
struct NNSiMcsEnsWork
{
    NNSMcsRingBuffer    rdRb;
    NNSMcsRingBuffer    wrRb;
    NNSiMcsEnsMsgBuf    msgBuf;
};


/* ========================================================================
    static variables
   ======================================================================== */

static NNSFndList sRecvCBInfoList;          // Callback function list at reception
static u8 sInitialized = FALSE;             // TRUE if initialized
static u8 sProtocolError = FALSE;           // mcs library-level mismatch
static u8 sbLengthEnable = FALSE;           // TRUE if length is obtained and it is a non-zero value
static u8 sbHostDataRecived = FALSE;        // TRUE if the data was received from host at first

static NNSMcsDeviceCaps sDeviceCaps =
                        {
                            NITRODEVID_NULL,    // Device recognition ID
                            0x00000000,         // resource identification information flag
                        };

static NNSiMcsMsg sWriteMsg;	// Write buffer
static OSMutex sMutex;


/* ========================================================================
    static functions
   ======================================================================== */

static inline
uint32_t
RoundUp(
    uint32_t    value,
    uint32_t    alignment
)
{
    return (value + alignment -1) & ~(alignment -1);
}

static inline
uint32_t
RoundDown(
    uint32_t    value,
    uint32_t    alignment
)
{
    return value & ~(alignment -1);
}

static inline
u32
min_u32(
    u32     a,
    u32     b
)
{
    return a < b ? a: b;
}

/*
    Designate the channel, then get the pointer to NNSMcsRecvCBInfo
*/
static NNSMcsRecvCBInfo*
GetRecvCBInfo(
    NNSFndList* pList,
    uint32_t    channel
)
{
    NNSMcsRecvCBInfo* pInfo = NULL;

    while (NULL != (pInfo = NNS_FndGetNextListObject(pList, pInfo)))
    {
        if (pInfo->channel == channel)
        {
            return pInfo;
        }
    }

    return NULL;
}

static void
CallbackRecv(
    u32         userData,
    u32         channel,
    const void* pRecv,
    uint32_t    recvSize
)
{
    const NNSiMcsMsg *const pMsg = (NNSiMcsMsg*)pRecv;
    const u32 dataSize = recvSize - NNSi_MCS_MSG_HEAD_SIZE;

    if ( recvSize < NNSi_MCS_MSG_HEAD_SIZE
      || NNSi_MCS_MSG_HEAD_VERSION != pMsg->head.version
    )
    {
        sProtocolError = TRUE;
        return;
    }

    if (NNSi_MCS_SYSMSG_CHANNEL == channel)
    {
        u32 bConnect;

        if (dataSize == sizeof(bConnect))
        {
            bConnect = *(u32*)pMsg->data;
            sbHostDataRecived = (u8)bConnect;
        }
    }
    else
    {
        NNSFndList *const pList = (NNSFndList*)userData;
        NNSMcsRecvCBInfo *const pInfo = GetRecvCBInfo(pList, channel);

        if (pInfo)
        {
            (*pInfo->cbFunc)(pMsg->data, dataSize, pInfo->userData, pMsg->head.offset, pMsg->head.totalLen);
        }
    }
}

static void
CallbackRecvDummy(
    uintptr_t   /* userData*/,
    uint32_t    /* channel*/,
    const void* /* pRecv*/,
    uint32_t    /* recvSize*/
)
{
    // do nothing
}

static void
DataRecvCallback(
    const void* pData,
    u32         dataSize,
    u32         userData,
    u32         offset,
    u32         totalSize
)
{
    DefRecvCBInfo* cbInfo = (DefRecvCBInfo*)userData;

    if (NNS_McsGetRingBufferWritableBytes(cbInfo->ringBuf) < offset + dataSize)
    {
        // ERROR -  the buffer has been exhausted
        NNS_WARNING(FALSE, "NNS mcs error : buffer is not enough. writable %d, data size %d, offset %d, total size %d\n",
            NNS_McsGetRingBufferWritableBytes(cbInfo->ringBuf),
            dataSize,
            offset,
            totalSize);
        return;
    }

    {
        BOOL ret = NNS_McsWriteRingBufferEx(
            cbInfo->ringBuf,
            pData,
            offset,
            dataSize,
            totalSize);
        NNS_ASSERT(ret);
    }
}

static inline NNSiMcsEnsWork*
GetEnsWorkPtr()
{
    return (NNSiMcsEnsWork*)NNSi_MCS_ENS_WRITE_RB_END;
}

static void
SetMaskResource(u32 maskResource)
{
    const BOOL preIRQ = OS_DisableIrq();
    sDeviceCaps.maskResource = maskResource;
    (void)OS_RestoreIrq(preIRQ);
}

static void
WaitSendData(void)
{
    NNS_McsPollingIdle();   // Set so that the PC-side is not blocked by the write wait

    NNSi_McsSleep(8);
}

static void
WaitDeviceEnable(void)
{
    NNS_McsPollingIdle();

    NNSi_McsSleep(16);
}

int         _isdbusmgr_isresourceavailable(int /*typeResource*/);

__declspec ( weak )
int
_isdbusmgr_isresourceavailable(int /*typeResource*/)
{
    return FALSE;
}

static BOOL
IsDebuggerPresent()
{
    static const int    ISDRESOURCE_SW_DEBUGGER    =   0x00200;

    return _isdbusmgr_isresourceavailable(ISDRESOURCE_SW_DEBUGGER);
}

static inline void
Lock()
{
    if (OS_IsThreadAvailable())
    {
        OS_LockMutex(&sMutex);
    }
}

static inline void
Unlock()
{
    if (OS_IsThreadAvailable())
    {
        OS_UnlockMutex(&sMutex);
    }
}

static BOOL
OpenEmulator(NNSMcsDeviceCaps* pCaps)
{
    const NNSMcsRingBuffer rdRb = (NNSMcsRingBuffer)NNSi_MCS_ENS_READ_RB_START;
    const NNSMcsRingBuffer wrRb = (NNSMcsRingBuffer)NNSi_MCS_ENS_WRITE_RB_START;

    if (! NNS_McsCheckRingBuffer(rdRb) || ! NNS_McsCheckRingBuffer(wrRb))
    {
        return FALSE;
    }

    {
        NNSiMcsEnsWork* pWork = GetEnsWorkPtr();
        pWork->rdRb = rdRb;
        pWork->wrRb = wrRb;
    }

    pCaps->deviceID     = 0;
    pCaps->maskResource = NITROMASK_RESOURCE_POLL;

    return TRUE;
}

static BOOL
OpenISDevice(NNSMcsDeviceCaps* pCaps)
{
    // Device priority
    enum
    {
        DEV_PRI_NONE,               // No device
        DEV_PRI_UNKNOWN,            // Unknown device
        DEV_PRI_NITRODBG,           // IS-NITRO-DEBUGGER
        DEV_PRI_NITROUIC,           // IS-NITRO-UIC
        DEV_PRI_NITROUSB,           // IS-NITRO-EMULATOR

        DEV_PRI_MAX
    };

    int devPri = DEV_PRI_NONE;
    int devID = 0;
    const int devNum = NNS_McsGetMaxCaps();
    int findDevID = 0;
    const NITRODEVCAPS* pNITROCaps;
    // OS_Printf("on DEBBUGER ? %s\n", IsDebuggerPresent() ? "yes": "no");

    for (findDevID = 0; findDevID < devNum; ++findDevID)
    {
        int findDevPri = DEV_PRI_NONE;
        pNITROCaps = NITROToolAPIGetDeviceCaps(findDevID);
        switch (pNITROCaps->m_nDeviceID)
        {
        case NITRODEVID_NITEMULATOR:    // IS-NITRO-EMULATOR
            if (IsDebuggerPresent())
            {
                findDevPri = DEV_PRI_NITRODBG;
            }
            else    // When not launched from IS-NITRO-DEBUGGER software
            {
                findDevPri = DEV_PRI_NITROUSB;
            }
            break;
        case NITRODEVID_NITUIC:         // IS-NITRO-UIC CARTRIDGE
            findDevPri = DEV_PRI_NITROUIC;
            break;
        default:
            findDevPri = DEV_PRI_UNKNOWN;
        }

        if (devPri < findDevPri)
        {
            devPri = findDevPri;
			devID  = findDevID;
        }
    }

    if (devPri == DEV_PRI_NONE || devPri == DEV_PRI_UNKNOWN)
    {
        OS_Warning("no device.\n");     // No connectable device found.
        return FALSE;
    }

    pNITROCaps = NITROToolAPIGetDeviceCaps(devID);
    if (! NITROToolAPIOpen(pNITROCaps))
    {
        return FALSE;   // Failed to open and initialize device.
    }

    pCaps->deviceID     = pNITROCaps->m_nDeviceID;
    pCaps->maskResource = pNITROCaps->m_dwMaskResource;
    sbLengthEnable = FALSE;

    {
        // Register the callback function (If data is sent from the Windows application, this function is called.)
        BOOL bSuccess = NITROToolAPISetReceiveStreamCallBackFunction(CallbackRecv, (u32)&sRecvCBInfoList);
        NNS_ASSERT(bSuccess);
    }

    return TRUE;
}

static inline BOOL
CloseEmulator()
{
    NNSiMcsEnsWork* pWork = GetEnsWorkPtr();
    pWork->rdRb = 0;
    pWork->wrRb = 0;

    return TRUE;
}


/* ========================================================================
    External functions (public)
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_McsInit

  Description:  Initialization function for communication with devices. 
                Before using the NNS_Mcs function, 
                this function must be called. 

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsInit()
{
    if(sInitialized)
    {
        return;
    }

    if (! OS_IsRunOnEmulator())
    {
        NITROToolAPIInit();    // Initialization of communications with NITRO
    }

    NNS_FND_INIT_LIST(&sRecvCBInfoList, NNSMcsRecvCBInfo, link);

    OS_InitMutex(&sMutex);

    sInitialized = TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_McsGetMaxCaps

  Description:  Gets the total number of communication devices currently connected to the target

  Arguments:    None.

  Returns:      Returns the total number of communication devices currently connected to the target.
 *---------------------------------------------------------------------------*/
int
NNS_McsGetMaxCaps()
{
    int num;

    NNS_ASSERT(sInitialized);
    Lock();

    if (OS_IsRunOnEmulator())
    {
        num = 1;
    }
    else
    {
        num = NITROToolAPIGetMaxCaps(); // max number of devices
    }

    Unlock();
    return num;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsOpen

  Description:  Opens the device, then stores the information for that opened device
                in the variable specified by argument.
                When multiple devices are connected,
                the devices found are opened in the following order:
                  1. IS-NITRO-UIC
                  2. IS-NITRO-EMULATOR
                  4. Unknown device

  Arguments:    pCaps:  Pointer to the structure used to get the information about the communication device

  Returns:      Returns TRUE if opened successfully.
                Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsOpen(NNSMcsDeviceCaps* pCaps)
{
    BOOL bSuccess;

    NNS_ASSERT(sInitialized);
    Lock();

    if (OS_IsRunOnEmulator())
    {
        bSuccess = OpenEmulator(pCaps);
    }
    else
    {
        bSuccess = OpenISDevice(pCaps);
    }

    if (bSuccess)
    {
        sDeviceCaps.deviceID = pCaps->deviceID;
        SetMaskResource(pCaps->maskResource);

        sbHostDataRecived = FALSE;
    }

    Unlock();
    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsClose

  Description:  Closes open devices.

  Arguments:    None.

  Returns:      Returns TRUE if closed successfully.
                Returns FALSE if failed.
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsClose()
{
    BOOL bSuccess;

    Lock();

    if (OS_IsRunOnEmulator())
    {
        bSuccess = CloseEmulator();
    }
    else
    {
        bSuccess = NITROToolAPIClose();
    }

    if (bSuccess)
    {
        sDeviceCaps.deviceID = NITRODEVID_NULL; // initialization of device identification ID
        SetMaskResource(0x00000000);            // flag initialization of resource identification information
    }

    Unlock();
    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsRegisterRecvCallback

  Description:  Registers the function to be called-back when receiving data.

                Do not send or receive data inside the callback function to be registered.
                Also, since there are times when interrupts are disabled,
                do not perform any interrupt wait loops.

  Arguments:    pInfo:     Pointer to the structure that holds the callback function information
                channel:   A user-defined value for identifying data transmissions.
                cbFunc:    The callback function to be registered.
                userData:  user-assigned value

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsRegisterRecvCallback(
    NNSMcsRecvCBInfo*   pInfo,
    u16                 channel,
    NNSMcsRecvCallback  cbFunc,
    u32                 userData
)
{
    NNS_ASSERT(sInitialized);
    Lock();

    NNS_ASSERT(NULL == GetRecvCBInfo(&sRecvCBInfoList, channel));   // Be sure no one else is already on the same channel

    pInfo->channel  = channel;
    pInfo->cbFunc   = cbFunc;
    pInfo->userData = userData;

    NNS_FndAppendListObject(&sRecvCBInfoList, pInfo);

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsRegisterStreamRecvBuffer

  Description:  Registers the buffer for data reception.
                Since the function NNS_McsRegisterRecvCallback() is called internally,
                it cannot be used at the same time as a data reception callback.
                Allocates the information area to be used internally for the specified buffer.
                The buffer size must therefore be at least 48.

                When the receive buffer is full and there is no free space to
                store newly received data, the received data filling the buffer is discarded.
                Be sure the buffer size is large enough to hold the
                amount of data used in communication.

  Arguments:    channel:   A user-defined value for identifying data transmissions.
                buf:       The reception buffer to register
                bufSize:   The size of the reception buffer to register

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsRegisterStreamRecvBuffer(
    u16     channel,
    void*   buf,
    u32     bufSize
)
{
    uintptr_t start = (uintptr_t)buf;
    uintptr_t end = start + bufSize;
    u8* pBBuf;
    DefRecvCBInfo* pInfo;

    NNS_ASSERT(sInitialized);
    Lock();

    start = RoundUp(start, 4);
    end = RoundDown(end, 4);

    NNS_ASSERT(start < end);

    buf = (void*)start;
    bufSize = end - start;

    NNS_ASSERT(bufSize >= sizeof(DefRecvCBInfo) + sizeof(NNSiMcsRingBufferHeader) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t));

    pBBuf = buf;
    pInfo = buf;

    pInfo->ringBuf = NNS_McsInitRingBuffer(pBBuf + sizeof(DefRecvCBInfo), bufSize - sizeof(DefRecvCBInfo));

    NNS_McsRegisterRecvCallback(
        &pInfo->cbInfo,
        channel,
        DataRecvCallback,
        (uintptr_t)pInfo);

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsUnregisterRecvResource

  Description:  Cancels the registration of the receive callback function registered with NNS_McsRegisterRecvCallback() or the receive buffer registered with NNS_McsRegisterStreamRecvBuffer(). 
                   
                   

  Arguments:    channel:  The value designated when registering the callback function and buffer.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsUnregisterRecvResource(u16 channel)
{
    NNSMcsRecvCBInfo* pInfo = NULL;

    NNS_ASSERT(sInitialized);
    Lock();

    pInfo = GetRecvCBInfo(&sRecvCBInfoList, channel);
    NNS_ASSERT(pInfo);

    NNS_FndRemoveListObject(&sRecvCBInfoList, pInfo);

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsGetStreamReadableSize

  Description:  Gets the data size readable with one call of the NNS_McsReadStream() function. 
                 

  Arguments:    channel:    Value used to identify data reception. Assigned by the user.

  Returns:      Returns the readable data size.
 *---------------------------------------------------------------------------*/
u32
NNS_McsGetStreamReadableSize(u16 channel)
{
    u32 size;

    NNS_ASSERT(sInitialized);
    Lock();

    {
        DefRecvCBInfo* pCBInfo = (DefRecvCBInfo*)GetRecvCBInfo(&sRecvCBInfoList, channel)->userData;

        NNS_ASSERT(pCBInfo);

        size = NNS_McsGetRingBufferReadableBytes(pCBInfo->ringBuf);
    }

    Unlock();
    return size;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsGetTotalStreamReadableSize

  Description:  Gets the total readable data size in the receive buffer.

  Arguments:    channel:    Value used to indentify data reception. Assigned by the user.

  Returns:      Returns the total data size.
 *---------------------------------------------------------------------------*/
u32
NNS_McsGetTotalStreamReadableSize(u16 channel)
{
    u32 size;

    NNS_ASSERT(sInitialized);
    Lock();

    {
        DefRecvCBInfo* pCBInfo = (DefRecvCBInfo*)GetRecvCBInfo(&sRecvCBInfoList, channel)->userData;

        NNS_ASSERT(pCBInfo);

        size =  NNS_McsGetRingBufferTotalReadableBytes(pCBInfo->ringBuf);
    }

    Unlock();
    return size;
}


/*---------------------------------------------------------------------------*
  Name:         NNS_McsReadStream

  Description:  Receives data.

                If the length of the received data is larger than the buffer size designated with size,
                reads only the portion designated by size. In this case, the return value will be FALSE.
                 The remaining size can be obtained with NNS_McsGetStreamReadableSize().

  Arguments:    channel:    Value for identifying data reception. Assigned by the user.
                data:       Pointer to the buffer that stores the data to be read.
                size:       Size of the buffer that stores the data to be read.
                pReadSize:  The size that was actually read.

  Returns:      Returns TRUE if the function succeeds, or FALSE if it fails.
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsReadStream(
    u16         channel,
    void*       data,
    u32         size,
    u32*        pReadSize
)
{
    BOOL bSuccess;

    NNS_ASSERT(sInitialized);
    Lock();

    {
        DefRecvCBInfo* pCBInfo = (DefRecvCBInfo*)GetRecvCBInfo(&sRecvCBInfoList, channel)->userData;

        NNS_ASSERT(pCBInfo);

        bSuccess = NNS_McsReadRingBuffer(pCBInfo->ringBuf, data, size, pReadSize);
    }

    Unlock();
    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsGetStreamWritableLength

  Description:  Gets the max length of the buffer for sending data.
                No length greater than this can be sent.

  Arguments:    pLength:  Pointer to the variable that stores the length of the data send buffer.

  Returns:      Returns TRUE if the function is successful, FALSE if it fails.
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsGetStreamWritableLength(u32* pLength)
{
    BOOL ret = FALSE;
    u32 length;

    NNS_ASSERT(sInitialized);
    Lock();

    if (OS_IsRunOnEmulator())
    {
        length = NNS_McsGetRingBufferWritableBytes(GetEnsWorkPtr()->wrRb);
        ret = TRUE;
    }
    else
    {
        u32 i;

        // With UIC, 0 is returned for some dozens of milliseconds after the device opens.
        for (i = 0; i < UIC_WAIT_TIMEOUT_FRAME; ++i)
        {
            ret = 0 != NITROToolAPIStreamGetWritableLength(&length);

            if (! ret)
            {
                OS_Printf("NNS Mcs error: faild NITROToolAPIStreamGetWritableLength()\n");
                break;
            }

            if (sbLengthEnable)
            {
                break;
            }

            if (length > 0)
            {
                sbLengthEnable = TRUE;
                break;
            }

            WaitDeviceEnable();
        }
    }

    if (ret)
    {
        if (length < NNSi_MCS_MSG_HEAD_SIZE)
        {
            *pLength = 0;
        }
        else
        {
            *pLength = length - NNSi_MCS_MSG_HEAD_SIZE;
        }
    }

    Unlock();
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsWriteStream

  Description:  Sends data.

  Arguments:    channel:  Value for identification of the outgoing data. Assigned by the user.
                data:     Pointer to the buffer that stores the data to send.
                size:     The size of the data to send.

  Returns:      Returns TRUE if the function is successful, FALSE if it fails.
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsWriteStream(
    u16         channel,
    const void* data,
    u32         size
)
{
    const u8 *const pSrc = (const u8*)data;
    int retryCnt = 0;
    u32 offset = 0;

    NNS_ASSERT(sInitialized);
    Lock();

    // OS_Printf("NNS_McsWriteStream channel %d, size %d\n", (int)(channel), size);

    while (offset < size)
    {
        const u32 restSize = size - offset;
        u32 length;

        if (! NNS_McsGetStreamWritableLength(&length))  // Obtains the amount of data that can be written at once
        {
            break;
        }

        if (restSize > length && length < NNSi_MCS_MSG_DATA_SIZE_MIN)
        {
            if (++retryCnt > SEND_RETRY_MAX)
            {
                NNS_WARNING(FALSE, "NNS Mcs error: send time out writable bytes %d, rest bytes %d\n", length, restSize);
                break;
            }
            WaitSendData(); // Wait until it can be written
        }
        else
        {
            NNSiMcsMsg* pBlock = &sWriteMsg;

            length = min_u32(min_u32(restSize, length), NNSi_MCS_MSG_DATA_SIZE_MAX);

            pBlock->head.version = NNSi_MCS_MSG_HEAD_VERSION;
            pBlock->head.reserved = 0;
            pBlock->head.offset = offset;
            pBlock->head.totalLen = size;
            MI_CpuCopy8(pSrc + offset, pBlock->data, length);

            // OS_Printf("1 write offset %d, length [%d/%d]\n", pBlock->head.offset, length, pBlock->head.totalLen);

            if (OS_IsRunOnEmulator())
            {
                NNSiMcsEnsWork *const pWork = GetEnsWorkPtr();

                pWork->msgBuf.channel = channel;
                MI_CpuCopy8(pBlock, pWork->msgBuf.buf, NNSi_MCS_MSG_HEAD_SIZE + length);
                if (! NNS_McsWriteRingBuffer(
                    pWork->wrRb,
                    &pWork->msgBuf,
                    offsetof(NNSiMcsEnsMsgBuf, buf) + NNSi_MCS_MSG_HEAD_SIZE + length)
                )
                {
                    NNS_WARNING(FALSE, "NNS Mcs error: send error\n");
                    break;
                }
            }
            else
            {
                if (! NITROToolAPIWriteStream(
                    channel,
                    pBlock,
                    NNSi_MCS_MSG_HEAD_SIZE + length)
                )
                {
                    NNS_WARNING(FALSE, "NNS Mcs error: failed NITROToolAPIWriteStream()\n");
                    break;
                }
            }

            offset += length;
        }
    }

    Unlock();
    return offset == size;  // If it succeeds in writing to the end, returns TRUE
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsClearBuffer

  Description:  Clears the contents of the communication buffer.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsClearBuffer()
{
    NNSMcsRecvCBInfo* pInfo = NULL;

    NNS_ASSERT(sInitialized);
    Lock();

    if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_POLL)
    {
        if (OS_IsRunOnEmulator())
        {
            NNSiMcsEnsWork* pWork = GetEnsWorkPtr();
            NNS_McsClearRingBuffer(pWork->rdRb);
        }
        else
        {
            // register the dummy callback function
            BOOL bSuccess = NITROToolAPISetReceiveStreamCallBackFunction(CallbackRecvDummy, 0);
            NNS_ASSERT(bSuccess);

            NITROToolAPIPollingIdle();

            // re-register the original callback function
            bSuccess = NITROToolAPISetReceiveStreamCallBackFunction(CallbackRecv, (u32)&sRecvCBInfoList);
            NNS_ASSERT(bSuccess);
        }
    }

    while (NULL != (pInfo = NNS_FndGetNextListObject(&sRecvCBInfoList, pInfo)))
    {
        if (pInfo->cbFunc == DataRecvCallback)  // Function using the ring buffer
        {
            DefRecvCBInfo* pDefCBInfo = (DefRecvCBInfo*)pInfo->userData;
            NNS_McsClearRingBuffer(pDefCBInfo->ringBuf);
        }
    }

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsIsServerConnect

  Description:  Returns whether the mcs server is connected to or disconnected from NITRO hardware.
                 

  Arguments:    None.

  Returns:      Returns true if the mcs server is connected to the NITRO hardware.
                Returns false if it is disconnected.
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsIsServerConnect()
{
    BOOL bSuccess;

    NNS_ASSERT(sInitialized);
    Lock();

    NNS_McsPollingIdle();

    bSuccess = sbHostDataRecived;

    Unlock();
    return bSuccess;
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsPollingIdle

  Description:  Call this function inside the main loop.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsPollingIdle()
{
    Lock();

    if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_POLL)
    {
        if (OS_IsRunOnEmulator())
        {
            NNSiMcsEnsWork* pWork = GetEnsWorkPtr();
            uint32_t readableBytes;
            uint32_t readBytes;

            while (0 < (readableBytes = NNS_McsGetRingBufferReadableBytes(pWork->rdRb)))
            {
                NNS_ASSERT(offsetof(NNSiMcsEnsMsgBuf, buf) < readableBytes && readableBytes <= sizeof(NNSiMcsEnsMsgBuf));

                {
                    BOOL bRet = NNS_McsReadRingBuffer(pWork->rdRb, &pWork->msgBuf, readableBytes, &readBytes);
                    NNS_ASSERT(bRet);
                }

                CallbackRecv((u32)&sRecvCBInfoList, pWork->msgBuf.channel, pWork->msgBuf.buf, readBytes - offsetof(NNSiMcsEnsMsgBuf, buf));
            }
        }
        else
        {
            NITROToolAPIPollingIdle();
        }

        if (sProtocolError)
        {
            const u32 data = TRUE;  // Connection
            u32 length;
            if ( NNS_McsGetStreamWritableLength(&length)
              || sizeof(data) <= length
            )
            {
                // Send message to indicate the NITRO's header version.
                if (NNS_McsWriteStream(NNSi_MCS_SYSMSG_CHANNEL, &data, sizeof(data)))
                {
                    // If written, stop on the NITRO side.
                    OS_Panic("mcs message version error.\n");
                }
            }
        }
    }

    Unlock();
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsVBlankInterrupt

  Description:  Call this function inside the VBlank interrupt handler.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsVBlankInterrupt()
{
    if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_VBLANK)
    {
        NITROToolAPIVBlankInterrupt();
    }
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsCartridgeInterrupt

  Description:  Call this function inside the cartridge interrupt handler.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsCartridgeInterrupt()
{
    if (sDeviceCaps.maskResource & NITROMASK_RESOURCE_CARTRIDGE)
    {
        NITROToolAPICartridgeInterrupt();
    }
}


/* #if ! defined(NNS_FINALROM)*/
#endif

