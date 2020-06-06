/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - report
  File:     dwc_report.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_report.h,v $
  Revision 1.7  09/12/2005 05:05:03  sasakit
  Added DWC_Printf type.

  Revision 1.6  08/23/2005 13:52:21  arakit
  GameSpy debugging display has been introduced in DWC_Printf.

  Revision 1.5  08/20/2005 08:01:20  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files

  Revision 1.4  08/19/2005 11:05:00  sasakit
  Added Copyright

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_REPORT_H_
#define DWC_REPORT_H_

#include <nitro.h>

#ifdef  __cplusplus
extern "C" {
#endif


/** --------------------------------------------------------------------
  Flag for outputting dwc debug data
  ----------------------------------------------------------------------*/
enum
{
    DWC_REPORTFLAG_INFO        = 0x00000001,
    DWC_REPORTFLAG_ERROR       = 0x00000002,
    DWC_REPORTFLAG_DEBUG       = 0x00000004,
    DWC_REPORTFLAG_WARNING     = 0x00000008,
    DWC_REPORTFLAG_ACHECK      = 0x00000010,
    DWC_REPORTFLAG_LOGIN       = 0x00000020,
    DWC_REPORTFLAG_MATCH_NN    = 0x00000040,
    DWC_REPORTFLAG_MATCH_GT2   = 0x00000080,
    DWC_REPORTFLAG_TRANSPORT   = 0x00000100,
    DWC_REPORTFLAG_QR2_REQ     = 0x00000200,
    DWC_REPORTFLAG_SB_UPDATE   = 0x00000400,
    DWC_REPORTFLAG_SEND_INFO   = 0x00008000,
    DWC_REPORTFLAG_RECV_INFO   = 0x00010000,
    DWC_REPORTFLAG_UPDATE_SV   = 0x00020000,
    DWC_REPORTFLAG_CONNECTINET = 0x00040000,
    DWC_REPORTFLAG_AUTH        = 0x01000000,
    DWC_REPORTFLAG_AC          = 0x02000000,
    DWC_REPORTFLAG_BM          = 0x04000000,
    DWC_REPORTFLAG_UTIL        = 0x08000000,
    DWC_REPORTFLAG_GAMESPY     = 0x80000000,
    DWC_REPORTFLAG_ALL         = 0xffffffff
};

#ifndef SDK_FINALROM
void        DWC_SetReportLevel      ( u32 level );
void        DWC_Printf              ( u32 level, const char* fmt, ... );
#else
#define     DWC_SetReportLevel( level )         ((void)0)
#define     DWC_Printf( level, ... )        ((void)0)
#endif


#ifdef  __cplusplus
}       /* extern "C"*/
#endif

#endif // DWC_REPORT_H_
