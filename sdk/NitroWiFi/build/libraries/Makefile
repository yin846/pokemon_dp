#! make -f
#----------------------------------------------------------------------------
# Project: NitroWiFi - libraries
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
# Revision 1.15  03/12/2006 23:55:09  okubata_ryoma
# Copyright fix
#
# Revision 1.14  02/28/2006 08:49:05  okubata_ryoma
# changes resulting from NitroCrypto removal
#
# Revision 1.13  12/21/2005 00:22:27  seiki_masashi
# revised crypt to be enclosed in NITROWIFI_HIDDEN
#
# Revision 1.12  12/13/2005 11:54:39  seiki_masashi
# added a private crypt directory
#
# Revision 1.11  08/19/2005 12:28:20  yasu
# Support for add-ins
#
# Revision 1.10  08/17/2005 10:49:55  yasu
# added add-ins/env
#
# Revision 1.9  07/29/2005 05:08:16  terui
# added stubs to SUBDIRS
#
# Revision 1.8  07/22/2005 14:04:13  yasu
# temporary commenting out
#
# Revision 1.7  07/22/2005 13:57:49  yasu
# indent revisions
#
# Revision 1.6  07/21/2005 11:20:14  seiki_masashi
# added bsafe to build targets
#
# Revision 1.5  07/15/2005 13:33:25  yasu
# Added soc library
#
# Revision 1.4  07/15/2005 09:27:32  yasu
# adjusted tree in conjunction with acceptance of cps and ssl libraries
#
# Revision 1.3  07/07/2005 10:46:40  terui
# added wcm to SUBDIRS
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

SUBDIRS			 = wcm soc stubs

#----------------------------------------------------------------------------

include	$(NITROWIFI_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
