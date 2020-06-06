#! make -f
#----------------------------------------------------------------------------
# Project:  NITRO-System - build
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
# Revision 1.14  07/24/2004 00:30:13  nishida
# Changed so tools and below are buildt only as PRIVATE.
#
# Revision 1.13  07/23/2004 00:01:58  nishida
# Changed so after tests and below are buildt only as PRIVATE.
#
# Revision 1.12  06/14/2004 04:29:22  nishida
# Corrected spelling (instructions).
#
# Revision 1.11  05/10/2004 06:05:20  nishida
# Fixed so build is performed below tests.
#
# Revision 1.10  04/21/2004 00:19:42  nishida
# Fixed NITRO_WITHOUT_DEMOS defintions so build is not performed on model.
#
# Revision 1.9  03/31/2004 00:24:53  ida
# remove build components
#
# Revision 1.8  03/29/2004 01:55:28  ida
# build tools before demo
#
# Revision 1.7  03/26/2004 06:54:37  ida
# add componets
#
# Revision 1.6  03/22/2004 09:40:24  kitamurs
# Added demos to subdirectory.
#
# Revision 1.5  03/18/2004 01:21:15  ida
# add tools directory
#
# Revision 1.4  02/19/2004 08:56:30  ida
# remove components
#
# Revision 1.3  02/17/2004 05:45:43  ida
# add "components"
#
# Revision 1.2  02/13/2004 01:41:17  ida
# remove libraries_sp
#
# Revision 1.1  01/27/2004 09:00:23  nishida
# Registered the initial version.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------
ifdef	NITROSYSTEM_PRIVATE
SUBDIRS.TOOLS = tools
endif

SUBDIRS = \
		libraries \
		$(SUBDIRS.TOOLS) \
		$(SUBDIRS.TESTS)


#----------------------------------------------------------------------------

include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====

