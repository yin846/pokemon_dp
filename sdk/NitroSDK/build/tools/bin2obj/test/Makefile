#! make -f
#---------------------------------------------------------------------------
#  Project:  NitroSDK - tools
#  File:     Makefile
#
#  Copyright 2005 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: Makefile,v $
#  Revision 1.2  06/13/2005 08:52:26  yasu
#  Support for %f as symbol name
#
#  Revision 1.1  06/13/2005 02:56:34  yasu
#  Revised options, confirmed operation with test creation
#
#  $NoKeywords: $
#---------------------------------------------------------------------------

SRCS			= test.c
TARGET_BIN		= test.srl
EXT_OBJS		= testdata.o

LDIRT_CLEAN		= testdata.o

include $(NITROSDK_ROOT)/build/buildtools/commondefs

do-build:		$(TARGETS)

include $(NITROSDK_ROOT)/build/buildtools/modulerules

testdata.o:		testdata.txt
			$(BIN2OBJ) $< $@

#

