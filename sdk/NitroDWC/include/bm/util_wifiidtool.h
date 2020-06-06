/*---------------------------------------------------------------------------*
  Project:  NitroDWC - auth - util_wifiidtool
  File:     util_wifiidtool.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: util_wifiidtool.h,v $
  Revision 1.1  08/30/2005 02:31:23  nakata
  Add util_wifiidtoos.h to $NITRODWC_ROOT/include/bm.

  Revision 1.9  08/22/2005 13:07:13  sasakit
  Add function for determining whether or not a WiFiID should be created.

  Revision 1.8  2005/08/20 10:34:04  akimaru
  Non-public parts have been split off into util_wifiidtool.h

  Revision 1.7  08/20/2005 08:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_UTIL_WIFIIDTOOL_H_
#define DWC_UTIL_WIFIIDTOOL_H_

#include <nitro.h>

#ifdef __cplusplus
extern "C" {
#endif		// __cplusplus


	/*---------------------------------------------------------------------------*
  	Name:        DWC_Auth_Id

	Description:  Structure storing UserID.
	u64     	   uId:  Authenticated user ID
	u64unAttestationID:  Unauthenticated user ID    			  
	u8		       flg:  This flag indicates whether or not the ID has been authenticated. 0: Unauthenticated, 1: Authenticated
   	Returns:	  None
 	*---------------------------------------------------------------------------*/
	typedef struct{
		u64 uId;
		u64 notAttestedId;
		u32  flg;
	}DWCAuthWiFiId;
	
	/*---------------------------------------------------------------------------*
  	Name:         DWC_Auth_GetId

	Description:  Get DWC_Auth_Id
  	Arguments:    id :			
	  

   	Returns:	  None
 	*---------------------------------------------------------------------------*/
	void DWC_Auth_GetId( DWCAuthWiFiId* id );

	/*---------------------------------------------------------------------------*
	  Name:         DWC_Auth_CheckPseudoWiFiID
	
	  Description:  Check whether or not the temporary WiFiID to be generated offline has been generated
	
	  Arguments:    None
	
	  Returns:      TRUE  : Generated.
	                FALSE : Not generated.
	 *---------------------------------------------------------------------------*/
	BOOL DWC_Auth_CheckPseudoWiFiID( void );

BOOL DWC_Auth_CheckWiFiIDNeedCreate( void );


#ifdef __cplusplus
}
#endif		// __cplusplus


#endif // DWC_UTIL_WIFIIDTOOL_H_
