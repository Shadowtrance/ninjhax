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
NAME = spiderninja

.PHONY: all

all: $(NAME).html $(NAME).bin

clean:
	@cd sn_initial && make clean
	@rm -f $(NAME).bin $(NAME).dat $(NAME).html
	#@cd 2050 && make clean
	#@cd 3074 && make clean
	@cd 4096 && make clean
	@echo "all cleaned up !"

-include $(DFILES)

$(NAME).dat:
	@cd $(SPIDERVERSION) && make
	@cp $(SPIDERVERSION)/$@ ./

$(NAME).html: $(NAME).dat
	@python $(SCRIPTS)/browserify.py $< $@

$(NAME).bin: sn_initial/sn_initial.bin
	@cp sn_initial/sn_initial.bin $@ 

sn_initial/sn_initial.bin:
	$(MAKE) -C sn_initial
