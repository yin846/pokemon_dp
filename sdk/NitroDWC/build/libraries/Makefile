#! make -f
#----------------------------------------------------------------------------
# Project:  NitroDWC - libraries
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
# Revision 1.5  06/29/2006 13:54:40  takayama
# Changed the order of commondefs
#
# Revision 1.4  09/05/2005 08:29:42  sasakit
# Corrected a problem where a malfunction would occur when there was no @privates file.
#
# Revision 1.3  08/31/2005 02:50:54  sasakit
# Made it so the account module compiles by default.
#
# Revision 1.2  08/19/2005 12:12:09  sasakit
# Revised for release
#
# Revision 1.1.1.1  2005/08/02 08:02:35  sasakit
# First import of NitroDWC source tree.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITRODWC_ROOT)/build/buildtools/commondefs

#----------------------------------------------------------------------------

SUBDIRS	=	base account

-include @privates

#----------------------------------------------------------------------------

include	$(NITRODWC_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
