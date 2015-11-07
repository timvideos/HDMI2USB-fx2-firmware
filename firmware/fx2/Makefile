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
TARGET := hdmi2usb
FX2LIBDIR := ../../third_party/fx2lib
LIBS := $(FX2LIBDIR)/lib/fx2.lib
INCS := -I sdcc -I$(FX2LIBDIR)/include -I.

# Settings specific for the TimVideo hdmi2usb firmware
BOARD ?= atlys
FLAGS +=-DDEBUG -DBOARD_${BOARD}
USE_16K := 1

# Must only be hex numbers
FIRMWARE_VERSION := $(shell date +%Y%m%d)

CC_SRCS := app.c cdc.c debug.c softserial.c uvc.c firmware.c read-serialno.c to-uart.c descriptors_hdmi2usb_alt.c
CC_OBJS := $(CC_SRCS:%.c=%.rel)

CC := sdcc
CFLAGS += -DDATE=0x$(FIRMWARE_VERSION) -mmcs51 --std-c99 -DSDCC $(FLAGS)

ifeq ($(USE_16K),1)
	CFLAGS += --code-size 0x3e00 --xram-size 0x0200 -Wl"-b DSCR_AREA=0x2e00" -Wl"-b INT2JT=0x3f00"
else
	CFLAGS += --code-size 0x1e00 --xram-size 0x0200 -Wl"-b DSCR_AREA=0x1e00" -Wl"-b INT2JT=0x1f00"
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


# Use make V=1 for a verbose build.
ifndef V
	Q_CC=@echo      '      CC ' $@;
	Q_LINK=@echo    '    LINK ' $@;
	Q_RM=@echo      '   CLEAN ';
	Q_OBJCOPY=@echo ' OBJCOPY ' $@;
	Q_GEN=@echo     '     GEN ' $@;
endif

.PHONY: all clean check check_int2jt

all: $(TARGET).hex

$(CC_SRCS): $(FX2LIBDIR)/lib/fx2.lib

$(FX2LIBDIR)/lib/fx2.lib: $(FX2LIBDIR)/.git
	cd $(dir $@) && make -j1

# We depend on a file inside the directory as git creates an
# empty dir for us.
#
# Note that although we have the variable FX2LIBDIR, the submodule
# magic will always check it out in fx2lib/
$(FX2LIBDIR)/.git:
	git submodule init $@
	git submodule update --recursive $@

$(TARGET).hex: $(CC_OBJS)
	$(Q_LINK)$(CC) $(CFLAGS) -o $@ $+ $(LIBS)

%.rel: %.c
	$(Q_CC)$(CC) $(CFLAGS) -c --disable-warning 85 $(INCS) $?

date.h: Makefile
	$(Q_GEN)
	@echo "// This is an auto-generated file. Please modify the Makefile instead " > $@
	@echo "#ifndef _DATE_H_" >> $@
	@echo "#define _DATE_H_" >> $@
	@echo "#define DATE 0x$(FIRMWARE_VERSION)" >> $@
	@echo "#define VID 0x$(VID)" >> $@
	@echo "#define PID 0x$(PID)" >> $@
	@echo "#define DID 0x$(DID)" >> $@
	@echo "#endif // _DATE_H_" >> $@

check-descriptors:
	gcc -I../../third_party/fx2lib/include descriptors_hdmi2usb_alt.c

descriptors_hdmi2usb_alt.c: date.h

check_int2jt: $(TARGET).hex
	@export REQUESTED=$(shell grep "INT2JT=" $(TARGET).map | sed -e's/INT2JT=//'); \
	export ACTUAL=$(shell grep "C:.*INT2JT" $(TARGET).map | sed -e's/C: *0*\([^ ]*\)  _INT2JT.*/0x\1/' | tr A-Z a-z ); \
	if [ "$$REQUESTED" != "$$ACTUAL" ]; then \
		echo "INT2JT at $$ACTUAL but requested $$REQUESTED"; \
		exit 1; \
	fi

check: check_int2jt

clean:
	$(Q_RM)$(RM) *.iic *.asm *.lnk *.lst *.map *.mem *.rel *.rst *.sym \
		*.lk progOffsets.h firmware.hex date.inc date.h
	cd $(FX2LIBDIR) && make clean

distclean: clean
	$(RM) -r $(FX2LIBDIR)
