# Grand Unified Makefile

# File naming conventions:
#	project/module.cpp       = Part of the library
#	project/module-test.cpp  = Unit test for part of the library
#	project/app-module.cpp   = Part of the application
# If the current directory ends in -lib or -engine, the library is installable;
# otherwise, if any application modules exist, the application is installable;
# otherwise, nothing is installable.

# Project identity

LIBROOT ?= ..
project_name := $(shell ls */*.{c,h,cpp,hpp} 2>/dev/null | sed -E 's!/.*!!'| uniq -c | sort | tail -n 1 | sed -E 's!^[ 0-9]+!!')
project_tag := $(shell echo "$$PWD" | sed -E 's!^([^/]*/)*([^/]*-)?!!')
dependency_file := dependencies.make
install_prefix := /usr/local
scripts_dir := $(LIBROOT)/core-lib/scripts

# Toolset identity

build_host := $(shell uname | tr A-Z a-z | sed -E 's/[^a-z].*//')

ifeq ($(TOOLS),msvc)
	build_target := $(shell cl 2>&1 | head -n 1 | sed -E 's/^.*Version ([0-9]+)\.([0-9]+)\.[0-9]+ for (.+)$$/msvc-\1\2-\3/')
	cross_target := msvc
else
	build_target := $(shell gcc -v 2>&1 | grep '^Target:' | sed -E -e 's/^Target: //' -e 's/[0-9.]*$$//' | tr A-Z a-z)
	cross_target := $(shell echo $(build_target) | tr A-Z a-z | sed -E -e 's/-gnu$$//' -e 's/.*-//' -e 's/[^a-z].*//')
endif

# Toolset configuration

# Start with generic Unix settings, then modify based on the toolset.
# Empty comments are a makefile trick to create a variable value with a trailing space.

BUILD := build/$(build_target)
LIBTAG := $(cross_target)
CXX := g++
common_flags := -march=ivybridge -mfpmath=sse
diagnostic_flags := -Wall -Wextra -Werror
cc_specific_flags :=
cxx_specific_flags :=
objc_specific_flags :=
cc_defines := -DNDEBUG=1
nontest_flags := -O2
test_flags := -O1
cc_output := -o #
include_path := -I.
AR := ar
ar_specific_flags := -rsu
ar_output :=
LDLIBS :=
ld_specific_flags :=
ld_output := -o #
library_path := -L$(BUILD)
lib_prefix := lib
lib_suffix := .a
exe_suffix :=
native_windows :=
STRIP := @echo >/dev/null

ifeq ($(build_host),cygwin)
	exe_suffix := .exe
endif

ifeq ($(cross_target),msvc)
	install_prefix := ~/Dropbox/Lib/msvc
	windows_prefix := $(shell cygpath -aw $(install_prefix))
	vcpkg_prefix := $(VCPKG_ROOT)/installed/x64-windows-static
	CXX := cl
	common_flags := /EHc /EHs /fp:precise /Gy /MT /MP /nologo /sdl /utf-8 /Y-
	diagnostic_flags := /W4 /WX
	cc_specific_flags :=
	cxx_specific_flags := /permissive- /std:c++latest
	cc_defines := /D_CRT_SECURE_NO_WARNINGS=1 /DNDEBUG=1 /DNOMINMAX=1 /DUNICODE=1 /D_UNICODE=1 /DWINVER=0x601 /D_WIN32_WINNT=0x601
	nontest_flags := /O2
	test_flags :=
	cc_output := /Fo
	include_path := /I. /I$(windows_prefix)/include /I$(vcpkg_prefix)/include
	AR := lib
	ar_specific_flags :=
	ar_output := /OUT:
	LD := link
	LDLIBS :=
	ld_specific_flags := /CGTHREADS:4 /MACHINE:X64 /NOLOGO /NXCOMPAT /SUBSYSTEM:CONSOLE
	ld_output := /OUT:
	library_path := /LIBPATH:$(BUILD) /LIBPATH:$(windows_prefix)/lib /LIBPATH:$(vcpkg_prefix)/lib
	lib_prefix :=
	lib_suffix := .lib
	RC := rc
	RCFLAGS :=
	rc_output := /fo
	native_windows := yes
endif

ifeq ($(cross_target),cygwin)
	cc_defines += -D_REENTRANT=1 -D_XOPEN_SOURCE=700
	cxx_specific_flags += -std=gnu++1z
	ld_specific_flags += -s -Wl,--enable-auto-import
endif

ifeq ($(cross_target),darwin)
	LIBTAG := mac
	CXX := clang++
	cc_defines += -D_DARWIN_C_SOURCE=1 -D_REENTRANT=1 -D_XOPEN_SOURCE=700
	cxx_specific_flags += -std=c++1z -stdlib=libc++
	ld_specific_flags += -framework Cocoa
	STRIP := strip
endif

ifeq ($(cross_target),linux)
	cc_defines += -D_REENTRANT=1 -D_XOPEN_SOURCE=700
	cxx_specific_flags += -std=gnu++1z
	ld_specific_flags += -s
endif

ifneq ($(shell grep -Fo sdl $(dependency_file)),)
	cc_defines += -DSDL_MAIN_HANDLED=1
endif

# Collect the build tools and their options

CC := $(CXX)
OBJC := $(CXX)
OBJCXX := $(CXX)
CFLAGS := $(common_flags) $(diagnostic_flags) $(include_path) $(cc_specific_flags) $(cc_defines) $(nontest_flags)
CXXFLAGS := $(common_flags) $(diagnostic_flags) $(include_path) $(cxx_specific_flags) $(cc_defines) $(nontest_flags)
OBJCFLAGS := $(common_flags) $(diagnostic_flags) $(include_path) $(objc_specific_flags) $(cc_defines) $(nontest_flags)
OBJCXXFLAGS := $(common_flags) $(diagnostic_flags) $(include_path) $(objc_specific_flags) $(cxx_specific_flags) $(cc_defines) $(nontest_flags)
test_cflags := $(common_flags) $(diagnostic_flags) $(include_path) $(cc_specific_flags) $(cc_defines) $(test_flags)
test_cxxflags := $(common_flags) $(diagnostic_flags) $(include_path) $(cxx_specific_flags) $(cc_defines) $(test_flags)
test_objcflags := $(common_flags) $(diagnostic_flags) $(include_path) $(objc_specific_flags) $(cc_defines) $(test_flags)
test_objcxxflags := $(common_flags) $(diagnostic_flags) $(include_path) $(objc_specific_flags) $(cxx_specific_flags) $(cc_defines) $(test_flags)

ifeq ($(cross_target),msvc)
	ARFLAGS := $(ar_specific_flags) $(library_path)
	LDFLAGS := $(ld_specific_flags) $(library_path)
	test_ldflags := $(ld_specific_flags) $(library_path)
else
	ARFLAGS := $(ar_specific_flags)
	LD := $(CXX)
	LDFLAGS := $(common_flags) $(diagnostic_flags) $(cc_defines) $(nontest_flags) $(ld_specific_flags) $(library_path)
	test_ldflags := $(common_flags) $(diagnostic_flags) $(cc_defines) $(test_flags) $(ld_specific_flags) $(library_path)
endif

# File names

all_headers := $(shell find $(project_name) -name *.hpp)
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
static_library := $(BUILD)/$(lib_prefix)$(project_name)$(lib_suffix)

ifneq ($(native_windows),)
	resource_files := $(wildcard resources/*.rc) $(wildcard resources/*.ico)
	resource_object := $(patsubst resources/%.rc,$(BUILD)/%.o,$(firstword $(resource_files)))
	app_objects += $(resource_object)
endif

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

app_target := $(BUILD)/$(project_name)$(exe_suffix)
static_target :=
test_target := $(BUILD)/test-$(project_name)$(exe_suffix)

ifneq ($(library_sources),)
	static_target := $(static_library)
endif

# Common build targets

.DELETE_ON_ERROR:

.PHONY: all static app run tests check install symlinks uninstall doc unlink undoc clean clean-all dep
.PHONY: help help-prefix help-app help-test help-install help-suffix print-%

all: static app tests doc

unlink:
	rm -f $(static_library) $(app_target) $(test_target)

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

tests: $(test_target)

check: all
	@rm -rf __test_*
	$(test_target)
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

ifeq ($(native_windows),yes)

symlinks:
	$(error make symlinks is not supported on $(cross_target), use make install)

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

ifeq ($(native_windows),yes)

symlinks:
	$(error make symlinks is not supported on $(cross_target), use make install)

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

ifeq ($(native_windows),yes)

symlinks:
	$(error make symlinks is not supported on $(cross_target), use make install)

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

ifeq ($(native_windows),)
	ifneq ($(cross_target),linux)
		LDLIBS += -liconv
	endif
	LDLIBS += -lpthread -lz
endif

# Rules for building objects from source

$(BUILD)/%-test.o: $(project_name)/%-test.c
	@mkdir -p $(dir $@)
	$(CC) $(test_cflags) -c $< $(cc_output)$@

$(BUILD)/%-test.o: $(project_name)/%-test.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(test_cxxflags) -c $< $(cc_output)$@

$(BUILD)/%-test.o: $(project_name)/%-test.m
	@mkdir -p $(dir $@)
	$(OBJC) $(test_objcflags) -c $< $(cc_output)$@

$(BUILD)/%-test.o: $(project_name)/%-test.mm
	@mkdir -p $(dir $@)
	$(OBJCXX) $(test_objcxxflags) -c $< $(cc_output)$@

$(BUILD)/%.o: $(project_name)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(cc_output)$@

$(BUILD)/%.o: $(project_name)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< $(cc_output)$@

$(BUILD)/%.o: $(project_name)/%.m
	@mkdir -p $(dir $@)
	$(OBJC) $(OBJCFLAGS) -c $< $(cc_output)$@

$(BUILD)/%.o: $(project_name)/%.mm
	@mkdir -p $(dir $@)
	$(OBJCXX) $(OBJCXXFLAGS) -c $< $(cc_output)$@

# Rules for building the final target from objects

$(static_library): $(library_objects)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(AR) $(ARFLAGS) $(ar_output)$@ $^

$(app_target): $(app_objects) $(static_target)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(LD) $(LDFLAGS) $^ $(LDLIBS) $(ld_output)$@
	$(STRIP) $@

$(test_target): $(test_objects) $(static_target)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(LD) $(test_ldflags) $^ $(LDLIBS) $(ld_output)$@
	$(STRIP) $@

# Other build rules

$(project_name)/library.hpp: $(library_headers)
	echo "#pragma once" > $@
	echo >> $@
	echo $(sort $(library_headers)) | tr ' ' '\n' | sed -E 's/.+/#include "&"/' >> $@

$(project_name)/unit-test.cpp: $(filter-out $(project_name)/unit-test.cpp,$(test_sources))
	$(scripts_dir)/make-tests

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
