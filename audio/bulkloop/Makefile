FX2LIBDIR=../../third_party/fx2lib
LINUXLIBDIR=../../third_party/fx2lib-linux-headers
BASENAME = bulkloop
SOURCES=bulkloop.c
PID=0x1004
BUILDDIR=build
INCLUDES=$(LINUXLIBDIR)

include $(FX2LIBDIR)/lib/fx2.mk

DSCR_AREA=-DDSCR_AREA=0x3e00 -DDSCR_VID=$(VID) -DDSCR_PID=$(PID)
SDCCFLAGS += --std-c99
SOURCES += descriptors.c

$(BUILDDIR)/descriptors_stringtable.h: ./generate_stringtable.py descriptors.strings
	./$< --header < descriptors.strings > $@

$(BUILDDIR)/descriptors_stringtable.inc: generate_stringtable.py descriptors.strings
	./$< --cfile < descriptors.strings > $@


$(BUILDDIR)/$(BASENAME).ihx: descriptors.c $(BUILDDIR)/descriptors_stringtable.h $(BUILDDIR)/descriptors_stringtable.inc

# Check the descriptors using GCC for better error messages
check-descriptors: $(DEPS)
	gcc -Wall -Werror -I$(FX2LIBDIR)/include descriptors.c
	rm a.out

clean-descriptors:
	rm -f $(foreach ext, h inc, $(BUILDDIR)/descriptors_stringtable.${ext})

clean: clean-descriptors

$(LINUXLIBDIR)/.git: ../../.gitmodules
	git submodule update --recursive --init $(dir $@)
	touch $@ -r ../../.gitmodules

.PHONY: check-descriptors clean-descriptors

test: test.cpp
	g++ -o test test.cpp -lusb-1.0
