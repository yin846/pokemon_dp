/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     base.h

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: base.h,v $
  Revision 1.23  2005/02/25 02:59:48  kitamurs
  Warning measures

  Revision 1.22  01/24/2005 06:56:31 AM  kitamurs
  Changed format

  Revision 1.21  01/21/2005 04:58:57 AM  kitamurs
  Changed year

  Revision 1.20  01/17/2005 12:25:49 AM  kitamurs
  Changed include guard name.

  Revision 1.19  01/13/2005 08:00:31 AM  kitamurs
  Corrected spelling error

  Revision 1.18  12/20/2004 06:47:02 AM  kitamurs
  Changed the function orders

  Revision 1.17  12/20/2004 05:06:06 AM  kitamurs
  Connection confirmation process on server

  Revision 1.16  12/16/2004 02:32:33 AM  kitamurs
  Improved the wait method for recognizing device when connecting UIC

  Revision 1.15  11/16/2004 06:11:22 AM  kitamurs
  Deleted unnecessary members

  Revision 1.14  11/16/2004 12:09:08 AM  kitamurs
  Fixed the problem with output of the warning when building FINALROM.

  Revision 1.13  11/12/2004 02:03:20 AM  kitamurs
  Added support for divided transfer

  Revision 1.12  11/10/2004 06:14:50 AM  kitamurs
  Added NNS_McsOpenEx

  Revision 1.11  10/19/2004 01:17:18 AM  kitamurs
  - Changed SDK_... to NNS_...
  - NNS_McsGetTotaStreamReadableSize -> NNS_McsGetTotalStreamReadableSize

  Revision 1.10  10/13/2004 02:46:39 AM  kitamurs
  Added NNS_McsUnregisterRecvResouce()

  Revision 1.9  10/04/2004 05:45:13 AM  kitamurs
  Added the buffer clear function

  Revision 1.8  09/02/2004 03:01:10 AM  kitamurs
  Renamed functions once again

  Revision 1.7  08/26/2004 02:15:02 AM  kitamurs
  - Changed the channel types from u32 to u16 once again
  - Changed the name of the functions

  Revision 1.6  08/05/2004 05:52:35 AM  kitamurs
  Changed the channel type from u32 to u16

  Revision 1.5  07/12/2004 02:23:45 AM  kitamurs
  Corrected the compile not going through on the FINALROM build

  Revision 1.4  06/14/2004 05:25:00 AM  kitamurs
  FINAL_ROM provisions

  Revision 1.3  06/09/2004 07:36:00 AM  kitamurs
  Provisional port of the file I/O

  Revision 1.2  05/31/2004 06:59:57 AM  kitamurs
  Addition of NNS_McsClose()

  Revision 1.1  05/25/2004 07:00:25 AM  kitamurs
  Moved directories

  Revision 1.1  05/24/2004 02:17:12 AM  kitamurs
  Moved directories

  Revision 1.4  04/28/2004 05:12:34 AM  kitamurs
  Added load functions

  Revision 1.3  04/21/2004 09:19:58 AM  kitamurs
  Defined write functions

  Revision 1.2  04/15/2004 01:41:32 AM  kitamurs
  Corrected typos

  Revision 1.1  04/14/2004 07:02:51 AM  kitamurs
  Registration of the initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef NNS_MCS_BASE_H_
#define NNS_MCS_BASE_H_

#include <nitro/types.h>
#include <nnsys/fnd/list.h>
#include <isdbglib.h>

#ifdef __cplusplus
extern "C" {
#endif


/* =======================================================================
    Type Definitions
   ======================================================================== */


// Device Information
typedef struct NNSMcsDeviceCaps NNSMcsDeviceCaps;
struct NNSMcsDeviceCaps
{
    u32         deviceID;           // Device recognition ID
    u32         maskResource;       // Resource needed to operate this device
};


// Callback function type when receiving
typedef void (*NNSMcsRecvCallback)(const void* recv, u32 recvSize, u32 userData, u32 offset, u32 totalSize);


// Callback function information when receiving
typedef struct NNSMcsRecvCBInfo NNSMcsRecvCBInfo;
struct NNSMcsRecvCBInfo
{
    u32                 channel;
    NNSMcsRecvCallback  cbFunc;
    u32                 userData;
    NNSFndLink          link;
};


/* =======================================================================
    Function Prototypes
   ======================================================================== */

#if defined(NNS_FINALROM)

    #define             NNS_McsInit()           ((void)0)

    #define             NNS_McsGetMaxCaps()     (0)

    #define             NNS_McsOpen(pCaps)      ( (void)(pCaps), FALSE )

    #define             NNS_McsClose()          (FALSE)

    #define             NNS_McsRegisterRecvCallback(pInfo, channel, cbFunc, userData)   ( (void)( (void)( (void)( (void)( (void)(pInfo), (channel) ), (cbFunc) ), (userData) ), 0 ) )

    #define             NNS_McsRegisterStreamRecvBuffer(channel, buf, bufSize)          ( (void)( (void)( (void)( (void)(channel), (buf) ), (bufSize) ), 0 ) )

    #define             NNS_McsUnregisterRecvResource(channel)      ( (void)(channel) )

    #define             NNS_McsGetStreamReadableSize(channel)       ( (void)(channel), 0U )

    #define             NNS_McsGetTotalStreamReadableSize(channel)  ( (void)(channel), 0U )

    #define             NNS_McsReadStream(channel, data, size, pReadSize)   ( (void)( (void)( (void)( (void)(channel), (data) ), (size) ), (pReadSize) ), FALSE )

    #define             NNS_McsGetStreamWritableLength(pLength)             ( (void)(pLength), FALSE )

    #define             NNS_McsWriteStream(channel, data, size)             ( (void)( (void)( (void)(channel), (data) ), (size) ), FALSE )

    #define             NNS_McsClearBuffer()            ((void)0)

    #define             NNS_McsIsServerConnect()        (FALSE)

    #define             NNS_McsPollingIdle()            ((void)0)

    #define             NNS_McsVBlankInterrupt()        ((void)0)

    #define             NNS_McsCartridgeInterrupt()     ((void)0)

/* #if defined(NNS_FINALROM)*/
#else

    void                NNS_McsInit(void);

    int                 NNS_McsGetMaxCaps(void);

    BOOL                NNS_McsOpen(
                            NNSMcsDeviceCaps* pCaps);

    BOOL                NNS_McsClose(void);

    void                NNS_McsRegisterRecvCallback(
                            NNSMcsRecvCBInfo*   pInfo,
                            u16                 channel,
                            NNSMcsRecvCallback  cbFunc,
                            u32                 userData);

    void                NNS_McsRegisterStreamRecvBuffer(
                            u16     channel,
                            void*   buf,
                            u32     bufSize);

    void                NNS_McsUnregisterRecvResource(
                            u16     channel);

    u32                 NNS_McsGetStreamReadableSize(
                            u16     channel);

    u32                 NNS_McsGetTotalStreamReadableSize(
                            u16     channel);

    BOOL                NNS_McsReadStream(
                            u16         channel,
                            void*       data,
                            u32         size,
                            u32*        pReadSize);

    BOOL                NNS_McsGetStreamWritableLength(
                            u32*    pLength);

    BOOL                NNS_McsWriteStream(
                            u16         channel,
                            const void* data,
                            u32         size);

    void                NNS_McsClearBuffer(void);

    BOOL                NNS_McsIsServerConnect(void);

    void                NNS_McsPollingIdle(void);

    void                NNS_McsVBlankInterrupt(void);

    void                NNS_McsCartridgeInterrupt(void);


/* #if defined(NNS_FINALROM)*/
#endif

#ifdef __cplusplus
} /* extern "C"*/
#endif

/* NNS_MCS_BASE_H_*/
#endif

