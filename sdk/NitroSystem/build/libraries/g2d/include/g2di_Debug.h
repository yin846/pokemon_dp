/*---------------------------------------------------------------------------*
  Project:   NITRO-System - Graphics 2D
  
  File:     .c

  Copyright 2004 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: g2di_Debug.h,v $
  Revision 1.2  05/28/2004 05:21:36 AM  kitani_toshikazu
  Reflection of fixes for PR1 release.

  Revision 1.1  04/21/2004 07:20:08 AM  kitani_toshikazu
  Updated the folder structures related to the internal module

  Revision 1.2  04/21/2004 06:39:37 AM  kitani_toshikazu
  fix small bugs.

  Revision 1.1  04/21/2004 02:24:12 AM  kitani_toshikazu
  Initial check in.


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
 
//
// This module should be exposed inside the library only.
//

#ifndef NNS_G2DI_DEBUG_H_
#define NNS_G2DI_DEBUG_H_

#include <nitro.h>
//#define NNSI_G2D_DEBUG

#ifdef NNSI_G2D_DEBUG
    #define NNSI_G2D_DEBUGMSG0( ... )    OS_Printf( __VA_ARGS__ );
#else  // NNSI_G2D_DEBUG
    #define NNSI_G2D_DEBUGMSG0( ... )     ((void) 0)
#endif // NNSI_G2D_DEBUG

#define NNSI_G2D_DEBUGMSG1( ... )     ((void) 0)



#endif // NNS_G2DI_DEBUG_H_

