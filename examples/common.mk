#
# Copyright (C) 2009-2012 Chris McClelland
# Copyright (C) 2015 Joel Stanley <joel@jms.id.au>
# Copyright (C) 2017 Kyle Robbertze <krobbertze@gmail.com>
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
# To load the firmware you will need:
# 	HDMI2USB-mode-swtich from https://github.com/timvideos/HDMI2USB-mode-switch
# 
LIBS ?= $(FX2LIBDIR)/lib/fx2.lib
INCS += -I sdcc -I$(FX2LIBDIR)/include -I. -I../../common/boards

USE_16K ?= 1

CC_OBJS ?= $(CC_SRCS:%.c=%.rel)

CC = sdcc

CFLAGS += -mmcs51 --xram-size 0x0200 $(FLAGS)
CFLAGS += -DBOARD_$(BOARD)

ifeq ($(USE_16K),1)
	CFLAGS += --code-size 0x3e00
else
	CFLAGS += --code-size 0x1e00
endif

# Use make V=1 for a verbose build.
ifndef V
	Q_CC=@echo      '      CC ' $@;
	Q_AS=@echo      '      AS ' $@;
	Q_LINK=@echo    '    LINK ' $@;
	Q_RM=@echo      '   CLEAN ';
	Q_OBJCOPY=@echo ' OBJCOPY ' $@;
	Q_GEN=@echo     '     GEN ' $@;
endif

.PHONY: all clean

all: $(TARGET).hex

$(CC_SRCS): $(FX2LIBDIR)/lib/fx2.lib

$(FX2LIBDIR)/lib/fx2.lib: $(FX2LIBDIR)/.git
	cd $(dir $@) && make -j1

 
# We depend on a file inside the directory as git creates an
# empty dir for us.
#
# Note that although we have the variable FX2LIBDIR, the submodule
# magic will always check it out in fx2lib/
$(FX2LIBDIR)/.git: $(GITMODULESDIR)
	git submodule sync --recursive -- $$(dirname $@)
	git submodule update --recursive --init $$(dirname $@)
	touch $@ -r $<

$(TARGET).hex: $(CC_OBJS)
	$(Q_LINK)$(CC) $(CFLAGS) -o $@ $+ $(LIBS)

%.rel: %.c
	$(Q_CC)$(CC) $(CFLAGS) -c --disable-warning 85 $(INCS) $?

clean:
	$(Q_RM)$(RM) *.iic *.asm *.lnk *.lst *.map *.mem *.rel *.rst *.sym \
		*.lk serial.hex
	cd $(FX2LIBDIR) && make clean

distclean: clean
	$(RM) -r $(FX2LIBDIR)

load: $(TARGET).hex
	hdmi2usb-mode-switch --load-fx2-firmware $(TARGET).hex
