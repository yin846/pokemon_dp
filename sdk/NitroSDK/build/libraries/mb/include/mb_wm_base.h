/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_wm_base.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_wm_base.h,v $
  Revision 1.3  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.2  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.1  02/21/2005 00:40:57  yosizaki
  replace filename from tmp_lib_stub.

  Revision 1.14  01/27/2005 07:05:19  takano_makoto
  fix copyright header.

  Revision 1.13  11/29/2004 04:47:36  yosizaki
  allocate sendBuf. (avoid to share with SetMPData's buffer)

  Revision 1.12  11/22/2004 12:56:22  takano_makoto
  Deleted the structure member for getting the WM's status buffer

  Revision 1.11  11/10/2004 07:43:13  takano_makoto
  Eliminated the MBw function from the child-side processes in mb_gameinfo.c    

  Revision 1.10  10/18/2004 11:44:30  yosizaki
  add MB_StartParentEx, MB_EndEx.

  Revision 1.9  10/05/2004 09:45:29  terui
  Standardized MAC addresses as type u8 arrays.

  Revision 1.7  2004/09/22 09:42:11  sato_masaki
  Merged with IPL_branch 1.5.2.2

  Revision 1.6  2004/09/17 11:35:06  sato_masaki
  add function MBi_IsStarted()

  Revision 1.5  2004/09/15 06:25:01  sato_masaki
  Made variables of parts that were dependent on MB_COMM_P_SENDLEN, MB_COMM_C_SENDLEN definitions.

  Revision 1.4  2004/09/14 02:41:32  sato_masaki
  Added mpBusy to MBiParam.

  Revision 1.2  2004/09/06 11:43:52  sato_masaki
  Added the MBi_GetTgid() function.

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef MB_WM_BASE_H_
#define MB_WM_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nitro/wm.h>

/* Maximum number of connections*/
#define MB_NUM_PARENT_INFORMATIONS              16

#define MB_MAX_SEND_BUFFER_SIZE         (0x400)

/*
 * Select multiboot parent / child.
 * (Cannot execute if child does no do IPL)    
 */
#define MB_MODE_PARENT                          1
#define MB_MODE_CHILD                           2

/*
 * Definitions unique to wm_lib
 */

#define MB_CALLBACK_CHILD_CONNECTED             0
#define MB_CALLBACK_CHILD_DISCONNECTED          1
#define MB_CALLBACK_MP_PARENT_SENT              2
#define MB_CALLBACK_MP_PARENT_RECV              3
#define MB_CALLBACK_PARENT_FOUND                4
#define MB_CALLBACK_PARENT_NOT_FOUND            5
#define MB_CALLBACK_CONNECTED_TO_PARENT         6
#define MB_CALLBACK_DISCONNECTED                7
#define MB_CALLBACK_MP_CHILD_SENT               8
#define MB_CALLBACK_MP_CHILD_RECV               9
#define MB_CALLBACK_DISCONNECTED_FROM_PARENT    10
#define MB_CALLBACK_CONNECT_FAILED              11
#define MB_CALLBACK_DCF_CHILD_SENT              12
#define MB_CALLBACK_DCF_CHILD_SENT_ERR          13
#define MB_CALLBACK_DCF_CHILD_RECV              14
#define MB_CALLBACK_DISCONNECT_COMPLETE         15
#define MB_CALLBACK_DISCONNECT_FAILED           16
#define MB_CALLBACK_END_COMPLETE                17
#define MB_CALLBACK_MP_CHILD_SENT_ERR           18
#define MB_CALLBACK_MP_PARENT_SENT_ERR          19
#define MB_CALLBACK_MP_STARTED                  20
#define MB_CALLBACK_INIT_COMPLETE               21
#define MB_CALLBACK_END_MP_COMPLETE             22
#define MB_CALLBACK_SET_GAMEINFO_COMPLETE       23
#define MB_CALLBACK_SET_GAMEINFO_FAILED         24
#define MB_CALLBACK_MP_SEND_ENABLE              25
#define MB_CALLBACK_PARENT_STARTED              26
#define MB_CALLBACK_BEACON_LOST                 27
#define MB_CALLBACK_BEACON_SENT                 28
#define MB_CALLBACK_BEACON_RECV                 29
#define MB_CALLBACK_MP_SEND_DISABLE             30
#define MB_CALLBACK_DISASSOCIATE                31
#define MB_CALLBACK_REASSOCIATE                 32
#define MB_CALLBACK_AUTHENTICATE                33
#define MB_CALLBACK_SET_LIFETIME                34
#define MB_CALLBACK_DCF_STARTED                 35
#define MB_CALLBACK_DCF_SENT                    36
#define MB_CALLBACK_DCF_SENT_ERR                37
#define MB_CALLBACK_DCF_RECV                    38
#define MB_CALLBACK_DCF_END                     39
#define MB_CALLBACK_MPACK_IND                   40
#define MB_CALLBACK_MP_CHILD_SENT_TIMEOUT       41
#define MB_CALLBACK_SEND_QUEUE_FULL_ERR         42

#define MB_CALLBACK_API_ERROR                   255     // Error on retuned value of API call
#define MB_CALLBACK_ERROR                       256


/*
 * Information on each parent received form beacon. * (Processed internally by child.)    
 */
typedef struct ParentInfo
{
    union
    {
        WMBssDesc parentInfo[1];
        u8      parentInfo_area[WM_BSS_DESC_SIZE] ATTRIBUTE_ALIGN(32);
    };
    /* Probably not used at all except for mac and GameInfo*/
    WMStartScanCallback scan_data;
    u8      reserved1[8];
}
ParentInfo;


/* Format of functions used for MB callbacks*/
typedef void (*MBCallbackFunc) (u16 type, void *arg);


/*
 * wm_lib parameter structure.
 * Temporarily forced to link to part that serves as wm_lib - wm_tool
 * bridge, so it has a portion that does not conform to ANSI-STRICT as * relates to the use of "union."
 (This will be changed later, so postpone)    
 */
typedef struct
{
    /* Parent information setting (Used by parent)*/
    union
    {
        WMParentParam parentParam;
        u8      parentParam_area[WM_PARENT_PARAM_SIZE] ATTRIBUTE_ALIGN(32);
    };

    /* WM internal buffer passed to StartMP (Must be used with SetMP)*/
    u16     sendBuf[MB_MAX_SEND_BUFFER_SIZE / sizeof(u16)] ATTRIBUTE_ALIGN(32);

    /* Buffer that holds parent info (Used by child)*/
    union
    {
        WMBssDesc parentInfoBuf;
        u8      parentInfoBuf_area[WM_BSS_DESC_SIZE] ATTRIBUTE_ALIGN(32);
    };

    u16     p_sendlen;
    u16     p_recvlen;

    WMMpRecvBuf *recvBuf;              /* Receive buffer*/

    /* Parent / child callback*/
    void    (*callback_ptr) (void *arg);

    u8      mpBusy;                    /* MP sending (busy) flag    */
    u8      mbIsStarted;
    u8      reserved0[10];

    u16     sendBufSize;               // Size of Transmission buffer
    u16     recvBufSize;               // Size of MP Reception buffer

    MBCallbackFunc callback;           // Callback for WM_lib
    const WMBssDesc *pInfo;            // Pointer to information of connected parent (Used by child)
    u16     mode;                      // MB_MODE_***
    u16     endReq;

    u16     mpStarted;                 // Flag indicating MP has started
    u16     child_bitmap;              // Child connection state

    /* Continuous send related*/
    u16     contSend;                  // Flag indicating execution of continuous transmission (1: Continuous transmission, 0: Sending 1 frame at a time) 
    u8      reserved1[2];

    // This is related to gameinfo
    u8      uname[WM_SIZE_USERNAME] ATTRIBUTE_ALIGN(4);
    u8      gname[WM_SIZE_GAMENAME] ATTRIBUTE_ALIGN(4);
    u16     currentTgid;               // TGID of parent being connected (Check with BeaconRecv.Ind)
    u8      reserved2[22];

    u16     userGameInfo[((WM_SIZE_USER_GAMEINFO + 32) & ~(32 - 1)) /
                         sizeof(u16)] ATTRIBUTE_ALIGN(32);

    /* Data unique to child*/
    struct
    {
        /*
         * Number of parents that have been discovered.
         * 0 as initial value; ++ on StartScan success; 0 if StartConnect fails.
         * Nobody sees this now, but would be convenient to user if    
         * entered in BeconRecvState.    
         */
        u16     found_parent_count;
        /*
         * The AID allocated to oneself;
         * 0 as initial value; n when StartConnect succeeds.
         * Nobody sees this now.    
         */
        u16     my_aid;

        BOOL    scanning_flag;
        BOOL    scan_channel_flag;
        int     last_found_parent_no;

        u8      reserved8[16];

        /* Information array of 16 parent units*/
        ParentInfo parent_info[MB_NUM_PARENT_INFORMATIONS];
    };

}
MBiParam;


// ===============================================================================
// function

/* Get the last discovered parent*/
int     MBi_GetLastFountParent(void);

/* Get the parent's BSS*/
WMBssDesc *MBi_GetParentBssDesc(int parent);

/* Set the maximum scan time*/
void    MBi_SetMaxScanTime(u16 time);

int     MBi_SendMP(const void *buf, int len, int pollbmp);

int     MBi_GetSendBufSize(void);

int     MBi_GetRecvBufSize(void);

int     MBi_CommConnectToParent(const WMBssDesc *bssDescp);

u32     MBi_GetGgid(void);

u16     MBi_GetTgid(void);

u8      MBi_GetAttribute(void);

int     MBi_RestartScan(void);

int     MBi_GetScanChannel(void);

u16     MBi_GetAid(void);

BOOL    MBi_IsStarted(void);

int     MBi_CommEnd(void);

void    MBi_CommParentCallback(u16 type, void *arg);
void    MBi_CommChildCallback(u16 type, void *arg);

#ifdef __cplusplus
}
#endif

#endif /*  MB_WM_BASE_H_    */
