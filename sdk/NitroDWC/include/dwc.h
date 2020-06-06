/*---------------------------------------------------------------------------*
  Project:  NitroDWC - dwc - dwc
  File:     dwc.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc.h,v $
  Revision 1.8  2006/04/27 11:17:33  takayama
  Revised the NITRODWC_NOGS options from being browser-oriented to more standard

  Revision 1.7  04/26/2006 08:34:32  nakata
  Made changes so that DWC_Nd would operate even if it includes dwc.h

  Revision 1.6  04/21/2006 02:04:41  takayama
  Support for NITRODWC_NOGS

  Revision 1.5  2006/04/10 10:16:57  hayashi
  added the ranking module

  Revision 1.4  08/20/2005 07:04:03  sasakit
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h

  Revision 1.3  08/20/2005 07:01:18  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_H_
#define DWC_H_

//#include <auth/dwc_auth.h>
#include <util/dwc_utility.h>
#include <bm/dwc_bm_init.h>
#include <ac/dwc_ac.h>
#include <base/dwc_core.h>
#include <base/dwc_nd.h>
#ifndef NITRODWC_NOGS
#include <base/dwc_ranking.h>
#endif

#endif // DWC_H_
