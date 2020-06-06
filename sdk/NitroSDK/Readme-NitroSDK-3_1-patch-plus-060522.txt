****************************************************************************
*                                                                          *
*  Readme-NitroSDK-3_1-patch-plus-060522.txt                               *
*                                                                          *
*  Plus Patch for NITRO-SDK 3.1                                            *
*                                                                          *
*  May 25th, 2006                                                          *
*                                                                          *
****************************************************************************


Introduction

This patch corrects problems found in NITRO-SDK after the 3.1 release. To install this patch, copy and overwrite files to the directory where NITRO-SDK 3.1 (05/11/2006 version) is installed.

* Apply this Plus Patch if you are using NITRO-SDK 3.1.

Caution:

If the NitroSDK library has been built before, an error may arise during the program build process. This is due to cache data left over from the previous build. If this happens, running "make clobber" at the NitroSDK root directory will delete this cache data. Performing "make" in the same NitroSDK root directory at this point will remedy this problem.

Revision Content:
    
Contents revised in 3_1-patch-plus-060522
    --------------------------------------------------------------------------
    
- (PM) Fixed issue where a power down would not occur when the DS Card was removed while the LCD was off.

As a result of the revisions described in "Note 36-13: (PM), Revisions to the PM_ForceToPowerOff and PM_ForceToPowerOffAsync functions," when a DS Card was removed while the LCD was turned off, the device would not power down, even if it was closed, because PM_ForceToPowerOffAsync could not be called while interrupts were disabled. This has been fixed.


File List:

Source Files
	/NitroSDK/build/libraries/pxi/common/src/pxi_fifo.c
	/NitroSDK/build/libraries/spi/ARM9/src/pm.c

Header Files
	/NitroSDK/include/nitro/pxi/common/fifo.h
	/NitroSDK/include/nitro/version.h

Library Files
	/NitroSDK/lib/ARM9-TS/Debug/crt0.o
	/NitroSDK/lib/ARM9-TS/Debug/libpxi.a
	/NitroSDK/lib/ARM9-TS/Debug/libpxi.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS0.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS100.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS200.a
	/NitroSDK/lib/ARM9-TS/Debug/libspi_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/crt0.o
	/NitroSDK/lib/ARM9-TS/Release/libpxi.a
	/NitroSDK/lib/ARM9-TS/Release/libpxi.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi.a
	/NitroSDK/lib/ARM9-TS/Release/libspi.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS0.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS100.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS200.a
	/NitroSDK/lib/ARM9-TS/Release/libspi_TS200.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/crt0.o
	/NitroSDK/lib/ARM9-TS/Rom/libpxi.a
	/NitroSDK/lib/ARM9-TS/Rom/libpxi.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS0.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS0.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS100.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS100.thumb.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS200.a
	/NitroSDK/lib/ARM9-TS/Rom/libspi_TS200.thumb.a

Component Files
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Debug/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Release/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS/Rom/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Debug/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Release/ichneumon_sub_TS200.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS0.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS100.nef
	/NitroSDK/components/ichneumon/ARM7-TS.thumb/Rom/ichneumon_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS/Debug/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS/Release/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS/Rom/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Debug/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Release/mongoose_sub_TS200.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS0.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS100.nef
	/NitroSDK/components/mongoose/ARM7-TS.thumb/Rom/mongoose_sub_TS200.nef

End