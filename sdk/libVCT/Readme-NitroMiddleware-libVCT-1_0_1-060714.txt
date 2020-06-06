==============================================================================
==
==  Readme-NitroMiddleware-libVCT-1_0_1-060714.txt
==
==  VoiceChat Library version 1.0.1
==
==  July 14, 2006
==
==============================================================================

==============================================================================
1. Introduction
==============================================================================
 
  VoiceChat Library is a set of libraries that enable voice communication over
  a TCP/IP network. It includes libraries for both voice communication 
  functionality and the ability to control calls.
  
  Special hardware and/or software are not necessary for voice communications
  using VoiceChat Library. However, NitroWiFi and NitroDWC must be used for 
  communication.
  
  The communication styles available with VoiceChat Library are one-to-one
  (Telephone Mode), one-to-group (Transceiver Mode), and group-to-group
  (Conference Mode).
  
  For further information, see the VoiceChat Library Reference Manual
  (AboutVoiceChat.pdf) located in "docs/TechnicalNotes/" and the HTML
  Reference Manual located in "man/en_US".


==============================================================================
2. Directories
==============================================================================

  * libVCT/
        build/
            demos/       Demonstration program
        docs/            Documents
        include/         Include headers
        lib/             Libraries
        man/en_US        HTML reference manual
        Readme.txt       This document


==============================================================================
3. Demonstration Program
==============================================================================

  This package includes the following VoiceChat Library demonstration programs.
    
  * VoiceChatDemo
    This is a sample application program of the VoiceChat Library working on
    NitroDWC.
    It demonstrates sample operations in all modes (Telephone, Transceiver,
    and Conference Modes).
    
  * VADDemo
    This is a sample application program for testing VAD.


==============================================================================
4. Revision History
==============================================================================

  Version 1.0.1 (July 14, 2006)

   * Removed unused files
   * Fixed CodeWarrior project file
   * Removed test code from VADDemo program
   * Removed dbs codes

  Version 1.0.0 (June 1, 2006)
  
   * The first edition was released.
