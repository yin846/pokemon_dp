#! make -f
#----------------------------------------------------------------------------
# Project:  NITRO-System - libraries - Multiple Channel Stream
# File:     Makefile
#
# Copyright 2004,2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.4  01/21/2005 04:51:57  kitamurs
# Changed year
#
# Revision 1.3  12/15/2004 06:06:27  kitamurs
# Print features
#
# Revision 1.2  06/09/2004 07:34:11  kitamurs
# Provisional port of the file I/O
#
# Revision 1.1  05/25/2004 06:59:11  kitamurs
# Moved directories
#
# Revision 1.1  05/24/2004 02:14:08  kitamurs
# Moved directories
#
# Revision 1.2  04/28/2004 05:11:45  kitamurs
# Added load functions
#
# Revision 1.1  04/14/2004 07:03:43  kitamurs
# Registration of the initial version
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=


#----------------------------------------------------------------------------

SRCS	= \
	base.c \
	ringBuffer.c \
	fileIOmain.c \
	print.c

TARGET_LIB	= libnnsmcs$(NITRO_LIBSUFFIX).a


include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

