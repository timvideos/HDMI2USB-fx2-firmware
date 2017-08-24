#
# Copyright (C) 2009-2012 Chris McClelland
# Copyright 2015 Joel Stanley <joel@jms.id.au>
# Copyright 2017 Kyle Robbertze <krobbertze@gmail.com>
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
#   Git from https://git-scm.com/
#
# To build a firmware suitable for loading into RAM:
#   make
#
# To build a firmware suitable for loading from EEPROM:
#   make FLAGS="-DEEPROM"
#   
# To load a firmware:
# 	make load
# 	You will need HDMI2USB-mode-switch from
# 	https://github.com/timvideos/HDMI2USB-mode-switch
#
# Common rules
MAKEFILE_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
export PATH := $(MAKEFILE_PATH)../conda/bin:$(PATH)

LIBS ?= $(FX2LIBDIR)/lib/fx2.lib
INCS += -I sdcc -I$(FX2LIBDIR)/include -I. -I$(COMMON_DIR)/boards

# Settings specific for the TimVideo hdmi2usb firmware
BOARD ?= opsis
FLAGS +=-DDEBUG -DBOARD_$(BOARD)

# Must only be hex numbers
FIRMWARE_VERSION := $(shell date +%Y%m%d)

CC_OBJS := $(CC_SRCS:%.c=%.rel)
AS_OBJS := $(AS_SRCS:%.a51=%.rel)

CC := sdcc
AS8051 := sdas8051
AS := $(AS8051)

CFLAGS += -DDATE=0x$(FIRMWARE_VERSION) -mmcs51 $(FLAGS)
CFLAGS += --std-c99 -DSDCC -Wa"-p" --xram-size 0x0200 

# Use make V=1 for a verbose build.
ifndef V
    Q_CC=@echo      '      CC ' $@;
    Q_AS=@echo      '      AS ' $@;
    Q_LINK=@echo    '    LINK ' $@;
    Q_RM=@echo      '   CLEAN ';
    Q_OBJCOPY=@echo ' OBJCOPY ' $@;
    Q_GEN=@echo     '     GEN ' $@;
endif

.PHONY: all clean distclean check check-descriptors check_int2jt load

all: $(TARGET).hex

check_int2jt: $(TARGET).hex
	@export REQUESTED=$(shell grep "INT2JT=" $(TARGET).map | sed -e's/INT2JT=//'); \
	export ACTUAL=$(shell grep "C:.*INT2JT" $(TARGET).map | sed -e's/C: *0*\([^ ]*\)  _INT2JT.*/0x\1/' | tr A-Z a-z ); \
	if [ "$$REQUESTED" != "$$ACTUAL" ]; then \
		echo "INT2JT at $$ACTUAL but requested $$REQUESTED"; \
		exit 1; \
	fi

check: check_int2jt

clean:
	$(Q_RM)$(RM) *.adb *.asm *.cdb *.iic *.lk *.lnk *.lst *.omf *.map \
		*.mem *.rel *.rst *.sym descriptors_strings.* version_data.* a.out \
		date.h date.inc progOffsets.h ${TARGET}.hex $(FIRMWARE)/*.asm \
		$(FIRMWARE)/*.lst $(FIRMWARE)/*.rel $(FIRMWARE)/*.rst $(FIRMWARE)/*.sym
	cd $(FX2LIBDIR) && make clean

distclean: clean
	$(RM) -r $(FX2LIBDIR)

load: $(TARGET).hex
	hdmi2usb-mode-switch --load-fx2-firmware $(TARGET).hex

$(CC_SRCS) $(AS_SRCS): $(FX2LIBDIR)/lib/fx2.lib

$(FX2LIBDIR)/lib/fx2.lib: $(FX2LIBDIR)/.git
	cd $(dir $@) && make -j1

# We depend on a file inside the directory as git creates an
# empty dir for us.
#
# Note that although we have the variable FX2LIBDIR, the submodule
# magic will always check it out in fx2lib/
$(FX2LIBDIR)/.git: ../.gitmodules
	git submodule sync --recursive -- $$(dirname $@)
	git submodule update --recursive --init $$(dirname $@)
	touch $@ -r ../.gitmodules

$(TARGET).hex: $(CC_OBJS) $(AS_OBJS)
	$(Q_LINK)$(CC) $(CFLAGS) -o $@ $+ $(LIBS)

%.rel: %.a51
	$(Q_AS)$(AS) -logs $?
