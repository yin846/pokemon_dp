/*---------------------------------------------------------------------------*
  Project:  NitroSDK - MB - include
  File:     mb_private.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: mb_private.h,v $
  Revision 1.38  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.37  11/07/2005 01:22:05  okubata_ryoma
  Switched from SDK_STATIC_ASSERT to SDK_COMPILER_ASSERT

  Revision 1.36  02/28/2005 05:26:25  yosizaki
  do-indent.

  Revision 1.35  02/21/2005 00:39:34  yosizaki
  replace prefix MBw to MBi.

  Revision 1.34  02/20/2005 23:53:07  takano_makoto
  invalidate PRINT_DEBUG

  Revision 1.33  02/18/2005 11:56:03  yosizaki
  fix around hidden warnings.

  Revision 1.32  01/27/2005 07:05:32  takano_makoto
  fix copyright header.

  Revision 1.31  01/27/2005 07:03:56  takano_makoto
  Added the useWvrFlag member to the MB_CommPWork structure

  Revision 1.30  11/22/2004 12:52:13  takano_makoto
  Adjusted information that can be partitioned and hid in the structure in mb_block mb_gameinfo.    

  Revision 1.29  11/11/2004 11:52:31  yosizaki
  add structures for task-thread.

  Revision 1.28  11/10/2004 13:14:20  yosizaki
  add fileinfo members for cache-system.

  Revision 1.27  10/28/2004 11:02:11  yada
  just fix comment

  Revision 1.26  10/18/2004 11:44:30  yosizaki
  add MB_StartParentEx, MB_EndEx.

  Revision 1.25  10/15/2004 08:50:34  yosizaki
  add start_mp_busy.

  Revision 1.24  2004/10/05 10:05:49  sato_masaki
  Added process to identify the MB termination process when receiving BOOT_REQ

  Revision 1.23  2004/09/25 09:36:52  sato_masaki
  Change so that MB_CommGetChildUser() copies child information in buffer.    

  Revision 1.22  2004/09/25 04:40:34  sato_masaki
  Merged with NitroSDK_IPL_branch 1.14.2.5.

  Revision 1.21  2004/09/24 07:30:57  sato_masaki
  small fix

  Revision 1.20  2004/09/24 01:42:39  sato_masaki
  Fixed bugs related to size of req_data_buf.

  Revision 1.19  2004/09/22 09:42:27  sato_masaki
  Changed MB_MAX_BLOCK value.

  Revision 1.18  2004/09/20 12:52:54  sato_masaki
  Changed the arrangement of MB_CommPWork and MB_CommCWork.

  Revision 1.17  2004/09/17 11:38:10  sato_masaki
  add 'isMbInitialized' member to MB_CommCommonWork structure

  Revision 1.16  09/17/2004 04:03:05  miya
  Removed unnecessary parts

  Revision 1.15  09/16/2004 08:43:29  miya
  Addition of members for file updates

  Revision 1.14  2004/09/15 06:23:32  sato_masaki
  Added variables that store the values that were dependent on the definitions of MB_COMM_P_SENDLEN and MB_COMM_C_SENDLEN in MB_CommCommonWork.
    (block_size_max, req_data_piece_size, req_data_piece_num)

  Revision 1.13  2004/09/14 13:19:50  sato_masaki
  Changed to send request data in fragments.

  Revision 1.12  2004/09/13 13:18:10  sato_masaki
  modify pad bytes

  Revision 1.11  09/11/2004 12:41:42  yosiokat
  -  Added MB_SCAN_TIME_NORMAL and MB_SCAN_TIME_LOCKING along with the corrections to the lick time and the count process of the parent device information lifespan.    

  Revision 1.10  2004/09/11 11:08:49  sato_masaki
  Split up the MB_CommWork structure into MB_CommPWork, MB_CommCWork and MB_CommCommonWork.

  Revision 1.9  2004/09/10 07:22:47  sato_masaki
  debug swith off

  Revision 1.8  2004/09/10 02:59:10  sato_masaki
  PRINT_DEBUG, Made changes to the CALLBACK_WM_STATE definition.

  Revision 1.7  2004/09/09 09:17:00  sato_masaki
  Defined the CALLBACK_WM_STATE.

  Revision 1.6  2004/09/07 04:48:21  sato_masaki
  Padding adjustment.

  Revision 1.5  2004/09/07 04:26:17  sato_masaki
  Renamed rom_header.h to mb_rom_header.h.

  Revision 1.4  2004/09/06 11:12:38  sato_masaki
  Moved the multiboot RAM address definition  (used internally) to mb_child.h.

  Revision 1.3  2004/09/04 11:03:50  sato_masaki
  small fix

  Revision 1.2  2004/09/04 09:59:03  sato_masaki
  Organization of the descriptions.

  Revision 1.1  2004/09/03 07:06:28  sato_masaki
  Divided files according to features.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/

/*
    Only this header file is included in each source file of the MB library.
*/

#ifndef _MB_PRIVATE_H_
#define _MB_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if	!defined(NITRO_FINALROM)
/*
#define PRINT_DEBUG
*/
#endif

#define CALLBACK_WM_STATE       0

#include <nitro/types.h>
#include <nitro/wm.h>
#include <nitro/mb.h>
#include "mb_wm_base.h"
#include "mb_common.h"
#include "mb_block.h"
#include "mb_rom_header.h"
#include "mb_gameinfo.h"
#include "mb_fileinfo.h"
#include "mb_child.h"

/* New features for cache method*/
#include "mb_cache.h"
#include "mb_task.h"

/* debug output*/
#ifdef  PRINT_DEBUG
#define MB_OUTPUT( ... )        OS_TPrintf( __VA_ARGS__ )
#define MB_DEBUG_OUTPUT( ... )  MBi_DebugPrint(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

#else
#define MB_OUTPUT( ... )        ((void)0)
#define MB_DEBUG_OUTPUT( ... )  ((void)0)
#endif

// ----------------------------------------------------------------------------
// definition

/* ----------------------------------------------------------------------------

    definitions related to the block transmission

   ----------------------------------------------------------------------------*/

#define MB_IPL_VERSION                          (0x0001)
#define MB_MAX_BLOCK                            12288   // max 12288 * 256 = 3.0Mbyte

#define MB_NUM_PARENT_INFORMATIONS              16      /* Max number of parents*/

#define MB_SCAN_TIME_NORMAL                     10
#define MB_SCAN_TIME_LOCKING                    220

//SDK_COMPILER_ASSERT( sizeof(MBDownloadFileInfo) <= MB_COMM_BLOCK_SIZE );


/* ----------------------------------------------------------------------------

    definition of types used internally

   ----------------------------------------------------------------------------*/

/*
 * the work region used by the MB library
 */

typedef struct MB_CommCommonWork
{
    //  --- shared    ---
    //  send / receive buffer
    u32     sendbuf[MB_MAX_SEND_BUFFER_SIZE / sizeof(u32)] ATTRIBUTE_ALIGN(32);
    u32     recvbuf[MB_MAX_RECV_BUFFER_SIZE * MB_MAX_CHILD * 2 / sizeof(u32)] ATTRIBUTE_ALIGN(32);
    MBUserInfo user;                   // user information about this device
    u16     isMbInitialized;
    int     block_size_max;
    BOOL    start_mp_busy;             /* WM_StartMP() is operating*/
    BOOL    is_started_ex;             /* WM_StartParentEX() called*/
    u8      padding[28];
}
MB_CommCommonWork;

typedef struct MB_CommPWork
{
    MB_CommCommonWork common;

    //  --- for the parent device  ---
    MBUserInfo childUser[MB_MAX_CHILD] ATTRIBUTE_ALIGN(4);
    u16     childversion[MB_MAX_CHILD]; // child version information (number of child devices part)    
    u32     childggid[MB_MAX_CHILD];   // child GGID(number of child devices part)    
    MBCommPStateCallback parent_callback;       // parent state callback function pointer
    int     p_comm_state[MB_MAX_CHILD]; // wait for state for each child    
    u8      file_num;                  // number of registered files
    u8      cur_fileid;                // number of file currently sending    
    s8      fileid_of_child[MB_MAX_CHILD];      // file number that child requested (-1 when there is no request)
    u8      child_num;                 // number of entry children
    u16     child_entry_bmp;           // child entry control bitmap
    MbRequestPieceBuf req_data_buf;    // buffer for receiving child partition requests
    u16     req2child[MB_MAX_CHILD];   // The request to send to the child
    MBUserInfo childUserBuf;           // child information buffer for passing to application

    //  file information
    struct
    {
        // the DownloadFileInfo buffer
        MBDownloadFileInfo dl_fileinfo;
        MBGameInfo game_info;
        MB_BlockInfoTable blockinfo_table;
        MBGameRegistry *game_reg;
        void   *src_adr;               //  the starting address of the boot image on the Parent Device
        u16     currentb;              // the current block
        u16     nextb;                 // the next block to send
        u16     pollbmp;               // PollBitmap for sending data
        u16     gameinfo_child_bmp;    // the current member bitmap for updating the GameInfo
        u16     gameinfo_changed_bmp;  // the member bitmap generated by the change for the GameInfo update    
        u8      active;
        u8      update;

        /*
         * Added to support cache format read.    
         * Content that these specify is in src_adr.
         */
        MBiCacheList *cache_list;      /* Cache list*/
        u32    *card_mapping;          /* CARD address for start of each segment*/

    }
    fileinfo[MB_MAX_FILE];

    BOOL    useWvrFlag;                // Flag indicating whether WVR is being used
    u8      padding2[20];

    /* Added for task thread*/
    u8      task_work[2 * 1024];
    MBiTaskInfo cur_task;

}
MB_CommPWork;


typedef struct MB_CommCWork
{
    MB_CommCommonWork common;

    //  --- for children  ---
    WMBssDesc bssDescbuf ATTRIBUTE_ALIGN(32);   // backup for WMBssDesc
    MBDownloadFileInfo dl_fileinfo;    // the DownloadFileInfo buffer of the child device
    MBCommCStateCallbackFunc child_callback;    // the state callback function pointer of the child device
    int     c_comm_state;              // the state of the child device
    int     connectTargetNo;           // the MbBeaconRecvStatus list number of the connection  target
    u8      fileid;                    // the requested file number
    u8      _padding1[1];
    u16     user_req;
    u16     got_block;                 // The number of blocks that have downloaded    
    u16     total_block;               // The total blocks of the download files
    u8      recvflag[MB_MAX_BLOCK / 8]; // flag that shows the block receive state
    MB_BlockInfoTable blockinfo_table;
    int     last_recv_seq_no;          // Number of the last received block sequence    
    u8      boot_end_flag;             // Flag for determining whether the MB termination is being processed by BOOT_READY
    u8      _padding2[15];
}
MB_CommCWork;


/* ----------------------------------------------------------------------------

    definition of variables used internally

   ----------------------------------------------------------------------------*/

extern MB_CommCommonWork *mbc;
/* Pointer to work macro; for parent*/
#define pPwork                                  ((MB_CommPWork*)mbc)
/* Pointer to work macro; for child*/
#define pCwork                                  ((MB_CommCWork*)mbc)


#ifdef __cplusplus
}
#endif

#endif /*  _MB_PRIVATE_H_*/
