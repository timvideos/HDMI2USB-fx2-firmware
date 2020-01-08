# configuration
BOARD ?= opsis
MODEL ?= small
SYNCDELAYLEN ?= 4

# use conda enviorment if it exists
MAKEFILE_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
export PATH := $(MAKEFILE_PATH)../conda/bin:$(PATH)

CFLAGS =
CFLAGS += --std-sdcc99
CFLAGS += -Wa"-p"
CFLAGS += --xram-size 0x0200
CFLAGS += -DSYNCDELAYLEN=$(SYNCDELAYLEN)

# should to override previously set flags
FLAGS ?=
CFLAGS += $(FLAGS)

# set USB IDs depending on board
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

CFLAGS += -DVID=0x$(VID) -DPID=0x$(PID) -DDID=0x$(DID)

LIBFX2DIR ?= ../third_party/libfx2
# variable required by libfx2 build system
LIBFX2 = $(LIBFX2DIR)/firmware/library
include $(LIBFX2)/fx2rules.mk

# force proper dependecy to automatically build libfx2
$(LIBFX2)/lib/$(MODEL)/fx2.lib: $(LIBFX2)/.stamp
