#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools - ntrcomp
#  File:     Makefile
#
#  Copyright 2003,2004 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.1  2004/03/04 05:48:49  yasu
#  add ntrcomp
#
#  $NoKeywords: $
#---------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

TARGETS		= ntrcomp.exe

OBJECTS		= ntrcomp_main.o nitroCompLib.o

INSTALL_DIR     = $(NITRO_INSTALL_TOOLSDIR)/bin
INSTALL_TARGETS	= $(TARGETS)

LDIRT_CLEAN	= $(OBJECTS) $(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules.x86

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

$(TARGETS):		$(OBJECTS)
			$(CC_X86) $+ -o $@

ntrcomp_main.o:		ntrcomp_main.c nitroCompLib.h
nitroCompLib.o:		nitroCompLib.c nitroCompLib.h
