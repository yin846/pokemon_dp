#----------------------------------------------------------------------------
# Project:  Nintendo Nitro System G3D -
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
# Revision 1.11  08/17/2004 10:44:00  nishida_kenji
# Add 1mat1shp.c
#
# Revision 1.10  07/27/2004 13:53:27  nishida_kenji
# 3dmax.c -> 3dsmax.c
#
# Revision 1.9  07/16/2004 07:27:34  nishida_kenji
# model.c
#
# Revision 1.8  07/15/2004 12:59:29  nishida_kenji
# add xsi.c xsi.h
#
# Revision 1.7  07/10/2004 08:48:00  nishida_kenji
# (none)
#
# Revision 1.6  07/08/2004 12:49:44  nishida_kenji
# add several files
#
# Revision 1.3  06/24/2004 01:24:06  nishida_kenji
# Fix header comments.
#
# Revision 1.2  06/15/2004 09:50:52  nishida_kenji
# (none)
#
# Revision 1.1  06/14/2004 06:24:36  nishida_kenji
# Initial check in
#
# $NoKeywords: $
#----------------------------------------------------------------------------


#----------------------------------------------------------------------------

SRCS		=	kernel.c                     \
				glbstate.c                   \
				anm.c                        \
				cgtool.c                     \
				sbc.c                        \
				1mat1shp.c                   \
				gecom.c                      \
				util.c                       \
				mem.c                        \
				model.c                      \
				binres/res_print.c           \
                binres/res_print_nsbca.c     \
                binres/res_print_nsbma.c     \
                binres/res_print_nsbta.c     \
                binres/res_print_nsbtp.c     \
                binres/res_print_nsbva.c     \
                binres/res_struct.c          \
                binres/res_struct_accessor.c \
				binres/res_struct_accessor_anm.c \
				anm/nsbca.c                  \
				anm/nsbma.c                  \
				anm/nsbta.c                  \
				anm/nsbtp.c                  \
				anm/nsbva.c                  \
				cgtool/basic.c               \
				cgtool/maya.c                \
				cgtool/si3d.c                \
				cgtool/3dsmax.c				 \
				cgtool/xsi.c

TARGET_LIB	=	libnnsg3d$(NITRO_LIBSUFFIX).a


#SRCDIR		=	# using default
SRCDIR		= 	src src/binres src/anm src/cgtool
#LCFILE		=	# using default
#LINCLUDES  =   ./


INSTALL_TARGETS	=	$(TARGETS)
INSTALL_DIR		=	$(NITROSYSTEM_INSTALL_LIBDIR)

#----------------------------------------------------------------------------
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs



do-build:		$(TARGETS)


include	$(NITROSYSTEM_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====



