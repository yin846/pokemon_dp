/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - account
  File:     dwc_account.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision: 1.16 $

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <bm/util_wifiidtool.h>
#include <base/dwc_account.h>

#define DWCi_CHECK_FLAG(flag,mask)      ((flag&mask)==mask)

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetMaskBits

  Description:  Gets some bit strings of u32 data.

  Arguments:    data   : u32 data that was used to store data to be retrieved
                shift : Number of spaces to shift bit strings right
                mask  : Bit mask used for bit strings that have been shifted

  Returns:      Bit string that was pulled out
 *---------------------------------------------------------------------------*/
static inline u32
DWCi_Acc_GetMaskBits( u32 data,
                      u32 shift,
                      u32 mask )
{
    return ((data >> shift) & mask);
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetMaskBits

  Description:  Some bit strings shorter than u32 are stored in u32 data.

  Arguments:    ptr   : Pointer to u32 data that needs to be stored
                data   : Bit string that needs to be stored
                shift : Number of spaces to shift bit string left
                mask  : Bit mask used for bit strings that have been shifted

  Returns:      TRUE if the bit string has been masked and the result written
                Does nothing and returns FALSE otherwise
 *---------------------------------------------------------------------------*/
static BOOL
DWCi_Acc_SetMaskBits( u32* ptr,
                      u32  data,
                      u32  shift,
                      u32  mask )
{
    // Checks whether or not the data is included in the mask.
    if ( (data & ~mask) != 0 )
    {
        return FALSE;
    }

    *ptr = (*ptr & ~(mask << shift)) | (data << shift);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetUserId

  Description:  Gets UserID from the DWCAccLoginId structure.

  Arguments:    userdata : Pointer to the DWCAccLoginId structure used to store user data

  Returns:      UserID 64bits
 *---------------------------------------------------------------------------*/
u64
DWCi_Acc_GetUserId( const DWCAccLoginId* userdata )
{
    u32 hi32;

    hi32 = DWCi_Acc_GetMaskBits( userdata->id_data, DWC_ACC_USERID_HI32_SHIFT, DWC_ACC_USERID_HI32_MASK );

    return (u64)hi32 << 32 | userdata->userid_lo32;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetPlayerId

  Description:  Gets the PlayerID from the DWCAccLoginID structure.

  Arguments:    userdata : Pointer to the DWCAccLoginId structure used to store user data

  Returns:      PlayerID 32bits
 *---------------------------------------------------------------------------*/
u32
DWCi_Acc_GetPlayerId( const DWCAccLoginId* userdata )
{
    return userdata->playerid;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetFriendKey

  Description:  Gets the friend key from the DWCAccFriendKey structure.

  Arguments:    data   : Pointer to the DWCAccFriendKey structure used to store user data

  Returns:      Friend key of 64 bits
 *---------------------------------------------------------------------------*/
u64
DWCi_Acc_GetFriendKey( const DWCAccFriendKey* data )
{
    return (u64)data->friendkey_hi32 << 32 | data->friendkey_lo32;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetGsProfileId

  Description:  Gets the gsProfileId from the DWCAccFriendKey structure.

  Arguments:    data   : Pointer to the DWCAccGsProfileId structure used to store user data

  Returns:      Friend key of 64 bits
 *---------------------------------------------------------------------------*/
int
DWCi_Acc_GetGsProfileId( const DWCAccGsProfileId* data )
{
    return data->id;
}


/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetUserId

  Description:  Stores the UserID in the DWCAccLoginId structure.

  Arguments:    userdata : Pointer to the DWCAccLoginId structure used to store user data
                userid   : UserID 64bits

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetUserId( DWCAccLoginId*  userdata,
                    u64             userid      )
{
    if ( !DWCi_Acc_SetMaskBits( &userdata->id_data, (u32)(userid>>32), DWC_ACC_USERID_HI32_SHIFT, DWC_ACC_USERID_HI32_MASK ) )
    {
        // userid value out of bounds
    }
    userdata->userid_lo32 = (u32)userid;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetPlayerId

  Description:  Stores the PlayerID in the DWCAccLoginId structure.

  Arguments:    userdata : Pointer to the DWCAccLoginId structure used to store user data
                playerid : PlayerID 64bits

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetPlayerId( DWCAccLoginId*    userdata,
                      u32               playerid        )
{
    userdata->playerid = playerid;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetPlayerId

  Description:  Stores the PlayerID in the DWCAccLoginId structure.

  Arguments:    userdata : Pointer to the DWCAccLoginId structure used to store user data
                playerid : PlayerID 64bits

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetFriendKey( DWCAccFriendKey* data,
                       u64              friendkey )
{
    data->friendkey_lo32 = (u32)friendkey;
    data->friendkey_hi32 = (u32)(friendkey >> 32);
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetGsProfileId

  Description:  Stores the GsProfile in the DWCAccLoginId structure.

  Arguments:    userdata : Pointer to the DWCAccLoginId structure used to store user data
                friendkey : PlayerID 64bits

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetGsProfileId( DWCAccGsProfileId* data, int gs_profile_id )
{
    data->id = gs_profile_id;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_GetFlags

  Description:  Gets flags from the DWCAccFlag structures

  Arguments:    userdata : Pointer to the DWCAccLoginId structure used to store user data

  Returns:      Flag (enabled, 12 bits)
 *---------------------------------------------------------------------------*/
static u32
DWCi_Acc_GetFlags( const DWCAccFlag* userdata )
{
    return DWCi_Acc_GetMaskBits( userdata->flags, DWC_ACC_FLAGS_SHIFT, DWC_ACC_FLAGS_MASK );
}

// Data type
static u32
DWCi_Acc_GetFlag_DataType( const DWCAccFlag* userdata )
{
    return DWCi_Acc_GetFlags( userdata ) & DWC_ACC_FRIENDDATA_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_IsBuddyFriendData

  Description:  If a GS profile ID is stored in the DWCAccFriendData structure,
                a check is made whether or not the ID is associated with a friend already registered on the server.

  Arguments:    frienddata : Pointer to the DWCAccFriendData structure used to store friend data

  Returns:      TRUE  : Already a friend
                FALSE : Not a friend yet.
 *---------------------------------------------------------------------------*/
BOOL
DWC_IsBuddyFriendData( const DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        u32 type;

        type = DWCi_Acc_GetFlags( &frienddata->flags );

        return ( DWCi_CHECK_FLAG( type, DWC_ACC_FRIENDDATA_ISBUDDY ) ) ? TRUE : FALSE;
    }

    return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         DWC_

  Description:  If a GS profile ID is stored in the DWCAccFriendData structure,
                a check is made whether or not the ID is associated with a friend already registered on the server.

  Arguments:    frienddata : Pointer to the DWCAccFriendData structure used to store friend data

  Returns:      TRUE  : Already a friend
                FALSE : Not a friend yet.
 *---------------------------------------------------------------------------*/
int
DWC_GetFriendDataType( const DWCAccFriendData* frienddata )
{
    return (int)DWCi_Acc_GetFlag_DataType( &frienddata->flags );
}


/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetFlags

  Description:  Stores the flag in the DWCAccLoginId structure.

  Arguments:    userdata : Pointer to the DWCAccLoginId structure used to store user data
                flags    : Flag (enabled, 12 bits)

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
DWCi_Acc_SetFlags( DWCAccFlag* userdata,
                   u32         flags )
{
    DWCi_Acc_SetMaskBits( &userdata->flags, flags, DWC_ACC_FLAGS_SHIFT, DWC_ACC_FLAGS_MASK );
}

// Data type
static void
DWCi_Acc_SetFlag_DataType( DWCAccFlag* userdata,
                           int         type )
{
    u32 flags;

    flags = DWCi_Acc_GetFlags( userdata );

    flags = ( flags & ~DWC_ACC_FRIENDDATA_MASK ) | type;

    DWCi_Acc_SetFlags( userdata, flags );
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_SetBuddyFriendData

  Description:  If a GS profile ID is stored in the DWCAccFriendData structure,
                Set if the ID is associated with a friend already registered on the server.

  Arguments:    frienddata : Pointer to the DWCAccFriendData structure used to store friend data

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_SetBuddyFriendData( DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        u32 flags;

        flags = DWCi_Acc_GetFlags( &frienddata->flags );

        flags = ( flags &  ~DWC_ACC_FRIENDDATA_ISBUDDY_MASK ) | DWC_ACC_FRIENDDATA_ISBUDDY;

        DWCi_Acc_SetFlags( &frienddata->flags, flags );
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_ClearBuddyFlagFriendData

  Description:  If a GS profile ID is stored in the DWCAccFriendData structure,
                clears the friend establishment flag that was set by the friend data.

  Arguments:    frienddata : Pointer to the DWCAccFriendData structure used to store friend data

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_ClearBuddyFlagFriendData( DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        u32 flags;

        flags = DWCi_Acc_GetFlags( &frienddata->flags );

        flags &=  ~DWC_ACC_FRIENDDATA_ISBUDDY_MASK;

        DWCi_Acc_SetFlags( &frienddata->flags, flags );
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_CreateFriendKey

  Description:  Creates a friend key from the GS ProfileId

  Arguments:    userdata : Pointer to the structure storing the GS Profile ID that is used as the basis for a friend key

  Returns:      None
 *---------------------------------------------------------------------------*/
u64
DWC_Acc_CreateFriendKey( int gs_profile_id,
                         u32 gamecode )
{
    MATHCRC8Table table;
    u32 code[2];
    int crc;

    code[0] = (u32)gs_profile_id;
    code[1] = gamecode;

    MATH_CRC8InitTable( &table );
    crc = MATH_CalcCRC8( &table, code, sizeof( code ) ) & 0x7f; // Only 7 bits valid.

    return ( ((u64)crc << 32) | (u32)gs_profile_id );
}


/*---------------------------------------------------------------------------*
  Name:         DWC_CheckFriendKey

  Description:  Checks whether or not the friend key is correct.

  Arguments:    userdata : user data
                friendkey : Friend key

  Returns:      TRUE  : Correct
                FALSE : Invalid code
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckFriendKey( const DWCAccUserData* userdata, u64 friendkey )
{
    return DWC_Acc_CheckFriendKey( friendkey, userdata->gamecode );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_CheckFriendKey

  Description:  Checks whether or not the friend key is correct.

  Arguments:    friendkey : Friend key
                gamecode  : Value to be used for invalid code key

  Returns:      TRUE  : Correct
                FALSE : Invalid code
 *---------------------------------------------------------------------------*/
BOOL
DWC_Acc_CheckFriendKey( u64 friendkey,
                        u32 gamecode )
{
    MATHCRC8Table table;
    u32 code[2];
    int crc;

    // negative GS profile IDs are invalid
    if ( friendkey & 0x80000000 ) return FALSE;

    code[0] = (u32)friendkey;
    code[1] = gamecode;

    MATH_CRC8InitTable( &table );
    crc = MATH_CalcCRC8( &table, code, sizeof( code ) ) & 0x7f; // Only 7 bits valid.

    if ( crc != (friendkey >> 32) )
    {
        return FALSE;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_FriendKeyToGsProfileId

  Description:  Retrieves the GS ProfileId from the friend registration key

  Arguments:    friendkey : Friend key
                gamecode  : Value to be used for invalid code key

  Returns:      Retrieval failed if 0 is returned (invalid friend registration key)
                Otherwise the GS ProfileId is returned.
 *---------------------------------------------------------------------------*/
int
DWC_Acc_FriendKeyToGsProfileId( u64 friendkey,
                                u32 gamecode )
{
    if ( !DWC_Acc_CheckFriendKey( friendkey, gamecode ) )
    {
        return 0;
    }

    return (int)( friendkey & 0xffffffff );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_FriendKeyToNumericKey

  Description:  The friend registration key is converted to a text string that represents its numeric value

  Arguments:    friendkey : The friend registration key (representing a numeric value)
                numerickey : The friend registration key (representing a numeric value) is output

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_Acc_FriendKeyToNumericKey( u64   friendkey,
                               char* numerickey )
{
    OS_SNPrintf( numerickey, DWC_ACC_FRIENDKEY_STRING_BUFSIZE, "%012llu", friendkey );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_NumericKeyToFriendKey

  Description:  The friend registration key as given by a text string representing a numeric values is converted into that numeric value

  Arguments:    numerickey : The friend registration key (representing a numeric value) is used as input

  Returns:      Friend registration key (representing a numeric value)
 *---------------------------------------------------------------------------*/
u64
DWC_Acc_NumericKeyToFriendKey( const char* numerickey )
{
    int i;
    u64 result = 0;
    u64 dec = 1;

    for ( i = 0 ; i < DWC_ACC_FRIENDKEY_STRING_LENGTH ; ++i )
    {
        result +=  (*(numerickey + DWC_ACC_FRIENDKEY_STRING_LENGTH - 1 - i) - '0') * dec;

        dec *= 10;
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_U64ToString32

  Description:  Conversion to a base-32 text string 5 bits are represented using a single character

  Arguments:    num    : Numeric value to be converted
                bitnum : Bit length to be converted
                result : Buffer that stores the result (bitnum+4)/5 + 1 bytes
                         Size of region required.

  Returns:      None
 *---------------------------------------------------------------------------*/
static void
DWCi_Acc_U64ToString32( u64   num,
                        int   bitnum,
                        char* result )
{
    int i;
    const int loopnum = (bitnum + 4) / 5;
    const char* const sStringTable = "0123456789abcdefghijklmnopqrstuv";
    //                                          0123456789012345678901

    for ( i = 0 ; i < loopnum ; ++i )
    {
        *(result + loopnum - 1 - i) = *(sStringTable + (num & 0x1f));

        num >>= 5;
    }

    *( result + loopnum ) = 0; // NULL termination inserted.
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_LoginIdToUserName

  Description:  The LoginId in the DWCAccLoginId structure is converted into its text representation.
                9 digits of user data + game code + 7 digits of Player ID.
                A 21-byte output region, including NULL termination, is required.

  Arguments:    loginid  : Pointer to the DWCAccLoginId structure used to store user data
                gamecode  : Game code
                username : Buffer that stores the result 21 bytes required.

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_LoginIdToUserName( const DWCAccLoginId* loginid,
                            u32                  gamecode,
                            char*                username )
{
    char userid[DWC_ACC_USERNAME_STRING_BUFSIZE];
    char playerid[DWC_ACC_USERNAME_STRING_BUFSIZE];

    DWCi_Acc_U64ToString32( DWCi_Acc_GetUserId( loginid ), DWC_ACC_USERID_BITS, userid );
    DWCi_Acc_U64ToString32( DWCi_Acc_GetPlayerId( loginid ), DWC_ACC_PLAYERID_BITS, playerid );

    OS_SNPrintf( username, DWC_ACC_USERNAME_STRING_BUFSIZE,
                 "%s%c%c%c%c%s",
                 userid,
                 (u8)(gamecode >> 24), (u8)(gamecode >> 16), (u8)(gamecode >> 8), (u8)gamecode,
                 playerid );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_Acc_CreateUserData

  Description:  Create user data.

  Arguments:    userdata : Buffer that stores user data
                gamecode  : Game code assigned by Nintendo to the application

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_CreateUserData( DWCAccUserData* userdata, u32 gamecode )
{
    MATHCRC32Table      table;

    MI_CpuClear8( userdata, DWC_ACC_USERDATA_BUFSIZE );

    // Sets user data
    userdata->size = DWC_ACC_USERDATA_BUFSIZE;
    userdata->gs_profile_id = 0;
    userdata->gamecode = gamecode;

    // Generates a temporary LoginId.
    DWCi_Acc_CreateTempLoginId( &userdata->pseudo );

    // Since an authenticated ID cannot be generated at the time user data is created, nothing is stored here.
    DWCi_Acc_SetFlag_DataType( (DWCAccFlag*)&userdata->authentic, DWC_ACC_FRIENDDATA_NODATA   );

    // CRC calculation
    MATH_CRC32InitTable( &table );
    userdata->crc32 = MATH_CalcCRC32( &table, userdata, sizeof( DWCAccUserData ) - 4 );

    // Enable the dirty flag.
    userdata->flag |= DWC_ACC_USERDATA_DIRTY;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_CreateTempLoginId

  Description:  Creates a temporary LoginID.

  Arguments:    loginid  : Pointer to address in which the LoginID to be checked has been stored

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_CreateTempLoginId( DWCAccLoginId* loginid )
{
    DWCAuthWiFiId       authid;
    MATHRandContext32   randcontext;
	u32					buffer[8];
	u32					i;
	
	// [nakata] changed initialization of random context from a tick base to a LowEntropyData base
	OS_GetLowEntropyData(buffer);
	for(i=1; i<8; i++) {
		buffer[i] = buffer[i]^buffer[i-1];
	}
    MATH_InitRand32( &randcontext, buffer[7] );

    // Insert this UserID.
    DWC_Auth_GetId( &authid );
    if ( authid.flg )
    {
        DWCi_Acc_SetUserId( loginid, authid.uId );
    }
    else
    {
        DWCi_Acc_SetUserId( loginid, authid.notAttestedId );
    }
#if 0
    DWCi_Acc_SetPlayerId( loginid, 1 ); // Set to fixed value for debugging.
#else
    DWCi_Acc_SetPlayerId( loginid, MATH_Rand32( &randcontext, 0 ) );
#endif
    DWCi_Acc_SetFlag_DataType( (DWCAccFlag*)loginid,    DWC_ACC_FRIENDDATA_LOGIN_ID );
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_CheckConsoleUserId

  Description:  Compares the UserID for the login to the UserId for the console

  Arguments:    loginid  : Pointer to address in which the LoginID to be checked has been stored

  Returns:      TRUE  : Same
                FALSE : Different
 *---------------------------------------------------------------------------*/
BOOL
DWCi_Acc_CheckConsoleUserId( const DWCAccLoginId* loginid )
{
    DWCAuthWiFiId       authid;
    
    DWC_Auth_GetId( &authid );
    if ( authid.flg )
    {
        return (DWCi_Acc_GetUserId( loginid ) == authid.uId) ? TRUE : FALSE;
    }
    else
    {
        return (DWCi_Acc_GetUserId( loginid ) == authid.notAttestedId) ? TRUE : FALSE;
    }
}


/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_IsValidLoginId

  Description:  Checks whether or not the LoginId is correct.

  Arguments:    loginid  : Pointer to address in which the LoginID to be checked has been stored

  Returns:      TRUE  : Stores the LoginID
                FALSE : Stored data is not a LoginID
 *---------------------------------------------------------------------------*/
BOOL
DWCi_Acc_IsValidLoginId( const DWCAccLoginId* loginid )
{
    return ( DWCi_Acc_GetFlag_DataType( (DWCAccFlag*)loginid ) == DWC_ACC_FRIENDDATA_LOGIN_ID ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_IsAuthentic

  Description:  Checks whether or not the user data has been authenticated.

  Arguments:    userdata : Pointer to address in which the LoginID to be checked has been stored

  Returns:      TRUE  : Already authenticated.
                FALSE : Not yet authenticated.
 *---------------------------------------------------------------------------*/
BOOL
DWCi_Acc_IsAuthentic( const DWCAccUserData* userdata )
{
    return DWCi_Acc_IsValidLoginId( &userdata->authentic );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_IsValidFriendData / DWCi_Acc_IsValidFriendData

  Description:  Checks whether or not valid data has been set in FriendData.

  Arguments:    frienddata : Pointer to FriendData structure

  Returns:      TRUE  : Valid data
                FALSE : Invalid data (or no data)
 *---------------------------------------------------------------------------*/
BOOL
DWC_IsValidFriendData( const DWCAccFriendData* frienddata )
{
    u32 type = DWCi_Acc_GetFlag_DataType( &frienddata->flags );

    if ( type ==  DWC_ACC_FRIENDDATA_NODATA )
    {
        return FALSE;
    }

    return TRUE;
}

// For backward compatibility.
BOOL
DWCi_Acc_IsValidFriendData( const DWCAccFriendData* frienddata )
{
    return DWC_IsValidFriendData( frienddata );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CreateUserData

  Description:  Create user data. This user data should be
                stored in buffer memory.

  Arguments:    userdata : Pointer to the address in which the generated user data is stored

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_CreateUserData( DWCAccUserData* userdata, u32 gamecode )
{
    DWCi_Acc_CreateUserData( userdata, gamecode );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CheckUserData

  Description:  Check user data.

  Arguments:    userdata : Pointer to address in which the user data to be checked has been stored

  Returns:      TRUE  : Normal
                FALSE : Broken
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckUserData( const DWCAccUserData* userdata )
{
    u32                 crc32;
    MATHCRC32Table      table;

    MATH_CRC32InitTable( &table );
    crc32 = MATH_CalcCRC32( &table, userdata, sizeof( DWCAccUserData ) - 4 );

    return ( crc32 == userdata->crc32 ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CheckHasProfile

  Description:  Checks whether or not user data has already obtained a Profile ID.

  Arguments:    userdata : Pointer to address in which the user data to be checked has been stored

  Returns:      TRUE  : Profile ID already obtained
                FALSE : Profile ID not yet obtained
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckHasProfile( const DWCAccUserData* userdata )
{
    return ( DWCi_Acc_IsValidLoginId( &userdata->authentic ) && userdata->gs_profile_id > 0 ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CheckValidConsole

  Description:  Checks the integrity of the user data and the console.

  Arguments:    userdata : Pointer to address in which the user data to be checked has been stored

  Returns:      TRUE  : The user ID and password stored for the console can be used
                FALSE : Cannot be used
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckValidConsole( const DWCAccUserData* userdata )
{
    DWCAuthWiFiId       authid;

    if ( DWCi_Acc_GetFlag_DataType( (DWCAccFlag*)&userdata->authentic ) == DWC_ACC_FRIENDDATA_NODATA )
    {
        // TRUE if there is no authenticated Login ID
        return TRUE;
    }

    DWC_Auth_GetId( &authid );

    if ( !authid.flg )
    {
        // FALSE unless the console's user ID has been authenticated (regardless of whether or not there is an authenticated Login ID)
        return FALSE;
    }

    return ( DWCi_Acc_GetUserId( &userdata->authentic ) == authid.uId ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_SetUserIdToUserData

  Description:  Set information such as UserID and PlayerID for the user data.

  Arguments:    userdata : Buffer that stores user data
                userid   : User ID
                playerid : Player ID

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_SetLoginIdToUserData( DWCAccUserData*      userdata,
                               const DWCAccLoginId* loginid,
                               int                  gs_profile_id )
{
    MATHCRC32Table      table;

    // Set the LoginID.
    userdata->authentic = *loginid;

    // Set the gs_profile_id
    userdata->gs_profile_id = gs_profile_id;

    // CRC calculation
    MATH_CRC32InitTable( &table );
    userdata->crc32 = MATH_CalcCRC32( &table, userdata, sizeof( DWCAccUserData ) - 4 );

    // Enable the dirty flag.
    userdata->flag |= DWC_ACC_USERDATA_DIRTY;
}


/*---------------------------------------------------------------------------*
  Name:         DWC_CheckDirtyFlag / DWCi_Acc_IsDirty

  Description:  Checks the user data modification flag.

  Arguments:    userdata : Buffer that stores user data

  Returns:      TRUE  : User data has changed.
                FALSE : User data has not changed.
 *---------------------------------------------------------------------------*/
BOOL
DWC_CheckDirtyFlag( const DWCAccUserData* userdata )
{
    return DWCi_Acc_IsDirty( userdata );
}

BOOL
DWCi_Acc_IsDirty( const DWCAccUserData* userdata )
{
    return DWCi_CHECK_FLAG( userdata->flag, DWC_ACC_USERDATA_DIRTY ) ? TRUE : FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_ClearDirty

  Description:  Clears the user data modification flag.

  Arguments:    userdata : Buffer that stores user data

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_ClearDirtyFlag( DWCAccUserData* userdata )
{
    DWCi_Acc_ClearDirty( userdata );
}

void
DWCi_Acc_ClearDirty( DWCAccUserData* userdata )
{
    MATHCRC32Table      table;

    userdata->flag &= ~DWC_ACC_USERDATA_DIRTY;

    // CRC calculation
    MATH_CRC32InitTable( &table );
    userdata->crc32 = MATH_CalcCRC32( &table, userdata, sizeof( DWCAccUserData ) - 4 );
}

/*---------------------------------------------------------------------------*
  Name:         DWCi_Acc_TestFlagFunc

  Description:  Internal function used for testing.

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWCi_Acc_TestFlagFunc( void )
{
    DWCAccLoginId id;
    u32 flags;

    MI_CpuClear8( &id, sizeof( DWCAccLoginId ) );

    flags = DWCi_Acc_GetFlags( (DWCAccFlag*)&id );
    OS_TPrintf("%08x: %08x\n",flags, DWCi_Acc_GetFlag_DataType((DWCAccFlag*)&id ) );
    if ( DWCi_Acc_IsValidLoginId( &id ) )
    {
        OS_TPrintf("valid\n");
    }
    else
    {
        OS_TPrintf("invalid\n");
    }

    DWCi_Acc_SetFlag_DataType( (DWCAccFlag*)&id , DWC_ACC_FRIENDDATA_LOGIN_ID );

    flags = DWCi_Acc_GetFlags( (DWCAccFlag*)&id );
    OS_TPrintf("%08x: %08x\n",flags, DWCi_Acc_GetFlag_DataType((DWCAccFlag*)&id ) );
    if ( DWCi_Acc_IsValidLoginId( &id ) )
    {
        OS_TPrintf("valid\n");
    }
    else
    {
        OS_TPrintf("invalid\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_GetFriendKey

  Description:  Gets the friend key from the FriendData.

  Arguments:    frienddata : Pointer to FriendData structure

  Returns:      non-zero: Friend key
                0     : Incorrect input
 *---------------------------------------------------------------------------*/
u64
DWC_GetFriendKey( const DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_FRIEND_KEY )
    {
        return DWCi_Acc_GetFriendKey( &frienddata->friend_key );
    }

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_GetGsProfileId

  Description:  Gets the gsProfileId from the FriendData.

  Arguments:    userdata : Pointer to the address storing the user data for this user
                frienddata : Pointer to FriendData structure

  Returns:      non-zero: gsProfileId
                0     : Incorrect input
                -1    : Temporary ID -> Get the LoginID text string data and perform a search.
 *---------------------------------------------------------------------------*/
int
DWC_GetGsProfileId( const DWCAccUserData* userdata,
                    const DWCAccFriendData* frienddata )
{
    u32 type = DWCi_Acc_GetFlag_DataType( &frienddata->flags );

    switch ( type )
    {
    case DWC_ACC_FRIENDDATA_FRIEND_KEY:
        {
            u64 friend_key = DWCi_Acc_GetFriendKey( &frienddata->friend_key );

            if ( DWC_Acc_CheckFriendKey( friend_key, userdata->gamecode ) )
            {
                return DWC_Acc_FriendKeyToGsProfileId( friend_key, userdata->gamecode );
            }

            return 0;
        }
        // NOT REACH HERE
        break;

    case DWC_ACC_FRIENDDATA_GS_PROFILE_ID:
        return DWCi_Acc_GetGsProfileId( &frienddata->gs_profile_id );
        // NOT REACH HERE
        break;
    case DWC_ACC_FRIENDDATA_LOGIN_ID:
        return -1;
        // NOT REACH HERE
        break;
    default:
        // NOT REACH HERE
        break;
    }

    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CreateFriendKey

  Description:  Generates a friend key to be passed to others based on user data.
                The friend key depends on the game code.

  Arguments:    userdata : Pointer to the address storing the user data for this user

  Returns:      non-zero: Friend key
                0     : Input incorrect (e.g. unable to retrieve gsProfileID, etc.)
 *---------------------------------------------------------------------------*/
u64
DWC_CreateFriendKey( const DWCAccUserData* userdata )
{
    u64 ret = 0;

    if ( userdata->gs_profile_id != 0 )
    {
        ret = DWC_Acc_CreateFriendKey( userdata->gs_profile_id, userdata->gamecode );
    }

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CreateFriendKeyToken

  Description:  Creates FriendData to be stored in a friend roster based on the friend key.

  Arguments:    token      : Pointer to the address used to store FriendData
                friend_key : Friend key

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_CreateFriendKeyToken( DWCAccFriendData* token,
                          u64 friend_key )
{
    MI_CpuClear8( token, sizeof( DWCAccFriendData ) );
    DWCi_Acc_SetFriendKey( &token->friend_key, friend_key );
    DWCi_Acc_SetFlag_DataType( &token->flags, DWC_ACC_FRIENDDATA_FRIEND_KEY );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_CreateExchangeToken

  Description:  Creates FriendData to be stored in a friend roster based on the friend key.

  Arguments:    userdata : Pointer to the address storing user data
                token      : Pointer to the address used to store FriendData

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_CreateExchangeToken( const DWCAccUserData* userdata,
                         DWCAccFriendData* token )
{
    MI_CpuClear8( token, sizeof( DWCAccFriendData ) );

    if ( DWCi_Acc_IsAuthentic( userdata ) )
    {
        DWCi_Acc_SetGsProfileId( &token->gs_profile_id, userdata->gs_profile_id );
        DWCi_Acc_SetFlag_DataType( &token->flags, DWC_ACC_FRIENDDATA_GS_PROFILE_ID );
    }
    else
    {
        // Although various flags have been inserted in an attempt to increase the probability,
        // due to the decision not to tie the console for which a temp ID is being generated to the console connecting to the net for which this ID is to be gotten
        // nearly all of the information is wasted.
        token->login_id = userdata->pseudo;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DWC_SetGsProfileId

  Description:  Stores the gsProfileId in the friend roster.

  Arguments:    frienddata : Pointer to FriendData structure
                gs_profile_id : gsProfileId

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_SetGsProfileId( DWCAccFriendData* frienddata,
                    int gs_profile_id )
{
    MI_CpuClear8( frienddata, sizeof( DWCAccFriendData ) );

    DWCi_Acc_SetGsProfileId( &frienddata->gs_profile_id, gs_profile_id );
    DWCi_Acc_SetFlag_DataType( &frienddata->flags, DWC_ACC_FRIENDDATA_GS_PROFILE_ID );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_LoginIdToUserName

  Description:  Gets the user name to be used in the search for a friend.

  Arguments:    userdata : Pointer to the address storing user data
                token      : Pointer to the address storing FriendData
                username : Buffer to which results are output (DWC_ACC_USERNAME_STRING_BUFSIZE)

  Returns:      None
 *---------------------------------------------------------------------------*/
void
DWC_LoginIdToUserName( const DWCAccUserData* userdata,
                       const DWCAccFriendData* frienddata,
                       char* username )
{
    DWCi_Acc_LoginIdToUserName( &frienddata->login_id, userdata->gamecode, username );
}

/*---------------------------------------------------------------------------*
  Name:         DWC_IsEqualFriendData

  Description:  Checks whether or not DWCFriendData is the same.

  Arguments:    frienddata1 : Pointer to friend data to be compared
                frienddata2 : Pointer to friend data to be compared

  Returns:      TRUE  : Same
                FALSE : Different
 *---------------------------------------------------------------------------*/
BOOL
DWC_IsEqualFriendData( const DWCAccFriendData* frienddata1,
                       const DWCAccFriendData* frienddata2 )
{
    u32 type1, type2;
    type1 = DWCi_Acc_GetFlag_DataType( &frienddata1->flags );
    type2 = DWCi_Acc_GetFlag_DataType( &frienddata2->flags );

    if ( type1 != type2 )
    {
        return FALSE;
    }
    else if ( type1 == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        return ( DWCi_Acc_GetGsProfileId( &frienddata1->gs_profile_id ) == DWCi_Acc_GetGsProfileId( &frienddata2->gs_profile_id ) ) ?
            TRUE : FALSE;
    }
    else if ( type1 == DWC_ACC_FRIENDDATA_LOGIN_ID )
    {
        return ( ( DWCi_Acc_GetUserId  ( &frienddata1->login_id ) == DWCi_Acc_GetUserId  ( &frienddata2->login_id ) ) &&
                 ( DWCi_Acc_GetPlayerId( &frienddata1->login_id ) == DWCi_Acc_GetPlayerId( &frienddata2->login_id ) )    ) ?
            TRUE : FALSE;
    }
    else if ( type1 == DWC_ACC_FRIENDDATA_FRIEND_KEY )
    {
        return ( DWCi_Acc_GetFriendKey( &frienddata1->friend_key ) == DWCi_Acc_GetFriendKey( &frienddata2->friend_key ) ) ?
            TRUE : FALSE;
    }

    return FALSE;
}

void
DWC_ReportFriendData( const DWCAccUserData* userdata, const DWCAccFriendData* frienddata )
{
    if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_GS_PROFILE_ID )
    {
        if ( DWC_IsBuddyFriendData( frienddata ) )
        {
            OS_TPrintf(" GS_ID : %d (ok)\n", DWCi_Acc_GetGsProfileId( &frienddata->gs_profile_id ));
        }
        else
        {
            OS_TPrintf(" GS_ID : %d\n", DWCi_Acc_GetGsProfileId( &frienddata->gs_profile_id ));
        }
    }
    else if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_FRIEND_KEY )
    {
        char name[DWC_ACC_FRIENDKEY_STRING_BUFSIZE];
        DWC_Acc_FriendKeyToNumericKey( DWCi_Acc_GetFriendKey( &frienddata->friend_key ), name );
        OS_TPrintf(" F_KEY : %s\n", name );
    }
    else if ( DWCi_Acc_GetFlag_DataType( &frienddata->flags ) == DWC_ACC_FRIENDDATA_LOGIN_ID )
    {
        char name[DWC_ACC_USERNAME_STRING_BUFSIZE];
        DWCi_Acc_LoginIdToUserName( &frienddata->login_id, userdata->gamecode, name );
        OS_TPrintf(" LN_ID : %s\n", name );
    }
    else
    {
        OS_TPrintf(" NO_DATA \n");
    }
    
}

void
DWC_ReportUserData( const DWCAccUserData* userdata )
{
    OS_TPrintf("*******************************\n");
    OS_TPrintf(" [pseudo login id]\n");
    DWC_ReportFriendData( userdata, (DWCAccFriendData*)&userdata->pseudo );
    OS_TPrintf("+++++++++++++++++++++++++++++++\n");
    OS_TPrintf(" [authentic login id]\n");
    DWC_ReportFriendData( userdata, (DWCAccFriendData*)&userdata->authentic );
    OS_TPrintf("+++++++++++++++++++++++++++++++\n");
    OS_TPrintf(" GS_ID : %d\n", userdata->gs_profile_id );
    OS_TPrintf("*******************************\n");
}
