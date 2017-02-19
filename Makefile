
# We depend on the .git file inside the directory as git creates an empty dir
# for us.
$(FX2LIBDIR)/.git: .gitmodules
	git submodule sync --recursive -- $$(dirname $@)
	git submodule update --recursive --init $$(dirname $@)
	touch $@ -r .gitmodules
