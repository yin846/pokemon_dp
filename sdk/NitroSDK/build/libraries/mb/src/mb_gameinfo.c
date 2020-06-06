/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - libraries
  File:     mb_gameinfo.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

    $Log: mb_gameinfo.c,v $
    Revision 1.45  03/13/2006 06:33:33  yosizaki
    add MB_FAKESCAN_PARENT_BEACON.

    Revision 1.44  2006/02/20 02:34:17  seiki_masashi
    Removed WMGameInfo.version and added magicNumber and ver 
    Changed name of WMGameInfo.gameNameCount_attribute to attribute 
    Removed support for WM_GAMEINFO_TYPE_OLD 

    Revision 1.43  2006/01/18 02:11:30  kitase_hirotake
    do-indent

    Revision 1.42  11/18/2005 05:31:12  yosizaki
    fix MBi_ReadyBeaconSendStatus().

    Revision 1.41  11/14/2005 05:37:28  yosizaki
    fix about WM_ATTR_FLAG_ENTRY flag.

    Revision 1.40  03/04/2005 12:35:05  yosiokat
    Fixed a bug in MB_DeleteGameInfo that caused game delivery to operate incorrectly after deleting a game.
    The problem would arise if the game was deleted while the beacon was delivering game information about that game.

    Revision 1.39  02/28/2005 05:26:24  yosizaki
    do-indent.

    Revision 1.38  02/21/2005 00:39:34  yosizaki
    replace prefix MBw to MBi.

    Revision 1.37  02/18/2005 11:56:03  yosizaki
    fix around hidden warnings.

    Revision 1.36  2005/01/24 23:48:25  takano_makoto
    Revised ASSERT

    Revision 1.35  01/18/2005 05:31:03  takano_makoto
    Revised things so even data after the gameIntroduction NUL character gets sent.

    Revision 1.33  01/17/2005 09:29:06  takano_makoto
    Changed MB_XXXUserBeaconData to MB_XXXUserVolatData. Added MB_SetSendVolatCallback function.

    Revision 1.32  01/17/2005 06:26:34  takano_makoto
    In MB_SetUserBeaconData, prohibited interrupts when data is set.

    Revision 1.31  01/17/2005 05:54:29  takano_makoto
    Added MB_SetUserBeaconData, MB_GetUserBeaconData.

    Revision 1.30  01/11/2005 07:41:13  takano_makoto
    fix copyright header.

    Revision 1.29  11/22/2004 12:43:44  takano_makoto
    Made changes not to use the MBw function. Added MB_GetGameInfoRecvList.

    Revision 1.28  11/10/2004 07:40:03  takano_makoto
    Eliminated MBw function from mb_gameinfo.c, and cut out the scan parameter settings    

    Revision 1.27  11/10/2004 03:17:52  takano_makoto
    Made corrections so that the buffer allocated with mbrs is passed from the outside.

    Revision 1.26  11/02/2004 18:17:58  takano_makoto
    fix DEBUG Print.

    Revision 1.24  10/28/2004 11:01:42  yada
    just fix comment

    Revision 1.23  10/05/2004 09:45:29  terui
    Standardized MAC addresses as type u8 arrays.

    Revision 1.22  10/04/2004 13:40:25  terui
    Standardized game group IDs as type u32.

    Revision 1.21  09/29/2004 05:12:28  sato_masaki
    Corrected an error where the GGID for each game was not passed to BssDesc.
    
    Revision 1.20  09/28/2004 12:49:48  yosiokat
    IPL branch 1.14.2.7 and merge.     
    
    Revision 1.14.2.7  2004/09/28 10:53:13  yosiokat
    - Corrected an error where the beaconNo was not incremented in MBi_SendVolatBeacon.
    - Changed MB_SAME_BEACON_RECV_MAX_COUNT to "3".
    
    Revision 1.14.2.6  2004/09/28 02:42:11  yosiokat
    Deleted the processing in MBi_UnlockScanTarget for seeking and locking the next lock target during unlock execution.
    
    Revision 1.14.2.5  2004/09/28 02:40:25  yosiokat
    - Added a beaconNo for incrementing each time transmission is made to the parent beacon.     
    - Added processing to invalidate parent information when a child repeatedly receives a parent's beacon whose beaconNo is not moving.
    
    Revision 1.14.2.4  2004/09/21 08:31:05  yosiokat
    Corrected an error where things go haywire when doing an add -> delete -> add of game information in the parent.    
    
    Revision 1.19  09/22/2004 09:41:42  sato_masaki
    IPL_branch 1.14.2.4 and merge     
    (Corrected an error where things go haywire when doing an add -> delete -> add of game information in the parent.)
    
    Revision 1.18  09/20/2004 10:45:41  yosiokat
    Merged with the IPL branch Rev. 1.14.2.3
    
    Revision 1.14.2.3  2004/09/20 10:15:49  yosiokat
    - Deleted __UGI_IN_GGID. Set to a formula where the GGID is always inside the beacon.    
    - Deleted MBi_ClearFixedInfo.
    - Split up the SSID configuration inside MB_RecvGameInfoBeacon into the MBi_SetSSIDToBssDesc function.    
    - Set the TGID check performed inside MBi_GetStoreElement so that it is performed inside MBi_AnalyzeBeacon as MBi_CheckTGID.
    - Corrected the sequence number check inside MBi_CheckSeqNoFixed. (One part was moved to MBi_CheckSeqNoFixed)
    - Voided the return values of MBi_CheckSeqNoFixed and MBi_CheckSeqNoVolat.
    - Deleted seqNoFixed and seqNoVolat from MbGameInfoRecvList, and made corrections so that it uses the seqNoFixed and seqNoVolat inside the same gameInfo.
    
    Revision 1.14.2.2  2004/09/18 08:41:04  miya
    Merged with HEAD
    
    Revision 1.17  09/17/2004 04:00:38  miya
    Fixed a bug where the 2nd and successive game distribution information was not being received.
    
    Revision 1.16  09/16/2004 12:20:42  miya
    Conversions for file updates     
    
    Revision 1.15  09/16/2004 11:22:22  miya
    Addition of members for file updates
    
    Revision 1.14  09/15/2004 12:42:42  yosiokat
    Corrected where beacon reception was not performing well when a play member was deleted
    
    Revision 1.13  09/15/2004 09:22:27  yosiokat
    Fixed a bug where mbss.seqNoVolat was not being set when switching games inside MBi_ReadyBeaconSendStatus.
    
    Revision 1.12  09/15/2004 07:03:10  yosiokat
    - Set up the usefulGameInfoFlag, and set it so that it continues to be active until the Parent Device data lifespan arrives once a complete set of Parent Device data is received.    
    - Put both seqNoFixed and seqNoVolat in each beacon, and made changes so that it performs the scan lock determination.    
    
    Revision 1.11  09/14/2004 14:40:14  yosiokat
    small fix.
    
    Revision 1.10  09/14/2004 14:22:19  yosiokat
    - Put the checksum into the MbBeacon, and made changes so that when the beacon is received, a beacon that the checksum confirmation does not pass through is gotten rid of.    
    - Moved the constants that are only used inside mb_gameinfo.c from mb.h.
    - Corrected the definition of MB_SEND_MEMBER_MAX_NUM.
    
    Revision 1.9  09/13/2004 09:28:16  yosiokat
    -  Added wmTypeEvent to the argument of MB_CountGameInfoLifetime as a means to prevent no lock target being found when scan locking.
    - Set so that when performing scan lock operations, if a lock target is not found four times in succession, the lock is released.    
    
    Revision 1.8  09/11/2004 12:47:47  yosiokat
    -  Corrected the problem where, when scan locking, the lock time and the Parent Device information lifespan count process could not be supported in the process of modifying the scan time.
    
    Revision 1.7  09/10/2004 01:31:06  sato_masaki
    Changes to the PlayerFlag format. (operations complete)
    
    Revision 1.6  09/09/2004 14:01:47  sato_masaki
    In mb_gameinfo.c, changed the PlayerFlag format. (Ongoing task)
    
    Revision 1.5  09/09/2004 10:04:17  sato_masaki
    small fix
    
    Revision 1.4  09/09/2004 09:22:40  sato_masaki
    Changed the method of setting in the SSID of the GGID and TGID.
    
    Revision 1.3  09/08/2004 13:01:30  sato_masaki
    Added a process to the MBi_SetMaxScanTime() function for specifying the Scan time when the scan target is locked and when it is unlocked.
    WM_ERRCODE_INVALID_POLLBITMAP.
    
    Revision 1.2  09/06/2004 12:00:38  sato_masaki
    - Made changes so that the configuration of the fileNo to GameInfo is performed inside MB_RegisterFile()(mb_fileinfo.c),
     so it was deleted from MB_AddGameInfo().
    - Deleted the Parent Device GGID check inside MBi_CheckMBParent(), since
     the Child Device will become unable to keep the Parent Device GGID information
     due to the GGID of the Parent Device program differing depending on the application (configured at initialization).
    - Changed the GGID when SetGameInfo is performed to a Parent Device unique GGID configured at WM initialization.
    - Changed so that in MB_RecvGameInfoBeacon(), using the game's GGID and the parent's TGID,
     it generates an SSID to pass to the child.
    
    Revision 1.1  09/03/2004 07:04:36  sato_masaki
    Divided files according to features.
    
    Revision 1.15  08/26/2004 09:53:00  sato_masaki
    Fixed the TGID at 1 (measures for checking the SSID)
    
    Revision 1.14  08/26/2004 04:54:08  sato_masaki
    OS_Printf is changed into MB_DEBUG_OUTPUT
    
    Revision 1.13  08/19/2004 22:55:25  yosiokat
    Multiboot flag and GGID no longer set by MB_Init, and instead set by WM_SetGameInfo when sending beacon.    
    
    Revision 1.12  08/19/2004 16:00:53  miya
    modified WM_SetGameInfo function
    
    Revision 1.11  08/19/2004 14:32:12  yosiokat
    - Turned OFF __UGI_IN_GGID.
    - Changed the fixed elements of the MbBeacon structure flagmentNo, flagmentMaxNum and size to u8 designations.
    - Added the conversion of the line feed codes 0x0d and 0x0a to MBi_ExUTF16_LEtoSJIS_BE.
    
    Revision 1.10  08/18/2004 23:57:14  yosizaki
    fix warning (p_game on SDK_FINALROM)
    
    Revision 1.9  08/18/2004 13:52:04  yosizaki
    add conversion routine for Unicode<->SJIS.
    
    Revision 1.8  08/18/2004 09:54:23  miya

    
    Revision 1.7  08/18/2004 09:30:47  yosiokat
    - Changed the alignment of bsendBuff to 32 bytes.
    
    Revision 1.6  08/18/2004 09:08:32  miya
    modified WMGameInfo format
    
    Revision 1.5  08/12/2004 09:54:50  yosiokat
    Made changes so that the obtaining of the MBi_MakeGameInfo icon data is read out from the file.
    
    Revision 1.4  08/12/2004 09:17:53  yosiokat
    - Made changes so that a time limit is set in the scan lock
    - Added MB_DeleteRecvGameInfo
    
    Revision 1.3  08/09/2004 14:56:43  yosizaki
    format all the symbol for API
    
    Revision 1.2  08/07/2004 15:02:37  yosizaki
    merge all the diff. (-2004/08/07)
    
    Revision 1.1  08/07/2004 07:14:51  yosizaki
    (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#include <nitro.h>
#include "mb_gameinfo.h"
#include "mb_common.h"
#include "mb_private.h"                // for MB_SCAN_TIME_NORMAL & MB_SCAN_TIME_LOKING & MB_OUTPUT, MB_DEBUG_OUTPUT


/*

  ********** CAUTIONS **********
 - During scan lock, the scan time should be set above the beacon interval of the Parent Device.     (200ms or greater)
    However, if this is done, it will mean that the scan time will change dynamically, so what will happen to
    the maximum value of LifetimeCount and LockTimeCount and the values already counted?

*/


/*
 * The wmDEMOlib function that is being used:
 *  - none
 *
 * The WM API that is being used:
 *  - WM_SetGameInfo (MBi_SendFixedBeacon, MBi_SendVolatBeacon)
 *
 *
 *
 *
 */

// define data---------------------------------------------

#define MB_GAME_INFO_LIFETIME_SEC       60      // Lifetime of the received game information (it is deleted if a beacon is not received in this time period (seconds))
#define MB_SCAN_LOCK_SEC                8       // Maximum time for locking a specific parent and scanning
#define MB_SAME_BEACON_RECV_MAX_COUNT   3       // The maximum number of times the parent's beacon can be received when the parent repeatedly sends out the same beacon.

#define MB_BEACON_DATA_SIZE         (WM_SIZE_USER_GAMEINFO - 8)
#define MB_BEACON_FIXED_DATA_SIZE   (MB_BEACON_DATA_SIZE   - 6) // the data size of the Parent Device game information fixed region in the beacon
#define MB_BEACON_VOLAT_DATA_SIZE   (MB_BEACON_DATA_SIZE   - 8) // the data size of the Parent Device game information volatile region in the beacon    
#define MB_SEND_MEMBER_MAX_NUM      (MB_BEACON_VOLAT_DATA_SIZE / sizeof(MBUserInfo))    // the maximum amount of member information that can be send at one time on the beacon

#define FIXED_NORMAL_SIZE            sizeof(MBGameInfoFixed)    // the normal size of the game information fixed region
#define FIXED_NO_ICON_SIZE          (sizeof(MBGameInfoFixed) - sizeof(MBIconInfo))      // the size of the game information fixed region without the icon
#define FIXED_FLAGMENT_MAX(size)    ( ( size / MB_BEACON_FIXED_DATA_SIZE) + ( size % MB_BEACON_FIXED_DATA_SIZE ? 1 : 0 ) )
                                                                                    // the number of beacon subdivisions of the game information fixed region

#define MB_LIFETIME_MAX_COUNT       ( MB_GAME_INFO_LIFETIME_SEC * 1000 / MB_SCAN_TIME_NORMAL + 1 )
#define MB_LOCKTIME_MAX_COUNT       ( MB_SCAN_LOCK_SEC          * 1000 / MB_SCAN_TIME_NORMAL + 1 )
                                                                                    // what converted the Parent Device game information existence period into ScanTime units    
                                                                                    // what converted the scan lock time into ScanTime units    
#define MB_SCAN_COUNT_UNIT_NORMAL   ( 1 )
#define MB_SCAN_COUNT_UNIT_LOCKING  ( MB_SCAN_TIME_LOCKING / MB_SCAN_TIME_NORMAL )



typedef enum MbBeaconState
{
    MB_BEACON_STATE_STOP = 0,
    MB_BEACON_STATE_READY,
    MB_BEACON_STATE_FIXED_START,
    MB_BEACON_STATE_FIXED,
    MB_BEACON_STATE_VOLAT_START,
    MB_BEACON_STATE_VOLAT,
    MB_BEACON_STATE_NEXT_GAME
}
MbBeaconState;


/* Beacon Format Structure*/
typedef struct MbBeacon
{
    u32     ggid;                      // GGID
    u8      dataAttr:2;                // the data attributes (designated by MbBeaconDataAttr)
    u8      fileNo:6;                  // File No.
    u8      seqNoFixed;                // Fixed data sequence number (will be added if there was an update in the data contents)
    u8      seqNoVolat;                // Volat data sequence number (will be added if there was an update in the data contents)
    u8      beaconNo;                  // Beacon number (increments for each beacon transmission)
    /* Original MbBeaconData*/
    union
    {
        struct
        {                              // when sending MbGameInfoFixed
            u16     sum;               // 16bit checksum
            u8      flagmentNo;        // the current number of the fragmented data
            u8      flagmentMaxNum;    // the maximum amount of fragmented data
            u8      size;              // data size
            u8      rsv;
            u8      data[MB_BEACON_FIXED_DATA_SIZE];    // the data entity
        }
        fixed;
        struct
        {                              // when sending MbGameInfoVolatile
            u16     sum;               // 16bit checksum
            u8      nowPlayerNum;      // Current number of players
            u8      pad[1];
            u16     nowPlayerFlag;     // express the player numbers of all current players as a bit    
            u16     changePlayerFlag;  // The bit expressing the number of the player information that got changed in this time's update. (Used for judgment only at the instant when the sequence number has changed.)
            MBUserInfo member[MB_SEND_MEMBER_MAX_NUM];  // send each MB_SEND_MEMBER_MAX_NUM of the user information of each Child Device (terminate if the PlayerNo == 15( Parent Device ))
            u8      userVolatData[MB_USER_VOLAT_DATA_SIZE];     // The data that can be set by the user
        }
        volat;
    }
    data;

}
MbBeacon;

/* The parent-side structure for the beacon-transmission status*/
typedef struct MbBeaconSendStatus
{
    MBGameInfo *gameInfoListTop;       // the pointer to the top of the game information list (a one-way list )
    MBGameInfo *nowGameInfop;          // the pointer to the game information that is currently being sent
    u8     *srcp;                      // the pointer to the source of the game information that is currently being sent
    u8      state;                     // The beacon transmission state (Transmission of fixed data part, dynamic status part. The state changes once all of each kind of data are sent)
    u8      seqNoFixed;                // Fixed region's sequence number
    u8      seqNoVolat;                // Volatile region's sequence number
    u8      flagmentNo;                // the fragment number of the sequence number of the volatile region (in the case of a fixed region)    
    u8      flagmentMaxNum;            // the number of fragments of the sequence number of the volatile region (in the case of a fixed region)    
    u8      beaconNo;
    u8      pad[2];
}
MbBeaconSendStatus;



// function's prototype------------------------------------
static BOOL MBi_ReadIconInfo(const char *filePathp, MBIconInfo *iconp, BOOL char_flag);
static void MBi_ClearSendStatus(void);
static BOOL MBi_ReadyBeaconSendStatus(void);
static void MBi_InitSendFixedBeacon(void);
static void MBi_SendFixedBeacon(u32 ggid, u16 tgid, u8 attribute);
static void MBi_InitSendVolatBeacon(void);
static void MBi_SendVolatBeacon(u32 ggid, u16 tgid, u8 attribute);

static BOOL MBi_CheckMBParent(WMBssDesc *bssDescp);
static void MBi_SetSSIDToBssDesc(WMBssDesc *bssDescp, u32 ggid);
static int MBi_GetStoreElement(WMBssDesc *bssDescp, MBBeaconMsgCallback Callbackp);
static void MBi_CheckCompleteGameInfoFlagments(int index, MBBeaconMsgCallback Callbackp);
static void MBi_AnalyzeBeacon(WMBssDesc *bssDescp, int index, u16 linkLevel);
static void MBi_CheckTGID(WMBssDesc *bssDescp, int inex);
static void MBi_CheckSeqNoFixed(int index);
static void MBi_CheckSeqNoVolat(int index);
static void MBi_InvalidateGameInfoBssID(u8 *bssidp);
static void MBi_RecvFixedBeacon(int index);
static void MBi_RecvVolatBeacon(int index);

static void MBi_LockScanTarget(int index);
static void MBi_UnlockScanTarget(void);
static int mystrlen(u16 *str);


// const data----------------------------------------------

// global variables----------------------------------------

static MbBeaconSendStatus mbss;        // beacon send status

static MbBeaconRecvStatus mbrs;        // beacon receive status
static MbBeaconRecvStatus *mbrsp = NULL;        // beacon receive status

// static variables----------------------------------------
static MbScanLockFunc sLockFunc = NULL; // Pointer to the function for setting scan lock
static MbScanUnlockFunc sUnlockFunc = NULL;     // Pointer to the function for releasing scan lock

static MbBeacon bsendBuff ATTRIBUTE_ALIGN(32);  // beacon send buffer
static MbBeacon *brecvBuffp;           // beacon receive buffer
static WMBssDesc bssDescbuf ATTRIBUTE_ALIGN(32);        // the BssDesc temporary buffer

static MBSendVolatCallbackFunc sSendVolatCallback = NULL;       // User data transmission callback
static u32 sSendVolatCallbackTimming;  // Dummy that generates transmission callback

// function's description-----------------------------------------------


//=========================================================
// Beacon receive work buffer operation
//=========================================================
// Obtains the work buffer that is set
const MbBeaconRecvStatus *MB_GetBeaconRecvStatus(void)
{
    return mbrsp;
}

// Sets the work buffer from the static variable (for compatibility with the older version)
void MBi_SetBeaconRecvStatusBufferDefault(void)
{
    mbrsp = &mbrs;
}

// Sets the beacon receive work buffer
void MBi_SetBeaconRecvStatusBuffer(MbBeaconRecvStatus * buf)
{
    mbrsp = buf;
}


// Sets the function for scan lock
void MBi_SetScanLockFunc(MbScanLockFunc lock, MbScanUnlockFunc unlock)
{
    sLockFunc = lock;
    sUnlockFunc = unlock;
}



//=========================================================
// send the game information from the Parent Device by beacon    
//=========================================================

// generate the Parent Device game information to send by beacon
void MBi_MakeGameInfo(MBGameInfo *gameInfop,
                      const MBGameRegistry *mbGameRegp, const MBUserInfo *parent)
{
    BOOL    icon_disable;

    // clear all for the time being
    MI_CpuClear16(gameInfop, sizeof(MBGameInfo));

    // register the icon data
    gameInfop->dataAttr = MB_BEACON_DATA_ATTR_FIXED_NORMAL;
    icon_disable = !MBi_ReadIconInfo(mbGameRegp->iconCharPathp, &gameInfop->fixed.icon, TRUE);
    icon_disable |= !MBi_ReadIconInfo(mbGameRegp->iconPalettePathp, &gameInfop->fixed.icon, FALSE);

    if (icon_disable)
    {
        gameInfop->dataAttr = MB_BEACON_DATA_ATTR_FIXED_NO_ICON;
        MI_CpuClearFast(&gameInfop->fixed.icon, sizeof(MBIconInfo));
    }

    // GGID
    gameInfop->ggid = mbGameRegp->ggid;

    // register the user information for the parent
    if (parent != NULL)
    {
        MI_CpuCopy16(parent, &gameInfop->fixed.parent, sizeof(MBUserInfo));
    }

    // register the maximum number of players
    gameInfop->fixed.maxPlayerNum = mbGameRegp->maxPlayerNum;

    // registration of the game name and the description of the game contents
    {
        int     len;

#if defined(MB_CHANGE_TO_UNICODE)
#define COPY_GAME_INFO_STRING   MBi_ExSJIS_BEtoUTF16_LE
#else  /* defined(MB_CHANGE_TO_UNICODE)*/
#define COPY_GAME_INFO_STRING   MI_CpuCopy16
#endif /* defined(MB_CHANGE_TO_UNICODE)*/

        len = mystrlen(mbGameRegp->gameNamep) << 1;
        COPY_GAME_INFO_STRING((u8 *)mbGameRegp->gameNamep, gameInfop->fixed.gameName, (u16)len);
        // Data after the NUL character is also sent in order to pad the data after the game content explanation.
        len = MB_GAME_INTRO_LENGTH * 2;
        COPY_GAME_INFO_STRING((u8 *)mbGameRegp->gameIntroductionp,
                              gameInfop->fixed.gameIntroduction, (u16)len);
    }

    // register the Parent Device portion of the player information
    gameInfop->volat.nowPlayerNum = 1;
    gameInfop->volat.nowPlayerFlag = 0x0001;    // the player number of the Parent Device is 0
    gameInfop->broadcastedPlayerFlag = 0x0001;

    // NOTE: there is no registration, since there are no play members at multiboot startup
}


// read icon data
static BOOL MBi_ReadIconInfo(const char *filePathp, MBIconInfo *iconp, BOOL char_flag)
{
    FSFile  file;
    s32     size = char_flag ? MB_ICON_DATA_SIZE : MB_ICON_PALETTE_SIZE;
    u16    *dstp = char_flag ? iconp->data : iconp->palette;

    if (filePathp == NULL)
    {                                  // returns FALSE if there is no file designation.
        return FALSE;
    }

    FS_InitFile(&file);

    if (FS_OpenFile(&file, filePathp) == FALSE)
    {                                  // returns FALSE if a file could not be opened
        MB_DEBUG_OUTPUT("\t%s : file open error.\n", filePathp);
        return FALSE;
    }
    else if ((u32)size != FS_GetLength(&file))
    {
        MB_DEBUG_OUTPUT("\t%s : different file size.\n", filePathp);
        (void)FS_CloseFile(&file);
        return FALSE;
    }

    (void)FS_ReadFile(&file, dstp, size);
    (void)FS_CloseFile(&file);
    return TRUE;
}


// update the volatile portion of the game information    
void MB_UpdateGameInfoMember(MBGameInfo *gameInfop,
                             const MBUserInfo *member, u16 nowPlayerFlag, u16 changePlayerFlag)
{
    int     i;
    u8      playerNum = 1;

    MI_CpuCopy16(member, &gameInfop->volat.member[0], sizeof(MBUserInfo) * MB_MEMBER_MAX_NUM);
    /* Count the number of child devices*/
    for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
    {
        if (nowPlayerFlag & (0x0002 << i))
        {
            playerNum++;
        }
    }
    gameInfop->volat.nowPlayerNum = playerNum;
    gameInfop->volat.nowPlayerFlag = (u16)(nowPlayerFlag | 0x0001);
    gameInfop->volat.changePlayerFlag = changePlayerFlag;
    gameInfop->seqNoVolat++;
}


// calculate the length of the character string
static int mystrlen(u16 *str)
{
    int     len = 0;
    while (*str++)
        len++;
    return len;
}


// add the game information to the send list
void MB_AddGameInfo(MBGameInfo *newGameInfop)
{
    MBGameInfo *gInfop = mbss.gameInfoListTop;

    if (!gInfop)
    {                                  // if the top of mbss is NULL, register at the top
        mbss.gameInfoListTop = newGameInfop;
    }
    else
    {                                  // either that, or follow the list and add it to the end
        while (gInfop->nextp != NULL)
        {
            gInfop = gInfop->nextp;
        }
        gInfop->nextp = newGameInfop;
    }
    newGameInfop->nextp = NULL;        // the next of the added game information is the termination point
}


// delete the game information from the send list
BOOL MB_DeleteGameInfo(MBGameInfo *gameInfop)
{
    MBGameInfo *gInfop = mbss.gameInfoListTop;
    MBGameInfo *before;

    while (gInfop != NULL)
    {
        if (gInfop != gameInfop)
        {
            before = gInfop;
            gInfop = gInfop->nextp;
            continue;
        }

        // It matched, so it is deleted from the list.
        if ((u32)gInfop == (u32)mbss.gameInfoListTop)
        {                              // when deleting the top of the list
            mbss.gameInfoListTop = mbss.gameInfoListTop->nextp;
        }
        else
        {
            before->nextp = gInfop->nextp;
        }

        if ((u32)gameInfop == (u32)mbss.nowGameInfop)   // If the game information being deleted is currently being transmitted
        {                              // Reset the game information from the rebuilt list.
            mbss.nowGameInfop = NULL;
            if (!MBi_ReadyBeaconSendStatus())
            {
                mbss.state = MB_BEACON_STATE_READY;     // If the game information has been completely deleted, the status shifts to READY.
            }
        }
        return TRUE;
    }

    // When there is no game information
    return FALSE;
}


// initialization of the game information send configuration
void MB_InitSendGameInfoStatus(void)
{
    mbss.gameInfoListTop = NULL;       // delete the entire game information list
    mbss.nowGameInfop = NULL;          // also delete the current send game
    mbss.state = MB_BEACON_STATE_READY;
    sSendVolatCallback = NULL;
    MBi_ClearSendStatus();             // also clear the send status
}


// clear the send status
static void MBi_ClearSendStatus(void)
{
    mbss.seqNoFixed = 0;
    mbss.seqNoVolat = 0;
    mbss.flagmentNo = 0;
    mbss.flagmentMaxNum = 0;
    mbss.beaconNo = 0;
}


// beacon send
void MB_SendGameInfoBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    while (1)
    {
        switch (mbss.state)
        {
        case MB_BEACON_STATE_STOP:
        case MB_BEACON_STATE_READY:
            if (!MBi_ReadyBeaconSendStatus())
            {                          // game information send preparations
                return;
            }
            break;
        case MB_BEACON_STATE_FIXED_START:
            MBi_InitSendFixedBeacon();
            break;
        case MB_BEACON_STATE_FIXED:
            MBi_SendFixedBeacon(ggid, tgid, attribute);
            return;
        case MB_BEACON_STATE_VOLAT_START:
            MBi_InitSendVolatBeacon();
            break;
        case MB_BEACON_STATE_VOLAT:
            MBi_SendVolatBeacon(ggid, tgid, attribute);
            return;
        case MB_BEACON_STATE_NEXT_GAME:
            break;
        }
    }
}


// prepare the beacon send status so it is sendable
static BOOL MBi_ReadyBeaconSendStatus(void)
{
    MBGameInfo *nextGameInfop;

    // return an error if the game information has not been registered
    if (!mbss.gameInfoListTop)
    {
        /* The MB flag and ENTRY flag are dropped here.*/
        (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, MBi_GetGgid(),
                             MBi_GetTgid(), WM_ATTR_FLAG_CS);
        return FALSE;
    }

    // selection of the GameInfo to send next
    if (!mbss.nowGameInfop || !mbss.nowGameInfop->nextp)
    {
        // if there is still no game information being sent, or if at the end of the list, make preparations to send the top of the list
        nextGameInfop = mbss.gameInfoListTop;
    }
    else
    {
        // either that, or make preparations to send the next game in the list
        nextGameInfop = mbss.nowGameInfop->nextp;
    }

    mbss.nowGameInfop = nextGameInfop;

    MBi_ClearSendStatus();
    mbss.seqNoVolat = mbss.nowGameInfop->seqNoVolat;

    mbss.state = MB_BEACON_STATE_FIXED_START;

    return TRUE;
}


// send initialization for the fixed data portion of the game information
static void MBi_InitSendFixedBeacon(void)
{
    if (mbss.state != MB_BEACON_STATE_FIXED_START)
    {
        return;
    }

    if (mbss.nowGameInfop->dataAttr == MB_BEACON_DATA_ATTR_FIXED_NORMAL)
    {
        mbss.flagmentMaxNum = FIXED_FLAGMENT_MAX(FIXED_NORMAL_SIZE);
        mbss.srcp = (u8 *)&mbss.nowGameInfop->fixed;
    }
    else
    {
        mbss.flagmentMaxNum = FIXED_FLAGMENT_MAX(FIXED_NO_ICON_SIZE);
        mbss.srcp = (u8 *)&mbss.nowGameInfop->fixed.parent;
    }
    mbss.state = MB_BEACON_STATE_FIXED;
}


// split up the fixed data portion of the game information and send it by beacon
static void MBi_SendFixedBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    u32     lastAddr = (u32)mbss.nowGameInfop + sizeof(MBGameInfoFixed);

    if ((u32)mbss.srcp + MB_BEACON_FIXED_DATA_SIZE <= lastAddr)
    {
        bsendBuff.data.fixed.size = MB_BEACON_FIXED_DATA_SIZE;
    }
    else
    {
        bsendBuff.data.fixed.size = (u8)(lastAddr - (u32)mbss.srcp);
        MI_CpuClear16((void *)((u8 *)bsendBuff.data.fixed.data + bsendBuff.data.fixed.size),
                      (u32)(MB_BEACON_FIXED_DATA_SIZE - bsendBuff.data.fixed.size));
    }

    // set to the beacon send buffer
    MB_DEBUG_OUTPUT("send flagment= %2d  adr = 0x%x\n", mbss.flagmentNo, mbss.srcp);
    MI_CpuCopy16(mbss.srcp, bsendBuff.data.fixed.data, bsendBuff.data.fixed.size);
    bsendBuff.data.fixed.flagmentNo = mbss.flagmentNo;
    bsendBuff.data.fixed.flagmentMaxNum = mbss.flagmentMaxNum;
    bsendBuff.dataAttr = mbss.nowGameInfop->dataAttr;
    bsendBuff.seqNoFixed = mbss.nowGameInfop->seqNoFixed;
    bsendBuff.seqNoVolat = mbss.seqNoVolat;
    bsendBuff.ggid = mbss.nowGameInfop->ggid;
    bsendBuff.fileNo = mbss.nowGameInfop->fileNo;
    bsendBuff.beaconNo = mbss.beaconNo++;
    bsendBuff.data.fixed.sum = 0;
    bsendBuff.data.fixed.sum = MBi_calc_cksum((u16 *)&bsendBuff.data, MB_BEACON_DATA_SIZE);

    // update the send status
    mbss.flagmentNo++;
    if (mbss.flagmentNo < mbss.flagmentMaxNum)
    {
        mbss.srcp += MB_BEACON_FIXED_DATA_SIZE;
    }
    else
    {
        mbss.state = MB_BEACON_STATE_VOLAT_START;
    }

    /* Register the GGID of the parent program itself*/
    (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, ggid, tgid,
                         (u8)(attribute | WM_ATTR_FLAG_MB | WM_ATTR_FLAG_ENTRY));
    // the multiboot flag is activated here
}


// send initialization for the volatile data portion of the game information    
static void MBi_InitSendVolatBeacon(void)
{
    mbss.nowGameInfop->broadcastedPlayerFlag = 0x0001;
    mbss.seqNoVolat = mbss.nowGameInfop->seqNoVolat;
    mbss.state = MB_BEACON_STATE_VOLAT;
}


// split up the volatile data portion of the game information and send it by beacon    
static void MBi_SendVolatBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    int     i;
    int     setPlayerNum;
    u16     remainPlayerFlag;

    if (mbss.seqNoVolat != mbss.nowGameInfop->seqNoVolat)
    {                                  // resend when data updates during a send
        MBi_InitSendVolatBeacon();
    }

    // set the beacon information
    bsendBuff.dataAttr = MB_BEACON_DATA_ATTR_VOLAT;
    bsendBuff.seqNoFixed = mbss.nowGameInfop->seqNoFixed;
    bsendBuff.seqNoVolat = mbss.seqNoVolat;
    bsendBuff.ggid = mbss.nowGameInfop->ggid;
    bsendBuff.fileNo = mbss.nowGameInfop->fileNo;
    bsendBuff.beaconNo = mbss.beaconNo++;

    // set the current player information state
    bsendBuff.data.volat.nowPlayerNum = mbss.nowGameInfop->volat.nowPlayerNum;
    bsendBuff.data.volat.nowPlayerFlag = mbss.nowGameInfop->volat.nowPlayerFlag;
    bsendBuff.data.volat.changePlayerFlag = mbss.nowGameInfop->volat.changePlayerFlag;

    // Set the application settings data
    if (sSendVolatCallbackTimming == MB_SEND_VOLAT_CALLBACK_TIMMING_BEFORE
        && sSendVolatCallback != NULL)
    {
        sSendVolatCallback(mbss.nowGameInfop->ggid);
    }

    for (i = 0; i < MB_USER_VOLAT_DATA_SIZE; i++)
    {
        bsendBuff.data.volat.userVolatData[i] = mbss.nowGameInfop->volat.userVolatData[i];
    }

    MB_DEBUG_OUTPUT("send PlayerFlag = %x\n", mbss.nowGameInfop->volat.nowPlayerFlag);

    // set the player information to send this time
    MI_CpuClear16(&bsendBuff.data.volat.member[0], sizeof(MBUserInfo) * MB_SEND_MEMBER_MAX_NUM);
    setPlayerNum = 0;
    remainPlayerFlag =
        (u16)(mbss.nowGameInfop->broadcastedPlayerFlag ^ mbss.nowGameInfop->volat.nowPlayerFlag);
    for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
    {
        if ((remainPlayerFlag & (0x0002 << i)) == 0)
        {
            continue;
        }

        MB_DEBUG_OUTPUT("  member %d set.\n", i);

        MI_CpuCopy16(&mbss.nowGameInfop->volat.member[i],
                     &bsendBuff.data.volat.member[setPlayerNum], sizeof(MBUserInfo));
        mbss.nowGameInfop->broadcastedPlayerFlag |= 0x0002 << i;
        if (++setPlayerNum == MB_SEND_MEMBER_MAX_NUM)
        {
            break;
        }
    }
    if (setPlayerNum < MB_SEND_MEMBER_MAX_NUM)
    {                                  // terminate when the maximum number of sends was not reached
        bsendBuff.data.volat.member[setPlayerNum].playerNo = 0;
    }

    // set the checksum
    bsendBuff.data.volat.sum = 0;
    bsendBuff.data.volat.sum = MBi_calc_cksum((u16 *)&bsendBuff.data, MB_BEACON_DATA_SIZE);


    // check for send end
    if (mbss.nowGameInfop->broadcastedPlayerFlag == mbss.nowGameInfop->volat.nowPlayerFlag)
    {
        mbss.state = MB_BEACON_STATE_READY;     // once all information has finished sending, prepare a state for the next game information to be sent     
    }

    /* Register the GGID of the parent program itself*/

    (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, ggid, tgid,
                         (u8)(attribute | WM_ATTR_FLAG_MB | WM_ATTR_FLAG_ENTRY));
    // the multiboot flag is activated here

    if (sSendVolatCallbackTimming == MB_SEND_VOLAT_CALLBACK_TIMMING_AFTER
        && sSendVolatCallback != NULL)
    {
        sSendVolatCallback(mbss.nowGameInfop->ggid);
    }

}


//=========================================================
// reception of the game information beacon from the Child Device    
//=========================================================

// initialization of the game information receive status
void MB_InitRecvGameInfoStatus(void)
{
    MI_CpuClearFast(mbrsp, sizeof(MbBeaconRecvStatus));

    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_NORMAL;
}


// beacon receive
BOOL MB_RecvGameInfoBeacon(MBBeaconMsgCallback Callbackp, u16 linkLevel, WMBssDesc *bssDescp)
{
    int     index;

    // the beacon obtained this time determines whether or not this is a multiboot Parent Device
    if (!MBi_CheckMBParent(bssDescp))
    {
        return FALSE;
    }

    /* Copy the obtained bssDesc to a temporary buffer*/
    MI_CpuCopy16(bssDescp, &bssDescbuf, sizeof(WMBssDesc));

    /* Swap bssDescp in local buffer */
    bssDescp = &bssDescbuf;

    brecvBuffp = (MbBeacon *) bssDescp->gameInfo.userGameInfo;

    // Confirm checksum.
    if (MBi_calc_cksum((u16 *)&brecvBuffp->data, MB_BEACON_DATA_SIZE))
    {
        MB_DEBUG_OUTPUT("Beacon checksum error!\n");
        return FALSE;
    }

    // Determines the location for storing the parent's game information (if data has already been received from that parent, the same location is used)
    index = MBi_GetStoreElement(bssDescp, Callbackp);
    if (index < 0)
    {
        return FALSE;                  // Returns an error because there is no storage location
    }
    MB_DEBUG_OUTPUT("GameInfo Index:%6d\n", index);

    // Beacon analysis
    MBi_AnalyzeBeacon(bssDescp, index, linkLevel);

    // Determines whether all the parent's game data fragments are gathered together and makes a notification with a callback
    MBi_CheckCompleteGameInfoFlagments(index, Callbackp);

    return TRUE;
}


// Determines whether the beacon fetched this time is from a multi-boot parent    
static BOOL MBi_CheckMBParent(WMBssDesc *bssDescp)
{
    // Determines whether it is a multi-boot parent
    if ((bssDescp->gameInfo.magicNumber != WM_GAMEINFO_MAGIC_NUMBER)
        || !(bssDescp->gameInfo.attribute & WM_ATTR_FLAG_MB)
        )
    {
        MB_DEBUG_OUTPUT("not MB parent : %x%x\n",
                        *(u16 *)(&bssDescp->bssid[4]), *(u32 *)bssDescp->bssid);
        return FALSE;
    }
    else
    {

        MB_DEBUG_OUTPUT("MB parent     : %x%x",
                        *(u16 *)(&bssDescp->bssid[4]), *(u32 *)bssDescp->bssid);
        return TRUE;
    }
}


// Sets bssDesc to SSID.
static void MBi_SetSSIDToBssDesc(WMBssDesc *bssDescp, u32 ggid)
{
    /* 
       SSID settings

       SSID is generated from
       the download application specific GGID and the multi-boot parent TGID.
       The child reconnects with the parent application using this SSID.
     */
    bssDescp->ssidLength = 32;
    ((u16 *)bssDescp->ssid)[0] = (u16)(ggid & 0x0000ffff);
    ((u16 *)bssDescp->ssid)[1] = (u16)((ggid & 0xffff0000) >> 16);
    ((u16 *)bssDescp->ssid)[2] = bssDescp->gameInfo.tgid;
}


// Fetches where to store the receiving list elements based on bssDesc.    
static int MBi_GetStoreElement(WMBssDesc *bssDescp, MBBeaconMsgCallback Callbackp)
{
    int     i;

    // Determines whether the same game information has already been received from that parent (if "GGID", "BSSID" and "fileNo" all match, the determination is made that the game information is the same)
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        MBGameInfoRecvList *info = &mbrsp->list[i];

        if ((mbrsp->usingGameInfoFlag & (0x01 << i)) == 0)
        {
            continue;
        }
        // Does GGID match?
        if (info->gameInfo.ggid != brecvBuffp->ggid)
        {
            continue;
        }
        // Does the MAC address match?
        if (!WM_IsBssidEqual(info->bssDesc.bssid, bssDescp->bssid))
        {
            continue;
        }
        // Does the file number match?
        if (mbrsp->list[i].gameInfo.fileNo != brecvBuffp->fileNo)
        {
            continue;
        }

        // =========================================
        // Determines whether the receiving location for this parent information has already been allocated.      
        // =========================================
        if (!(mbrsp->validGameInfoFlag & (0x01 << i)))
        {
            MBi_LockScanTarget(i);     // If the corresponding parent information is not lined up, enable scan lock.    
        }
        return i;
    }

    // It has not yet been received, so the list's NULL position is found and made the storage location.    
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        if (mbrsp->usingGameInfoFlag & (0x01 << i))
        {
            continue;
        }

        MI_CpuCopy16(bssDescp, &mbrsp->list[i].bssDesc, sizeof(WMBssDesc));
        // Copies BssDesc
        mbrsp->list[i].gameInfo.seqNoFixed = brecvBuffp->seqNoFixed;
        mbrsp->usingGameInfoFlag |= (u16)(0x01 << i);

        MB_DEBUG_OUTPUT("\n");
        // Only this parent is locked as a Scan target.    
        MBi_LockScanTarget(i);
        return i;
    }

    // If all the storage locations are full, an error is returned with a callback notification
    if (Callbackp != NULL)
    {
        Callbackp(MB_BC_MSG_GINFO_LIST_FULL, NULL, 0);
    }
    return -1;
}


// Determines whether all the parent's game data fragments are gathered together and makes a notification with a callback
static void MBi_CheckCompleteGameInfoFlagments(int index, MBBeaconMsgCallback Callbackp)
{
    MBGameInfoRecvList *info = &mbrsp->list[index];

    /* Acknowledges parent discovery to the application in any case*/
    if (Callbackp != NULL)
    {
        Callbackp(MB_BC_MSG_GINFO_BEACON, info, index);
    }
    if ((info->getFlagmentFlag == info->allFlagmentFlag) && (info->getFlagmentFlag)     // If the new parent's game information is complete
        && (info->gameInfo.volat.nowPlayerFlag)
        && (info->getPlayerFlag == info->gameInfo.volat.nowPlayerFlag))
    {
        if (mbrsp->validGameInfoFlag & (0x01 << index))
        {
            return;
        }
        mbrsp->validGameInfoFlag |= 0x01 << index;
        mbrsp->usefulGameInfoFlag |= 0x01 << index;
        MBi_UnlockScanTarget();        // Releases the lock on the scan target
        MB_DEBUG_OUTPUT("validated ParentInfo = %d\n", index);
        if (Callbackp != NULL)
        {
            Callbackp(MB_BC_MSG_GINFO_VALIDATED, info, index);
        }
    }
    else
    {                                  // If the parent game information already fetched is updated and is incomplete    
        if ((mbrsp->validGameInfoFlag & (0x01 << index)) == 0)
        {
            return;
        }
        mbrsp->validGameInfoFlag ^= (0x01 << index);
        MB_DEBUG_OUTPUT("Invaldated ParentInfo = %d\n", index);
        if (Callbackp != NULL)
        {
            Callbackp(MB_BC_MSG_GINFO_INVALIDATED, info, index);
        }
    }
}


// The parent's game information lifetime count (Also does a scan lock time count)
void MB_CountGameInfoLifetime(MBBeaconMsgCallback Callbackp, BOOL found_parent)
{
    int     i;
    BOOL    unlock = FALSE;

    // Determines the lifetime of the game information receiving list & confirms the presence of a new lock target
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        MBGameInfoRecvList *info = &mbrsp->list[i];
        u16     mask = (u16)(0x0001 << i);
        if ((mbrsp->usingGameInfoFlag & mask) == 0)
        {
            continue;
        }
        // Lifetime determination
        info->lifetimeCount -= mbrsp->scanCountUnit;
        if (info->lifetimeCount >= 0)
        {
            continue;
        }
        info->lifetimeCount = 0;
        if (mbrsp->validGameInfoFlag & mask)
        {
            if (Callbackp != NULL)
            {
                Callbackp(MB_BC_MSG_GINFO_LOST, NULL, i);
            }
        }
        if (mbrsp->nowScanTargetFlag & mask)
        {
            unlock = TRUE;
        }
        mbrsp->usingGameInfoFlag &= ~mask;
        MB_DeleteRecvGameInfo(i);      // Deletes game information after callback notification.
        MB_DEBUG_OUTPUT("gameInfo %2d : lifetime end.\n", i);
    }

    // Scan lock time count
    if (mbrsp->nowScanTargetFlag && mbrsp->nowLockTimeCount > 0)
    {
        mbrsp->nowLockTimeCount -= mbrsp->scanCountUnit;        // Releases the lock and searches for the next lock target if the scan lock time is over.    
        if (mbrsp->nowLockTimeCount < 0)
        {
            MB_DEBUG_OUTPUT("scan lock time up!\n");
            unlock = TRUE;
        }
        else if (!found_parent)
        {
            if (++mbrsp->notFoundLockTargetCount > 4)
            {
                MB_DEBUG_OUTPUT("scan lock target not found!\n");
                unlock = TRUE;
            }
        }
        else
        {
            mbrsp->notFoundLockTargetCount = 0;
        }
    }

    // Scan unlock processing.
    if (unlock)
    {
        mbrsp->nowLockTimeCount = 0;
        MBi_UnlockScanTarget();
    }
}


// Beacon analysis
static void MBi_AnalyzeBeacon(WMBssDesc *bssDescp, int index, u16 linkLevel)
{
    MBi_CheckTGID(bssDescp, index);    // Checks the TGID
    MBi_CheckSeqNoFixed(index);        // Checks seqNoFixed
    MBi_CheckSeqNoVolat(index);        // Checks seqNoVolat

    // Fetches data for the shared portion of the receiving beacon    
    {
        MBGameInfoRecvList *info = &mbrsp->list[index];

        // Excludes parents that are malfunctioning and repeatedly sending out the same beacon.
        if (info->beaconNo == brecvBuffp->beaconNo)
        {
            if (++info->sameBeaconRecvCount > MB_SAME_BEACON_RECV_MAX_COUNT)
            {
                info->lifetimeCount = 0;        // Sets the parent information lifetime to zero so it gets deleted.
                MB_OUTPUT("The parent broadcast same beacon.: %d\n", index);
                MBi_InvalidateGameInfoBssID(&info->bssDesc.bssid[0]);
                return;                // If the parent has distributed this game information elsewhere, that is deleted as well.
            }
        }
        else
        {
            info->sameBeaconRecvCount = 0;
        }
        // Data receiving for normal parents
        info->beaconNo = brecvBuffp->beaconNo;
        info->lifetimeCount = MB_LIFETIME_MAX_COUNT;    // Extends the lifetime of the parent's information.
        info->gameInfo.ggid = brecvBuffp->ggid; // Gets ggid.
        info->gameInfo.fileNo = brecvBuffp->fileNo;     // Gets fileNo.
        info->linkLevel = linkLevel;   // Gets the signal strength.
        // Sets bssDesc to SSID.
        MBi_SetSSIDToBssDesc(&info->bssDesc, info->gameInfo.ggid);
    }

    // Gets data for each data type in the receiving beacon
    if (brecvBuffp->dataAttr == MB_BEACON_DATA_ATTR_VOLAT)
    {
        MBi_RecvVolatBeacon(index);
    }
    else
    {
        MBi_RecvFixedBeacon(index);
    }
}


// Checks tgid
static void MBi_CheckTGID(WMBssDesc *bssDescp, int index)
{
    if (mbrsp->list[index].bssDesc.gameInfo.tgid == bssDescp->gameInfo.tgid)
    {
        return;
    }

    // If tgid has been increased, the parent is determined to have rebooted. All data is cleared and fetched again.    
    MB_DEBUG_OUTPUT("\ntgid updated! : %x%x", *(u16 *)(&bssDescp->bssid[4]),
                    *(u32 *)bssDescp->bssid);
    MB_DeleteRecvGameInfoWithoutBssdesc(index);
    MI_CpuCopy16(bssDescp, &mbrsp->list[index].bssDesc, sizeof(WMBssDesc));
    // The new bssDesc is copied in the case of a tgid update.
    MBi_LockScanTarget(index);         // Scan locks the appropriate parent.
}


// Checks the SeqNo of the fixed data
static void MBi_CheckSeqNoFixed(int index)
{
    // Handles the situation where the sequence number has been updated
    if (mbrsp->list[index].gameInfo.seqNoFixed == brecvBuffp->seqNoFixed)
    {
        return;
    }
    // Clears the data received thus far if the sequence number has been updated.
    MB_DEBUG_OUTPUT("\n seqNoFixed updated!");
    MB_DeleteRecvGameInfoWithoutBssdesc(index);
    MBi_LockScanTarget(index);         // Scan locks the appropriate parent.
    mbrsp->list[index].gameInfo.seqNoFixed = brecvBuffp->seqNoFixed;
}


// Checks the SeqNo of the volat data
static void MBi_CheckSeqNoVolat(int index)
{
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];

    // Handles the situation where the sequence number has been updated
    if (mbrsp->list[index].gameInfo.seqNoVolat != brecvBuffp->seqNoVolat)
    {
        MB_DEBUG_OUTPUT("\n seqNoVolat updated!");
        MBi_LockScanTarget(index);     // Checks whether a scan lock is possible, then does so.
    }
}


// Invalidates all the parent game information of the target BSSID.
static void MBi_InvalidateGameInfoBssID(u8 *bssidp)
{
    int     i;
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        if ((mbrsp->usingGameInfoFlag & (0x01 << i)) == 0)
        {
            continue;
        }

        if (!WM_IsBssidEqual(bssidp, mbrsp->list[i].bssDesc.bssid))
        {
            continue;
        }

        // Deletion target data and determination.
        mbrsp->list[i].lifetimeCount = 0;       // Sets the parent information lifetime to zero so it gets deleted.
        MB_OUTPUT("The parent broadcast same beacon.: %d\n", i);
    }
}


// Splits up the fixed data portion of the game information and receives the beacon
static void MBi_RecvFixedBeacon(int index)
{
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];
    u32     lastAddr = (u32)&grecvp->gameInfo + sizeof(MBGameInfoFixed);
    u8     *dstp;

    // Does not receive the beacon if it has been fetched, and the sequence number has not been updated.    
    if (grecvp->gameInfo.seqNoFixed == brecvBuffp->seqNoFixed)
    {
        if (grecvp->getFlagmentFlag & (0x01 << brecvBuffp->data.fixed.flagmentNo))
        {
            return;
        }
    }

    // Checks that the receiving beacon does not exceed the receiving buffer.    
    if (brecvBuffp->dataAttr == MB_BEACON_DATA_ATTR_FIXED_NORMAL)
    {
        dstp = (u8 *)&grecvp->gameInfo.fixed;
    }
    else
    {
        dstp = (u8 *)&grecvp->gameInfo.fixed.parent;
    }
    dstp += MB_BEACON_FIXED_DATA_SIZE * brecvBuffp->data.fixed.flagmentNo;
    // Calculates the receiving address for the game information buffer.

    if ((u32)dstp + brecvBuffp->data.fixed.size > lastAddr)
    {
        MB_DEBUG_OUTPUT("recv beacon gInfoFixed Buffer over!\n");
        // Ignores the beacon data that overflows the buffer.
        return;
    }

    // Sets the received beacon to the target game information buffer
    MB_DEBUG_OUTPUT("recv flagment= %2d  adr = 0x%x", brecvBuffp->data.fixed.flagmentNo, dstp);
    MI_CpuCopy16(brecvBuffp->data.fixed.data, dstp, brecvBuffp->data.fixed.size);
    grecvp->gameInfo.dataAttr = brecvBuffp->dataAttr;
    grecvp->getFlagmentFlag |= 0x01 << brecvBuffp->data.fixed.flagmentNo;
    grecvp->allFlagmentFlag = (u32)((0x01 << brecvBuffp->data.fixed.flagmentMaxNum) - 1);
    MB_DEBUG_OUTPUT("\t now flagment = 0x%x \t all flagment = 0x%x\n",
                    grecvp->getFlagmentFlag, grecvp->allFlagmentFlag);
}


// Splits up the fluid data portion of the game information and receives the beacon     
static void MBi_RecvVolatBeacon(int index)
{
    int     i;
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];

    /* Always receive user definition data*/
    for (i = 0; i < MB_USER_VOLAT_DATA_SIZE; i++)
    {
        grecvp->gameInfo.volat.userVolatData[i] = brecvBuffp->data.volat.userVolatData[i];
    }
    MI_CpuCopy16(brecvBuffp, &grecvp->bssDesc.gameInfo.userGameInfo, WM_SIZE_USER_GAMEINFO);

    // Processes when a member information update is detected
    if (grecvp->gameInfo.seqNoVolat != brecvBuffp->seqNoVolat)
    {
        if ((u8)(grecvp->gameInfo.seqNoVolat + 1) == brecvBuffp->seqNoVolat)
        {                              // Inherits member information not updated if the seqNoVolat is off by one
            for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
            {
                if (brecvBuffp->data.volat.changePlayerFlag & (0x02 << i))
                {
                    MI_CpuClear16(&grecvp->gameInfo.volat.member[i], sizeof(MBUserInfo));
                }
            }
            grecvp->getPlayerFlag &= ~brecvBuffp->data.volat.changePlayerFlag;
            mbrsp->validGameInfoFlag &= ~(0x0001 << index);
        }
        else
        {                              // Clears all the member information received thus far if seqNoVolat is off by more
            MI_CpuClear16(&grecvp->gameInfo.volat.member[0],
                          sizeof(MBUserInfo) * MB_MEMBER_MAX_NUM);
            grecvp->getPlayerFlag = 0;
            mbrsp->validGameInfoFlag &= ~(0x0001 << index);
        }
        grecvp->gameInfo.seqNoVolat = brecvBuffp->seqNoVolat;
    }
    else if (grecvp->getPlayerFlag == brecvBuffp->data.volat.nowPlayerFlag)
    {
        return;                        // Does not receive the beacon if it has been fetched, and the sequence number has not been updated.    
    }

    // Reads the player information
    grecvp->gameInfo.volat.nowPlayerNum = brecvBuffp->data.volat.nowPlayerNum;
    grecvp->gameInfo.volat.nowPlayerFlag = brecvBuffp->data.volat.nowPlayerFlag;
    grecvp->gameInfo.volat.changePlayerFlag = brecvBuffp->data.volat.changePlayerFlag;
    grecvp->getPlayerFlag |= 0x0001;

    // Reads the user information for each member
    for (i = 0; i < MB_SEND_MEMBER_MAX_NUM; i++)
    {
        int     playerNo = (int)brecvBuffp->data.volat.member[i].playerNo;
        if (playerNo == 0)
        {
            continue;
        }
        MB_DEBUG_OUTPUT("member %d recv.\n", playerNo);
        MI_CpuCopy16(&brecvBuffp->data.volat.member[i],
                     &grecvp->gameInfo.volat.member[playerNo - 1], sizeof(MBUserInfo));
        grecvp->getPlayerFlag |= 0x01 << playerNo;
    }
}


// Locks the scan target to a single parent
static void MBi_LockScanTarget(int index)
{
    /* Ignore if there is already a locked target*/
    if (mbrsp->nowScanTargetFlag)
    {
        return;
    }

    if (sLockFunc != NULL)
    {
        sLockFunc(mbrsp->list[index].bssDesc.bssid);
    }
    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_LOCKING;

    mbrsp->nowScanTargetFlag = (u16)(0x01 << index);    // Shows the new scan lock target in bits.
    mbrsp->nowLockTimeCount = MB_LOCKTIME_MAX_COUNT;    // Initializes the scan lock time
    MB_DEBUG_OUTPUT("scan target locked. : %x %x %x %x %x %x\n",
                    mbrsp->list[index].bssDesc.bssid[0],
                    mbrsp->list[index].bssDesc.bssid[1],
                    mbrsp->list[index].bssDesc.bssid[2],
                    mbrsp->list[index].bssDesc.bssid[3],
                    mbrsp->list[index].bssDesc.bssid[4], mbrsp->list[index].bssDesc.bssid[5]);
}


// Releases the scan target lock.
static void MBi_UnlockScanTarget(void)
{
    if (mbrsp->nowScanTargetFlag == 0)
    {
        return;
    }

    if (sUnlockFunc != NULL)
    {
        sUnlockFunc();
    }
    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_NORMAL;
    mbrsp->nowScanTargetFlag = 0;
    mbrsp->notFoundLockTargetCount = 0;

    MB_DEBUG_OUTPUT(" unlock target\n");
}


// Completely deletes the received game information (also deletes the game information enabled flag)
void MB_DeleteRecvGameInfo(int index)
{
    mbrsp->usefulGameInfoFlag &= ~(0x0001 << index);
    mbrsp->validGameInfoFlag &= ~(0x0001 << index);
    MI_CpuClear16(&mbrsp->list[index], sizeof(MBGameInfoRecvList));
}


// Deletes the received game information except for bssDesc.
void MB_DeleteRecvGameInfoWithoutBssdesc(int index)
{
    mbrsp->usefulGameInfoFlag &= ~(0x0001 << index);
    mbrsp->validGameInfoFlag &= ~(0x0001 << index);
    mbrsp->list[index].getFlagmentFlag = 0;
    mbrsp->list[index].allFlagmentFlag = 0;
    mbrsp->list[index].getPlayerFlag = 0;
    mbrsp->list[index].linkLevel = 0;
    MI_CpuClear16(&(mbrsp->list[index].gameInfo), sizeof(MBGameInfo));
}

// get pointer to received parent information structure
MBGameInfoRecvList *MB_GetGameInfoRecvList(int index)
{
    // Returns NULL if there is no valid data
    if ((mbrsp->usefulGameInfoFlag & (0x01 << index)) == 0)
    {
        return NULL;
    }

    return &mbrsp->list[index];
}


//=========================================================
// Setting the user Volatile data
//=========================================================

/*---------------------------------------------------------------------------*
  Name:         MB_SetSendVolatileCallback

  Description:  Sets the multiboot's beacon-transmission callback.
  
  Arguments:    callback    The callback function for completed transmission.
                            This callback is called when data is sent.
                timing     The timing for generating the callback.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_SetSendVolatCallback(MBSendVolatCallbackFunc callback, u32 timming)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    sSendVolatCallback = callback;
    sSendVolatCallbackTimming = timming;

    (void)OS_RestoreInterrupts(enabled);
}



/*---------------------------------------------------------------------------*
  Name:         MB_SetUserVolatData

  Description:  Sets user data in an empty region of the multiboot's beacon
  
  Arguments:    ggid     Specifies the ggid of the program set  by MB_RegisterFile.
                            User data is   attached to this file's beacon.
                userData    Pointer to the user data being set.
                size        Size of the user data being set. (8 bytes max)
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_SetUserVolatData(u32 ggid, const u8 *userData, u32 size)
{
    MBGameInfo *gameInfo;

    SDK_ASSERT(size <= MB_USER_VOLAT_DATA_SIZE);
    SDK_NULL_ASSERT(userData);

    gameInfo = mbss.gameInfoListTop;
    if (gameInfo == NULL)
    {
        return;
    }

    while (gameInfo->ggid != ggid)
    {
        if (gameInfo == NULL)
        {
            return;
        }
        gameInfo = gameInfo->nextp;
    }

    {
        u32     i;

        OSIntrMode enabled = OS_DisableInterrupts();

        for (i = 0; i < size; i++)
        {
            gameInfo->volat.userVolatData[i] = userData[i];
        }

        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetUserVolatData

  Description:  Gets the user data in an empty region of the beacon.
  
  Arguments:    gameInfo    Pointer to the gameInfo parameter that will be  obtained at Scan time.
  
  Returns:      Pointer to the user data.
 *---------------------------------------------------------------------------*/
void   *MB_GetUserVolatData(const WMGameInfo *gameInfo)
{
    MbBeacon *beacon;

    SDK_NULL_ASSERT(gameInfo);

    if (!(gameInfo->attribute & WM_ATTR_FLAG_MB))
    {
        return NULL;
    }

    beacon = (MbBeacon *) (gameInfo->userGameInfo);

    if (beacon->dataAttr != MB_BEACON_DATA_ATTR_VOLAT)
    {
        return NULL;
    }

    return beacon->data.volat.userVolatData;
}








#if defined(MB_CHANGE_TO_UNICODE)

/******************************************************************************/
/* Unicode - SJIS*/

/* There were definitions, so there have been prepared as a stopgap*/
#define SJIS_HIGHER_CODE1_MIN   0x81
#define SJIS_HIGHER_CODE1_MAX   0x9F
#define SJIS_HIGHER_CODE2_MIN   0xE0
#define SJIS_HIGHER_CODE2_MAX   0xFC

// define data------------------------------------------
#define TBL8140_ELEM_NUM                0xbd    // Number of elements in the 8140tbl
#define TBL849f_ELEM_NUM                0x20    // Number of elements in the 849ftbl

typedef struct SjisUtf16Pare
{
    u16     sjis;
    u16     unicode;
}
SjisUtf16Pare;

// extern data------------------------------------------

// function's prototype declaration---------------------
static u16 SearchUnicodeTable(u16 unicode, SjisUtf16Pare * tblp, int elem_num);

// global variable -------------------------------------

// static variable -------------------------------------

// const data  -----------------------------------------
static const SjisUtf16Pare tblSJIS_UTF16_8140[TBL8140_ELEM_NUM];
static const SjisUtf16Pare tblSJIS_UTF16_849f[TBL849f_ELEM_NUM];


//======================================================
// Converts from SJIS-BE to UTF16-LE
//======================================================
void MBi_ExSJIS_BEtoUTF16_LE(u8 *sjisp, u16 *unip, u16 length)
{
    u16     code;

    while ((*sjisp) && (length-- > 0))
    {
        if (((*sjisp >= SJIS_HIGHER_CODE1_MIN) && (*sjisp <= SJIS_HIGHER_CODE1_MAX))
            || ((*sjisp >= SJIS_HIGHER_CODE2_MIN) && (*sjisp <= SJIS_HIGHER_CODE2_MAX)))
        {                              // Is it SJIS?
            code = (u16)((u16)*sjisp++ << 8);
            code |= (u16)*sjisp++;
        }
        else
        {                              // ASCII
            code = (u16)*sjisp++;
        }

        if (code == 0x005c)
        {
            *unip = 0x00a5;            // \

        }
        else if (code == 0x007e)
        {
            *unip = 0x203e;            // ~

        }
        else if ((code == 0x000d) || (code == 0x000a))
        {
            *unip = code;              // Carriage return code    

        }
        else if ((code >= 0x0020) && (code < 0x007e))   // ' ' Å` }
        {
            *unip = code;

        }
        else if ((code >= 0x00a1) && (code <= 0x00df))  // ° Å` ﬂ
        {
            *unip = (u16)((code - 0x00a1) + 0xff61);

        }
        else if ((code >= 0x8140) && (code <= 0x81fc))  // 'Å@' Å` Åõ
        {
            // Scattered, so look up in the table
            *unip = tblSJIS_UTF16_8140[code - 0x8140].unicode;

        }
        else if ((code >= 0x824f) && (code <= 0x8258))  // ÇO Å` ÇX
        {
            *unip = (u16)((code - 0x824f) + 0xff10);

        }
        else if ((code >= 0x8260) && (code <= 0x8279))  // Ç` Å` Çy
        {
            *unip = (u16)((code - 0x8260) + 0xff21);

        }
        else if ((code >= 0x8281) && (code <= 0x829a))  // ÇÅ Å` Çö
        {
            *unip = (u16)((code - 0x8281) + 0xff41);

        }
        else if ((code >= 0x829f) && (code <= 0x82f1))  // Çü Å` ÇÒ
        {
            *unip = (u16)((code - 0x829f) + 0x3041);

        }
        else if ((code >= 0x8340) && (code <= 0x8396))  // É@ Å` Éñ  Note: 0x837f is missing
        {
            *unip = (u16)((code - 0x8340) + 0x30a1);
            if (code == 0x837f)
            {
                *unip = 0x3000;
            }
            else if (code > 0x837f)
            {
                (*unip)--;
            }

        }
        else if ((code >= 0x839f) && (code <= 0x83b6))  // Éü Å` É∂
        {
            *unip = (u16)((code - 0x839f) + 0x0391);
            if (code >= 0x83b0)
                (*unip)++;

        }
        else if ((code >= 0x83bf) && (code <= 0x83d6))  // Éø Å` É÷
        {
            *unip = (u16)((code - 0x83bf) + 0x03b1);
            if (code >= 0x83d0)
                (*unip)++;

        }
        else if ((code >= 0x8440) && (code <= 0x8460))  // Ñ@ Å` Ñ`
        {
            *unip = (u16)((code - 0x8440) + 0x0410);
            if (code == 0x8446)
            {
                *unip = 0x0401;
            }
            else if (code > 0x8446)
            {
                (*unip)--;
            }

        }
        else if ((code >= 0x8470) && (code <= 0x8491))  // Ñp Å` Ñë Note: 0x847f is missing
        {
            *unip = (u16)((code - 0x8470) + 0x0430);
            if (code == 0x8476)
            {
                *unip = 0x0451;
            }
            else if (code == 0x847f)
            {
                *unip = 0x3000;
            }
            else if (code > 0x8476)
            {
                (*unip)--;
                if (code > 0x847f)
                {
                    (*unip)--;
                }
            }

        }
        else if ((code >= 0x849f) && (code <= 0x84be))  // Ñü Å` Ñæ
        {
            // Scattered, so look up in the table
            *unip = tblSJIS_UTF16_849f[code - 0x849f].unicode;
        }
        else
        {
            *unip = 0x3000;
        }

        unip++;
    }
}


//======================================================
// Converts from UTF16-LE to SJIS-BE
//======================================================
void MBi_ExUTF16_LEtoSJIS_BE(u8 *sjisp, u16 *unip, u16 length)
{
    u16     code, sjis_le;

    while ((*unip) && (length-- > 0))
    {
        code = *unip++;
        sjis_le = 0;

        if (code == 0x00a5)
        {
            sjis_le = 0x005c;          // \

        }
        else if (code == 0x005c)
        {
            sjis_le = 0x815f;          // Å_

        }
        else if (code == 0x203e)
        {
            sjis_le = 0x007e;          // ~

        }
        else if ((code == 0x000d) || (code == 0x000a))
        {
            sjis_le = code;            // Carriage return code    

        }
        else if ((code >= 0x0020) && (code < 0x007e))   // ' ' Å` }
        {
            sjis_le = code;

        }
        else if ((code >= 0x00a2) && (code <= 0x00f7))
        {                              // Unicode codes 0x00a2 to 0x00f7 are located in SJIS 0x814c to 0x81f7
            sjis_le =
                SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_8140[0xc],
                                   0x81f7 - 0x814c);

        }
        else if ((code >= 0xff61) && (code <= 0xff9f))  // ° Å` ﬂ
        {
            sjis_le = (u16)((code - 0xff61) + 0x00a1);

        }
        else if (code == 0x4edd)
        {
            sjis_le = 0x8157;

        }
        else if ((code >= 0xff01) && (code <= 0xffe5))
        {
            if ((code >= 0xff10) && (code <= 0xff19))   // 0 - 9
            {
                sjis_le = (u16)((code - 0xff10) + 0x824f);

            }
            else if ((code >= 0xff21) && (code <= 0xff3a))      // A - Z
            {
                sjis_le = (u16)((code - 0xff21) + 0x8260);

            }
            else if ((code >= 0xff41) && (code <= 0xff5a))      // a - z
            {
                sjis_le = (u16)((code - 0xff41) + 0x8281);
            }
            else
            {                          // Unicode codes 0xff01 to 0xffe5 are located in SJIS 0x8143 to 0x8197
                sjis_le =
                    SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_8140[3],
                                       0x8197 - 0x8143);
            }

        }
        else if ((code >= 0x3000) && (code <= 0x30fe))
        {
            if ((code >= 0x3041) && (code <= 0x3093))   // Çü Å` ÇÒ
            {
                sjis_le = (u16)((code - 0x3041) + 0x829f);

            }
            else if ((code >= 0x30a1) && (code <= 0x30f6))      // É@ Å` Éñ  Note: 0x837f is missing
            {
                sjis_le = (u16)((code - 0x30a1) + 0x8340);
                if (code >= 0x30e0)
                {
                    (sjis_le)++;
                }
            }
            else
            {                          // Unicode codes 0x3000 to 0x30fe are located in SJIS 0x8140 to 0x81ac
                sjis_le =
                    SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_8140[0],
                                       0x81ac - 0x8140);
            }

        }
        else if ((code >= 0x0391) && (code <= 0x03a9))  // Éü Å` É∂
        {
            sjis_le = (u16)((code - 0x0391) + 0x839f);
            if (code >= 0x03a3)
                (sjis_le)--;

        }
        else if ((code >= 0x03b1) && (code <= 0x03c9))  // Éø Å` É÷
        {
            sjis_le = (u16)((code - 0x03b1) + 0x83bf);
            if (code >= 0x03c3)
                (sjis_le)--;

        }
        else if (code == 0x0401)
        {
            sjis_le = 0x8446;

        }
        else if ((code >= 0x0410) && (code <= 0x042f))  // Ñ@ Å` Ñ`
        {
            sjis_le = (u16)((code - 0x0410) + 0x8440);
            if (code >= 0x0416)
            {
                (sjis_le)++;
            }

        }
        else if ((code >= 0x0430) && (code <= 0x044f))  // Ñp Å` Ñë Note: 0x847f is missing
        {
            sjis_le = (u16)((code - 0x0430) + 0x8470);
            if (code >= 0x0436)
            {
                (sjis_le)++;
                if (code >= 0x043e)
                {
                    (sjis_le)++;
                }
            }

        }
        else if (code == 0x0451)
        {
            sjis_le = 0x8476;

        }
        else if ((code >= 0x2500) && (code <= 0x254b))  // Ñü Å` Ñæ
        {
            sjis_le =
                SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_849f, TBL849f_ELEM_NUM);

        }
        else if (((code >= 0x2010) && (code <= 0x2312)) || ((code >= 0x25a0) && (code <= 0x266f)))
        {                              // The above-mentioned codes are located in SJIS 0x815c to 0x81fc
            sjis_le =
                SearchUnicodeTable(code, (SjisUtf16Pare *) & tblSJIS_UTF16_8140[0x815c - 0x8140],
                                   0x81fc - 0x815c);

        }
        else
        {
            sjis_le = 0x8140;
        }

        // Stores the converted SJIS codes in the buffer
        if (sjis_le & 0xff00)
        {                              // If not ASCII codes, stores in big-endian format.
            *sjisp++ = (u8)(sjis_le >> 8);
        }
        *sjisp++ = (u8)(sjis_le);
    }
}


// Table lookup Unicode -> SJIS
static u16 SearchUnicodeTable(u16 unicode, SjisUtf16Pare * tblp, int elem_num)
{
    elem_num++;
    while (elem_num--)
    {
        if (tblp->unicode == unicode)
        {
            return tblp->sjis;
        }
        tblp++;
    }
    return 0x8140;
}


//======================================================
// Check SJIS-BE <-> UTF16-LE conversions
//======================================================
void MBi_CheckSJIS_BEtoUTF16_LE(void)
{
    u16     sjis, sjis_be, rev_sjis, sjis_le;
    u16     unicode;

    // Checks ASCII codes
    for (sjis = 0; sjis < 0x00ff; sjis++)
    {
        unicode = 0;
        rev_sjis = 0;
        MBi_ExSJIS_BEtoUTF16_LE((u8 *)&sjis, &unicode, 1);
        MBi_ExUTF16_LEtoSJIS_BE((u8 *)&rev_sjis, &unicode, 1);
        sjis_le = (u16)((rev_sjis >> 8) | (rev_sjis << 8));
        MB_DEBUG_OUTPUT("0x%x\t-> 0x%x\t-> 0x%x\n", sjis, unicode, sjis_le);
    }

    // Checks SJIS codes
    for (sjis = 0x8140; sjis < 0x84ff; sjis++)
    {
        unicode = 0;
        rev_sjis = 0;
        sjis_be = (u16)((sjis >> 8) | (sjis << 8));
        MBi_ExSJIS_BEtoUTF16_LE((u8 *)&sjis_be, &unicode, 1);
        MBi_ExUTF16_LEtoSJIS_BE((u8 *)&rev_sjis, &unicode, 1);
        sjis_le = (u16)((rev_sjis >> 8) | (rev_sjis << 8));
        MB_DEBUG_OUTPUT("0x%x\t-> 0x%x\t-> 0x%x\n", sjis, unicode, sjis_le);
    }
}


//======================================================
// SJIS code -> Unicode table
//======================================================

// 0x8140-
static const SjisUtf16Pare tblSJIS_UTF16_8140[TBL8140_ELEM_NUM] = {
    {0x8140, 0x3000},                  // Å@ 
    {0x8141, 0x3001},                  // ÅA 
    {0x8142, 0x3002},                  // ÅB 
    {0x8143, 0xFF0C},                  // ÅC 
    {0x8144, 0xFF0E},                  // ÅD 
    {0x8145, 0x30FB},                  // ÅE 
    {0x8146, 0xFF1A},                  // ÅF 
    {0x8147, 0xFF1B},                  // ÅG 
    {0x8148, 0xFF1F},                  // ÅH 
    {0x8149, 0xFF01},                  // ÅI 
    {0x814A, 0x309B},                  // ÅJ 
    {0x814B, 0x309C},                  // ÅK 
    {0x814C, 0x00B4},                  // ÅL 
    {0x814D, 0xFF40},                  // ÅM 
    {0x814E, 0x00A8},                  // ÅN 
    {0x814F, 0xFF3E},                  // ÅO 
    {0x8150, 0xFFE3},                  // ÅP 
    {0x8151, 0xFF3F},                  // ÅQ 
    {0x8152, 0x30FD},                  // ÅR 
    {0x8153, 0x30FE},                  // ÅS 
    {0x8154, 0x309D},                  // ÅT 
    {0x8155, 0x309E},                  // ÅU 
    {0x8156, 0x3003},                  // ÅV 
    {0x8157, 0x4EDD},                  // ÅW 
    {0x8158, 0x3005},                  // ÅX 
    {0x8159, 0x3006},                  // ÅY 
    {0x815A, 0x3007},                  // ÅZ 
    {0x815B, 0x30FC},                  // Å[ 
    {0x815C, 0x2015},                  // Å\ 
    {0x815D, 0x2010},                  // Å] 
    {0x815E, 0xFF0F},                  // Å^ 
    {0x815F, 0x005C},                  // Å_ 
    {0x8160, 0x301C},                  // Å` 
    {0x8161, 0x2016},                  // Åa 
    {0x8162, 0xFF5C},                  // Åb 
    {0x8163, 0x2026},                  // Åc 
    {0x8164, 0x2025},                  // Åd 
    {0x8165, 0x2018},                  // Åe 
    {0x8166, 0x2019},                  // Åf 
    {0x8167, 0x201C},                  // Åg 
    {0x8168, 0x201D},                  // Åh 
    {0x8169, 0xFF08},                  // Åi 
    {0x816A, 0xFF09},                  // Åj 
    {0x816B, 0x3014},                  // Åk 
    {0x816C, 0x3015},                  // Ål 
    {0x816D, 0xFF3B},                  // Åm 
    {0x816E, 0xFF3D},                  // Ån 
    {0x816F, 0xFF5B},                  // Åo 
    {0x8170, 0xFF5D},                  // Åp 
    {0x8171, 0x3008},                  // Åq 
    {0x8172, 0x3009},                  // År 
    {0x8173, 0x300A},                  // Ås 
    {0x8174, 0x300B},                  // Åt 
    {0x8175, 0x300C},                  // Åu 
    {0x8176, 0x300D},                  // Åv 
    {0x8177, 0x300E},                  // Åw 
    {0x8178, 0x300F},                  // Åx 
    {0x8179, 0x3010},                  // Åy 
    {0x817A, 0x3011},                  // Åz 
    {0x817B, 0xFF0B},                  // Å{ 
    {0x817C, 0x2212},                  // Å| 
    {0x817D, 0x00B1},                  // Å} 
    {0x817E, 0x00D7},                  // Å~ 
    {0x817F, 0x3000},                  //  
    {0x8180, 0x00F7},                  // ÅÄ 
    {0x8181, 0xFF1D},                  // ÅÅ 
    {0x8182, 0x2260},                  // ÅÇ 
    {0x8183, 0xFF1C},                  // ÅÉ 
    {0x8184, 0xFF1E},                  // ÅÑ 
    {0x8185, 0x2266},                  // ÅÖ 
    {0x8186, 0x2267},                  // ÅÜ 
    {0x8187, 0x221E},                  // Åá 
    {0x8188, 0x2234},                  // Åà 
    {0x8189, 0x2642},                  // Åâ 
    {0x818A, 0x2640},                  // Åä 
    {0x818B, 0x00B0},                  // Åã 
    {0x818C, 0x2032},                  // Åå 
    {0x818D, 0x2033},                  // Åç 
    {0x818E, 0x2103},                  // Åé 
    {0x818F, 0xFFE5},                  // Åè 
    {0x8190, 0xFF04},                  // Åê 
    {0x8191, 0x00A2},                  // Åë 
    {0x8192, 0x00A3},                  // Åí 
    {0x8193, 0xFF05},                  // Åì 
    {0x8194, 0xFF03},                  // Åî 
    {0x8195, 0xFF06},                  // Åï 
    {0x8196, 0xFF0A},                  // Åñ 
    {0x8197, 0xFF20},                  // Åó 
    {0x8198, 0x00A7},                  // Åò 
    {0x8199, 0x2606},                  // Åô 
    {0x819A, 0x2605},                  // Åö 
    {0x819B, 0x25CB},                  // Åõ 
    {0x819C, 0x25CF},                  // Åú 
    {0x819D, 0x25CE},                  // Åù 
    {0x819E, 0x25C7},                  // Åû 
    {0x819F, 0x25C6},                  // Åü 
    {0x81A0, 0x25A1},                  // Å† 
    {0x81A1, 0x25A0},                  // Å° 
    {0x81A2, 0x25B3},                  // Å¢ 
    {0x81A3, 0x25B2},                  // Å£ 
    {0x81A4, 0x25BD},                  // Å§ 
    {0x81A5, 0x25BC},                  // Å• 
    {0x81A6, 0x203B},                  // Å¶ 
    {0x81A7, 0x3012},                  // Åß 
    {0x81A8, 0x2192},                  // Å® 
    {0x81A9, 0x2190},                  // Å© 
    {0x81AA, 0x2191},                  // Å™ 
    {0x81AB, 0x2193},                  // Å´ 
    {0x81AC, 0x3013},                  // Å¨ 
    {0x81AD, 0x3000},                  //  
    {0x81AE, 0x3000},                  //  
    {0x81AF, 0x3000},                  //  
    {0x81B0, 0x3000},                  //  
    {0x81B1, 0x3000},                  //  
    {0x81B2, 0x3000},                  //  
    {0x81B3, 0x3000},                  //  
    {0x81B4, 0x3000},                  //  
    {0x81B5, 0x3000},                  //  
    {0x81B6, 0x3000},                  //  
    {0x81B7, 0x3000},                  //  
    {0x81B8, 0x2208},                  // Å∏ 
    {0x81B9, 0x220B},                  // Åπ 
    {0x81BA, 0x2286},                  // Å∫ 
    {0x81BB, 0x2287},                  // Åª 
    {0x81BC, 0x2282},                  // Åº 
    {0x81BD, 0x2283},                  // ÅΩ 
    {0x81BE, 0x222A},                  // Åæ 
    {0x81BF, 0x2229},                  // Åø 
    {0x81C0, 0x3000},                  //  
    {0x81C1, 0x3000},                  //  
    {0x81C2, 0x3000},                  //  
    {0x81C3, 0x3000},                  //  
    {0x81C4, 0x3000},                  //  
    {0x81C5, 0x3000},                  //  
    {0x81C6, 0x3000},                  //  
    {0x81C7, 0x3000},                  //  
    {0x81C8, 0x2227},                  // Å» 
    {0x81C9, 0x2228},                  // Å… 
    {0x81CA, 0x00AC},                  // Å  
    {0x81CB, 0x21D2},                  // ÅÀ 
    {0x81CC, 0x21D4},                  // ÅÃ 
    {0x81CD, 0x2200},                  // ÅÕ 
    {0x81CE, 0x2203},                  // ÅŒ 
    {0x81CF, 0x3000},                  //  
    {0x81D0, 0x3000},                  //  
    {0x81D1, 0x3000},                  //  
    {0x81D2, 0x3000},                  //  
    {0x81D3, 0x3000},                  //  
    {0x81D4, 0x3000},                  //  
    {0x81D5, 0x3000},                  //  
    {0x81D6, 0x3000},                  //  
    {0x81D7, 0x3000},                  //  
    {0x81D8, 0x3000},                  //  
    {0x81D9, 0x3000},                  //  
    {0x81DA, 0x2220},                  // Å⁄ 
    {0x81DB, 0x22A5},                  // Å€ 
    {0x81DC, 0x2312},                  // Å‹ 
    {0x81DD, 0x2202},                  // Å› 
    {0x81DE, 0x2207},                  // Åﬁ 
    {0x81DF, 0x2261},                  // Åﬂ 
    {0x81E0, 0x2252},                  // Å‡ 
    {0x81E1, 0x226A},                  // Å· 
    {0x81E2, 0x226B},                  // Å‚ 
    {0x81E3, 0x221A},                  // Å„ 
    {0x81E4, 0x223D},                  // Å‰ 
    {0x81E5, 0x221D},                  // ÅÂ 
    {0x81E6, 0x2235},                  // ÅÊ 
    {0x81E7, 0x222B},                  // ÅÁ 
    {0x81E8, 0x222C},                  // ÅË 
    {0x81E9, 0x3000},                  //  
    {0x81EA, 0x3000},                  //  
    {0x81EB, 0x3000},                  //  
    {0x81EC, 0x3000},                  //  
    {0x81EE, 0x3000},                  //  
    {0x81EE, 0x3000},                  //  
    {0x81EF, 0x3000},                  //  
    {0x81F0, 0x212B},                  // Å 
    {0x81F1, 0x2030},                  // ÅÒ 
    {0x81F2, 0x266F},                  // ÅÚ 
    {0x81F3, 0x266D},                  // ÅÛ 
    {0x81F4, 0x266A},                  // ÅÙ 
    {0x81F5, 0x2020},                  // Åı 
    {0x81F6, 0x2021},                  // Åˆ 
    {0x81F7, 0x00B6},                  // Å˜ 
    {0x81F8, 0x3000},                  //  
    {0x81F9, 0x3000},                  //  
    {0x81FA, 0x3000},                  //  
    {0x81FB, 0x3000},                  //  
    {0x81FC, 0x25EF},                  // Å¸ 
};


// 0x849f - 
// 0x2500 to 0x254b in Unicode
static const SjisUtf16Pare tblSJIS_UTF16_849f[TBL849f_ELEM_NUM] = {
    {0x849F, 0x2500},                  // Ñü 
    {0x84A0, 0x2502},                  // Ñ† 
    {0x84A1, 0x250C},                  // Ñ° 
    {0x84A2, 0x2510},                  // Ñ¢ 
    {0x84A3, 0x2518},                  // Ñ£ 
    {0x84A4, 0x2514},                  // Ñ§ 
    {0x84A5, 0x251C},                  // Ñ• 
    {0x84A6, 0x252C},                  // Ñ¶ 
    {0x84A7, 0x2524},                  // Ñß 
    {0x84A8, 0x2534},                  // Ñ® 
    {0x84A9, 0x253C},                  // Ñ© 
    {0x84AA, 0x2501},                  // Ñ™ 
    {0x84AB, 0x2503},                  // Ñ´ 
    {0x84AC, 0x250F},                  // Ñ¨ 
    {0x84AD, 0x2513},                  // Ñ≠ 
    {0x84AE, 0x251B},                  // ÑÆ 
    {0x84AF, 0x2517},                  // ÑØ 
    {0x84B0, 0x2523},                  // Ñ∞ 
    {0x84B1, 0x2533},                  // Ñ± 
    {0x84B2, 0x252B},                  // Ñ≤ 
    {0x84B3, 0x253B},                  // Ñ≥ 
    {0x84B4, 0x254B},                  // Ñ¥ 
    {0x84B5, 0x2520},                  // Ñµ 
    {0x84B6, 0x252F},                  // Ñ∂ 
    {0x84B7, 0x2528},                  // Ñ∑ 
    {0x84B8, 0x2537},                  // Ñ∏ 
    {0x84B9, 0x253F},                  // Ñπ 
    {0x84BA, 0x251D},                  // Ñ∫ 
    {0x84BB, 0x2530},                  // Ñª 
    {0x84BC, 0x2525},                  // Ñº 
    {0x84BD, 0x2538},                  // ÑΩ 
    {0x84BE, 0x2542},                  // Ñæ 
};

#endif /* defined(MB_CHANGE_TO_UNICODE) */
