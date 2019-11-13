TARGET    = cdc-firmware
LIBRARIES = fx2 fx2usb fx2isrs
MODEL     = small

SOURCES = main

CFLAGS = --std-sdcc99 -Wa"-p" --xram-size 0x0200 -DDEBUG -DSYNCDELAYLEN=4

LIBFX2  = ../third_party/libfx2/firmware/library
include $(LIBFX2)/fx2rules.mk
