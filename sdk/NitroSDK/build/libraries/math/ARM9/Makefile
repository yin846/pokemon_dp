#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - math
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
# Revision 1.7  05/13/2005 09:23:09  seiki_masashi
# Added FFT function
#
# Revision 1.6  04/12/2005 11:12:27  seiki_masashi
# Added checksum.h 
#
# Revision 1.5  04/12/2005 10:15:36  takano_makoto
# Add qsort.c.
#
# Revision 1.4  04/11/2005 10:35:56  seiki_masashi
# Added dgt.c 
#
# Revision 1.3  04/11/2005 01:48:53  seiki_masashi
# Added crc.c
#
# Revision 1.2  01/11/2005 07:39:59  takano_makoto
# fix copyright header.
#
# Revision 1.1  01/06/2005 06:25:50  seiki_masashi
# Made chage so library for ARM7 is also created
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=


#----------------------------------------------------------------------------

# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

SRCDIR			= ../common/src src

SRCS			= math.c dgt.c crc.c checksum.c qsort.c fft.c

TARGET_LIB		= libmath$(NITRO_LIBSUFFIX).a


include	$(NITROSDK_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NITRO_INSTALL_LIBDIR)


#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

