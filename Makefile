# Grand Unified Makefile

# File naming conventions:
#	project/module.cpp       = Part of the library
#	project/module-test.cpp  = Unit test for part of the library
#	project/app-module.cpp   = Part of the application
# If the current directory ends in -lib or -engine, the library is installable;
# otherwise, if any application modules exist, the application is installable;
# otherwise, nothing is installable.

# Common variables

LIBROOT ?= ..
project_name := $(shell ls */*.{c,h,cpp,hpp} 2>/dev/null | sed -E 's!/.*!!'| uniq -c | sort | tail -n 1 | sed -E 's!^[ 0-9]+!!')
project_tag := $(shell echo "$$PWD" | sed -E 's!^([^/]*/)*([^/]*-)?!!')
install_prefix := /usr/local
build_host := $(shell uname | tr A-Z a-z | sed -E 's/[^a-z].*//')
build_target := $(shell gcc -v 2>&1 | grep '^Target:' | sed -E -e 's/^Target: //' -e 's/[0-9.]*$$//' | tr A-Z a-z)
cross_host := $(shell echo $(build_target) | tr A-Z a-z | sed -E -e 's/-gnu$$//' -e 's/.*-//' -e 's/[^a-z].*//')
BUILD := build/$(build_target)
CXX := g++
AR := ar
ARFLAGS := -rsu
LDLIBS :=
STRIP := @echo >/dev/null
LIBTAG :=
common_flags := -I. -march=ivybridge -mfpmath=sse
diagnostic_flags := -Wall -Wextra -Werror -Wdouble-promotion -Wformat=2 -Wnull-dereference -Wold-style-cast
cc_specific_flags :=
cxx_specific_flags :=
objc_specific_flags :=
cc_defines := -DNDEBUG=1
nontest_flags := -O2
test_flags := -O1
ld_specific_flags := -L$(BUILD)
exe_suffix :=
dependency_file := dependencies.make
static_library := $(BUILD)/lib$(project_name).a
all_headers := $(shell find $(project_name) -name *.h -or -name *.hpp)
library_headers := $(filter-out $(project_name)/library.hpp Makefile,$(shell grep -EL '// NOT INSTALLED' $(all_headers) Makefile)) # Dummy entry to avoid empty list
all_sources := $(shell find $(project_name) -name *.c -or -name *.cpp -or -name *.m -or -name *.mm)
app_sources := $(shell echo "$(all_sources)" | tr ' ' '\n' | grep app-)
test_sources := $(shell echo "$(all_sources)" | tr ' ' '\n' | grep '[-]test')
library_sources := $(filter-out $(app_sources) $(test_sources),$(all_sources))
app_objects := $(shell sed -E 's!$(project_name)/([^ ]+)\.[a-z]+!$(BUILD)/\1.o!g' <<< '$(app_sources)')
test_objects := $(shell sed -E 's!$(project_name)/([^ ]+)\.[a-z]+!$(BUILD)/\1.o!g' <<< '$(test_sources)')
library_objects := $(shell sed -E 's!$(project_name)/([^ ]+)\.[a-z]+!$(BUILD)/\1.o!g' <<< '$(library_sources)')
doc_index := $(wildcard $(project_name)/index.md)
doc_sources := $(shell find $(project_name) -name '*.md' | sort)
doc_pages := doc/style.css doc/index.html $(patsubst $(project_name)/%.md,doc/%.html,$(doc_sources))
scripts_dir := $(LIBROOT)/core-lib/scripts

# System or tool specific variables

ifeq ($(build_host),cygwin)
	LIBTAG := cygwin
	exe_suffix := .exe
	ld_specific_flags += -Wl,--enable-auto-import
endif

ifeq ($(build_host),darwin)
	LIBTAG := mac
	CXX := clang++
	cc_defines += -D_DARWIN_C_SOURCE=1
	ld_specific_flags += -framework Cocoa
endif

ifeq ($(build_host),linux)
	LIBTAG := linux
endif

ifeq ($(CXX),clang++)
	STRIP := strip
	cxx_specific_flags += -std=c++1z -stdlib=libc++
else
	cxx_specific_flags += -std=gnu++1z
	ld_specific_flags += -s
endif

ifeq ($(cross_host),mingw)
	LIBTAG := mingw
	mingw_root := $(shell type -P gcc | sed -E 's!/bin/.+!!')
	install_prefix := $(mingw_root)
	common_flags += -mthreads
	cc_defines += -DNOMINMAX=1 -DUNICODE=1 -D_UNICODE=1 -DWINVER=0x601 -D_WIN32_WINNT=0x601 -DPCRE_STATIC=1
	RC := windres
	RCFLAGS := -O coff
	resource_files := $(wildcard resources/*.rc) $(wildcard resources/*.ico)
	resource_object := $(patsubst resources/%.rc,$(BUILD)/%.o,$(firstword $(resource_files)))
	app_objects += $(resource_object)
else
	cc_defines += -D_REENTRANT=1 -D_XOPEN_SOURCE=700
endif

ifneq ($(shell grep -Fo sdl $(dependency_file)),)
	cc_defines += -DSDL_MAIN_HANDLED=1
endif

ifeq ($(library_sources),)
	static_target :=
else
	static_target := $(static_library)
endif

app_target := $(BUILD)/$(project_name)$(exe_suffix)
unit_test_target := $(BUILD)/test-$(project_name)$(exe_suffix)

# Collect the build tools and their options

CC := $(CXX)
OBJC := $(CXX)
OBJCXX := $(CXX)
LD := $(CXX)
CFLAGS := $(common_flags) $(diagnostic_flags) $(cc_specific_flags) $(cc_defines) $(nontest_flags)
CXXFLAGS := $(common_flags) $(diagnostic_flags) $(cxx_specific_flags) $(cc_defines) $(nontest_flags)
OBJCFLAGS := $(common_flags) $(diagnostic_flags) $(objc_specific_flags) $(cc_defines) $(nontest_flags)
OBJCXXFLAGS := $(common_flags) $(diagnostic_flags) $(objc_specific_flags) $(cxx_specific_flags) $(cc_defines) $(nontest_flags)
LDFLAGS := $(common_flags) $(diagnostic_flags) $(cc_defines) $(nontest_flags) $(ld_specific_flags)
test_cflags := $(common_flags) $(diagnostic_flags) $(cc_specific_flags) $(cc_defines) $(test_flags)
test_cxxflags := $(common_flags) $(diagnostic_flags) $(cxx_specific_flags) $(cc_defines) $(test_flags)
test_objcflags := $(common_flags) $(diagnostic_flags) $(objc_specific_flags) $(cc_defines) $(test_flags)
test_objcxxflags := $(common_flags) $(diagnostic_flags) $(objc_specific_flags) $(cxx_specific_flags) $(cc_defines) $(test_flags)
test_ldflags := $(common_flags) $(diagnostic_flags) $(cc_defines) $(test_flags) $(ld_specific_flags)

# Work out which kind of installable target we want

ifeq ($(project_tag),lib)
	ifeq ($(library_sources),)
		install_type := header-lib
	else
		install_type := object-lib
	endif
else
	ifeq ($(project_tag),engine)
		install_type := object-lib
	else
		ifneq ($(app_sources),)
			install_type := application
		else
			install_type := nothing
		endif
	endif
endif

# Common build targets

.DELETE_ON_ERROR:

.PHONY: all static app run tests check install symlinks uninstall doc unlink undoc clean clean-all dep
.PHONY: help help-prefix help-app help-test help-install help-suffix print-%

all: static app tests doc

unlink:
	rm -f $(static_library) $(app_target) $(unit_test_target)

undoc:
	rm -f doc/*.html

clean:
	rm -rf $(BUILD) *.stackdump __test_*

clean-all:
	rm -rf build doc *.stackdump __test_*

dep:
	$(scripts_dir)/make-dependencies

help: help-suffix

help-prefix:
	@echo
	@echo "Make targets:"
	@echo
	@echo "    all        = Build everything (default)"
	@echo "    static     = Build the static library"

help-suffix: help-install
	@echo "    doc        = Build the documentation"
	@echo "    unlink     = Delete link targets but not object modules"
	@echo "    undoc      = Delete the documentation"
	@echo "    clean      = Delete all targets for this toolset"
	@echo "    clean-all  = Delete all targets for all toolsets and docs"
	@echo "    dep        = Rebuild makefile dependencies"
	@echo "    help       = List make targets"
	@echo "    print-*    = Query a makefile variable"
	@echo

print-%:
	@echo "$* = $($*)"

# Targets that depend on whether we're building an app

ifneq ($(app_sources),)

app: $(app_target)

run: app
	$(app_target)

help-app: help-prefix
	@echo "    app        = Build the application"
	@echo "    run        = Build and run the application"

else

app:

run:

help-app: help-prefix

endif

# Targets that depend on whether we have any unit tests

ifneq ($(test_sources),)

tests: $(unit_test_target)

check: all
	@rm -rf __test_*
	$(unit_test_target)
	@rm -rf __test_*

help-test: help-app
	@echo "    tests      = Build the unit tests"
	@echo "    check      = Build everything and run the unit tests"

else

tests:

check:

help-test: help-app

endif

# Targets that depend on whether we have any documentation

ifneq ($(doc_sources),)

doc: $(doc_pages)

else

doc:

endif

# Targets specific to building an application

ifeq ($(install_type),application)

static: $(static_target)

install: uninstall app
	mkdir -p $(install_prefix)/bin
	cp $(app_target) $(install_prefix)/bin

uninstall:
	rm -f $(install_prefix)/bin/$(notdir $(app_target))

help-install: help-test
	@echo "    install    = Install the library"
	@echo "    symlinks   = Install the library using symlinks"
	@echo "    uninstall  = Uninstall the library"

ifeq ($(cross_host),mingw)

symlinks:
	$(error make symlinks is not supported on $(cross_host), use make install)

else

symlinks: uninstall app
	mkdir -p $(install_prefix)/bin
	ln -fs $(abspath $(app_target)) $(install_prefix)/bin

endif

endif

# Targets specific to building a header-only library

ifeq ($(install_type),header-lib)

static: $(static_target) $(project_name)/library.hpp

install: uninstall static
	mkdir -p $(install_prefix)/lib $(install_prefix)/include/$(project_name)
	cp $(library_headers) $(project_name)/library.hpp $(install_prefix)/include/$(project_name)

uninstall:
	if [ "$(install_prefix)" ]; then rm -f $(install_prefix)/lib/$(notdir $(static_library)); rm -rf $(install_prefix)/include/$(project_name); fi

help-install: help-test
	@echo "    install    = Install the application"
	@echo "    symlinks   = Install the application using symlinks"
	@echo "    uninstall  = Uninstall the application"

ifeq ($(cross_host),mingw)

symlinks:
	$(error make symlinks is not supported on $(cross_host), use make install)

else

symlinks: uninstall static
	mkdir -p $(install_prefix)/lib $(install_prefix)/include
	ln -fs $(abspath $(project_name)) $(install_prefix)/include

endif

endif

# Targets specific to building a non-header library

ifeq ($(install_type),object-lib)

static: $(static_target) $(project_name)/library.hpp

install: uninstall static
	mkdir -p $(install_prefix)/lib $(install_prefix)/include/$(project_name)
	cp $(library_headers) $(project_name)/library.hpp $(install_prefix)/include/$(project_name)
	cp $(static_library) $(install_prefix)/lib

uninstall:
	if [ "$(install_prefix)" ]; then rm -f $(install_prefix)/lib/$(notdir $(static_library)); rm -rf $(install_prefix)/include/$(project_name); fi

help-install: help-test
	@echo "    install    = Install the application"
	@echo "    symlinks   = Install the application using symlinks"
	@echo "    uninstall  = Uninstall the application"

ifeq ($(cross_host),mingw)

symlinks:
	$(error make symlinks is not supported on $(cross_host), use make install)

else

symlinks: uninstall static
	mkdir -p $(install_prefix)/lib $(install_prefix)/include
	ln -fs $(abspath $(project_name)) $(install_prefix)/include
	ln -fs $(abspath $(static_library)) $(install_prefix)/lib

endif

endif

# Fallback targets when none of the above apply

ifeq ($(install_type),nothing)

static: $(static_target)

install:

symlinks:

uninstall:

help-install: help-test

endif

# Include the dependency list

-include $(dependency_file)

# System specific link libraries

ifneq ($(cross_host),mingw)
	ifneq ($(cross_host),linux)
		LDLIBS += -liconv
	endif
	LDLIBS += -lpthread -lz
endif

# Rules for building objects from source

$(BUILD)/%-test.o: $(project_name)/%-test.c
	@mkdir -p $(dir $@)
	$(CC) $(test_cflags) -c $< -o $@

$(BUILD)/%-test.o: $(project_name)/%-test.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(test_cxxflags) -c $< -o $@

$(BUILD)/%-test.o: $(project_name)/%-test.m
	@mkdir -p $(dir $@)
	$(OBJC) $(test_objcflags) -c $< -o $@

$(BUILD)/%-test.o: $(project_name)/%-test.mm
	@mkdir -p $(dir $@)
	$(OBJCXX) $(test_objcxxflags) -c $< -o $@

$(BUILD)/%.o: $(project_name)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(project_name)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: $(project_name)/%.m
	@mkdir -p $(dir $@)
	$(OBJC) $(OBJCFLAGS) -c $< -o $@

$(BUILD)/%.o: $(project_name)/%.mm
	@mkdir -p $(dir $@)
	$(OBJCXX) $(OBJCXXFLAGS) -c $< -o $@

ifeq ($(cross_host),mingw)
$(resource_object): $(resource_files)
	mkdir -p $(dir $@)
	$(RC) $(RCFLAGS) $< $@
endif

# Rules for building the final target from objects

$(static_library): $(library_objects)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(AR) $(ARFLAGS) $@ $^

$(app_target): $(app_objects) $(static_target)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(STRIP) $@

$(unit_test_target): $(test_objects) $(static_target)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(LD) $(test_ldflags) $^ $(LDLIBS) -o $@
	$(STRIP) $@

# Other build rules

$(project_name)/library.hpp: $(library_headers)
	echo "#pragma once" > $@
	echo >> $@
	echo $(sort $(library_headers)) | tr ' ' '\n' | sed -E 's/.+/#include "&"/' >> $@

ifeq ($(doc_index),)
doc/index.html: $(doc_sources) $(scripts_dir)/make-doc $(scripts_dir)/make-index
	@mkdir -p $(dir $@)
	$(scripts_dir)/make-index > __index__.md
	$(scripts_dir)/make-doc __index__.md $@
	rm -f __index__.md
endif

doc/%.html: $(project_name)/%.md $(scripts_dir)/make-doc
	@mkdir -p $(dir $@)
	$(scripts_dir)/make-doc $< $@

doc/style.css: $(scripts_dir)/style.css
	@mkdir -p $(dir $@)
	cp $< $@
