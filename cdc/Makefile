TARGET    = cdc-firmware
LIBRARIES = fx2 fx2usb fx2isrs
MODEL     = small
BOARD = atlys

LIBFX2  = ../third_party/libfx2/firmware/library
SOURCES = main descriptors

SYNCDELAYLEN = 4

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

CFLAGS = --std-sdcc99 -Wa"-p" --xram-size 0x0200 
CFLAGS += -DDEBUG -DSYNCDELAYLEN=$(SYNCDELAYLEN)
CFLAGS += -DVID=0x$(VID) -DPID=0x$(PID) -DDID=0x$(DID)

include $(LIBFX2)/fx2rules.mk
