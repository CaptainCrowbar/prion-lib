# Grand Unified Makefile

# File naming conventions:
#	project/module.cpp       = Part of the library
#	project/module-test.cpp  = Unit test for part of the library
#	project/app-module.cpp   = Part of the application
# If the current directory ends in -lib or -engine, the library is installable;
# otherwise, if any application modules exist, the application is installable;
# otherwise, nothing is installable.

LIBROOT ?= ..
NAME := $(shell ls */*.{c,h,cpp,hpp} 2>/dev/null | sed -E 's!/.*!!'| uniq -c | sort | tail -n 1 | sed -E 's!^[ 0-9]+!!')
TAG := $(shell echo "$$PWD" | sed -E 's!^([^/]*/)*([^/]*-)?!!')
PREFIX := /usr/local
HOST := $(shell uname | tr A-Z a-z | sed -E 's/[^a-z].*//')
TARGET := $(shell gcc -v 2>&1 | grep '^Target:' | sed -E -e 's/^Target: //' -e 's/[0-9.]*$$//' | tr A-Z a-z)
XHOST := $(shell echo $(TARGET) | tr A-Z a-z | sed -E -e 's/-gnu$$//' -e 's/.*-//' -e 's/[^a-z].*//')
BUILD := build/$(TARGET)
CXX := g++
FLAGS := -I. -march=ivybridge -mfpmath=sse -Wall -Wextra -Werror
CCFLAGS :=
CXXFLAGS :=
OBJCFLAGS :=
DEFINES := -DNDEBUG=1
OPT := -O2
TESTOPT := -O1
AR := ar
ARFLAGS := -rsu
LDFLAGS := -L$(BUILD)
LDLIBS :=
STRIP := @echo >/dev/null
EXE :=
DEPENDS := dependencies.make
STATICLIB := $(BUILD)/lib$(NAME).a
HEADERS := $(shell find $(NAME) -name *.h -or -name *.hpp)
LIBHEADERS := $(filter-out $(NAME)/library.hpp Makefile,$(shell grep -EL '// NOT INSTALLED' $(HEADERS) Makefile)) # Dummy entry to avoid empty list
SOURCES := $(shell find $(NAME) -name *.c -or -name *.cpp -or -name *.m -or -name *.mm)
APPSOURCES := $(shell echo "$(SOURCES)" | tr ' ' '\n' | grep app-)
TESTSOURCES := $(shell echo "$(SOURCES)" | tr ' ' '\n' | grep '[-]test')
LIBSOURCES := $(filter-out $(APPSOURCES) $(TESTSOURCES),$(SOURCES))
APPOBJECTS := $(shell sed -E 's!$(NAME)/([^ ]+)\.[a-z]+!$(BUILD)/\1.o!g' <<< '$(APPSOURCES)')
LIBOBJECTS := $(shell sed -E 's!$(NAME)/([^ ]+)\.[a-z]+!$(BUILD)/\1.o!g' <<< '$(LIBSOURCES)')
TESTOBJECTS := $(shell sed -E 's!$(NAME)/([^ ]+)\.[a-z]+!$(BUILD)/\1.o!g' <<< '$(TESTSOURCES)')
DOCINDEX := $(wildcard $(NAME)/index.md)
DOCSOURCES := $(shell find $(NAME) -name '*.md' | sort)
DOCS := doc/style.css doc/index.html $(patsubst $(NAME)/%.md,doc/%.html,$(DOCSOURCES))
LIBTAG :=
SCRIPTS := $(LIBROOT)/core-lib/scripts

ifeq ($(HOST),cygwin)
	EXE := .exe
	LDFLAGS += -Wl,--enable-auto-import
	LIBTAG := cygwin
endif

ifeq ($(HOST),darwin)
	CXX := clang++
	DEFINES += -D_DARWIN_C_SOURCE=1
	LDFLAGS += -framework Cocoa
	LIBTAG := mac
endif

ifeq ($(HOST),linux)
	LIBTAG := linux
endif

ifeq ($(CXX),clang++)
	CXXFLAGS += -std=c++1z -stdlib=libc++
	STRIP := strip
else
	CXXFLAGS += -std=gnu++1z
	LDFLAGS += -s
endif

ifeq ($(XHOST),mingw)
	MINGW := $(shell type -P gcc | sed -E 's!/bin/.+!!')
	PREFIX := $(MINGW)
	FLAGS += -mthreads
	DEFINES += -DNOMINMAX=1 -DUNICODE=1 -D_UNICODE=1 -DWINVER=0x601 -D_WIN32_WINNT=0x601 -DPCRE_STATIC=1
	LIBPATH := $(MINGW)/lib $(subst ;, ,$(LIBRARY_PATH))
	LIBTAG := mingw
	RC := windres
	RCFLAGS := -O coff
	RCSOURCES := $(wildcard resources/*.rc) $(wildcard resources/*.ico)
	RCOBJECT := $(patsubst resources/%.rc,$(BUILD)/%.o,$(firstword $(RCSOURCES)))
	APPOBJECTS += $(RCOBJECT)
else
	DEFINES += -D_REENTRANT=1 -D_XOPEN_SOURCE=700
	LIBPATH := /usr/lib $(subst :, ,$(LIBRARY_PATH))
endif

ifneq ($(shell grep -Fo sdl $(DEPENDS)),)
	DEFINES += -DSDL_MAIN_HANDLED=1
endif

LD := $(CXX)
MAINAPP := $(BUILD)/$(NAME)$(EXE)
TESTAPP := $(BUILD)/test-$(NAME)$(EXE)

ifeq ($(TAG),lib)
	ifeq ($(LIBSOURCES),)
		INSTALLABLE := header-lib
	else
		INSTALLABLE := object-lib
	endif
else
	ifeq ($(TAG),engine)
		INSTALLABLE := object-lib
	else
		ifneq ($(APPSOURCES),)
			INSTALLABLE := application
		else
			INSTALLABLE := nothing
		endif
	endif
endif

.DELETE_ON_ERROR:

.PHONY: all static app run tests check install symlinks uninstall doc unlink undoc clean clean-all dep help
.PHONY: help-prefix help-app help-test help-install help-suffix print-%

all: static app tests doc

unlink:
	rm -f $(STATICLIB) $(MAINAPP) $(TESTAPP)

undoc:
	rm -f doc/*.html

clean:
	rm -rf $(BUILD) *.stackdump __test_*

clean-all:
	rm -rf build doc *.stackdump __test_*

dep:
	$(SCRIPTS)/make-dependencies

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

ifeq ($(APPSOURCES),)
app:
run:
help-app: help-prefix
else
app: $(MAINAPP)
run: app
	$(MAINAPP)
help-app: help-prefix
	@echo "    app        = Build the application"
	@echo "    run        = Build and run the application"
endif

ifeq ($(LIBSOURCES),)
STATICPART :=
else
STATICPART := $(STATICLIB)
endif

ifeq ($(TESTSOURCES),)
tests:
check:
help-test: help-app
else
tests: $(TESTAPP)
check: all
	@rm -rf __test_*
	$(TESTAPP)
	@rm -rf __test_*
help-test: help-app
	@echo "    tests      = Build the unit tests"
	@echo "    check      = Build everything and run the unit tests"
endif

ifeq ($(DOCSOURCES),)
doc:
else
doc: $(DOCS)
endif

ifeq ($(INSTALLABLE),application)
static: $(STATICPART)
install: uninstall app
	mkdir -p $(PREFIX)/bin
	cp $(MAINAPP) $(PREFIX)/bin
uninstall:
	rm -f $(PREFIX)/bin/$(notdir $(MAINAPP))
help-install: help-test
	@echo "    install    = Install the library"
	@echo "    symlinks   = Install the library using symlinks"
	@echo "    uninstall  = Uninstall the library"
ifeq ($(XHOST),mingw)
symlinks:
	$(error make symlinks is not supported on $(XHOST), use make install)
else
symlinks: uninstall app
	mkdir -p $(PREFIX)/bin
	ln -fs $(abspath $(MAINAPP)) $(PREFIX)/bin
endif
endif

ifeq ($(INSTALLABLE),header-lib)
static: $(STATICPART) $(NAME)/library.hpp
install: uninstall static
	mkdir -p $(PREFIX)/lib $(PREFIX)/include/$(NAME)
	cp $(LIBHEADERS) $(NAME)/library.hpp $(PREFIX)/include/$(NAME)
uninstall:
	if [ "$(PREFIX)" ]; then rm -f $(PREFIX)/lib/$(notdir $(STATICLIB)); rm -rf $(PREFIX)/include/$(NAME); fi
help-install: help-test
	@echo "    install    = Install the application"
	@echo "    symlinks   = Install the application using symlinks"
	@echo "    uninstall  = Uninstall the application"
ifeq ($(XHOST),mingw)
symlinks:
	$(error make symlinks is not supported on $(XHOST), use make install)
else
symlinks: uninstall static
	mkdir -p $(PREFIX)/lib $(PREFIX)/include
	ln -fs $(abspath $(NAME)) $(PREFIX)/include
endif
endif

ifeq ($(INSTALLABLE),object-lib)
static: $(STATICPART) $(NAME)/library.hpp
install: uninstall static
	mkdir -p $(PREFIX)/lib $(PREFIX)/include/$(NAME)
	cp $(LIBHEADERS) $(NAME)/library.hpp $(PREFIX)/include/$(NAME)
	cp $(STATICLIB) $(PREFIX)/lib
uninstall:
	if [ "$(PREFIX)" ]; then rm -f $(PREFIX)/lib/$(notdir $(STATICLIB)); rm -rf $(PREFIX)/include/$(NAME); fi
help-install: help-test
	@echo "    install    = Install the application"
	@echo "    symlinks   = Install the application using symlinks"
	@echo "    uninstall  = Uninstall the application"
ifeq ($(XHOST),mingw)
symlinks:
	$(error make symlinks is not supported on $(XHOST), use make install)
else
symlinks: uninstall static
	mkdir -p $(PREFIX)/lib $(PREFIX)/include
	ln -fs $(abspath $(NAME)) $(PREFIX)/include
	ln -fs $(abspath $(STATICLIB)) $(PREFIX)/lib
endif
endif

ifeq ($(INSTALLABLE),nothing)
static: $(STATICPART)
install:
symlinks:
uninstall:
help-install: help-test
endif

-include $(DEPENDS)

ifneq ($(XHOST),mingw)
	ifneq ($(XHOST),linux)
		LDLIBS += -liconv
	endif
	LDLIBS += -lpthread -lz
endif

$(BUILD)/%-test.o: $(NAME)/%-test.c
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(CCFLAGS) $(DEFINES) $(TESTOPT) -c $< -o $@

$(BUILD)/%-test.o: $(NAME)/%-test.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(CXXFLAGS) $(DEFINES) $(TESTOPT) -c $< -o $@

$(BUILD)/%-test.o: $(NAME)/%-test.m
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(OBJCFLAGS) $(DEFINES) $(TESTOPT) -c $< -o $@

$(BUILD)/%-test.o: $(NAME)/%-test.mm
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(OBJCFLAGS) $(CXXFLAGS) $(DEFINES) $(TESTOPT) -c $< -o $@

$(BUILD)/%.o: $(NAME)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(CCFLAGS) $(DEFINES) $(OPT) -c $< -o $@

$(BUILD)/%.o: $(NAME)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(CXXFLAGS) $(DEFINES) $(OPT) -c $< -o $@

$(BUILD)/%.o: $(NAME)/%.m
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(OBJCFLAGS) $(DEFINES) $(OPT) -c $< -o $@

$(BUILD)/%.o: $(NAME)/%.mm
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $(OBJCFLAGS) $(CXXFLAGS) $(DEFINES) $(OPT) -c $< -o $@

ifeq ($(XHOST),mingw)
$(RCOBJECT): $(RCSOURCES)
	mkdir -p $(dir $@)
	$(RC) $(RCFLAGS) $< $@
endif

$(STATICLIB): $(LIBOBJECTS)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(AR) $(ARFLAGS) $@ $^

$(MAINAPP): $(APPOBJECTS) $(STATICPART)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(LD) $(FLAGS) $(DEFINES) $(OPT) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(STRIP) $@

$(TESTAPP): $(TESTOBJECTS) $(STATICPART)
	@mkdir -p $(dir $@)
	@rm -f $@
	$(LD) $(FLAGS) $(DEFINES) $(TESTOPT) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(STRIP) $@

$(NAME)/library.hpp: $(LIBHEADERS)
	echo "#pragma once" > $@
	echo >> $@
	echo $(sort $(LIBHEADERS)) | tr ' ' '\n' | sed -E 's/.+/#include "&"/' >> $@

ifeq ($(DOCINDEX),)
doc/index.html: $(DOCSOURCES) $(SCRIPTS)/make-doc $(SCRIPTS)/make-index
	@mkdir -p $(dir $@)
	$(SCRIPTS)/make-index > __index__.md
	$(SCRIPTS)/make-doc __index__.md $@
	rm -f __index__.md
endif

doc/%.html: $(NAME)/%.md $(SCRIPTS)/make-doc
	@mkdir -p $(dir $@)
	$(SCRIPTS)/make-doc $< $@

doc/style.css: $(SCRIPTS)/style.css
	@mkdir -p $(dir $@)
	cp $< $@
