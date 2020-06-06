/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wbt_internal.h

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wbt_internal.h,v $
  Revision 1.12  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.11  02/28/2005 05:26:34  yosizaki
  do-indent.

  Revision 1.10  10/15/2004 07:13:14  miya
  Dealt with get_blockinfo polling

  Revision 1.9  09/23/2004 07:49:07  miya
  Added init complete flag.

  Revision 1.8  09/01/2004 03:59:54  miya
  Deleted PutBlock, RequestRegisterBlock, ResponseRegisterBlock, and Done commands.

  Revision 1.7  08/28/2004 11:26:30  miya
  Added WBT_PutUserData function

  Revision 1.6  08/27/2004 07:07:46  miya
  Made changes to names, etc.

  Revision 1.5  08/26/2004 12:59:27  miya
  Made changes to names, etc.

  Revision 1.4  08/26/2004 03:18:50  miya
  Added the WBT_RegisterUserDefinedBlock function

  Revision 1.3  08/25/2004 12:28:38  miya
  Added RegBlock-related commands

  Revision 1.2  08/23/2004 12:39:20  miya
  Changed a part that could be problematic with the UI

  Revision 1.1  08/23/2004 09:46:28  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#ifndef	LIBRARIES_WBT_INTERNAL_H__
#define	LIBRARIES_WBT_INTERNAL_H__

#ifdef	__cplusplus
extern "C" {
#endif

/*===========================================================================*/

#include	<nitro/types.h>
#include	<nitro/os.h>


/*---------------------------------------------------------------------------*
	Type Definitions
 *---------------------------------------------------------------------------*/

enum
{
    PARSE_DUMMY = 0,
    PARSE_COMMAND,
    PARSE_TARGET_BITMAP,
    PARSE_COMMAND_COUNTER,
    PARSE_BLOCK_NUM_ENTRY,
    PARSE_PEER_PACKETSIZE,
    PARSE_MY_PACKETSIZE,
    PARSE_BLOCK_ID,
    PARSE_BLOCK_SEQ_NO,
    PARSE_USER_DATA_BUF,
    PARSE_USER_DATA_SIZE,
    PARSE_END
};

enum
{
    COMMAND_STATE_DUMMY = 0,
    COMMAND_STATE_NONE,
    COMMAND_STATE_GET_REQUEST_FROM_PEER,
    COMMAND_STATE_END
};

/*---------------------------------------------------------------------------*
	constant definitions
 *---------------------------------------------------------------------------*/

#define WBT_INFO_PACKET_BITMAP_BUF_SIZE (((sizeof(WBTBlockInfo)+(sizeof(u32)-1))/sizeof(u32))*sizeof(u32))



/*---------------------------------------------------------------------------*
	structure definitions
 *---------------------------------------------------------------------------*/

typedef struct
{
    u8      command_state;
    WBTPacketCommand command;
    WBTCommandCounter peer_cmd_counter;
    WBTCommandCounter last_peer_cmd_counter;
    WBTPacketSize my_packet_size;
    WBTPacketSize peer_packet_size;
    WBTAidBitmap target_bitmap;
    WBTBlockNumEntry num_entry;
    WBTPermission permission;
    u8      user_data_size;
    u8      padd2[2];
    /* Items below should be unions.*/
    WBTBlockId block_id;
    WBTBlockSeqNo block_seq_no;
    WBTBlockSize block_size;
    WBTBlockId info_id;
    void   *data_ptr;
}
WBTRecvToken;


typedef struct
{
    WBTBlockSize block_size;           /* byte*/
    WBTBlockSeqNo num_of_seq;
    WBTBlockSeqNo last_send_seq_no;
    WBTBlockSeqNo last_recv_seq_no;
    WBTBlockSeqNo current_seq_count;
    u32    *bitmap_buf;
    void   *recv_buf;
    u32     binfo_bitmap_buf[WBT_INFO_PACKET_BITMAP_BUF_SIZE / sizeof(u32)];
}
WBTPacketBitmap;


typedef struct
{
    WBTRecvToken recv_token;
    WBTPacketBitmap pkt_bmp;
}
WBTPeerParam;


typedef struct
{
    WBTCommand current_user_cmd;
    WBTCommand next_user_cmd;
    WBTCommand system_cmd;
    WBTPeerParam peer_param[WBT_NUM_OF_AID];
}
WBTCmdParam;

/*---------------------------------------------------------------------------*
	Variable definitions
 *---------------------------------------------------------------------------*/
extern BOOL wbt_initialize_flag;

/*---------------------------------------------------------------------------*
	function definitions
 *---------------------------------------------------------------------------*/


static inline int div32(int a)
{
    return (a >> 5);
}

static inline int mod32(int a)
{
    return (a & 0x1f);
}


WBTPacketSize WBTi_get_peer_data_packet_size(void);
WBTPacketSize WBTi_get_my_data_packet_size(void);
void    WBTi_set_peer_data_packet_size(int size);
void    WBTi_set_my_data_packet_size(int size);

WBTBlockInfoList *WBTi_get_block_info_list_header(void);
void    WBTi_set_block_info_list_header(WBTBlockInfoList *lh);
WBTBlockInfoList *WBTi_get_block_info_list(WBTBlockId block_id);


WBTCommandCounter WBTi_get_my_user_command_counter(void);
void    WBTi_increment_my_user_command_counter(void);
void    WBTi_reset_my_user_command_counter(void);

BOOL    WBTi_SetUserCommand(WBTCommand *cmd);
WBTCommand *WBTi_GetUserCommand(void);
WBTCmdParam *WBTi_GetCommandParameter(void);


int     WBTi_token_check_req_get_block_done(WBTCmdParam * cmd_param, int aid, const void *buf,
                                            int size);
int     WBTi_token_check_req_sync(WBTCmdParam * cmd_param, int aid, const void *buf, int size);
int     WBTi_token_check_req_get_block(WBTCmdParam * cmd_param, int aid, const void *buf, int size);
int     WBTi_token_check_req_get_blockinfo(WBTCmdParam * cmd_param, int aid, const void *buf,
                                           int size);
int     WBTi_token_check_req_get_blockinfo2(WBTCmdParam * cmd_param, int aid, const void *buf,
                                            int size);
int     WBTi_token_check_req_user_data(WBTCmdParam * cmd_param, int aid, const void *buf, int size);
int     WBTi_current_user_request(WBTCmdParam * cmd_param, const void *buf, int size);
int     WBTi_next_user_request(WBTCmdParam * cmd_param, const void *buf, int size);
int     WBTi_get_token_from_recv_buffer(WBTCmdParam * cmd_param, int aid, const void *data,
                                        int recv_size);

int     WBTi_MakeReqWaitCommand(const void *buf, WBTAidBitmap target, WBTCommandCounter counter);


int     WBTi_MakeReqSyncCommand(const void *buf, WBTAidBitmap target, WBTCommandCounter counter,
                                WBTPacketSize my_packet_size, WBTPacketSize peer_packet_size);
int     WBTi_MakeResSyncCommand(const void *buf, WBTAidBitmap target, WBTCommandCounter counter,
                                WBTBlockNumEntry num_entry, WBTPacketSize my_packet_size,
                                WBTPacketSize peer_packet_size);

int     WBTi_MakeReqBlockInfoCommand(const void *buf, WBTAidBitmap target,
                                     WBTCommandCounter counter, WBTBlockId block_id,
                                     WBTBlockSeqNo block_seq_no);
int     WBTi_MakeResBlockInfoCommand(const void *buf, WBTAidBitmap target,
                                     WBTCommandCounter counter, WBTBlockId block_id,
                                     WBTBlockSeqNo block_seq_no, u8 *data, int data_size);
int     WBTi_MakeReqBlockCommand(const void *buf, WBTAidBitmap target, WBTCommandCounter counter,
                                 WBTBlockId block_id, WBTBlockSeqNo block_seq_no);
int     WBTi_MakeResBlockCommand(const void *buf, WBTAidBitmap target, WBTCommandCounter counter,
                                 WBTBlockId block_id, WBTBlockSeqNo block_seq_no, u8 *data,
                                 int data_size);
int     WBTi_MakeReqBlockDoneCommand(const void *buf, WBTAidBitmap target,
                                     WBTCommandCounter counter, WBTBlockId block_id);
int     WBTi_MakeResBlockDoneCommand(const void *buf, WBTAidBitmap target,
                                     WBTCommandCounter counter, WBTBlockId block_id);
int     WBTi_MakeReqUserDataCommand(const void *buf, WBTAidBitmap target, WBTCommandCounter counter,
                                    void *user_data, u8 size);
int     WBTi_MakeResUserDataCommand(const void *buf, WBTAidBitmap target,
                                    WBTCommandCounter counter);



/*===========================================================================*/

#ifdef	__cplusplus
}          /* extern "C"*/
#endif

#endif /* LIBRARIES_WBT_INTERNAL_H__ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
