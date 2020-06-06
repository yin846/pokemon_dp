/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: soc_cleanup.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_cleanup.c,v $
Revision 1.6  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.5  10/12/2005 01:45:29  yasu
Cancel DCF callback

Revision 1.4  09/28/2005 11:44:37  yasu
cleaned up the code
added SOCL_CalmDown()
as we don't yet have official support for the CPS library, covered in processing in SOC

Revision 1.3  09/27/2005 14:18:09  yasu
asynchronous support for SOC_Close

Revision 1.2  09/15/2005 12:51:01  yasu
DHCP Requested IP support

Revision 1.1  08/18/2005 13:17:59  yasu
moved cleanup API to separate file

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>
#include "../wcm/include/wcm_cpsif.h"

/*---------------------------------------------------------------------------*
Name: SOCL_Clearup

Description: Shuts down the SOC/CPS socket library.

Arguments: None

Returns: 0  ended normally
-1  not all socket services are stopped
 *---------------------------------------------------------------------------*/
int SOCL_Cleanup(void)
{
    int result;

    SDK_ASSERT(SOCLiConfigPtr);

// if the IP address is not saved, saves it
    if (SOCLiRequestedIP == 0)
    {
        SOCLiRequestedIP = CPSMyIp;
    }

// Closes all sockets.
// SOCLiUDPSendSocket is also closed by this process
    while (SOCL_EINPROGRESS == SOCL_CalmDown())
    {
        OS_Sleep(100);
    }

    result = SOCLi_CleanupCommandPacketQueue();

    if (result >= 0)
    {
#ifdef SDK_MY_DEBUG
        OS_TPrintf("CPS_Cleanup\n");
#endif

// blocks because DHCP Release will be performed
// to avoid this use the process in SOCL_CalmDown() to go forward
        CPS_Cleanup();

// Asynchronous cleanup measures
        CPS_SetScavengerCallback(NULL);

        if (!SOCLiConfigPtr->lan_buffer)    // releases memory allocated herein
        {
            SOCLi_Free(SOCLiCPSConfig.lan_buf);
        }

        SOCLiConfigPtr = NULL;
    }

    return result;
}

/*---------------------------------------------------------------------------*
Name: SOCL_CloseAll

Description: closes all user sockets as asynchronous
(leaving special sockets)
  
Arguments: None
  
Returns: SOCL_EINPROGRESS=-26 indicates close process is running
SOCL_ESUCCESS   = 0  indicates completion
 *---------------------------------------------------------------------------*/
int SOCL_CloseAll(void)
{
    SOCLSocket*     socket;
    OSIntrMode      enable;

    for (;;)
    {
// as the SocketList state may change during a close call
// always search from the header
        enable = OS_DisableInterrupts();
        for (socket = SOCLiSocketList; socket; socket = socket->next)
        {
// Is it a general socket for which close process has not yet begun?
            if ((int)socket != SOCLiUDPSendSocket && !SOCL_SocketIsWaitingClose(socket))
            {
                break;
            }
        }
        (void)OS_RestoreInterrupts(enable);

        if (!socket)
        {
            break;
        }

        (void)SOCL_Close((int)socket);
    }

// end if the socket list is empty or if they are only UDPSend sockets and the remove list is empty
    if (SOCLiSocketList == NULL || ((int)SOCLiSocketList == SOCLiUDPSendSocket && SOCLiSocketList->next == NULL))
    {
        if (SOCLiSocketListTrash == NULL)
        {
            return SOCL_ESUCCESS;
        }
    }

    return SOCL_EINPROGRESS;
}

/*---------------------------------------------------------------------------*
Name: SOCL_CalmDown

Description: Closes all sockets and supplemental sockets as asynchronous, and performs
cleanup preparations. CPS library stop process also begins.
  
Arguments: None
  
Returns: SOCL_EINPROGRESS=-26 indicates close process is running
SOCL_ESUCCESS   = 0  indicates completion
 *---------------------------------------------------------------------------*/
int SOCL_CalmDown(void)
{
    int result;

    if (SOCLiUDPSendSocket)
    {
        result = SOCL_CloseAll();

        if (result == SOCL_ESUCCESS)
        {
            (void)SOCL_Close(SOCLiUDPSendSocket);

            if (SOCL_IsClosed(SOCLiUDPSendSocket))
            {
                SOCLiUDPSendSocket = NULL;
            }

            result = SOCL_EINPROGRESS;
        }

        SOCLi_TrashSocket();
    }
    else
    {   // Asynchronous cleanup measures
        if (CPS_CalmDown())
        {
            WCM_SetRecvDCFCallback(NULL);
            result = SOCL_ESUCCESS;
        }
        else
        {
            result = SOCL_EINPROGRESS;
        }
    }

    return result;
}
