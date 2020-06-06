/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - defval
  File:     set_defval.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: set_defval.c,v $
  Revision 1.2  01/18/2006 02:12:28  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/20 07:07:15  yasu
  add defval

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "defval.h"

static BOOL add_dvalue(const char *name, const char *value);
static char *clip_whitespace(char *str);


//---------------------------------------------------------------------------
//  List that maintains defined values
//---------------------------------------------------------------------------
tDefineValue *gDefineValueTop = NULL;

/*---------------------------------------------------------------------------*
  Name:         add_dvalue
  
  Description:  Adds a new value to the definition value list.
  
  Arguments:    name        Variable name
                value Variable's value
  
                If name  is NULL or "", then do nothing and stop (Return FALSE)
                If value is NULL, it is treated as being defined as "".
  
  Returns:      TRUE   Newly added
                FALSE  Value with the same name is already stored (Overwritten by the new one)
 *---------------------------------------------------------------------------*/
static BOOL add_dvalue(const char *name, const char *value)
{
    tDefineValue *t;

    //
    // Handling when NULL
    //   If name is NULL or "", then do nothing and stop (Return TRUE)
    //   If value is NULL, it is treated as being defined as "".
    //
    if (name == NULL || name[0] == '\0')
    {
        return TRUE;
    }
    if (value == NULL)
    {
        value = (const char *)"";
    }

    DebugPrintf("DEFINE:$(%s)=\"%s\"\n", name, value);

    // If a definition name with the same name exists, replace it with the new value.
    if (NULL != (t = get_dvalue_listptr(name)))
    {
        if (t->value)
            free(t->value);
        t->value = StrDup(value);
        return FALSE;
    }

    // If it is a new definition name, expand the list and register it.
    t = Calloc(sizeof(tDefineValue));
    t->name = StrDup(name);
    t->value = StrDup(value);
    t->next = gDefineValueTop;
    gDefineValueTop = t;

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         add_dvalue_by_equality
  
  Description:  Adds a new value in the form of an equation to the definition list.
  
  Arguments:    equality Equation "DEFINE=VALUE"
  
                If equality is NULL or "", does nothing.
  
  Returns:      TRUE   Newly added
                FALSE  Value with the same name is already stored (Overwritten by the new one)
 *---------------------------------------------------------------------------*/
BOOL add_dvalue_by_equality(const char *equality)
{
    char   *name;
    char   *value;
    char   *scratch;
    BOOL    result;

    //
    //  Copy equation to changeable region, divide into name and value before and after '='
    //
    scratch = StrDup(equality);
    name = scratch;
    value = scratch;
    while ('\0' != *value)
    {
        if ('=' == *value)
        {
            *value = '\0';             // Divide by entering '\0' at the position of '='
            value++;
            break;
        }
        value++;
    }
    name = clip_whitespace(name);      // Delete spaces before and after
    value = clip_whitespace(value);

    //
    //  Register the value
    //
    result = add_dvalue(name, value);
    free(scratch);

    return result;
}

/*---------------------------------------------------------------------------*
  Name:         add_dvalue_from_file
  
  Description:  Adds equation in file to the definition value list.
  
  Arguments:    filename Filename
                "DEFINE=VALUE" list
  
  Returns:      TRUE   Successful
                FALSE  Value with the same name is already stored (Overwritten by the new one)
 *---------------------------------------------------------------------------*/
BOOL add_dvalue_from_file(const char *filename)
{
    FILE   *fp;
    char   *buffer;
    int     buffer_size;
    int     line_num;

    if (filename[0] == '-' && filename[1] == '\0')
    {
        fp = stdin;
    }
    else if (NULL == (fp = fopen(filename, "r")))
    {
        fprintf(stderr, "Cannot open file \"%s\".\n", filename);
        return FALSE;
    }

    //
    // Read the file line by line and register the variables
    //
    buffer = NULL;
    buffer_size = line_num = 0;
    while (NULL != Fgets(&buffer, &buffer_size, fp))
    {
        line_num++;
        if (!add_dvalue_by_equality(buffer))
        {
            fprintf(stderr, "line %d: found same entry: %s\n", line_num, buffer);
        }
    }

    if (fp != stdin)
        fclose(fp);

    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         chip_whitespace
  
  Description:  Deletes blank space before and after a string.
  
  Arguments:    str    Beginning of the string to process
  
                The contents of str are overwritten, so it does work with a const area
  
  Returns:      The new starting position
 *---------------------------------------------------------------------------*/
static char *clip_whitespace(char *str)
{
    int     n;

    // Fill the trailing blank space with '\0'
    for (n = strlen(str) - 1; n >= 0; n--)
    {
        if (!isspace(str[n]))
        {
            str[n + 1] = '\0';
            break;
        }
    }

    // Skip leading blank space and return a pointer to the beginning of the string
    for (n = 0; str[n] != '\0'; n++)
    {
        if (!isspace(str[n]))
        {
            break;
        }
    }
    return str + n;
}
