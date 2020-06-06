/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wbt_command.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wbt_command.c,v $
  Revision 1.12  01/18/2006 02:12:38  kitase_hirotake
  do-indent

  Revision 1.11  02/28/2005 05:26:32  yosizaki
  do-indent.

  Revision 1.10  02/18/2005 12:03:23  yosizaki
  fix around hidden warnings.

  Revision 1.9  10/15/2004 05:16:47  miya
  Added process when NULL pointer

  Revision 1.8  09/01/2004 03:59:50  miya
  Deleted PutBlock, RequestRegisterBlock, ResponseRegisterBlock, and Done commands.

  Revision 1.7  08/28/2004 11:26:42  miya
  Added WBT_PutUserData function

  Revision 1.6  08/27/2004 07:07:52  miya
  Made changes to names, etc.

  Revision 1.5  08/26/2004 12:59:23  miya
  Made changes to names, etc.

  Revision 1.4  08/25/2004 12:28:48  miya
  Added RegBlock-related commands

  Revision 1.3  08/24/2004 08:14:07  miya
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
	Internal function definitions
 *---------------------------------------------------------------------------*/
static inline void *WBTi_MakeCommandHeader(const void *buf, WBTPacketCommand cmd,
                                           WBTAidBitmap target);
static inline int wbt_CommandLength(const void *buf_start, const void *buf_end);
static inline void *wbt_CommandPtr(const void *buf, int size);
static inline void *wbt_CommandCopy(const void *src, const void *dest, int size);

/* ------------------------ Internal functions --------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         wbt_CommandCopy

  Description:  Copy the data of the specified size from buffer

  Arguments:    src  - Copy source
                dest - Copy destination
                size - Copy size

  Returns:      u8*  Adds the copy size to the pointer of the copy source and returns
 *---------------------------------------------------------------------------*/
static inline void *wbt_CommandCopy(const void *src, const void *dest, int size)
{
    MI_CpuCopy8((void *)src, (void *)dest, (u32)size);
    return (void *)((u32)dest + (u32)size);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeCommandHeader

  Description:  Creates a command header

  Arguments:    arg_buf - buffer for creating command
                cmd     - command
                target  - Target for sending command (bitmap)

  Returns:      u8*     - The next address of the buffer that created command
 *---------------------------------------------------------------------------*/
static inline void *WBTi_MakeCommandHeader(const void *arg_buf, WBTPacketCommand cmd,
                                           WBTAidBitmap target)
{
    void   *buf = (void *)arg_buf;
    buf = wbt_CommandCopy(&cmd, buf, sizeof(WBTPacketCommand));
    buf = wbt_CommandCopy(&target, buf, sizeof(WBTAidBitmap));
    return buf;
}


/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeCommandLength

  Description:  Returns the length of the command

  Arguments:    buf_start - Beginning of the command
                buf_end   - End of the command

  Returns:      int - command size
 *---------------------------------------------------------------------------*/
static inline int wbt_CommandLength(const void *buf_start, const void *buf_end)
{
    return (int)((u32)(buf_end) - (u32)(buf_start));
}


/* ------------------------ External functions --------------------------------------*/


/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeReqWaitCommand

  Description:  Creates RequestWait command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                           

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int WBTi_MakeReqWaitCommand(const void *buf, u16 target, WBTCommandCounter counter)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_REQ_WAIT, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeReqSynCommand

  Description:  Creates RequesSync command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                                    
                my_packet_size   - size of the packet data sections of itself
                peer_packet_size - size of the packet data section of the other party

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeReqSyncCommand(const void *buf,
                        WBTAidBitmap target,
                        WBTCommandCounter counter,
                        WBTPacketSize my_packet_size, WBTPacketSize peer_packet_size)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_REQ_SYNC, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&my_packet_size, buf_end, sizeof(WBTPacketSize));
    buf_end = wbt_CommandCopy(&peer_packet_size, buf_end, sizeof(WBTPacketSize));
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeResSyncCommand

  Description:  Creates ResponseSync command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                                    
                num_entry        -  Number of blocks registered at the other party
                my_packet_size   - size of the packet data sections of itself
                peer_packet_size - size of the packet data section of the other party

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeResSyncCommand(const void *buf,
                        WBTAidBitmap target,
                        WBTCommandCounter counter,
                        WBTBlockNumEntry num_entry,
                        WBTPacketSize my_packet_size, WBTPacketSize peer_packet_size)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_RES_SYNC, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&num_entry, buf_end, sizeof(WBTBlockNumEntry));
    buf_end = wbt_CommandCopy(&my_packet_size, buf_end, sizeof(WBTPacketSize));
    buf_end = wbt_CommandCopy(&peer_packet_size, buf_end, sizeof(WBTPacketSize));
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeReqBlockCommand

  Description:  Creates RequestBlock command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                                
                block_id     - Block ID to request
                block_seq_no - Block sequence number to request

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeReqBlockCommand(const void *buf,
                         WBTAidBitmap target,
                         WBTCommandCounter counter, WBTBlockId block_id, WBTBlockSeqNo block_seq_no)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_REQ_GET_BLOCK, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&block_id, buf_end, sizeof(WBTBlockId));
    buf_end = wbt_CommandCopy(&block_seq_no, buf_end, sizeof(WBTBlockSeqNo));
    return wbt_CommandLength(buf, buf_end);
}


/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeResBlockCommand

  Description:  Creates  ResponseBlock command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                                
                block_id     - Block ID to return
                block_seq_no - Sequence number to return
                data         - Pointer to the data to return
                data_size    - Size of the data to return

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeResBlockCommand(const void *buf,
                         WBTAidBitmap target,
                         WBTCommandCounter counter,
                         WBTBlockId block_id, WBTBlockSeqNo block_seq_no, u8 *data, int data_size)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_RES_GET_BLOCK, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&block_id, buf_end, sizeof(WBTBlockId));
    buf_end = wbt_CommandCopy(&block_seq_no, buf_end, sizeof(WBTBlockSeqNo));

    if (data == NULL)
    {
        MI_CpuClear8(buf_end, (u32)data_size);
    }
    else
    {
        MI_CpuCopy8(data, buf_end, (u32)data_size);
    }
    buf_end = (u8 *)((u32)buf_end + (u32)data_size);
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeReqBlockInfoCommand

  Description:  Creates RequestBlockInfo command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                                 
                block_info_id - Block information ID to request
                block_seq_no  - Block information sequence number to request

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeReqBlockInfoCommand(const void *buf,
                             WBTAidBitmap target,
                             WBTCommandCounter counter,
                             WBTBlockId block_info_id, WBTBlockSeqNo block_seq_no)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_REQ_GET_BLOCKINFO, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&block_info_id, buf_end, sizeof(WBTBlockId));
    buf_end = wbt_CommandCopy(&block_seq_no, buf_end, sizeof(WBTBlockSeqNo));
    return wbt_CommandLength(buf, buf_end);
}


/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeResBlockInfoCommand

  Description:  Creates ResponseBlockInfo command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                                 
                block_info_id - Block information ID to return
                block_seq_no  - Block information sequence number to return
                data          - Pointer to the block information to return
                data_size     - Size of the block information data to return

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeResBlockInfoCommand(const void *buf,
                             WBTAidBitmap target,
                             WBTCommandCounter counter,
                             WBTBlockId block_info_id,
                             WBTBlockSeqNo block_seq_no, u8 *data, int data_size)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_RES_GET_BLOCKINFO, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&block_info_id, buf_end, sizeof(WBTBlockId));
    buf_end = wbt_CommandCopy(&block_seq_no, buf_end, sizeof(WBTBlockSeqNo));
    if (data == NULL)
    {
        MI_CpuClear8(buf_end, (u32)data_size);
    }
    else
    {
        MI_CpuCopy8(data, buf_end, (u32)data_size);
    }
    buf_end = (u8 *)((u32)buf_end + (u32)data_size);
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeReqBlockDoneCommand

  Description:  Creates RequestBlockDone command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                            
                block_id - Block ID to request termination

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeReqBlockDoneCommand(const void *buf,
                             WBTAidBitmap target, WBTCommandCounter counter, WBTBlockId block_id)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_REQ_GET_BLOCK_DONE, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&block_id, buf_end, sizeof(WBTBlockId));
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeResBlockDoneCommand

  Description:  Creates ResponseBlockDone command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                            
                block_id - Block ID to request termination

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeResBlockDoneCommand(const void *buf,
                             WBTAidBitmap target, WBTCommandCounter counter, WBTBlockId block_id)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_RES_GET_BLOCK_DONE, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&block_id, buf_end, sizeof(WBTBlockId));
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeReqUserDataCommand

  Description:  Creates RequestUserData command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                             
                user_data - Pointer to outgoing data
                size - The data size

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int
WBTi_MakeReqUserDataCommand(const void *buf,
                            WBTAidBitmap target,
                            WBTCommandCounter counter, void *user_data, u8 size)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_REQ_USER_DATA, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    buf_end = wbt_CommandCopy(&size, buf_end, sizeof(u8));
    buf_end = wbt_CommandCopy(user_data, buf_end, WBT_SIZE_USER_DATA);
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  Name:         WBTi_MakeResUserDataCommand

  Description:  Creates ResponseUserData command

  Arguments:    buf     - Buffer for creating command
                target  - Target for sending command (bitmap)
                counter   - Command counter (increments each time the user command is issued.)
                           

  Returns:      int     - Send size
 *---------------------------------------------------------------------------*/
int WBTi_MakeResUserDataCommand(const void *buf, WBTAidBitmap target, WBTCommandCounter counter)
{
    u8     *buf_end;
    buf_end = WBTi_MakeCommandHeader(buf, WBT_CMD_RES_USER_DATA, target);
    buf_end = wbt_CommandCopy(&counter, buf_end, sizeof(WBTCommandCounter));
    return wbt_CommandLength(buf, buf_end);
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
