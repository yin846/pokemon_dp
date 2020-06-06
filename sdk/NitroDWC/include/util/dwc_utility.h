/*---------------------------------------------------------------------------*
  Project:  NitroDWC - util - dwc_utility
  File:     dwc_utility.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: dwc_utility.h,v $
  Revision 1.9  2005/09/13 13:12:56  tsukada
  Reduce size of work memory

  Revision 1.8  09/10/2005 15:07:44  tsukada
  Changed parameter specifications

  Revision 1.7  09/01/2005 14:22:17  tsukada
  New

  Revision 1.6  08/20/2005 08:01:21  sasakit
  Format of header includes has been made uniform.
  Changed to bm/dwc_init.h -> bm/dwc_bm_init.h
  Attached Copyright to the header.
  Name spacing for structures has been made as close to the rule as possible.
  Slightly revised the util_wifiidtool.h and dwc_backup.h files


  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef DWC_UTILITY_H_
#define DWC_UTILITY_H_
/*****************************************************************************/
/*                                                                           */
/*  DWC utility header file*/
/*      dwc_utility.h*/
/*          ver. 1.2*/
/*                                                                           */
/*****************************************************************************/

#ifdef  __cplusplus
extern  "C" {
#endif         // __cplusplus

//=============================================================================
// Definitions
//=============================================================================
// Work memory size
#define  DWC_UTILITY_WORK_SIZE    0x40000

// Language to be used
enum  DWC_LANGUAGE {
	DWC_LANGUAGE_JAPANESE,             // Japanese
	DWC_LANGUAGE_ENGLISH,              // English
	DWC_LANGUAGE_FRENCH,               // France
	DWC_LANGUAGE_GERMAN,               // German
	DWC_LANGUAGE_ITALIAN,              // Italian
	DWC_LANGUAGE_SPANISH               // Spanish
};

// Return value of function for starting the setup utility
enum  DWCUTILRESULT {
	DWC_UTIL_RESULT_SUCCESS =  0,      // Succeeded
	DWC_UTIL_RESULT_FAILED  = ~0       // Argument error
};

// Flags
enum  DWCUTILFLAG0 {
	DWC_UTIL_FLAG0_TOP      = 0x0000,
	DWC_UTIL_FLAG0_SETTING  = 0x0001
};
enum  DWCUTILFLAG1 {
	DWC_UTIL_FLAG1_RAKU     = 0x0001,
	DWC_UTIL_FLAG1_USA      = 0x0002
};

//-----------------------------------------------------------------------------
// Parameters
//-----------------------------------------------------------------------------
//For USA
// Start from top menu
#define  DWC_UTILITY_TOP_MENU_FOR_USA                                         \
	(DWC_UTIL_FLAG0_TOP | (DWC_UTIL_FLAG1_USA << 4))

// Start from selection of target for Wi-Fi connection
#define  DWC_UTILITY_SETTING_FOR_USA                                          \
	(DWC_UTIL_FLAG0_SETTING | (DWC_UTIL_FLAG1_USA << 4))

//For Europe
// Start from top menu
#define  DWC_UTILITY_TOP_MENU_FOR_EUR                                         \
	(DWC_UTIL_FLAG0_TOP)

// Start from selection of target for Wi-Fi connection
#define  DWC_UTILITY_SETTING_FOR_EUR                                          \
	(DWC_UTIL_FLAG0_SETTING)

//For Japan
// Start from top menu
#define  DWC_UTILITY_TOP_MENU_FOR_JPN                                         \
	(DWC_UTIL_FLAG0_TOP | (DWC_UTIL_FLAG1_RAKU << 4))

// Start from selection of target for Wi-Fi connection
#define  DWC_UTILITY_SETTING_FOR_JPN                                          \
	(DWC_UTIL_FLAG0_SETTING | (DWC_UTIL_FLAG1_RAKU << 4))

//For All Regions
// (Japanese cannot be used)
// Start from top menu
#define  DWC_UTILITY_TOP_MENU_COMMON                                          \
	(DWC_UTIL_FLAG0_TOP)

// Start from selection of target for Wi-Fi connection
#define  DWC_UTILITY_SETTING_COMMON                                           \
	(DWC_UTIL_FLAG0_SETTING)

#define  DWC_UTILITY_TOP_MENU_EXCEPT_JPN                                      \
	DWC_UTILITY_TOP_MENU_COMMON
#define  DWC_UTILITY_SETTING_EXCEPT_JPN                                       \
	DWC_UTILITY_SETTING_COMMON

//=============================================================================
// functions
//=============================================================================
//-----------------------------------------------------------------------------
// Function : Start setup utility
// Arguments : work - Start address of work memory
//                    NB: Must have a byte size larger than DWC_UTILITY_WORK_SIZE and
//                       must be 32-byte aligned
//                     * May be destroyed after function has ended processing
//        language  - Language used by the application
//        param     - Select from the list below
//                        DWC_UTILITY_TOP_MENU_FOR_USA
//                        DWC_UTILITY_SETTING_FOR_USA
//                        DWC_UTILITY_TOP_MENU_FOR_EUR
//                        DWC_UTILITY_SETTING_FOR_EUR
//                        DWC_UTILITY_TOP_MENU_FOR_JPN
//                        DWC_UTILITY_SETTING_FOR_JPN
//                        DWC_UTILITY_TOP_MENU_COMMON
//                        DWC_UTILITY_SETTING_COMMON
// Return values: int - 0          : normal end
//                    Other status : Argument error
//-----------------------------------------------------------------------------
int  DWC_StartUtility(void* work, int language, int param);

/*----------------------------------------------------------------------------/
[Precautions when using the Setup Utility]

* ROM image
  Data used by the setup utility must be stored as a ROM image.
  Files
      /file/dwc/utility.bin
  needs to be located under the rootdirectory name
      /dwc/
   
  Specifically, be sure to add the three lines below to the RomSpec session of the ROM spec file

      RomSpec
      {
	      .
	      .
	      .
          # From here
		  HostRoot    ./file/dwc    # File directory on the PC host
          Root        /dwc
          File        utility.bin
          # To here
      }

  add three digits.

* Archive name
      "dwc"
  The above archive name is reserved.
  Do not use this archive name with applications.

* Notes on making calls
  ・ First allow all processing to end before calling the function that starts the setup utility.
     
	Be sure to turn off functions such as auto sampling for the touch panel.
  ・ A natural connection results if called from a white screen because the setup utility
    was created to be started from a completely white screen.

* Notes on exiting functions
  ・ Output to the LCD is terminated when it is turned off.
  ・ Exit functions under status where all IRQ interrupts are disabled.
  ・ VRAM status is undefined.
  ・ However, if the return value of a function is an error, terminate it immediately without performing any processing.

/*---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Function : Start setup utility
//          Secure work memory internally
// Arguments : language - Language used by the application
//        param - Select from the list below
//                        DWC_UTILITY_TOP_MENU_EXCEPT_JPN
//                        DWC_UTILITY_SETTING_EXCEPT_JPN
//                        DWC_UTILITY_TOP_MENU_FOR_JPN
//                        DWC_UTILITY_SETTING_FOR_JPN
// Return values: int       - 0        : normal end
//                    Other status : Argument error
//-----------------------------------------------------------------------------
int  DWC_StartUtilityEx(int language, int param);


#ifdef  __cplusplus
}
#endif         // __cplusplus


#endif // DWC_UTILITY_H_
