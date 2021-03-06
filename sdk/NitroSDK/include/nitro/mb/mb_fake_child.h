/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_fake_child.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_fake_child.h,v $
  Revision 1.7  03/13/2006 06:34:02  yosizaki
  add MB_FAKESCAN_PARENT_BEACON.

  Revision 1.6  2006/01/18 02:11:29  kitase_hirotake
  do-indent

  Revision 1.5  12/01/2005 09:15:37  takano_makoto
  Added MB_FAKE_WORK_SIZE

  Revision 1.4  03/01/2005 01:57:00  yosizaki
  Revised copyright year

  Revision 1.3  02/28/2005 05:26:24  yosizaki
  do-indent.

  Revision 1.2  11/24/2004 23:44:44  takano_makoto
  Added MB_FakeReadParentBssDesc, MB_FakeEnd.

  Revision 1.1  11/22/2004 12:57:34  takano_makoto
  Initial update.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/


#ifndef NITRO_MB_MB_FAKE_CHILD_H_
#define NITRO_MB_MB_FAKE_CHILD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/mb/mb.h>

// 
// This is the size of the work buffer added as the MB_FakeInit() argument.
#define     MB_FAKE_WORK_SIZE       (0x5f20)

enum
{
    MB_FAKESCAN_PARENT_FOUND,
    MB_FAKESCAN_PARENT_LOST,
    MB_FAKESCAN_API_ERROR,
    MB_FAKESCAN_END_SCAN,
    MB_FAKESCAN_PARENT_BEACON
};

/*
 * Notification callback for scan parent
 */
typedef struct
{
    u16     index;                     // Index for managing obtained parents
    u16     padding;
    MBGameInfo *gameInfo;              // parent game information
    WMBssDesc *bssDesc;                // Parent connection information
}
MBFakeScanCallback;

/*
 * Error notifying callback
 */
typedef struct
{
    u16     apiid;                     // API code
    u16     errcode;                   // error code
}
MBFakeScanErrorCallback;

typedef void (*MBFakeScanCallbackFunc) (u16 type, void *arg);

void    MB_FakeInit(void *buf, const MBUserInfo *user);
void    MB_FakeEnd(void);
u32     MB_FakeGetWorkSize(void);
void    MB_FakeSetCStateCallback(MBCommCStateCallbackFunc callback);
void    MB_FakeStartScanParent(MBFakeScanCallbackFunc callback, u32 ggid);
void    MB_FakeEndScan(void);
BOOL    MB_FakeEntryToParent(u16 index);
BOOL    MB_FakeGetParentGameInfo(u16 index, MBGameInfo *pGameInfo);
BOOL    MB_FakeGetParentBssDesc(u16 index, WMBssDesc *pBssDesc);
BOOL    MB_FakeReadParentBssDesc(u16 index, WMBssDesc *pBssDesc, u16 parent_max_size,
                                 u16 child_max_size, BOOL ks_flag, BOOL cs_flag);

#ifdef __cplusplus
}
#endif


#endif // NITRO_MB_MB_FAKE_CHILD_H_
