*******************************************************************************
*                                                                             *
*     Readme-NitroSDK-3_0-060125.txt                                          *
*                                                                             *
*     NITRO-SDK 3.1                                                           *
*                                                                             *
*     May 11, 2006                                                            *
*                                                                             *
*******************************************************************************


[Versions with Verified Operation]

The operation of the SDK with the NITRO-System Library versions listed below were verified.

NITRO-System Library (09/01/2005 version)(NITRO-System-Library-050901.zip) with the NITRO-System Library (09/01/2005 version) and NITRO-SDK 3.0 support patch (NITRO-System-Library-050901-patch-NitroSDK3_0-2.zip) applied.

[Note]

Not compatible with modules built with NITRO-SDK 2.2 or earlier. Always use versions of external modules, such as Middleware, that are compatible with NITRO-SDK3.0.

IS-NITRO-DEBUGGER Ver1.65 or later must be installed to build programs under this SDK. For details, see Note 36-03.

The AGB backup access function (see Note 38-02) added to this release version does not function normally when using IS-NITRO-DEBUGGER. It should function normally in the next IS-NITRO-DEBUGGER version upgrade (Ver. 1.67). Until then, be sure to test the function on a DS system. 

[Revision History]

The revision history since the official release of NITRO-SDK 3.0 is listed below.

Note: The same information is available in the following HTML file.

NitroSDK\man\ja_JP\changelog.htm

Contents:

NITRO-SDK 3.1 Official Release (05/02/2006)
Note 38-01: (CARD) Revised DMA transfer from ROM 
Note 38-02: (CTRDG) Added an AGB backup access function
Note 38-03: (CTRDG) Added the CTRDG_SetPhiClock  and CTRDG_GetPhiClock functions
Note 38-04: (FS) Internal revision of the ROM archive
Note 38-05: (FX) Changed the FX_Mul function
Note 38-06: (FX) Revised the VEC_Normalize function
Note 38-07: (GX) Added a description to the reference for the G2_SetBlendAlpha function
Note 38-08: (MATH) Revised the reference for the MATHCompareFunc function
Note 38-09:(MI) Corrected a problem with compression of the MI_CompressLZ function
Note 38-10:(OS) Revised the OS_GetConsoleType function
Note 38-11: (OS) Revised the exceptionDisplay-2 sample demo
Note 38-12: (PRC) Revised figure dictionary data for the demo
Known issues in SDK 3.1 release version

NITRO-SDK 3.1 RC (04/11/2006)
Note 37-01: (CARD) Made compatible with new backup devices.
Note 37-02: (DevEnv) Added .itcm.bss/.dtcm.bss/.wram.bss section 
Note 37-03: (DevEnv) Revised modulerules.cctype.CW 
Note 37-04: (FX) Added reference to MTX_Copy* function.
Note 37-05: (GX) Added G3OP_DUMMY_COMMAND 
Note 37-06: (GX) Revised the geometry command cycle list
Note 37-07: (MI) Added an endian-aware memory access function 
Note 37-08: (OS) Revised OS_CheckHeap
Note 37-09: (OS) Revised arena overview 
Note 37-10: (OS) 8-byte alignment support for the stack 
Note 37-11: (PM) Revised the PM_ForceToPowerOff and PM_ForceToPowerOffAsync functions
Note 37-12: (SND) Added the SND_SetTrackMuteEx function
Note 37-13: (TOOL) Revised reference regarding attachsign 
Note 37-14: (TOOL) Corrected a problem where PPM files could not be loaded using the ppmconv tool 
Note 37-15: (VIB) Revised pulse_vib sample demo
Note 37-16: (WM) Revised the dataShare-Model sample demo
Note 37-17: (WM) Added the wireless-all sample demo
Note 37-18: (WM) Corrected sample demos
Known issues in SDK 3.1 RC

NITRO-SDK 3.1 PR (03/14/2006)
Note 36-01: (ARM7) Improved automatic component removal detection
Note 36-02: (CARD) Made internal revisions to the CARD_CheckPulledOut function
Note 36-03: (CTRDG) Strengthened access restrictions to the AGB cartridge region
Note 36-04: (CTRDG) Added the CTRDG_CheckPulledOut function
Note 36-05: (GX) Corrected 2D_CharBg_Direct sample demos
Note 36-06: (GX) Edited references in the G2_GetBG2ScrPtr and G2_GetBG3ScrPtr functions
Note 36-07: (MB) Added MB_FAKESCAN_PARENT_BEACON notification
Note 36-08: (MB) Changed the operation of the MB_ReadMultiBootParentBssDesc function
Note 36-09: (MB) Corrected a problem with the number of retries when reconnecting to a clone boot sample parent
Note 36-10: (MB) Updated documentation
Note 36-11: (MIC) Described restrictions during automatic sampling
Note 36-12: (PM) Corrected a problem related to the PMAmpGain enumeration type
Note 36-13: (PM) Changed the PM_ForceToPowerOff and PM_ForceToPowerOffAsync functions
Note 36-14: (TOOL) Fixed the order that data is stored in ROM when making file specifications that use wildcards with makerom
Note 36-15: (VIB) Added VIB library
Note 36-16: (WBT) Corrected the WFS sample module
Note 36-17: (WBT) Revised function reference
Note 36-18: (WBT) Added function reference overview
Note 36-19: (WM) Changed specifications regarding the NULL response of children during MP communications
Note 36-20: (WM) Changed WMGameInfo structure definitions
Note 36-21: (WM) Added the WM_IsValidGameBeacon and WM_IsValidGameInfo functions
Note 36-22: (WM) Corrected a problem with minPollBmpMode
Note 36-23: (WM) Updated documentation
Note 36-24: (Misc) Revised description of macros
Known issues in SDK 3.1 PR



Details
NITRO-SDK Version 3.1 Release
(2006/05/02)
--------------------------------------------------------------------------------
Note 38-01: (CARD) Made revisions for DMA transfer from ROM. A problem was corrected where variables adjacent to the transfer destination buffer used for DMA transfer from the ROM were being affected by cache operations if the buffer cache line was not aligned at 32 byte boundaries. 

--------------------------------------------------------------------------------
Note 38-02: (CTRDG) Added AGB backup access functions. Added a series of APIs to the CTRDG library for accessing the backup device installed in the AGB cartridge. 

--------------------------------------------------------------------------------
Note 38-03: (CTRDG) Added the CTRDG_SetPhiClock and CTRDG_GetPhiClock functions. The CTRDG_SetPhiClock and CTRDG_GetPhiClock functions were created for getting and setting the clock value output from the PHI terminal of the cartridge. 
Similar functions were called MI_SetPhiClock and MI_GetPhiClock in past version releases. But, this was changed to CTRDG_ when categories were reviewed. 
Even for internal operation, the CTRDG_SetPhiClock function was changed to set the clocks of both the ARM7 and ARM9. 
Previously, it only changed the setting of the ARM9, but the clock output pattern would change when moving the cartridge access right to the ARM7. 

The operation of the CTRDG_GetPhiClock function for getting the clock settings is the same as the MI-GetPhiClock function. 

--------------------------------------------------------------------------------
Note 38-04: (FS) Internally revised the ROM archive. A problem was corrected where processing would not complete if the callback function set using CARD_SetPulledOutCallback returned FALSE and an attempt was made to access ROM files using a function like FS_OpenFile before the CARD_TerminateForPulledOut function could be called. 

This problem did not occur when loading a file table into memory using FS_LoadTable. 

--------------------------------------------------------------------------------
Note 38-05: (FX) Changed the FX_Mul function. Changed FX_Mul so that a message is displayed by the macro SDK_WARNING if overflow occurs.
Additions were also made to the function reference.  

--------------------------------------------------------------------------------
Note 38-06: (FX) Revised the VEC_Normalize function. There was a problem where correct results could not be obtained when the value given to this function was greater than a set value, so it was changed so that a message would be displayed by the SDK_WARNING macro when the value was above this set amount. 
Additions were also made to the function reference.  

--------------------------------------------------------------------------------
Note 38-07: (GX) Added a description to the reference for G2_SetBlendAlpha. Added a note to the reference for the G2_SetBlendAlpha function that a backdrop cannot be specified for the first target screen.

--------------------------------------------------------------------------------
Note 38-08: (MATH) Revised the reference for MATHCompareFunc. This function is used when sorting using the MATH_QSort function. A note was added to the reference for MATHCompareFunc that 0 must be returned when two elements being compared are equal.  

--------------------------------------------------------------------------------
Note 38-09:(MI) Corrected a problem with compression of the MI_CompressLZ function.
The problem where over-access would occur during LZ compression was resolved as shown in Note 35-04. However, the resolution was not reflected in MI_CompressLZ--the low-speed version of the LZ compression function. 
This problem was corrected. 

--------------------------------------------------------------------------------
Note 38-10:(OS) Revised the OS_GetConsoleType function.
A problem was corrected where OS_GetConsoleType might return a value that differs from that described in the function reference.  

--------------------------------------------------------------------------------
Note 38-11: (OS) Revised the exceptionDisplay-2 sample demo,
There was a problem where the required function would be dead stripped when using CW2.0 in the $NitroSDK/build/demos/os/exceptionDisplay-2 sample demo. This problem has been corrected to explicitly prevent the dead stripping. 

--------------------------------------------------------------------------------
Note 38-12: (PRC) Revised figure dictionary data for demos.
The stroke order for the Japanese character "GE" in the demo figure dictionary data was incorrect. This problem was corrected.  

--------------------------------------------------------------------------------
Known issues in SDK 3.1 release version
- Provisional support for CodeWarrior 2.0 specification.
However, accompanying changes will have no effect on current applications.
We plan to officially support the extended language codes in the next release.

--------------------------------------------------------------------------------

NITRO-SDK Version 3.1 RC
(2006/04/11)
--------------------------------------------------------------------------------
Note 37-01: (CARD) Made compatible with new backup devices
Made the Card Library compatible with the 8MB Flash Backup Device and added CARD_BACKUP_TYPE_FLASH_8MBITS to the CARDBackupType enumerated type.

In conjunction with the change, the reference and sample demos were updated. 

--------------------------------------------------------------------------------
Note 37-02: (DevEnv) Added .itcm.bss/.dtcm.bss/.wram.bss section 
Data placed in a .bss section that did not include initial values was accidentally being placed in the .data section due to the fact a .bss section had not been specified in the .itcm/.dtcm/.wram section until now.

Revised $NitroSDK/build/tools/makelcf and $NitroSDK/include/nitro/section.h, revised the .template files found under $NitroSDK/include/nitro/specfiles and $NitroSDK/build/components/ichneumon, and added a .bss section to .itcm/.dtcm/.wram. 

--------------------------------------------------------------------------------
Note 37-03: (DevEnv) Revised modulerules.cctype.CW 
Corrected the problem where, depending on the version of the sed command, a NITRO-SDK build error would sometimes occur.  

--------------------------------------------------------------------------------
Note 37-04: (FX) Added reference to MTX_Copy* function.
Added the descriptions of the MTX_Copy* function to the FX category in the function reference. 
The following function references were added. 

MTX_Copy22 function 
MTX_Copy33 function 
MTX_Copy43 function 
MTX_Copy44 function 

--------------------------------------------------------------------------------
Note 37-05: (GX) Added G3OP_DUMMY_COMMAND 
Added 0xFF, an undefined geometry command, as the dummy command G3OP_DUMMY_COMMAND.  

--------------------------------------------------------------------------------
Note 37-06: (GX) Revised the geometry command cycle list
We confirmed that the number of command cycles corresponding to each source increases when an environment map is used. 
Information was added to the geometry command cycle list in the function reference. 

--------------------------------------------------------------------------------
Note 37-07: (MI) Added endian-aware memory access functions 
Added a group of functions for reading/writing memory with full awareness of little endian and big endian issues. 

--------------------------------------------------------------------------------
Note 37-08: (OS) Revised OS_CheckHeap
There was a problem where an error would be returned if the heap handle specification was OS_CURRENT_HEAP_HANDLE. 
This is fixed. 

--------------------------------------------------------------------------------
Note 37-09: (OS) Revised arena overview
Corrected a mistake in the graphic about arena management and memory allocation in the OS arena overview in the function reference. Also added descriptive text.  

--------------------------------------------------------------------------------
Note 37-10: (OS) 8-byte alignment support for the stack 
Adjusted stack processing so that the stack pointer is always aligned at 8 byte boundaries during function calls. 

--------------------------------------------------------------------------------
Note 37-11: (PM) Revised the PM_ForceToPowerOff and PM_ForceToPowerOffAsync functions
Corrected a problem with the specification changes given in Note 36-13. 

--------------------------------------------------------------------------------
Note 37-12: (SND) Added the SND_SetTrackMuteEx function
Added the SND_SetTrackMuteEx function for selecting processing to apply to a sound played while a sequence track is muted. 

--------------------------------------------------------------------------------
Note 37-13: (TOOL) Revised reference regarding attachsign
Changed the description for getting the authentication code in the attachsign tool reference. 

--------------------------------------------------------------------------------
Note 37-14: (TOOL) Corrected a problem where PPM files could not be loaded using the ppmconv tool 
when an attempt was made to convert ppm files created with photoshop (via a NetPBMformats plugin). The message "Cannot convert file xxxx.ppm" appeared and conversion failed. 
The problem was due to the fact that the processing implemented for analyzing file headers did not fully cover ppm specifications. 

This bug was fixed.
Several ppm files which could not be converted can now be converted. 
This fix has no impact on the ppm files that could be converted correctly. 

--------------------------------------------------------------------------------
Note 37-15: (VIB) Revised pulse_vib sample demo
The SVC_WaitVBlankIntr function deleted from the source code for all sample demos in the NITRO-SDK as described in Note 36-15 was being used in the pulse_vib sample demo, so this function was changed to OS-WaitVBlankIntr.
Furthermore, specifications have changed so that the sample does not stop even if parameters that violate restrictions on the Rumble Pak library are entered.  

--------------------------------------------------------------------------------
Note 37-16: (WM) Revised the dataShare-Model sample demo
If the number of children specified by the user (WH_CHILD_MAX) is less than WM_NUM_MAX_CHILD, the WM_GetSharedDataAddress function would be called instead of WH__CHILD_MAX. This problem was fixed. 

--------------------------------------------------------------------------------
Note 37-17: (WM) Added the wireless-all sample demo
Added the sample demo $NitroSDK/build/demos/wm/wireless-all to show the comprehensive use of WM library functions. 

--------------------------------------------------------------------------------
Note 37-18: (WM) Corrected sample demos
Added the following revisions to the WM sample demo. 

Revisions were made so that both data sharing and MP direct control modes can be used in the WC sample module of $NitroSDK/build/demos/wireless_shared/wc.  
Made changes so that the maximum number of MP communications per frame when using the $NitroSDK/build/demos/wireless_shared/wh WH sample module can be set in wh_config.h as WH_MP_FREQUENCY.
The default value of 1 is used for conventional programs where this value is undefined.  

--------------------------------------------------------------------------------
Known issues in SDK 3.1 RC
- Added a series of APIs to the CTRDG library for accessing the backup device installed in the AGB Game Pak.
However, operations are not guaranteed because verification of these newly added AGB backup access functions is not complete.
This revision will be officially supported in the next release.

- Provisional support for CodeWarrior 2.0. However, accompanying changes will have no effect on current applications. 
We plan to officially support the extended language codes in the next release.

--------------------------------------------------------------------------------

NITRO-SDK Version 3.1 PR
(2006/03/14)
--------------------------------------------------------------------------------
Note 36-01: (ARM7) Improved automatic component removal detection
In the past, when either one of the card or cartridge buses was locked for an extended period using the OS_Lock* function, you could not execute device removal detection for the other bus. This problem is fixed in this release so removal detection can be performed. 

--------------------------------------------------------------------------------
Note 36-02: (CARD) Internal revisions made to the CARD_CheckPulledOut function
Specifications have been changed so when the CARD_CheckPulledOut function is called multiple times, a callback function is not called for each instance. The callback can now only be called once by calling the CARD_CheckPulledOut function or by ARM7 removal detection. 

--------------------------------------------------------------------------------
Note 36-03: (CTRDG) Strengthened access restrictions to the AGB cartridge region
To avoid sudden write access to the AGB cartridge region, processing for changing the initial settings of the protection region was added to the CTRDG_Init function. When accessing a cartridge to write, cartridge information must be checked, and then the CTRDG_Enable function must be called.

Note: Processing will be forcibly stopped when outputting a debug string using the OS_Printf or another function unless using IS-NITRO-DEBUGGER ver1.65, which is compatible with this function. 

--------------------------------------------------------------------------------
Note 36-04: (CTRDG) Added the CTRDG_CheckPulledOut function.
The function CTRDG_CheckPulledOut was added for quick cartridge removal detection from the ARM9. 

--------------------------------------------------------------------------------
Note 36-05: (GX) Corrected 2D_CharBg_Direct sample demos
Revisions were made so that correct settings are made by the G2_SetBG3ControlDCBmp function before the GX_LoadBG3Bmp when using the sample demo $NitroSDK/build/demos/gx/UnitTours/2D_CharBg_Direct. 

--------------------------------------------------------------------------------
Note 36-06: (GX) Edited references in G2_GetBG2ScrPtr and G2_GetBG3ScrPtr functions
There were incorrect descriptions of the instances with a bitmap BG in a G2_GetBG2ScrPtr and G2_GetGB3ScrPtr function reference. This problem was corrected. 

--------------------------------------------------------------------------------
Note 36-07: (MB) Added MB_FAKESCAN_PARENT_BEACON notification
MB_FAKESCAN_PARENT_BEACON was added to events that are notified to the MBFakeScanCallbackFunc callback. This notification is issued each time an MB parent beacon is received.

In relation to this, processing for using the MB_GetUserVolatData function was added with the sample demos multiboot-Model and fake_child. 

--------------------------------------------------------------------------------
Note 36-08: (MB) Changed the operation of MB_ReadMultiBootParentBssDesc
A problem was corrected where the magicNumber and ver members of gameInfo, which should usually be specified inside the MB_ReadMultiBootParentBssDesc function, remained uninitialized. 

--------------------------------------------------------------------------------
Note 36-09: (MB) Corrected a retry problem when reconnecting to a clone boot sample parent
A problem was corrected where revisions described in Note 20-08 were not applied to the clone boot sample and retries were not allowed after one attempt to connect to the parent failed after child boot. 

--------------------------------------------------------------------------------
Note 36-10: (MB) Updated documentation
The following MB library-related document was updated.
$NitroSDK/docs/technicalNotes/AboutMultiBoot.pdf

--------------------------------------------------------------------------------
Note 36-11: (MIC) Described restrictions during automatic sampling
A note was added to the reference manual regarding a restriction that does not allow extended main memory to be specified as a buffer for storing sampling results when performing auto-sampling for the mic. No change was made to library operations. 

--------------------------------------------------------------------------------
Note 36-12: (PM) Corrected problem related to the PMAmpGain enumeration type
The constant PM_AMPGAIN_DEFAULT used inside the PM_GAINAMP_DEFAULT macro was not defined. This problem was corrected. PM_AMPGAIN_DEFAULT was added to the PMAmpGain enumeration type.

--------------------------------------------------------------------------------
Note 36-13: (PM) Changed the PM_ForceToPowerOff and PM_ForceToPowerOffAsync functions
When an application attempts to turn OFF the power of the DS System, the DS System sometimes restarts if the main power was turned off during LCD OFF status.
The specifications of the PM_ForceToPowerOff and PM_ForceToPowerOffAsync functions have therefore been changed so that power is turned OFF after LCDs are turned ON. 

--------------------------------------------------------------------------------
Note 36-14: - (TOOL)  Fixed the inclusion order within ROM when specifying files to makerom using wildcards. 
When using wildcards to specify files to store in ROM with makerom, the order in which files were stored was sometimes affected by the entry order of the file system on the PC used to build the application. This resulted in the file order changing when the same application was built on different PCs.
This problem was fixed so that files are now stored in the order sorted by name using makerom. 

--------------------------------------------------------------------------------
Note 36-15: (VIB) Added VIB library
The VIB library was added to control the Rumble Pak option cartridge for the Nintendo DS. 

--------------------------------------------------------------------------------
Note 36-16: (WBT) Corrected WFS sample module
We fixed the following problems with the WFS sample module included in $NitroSDK/build/demos/wireless_shared/wfs. 

A problem was corrected where data at the end of the list becomes undefined during file list initialization process on the parent side. 
A problem was corrected where file handles on the parent side sometimes ran short when the maximum number of connected child devices simultaneously sent file requests too frequently. 
A problem was corrected where, if a child device disconnected during a file request, the file requests of other child devices did not progress. 

--------------------------------------------------------------------------------
Note 36-17: (WBT) Revised function reference
A problem was corrected where the fact that the following commands can be directly issued from one child to another was described incorrectly in the function reference. 

WBT_PutUserData
WBT_GetBlockInfo
WBT_GetBlock

--------------------------------------------------------------------------------
Note 36-18: (WBT) Added function reference overview
The item "Overview" was added to the WBT library function reference.

--------------------------------------------------------------------------------
Note 36-19: (WM) Changed specifications regarding the NULL response of children during MP communications
Specifications were changed so that child devices would not return a Null response during MP communications as described in Note 30-06. In rare cases, this would result in unstable operations depending on the environment, so the change described in Note 30-06 was canceled and the previous specification was restored.

In addition, instead of allowing Null responses from child devices, specifications changed so if a parent device receives a Null response from a child device, it assumes the child device had a communication failure.
Since the WM_SetMPInterval function was added, excess load problems on the child side should not occur even if changes are made to allow the return of Null responses. 

--------------------------------------------------------------------------------
Note 36-20: (WM) Changed WMGameInfo structure definitions
The definition of the WMGameInfo structure changed. The version field was eliminated and the name was changed to magicNumber. The magicNumber field is fixed at a value of 0x0001 when used as a DS beacon. In addition, the ver field was established using one byte from the reserved region.
Under the current version, the ver field is always 0. I the future, this will increase as the format of the WMGameInfo structure changes. Note: In order to ensure future backward compatibility, there must be no restriction on operations based on checking the ver field. Due to these changes, you must rewrite programs that accessed WMGameInfo.version.
In addition, the name of the field gameNameCount_attribute changed to "attribute." Although this field can be referenced using the old name, the new field name should be used from now on.
In addition, support of the compile switch WM_GAMEINFO_TYPE_OLD, which is never used, has ended due to this change. 

--------------------------------------------------------------------------------
Note 36-21: (WM) Added the WM_IsValidGameBeacon and WM_IsValidGameInfo functions
WM_IsValidGameBeacon and WM_IsValidGameInfo functions were added to determine whether or not the scan result beacon is that of a DS wireless play parent. Internally, it determines whether WMGameInfo.magicNumber is correct. Furthermore, each WM demo was changed to use this function. 
  

--------------------------------------------------------------------------------
Note 36-22: (WM) Corrected a problem with minPollBmpMode
A problem was corrected where communications would be discontinued after a set time interval and the MP lifetime would run out when the MP communications option minPollBmpMode was enabled.

Only titles that perform special communications using minPollBmpMode were affected by this problem.

--------------------------------------------------------------------------------
Note 36-23: (WM) Updated documentation
The following WM library-related document was updated.
$NitroSDK/docs/technicalNotes/AboutWirelessManager.pdf

--------------------------------------------------------------------------------
Note 36-24: (Misc) Revised description of macros
Corrected the descriptions of macros where () were not attached to arguments.
This change has no effect as long as arguments where operations change depending on the presence of parentheses are not being used.
The following macros were revised. 

SDK_NULL_ASSERT
SDK_ALIGN4_ASSERT
GX_VFCFX10
GX_FX16ST
GX_COMP4x4_PLTT_IDX
GX_PACK_MTXMODE_PARAM
GX_PACK_VIEWPORT_PARAM
SVC_CpuClear
SVC_CpuCopy
WC_ROUNDUP_32
Corresponding function references were revised along with this change. 

--------------------------------------------------------------------------------
Known issues in SDK 3.1 PR
None. 

--------------------------------------------------------------------------------

End
