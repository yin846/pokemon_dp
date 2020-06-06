/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - dwc_core
  File:     dwc_core.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_core.h,v $
  Revision 1.16  06/30/2006 03:03:31  takayama
  There was a dependency on the include order, so dwc_main.h was moved to the end.

  Revision 1.15  06/29/2006 09:50:18  takayama
  Removed dwc_common during the NITRODWC_NOGS options

  Revision 1.14  04/27/2006 11:17:33  takayama
  Revised the NITRODWC_NOGS options from being browser-oriented to more standard

  Revision 1.13  04/21/2006 01:33:43  takayama
  Made adjustments to the build environment on NITORDWC_NOGS

  Revision 1.12  04/20/2006 13:21:12  takayama
  Support for the NITRODWC_NOGS options

  Revision 1.11  03/07/2006 01:18:16  nakata
  Performed necessary changes resulting from collecting added NAS functionality into dwc_nasfunc.c/dwc_nasfunc.h.

  Revision 1.10  11/02/2005 02:49:33  arakit
  Adjusted the file's include relationships.

  Revision 1.9  2005/09/05 05:17:22  onozawa
  Added dwc_ghttp.h

  Revision 1.8  08/20/2005 07:01:19  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_CORE_H_
#define DWC_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <base/dwc_connectinet.h>
#include <base/dwc_error.h>
#include <base/dwc_init.h>
#include <base/dwc_memfunc.h>
#include <base/dwc_nasfunc.h>
#include <base/dwc_report.h>

#ifndef NITRODWC_NOGS
#include <base/dwc_account.h>
#include <base/dwc_base_gamespy.h>
#include <base/dwc_common.h>
#include <base/dwc_friend.h>
#include <base/dwc_ghttp.h>
#include <base/dwc_login.h>
#include <base/dwc_match.h>
#include <base/dwc_transport.h>
#include <base/dwc_main.h>
#endif


#ifdef __cplusplus
}
#endif


#endif // DWC_CORE_H_
