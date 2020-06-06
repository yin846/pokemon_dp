#! make -f
#----------------------------------------------------------------------------
# Project:   NitroWiFi - SOC - libraries
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
# Revision 1.17  23.08.05 05:42:25  yasu
# NITROWIFI_INSTALL_ROOT related revisions
#
# Revision 1.16  18.08.05 14:19:23  yasu
# added cleanup and list
#
# Revision 1.15  08.08.05 07:16:43  seiki_masashi
# restored socl_ssl.c
# revised such that the path passed to sed is in cygwin format
#
# Revision 1.14  07.08.05 14:03:20  yasu
# added socl_misc.c/temporarily commented out socl_ssl.c
#
# Revision 1.13  06.08.05 12:55:54  seiki_masashi
# added SOCL_EnableSsl
#
# Revision 1.12  30.07.05 16:31:14  yasu
# revisions in conjunction with command pipe separation
#
# Revision 1.11  28.07.05 12:16:25  yasu
# implemented SOC_Poll
#
# Revision 1.10  25.07.05 11:15:01  yasu
# added the SOC library
#
# Revision 1.9  22.07.05 14:55:53  yasu
# added listen accept
#
# Revision 1.8  22.07.05 13:45:05  yasu
# pseudo-implementation of asynchronous processing
#
# Revision 1.7  19.07.05 15:06:39  yasu
# added SOCL_Read/Write
#
# Revision 1.6  18.07.05 14:16:36  yasu
# created send/receive threads
#
# Revision 1.5  15.07.05 14:33:11  yasu
# adjusted soc library source
#
# Revision 1.4  28.06.05 03:35:14  yasu
# separated socket thread creation function
#
# Revision 1.3  23.06.05 12:13:17  yasu
# load in NitroWiFi's commondefs, modulerules
#
# Revision 1.2  22.06.05 11:52:13  yasu
# created alpha version
#
# Revision 1.1  20.06.05 09:42:29  terui
# created new tree structure
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS			 =

#----------------------------------------------------------------------------
# build ARM & THUMB libraries
NITRO_CODEGEN_ALL 	?= True

#SRCDIR			= src
#INCDIR			=

SRCS			= socl_startup.c	\
			  socl_command.c	\
			  socl_create.c		\
			  socl_listen_accept.c	\
			  socl_bind.c		\
			  socl_read.c		\
			  socl_write.c		\
			  socl_shutdown.c	\
			  socl_close.c		\
			  socl_cleanup.c	\
			  socl_resolve.c	\
			  socl_poll.c		\
			  socl_const.c		\
			  socl_misc.c		\
			  socl_list.c		\
			  soc.c

SRCS			+= socl_ssl.c

TARGET_LIB		= libsoc$(NITRO_LIBSUFFIX).a

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

INSTALL_TARGETS		= $(TARGETS)
INSTALL_DIR		= $(NWIFI_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules

ERRTABLE_H	= include/errtable.h

$(OBJDIR)/socl_misc.o:	$(ERRTABLE_H)

$(ERRTABLE_H): $(NWIFI_ROOT)/include/nitroWiFi/socl.h
	mkdir -p include
	sed -e '/^\/\/ ERROR-CODE BEGINS/,/^\/\/ ERROR-CODE ENDS/!d;\
		/SOC_/!d;\
		s/^.*SOC_\([A-Z]*\).*$$/{ SOC_\1, "\1" },/g' $< > $@

LDIRT_CLEAN	= $(ERRTABLE_H)

#===== End of Makefile =====
