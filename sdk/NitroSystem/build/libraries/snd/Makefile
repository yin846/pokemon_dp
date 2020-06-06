#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSystem - Snd - libraries
# File:     Makefile
#
# Copyright 2004 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.19  10/04/2004 08:51:37  ida
# add output_effect.c
#
# Revision 1.18  08/02/2004 05:27:34  ida
# add stream library
#
# Revision 1.17  07/16/2004 06:42:49  ida
# fix Project name
#
# Revision 1.16  06/28/2004 07:10:38  ida
# lock.c -> resource_mgr.c
#
# Revision 1.15  06/23/2004 06:17:13  ida
# add waveout.c
#
# Revision 1.14  06/21/2004 01:08:04  ida
# add lock.c
#
# Revision 1.13  05/13/2004 01:01:06  ida
# merge seqarc.c and data.c to seqdata.c
#
# Revision 1.12  05/06/2004 23:55:49  ida
# fix typo
#
# Revision 1.11  04/26/2004 02:19:18  ida
# add heap.c sndarc_loader.c
#
# Revision 1.10  02/20/2004 05:02:59  ida
# change directory
#
# Revision 1.1  02/13/2004 01:42:05  ida
# initial version
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

SRCS	= \
	main.c \
	resource_mgr.c \
	waveout.c \
	player.c \
	stream.c \
	capture.c \
	sndarc.c \
	heap.c \
	sndarc_loader.c \
	sndarc_player.c \
	sndarc_stream.c \
	output_effect.c \
	seqdata.c	\
	fader.c

TARGET_LIB	=	libnnssnd$(NITRO_LIBSUFFIX).a

include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

