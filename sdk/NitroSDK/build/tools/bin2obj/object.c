/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     object.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: object.c,v $
  Revision 1.2  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.1  2005/06/10 12:55:43  yasu
  1st refactoring

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "bin2obj.h"

static void header_init(ELF32_ElfHeader * h);

/*---------------------------------------------------------------------------*
  Name:         object_init

  Description:  Initializes object information.

  Arguments:    obj    Object
  
  Returns:      TRUE succeeded   FALSE failed
 *---------------------------------------------------------------------------*/
void object_init(Object * obj)
{
    ELF32_SectionHeader *s;
    ELF32_ElfHeader *h;

    //-----------------------------------------------------------------------
    // Initialize area ZERO
    //-----------------------------------------------------------------------
    memset(obj, 0, sizeof(Object));

    //-----------------------------------------------------------------------
    // Initialization of Elf Header
    //-----------------------------------------------------------------------
    h = &obj->header;
    header_init(h);

    //-----------------------------------------------------------------------
    // Registration of default section
    //-----------------------------------------------------------------------

    // The section with index=0 is dummy=NULL
    (void)add_section(obj, NULL, SHT_NULL, 0, 0, 0);

    // index=1 section name table
    h->e_shstrndx = add_section(obj, ".shstrtab", SHT_STRTAB, SHF_NULL, obj->section_name.size, 0);

    // index=2 symbol info structure
    obj->symbol_index = add_section(obj, ".symtab", SHT_SYMTAB, SHF_NULL, 0, 4);

    // index=3 symbol name table
    obj->symbol_name_index =
        add_section(obj, ".strtab", SHT_STRTAB, SHF_NULL, obj->symbol_name.size, 1);

    // Add symbol info setting
    s = &obj->section[obj->symbol_index];
    s->sh_link = obj->symbol_name_index;        // symbol name table link
    s->sh_info = -1;                   // Initialization of symbol's last index value
    s->sh_entsize = sizeof(ELF32_Symbol);       // symbol entry size

    //-----------------------------------------------------------------------
    // Symbol table initialization
    //-----------------------------------------------------------------------

    // Register index=0 NULL symbol
    (void)add_symbol(obj, NULL, 0, 0, 0);
}

/*---------------------------------------------------------------------------*
  Name:         header_init

  Description:  Creates ELF header info.

  Arguments:    elfHeader  ELF header info
  
  Returns:      None
 *---------------------------------------------------------------------------*/
static void header_init(ELF32_ElfHeader * h)
{
    memset(h, 0, sizeof(ELF32_ElfHeader));
    h->e_ident[0] = ELFMAG0;           // 0x7f
    h->e_ident[1] = ELFMAG1;           // 'E'
    h->e_ident[2] = ELFMAG2;           // 'L'
    h->e_ident[3] = ELFMAG3;           // 'F'
    h->e_ident[4] = ELFCLASS32;        // 32bit
    h->e_ident[5] = ELFDATA2LSB;       // Little Endian
    h->e_ident[6] = EV_CURRENT;        // CURRENT
    h->e_type = ET_REL;
    h->e_machine = EM_ARM;
    h->e_version = EV_CURRENT;
    h->e_entry = 0;
    h->e_phoff = 0;
    h->e_shoff = 0;                    // Fill with correct values later
    h->e_flags = 0;
    h->e_ehsize = sizeof(ELF32_ElfHeader);
    h->e_phentsize = 0;
    h->e_phnum = 0;
    h->e_shentsize = sizeof(ELF32_SectionHeader);
    h->e_shnum = 0;                    // +1 each time section increases
    h->e_shstrndx = 0;                 // Fill with correct values later

    return;
}

/*---------------------------------------------------------------------------*
  Name:         map_section

  Description:  Writes the location information of the section.

  Arguments:    obj    Object
  
  Returns:      None
 *---------------------------------------------------------------------------*/
void map_section(Object * obj)
{
    ELF32_ElfHeader *h;
    ELF32_SectionHeader *s;
    u32     offset;
    int     i;

    // ELF HEADER/SECTION HEADER position calculation
    h = &obj->header;
    offset = h->e_ehsize;
    h->e_shoff = roundup(offset, 4);
    offset = h->e_shoff + h->e_shentsize * h->e_shnum;

    // Section position calculation
    for (i = 1; i < obj->header.e_shnum; i++)
    {
        s = &obj->section[i];
        s->sh_offset = roundup(offset, s->sh_addralign);
        offset = s->sh_offset + s->sh_size;
    }
    return;
}

/*---------------------------------------------------------------------------*
  Name:         roundup

  Description:  Rounds up integers.

  Arguments:    val       Value
                align     Boundary value
  
  Returns:      Result
 *---------------------------------------------------------------------------*/
u32 roundup(u32 val, u32 align)
{
    if (align > 1)
    {
        u32     n = val % align;

        if (n > 0)
        {
            val += (align - n);
        }
    }
    return val;
}
