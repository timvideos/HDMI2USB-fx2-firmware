# The Cypress FX2 is a programmable USB interface chip found on the Atlys and
# Opsis board (and many other devices out there).
#
# The IC is an 8051 core with a hardware support for the USB protocol. Firmware
# for the FX2 is developed using the fx2lib library and compiled with the SDCC
# compiler.
#
# The firmware can be loaded via USB using a number of tools such as fxload or
# fpgalink. Loading new firmware will cause the FX2 to disconnect and then
# reconnect to the USB bus, often causing it to change USB IDs and device
# files.
#
# Being a programmable device, the FX2 can emulate many other USB devices. We
# use it to emulate a USB UVC Webcam and a USB CDC-ACM serial port.
#

MODESWITCH_CMD = hdmi2usb-mode-switch
FX2LIBDIR = ./third_party/fx2lib

TARGETS += fx2

help-fx2:
	@echo " make load-fx2"
	@echo " make view"

gateware-generate-fx2:
	@true

gateware-build-fx2:
	cp gateware/streamer/vhdl/header.hex $(MSCDIR)/build/header.hex

# Firmware for the Cypress FX2
firmware-fx2: hdmi2usb/hdmi2usb.hex
	@true

embed-fx2: firmware/lm32/fx2_fw_hdmi2usb.c
	@true

load-fx2: hdmi2usb/hdmi2usb.hex
	$(MODESWITCH_CMD) --load-fx2-firmware hdmi2usb/hdmi2usb.hex

flash-fx2:
	@true

clean-fx2:
	$(MAKE) -C hdmi2usb clean

hdmi2usb/hdmi2usb.hex:
	$(MAKE) -C hdmi2usb

firmware/lm32/fx2_fw_hdmi2usb.c: microload/generate_2nd_stage.py hdmi2usb/hdmi2usb.hex
	microload/generate_2nd_stage.py hdmi2usb/hdmi2usb.hex > firmware/lm32/fx2_fw_hdmi2usb.c

# Audio firmware for the Cypress FX2
firmware-audio-fx2: audio/audio.hex
	@true

load-audio-fx2: audio/audio.hex
	$(MODESWITCH_CMD) --load-fx2-firmware audio/audio.hex

clean-audio-fx2:
	$(MAKE) -C audio clean

audio/audio.hex:
	$(MAKE) -C audio

# Default USB VID/PID for the FX2
flash-unconfigured:
	$(MAKE) -C eeprom-unconfigured flash

firmware-unconfigured:
	$(MAKE) -C eeprom-unconfigured hdmi2usb_unconfigured.iic

clean-unconfigured:
	$(MAKE) -C eeprom-unconfigured clean

# Microload
microload: fifo-microload fifo-i2c

fifo-microload:
	$(MAKE) -C microload fifo

i2c-microload:
	$(MAKE) -C microload i2c

clean-microload:
	$(MAKE) -C microload clean

# Utility functions
view:
	./scripts/view-hdmi2usb.sh

docs: export PROJECT_NUMBER:=$(shell git describe --always --dirty --long)

docs:
	doxygen docs/docs.conf

clean-docs:
	rm -fr docs/html docs/latex

# We depend on the .git file inside the directory as git creates an empty dir
# for us.
$(FX2LIBDIR)/.git: .gitmodules
	git submodule sync --recursive -- $$(dirname $@)
	git submodule update --recursive --init $$(dirname $@)
	touch $@ -r .gitmodules

.PHONY: docs clean-docs help-fx2 gateware-fx2 firmware-fx2 load-fx2 clean-fx2 view
