#! make -f
#----------------------------------------------------------------------------
# Project:  NitroDWC
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
# Revision 1.5  03/27/2006 06:37:41  takayama
# Made the add-ins directory private
#
# Revision 1.4  08/20/2005 04:11:10  sasakit
# Deleted unnecessary comments
#
# Revision 1.3  08/04/2005 12:45:52  sasakit
# Created a mechanism for taking snapshots.
#
# Revision 1.1  08/04/2005 09:33:05  sasakit
# Transplanted Makefiles for NitroSDK release.
#
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include $(NITRODWC_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

ifdef NITRODWC_PRIVATE
SUBDIRS = add-ins
endif

SUBDIRS += build

#----------------------------------------------------------------------------

include $(NITRODWC_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
