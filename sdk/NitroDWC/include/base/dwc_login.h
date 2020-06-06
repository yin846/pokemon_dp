/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_login
  File:     dwc_login.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_login.h,v $
  Revision 1.17  03/07/2006 02:18:16  nakata
  Performed necessary changes resulting from collecting added NAS functionality into dwc_nasfunc.c/dwc_nasfunc.h.

  Revision 1.16  01/23/2006 04:50:43  arakit
  revised comments to match standardized terminology

  Revision 1.15  12/26/2005 07:57:44  nakata
  Changed DWC_AUTH_INGAMESN_NOTCHECKED to DWC_AUTH_INGAMESN_NOT_CHECKED.
  Changed DWC_INGAMESN_NOTCHECKED to DWC_INGAMESN_NOT_CHECKED.

  Revision 1.14  12/26/2005 07:43:20  nakata
  Moved ingamesn check functions from dwc_common.c to dwc_login.c.

  Revision 1.13  12/21/2005 03:08:54  arakit
  Made the screen name from within the game transfer to the server when logging in.

  Revision 1.12  11/04/2005 05:04:49  arakit
  Added the DWCi_GetAuthInfo() internal function that gets authentication info.

  Revision 1.11  11/02/2005 02:49:33  arakit
  Adjusted the file's include relationships.

  Revision 1.10  10/07/2005 07:48:45  sasakit
  Inserted a gpConnect timeout.

  Revision 1.9  10/06/2005 01:15:11  sasakit
  Made it so connecting status can be retrieved.

  Revision 1.8  10/01/2005 11:30:49  sasakit
  Corrected a problem where execution freezes if AvailableCheck for the GS fails at login.

  Revision 1.7  09/05/2005 14:42:15  sasakit
  Added processing of authentication errors during login

  Revision 1.6  08/26/2005 09:15:22  arakit
  EOrganized ASSERT and check of module control variables.
  EChanged code so that arguments of the external function DWC_ShutdownFriendsMatch() are not taken.
  Also, made code so that control variables for each module cannot be cleared to NULL by the same function.
  EChanged the error processing function and inserted error setting and error checking where necessary.

  Revision 1.5  08/20/2005 08:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_LOGIN_H_
#define DWC_LOGIN_H_

#include "auth/dwc_auth.h"

#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
#define DWC_LOGIN_AUTH_TIMEOUT       ( 10 * 1000 ) // [ms]
#define DWC_LOGIN_GPCONNECT_TIMEOUT  ( 60 * 1000 ) // [ms]


//----------------------------------------------------------------------------
// enum
//----------------------------------------------------------------------------
// Login progress status enumerator
typedef enum {
    DWC_LOGIN_STATE_INIT = 0,         // Initial state
    DWC_LOGIN_STATE_REMOTE_AUTH,      // Currently performing remote authentication
    DWC_LOGIN_STATE_CONNECTING,       // Connecting to GP server
    DWC_LOGIN_STATE_GPGETINFO,        // When getting lastname upon login to GP
    DWC_LOGIN_STATE_GPSETINFO,        // lastname setting upon the first login to GP
    DWC_LOGIN_STATE_CONNECTED,        // Connected status
    DWC_LOGIN_STATE_NUM
} DWCLoginState;

//----------------------------------------------------------------------------
// typedef - function
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Login complete callback type
  Arguments: error: DWC error type
          profileID : Profile ID for self that was gotten
          param : Parameters for the callback
  Return value: None
 *---------------------------------------------------------------------------*/
typedef void (*DWCLoginCallback)(DWCError error, int profileID, void *param);


//----------------------------------------------------------------------------
// struct
//----------------------------------------------------------------------------
// Login control structure
typedef struct DWCstLoginControl
{
    GPConnection* pGpObj;       // Pointer to the gp connection structure pointer
    DWCLoginState state;        // Login progress status
    int  productID;             // Product ID of game distributed by GameSpy
    u32  gamecode;              // gamecode assigned to game by Nintendo
    //const char* userID;         // User ID for certification
    //const char* password;       // Password for certification
    const u16* playerName;      // Pointer to the player name (screen name within the game)
    DWCLoginCallback callback;  // Login complete callback
    void *param;                // Parameter for the above callback

    DWCUserData* userdata;      // Pointer to the user data.

    // working
    void* bmwork;
    void* http;
    OSTick startTick;
    u32    connectFlag;
    OSTick connectTick;

    DWCAccLoginId tempLoginId;

    char authToken[GP_AUTHTOKEN_LEN];  // Token for authentication
    char partnerChallenge[GP_PARTNERCHALLENGE_LEN];  // Challenge value for authentication

    char username[DWC_ACC_USERNAME_STRING_BUFSIZE];

} DWCLoginControl;


//----------------------------------------------------------------------------
// function - external
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
extern void DWCi_LoginInit(DWCLoginControl* logcnt, DWCUserData* userdata, GPConnection* pGpObj, int productID, u32 gamecode, const u16* playerName, DWCLoginCallback callback, void* param);


extern void DWCi_LoginAsync( void );


extern void DWCi_LoginProcess(void);


extern DWCAccUserData* DWCi_GetUserData(void);


extern void DWCi_StopLogin(DWCError error, int errorCode);


extern void DWCi_ShutdownLogin(void);


extern BOOL DWCi_CheckLogin( void );


extern BOOL DWCi_GetAuthInfo( char** authToken, char** partnerChallenge );


#ifdef __cplusplus
}
#endif


#endif // DWC_LOGIN_H_
