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

all: cn_bootloader_bin.h

clean:
	@rm -f cn_bootloader.bin cn_bootloader_bin.h
	@echo "all cleaned up !"

%.bin: %.s
	armips $<

cn_bootloader_bin.h: cn_bootloader.bin
	@xxd -i $< > $@
	@sed -i '' 's/unsigned/const static unsigned/g' $@
