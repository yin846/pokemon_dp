#! make -f
#----------------------------------------------------------------------------
# Project:  NitroSDK - root
# File:     Makefile
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
# Revision 1.22  01/29/2006 23:56:59  kitase_hirotake
# Fixed misidentification that occurred when there was white space before the year. 
#
# Revision 1.21  01/16/2006 00:39:42  kitase_hirotake
# Changed so unnecessary indents are not used.
#
# Revision 1.20  01/11/2006 06:25:45  kitase_hirotake
# 2005->2006 supported
#
# Revision 1.19  07/20/2005 03:59:49  yasu
# Modified such that files named '_' are skipped during search.
#
# Revision 1.18  07/18/2005 01:33:00  yasu
# Revised copyright.
#
# Revision 1.17  07/15/2005 01:12:24  yasu
# Revised copyright of check code.
#
# Revision 1.16  07/07/2005 00:31:37  yasu
# Revised copyright display check
#
# Revision 1.15  07/06/2005 00:03:49  yasu
# Added target check_code
#
# Revision 1.14  02/28/2005 04:36:25  yosizaki
# add do-copyright.
#
# Revision 1.13  02/25/2005 07:40:45  yasu
# Made it so only CC is enabled when checking the "indent" process
#
# Revision 1.12  10/25/2004 11:25:26  yasu
# Use libdiff
#
# Revision 1.11  10/05/2004 02:30:37  yasu
# Set LMACRO_FLAGS=1 instead of MACRO_FLAGS
#
# Revision 1.10  10/05/2004 00:19:31  yasu
# Fix SDK_NO_MESSAGE
#
# Revision 1.9  10/04/2004 11:47:41  yasu
# Corrected the "find" conditions
#
# Revision 1.8  10/03/2004 10:01:18  yasu
# Fix sdkindent rules
#
# Revision 1.7  10/03/2004 09:32:10  yasu
# renew indent rules
#
# Revision 1.6  06/29/2004 00:59:49  yasu
# Add setup libcxx for workaround against lcf error
#
# Revision 1.5  05/06/2004 10:18:18  yasu
# fix typo
#
# Revision 1.4  03/17/2004 07:31:04  yasu
# add indent correction rule
#
# Revision 1.3  02/05/2004 07:12:05  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.2  10/30/2003 05:51:52  yasu
# build: deleted target
#
# Revision 1.1  10/29/2003 13:27:26  yasu
# Completed the make propagation framework
# Made all/install/clean/clobber available
#
# $NoKeywords: $
#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/commondefs


#----------------------------------------------------------------------------

SUBDIRS 	= build


#----------------------------------------------------------------------------

include	$(NITROSDK_ROOT)/build/buildtools/modulerules


#----------------------------------------------------------------------------
.PHONY:		setup

setup:
	$(MAKE) -C build/buildsetup/libcxx

#----------------------------------------------------------------------------
.PHONY:	indent-all do-indent do-rename do-diff

SDKINDENT	= $(NITRO_TOOLSDIR)/bin/sdkindent
SDKDIFF		= $(NITRO_TOOLSDIR)/bin/sdkindent.libdiff
SDKDIR_BUILD	?= build
SDKDIR_INCLUDE	= include
#SDKDIR_BUILDOPT	= LD=true MAKEROM=true MAKELCF=true
SDKDIR_BUILDOPT	= 
SDKCOPYRIGHT	= $(NITRO_TOOLSDIR)/bin/sdkindent.copyright

indent-all:
	( export LMACRO_FLAGS=-DSDK_NO_MESSAGE=1;		\
	  export NITRO_NO_OPT_G=1;				\
	  $(MAKE) -kB $(SDKDIR_BUILDOPT) -C $(SDKDIR_BUILD) $(SDKINDENT_BUILD);	\
	  $(REMAKE) do-rename;					\
	  $(REMAKE) do-indent;					\
	  $(REMAKE) do-copyright;				\
	  $(MAKE) -skB $(SDKDIR_BUILDOPT) -C $(SDKDIR_BUILD) $(SDKINDENT_BUILD); \
	  $(REMAKE) do-diff )

indent-sub:
	( export LMACRO_FLAGS=-DSDK_NO_MESSAGE=1;		\
	  export NITRO_NO_OPT_G=1;				\
	  $(REMAKE) do-indent;					\
	  $(MAKE) -skB -C $(SDKDIR_BUILD) $(SDKINDENT_BUILD);	\
	  $(REMAKE) do-diff )

indent-diff:
	  $(REMAKE) do-diff

do-indent:
	@echo ===== INDENT STAGE =====
	/bin/find $(SDKDIR_BUILD)   -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print -exec $(SDKINDENT) {} \;
	/bin/find $(SDKDIR_INCLUDE) -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print -exec $(SDKINDENT) {} \;

do-copyright:
	@echo ===== COPYRIGHT STAGE =====
	/bin/find $(SDKDIR_BUILD)   -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print \
		-mtime -`/bin/date +%j` \
		-exec $(SDKCOPYRIGHT) {} \;
	/bin/find $(SDKDIR_INCLUDE) -type f \
		-name '*.[ch]' ! -name '*.yy.c' ! -name '*.yy.patched.c' \
		-size -128k -print \
		-mtime -`/bin/date +%j` \
		-exec $(SDKCOPYRIGHT) {} \;

do-rename:
	@echo ===== RENAME STAGE =====
	/bin/find $(SDKDIR_BUILD) -type f -name '*.o' -exec $(MV) {} {}.backup \;

do-diff:
	@echo ===== COMPARE STAGE =====
	/bin/find $(SDKDIR_BUILD) -type f -name '*.o' -exec $(SDKDIFF) {} {}.backup \;

#----------------------------------------------------------------------------
.PHONY:	check_code check_lang_en check_copyright_date_2006

CHECKEES	= build include add-ins

check_code: check_lang_en check_copyright_date_2006

#
#  Check backslash code for language EN
#	Inverse result code of grep (e.g. grep returns 1 if not found)
#	 -> Error if grep finds 0x5c(\) on 2nd byte of 16bit-wide
#           Japanese kanji code at the end of line
#
check_lang_en:
	@echo --- $@ ---
	-@! grep -I -n -r --exclude='*.txt' "[^[:space:] -~][\]$$" $(CHECKEES)

#
#  Check Copyright date for 2006
#
check_copyright_date_2006:
	@echo --- $@ ---
	-@for dir in $(CHECKEES); do \
	    /bin/find $$dir \
		\! \( -type d \( -name '_*' -o \
				 -name CVS  -o \
				 -exec /bin/test -e {}/_ \; \) -prune \) \
		-type f \
		\! -name '*.exe' \! -name '*.[dao]' \! -name '*.sbin' \
		\! -name '*.nlf' \! -name '*.nef'   \! -name '*.xMAP' \
		-mtime -`/bin/date +%j` \
		-exec grep -q 'Copyright [0-9 ,-]* Nintendo' {} \; \
		\! -exec grep -q 'Copyright [0-9 ,-]*2006' {} \; \
		-printf "Error: Copyright Date: %p\n"; \
	done

#===== End of Makefile =====
