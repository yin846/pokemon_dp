#! make -f
#----------------------------------------------------------------------------
# Project: NitroWiFi - tools
# File: Makefile
#
# Copyright 2003-2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.5  03/16/2006 00:33:59  kitase_hirotake
# corrected wifiindent for PRIVATE only
#
# Revision 1.4  03/15/2006 09:52:49  kitase_hirotake
# add wifiindent
#
# Revision 1.3  02/28/2006 08:49:40  okubata_ryoma
# changes resulting from NitroCrypto removal
#
# Revision 1.2  01/30/2006 02:36:53  seiki_masashi
# Added the certconv tool.
#
# Revision 1.1  01/23/2006 06:44:35  seiki_masashi
# Added the rc4conv tool.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS 	=  certconv

ifdef	NITROWIFI_NOSUPPORT
SUBDIRS 	+= wifiindent
endif


#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
