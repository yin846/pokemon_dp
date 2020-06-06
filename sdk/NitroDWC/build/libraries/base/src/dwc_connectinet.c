/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - connectinet
  File:     dwc_connectinet.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_connectinet.c,v $
  Revision 1.50  07/27/2006 01:03:02  nakata
  Made changes so that the error code is set to 540xx with DWC_UpdateConnection

  Revision 1.49  06/27/2006 06:43:52  nakata
  Added a DCF watch function for where there is no FriendsMatch library

  Revision 1.48  06/01/2006 07:48:55  nakata
  Suppressed the warning displayed at FINALROM build

  Revision 1.47  05/25/2006 23:54:11  uemura
  Added the DWC_CheckWiFiStation function

  Revision 1.46  04/17/2006 07:33:40  nakata
  Added the DWC_EnableHotSpot function, which configures hotspots for browsers

  Revision 1.45  03/09/2006 08:05:52  uemura
  changed argument type for DWC_SetConnectApType from int to DWCApInfoType

  Revision 1.44  03/09/2006 07:35:13  nakata
  deleted spotinfo from DWCApInfo, changed related portions

  Revision 1.43  03/09/2006 06:07:41  uemura
  fixed bug where value couldn't be accurately determined when a hexadecimal 0 was used in the WEP argument for DWC_DebugConnectInetAsync
  changed so that the argument passed WEPMODE

  Revision 1.42  03/09/2006 04:38:38  uemura
  Fixed comments.

  Revision 1.41  03/09/2006 02:50:37  uemura
  Fixed comments.

  Revision 1.40  03/09/2006 02:27:17  uemura
  added DWC_SetConnectApType and DWC_DebugConnectInetAsync
  added these two functions so they can be used with the dwc_connect_inet_sample compile switch

  Revision 1.39  02/24/2006 10:31:29  sotoike
  Fixed possibility of accessing a NULL pointer in the DWC_CleanupInet function.

  Revision 1.38  02/23/2006 08:21:56  nakata
  changed the names of the constants associated with DWCApInfo

  Revision 1.37  02/23/2006 06:48:34  nakata
  Fixed a bug where invalid data was returned by the DWC_GetApInfo function.

  Revision 1.36  02/23/2006 04:32:07  sotoike
  (none)

  Revision 1.35  02/23/2006 02:55:40  nakata
  added documentation related to DWC_GetApInfo function

  Revision 1.34  02/23/2006 01:02:19  nakata
  fixed a bug in the DWC_GetApInfo function that gets AP data

  Revision 1.33  02/23/2006 00:53:54  nakata
  added the function getting AP data, DWC_GetApInfo, to dwc_connectinet.c
  suppressed unneeded debug messages in dwc_sample

  Revision 1.32  02/22/2006 05:10:36  sotoike
  when stpInetCntl is NULL (i.e., when DWC_InitInet has not been called)
  fixed a hanging bug when DWC_ProcessInet was called
  Revision 1.31  02/22/2006 04:29:44  sotoike
  fixed a bug if DWC_GetInetStatus was called between DWC_InitInet and
  DWC_ConnectInetAsync when accessing a NULL pointer caused a hang

  Revision 1.30  01/23/2006 04:50:26  arakit
  revised comments to match standardized terminology

  Revision 1.29  12/27/2005 09:29:54  arakit
  Made DWC_GetUdpPacketDropNum() run normally even in the FINALROM build.

  Revision 1.28  11/07/2005 06:18:30  sasakit
  Changed GsStatus to OwnStatus.

  Revision 1.27  10/18/2005 00:08:25  sasakit
  Gave up on a function to lower thread priority during DWC auth connections.

  Revision 1.26  2005/10/17 07:31:44  sasakit
  Deleted a function for adjusting thread priority during CPS SSL execution.

  Revision 1.25  2005/10/17 06:35:56  sasakit
  Activated a function for adjusting thread priority during SSL execution. (added include)
  Deleted warning

  Revision 1.24  2005/10/17 06:34:33  sasakit
  Activated a function for adjusting thread priority during SSL execution.

  Revision 1.23  10/13/2005 08:14:09  arakit
  Made specifications so that a debug print display is made when the report level is DWC_REPORTFLAG_DEBUG when calling major external functions.
  Changed to enable debug print display.

  Revision 1.22  10/11/2005 02:00:12  sasakit
  Corrected a problem where CleanupInet could not be executed normally when an error occurred during a connection.
  Corrected a problem where SOCL_CalmDown() could not be called prior to SOC_Startup(). (The call has been moved to AC)

  Revision 1.21  10/06/2005 05:59:34  sasakit
  Made specifications so that functions for lowering the priority level of  thread can be adjusted externally when calculating powers for SSL.

  Revision 1.20  10/05/2005 07:10:11  sasakit
  Added a function for lowering the priority level of threads that calculate powers for SSL.

  Revision 1.19  10/01/2005 14:57:33  sasakit
  Corrected a problem where DWC_GetInetStatus() wouldstop when accessing a NULL pointer.

  Revision 1.18  10/01/2005 12:07:01  sasakit
  Corrected problem in ConnectInet processing

  Revision 1.17  10/01/2005 10:56:44  sasakit
  Made changes so that DISCONNECTEd does not result even when DWC_ProcessInet() is called from within DWC_CleanupInetAsync()

  Revision 1.16  09/29/2005 09:06:53  sasakit
  Made changes so that DWC_CleanupInetAsync() is blocked as little as possible. (Support for SOCL_CalmDown)

  Revision 1.15  09/27/2005 13:22:41  sasakit
  Corrected problem where the connection result (error status) was not being placed in DWC_GetLastError().

  Revision 1.14  09/20/2005 10:56:51  sasakit
  Returned to original status.
  >> Since conntest.nintendowifi.net has been dropped, changed code to access www.nintendo.co.jp.

  Revision 1.13  09/19/2005 18:57:35  sasakit
  Since conntest.nintendowifi.net has been dropped, changed code to access www.nintendo.co.jp.

  Revision 1.12  09/17/2005 00:32:28  sasakit
  Added function for getting the number of dropped UDP packets.
  Revised the implementation of DWC_GetLinkLevel().

  Revision 1.11  09/14/2005 01:55:00  sasakit
  Organized processing for disconnecting from the AP.

  Revision 1.10  09/13/2005 07:29:16  sasakit
  Made sure that PowerMode is always set to ACTIVE.

  Revision 1.9  09/07/2005 07:13:34  sasakit
  Inserted statements for strictly calling DWC_AC functions.
  Made it so that calling DWC_InitInet() twice does not result in an error. (Made it so this is ignored.)

  Revision 1.8  09/06/2005 09:12:09  sasakit
  Added processing of errors used during ConnectInet

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitroWiFi/socl.h>

#include <ac/dwc_ac.h>
#include <base/dwc_connectinet.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_report.h>
#include <base/dwc_error.h>

#include <auth/dwc_auth.h>
#include <auth/dwc_netcheck.h>

/** --------------------------------------------------------------------
  defines
  ----------------------------------------------------------------------*/
#define DWC_CONNECTINET_DMA_DEFAULT      3
#define DWC_CONNECTINET_POWERMODE_SAVE   0
#define DWC_CONNECTINET_POWERMODE_ACTIVE 1

#define DWC_CONNECTINET_SSL_DEFAULT_PRIORITY 20

/** --------------------------------------------------------------------
  statics
  ----------------------------------------------------------------------*/
static DWCInetControl* stpInetCntl;

/*---------------------------------------------------------------------------*
  Name:         DWC_InitInetEx

  Description:  Initialization of Internet connection management

  Arguments:    inetCntl  : Pointer to the structure used to manage network connections
                dmaNo : dma number to be used (used by wm)
                powerMode : WM power mode setting
                ssl_priority : Priority of ssl thread

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_InitInetEx( DWCInetControl* inetCntl, u16 dmaNo, u16 powerMode, u16 ssl_priority )
{
#ifdef SDK_FINALROM
#pragma unused(powerMode)
#endif
    SDK_ASSERT( powerMode == DWC_CONNECTINET_POWERMODE_ACTIVE );

    if ( stpInetCntl == NULL )
    {
        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "InitInet dma : %d, powerMode: %d\n", dmaNo, powerMode );

        SDK_ASSERT( inetCntl );

        MI_CpuClear8( inetCntl, sizeof( DWCInetControl ) );

        inetCntl->dmaNo = dmaNo;
        inetCntl->powerMode = DWC_CONNECTINET_POWERMODE_ACTIVE;

        inetCntl->state = DWC_CONNECTINET_STATE_IDLE;
        inetCntl->online = FALSE;

#ifndef SDK_FINALROM
		inetCntl->connectType = 0;
#endif	// SDK_FINALROM

        stpInetCntl = inetCntl;

        // The default authentication server is the test server
        DWC_SetAuthServer( DWC_CONNECTINET_AUTH_TEST );

#if SDK_WIFI_VERSION_DATE >= 20051005
        CPS_SetSslLowThreadPriority( ssl_priority );
#else
        (void)ssl_priority;
#endif
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_InitInet

  Description:  Initialization of Internet connection management

  Arguments:    inetCntl  : Pointer to the structure used to manage network connections

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_InitInet( DWCInetControl* inetCntl )
{
    DWC_InitInetEx( inetCntl, DWC_CONNECTINET_DMA_DEFAULT, DWC_CONNECTINET_POWERMODE_ACTIVE, DWC_CONNECTINET_SSL_DEFAULT_PRIORITY );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_SetAuthServer

  Description:  Changed server to be used for authentication

  Arguments:    type: Authentication server type

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_SetAuthServer( DWCInetAuthType type )
{
    switch ( type )
    {
    case DWC_CONNECTINET_AUTH_TEST:
        DWC_Auth_SetCustomNas("https://nas.test.nintendowifi.net/ac");
        break;

    case DWC_CONNECTINET_AUTH_DEVELOP:
        OS_TPrintf("******************************************************************************************\n");
        OS_TPrintf(" DWC_CONNECTINET_AUTH_DEVELOP: This setting is for authentication server developing ONLY. \n");
        OS_TPrintf("******************************************************************************************\n");
        DWC_Auth_SetCustomNas("https://nas.dev.nintendowifi.net/ac");
        break;

    case DWC_CONNECTINET_AUTH_RELEASE:
        DWC_Auth_SetCustomNas("https://nas.nintendowifi.net/ac");
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_SetConnectApType

  Description:  specifies the AP type for connecting to the Internet

  Arguments:    type: connection's AP type

  Returns:      None
 *---------------------------------------------------------------------------*/
#ifndef SDK_FINALROM
void
DWC_SetConnectApType( DWCApInfoType type )
{
	SDK_ASSERT( 0 <= type && type < DWC_APINFO_TYPE_OTHER );
    OS_TPrintf("***************************************************************\n");
    OS_TPrintf(" DWC_CONNECTINET_SET_AP_TYPE: This setting is debug ONLY. \n");
    OS_TPrintf("***************************************************************\n");
	// specifies the AP type for the connection
	stpInetCntl->connectType = type + 1;
}
#endif	// SDK_FINALROM

/*---------------------------------------------------------------------------*
  Name:         DWC_ConnectInetAsync

  Description:  Connects to the Internet.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_ConnectInetAsync( void )
{
    DWCACConfig inp;

    if ( stpInetCntl != NULL )
    {
        if ( stpInetCntl->state != DWC_CONNECTINET_STATE_IDLE )
        {
            return;
            // NOT REACH HERE
        }

        MI_CpuClear8( &inp, sizeof(DWCACConfig) );

        inp.dmaNo     = (u8)stpInetCntl->dmaNo;       //DMA number
        inp.powerMode = (u8)stpInetCntl->powerMode;   //‚OFSAVE ‚PFACTIVE
        inp.alloc     = (DWCACAlloc)DWC_Alloc;    //Pointer to alloc function used during automatic connection processing
        inp.free      = (DWCACFree) DWC_Free;     //Pointer to free function used during automatic connection processing

#ifndef SDK_FINALROM
		// specifies the connection AP type
		inp.option.connectType = (u8)stpInetCntl->connectType;
#endif	// SDK_FINALROM

        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "ConnectInet Async\n" );

        stpInetCntl->state = DWC_CONNECTINET_STATE_OPERATING;

        if ( !DWC_AC_Create(&inp) )
        {
            // Process flow arrives here if WCM_ initialization fails.
            DWCi_SetError( DWC_ERROR_FATAL, DWC_ECODE_TYPE_WM_INIT );
        }
    }
    else
    {
        // Not initialized.
        DWCi_SetError( DWC_ERROR_FATAL, DWC_ECODE_TYPE_NOT_INIT );
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_DebugConnectInetAsync

  Description:  Specifies the AP and connects to the Internet.

  Arguments:    ssid    : connection's AP SSID
                wep     : connection's AP WEP key
                wepMode : the WEP key mode

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_DebugConnectInetAsync( const void *ssid, const void *wep, int wepMode )
{
	DWC_ConnectInetAsync();

	if( DWC_GetInetStatus() == DWC_CONNECTINET_STATE_OPERATING )
	{
		// resets the connection target
		DWC_AC_SetSpecifyAp( ssid, wep, wepMode );
	}
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CheckInet

  Description:  Checks the progress of the process connecting to the Internet.

  Arguments:    None

  Returns:      TRUE  : Process end
                FALSE : Processing
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckInet( void )
{
    if ( stpInetCntl == NULL )
    {
        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "ConnectInet library is not initialized.\n" );

        return FALSE;
    }

    if ( stpInetCntl->ac_state != 0 )
    {
        stpInetCntl->state = DWC_CONNECTINET_STATE_OPERATED;

        (void)DWC_GetInetStatus(); // Stores the result in DWC_GetLastError().

        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_ProcessInet

  Description:  Executes processing for connecting to the Internet.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_ProcessInet( void )
{
  if( stpInetCntl == NULL){ return; } // stage where DWC_InitInet has not been called

    if ( stpInetCntl->state == DWC_CONNECTINET_STATE_OPERATING )
    {
        stpInetCntl->ac_state = DWC_AC_Process();
    }
    else if ( stpInetCntl->state == DWC_CONNECTINET_STATE_CONNECTED )
    {
        // DCF connection check
        if ( stpInetCntl->online )
        {
            if ( WCM_GetPhase() != WCM_PHASE_DCF )
            {
                // Disconnected.
                stpInetCntl->online = FALSE;
                stpInetCntl->state = DWC_CONNECTINET_STATE_DISCONNECTED;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_GetInetStatus

  Description:  Gets the status after processing for connecting to the Internet has completed.

  Arguments:    None

  Returns:      Status
 *---------------------------------------------------------------------------*/
DWCInetResult
DWC_GetInetStatus( void )
{
  int state;
  DWCInetResult result = DWC_CONNECTINET_STATE_NOT_INITIALIZED;

  if ( stpInetCntl == NULL ){ return (result); } // uninitialized

  result =  (DWCInetResult)stpInetCntl->state;
  
  if(result == DWC_CONNECTINET_STATE_IDLE){ return (result); } // DWC_AC_GetStatus() cannot be called yet at this stage

  state = DWC_AC_GetStatus();

  if ( state ==  DWC_AC_STATE_COMPLETE )
	{
	  // Already connected.
	  result = DWC_CONNECTINET_STATE_CONNECTED;
	  stpInetCntl->state = result;
	  stpInetCntl->online = TRUE;
	}
  else if ( state < 0 )
	{
	  // Error processing.
	  if ( state >= -10 )
		{
		  // FATAL error
		  DWCi_SetError( DWC_ERROR_FATAL, DWC_ECODE_TYPE_AC_FATAL + state );
		  
		  result = DWC_CONNECTINET_STATE_FATAL_ERROR;
		  stpInetCntl->state = result;
		}
	  else
		{
		  // Error generated by AC.
		  DWCi_SetError( DWC_ERROR_AC_ANY, state );
		  
		  result = DWC_CONNECTINET_STATE_ERROR;
		  stpInetCntl->state = result;
		}
	}
  else
	{
	  // Processing.
	  result = DWC_CONNECTINET_STATE_OPERATING;
	}
  
  return result;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CleanupInet

  Description:  Closes the Internet connection and frees associated memory.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_CleanupInet( void )
{
    // [arakit] main 051013
    DWC_Printf( DWC_REPORTFLAG_DEBUG, "!!DWC_CleanupInet() was called!!\n" );
    
    if ( stpInetCntl == NULL )
    {
        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "ConnectInet library is not initialized.\n" );
        // Does nothing since initialization has not been performed yet.
        return;
    }

	if(stpInetCntl->state == DWC_CONNECTINET_STATE_IDLE){ // no memory has yet been allocated
	  stpInetCntl = NULL; // only the use of the controls is stopped
	  return; 
	}

    // Cleanup processing.
    while ( !DWC_AC_Destroy() )
    {
        OS_Sleep( 10 );
    }

    stpInetCntl = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CleanupInetAsync

  Description:  Closes the Internet connection and frees associated memory. (asynchronous version)
                Call each game frame until TRUE is returned.

  Arguments:    None

  Returns:      TRUE  : Cleanup has ended.
                FALSE : Currently cleaning up.
 *---------------------------------------------------------------------------*/
BOOL
DWC_CleanupInetAsync( void )
{
    // [arakit] main 051013
    DWC_Printf( DWC_REPORTFLAG_DEBUG, "!!DWC_CleanupInetAsync() was called!!\n" );
    
    if ( stpInetCntl == NULL )
    {
        DWC_Printf( DWC_REPORTFLAG_CONNECTINET, "ConnectInet library is not initialized.\n" );
        // Does nothing since initialization has not been performed yet.
        return TRUE;
    }


    // Only when in a stable state.
    if ( stpInetCntl->state == DWC_CONNECTINET_STATE_FATAL_ERROR )
    {
        // Does nothing if there is a FATAL error.
        // [arakit] main 051013
        DWC_Printf( DWC_REPORTFLAG_DEBUG, "CleanupInet Status: FATAL ERROR.\n" );
        return FALSE;
    }
    else if ( stpInetCntl->state == DWC_CONNECTINET_STATE_IDLE ) // Cleared and terminated when IDLE.
    {
        DWC_Printf( DWC_REPORTFLAG_DEBUG, "CleanupInet Status: %d\n", stpInetCntl->state );

        stpInetCntl = NULL;

        return TRUE;
    }
    else
    {
        stpInetCntl->state = DWC_CONNECTINET_STATE_DISCONNECTING;
    }

#if 0
#if SDK_WIFI_VERSION_DATE >= 20050929
    // Wait for all TCPs to close.
    if ( SOCL_CalmDown() != SOCL_ESUCCESS )
    {
        // closing.
        DWC_Printf( DWC_REPORTFLAG_DEBUG, "CleanupInet Status: %d %d\n", stpInetCntl->state, SOCL_CalmDown() );

        return FALSE;
    }
#endif
#endif

    // AC release processing.
    if ( DWC_AC_Destroy() )
    {
        // End connection processing.
        stpInetCntl = NULL;

        return TRUE;
    }

    return FALSE;
}

BOOL
DWCi_CheckDisconnected( void )
{
    if ( stpInetCntl != NULL && stpInetCntl->state == DWC_CONNECTINET_STATE_DISCONNECTED )
    {
        return TRUE;
    }

    return FALSE;
}

WMLinkLevel
DWC_GetLinkLevel( void )
{
#if SDK_WIFI_VERSION_DATE >= 20050914
    return WCM_GetLinkLevel();
#else
    return WM_GetLinkLevel();
#endif
}

int
DWC_GetUdpPacketDropNum( void )
{
//#ifndef SDK_FINALROM
#if SDK_WIFI_VERSION_DATE >= 20050914
    return (int)SOCLi_CheckCount[ SOCL_CHECKCOUNT_UDPRCVCB_2 ];
#else
    return 0;
#endif
//#else
//    return 0;
//#endif
}

// [nakata] from here is the expanded AP data get function

// [nakata] external function definitions
extern u8* WCM_GetApMacAddress( void );
extern u8* WCM_GetApEssid     ( u16* length );

/*---------------------------------------------------------------------------*
  Name:         DWC_GetApInfo

  Description:  Gets the data specific to the AP currently connected.

  Arguments:    apinfo  : pointer to the memory region where the gotten data is written

  Returns:      TRUE  : get was successful
	            FALSE : get failed
 *---------------------------------------------------------------------------*/
BOOL DWC_GetApInfo(DWCApInfo *apinfo)
{
	DWCACApData	apdata;
	
	OSIntrMode	preirq;
	unsigned char	*wcmapbssid;
	unsigned char	*wcmapessid;
	unsigned short	essidlen;
	
	// [nakata] 0 clear of target storage area
	MI_CpuClear8(apinfo, sizeof(DWCApInfo));
	apinfo->area = DWC_APINFO_AREA_UNKNOWN;

	// [nakata] returns FALSE if no Internet connection has been established
	if(DWC_GetInetStatus() != DWC_CONNECTINET_STATE_CONNECTED)
		return FALSE;
	
	// [nakata] gets the AP type
	apinfo->aptype = (DWCApInfoType)DWC_AC_GetApType();
	
	// [nakata] gets the BSSID within apinfo
	preirq = OS_DisableInterrupts();
	wcmapbssid = WCM_GetApMacAddress();
	DC_InvalidateRange(wcmapbssid, WM_SIZE_BSSID);
	if(wcmapbssid == NULL) {
		preirq = OS_RestoreInterrupts(preirq);
		return FALSE;
	}
	MI_CpuCopy8(wcmapbssid, apinfo->bssid, 6);
	
	// [nakata] when necessary, gets the ESSID within apinfo
	if(apinfo->aptype >= 0 && apinfo->aptype < 3) {
		wcmapessid = WCM_GetApEssid(&essidlen);
		DC_InvalidateRange(wcmapessid, WM_SIZE_SSID);
		if(wcmapessid == NULL) {
			preirq = OS_RestoreInterrupts(preirq);
			return FALSE;		
		}
		MI_CpuCopy8(wcmapessid, apinfo->essid, essidlen);
	}
	preirq = OS_RestoreInterrupts(preirq);
	
	// [nakata] gets the APDATA analysis results within apinfo
	MI_CpuClear8(&apdata, sizeof(DWCACApData));
	if(DWC_AC_GetApData(&apdata) == TRUE && apdata.data[0] != 0x00) {
		apinfo->area		= (DWCApInfoArea)(apdata.data[0] - '0');
		MI_CpuCopy8(&apdata.data[1], apinfo->spotinfo, DWC_SIZE_SPOTINFO);
	}
	
	return TRUE;
}
// [nakata] to here was the expanded AP data get function

// [nakata] from here is the expanded browser support section

/*---------------------------------------------------------------------------*
  Name:         DWC_EnableHotspot

  Description:  Enables the use of hotspots that the Nintendo Wi-Fi Connection does not support

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void DWC_EnableHotspot(void)
{
	DWC_Netcheck_SetAllow302(TRUE);
}

// [nakata] expanded browser support section ends here

// [uemura] 2006/05/24 Additions
/*---------------------------------------------------------------------------*
  Name:         DWC_CheckWiFiStation

  Description:  Checks to see if it is a Wi-Fi station.

  Arguments:    ssid    : AP SSID to check
                len    : SSID length

  Returns:      TRUE  : Wi-Fi Station
                FALSE : Not a Wi-Fi Station
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckWiFiStation( const void *ssid, u16 len )
{
	return DWC_AC_CheckWiFiStation( ssid, len );
}

// [uemura] ends here

// [nakata] 06/06/27 moved from dwc_main.c
/*---------------------------------------------------------------------------*
  DCF communicaton processing update function
  Arguments : None
  Return value : Disconnected when TRUE
  Use : Called every game frame to monitor DCF communications.
 *---------------------------------------------------------------------------*/
BOOL DWC_UpdateConnection( void )
{
    DWC_ProcessInet();

    if ( DWCi_CheckDisconnected() )
    {
		u8 aptype = DWC_AC_GetApType();
        // Disconnected.
		// Configure special error code that goes in the AC error range
        DWCi_SetError( DWC_ERROR_DISCONNECTED, -54000-aptype );

        // [arakit] main 051013
        DWC_Printf( DWC_REPORTFLAG_DEBUG,
                    "Detected DCF shutdown and call DWC_CleanupInet().\n" );

        DWC_CleanupInet();

        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  DCF communicaton processing update function
  Arguments : None
  Return value: None
  Use : Called when DCF has been disconnected. Put code for any required processing here.
 *---------------------------------------------------------------------------*/
void DWC_ForceShutdown( void )
{
    // Functions which can be forcibly called without problem are called here.
}
// [nakata] ends here
