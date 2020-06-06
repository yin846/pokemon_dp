/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wbt_ui.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wbt_ui.c,v $
  Revision 1.22  01/18/2006 02:12:39  kitase_hirotake
  do-indent

  Revision 1.21  02/28/2005 05:26:34  yosizaki
  do-indent.

  Revision 1.20  02/18/2005 12:03:23  yosizaki
  fix around hidden warnings.

  Revision 1.19  11/30/2004 09:36:11  miya
  small fix.

  Revision 1.18  10/04/2004 08:39:09  miya
  add WBT_CancelCurrentCommand

  Revision 1.17  09/23/2004 07:49:45  miya
  Added init complete flag.

  Revision 1.16  09/21/2004 05:29:21  yosizaki
  adjust some variable types.

  Revision 1.15  09/14/2004 01:33:51  miya
  Added WBT_SetPacketSize function.

  Revision 1.14  09/09/2004 10:58:19  miya
  bug fixes

  Revision 1.13  09/07/2004 02:19:11  yosizaki
  change some interfaces. (const void*, and so on)

  Revision 1.12  09/04/2004 03:22:01  miya
  Corrected the UnregisterBlock function

  Revision 1.11  09/03/2004 11:42:06  miya
  delete RegisterUserDefinedBlock func.

  Revision 1.10  09/01/2004 03:59:45  miya
  Deleted PutBlock, RequestRegisterBlock, ResponseRegisterBlock, and Done commands.

  Revision 1.9  09/01/2004 00:16:38  miya
  Changed size of user data.

  Revision 1.8  08/28/2004 11:26:37  miya
  Added WBT_PutUserData function

  Revision 1.7  08/27/2004 07:07:52  miya
  Made changes to names, etc.

  Revision 1.6  08/26/2004 12:59:19  miya
  Made changes to names, etc.

  Revision 1.5  08/26/2004 03:18:54  miya
  Added the WBT_RegisterUserDefinedBlock function

  Revision 1.4  08/25/2004 12:28:43  miya
  Added RegBlock-related commands

  Revision 1.3  08/24/2004 02:40:04  miya
  comments added

  Revision 1.2  08/23/2004 12:39:28  miya
  Changed a part that could be problematic with the UI

  Revision 1.1  08/23/2004 09:46:09  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include	<nitro.h>
#include	<nitro/wbt.h>
#include	"wbt_internal.h"


/*****************************************************/
/*              AID bitmap*/
/*    +---------------------------------+            */
/* Hi |C15|C14|C13|  -------| C2| C1| P | Lo 16BIT*/
/*    +---------------------------------+            */
/*****************************************************/


/* ------------------------ Debug functions --------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         WBT_PrintBTList

  Description:  Displays the block information list with OS_Printf

  Arguments:    none

  Returns:      none
 *---------------------------------------------------------------------------*/
void WBT_PrintBTList(void)
{
    WBTBlockInfoList *list = WBTi_get_block_info_list_header();
    while (list != NULL)
    {
        OS_Printf("BTList id = %d\n", list->data_info.id);
        OS_Printf("  data size %d\n", list->data_info.block_size);
        OS_Printf("  uid %s\n", list->data_info.user_id);
        OS_Printf("  info ptr = %p\n", &(list->data_info));
        list = list->next;
    }
}

/* ------------------------ External functions --------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         WBT_AidbitmapToAid

  Description:  Converts the AID bitmap to AID (looks only the least significant bit)

  Arguments:    abmp - AID bitmap

  Returns:      int  - AID
 *---------------------------------------------------------------------------*/
int WBT_AidbitmapToAid(WBTAidBitmap abmp)
{
    int     aid = 0;
    for (aid = 0; aid < WBT_NUM_OF_AID; aid++)
    {
        if ((abmp >> aid) & 1)
        {
            return aid;
        }
    }
    return -1;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_CalcPacketbitmapSize

  Description:  Calculates the size of the buffer used for recording packet reception number
                (For a child, it must be called after synchronizing with the parent)

  Arguments:    block_size - Receiving block size

  Returns:      int - Buffer size for recording packet reception number (bytes)
 *---------------------------------------------------------------------------*/
int WBT_CalcPacketbitmapSize(int block_size)
{
    int     num_of_bits;
    int     num_of_words;
    WBTPacketSize pkt_size = WBTi_get_peer_data_packet_size();
    num_of_bits = ((block_size / pkt_size) + ((block_size % pkt_size) ? 1 : 0));
    num_of_words = div32(num_of_bits) + (mod32(num_of_bits) ? 1 : 0);

#ifdef PRINTF_DEBUG_L1
    OS_Printf("pkt bitmap size = %d words(%d byte)\n", num_of_words, sizeof(u32) * num_of_words);
#endif
    return (int)(sizeof(u32) * num_of_words);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_GetCurrentDownloadProgress

  Description:  Looks at the progress of the current block reception

  Arguments:    block_id       - Block ID of block that is being received
                aid            - AID of the send target
                *current_count - Packet account that completed the reception
                *total_count   - Total packet count

  Returns:      BOOL - Success/failure
 *---------------------------------------------------------------------------*/
BOOL WBT_GetCurrentDownloadProgress(u32 block_id, int aid, int *current_count, int *total_count)
{
    WBTCmdParam *cmd_param;
    WBTCommand *current_user_cmd;
    WBTPacketBitmap *pkt_bmp;

    cmd_param = WBTi_GetCommandParameter();
    pkt_bmp = &(cmd_param->peer_param[aid].pkt_bmp);
    current_user_cmd = &(cmd_param->current_user_cmd);

    if (current_user_cmd->command == WBT_CMD_REQ_GET_BLOCK)
    {
        if (current_user_cmd->get.block_id == block_id)
        {
            *total_count = pkt_bmp->num_of_seq;
            *current_count = pkt_bmp->current_seq_count;
            return TRUE;
        }
        else
        {
#ifdef PRINTF_DEBUG
            OS_Printf("invalid block id, current id = %d\n", current_user_cmd->get.block_id);
#endif
        }
    }
    else
    {
#ifdef PRINTF_DEBUG
        OS_Printf("cannot %s function. not downloading..\n", __FUNCTION__);
#endif
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_SetPacketSize

  Description:  Function that changes the sending/receiving packet size of the parent (use it to change after calling WBT_InitParent)

  Arguments:    send_packet_size - Send packet size
                recv_packet_size - Receive packet size

  Returns:      BOOL   - FALSE Size change failed
                         TRUE  Size change successful
 *---------------------------------------------------------------------------*/
BOOL WBT_SetPacketSize(int send_packet_size, int recv_packet_size)
{
    WBTCmdParam *cmd_param;
    WBTCommand *current_user_cmd;

    SDK_MIN_ASSERT(send_packet_size, WBT_PACKET_SIZE_MIN);
    SDK_MIN_ASSERT(recv_packet_size, WBT_PACKET_SIZE_MIN);

    cmd_param = WBTi_GetCommandParameter();
    current_user_cmd = &(cmd_param->current_user_cmd);
    if ((current_user_cmd->command == WBT_CMD_REQ_GET_BLOCK)
        || (current_user_cmd->command == WBT_CMD_REQ_GET_BLOCKINFO))
    {
        /* Cannot change packet size while receiving block*/
        return FALSE;
    }

    WBTi_set_my_data_packet_size(send_packet_size - WBT_PACKET_SIZE_MIN);
    WBTi_set_peer_data_packet_size(recv_packet_size - WBT_PACKET_SIZE_MIN);
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         WBT_NumOfRegisteredBlock

  Description:  Returns the number of the blocks that have been registered

  Arguments:    none.

  Returns:      WBTBlockNumEntry - Block numbers
 *---------------------------------------------------------------------------*/
int WBT_NumOfRegisteredBlock(void)
{
    int     count = 0;
    WBTBlockInfoList *list = WBTi_get_block_info_list_header();
    while (list != NULL)
    {
        count++;
        list = list->next;
    }
    return count;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_RegisterBlock

  Description:  Registers the sendable (deliverable) block

  Arguments:    block_info_list - Structure for registering block information
                block_id       - Block ID
                user_id         - User definition information
                data_ptr        - Pointer to the data (when it is NULL, the callback is used every time there is a request from the other party.
                                  Users can set the data pointer
                                  in the callback functions.
                data_size       - Data size
                permission_bmp  - Delivery permission bitmap ("plans" to permit with 0)

  Returns:      BOOL   - TRUE  Registration successful
                         FALSE block_id is already registered
 *---------------------------------------------------------------------------*/
BOOL
WBT_RegisterBlock(WBTBlockInfoList *block_info_list, WBTBlockId block_id, const void *user_id,
                  const void *data_ptr, int data_size, WBTAidBitmap permission_bmp)
{
    WBTBlockInfoList *list;

    OSIntrMode enabled;
    enabled = OS_DisableInterrupts();  /* cpsr irq */

    if (WBTi_get_block_info_list_header() == NULL)
    {
        WBTi_set_block_info_list_header(block_info_list);
    }
    else
    {
        list = WBTi_get_block_info_list_header();
        while (list->next != NULL)
        {
            list = list->next;
        }
        list->next = block_info_list;
    }

    if (data_ptr == NULL)
    {
        block_info_list->block_type = WBT_BLOCK_LIST_TYPE_USER;
    }
    else
    {
        block_info_list->data_ptr = (void *)data_ptr;
        block_info_list->block_type = WBT_BLOCK_LIST_TYPE_COMMON;
    }

    block_info_list->next = NULL;
    block_info_list->data_info.id = block_id;
    block_info_list->data_info.block_size = data_size;
    block_info_list->permission_bmp = permission_bmp;
    if (user_id != NULL)
    {
        MI_CpuCopy8(user_id, block_info_list->data_info.user_id, WBT_USER_ID_LEN);
    }
    else
    {
        MI_CpuClear8(block_info_list->data_info.user_id, WBT_USER_ID_LEN);
    }

    (void)OS_RestoreInterrupts(enabled);        /* cpsr irq */
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         WBT_UnregisterBlock

  Description:  Removes from the block delivery registration

  Arguments:    block_id - Block ID to stop the delivery

  Returns:      WBTBlockInfoList -Structure for registering block information
 *---------------------------------------------------------------------------*/
WBTBlockInfoList *WBT_UnregisterBlock(WBTBlockId block_id)
{
    WBTBlockInfoList *list, *pre_list;
    OSIntrMode enabled;
    enabled = OS_DisableInterrupts();  /* cpsr irq */

    pre_list = WBTi_get_block_info_list_header();
    if (pre_list == NULL)
    {
        (void)OS_RestoreInterrupts(enabled);    /* cpsr irq */
        return (WBTBlockInfoList *)NULL;
    }

    if (pre_list->data_info.id == block_id)
    {
        WBTi_set_block_info_list_header(pre_list->next);
        (void)OS_RestoreInterrupts(enabled);    /* cpsr irq */
        return pre_list;
    }

    while (NULL != (list = pre_list->next))
    {
        if (list->data_info.id == block_id)
        {
            pre_list->next = list->next;
            (void)OS_RestoreInterrupts(enabled);        /* cpsr irq */
            return list;
        }
        pre_list = pre_list->next;
    }

    (void)OS_RestoreInterrupts(enabled);        /* cpsr irq */
    return (WBTBlockInfoList *)NULL;
}

/*---------------------------------------------------------------------------*
  Name:         WBT_RequstSync

  Description:  Synchronizes with the other party (must be called when starting the block transfer)

  Arguments:    target   - The other party to synchronize (specify with AID bitmap)
                callback - Callback function that is called after the synchronization

  Returns:      BOOL   - FALSE: Previous command is running.
                         TRUE Command issue successful
 *---------------------------------------------------------------------------*/
BOOL WBT_RequestSync(WBTAidBitmap target, WBTCallback callback)
{
    WBTCommand uc;
    uc.command = WBT_CMD_REQ_SYNC;
    uc.target_bmp = target;
    uc.callback = callback;
    return WBTi_SetUserCommand(&uc);
}


/*---------------------------------------------------------------------------*
  Name:         WBT_GetBlockInfo

  Description:  Obtains the block information

  Arguments:    target           - The other party to synchronize (specify with AID bitmap)
                block_info_no    - Block information number
                block_info_table - Pointer table to the buffer that stores the obtained block information
                callback         - Callback function that is called after the synchronization

  Returns:      BOOL   - FALSE: Previous command is running.
                         TRUE Command issue successful
 *---------------------------------------------------------------------------*/
BOOL
WBT_GetBlockInfo(WBTAidBitmap target, int block_info_no, WBTBlockInfoTable *block_info_table,
                 WBTCallback callback)
{
    WBTCommand uc;


    uc.command = WBT_CMD_REQ_GET_BLOCKINFO;
    uc.target_bmp = target;
    uc.callback = callback;
    uc.get.block_id = (WBTBlockId)block_info_no;

    uc.get.recv_data_size = sizeof(WBTBlockInfo);

    MI_CpuCopy32(block_info_table, &(uc.get.recv_buf_table), sizeof(WBTRecvBufTable));

    return WBTi_SetUserCommand(&uc);
}


/*---------------------------------------------------------------------------*
  Name:         WBT_GetBlock

  Description:  Obtains the block
                
  Arguments:    target         - The other party to synchronize (specify with AID bitmap)
                block_id       - Block ID
                recv_buf_table - Pointer table to the buffer that stores the received block
                recv_size      - Receive block size
                p_bmp_table    - Pointer table to the buffer for recording packet reception number
                callback       - Callback function that is used after the block is obtained

  Returns:      BOOL   - FALSE: Previous command is running.
                         TRUE Command issue successful
 *---------------------------------------------------------------------------*/
BOOL
WBT_GetBlock(WBTAidBitmap target, WBTBlockId block_id, WBTRecvBufTable *recv_buf_table,
             u32 recv_size, WBTPacketBitmapTable *p_bmp_table, WBTCallback callback)
{
    WBTCommand uc;
    uc.command = WBT_CMD_REQ_GET_BLOCK;
    uc.target_bmp = target;
    uc.callback = callback;
    uc.get.block_id = block_id;

    uc.get.recv_data_size = recv_size;

    MI_CpuCopy32(recv_buf_table, &(uc.get.recv_buf_table), sizeof(WBTRecvBufTable));
    MI_CpuCopy32(p_bmp_table, &(uc.get.pkt_bmp_table), sizeof(WBTPacketBitmapTable));

    return WBTi_SetUserCommand(&uc);
}


/*---------------------------------------------------------------------------*
  Name:         WBT_PutUserData

  Description:  Sends the data that is smaller than 9 bytes to the other party
                
  Arguments:    target    - The other party (specify with AID bitmap)
                user_data - Pointer to the data you want to send
                size - The data size
                callback - Callback function

  Returns:      BOOL   - FALSE: Previous command is running.
                         TRUE Command issue successful
 *---------------------------------------------------------------------------*/
BOOL WBT_PutUserData(WBTAidBitmap target, const void *user_data, int size, WBTCallback callback)
{
    WBTCommand uc;

    if (size < 0 || WBT_SIZE_USER_DATA < size)
    {
        return FALSE;
    }
    uc.command = WBT_CMD_REQ_USER_DATA;
    uc.target_bmp = target;
    uc.callback = callback;
    MI_CpuCopy8(user_data, uc.user_data.data, (u32)size);
    uc.user_data.size = (u8)size;
    return WBTi_SetUserCommand(&uc);
}


/*---------------------------------------------------------------------------*
  Name:         WBT_CancelCurrentCommand

  Description:  Stops the WBT command that is being issued.
                
  Arguments:    target    - The other party (specify with AID bitmap)

  Returns:      BOOL   - FALSE No command to cancel
                         TRUE  Cancellation succeeded
 *---------------------------------------------------------------------------*/
BOOL WBT_CancelCurrentCommand(WBTAidBitmap cancel_target)
{
    OSIntrMode enabled;
    int     target_aid;
    BOOL    ret_val = FALSE;
    WBTAidBitmap peer_bmp;
    WBTCommand *current_user_cmd;

    enabled = OS_DisableInterrupts();
    if (wbt_initialize_flag == TRUE)
    {
        current_user_cmd = &(WBTi_GetCommandParameter()->current_user_cmd);

        if (current_user_cmd->command != WBT_CMD_REQ_NONE)
        {
            for (target_aid = WBT_AID_PARENT; target_aid < (WBT_AID_CHILD_LAST + 1); target_aid++)
            {
                peer_bmp = WBT_AidToAidbitmap(target_aid);

                if (current_user_cmd->target_bmp & peer_bmp & cancel_target)
                {
                    ret_val = TRUE;
                    current_user_cmd->target_bmp &= (~peer_bmp);
                    current_user_cmd->event = WBT_CMD_CANCEL;
                    current_user_cmd->peer_bmp = peer_bmp;

                    if (current_user_cmd->callback)
                    {
                        current_user_cmd->callback((void *)current_user_cmd);
                    }
                }
            }
            current_user_cmd->command = WBT_CMD_REQ_NONE;
        }
    }
    (void)OS_RestoreInterrupts(enabled);

    return ret_val;
}



/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
