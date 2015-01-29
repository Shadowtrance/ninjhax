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

all: cn_save_initial_loader_bin.h

clean:
	@rm -f cn_save_initial_loader.bin cn_save_initial_loader_bin.h
	@cd WEST && make clean
	@cd JPN && make clean
	@echo "all cleaned up !"

cn_save_initial_loader.bin:
	@cd $(CNVERSION) && make
	@cp $(CNVERSION)/$@ ./
	@python $(SCRIPTS)/obfuscator5000.py $@

cn_save_initial_loader_bin.h: cn_save_initial_loader.bin
	@xxd -i $< > $@
	@sed -i '' 's/unsigned/const static unsigned/g' $@
