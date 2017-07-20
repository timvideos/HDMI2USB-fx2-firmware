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
TARGET := audio
FX2LIBDIR := ../third_party/fx2lib
LINUXHEADERSDIR := ../third_party/fx2lib-linux-headers
COMMON_DIR := ../common
INCS := -I$(LINUXHEADERSDIR)

CFLAGS := -DDSCR_AREA=0x3d00 --code-size 0x3c00 --xram-size 0x0200
CFLAGS += --xram-loc 0x3c00 -Wl"-b INT2JT=0x3f00"

CC_SRCS := descriptors.c debug.c audiodata.c firmware.c version_data.c

include $(COMMON_DIR)/common.mk

# See https://github.com/timvideos/HDMI2USB/wiki/USB-IDs
ifeq (${BOARD},atlys)
VID := 1D50
PID := 60B7
DID := 0012
else
ifeq (${BOARD},fx2miniboard)
VID := 1D50
PID := 60B7
DID := 0012
else
ifeq (${BOARD},opsis)
VID := 2A19
PID := 5441
DID := 0012
else
$(error "Unknown board type '$(BOARD)'")
endif
endif
endif

<<<<<<< HEAD
check-descriptors: descriptors.c
	gcc -I${FX2LIBDIR}/include -I$(LINUXHEADERSDIR) descriptors.c
=======
CFLAGS += -DDATE=0x$(FIRMWARE_VERSION) -mmcs51 --std-c99 -DSDCC -Wa"-p" $(FLAGS)
CFLAGS += -DDSCR_AREA=0x3d00 --code-size 0x3c00 --xram-size 0x0200
CFLAGS += --xram-loc 0x3c00 -Wl"-b INT2JT=0x3f00"

# Use make V=1 for a verbose build.
ifndef V
    Q_CC=@echo      '      CC ' $@;
    Q_AS=@echo      '      AS ' $@;
    Q_LINK=@echo    '    LINK ' $@;
    Q_RM=@echo      '   CLEAN ';
    Q_OBJCOPY=@echo ' OBJCOPY ' $@;
    Q_GEN=@echo     '     GEN ' $@;
endif

.PHONY: all clean check check_int2jt

all: $(TARGET).hex

$(CC_SRCS) $(AS_SRCS): $(FX2LIBDIR)/lib/fx2.lib $(LINUXHEADERSDIR)/.git
>>>>>>> audio: Place descriptors after the interrupt vector in memory

$(CC_SRCS): $(LINUXHEADERSDIR)/.git

# We depend on a file inside the directory as git creates an
# empty dir for us.
#
# Note that although we have the variable FX2LIBDIR, the submodule
# magic will always check it out in fx2lib/
$(LINUXHEADERSDIR)/.git: ../.gitmodules
	git submodule sync --recursive -- $$(dirname $@)
	git submodule update --recursive --init $$(dirname $@)
	touch $@ -r ../.gitmodules

%.rel: %.c descriptors.h
	$(Q_CC)$(CC) $(CFLAGS) -c --disable-warning 85 $(INCS) $<

version_data.h: Makefile
	$(Q_GEN) $(COMMON_DIR)/version_data.sh $(VID) $(PID) $(DID)

# FIXME: This should be part of a common Makefile in $(LINUXHEADERSDIR)/.git
descriptors_strings.h: $(LINUXHEADERSDIR)/.git descriptors.strings
	@python2 $(LINUXHEADERSDIR)/descriptors_string_table.py --header < descriptors.strings > descriptors_strings.h

descriptors_strings.inc: $(LINUXHEADERSDIR)/.git descriptors.strings
	@python2 $(LINUXHEADERSDIR)/descriptors_string_table.py --cfile < descriptors.strings > descriptors_strings.inc

descriptors.h: version_data.h descriptors_strings.h
descriptors.c: descriptors_strings.h descriptors_strings.inc
