/*---------------------------------------------------------------------------*
  Project:  NitroDWC - ac - dwc_ac
  File:     dwc_ac.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  Revision 1.5  08/20/2005 07:01:18  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_AC_H_
#define DWC_AC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 *	Project		: Wi-Fi Connection
 *	Description	: Header for library for automatically connecting to network
 *	File		: dwc_ac.h
 *	Version		: 0.0
 *	Author		: Uemura
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *	Definitions
 *---------------------------------------------------------------------------*/
// Status during automatic connection (the value returned by DWC_AC_GetStatus)
enum{
	// Comment must be displayed for errors of -10000 or more
	// -2xxxx   Error code during NetCheck, usually a recognition type error
	// -5xxxx  Error code during AC and NetCheck, error indicates connection failed (for details, see description below)
	// The xxxx part of -5xxxx represents the type of connection destination closest to the internet and an error number.
	// The first two digits represent the error number while the last two digits represent the type of connection destination
	// First Two Digits    Last Two Digits    Error
	// 00      XX     Communications have been cut
	// 00      99     Cannot find AP beacon.
	// 10      99     AP beacon found, but SSID does not match.
	// 11      XX     WEP key incorrect, mode different
	// 12      XX     Limit on number of AP connections exceeded.
	// 13      XX     Cannot connect to AP.
	// 20      XX     DHCP error.
	// 21      XX     Name cannot be resolved. DNS Error.
	// 22      XX     Cannot connect to Internet 1 (due to HTTP Excluding 200 or 302 or Timeout).
	// 23      XX     Cannot connect to Internet 2 (Due to HTTP Excluding 200 or Timeout or CAinvalid during call Account Creates).
	// 30      XX     Cannot connect to Internet 3 (due to hot spot error: When Post203HTML and SSID to NAS,Å@HTTPExcluding200 or Timeout or  CAinvalid)
	// 31      XX     Cannot connect to Internet 4 (due to hot spot error: Postto Hotspot Auth Server (HTTPS))
	// 32      XX     Cannot connect to Internet 5 (due to hot spot error: HTTP302 during second Test Blacklist site).

	// The following are program errors
	DWC_AC_STATE_ERROR_FATAL            = -10,				// Error : A fatal error occured during wireless communication
	DWC_AC_STATE_ERROR_START_UP,							// Error  Start connection (WCM_StartupAsyc) failed
	DWC_AC_STATE_ERROR_SOCKET_START,						// Error  Socket start (SOC_Startup) failed
	DWC_AC_STATE_ERROR_NETCHECK_CREATE,						// Error  Connection test (DWC_Netcheck_Create) failed to start
	DWC_AC_STATE_ERROR_IRREGULAR,							// Error  Occurs when the return value of WCM_GetPhase was set to 'WCM_PHASE_IRREGULAR"

	// The following is the status within the DWC_AC
	DWC_AC_STATE_NULL                   = 0,				// Initial state
	DWC_AC_STATE_SEARCH,									// Searching
	DWC_AC_STATE_CONNECT,									// Connecting to AP
	DWC_AC_STATE_TEST,										// Checking connection to the Internet
	DWC_AC_STATE_RETRY,										// Re-connected to another AP
	DWC_AC_STATE_COMPLETE									// Connection complete
};

// Type of access point (the value returned by DWC_AC_GetApType)
enum{
	DWC_AC_AP_TYPE_USER1,									// User Setting 1
	DWC_AC_AP_TYPE_USER2,									// User Setting 2
	DWC_AC_AP_TYPE_USER3,									// User Setting 3
	DWC_AC_AP_TYPE_USB,										// Ninteno USBAP
	DWC_AC_AP_TYPE_SHOP,									// Nintendo SHOPAP
	DWC_AC_AP_TYPE_FREESPOT,								// FreeSpot
	DWC_AC_AP_TYPE_WAYPORT,									// WayPort
	DWC_AC_AP_TYPE_NINTENDOWFC,								// NINTENDOWFC
	DWC_AC_AP_TYPE_FALSE = 0xff								// Get failed
};

/*---------------------------------------------------------------------------*
 *	structure definitions
 *---------------------------------------------------------------------------*/
typedef struct tagDWCACApData{								// Data structure for the AP to be connected to
	u8          bssid[ WM_SIZE_BSSID ];						// MAC address of the AP connected to
	u8          data            [ 10 ];						// Data included in SSID for the Nintendo store AP
} DWCACApData;

typedef struct tagDWCACOption{								// Option settings for AC
	u8          connectType;								// Specifies the type of AP being connected to (O: No specification, 1Å`8: Specifies the AP connected to)
	u8          skipNetCheck;								// Skip NetCheck (1: No check, 0: Check)
} DWCACOption;

typedef struct tagDWCACConfig{								// Config for DWC_AC routines
	void*       (* alloc )( u32 name, s32 size );			// Pointer to alloc function
	void        (* free  )( u32 name, void *ptr, s32 size );// Pointer to free function
	u8          dmaNo;										// 0 - 3
	u8          powerMode;									// 1 : ACTIVEÅ@0 : SAVE
	DWCACOption option;										// Option settings
} DWCACConfig;

/*---------------------------------------------------------------------------*
 *	function definitions
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_Create
 * Description	: Initialization function  Allocates memory and initializes DWC_AC and WCM.
 * Arguments	: *config - Config data for DWC_AC
 * Returns		: BOOL    - TRUE  : Registration successful Succeeded
 *				:           FALSE : Registration failed Failed
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_Create( DWCACConfig *config );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_Process
 * Description	: Main function for automatic conections
 *				: Call each frame until non-zero value is returned.
 * Arguments	: None.
 * Returns		: int -  0 : Processing
 *				:        1 : Connection complete
 *				:        2 : Connection complete, however multiple SSIDs for the same name have been discovered (limited to user-specified APs)
 *				:       -1 : Error occurred
 *---------------------------------------------------------------------------*/
int  DWC_AC_Process( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_GetStatus
 * Description	: Gets the DWC_AC library status during automatic connection
 * Arguments	: None.
 * Returns		: in  -  Positive value Current processing contents
 *				:       Negative values: Error details
 *---------------------------------------------------------------------------*/
int  DWC_AC_GetStatus( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_GetApType
 * Description	: Get type of AP that has been connected to
 *				: Get unsuccessful until after DWC_AC has succeeded
 * Arguments	: None.
 * Returns		: u8   -0 - 7 : Type of AP that has been connected to
 *				:        0xff : Get failed
 *---------------------------------------------------------------------------*/
u8   DWC_AC_GetApType( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_GetApData
 * Description	: Get detailed information on the AP that has been connected
 *				: Get unsuccessful until after DWC_AC has succeeded
 * Arguments	: *apdata - Buffer for storing ap data
 * Returns		: BOOL   - TRUE  : Registration successful Succeeded
 *				:          FALSE : Registration failed Failed
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_GetApData( DWCACApData *apdata );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_Destroy
 * Description	: Exit function : Closes the connection and frees the buffers
 *				: Be sure to call every frame until TRUE results
 * Arguments	: None.
 * Returns		: BOOL   - TRUE  : Registration successful Succeeded
 *				:          FALSE : Registration failed Failed
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_Destroy( void );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_SetSpecifyAp
 * Description	: performs connection settings for specified AP
 * Arguments	: *ssid   - SSID for connection AP
 *              : *wep    - WEP key for connection AP
 *              : wepMode - WEP key mode
 * Returns		: None.
 *---------------------------------------------------------------------------*/
void DWC_AC_SetSpecifyAp( const void *ssid, const void *wep, int wepMode );

/*---------------------------------------------------------------------------*
 * Name			: DWC_AC_CheckWiFiStation
 * Description	: Checks whether or not it is a Wi-Fi station from an AP SSID.
 * Arguments	: *ssid - SSID for scan AP
 *              : len   - SSID length
 * Returns		: BOOL  - TRUE  : Registration successful Succeeded
 *				:         FALSE : Registration failed Failed
 *---------------------------------------------------------------------------*/
BOOL DWC_AC_CheckWiFiStation( const void *ssid, u16 len );

#ifdef __cplusplus
}
#endif


#endif // DWC_AC_H_
