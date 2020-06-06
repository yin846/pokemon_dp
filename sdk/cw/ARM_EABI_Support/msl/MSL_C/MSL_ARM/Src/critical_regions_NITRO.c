/* Metrowerks Standard Library
 * Copyright © 1995-2004 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2005/01/18 20:57:07 $
 * $Revision: 1.1.2.2 $
 */

#include <critical_regions.h>

#include <nitro/os/common/thread.h>
#include <nitro/os/common/mutex.h>

#if _MSL_THREADSAFE

	OSMutex __cs[num_critical_regions];
	int __cs_id[num_critical_regions];
	int __cs_ref[num_critical_regions];
	
	static void __msl_setup_criticals(void)
	{
		__init_critical_regions();
	}
	
	#ifdef __mwlinker__
		#pragma INIT_BEFORE_TERM_AFTER on
	#endif

#endif /* _MSL_THREADSAFE */

/* Change record:
 * jrusso 050118 remove CALL_ON_MODULE_BIND pragma
 * JWW 020331 Initialize Mach-O regions on module bind and keep refcount info for locked threads
 * JWW 020423 Use new INIT_BEFORE_TERM_AFTER to setup critical regions first on Mach-O
 */
