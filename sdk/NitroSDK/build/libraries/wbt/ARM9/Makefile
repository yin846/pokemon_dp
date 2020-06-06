#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - WBT - libraries
# File:     Makefile
#
# Copyright 2003,2004 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.2  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.1  2004/08/23 09:45:24  yosizaki
# (none)
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

NITRO_PROC		= ARM9

SRCDIR			= src
INCDIR			= include

SRCS			= wbt.c wbt_command.c wbt_hook.c \
			  wbt_ui.c  wbt_data.c 

TARGET_LIB		= libwbt$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSDK_ROOT)/build/buildtools/modulerules



#===== End of Makefile =====
