#! make -f
#----------------------------------------------------------------------------
#  Project:  NitroSDK - buildtools - compiler dependant settings for CodeWarrior
#  File:     modulerules.cctype.CW
#
#  Copyright 2003-2006 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Log: modulerules.cctype.CW,v $
# Revision 1.67  05/01/2006 01:15:43  okubata_ryoma
# Made changes in line with the change to the method of comparing CW versions
#
# Revision 1.66  03/20/2006 08:01:28  okubata_ryoma
# revisions to prevent a build error resulting from the sed command version
#
# Revision 1.65  02/21/2006 00:11:15  okubata_ryoma
# Copyright fix
#
# Revision 1.64  02/13/2006 08:32:39  okubata_ryoma
# Added support for CW2.0beta
#
# Revision 1.63  09/17/2005 06:17:51  yasu
# Corrected compiler version information processing.
#
# Revision 1.62  09/17/2005 04:56:24  yasu
# Updated compiler version information.
#
# Revision 1.61  08/23/2005 05:12:25  yasu
# Eliminated the -D option that was unrelated to SDK but previously available during creation of precompile headers
#
# Revision 1.60  08/08/2005 00:06:56  yasu
# Defined TARGET_BIN_BASENAME for handling mongoose builds and differentiated with TARGET_NAME
#
# Revision 1.59  08/07/2005 13:15:48  yasu
# Set the library path to vpath
#
# Revision 1.58  07/26/2005 07:29:58  yasu
# Changed the cache location in the pre-compile header.
#
# Revision 1.57  07/26/2005 00:40:56  seiki_masashi
# Replace -include with dummy during creation of pre-compile headers.
#
# Revision 1.56  07/25/2005 05:02:02  yasu
# Changed the trigger for deleting PCH from make clean to make clobber.
#
# Revision 1.55  07/25/2005 01:31:12  yasu
# .pch -> .mch �ւ̕ύX
#
# Revision 1.54  07/21/2005 11:55:21  yasu
# Added EDEPENDS_NEF/BIN.
#
# Revision 1.53  07/14/2005 05:11:49  yasu
# Separated pre-compile headers into C and C++ categories.
#
# Revision 1.52  07/14/2005 02:27:36  yasu
# Added mechanism for suppressing NITRO_STD_PCHDR for each module.
#
# Revision 1.51  07/11/2005 06:12:37  yasu
# Added check for pch file dependency relationships.
#
# Revision 1.50  07/08/2005 13:25:23  yasu
# Supported pre-compile header test.
#
# Revision 1.49  06/22/2005 09:40:07  yasu
# Supported ELIBRARIES.
#
# Revision 1.48  06/02/2005 00:10:41  seiki_masashi
# Revised display of copyright.
#
# Revision 1.47  06/01/2005 07:33:30  seiki_masashi
# Added directories containing files that are the source of calls to the head of the #include search path.
#
# Revision 1.46  12/27/2004 03:51:22  yasu
# Change CC->MWCC , AS->MWAS, LD->MWLD and AR->MWAR
#
# Revision 1.45  11/05/2004 06:42:09  yasu
# Add version info for CW NITRO V1.0
#
# Revision 1.44  10/03/2004 02:04:03  yasu
# Support component file list for compstatic tool
#
# Revision 1.43  09/17/2004 03:35:34  yasu
# Invalidate NITRO_COMPRESS for ARM7 build
#
# Revision 1.42  09/09/2004 11:41:32  yasu
# Support NITRO_DIGEST
#
# Revision 1.41  09/08/2004 00:06:26  yasu
# Set NITRO_STDOUT to '> /dev/null' if -s option defined
#
# Revision 1.40  09/06/2004 05:34:38  yasu
# Add -a option on compstatic
#
# Revision 1.39  09/06/2004 01:08:41  yasu
# Add -c option on compstatic
#
# Revision 1.38  09/04/2004 12:37:52  yasu
# Add OverlayTable for compstatic tool
#
# Revision 1.37	 09/01/2004 07:30:03  yasu
# Support NITRO_COMPRESS
#
# Revision 1.36	 08/10/2004 14:18:48  yasu
# Add dependancies for makelcf
#
# Revision 1.35	 08/10/2004 00:13:46  yasu
# Support CodeWarrior 0.6.1
#
# Revision 1.34	 07/26/2004 08:11:08  yasu
# Rename ADDRESS_ARM7 -> ADDRESS_DTCM
#
# Revision 1.33	 07/26/2004 04:59:02  yasu
# Add ADDRESS_ARM7 on makelcf
#
# Revision 1.32	 07/18/2004 12:20:05  yasu
# Change ["] -> ['] of MAKELCF_DEFS for Mingw
#
# Revision 1.31	 07/18/2004 06:10:46  yasu
# show makelcf command line on making *.autogen.lcf
#
# Revision 1.30	 07/13/2004 12:34:19  yasu
# Check libcxx workaround on invoking linker
#
# Revision 1.29	 07/10/2004 04:55:30  yasu
# support USE_MY_RULE_A, USE_MY_RULE_NEF
#
# Revision 1.28	 07/10/2004 04:20:23  yasu
# Use response file to support overlay group
#
# Revision 1.27	 06/28/2004 10:52:11  yasu
# Delete group setting
#
# Revision 1.26	 05/01/2004 06:10:33  yada
# add build number rule for CW0.5
#
# Revision 1.25	 04/21/2004 11:22:06  yasu
# Support codecheck as workaround for blxeq/ne problem
#
# Revision 1.24	 04/20/2004 03:49:38  yasu
# add LDEPENDS_BIN/NEF for dependancies
#
# Revision 1.23	 04/14/2004 10:14:53  yasu
# delete LIBRARIES_STATIC
#
# Revision 1.22	 04/03/2004 08:43:35  yasu
# with MINGW, support detect CW versions
#
# Revision 1.21	 04/03/2004 08:01:43  yasu
# auto-detect CW verinfo and switch strb parameters
#
# Revision 1.20	 04/01/2004 12:54:38  yasu
# support LIBRARIES_STATIC for makerom
#
# Revision 1.19	 03/23/2004 09:28:57  yasu
# add $(MAKEFILE) for dependancy list of %.nef
#
# Revision 1.18	 03/18/2004 02:29:44  yasu
# change condition to switch rom packing method (makerom or elftobin)
#
# Revision 1.17	 02/23/2004 05:01:38  yasu
# axf->nef
#
# Revision 1.16	 02/17/2004 09:22:48  yasu
# support dependancies with MinGW
#
# Revision 1.15	 02/16/2004 01:24:24  yasu
# support file dependancies
#
# Revision 1.14	 02/05/2004 07:09:01  yasu
# change SDK prefix iris -> nitro
#
# Revision 1.13	 01/15/2004 10:49:31  yasu
# support overlay
#
# Revision 1.12	 12/10/2003 05:12:48  yasu
# Changed so that the xxxx/src.c format can be used when specifying a source file.
# Added Makefile.srcs.
# Revised so that the IS Debugger library is linked if IS_NITRO_DIR has been defined.
#
# Revision 1.11	 12/02/2003 04:53:06  yasu
# Corrected coding of headers.
#
# Revision 1.10	 12/02/2003 03:45:11  yasu
# Made corrections to IS-NITRO-EMULATOR library.
#
# Revision 1.9	12/01/2003 11:30:55  yasu
# Switched ROM creation tool to objcopy -> elftobin.
#
# Revision 1.8	11/28/2003 02:01:12  yasu
# Replaced standard library for CW for NITRO.
#
# Revision 1.7	11/17/2003 01:30:43  yasu
# Separated C++ definitions from C definitions.
#
# Revision 1.6	11/11/2003 03:06:31  yasu
# Previous revisions are invalidated due to the fact that the base.a position is specified using an lcf file.
#
# Revision 1.5	11/11/2003 02:12:16  yasu
# Revised so that base.a is linked before OBJS.
#
# Revision 1.4	11/06/2003 10:38:39  yasu
# Corrected mistaken setting for the library search path.
#
# Revision 1.3	11/05/2003 09:14:17  yasu
# Added library generation tool.
#
# Revision 1.2	11/05/2003 07:12:36  yasu
# Temporarily completed compile dependency rules.
#
# Revision 1.1	10/29/2003 06:47:28  yasu
# Support "make clean/clobber" only
#
# $NoKeywords: $
#----------------------------------------------------------------------------

AINCLUDE_DIRS		= $(LINCLUDES)	   $(EINCLUDES)     $(GINCLUDES)
ALIBRARY_DIRS		= $(LLIBRARY_DIRS) $(ELIBRARY_DIRS) $(GLIBRARY_DIRS)
ALIBRARIES		= $(LLIBRARIES)	   $(ELIBRARIES)    $(GLIBRARIES)

#
# (make version 3.80)
# 
# VPATH does not support the path name included white space like "C:/Program Files"
#	Please, be careful !!!
#
ifneq ($(OS),Windows_NT)
ALIBVPATH		 = $(subst : ,,$(patsubst %:,/:,$(subst :,: ,$(ALIBRARY_DIRS))))
else
ifdef	MSYSTEM
NITRO_LIBVPATH_		 = $(subst \:,\ ,$(subst $(SPACE),:,$(ALIBRARY_DIRS)))
else
CYGDRV			?= $(shell cygpath -u c:)/..
ALIBVPATH		 = $(subst : ,,$(patsubst %:,$(CYGDRV)/%:,$(subst :,: ,$(ALIBRARY_DIRS))))
endif
endif

vpath	%.a		$(ALIBVPATH)
vpath	%.o		$(ALIBVPATH)

INCLUDE_DIRS		= $(subst \ -I,\ ,$(addprefix -I,$(call PATH_UNIX2DOS,$(AINCLUDE_DIRS))))
LIBRARY_DIRS		= $(subst \ -L,\ ,$(addprefix -L,$(call PATH_UNIX2DOS,$(ALIBRARY_DIRS))))
LIBRARIES		= $(subst \ -l,\ ,$(addprefix -l,$(ALIBRARIES)))

INCLUDES		= -gccinc -I. $(INCLUDE_DIRS)

ifdef	MSYSTEM		# for MinGW32
QUOTE			='
else
QUOTE			="
endif

#----------------------------------------------------------------------------

.PRECIOUS:	$(OBJDIR)/%.o $(BINDIR)/%.nef $(LCFILE) $(PCHDR_H) $(PCHDR_PCH)

#
# Generate dependency files
#
DEPEND_RULE	= $(QUOTE)/ARM_EABI_Support/d;s/\\x0D//g;s/\\\\\\([^ ]\\)/\\/\\1/g;$(QUOTE)

define MOVE_SRC_DEPEND
	$(SED) -e $(DEPEND_RULE) -e $(PATH_DEPEND_RULE) $(*F).d > $(DEPENDDIR)/$(*F).d && $(RM) $(*F).d
endef


ifdef	NITRO_STD_PCHDR
ifndef	NITRO_NO_STD_PCHDR

# .mch
PCHDR_SRC	= $(ROOT)/include/nitro.h
PCHDR_CACHE	= $(ROOT)/cache/include

PCHDR_C_DEFS	= $(sort $(filter -DSDK_%,$(CCFLAGS)))
PCHDR_C_NODEF	= $(filter-out -D%,$(CCFLAGS))
PCHDR_C_NAME	= $(subst $(SPACE),,$(subst -DSDK_,.,$(subst =,-,$(PCHDR_C_DEFS))))
PCHDR_C_BASE	= $(PCHDR_CACHE)/$(notdir $(PCHDR_SRC))$(PCHDR_C_NAME).C
PCHDR_C_MCH	= $(PCHDR_C_BASE).mch
PCHDR_C_DEPEND	= $(PCHDR_C_BASE).depend
PCHDR_CCFLAGS	= -include $(call PATH_UNIX2DOS,$(PCHDR_C_MCH))
CCFLAGS_NOLDEF	= $(PCHDR_C_DEFS) $(PCHDR_C_NODEF) 

PCHDR_CXX_DEFS	= $(sort $(filter -DSDK_%,$(CXXFLAGS)))
PCHDR_CXX_NODEF	= $(filter-out -D%,$(CXXFLAGS))
PCHDR_CXX_NAME	= $(subst $(SPACE),,$(subst -DSDK_,.,$(subst =,-,$(PCHDR_CXX_DEFS))))
PCHDR_CXX_BASE	= $(PCHDR_CACHE)/$(notdir $(PCHDR_SRC))$(PCHDR_CXX_NAME).CXX
PCHDR_CXX_MCH	= $(PCHDR_CXX_BASE).mch
PCHDR_CXX_DEPEND= $(PCHDR_CXX_BASE).depend
PCHDR_CXXFLAGS	= -include $(call PATH_UNIX2DOS,$(PCHDR_CXX_MCH))
CXXFLAGS_NOLDEF	= $(PCHDR_CXX_DEFS) $(PCHDR_CXX_NODEF) 

GDIRT_CLOBBER	+= $(PCHDR_CACHE)/$(notdir $(PCHDR_SRC)).*

ifeq	($(MAKECMDGOALS),do-build)

## $(call BUILD_PCHDR,$(BASE),$(CCFLAGS))
define BUILD_PCHDR
	@$(CP) $(PCHDR_SRC) $(1).h
	($(CD) $(dir $(1).mch) && \
	 $(CC) $(subst -include ,-DSDK_PCHDR_DUMMY_,$(2)) $(INCLUDES) -MD -precompile $(notdir $(1).mch) $(notdir $(1).h))
	@($(ECHO) $(1).mch: $(PCHDR_SRC) \\       >  $(1).depend && \
	  $(SED) -e '1d' -e $(DEPEND_RULE) -e $(PATH_DEPEND_RULE) $(1).d >> $(1).depend && $(RM) $(1).d $(1).h)
endef

# precomipled header for C
ifneq	($(strip $(OBJS_CC)),)
-include $(PCHDR_C_DEPEND)

$(PCHDR_C_DEPEND): $(PCHDR_C_MCH)

$(PCHDR_C_MCH):
	$(call BUILD_PCHDR,$(PCHDR_C_BASE),$(CCFLAGS_NOLDEF))
endif

# precomipled header for C++
ifneq	($(strip $(OBJS_CXX)),)
-include $(PCHDR_CXX_DEPEND)

$(PCHDR_CXX_DEPEND): $(PCHDR_CXX_MCH)

$(PCHDR_CXX_MCH):
	$(call BUILD_PCHDR,$(PCHDR_CXX_BASE),$(CXXFLAGS_NOLDEF))
endif

endif	# do-build
endif	# NITRO_NO_STD_PCHDR
endif	# NITRO_STD_PCHDR

# .c
$(OBJS_CC):%.o:
	$(CC) $(CCFLAGS) $(INCLUDES) $(PCHDR_CCFLAGS) -c $< -MD -o $@
	@$(MOVE_SRC_DEPEND)

# .cpp
$(OBJS_CXX):%.o:
	$(CC) $(CXXFLAGS) $(INCLUDES) $(PCHDR_CXXFLAGS) -c $< -MD -o $@
	@$(MOVE_SRC_DEPEND)

# .s
$(OBJS_AS):%.o:
	$(AS) $(ASFLAGS) $(INCLUDES) -c $< -MD -o $@
	@$(MOVE_SRC_DEPEND)

# .lcf .par
ifdef	LCFILE_SPEC

ADDRESS_DTCM	?= $(DEFAULT_ADDRESS_DTCM)

# exporting
GLIBRARIES_LINK	= $(filter-out $(LIBRARIES_UNLINK),$(ELIBRARIES) $(GLIBRARIES))
LLIBRARIES_LINK	= $(filter-out $(LIBRARIES_UNLINK),$(LLIBRARIES))

MAKELCF_DEFS	= -DNITROSDK_ROOT='$(ROOT)'			\
		  -DTARGET_NAME='$(TARGET_NAME)'		\
		  -DOBJS_STATIC='$(OBJS_STATIC)'		\
		  -DOBJS_AUTOLOAD='$(OBJS_AUTOLOAD)'		\
		  -DOBJS_OVERLAY='$(OBJS_OVERLAY)'		\
		  -DGLIBS='$(GLIBRARIES_LINK)'			\
		  -DLLIBS='$(LLIBRARIES_LINK)'			\
		  -DCW_LIBS='$(CW_LIBS)'			\
		  -DOBJDIR='$(OBJDIR)'				\
		  -DLIBDIR='$(LIBDIR)'				\
		  -DBINDIR='$(BINDIR)'				\
		  -DPROC='$(CODEGEN_PROC)'			\
		  -DPLATFORM='$(NITRO_PLATFORM)'		\
		  -DCODEGEN='$(CODEGEN_ARCH)'			\
		  -DBUILD='$(NITRO_BUILD_DIR)'			\
		  -DADDRESS_DTCM='$(ADDRESS_DTCM)'

MAKELCF_OPTS	= $(MAKELCF_FLAGS) $(MAKELCF_DEFS)


$(LCFILE_AUTOGEN): $(LCFILE_SPEC) $(LCFILE_TEMPLATE) $(MAKEFILE) $(NITRO_BUILDTOOLSDIR)/commondefs
	$(MAKELCF) $(MAKELCF_OPTS)  $< $(LCFILE_TEMPLATE) $@

$(LDRES_AUTOGEN):  $(LCFILE_SPEC) $(LDRES_TEMPLATE)  $(MAKEFILE) $(NITRO_BUILDTOOLSDIR)/commondefs
	@$(MAKELCF) $(MAKELCF_OPTS)  $< $(LDRES_TEMPLATE)  $@

endif

# compress/digest
ifeq	($(CODEGEN_PROC),ARM9)
ifdef	NITRO_COMPRESS
COMPSTATIC_CMDS		= -9 -c
#COMPSTATIC_CMDS	= -7 -c
endif
ifdef	NITRO_DIGEST
COMPSTATIC_CMDS		+= -a
endif
endif

ifdef	COMPSTATIC_CMDS
COMPSUFFIX		?= $(DEFAULT_COMPSUFFIX)
ifneq	($(COMPSUFFIX),)
COMPSTATIC_FLAGS	+= -e$(COMPSUFFIX)
endif
endif


# libcxx workaround
ifdef	SDK_CW_WA_LIBCXX
CW_LIBCXX	= $(CW_LIBCXX_DIR)/$(CW_LIBCXX_ARM)
$(CW_LIBCXX):	$(ROOT)/build/buildsetup/libcxx/Makefile
	$(MAKE) -f $<
endif


# .nef
ifndef	USE_MY_RULE_NEF
ifneq	($(NITRO_MAKEROM),)
$(BINDIR)/$(TARGET_BIN_BASENAME).nef: $(OBJS) $(LCFILE) $(MAKEFILE) $(LDEPENDS_NEF) $(EDEPENDS_NEF) $(ALIBRARIES) $(LDRES_FILE) $(CW_LIBCXX)
	$(LD) $(LDFLAGS) $(LIBRARY_DIRS) @$(LDRES_FILE) $(LCFILE) -o $@
ifdef	COMPSTATIC_CMDS
	$(COMPSTATIC) $(COMPSTATIC_CMDS) $(COMPSTATIC_FLAGS) -f $(BINDIR)/component.files $(NITRO_STDOUT)
endif
else
$(BINDIR)/$(TARGET_BIN_BASENAME).nef: $(OBJS) $(LCFILE) $(MAKEFILE) $(LDEPENDS_NEF) $(EDEPENDS_NEF) $(ALIBRARIES) $(CW_LIBCXX)
	$(LD) $(LDFLAGS) $(LIBRARY_DIRS) $(OBJS) $(LIBRARIES) $(LCFILE) -o $@
endif
endif

# .a
ifndef	USE_MY_RULE_A
$(LIBDIR)/$(TARGET_LIB): $(OBJS)
	$(AR) $(ARFLAGS) $(OBJS) -o $@
endif


#----------------------------------------------------------------------------
# Get SDK_CW_BUILD_NUMBER
#
# IRIS	V0.2 Hotfix 1
#	SDK_CW_BUILD_NUMBER_CC=0038
#	SDK_CW_BUILD_NUMBER_LD=0050
#
# NITRO V0.3
#	SDK_CW_BUILD_NUMBER_CC=0038
#	SDK_CW_BUILD_NUMBER_LD=0056
#
# NITRO V0.4.1
#	SDK_CW_BUILD_NUMBER_CC=0043
#	SDK_CW_BUILD_NUMBER_LD=0057
#
# NITRO V0.5
#	SDK_CW_BUILD_NUMBER_CC=0046
#	SDK_CW_BUILD_NUMBER_LD=0058
#
# NITRO V0.5 + HotFix1
#	SDK_CW_BUILD_NUMBER_CC=0049
#	SDK_CW_BUILD_NUMBER_LD=0061
#
# NITRO V0.5
#	SDK_CW_BUILD_NUMBER_CC=0050
#	SDK_CW_BUILD_NUMBER_LD=0062
#
# NITRO V0.6.1
#	SDK_CW_BUILD_NUMBER_CC=0050
#	SDK_CW_BUILD_NUMBER_LD=0063
#
# NITRO V0.7
#	SDK_CW_BUILD_NUMBER_CC=0050
#	SDK_CW_BUILD_NUMBER_LD=0063
#
# NITRO V1.0
#	SDK_CW_BUILD_NUMBER_CC=0056
#	SDK_CW_BUILD_NUMBER_LD=0067
#
# NITRO V1.0.2
#	SDK_CW_BUILD_NUMBER_CC=0068
#	SDK_CW_BUILD_NUMBER_LD=0068
#
# NITRO V1.0
#	SDK_CW_BUILD_NUMBER_CC=0073
#	SDK_CW_BUILD_NUMBER_LD=0072
#
# NITRO V1.2SP1
#	SDK_CW_BUILD_NUMBER_CC=0074
#	SDK_CW_BUILD_NUMBER_LD=0072
#
# NITRO V1.2SP2+patch
#	SDK_CW_BUILD_NUMBER_CC=0080
#	SDK_CW_BUILD_NUMBER_LD=0074
#
# NITRO V2.0beta
#	SDK_CW_BUILD_NUMBER_CC=0107
#	SDK_CW_BUILD_NUMBER_LD=0077
#
ifneq	($(findstring $(MAKECMDGOALS),do-build make-verinfo clean-verinfo),)

.PHONY: make-verinfo
make-verinfo: $(SDK_VERINFO_CW_CC) $(SDK_VERINFO_CW_LD)

$(SDK_VERINFO_CW_CC):
	@$(MWCC) -version | \
	 $(SED) -e $(QUOTE)/^Version /!d;s/^Version \([0-9.]*\).*(build \([0-9]*\)).*/SDK_CW_BUILD_VERSION_CC=\1\nSDK_CW_BUILD_NUMBER_CC=\2/i;$(QUOTE) >$(SDK_VERINFO_CW_CC)

$(SDK_VERINFO_CW_LD):
	@$(MWLD) -version | \
	 $(SED) -e $(QUOTE)/^Version /!d;s/^Version \([0-9.]*\).*(build \([0-9]*\)).*/SDK_CW_BUILD_VERSION_LD=\1\nSDK_CW_BUILD_NUMBER_LD=\2/i;$(QUOTE) >$(SDK_VERINFO_CW_LD)

endif

.PHONY: clean-verinfo
clean-verinfo:
	@$(RM) $(NITRO_BUILDTOOLSDIR)/verinfo.*


#----------------------------------------------------------------------------
# Check codegen bug around blxeq/blxne
#

.PHONY: codecheck

codecheck:
	@$(ECHO) --- checking codegen ---
	@$(NITRO_TOOLSDIR)/bin/blxchecker

#----- End of modulerules.cctype.CW -----
