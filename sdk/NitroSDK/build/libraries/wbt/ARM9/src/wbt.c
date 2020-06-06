/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wbt.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wbt.c,v $
  Revision 1.25  03/01/2006 02:12:50  adachi_hiroaki
  Changed code so that warning is not displayed when compiled with gcc

  Revision 1.24  2006/01/18 03:12:38  kitase_hirotake
  do-indent

  Revision 1.23  12/16/2005 08:54:48  yosizaki
  fix result of user-callback.

  Revision 1.22  10/20/2005 05:01:20  yosizaki
  fix system-callback on GetBlockInfo-done.

  Revision 1.21  02/28/2005 06:26:32  yosizaki
  do-indent.

  Revision 1.20  02/18/2005 13:03:23  yosizaki
  fix around hidden warnings.

  Revision 1.19  10/15/2004 08:13:21  miya
  Dealt with get_blockinfo polling

  Revision 1.18  10/15/2004 06:18:58  miya
  Fixed bugs around get_blockinfo

  Revision 1.17  10/04/2004 09:38:42  miya
  modified comments

  Revision 1.16  09/21/2004 06:29:21  yosizaki
  adjust some variable types.

  Revision 1.15  09/14/2004 02:33:57  miya
  Added WBT_SetPacketSize function.

  Revision 1.14  09/09/2004 07:41:14  miya
  small fix.

  Revision 1.13  09/04/2004 09:57:22  miya
  Changed the WBT_CMD_PREPARE_SEND_DATA event processing

  Revision 1.12  09/03/2004 13:03:09  miya
  Fixed the bug during WBT_CMD_PREPARE_SEND_DATA event processing

  Revision 1.11  09/03/2004 10:29:31  seiki_masashi
  small fix

  Revision 1.10  09/01/2004 04:59:50  miya
  Deleted PutBlock, RequestRegisterBlock, ResponseRegisterBlock, and Done commands.

  Revision 1.9  08/28/2004 12:26:42  miya
  Added WBT_PutUserData function

  Revision 1.8  08/27/2004 08:07:52  miya
  Made changes to names, etc.

  Revision 1.7  08/26/2004 13:59:23  miya
  Made changes to names, etc.

  Revision 1.6  08/26/2004 06:00:53  miya
  bug fixes

  Revision 1.5  08/26/2004 04:19:00  miya
  Added the WBT_RegisterUserDefinedBlock function

  Revision 1.4  08/25/2004 13:28:48  miya
  Added RegBlock-related commands

  Revision 1.3  08/24/2004 06:46:37  miya
  comments added

  Revision 1.2  08/23/2004 13:39:28  miya
  Changed a part that could be problematic with the UI

  Revision 1.1  08/23/2004 10:46:09  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include	<nitro.h>
#include	<nitro/wbt.h>
#include	"wbt_internal.h"

// #define PRINTF_DEBUG 1
// #define PRINTF_DEBUG_L1 1
// #define PRINTF_DEBUG_L2 1


/*---------------------------------------------------------------------------*
	Not supported
	* Check for division by zero
	* Added time (count) out
	* Simultaneous reception of multiple user commands (can also simultaneously receive multiple blocks by linking with lists)
	* Should do something with last_block_id = -1 and  last_seq_no = -1 in the wbt_LastSendBlockSeqCheck function
	* Combine (separate?!) peer_param and own_param global data in wbt_data.c
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
	structure definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
	Internal function definitions
 *---------------------------------------------------------------------------*/
static inline BOOL address_to_myself(WBTAidBitmap bitmap);
static BOOL WBT_packet_bitmap_check(WBTPacketBitmap * pkt_bmp, WBTBlockSeqNo seq_no);
static void WBT_packet_bitmap_set(WBTPacketBitmap * pkt_bmp, WBTBlockSeqNo seq_no);
static void WBT_packet_bitmap_init(WBTPacketBitmap * pkt_bmp, WBTBlockSize block_size,  /* byte*/
                                   u32 *bitmap_buf, u8 *data_recv_buf);
static WBTBlockSeqNo wbt_packet_bitmap_next_seq_no(WBTPacketBitmap * pkt_bmp);
static BOOL wbt_save_block_seq(WBTPacketBitmap * pkt_bmp, WBTBlockSeqNo block_seq_no, u8 *data_ptr);
static WBTBlockSeqNo wbt_LastSendBlockSeqCheck(WBTBlockId block_id, WBTBlockSeqNo seq_no);
static u8 *wbt_GetBlockDataPtr(WBTBlockId block_id, WBTBlockSeqNo seq_no);
static void wbt_system_callback(WBTCmdParam * cmd_param, WBTCommandType event,
                                int aid, WBTResult result);
static void wbt_user_callback(WBTCmdParam * cmd_param, WBTCommandType event, int aid);


/*---------------------------------------------------------------------------*
	Data definitions
 *---------------------------------------------------------------------------*/

/* ------------------------ Internal functions --------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         address_to_myself

  Description:  Gets whether the self-bit is enabled in the AID bitmap

  Arguments:    target_bitmap - AID bitmap

  Returns:      BOOL - TRUE  The self-bit is enabled
                       FALSE The self-bit is disabled
 *---------------------------------------------------------------------------*/
static inline BOOL address_to_myself(WBTAidBitmap target_bitmap)
{
#pragma unused(target_bitmap)

    if (WBT_GetOwnAid() == -1)
    {
        return FALSE;
    }
   /*****************************************************/
    /*    +---------------------------------+            */
    /* Hi |C15|C14|C13|  -------| C2| C1| P | Lo 16BIT*/
    /*    +---------------------------------+            */
   /*****************************************************/
    return ((target_bitmap & (1 << WBT_GetOwnAid()))? TRUE : FALSE);
}


/*---------------------------------------------------------------------------*
  Name:         token_read

  Description:  Copies the specified size from the reception buffer

  Arguments:    src  - Copy source
                dest - Copy destination
                size - Copy size

  Returns:      u8*  Adds the copy size to the pointer of the copy source and returns
                    
 *---------------------------------------------------------------------------*/
static inline u8 *token_read(u8 *src, void *dest, int size)
{
    if (size < 1)
        return src;
    MI_CpuCopy8(src, dest, (u32)size);
    return (u8 *)((u32)src + (u32)size);
}


/*---------------------------------------------------------------------------*
  Name:         LastSendBlockSeqCheck

  Description:  Determines the block sequence number of the next block to send back
                from the block sequence number requested by the other station
                (The requested sequence number is not returned as-is)

  Arguments:    block_id - The block ID requested by the other station
                seq_no   - The block sequence number requested by the other station

  Returns:      WBTBlockSeqNo - Block sequence number that is sent back
 *---------------------------------------------------------------------------*/
static WBTBlockSeqNo wbt_LastSendBlockSeqCheck(WBTBlockId block_id, WBTBlockSeqNo seq_no)
{
    static WBTBlockId last_block_id = (WBTBlockId)-1;
    static WBTBlockSeqNo last_seq_no_1 = -1;
    static WBTBlockSeqNo last_seq_no_2 = -1;
    int     loop_counter;
    WBTBlockInfoList *list = WBTi_get_block_info_list_header();

    /* If block info*/
    if (block_id < WBT_NUM_MAX_BLOCK_INFO_ID)
    {
        /* If block info,  do nothing because it's too much trouble otherwise*/
        return seq_no;
    }

    /* If block info*/
    while (list != NULL)
    {
        if (list->data_info.id == block_id)
        {
            if (block_id == last_block_id)
            {
                loop_counter = 0;
                while (seq_no == last_seq_no_1 || seq_no == last_seq_no_2)
                {
                    seq_no++;
                    if (seq_no > (((list->data_info.block_size + WBTi_get_my_data_packet_size() - 1)
                                   / WBTi_get_my_data_packet_size()) - 1))
                    {
                        seq_no = 0;
                    }
                    loop_counter++;
                    if (loop_counter > 2)
                    {
                        break;
                    }
                }
            }
            last_block_id = block_id;
            last_seq_no_2 = last_seq_no_1;
            last_seq_no_1 = seq_no;
            return seq_no;
        }
        list = list->next;
    }
    return -1;
}



/*---------------------------------------------------------------------------*
  Name:         wbt_GetBlockDataPtr

  Description:  Gets block data pointer

  Arguments:    block_id       - Block ID
                seq_no     - Block sequence number

  Returns:      u8* - Pointer to block data
 *---------------------------------------------------------------------------*/
static u8 *wbt_GetBlockDataPtr(WBTBlockId block_id, WBTBlockSeqNo seq_no)
{
    u8     *ptr = NULL;
    WBTBlockInfoList *list = WBTi_get_block_info_list_header();
    int     block_info_count;

    /* If block info*/
    if (block_id < WBT_NUM_MAX_BLOCK_INFO_ID)
    {
        block_info_count = 0;
        while (1)
        {
            if (list != NULL)
            {
                if (block_info_count == block_id)
                {
                    ptr = (u8 *)&(list->data_info);
                    ptr += (seq_no * WBTi_get_my_data_packet_size());
                    return ptr;
                }
                block_info_count++;
                list = list->next;
            }
            else
            {
                return NULL;           /* Requested block ID not there*/
            }
        }
    }

    /* If block info*/
    while (list != NULL)
    {
        if (list->data_info.id == block_id)
        {
            ptr = list->data_ptr;
            ptr = (u8 *)((u32)ptr + (seq_no * WBTi_get_my_data_packet_size()));
            return ptr;
        }
        list = list->next;
    }
    return NULL;                       /* Requested block ID not there*/
}

#define BITS_32 32

/*---------------------------------------------------------------------------*
  Name:         wbt_packet_bitmap_init

  Description:  Initializes buffer for recording the packet received number

  Arguments:    pkt_bmp  - Pointer to the structure to control the buffer for recording the number of the packet reception
                block_size    - Block size
                bitmap_buf    - Buffer for recording the packet received number
                data_recv_buf - Data reception buffer

  Returns:      none.
 *---------------------------------------------------------------------------*/
static void wbt_packet_bitmap_init(WBTPacketBitmap * pkt_bmp, WBTBlockSize block_size,  /* byte*/
                                   u32 *bitmap_buf, u8 *data_recv_buf)
{
    int     bytes;
    WBTPacketSize pkt_size = WBTi_get_peer_data_packet_size();

    pkt_bmp->block_size = block_size;
    pkt_bmp->last_send_seq_no = 0;
    pkt_bmp->last_recv_seq_no = 0;
    pkt_bmp->current_seq_count = 0;
    pkt_bmp->num_of_seq = (block_size / pkt_size) + ((block_size % pkt_size) ? 1 : 0);
    pkt_bmp->bitmap_buf = bitmap_buf;
    pkt_bmp->recv_buf = data_recv_buf;
    bytes = WBT_CalcPacketbitmapSize(block_size);
#ifdef PRINTF_DEBUG_L2
    OS_Printf("pktbmp init block_size=%d num_of_seq=%d %d bytes\n",
              block_size, pkt_bmp->num_of_seq, bytes);
#endif
    MI_CpuClear8(bitmap_buf, (u32)bytes);
}

/*---------------------------------------------------------------------------*
  Name:         wbt_packet_bitmap_check

  Description:  Checks whether a block sequence has already been received

  Arguments:    pkt_bmp  - Pointer to the structure to control the buffer for recording the number of the packet reception
                seq_no     - Block sequence number

  Returns:      BOOL
 *---------------------------------------------------------------------------*/
static BOOL wbt_packet_bitmap_check(WBTPacketBitmap * pkt_bmp, WBTBlockSeqNo seq_no)
{
    /* Check the bit*/
    u32    *bmp;
    bmp = pkt_bmp->bitmap_buf + (div32(seq_no));

    if (*bmp & (1 << mod32(seq_no)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         wbt_packet_bitmap_set

  Description:  Records a received stamp in the received block sequence

  Arguments:    pkt_bmp  - Pointer to the structure to control the buffer for recording the number of the packet reception
                seq_no  - Received block sequence number

  Returns:      none.
 *---------------------------------------------------------------------------*/
static void wbt_packet_bitmap_set(WBTPacketBitmap * pkt_bmp, WBTBlockSeqNo seq_no)
{
    /* Set the bit*/
    u32    *bmp;
    pkt_bmp->current_seq_count++;
    pkt_bmp->last_recv_seq_no = seq_no;
    bmp = pkt_bmp->bitmap_buf + div32(seq_no);
    *bmp |= (1 << mod32(seq_no));
}


/*---------------------------------------------------------------------------*
  Name:         wbt_packet_bitmap_next_seq_no

  Description:  Gets the block sequence number to be requested next

  Arguments:    pkt_bmp  - Pointer to the structure to control the buffer for recording the number of the packet reception

  Returns:      WBTBlockSeqNo - Block sequence number
 *---------------------------------------------------------------------------*/
static WBTBlockSeqNo wbt_packet_bitmap_next_seq_no(WBTPacketBitmap * pkt_bmp)
{
    int     num;
    int     last_num;
    int     bit_num;
    u32    *bmp;

    num = pkt_bmp->last_recv_seq_no + 1;

    if (num >= pkt_bmp->num_of_seq)
    {
        num = 0;
    }

    last_num = num;

    bmp = pkt_bmp->bitmap_buf + div32(num);

    bit_num = mod32(num);

    while (1)
    {
        if (*bmp & (u32)((u32)1 << bit_num))
        {
            num++;
            if (num < pkt_bmp->num_of_seq)
            {
                bit_num++;

                if (bit_num > (BITS_32 - 1))
                {
                    bit_num = 0;
                    bmp++;
                }
            }
            else
            {
                num = 0;
                bit_num = 0;
                bmp = pkt_bmp->bitmap_buf;
            }
            if (num == last_num)
            {
                break;                 /* No fallen bit*/
            }
        }
        else
        {
            pkt_bmp->last_send_seq_no = num;
            return num;
        }
    }
    return -1;
}


/*---------------------------------------------------------------------------*
  Name:         wbt_save_block_seq

  Description:  Saves block data

  Arguments:    pkt_bmp  - Pointer to the structure to control the buffer for recording the number of the packet reception
                block_seq_no - Block sequence number
                data_ptr     - Pointer to reception data (source data)

  Returns:      BOOL - Success/failure
 *---------------------------------------------------------------------------*/
static BOOL wbt_save_block_seq(WBTPacketBitmap * pkt_bmp, WBTBlockSeqNo block_seq_no, u8 *data_ptr)
{

    s16     peer_pkt_size = WBTi_get_peer_data_packet_size();
    int     mod;
    u8     *dest_org = pkt_bmp->recv_buf;

    if (block_seq_no < pkt_bmp->num_of_seq)
    {
        if (block_seq_no == (pkt_bmp->num_of_seq - 1))
        {
            mod = pkt_bmp->block_size % peer_pkt_size;
            if (mod != 0)
            {
                MI_CpuCopy8(data_ptr, dest_org + block_seq_no * peer_pkt_size, (u32)mod);
            }
            else
            {
                MI_CpuCopy8(data_ptr, dest_org + block_seq_no * peer_pkt_size, peer_pkt_size);
            }
        }
        else
        {
            MI_CpuCopy8(data_ptr, dest_org + block_seq_no * peer_pkt_size, peer_pkt_size);
        }
    }
    else
    {
#ifdef PRINTF_DEBUG_L1
        OS_Printf("%s num of seq over seq no = %d num_of_seq = %d\n", __FUNCTION__, block_seq_no,
                  pkt_bmp->num_of_seq);
#endif
        return FALSE;                  /* error*/
    }

    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         wbt_system_callback

  Description:  Notify user that a request has arrived from the other station via a callback

  Arguments:    cmd_param - pointer to command parameter
                event     - Packet command that is the callback trigger
                            (user command of the other station)
                aid       - Other station AID
                result    - Process result

  Returns:      none.     -
 *---------------------------------------------------------------------------*/
static void
wbt_system_callback(WBTCmdParam * cmd_param, WBTCommandType event, int aid, WBTResult result)
{
    WBTCommand *system_cmd = &(cmd_param->system_cmd);
    WBTRecvToken *token = &(cmd_param->peer_param[aid].recv_token);

    if (event != WBT_CMD_PREPARE_SEND_DATA)
    {
        if (token->peer_cmd_counter == token->last_peer_cmd_counter)
        {
            return;
        }
    }

    /*
     * Since the child's GetBlockInfo() uses GetBlock() internally,
     * Filtering is required so that this notification is not sent to the parent.
     */
    if (event == WBT_CMD_REQ_GET_BLOCK_DONE)
    {
        if (token->block_id < WBT_NUM_MAX_BLOCK_INFO_ID)
        {
            return;
        }
    }

    system_cmd->command = (WBTCommandType)WBT_CMD_SYSTEM_CALLBACK;
    system_cmd->peer_bmp = WBT_AidToAidbitmap(aid);
    token->last_peer_cmd_counter = token->peer_cmd_counter;
    system_cmd->peer_cmd_counter = token->peer_cmd_counter;     /* For debugging*/
    system_cmd->result = result;
    switch (event)
    {
    case WBT_CMD_REQ_GET_BLOCK_DONE:
        system_cmd->blockdone.block_id = token->block_id;
        break;

    case WBT_CMD_REQ_SYNC:
        system_cmd->sync.num_of_list = token->num_entry;
        system_cmd->sync.peer_packet_size = token->peer_packet_size;
        system_cmd->sync.my_packet_size = token->my_packet_size;
        break;

    case WBT_CMD_PREPARE_SEND_DATA:
        break;

    case WBT_CMD_REQ_USER_DATA:
        if (token->user_data_size > WBT_SIZE_USER_DATA)
        {
            system_cmd->user_data.size = 0;
        }
        else
        {
            system_cmd->user_data.size = token->user_data_size;
            MI_CpuCopy8(token->data_ptr, system_cmd->user_data.data, system_cmd->user_data.size);
        }
        break;

    case WBT_CMD_REQ_ERROR:
        break;

    default:
        break;
    }
    system_cmd->event = event;

    if (system_cmd->callback)
    {
        system_cmd->callback((void *)system_cmd);
    }
}

/*---------------------------------------------------------------------------*
  Name:         wbt_user_callback

  Description:  Issues a callback to the user command

  Arguments:    cmd_param - pointer to command parameter
                event     - User command that is the callback trigger
                aid       - Other station AID

  Returns:      none.
 *---------------------------------------------------------------------------*/
static void wbt_user_callback(WBTCmdParam * cmd_param, WBTCommandType event, int aid)
{
    WBTAidBitmap peer_bmp;
    WBTCommand *current_user_cmd = &(cmd_param->current_user_cmd);
    WBTRecvToken *token = &(cmd_param->peer_param[aid].recv_token);
    current_user_cmd->peer_bmp = WBT_AidToAidbitmap(aid);       /* For debugging*/

    peer_bmp = WBT_AidToAidbitmap(aid);

    if (current_user_cmd->my_cmd_counter != token->peer_cmd_counter)
    {
        return;
    }
    current_user_cmd->peer_cmd_counter = token->peer_cmd_counter;

    if (current_user_cmd->target_bmp & peer_bmp)
    {
        current_user_cmd->target_bmp &= (~peer_bmp);

        switch (event)
        {
        case WBT_CMD_RES_SYNC:
#ifdef PRINTF_DEBUG
            OS_Printf("Get res Sync from %d my %d peer %d\n",
                      aid, token->my_packet_size, token->peer_packet_size);
#endif
            current_user_cmd->sync.num_of_list = token->num_entry;
            current_user_cmd->sync.peer_packet_size = token->peer_packet_size;
            current_user_cmd->sync.my_packet_size = token->my_packet_size;
            break;

        case WBT_CMD_RES_GET_BLOCK:
        case WBT_CMD_RES_GET_BLOCKINFO:
            /* Notify the user that recipient has confirmed that the block was completely received*/
#ifdef PRINTF_DEBUG_L1
            OS_Printf("get block my cmd counter = %d peer cmd counter = %d\n",
                      current_user_cmd->my_cmd_counter, token->peer_cmd_counter);
#endif
            break;

        case WBT_CMD_RES_USER_DATA:
            break;

        default:
            break;
        }

        current_user_cmd->event = event;
        /* This is a completion notification, so it's usually "SUCCESS." (Errors are notified to a system callback.)*/
        current_user_cmd->result = WBT_RESULT_SUCCESS;

        if (current_user_cmd->callback)
        {
            current_user_cmd->callback((void *)current_user_cmd);
        }
    }

    if (current_user_cmd->target_bmp == 0)
    {
        /* If response is not received from all children, the next command cannot be received. (Not good)*/
        /* Set up count in this function!? */
        current_user_cmd->command = WBT_CMD_REQ_NONE;
    }
}


/* ------------------------ External functions --------------------------------------*/


/*---------------------------------------------------------------------------*
  Name:         WBTi_get_token_from_recv_buffer

  Description:  Extracts the packet command from the reception buffer

  Arguments:    cmd_param - Pointer to command parameter
                aid       - Other station's AID
                data      - Reception buffer
                recv_size - Reception size

  Returns:      int
 *---------------------------------------------------------------------------*/
int
WBTi_get_token_from_recv_buffer(WBTCmdParam * cmd_param, int aid, const void *data, int recv_size)
{
    int     i = 0;
    int     state = PARSE_COMMAND;
    void   *recv_data = (void *)data;
    WBTRecvToken *token = &(cmd_param->peer_param[aid].recv_token);
    WBTCommand *system_cmd = &(cmd_param->system_cmd);
    WBTCommand *current_user_cmd = &(cmd_param->current_user_cmd);
    WBTPacketBitmap *pkt_bmp = &(cmd_param->peer_param[aid].pkt_bmp);
    WBTAidBitmap peer_bmp;

    token->command_state = COMMAND_STATE_NONE;
    if (recv_size == 0)
    {
        return 0;
    }

    /* Actually, state transition must be byte by byte while noting endianess*/
    while (i < recv_size)
    {
        switch (state)
        {
        case PARSE_COMMAND:
            recv_data = token_read(recv_data, &(token->command), sizeof(WBTPacketCommand));
            i += sizeof(WBTPacketCommand);
            state = PARSE_TARGET_BITMAP;
            break;
        case PARSE_TARGET_BITMAP:
            recv_data = token_read(recv_data, &(token->target_bitmap), sizeof(WBTAidBitmap));
            i += sizeof(WBTAidBitmap);
            if (FALSE == address_to_myself(token->target_bitmap))
            {
                /* Not relevant; Return.*/
                return i;
            }
            state = PARSE_COMMAND_COUNTER;
            break;
        case PARSE_COMMAND_COUNTER:
            recv_data =
                token_read(recv_data, &(token->peer_cmd_counter), sizeof(WBTCommandCounter));
            i += sizeof(WBTCommandCounter);

            switch (token->command)
            {
            case WBT_CMD_REQ_WAIT:
                return i;              /* Establish command*/

            case WBT_CMD_REQ_SYNC:
                state = PARSE_PEER_PACKETSIZE;
                break;
            case WBT_CMD_RES_SYNC:
                state = PARSE_BLOCK_NUM_ENTRY;
                break;
            case WBT_CMD_RES_USER_DATA:
                /* Use for bug and user callback*/
                if (current_user_cmd->command == WBT_CMD_REQ_USER_DATA)
                {
                    wbt_user_callback(cmd_param, (WBTCommandType)(token->command), aid);
                }
                return i;              /* Establish command*/
            case WBT_CMD_REQ_USER_DATA:
                state = PARSE_USER_DATA_SIZE;
                break;
            default:
                state = PARSE_BLOCK_ID;
                break;
            }
            break;
        case PARSE_USER_DATA_SIZE:
            recv_data = token_read(recv_data, &(token->user_data_size), sizeof(u8));
            i += sizeof(u8);
            state = PARSE_USER_DATA_BUF;
            break;
        case PARSE_USER_DATA_BUF:
            token->data_ptr = recv_data;
            recv_data = (void*)((u8 *)recv_data + WBT_SIZE_USER_DATA);
            i += WBT_SIZE_USER_DATA;
            wbt_system_callback(cmd_param, (WBTCommandType)token->command, aid, WBT_RESULT_SUCCESS);
            token->command_state = COMMAND_STATE_GET_REQUEST_FROM_PEER; /* Establish command*/
            return i;                  /* Establish command*/
            break;
        case PARSE_BLOCK_NUM_ENTRY:
            recv_data = token_read(recv_data, &(token->num_entry), sizeof(WBTBlockNumEntry));
            i += sizeof(WBTBlockNumEntry);
            switch (token->command)
            {
            case WBT_CMD_RES_SYNC:
                state = PARSE_PEER_PACKETSIZE;
                break;
            default:
                /* Unknown command*/
                break;
            }
            break;
        case PARSE_PEER_PACKETSIZE:
            recv_data = token_read(recv_data, &(token->peer_packet_size), sizeof(WBTPacketSize));
            i += sizeof(WBTPacketSize);
            recv_data = token_read(recv_data, &(token->my_packet_size), sizeof(WBTPacketSize));
            i += sizeof(WBTPacketSize);
            if (WBT_AID_PARENT != WBT_GetOwnAid())
            {
                WBTi_set_my_data_packet_size(token->my_packet_size);
                WBTi_set_peer_data_packet_size(token->peer_packet_size);
            }
            switch (token->command)
            {
            case WBT_CMD_REQ_SYNC:
                /* Callback to indicate to user that recipient is synchronized.*/
                wbt_system_callback(cmd_param, (WBTCommandType)token->command, aid,
                                    WBT_RESULT_SUCCESS);
                /* Leave the rest to the functions used for sending*/
                token->command_state = COMMAND_STATE_GET_REQUEST_FROM_PEER;     /* Establish command*/
                return i;
            case WBT_CMD_RES_SYNC:
                /* Remember the recipients block list size; Child only*/
                if (current_user_cmd->command == WBT_CMD_REQ_SYNC)
                {
                    wbt_user_callback(cmd_param, (WBTCommandType)(token->command), aid);
                }
                else
                {
                    /* Received unexpected RES_SYNC*/
                }
                return i;              /* Establish command*/
            default:
                /* Unknown command*/
                wbt_system_callback(cmd_param, WBT_CMD_RES_ERROR, aid,
                                    WBT_RESULT_ERROR_UNKNOWN_PACKET_COMMAND);
                return i;
            }
            break;
        case PARSE_BLOCK_ID:
            recv_data = token_read(recv_data, &(token->block_id), sizeof(WBTBlockId));
            i += sizeof(WBTBlockId);
            switch (token->command)
            {
            case WBT_CMD_REQ_GET_BLOCK_DONE:
                /* Callback to notify the user that the recipient has finished downloading*/
                wbt_system_callback(cmd_param, (WBTCommandType)token->command, aid,
                                    WBT_RESULT_SUCCESS);
                /* Leave the rest to the functions used for sending*/
                token->command_state = COMMAND_STATE_GET_REQUEST_FROM_PEER;     /* Establish command*/
                return i;
                break;
            case WBT_CMD_RES_GET_BLOCK_DONE:
                if (current_user_cmd->command == WBT_CMD_REQ_GET_BLOCK)
                {
                    /* Confirm if this is the block ID that was requested*/
                    if (current_user_cmd->get.block_id == token->block_id)
                    {
                        wbt_user_callback(cmd_param, WBT_CMD_RES_GET_BLOCK, aid);
                    }
                    else
                    {
                        /* Unrelated block ID*/
                    }
#ifdef PRINTF_DEBUG
                    OS_Printf("c usr cmd tbmp 0x%x\n", current_user_cmd->target_bmp);
#endif
                }
                else if (current_user_cmd->command == WBT_CMD_REQ_GET_BLOCKINFO)
                {
                    /* Confirm if this is the block ID that was requested*/
                    if (current_user_cmd->get.block_id == token->block_id)
                    {
                        wbt_user_callback(cmd_param, WBT_CMD_RES_GET_BLOCKINFO, aid);
                    }
                    else
                    {
                        /* Unrelated block ID*/
                    }
#ifdef PRINTF_DEBUG
                    OS_Printf("c usr cmd tbmp 0x%x\n", current_user_cmd->target_bmp);
#endif
                }
                else
                {
                    /* Unrelated BlockDone*/
                }
                return i;              /* Establish command*/
            case WBT_CMD_REQ_GET_BLOCK:
            case WBT_CMD_RES_GET_BLOCK:
            case WBT_CMD_REQ_GET_BLOCKINFO:
            case WBT_CMD_RES_GET_BLOCKINFO:
                state = PARSE_BLOCK_SEQ_NO;
                break;
            default:
                /* Unknown command*/
                wbt_system_callback(cmd_param, WBT_CMD_RES_ERROR, aid,
                                    WBT_RESULT_ERROR_UNKNOWN_PACKET_COMMAND);
                break;
            }
            break;
        case PARSE_BLOCK_SEQ_NO:
            recv_data = token_read(recv_data, &(token->block_seq_no), sizeof(WBTBlockSeqNo));
            i += sizeof(WBTBlockSeqNo);
            switch (token->command)
            {
            case WBT_CMD_REQ_GET_BLOCK:
            case WBT_CMD_REQ_GET_BLOCKINFO:
                /* Leave the rest to the functions used for sending*/
#ifdef PRINTF_DEBUG_L1
                OS_Printf("get req Block from %d id = %d seq no = %d\n",
                          aid, token->block_id, token->block_seq_no);
#endif
                token->command_state = COMMAND_STATE_GET_REQUEST_FROM_PEER;     /* Establish command*/
                return i;
            case WBT_CMD_RES_GET_BLOCK:
            case WBT_CMD_RES_GET_BLOCKINFO:
                token->data_ptr = recv_data;
                /* If this is the requested block ID, save*/

                if ((current_user_cmd->command == WBT_CMD_REQ_GET_BLOCK) ||
                    (current_user_cmd->command == WBT_CMD_REQ_GET_BLOCKINFO))
                {
                    if (token->block_id == current_user_cmd->get.block_id)
                    {

                        peer_bmp = WBT_AidToAidbitmap(aid);
                        if (current_user_cmd->target_bmp & peer_bmp)
                        {
                            if (FALSE == wbt_packet_bitmap_check(pkt_bmp, token->block_seq_no))
                            {
#ifdef PRINTF_DEBUG_L1
                                OS_Printf("get Block save from %d id = %d seq no = %d\n",
                                          aid, token->block_id, token->block_seq_no);
#endif
                                //    OS_Printf("seq no = %d num_of_seq = %d\n", token->block_seq_no, pkt_bmp->num_of_seq);
                                if (TRUE ==
                                    wbt_save_block_seq(pkt_bmp, token->block_seq_no,
                                                       token->data_ptr))
                                {
                                    wbt_packet_bitmap_set(pkt_bmp, token->block_seq_no);
                                }
                                else
                                {
                                    /* Save failed*/
                                    wbt_system_callback(cmd_param, WBT_CMD_RES_ERROR, aid,
                                                        WBT_RESULT_ERROR_SAVE_FAILURE);
                                }
                            }
                            else
                            {
                                /* Check finished*/
                            }
                        }
                        else
                        {
                            /* Not the expected machine*/
                        }
                    }
                    else
                    {
                        /* Not the expected block ID*/
                    }
                }
                else
                {
                    /* Not inside of GetBlock*/
                }
                return i;              /* Establish command*/
            default:
                wbt_system_callback(cmd_param, WBT_CMD_RES_ERROR, aid,
                                    WBT_RESULT_ERROR_UNKNOWN_PACKET_COMMAND);
                break;
            }
            break;
        default:
            /* Is parser operation correct? */
            wbt_system_callback(cmd_param, WBT_CMD_RES_ERROR, aid, WBT_RESULT_ERROR_PARSE_INVALID);
            break;
        }
    }
    /* Receive buffer overflow*/
    wbt_system_callback(cmd_param, WBT_CMD_RES_ERROR, aid, WBT_RESULT_ERROR_RECV_BUFFER_OVERFLOW);
    return i;
}



/*---------------------------------------------------------------------------*
  Name:         WBTi_token_check_req_sync

  Description:  Checks whether a Synch request has arrived from the other station
                If it has come, makes response command.

  Arguments:    cmd_param - pointer to command parameter
                aid       - Other station AID
                buf       - buffer for creating send command
                size      - buffer size for creating send command

  Returns:      int       - send command size
 *---------------------------------------------------------------------------*/
int WBTi_token_check_req_sync(WBTCmdParam * cmd_param, int aid, const void *buf, int size)
{
#pragma unused(size)
    int     ret_size = 0;

    WBTRecvToken *recv_token = &(cmd_param->peer_param[aid].recv_token);
    WBTAidBitmap peer_bmp = WBT_AidToAidbitmap(aid);

    if (recv_token->command_state == COMMAND_STATE_GET_REQUEST_FROM_PEER)
    {
        if (recv_token->command == WBT_CMD_REQ_SYNC)
        {
            /* Send your own block list size*/
            ret_size = WBTi_MakeResSyncCommand(buf, peer_bmp, recv_token->peer_cmd_counter,
                                               (WBTBlockNumEntry)WBT_NumOfRegisteredBlock(),
                                               WBTi_get_my_data_packet_size(),
                                               WBTi_get_peer_data_packet_size());
            /* Discard command state after reading*/
            recv_token->command_state = COMMAND_STATE_NONE;
        }
    }
    return ret_size;
}

/*---------------------------------------------------------------------------*
  Name:         wbt_token_check_req_get_block_done

  Description:  Checks whether GetBlockDone request is received from the other station
                If it has come, makes response command.

  Arguments:    cmd_param - pointer to command parameter
                aid       - Other station AID
                buf       - buffer for creating send command
                size      - buffer size for creating send command

  Returns:      int       - send command size
 *---------------------------------------------------------------------------*/
int WBTi_token_check_req_get_block_done(WBTCmdParam * cmd_param, int aid, const void *buf, int size)
{
#pragma unused(size)
    int     ret_size = 0;
    WBTRecvToken *recv_token = &(cmd_param->peer_param[aid].recv_token);
    WBTAidBitmap peer_bmp = WBT_AidToAidbitmap(aid);

    if (recv_token->command_state == COMMAND_STATE_GET_REQUEST_FROM_PEER)
    {
        if (recv_token->command == WBT_CMD_REQ_GET_BLOCK_DONE)
        {
#ifdef PRINTF_DEBUG_L1
            OS_Printf("send BlockDone to %d id = %d\n", aid, recv_token->block_id);
#endif
            ret_size =
                WBTi_MakeResBlockDoneCommand(buf, peer_bmp, recv_token->peer_cmd_counter,
                                             recv_token->block_id);
            /* Discard command state after reading*/
            recv_token->command_state = COMMAND_STATE_NONE;
        }
    }
    return ret_size;
}


/*---------------------------------------------------------------------------*
  Name:         wbt_token_check_req_user_data

  Description:  Checks whether UserDatak request is received from the other station
                If it has come, makes response command.

  Arguments:    cmd_param - pointer to command parameter
                aid       - Other station AID
                buf       - buffer for creating send command
                size      - buffer size for creating send command

  Returns:      int       - send command size
 *---------------------------------------------------------------------------*/
int WBTi_token_check_req_user_data(WBTCmdParam * cmd_param, int aid, const void *buf, int size)
{
#pragma unused(size)
    int     ret_size = 0;

    WBTRecvToken *recv_token = &(cmd_param->peer_param[aid].recv_token);
    WBTAidBitmap peer_bmp = WBT_AidToAidbitmap(aid);

    if (recv_token->command_state == COMMAND_STATE_GET_REQUEST_FROM_PEER)
    {
        if (recv_token->command == WBT_CMD_REQ_USER_DATA)
        {
            ret_size = WBTi_MakeResUserDataCommand(buf, peer_bmp, recv_token->peer_cmd_counter);
            /* Discard command state after reading*/
            recv_token->command_state = COMMAND_STATE_NONE;
        }
    }
    return ret_size;
}



/*---------------------------------------------------------------------------*
  Name:         wbt_token_check_req_get_block

  Description:  Checks whether GetBlock is received from the other station
                If it has come, makes response command.

  Arguments:    cmd_param - pointer to command parameter
                aid       - Other station AID
                buf       - buffer for creating send command
                size      - buffer size for creating send command

  Returns:      int       - send command size
 *---------------------------------------------------------------------------*/
int WBTi_token_check_req_get_block(WBTCmdParam * cmd_param, int aid, const void *buf, int size)
{
#pragma unused(size)

    int     ret_size = 0;
    u8     *data_ptr;

    u16     target;
    WBTBlockSeqNo block_seq_no;

    WBTCommand *current_user_cmd = &(cmd_param->current_user_cmd);
    WBTCommand *system_cmd = &(cmd_param->system_cmd);
    WBTRecvToken *recv_token = &(cmd_param->peer_param[aid].recv_token);
    WBTPacketBitmap *pkt_bmp = &(cmd_param->peer_param[aid].pkt_bmp);
    WBTAidBitmap peer_bmp = WBT_AidToAidbitmap(aid);
    WBTBlockInfoList *list;


  /****** Request from peer (Respond immediately) **************/
    if (recv_token->command_state == COMMAND_STATE_GET_REQUEST_FROM_PEER)
    {
        if (WBT_GetOwnAid() == WBT_AID_PARENT)
        {
            target = 0xfffe;           //Do all first -> Clean up later
        }
        else
        {
            target = 0x1;              //
        }
        if (recv_token->command == WBT_CMD_REQ_GET_BLOCK)
        {
            list = WBTi_get_block_info_list(recv_token->block_id);
            if (list == NULL)
            {
                /* Doesn't have requested block -> For now, do nothing (You can return an error)*/
            }
            else
            {
                /* Send own block*/
                /* Don't send block that was just sent*/
                block_seq_no =
                    wbt_LastSendBlockSeqCheck(recv_token->block_id, recv_token->block_seq_no);
                if (block_seq_no == -1)
                {
                    /* Not the specified ID list -> For now, do nothing (You can return an error)*/
#ifdef PRINTF_DEBUG_L1
                    OS_Printf("error wbt_LastSendBlockSeqCheck block id = %d seq no = -1\n",
                              recv_token->block_id);
#endif
                }
                else
                {
                    if (list->block_type == WBT_BLOCK_LIST_TYPE_COMMON)
                    {
                        data_ptr = wbt_GetBlockDataPtr(recv_token->block_id, block_seq_no);
                        /* NULL can be returned*/
#ifdef PRINTF_DEBUG_L1
                        OS_Printf("send BlockData to %d id = %d seq no = %d pktsize %d\n", aid,
                                  recv_token->block_id, recv_token->block_seq_no,
                                  WBTi_get_my_data_packet_size());
#endif
                        /*  target = block_info->permission_bmp*/
                        ret_size =
                            WBTi_MakeResBlockCommand(buf, target, recv_token->peer_cmd_counter,
                                                     recv_token->block_id, block_seq_no, data_ptr,
                                                     WBTi_get_my_data_packet_size());
                    }
                    else if (list->block_type == WBT_BLOCK_LIST_TYPE_USER)
                    {
                        system_cmd->prepare_send_data.block_id = recv_token->block_id;
                        system_cmd->prepare_send_data.block_seq_no = block_seq_no;
                        system_cmd->prepare_send_data.own_packet_size =
                            WBTi_get_my_data_packet_size();
                        system_cmd->prepare_send_data.data_ptr = NULL;

                        wbt_system_callback(cmd_param, WBT_CMD_PREPARE_SEND_DATA, aid,
                                            WBT_RESULT_SUCCESS);
#ifdef PRINTF_DEBUG_L1
                        OS_Printf("peer req seq no  = %d seq no = %d dataptr = %p\n",
                                  recv_token->block_seq_no, block_seq_no,
                                  system_cmd->prepare_send_data.data_ptr);
#endif

                        if (system_cmd->prepare_send_data.data_ptr != NULL)
                        {
                            /* data_ptr == Return even when NULL*/
                            ret_size =
                                WBTi_MakeResBlockCommand(buf, target, recv_token->peer_cmd_counter,
                                                         system_cmd->prepare_send_data.block_id,
                                                         system_cmd->prepare_send_data.block_seq_no,
                                                         system_cmd->prepare_send_data.data_ptr,
                                                         system_cmd->prepare_send_data.
                                                         own_packet_size);
                        }
                        else
                        {
                            /* The outgoing data was not ready*/
                        }

                    }
                }
            }
            recv_token->command_state = COMMAND_STATE_NONE;
            /* Discard command state after reading*/
        }
    }
    return ret_size;
}


/*---------------------------------------------------------------------------*
  Name:         wbt_token_check_req_get_blockinfo

  Description:  Checks whether GetBlockInfo request has come from other station.
                If it has come, makes response command.

  Arguments:    cmd_param - pointer to command parameter
                aid       - Other station AID
                buf       - buffer for creating send command
                size      - buffer size for creating send command

  Returns:      int       - send command size
 *---------------------------------------------------------------------------*/
int WBTi_token_check_req_get_blockinfo(WBTCmdParam * cmd_param, int aid, const void *buf, int size)
{
#pragma unused(size)

    int     ret_size = 0;
    u8     *data_ptr;
    u16     target;
    WBTCommand *current_user_cmd = &(cmd_param->current_user_cmd);
    WBTRecvToken *recv_token = &(cmd_param->peer_param[aid].recv_token);

  /****** Request from peer (Respond immediately) **************/
    if (recv_token->command_state == COMMAND_STATE_GET_REQUEST_FROM_PEER)
    {
        if (WBT_GetOwnAid() == WBT_AID_PARENT)
        {
            target = 0xffff;           //Do all first -> Clean up later
        }
        else
        {
            target = 0x1;              //
        }
        if (recv_token->command == WBT_CMD_REQ_GET_BLOCKINFO)
        {
            data_ptr = wbt_GetBlockDataPtr(recv_token->block_id, recv_token->block_seq_no);
            /* If there is no list, return NULL*/
#ifdef PRINTF_DEBUG_L1
            OS_Printf("send BlockData to %d id = %d seq no = %d pktsize %d\n", aid,
                      recv_token->block_id, recv_token->block_seq_no,
                      WBTi_get_my_data_packet_size());
#endif
            if (data_ptr)
            {
                /*  target = block_info->permission_bmp*/
                ret_size = WBTi_MakeResBlockInfoCommand(buf, target, recv_token->peer_cmd_counter,
                                                        recv_token->block_id,
                                                        recv_token->block_seq_no, data_ptr,
                                                        WBTi_get_my_data_packet_size());
                /* Discard command state after reading*/
                recv_token->command_state = COMMAND_STATE_NONE;
            }
        }
    }
    return ret_size;
}

/*---------------------------------------------------------------------------*
  Name:         wbt_token_check_req_get_blockinfo2

  Description:  Checks whether GetBlockInfo request has come from other station.
                If it has come, makes response command.

  Arguments:    cmd_param - pointer to command parameter
                aid       - Other station AID
                buf       - buffer for creating send command
                size      - buffer size for creating send command

  Returns:      int       - send command size
 *---------------------------------------------------------------------------*/
int WBTi_token_check_req_get_blockinfo2(WBTCmdParam * cmd_param, int aid, const void *buf, int size)
{
#pragma unused(size)

    int     ret_size = 0;
    u8     *data_ptr;
    u16     target;
    WBTCommand *current_user_cmd = &(cmd_param->current_user_cmd);
    WBTRecvToken *recv_token = &(cmd_param->peer_param[aid].recv_token);

  /****** Request from peer (Respond immediately) **************/
    if (recv_token->command_state == COMMAND_STATE_GET_REQUEST_FROM_PEER)
    {
        if (WBT_GetOwnAid() == WBT_AID_PARENT)
        {
            target = 0xffff;           //Do all first -> Clean up later
        }
        else
        {
            target = 0x1;              //
        }
        if (recv_token->command == WBT_CMD_REQ_GET_BLOCKINFO)
        {
            data_ptr = wbt_GetBlockDataPtr(recv_token->block_id, recv_token->block_seq_no);
            /* If there is no list, return NULL*/
#ifdef PRINTF_DEBUG_L1
            OS_Printf("send BlockData to %d id = %d seq no = %d pktsize %d\n", aid,
                      recv_token->block_id, recv_token->block_seq_no,
                      WBTi_get_my_data_packet_size());
#endif
            /*  target = block_info->permission_bmp*/
            ret_size = WBTi_MakeResBlockInfoCommand(buf, target, recv_token->peer_cmd_counter,
                                                    recv_token->block_id, recv_token->block_seq_no,
                                                    data_ptr, WBTi_get_my_data_packet_size());
            /* Discard command state after reading*/
            recv_token->command_state = COMMAND_STATE_NONE;
        }
    }
    return ret_size;
}


/*---------------------------------------------------------------------------*
  Name:         wbt_current_user_request

  Description:  Checks whether a user command is currently executing
                and if so creates a response command.

  Arguments:    cmd_param - pointer to command parameter
                send_buf  - Buffer for creating send command
                size      - buffer size for creating send command

  Returns:      int       - send command size
 *---------------------------------------------------------------------------*/
int WBTi_current_user_request(WBTCmdParam * cmd_param, const void *send_buf, int size)
{
#pragma unused(size)

    int     aid;
    int     send_size = 0;
    u16     target = 0xffff;           //Send to all at first
    WBTCommand *current_user_cmd = &(cmd_param->current_user_cmd);
    WBTBlockSeqNo next_seq_no;
    WBTPacketBitmap *pkt_bmp;

    switch (current_user_cmd->command)
    {
    case WBT_CMD_REQ_SYNC:
#ifdef PRINTF_DEBUG
        OS_Printf("send ReqSync cur user cmd to 0x%04x cmd counter %d\n",
                  current_user_cmd->target_bmp, current_user_cmd->my_cmd_counter);
#endif
        send_size = WBTi_MakeReqSyncCommand(send_buf,
                                            current_user_cmd->target_bmp,
                                            current_user_cmd->my_cmd_counter,
                                            WBTi_get_my_data_packet_size(),
                                            WBTi_get_peer_data_packet_size());
        break;
    case WBT_CMD_REQ_GET_BLOCK:
    case WBT_CMD_REQ_GET_BLOCKINFO:
        for (aid = 0; aid < WBT_NUM_OF_AID; aid++)
        {
            /* Improve algorithm here*/
            /* Confirm that peer is the one requested*/
            if (current_user_cmd->target_bmp & WBT_AidToAidbitmap(aid))
            {
                pkt_bmp = &(cmd_param->peer_param[aid].pkt_bmp);
                next_seq_no = wbt_packet_bitmap_next_seq_no(pkt_bmp);

                /* If not received completely, request the next block; if done receiving, issue BlockDone*/
                if (next_seq_no == -1 /* receive complete*/ )
                {
#ifdef PRINTF_DEBUG
                    OS_Printf("send ReqBlockDone to %d 0x%04x\n", aid, WBT_AidToAidbitmap(aid));
#endif
                    send_size = WBTi_MakeReqBlockDoneCommand(send_buf, WBT_AidToAidbitmap(aid),
                                                             current_user_cmd->my_cmd_counter,
                                                             current_user_cmd->get.block_id);
                }
                else
                {
#ifdef PRINTF_DEBUG_L1
                    OS_Printf("send ReqBlock id=%d seq=%d\n", current_user_cmd->get.block_id,
                              next_seq_no);
#endif
                    target = current_user_cmd->target_bmp;
                    if (current_user_cmd->command == WBT_CMD_REQ_GET_BLOCK)
                    {
                        send_size =
                            WBTi_MakeReqBlockCommand(send_buf, target,
                                                     current_user_cmd->my_cmd_counter,
                                                     current_user_cmd->get.block_id, next_seq_no);
                    }
                    else
                    {
                        send_size =
                            WBTi_MakeReqBlockInfoCommand(send_buf, target,
                                                         current_user_cmd->my_cmd_counter,
                                                         current_user_cmd->get.block_id,
                                                         next_seq_no);
                    }
                }
                if (send_size)
                {
                    return send_size;
                }
            }
        }                              /* for( aid = 0 ; .. ; ..)*/
        break;

    case WBT_CMD_REQ_USER_DATA:
        send_size = WBTi_MakeReqUserDataCommand(send_buf,
                                                current_user_cmd->target_bmp,
                                                current_user_cmd->my_cmd_counter,
                                                current_user_cmd->user_data.data,
                                                current_user_cmd->user_data.size);
        break;

    case WBT_CMD_REQ_NONE:
        break;
    default:
#ifdef PRINTF_DEBUG
        OS_Printf("Unknown User Command:Error %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
        break;
    }
    return send_size;
}


/*---------------------------------------------------------------------------*
  Name:         wbt_next_user_request

  Description:  Checks whether there is a user command that should be executed next.
                and if so creates a response command.

  Arguments:    cmd_param - pointer to command parameter
                send_buf  - Buffer for creating send command
                size      - buffer size for creating send command

  Returns:      int       - send command size
 *---------------------------------------------------------------------------*/
int WBTi_next_user_request(WBTCmdParam * cmd_param, const void *send_buf, int size)
{
#pragma unused(size)
    int     i;
    int     send_size = 0;
    WBTAidBitmap target = 0xffff;      //Send to all at first
    WBTBlockSeqNo seq_no;

    WBTPacketBitmap *pkt_bmp;
    WBTCommand *next_user_cmd = &(cmd_param->next_user_cmd);
    WBTCommand *current_user_cmd = &(cmd_param->current_user_cmd);
    WBTCommandCounter my_cmd_counter;

    if (current_user_cmd->command != WBT_CMD_REQ_NONE)
    {
        return send_size;
    }

    if (next_user_cmd->command == WBT_CMD_REQ_NONE)
    {
        return send_size;
    }

    WBTi_increment_my_user_command_counter();
    my_cmd_counter = WBTi_get_my_user_command_counter();

    switch (next_user_cmd->command)
    {
    case WBT_CMD_REQ_SYNC:
        MI_CpuCopy32(next_user_cmd, current_user_cmd, sizeof(WBTCommand));
        current_user_cmd->my_cmd_counter = my_cmd_counter;

#ifdef PRINTF_DEBUG
        OS_Printf("send ReqSync next usr cmd to 0x%04x cmd counter %d\n",
                  current_user_cmd->target_bmp, current_user_cmd->my_cmd_counter);
#endif
        send_size = WBTi_MakeReqSyncCommand(send_buf,
                                            current_user_cmd->target_bmp,
                                            current_user_cmd->my_cmd_counter,
                                            WBTi_get_my_data_packet_size(),
                                            WBTi_get_peer_data_packet_size());
        break;

    case WBT_CMD_REQ_GET_BLOCKINFO:
        MI_CpuCopy32(next_user_cmd, current_user_cmd, sizeof(WBTCommand));
        current_user_cmd->my_cmd_counter = my_cmd_counter;

        for (i = 0; i < WBT_NUM_OF_AID; i++)
        {
            if ((((current_user_cmd->target_bmp) >> i) & 1))
            {
                pkt_bmp = &(cmd_param->peer_param[i].pkt_bmp);
                wbt_packet_bitmap_init(pkt_bmp, (int)current_user_cmd->get.recv_data_size,
                                       pkt_bmp->binfo_bitmap_buf,
                                       current_user_cmd->get.recv_buf_table.recv_buf[i]);
            }
        }
        seq_no = 0;
        if (next_user_cmd->command == WBT_CMD_REQ_GET_BLOCK)
        {
            send_size = WBTi_MakeReqBlockCommand(send_buf,
                                                 current_user_cmd->target_bmp,
                                                 current_user_cmd->my_cmd_counter,
                                                 current_user_cmd->get.block_id,
                                                 (WBTBlockSeqNo)seq_no);
        }
        else
        {
            send_size = WBTi_MakeReqBlockInfoCommand(send_buf,
                                                     current_user_cmd->target_bmp,
                                                     current_user_cmd->my_cmd_counter,
                                                     current_user_cmd->get.block_id,
                                                     (WBTBlockSeqNo)seq_no);
        }
        break;

    case WBT_CMD_REQ_GET_BLOCK:
        MI_CpuCopy32(next_user_cmd, current_user_cmd, sizeof(WBTCommand));
        current_user_cmd->my_cmd_counter = my_cmd_counter;
#ifdef PRINTF_DEBUG_L1
        OS_Printf("bitmap_init target=%08x\n", current_user_cmd->target_bmp);
#endif

        for (i = 0; i < WBT_NUM_OF_AID; i++)
        {
            if ((((current_user_cmd->target_bmp) >> i) & 1))
            {
                pkt_bmp = &(cmd_param->peer_param[i].pkt_bmp);
                wbt_packet_bitmap_init(pkt_bmp, (int)current_user_cmd->get.recv_data_size,
                                       current_user_cmd->get.pkt_bmp_table.packet_bitmap[i],
                                       current_user_cmd->get.recv_buf_table.recv_buf[i]);
            }
        }
        seq_no = 0;
        if (next_user_cmd->command == WBT_CMD_REQ_GET_BLOCK)
        {
            send_size = WBTi_MakeReqBlockCommand(send_buf,
                                                 current_user_cmd->target_bmp,
                                                 current_user_cmd->my_cmd_counter,
                                                 current_user_cmd->get.block_id,
                                                 (WBTBlockSeqNo)seq_no);
        }
        else
        {
            send_size = WBTi_MakeReqBlockInfoCommand(send_buf,
                                                     current_user_cmd->target_bmp,
                                                     current_user_cmd->my_cmd_counter,
                                                     current_user_cmd->get.block_id,
                                                     (WBTBlockSeqNo)seq_no);
        }
        break;

    case WBT_CMD_REQ_USER_DATA:
        MI_CpuCopy32(next_user_cmd, current_user_cmd, sizeof(WBTCommand));
        current_user_cmd->my_cmd_counter = my_cmd_counter;
        send_size = WBTi_MakeReqUserDataCommand(send_buf,
                                                current_user_cmd->target_bmp,
                                                current_user_cmd->my_cmd_counter,
                                                current_user_cmd->user_data.data,
                                                current_user_cmd->user_data.size);
        break;

    case WBT_CMD_REQ_NONE:
        break;

    default:
#ifdef PRINTF_DEBUG
        OS_Printf("Unknown User Command:Error %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
        break;
    }
    next_user_cmd->command = WBT_CMD_REQ_NONE;

    return send_size;;
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
