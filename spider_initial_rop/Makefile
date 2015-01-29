ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifeq ($(strip $(CTRULIB)),)
$(error "Please set CTRULIB in your environment. export DEVKITARM=<path to>ctrulib/libctru")
endif

ifeq ($(filter $(DEVKITARM)/bin,$(PATH)),)
export PATH:=$(DEVKITARM)/bin:$(PATH)
endif

SCRIPTS = "../scripts"

all: spider_initial_rop_bin.h

clean:
	@rm -f spider_initial_rop.bin spider_initial_rop_bin.h
	@cd 2050 && make clean
	@cd 3074 && make clean
	@cd 4096 && make clean
	@cd SKATER_10 && make clean
	@echo "all cleaned up !"

spider_initial_rop.bin:
	@cd $(SPIDERVERSION) && make
	@cp $(SPIDERVERSION)/$@ ./
	@python $(SCRIPTS)/obfuscator5000.py $@

spider_initial_rop_bin.h: spider_initial_rop.bin
	@xxd -i $< > $@
	@sed -i '' 's/unsigned/const static unsigned/g' $@
