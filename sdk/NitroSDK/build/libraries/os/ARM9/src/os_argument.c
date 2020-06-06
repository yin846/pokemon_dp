/*---------------------------------------------------------------------------*
  Project:  NitroSDK - OS
  File:     os_argument.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: os_argument.c,v $
  Revision 1.11  01/18/2006 02:11:30  kitase_hirotake
  do-indent

  Revision 1.10  10/07/2005 06:24:24  yada
  small fix

  Revision 1.9  09/09/2005 05:20:33  yada
  small fix

  Revision 1.8  09/09/2005 04:35:48  yada
  add conversion from string to arguments

  Revision 1.7  2005/09/06 10:16:42  yasu
  Revised return values during '-' mode.

  Revision 1.6  2005/09/06 09:10:37  yasu
  Implemented '-' and '+' mode.

  Revision 1.5  2005/08/30 10:35:28  yasu
  Revised so that even conventional arguments that are not options can be handled transparently as options.

  Revision 1.4  2005/08/30 09:00:14  yasu
  Added OS_GetOpt.

  Revision 1.3  07/21/2005 12:41:50  yada
  consider for NITRO_FINALROM

  Revision 1.2  07/21/2005 09:44:36  yada
  fix bury rule

  Revision 1.1  07/21/2005 02:31:41  yada
  initial release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro/os.h>

//---- This area is for argument string, and may be
//     modified from external tools.
#ifndef OS_NO_ARGUMENT
#include <nitro/version_begin.h>
static OSArgumentBuffer OSi_ArgumentBuffer = {
    OS_ARGUMENT_ID_STRING,
    OS_ARGUMENT_BUFFER_SIZE,
    {'\0', '\0'},
};
#include <nitro/version_end.h>

//---- pointer to current argument buffer
const char *sCurrentArgBuffer = &OSi_ArgumentBuffer.buffer[0];

static const char *match_opt(int optchar, const char *optstring);
#endif //ifndef OS_NO_ARGUMENT

const char *OSi_OptArg = NULL;
int     OSi_OptInd = 1;
int     OSi_OptOpt = 0;


/*---------------------------------------------------------------------------*
  Name:         OS_GetArgc

  Description:  Get number of valid arguments.
                This function is for debug.

  Arguments:    None

  Returns:      number of valid arguments.
                if no argument, return 1.
                if not set argument buffer, return 0.
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
int OS_GetArgc(void)
{
    int     n = 0;
    const char *p = (const char *)sCurrentArgBuffer;

    for (; *p; p++, n++)
    {
        while (*p)
        {
            p++;
        }
    }

    return n;
}
#endif // ifndef OS_NO_ARGUMENT

/*---------------------------------------------------------------------------*
  Name:         OS_GetArgv

  Description:  Get the pointer to the specified argument string.
                This function is for debug.

  Arguments:    n : index of argument. n==1 means the first argument.

  Returns:      n must less than value of OS_GetArgc()
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
const char *OS_GetArgv(int n)
{
    const char *p = (const char *)sCurrentArgBuffer;

    SDK_ASSERT(n >= 0);

    for (; *p && n > 0; p++, n--)
    {
        while (*p)
        {
            p++;
        }
    }

    return (*p) ? p : NULL;
}
#endif // ifndef OS_NO_ARGUMENT


/*---------------------------------------------------------------------------*
  Name:         OS_GetOpt

  Description:  getopt() like function to get command line options
  
  Arguments:    optstring: Option character string
                           Internal parameters are reset if NULL.

  Returns:      Option character code.
                '?' Indicates the option character code is unclear
                -1  Indicates the option does not exist
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
int OS_GetOpt(const char *optstring)
{
    static BOOL end_of_option = FALSE;
    int     optchar;
    const char *arg;
    const char *opt;
    const char *optarg;

    OSi_OptArg = NULL;
    OSi_OptOpt = 0;

    // Reset if optstring is NULL
    if (optstring == NULL)
    {
        OSi_OptInd = 1;
        end_of_option = FALSE;
        return 0;
    }

    // Get command line arguments
    arg = OS_GetArgv(OSi_OptInd);

    if (arg == NULL)
    {
        return -1;
    }

    if (optstring[0] == '-')           // Minus Mode
    {
        OSi_OptInd++;                  // Process arguments

        // Take as conventional argument if it does not start with '-'
        if (end_of_option || arg[0] != '-')
        {
            OSi_OptArg = arg;          // Even conventional arguments are set to OptArg.
            return 1;
        }

        // Analyze options
        optchar = arg[1];

        if (optchar == '-')            // End of options when '--' is encountered
        {
            end_of_option = TRUE;      // Conventional arguments from here on
            return OS_GetOpt(optstring);
        }
    }
    else                               // normal mode
    {
        // Take as conventional argument if it does not start with '-'
        if (end_of_option || arg[0] != '-')
        {
            return -1;                 // OptArg remains NULL
        }

        OSi_OptInd++;                  // Process arguments

        // Analyze options
        optchar = arg[1];

        if (optchar == '-')            // End of options when '--' is encountered
        {
            end_of_option = TRUE;      // Conventional arguments from here on
            return -1;
        }

    }

    opt = match_opt(optchar, optstring);

    if (opt == NULL)
    {
        OSi_OptOpt = optchar;          // Unknown option
        return '?';
    }

    if (opt[1] == ':')                 // OptArg search specification?
    {
        optarg = OS_GetArgv(OSi_OptInd);

        if (optarg == NULL || optarg[0] == '-')
        {
            if (opt[2] != ':')         // '::' Or not?
            {
                OSi_OptOpt = optchar;  // OptArg not present.
                return '?';
            }
            // OptArg not present.
            // OSi_OptArg = NULL
        }
        else
        {
            OSi_OptArg = optarg;
            OSi_OptInd++;              // Process arguments
        }
    }
    return optchar;
}

static const char *match_opt(int optchar, const char *optstring)
{
    if (optstring[0] == '-' || optstring[0] == '+')
    {
        optstring++;
    }

    if (optchar != ':')
    {
        while (*optstring)
        {
            if (optchar == *optstring)
            {
                return optstring;
            }
            optstring++;
        }
    }
    return NULL;
}
#endif // ifndef OS_NO_ARGUMENT

/*---------------------------------------------------------------------------*
  Name:         OS_ConvertToArguments

  Description:  convert string data to arg binary
  
  Arguments:    str     : string
  				cs	    : character to separate
  				buffer  : buffer to store
  				bufSize : max buffer size
 
  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
void OS_ConvertToArguments(const char *str, char cs, char *buffer, u32 bufSize)
{

    BOOL    isPause = TRUE;
    char   *p = buffer;
    char   *pEnd = buffer + bufSize;
    BOOL    isQuoted = FALSE;

    while (1)
    {
        //---- skip separater
        while (*str == cs && p < pEnd)
        {
            str++;
        }

        //---- store argument string
        while (*str && p < pEnd)
        {
            //---- check quote
            if (*str == '\"')
            {
                isQuoted = (isQuoted == FALSE);
                str++;
                continue;
            }

            //---- found separater
            else if (*str == cs && isQuoted == FALSE)
            {
                break;
            }

            *p++ = *str++;
        }

        //---- 1) reached to the buffer end
        if (p >= pEnd)
        {
            OS_Printf("pend-2=%x\n", pEnd - 2);
            *(pEnd - 2) = '\0';
            *(pEnd - 1) = '\0';
            break;
        }

        //---- 2) reached to the string end
        if (*str == '\0')
        {
            while ((p + 1) >= pEnd)
            {
                p--;
            }
            *p++ = '\0';
            *p = '\0';
            break;
        }

        //---- 3) separater
        if (*str == cs)
        {
            *p++ = '\0';
        }
    }

    //---- end mark
    if (p < pEnd)
    {
        *p++ = '\0';
    }
}
#endif //ifndef OS_NO_ARGUMENT

/*---------------------------------------------------------------------------*
  Name:         OS_SetArgumentBuffer

  Description:  force to set argument buffer.
  
  Arguments:    buffer  : argument buffer
 
  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
void OS_SetArgumentBuffer(const char *buffer)
{
    sCurrentArgBuffer = buffer;
}
#endif //ifndef OS_NO_ARGUMENT

/*---------------------------------------------------------------------------*
  Name:         OS_GetArgumentBuffer

  Description:  get pointer to argument buffer.
  
  Arguments:    None.
 
  Returns:      pointer to argument buffer.
 *---------------------------------------------------------------------------*/
#ifndef OS_NO_ARGUMENT
const char *OS_GetArgumentBuffer(void)
{
    return sCurrentArgBuffer;
}
#endif //ifndef OS_NO_ARGUMENT
