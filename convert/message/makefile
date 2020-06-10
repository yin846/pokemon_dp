##########################################################
#	メッセージデータコンバート
##########################################################
include $(NITROSDK_ROOT)/build/buildtools/commondefs.wine

RESOURCE_LIST	= resource.list
DEPEND_LIST	= depend.list
TARGET_LIST	= target.list
DATA_DIR	= data
HEADER_DIR	= ../../include/msgdata
VERSION_FILE	= ../../version


include $(NITROSDK_ROOT)/build/buildtools/commondefs
-include $(TARGET_LIST)
-include $(VERSION_FILE)


##########################################################
#	動作部分
##########################################################
.SUFFIXES: .dat .gmm

build: msgenc.exe
	@mkdir -p $(DATA_DIR) $(HEADER_DIR)
	$(MAKE)	depend
	$(MAKE)	datfiles


#=========================================================
#	dat を作成
#=========================================================
datfiles: $(TARGETS)
#	perl msgconv.pl $< $(PM_LANG)


#=========================================================
#	clean
#=========================================================
clean:
	-rm $(DATA_DIR)/*.dat
	-rm $(HEADER_DIR)/*.h
	-rm $(DEPEND_LIST) $(TARGET_LIST)
	-rm msgenc.exe

#=========================================================
#	depend
#=========================================================
depend:
	$(shell find src|grep gmm$$ > $(RESOURCE_LIST)) 
	perl  makedepend.pl $(RESOURCE_LIST) $(DATA_DIR) $(DEPEND_LIST) $(TARGET_LIST) $(PM_LANG)
	rm $(RESOURCE_LIST)


msgenc.exe: msgenc.c
	$(CC_X86) -Wno-unused-result -O2 -o $@ $<


-include $(DEPEND_LIST)

