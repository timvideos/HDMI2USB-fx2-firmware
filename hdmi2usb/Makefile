#
# Copyright (C) 2009-2012 Chris McClelland
# Copyright 2015 Joel Stanley <joel@jms.id.au>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# To build the firmware you will need:
#   SDCC from http://sdcc.sourceforge.net
#   fx2lib from http://fx2lib.wiki.sourceforge.net
#
# To build a firmware suitable for loading into RAM:
#   make
#
# To build a firmware suitable for loading from EEPROM:
#   make FLAGS="-DEEPROM"
#
#
# This Makefile assumes a recent version of SDCC. If you're still using an older
# version with the asx8051 assembler, build like this:
#   make AS8051=asx8051
#
TARGET := hdmi2usb
FX2LIBDIR := ../third_party/fx2lib
LIBFX2DIR := ../third_party/libfx2/firmware/library
LIBFX2_MODEL := medium
COMMON_DIR := ../common
USE_16K := 1

CC_SRCS := app.c cdc.c debug.c softserial.c uvc.c firmware.c read-serialno.c to-uart.c
AS_SRCS := descriptors_hdmi2usb.a51

include $(COMMON_DIR)/common.mk

ifeq ($(USE_16K),1)
	CFLAGS += --code-size 0x3e00 -Wl"-b DSCR_AREA=0x2e00" -Wl"-b INT2JT=0x3d00"
else
	CFLAGS += --code-size 0x1e00 -Wl"-b DSCR_AREA=0x1e00" -Wl"-b INT2JT=0x1f00"
endif

ifeq (${BOARD},atlys)
VID := 1D50
PID := 60B7
DID := 0002
else
ifeq (${BOARD},opsis)
VID := 2A19
PID := 5442
DID := 0002
else
$(error "Unknown board type '$(BOARD)'")
endif
endif

.PHONY: all clean check check_int2jt

progOffsets.h: prog.rel
	$(Q_GEN)cat prog.lst | ./lstParse.py > progOffsets.h

date.inc:
	$(Q_GEN)
	@echo "; This is an auto-generated file. Please modify the Makefile instead " > $@
	@echo DATE=0x$(FIRMWARE_VERSION) > $@
	@echo VID=0x$(VID) >> $@
	@echo PID=0x$(PID) >> $@
	@echo DID=0x$(DID) >> $@

%.rel: %.c
	$(Q_CC)$(CC) $(CFLAGS) -c --disable-warning 85 $(INCS) $<

descriptors_hdmi2usb.a51: date.inc

livePatch.c: progOffsets.h
