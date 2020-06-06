#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - add-ins - libraries - env
# File:     Makefile
#
# Copyright 2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.3  2005/08/23 04:40:53  yasu
# revised to allow install destination to be specified during make install
#
# Revision 1.2  2005/08/19 12:28:20  yasu
# add-ins support
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------
#  a problem occurs if commondefs.add-ins.* is deleted before the 
#  installed libraries are clobbered; halts clobber if relevant files exist
#
ifeq	($(MAKECMDGOALS),clobber)
FILE_TESTEE	= $(NWIFI_ADDINS_ENV_LIBDIR)/$(firstword $(NWIFI_ADDINS_ENV_LIBS))
MUST_WAIT	= $(shell test -e $(FILE_TESTEE) && echo TRUE)
endif

ifeq	($(MUST_WAIT),TRUE)
INSTALL_TARGETS	=
INSTALL_DIR	=
else
INSTALL_TARGETS	= commondefs.add-ins.env
INSTALL_DIR	= $(NWIFI_INSTALL_ROOT)/build/buildtools
endif

#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
