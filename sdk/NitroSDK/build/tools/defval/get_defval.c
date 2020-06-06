/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - defval
  File:     get_defval.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: get_defval.c,v $
  Revision 1.2  01/18/2006 02:11:20  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/20 07:07:15  yasu
  add defval

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "defval.h"

/*---------------------------------------------------------------------------*
  Name:         get_dvalue_listptr
  
  Description:  Obtains a pointer to the list corresponding to the definition name.
  
  Arguments:    name        Variable name
  
  Returns:      Pointer to the definition list
 *---------------------------------------------------------------------------*/
tDefineValue *get_dvalue_listptr(const char *name)
{
    tDefineValue *t;

    for (t = gDefineValueTop; t; t = t->next)
    {
        if (!strcmp(t->name, name))
        {
            return t;
        }
    }
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         get_dvalue
  
  Description:  Gets the value of the definition name.
                Search the definition value list; If none, search the environment variables
                If still none, return NULL.
  
  Arguments:    name        Variable name
  
  Returns:      Pointer to the value string (No need to free)
                NULL: No matching variable
 *---------------------------------------------------------------------------*/
static char *get_dvalue(const char *name)
{
    tDefineValue *t = get_dvalue_listptr(name);

    return t ? t->value : getenv(name);
}

/*---------------------------------------------------------------------------*
  Name:         modify_dvalue
  
  Description:  Returns duplicate definition values after conversion using variable modifiers (:r, :t, :e, :h).
                 
  
  Arguments:    value     Value to modify
                modifier  Modifier (one of 'r', 't', 'e', 'h')
  
  Returns:      Pointer to the conversion value string (Must be freed)
 *---------------------------------------------------------------------------*/
static char *modify_dvalue(const char *value, char modifier)
{
    char   *value_dir;
    char   *value_base;
    char   *value_ext;
    char   *modified_value;

    if (value == NULL)
        return NULL;
    if (modifier == '\0')
        return StrDup(value);

    //
    // Modification process
    //   Divide the input value into filename elements and rebuild based on modifiers
    //
    UnpackFileName(value, &value_dir, &value_base, &value_ext);

    switch (modifier)
    {
    case 'h':                         // Directory name with final /, \
        modified_value = StrDup(value_dir);
        break;

    case 't':                         // Filename with no directory
        modified_value = StrCatDup(value_base, value_ext);
        break;

    case 'r':                         // Name with file extension removed from full path
        modified_value = StrCatDup(value_dir, value_base);
        break;

    case 'e':                         // File extension
        modified_value = StrDup(value_ext);
        break;

    default:                          // Unknown
        fprintf(stderr, "Unknown modifier ':%c'... Ignored.\n", modifier);
        modified_value = StrDup(value);
        break;
    }

    free(value_dir);
    free(value_base);
    free(value_ext);

    return modified_value;
}

/*---------------------------------------------------------------------------*
  Name:         get_modified_dvalue
  
  Description:  Takes value from the definition value list,
                applies conversion based on modifier (:r,:t,:e,:h) and returns a duplicate.
  
  Arguments:    name      Definition name
  
  Returns:      Pointer to the conversion value string (Must be freed)
 *---------------------------------------------------------------------------*/
static char *get_modified_dvalue(const char *name)
{
    char   *name_copy = StrDup(name);
    int     name_len = strlen(name_copy);
    char    modifier = '\0';
    char   *value;

    // Take the modifier out
    if (name_len > 2 && name_copy[name_len - 2] == ':')
    {
        name_copy[name_len - 2] = '\0';
        modifier = name_copy[name_len - 1];
    }

    // Get value
    value = modify_dvalue(get_dvalue(name_copy), modifier);
    free(name_copy);

    DebugPrintf("REFERED(%s)=[%s]\n", name, value ? value : "(NULL)");
    return value;
}

/*---------------------------------------------------------------------------*
  Name:         put_modified_dvalue
  
  Description:  Outputs a string to a file.
                It also expands $(XXXXX).
  
  Arguments:    fp	Output file name
                str	Character string
  
  Returns:      TRUE   success
 *---------------------------------------------------------------------------*/
BOOL puts_modified_dvalue(FILE * fp, const char *str)
{
    const char *str_name;
    int     str_name_len;
    char   *name;
    char   *value;
    int     result;

    while ('\0' != *str)
    {
        //
        // Search for $(XXX); If found, output the value.
        //
        if ('$' == *str && '(' == *(str + 1))
        {
            str_name = str + 2;
            str_name_len = 0;
            while ('\0' != str_name[str_name_len])
            {
                if (')' == str_name[str_name_len])
                {
                    name = StrNDup(str_name, str_name_len);
                    value = get_modified_dvalue(name);
                    free(name);

                    if (NULL != value)
                    {
                        result = fputs(value, fp);
                        free(value);
                        if (result == EOF)
                        {
                            return FALSE;
                        }
                    }
                    str = str_name + str_name_len;
                    goto skipout;
                }
                str_name_len++;
            }
        }

        //
        // If not $(XXX), output one char at a time
        //
        if (EOF == fputc(*str, fp))
        {
            return FALSE;
        }
      skipout:
        str++;
    }
    return TRUE;
}
