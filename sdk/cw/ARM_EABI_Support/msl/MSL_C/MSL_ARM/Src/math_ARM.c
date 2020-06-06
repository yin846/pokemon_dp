/* Metrowerks Standard Library
 * Copyright © 1995-2004 Metrowerks Corporation.  All rights reserved.
 *
 * $Date: 2004/02/03 21:04:41 $
 * $Revision: 1.6.2.3 $
 */

/* $Id: math_ARM.c,v 1.6.2.3 2004/02/03 21:04:41 ceciliar Exp $ */

/*	force definitions to be generated */
#undef _MSL_USE_INLINE
#undef _MSL_INLINE
#undef _MSL_MATH_CDECL

#define _MSL_USE_INLINE 	1
#define _MSL_INLINE 		_MSL_IMP_EXP_C __declspec(weak)

/*	special calling convention for math lib? */
#define _MSL_MATH_CDECL 	_MSL_CDECL

/*	pragmas to control codegen for this library? */

/*	include header and instantiate _MSL_INLINEs as functions */
#include <cmath>

_MSL_BEGIN_NAMESPACE_STD
_MSL_BEGIN_EXTERN_C


_MSL_IMP_EXP_C  double      _MSL_MATH_CDECL log2(double x)
{

	#if _MSL_LITTLE_ENDIAN
		static const unsigned long f[2] = {0x652b82fe, 0x3ff71547};
	#else
		static const unsigned long f[2] = {0x3ff71547, 0x652b82fe};
	#endif
	
	return log(x) * *(double*)f;
}

_MSL_INLINE double  _MSL_MATH_CDECL scalbln (double x, long n) 
{
	return ldexp(x,(int)n);
}

_MSL_IMP_EXP_C  double      _MSL_MATH_CDECL nan(const char* x)
{
	#pragma unused(x)
	return NAN;															/*- hh 010411 -*/		
}


_MSL_END_EXTERN_C
_MSL_END_NAMESPACE_STD

/* Change record: 
 * cc  030815 Added _MSL_LITTLE_ENDIAN check to log2
 */