/*---------------------------------------------------------------------------*
  Project:  NitroSDK - bin2obj
  File:     bin2obj.c

  Copyright 2005.2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: bin2obj.c,v $
  Revision 1.6  01/18/2006 02:11:19  kitase_hirotake
  do-indent

  Revision 1.5  2005/06/14 00:57:07  yasu
  Added -s option

  Revision 1.4  2005/06/13 08:52:26  yasu
  Support for %f as symbol name

  Revision 1.3  2005/06/13 02:56:34  yasu
  Revised options, confirmed operation with test creation

  Revision 1.2  2005/06/10 12:55:42  yasu
  1st refactoring

  Revision 1.1  2005/06/10 00:43:01  yasu
  Initial version

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include "bin2obj.h"

/*---------------------------------------------------------------------------*
  Name:         main

  Description:  bin2obj  main
 *---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    Bin2ObjArgs args;
    BOOL    result;

    cook_args(&args, argc, argv);
    result = bin2obj(&args);
    free_args(&args);

    return result ? 0 : 1;
}

/*---------------------------------------------------------------------------*
  Name:         bin2obj

  Description:  Outputs object information.

  Arguments:    t     bin2obj argument after processing
  
  Returns:      TRUE succeeded   FALSE failed
 *---------------------------------------------------------------------------*/
BOOL bin2obj(const Bin2ObjArgs * t)
{
    Object  object;

    //
    // Create object data
    //
    object_init(&object);

    if (!add_datasec(&object,
                     t->section_rodata, t->section_rwdata,
                     t->symbol_begin, t->symbol_end, t->binary_filename, t->writable, t->align))
    {
        return FALSE;
    }

    map_section(&object);

    //
    // Output object
    //
    if (!output_object(&object, t->object_filename))
    {
        return FALSE;
    }

    return TRUE;
}
