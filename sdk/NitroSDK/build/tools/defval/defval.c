/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - defval
  File:     defval.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: defval.c,v $
  Revision 1.2  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/20 07:07:15  yasu
  add defval

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include        "defval.h"
#include        "version.h"

static void usage(const char *argv0);
static BOOL defval(const char *file_in, const char *file_out);

/*---------------------------------------------------------------------------*
  Name:         main
  
  Description:  defval main function
                Processes options and calls the main processing function.
  
  Arguments:    argc   Argument count
                argv   Argument
  
  Returns:      0  ended normally
                Non-0  Error
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int     n, fargc;
    char  **fargv;
    BOOL    result;

    //
    // Analyze options
    //
    while ((n = getopt(argc, argv, "D:M:dh")) != -1)
    {
        switch (n)
        {
        case 'D':
            (void)add_dvalue_by_equality(optarg);
            break;

        case 'M':
            if (!add_dvalue_from_file(optarg))
            {
                fprintf(stderr, "Stop.\n");
                return 1;
            }
            break;

        case 'd':
            SetDebugMode(TRUE);
            break;

        case 'h':
        case '?':
        case ':':
        default:
            usage(argv[0]);
        }
    }

    //
    // Start processing
    //
    fargc = argc - optind;
    fargv = argv + optind;

    switch (fargc)
    {
    case 0:
        result = defval("-", "-");
        break;

    case 1:
        result = defval(fargv[0], "-");
        break;

    case 2:
    default:
        result = defval(fargv[0], fargv[1]);
        break;
    }
    return result ? 0 : 1;
}

static void usage(const char *argv0)
{
    char   *appname;

    UnpackFileName(argv0, NULL, &appname, NULL);        // Get command name

    fprintf(stderr,
            "NITRO-SDK Development Tool - %s - filter to support user defines\n"
            "Build %lu\n\n"
            "Usage: %s [-DNAME=VALUE...] [-MDEFINES_FILE] INFILE OUTFILE\n\n",
            appname, SDK_DATE_OF_LATEST_FILE, appname);

    free(appname);
    exit(2);
}

/*---------------------------------------------------------------------------*
  Name:         main
  
  Description:  defval main function
                Processes options and calls the main processing function.
  
  Arguments:    file_in  Input file
                file_out Output file
  
  Returns:      TRUE   Ended successfully
                FALSE  Error
 *---------------------------------------------------------------------------*/
static BOOL defval(const char *file_in, const char *file_out)
{
    BOOL    result;
    FILE   *fp_in;
    FILE   *fp_out;
    char   *buffer;
    int     buffer_size;

    DebugPrintf("file_in=[%s] file_out=[%s]\n", file_in, file_out);

    result = FALSE;
    buffer = NULL;

    if (NULL == (fp_in = Fopen(file_in, "r")) || NULL == (fp_out = Fopen(file_out, "w")))
    {
        goto escape;
    }

    while (NULL != Fgets(&buffer, &buffer_size, fp_in))
    {
        DebugPrintf("buffer=[%s] buffer_size=[%d]\n", buffer, buffer_size);

        if (!puts_modified_dvalue(fp_out, buffer))
        {
            fprintf(stderr, "Error: Cannot write %s.\n", file_out);
            goto escape;
        }
    }
    result = TRUE;

  escape:
    Free((void **)&buffer);
    Fclose(fp_in);
    Fclose(fp_out);

    return result;
}
