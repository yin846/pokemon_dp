/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wbt_hook.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wbt_hook.c,v $
  Revision 1.15  01/18/2006 02:12:38  kitase_hirotake
  do-indent

  Revision 1.14  02/28/2005 05:26:33  yosizaki
  do-indent.

  Revision 1.13  10/15/2004 07:13:18  miya
  Dealt with get_blockinfo polling

  Revision 1.12  10/15/2004 05:15:15  miya
  Changed priority order of response to other station

  Revision 1.11  09/23/2004 07:49:45  miya
  Added init complete flag.

  Revision 1.10  09/13/2004 06:37:30  miya
  Changed the meaning of the arguments of WBT_InitParent

  Revision 1.9  09/07/2004 02:19:11  yosizaki
  change some interfaces. (const void*, and so on)

  Revision 1.8  09/01/2004 03:59:45  miya
  Deleted PutBlock, RequestRegisterBlock, ResponseRegisterBlock, and Done commands.

  Revision 1.7  08/28/2004 11:26:37  miya
  Added WBT_PutUserData function

  Revision 1.6  08/27/2004 07:07:52  miya
  Made changes to names, etc.

  Revision 1.5  08/26/2004 12:59:19  miya
  Made changes to names, etc.

  Revision 1.4  08/25/2004 12:32:26  miya
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

/*---------------------------------------------------------------------------*
	Variable definitions
 *---------------------------------------------------------------------------*/
BOOL    wbt_initialize_flag = FALSE;   /* Initialization complete flag*/

/*---------------------------------------------------------------------------*
	Internal function definitions
 *---------------------------------------------------------------------------*/
static int wbt_parent_make_send_data(const void *buf, int size);
static void wbt_parent_recv_data_parse(int aid, const void *buf, int size);
static void wbt_child_recv_data_parse(const void *buf, int size);
static int wbt_child_make_send_data(const void *buf, int size);



/* ------------------------ Internal functions --------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         wbt_parent_make_send_data

  Description:  Function (body) that creates send data for the parent

  Arguments:    buf  - Send buffer
                size - Send buffer size

  Returns:      int - Block transfer send size
 *---------------------------------------------------------------------------*/
static int wbt_parent_make_send_data(const void *buf, int size)
{
    int     send_size = 0;
    int     target_aid;
    static int last_target_aid = WBT_AID_CHILD_FIRST;
    WBTCmdParam *cmd_param = WBTi_GetCommandParameter();


#if 1
    target_aid = last_target_aid;
    do
    {
        target_aid++;
        if (target_aid > WBT_AID_CHILD_LAST)
        {
            target_aid = WBT_AID_CHILD_FIRST;
        }
        send_size = WBTi_token_check_req_user_data(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            last_target_aid = target_aid;
            return send_size;
        }
        send_size = WBTi_token_check_req_sync(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            last_target_aid = target_aid;
            return send_size;
        }
        send_size = WBTi_token_check_req_get_blockinfo(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            last_target_aid = target_aid;
            return send_size;
        }
        send_size = WBTi_token_check_req_get_block_done(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            last_target_aid = target_aid;
            return send_size;
        }
    }
    while (target_aid != last_target_aid);

#else

    for (target_aid = WBT_AID_CHILD_FIRST; target_aid < (WBT_AID_CHILD_LAST + 1); target_aid++)
    {
        send_size = WBTi_token_check_req_user_data(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            return send_size;
        }
    }


    for (target_aid = WBT_AID_CHILD_FIRST; target_aid < (WBT_AID_CHILD_LAST + 1); target_aid++)
    {
        send_size = WBTi_token_check_req_sync(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            return send_size;
        }
    }

    for (target_aid = WBT_AID_CHILD_FIRST; target_aid < (WBT_AID_CHILD_LAST + 1); target_aid++)
    {
        send_size = WBTi_token_check_req_get_blockinfo(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            return send_size;
        }
    }

    for (target_aid = WBT_AID_CHILD_FIRST; target_aid < (WBT_AID_CHILD_LAST + 1); target_aid++)
    {
        send_size = WBTi_token_check_req_get_block_done(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            return send_size;
        }
    }

#endif

    send_size = WBTi_current_user_request(cmd_param, buf, size);
    if (send_size)
    {
        return send_size;
    }

    send_size = WBTi_next_user_request(cmd_param, buf, size);
    if (send_size)
    {
        return send_size;
    }


    target_aid = last_target_aid;
    do
    {

        target_aid++;
        if (target_aid > WBT_AID_CHILD_LAST)
        {
            target_aid = WBT_AID_CHILD_FIRST;
        }

        send_size = WBTi_token_check_req_get_block(cmd_param, target_aid, buf, size);

        if (send_size)
        {
            last_target_aid = target_aid;
            return send_size;
        }

    }
    while (target_aid != last_target_aid);


    for (target_aid = WBT_AID_CHILD_FIRST; target_aid < (WBT_AID_CHILD_LAST + 1); target_aid++)
    {
        send_size = WBTi_token_check_req_get_blockinfo2(cmd_param, target_aid, buf, size);
        if (send_size)
        {
            return send_size;
        }
    }



    /* When there is nothing to do*/
    send_size = WBTi_MakeReqWaitCommand(buf, WBT_AIDBIT_CHILD_ALL /* Send to all at first*/ ,
                                        (WBTCommandCounter)0);

    return send_size;
}

/*---------------------------------------------------------------------------*
  Name:         wbt_parent_recv_data_parse

  Description:  Function (body) that analyzes the receive data for the parent

  Arguments:    aid       - AID of the other party that receives
                buf  - Receive buffer
                size - Receive buffer size

  Returns:      none.
 *---------------------------------------------------------------------------*/
static void wbt_parent_recv_data_parse(int aid, const void *buf, int size)
{
    int     ret_size;
    WBTCmdParam *cmd_param;

    if (size > 0)
    {

        cmd_param = WBTi_GetCommandParameter();
        ret_size = WBTi_get_token_from_recv_buffer(cmd_param, aid, buf, size);
        if (ret_size > size)
        {
#ifdef PRINTF_DEBUG
            OS_Printf("Error %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
        }
    }
    else
    {
#ifdef PRINTF_DEBUG
        OS_Printf("Error %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
        return;
    }
}

/*---------------------------------------------------------------------------*
  Name:         wbt_child_recv_data_parse

  Description:  Function (body) that analyzes the receive data for a child

  Arguments:    buf  - Receive buffer
                size - Receive buffer size

  Returns:      none.
 *---------------------------------------------------------------------------*/
static void wbt_child_recv_data_parse(const void *buf, int size)
{
    int     i;
    WBTCmdParam *cmd_param = WBTi_GetCommandParameter();
    i = WBTi_get_token_from_recv_buffer(cmd_param, WBT_AID_PARENT, buf, size);
}


/*---------------------------------------------------------------------------*
  Name:         wbt_child_make_send_data

  Description:  Function (body) that creates send data of a child

  Arguments:    buf  - Send buffer
                size - Send buffer size

  Returns:      int - Block transfer send size
 *---------------------------------------------------------------------------*/
static int wbt_child_make_send_data(const void *buf, int size)
{
#pragma unused(size)
    int     send_size = 0;
    u16     target = 0xffff;
    WBTCmdParam *cmd_param = WBTi_GetCommandParameter();

    /* Must check size*/

    send_size = WBTi_token_check_req_user_data(cmd_param, WBT_AID_PARENT, buf, size);
    if (send_size)
    {
        return send_size;
    }

    send_size = WBTi_token_check_req_sync(cmd_param, WBT_AID_PARENT, buf, size);
    if (send_size)
    {
        return send_size;
    }

    send_size = WBTi_token_check_req_get_blockinfo(cmd_param, WBT_AID_PARENT, buf, size);
    if (send_size)
    {
        return send_size;
    }

    send_size = WBTi_token_check_req_get_block_done(cmd_param, WBT_AID_PARENT, buf, size);
    if (send_size)
    {
        return send_size;
    }

    send_size = WBTi_token_check_req_get_block(cmd_param, WBT_AID_PARENT, buf, size);
    if (send_size)
    {
        return send_size;
    }

    send_size = WBTi_token_check_req_get_blockinfo2(cmd_param, WBT_AID_PARENT, buf, size);
    if (send_size)
    {
        return send_size;
    }

    send_size = WBTi_current_user_request(cmd_param, buf, size);
    if (send_size)
    {
        return send_size;
    }

    /* If there is no command from the parent machine, look for user command*/
    send_size = WBTi_next_user_request(cmd_param, buf, size);
    if (send_size)
    {
        return send_size;
    }


    /* When there is nothing to do*/
    send_size = WBTi_MakeReqWaitCommand(buf, target, (WBTCommandCounter)0);

    return send_size;
}



/* ------------------------ External functions --------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         WBT_InitParent

  Description:  Block transfer initialization function for the parent

  Arguments:    send_packet_size - Send packet size
                recv_packet_size - Receive packet size
                callback - Callback function that is used when the requests from the other party is generated

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_InitParent(int send_packet_size, int recv_packet_size, WBTCallback callback)
{
    WBTCommand cmd;
    WBTCmdParam *cmd_param;
    int     i;
    OSIntrMode enabled;

    SDK_MIN_ASSERT(send_packet_size, WBT_PACKET_SIZE_MIN);
    SDK_MIN_ASSERT(recv_packet_size, WBT_PACKET_SIZE_MIN);

    enabled = OS_DisableInterrupts();

    if (wbt_initialize_flag == TRUE)
    {
        (void)OS_RestoreInterrupts(enabled);
        return;
    }
    wbt_initialize_flag = TRUE;

    WBT_SetOwnAid(WBT_AID_PARENT);
    WBTi_set_block_info_list_header(NULL);

    cmd_param = WBTi_GetCommandParameter();
    cmd_param->current_user_cmd.command = WBT_CMD_REQ_NONE;
    cmd_param->current_user_cmd.target_bmp = 0;
    cmd_param->current_user_cmd.peer_bmp = 0;
    cmd_param->current_user_cmd.callback = NULL;

    cmd_param->system_cmd.command = WBT_CMD_REQ_NONE;
    cmd_param->system_cmd.target_bmp = 0;
    cmd_param->system_cmd.peer_bmp = 0;
    cmd_param->system_cmd.callback = callback;

    for (i = 0; i < WBT_NUM_OF_AID; i++)
    {
        cmd_param->peer_param[i].recv_token.last_peer_cmd_counter = 0;
    }


    cmd.command = WBT_CMD_REQ_NONE;
    (void)WBTi_SetUserCommand(&cmd);
    WBTi_reset_my_user_command_counter();
    WBTi_set_my_data_packet_size(send_packet_size - WBT_PACKET_SIZE_MIN);
    WBTi_set_peer_data_packet_size(recv_packet_size - WBT_PACKET_SIZE_MIN);
    (void)OS_RestoreInterrupts(enabled);
}

/*---------------------------------------------------------------------------*
  Name:         WBT_Init_Child

  Description:  Block transfer initialization function for a child

  Arguments:    callback - Callback function that is used when the requests from the other party is generated

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_InitChild(WBTCallback callback)
{
    WBTCommand cmd;
    WBTCmdParam *cmd_param;
    int     i;
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();

    if (wbt_initialize_flag == TRUE)
    {
        (void)OS_RestoreInterrupts(enabled);
        return;
    }
    wbt_initialize_flag = TRUE;

    WBT_SetOwnAid(-1);

    WBTi_set_block_info_list_header(NULL);
    cmd_param = WBTi_GetCommandParameter();
    cmd_param->current_user_cmd.command = WBT_CMD_REQ_NONE;
    cmd_param->current_user_cmd.target_bmp = 0;
    cmd_param->current_user_cmd.peer_bmp = 0;
    cmd_param->current_user_cmd.callback = NULL;

    cmd_param->system_cmd.command = WBT_CMD_REQ_NONE;
    cmd_param->system_cmd.target_bmp = 0;
    cmd_param->system_cmd.peer_bmp = 0;
    cmd_param->system_cmd.callback = callback;

    for (i = 0; i < WBT_NUM_OF_AID; i++)
    {
        cmd_param->peer_param[i].recv_token.last_peer_cmd_counter = 0;
    }

    cmd.command = WBT_CMD_REQ_NONE;
    (void)WBTi_SetUserCommand(&cmd);
    WBTi_reset_my_user_command_counter();
    WBTi_set_my_data_packet_size(0);
    WBTi_set_peer_data_packet_size(0);
    (void)OS_RestoreInterrupts(enabled);
}



/*---------------------------------------------------------------------------*
  Name:         WBT_End

  Description:  Parent and child common block transfer end function

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void WBT_End(void)
{
    WBTCommand cmd;
    WBTCmdParam *cmd_param;
    int     i;
    OSIntrMode enabled;

    enabled = OS_DisableInterrupts();  /* cpsr irq */

    if (wbt_initialize_flag == FALSE)
    {
        (void)OS_RestoreInterrupts(enabled);    /* cpsr irq */
        return;
    }
    wbt_initialize_flag = FALSE;

#if 1
    WBT_SetOwnAid(-1);
    WBTi_set_block_info_list_header(NULL);

    cmd_param = WBTi_GetCommandParameter();
    cmd_param->current_user_cmd.command = WBT_CMD_REQ_NONE;
    cmd_param->current_user_cmd.target_bmp = 0;
    cmd_param->current_user_cmd.peer_bmp = 0;
    cmd_param->current_user_cmd.callback = NULL;

    cmd_param->system_cmd.command = WBT_CMD_REQ_NONE;
    cmd_param->system_cmd.target_bmp = 0;
    cmd_param->system_cmd.peer_bmp = 0;
    cmd_param->system_cmd.callback = NULL;

    for (i = 0; i < WBT_NUM_OF_AID; i++)
    {
        cmd_param->peer_param[i].recv_token.last_peer_cmd_counter = 0;
    }

    cmd.command = WBT_CMD_REQ_NONE;
    (void)WBTi_SetUserCommand(&cmd);
    WBTi_reset_my_user_command_counter();
    WBTi_set_my_data_packet_size(-1);
    WBTi_set_peer_data_packet_size(-1);
#endif

    (void)OS_RestoreInterrupts(enabled);        /* cpsr irq */
}



/*---------------------------------------------------------------------------*
  Name:         WBT_MpParentSendHook

  Description:  Function that creates send data for the parent

  Arguments:    sendbuf   - Send buffer
                send_size - Send buffer size

  Returns:      int - Block transfer send size
 *---------------------------------------------------------------------------*/
int WBT_MpParentSendHook(void *sendbuf, int send_size)
{
    u16     command_size = 0;

    SDK_ASSERT(wbt_initialize_flag);

    if ((send_size > 0) && (sendbuf != NULL))
    {
        command_size = (u16)wbt_parent_make_send_data(sendbuf, send_size);
    }
    return command_size;
}


/*---------------------------------------------------------------------------*
  Name:         WBT_MpParentRecvHook

  Description:  Function that analyzes the receive data of the parent

  Arguments:    recv_buf  - Receive buffer
                recv_size - Receive buffer size
                aid       - AID of the other party that receives

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_MpParentRecvHook(const void *recv_buf, int recv_size, int aid)
{
    SDK_ASSERT(wbt_initialize_flag);

    if ((recv_buf == NULL) || (recv_size == 0) || (recv_size == 0xffff))
    {
        /* Receive failed; Stop*/
    }
    else
    {
        wbt_parent_recv_data_parse(aid, recv_buf, recv_size);
    }
}



/*---------------------------------------------------------------------------*
  Name:         WBT_MpChildSendHook

  Description:  Function that creates send data for a child

  Arguments:    sendbuf   - Send buffer
                send_size - Send buffer size

  Returns:      int - Block transfer send size
 *---------------------------------------------------------------------------*/
int WBT_MpChildSendHook(void *sendbuf, int send_size)
{
    int     command_size;

    SDK_ASSERT(wbt_initialize_flag);

    if ((send_size > 0) && (sendbuf != NULL))
    {
        command_size = wbt_child_make_send_data(sendbuf, send_size);
    }
    return command_size;
}


/*---------------------------------------------------------------------------*
  Name:         WBT_MpChildRecvHook

  Description:  Function that analyzes the receive data for the child

  Arguments:    recv_buf  - Receive buffer
                recv_size - Receive buffer size

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_MpChildRecvHook(const void *recv_buf, int recv_size)
{
    SDK_ASSERT(wbt_initialize_flag);

    if ((recv_buf == NULL) || (recv_size == 0) || (recv_size == 0xffff))
    {
        /* Receive failed; Stop*/
    }
    else
    {
        wbt_child_recv_data_parse(recv_buf, recv_size);
    }
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
