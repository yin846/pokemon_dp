#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - FS - libraries - ARM9
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
# Revision 1.12  2005/06/28 11:32:11  yosizaki
# add arc_mem.c
#
# Revision 1.11  05/30/2005 23:50:31  yosizaki
# update copyright date.
#
# Revision 1.10  05/30/2005 04:12:47  yosizaki
# add fs_command_default.c
#
# Revision 1.9  08/27/2004 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.8  07/14/2004 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.7  07/14/2004 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.6  07/08/2004 13:38:57  yosizaki
# add fs_command.c
#
# Revision 1.5  05/10/2004 06:28:56  yosizaki
# add fs_archive, fs_rom.
#
# Revision 1.4  05/06/2004 10:18:18  yasu
# fix typo
#
# Revision 1.3  04/21/2004 11:18:48  yosizaki
# NITRO_CONSERVATIVE=TRUE (temporary)
#
# Revision 1.2  04/13/2004 12:12:48  yosizaki
# fix header comment.
#
# Revision 1.1  04/01/2004 11:36:18  yosizaki
# (none)
#
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=


#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

#NITRO_CONSERVATIVE = TRUE
SRCDIR			= ../common/src src

SRCS			=	fs_command.c	\
					fs_command_default.c	\
					fs_archive.c	\
					fs_file.c	\
					fs_rom.c	\
					fs_mem.c	\
					fs_overlay.c

TARGET_LIB		= libfs$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

