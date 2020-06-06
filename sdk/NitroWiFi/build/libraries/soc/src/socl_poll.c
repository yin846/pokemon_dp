/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_poll.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_poll.c,v $
Revision 1.12  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.11  10/04/2005 10:31:40  yasu
Revised code so that data was considered readable as long as it remained in the receive buffer when FIN was received from a remote host in SOC_GetStatus.
  where there is data, revised to recognize as READABLE

Revision 1.10  09/01/2005 06:42:55  yasu
consolidated processes determining whether a socket is active

Revision 1.9  08/26/2005 04:02:25  yasu
warning controls

Revision 1.8  08/26/2005 03:45:25  yasu
revisions made in conjunction with changes to the SOCL_GetStatus UDP receiving system

Revision 1.7  08/24/2005 09:25:13  yasu
added SOCL_SocketIsInvalid

Revision 1.6  08/18/2005 04:27:15  yasu
attention to lines ending with the yen mark

Revision 1.5  08/17/2005 04:10:28  yasu
added SOC_Poll functionality

Revision 1.4  08/12/2005 09:51:10  yasu
strictly limited returned values for IsWritable function to BOOL

Revision 1.3  08/01/2005 07:34:28  yasu
added check for connected status to IsWritable

Revision 1.2  07/29/2005 01:49:02  yasu
compiler strategy for English version

Revision 1.1  07/28/2005 11:15:17  yasu
implemented SOC_Poll

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>
#include <nitroWiFi/soc.h>

/*---------------------------------------------------------------------------*
Name: SOCL_GetStatus

Description: Checks the status of the specified socket.

Arguments: s       socket indicator
  
Returns: the following are indicates as OP options
SOC_POLLNVAL		0x80	socket is invalid
SOC_POLLHUP		0x40	in connection process or else not connected
SOC_POLLERR		0x20	Error occurred
SOC_POLLWRNORM		0x08	writeable
SOC_POLLRDNORM		0x01	readable
 *---------------------------------------------------------------------------*/
int SOCL_GetStatus(int s)
{
    SOCLSocket*     socket = (SOCLSocket*)s;
    int         result = 0;
    OSIntrMode  enable;

    if (SOCL_SocketIsInvalid(socket))
    {
        result |= SOC_POLLNVAL;
    }
    else
    {
        if (SOCL_SocketIsError(socket))
        {
            result |= SOC_POLLERR;
        }

        if (SOCL_SocketIsUDP(socket) || SOCL_SocketIsConnected(socket))
        {
            enable = OS_DisableInterrupts();
            if (SOCLi_GetReadBufferOccpiedSize(socket) > 0)
            {
                result |= SOC_POLLRDNORM;
            }

            if (SOCLi_GetWriteBufferFreeSize(socket) > 0)
            {
                result |= SOC_POLLWRNORM;
            }
            (void)OS_RestoreInterrupts(enable);
        }

        if (SOCL_SocketIsTCP(socket))
        {
// confirms whether in connection status and maintains flag
// Disconnect status will not result as long as status is READABLE
            if (SOCL_SocketIsConnected(socket) && socket->cps_socket.state != CPS_STT_ESTABLISHED &&
                !(result & SOC_POLLRDNORM))
            {
                socket->state &= ~(SOCL_STATUS_CONNECTING | SOCL_STATUS_CONNECTED);
            }

            if (!SOCL_SocketIsConnecting(socket) && !SOCL_SocketIsConnected(socket))
            {
                result |= SOC_POLLHUP;
            }
        }

    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_GetReadBufferOccpiedSize

Description: Checks the size of data received at the specified socket.

Arguments: socket          socket
  
Returns: size
 *---------------------------------------------------------------------------*/
s32 SOCLi_GetReadBufferOccpiedSize(SOCLSocket* socket)
{
    SOCLiSocketRecvCommandPipe*     recv_pipe = socket->recv_pipe;
    s32 size = 0;

    if (recv_pipe)
    {
        if (SOCL_SocketIsUDP(socket))
        {
            SOCLiSocketUdpData*     udpout = recv_pipe->udpdata.out;
            size = (s32) (udpout ? udpout->size : 0);
        }
        else if (SOCL_SocketIsTCP(socket))
        {
            size = (s32) socket->cps_socket.rcvbufp - (s32) recv_pipe->consumed;
        }
    }

    return size;
}
