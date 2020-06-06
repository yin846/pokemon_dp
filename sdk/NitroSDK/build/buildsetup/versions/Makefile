#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - buildsetup - versions
# File:     Makefile
#
# Copyright 2003-2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.8  05/01/2006 01:16:18  okubata_ryoma
# Made changes in line with the change to the method of comparing CW versions
#
# Revision 1.7  09/17/2005 05:52:46  yasu
# Added compiler version information to version.h.
#
# Revision 1.6  07/06/2005 23:57:13  yasu
# Updated copyright year
#
# Revision 1.5  07/06/2005 13:20:56  yasu
# Changed SDK_VERSION_MAJOR to NITROSDK_VERSION_MAJOR
#
# Revision 1.4  07/06/2005 13:16:55  yasu
# Makefile selection using SDK_VERSION_MAJOR
#
# Revision 1.3  09/17/2004 11:23:31  yasu
# update only newly build
#
# Revision 1.2  09/17/2004 10:45:56  yasu
# fix install directory
#
# Revision 1.1  09/15/2004 13:17:23  yasu
# Generate version.h before build tree
#
# $NoKeywords: $
#----------------------------------------------------------------------------

NITRO_FORCE_BUILD	= True
include	$(NITROSDK_ROOT)/build/buildtools/commondefs
-include $(SDK_VERINFO_CW_CC) $(SDK_VERINFO_CW_LD)

TARGETS			= version.h

ifeq	($(NITROSDK_VERSION_MAJOR),3)
TARGETS_MAKEFILE	= $(ROOT)/release/Makefile.3x
else
ifeq	($(NITROSDK_VERSION_MAJOR),2)
TARGETS_MAKEFILE	= $(ROOT)/release/Makefile.2x
else
TARGETS_MAKEFILE	= $(ROOT)/release/Makefile
endif
endif

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_INCDIR)/nitro
LDIRT_CLEAN		= $(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

do-build:	$(TARGETS)

SDK_BUILDVER_CW_CC	= $(shell echo $(SDK_CW_BUILD_VERSION_CC) | sed -e 's/^\(.\)/\1/')
SDK_BUILDVER_CW_LD	= $(shell echo $(SDK_CW_BUILD_VERSION_LD) | sed -e 's/^\(.\)/\1/')
SDK_BUILDNUM_CW_CC	= $(shell echo $(SDK_CW_BUILD_NUMBER_CC) | sed -e 's/^0*\(.\)/\1/')
SDK_BUILDNUM_CW_LD	= $(shell echo $(SDK_CW_BUILD_NUMBER_LD) | sed -e 's/^0*\(.\)/\1/')

MAKEVER_FLAGS		= VERSIONFILE=$(TARGETS) \
			  SDK_BUILDVER_CW_CC=$(SDK_BUILDVER_CW_CC) \
			  SDK_BUILDVER_CW_LD=$(SDK_BUILDVER_CW_LD) \
			  SDK_BUILDNUM_CW_CC=$(SDK_BUILDNUM_CW_CC) \
			  SDK_BUILDNUM_CW_LD=$(SDK_BUILDNUM_CW_LD)

$(TARGETS):	$(TARGETS_MAKEFILE)
		@$(ECHO) SDK_BUILDVER_CW_CC=$(SDK_BUILDVER_CW_CC)
		@$(ECHO) SDK_BUILDVER_CW_LD=$(SDK_BUILDVER_CW_LD)
		@$(ECHO) SDK_BUILDNUM_CW_CC=$(SDK_BUILDNUM_CW_CC)
		@$(ECHO) SDK_BUILDNUM_CW_LD=$(SDK_BUILDNUM_CW_LD)
		@$(MAKE) -s -f $(TARGETS_MAKEFILE) $(MAKEVER_FLAGS) version > /dev/null


#===== End of Makefile =====
