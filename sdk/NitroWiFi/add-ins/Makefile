#! make -f
#----------------------------------------------------------------------------
# Project:  NitroWiFi - add-ins
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
# Revision 1.3  09/17/2005 12:46:12  terui
# Modified so make in NitroDWC is not performed.
#
# Revision 1.2  09/06/2005 00:19:42  yasu
# Stopped compile due to porting env to NitroSDK
#
# Revision 1.1  08/19/2005 12:28:58  yasu
# Support add-ins
#
# $NoKeywords: $
#----------------------------------------------------------------------------
include	$(NITROSDK_ROOT)/build/buildtools/commondefs

# <Note>
#
# This Makefile exports the following symbols, in order,
# NITRO_ONLY_SETUP
# NITRO_ONLY_LIBS
# NITRO_ONLY_DEMOS
# NITRO_ONLY_CLEANUP
# and runs make four times. (Only during Nightly Build)
# The Makefiles in each of the SUBDIRS must distinguish these using ifdef.
#

SUBDIRS 	+= 

#ifdef	NITRO_NOSUPPORT
#SUBDIRS 	+= NitroDWC
#endif

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
