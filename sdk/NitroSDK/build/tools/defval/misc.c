/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - defval
  File:     misc.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: misc.c,v $
  Revision 1.2  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/20 07:07:15  yasu
  add defval

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "misc.h"

/*---------------------------------------------------------------------------*
  Name:         StrDup

  Description:  Error handling version of strdup
  
  Arguments:    str     String to copy
  
  Returns:      String that was copied
 *---------------------------------------------------------------------------*/
char   *StrDup(const char *str)
{
    char   *cp;

    if (NULL == (cp = strdup(str ? str : "")))
    {
        fprintf(stderr, "Error: No memory.");
        exit(2);
    }
    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         StrNDup

  Description:  Error-handling version of strdup with char length specification.
  
  Arguments:    str     String to copy
                len     String to copy (Not including '\0')
  
  Returns:      String that was copied
 *---------------------------------------------------------------------------*/
char   *StrNDup(const char *str, int len)
{
    char   *cp;

    cp = Calloc(len + 1);
    if (str && len)
        (void)strncpy(cp, str, len);
    return cp;

    // Area having len+1 bytes was allocated with malloc,
    // so there is no need to add  '\0' after strncpy.
    // Can also support cases where str is NULL
}

/*---------------------------------------------------------------------------*
  Name:         StrCatDup

  Description:  Allocates a concatenated string from the heap.
  
  Arguments:    str1    String 1 ==When NULL, treated as ""
                str2    String 2 ==When NULL, treated as ""
  
  Returns:      Concatenated string
 *---------------------------------------------------------------------------*/
char   *StrCatDup(const char *str1, const char *str2)
{
    int     len1, len2;
    char   *cp;

    len1 = str1 ? strlen(str1) : 0;
    len2 = str2 ? strlen(str2) : 0;

    cp = Calloc(len1 + len2 + 1);

    if (str1)
        (void)strcpy(cp, str1);
    if (str2)
        (void)strcat(cp, str2);

    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         Calloc

  Description:  Error-handling version of calloc (Only one argument))
  
  Arguments:    size    Length of area to allocate
  
  Returns:      Allocated area
 *---------------------------------------------------------------------------*/
void   *Calloc(int size)
{
    void   *cp;

    if (NULL == (cp = calloc(1, size)))
    {
        fprintf(stderr, "Error: No memory.");
        exit(2);
    }
    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         Free

  Description:  NULL-handling version of free
                Variable *ptr is cleared to NULL
  
  Arguments:    ptr  Area to free
  
  Returns:      None
 *---------------------------------------------------------------------------*/
void Free(void **ptr)
{
    if (NULL != *ptr)
    {
        free(*ptr);
        *ptr = NULL;
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         Realloc

  Description:  Error-handling version of realloc
  
  Arguments:    buffer  Original size
                size    Length of area to allocate
  
  Returns:      Allocated area
 *---------------------------------------------------------------------------*/
void   *Realloc(void *buffer, int size)
{
    void   *cp;

    cp = Calloc(size);
    if (buffer)
    {
        strcpy(cp, buffer);
        free(buffer);
    }
    return cp;
}

/*---------------------------------------------------------------------------*
  Name:         Fopen

  Description:  Version of fopen that has stdin/stdout setting;
                If '-' is specified, or if the access mode is 'r', set to stdin.
                                    If the access mode is 'w', set to stdout.
  
  Arguments:    name   (Pointer to) the pointer to the file buffer
                mode   (Pointer to) the file buffer size
  
  Returns:      fp               File pointer
 *---------------------------------------------------------------------------*/
FILE   *Fopen(const char *filename, const char *mode)
{
    FILE   *fp;

    DebugPrintf("filename=[%s] mode=[%s]\n", filename, mode);

    if ('-' == filename[0] && '\0' == filename[1])
    {
        fp = (mode[0] == 'r') ? stdin : stdout;
    }
    else
    {
        if (NULL == (fp = fopen(filename, mode)))
        {
            fprintf(stderr, "Error: cannot open %s\n", filename);
            return NULL;
        }
    }

    DebugPrintf("fp=[%x] stdin=[%x] stdout=[%x]\n", fp, stdin, stdout);

    return fp;
}

/*---------------------------------------------------------------------------*
  Name:         Fclose

  Description:  stdin/stdout of fclose is taken into consideration.
                If NULL, stdin, stdout, stderr, does not close
  
  Arguments:    fp               File pointer
  
  Returns:      None
 *---------------------------------------------------------------------------*/
void Fclose(FILE * fp)
{
    if (fp != NULL && fp != stdin && fp != stdout && fp != stderr)
    {
        fclose(fp);
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         Fgets

  Description:  Reads the file one line at a time.
                Unlike the normal fgets, uses dynamic control of the buffer size
                to ensure reading one line.
  
  Arguments:    *buffer  (Pointer to) the pointer to the file buffer
                *size   (Pointer to) the file buffer size
                fp        The file (If =NULL, reads from stdin)
  
      'buffer' and 'size' are realloc'ed as necessary inside Fgets.
      Passing 'size' as an argument reuses 'buffer' so that alloc/free processing
      is reduced.
      It is expected that the first call will be with *buffer to set NULL.
  
  Returns:      If successful, returns *buffer. If it fails at the end of file, etc., returns NULL.
 *---------------------------------------------------------------------------*/
char   *Fgets(char **pbuffer, int *pbuffer_size, FILE * fp)
{
    char   *buffer;
    int     buffer_size;
    int     buffer_gets;

    assert(pbuffer);
    assert(pbuffer_size);

    buffer = *pbuffer;
    buffer_size = *pbuffer_size;

    if (buffer == NULL || buffer_size == 0)
    {
        buffer_size = DEFAULT_LINE_BUFFER_SIZE;
        buffer = Calloc(buffer_size);
    }

    buffer_gets = 0;

    while (NULL != fgets(buffer + buffer_gets, buffer_size - buffer_gets, fp))
    {
        //
        //  If the loaded line reaches the line-end, register the definition value
        //    In fgets-based reading, the end of a line is determined when:
        //      1) Data could not be loaded up to the very end of the buffer area;
        //      2) Data could be loaded fully, but the line ends exactly at that location.
        //
        buffer_gets = strlen(buffer);
        if (buffer_gets < buffer_size - 1 || buffer[buffer_gets - 1] == '\n')
        {
            *pbuffer = buffer;
            *pbuffer_size = buffer_size;
            return buffer;
        }
        //
        //  If the loaded line does not reach the end of the line, double the buffer size,
        //  and continue reading.
        //
        else
        {
            buffer_size *= 2;
            buffer = Realloc(buffer, buffer_size);
        }
    }

    // Processing at the end of file
    free(*pbuffer);
    *pbuffer = NULL;
    *pbuffer_size = 0;
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         UnpackFileName

  Description:  Analyzes the file name and divides it into parts
                (directory, file base, file extension)
  
  Arguments:    path   Full path name
                dir    Directory  (Including '/' etc.)
                base   File base
                ext    File extension (including '.')
  
                * Because dir, base, and ext are allocated from the heap, they must be freed
                * When dir, base or ext is NULL, no value is assigned
  
  Returns:      None
 *---------------------------------------------------------------------------*/
void UnpackFileName(const char *path, char **dir, char **base, char **ext)
{
    int     i, base_top, ext_top;
    int     path_len = strlen(path);

    // Search for the position of the final '.' and the path delimiters
    ext_top = path_len;
    base_top = 0;

    for (i = path_len - 1; i >= 0; i--)
    {
        if (path[i] == '.' && ext_top == path_len)
        {
            ext_top = i;
        }
        if (path[i] == '/' || path[i] == '\\' || path[i] == ':')
        {
            base_top = i + 1;
            break;
        }
    }

    // Processing of "." and ".." (In this case, the base is "." or "..", with no extension)
    if (!strcmp(path + base_top, ".") || !strcmp(path + base_top, ".."))
    {
        ext_top = path_len;
    }

    // Assign
    if (dir)
        *dir = StrNDup(path, base_top);
    if (base)
        *base = StrNDup(path + base_top, ext_top - base_top);
    if (ext)
        *ext = StrDup(path + ext_top);
    return;
}

/*---------------------------------------------------------------------------*
  Name:         SetDebugMode
  
  Description:  Sets debug mode.
  
  Arguments:    mode   The mode
  
  Returns:      None
 *---------------------------------------------------------------------------*/
static BOOL sDebugMode = FALSE;

void SetDebugMode(BOOL mode)
{
    sDebugMode = mode;
}

/*---------------------------------------------------------------------------*
  Name:         DebugPrintf
  
  Description:  Debug print
  
  Arguments:    fmt   Printf  format
                ...   Variables to print
  
  Returns:      None
 *---------------------------------------------------------------------------*/
void DebugPrintf(const char *fmt, ...)
{
    va_list va;

    if (sDebugMode)
    {
        va_start(va, fmt);
        vfprintf(stderr, fmt, va);
        va_end(va);
    }
}
