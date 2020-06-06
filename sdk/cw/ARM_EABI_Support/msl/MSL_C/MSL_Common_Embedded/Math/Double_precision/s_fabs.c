#ifndef _No_Floating_Point  
/* @(#)s_fabs.c 1.2 95/01/04 */
/* $Id: s_fabs.c,v 1.6.4.1 2004/02/06 02:17:59 ceciliar Exp $ */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/*
 * fabs(x) returns the absolute value of x.
 */

#if !defined(__MIPS__)

#define _MSL_USE_INLINE 0		/*- ejs 030113 -*/

#include "fdlibm.h"

#ifndef __USING_INTRINSIC_FABS__  /*- cc 020729 -*/

#ifdef __STDC__
	 double   fabs(double x)	/* wrapper pow */
#else
	double fabs(x)
	double x;
#endif
{
	__HI(x) &= 0x7fffffff;
        return x;
}

#endif /* __USING_INTRINSIC_FABS__	*/ /*- cc 020729 -*/

#endif /* !defined(__MIPS__)	*/

#endif /* _No_Floating_Point  */