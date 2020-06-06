#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - libraries - os
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
# Revision 1.3  05/25/2005 11:42:18  terui
# Revised so that Makefile.CORE is placed in an include statement
#
# Revision 1.2  09/13/2004 05:37:28  yasu
# merged with NitroSDK_2_00pr6_branch
#
# Revision 1.1.2.1  09/13/2004 04:39:29  yasu
# build libos for TS_VERSION=0,100 and 200
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=
SUBMAKES	=

#----------------------------------------------------------------------------
# Set TS version
#
override NITRO_TS_VERSION = 0

TS_VERSION	= _TS$(NITRO_TS_VERSION)
SRCDIR		= ../../../common/src ../../src

include	../../Makefile.CORE


#===== End of Makefile =====

