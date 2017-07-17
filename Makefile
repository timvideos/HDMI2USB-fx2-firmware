FX2LIBDIR = ./third_party/fx2lib

# We depend on the .git file inside the directory as git creates an empty dir
# for us.
$(FX2LIBDIR)/.git: .gitmodules
	git submodule sync --recursive -- $$(dirname $@)
	git submodule update --recursive --init $$(dirname $@)
	touch $@ -r .gitmodules

# FIXME: Add check_int2jit from hdmi2usb/Makefile

docs: export PROJECT_NUMBER:=$(shell git describe --always --dirty --long)

.PHONY: docs clean

docs:
	doxygen docs/docs.conf

clean:
	rm -fr docs/html docs/latex
