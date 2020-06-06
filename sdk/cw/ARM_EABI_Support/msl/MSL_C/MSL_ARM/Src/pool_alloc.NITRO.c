/* Metrowerks Standard Library
 * Copyright � 1995-2004 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2004/04/07 16:25:04 $
 * $Revision: 1.1.2.1 $
 */

/*
 *	Routines
 *	--------
 *		__sys_alloc
 *		__sys_free
 *
 *	Note
 *	----
 *
 *		These routines, being platform-specific, were split off of pool_alloc.c.
 *
 */


#include <ansi_parms.h>
#include <pool_alloc.h>
#include <nitro/os/common/arena.h>
#include <nitro/os/common/alloc.h>

void * _MSL_CDECL __sys_alloc(size_t size)
{
	OS_Alloc(size);
}

void _MSL_CDECL __sys_free(void *p)
{
	OS_Free(p);
}

size_t _MSL_CDECL __sys_pointer_size(void* p)
{
	return OS_ReferentSize( OS_ARENA_MAIN, p);
}

/* Change record:
 * cc  040407 Added OS calls
 */
