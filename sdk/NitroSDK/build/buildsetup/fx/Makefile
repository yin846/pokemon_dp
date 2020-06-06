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
# Revision 1.7  2004/07/17 09:39:55  yasu
# Check modification time on install
#
# Revision 1.6  2004/04/14 12:16:26  yasu
# := -> =
#
# Revision 1.5  2004/02/05 07:02:57  yasu
# change iris -> nitro
#
# Revision 1.4  2003/11/30 09:35:19  yasu
# Made compile ROOT and install root independent.
#
# Revision 1.3  2003/11/17 01:53:13  yasu
# Revised behavior of clean/clobber.
#
# Revision 1.2  2003/11/17 00:33:45  nishida_kenji
# Directory change
#
# Revision 1.1  2003/11/14 12:04:05  nishida_kenji
# New
#
# $NoKeywords:$
#----------------------------------------------------------------------------

WORKDIR			=	fx
TARGET			=	$(WORKDIR)/fx_const.h

CSVSRC			=	fx_const.csv
SCRIPT			=	fx_const_generator.pl

NEWDIRS			=	$(WORKDIR)

#----------------------------------------------------------------------------

# build
TARGETS			=	$(TARGET)

# install
INSTALL_TARGETS		=	$(TARGET)
INSTALL_DIR		=	$(NITRO_INSTALL_INCDIR)/nitro/$(WORKDIR)
override DO_INSTALL	=	$(DO_INSTALL_WILDCARD)

# clean/clobber
LDIRT_CLEAN		=	$(TARGETS) \
				$(firstword $(subst /, ,$(WORKDIR)))
LDIRT_CLOBBER		=

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

do-build:		$(TARGET)

$(TARGET):		$(CSVSRC) $(SCRIPT)
	$(CP)     $(CSVSRC) $(WORKDIR)/$(CSVSRC)
	$(PERL)   $(SCRIPT) $(WORKDIR)/$(CSVSRC)


#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#----- End of Makefile -----

