#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - RTC - libraries
# File:     Makefile
#
# Copyright 2003-2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.8  05/18/2005 03:54:06  seiki_masashi
# Revised comments
#
# Revision 1.7  05/18/2005 02:55:03  seiki_masashi
# Revised copyright notation
#
# Revision 1.6  05/17/2005 12:53:45  seiki_masashi
# Added conversion function
#
# Revision 1.5  08/27/2004 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.4  07/14/2004 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.3  07/14/2004 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.2  06/18/2004 10:22:02  yasu
# fix typo
#
# Revision 1.1  05/12/2004 02:33:01  terui
# initial upload
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

NITRO_PROC		= ARM9

SRCS			= external.c		\
			  internal.c		\
			  convert.c

TARGET_LIB		= librtc$(NITRO_LIBSUFFIX).a

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====

