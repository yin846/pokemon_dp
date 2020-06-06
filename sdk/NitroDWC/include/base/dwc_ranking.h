/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  -
  File:     dwc_ranking.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_ranking.h,v $
  Revision 1.11  05/30/2006 14:08:50  hayashi
  added DWC_RnkResGetTotal

  Revision 1.10  2006/05/22 07:29:58  hayashi
  changed to be set the server by DWC_SetAuthServer()

  Revision 1.9  05/12/2006 08:28:54  hayashi
  NOTINITIALIZED -> NOTREADY
  RIVAL -> NEAR
  DWCAccUserData -> DWCUserData
  BOOL	DWC_RnkShutdown() -> DWCRnkError

  Revision 1.8  04/26/2006 09:25:41  hayashi
  specification is changed related to PID handling

  Revision 1.7  04/14/2006 05:03:50  hayashi
  do indent

  Revision 1.6  04/13/2006 11:35:19  hayashi
  Change prefix Ranking->Rnk

  Revision 1.5  04/13/2006 09:01:38  hayashi
  character code converting

  Revision 1.4  04/11/2006 09:01:33  hayashi
  Made the connection target server selectable with DWC_RnkInitialize

  Revision 1.3  04/11/2006 07:04:19  hayashi
  The base64 encoding size calculation was erroneous, so revised

  Revision 1.2  04/10/2006 13:26:11  hayashi
  Added the since parameter

  Revision 1.1  04/10/2006 13:06:11  hayashi
  added the ranking module


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_RNK_H_
#define DWC_RNK_H_


#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

// The maximum size that can be used in the user-defined data
#define DWC_RNK_DATA_MAX		768

// The maximum value that can be designated as a category
#define DWC_RNK_CATEGORY_MAX	100

// Constant that shows the ascending and descending order of the ranking list to be obtained
#define DWC_RNK_ORDER_ASC		0	// Ascending sequence
#define DWC_RNK_ORDER_DES		1	// Descending sequence

// The upper limit of the maximum value of the ranking list to be obtained 
#define DWC_RNK_GET_MAX         10

// Maximum number of friends
#define DWC_RNK_FRIENDS_MAX		64


/* -------------------------------------------------------------------------
        enum
   ------------------------------------------------------------------------- */

// error code
//
typedef enum{

	DWC_RNK_SUCCESS = 0,						// Succeeded
	DWC_RNK_IN_ERROR,							// Error occurring

	DWC_RNK_ERROR_INVALID_PARAMETER,			// Invalid parameter

	// 
	// DWC_RnkInitialize error value
	// 
	DWC_RNK_ERROR_INIT_ALREADYINITIALIZED,		// Initialized
	DWC_RNK_ERROR_INIT_INVALID_INITDATASIZE,	// Invalid initialization data size
	DWC_RNK_ERROR_INIT_INVALID_INITDATA,		// Invalid initialization data
	DWC_RNK_ERROR_INIT_INVALID_USERDATA,		// Invalid user information

	//
	// DWC_RnkPutScoreAsync error value
	//
	DWC_RNK_ERROR_PUT_NOTREADY,					// not initialized
	DWC_RNK_ERROR_PUT_INVALID_KEY,				// Encryption key is invalid
	DWC_RNK_ERROR_PUT_NOMEMORY,					// insufficient memory
 	
	//
	// DWC_RnkGetScoreAsync error value
	//
	DWC_RNK_ERROR_GET_NOTREADY,					// not initialized
	DWC_RNK_ERROR_GET_INVALID_KEY,				// Encryption key is invalid
	DWC_RNK_ERROR_GET_NOMEMORY,					// insufficient memory
 
	//
	// DWC_RnkCancelProcess error value
	//
	DWC_RNK_ERROR_CANCEL_NOTASK,				// No process during asynchronous processing

	//
	// DWC_RnkProcess error value
	//
	DWC_RNK_PROCESS_NOTASK,				// No process during asynchronous processing

	//
	// DWC_RnkGetResponse error value
	//
	DWC_RNK_ERROR_INVALID_MODE,					// Invalid mode
	DWC_RNK_ERROR_NOTCOMPLETED,					// Communications not completed
	DWC_RNK_ERROR_EMPTY_RESPONSE				// Empty response


} DWCRnkError;


// Module status
//
typedef enum{

	DWC_RNK_STATE_NOTREADY	= 0,	// Uninitialized state

	DWC_RNK_STATE_INITIALIZED,		// Initialization complete
	DWC_RNK_STATE_PUT_ASYNC,		// Currently in PUT asynchronous processing
	DWC_RNK_STATE_GET_ASYNC,		// Currently in GET asynchronous processing
	DWC_RNK_STATE_COMPLETED,		// End

	DWC_RNK_STATE_ERROR				// Error occurred

} DWCRnkState;


// Region code
//
typedef enum{

	DWC_RNK_REGION_JP	= 0x01,	// Japan
	DWC_RNK_REGION_US	= 0x02,	// United States
	DWC_RNK_REGION_EU	= 0x04,	// Europe

	DWC_RNK_REGION_ALL	= 0x07	// World

} DWCRnkRegion;


// DWC_RnkGetScoreAsync get mode
//
typedef enum{

	DWC_RNK_GET_MODE_ORDER,		// Order
	DWC_RNK_GET_MODE_TOPLIST,	// High score
	DWC_RNK_GET_MODE_NEAR,		// Surrounding score
	DWC_RNK_GET_MODE_FRIENDS	// Friend-specific ranking

} DWCRnkGetMode;


/* -------------------------------------------------------------------------
        typedef - function
   ------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

// Structure for storing score information
//
typedef struct{

	u32					order;		// Order
	s32					pid;		// GameSpy Profile ID
	s32					score;		// score
	DWCRnkRegion		region;		// Region code
	u32					lastupdate;	// The amount of time that has passed since the last update (in minutes)

	u32					size;		// userdata size (in bytes)
	void*				userdata;	// Buffer for user-defined data

} DWCRnkData;


// Parameters designated with DWC_RnkGetScoreAsync
//
typedef struct{

	u32				size;	// Size (in bytes)

	union{

		//
		// mode = DWC_RNK_GET_MODE_ORDER
		//
		struct{

			u32	sort;							// DWC_RNK_ORDER_ASC: ascending order
												// DWC_RNK_ORDER_DES: descending order
			u32	since;							// How many seconds ago?

		}			order;

		//
		// mode = DWC_RNK_GET_MODE_TOPLIST
		//
		struct{

			u32	sort;							// DWC_RNK_ORDER_ASC: ascending order
												// DWC_RNK_ORDER_DES: descending order
			u32	limit;							// The maximum number to get
			u32	since;							// How many seconds ago?

		}			toplist;

		//
		// mode = DWC_RNK_GET_MODE_NEAR
		//
		struct{

			u32	sort;							// DWC_RNK_ORDER_ASC: ascending order
												// DWC_RNK_ORDER_DES: descending order
			u32	limit;							// The maximum number to get
			u32	since;							// How many seconds ago?

		}			near;

		//
		// mode = DWC_RNK_GET_MODE_FRIENDS
		//
		struct{


			u32	sort;							// DWC_RNK_ORDER_ASC: ascending order
												// DWC_RNK_ORDER_DES: descending order
			u32	limit;							// The maximum number to get
			u32	since;							// How many seconds ago?
			s32	friends[DWC_RNK_FRIENDS_MAX];	// Friend PID list

		}			friends;
	
		u32			data;

	};

} DWCRnkGetParam;


/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

extern DWCRnkError	DWC_RnkInitialize(		const char* initdata,
											const DWCUserData* userdata);

extern DWCRnkError	DWC_RnkShutdown();

extern DWCRnkError	DWC_RnkPutScoreAsync(	u32 category,
											DWCRnkRegion region,
											s32 score,
											void* data,
											u32 size );

extern DWCRnkError	DWC_RnkGetScoreAsync(	DWCRnkGetMode mode,
											u32 category,
											DWCRnkRegion region,
											DWCRnkGetParam* param );

extern DWCRnkError	DWC_RnkCancelProcess();

extern DWCRnkError	DWC_RnkProcess();

extern DWCRnkState	DWC_RnkGetState();

//
// For obtaining response
//
extern DWCRnkError	DWC_RnkResGetRow(		DWCRnkData* out,
											u32 index );

extern DWCRnkError	DWC_RnkResGetRowCount(	u32* count );

extern DWCRnkError	DWC_RnkResGetOrder(		u32* order );

extern DWCRnkError	DWC_RnkResGetTotal(		u32* total );


#ifdef __cplusplus
}
#endif

#endif // DWC_RNK_H_
