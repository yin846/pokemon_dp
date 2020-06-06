/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WBT - libraries
  File:     wbt_data.c

  Copyright 2003-2005 Nintendo. All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law. They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wbt_data.c,v $
  Revision 1.7  2005/02/28 05:26:32  yosizaki
  do-indent.

  Revision 1.6  2004/09/23 07:49:45  miya
  Added an init complete flag

  Revision 1.5  2004/08/27 07:07:52  miya
  Changed names, etc.

  Revision 1.4  2004/08/26 03:18:57  miya
  Added the WBT_RegisterUserDefinedBlock function

  Revision 1.3  2004/08/24 02:40:04  miya
  Comment addition

  Revision 1.2  2004/08/23 12:39:28  miya
  Changed the section that may cause the UI problem

  Revision 1.1  2004/08/23 09:46:09  yosizaki
  (none)

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include	<nitro.h>
#include	<nitro/wbt.h>
#include	"wbt_internal.h"

/*---------------------------------------------------------------------------*
	Structure definitions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
	Internal function definitions
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
	Data definitions
 *---------------------------------------------------------------------------*/

static int my_aid = -1;                /* My AID */
static WBTPacketSize peer_data_packet_size;     /* Size of peer packet data */
static WBTPacketSize my_data_packet_size;       /* Size of own packet data */
static WBTBlockInfoList *list_header;  /* Block info list header */
static WBTCmdParam cmd_parameter;      /* Command parameters */
static WBTCommandCounter my_command_counter = 0;        /* User command issue command  */


/* ------------------------ Internal functions -------------------------------------- */
/*---------------------------------------------------------------------------*
  Name:         WBTi_get_my_user_command_counter

  Description:  Obtains the user command counter of self

  Arguments:    none.

  Returns:      WBTCommandCounter - User command counter
 *---------------------------------------------------------------------------*/
WBTCommandCounter WBTi_get_my_user_command_counter(void)
{
    return my_command_counter;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_increment_my_user_command_counter

  Description:  Increments the user command counter of self

  Arguments:    none.

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBTi_increment_my_user_command_counter(void)
{
    my_command_counter++;
    if (my_command_counter == 0)       /* 0 is not used */
    {
        my_command_counter++;
    }
}


/*---------------------------------------------------------------------------*
  Name:         WBTi_reset_my_user_command_counter

  Description:  Gets whether the self-bit is enabled in the AID bitmap

  Arguments:    target_bitmap - AID bitmap

  Returns:      BOOL - TRUE  The self-bit is enabled
                       FALSE The self-bit is disabled
 *---------------------------------------------------------------------------*/
void WBTi_reset_my_user_command_counter(void)
{
    my_command_counter = 0;
}

/*---------------------------------------------------------------------------*
  Name:         address_to_myself

  Description:  Gets whether the self-bit is enabled in the AID bitmap

  Arguments:    target_bitmap - AID bitmap

  Returns:      BOOL - TRUE  The self-bit is enabled
                       FALSE The self-bit is disabled
 *---------------------------------------------------------------------------*/
static inline BOOL address_to_myself(u16 target_bitmap)
{
#pragma unused(target_bitmap)

    if (my_aid == -1)
    {
        return FALSE;
    }
   /*****************************************************/
    /*    +---------------------------------+            */
    /* Hi |C15|C14|C13|  -------| C2| C1| P | Lo 16BIT   */
    /*    +---------------------------------+            */
   /*****************************************************/
    return ((target_bitmap & (1 << my_aid)) ? TRUE : FALSE);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_get_peer_data_packet_size

  Description:  Obtains the size of the packet data section of the other party

  Arguments:    none.

  Returns:      WBTPacketSize - Size of the packet data section of the other party
 *---------------------------------------------------------------------------*/
WBTPacketSize WBTi_get_peer_data_packet_size(void)
{
    return peer_data_packet_size;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_get_my_data_packet_size

  Description:  Obtains the size of the packet data section of self

  Arguments:    none.

  Returns:      WBTPacketSize -  Size of the packet data section of self
 *---------------------------------------------------------------------------*/
WBTPacketSize WBTi_get_my_data_packet_size(void)
{
    return my_data_packet_size;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_set_peer_data_packet_size

  Description:  Sets the size of the packet data section of the other party

  Arguments:    size - Size of the packet data section of the other party

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBTi_set_peer_data_packet_size(int size)
{
    peer_data_packet_size = (WBTPacketSize)size;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_set_my_data_packet_size

  Description:  Sets the size of the packet data section of self

  Arguments:    size - Size of the packet data section of self

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBTi_set_my_data_packet_size(int size)
{
    my_data_packet_size = (WBTPacketSize)size;
}


/*---------------------------------------------------------------------------*
  Name:         wbt_GetCommandParameter

  Description:  Obtains a pointer to the command parameter

  Arguments:    none.

  Returns:      WBTCmdParam - Pointer to the command parameter
 *---------------------------------------------------------------------------*/
WBTCmdParam *WBTi_GetCommandParameter(void)
{
    return &(cmd_parameter);
}



/*---------------------------------------------------------------------------*
  Name:         wbt_GetUserCommand

  Description:  Obtains a pointer to the next user command.

  Arguments:    none.

  Returns:      WBTCommand - Pointer to the next user command
 *---------------------------------------------------------------------------*/
WBTCommand *WBTi_GetUserCommand(void)
{
    return &(cmd_parameter.next_user_cmd);
}


/*---------------------------------------------------------------------------*
  Name:         WBTi_SetUserCommand

  Description:  Sets the next user command

  Arguments:    cmd - Pointer to the user command to set

  Returns:      none.
 *---------------------------------------------------------------------------*/
BOOL WBTi_SetUserCommand(WBTCommand *cmd)
{
    WBTCommand *user_command;

    OSIntrMode enabled;
    enabled = OS_DisableInterrupts();  /* cpsr irq */

    if (wbt_initialize_flag != TRUE)
    {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    user_command = WBTi_GetUserCommand();
    if (user_command == NULL || user_command->command != WBT_CMD_REQ_NONE)
    {
        (void)OS_RestoreInterrupts(enabled);
        return FALSE;
    }

    MI_CpuCopy32(cmd, &(cmd_parameter.next_user_cmd), sizeof(WBTCommand));
    (void)OS_RestoreInterrupts(enabled);

    return TRUE;


}


/* ------------------------ Eexternal functions -------------------------------------- */
/*---------------------------------------------------------------------------*
  Name:         WBTi_get_block_info_list_header

  Description:  Obtains the beginning of the block information list

  Arguments:    none.

  Returns:      WBTBlockInfoList - Block information list
 *---------------------------------------------------------------------------*/
WBTBlockInfoList *WBTi_get_block_info_list_header(void)
{
    return list_header;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_set_block_info_list_header

  Description:  Sets the block information to the beginning of the block information list

  Arguments:    lh - Block information list

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBTi_set_block_info_list_header(WBTBlockInfoList *lh)
{
    list_header = lh;
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_get_block_info_list

  Description:  Obtains the block information list

  Arguments:    block_id       - Block ID

  Returns:      WBTBlockInfoList - Block information list
 *---------------------------------------------------------------------------*/
WBTBlockInfoList *WBTi_get_block_info_list(WBTBlockId block_id)
{
    WBTBlockInfoList *list = WBTi_get_block_info_list_header();
    while (list != NULL)
    {
        if (list->data_info.id == block_id)
        {
            return list;
        }
        list = list->next;
    }
    return NULL;
}


/*---------------------------------------------------------------------------*
  Name:         WBT_SetOwnAid

  Description:  Sets AID of self

  Arguments:    aid - AID of self

  Returns:      none.
 *---------------------------------------------------------------------------*/
void WBT_SetOwnAid(int aid)
{
    my_aid = aid;
}


/*---------------------------------------------------------------------------*
  Name:         WBT_GetOwnAid

  Description:  Obtains AID of self

  Arguments:    none.

  Returns:      int - AID of self
 *---------------------------------------------------------------------------*/
int WBT_GetOwnAid(void)
{
    return my_aid;
}



/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/

