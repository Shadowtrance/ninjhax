ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

ifeq ($(strip $(CTRULIB)),)
$(error "Please set CTRULIB in your environment. export DEVKITARM=<path to>ctrulib/libctru")
endif

ifeq ($(filter $(DEVKITARM)/bin,$(PATH)),)
export PATH:=$(DEVKITARM)/bin:$(PATH)
endif

OBJCOPY = arm-none-eabi-objcopy
CC = arm-none-eabi-gcc
ASFLAGS=-nostartfiles -nostdlib
SCRIPTS = "../scripts"
NAME = spiderninja

.PHONY: clean

all: $(NAME).dat

clean:
	@rm -f $(NAME).dat
	@echo "all cleaned up !"

%.ro: %.S
	$(CC) -c -o $@ $< $(ASFLAGS)

%.dat: %.ro
	$(OBJCOPY) -O binary $^ $@
