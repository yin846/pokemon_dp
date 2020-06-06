#include <nitro.h>
#include <base/dwc_base_gamespy.h>

#include <auth/dwc_auth.h>
#include <base/dwc_report.h>
#include <base/dwc_account.h>
#include <base/dwc_error.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_login.h>
#include <base/dwc_friend.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>
#include <base/dwc_common.h>


//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------------
typedef void (*DWCRemoteAuthCallback)(const char* authToken, const char* partnerChallenge, void* param);

//----------------------------------------------------------------------------
// static variable
//----------------------------------------------------------------------------
// Pointer to login control object
static DWCLoginControl* stpLoginCnt = NULL;

static DWCRemoteAuthCallback stRemoteAuthCallback;
static void* pStRemoteAuthParam;

//----------------------------------------------------------------------------
// prototype
//----------------------------------------------------------------------------
static void DWCi_CloseLogin(void);

static GPResult DWCi_HandleGPError     (GPResult result);
static void     DWCi_GPConnectCallback (GPConnection* pconnection, GPConnectResponseArg* arg, void* param);
static void     DWCi_RemoteAuthCallback(const char* authToken, const char* partnerChallenge, void* param);
static void     DWCi_GPConnect( const char* authToken,  const char* partnerChallenge, GPCallback callback, DWCLoginState nextstate );

static void DWCi_GPGetInfo( void );
static void DWCi_GPGetInfoCallback( GPConnection* pconnection, GPGetInfoResponseArg* arg, void* param );

static void DWCi_RemoteLogin       ( DWCRemoteAuthCallback callback, void* param );
static void DWCi_RemoteLoginProcess( void );


//----------------------------------------------------------------------------
// function - internal
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for initializing the login control structure
  Argument: logcnt : Pointer to login control object
          userdata: pointer to the user data object
          pGpObj  :	Pointer to the gp connection structure pointer
          productID     Product ID received from GameSpy
          gamecode: code unique to the game (ordinarily the initial code)
          playerName: screen name within the game
          callback:   callback function for notification of login completion
          param : Parameters for the callback
  Return value: None
  Use : Initializes the login control structure
 *---------------------------------------------------------------------------*/
void DWCi_LoginInit(DWCLoginControl* logcnt, DWCUserData* userdata, GPConnection* pGpObj, int productID, u32 gamecode, const u16* playerName, DWCLoginCallback callback, void* param)
{
    SDK_ASSERT(logcnt);

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "Login Init\n" );

    stpLoginCnt = logcnt;

    MI_CpuClear8( stpLoginCnt, sizeof( DWCLoginControl ) );

    // Initialization of the login control structure
    stpLoginCnt->pGpObj     = pGpObj;
    stpLoginCnt->state      = DWC_LOGIN_STATE_INIT;
    stpLoginCnt->productID  = productID;
    stpLoginCnt->gamecode   = gamecode;
    stpLoginCnt->playerName = playerName;
    stpLoginCnt->callback   = callback;
    stpLoginCnt->param      = param;

    stpLoginCnt->userdata   = userdata;

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "******************************************\n" );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  pseudo    UserID   : %016llx\n", DWCi_Acc_GetUserId( &userdata->pseudo ) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  pseudo    PlayerID : %08x\n"   , DWCi_Acc_GetPlayerId( &userdata->pseudo ) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  authentic UserID   : %016llx\n", DWCi_Acc_GetUserId( &userdata->authentic ) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  authentic PlayerID : %08x\n"   , DWCi_Acc_GetPlayerId( &userdata->authentic ) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "******************************************\n" );

}


/*---------------------------------------------------------------------------*
  Wi-Fi connection login function
  Arguments : None
  Return value: None
  application: commences login to Wi-Fi connection
 *---------------------------------------------------------------------------*/
//void DWCi_LoginAsync(DWCLoginCallback callback, void* param)
void DWCi_LoginAsync( void )
{

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "Login Async\n" );

    // Calls the original authentication server
    DWCi_RemoteLogin( DWCi_RemoteAuthCallback, NULL );

    stpLoginCnt->state = DWC_LOGIN_STATE_REMOTE_AUTH;
    stpLoginCnt->connectFlag = FALSE;
}


/*---------------------------------------------------------------------------*
  Function for updating login processing
  Arguments : None
  Return value: None
  Use : Called every game frame, this function updates login communication processing
 *---------------------------------------------------------------------------*/
void DWCi_LoginProcess(void)
{

    if (!stpLoginCnt || DWCi_IsError()) return;

    switch (stpLoginCnt->state){
    case DWC_LOGIN_STATE_REMOTE_AUTH:  // Currently performing remote authentication
        DWCi_RemoteLoginProcess();
        break;

    case DWC_LOGIN_STATE_CONNECTING:  // Connecting to GP server
    case DWC_LOGIN_STATE_GPGETINFO:
    case DWC_LOGIN_STATE_GPSETINFO:
        if (stpLoginCnt->pGpObj && *stpLoginCnt->pGpObj)
        {
            gpProcess(stpLoginCnt->pGpObj);
        }
        if ( stpLoginCnt->connectFlag &&
             ( OS_TicksToMilliSeconds( OS_GetTick() - stpLoginCnt->connectTick ) > DWC_LOGIN_GPCONNECT_TIMEOUT ) )
        {
            // Timeout occurred
            DWCi_StopLogin( DWC_ERROR_NETWORK, DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GP+DWC_ECODE_TYPE_TIMEOUT );
            stpLoginCnt->connectFlag = FALSE;
        }
        break;

    case DWC_LOGIN_STATE_INIT:
    case DWC_LOGIN_STATE_CONNECTED:
    default:
        // Escape without doing anything in cases other than the above
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Ignore invalid login state.\n");
        break;
    }
}


/*---------------------------------------------------------------------------*
  Function for getting user data
  Arguments : None
  Return value: Pointer to user data
  Use : Gets pointer to user data
 *---------------------------------------------------------------------------*/
DWCAccUserData* DWCi_GetUserData(void)
{

    if (stpLoginCnt){
        return stpLoginCnt->userdata;
    }
    else {
        return NULL;
    }
}


/*---------------------------------------------------------------------------*
  Function for ending on a login error
  Arguments: error: DWC error type
          errorCode : Error code
  Return value: None
  Use : Processing is halted if an error occurs during login processing
 *---------------------------------------------------------------------------*/
void DWCi_StopLogin(DWCError error, int errorCode)
{

    if (!stpLoginCnt || (error == DWC_ERROR_NONE)) return;

    DWCi_SetError(error, errorCode);

    // Call to the login completion callback function
    if ( stpLoginCnt->callback != NULL )
    {
        stpLoginCnt->callback(error, 0, stpLoginCnt->param);
    }

    DWCi_CloseLogin();

    // After this, it's necessary to release the GP object using DWC_ShutdownFriendsMatch()
}


/*---------------------------------------------------------------------------*
  Function for freeing login control objects
  Arguments : None
  Return value: None
  Use : Clears to NULL a pointer to a login control object
 *---------------------------------------------------------------------------*/
void DWCi_ShutdownLogin(void)
{
    // Checks whether or not authentication is currently being performed
    if ( stpLoginCnt->http != NULL )
    {
        DWC_Auth_Abort();
        DWC_Auth_Destroy();

        DWC_Free( DWC_ALLOCTYPE_AUTH, stpLoginCnt->http, 0 );
        stpLoginCnt->http = NULL;
    }

    stpLoginCnt = NULL;
}


//----------------------------------------------------------------------------
// function - static
//----------------------------------------------------------------------------
/*---------------------------------------------------------------------------*
  Function for ending login
  Arguments : None
  Return value: None
  application: initializes the login control object and ends the login process
 *---------------------------------------------------------------------------*/
static void DWCi_CloseLogin(void)
{

    if (!stpLoginCnt) return;
    
    // Initialization of login control objects
    stpLoginCnt->state = DWC_LOGIN_STATE_INIT;
    stpLoginCnt->connectFlag = FALSE;

    // password, authToken and partnerChallenge are all used for login to Persistent Storage
    // Use for login
}


/*---------------------------------------------------------------------------*
  Error processing function for GPResult
  Argument : result : The type of result due to GP processing
  Return value: The type of result due to GP processing (the argument is returned unchanged)
  Use : Calls the corresponding callback by passing the corresponding DWC error, while displaying errors (or stopping) corresponding to the value of GPResult
          The corresponding DWC error is passed and the login procedure is terminated
 *---------------------------------------------------------------------------*/
static GPResult DWCi_HandleGPError(GPResult result)
{
    int errorCode;
    DWCError dwcError;

    if (result == GP_NO_ERROR) return GP_NO_ERROR;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Login, GP error %d\n", result);

    switch (result){
    case GP_MEMORY_ERROR:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_ALLOC;
        break;
    case GP_PARAMETER_ERROR:
        dwcError  = DWC_ERROR_FATAL;
        errorCode = DWC_ECODE_TYPE_PARAM;
        break;
    case GP_NETWORK_ERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_NETWORK;
        break;
    case GP_SERVER_ERROR:
        dwcError  = DWC_ERROR_NETWORK;
        errorCode = DWC_ECODE_TYPE_SERVER;
        break;
    }

    // Base code added
    errorCode += DWC_ECODE_SEQ_LOGIN+DWC_ECODE_GS_GP;

    DWCi_StopLogin(dwcError, errorCode);  // The login procedure has ended on an error

    return result;
}


/*---------------------------------------------------------------------------*
  GP connect callback function
  Arguments: pconnection : Pointer to the GP connection object
          arg : Pointer to the GP connection response structure
          param : Parameters for the callback
  Return value: None
  Use : GP connect callback
 *---------------------------------------------------------------------------*/
static void DWCi_GPConnectCallback(GPConnection* pconnection, GPConnectResponseArg* arg, void* param)
{
#pragma unused( param )
    GPResult gpResult;

    DWC_Printf(DWC_REPORTFLAG_LOGIN, "Finished connecting to GP server, result = %d\n", arg->result);
    // End once the callback is called.
    stpLoginCnt->connectFlag = FALSE;

    if (arg->result == GP_NO_ERROR)
    {
        if ( stpLoginCnt->state == DWC_LOGIN_STATE_CONNECTING )
        {
            // GP status is set to online mode.
            //gpResult = DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);

            //if (DWCi_HandleGPError(gpResult)) return;

            if ( stpLoginCnt->userdata->gs_profile_id == arg->profile )
            {
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "  gs profile id is valid.\n" );

                stpLoginCnt->state = DWC_LOGIN_STATE_CONNECTED;

                // GP status is set to online mode.
                gpResult = DWCi_SetGPStatus(DWC_STATUS_ONLINE, "", NULL);
                if (DWCi_HandleGPError(gpResult)) return;

                // Connection successful. Call to the login completion callback function
                stpLoginCnt->callback(DWC_ERROR_NONE, arg->profile, stpLoginCnt->param);

                // Create gt2 socket
                if (!DWCi_GT2Startup()){
                    // Start QR2 if successful
                    if (DWCi_QR2Startup(arg->profile)) return;
                }
                else return;
                // Login error end processing is performed if an error occurs inside any function
                // 
            }
            else
            {
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "  gs profile id is invalid.\n" );

                DWCi_StopLogin( DWC_ERROR_NETWORK, DWC_ECODE_SEQ_LOGIN );
            }
        }
        else if ( stpLoginCnt->state == DWC_LOGIN_STATE_GPGETINFO )
        {
            // Sequence used to check the lastname using GPGetInfo
            gpResult = gpGetInfo( pconnection, arg->profile, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback, NULL );
            if (DWCi_HandleGPError(gpResult)) return;
        }
    }
    else
    {
        // If connection fails
        (void)DWCi_HandleGPError(arg->result);
    }
}

/*---------------------------------------------------------------------------*
  Remote authentication complete callback function
  Arguments: authToken : Pointer to the retrieved token used for authentication
          parnterchallenge : Pointer to the retrieved challenge
          param : Parameters for the callback
  Return value: None
  Use : Remote authentication complete callback
 *---------------------------------------------------------------------------*/
static void DWCi_RemoteAuthCallback(const char* authToken, const char* partnerChallenge, void* param)
{
#pragma unused( param )

    DWCi_GPConnect( authToken, partnerChallenge, (GPCallback)DWCi_GPConnectCallback, DWC_LOGIN_STATE_CONNECTING );
}

static void DWCi_GPConnect( const char* authToken,  const char* partnerChallenge,
                            GPCallback callback, DWCLoginState next_state )
{
    GPResult gpResult;

    DWC_Printf(DWC_REPORTFLAG_LOGIN, "Succeeded to remote authentication.\n");

    strcpy( stpLoginCnt->authToken, authToken );
    strcpy( stpLoginCnt->partnerChallenge, partnerChallenge );

    stpLoginCnt->connectTick = OS_GetTick();
    stpLoginCnt->connectFlag = TRUE;

    gpResult = gpConnectPreAuthenticated(stpLoginCnt->pGpObj, stpLoginCnt->authToken,
                                         stpLoginCnt->partnerChallenge, GP_FIREWALL,
                                         GP_NON_BLOCKING,
                                         callback,
                                         NULL);
    if (DWCi_HandleGPError(gpResult))
    {
        return;
    }

    stpLoginCnt->state = next_state;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void DWCi_RemoteLogin( DWCRemoteAuthCallback callback,
                              void*                 param )
{
    DWCAuthParam auth;
    void* http;

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "Start Remote Auth\n" );

    MI_CpuClear8( &auth, sizeof(auth) );

    stRemoteAuthCallback = callback;
    pStRemoteAuthParam   = param;

    // Creates data for authentication
    if ( DWCi_Acc_IsAuthentic( stpLoginCnt->userdata ) )
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, "  Hmm.. you already have authentic login id.\n" );

        // The authenticated userid is used since authentication is complete.
        DWCi_Acc_LoginIdToUserName( &stpLoginCnt->userdata->authentic, stpLoginCnt->userdata->gamecode, stpLoginCnt->username );
    }
    else
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, "  Hmm.. you need to create authentic login id.\n" );

        // Check how many times this has executed.
        if ( !DWCi_Acc_IsValidLoginId( &stpLoginCnt->tempLoginId ) )
        {
            DWC_Printf( DWC_REPORTFLAG_LOGIN, "    Hmm.. you are the first time to get authentic login id." );
            // If this is the first execution, copy and use the temporary id.
            if ( DWCi_Acc_CheckConsoleUserId( &stpLoginCnt->userdata->pseudo ) )
            {
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "- copy temp loginid from pseudo login id\n" );
                stpLoginCnt->tempLoginId = stpLoginCnt->userdata->pseudo;
            }
            else
            {
                // Since this host differs from the one that made the temporary ID.
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "- create temp loginid from console user id\n" );
                DWCi_Acc_CreateTempLoginId( &stpLoginCnt->tempLoginId );
            }
        }
        else
        {
            MATHRandContext32   randcontext;

            DWC_Printf( DWC_REPORTFLAG_LOGIN, "    Hmm.. you are NOT the first times to get authentic login id.\n" );
            // Only replace the player ID beginning from the second execution.
            // [todo] Randomization. -> ok
            //DWCi_Acc_SetPlayerId( &stpLoginCnt->tempLoginId, DWCi_Acc_GetPlayerId( &stpLoginCnt->tempLoginId ) + 1 );

            MATH_InitRand32( &randcontext, OS_GetTick() );
            DWCi_Acc_SetPlayerId( &stpLoginCnt->tempLoginId, MATH_Rand32( &randcontext, 0 ) );
        }

        DWCi_Acc_LoginIdToUserName( &stpLoginCnt->tempLoginId, stpLoginCnt->gamecode, stpLoginCnt->username );
    }

    // Copies to the data for certification of in-game screen names
	MI_CpuCopy8(stpLoginCnt->playerName, auth.ingamesn, DWCi_WStrLen(stpLoginCnt->playerName)*2+2);

    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  ingamesn : %s\n", auth.ingamesn );

    strcpy( auth.gsbrcd, &(stpLoginCnt->username[DWC_ACC_USERNAME_GSBRCD_OFFSET]) );
    DWC_Printf( DWC_REPORTFLAG_LOGIN, "  gsbrcd   : %s.\n", auth.gsbrcd );

    auth.alloc = (DWCAuthAlloc)DWC_Alloc;
    auth.free  = (DWCAuthFree)DWC_Free;

    http = DWC_AllocEx( DWC_ALLOCTYPE_AUTH, sizeof( DWCHttp ), 4 );
    SDK_ASSERTMSG( http, "DWC_LOGIN: can't alloc memory %s :%d\n", __FILE__, __LINE__ );

    // Remember this for releasing later
    stpLoginCnt->http   = http;
    stpLoginCnt->startTick = OS_GetTick();

    // Start authentication processing.
    DWC_Auth_Create( &auth, http );
}

static void DWCi_RemoteLoginProcess( void )
{
    if ( DWC_Auth_GetError() == DWCAUTH_E_FINISH )
    {
        DWCAuthResult result;
        DWC_Printf(DWC_REPORTFLAG_LOGIN, " *** Auth Done\n");
        DWC_Auth_GetResult( &result );

        strcpy( stpLoginCnt->authToken , result.token );
        strcpy( stpLoginCnt->partnerChallenge , result.challenge );

        DWC_Auth_Destroy();

        DWC_Free( DWC_ALLOCTYPE_AUTH, stpLoginCnt->http, 0 );
        stpLoginCnt->http = NULL;

        // Get authentication packet (call the callback)
        // Originally, processing for the player ID, etc. was also necessary.
        if ( DWCi_Acc_IsAuthentic( stpLoginCnt->userdata ) )
        {
            stRemoteAuthCallback( stpLoginCnt->authToken, stpLoginCnt->partnerChallenge, pStRemoteAuthParam );
        }
        else
        {
            // Get the LoginID since this is the first connection.
            DWCi_GPConnect( stpLoginCnt->authToken, stpLoginCnt->partnerChallenge, (GPCallback)DWCi_GPConnectCallback, DWC_LOGIN_STATE_GPGETINFO );
        }
    }
    else if ( DWC_Auth_GetError() != DWCAUTH_E_NOERR )
    {
        OSTick nowTick;

        DWC_Printf(DWC_REPORTFLAG_LOGIN, " *** Auth Error\n");
        nowTick = OS_GetTick();
        if ( OS_TicksToMilliSeconds( nowTick - stpLoginCnt->startTick ) > DWC_LOGIN_AUTH_TIMEOUT )
        {
            DWCAuthResult result;
            DWC_Auth_GetResult( &result );

            // Error occurred
            DWC_Auth_Destroy();
            DWC_Free( DWC_ALLOCTYPE_AUTH, stpLoginCnt->http, 0 );
            stpLoginCnt->http = NULL;

            DWCi_StopLogin( DWC_ERROR_AUTH_ANY, result.returncode );

            return;
            // NOT REACH HERE
        }
        else
        {
            // Retry using the same parameters until timeout value is reached.
            DWCAuthParam auth;

            DWC_Printf( DWC_REPORTFLAG_LOGIN, "  **** AUTH FAILED ... retry AUTH_ERROR:%d.\n", DWC_Auth_GetError() );

            // Reworked authentication processing.
            DWC_Auth_Destroy();
            
            MI_CpuClear8( &auth, sizeof(auth) );

            // Copies to the data for certification of in-game screen names
            MI_CpuCopy8(stpLoginCnt->playerName, auth.ingamesn, DWCi_WStrLen(stpLoginCnt->playerName)*2+2);

            DWC_Printf( DWC_REPORTFLAG_LOGIN, "  ingamesn : %s\n", auth.ingamesn );
            
            strcpy( auth.gsbrcd, &(stpLoginCnt->username[DWC_ACC_USERNAME_GSBRCD_OFFSET]) );
            auth.alloc = (DWCAuthAlloc)DWC_Alloc;
            auth.free  = (DWCAuthFree)DWC_Free;

            DWC_Printf( DWC_REPORTFLAG_LOGIN, "  gsbrcd   : %s.\n", auth.gsbrcd );

            DWC_Auth_Create( &auth, stpLoginCnt->http );
        }
    }
}

static void DWCi_GPGetInfoCallback( GPConnection* pconnection, GPGetInfoResponseArg* arg, void* param )
{
#pragma unused( param )

    if ( arg->result == GP_NO_ERROR )
    {
        if ( stpLoginCnt->state == DWC_LOGIN_STATE_GPGETINFO )
        {
            if ( arg->lastname[0] == 0 )
            {
                GPResult gpResult;
                char pseudo_name[DWC_ACC_USERNAME_STRING_BUFSIZE];
                // Sets lastname since the profileID is used for the first time.
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    login id is authenticated. set lastname field.\n" );
                DWCi_Acc_LoginIdToUserName( &stpLoginCnt->userdata->pseudo, stpLoginCnt->gamecode, pseudo_name );
                gpResult = gpSetInfos( pconnection, GP_LASTNAME, pseudo_name );
                if (DWCi_HandleGPError(gpResult)) return;

                stpLoginCnt->state = DWC_LOGIN_STATE_GPSETINFO;

                // Gets data and checks whether or not this is actually set.
                gpResult = gpGetInfo( pconnection, arg->profile, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback, NULL );
                if (DWCi_HandleGPError(gpResult)) return;

                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    call gpSetInfos\n" );
            }
            else
            {
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    this login id is used by anybody.... retry.\n" );

                // Disconnect for now.
                gpDisconnect( pconnection );

                // Retry using a different PlayerID.
                DWCi_RemoteLogin( DWCi_RemoteAuthCallback, NULL );
                stpLoginCnt->state = DWC_LOGIN_STATE_REMOTE_AUTH;
            }
        }
        else if ( stpLoginCnt->state == DWC_LOGIN_STATE_GPSETINFO )
        {
            // Verify after data has been set
            char pseudo_name[DWC_ACC_USERNAME_STRING_BUFSIZE];
            DWCi_Acc_LoginIdToUserName( &stpLoginCnt->userdata->pseudo, stpLoginCnt->gamecode, pseudo_name );

            if ( strcmp( arg->lastname, pseudo_name ) == 0 )
            {
                char username[DWC_ACC_USERNAME_STRING_BUFSIZE];
                DWCi_Acc_LoginIdToUserName( &stpLoginCnt->tempLoginId, stpLoginCnt->gamecode, username );
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    Account is created : %s(%s) - %d.\n", username, pseudo_name, arg->profile );

                DWCi_Acc_SetLoginIdToUserData( stpLoginCnt->userdata, &stpLoginCnt->tempLoginId, arg->profile );

                gpDisconnect( pconnection ); // Cancels connection status.

                // The main connection.
                stRemoteAuthCallback( stpLoginCnt->authToken, stpLoginCnt->partnerChallenge, pStRemoteAuthParam );
            }
            else
            {
                // [todo] retry
                GPResult gpResult;
                DWC_Printf( DWC_REPORTFLAG_LOGIN, "    Login but gpSetInfo failed... %s : %d retry gpGetInfo.\n", arg->lastname, arg->profile );

                gpResult = gpGetInfo( pconnection, arg->profile, GP_DONT_CHECK_CACHE, GP_NON_BLOCKING, (GPCallback)DWCi_GPGetInfoCallback, NULL );
                if (DWCi_HandleGPError(gpResult)) return;
            }
        }
    }
    else
    {
        DWC_Printf( DWC_REPORTFLAG_LOGIN, " ERROR: gpGetInfo. why??? : %d\n", arg->result );
    }
}

BOOL
DWCi_CheckLogin( void )
{
    if ( stpLoginCnt != NULL && stpLoginCnt->state == DWC_LOGIN_STATE_CONNECTED )
    {
        return TRUE;
    }

    return FALSE;
}

BOOL
DWCi_GetAuthInfo( char** authToken, char** partnerChallenge )
{
    if ( !DWCi_CheckLogin() ) return FALSE;
    
    *authToken        = stpLoginCnt->authToken;
    *partnerChallenge = stpLoginCnt->partnerChallenge;

    return TRUE;
}
