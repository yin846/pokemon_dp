/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     MbGameInfo.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_gameinfo.h,v $
  Revision 1.12  03/13/2006 06:33:33  yosizaki
  add MB_FAKESCAN_PARENT_BEACON.

  Revision 1.11  2006/01/18 02:11:30  kitase_hirotake
  do-indent

  Revision 1.10  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.9  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.8  11/22/2004 12:50:31  takano_makoto
  Moved MBGameInfoRecvList and MBGameInfoRecvStatus from mb_child.h.

  Revision 1.7  11/10/2004 07:42:51  takano_makoto
  Eliminated the MBw function from the child-side processes in mb_gameinfo.c  

  Revision 1.5  09/14/2004 23:55:48  yosizaki
  disable MB_CHANGE_TO_UNICODE
      (change MBGameregistry's member to Unicode)

  Revision 1.4  09/13/2004 09:24:39  yosiokat
  -  Added wmTypeEvent to the argument of MB_CountGameInfoLifetime as a means to prevent no lock target being found when scan locking.

  Revision 1.3  09/09/2004 14:02:23  sato_masaki
  In mb_gameinfo.c, changed the PlayerFlag format. (Ongoing task)

  Revision 1.2  09/08/2004 04:34:47  sato_masaki
  small fix

  Revision 1.1  09/03/2004 07:06:28  sato_masaki
  Divided files according to features.

  Revision 1.6  09/01/2004 09:40:51  sato_masaki
  Changed the value of MB_AUTHCODE_SIZE.

  Revision 1.5  08/30/2004 06:31:38  sato_masaki
  Added the code auth_code into MBDownloadFileInfo for authentication.

  Revision 1.4  08/19/2004 07:43:11  sato_masaki
  Added a macro definition related to the GameInfo flag.

  Revision 1.3  08/12/2004 09:15:52  yosiokat
  - Added nowLockTimeCount and lockTimeMaxCount into MbBeaconRecvStatus for the scan lock time limit count.

  Revision 1.2  08/09/2004 14:55:22  yosizaki
  format all the symbol for API

  Revision 1.1  08/07/2004 07:03:27  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


/*
 * This header file is used only for internal implementation and on the child device side.  
 * It is not needed for the creation of normal multiboot parent devices.  
 */


#ifndef MB_GAME_INFO_H_
#define MB_GAME_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <nitro/types.h>
#include <nitro/wm.h>
#include <nitro/mb/mb_gameinfo.h>


//=============================================================================
// 
// definition of data types
//
//=============================================================================

#define MB_GAMEINFO_PARENT_FLAG                     (0x0001)
#define MB_GAMEINFO_CHILD_FLAG( __child_aid__ )     ( 1 << ( __child_aid__ ) )

//---------------------------------------------------------
// The game information transmitted by the beacon from the Parent Device to the Child Device.
//---------------------------------------------------------

/*
 * Child side game information receive list structure.    
 */
typedef struct MBGameInfoRecvList
{
    MBGameInfo gameInfo;               // parent game information
    WMBssDesc bssDesc;                 // information for parent connection
    u32     getFlagmentFlag;           // Show currently received fragment beacon as bits.  
    u32     allFlagmentFlag;           // value of flagmentMaxNum converted to bits
    u16     getPlayerFlag;             // Show currently received player flag as bit.    
    s16     lifetimeCount;             // lifetime counter for this information (if this parent beacon is received, lifetime is prolonged)
    u16     linkLevel;                 /* Value (expressed in four steps) indicating the strength*/
    u8      beaconNo;                  // number of last received beacon
    u8      sameBeaconRecvCount;       // the number of times the same beacon number was consecutively received
}
MBGameInfoRecvList, MbGameInfoRecvList;


/*
 * Child side beacon receive status structure    
 */
typedef struct MbBeaconRecvStatus
{
    u16     usingGameInfoFlag;         // Shows in bits the gameInfo array elements being used to receive game information.
    u16     usefulGameInfoFlag;        // Receives all beacons once and shows game information with validGameInfoFlag on.
    // (When updating communication members, etc., the validGameInfoFlag may temporarily go down. For display and connection, use this flag to determine.)
    u16     validGameInfoFlag;         // Shows as bits the gameInfo array element that has completely received game information.  
    u16     nowScanTargetFlag;         // Shows current scan target as bits.  
    s16     nowLockTimeCount;          // Remaining lock time of current Scan target.
    s16     notFoundLockTargetCount;   // number of times current ScanTarget consecutively not found
    u16     scanCountUnit;             // value of current scan time converted to count number
    u8      pad[2];
    MBGameInfoRecvList list[MB_GAME_INFO_RECV_LIST_NUM];        // game information receive list
}
MbBeaconRecvStatus;
/*
 * Note: On the child,
 * getFlagmentFlag == allFlagmentFlag and
 * getPlayerFlag   == gameInfo.volat.nowPlayerFlag,
 * and acquisition of parent information completes.
 */


/*
 * The msg returned by the callback functions of
 *  MB_RecvGameInfoBeacon and MB_CountGameInfoLifetime.
 */
typedef enum MbBeaconMsg
{
    MB_BC_MSG_GINFO_VALIDATED = 1,
    MB_BC_MSG_GINFO_INVALIDATED,
    MB_BC_MSG_GINFO_LOST,
    MB_BC_MSG_GINFO_LIST_FULL,
    MB_BC_MSG_GINFO_BEACON
}
MbBeaconMsg;

typedef void (*MBBeaconMsgCallback) (MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index);

typedef void (*MbScanLockFunc) (u8 *macAddress);
typedef void (*MbScanUnlockFunc) (void);

/******************************************************************************/
/* The following functions are used internally*/


//------------------
// Parent Device Functions   
//------------------

    // initialization of the send status
void    MB_InitSendGameInfoStatus(void);

    // create the MbGameInfo from the MbGameRegistry
void    MBi_MakeGameInfo(MBGameInfo *gameInfop,
                         const MBGameRegistry *mbGameRegp, const MBUserInfo *parent);

    // update the Child Device member information of MbGameInfo
void    MB_UpdateGameInfoMember(MBGameInfo *gameInfop,
                                const MBUserInfo *member, u16 nowPlayerFlag, u16 changePlayerFlag);

    // Add to the send list so that the generated MbGameInfo is transmitted via the beacon.
void    MB_AddGameInfo(MBGameInfo *newGameInfop);

    // Delete the MbGameInfo that is added to the send list.
BOOL    MB_DeleteGameInfo(MBGameInfo *gameInfop);

    // Put the MbGameInfo that is registered in the send list on the beacon and transmit it.
void    MB_SendGameInfoBeacon(u32 ggid, u16 tgid, u8 attribute);


//------------------
// Child Device -side functions   
//------------------
    // allocate beacon receive status buffer in static
void    MBi_SetBeaconRecvStatusBufferDefault(void);
    // set beacon receive status buffer
void    MBi_SetBeaconRecvStatusBuffer(MbBeaconRecvStatus * buf);

    // initialization of the receive status
void    MB_InitRecvGameInfoStatus(void);

    // Extract MbGameInfo from received beacon  
BOOL    MB_RecvGameInfoBeacon(MBBeaconMsgCallback Callbackp, u16 linkLevel, WMBssDesc *bssDescp);

    // the lifespan count of the Parent Device information list
void    MB_CountGameInfoLifetime(MBBeaconMsgCallback Callbackp, BOOL found_parent);

    // set scan lock function
void    MBi_SetScanLockFunc(MbScanLockFunc lockFunc, MbScanUnlockFunc unlockFunc);

    // get pointer to received parent information structure
MBGameInfoRecvList *MB_GetGameInfoRecvList(int index);

/* Get the beacon reception state*/
const MbBeaconRecvStatus *MB_GetBeaconRecvStatus(void);

/* Delete the specified game information*/
void    MB_DeleteRecvGameInfo(int index);
void    MB_DeleteRecvGameInfoWithoutBssdesc(int index);

//------------------
// Symbols for the transition to UNICODE
//------------------
//#define   MB_CHANGE_TO_UNICODE

#if defined(MB_CHANGE_TO_UNICODE)

void    MBi_ExSJIS_BEtoUTF16_LE(u8 *sjisp, u16 *unip, u16 length);
void    MBi_ExUTF16_LEtoSJIS_BE(u8 *sjisp, u16 *unip, u16 length);
void    MBi_CheckSJIS_BEtoUTF16_LE(void);

#endif /* defined(MB_CHANGE_TO_UNICODE)*/

#ifdef __cplusplus
}
#endif

#endif // MB_GAME_INFO_H_
