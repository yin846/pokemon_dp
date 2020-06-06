/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_connectinet
  File:     dwc_connectinet.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_connectinet.h,v $
  Revision 1.21  06/27/2006 06:43:56  nakata
  Added a DCF watch function for where there is no FriendsMatch library

  Revision 1.20  05/25/2006 23:54:11  uemura
  Added the DWC_CheckWiFiStation function

  Revision 1.19  04/17/2006 07:33:34  nakata
  Added the DWC_EnableHotSpot function, which configures hotspots for browsers

  Revision 1.18  03/09/2006 08:05:52  uemura
  changed argument type for DWC_SetConnectApType from int to DWCApInfoType

  Revision 1.17  03/09/2006 07:35:13  nakata
  deleted spotinfo from DWCApInfo, changed related portions

  Revision 1.16  03/09/2006 06:07:41  uemura
  fixed bug where value couldn't be accurately determined when a hexadecimal 0 was used in the WEP argument for DWC_DebugConnectInetAsync
  changed so that the argument passed WEPMODE

  Revision 1.15  03/09/2006 02:50:37  uemura
  Fixed comments.

  Revision 1.14  03/09/2006 02:27:17  uemura
  added DWC_SetConnectApType and DWC_DebugConnectInetAsync
  added these two functions so they can be used with the dwc_connect_inet_sample compile switch

  Revision 1.13  02/23/2006 08:21:47  nakata
  changed the names of the constants associated with DWCApInfo

  Revision 1.12  02/23/2006 06:58:04  nakata
  changed the type of DWCApinfo.bssid from char to unsigned char

  Revision 1.11  02/23/2006 02:55:40  nakata
  added documentation related to DWC_GetApInfo function

  Revision 1.10  02/23/2006 00:53:54  nakata
  added the function getting AP data, DWC_GetApInfo, to dwc_connectinet.c
  suppressed unneeded debug messages in dwc_sample

  Revision 1.9  10/06/2005 07:39:38  sasakit
  Made specifications so that functions for lowering the priority level of  thread can be adjusted externally when calculating powers for SSL.

  Revision 1.8  10/01/2005 10:57:00  sasakit
  Made changes so that DISCONNECTEd does not result even when DWC_ProcessInet() is called from within DWC_CleanupInetAsync()

  Revision 1.7  09/17/2005 00:31:18  sasakit
  Added function for getting the number of dropped UDP packets.

  Revision 1.6  09/14/2005 01:55:31  sasakit
  Organized processing for disconnecting from the AP.
  Added DWC_GetLinkLevel().

  Revision 1.5  09/07/2005 07:13:55  sasakit
  Inserted statements for strictly calling DWC_AC functions.

  Revision 1.4  09/06/2005 09:12:38  sasakit
  Added processing of errors used during ConnectInet

  Revision 1.3  08/31/2005 04:13:43  sasakit
  Added function for switching servers.

  Revision 1.2  08/20/2005 07:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_CONNECTINET_H_
#define DWC_CONNECTINET_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DWCstInetControl
{
    volatile int ac_state;
    u16  state;
    u16  online;
    u16  dmaNo;
    u16  powerMode;

#ifndef SDK_FINALROM
	int  connectType;
#endif	// SDK_FINALROM
} DWCInetControl;

typedef enum
{
    DWC_CONNECTINET_STATE_NOT_INITIALIZED = 0,
    DWC_CONNECTINET_STATE_IDLE,
    DWC_CONNECTINET_STATE_OPERATING,
    DWC_CONNECTINET_STATE_OPERATED,
    DWC_CONNECTINET_STATE_CONNECTED,
    DWC_CONNECTINET_STATE_DISCONNECTING,
    DWC_CONNECTINET_STATE_DISCONNECTED,
    DWC_CONNECTINET_STATE_ERROR,
    DWC_CONNECTINET_STATE_FATAL_ERROR,

    DWC_CONNECTINET_STATE_LAST
} DWCInetResult;

typedef enum
{
    DWC_CONNECTINET_AUTH_TEST,
    DWC_CONNECTINET_AUTH_DEVELOP,
    DWC_CONNECTINET_AUTH_RELEASE,
    DWC_CONNECTINET_AUTH_LAST
} DWCInetAuthType;

// [nakata] from here is the expanded AP data get function

// area code
typedef enum {
	DWC_APINFO_AREA_JPN = 0,
	DWC_APINFO_AREA_USA,
	DWC_APINFO_AREA_EUR,
	DWC_APINFO_AREA_AUS,
	DWC_APINFO_AREA_UNKNOWN = 0xff
} DWCApInfoArea;

// AP type
typedef enum {
	DWC_APINFO_TYPE_USER0 = 0,
	DWC_APINFO_TYPE_USER1,
	DWC_APINFO_TYPE_USER2,
	DWC_APINFO_TYPE_USB,
	DWC_APINFO_TYPE_SHOP,
	DWC_APINFO_TYPE_FREESPOT,
	DWC_APINFO_TYPE_WAYPORT,
	DWC_APINFO_TYPE_OTHER,
	DWC_APINFO_TYPE_FALSE = 0xff
} DWCApInfoType;

#define DWC_SIZE_SPOTINFO 9

typedef struct DWCstApInfo
{
	DWCApInfoType		aptype;
	DWCApInfoArea		area;
	char				spotinfo[DWC_SIZE_SPOTINFO+1];
	char				essid[WM_SIZE_SSID+1];
	u8 					bssid[WM_SIZE_BSSID];
} DWCApInfo;

BOOL DWC_GetApInfo(DWCApInfo *apinfo);

// [nakata] to here was the expanded AP data get function

// [nakata] from here is the expanded browser support section
void DWC_EnableHotspot(void);
// [nakata] expanded browser support section ends here

// [uemura] 2006/05/24 Additions
BOOL DWC_CheckWiFiStation( const void *ssid, u16 len );
// [uemura] ends here

typedef void* (* DWCACAlloc )( u32 name, s32 size );				// Pointer to alloc function
typedef void  (* DWCACFree  )( u32 name, void *ptr, s32 size );		// Pointer to free function

extern void  DWC_InitInetEx( DWCInetControl* inetCntl, u16 dmaNo, u16 powerMode, u16 ssl_priority );
extern void  DWC_InitInet( DWCInetControl* inetCntl );
extern void  DWC_SetAuthServer( DWCInetAuthType type );

extern void  DWC_ConnectInetAsync( void );
extern void  DWC_DebugConnectInetAsync( const void *ssid, const void *wep, int wepMode );

extern BOOL  DWC_CheckInet( void );

extern void  DWC_ProcessInet( void );

extern DWCInetResult DWC_GetInetStatus( void );

extern void  DWC_CleanupInet( void );
extern BOOL  DWC_CleanupInetAsync( void );

extern BOOL  DWCi_CheckDisconnected( void );
extern WMLinkLevel DWC_GetLinkLevel( void );
extern int   DWC_GetUdpPacketDropNum( void );

// [nakata] 06/06/27 moved from dwc_main.c
extern void DWC_ForceShutdown( void );
extern BOOL DWC_UpdateConnection( void );
// [nakata] ends here

#ifndef SDK_FINALROM
void         DWC_SetConnectApType( DWCApInfoType type );
#else
#define      DWC_SetConnectApType( type )       ((void)0)
#endif

#ifdef __cplusplus
}
#endif


#endif // DWC_CONNECTINET_H_
