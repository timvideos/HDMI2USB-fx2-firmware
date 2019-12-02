### The Cypress FX2 is a programmable USB interface chip found on the Atlys and
### Opsis board (and many other devices out there).
###
### The IC is an 8051 core with a hardware support for the USB protocol. Firmware
### for the FX2 is developed using libfx2 library and compiled with the SDCC.
###
### The firmware can be loaded via USB using a number of tools such as fxload or
### fpgalink. Loading new firmware will cause the FX2 to disconnect and then
### reconnect to the USB bus, often causing it to change USB IDs and device
### files.
###
### Being a programmable device, the FX2 can emulate many other USB devices. We
### use it to emulate a USB UVC Webcam, USB Audio device and a USB CDC-ACM serial 
### port.

MODESWITCH_CMD = hdmi2usb-mode-switch
LIBFX2DIR = ./third_party/libfx2

all: conda firmware-fx2

###
### conda - self contained environment 
### ----------------------------------------------------------------------------

export PATH := $(shell pwd)/conda/bin:$(PATH)

Miniconda3-latest-Linux-x86_64.sh:
	@echo
	@echo " Download conda..."
	@echo "-----------------------------"
	wget -c https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh
	@chmod a+x Miniconda3-latest-Linux-x86_64.sh

conda: Miniconda3-latest-Linux-x86_64.sh ### 
	@echo
	@echo " Setting up conda"
	@echo "-----------------------------"
	@./Miniconda3-latest-Linux-x86_64.sh -p $@ -b
	@conda config --set always_yes yes --set changeps1 no
	@conda update -q conda
	@conda config --add channels timvideos
	@echo
	@echo " Install sdcc (compiler)"
	@echo "-----------------------------"
	@conda install sdcc
	@echo
	@echo " Install HDMI2USB-mode-switch"
	@echo "-----------------------------"
	@pip install --upgrade git+https://github.com/timvideos/HDMI2USB-mode-switch.git

clean-conda: ###
	rm -rf conda
	rm Miniconda3-*.sh || true

###
### FX2 HDMI2USB firmware 
### ----------------------------------------------------------------------------

firmware-fx2: libfx2 hdmi2usb/hdmi2usb.ihex ###
	@true

load-fx2: firmware-fx2 ###
	$(MODESWITCH_CMD) --load-fx2-firmware hdmi2usb/hdmi2usb.hex

clean-fx2: ###
	$(MAKE) -C hdmi2usb clean

hdmi2usb/hdmi2usb.ihex:
	$(MAKE) -C hdmi2usb

###
### libfx2 - FX2 chip support library
### ----------------------------------------------------------------------------

libfx2: $(LIBFX2DIR)/firmware/library/lib ###
	@true

clean-libfx2: ###
	$(MAKE) -C $(LIBFX2DIR)/firmware/library clean

$(LIBFX2DIR)/firmware/library/lib: $(LIBFX2DIR)/.git
	$(MAKE) -C $(LIBFX2DIR)/firmware/library

# updates git submodule
$(LIBFX2DIR)/.git: .gitmodules
	git submodule sync --recursive -- $$(dirname $@)
	git submodule update --recursive --init $$(dirname $@)
	touch $@ -r .gitmodules

###
### Microload bootloader
### ----------------------------------------------------------------------------

microload: fifo-microload i2c-microload ###

microload-fifo: ###
	$(MAKE) -C microload fifo

microload-i2c: ###
	$(MAKE) -C microload i2c

microload-clean: ###
	$(MAKE) -C microload clean

###
### Default USB VID/PID for the FX2
### ----------------------------------------------------------------------------

flash-unconfigured:
	$(MAKE) -C eeprom-unconfigured flash

firmware-unconfigured:
	$(MAKE) -C eeprom-unconfigured hdmi2usb_unconfigured.iic

clean-unconfigured:
	$(MAKE) -C eeprom-unconfigured clean

###
### Documentation 
### ----------------------------------------------------------------------------

docs: export PROJECT_NUMBER:=$(shell git describe --always --dirty --long)

docs: ###
	doxygen docs/docs.conf

help: ### Show this help
	@sed -ne '/@sed/!s/###//p' $(MAKEFILE_LIST) \
		| sed -e 's/^\(\S\+\):.*/   \1/'

.PHONY: docs clean-docs firmware-fx2 load-fx2 clean-fx2 libfx2 clean-libfx2
.DEFAULT_GOAL := all
