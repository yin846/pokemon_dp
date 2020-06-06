/*---------------------------------------------------------------------------*
  Project:  NitroSDK - WM - libraries
  File:     wm_private.h

  Copyright 2003-2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: wm_private.h,v $
  Revision 1.27  11/01/2005 08:50:20 AM  seiki_masashi
  Added WM_DEBUG_INDICATES

  Revision 1.26  02/28/2005 05:26:35 AM  yosizaki
  do-indent.

  Revision 1.25  02/23/2005 12:48:14 PM  seiki_masashi
  Cleaned up debug display

  Revision 1.24  02/22/2005 02:14:05 AM  seiki_masashi
  small fix

  Revision 1.23  02/22/2005 01:57:28 AM  seiki_masashi
  small fix

  Revision 1.22  02/22/2005 01:53:47 AM  seiki_masashi
  Added WM_WARNING, WM_ASSERT
  Deleted DEBUG_WM_USE_PRINTF

  Revision 1.21  02/18/2005 12:27:45 PM  seiki_masashi
  Debug switch statement changed from #if to #ifdef

  Revision 1.20  02/03/2005 11:41:47 PM  ooe
  Revised copyright notice

  Revision 1.19  01/31/2005 12:53:06 AM  seiki_masashi
  Added debug flag definition

  Revision 1.18  08/18/2004 12:23:40 AM  seiki_masashi
  Moved part of constant to wm.h

  Revision 1.17  08/17/2004 12:49:11 PM  seiki_masashi
  Added constant

  Revision 1.16  08/12/2004 02:23:26 PM  seiki_masashi
  Added WM_DEBUG_INDICATES

  Revision 1.15  08/11/2004 04:24:24 AM  seiki_masashi
  Debug flag added

  Revision 1.14  08/07/2004 05:16:44 AM  seiki_masashi
  WM_DEBUG set to OFF

  Revision 1.13  08/07/2004 12:20:29 AM  seiki_masashi
  WM_HEADER_SEQ_NUM_MASK added

  Revision 1.12  08/06/2004 05:11:39 AM  seiki_masashi
  add WM_DEBUG switch

  Revision 1.11  08/05/2004 10:02:44 AM  seiki_masashi
  KS bit position in wmHeader changed from 0x0002 to 0x4000

  Revision 1.10  08/04/2004 01:33:34 AM  seiki_masashi
  VSYNC flag position changed
  Preparation for introducing port.

  Revision 1.9  07/28/2004 02:44:11 AM  terui
  Unnecessary definitions deleted.
  Definitions for WM internal states moved to wm.h

  Revision 1.8  07/27/2004 07:30:30  yasu
  Add #pragma  warn_padding off

  Revision 1.7  2004/07/11 03:12:23 AM  ooe
  Beacon-related CMDSTATE added

  Revision 1.6  07/10/2004 11:49:24 AM  terui
  Revision caused by change in installation location of Marionea

  Revision 1.5  07/08/2004 09:50:22 AM  terui
  Changes due to beginning of WM7 revision

  Revision 1.8  07/06/2004 08:11:47 AM  Ooe
  Support for WL-1.56.00

  Revision 1.7  07/06/2004 01:54:47 AM  Ooe
  WM header bit definition added

  Revision 1.6  07/03/2004 07:30:21 AM  Ooe
  CMDSTATE added

  Revision 1.5  06/24/2004 10:04:23 AM  Ooe
  Added WM_SetGameInfo()

  Revision 1.4  2004/06/21 06:40:20 AM  Ooe
  (none)

  Revision 1.3  06/19/2004 05:56:20 AM  Miya
  modified header file reference

  Revision 1.4  06/19/2004 04:05:16 AM  miya
  ver. up Marionea-1.48.00

  Revision 1.3  06/14/2004 04:18:44 AM  terui
  Undefine OS_Printf on FINALROM build mode.

  Revision 1.2  06/02/2004 02:29:51 AM  miya
  ver. up

  Revision 1.1  05/26/2004 08:54:40 AM  terui
  Additions accompanying incorporation of wireless communication

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef LIBRARIES_WM_PRIVATE_H_
#define LIBRARIES_WM_PRIVATE_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*===========================================================================*/


#include <nitro.h>

#ifdef SDK_ARM7
#include <nitro_wl/ARM7/WlLib.h>
#endif

/*---------------------------------------------------------------------------*
    Constant Definitions
 *---------------------------------------------------------------------------*/

//#define WM_DEBUG                // WM debug flag (mainly ARM7)
//#define WM_DEBUG_DATASHARING    // DataSharing-related debug message flag (ARM9)
//#define WM_DEBUG_CALLBACK       // ARM9 wireless PXI interrupt debug display flag (ARM9)
//#define WM_DEBUG_AIDBITMAP      // ARM9 AID BITMAP debug display flag related to the (ARM9)

/*---------------------------------------------------------------------------*
    Definition of macros
 *---------------------------------------------------------------------------*/

#ifdef SDK_DEBUG
#define WM_WARNING(...) \
    ( (void) ( WMi_Warning(__FILE__, __LINE__, __VA_ARGS__) ) )
#else
#define WM_WARNING(...)    ((void) 0)
#endif

#ifdef WM_DEBUG
#define WM_ASSERT(exp) \
    ( (void) ( (exp) || (WMi_Warning(__FILE__, __LINE__, "Failed assertion " #exp), 0) ) )
#define WM_ASSERTMSG(exp, ...) \
    ( (void) ( (exp) || (WMi_Warning(__FILE__, __LINE__, __VA_ARGS__), 0) ) )
#define WM_DPRINTF WMi_Printf
#else  // WM_DEBUG
#define WM_ASSERT(exp)     ((void) 0)
#define WM_ASSERTMSG(...)     ((void) 0)
#define WM_DPRINTF(...)     ((void) 0)
#endif // WM_DEBUG

#ifndef SDK_FINALROM
#define WMi_Printf      OS_TPrintf
#define WMi_Warning     OSi_TWarning
#else
#define WMi_Printf(...)    ((void) 0)
#define WMi_Warning(...)   ((void) 0)
#endif


/*===========================================================================*/

#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif /* LIBRARIES_WM_PRIVATE_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
