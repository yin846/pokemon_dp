#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - mi/ARM9
# File:     Makefile
#
# Copyright 2003-2005 Nintendo. All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law. They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.14  2005/02/09 05:10:31  yada
# add mi_exMemory.c and mi_init.c
#
# Revision 1.13  2005/01/28 13:14:25  takano_makoto
# fix Copyright Header.
#
# Revision 1.12  2005/01/28 13:11:57  takano_makoto
# Added mi_compress.c
#
# Revision 1.11  2004/11/30 10:52:40  takano_makoto
# add mi_uncomp_stream.c
#
# Revision 1.10  2004/10/08 11:58:41  yosizaki
# add mi_dma_card.c
#
# Revision 1.9  2004/08/27 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.8  2004/08/25 00:50:55  yada
# separate each type of dma from mi_dma.c, so mi_dma_xxx.c are added
#
# Revision 1.7  2004/07/20 08:01:21  yada
# add mi_stream.c
#
# Revision 1.6  2004/07/14 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.5  2004/07/14 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.4  2004/05/06 10:18:18  yasu
# Fixed typo
#
# Revision 1.3  2004/04/26 11:05:29  yosizaki
# add mi_card.c
#
# Revision 1.2  2004/03/30 11:00:11  yada
# changed byte access from ARM7 only to common
#
# Revision 1.1  2004/02/12 11:32:11  yasu
# makefile for NitroSDK/build/libraries/mi/ARM9
#
# Revision 1.5  2004/02/10 01:22:57  yada
# Added mi_memory.c, mi_uncompress.c, and mi_swap.c
#
# Revision 1.4  2004/02/09 11:17:49  yasu
# support thumb version library
#
# Revision 1.3  2004/02/05 07:09:02  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.2  2004/01/26 12:43:52  yada
# Added mi_dma.c
#
# Revision 1.1  2003/12/24 07:52:02  yada
# Moved management of WRAM related from GX to MI
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCDIR			= ../common/src src
INCDIR			= ../common/include

SRCS			= mi_wram.c \
			  mi_dma.c \
			  mi_dma_hblank.c \
			  mi_dma_vblank.c \
			  mi_dma_mainmem.c \
			  mi_dma_gxcommand.c \
			  mi_memory.c \
			  mi_swap.c \
			  mi_uncompress.c \
			  mi_byteAccess.c \
			  mi_stream.c \
			  mi_card.c \
			  mi_dma_card.c \
			  mi_uncomp_stream.c \
			  mi_compress.c \
			  mi_exMemory.c \
			  mi_init.c

TARGET_LIB		= libmi$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

