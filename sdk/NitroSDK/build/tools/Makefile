#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - tools
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
# Revision 1.28  11/30/2005 01:31:48  yasu
# Added chscope
#
# Revision 1.27  10/21/2005 00:04:12  okubata_ryoma
# Added xml2env.
#
# Revision 1.26  10/20/2005 02:13:58  kitase_hirotake
# Added init2env.
#
# Revision 1.25  08/31/2005 06:38:47  yada
# add loadrun
#
# Revision 1.24  07/21/2005 02:41:01  yada
# add buryarg
#
# Revision 1.23  06/21/2005 06:22:33  yasu
# Created initial version
#
# Revision 1.22  06/20/2005 07:07:38  yasu
# add defval
#
# Revision 1.21  06/13/2005 23:58:49  yasu
# Revised copyright display
#
# Revision 1.20  06/13/2005 02:57:50  yasu
# add bin2obj
#
# Revision 1.19  11/02/2004 11:30:20  takano_makoto
# Released emuchild.exe
#
# Revision 1.18  10/03/2004 09:03:19  yasu
# add showversion
#
# Revision 1.17  09/29/2004 02:30:52  yasu
# add sdkindent
#
# Revision 1.16  09/17/2004 09:14:39  yasu
# Add addbanner and makebanner-default
#
# Revision 1.15  09/01/2004 07:17:45  yasu
# Add compstatic
#
# Revision 1.14  09/01/2004 01:29:25  miya
# add attachsign
#
# Revision 1.13  08/30/2004 08:41:14  yasu
# makebanner moves into CVS tree
#
# Revision 1.12  07/28/2004 07:00:21  yasu
# 'ppmconv' as new tool.
#
# Revision 1.11  06/17/2004 04:41:04  yosizaki
# add mic2wav.
#
# Revision 1.10  05/06/2004 10:18:18  yasu
# fix typo
#
# Revision 1.9  04/06/2004 13:10:53  yasu
# add slicerom
#
# Revision 1.8  03/04/2004 05:48:49  yasu
# add ntrcomp
#
# Revision 1.7  02/23/2004 06:33:47  yasu
# NITRO_PRIVATES -> NITRO_PRIVATE
#
# Revision 1.6  02/05/2004 07:09:03  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.5  01/19/2004 23:55:15  yasu
# fix dir name
#
# Revision 1.4  01/19/2004 14:21:42  yasu
# Removed makerom temporarily
#
# Revision 1.3  01/14/2004 02:11:38  yasu
# Connect make chain
#
# Revision 1.2  10/30/2003 05:51:52  yasu
# build: deleted target
#
# Revision 1.1  10/29/2003 13:27:27  yasu
# Completed the make propagation framework
# Made all/install/clean/clobber available
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS 	= bin2obj

ifdef	NITRO_PRIVATE
SUBDIRS 	+= makelcf		\
		   makelcf/test		\
		   makerom		\
		   makerom/test		\
		   addbanner		\
		   makebanner		\
		   makebanner/test	\
		   makebanner-default	\
		   makebanner		\
		   makebanner/test	\
		   compstatic		\
		   compstatic/test	\
		   ntrcompbw		\
		   ntrcomp		\
		   slicerom		\
		   mic2wav		\
		   ppmconv		\
		   attachsign		\
		   showversion		\
		   sdkindent		\
		   emuchild		\
		   bin2obj		\
		   defval		\
		   defval/test		\
		   buryarg		\
		   loadrun		\
		   xml2env		\
		   init2env		\
		   chscope

ifdef	IS_NITRO_DIR
SUBDIRS 	+= nitload-gcc
endif
endif


#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
