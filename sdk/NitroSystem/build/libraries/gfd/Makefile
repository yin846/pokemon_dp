#! make -f
#----------------------------------------------------------------------------
# Project:  Nintendo Nitro System Gfd
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
# Revision 1.13  09/22/2004 10:51:09  kitani_toshikazu
# Take the bit array VRAM manager from the build chain
#
# Revision 1.12  09/10/2004 10:42:57  kitani_toshikazu
# Added linked list-type VRAM manager  (textures, palettes)
#
# Revision 1.11  09/03/2004 08:13:58  kitani_toshikazu
# Registered linked list format manager.
#
# Revision 1.10  09/03/2004 05:48:51  nishida
# Corrected the duplication of gfd_BitArrayTexVramMan.c
#
# Revision 1.9  09/02/2004 23:54:16  kitani_toshikazu
# Added gfd_BitArrayTexVramMan.c
#
# Revision 1.8  09/01/2004 05:11:04  kitani_toshikazu
# Added a bit array palette manager and separated out the shared portions.
#
# Revision 1.7  07/12/2004 04:38:27  nishida
# Added fd_BitArrayTexVramMan.c during PRIVATE build.
#
# Revision 1.6  07/10/2004 04:34:29  nishida
# Added gfd_TexVramMan.c and gfd_PlttVramMan.c.
#
# Revision 1.5  07/08/2004 07:17:58  kitani_toshikazu
# Added the VRAM transmission manager
#
# Revision 1.4  06/16/2004 05:14:35  kitani_toshikazu
# Arrangement of sources.
#
# Revision 1.3  06/14/2004 04:33:07  nishida
# Corrected spelling (instructions).
#
# Revision 1.2  06/10/2004 06:34:34  kitani_toshikazu
# Added FrmExVramManager.
#
# Revision 1.1  06/09/2004 09:33:29  kitani_toshikazu
# Added gfd
#
#
# $NoKeywords: $
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------

SRCS		=	gfd_TexVramMan.c			\
				gfd_PlttVramMan.c			\
				gfd_FrameTexVramMan.c		\
				gfd_FramePlttVramMan.c		\
				gfd_VramTransferManager.c	\
				gfdi_LinkedListVramMan_Common.c	\
				gfd_LinkedListTexVramMan.c	\
				gfd_LinkedListPlttVramMan.c	\

				
TARGET_LIB	=	libnnsgfd$(NITRO_LIBSUFFIX).a

SRCDIR		= 	src							\
				src/VramManager 			\
				src/VramTransferMan			
				
#LCFILE		=	# using default


INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)

NITRO_BLXCHECKED = yes
#----------------------------------------------------------------------------
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====

