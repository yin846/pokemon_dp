#! make -f
#----------------------------------------------------------------------------
# Project:  NITRO-System - libraries - fundation
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
# Revision 1.9  01/21/2005 04:53:46  kitamurs
# Changed year
#
# Revision 1.8  07/21/2004 05:16:49  kitamurs
# Added allocator
#
# Revision 1.7  06/09/2004 07:42:43  nishida
# Added archive.c
#
# Revision 1.6  05/28/2004 04:52:12  kitamurs
# Corrected typing error
#
# Revision 1.5  03/05/2004 00:09:15  kitamurs
# Added unitheap.c
#
# Revision 1.4  03/01/2004 00:20:18  kitamurs
# Added frameheap
#
# Revision 1.3  02/24/2004 02:46:56  kitamurs
# Implemented expanded heap
#
# Revision 1.2  02/12/2004 01:06:37  nishida
# Fixed so that NitroSDK is supported in thumb mode
#
# Revision 1.1  01/30/2004 03:40:10  nishida
# Registered the initial version.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=


#----------------------------------------------------------------------------

SRCS	= \
	list.c \
	heapcommon.c \
	expheap.c \
	frameheap.c \
	unitheap.c \
	archive.c \
	allocator.c


TARGET_LIB	= libnnsfnd$(NITRO_LIBSUFFIX).a


include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

