#! make -f
#----------------------------------------------------------------------------
# Project:  NitroDWC - base - libraries
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
# Revision 1.15  06/29/2006 09:52:55  takayama
# Removed dwc_common.c during the NITRODWC_NOGS options
#
# Revision 1.14  04/27/2006 11:17:33  takayama
# Revised the NITRODWC_NOGS options from being browser-oriented to more standard
#
# Revision 1.13  04/25/2006 12:03:51  nakata
# Added the DWC_Nd wrapper
#
# Revision 1.12  04/21/2006 01:33:43  takayama
# Made adjustments to the build environment on NITORDWC_NOGS
#
# Revision 1.11  04/20/2006 13:21:11  takayama
# Support for the NITRODWC_NOGS options
#
# Revision 1.10  04/10/2006 10:16:57  hayashi
# added the ranking module
#
# Revision 1.9  03/07/2006 01:18:16  nakata
# Performed necessary changes resulting from collecting added NAS functionality into dwc_nasfunc.c/dwc_nasfunc.h.
#
# Revision 1.8  12/26/2005 04:23:17  nakata
# Added NAS return code 40 related functions to Auth.
# Added DWC_GetIngamesnCheckResult() to dwc_common.c/dwc_common.h.
# Fixed the bug that incorrect ingamesn is sent when auth is called in netcheck.
#
# Revision 1.7  2005/09/05 05:16:25  onozawa
# Added dwc_ghttp.c
#
# Revision 1.6  08/31/2005 11:25:35  sasakit
# dwc_report has been moved to the account module.
#
# Revision 1.5  08/31/2005 02:29:44  sasakit
# The module was split so that code used for initialization could be separated by overlay.
#
# Revision 1.4  08/19/2005 12:12:10  sasakit
# Revised for release
#
# Revision 1.3  08/19/2005 06:10:15  sasakit
# Added files.
#
# Revision 1.2  08/17/2005 13:15:01  arakit
# Improved the algorithm used for matching with a friend specification just like other matching formats.
# Made it so QR2 keys defined by the game can be set.
# Made it so the SB server update filter can be specified from the game.
# Created a system in which the server found by SB server update is evaluated and the priority level of matching is determined by the game,
#  
# Corrected a matching bug.
# Made it so matching can be cancelled at any time (implementation unfinished in the case of server-client type matching)
#
# Revision 1.1.1.1  2005/08/02 08:02:37  sasakit
# First import of NitroDWC source tree.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

SUBDIRS		=

#----------------------------------------------------------------------------

SRCS  = \
	dwc_error.c \
	dwc_memfunc.c \
	dwc_connectinet.c \
	dwc_nasfunc.c \
	dwc_nd.c

ifndef NITRODWC_NOGS
SRCS += \
	dwc_common.c \
	dwc_main.c \
	dwc_login.c \
	dwc_friend.c \
	dwc_match.c \
	dwc_transport.c \
	dwc_ghttp.c \
	dwc_ranking.c
endif

NITRODWC_MODULE_NAME :=   base
TARGET_LIB	=	lib$(NITRODWC_LIB_PREFIX)$(NITRODWC_MODULE_NAME)$(NITRO_LIBSUFFIX).a

CCFLAGS_MSGSTYLE = -msgstyle gcc

#----------------------------------------------------------------------------

include	$(NITRODWC_ROOT)/build/buildtools/commondefs

INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITRODWC_INSTALL_LIBDIR)

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(NITRODWC_ROOT)/build/buildtools/modulerules

#----------------------------------------------------------------------------

#===== End of Makefile =====
