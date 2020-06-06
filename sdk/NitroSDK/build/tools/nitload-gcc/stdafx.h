/*---------------------------------------------------------------------------*
  Project:  NitroSDK - Tools - nitro-gcc
  File:     stdafx.h

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: stdafx.h,v $
  Revision 1.2  01/18/2006 02:12:29  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/21 06:22:33  yasu
  Created initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "stdafx_orig.h"
#include "nitrodll.h"

//
// Suppress console message display
//
#define  printf(...)		((void)0)

/*---------------------------------------------------------------------------*
  Description:  dll.GetDebugPrint hack
                In the Makefile, replaces dll.GetDebugPrint with dll_GetDebugPrint
                and causes execution to branch here.
                If -1 comes down the stream, stops the program.
                The character after -1 is the return value.
 *---------------------------------------------------------------------------*/
BOOL    dll_GetDebugPrint(CNITRODLL * dll, NITROArch arch, char *string, INT * string_lenp,
                          INT string_max);
BOOL dll_GetDebugPrint(CNITRODLL * dll, NITROArch arch, char *string, INT * string_lenp,
                       INT string_max)
{
    BOOL    result = dll->GetDebugPrint(arch, string, string_lenp, string_max);
    int     retval;
    int     i;
    static BOOL isexit = FALSE;

    if (result && *string_lenp)
    {
        for (i = 0; i < *string_lenp; i++)
        {
            //
            // Determines if end-token has been received previously,
            // and if so, performs the end processing.
            //
            if (isexit)
            {
                if (i > 0)             // Flush the console buffer
                {
                    fputs(string, stdout);
                }
                retval = string[i] - '0';
                dll->Close();
                exit(retval);
            }

            //
            // Determine if it is the end-token
            //    If end, the next value becomes the application's return value,
            //    so enable the flag.
            //    The flag is enabled purposely in case the end-token and the return value are sent
            //    straddling two buffers.
            //
            if (string[i] == (char)-1)
            {
                string[i] = '\0';
                isexit = TRUE;
            }
        }
    }
    return result;
}
