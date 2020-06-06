/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - memfunc
  File:     dwc_memfunc.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_memfunc.c,v $
  Revision 1.2  09/17/2005 07:42:15  sasakit
  Corrected problem when decreasing the size of memory using realloc.

  Revision 1.1  08/19/2005 06:32:35  sasakit
  Added files.

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <base/dwc_memfunc.h>

/** --------------------------------------------------------------------
  statics
  ----------------------------------------------------------------------*/
static DWCAllocEx s_alloc;
static DWCFreeEx  s_free;

void
DWC_SetMemFunc( DWCAllocEx alloc, DWCFreeEx free )
{
    s_alloc = alloc;
    s_free  = free;
}

void*
DWC_Alloc( DWCAllocType name,
           u32          size )
{
    SDK_ASSERTMSG( s_alloc, "DWC_MEMFUNC: Allocator function isn't defined.\n" );

    return s_alloc( name, size, 32 );
}

void*
DWC_AllocEx( DWCAllocType name,
             u32          size,
             int          align  )
{
    SDK_ASSERTMSG( s_alloc, "DWC_MEMFUNC: Allocator function isn't defined.\n" );

    return s_alloc( name, size, align );
}

void
DWC_Free( DWCAllocType name,
          void*        ptr,
          u32          size )
{
    SDK_ASSERTMSG( s_free, "DWC_MEMFUNC: Free function isn't defined.\n" );

    s_free( name, ptr, size );
}

void*
DWC_Realloc( DWCAllocType name,
             void*        ptr,
             u32          oldsize,
             u32          newsize )
{
    return DWC_ReallocEx( name, ptr, oldsize, newsize, 32 );
}

void*
DWC_ReallocEx( DWCAllocType name,
               void*        ptr,
               u32          oldsize,
               u32          newsize,
               int          align    )
{
    void* newptr;

    newptr = s_alloc( name, newsize, align );

    if ( newptr == NULL )
    {
        return NULL;
    }

    if ( ptr != NULL )
    {
        MI_CpuCopy8( ptr, newptr, newsize );

        s_free( name, ptr, oldsize );
    }

    return newptr;
}


void*
DWCi_GsMalloc  (size_t size)
{
    return DWC_Alloc( DWC_ALLOCTYPE_GS, (u32)size );
}
void*
DWCi_GsRealloc (void* ptr, size_t size)
{
    return DWC_Realloc( DWC_ALLOCTYPE_GS, ptr, (u32)size, (u32)size );
}
void
DWCi_GsFree    (void* ptr)
{
    DWC_Free( DWC_ALLOCTYPE_GS, ptr, 0 );
}
void*
DWCi_GsMemalign(size_t boundary, size_t size)
{
    // Although incomplete, these specifications should be good enough
    return DWC_AllocEx( DWC_ALLOCTYPE_GS, (u32)size, (int)boundary );
}
