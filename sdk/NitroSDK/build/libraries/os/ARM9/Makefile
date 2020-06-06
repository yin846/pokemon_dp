#! make -f
#----------------------------------------------------------------------------
#  Project:  NitroSDK - libraries - os
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
# Revision 1.24  05/25/2005 11:45:18  terui
# Core section that creates library was moved to Makefile.CORE 
# TS-Prototype/undefine added to SUBDIRS
#
# Revision 1.23  04/26/2005 08:04:17  terui
# Fix comment
#
# Revision 1.22  04/18/2005 12:25:35  terui
# Added os_china.c to SRCS
#
# Revision 1.21  02/08/2005 08:39:00  terui
# Added os_vramExclusive to SRCS
#
# Revision 1.20  01/12/2005 11:10:37  takano_makoto
# fix copyright header.
#
# Revision 1.19  01/12/2005 01:02:18  yada
# add about SDK_TCM_APPLY
#
# Revision 1.18  09/13/2004 05:37:28  yasu
# merged with NitroSDK_2_00pr6_branch
#
# Revision 1.17.2.1  09/13/2004 04:39:29 AM  yasu
# build libos for TS_VERSION=0,100 and 200
#
# Revision 1.17  09/02/2004 07:13:15  yada
# add os_ownerInfo.c
#
# Revision 1.16  09/01/2004 07:06:02  sato_masaki
# add os_reset.c
#
# Revision 1.15  08/27/2004 09:22:05  yasu
# Fix around NITRO_CODEGEN_ALL
#
# Revision 1.14  07/14/2004 14:12:07  yasu
# Change NITRO_CODEGEN=ALL -> NITRO_CODEGEN_ALL=True
#
# Revision 1.13  07/14/2004 11:52:57  yasu
# Build ARM & THUMB code always
#
# Revision 1.12  06/02/2004 01:41:52  yasu
# split profile libs from oslib
#
# Revision 1.11  05/18/2004 06:12:20  yada
# small fix
#
# Revision 1.10  05/12/2004 05:16:50  yada
# consider for CALLTRACE and FUNCTIONCOST
#
# Revision 1.9  05/10/2004 11:45:04  yada
# add profile functions
#
# Revision 1.8  05/06/2004 10:18:18  yasu
# fix typo
#
# Revision 1.7  04/23/2004 00:28:25  yada
# Added BLXCHECKED
#
# Revision 1.6  04/13/2004 12:21:23  yada
# add os_callTrace.c
#
# Revision 1.5  02/27/2004 01:58:06  yasu
# delete os_halt.c
#
# Revision 1.4  02/25/2004 11:31:45  yada
# Changes related to switch from systemClock to Tick 
#
# Revision 1.3  02/23/2004 11:56:39  yada
# Added valarm.c
#
# Revision 1.2  02/14/2004 06:34:25  yasu
# add os_halt.c
#
# Revision 1.1  02/12/2004 11:09:37  yada
# ARM9/ARM7 sorting 
#
# Revision 1.26  02/10/2004 01:23:41  yada
# Deleted os_utility.c
#
# Revision 1.25  02/09/2004 11:36:34  yada
# Added os_system.c
#
# Revision 1.24  02/09/2004 11:17:49  yasu
# support thumb version library
#
# Revision 1.23  02/05/2004 07:09:02  yasu
# change SDK prefix "iris" to "nitro"
#
# Revision 1.22  02/05/2004 01:58:55  yada
# Added os_utility.c
#
# Revision 1.21  02/03/2004 11:20:12  yada
# Added os_alarm.c
#
# Revision 1.20  02/02/2004 05:30:02  yada
# Added systemClock.c
#
# Revision 1.19  01/27/2004 01:47:13  yada
# Added os_timer.c
#
# Revision 1.18  01/16/2004 07:57:15  yada
# Description of protectionUnit
#
# Revision 1.17  01/14/2004 12:05:39  yada
# Added os_tcm.c, os_protecion.c, and os_exception.c
#
# Revision 1.16  01/07/2004 00:21:50  yada
# Added src/os_alloc.c
#
# Revision 1.15  12/26/2003 06:42:58  yada
# Added os_arena
#
# Revision 1.14  12/22/2003 01:36:34  yada
# Added os_cache.c and os_init.c
#
# Revision 1.13  12/17/2003 09:33:45  yada
# Added os_cache.c
#
# Revision 1.12  11/30/2003 09:35:19  yasu
# Made compile ROOT and install ROOT independent.
#
# Revision 1.11  11/29/2003 10:11:09  yada
# Added os_mutex.c
#
# Revision 1.10  11/29/2003 04:37:01  yasu
# Revised names
#
# Revision 1.9  11/29/2003 04:04:19  yasu
# Changed os library file names
#
# Revision 1.8  11/29/2003 03:59:51  yasu
# Changed file name  osInterruptBase.c -> os_irqhandler.c
#
# Revision 1.7  11/26/2003 01:50:03  yada
# Added osMessage.c
#
# Revision 1.6  11/25/2003 11:18:47  yasu
# Added emulator.c
#
# Revision 1.5  11/25/2003 05:02:28  yasu
# Added thread.c / context.c
#
# Revision 1.4  11/13/2003 11:59:01  yasu
# Created initial version
#
# Revision 1.3  11/11/2003 01:52:45  nishida_kenji
# Added .c file
#
# Revision 1.2  11/06/2003 13:37:49  yasu
# Changed osInterruptBase.s  to .c
#
# Revision 1.1  11/06/2003 11:34:14  yasu
# Made additions related to interrupt handler
#
# $NoKeywords: $
#----------------------------------------------------------------------------

#---- define if let CallTrace/FunctionCost be available, 
NITRO_PROFILE_TYPE	=
#NITRO_PROFILE_TYPE	= CALLTRACE
#NITRO_PROFILE_TYPE	= FUNCTIONCOST

#----------------------------------------------------------------------------

SUBDIRS			= TS-Prototype/0 TS-Prototype/100 TS-Prototype/200 TS-Prototype/undefine
SUBMAKES		= Makefile.CALLTRACE Makefile.FUNCTIONCOST

#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs

include	$(NITROSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====

