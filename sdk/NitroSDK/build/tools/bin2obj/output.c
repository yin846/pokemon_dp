/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     output.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: output.c,v $
  Revision 1.4  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.3  2005/06/13 02:56:34  yasu
  Revised options, confirmed operation with test creation

  Revision 1.2  2005/06/10 12:55:43  yasu
  1st refactoring

  Revision 1.1  2005/06/10 00:43:01  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "bin2obj.h"

static BOOL output_region(FILE * fp, u32 offset, const u8 *ptr, u32 size);

/*---------------------------------------------------------------------------*
  Name:         output_object

  Description:  Output object to file

  Arguments:    obj		Object information
                filename	Output file name
  
  Returns:      None
 *---------------------------------------------------------------------------*/
BOOL output_object(const Object * obj, const char *filename)
{
    FILE   *fp;
    u32     i, n;
    u32     offset;
    u32     size;
    u8     *ptr;

    if (NULL == (fp = fopen(filename, "wb")))
    {
        fprintf(stderr, "Error: Cannot open file %s.\n", filename);
        return FALSE;
    }

    // Elf Header
    offset = 0;
    size = obj->header.e_ehsize;
    ptr = (u8 *)&obj->header;
    if (!output_region(fp, offset, ptr, size))
    {
        return FALSE;
    }

    // Section Header
    offset = obj->header.e_shoff;
    size = obj->header.e_shentsize * obj->header.e_shnum;
    ptr = (u8 *)&obj->section[0];
    if (!output_region(fp, offset, ptr, size))
    {
        return FALSE;
    }

    // Section: ShStrTab
    n = obj->header.e_shstrndx;
    offset = obj->section[n].sh_offset;
    size = obj->section[n].sh_size;
    ptr = (u8 *)obj->section_name.ptr;
    if (!output_region(fp, offset, ptr, size))
    {
        return FALSE;
    }

    // Section: Symbol
    n = obj->symbol_index;
    offset = obj->section[n].sh_offset;
    size = obj->section[n].sh_size;
    ptr = (u8 *)obj->symbol;
    if (!output_region(fp, offset, ptr, size))
    {
        return FALSE;
    }

    // Section: StrTab
    n = obj->symbol_name_index;
    offset = obj->section[n].sh_offset;
    size = obj->section[n].sh_size;
    ptr = (u8 *)obj->symbol_name.ptr;
    if (!output_region(fp, offset, ptr, size))
    {
        return FALSE;
    }

    // Section: Object
    for (i = 0; i < obj->num_data; i++)
    {
        n = obj->data[i].index;
        offset = obj->section[n].sh_offset;
        size = obj->section[n].sh_size;
        ptr = (u8 *)obj->data[i].section.ptr;
        if (!output_region(fp, offset, ptr, size))
        {
            return FALSE;
        }
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*
  Name:         output_region

  Description:  Outputs data with memory alignment

  Arguments:    fp               File pointer
                ptr              Data
                size             Memory block size
                offset           Write start offset
  Returns:      TRUE: success  FALSE: failure
 *---------------------------------------------------------------------------*/
static BOOL output_region(FILE * fp, u32 offset, const u8 *ptr, u32 size)
{
    static u8 zero[1] = { '\0' };

    while (ftell(fp) < offset)
    {
        if (1 != fwrite(zero, sizeof(u8), 1, fp))
        {
            return FALSE;
        }
    }
    return 1 == fwrite(ptr, size, 1, fp) ? TRUE : FALSE;
}
