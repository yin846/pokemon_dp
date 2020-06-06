/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  -
  File:     dwc_ranking.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_ranking.c,v $
  Revision 1.13  06/21/2006 01:04:22  hayashi
  changed to accept DWC_RnkPutScoreAsync with no user defined data.

  Revision 1.12  06/16/2006 04:51:58  hayashi
  fixed the wrong comment

  Revision 1.11  05/30/2006 14:08:50  hayashi
  added DWC_RnkResGetTotal

  Revision 1.10  05/26/2006 02:07:44  hayashi
  change the limit of number at getting friend ranking

  Revision 1.9  05/23/2006 12:49:31  hayashi
  exclude dwc_serverurl_internal.h including

  Revision 1.8  05/22/2006 07:29:58  hayashi
  changed to be set the server by DWC_SetAuthServer()

  Revision 1.7  05/12/2006 08:28:54  hayashi
  NOTINITIALIZED -> NOTREADY
  RIVAL -> NEAR
  DWCAccUserData -> DWCUserData
  BOOL	DWC_RnkShutdown() -> DWCRnkError

  Revision 1.6  04/26/2006 09:25:40  hayashi
  specification is changed related to PID handling

  Revision 1.5  04/24/2006 10:12:12  hayashi
  change the limit value of parameter checking  ==0 to <=1

  Revision 1.4  04/13/2006 11:35:19  hayashi
  Change prefix Ranking->Rnk

  Revision 1.3  04/13/2006 09:01:38  hayashi
  character code converting

  Revision 1.2  04/11/2006 09:01:33  hayashi
  Made the connection target server selectable with DWC_RnkInitialize

  Revision 1.1  04/10/2006 10:16:58  hayashi
  added the ranking module


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include <nitro.h>

#include <string.h>
#include <stdlib.h>

#include <base/dwc_account.h>
#include <base/dwc_ranking.h>
#include <base/dwc_error.h>
#include <base/dwc_memfunc.h>

#include <auth/dwc_http.h>

#include <ranking/dwc_ranksession.h>

/* -------------------------------------------------------------------------
        pre-processing switch
   ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

//
// This is also defined on dwc_serverurl_internal.h, but it is directly written to keep it simple in the structure
//
#define DWC_AUTH_NAS_URL		"https://nas.nintendowifi.net/ac"

/* -------------------------------------------------------------------------
        macro
   ------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

// Structure used to parse the initialization character string passed to DWC_RnkInitialize
typedef union{

	char	data[84];

	struct{
		char	secretkey[20];	// Private key
		char	randkey_1[8];	// Internal encryption key 1
		char	randkey_2[8];	// Internal encryption key 2
		char	randkey_3[8];	// Internal encryption key 3
		char	randkey_4[8];	// Internal encryption key 4
		char	gamename[32];	// Game name
	}info;

}	DWCiRankingInitData;

// Structure used to take in reception data
//
typedef struct{

	u32					size;	// Size (in bytes)
	DWCRnkGetMode		mode;	// Mode

	union{

		void*				header;

		struct{								// Header when getting the ranking list
											//
			u32	count;						//   Number of lines received
											//
			u32	total;						//   Total number
											//
			DWCRnkData	rankingdata;		//   Top pointer of the ranking data

		}				*listheader;

		struct{								// Header when getting the order
											//
			u32	order;						//   Order
											//
			u32	total;						//   Total number

		}				*orderheader;

	};

} DWCiRankingResponse;


// Reference the address of the authentication server in order to get the connection destination server
//
extern DWCHttpParam DWCauthhttpparam;

/* -------------------------------------------------------------------------
        variable
   ------------------------------------------------------------------------- */

// State management structure for the ranking module
struct {

	DWCRnkState	state;		// Status

	s32			pid;		// GameSpy Profile ID

}	g_rankinginfo	=		// Initial data
						{
							DWC_RNK_STATE_NOTREADY,	// Status
						};


/* -------------------------------------------------------------------------
        prototype
   ------------------------------------------------------------------------- */

DWCRnkError	DWCi_RankingGetResponse( DWCiRankingResponse* out );


/* -------------------------------------------------------------------------
        function - internal
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:		DWCi_RankingGetResponse

  Description:	Gets the data received on the network.
				The obtained buffer remains valid until it is deallocated by the
				DWC_RnkShutdown() call.

  Arguments:	out	The pointer to the DWCiRankingResponse structure that stores the results.

  Returns:		DWC_RNK_SUCCESS					Succeeded
  				DWC_RNK_IN_ERROR				Error occurring
  				DWC_RNK_ERROR_NOTCOMPLETED		Communications not completed
				DWC_RNK_ERROR_EMPTY_RESPONSE	Empty response
 *---------------------------------------------------------------------------*/

DWCRnkError	DWCi_RankingGetResponse( DWCiRankingResponse* out ){

	void* buf;
	u32 size;

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Status check
	//
	if( g_rankinginfo.state != DWC_RNK_STATE_COMPLETED ){
	
		// Networking is not complete
		return DWC_RNK_ERROR_NOTCOMPLETED;
	
	}

	// Get response data
	// 
	buf = DWCi_RankingSessionGetResponse( &size );

	if( size == 0 ){
	
		// Empty response data
		return DWC_RNK_ERROR_EMPTY_RESPONSE;
	
	}

	// Set to return value
	//
	out->size = size;
	out->mode = (DWCRnkGetMode)((u32*)buf)[0];
	out->header = &((u32*)buf)[1];

	return DWC_RNK_SUCCESS;

}


/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

/*---------------------------------------------------------------------------*
  Name:		DWC_RnkInitialize

  Description:	Initializes the ranking module.
  				If already initialized or if the initialization data is incorrect, this function will fail.
  				The private key string for networking is issued by the operator,
  				so this is unique on the server.

  Arguments:	initdata		The private key string for networking
  				userdata		User data

  Returns:
	DWC_RNK_SUCCESS							Succeeded
	DWC_RNK_ERROR_INIT_ALREADYINITIALIZED	Initialized
	DWC_RNK_ERROR_INIT_INVALID_INITDATASIZE	Invalid initialization data size
	DWC_RNK_ERROR_INIT_INVALID_INITDATA		Invalid initialization data
	DWC_RNK_ERROR_INIT_INVALID_USERDATA		Invalid user data

 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkInitialize(	const char* initdata,
								const DWCUserData* userdata ){

	u32 randkey_1,randkey_2,randkey_3,randkey_4;
	DWCiRankingInitData* ptr;
	char secretkey[21];
	char buf[9] = "";

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Status check
	if( g_rankinginfo.state != DWC_RNK_STATE_NOTREADY ){
	
		// Initialization complete
		return DWC_RNK_ERROR_INIT_ALREADYINITIALIZED;
	
	}

	// Inspect the validity of the user data
	if( !DWC_CheckUserData( userdata )
		|| !DWC_CheckHasProfile( userdata )){

		// Invalid user data
		return DWC_RNK_ERROR_INIT_INVALID_USERDATA;

	}

	// Initialization data check
	if( strlen( initdata ) >= sizeof( DWCiRankingInitData ) ){

		// Size is invalid
		return DWC_RNK_ERROR_INIT_INVALID_INITDATASIZE;

	}

	// Cast to pointer
	ptr = (DWCiRankingInitData*)initdata;

	// Get private key
	strncpy( secretkey, ptr->info.secretkey, 20 );
	secretkey[20] = '\0';

	// Internal encryption key recovery
	randkey_1 = strtoul( strncpy( buf, ptr->info.randkey_1, 8 ), NULL, 16 );
	randkey_2 = strtoul( strncpy( buf, ptr->info.randkey_2, 8 ), NULL, 16 );
	randkey_3 = strtoul( strncpy( buf, ptr->info.randkey_3, 8 ), NULL, 16 );
	randkey_4 = strtoul( strncpy( buf, ptr->info.randkey_4, 8 ), NULL, 16 );

	// Inspect the validity of the internal encryption key
	if( !DWCi_RankingValidateKey(	ptr->info.gamename,
									secretkey,
									randkey_1,
									randkey_2,
									randkey_3,
									randkey_4) ){
	
		// Invalid key
		return DWC_RNK_ERROR_INIT_INVALID_INITDATA;
	
	}

	// Keep the GameSpyProfileID
	g_rankinginfo.pid = userdata->gs_profile_id;

	// Select server to connect to
	if( strcmp( DWCauthhttpparam.url, DWC_AUTH_NAS_URL ) == 0 ){
	
		// Initialize HTTP module (live server)
		DWCi_RankingSessionInitialize( TRUE );

	}else{
	
		// Initialize HTTP module (development server)
		DWCi_RankingSessionInitialize( FALSE );

	}

	// Set status
	g_rankinginfo.state = DWC_RNK_STATE_INITIALIZED;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkShutdown

  Description:	Deallocates the resources used by the ranking module.
				Since the reception buffer is deallocated, the buffer obtained with
				DWCi_RankingGetResponse() is invalid.

  Arguments:	None

  Returns:		DWC_RNK_SUCCESS	Succeeded
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkShutdown(){

	// Deallocate HTTP module
	DWCi_RankingSessionShutdown();

	// Set status
	g_rankinginfo.state = DWC_RNK_STATE_NOTREADY;

	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkPutScoreAsync

  Description:	Starts the asynchronous process that records the score
  				

  Arguments:	category	Category
				region		Region code
  				score		score
				data		Pointer to the buffer for the user-defined data
				size		Data size (DWC_RNK_DATA_MAX or lower)

  Returns:		DWC_RNK_SUCCESS						Succeeded
  				DWC_RNK_IN_ERROR					Error occurring
  				DWC_RNK_ERROR_PUT_NOTREADY			uninitialized
				DWC_RNK_ERROR_INVALID_PARAMETER		Invalid parameter
				DWC_RNK_ERROR_PUT_INVALID_KEY		Encryption key is invalid
				DWC_RNK_ERROR_PUT_NOMEMORY			insufficient memory
 *---------------------------------------------------------------------------*/
DWCRnkError	DWC_RnkPutScoreAsync(	u32 category,
									DWCRnkRegion region,
									s32 score,
									void* data,
									u32 size ){

	DWCiRankingSessionResult res;

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Status check
	//
	if( ( g_rankinginfo.state != DWC_RNK_STATE_INITIALIZED ) 
		&& (g_rankinginfo.state != DWC_RNK_STATE_COMPLETED ) ){
	
		// not initialized
		return DWC_RNK_ERROR_PUT_NOTREADY;
	
	}

	// Parameter check
	//
	if( (category > DWC_RNK_CATEGORY_MAX)
		|| (size > DWC_RNK_DATA_MAX)){
	
		// Invalid parameter
		return DWC_RNK_ERROR_INVALID_PARAMETER;
	
	}

	if( (data == NULL) && (size != 0) ){
	
		// Invalid parameter
		//
		// When the user-defined data pointer is NULL, the size must also be 0.
		//
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	}

	// Request issue
	//
	res = DWCi_RankingSessionPutAsync(	category,
										g_rankinginfo.pid,
										region,
										score,
										data,
										size );

	// Error processing
	switch( res ){

	case DWCi_RANKING_SESSION_ERROR_INVALID_KEY:
		return DWC_RNK_ERROR_PUT_INVALID_KEY;

	case DWCi_RANKING_SESSION_ERROR_NOMEMORY:
		return DWC_RNK_ERROR_PUT_NOMEMORY;

	}

	// Set status
	g_rankinginfo.state = DWC_RNK_STATE_PUT_ASYNC;

	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkGetScoreAsync

  Description:	Starts the asynchronous process that gets the ranking data

  Arguments:	mode		Get mode
  				category	Category
				region		Region code (can designate multiple with OR)
				param		Parameter


  Returns:		DWC_RNK_SUCCESS						Succeeded
  				DWC_RNK_IN_ERROR					Error occurring
  				DWC_RNK_ERROR_GET_NOTREADY			uninitialized
				DWC_RNK_ERROR_INVALID_PARAMETER		Invalid parameter
				DWC_RNK_ERROR_GET_INVALID_KEY		Encryption key is invalid
				DWC_RNK_ERROR_GET_NOMEMORY			insufficient memory
  *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkGetScoreAsync(	DWCRnkGetMode mode,
									u32 category,
									DWCRnkRegion region,
									DWCRnkGetParam* param ){

	DWCiRankingSessionResult res;

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Status check
	//
	if( ( g_rankinginfo.state != DWC_RNK_STATE_INITIALIZED ) 
		&& (g_rankinginfo.state != DWC_RNK_STATE_COMPLETED ) ){
	
		// not initialized
		return DWC_RNK_ERROR_GET_NOTREADY;
	
	}

	// Parameter check
	//
	if( (category > DWC_RNK_CATEGORY_MAX)
		|| (param == NULL) ){
	
		// Invalid parameter
		return DWC_RNK_ERROR_INVALID_PARAMETER;
	
	}

	switch( mode ){

	// Order
	//
	case DWC_RNK_GET_MODE_ORDER:

		// Size check
		if( param->size != sizeof(param->order) ){
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		// Parameter check
		switch( param->order.sort ){

		case DWC_RNK_ORDER_ASC:	// Ascending sequence
		case DWC_RNK_ORDER_DES:	// Descending sequence
			// Normal
			break;

		default:
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		break;

	// High score
	//
	case DWC_RNK_GET_MODE_TOPLIST:

		// Size check
		if( param->size != sizeof(param->toplist) ){
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		// Parameter check
		switch( param->toplist.sort ){

		case DWC_RNK_ORDER_ASC:	// Ascending sequence
		case DWC_RNK_ORDER_DES:	// Descending sequence
			// Normal
			break;

		default:
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		if( param->toplist.limit > DWC_RNK_GET_MAX ||
			param->toplist.limit == 0 ){
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		break;

	case DWC_RNK_GET_MODE_NEAR:	// Surrounding score

		// Size check
		if( param->size != sizeof(param->near) ){
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		// Parameter check
		switch( param->near.sort ){

		case DWC_RNK_ORDER_ASC:	// Ascending sequence
		case DWC_RNK_ORDER_DES:	// Descending sequence
			// Normal
			break;

		default:
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		if( param->near.limit > DWC_RNK_GET_MAX ||
			param->near.limit <= 1 ){
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		break;

	case DWC_RNK_GET_MODE_FRIENDS:	// Friend-specific ranking


		// Size check
		if( param->size != sizeof(param->friends) ){
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		// Parameter check
		switch( param->friends.sort ){

		case DWC_RNK_ORDER_ASC:	// Ascending sequence
		case DWC_RNK_ORDER_DES:	// Descending sequence
			// Normal
			break;

		default:
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		if( param->friends.limit > (DWC_RNK_FRIENDS_MAX+1) ||
			param->friends.limit <= 1 ){
			// Invalid parameter
			return DWC_RNK_ERROR_INVALID_PARAMETER;
		}

		break;

	}

	// Request issue
	//
	res = DWCi_RankingSessionGetAsync(	mode,
										category,
										g_rankinginfo.pid,
										region,
										param );

	// Error processing
	switch( res ){

	case DWCi_RANKING_SESSION_ERROR_INVALID_KEY:
		return DWC_RNK_ERROR_GET_INVALID_KEY;

	case DWCi_RANKING_SESSION_ERROR_NOMEMORY:
		return DWC_RNK_ERROR_GET_NOMEMORY;

	}

	// Set status
	g_rankinginfo.state = DWC_RNK_STATE_GET_ASYNC;

	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkCancelProcess

  Description:	Cancels asynchronous processing.
  				This will transition to DWC_RNK_STATE_ERROR when cancelled, so it will end when networking is performed again, and
  				you will need to go through the initialization process again.

  Arguments:	None

  Returns:		DWC_RNK_SUCCESS					Succeeded
  				DWC_RNK_IN_ERROR				Error occurring
  				DWC_RNK_ERROR_CANCEL_NOTASK		There is no asynchronous task
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkCancelProcess(){

	// Kick if DWC error occurs
	if (DWCi_IsError())return DWC_RNK_IN_ERROR;

	// Status check
	//
	if( (g_rankinginfo.state != DWC_RNK_STATE_PUT_ASYNC) &&
		(g_rankinginfo.state != DWC_RNK_STATE_GET_ASYNC) ){
	
		// There are no tasks involved in asynchronous processing
		return DWC_RNK_ERROR_CANCEL_NOTASK;
	
	}

	// cancel
	//
	DWCi_RankingSessionCancel();

	// Set status
	//
	g_rankinginfo.state = DWC_RNK_STATE_ERROR;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkProcess

  Description:	Updates asynchronous processing.

  Arguments:	None

  Returns:		DWC_RNK_SUCCESS					Succeeded
  				DWC_RNK_IN_ERROR				Error occurring
				DWC_RNK_PROCESS_NOTASK	There is no asynchronous task
 *---------------------------------------------------------------------------*/

DWCRnkError DWC_RnkProcess(){

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Status check
	//
	if( (g_rankinginfo.state != DWC_RNK_STATE_PUT_ASYNC) &&
		(g_rankinginfo.state != DWC_RNK_STATE_GET_ASYNC) ){
	
		// There are no tasks involved in asynchronous processing
		return DWC_RNK_PROCESS_NOTASK;
	
	}

	// Advances the process
	//
	switch( DWCi_RankingSessionProcess() ){

	case DWCi_RANKING_SESSION_STATE_ERROR:

		// Error occurred
		g_rankinginfo.state = DWC_RNK_STATE_ERROR;

		break;
	
	case DWCi_RANKING_SESSION_STATE_CANCELED:
		
		//!< Cancel
		g_rankinginfo.state = DWC_RNK_STATE_ERROR;

		break;
	
	case DWCi_RANKING_SESSION_STATE_INITIAL:
		
		//!< Initial state
		break;

	case DWCi_RANKING_SESSION_STATE_REQUEST:		//!<Request issue
	case DWCi_RANKING_SESSION_STATE_GETTING_TOKEN:	//!<Getting tokens.
	case DWCi_RANKING_SESSION_STATE_GOT_TOKEN:		//!<Completed getting tokens
	case DWCi_RANKING_SESSION_STATE_SENDING_DATA:	//!<Sending data

		// Processing
		break;

	case DWCi_RANKING_SESSION_STATE_COMPLETED:

		// End
		g_rankinginfo.state = DWC_RNK_STATE_COMPLETED;

		break;
	
	}


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkGetState

  Description:	Gets the current process state.

  Arguments:	None

  Returns:		DWCRnkState
 *---------------------------------------------------------------------------*/

DWCRnkState	DWC_RnkGetState(){

	return g_rankinginfo.state;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkResGetRow

  Description:	Extracts and gets optional rows from the ranking list received with network communications.

  Arguments:	out		The pointer to the DWCRnkData structure that stores the results.
				index	Index of the list to be obtained.

  Returns:		DWC_RNK_SUCCESS					Succeeded
  				DWC_RNK_IN_ERROR				Error occurring
				DWC_RNK_ERROR_INVALID_MODE		Invalid mode
  				DWC_RNK_ERROR_INVALID_PARAMETER	Invalid parameter
  				DWC_RNK_ERROR_NOTCOMPLETED		Communications not completed
				DWC_RNK_ERROR_EMPTY_RESPONSE	Empty response
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkResGetRow(	DWCRnkData* out,
										u32 index ){

	DWCRnkData* ptr;
	DWCiRankingResponse res;
	DWCRnkError err;

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Gets response pointer
	// 
	err = DWCi_RankingGetResponse( &res );
	if( err != DWC_RNK_SUCCESS )
		return err;

	// Checks to see if the responses match
	// 
	switch( res.mode ){

	case DWC_RNK_GET_MODE_ORDER:

		// Error, since there is no ranking list in the rank get request
		return DWC_RNK_ERROR_INVALID_MODE;

	case DWC_RNK_GET_MODE_TOPLIST:
	case DWC_RNK_GET_MODE_NEAR:
	case DWC_RNK_GET_MODE_FRIENDS:

		break;

	}

	// Invalid parameter check
	//
	if( out == NULL )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	if( index >= res.listheader->count )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	// Seek
	//
	ptr = &res.listheader->rankingdata;

	while( index-- > 0 ){

		ptr = (DWCRnkData*)( ((u8*)&ptr->userdata) + ptr->size );

	}

	// Detect memory over-access
	// 
	if( (u32)&ptr->userdata + ptr->size > (u32)res.header + res.size ){

		return DWC_RNK_ERROR_INVALID_PARAMETER;

	}

	// Set return value
	*out = *ptr;
	out->userdata = &ptr->userdata;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkResGetRowCount

  Description:	Extracts and gets optional rows from the ranking list received with network communications.

  Arguments:	count	Pointer to the variable that gets the number of rows

  Returns:		DWC_RNK_SUCCESS					Succeeded
  				DWC_RNK_IN_ERROR				Error occurring
				DWC_RNK_ERROR_INVALID_MODE		Invalid mode
  				DWC_RNK_ERROR_INVALID_PARAMETER	Invalid parameter
  				DWC_RNK_ERROR_NOTCOMPLETED		Communications not completed
				DWC_RNK_ERROR_EMPTY_RESPONSE	Empty response
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkResGetRowCount( u32* count ){

	DWCiRankingResponse res;
	DWCRnkError err;

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Gets response pointer
	// 
	err = DWCi_RankingGetResponse( &res );
	if( err != DWC_RNK_SUCCESS )
		return err;

	// Invalid parameter check
	//
	if( count == NULL )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	// Checks to see if the responses match
	// 
	switch( res.mode ){

	case DWC_RNK_GET_MODE_ORDER:

		// Error, since there is no ranking list in the rank get request
		return DWC_RNK_ERROR_INVALID_MODE;

	case DWC_RNK_GET_MODE_TOPLIST:
	case DWC_RNK_GET_MODE_NEAR:
	case DWC_RNK_GET_MODE_FRIENDS:

		break;

	}

	// Set return value
	//
	*count = res.listheader->count;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkResGetOrder

  Description:	Gets the order.

  Arguments:	order*	Pointer to the variable that gets the order

  Returns:		DWC_RNK_SUCCESS					Succeeded
  				DWC_RNK_IN_ERROR				Error occurring
				DWC_RNK_ERROR_INVALID_MODE		Invalid mode
  				DWC_RNK_ERROR_INVALID_PARAMETER	Invalid parameter
  				DWC_RNK_ERROR_NOTCOMPLETED		Communications not completed
				DWC_RNK_ERROR_EMPTY_RESPONSE	Empty response
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkResGetOrder( u32* order ){

	DWCiRankingResponse res;
	DWCRnkError err;

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Gets response pointer
	// 
	err = DWCi_RankingGetResponse( &res );
	if( err != DWC_RNK_SUCCESS )
		return err;

	// Invalid parameter check
	//
	if( order == NULL )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	// Checks to see if the responses match
	// 
	if( res.mode != DWC_RNK_GET_MODE_ORDER )
		return DWC_RNK_ERROR_INVALID_MODE;

	// Set return value
	//
	*order = res.orderheader->order;


	return DWC_RNK_SUCCESS;

}


/*---------------------------------------------------------------------------*
  Name:		DWC_RnkResGetTotal

  Description:	Gets the total.

  Arguments:	total*	Pointer to the variable that gets the order

  Returns:		DWC_RNK_SUCCESS					Succeeded
  				DWC_RNK_IN_ERROR				Error occurring
  				DWC_RNK_ERROR_INVALID_PARAMETER	Invalid parameter
  				DWC_RNK_ERROR_NOTCOMPLETED		Communications not completed
				DWC_RNK_ERROR_EMPTY_RESPONSE	Empty response
 *---------------------------------------------------------------------------*/

DWCRnkError	DWC_RnkResGetTotal( u32* total ){

	DWCiRankingResponse res;
	DWCRnkError err;

	// Kick if DWC error occurs
	if (DWCi_IsError()) return DWC_RNK_IN_ERROR;

	// Gets response pointer
	// 
	err = DWCi_RankingGetResponse( &res );
	if( err != DWC_RNK_SUCCESS )
		return err;

	// Invalid parameter check
	//
	if( total == NULL )
		return DWC_RNK_ERROR_INVALID_PARAMETER;

	// Set return value
	//
	*total = res.orderheader->total;


	return DWC_RNK_SUCCESS;

}

