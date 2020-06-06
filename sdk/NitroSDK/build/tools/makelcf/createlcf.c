/*---------------------------------------------------------------------------*
  Project:  NitroSDK - tools - makelcf
  File:     createlcf.c

  Copyright 2003-2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: createlcf.c,v $
  Revision 1.19  04/06/2006 09:05:56  kitase_hirotake
  support for .itcm.bss .dtcm.bss .wram.bss

  Revision 1.18  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.17  09/01/2005 04:30:52 AM  yasu
  Support for Overlay Group

  Revision 1.16  08/26/2005 11:23:11 AM  yasu
  overlay support for ITCM/DTCM

  Revision 1.15  06/20/2005 12:21:48 AM  yasu
  Changed Surffix to Suffix

  Revision 1.14  02/28/2005 05:26:03  yosizaki
  do-indent.

  Revision 1.13  09/24/2004 13:24:04 AM  yasu
  Support NitroStaticInit

  Revision 1.12  08/25/2004 04:01:34 AM  yasu
  Changes in handling when the object is aaa.o (*)

  Revision 1.11  07/10/2004 04:06:17 AM  yasu
  Support command 'Library'
  Support modifier ':x' on template
  Fix up line continue '\'

  Revision 1.10  07/02/2004 07:34:53 AM  yasu
  Support OBJECT( )

  Revision 1.9  06/24/2004 07:18:54 AM  yasu
  Support keyword "Autoload"

  Revision 1.8  06/14/2004 11:28:45 AM  yasu
  support section filter "FOREACH.STATIC.OBJECTS=.sectionName"

  Revision 1.7  03/26/2004 05:07:11 AM  yasu
  support variables like as -DNAME=VALUE

  Revision 1.6  02/13/2004 07:13:03 AM  yasu
  support SDK_IRQ_STACKSIZE

  Revision 1.5  02/05/2004 07:09:03 AM  yasu
  change SDK prefix iris -> nitro

  Revision 1.4  01/15/2004 10:51:45 AM  yasu
  Implementation of a static StackSize

  Revision 1.3  01/14/2004 12:38:08 AM  yasu
  Change OverlayName->OverlayDefs

  Revision 1.2  01/07/2004 13:10:17 AM  yasu
  fix all warning at compile -Wall

  Revision 1.1  01/05/2004 02:32:59 AM  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include	<stdio.h>
#include	<strings.h>
#include	<io.h>                 // setmode()
#include	<fcntl.h>              // setmode()
#include	<stdarg.h>             // va_start(),va_end()
#include	<nitro_win32.h>
#include	"makelcf.h"
#include	"defval.h"
#include	"tlcf.tab.h"

static void PutString(const char *str);
static void PutAddress(u32 address);
static BOOL PutObject(const tObject * object, const char *secName);
static void PutStaticObject(const char *secName);
static void PutOverlayName(void);
static void PutOverlayGroup(void);
static void PutOverlayObject(const char *secName);
static void PutOverlayAddress(void);
static BOOL isObjectOK(const tObject * obj, const char *section, char filter_type);
static BOOL isSectionWildcard(const char *section);
static BOOL isSectionRegular(const char *section);
static int ForeachObjects(tObject ** ptrObject, tObject * start, int n);
static int ForeachStaticObjects(int n);
static int ForeachOverlayObjects(int n);
static int ForeachOverlays(int n);
static int PutToken(int n);
static void PutTokenBuffer(int start, int end);
static void PutTokenBufferAll(void);


FILE   *fp_out;
tObject *StaticObject = NULL;
tObject *StaticLibrary = NULL;
tOverlay *Autoload = NULL;
tObject *AutoloadObject = NULL;
tObject *AutoloadLibrary = NULL;
tOverlay *Overlay = NULL;
tObject *OverlayObject = NULL;
tObject *OverlayLibrary = NULL;

void lcf_error(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fprintf(stderr, "\n");
}

/*============================================================================
 *  Put Tokens
 */
static void PutString(const char *str)
{
    if (str)
        fprintf(fp_out, "%s", str);
}

static void PutAddress(u32 address)
{
    fprintf(fp_out, "0x%08lx", address);
}

static void PutNumber(u32 num)
{
    fprintf(fp_out, "%ld", num);
}


static int GetCharPos(const char *str, char target)
{
    int     n = 0;

    if (str)
    {
        while ('\0' != str[n] && target != str[n])
        {
            n++;
        }
    }
    return n;
}

static char *GetObjectParam(const char *str, char start, char end)
{
    int     pos_start = GetCharPos(str, start);
    int     pos_end = GetCharPos(str, end);
    int     len = pos_end - pos_start;
    char   *buffer;

    if (len > 0)
    {
        buffer = Alloc(len);
        strncpy(buffer, str + pos_start + 1, len - 1);
        buffer[len - 1] = '\0';
        return buffer;
    }
    return NULL;
}

static char *GetObjectSection(const char *str)
{
    return GetObjectParam(str, '=', ':');       // return NULL if no section
}

static char GetObjectModifier(const char *str)
{
    char   *mod = GetObjectParam(str, ':', '\0');
    char    ret;

    ret = mod ? mod[0] : '\0';
    Free(&mod);

    return ret;

}

static tMemType GetMemTypeParam(const char *str)
{
    char   *mod = GetObjectParam(str, '=', ':');
    int     i;

    static struct
    {
        tMemType type;
        char   *name;
    }
    table[] =
    {
        {
        MEMTYPE_MAIN, "MAIN"}
        ,
        {
        MEMTYPE_MAINEX, "MAINEX"}
        ,
        {
        MEMTYPE_ITCM, "ITCM"}
        ,
        {
        MEMTYPE_DTCM, "DTCM"}
        ,
        {
        MEMTYPE_ITCM_BSS, "ITCM.BSS"}
        ,
        {
        MEMTYPE_DTCM_BSS, "DTCM.BSS"}
        ,
        {
        MEMTYPE_SHARED, "SHARED"}
        ,
        {
        MEMTYPE_WRAM, "WRAM"}
        ,
        {
        MEMTYPE_WRAM_BSS, "WRAM.BSS"}
        ,
        {
        MEMTYPE_VRAM, "VRAM"}
        ,
        {
        MEMTYPE_NONE, "*"}
    ,};

    for (i = 0; table[i].type != MEMTYPE_NONE; i++)
    {
        if (isSame(mod, table[i].name))
        {
            break;
        }
    }
    Free(&mod);

    return table[i].type;
}

//
// Decides which memory region the section specified by name, address belongs to
//
static tMemType GetSectionMemType(const char *name, u32 address)
{
    BOOL    isARM9 = (Static.address < 0x02300000);     // Actual decision value
    tMemType memtype = MEMTYPE_NONE;

    if (isARM9)
    {
        if (isSame(name, "ITCM"))
            memtype = MEMTYPE_ITCM;
        else if (isSame(name, "DTCM"))
            memtype = MEMTYPE_DTCM;
        else if (isSame(name, "WRAM"))
            memtype = MEMTYPE_WRAM;
        else if (isSame(name, "ITCM.BSS"))
            memtype = MEMTYPE_ITCM_BSS;
        else if (isSame(name, "DTCM.BSS"))
            memtype = MEMTYPE_DTCM_BSS;
        else if (isSame(name, "WRAM.BSS"))
            memtype = MEMTYPE_WRAM_BSS;
        else if (HW_ITCM_IMAGE <= address && address < HW_ITCM_END)
            memtype = MEMTYPE_ITCM;
        else if (HW_MAIN_MEM_SUB <= address && address < HW_MAIN_MEM_SUB_END)
            memtype = MEMTYPE_DTCM;
        else if (HW_MAIN_MEM_MAIN <= address && address < HW_MAIN_MEM_MAIN_END)
            memtype = MEMTYPE_MAIN;
        else if (HW_MAIN_MEM_MAIN_END <= address && address < HW_MAIN_MEM_DEBUGGER)
            memtype = MEMTYPE_MAINEX;
        else if (HW_MAIN_MEM_SHARED <= address && address < HW_MAIN_MEM_SHARED_END)
            memtype = MEMTYPE_SHARED;
        else if (HW_WRAM <= address && address < HW_WRAM_END)
            memtype = MEMTYPE_WRAM;
        else if (HW_BG_PLTT <= address && address < HW_CTRDG_ROM)
            memtype = MEMTYPE_VRAM;
    }
    else
    {
        if (isSame(name, "WRAM"))
            memtype = MEMTYPE_WRAM;
        else if (HW_MAIN_MEM_MAIN <= address && address < HW_MAIN_MEM_SUB_END)
            memtype = MEMTYPE_MAIN;
        else if (HW_MAIN_MEM_SHARED <= address && address < HW_MAIN_MEM_SHARED_END)
            memtype = MEMTYPE_SHARED;
        else if (HW_WRAM <= address && address < HW_WRAM_END)
            memtype = MEMTYPE_WRAM;
        else if (HW_BG_PLTT <= address && address < HW_CTRDG_ROM)
            memtype = MEMTYPE_VRAM;
    }
    return memtype;
}

//
// Return overlay that corresponds to the section specified by name
//
static tOverlay *GetOverlayByName(const char *name)
{
    tOverlay *t;

    for (t = OverlayList.head; t; t = t->next)
    {
        if (isSame(t->name, name))
            return t;
    }
    for (t = AutoloadList.head; t; t = t->next)
    {
        if (isSame(t->name, name))
            return t;
    }
    return NULL;
}

//
// Decides which memory region the section specified by name belongs to
//
static tMemType RecgSectionMemTypeByName(const char *name)
{
    tOverlay *overlay;
    tMemType memtype, mt;
    tAfter *after;

    if (isSame(name, Static.name))
    {
        Static.memtype = GetSectionMemType(Static.name, Static.address);
        return Static.memtype;
    }

    overlay = GetOverlayByName(name);
    if (!overlay)
    {
        lcf_error("Unknown section %s, Ignored.", name);
        return MEMTYPE_NONE;
    }

    if (overlay->memtype == MEMTYPE_NONE)
    {
        memtype = GetSectionMemType(overlay->name, overlay->address);

        if (memtype == MEMTYPE_NONE)
        {
            overlay->memtype = MEMTYPE_DUMMY;

            for (after = overlay->afters.head; after; after = after->next)
            {
                mt = RecgSectionMemTypeByName(after->name);

                if (mt == MEMTYPE_DUMMY)
                {
                    // Looping
                    lcf_error("Recursive AFTER reference in section %s, Ignored.", name);
                    overlay->memtype = MEMTYPE_NONE;
                    return MEMTYPE_NONE;
                }
                else if (memtype == MEMTYPE_NONE)
                {
                    memtype = mt;
                }
                else if (memtype != mt)
                {
                    // Different memtype sections are collected in AFTER
                    lcf_error("Bad address mapping in section %s, Ignored.", name);
                    overlay->memtype = MEMTYPE_NONE;
                    return MEMTYPE_NONE;
                }
            }
        }
        overlay->memtype = memtype;
    }
    return overlay->memtype;
}


//====== COMMONS ======

/*---------------------------------------------------------------------------*
  Name:         isObjectOK

  Description:  Determines whether or not the designated object matches with
                the conditions designated by the section or the filter_type.

  Arguments:    obj         : object
                              (What is actually determined is
                              obj->sectionName, obj->objextType )
                section     : Designated section name
                filter_type : Qualifying options

  Returns:      None.
 *---------------------------------------------------------------------------*/

static BOOL isObjectOK(const tObject * obj, const char *section, char filter_type)
{
    //
    //  Checks pertaining to section naming
    //          A "*" can be used as the section wildcard.
    //          When the section is "*", all obj will pass
    //          When obj->section is  '*', specify section
    //          and will pass when general section (.text, .data, .bss, .init, etc.)
    //
    if (isSectionWildcard(section) ||
        (isSectionWildcard(obj->sectionName) && isSectionRegular(section)) ||
        isSame(obj->sectionName, section))
    {
        switch (filter_type)
        {
        case 'f':                     // only regular file
            if (obj->objectType == OBJTYPE_FILE)
                return TRUE;
            break;

        case '\0':                    // OK if NULL
            return TRUE;

        default:
            lcf_error("Unknown type setting Ignored.");
            break;
        }
    }
    return FALSE;
}

static BOOL isSectionWildcard(const char *section)
{
    return !section || isSame(section, "*");
}

static BOOL isSectionRegular(const char *section)
{
    return (isSame(section, ".text") ||
            isSame(section, ".rodata") ||
            isSame(section, ".sdata") ||
            isSame(section, ".data") ||
            isSame(section, ".sbss") ||
            isSame(section, ".bss") ||
            isSame(section, ".init") ||
            isSame(section, ".exception") || isSame(section, ".ctor") || isSame(section, ".sinit"));
}

// Foreach Objects
static int ForeachObjects(tObject ** ptrObject, tObject * start, int n)
{
    tTokenBuffer *token;
    char   *section;
    char    filter;

    token = &tokenBuffer[n];
    section = GetObjectSection(token->string);
    filter = GetObjectModifier(token->string);

    debug_printf("section =[%s]\n", section ? section : "");
    debug_printf("filter  =[%c]\n", filter ? filter : ' ');

    while (start)
    {
        if (isObjectOK(start, section, filter))
        {
            *ptrObject = start;
            PutTokenBuffer(n + 1, token->loop_end);
        }

        start = start->next;
    }

    Free(&section);

    *ptrObject = NULL;
    return token->loop_end;
}

// Object
static BOOL PutObject(const tObject * object, const char *sectionName)
{
    if (object)
    {
        char    modifier = GetObjectModifier(sectionName);

        PutString(ResolveStringModifier(object->objectName, modifier));

        if (isNeedSection(object))
        {
            char   *section = GetObjectSection(sectionName);

            if (section)
            {
                PutString(" (");
                PutString(section);
                PutString(")");
                Free(&section);
            }
        }
        return TRUE;
    }
    return FALSE;
}

// Address
static void PutSectionAddress(tOverlay * overlay)
{
    if (overlay->address)
    {
        PutAddress(overlay->address);
    }
    else
    {
        tAfter *t = overlay->afters.head;
        if (t)
        {
            fprintf(fp_out, "AFTER(%s", t->name);
            while (NULL != (t = t->next))
            {
                fprintf(fp_out, ",%s", t->name);
            }
            fprintf(fp_out, ")");
        }
    }
}


//======== STATIC ========//

// Object
static void PutStaticObject(const char *sectionName)
{
    if (!PutObject(StaticObject, sectionName))
    {
        lcf_error("No <FOREACH.STATIC.OBJECTS> Ignored.");
    }
}

// Foreach Objects
static int ForeachStaticObjects(int n)
{
    return ForeachObjects(&StaticObject, Static.objects.head, n);
}

// Library
static void PutStaticLibrary(const char *sectionName)
{
    if (!PutObject(StaticLibrary, sectionName))
    {
        lcf_error("No <FOREACH.STATIC.LIBRARIES> Ignored.");
    }
}

// Foreach Libraries
static int ForeachStaticLibraries(int n)
{
    return ForeachObjects(&StaticLibrary, Static.libraries.head, n);
}


//======== AUTOLOAD ========//

// Id
static void PutAutoloadId(void)
{
    if (Autoload)
        PutNumber(Autoload->id);
    else
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
}

// Name
static void PutAutoloadName(void)
{
    if (Autoload)
        PutString(Autoload->name);
    else
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
}

// Address
static void PutAutoloadAddress(void)
{
    if (Autoload)
        PutSectionAddress(Autoload);
    else
        lcf_error("No <FOREACH.AUTOLOADS> Ignored.");
}

// Object
static void PutAutoloadObject(const char *sectionName)
{
    if (!PutObject(AutoloadObject, sectionName))
    {
        lcf_error("No <FOREACH.AUTOLOAD.OBJECTS> Ignored.");
    }
}

// Foreach Objects
static int ForeachAutoloadObjects(int n)
{
    return ForeachObjects(&AutoloadObject, Autoload->objects.head, n);
}

// Library
static void PutAutoloadLibrary(const char *sectionName)
{
    if (!PutObject(AutoloadLibrary, sectionName))
    {
        lcf_error("No <FOREACH.AUTOLOAD.LIBRARIES> Ignored.");
    }
}

// Foreach Libraries
static int ForeachAutoloadLibraries(int n)
{
    return ForeachObjects(&AutoloadLibrary, Autoload->libraries.head, n);
}

// Foreach Autoloads
static int ForeachAutoloads(int n)
{
    tOverlay *start = AutoloadList.head;

    while (start)
    {
        Autoload = start;
        PutTokenBuffer(n + 1, tokenBuffer[n].loop_end);
        start = start->next;
    }
    Autoload = NULL;
    return tokenBuffer[n].loop_end;
}


//======== OVERLAY ========//

// Id
static void PutOverlayId(void)
{
    if (Overlay)
        PutNumber(Overlay->id);
    else
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
}

// Name
static void PutOverlayName(void)
{
    if (Overlay)
        PutString(Overlay->name);
    else
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
}

// Group
static void PutOverlayGroup(void)
{
    if (Overlay)
    {
        // If a group has not been set, use the name of the overlay as a group
        PutString(Overlay->group ? Overlay->group : Overlay->name);
    }
    else
    {
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
    }
}

// Address
static void PutOverlayAddress(void)
{
    if (Overlay)
        PutSectionAddress(Overlay);
    else
        lcf_error("No <FOREACH.OVERLAYS> Ignored.");
}

// Object
static void PutOverlayObject(const char *sectionName)
{
    if (!PutObject(OverlayObject, sectionName))
    {
        lcf_error("No <FOREACH.OVERLAY.OBJECTS> Ignored.");
    }
}

// Foreach Objects
static int ForeachOverlayObjects(int n)
{
    return ForeachObjects(&OverlayObject, Overlay->objects.head, n);
}

// Library
static void PutOverlayLibrary(const char *sectionName)
{
    if (!PutObject(OverlayLibrary, sectionName))
    {
        lcf_error("No <FOREACH.OVERLAY.LIBRARIES> Ignored.");
    }
}

// Foreach Libraries
static int ForeachOverlayLibraries(int n)
{
    return ForeachObjects(&OverlayLibrary, Overlay->libraries.head, n);
}

// Foreach Overlays
static int ForeachOverlays(int n)
{
    tOverlay *start = OverlayList.head;
    tTokenBuffer *token = &tokenBuffer[n];
    tMemType memtype = GetMemTypeParam(token->string);

    while (start)
    {
        if (memtype == MEMTYPE_NONE || memtype == RecgSectionMemTypeByName(start->name))
        {
            Overlay = start;
            PutTokenBuffer(n + 1, tokenBuffer[n].loop_end);
        }
        start = start->next;
    }
    Overlay = NULL;
    return tokenBuffer[n].loop_end;
}


/*============================================================================
 *  Put Token List
 */
static int PutToken(int n)
{
    switch (tokenBuffer[n].id)
    {
    case tSTRING:
        PutString(tokenBuffer[n].string);
        break;

    case tSTATIC_NAME:
        PutString(Static.name);
        break;

    case tSTATIC_ADDRESS:
        PutAddress(Static.address);
        break;

    case tSTATIC_OBJECT:
        PutStaticObject(tokenBuffer[n].string);
        break;

    case tSTATIC_LIBRARY:
        PutStaticLibrary(tokenBuffer[n].string);
        break;

    case tSTATIC_STACKSIZE:
        PutNumber(Static.stacksize);
        break;

    case tSTATIC_IRQSTACKSIZE:
        PutNumber(Static.stacksize_irq);
        break;

    case tFOREACH_STATIC_OBJECTS:
        n = ForeachStaticObjects(n);
        break;

    case tFOREACH_STATIC_LIBRARIES:
        n = ForeachStaticLibraries(n);
        break;

    case tAUTOLOAD_ID:
        PutAutoloadId();
        break;

    case tAUTOLOAD_NAME:
        PutAutoloadName();
        break;

    case tAUTOLOAD_ADDRESS:
        PutAutoloadAddress();
        break;

    case tAUTOLOAD_OBJECT:
        PutAutoloadObject(tokenBuffer[n].string);
        break;

    case tAUTOLOAD_LIBRARY:
        PutAutoloadLibrary(tokenBuffer[n].string);
        break;

    case tNUMBER_AUTOLOADS:
        PutNumber(AutoloadList.num);
        break;

    case tFOREACH_AUTOLOADS:
        n = ForeachAutoloads(n);
        break;

    case tFOREACH_AUTOLOAD_OBJECTS:
        n = ForeachAutoloadObjects(n);
        break;

    case tFOREACH_AUTOLOAD_LIBRARIES:
        n = ForeachAutoloadLibraries(n);
        break;

    case tOVERLAY_ID:
        PutOverlayId();
        break;

    case tOVERLAY_NAME:
        PutOverlayName();
        break;

    case tOVERLAY_GROUP:
        PutOverlayGroup();
        break;

    case tOVERLAY_ADDRESS:
        PutOverlayAddress();
        break;

    case tOVERLAY_OBJECT:
        PutOverlayObject(tokenBuffer[n].string);
        break;

    case tOVERLAY_LIBRARY:
        PutOverlayLibrary(tokenBuffer[n].string);
        break;

    case tNUMBER_OVERLAYS:
        PutNumber(OverlayList.num);
        break;

    case tFOREACH_OVERLAYS:
        n = ForeachOverlays(n);
        break;

    case tFOREACH_OVERLAY_OBJECTS:
        n = ForeachOverlayObjects(n);
        break;

    case tFOREACH_OVERLAY_LIBRARIES:
        n = ForeachOverlayLibraries(n);
        break;

    case tPROPERTY_OVERLAYDEFS:
        PutString(Property.overlaydefs);
        break;

    case tPROPERTY_OVERLAYTABLE:
        PutString(Property.overlaytable);
        break;

    case tPROPERTY_SUFFIX:
        PutString(Property.suffix);
        break;

    default:
        lcf_error("Unknown token\n");
    }
    return n;
}

static void PutTokenBuffer(int start, int end)
{
    int     i;

    for (i = start; i <= end; i++)
    {
        i = PutToken(i);
    }
}

static void PutTokenBufferAll(void)
{
    PutTokenBuffer(0, tokenBufferEnd - 1);
}


/*============================================================================
 *  Create LCF file
 */
int CreateLcfFile(const char *filename)
{
    if (filename)
    {
        if (NULL == (fp_out = fopen(filename, "wb")))
        {
            fprintf(stderr, "makelcf: Cannot write %s\n", filename);
            return 2;
        }
        PutTokenBufferAll();
        fclose(fp_out);
    }
    else
    {
        setmode(1, O_BINARY);
        fp_out = stdout;
        PutTokenBufferAll();
    }

    return 0;
}
