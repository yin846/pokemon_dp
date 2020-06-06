/*---------------------------------------------------------------------------*
  Project:  NitroDWC - base - init
  File:     dwc_init.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_init.c,v $
  Revision 1.8  12/15/2005 05:35:16  arakit
  Deleted the NVRAM clear function, DXC_Debug_ClearConsoleWiFiInfo(), for debug.
  Added  the debug function to destroy NVRAM, DWC_Debug_DWCInitError(), in order to test all return values in DWC_Init().
  Added DWC_Debug_DWCInitError().

  Revision 1.7  11/01/2005 10:38:48  arakit
  Corrected spelling of enum.

  Revision 1.6  10/06/2005 10:42:37  sasakit
  Buried the version.

  Revision 1.5  09/21/2005 07:56:02  sasakit
  Added function for deleting Wi-Fi user data of this console for debugging

  Revision 1.4  09/02/2005 11:14:23  sasakit
  Ceased calling NitroSDK initialization functions such as OS_Init() and clarified this change in the reference.

  Revision 1.3  08/31/2005 10:47:01  sasakit
  The return value of dwc_init has been set.

  Revision 1.2  08/31/2005 05:12:30  sasakit
  Supported move of util_wifiidtool.h

  Revision 1.1  08/31/2005 03:29:44  sasakit
  The module was split so that code used for initialization could be separated by overlay.

  Revision 1.4  08/23/2005 06:40:05  nakata
  Fixed the problem that is related to BM initialize.
  Before : Pass 32 BIT  aligned memory space to BM as work memory.
  After  : Pass 32 BYTE aligned memory space to BM as work memory.

  Revision 1.3  08/22/2005 13:06:57  sasakit
  Returned to creation method used when there was no WiFiID.

  Revision 1.2  08/20/2005 07:59:56  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files

  Revision 1.1  08/19/2005 13:12:10  sasakit
  Revised for release

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>

#include <bm/dwc_bm_init.h>
#include <bm/util_wifiidtool.h>
#include <base/dwc_report.h>
#include <base/dwc_init.h>

#include <dwc_version.h>

#include <nitro/version_begin.h>
static char id_string [] = SDK_MIDDLEWARE_STRING("NINTENDO", DWC_VERSION_STRING);
#include <nitro/version_end.h>

extern BOOL DWCi_AUTH_MakeWiFiID( void* work );
int
DWC_Init( void* work )
{
    int ret;
    BOOL created = FALSE;

    SDK_USING_MIDDLEWARE(id_string);

    SDK_ASSERTMSG( ( (u32)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

    ret = DWC_BM_Init( work );

    if ( DWC_Auth_CheckWiFiIDNeedCreate() )
    {
        DWC_Printf( DWC_REPORTFLAG_INFO, "Create UserID\n" );
        DWCi_AUTH_MakeWiFiID( work );

        created = TRUE;
    }
    else
    {
        DWC_Printf( DWC_REPORTFLAG_INFO, "Read UserID from DS\n" );
    }

    if ( ret < 0 )
    {
        // NVRAM contents are destroyed.
        if ( created )
        {
            return DWC_INIT_RESULT_DESTROY_USERID;
        }
        else
        {
            return DWC_INIT_RESULT_DESTROY_OTHER_SETTING;
        }
    }
    else if ( created )
    {
        // Created for some reason even though NVRAM is no problem
        return DWC_INIT_RESULT_CREATE_USERID;
    }

    return DWC_INIT_RESULT_NOERROR;
}

extern BOOL  DWCi_BACKUPlInit(void* work);
extern BOOL  DWCi_BACKUPlRead(void* mem);
extern BOOL  DWCi_BACKUPlWritePage(const void* data, const BOOL* page, void* work);
static    char s_work[0x400] ATTRIBUTE_ALIGN(32);

// A debug function to destroy the special point in NVRAM and check the error return value of DWC_Init().
void DWC_Debug_DWCInitError( void* work, int dwc_init_error )
{
    BOOL needCrc = FALSE;
    int  crcPage;

    if ( dwc_init_error == DWC_INIT_RESULT_NOERROR ) return;

    DWCi_BACKUPlInit(work);  // Initializes NVRAM access.

    if ( dwc_init_error == DWC_INIT_RESULT_CREATE_USERID )
    {
        DWCi_BACKUPlRead( s_work );  // NVRAM read

        // Clears only the members notAttestedId and attestedUserId from the WiFi Information on page 0.
        MI_CpuClear8( &s_work[0xf0], 10 );
        s_work[0xf0+0x0a] &= ~0x3f;

        needCrc = TRUE;
        crcPage = 0;     // Rewrites the data on page 0.
    }
    else if ( dwc_init_error == DWC_INIT_RESULT_DESTROY_USERID )
    {
        DWCi_BACKUPlRead( s_work );  // NVRAM read

        // Clears only the members notAttestedId and attestedUserId from the WiFi Information on page 1.
        MI_CpuClear8( &s_work[0x100+0xf0], 10 );
        s_work[0x100+0xf0+0x0a] &= ~0x3f;

        // After this, the data on page 0 is destroyed, so these are copied to page 0.
        // The correct value for page 0 is set to ap.state on page 1.
        s_work[0x100+0xef] = 1;

        needCrc = TRUE;
        crcPage = 1;     // Rewrites the data on page 1.
    }

    // When WiFi information is rewritten, CRC is recalculated and NVRAM is rewritten.
    if ( needCrc )
    {
        u16 hash;
        BOOL page[4] = { FALSE, FALSE, FALSE, FALSE };
        MATHCRC16Table crc16_tbl;
        
        // Initializes CRC
    	MATH_CRC16InitTable( &crc16_tbl );

        // Recalculates CRC
  		hash = MATH_CalcCRC16( &crc16_tbl, (u16 *)&s_work[crcPage*0x100], 0xFE );
        *(u16 *)(&s_work[crcPage*0x100+0xFE]) = hash;

        // Only NVRAM on the specified page is rewritten.
        page[crcPage] = TRUE;
        DWCi_BACKUPlWritePage( s_work, page, work );
    }

    // In order to complete DWC_BM_Init() with something other than DWC_INIT_OK, only page 0 is cleared.
    if ( dwc_init_error == DWC_INIT_RESULT_DESTROY_USERID ||
         dwc_init_error == DWC_INIT_RESULT_DESTROY_OTHER_SETTING )
    {
        BOOL page[4] = { TRUE, FALSE, FALSE, FALSE };
        
        MI_CpuClear8( s_work, sizeof(s_work) );
        DWCi_BACKUPlWritePage( s_work, page, work );
    }
}
