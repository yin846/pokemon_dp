/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - init
  File:     dwc_init.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_init.h,v $
  Revision 1.6  12/15/2005 05:35:29  arakit
  Deleted the NVRAM clear function, DXC_Debug_ClearConsoleWiFiInfo(), for debug.
  Added  the debug function to destroy NVRAM, DWC_Debug_DWCInitError(), in order to test all return values in DWC_Init().
  Added DWC_Debug_DWCInitError().

  Revision 1.5  11/01/2005 10:37:57  arakit
  Corrected spelling of enum.

  Revision 1.4  09/21/2005 07:54:59  sasakit
  Added function for deleting Wi-Fi user data of this console for debugging

  Revision 1.3  09/03/2005 01:49:19  sasakit
  Fix typo cplusplu -> cplusplus

  Revision 1.2  08/31/2005 03:35:41  sasakit
  Changed specifications so that memory-related functions do not need to be set when DWC_Init() is called,

  Revision 1.1  08/19/2005 13:14:44  sasakit
  Revised for release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_INIT_H_
#define DWC_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    DWC_INIT_RESULT_NOERROR,
    DWC_INIT_RESULT_CREATE_USERID,
    DWC_INIT_RESULT_DESTROY_USERID,
    DWC_INIT_RESULT_DESTROY_OTHER_SETTING,
    DWC_INIT_RESULT_LAST,

    DWC_INIT_RESULT_DESTORY_USERID = DWC_INIT_RESULT_DESTROY_USERID,
    DWC_INIT_RESULT_DESTORY_OTHER_SETTING = DWC_INIT_RESULT_DESTROY_OTHER_SETTING
};

extern int DWC_Init( void* work );

extern void DWC_Debug_DWCInitError( void* work, int dwc_init_error );

#ifdef __cplusplus
}
#endif

#endif // DWC_INIT_H_
