******************************************************************************
*                                                                            *
*  Readme-NitroDWC-1_2-060622.txt                                            *
*                                                                            *
*  NITRO-DWC 1.2                                                             *
*                                                                            *
*  June 22, 2006                                                             *
*                                                                            *
******************************************************************************

1. Introduction
==============================================================================
With NITRO-DWC, you can connect to the Internet using the wireless communications feature of the Nintendo DS (infrastructure mode) to register friends, reference the status of friends, and connect with other Nintendo DS units that meet the right conditions.
   
For proper use, you will need the libraries and compilers below:
   
- NITRO-SDK 3.0 or later
- NITRO-System Library 2005/09/01 or later
- NITRO-WiFi 1.0 or later
- CodeWarrior for NINTENDO DS Version 1.2 SP4 or later, or
CodeWarrior for NINTENDO DS Version 2.0 (See NOTE)
   
For further details about NITRO-DWC, refer to each of the programming manuals found in $(NITRODWC_ROOT)/docs, or the function reference manuals in $(NITRODWC_ROOT)/man.
   
   
NOTE: If you are using CodeWarrior Ver. 2.0, make sure to apply the latest patch


2. Using the Library
==============================================================================
When installing, unpack the archive file to a directory of your choice, and configure the path to the NitroDWC directory in the environmental variables.
   
If an older version of NITRO-DWC is already installed, you will need to either delete the existing NitroDWC directory or rename it. (Make sure you don't overwrite this package)
   
To use NITRO-DWC you must connect to Nintendo's Authentication server and to the GameSpy's server. Doing this requires an environment that can connect to the Internet.


3. Revised Content
==============================================================================
The revisions since NITRO-DWC 1.1 are as follows:
   
- Added the general-purpose ranking library
- Added the Download library
- Made additions and changes to the error information that the DWC_GetLastError and DWC_GetLastErrorEx functions can get
- Fixed the problem that occurred when FREESPOT was set for the SSID of connection targets 1 to 3 in the Settings utility
- Fixed the problem that occurred when the nickname in the DS owner information was 10 characters long
- Fixed the problem that MatchMake would not end if started after disconnecting during a game.
- Fixed problem of Reception timeout callback not being called
- Fixed the problem of not being to connect to some hotspots.
- Extended the screen name that the Authentication server can check to 25 characters
- Fixed the problem with arguments of DWCFriendStatusCallback function and DWCBuddyFriendCallback function
- Made changes to the internal operations of the process for obtaining service locator information.
- Made changes to the call conditions for the DWCBuddyFriendCallback function called when a friend relationship is established.
- Made revisions related to the access point authentication process when automatically connecting to the Internet.
- Made changes for when user-defined data is not used in the general purpose ranking library.
- Fixed the problems with the download server error in the download library.
- Made some touch-up revisions to the programming manual
- Made some touch-up revisions to the function reference
- Made additional revisions to the sample demo
- Deleted the 'add-ins' directory
   
For details about each of the above items, refer to the revision history listed in the function reference manual.


