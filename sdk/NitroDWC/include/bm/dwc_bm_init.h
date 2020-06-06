/*---------------------------------------------------------------------------*
  Project:  NitroDWC - bm - dwc_bm_init
  File:     dwc_bm_init.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_bm_init.h,v $
  Revision 1.2  2005/08/20 09:39:28  akimaru
  #define DWC_INIT_WORK_SIZE 0x700

  Revision 1.1  08/20/2005 08:01:20  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_BM_INIT_H_
#define DWC_BM_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
// Definitions
//=============================================================================
// Work memory size of initialization function
#define DWC_BM_INIT_WORK_SIZE 0x700
#define DWC_INIT_WORK_SIZE 0x700

//=============================================================================
// functions
//=============================================================================
//-----------------------------------------------------------------------------
// Function      : Initialization function
//                 Å¶ The work memory must at least DWC_INIT_WORK_SIZE in size
//                     must be 32-byte aligned
//                 Å¶ Work memory may be revoked after the function exits
// Arguments     : work  - Work memory
// Return values : int   -      0 : normal end
//                         -10002 : Normal end even though connection destination setting has disappeared
//                         -10003 : Normal end although User ID has disappeared
//                         -10000 : Ends on write error to DS backup memory
//                         -10001 : Ends on read error to DS backup memory
//-----------------------------------------------------------------------------
int  DWC_BM_Init(void* work);

#ifdef __cplusplus
}
#endif

#endif // DWC_BM_INIT_H_
