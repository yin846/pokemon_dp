/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - dwc_svl
  File:     dwc_svl.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_svl.h,v $
  Revision 1.4  05/29/2006 11:21:50  nakata
  Chnaged the SVL error code from 24000-27000 to 24000-25000

  Revision 1.3  05/25/2006 05:17:16  nakata
  Made changes in accordance with the addition to DWC_SVL of the feature for returning a DWC error

  Revision 1.2  2006/04/27 12:46:24  takayama
  svc(SVC) -> svl(SVL)

  Revision 1.1  2006/04/27 12:31:19  takayama
  dwc_svc.h -> dwc_svl.h

  Revision 1.5  03/23/2006 00:27:01  nakata
  Added the state DWC_SVL_STATE_DIRTY

  Revision 1.4  03/20/2006 05:00:24  nakata
  *** empty log message ***

  Revision 1.1  03/09/2006 08:54:14  nakata
  Made changes so that SVC-related items would not go into the package

  Revision 1.2  03/07/2006 01:16:22  nakata
  Revised line feed code.

  Revision 1.1  03/02/2006 05:21:58  nakata
  Added the DWC_Svc module.
  Changed the names of the DWC_Util_Alloc/DWC_Util_Free functions to DWC_Auth_Alloc/DWC_Auth_Free.



  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_SVL_H
#define DWC_SVL_H

#include "util_alloc.h"

#ifdef __cplusplus
extern "C" {
#endif // [nakata] __cplusplus

//=============================================================================
// [nakata] definitions
//=============================================================================

#define DWC_SVL_ERROR_BASENUM		(-24000)
#define DWC_SVL_ERROR_HTTPBASENUM	(-25000)

#define DWC_SVL_ERROR_FATAL			(-100)
#define DWC_SVL_ERROR_BADDATA		(-101)

#define DWC_SVL_HOST_LENGTH		64
#define DWC_SVL_TOKEN_LENGTH	300
#define DWC_SVL_STATUSDATA_LENGTH	1

// [nakata] Enum type used to show the state where the SVC is being obtained
typedef enum {
	DWC_SVL_STATE_DIRTY	= 0,// uninitialized
	DWC_SVL_STATE_IDLE,		// Process not running
	DWC_SVL_STATE_HTTP,		// HTTP networking active
	DWC_SVL_STATE_SUCCESS,	// Token successfully obtained
	DWC_SVL_STATE_ERROR,	// Failed to obtain token
	DWC_SVL_STATE_CANCELED,	// Canceled token acquisition
	DWC_SVL_STATE_MAX
} DWCSvlState;

// [nakata] Structure which stores the SVC acquisition results
typedef struct {
	BOOL	status;
	char svlhost[DWC_SVL_HOST_LENGTH+1];
	char svltoken[DWC_SVL_TOKEN_LENGTH+1];
} DWCSvlResult;

//=============================================================================
// [nakata] Functions
//=============================================================================

//-----------------------------------------------------------------------------
// Function      : Allocation and initialization of the region used by DWC_Scv
// Arguments     : alloc            - Pointer to the memory allocation function used internally
//               : free             - Pointer to the memory free function used internally
// Return values : TRUE   : Succeeded
//                 FALSE  : Failed
//-----------------------------------------------------------------------------
BOOL		DWC_Svl_Init(DWCAuthAlloc alloc, DWCAuthFree free);

//-----------------------------------------------------------------------------
// Function      : Deallocation of the region used by DWC_Svc
// Arguments     : None
// Return values : None
//-----------------------------------------------------------------------------
void		DWC_Svl_Cleanup(void);

//-----------------------------------------------------------------------------
// Function      : Function for getting tokens.
// Arguments     : svc    - A 4-character ASCII character string that designates service type.
//               : result - Pointer to the region where the obtained results are stored.
// Return values : TRUE  : Start getting
//                 FALSE : Cannot start getting
//-----------------------------------------------------------------------------
BOOL		DWC_Svl_GetTokenAsync(char *svc, DWCSvlResult *result);

//-----------------------------------------------------------------------------
// Function      : Function which advances the token acquisition process
// Arguments     : None
// Return values : DWCSvcState
//-----------------------------------------------------------------------------
DWCSvlState	DWC_Svl_Process(void);

//-----------------------------------------------------------------------------
// Function      : Function which interrupts the token acquisition process
// Arguments     : None
// Return values : None
//-----------------------------------------------------------------------------
void		DWC_Svl_Abort(void);

#ifdef __cplusplus
}
#endif

#endif
