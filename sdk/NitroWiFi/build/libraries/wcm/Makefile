#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - WCM - libraries
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
# Revision 1.4  10.09.05 03:09:17  terui
# Add util.c in SRCS
#
# Revision 1.3  23.08.05 04:42:25  yasu
# NITROWIFI_INSTALL_ROOT related revisions
#
# Revision 1.2  08.07.05 07:43:54  yasu
# revised the include method for commondefs/modulerules
#
# Revision 1.1  2005/07/07 10:44:36  terui
# new additions
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			 =

#----------------------------------------------------------------------------
# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCS			= system.c		\
			  aplist.c		\
			  cpsif.c		\
		      util.c

TARGET_LIB		= libwcm$(NITRO_LIBSUFFIX).a

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NWIFI_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
