#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - buildsetup - create register map
# File:     Makefile
#
# Copyright 2003-2004 Nintendo. All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law. They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.8  2004/07/17 09:39:55  yasu
# Check modification time on install
#
# Revision 1.7  2004/04/14 12:16:26  yasu
# := -> =
#
# Revision 1.6  2004/02/17 12:58:01  yasu
# fix install files
#
# Revision 1.5  2004/02/12 10:56:04  yasu
# new location of include files ARM9/ARM7
#
# Revision 1.4  2004/02/05 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.3  2003/12/16 09:45:31  yasu
# Support for nitro_sp
#
# Revision 1.2  2003/11/30 09:35:19  yasu
# Made compile ROOT and install root independent.
#
# Revision 1.1  2003/11/17 01:54:50  yasu
# Moved ioreg generating script to below ioreg/
#
# Revision 1.9  2003/11/14 12:04:21  nishida_kenji
# Added fx
#
# Revision 1.8  2003/11/12 12:03:25  yasu
# Made so that make clobber will delete INSTALL_TARGETS.
#
# Revision 1.7  2003/11/06 13:42:01  yasu
# Restored chain to sp.
#
# Revision 1.6  2003/11/05 10:32:23  yasu
# Temporarily removed link to sp.
#
# Revision 1.5  2003/11/05 09:58:40  yasu
# Revised NITRO_INC_DIR to NITRO_INCDIR
#
# Revision 1.4  2003/11/05 07:05:14  yasu
# Changes that follow revision of commondefs/modulerule
# Set chain to sp/
#
# Revision 1.3  2003/10/30 05:42:36  yasu
# Revision: Mistaken TARGET specification
#
# Revision 1.2  2003/10/30 04:48:06  yasu
# autogen directory -> hw directory
# Changed install target to $irisSDK/include/iris/hw
#
# Revision 1.1  2003/10/29 13:27:27  yasu
# Completed make propagation framework.
# Can use all/install/clean/clobber
#
# $NoKeywords:$
#----------------------------------------------------------------------------

SUBDIRS			=

#----------------------------------------------------------------------------

WORKDIR			=	nitro/hw/ARM9
TARGET			=	$(WORKDIR)/ioreg.h

CSVSRC			=	io_register_list.csv
SCRIPT			=	header_generator_ioreg.pl

NEWDIRS			=	$(WORKDIR)

#----------------------------------------------------------------------------

TARGET_PREFIX		=	$(subst .h,,$(TARGET))
TARGET_SUBFILES		=	$(TARGET_PREFIX)_*.h
TARGET_TMPCSV		=	$(TARGET_PREFIX).csv

# build
TARGETS			=	$(TARGET) $(TARGET_SUBFILES)

# install
INSTALL_TARGETS		=	$(TARGETS)
INSTALL_DIR		=	$(NITRO_INSTALL_INCDIR)/$(WORKDIR)
override DO_INSTALL	=	$(DO_INSTALL_WILDCARD)

# clean/clobber
LDIRT_CLEAN		=	$(TARGET_TMPCSV) $(TARGETS) \
				$(firstword $(subst /, ,$(WORKDIR)))
LDIRT_CLOBBER		=

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

do-build:		$(TARGET)

$(TARGET):		$(CSVSRC) $(SCRIPT)
	$(CP)     $(CSVSRC) $(TARGET_TMPCSV)
	$(PERL)   $(SCRIPT) $(TARGET_TMPCSV)

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#----- End of Makefile -----

