#! make -f
#----------------------------------------------------------------------------
# Project:  Nintendo Nitro System G2D -
# File:     Makefile
#
# Copyright 2004,2005 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: Makefile,v $
# Revision 1.16  06/27/2005 05:55:19  hatamoto_minoru
# Added g2di_BGManipulate.
#
# Revision 1.15  05/20/2005 04:54:21  hatamoto_minoru
# Added Font, CharCanvas, and TextCanvas.
#
# Revision 1.14  12/13/2004 08:24:28  kitani_toshikazu
# Merged branch g2d_multiCell
#
# Revision 1.13.8.2  2004/12/13 02:55:02  kitani_toshikazu
# Absorbed new multi-cell module in old module.
#
# Revision 1.13.8.1  2004/12/09 06:05:50  kitani_toshikazu
# (none)
#
# Revision 1.13  10/07/2004 12:46:20  kitani_toshikazu
# Added g2d_RendererCore.c
#
# Revision 1.12  09/02/2004 07:08:10  hatamoto_minoru
# Added BG related.
#
# Revision 1.11  07/17/2004 07:23:32  kitani_toshikazu
# Changed file name from g2d_OamSoftwareSpriteEmu to g2d_OamSoftwareSpriteDraw.
#
# Revision 1.10  07/17/2004 06:54:52  kitani_toshikazu
# Separated OAM software emulation drawing relation as separate module.
#
# Revision 1.9  07/14/2004 12:27:38  kitani_toshikazu
# Support for Vram transfer animation.
#
# Revision 1.8  06/21/2004 04:53:25  kitani_toshikazu
# Combined OAM managers Ex2 and Ex.
#
# Revision 1.7  06/21/2004 02:32:56  kitani_toshikazu
# Combined OAM managers Ex2 and Ex.
#
# Revision 1.6  06/14/2004 04:32:48  nishida
# Corrected spelling (instructions).
#
# Revision 1.5  05/10/2004 04:42:46  kitani_toshikazu
# Check-in for PR1.
#
# Revision 1.4  04/23/2004 00:27:24  kitani_toshikazu
# Added workaround for color palette swap table.
#
# Revision 1.3  04/21/2004 07:11:27  kitani_toshikazu
# Updated the folder structures related to the internal module
#
# Revision 1.2  04/21/2004 05:49:40  kitani_toshikazu
# Corrected a bug : g2d_debug_i.h => g2d_debug_i.c
#
# Revision 1.1  04/21/2004 04:22:28  kitani_toshikazu
# Initial checkin.
#
# $NoKeywords: $
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------

SRCS		=	g2d_NAN_load.c			\
				g2d_NOB_load.c			\
				g2d_NMC_load.c			\
				g2d_NEN_load.c			\
				g2d_NCG_load.c			\
				g2d_NCL_load.c			\
				g2d_NSC_load.c			\
				g2d_Load.c				\
				g2d_Softsprite.c		\
				g2d_Animation.c 		\
				g2d_OAM.c				\
				g2d_OAMEx.c				\
				g2d_PaletteTable.c		\
				g2d_Image.c				\
				g2d_SRTControl.c		\
				g2d_Node.c 				\
				g2d_CullingUtility.c	\
				g2d_CellAnimation.c		\
				g2d_MultiCellAnimation.c \
				g2d_Entity.c			\
				g2d_Renderer.c			\
				g2di_Mtx32.c			\
				g2di_debug.c			\
				g2d_CellTransferManager.c 		\
				g2d_OamSoftwareSpriteDraw.c		\
				g2d_Screen.c			\
				g2d_RendererCore.c		\
				g2d_Font.c			\
				g2d_CharCanvas.c	\
				g2d_TextCanvas.c	\
				g2d_NFT_load.c		\
				g2di_BitReader.c	\
				g2di_SplitChar.c	\
				g2di_BGManipulator.c	\
				
TARGET_LIB	=	libnnsg2d$(NITRO_LIBSUFFIX).a

#SRCDIR		=	# using default
SRCDIR		= 	src src/internal src/load
#LCFILE		=	# using default



INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)

#----------------------------------------------------------------------------
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs

do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====

