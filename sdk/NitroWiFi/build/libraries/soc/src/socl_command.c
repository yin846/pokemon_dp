/*---------------------------------------------------------------------------*
  Project:  NitroWiFi - SOC - libraries
  File:     socl_command.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: socl_command.c,v $
  Revision 1.10  05/17/2006 08:37:11  yasu
  Deleted the rescheduling process for thread inside SOCLi_CommandPacketHandler
  Integrated all thread switching into OS_ReadMessage

  Revision 1.9  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

  Revision 1.8  12/06/2005 11:53:13  yasu
  addressed issue in TCP BLOCK Write
  introduced a macro for determining BLOCK mode

  Revision 1.7  10/14/2005 06:51:35  yasu
  Corrected a problem where the initial value for the member 'response' was not being set when creating a command packet.

  Revision 1.6  10/14/2005 01:56:53  yasu
  Corrected a problem where close processing was incomplete whenever the command queue was full.

  Revision 1.5  09/27/2005 14:18:09  yasu
  asynchronous support for SOC_Close

  Revision 1.4  08/11/2005 07:02:00  yasu
  removed previous fixes

  Revision 1.3  08/11/2005 05:08:21  yasu
  revised functions with undefined returned values

  Revision 1.2  07/30/2005 22:30:14  yasu
  revised so demo would work

  Revision 1.1  07/30/2005 15:29:33  yasu
  separated command pipe variables into structures

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/soc.h>

//---------------------------------------------------------------------------*
// command packet queue
//---------------------------------------------------------------------------*
static OSMessage*       SOCLiCommandPackets;
static OSMessageQueue   SOCLiCommandPacketQueue;

/*---------------------------------------------------------------------------*
  Name:         SOCLi_StartupCommandPacketQueue

  Description:  initializes command packet queue

  Arguments:    cp_max_count number of packets
  
  Returns:       0  success
                -1  failed to block off memory
 *---------------------------------------------------------------------------*/
int SOCLi_StartupCommandPacketQueue(s32 cp_max_count)
{
    SOCLiCommandPacket*     cp_array;
    u32         cp_array_size;
    OSMessage*  cp_message_array;
    u32         cp_message_size;

    SDK_ASSERT(cp_max_count > 0);

    cp_message_size = MATH_ROUNDUP(cp_max_count * sizeof(OSMessage), 4);
    cp_array_size = MATH_ROUNDUP(cp_max_count * sizeof(SOCLiCommandPacket), 4);
    cp_message_array = (OSMessage*)SOCLi_Alloc(cp_array_size + cp_message_size);

    if (NULL == cp_message_array)
    {
        return -1;
    }

    // MessageQueue creation and
    // command packet addess are registered in the queue as messages
    OS_InitMessageQueue(&SOCLiCommandPacketQueue, cp_message_array, cp_max_count);

    cp_array = (SOCLiCommandPacket *) ((u32) cp_message_array + cp_message_size);

    while (cp_max_count > 0)
    {
        SOCLi_FreeCommandPacket(cp_array);

        cp_array++;
        cp_max_count--;
    }

    SOCLiCommandPackets = cp_message_array;
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CleanupCommandPacketQueue

  Description:  releases the command packet queue

  Arguments:    None

  Returns:       0  ended normally
                -1  not all of the command packet memory was recovered
 *---------------------------------------------------------------------------*/
int SOCLi_CleanupCommandPacketQueue(void)
{
    // if the queue is not full there are unrecovered packets
    if (!OS_IsMessageQueueFull(&SOCLiCommandPacketQueue))
    {
        return -1;
    }

    SOCLi_Free(SOCLiCommandPackets);
    SOCLiCommandPackets = NULL;
    return 0;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_AllocCommandPacket

  Description:  command packet allocation

  Arguments:    flag  command operation's block mode
                      OS_MESSAGE_NOBLOCK or OS_MESSAGE_BLOCK

  Returns:      pointer to the command packet
                NULL if the block mode is NOBLOCK and there is no command packet memory
                If the block mode is BLOCK and there is no command packet memory
                block until memory is available.
 *---------------------------------------------------------------------------*/
SOCLiCommandPacket* SOCLi_AllocCommandPacket(s32 flag)
{
    SOCLiCommandPacket*     cp;

    return(OS_ReceiveMessage(&SOCLiCommandPacketQueue, (void*) &cp, flag) ? cp : NULL);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CreateCommandPacket

  Description:  Creates the command packet.

  Arguments:    handler   command handler
                socket    target socket
                flag  command operation's block mode
                          OS_MESSAGE_NOBLOCK or OS_MESSAGE_BLOCK
                          There is no stipulation that the mode the socket is set to
                          will be used. At the most it is the command operation mode for this.
   
  Returns:      pointer to the command packet
                NULL if the block mode is NOBLOCK and there is no command packet memory
                If the block mode is BLOCK and there is no command packet memory
                block until memory is available.
 *---------------------------------------------------------------------------*/
SOCLiCommandPacket* SOCLi_CreateCommandPacket(SOCLiCommandHandler handler, SOCLSocket* socket, s32 flag)
{
    SOCLiCommandPacket*     cp;

    cp = SOCLi_AllocCommandPacket(flag);

    if (cp)
    {
        cp->h.handler = handler;
        cp->h.socket = socket;
        cp->h.response = NULL;
        cp->h.flag_mode = socket->flag_mode;
        cp->h.flag_block = (s8) flag;
    }

    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_FreeCommandPacket

  Description:  releases the command packet

  Arguments:    command   pointer to the command packet

  Returns:      None
 *---------------------------------------------------------------------------*/
void SOCLi_FreeCommandPacket(SOCLiCommandPacket* command)
{
    if (command)
    {
        (void)OS_SendMessage(&SOCLiCommandPacketQueue, command, OS_MESSAGE_NOBLOCK);
    }
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_GetCtrlPipe
  
  Description:  Gets the command control pipe

  Arguments:    socket		socket
  
  Returns:      Pointer to the command pipe used for socket control
 *---------------------------------------------------------------------------*/
SOCLiSocketCommandPipe* SOCLi_GetCtrlPipe(SOCLSocket* socket)
{
    // normally, Recv Pipe is the master
    // but for UDP Send, Send Pipe is the master, and it receives data.
    return(socket->recv_pipe) ? &socket->recv_pipe->h : &socket->send_pipe->h;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_SendCommandPacket
  
  Description:  sends the command packet

  Arguments:    pipe		the destination command pipe
                command         the command packet
  
  Returns:      when command->flag_block is NOBLOCK
                     SOCL_ESUCCESS=0 if there is send success
                         SOCL_ENOBUFS<0 if it fails
                         if it fails, recover the command
                when command->flag_block is BLOCK
                     always 0
 *---------------------------------------------------------------------------*/
int SOCLi_SendCommandPacket(SOCLiSocketCommandPipe* pipe, SOCLiCommandPacket* command)
{
    s32 result;
    s32 flag_block;

    SDK_ASSERT(pipe);

    flag_block = (command && !SOCL_FLAGISBLOCK(command->h.flag_block)) ? OS_MESSAGE_NOBLOCK : OS_MESSAGE_BLOCK;
    result = OS_SendMessage(&pipe->queue, command, flag_block);

    if (!result)
    {
        SOCLi_FreeCommandPacket(command);
    }

    return result ? SOCL_ESUCCESS : SOCL_ENOBUFS;
}

int SOCLi_SendCommandPacketToCtrlPipe(SOCLSocket* socket, SOCLiCommandPacket* command)
{
    SOCLiSocketCommandPipe*     pipe = SOCLi_GetCtrlPipe(socket);

    SDK_ASSERT(pipe);
    return SOCLi_SendCommandPacket(pipe, command);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ExecCommandPacket
  
  Description:  runs the command packet (sends and awaits process)

  Arguments:    pipe		the destination command pipe
                command         the command packet
  
  Returns:      when command->flag_block is NOBLOCK
                     SOCL_ESUCCESS=0 if there is send success
                         SOCL_ENOBUFS<0 if it fails
                         if it fails, recover the command
                when command->flag_block is BLOCK
                     returns the command return code
 *---------------------------------------------------------------------------*/
int SOCLi_ExecCommandPacket(SOCLiSocketCommandPipe* pipe, SOCLiCommandPacket* command)
{
    s32 result;

    SDK_ASSERT(pipe);
    SDK_ASSERT(command);

    if (command->h.flag_block == OS_MESSAGE_BLOCK)
    {
        OSMessage       response;
        OSMessageQueue  response_queue;

        OS_InitMessageQueue(&response_queue, &response, 1);
        command->h.response = &response_queue;

        (void)SOCLi_SendCommandPacket(pipe, command);
        (void)OS_ReceiveMessage(&response_queue, (void**) &result, OS_MESSAGE_BLOCK);
    }
    else
    {
        // command->h.flag_block == OS_MESSAGE_NOBLOCK/NORESULT
        command->h.response = NULL;
        result = SOCLi_SendCommandPacket(pipe, command);
    }

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_ExecCommandPacketInRecvPipe
                SOCLi_ExecCommandPacketInSendPipe
                SOCLi_ExecCommandPacketInCtrlPipe
  
  Description:  runs the command packet in RecvPipe/SendPipe (sends and awaits process)

  Arguments:    socket          socket
                command         the command packet
  
  Returns:      when command->flag_block is NOBLOCK
                     SOCL_ESUCCESS=0 if there is send success
                         SOCL_ENOBUFS<0 if it fails
                         if it fails, recover the command
                when command->flag_block is BLOCK
                     always 0
 *---------------------------------------------------------------------------*/
int SOCLi_ExecCommandPacketInRecvPipe(SOCLSocket* socket, SOCLiCommandPacket* command)
{
    SOCLiSocketRecvCommandPipe*     pipe = socket->recv_pipe;

    SDK_ASSERT(pipe);
    return SOCLi_ExecCommandPacket(&pipe->h, command);
}

int SOCLi_ExecCommandPacketInSendPipe(SOCLSocket* socket, SOCLiCommandPacket* command)
{
    SOCLiSocketSendCommandPipe*     pipe = socket->send_pipe;

    SDK_ASSERT(pipe);
    return SOCLi_ExecCommandPacket(&pipe->h, command);
}

int SOCLi_ExecCommandPacketInCtrlPipe(SOCLSocket* socket, SOCLiCommandPacket* command)
{
    SOCLiSocketCommandPipe*     pipe = SOCLi_GetCtrlPipe(socket);

    SDK_ASSERT(pipe);
    return SOCLi_ExecCommandPacket(pipe, command);
}

/*---------------------------------------------------------------------------*
  Name:         SOCLi_CommandPacketHandler
  
  Description:  thread entry for processing the command packet
                As it possible that socket structure members may have already been
                modofied by the following SOCL_* functions, in general this thread
                shall not reference the values within the socket structure. The necessary values are
                embedded in the command message for passing. However, this limitation
                does not apply to processes developed with this operation in mind.
                In addition, the priority for this thread must be higher than that of
                the calling SOCL function.
  
  Arguments:    arg             the waiting queue
  
  Returns:      None
 *---------------------------------------------------------------------------*/
void SOCLi_CommandPacketHandler(void* arg)
{
    SOCLiSocketCommandPipe*     cpipe = (SOCLiSocketCommandPipe*)arg;
    SOCLiCommandPacket*         command;
    OSIntrMode  enable;
    s32         result;

    //
    //  gets the command as a message, and runs the specified command
    //  if NULL is sent as a message, end
    //
    while (1)
    {
        // As it is possible during processing that OS_KillThread may stop a process thread,
        // the command will not be removed from the message queue until the completed message is sent.
        (void)OS_ReadMessage(&cpipe->queue, (void*) &command, OS_MESSAGE_BLOCK);

        if (command)
        {
            // Calls the callback.
            result = command->h.handler(command);

            // Distributes the message resulting from the process end.
            //   - deletes one command from the queue
            //   - writes the result to the socket structure
            //   - if needed, throws a competion message
            //   - returns a command to the command packet queue
            // All related processes have been compiled together in an atomic process.
            // Be aware that a hang might result unless all processes related to the internal
            // message are NOBLOCK.
            enable = OS_DisableInterrupts();
            (void)OS_DisableScheduler();
            (void)OS_ReceiveMessage(&cpipe->queue, (void*)NULL, OS_MESSAGE_NOBLOCK);

            if (command->h.socket)
            {
                command->h.socket->result = result;
            }

            if (command->h.response)
            {
                (void)OS_SendMessage(command->h.response, (void*)result, OS_MESSAGE_NOBLOCK);
            }

            SOCLi_FreeCommandPacket(command);   // NOBLOCK
            (void)OS_EnableScheduler();
            (void)OS_RestoreInterrupts(enable);

            // Depending on the above message send process in the next OS_ReadMessage(),
            // the execution rights will be transferred to other high-priority threads that have started up. 
        }
        else
        {
            break;
        }
    }

    return;
}
