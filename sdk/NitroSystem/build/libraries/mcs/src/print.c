/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Multiple Channel Stream library
  File:     print.c

  Copyright 2004,2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: print.c,v $
  Revision 1.5  01/21/2005 04:50:10  kitamurs
  Changed year

  Revision 1.4  12/21/2004 05:17:59  kitamurs
  Introduced check for connection

  Revision 1.3  12/16/2004 08:57:51  kitamurs
  Changed function names

  Revision 1.2  12/16/2004 07:25:03  kitamurs
  Newly created the Sleep function

  Revision 1.1  12/15/2004 06:06:23  kitamurs
  Print features

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#if ! defined(NNS_FINALROM)


#include <nitro.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <nnsys/misc.h>
#include <nnsys/mcs/base.h>
#include <nnsys/mcs/print.h>

#include "basei.h"
#include "printi.h"


/* ========================================================================
    Constants
   ======================================================================== */

static const int        SEND_RETRY_MAX = 10;


/* ========================================================================
    static variables
   ======================================================================== */

static NNSMcsRecvCBInfo sCBInfo;
static void*            spBuffer    = NULL;
static u32              sBufferSize = 0;


/* ========================================================================
    static functions
   ======================================================================== */

static inline BOOL
IsInitialize(void)
{
    return NULL != spBuffer;
}

static void
CallbackRecvDummy(
    const void* /*pRecv*/,
    u32         /*dwRecvSize*/,
    u32         /*dwUserData*/,
    u32         /*offset*/,
    u32         /*totalSize*/
)
{
    // do nothing
}

static inline BOOL
CheckConnect()
{
    if (NNS_McsIsServerConnect())
    {
        return TRUE;
    }
    else
    {
        OS_Printf("NNS Mcs Print: not sever connect\n");
        return FALSE;
    }
}

/*
    Send data
        Retry several times until successful. 
  */
static BOOL
WriteStream(
    const void* buf,
    u32         length
)
{
    int retryCount;

    for (retryCount = 0; retryCount < SEND_RETRY_MAX; ++retryCount)
    {
        u32 writableLength;
        if (! NNS_McsGetStreamWritableLength(&writableLength))
        {
            return FALSE;
        }

        if (length <= writableLength)
        {
            return NNS_McsWriteStream(NNSi_MCS_PRINT_CHANNEL, buf, length);
        }
        else
        {
            // OS_Printf("NNS Mcs Print: buffer short - retry\n");
            NNSi_McsSleep(16);
        }
    }

    OS_Printf("NNS Mcs Print: send time out\n");
    return FALSE;
}


/* ========================================================================
    External functions
   ======================================================================== */

/*---------------------------------------------------------------------------*
  Name:         NNS_McsInitPrint

  Description:  Initializes file I/O API.

  Arguments:    pBuffer:  Pointer to the work buffer for output.
                buffer:   Buffer size (in bytes).

  Returns:      None.
 *---------------------------------------------------------------------------*/
void
NNS_McsInitPrint(
    void*   pBuffer,
    u32     size
)
{
    NNS_ASSERT(pBuffer != NULL);
    NNS_ASSERT(size > 1);

    if (IsInitialize())
    {
        return;
    }

    spBuffer    = pBuffer;      // Also means that initialization is complete
    sBufferSize = size;

    NNS_McsRegisterRecvCallback(&sCBInfo, NNSi_MCS_PRINT_CHANNEL, CallbackRecvDummy, 0);
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsPutString

  Description:  Outputs character string to the mcs server's console.

  Arguments:    string:  The character string to output.

  Returns:      TRUE if the function succeeds, otherwise FALSE.
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsPutString(
    const char* string
)
{
    NNS_ASSERT(IsInitialize());
    NNS_ASSERT(string);

    if (! CheckConnect())
    {
        return FALSE;
    }

    return WriteStream(string, strlen(string));
}

/*---------------------------------------------------------------------------*
  Name:         NNS_McsPrintf

  Description:  Outputs formatted string to mcs server's console.

  Arguments:    format:  Formatted character string.
                ...   :  Arguments that can be omitted.

  Returns:      TRUE if the function succeeds, otherwise FALSE.
 *---------------------------------------------------------------------------*/
BOOL
NNS_McsPrintf(
    const char* format,
    ...
)
{
    int writeChars;
    va_list args;

    NNS_ASSERT(IsInitialize());
    NNS_ASSERT(format);

    if (! CheckConnect())
    {
        return FALSE;
    }

    va_start(args, format);

    writeChars = vsnprintf(spBuffer, sBufferSize, format, args);

    va_end(vlist);

    // With vsnprintf, if buffer cannot hold all, then the portion that it can hold is set there.
    // Also, 0 is set at the tail end.
    if (writeChars > 1)
    {
        return WriteStream(spBuffer, strlen(spBuffer));
    }

    return FALSE;
}

/* #if ! defined(NNS_FINALROM)*/
#endif

