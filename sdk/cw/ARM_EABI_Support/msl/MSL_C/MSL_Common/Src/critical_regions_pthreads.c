/* Metrowerks Standard Library
 * Copyright © 1995-2004 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2004/04/21 13:34:07 $
 * $Revision: 1.1.2.3 $
 */

#include <critical_regions.h>

#if _MSL_PTHREADS

#include <pthread.h>


	pthread_mutex_t __cs[num_critical_regions];
	pthread_t __cs_id[num_critical_regions];
	int __cs_ref[num_critical_regions];
	
	static void __msl_setup_criticals(void)
	{
		__init_critical_regions();
	}
	#pragma CALL_ON_MODULE_BIND __msl_setup_criticals
	
	#ifdef __mwlinker__
		#pragma INIT_BEFORE_TERM_AFTER on
	#endif

#endif /* _MSL_THREADSAFE */

/* Change record:
 * JWW 020331 Initialize Mach-O regions on module bind and keep refcount info for locked threads
 * JWW 020423 Use new INIT_BEFORE_TERM_AFTER to setup critical regions first on Mach-O
 */