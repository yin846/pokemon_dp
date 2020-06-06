#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - STUBS - libraries
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
# Revision 1.2  23.08.05 04:42:25  yasu
# NITROWIFI_INSTALL_ROOT related revisions
#
# Revision 1.1  29.07.05 05:10:17  terui
# New API for md5 calculations added as a MATH function wrapper.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			 =

#----------------------------------------------------------------------------
# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCS			= dummy_md5.c

TARGET_LIB		= libstubsmd5$(NITRO_LIBSUFFIX).a

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NWIFI_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
