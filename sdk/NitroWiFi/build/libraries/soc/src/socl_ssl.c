/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_ssl.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_ssl.c,v $
Revision 1.4  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.3  08/24/2005 09:25:13  yasu
added SOCL_SocketIsInvalid

Revision 1.2  08/10/2005 09:32:00  seiki_masashi
added SSL support

Revision 1.1  08/06/2005 11:55:46  seiki_masashi
added SOCL_EnableSsl

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

static int  SOCLi_EnableSslCallBack(void* arg);
static int  SOCLi_ExecEnableSslCommand(SOCLSocket* socket, SOCLSslConnection* con);

/*---------------------------------------------------------------------------*
Name: SOCL_EnableSsl

Description: Sets the socket to SSL mode.
Must be called before binding.
  
Arguments: s    socket
con  pointer to the SOCLSslConnection structure
use NULL to return normally from SSL mode
  
Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :  
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
int SOCL_EnableSsl(int s, SOCLSslConnection* con)
{
    SOCLSocket*     socket = (SOCLSocket*)s;

    if (SOCL_SocketIsInvalid(socket))
    {
        return SOCL_EINVAL;     // non-regulated value
    }

    if (!SOCL_SocketIsTCP(socket))
    {
        return SOCL_EINVAL;     // SSL can only be used on TCP
    }

    if (!SOCL_SocketIsCreated(socket))
    {
        return SOCL_ENETRESET;  // not initialized
    }

    if (SOCL_SocketIsConnecting(socket))
    {
        return SOCL_EINVAL;     // begins connection process
    }

    return SOCLi_ExecEnableSslCommand(socket, con);
}

/*---------------------------------------------------------------------------*
Name: SOCLi_ExecEnableSslCommand

Description: Sets the socket to SSL mode.
  
Arguments: s    socket
con  pointer to the SOCLSslConnection structure
use NULL to return normally from SSL mode
  
Returns: positive number or 0: Succeeded
negative number: error
currently supported error values are as below
                    - :  
As other error values may be added, other negative values
are handled as general errors.
 *---------------------------------------------------------------------------*/
static int SOCLi_ExecEnableSslCommand(SOCLSocket* socket, SOCLSslConnection* con)
{
    SOCLiCommandPacket*     command;
    s32 result;

// Throws a command to recv_pipe, and runs CPS_SetSsl within
// the callback function. An error results when the command cannot be held.
    command = SOCLi_CreateCommandPacket(SOCLi_EnableSslCallBack, socket, OS_MESSAGE_BLOCK);
    if (NULL == command)
    {
        return SOCL_EMFILE;
    }

// parameter settings for enable ssl
    command->enable_ssl.connection = con;

// runs the command
    result = SOCLi_ExecCommandPacketInRecvPipe(socket, command);

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_EnableSslCallBack

Description: Callback for EnableSsl.
  
Arguments: arg   pointer to the command block
  
Returns: value passed to SOCLi_ExecCommand* (in BLOCK mode)
 *---------------------------------------------------------------------------*/
static int SOCLi_EnableSslCallBack(void* arg)
{
    SOCLiCommandEnableSsl*          cpacket = (SOCLiCommandEnableSsl*)arg;
    SOCLSocket*         socket = cpacket->h.socket;
    SOCLiSocketRecvCommandPipe*     recv_pipe;
    CPSSoc*     cpsoc;

    SDK_ASSERT(socket);
    recv_pipe = socket->recv_pipe;
    SDK_ASSERT(recv_pipe);
    cpsoc = (CPSSoc*)OSi_GetSpecificData(&recv_pipe->h.thread, OSi_SPECIFIC_CPS);
    SDK_ASSERT(cpsoc);

    OS_LockMutex(&recv_pipe->h.in_use); // BLOCKED
    {
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_SetSsl(%d)\n", (cpacket->connection != NULL) ? 1 : 0);
#endif

        if (cpacket->connection != NULL)
        {
            cpsoc->con = cpacket->connection;
            CPS_SetSsl(1);
            socket->flag_mode = SOCL_FLAGMODE_SSL;
        }
        else
        {
            socket->flag_mode = SOCL_FLAGMODE_TCP;
            CPS_SetSsl(0);
            cpsoc->con = NULL;
        }
    }

    OS_UnlockMutex(&recv_pipe->h.in_use);

    return SOCL_ESUCCESS;               // Succeeded
}
