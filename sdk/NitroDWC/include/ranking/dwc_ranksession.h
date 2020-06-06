/*---------------------------------------------------------------------------*
  Project:  NitroDWC -  -
  File:     dwc_ranksession.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_ranksession.h,v $
  Revision 1.4  04/13/2006 11:41:52  hayashi
  character code fixed

  Revision 1.3  04/13/2006 11:35:19  hayashi
  Change prefix Ranking->Rnk

  Revision 1.2  04/11/2006 09:01:34  hayashi
  Made the connection target server selectable with DWC_RnkInitialize

  Revision 1.1  04/10/2006 10:16:59  hayashi
  added the ranking module


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef DWC_RANKSESSION_H_
#define DWC_RANKSESSION_H_

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------------------------------------
        enum
   ------------------------------------------------------------------------- */

//!< session status
typedef enum{

	DWCi_RANKING_SESSION_STATE_CANCELED,		//!< Cancel
	DWCi_RANKING_SESSION_STATE_ERROR,			//!<Error occurred
	DWCi_RANKING_SESSION_STATE_INITIAL,			//!< Initial state
	DWCi_RANKING_SESSION_STATE_INITIALIZED,		//!< After initialization
	DWCi_RANKING_SESSION_STATE_REQUEST,			//!<Request issue
	DWCi_RANKING_SESSION_STATE_GETTING_TOKEN,	//!<Getting tokens.
	DWCi_RANKING_SESSION_STATE_GOT_TOKEN,		//!<Completed getting tokens
	DWCi_RANKING_SESSION_STATE_SENDING_DATA,	//!<Sending data
	DWCi_RANKING_SESSION_STATE_COMPLETED		//!<Complete

}DWCiRankingSessionState;

//!< Function return value
typedef enum{

	DWCi_RANKING_SESSION_SUCCESS,				//!<normal end
	DWCi_RANKING_SESSION_ERROR_NOTINITIALIZED,	//!<uninitialized
	DWCi_RANKING_SESSION_ERROR_NOMEMORY,		//!<insufficient memory
	DWCi_RANKING_SESSION_ERROR_INVALID_KEY		//!<Invalid key

}DWCiRankingSessionResult;


/* -------------------------------------------------------------------------
        prototype
   ------------------------------------------------------------------------- */

void DWCi_RankingSessionInitialize( BOOL releaseserver );

void DWCi_RankingSessionShutdown( void );

BOOL DWCi_RankingValidateKey(	const char* gamename,
								const char* secretkey,
								u32 key1,
								u32 key2,
								u32 key3,
								u32 key4 );

DWCiRankingSessionResult DWCi_RankingSessionPutAsync( u32 category,
													s32 pid,
													DWCRnkRegion region,
													s32 score,
													void* data,
													u32 size );

DWCiRankingSessionResult DWCi_RankingSessionGetAsync( DWCRnkGetMode mode,
													u32 category,
													s32 pid,
													DWCRnkRegion region,
													DWCRnkGetParam* param );
DWCiRankingSessionState DWCi_RankingSessionProcess( void );

void* DWCi_RankingSessionGetResponse( u32* size );

void DWCi_RankingSessionCancel( void );


#ifdef __cplusplus
}
#endif


#endif
