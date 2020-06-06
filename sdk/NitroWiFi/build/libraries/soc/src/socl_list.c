/*---------------------------------------------------------------------------*
Project: NitroWiFi - SOC - libraries
File: socl_list.c

Copyright 2005.2006 Nintendo.  All rights reserved.

These coded instructions, statements, and computer programs contain
proprietary information of Nintendo of America Inc. and/or Nintendo
Company Ltd., and are protected by Federal copyright law.  They may
not be disclosed to third parties or copied or duplicated in any form,
in whole or in part, without the prior written consent of Nintendo.

$Log: socl_list.c,v $
Revision 1.5  03/10/2006 09:22:19  kitase_hirotake
  INDENT SOURCE

Revision 1.4  09/28/2005 11:44:37  yasu
cleaned up the code
added SOCL_CalmDown()
as we don't yet have official support for the CPS library, covered in processing in SOC

Revision 1.3  09/27/2005 14:18:09  yasu
asynchronous support for SOC_Close

Revision 1.2  09/01/2005 06:42:55  yasu
consolidated processes determining whether a socket is active

Revision 1.1  08/18/2005 13:18:49  yasu
added cleanup process through socket link list

$NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/socl.h>

SOCLSocket*         SOCLiSocketList = NULL;
SOCLSocket*         SOCLiSocketListTrash = NULL;

static SOCLSocket **     SOCLi_SocketGetNextPtr(SOCLSocket ** start, SOCLSocket* socket);
static void SOCLi_SocketRegisterList(SOCLSocket ** next, SOCLSocket* socket);
static void SOCLi_SocketUnregisterList(SOCLSocket ** next, SOCLSocket* socket);

/*---------------------------------------------------------------------------*
Name: SOCLi_SocketRegister

Description: registers the socket to the list

Arguments: socket          socket

Returns: None
 *---------------------------------------------------------------------------*/
void SOCLi_SocketRegister(SOCLSocket* socket)
{
    SOCLi_SocketRegisterList(&SOCLiSocketList, socket);
}

static void SOCLi_SocketRegisterList(SOCLSocket ** next, SOCLSocket* socket)
{
    socket->next = *next;
    *next = socket;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_SocketRegisterTrash

Description: registers the socket to the remove list

Arguments: socket          socket

Returns: None
 *---------------------------------------------------------------------------*/
void SOCLi_SocketRegisterTrash(SOCLSocket* socket)
{
    SOCLi_SocketRegisterList(&SOCLiSocketListTrash, socket);
}

/*---------------------------------------------------------------------------*
Name: SOCLi_SocketUnregister

Description: deletes the socket from the list

Arguments: socket          socket

Returns: None
 *---------------------------------------------------------------------------*/
void SOCLi_SocketUnregister(SOCLSocket* socket)
{
    SOCLi_SocketUnregisterList(&SOCLiSocketList, socket);
}

static void SOCLi_SocketUnregisterList(SOCLSocket ** next, SOCLSocket* socket)
{
    next = SOCLi_SocketGetNextPtr(next, socket);

    if (next)
    {
        *next = socket->next;
    }
}

static SOCLSocket ** SOCLi_SocketGetNextPtr(SOCLSocket ** next, SOCLSocket* socket)
{
    SOCLSocket*     t;

    for (t = *next; t; t = t->next)
    {
        if (t == socket)
        {
            return next;
        }

        next = &t->next;
    }

    return NULL;
}

/*---------------------------------------------------------------------------*
Name: SOCLi_SocketUnregisterTrash

Description: deletes the socket from the remove list

Arguments: socket          socket

Returns: None
 *---------------------------------------------------------------------------*/
void SOCLi_SocketUnregisterTrash(SOCLSocket* socket)
{
    SOCLi_SocketUnregisterList(&SOCLiSocketListTrash, socket);
}

/*---------------------------------------------------------------------------*
Name: SOCL_SocketIsInvalid

Description: confirms whether socket is valid and registered on the socket list

Arguments: socket          socket

Returns: value other than 0, registered, 0, not registered
 *---------------------------------------------------------------------------*/
int SOCL_SocketIsInvalid(SOCLSocket* socket)
{
    return((int)socket <= 0) || !SOCLi_SocketGetNextPtr(&SOCLiSocketList, socket);
}

/*---------------------------------------------------------------------------*
Name: SOCL_SocketIsInTrash

Description: confirms whether socket is registered on the remove socket list

Arguments: socket          socket

Returns: value other than 0, registered, 0, not registered
 *---------------------------------------------------------------------------*/
int SOCL_SocketIsInTrash(SOCLSocket* socket)
{
    return SOCLi_SocketGetNextPtr(&SOCLiSocketListTrash, socket) != NULL;
}
