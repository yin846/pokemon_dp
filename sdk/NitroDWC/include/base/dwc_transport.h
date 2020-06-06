/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_transport
  File:     dwc_transport.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_transport.h,v $
  Revision 1.18  01/23/2006 04:50:43  arakit
  revised comments to match standardized terminology

  Revision 1.17  01/17/2006 07:22:35  arakit
  ・ return values for DWC_SetUserSendCallback(), DWC_SetUserRecvCallback(),
    DWC_SetUserRecvTimeoutCallback(), DWC_SetSendDrop(), DWC_SetRecvDrop(),
    DWC_SetSendDelay(), DWC_SetRecvDelay(), DWC_Ping(),
    DWC_SetUserPingCallback() and DWC_SetSendSplitMax() changed from void to BOOL,
    and changed to return FALSE without doing anything before allocating control variables.

  Revision 1.16  09/24/2005 12:56:30  arakit
  Fixed so the access functions for AID and the number of connections so that
  only hosts that have completely matching are handled as valid AIDs.

  Revision 1.15  2005/09/24 08:16:26  nakatat
  Added DWC_IsSendableReliable function for determining if sending is possible

  Revision 1.14  2005/09/24 08:17:12  nakatat
  Added DWC_SetSendSplitMax function for setting the maximum size to use when splitting send data
  Added DWC_SendReliable function for determining if sending is possible
  Revised related parts.

  Revision 1.13  09/22/2005 10:45:01  arakit
  Software made to clear part of sTransInfo connection when closing a connection.

  Revision 1.12  09/22/2005 09:56:21  nakatat
  Added comments

  Revision 1.11  09/22/2005 08:50:45  nakatat
  Added a receive timeout notification feature.
  （DWC_SetUserRecvTimeoutCallback、DWC_SetRecvTimeoutTime）
  Added related members to DWCstTransportConnection and DWCstTransportInfo.

  Revision 1.10  09/13/2005 11:10:56  arakit
  Corrected a problem where processing stopped due to a data access exception when an attempt was made to send to an aid that was already closed.
    Fixed bugs causing lock-up.
  -Function for getting an AID bitmap   u32 DWC_GetAIDBitmap(void),
    Functions for reliable sending with AID specification u32 DWC_SendReliableBitmap( u32 bitmap, const void* buffer, int size )、
    and unreliable sending with AID specification u32 DWC_SendUnreliableBitmap( u32 bitmap, const void* buffer, int size )
    have been prepared.

  Revision 1.9  09/01/2005 12:15:00  arakit
  Added a synchronization adjustment process during matching completion.

  Revision 1.8  2005/08/30 10:39:45  ito
  ・ Added DWCi_SendReliable
  ・ Corrected problem related to setting delay

  Revision 1.7  08/26/2005 08:15:23  arakit
  ・Organized ASSERT and check of module control variables.
  ・Changed code so that arguments of the external function DWC_ShutdownFriendsMatch() are not taken.
  Also, made code so that control variables for each module cannot be cleared to NULL by the same function.
  ・Changed the error processing function and inserted error setting and error checking where necessary.

  Revision 1.6  08/20/2005 08:01:20  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_TRANSPORT_H_
#define DWC_TRANSPORT_H_

#include <darray.h>

#ifdef  __cplusplus
extern "C" {
#endif

//#pragma warn_padding on


#define	DWC_MAGIC_STRINGS					"DT"	// Text string applied to Reliable send header	   
#define	DWC_MAGIC_STRINGS_LEN				2		// Length of text string applied to Reliable send header	   
#define	DWC_TRANSPORT_SEND_MAX				1465	// Maximum size that can be sent at once
#define	DWC_TRANSPORT_GT2HEADER_SIZE		7		// Size of header applied to gt2

/** -----------------------------------------------------------------
  Used for Reliable send control
  -------------------------------------------------------------------*/
enum
{
	DWC_TRANSPORT_SEND_READY,						// Data send possible.
	DWC_TRANSPORT_SEND_BUSY,						// Sending data

	DWC_TRANSPORT_SEND_LAST			
};

/** -----------------------------------------------------------------
  Used for receive control
  -------------------------------------------------------------------*/
enum
{
	DWC_TRANSPORT_RECV_NOBUF,						// Receive buffer not set
	DWC_TRANSPORT_RECV_HEADER,						// Wait to receive header
	DWC_TRANSPORT_RECV_BODY,						// Wait for data
    DWC_TRANSPORT_RECV_SYSTEM_DATA,					// Wait for data used internally
	DWC_TRANSPORT_RECV_ERROR,

	DWC_TRANSPORT_RECV_LAST			
};

/** -----------------------------------------------------------------
  Identifier for send data
  -------------------------------------------------------------------*/
enum
{
	DWC_SEND_TYPE_INVALID,
    DWC_SEND_TYPE_USERDATA,							// User data

    // ↓ Identifiers used with system data (internally used data) are listed below
    DWC_SEND_TYPE_MATCH_SYN,						// SYN used for matchmaking end sync
    DWC_SEND_TYPE_MATCH_SYN_ACK,					// SYN-ACK used for matchmaking end sync
    DWC_SEND_TYPE_MATCH_ACK,						// ACK used for matchmaking end sync
    // ↑List ends here

    DWC_SEND_TYPE_MAX
};

/** -----------------------------------------------------------------
  Structure typedef
  -------------------------------------------------------------------*/
typedef struct DWCstTransportInfo			DWCTransportInfo;
typedef struct DWCstTransportHeader			DWCTransportHeader;
typedef struct DWCstTransportConnection		DWCTransportConnection;
typedef	struct DWCstDelayedMessage			DWCDelayedMessage;

/** -----------------------------------------------------------------
  Function typedef
  -------------------------------------------------------------------*/
typedef void	(*DWCUserSendCallback)( int size, u8 aid );
typedef void	(*DWCUserRecvCallback)( u8 aid, u8* buffer, int size );
typedef void	(*DWCUserRecvTimeoutCallback)( u8 aid );
typedef void	(*DWCUserPingCallback)( int latency, u8 aid );

/** -----------------------------------------------------------------
  Structure used to control send/receive
  -------------------------------------------------------------------*/
// Data used to manage each connection
struct DWCstTransportConnection
{
	const u8*				sendBuffer;				// pointer to send buffer	
	u8*						recvBuffer;				// Pointer to receive buffer
	int						recvBufferSize;			// receive buffer size

	int						sendingSize;			// Size of data being sent
	int						recvingSize;			// Size of data being received
    int						requestSendSize;		// Send request size
	int						requestRecvSize;		// Receive request size

    u8						sendState;				// Send status	
	u8						recvState;				// Receive status

    u8						lastRecvState;			// Last receive status before receiving header
    u8						pads[3];
    u16						lastRecvType;			// Last header type received
    
	OSTick					previousRecvTick;
	u32						recvTimeoutTime;

#ifndef	SDK_FINALROM

    u16						sendDelay;				// Delay used during send
    u16						recvDelay;				// Delay used during receive

#endif	// SDK_FINALROM
};

// Send/receive control
struct DWCstTransportInfo
{
	// Data used to manage each connection
    DWCTransportConnection		connections[ DWC_MAX_CONNECTIONS ];

    DWCUserSendCallback			sendCallback;			// A transmission completion callback.
	DWCUserRecvCallback			recvCallback;			// Receive complete callback
	DWCUserRecvTimeoutCallback	recvTimeoutCallback;	// Receive timeout callback
	DWCUserPingCallback			pingCallback;			// Ping callback

	u16							sendSplitMax;			// Maximum split size for sending
    
#ifndef	SDK_FINALROM

    MATHRandContext16			context;				// Generate random number
	u32							seed;					// Generate random number

	DArray						delayedSend;			// Buffer used when emulating send delay
	DArray						delayedRecv;			// Buffer used when emulating receive delay

    u8							sendDrop;				// Send packet loss rate
    u8							recvDrop;				// Receive packet loss rate
	u8							pads[2];
    
#endif	// SDK_FINALROM
};	

/** -----------------------------------------------------------------
  Header used for Reliable send
  -------------------------------------------------------------------*/
struct DWCstTransportHeader
{
	int						size;					// Size of data being sent
    u16						type;					// Type of send data
    char					magicStrings[ DWC_MAGIC_STRINGS_LEN ];	// Text string applied to Reliable send header for identification	   
};

/** -----------------------------------------------------------------
  Structure used for delay emulation
  -------------------------------------------------------------------*/
struct DWCstDelayedMessage
{
    GT2Connection			connection;
    int						filterID;
    u8*						message;
    int						size;
    BOOL					reliable;
    OSTick					startTime;				// Time data was sent
    OSTick					delayTime;	    		// Delay time
};

/** -----------------------------------------------------------------
  External functions		
  -------------------------------------------------------------------*/
extern BOOL		DWC_SendReliable				( u8 aid, const void* buffer, int size );		// Reliable send
extern u32		DWC_SendReliableBitmap			( u32 bitmap, const void* buffer, int size );	// Reliable transfer using a bitmap specification
extern BOOL		DWC_SendUnreliable				( u8 aid, const void* buffer, int size );		// Unreliable transfer
extern u32		DWC_SendUnreliableBitmap		( u32 bitmap, const void* buffer, int size );	// Unreliable transfer using a bitmap specification
extern BOOL		DWC_Ping						( u8 aid );										// Measure ping value
extern BOOL		DWC_SetRecvBuffer				( u8 aid, void* recvBuffer, int size );			// Set the receive buffer
extern BOOL		DWC_SetUserSendCallback			( DWCUserSendCallback callback );				// Send callback
extern BOOL		DWC_SetUserRecvCallback			( DWCUserRecvCallback callback );				// Receive callback
extern BOOL		DWC_SetUserRecvTimeoutCallback	( DWCUserRecvTimeoutCallback callback );		// Receive timeout callback
extern BOOL		DWC_SetRecvTimeoutTime			( u8 aid, u32 time );							// Set the receive timeout interval (unit: msec)
extern BOOL		DWC_SetUserPingCallback			( DWCUserPingCallback callback );				// Callback for completing Ping measurement
extern BOOL		DWC_SetSendSplitMax				( u16 sendSplitMax );							// Sets the maximum size for split transfer
extern BOOL		DWC_IsSendableReliable			( u8 aid );										// Function for determining if Reliable transfer is possible

#ifndef	SDK_FINALROM

extern BOOL		DWC_SetSendDelay				( u16 delay, u8 aid );							// Set delay for sending host
extern BOOL		DWC_SetRecvDelay				( u16 delay, u8 aid );							// Set delay for receiving host
extern BOOL		DWC_SetSendDrop					( u8 drop, u8 aid );							// Set the packet loss rate for sending host
extern BOOL		DWC_SetRecvDrop					( u8 drop, u8 aid );							// Set the packet loss rate for receiving host

#else

#define			DWC_SetSendDelay( delay,  aid )	( (void) 0 )
#define			DWC_SetRecvDelay( delay,  aid )	( (void) 0 )
#define			DWC_SetSendDrop( drop, aid )	( (void) 0 )
#define			DWC_SetRecvDrop( drop, aid )	( (void) 0 )

#endif	// SDK_FINALROM

void			DWCi_InitTransport				( DWCTransportInfo* info );
BOOL			DWCi_IsSendableReliable			( u8 aid, u16 type );
BOOL			DWCi_SendReliable				( u16 type, u8 aid, const void* buffer, int size );
void			DWCi_RecvCallback				( GT2Connection	connection, GT2Byte* message, int size, GT2Bool	reliable );
void		    DWCi_PingCallback				( GT2Connection connection, int latency );
void			DWCi_TransportProcess			( void );
void			DWCi_ShutdownTransport			( void );
void			DWCi_ClearTransConnection		( u8 aid );


//#pragma warn_padding reset


#ifdef  __cplusplus
}		/* extern "C"*/
#endif


#endif // DWC_TRANSPORT_H_
