/*---------------------------------------------------------------------------*
  Project:  NITRO-System - Graphics foundation
  File:     gfd_VramMan.h

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: gfd_VramMan.h,v $
  Revision 1.10  09/29/2004 02:03:51 AM  kitani_toshikazu
  Added comments

  Revision 1.9  09/02/2004 04:39:15 AM  kitani_toshikazu
  Changed the definition of debug macros when released.

  Revision 1.8  09/01/2004 05:10:49 AM  kitani_toshikazu
  Added a bit array palette manager and separated out the shared portions.

  Revision 1.7  08/30/2004 12:17:03 AM  kitani_toshikazu
  Corrected a spelling error in the comments

  Revision 1.6  2004/07/15 10:10:54 AM  nishida
  Defined a constant that indicates the direction of memory allocation from the VRAM

  Revision 1.5  07/10/2004 02:00:37 AM  nishida
  Organization of headers

  Revision 1.4  06/23/2004 08:14:05 AM  kitani_toshikazu
  Renamed the global variables

  Revision 1.3  2004/06/23 06:47:56  nishida_kenji
  Fix header comments.

  Revision 1.2  06/16/2004 05:15:25 AM  kitani_toshikazu
  Palette-related additions. Arrangement of sources.

  Revision 1.1  06/09/2004 09:35:59 AM  kitani_toshikazu
  Added gfd

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef NNS_GFD_VRAMMAN_H_
#define NNS_GFD_VRAMMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

//
// Switch to enable the debug output
// Debug output is enabled when defining.
//
//#define NNS_GFD_DEBUG


#ifdef SDK_DEBUG

    #define NNS_GFD_PRINTF( ... )   OS_Printf( __VA_ARGS__ )
    #define NNS_GFD_WARNING( ... )  OS_Warning( __VA_ARGS__ )

#else // SDK_DEBUG

    #define NNS_GFD_PRINTF( ... )   ((void) 0)
    #define NNS_GFD_WARNING( ... )  ((void) 0)

#endif // SDK_DEBUG
//------------------------------------------------------------------------------
#ifdef NNS_GFD_DEBUG
    #define NNS_GFD_DBGMSG( ... )   OS_Printf( __VA_ARGS__ )
#else // NNS_GFD_DEBUG
    #define NNS_GFD_DBGMSG( ... )   ((void) 0)
#endif // NNS_GFD_DEBUG





//
//	Constant that indicates the direction of memory allocation from VRAM
// ----------------------------------------------------------------------------
enum
{
	NNS_GFD_ALLOC_FROM_HIGH		= 0,		// Allocate memory from the top of VRAM
	NNS_GFD_ALLOC_FROM_LOW		= 1			// Allocate memory from the bottom of VRAM
};


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif // NNS_GFD_VRAMMAN_H_

