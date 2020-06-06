#! make -f
#----------------------------------------------------------------------------
# Project: NitroWiFi - build
# File: Makefile
#
# Copyright 2005.2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.9  03/10/2006 09:21:49  kitase_hirotake
# deleted make content from tests down
#
# Revision 1.8  01/29/2006 23:54:49  okubata_ryoma
# Revised copyright.
#
# Revision 1.7  01/27/2006 08:13:31  seiki_masashi
# Added tools to the build path.
#
# Revision 1.6  08/19/2005 12:28:20  yasu
# Support for add-ins
#
# Revision 1.5  06/28/2005 04:51:18  yasu
# revisions to build conditions for buildsetup/versions
#
# Revision 1.4  06/28/2005 04:46:24  yasu
# added buildsetup/versions
#
# Revision 1.3  06/28/2005 04:06:30  yasu
# added NITROWIFI_NOSUPPORT flag
#
# Revision 1.2  06/23/2005 11:13:17  yasu
# load in NitroWiFi's commondefs, modulerules
#
# Revision 1.1  06/22/2005 11:07:57  yasu
# created source tree template
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------
# STAGE: SETUP
#
ifdef	NITROWIFI_NOSUPPORT
SUBMAKES.SETUP		+= buildsetup/versions/Makefile
endif
SUBMAKES.SETUP		+= $(NWIFI_ADDINSDIR)/Makefile.setup

#----------------------------------------------------------------------------
# STAGE: TOOLS
#
SUBMAKES.TOOLS		+= tools/Makefile

#----------------------------------------------------------------------------
# STAGE: LIBS
#
SUBMAKES.LIBS		+= libraries/Makefile			\
			   $(NWIFI_ADDINSDIR)/Makefile.libs

#----------------------------------------------------------------------------
# STAGE: CLEANUP
#
SUBMAKES.CLEANUP	+= $(NWIFI_ADDINSDIR)/Makefile.cleanup

#----------------------------------------------------------------------------

SUBMAKES		= $(SUBMAKES.SETUP)			\
			  $(SUBMAKES.TOOLS)			\
			  $(SUBMAKES.LIBS)			\
			  $(SUBMAKES.CLEANUP)

#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
