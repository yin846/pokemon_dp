/*---------------------------------------------------------------------------*
Project: NitroWiFi - WCM - include
File: wcm_private.h

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: wcm_private.h,v $
Revision 1.9  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.8  10/05/2005 13:44:40  yasu
Fixed bug in OS_SecondsToTicks().

Revision 1.7  10/05/2005 09:40:38  terui
Changed NULL packet send interval for keep-alive from 60 sec to 70 sec.

Revision 1.6  10/05/2005 08:53:32  terui
Added a buffer for storing signal strength and a prototype declaration for the get function.

Revision 1.5  09/12/2005 10:01:13  terui
added the WCM_CONNECT_STATUSCODE_ILLEGAL_RATESET constants

Revision 1.4  09/01/2005 13:11:03  terui
moved Keep Alive packet send interval to constant definitions
added alarm variables to work structure
added variable that keeps the wlStatus when connect fails to work structure

Revision 1.3  08/09/2005 07:58:30  terui
adjusted the internal buffer that empties the WEP key for a 32-byte alignment

Revision 1.2  07/11/2005 12:05:31  terui
Added a dummy member to WCMWork structure to match the cache line.

Revision 1.1  07/07/2005 10:45:37  terui
new additions

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_WCM_PRIVATE_H_
#define NITROWIFI_WCM_PRIVATE_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/
#include <nitroWiFi/wcm.h>
#include <nitro/os.h>

/*---------------------------------------------------------------------------*
constant definitions
 *---------------------------------------------------------------------------*/

// status of reset duplicate call management flags
#define WCM_RESETTING_OFF   0
#define WCM_RESETTING_ON    1

// use status for each block within the AP data storage list
#define WCM_APLIST_BLOCK_EMPTY  0
#define WCM_APLIST_BLOCK_OCCUPY 1

// KEEP NULL packet send interval (seconds)
#define WCM_KEEP_ALIVE_SPAN ((OSTick) 70)

// debug display settings
#ifdef SDK_FINALROM
#define WCM_DEBUG   0
#else
#define WCM_DEBUG   1
#endif

// connect failure code definitions
#define WCM_CONNECT_STATUSCODE_ILLEGAL_RATESET  18

#if WCM_DEBUG
#define WCMi_Printf     OS_TPrintf("[WCM] %s(%d) Report: ", __FILE__, __LINE__); \
    OS_TPrintf
#define WCMi_Warning    OS_TPrintf("[WCM] %s(%d) Warning: ", __FILE__, __LINE__); \
    OS_TPrintf
#else
#define WCMi_Printf(...)    ((void)0)
#define WCMi_Warning(...)   ((void)0)
#endif

/*---------------------------------------------------------------------------*
structure definitions
 *---------------------------------------------------------------------------*/

// structure that brings together the WCM internal work variables
typedef struct WCMWork
{
    u8          wmWork[WCM_ROUNDUP32( WM_SYSTEM_BUF_SIZE ) ];
    u8          sendBuf[WCM_DCF_SEND_BUF_SIZE];
    u8          recvBuf[WCM_DCF_RECV_BUF_SIZE * 2];
    WMBssDesc   bssDesc;
    struct
    {
        u8  key[WM_SIZE_WEPKEY];
        u8  mode;
        u8  keyId;
        u8  reserved[WCM_ROUNDUP32( WM_SIZE_WEPKEY + 2 ) - ( WM_SIZE_WEPKEY + 2 ) ];
    } wepDesc;
    u32         phase;
    u32         option;
    u16         maxScanTime;
    u8          apListLock;
    u8          resetting;
    WCMConfig   config;
    WCMNotify   notify;
    s16         notifyId;
    u16         authId;
    u32         scanCount;
    WMScanExParam   scanExParam;
    OSAlarm         keepAliveAlarm;
    u16 wlStatusOnConnectFail;
    u8  reserved[6];

} WCMWork;

// AP data storage list block structure
typedef struct WCMApList
{
    u8      state;
    u8      reserved;
    u16     linkLevel;
    u32     index;
    void*   previous;
    void*   next;
    u32     data[WCM_ROUNDUP4( WCM_APLIST_SIZE ) / sizeof(u32) ];
} WCMApList;

// AP data storage list management header structure
typedef struct WCMApListHeader
{
    u32         count;
    WCMApList*  head;
    WCMApList*  tail;

} WCMApListHeader;

/*---------------------------------------------------------------------------*
function definitions
 *---------------------------------------------------------------------------*/

// gets a pointer to the WCM library's internal management structure
WCMWork*    WCMi_GetSystemWork(void);

// entry addition (update) to storage list request called when AP data is found
void        WCMi_EntryApList(WMBssDesc* bssDesc, u16 linkLevel);

// CPS interface initializing function called from the WCM_Init function
void        WCMi_InitCpsif(void);

// callback function to the CPS interface called when DCF data is received
void        WCMi_CpsifRecvCallback(WMDcfRecvBuf* recv);

// function to send Keep Alive NULL packets
void        WCMi_CpsifSendNullPacket(void);

// function to reset Keep Alive alarm
void        WCMi_ResetKeepAliveAlarm(void);

// Function for backing up the received signal strength when DCF data has been received
void        WCMi_ShelterRssi(u8 rssi);
u8          WCMi_GetRssiAverage(void);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* NITROWIFI_WCM_PRIVATE_H_ */

/*---------------------------------------------------------------------------*
End of file
 *---------------------------------------------------------------------------*/
