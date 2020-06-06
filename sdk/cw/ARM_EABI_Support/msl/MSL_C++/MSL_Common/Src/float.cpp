/* Metrowerks Standard Library
 * Copyright © 1995-2004 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2004/02/04 15:33:01 $
 * $Revision: 1.7.2.1 $
 */

// float.cpp

#ifdef __MACH__

#ifdef __cplusplus
	extern "C" {
#endif

long __double_huge[]	= {0x7FF00000, 0};
long __float_nan[]		= {0x7FFFFFFF};
long __float_huge[]		= {0x7f800000};

#ifdef __cplusplus
	}
#endif

#endif /* __MACH__ */