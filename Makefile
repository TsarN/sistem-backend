IS_WIN32 := $(shell uname | grep 'NT' > /dev/null && echo 1)
SBOX_LDFLAGS := $(if $(IS_WIN32),-lpsapi)
CHECKER_LDFLAGS := $(if $(IS_WIN32),-llibchecker,-lchecker)
RUNSBOX_LDFLAGS := $(if $(IS_WIN32),-llibsbox,-lsbox)
SHARED_LIB_SUFFIX := $(if $(IS_WIN32),.dll,.so)
EXECUTABLE_SUFFIX := $(if $(IS_WIN32),.exe)
CFLAGS := -std=c99 -g -Wall -Werror -I. $(if $(IS_WIN32), ,-fPIC)
CXXFLAGS := -g -Wall -Werror -I. $(if $(IS_WIN32), ,-fPIC)
LDFLAGS := -lm
PREFIX := /usr
LIBDIR := $(PREFIX)/lib
BINDIR := $(PREFIX)/bin
INCLUDEDIR := $(PREFIX)/include
MANDIR := $(PREFIX)/share/man
CC := gcc
CXX := g++

OBJS_RUNSBOX := runsbox.c.o
TARGET_RUNSBOX := runsbox$(EXECUTABLE_SUFFIX)

TARGET_LIBSBOX := libsbox
OBJS_LIBSBOX := sbox.c.o
LIBSBOX_SHARED := $(TARGET_LIBSBOX)$(SHARED_LIB_SUFFIX)
LIBSBOX_STATIC := $(TARGET_LIBSBOX).a
TARGET_LIBSBOX_ALL := $(LIBSBOX_SHARED) $(LIBSBOX_STATIC)

TARGET_LIBCHECKER := libchecker
OBJS_LIBCHECKER := checker.c.o
LIBCHECKER_SHARED := $(TARGET_LIBCHECKER)$(SHARED_LIB_SUFFIX)
LIBCHECKER_STATIC := $(TARGET_LIBCHECKER).a
TARGET_LIBCHECKER_ALL := $(LIBCHECKER_SHARED) $(LIBCHECKER_STATIC)

INSTALL_HEADERS := sbox.h checker.h
INSTALL_BIN := $(TARGET_RUNSBOX)
INSTALL_LIB := $(TARGET_LIBSBOX_ALL) $(TARGET_LIBCHECKER_ALL)
INSTALL_MAN1 := runsbox.1

TESTS := $(shell ls -d tests/*/ | sed -e 's@/$$@@' -e 's@^.*/@@')

VPATH := $(dir $(shell find -iname '*.c' -o -iname '*.h' -o -iname '*.cpp'))

TARGET_TESTS_C := $(addsuffix $(EXECUTABLE_SUFFIX) , $(basename $(shell find tests -iname 'test.c')))
TARGET_TESTS_CXX := $(addsuffix $(EXECUTABLE_SUFFIX) , $(basename $(shell find tests -iname 'test.cpp')))
TARGET_TESTS := $(TARGET_TESTS_C) $(TARGET_TESTS_CXX)
OBJS_TESTS_C := $(addsuffix .c.o , $(basename $(TARGET_TESTS_C)))
OBJS_TESTS_CXX := $(addsuffix .cpp.o , $(basename $(TARGET_TESTS_CXX)))
OBJS_TESTS := $(OBJS_TESTS_C) $(OBJS_TESTS_CXX)

TARGET_CHECKERS_C := $(addsuffix $(EXECUTABLE_SUFFIX) , $(basename $(shell find checkers -iname '*.c')))
TARGET_CHECKERS_CXX := $(addsuffix $(EXECUTABLE_SUFFIX) , $(basename $(shell find checkers -iname '*.cpp')))
TARGET_CHECKERS := $(TARGET_CHECKERS_C) $(TARGET_CHECKERS_CXX)
OBJS_CHECKERS_C := $(addsuffix .c.o , $(basename $(TARGET_CHECKERS_C)))
OBJS_CHECKERS_CXX := $(addsuffix .cpp.o , $(basename $(TARGET_CHECKERS_CXX)))
OBJS_CHECKERS := $(OBJS_CHECKERS_C) $(OBJS_CHECKERS_CXX)

all: $(TARGET_RUNSBOX) $(TARGET_LIBSBOX) $(TARGET_LIBCHECKER) $(TARGET_CHECKERS) $(TARGET_TESTS)

$(TARGET_LIBSBOX): $(TARGET_LIBSBOX_ALL)
$(TARGET_LIBCHECKER): $(TARGET_LIBCHECKER_ALL)
tests: $(TARGET_TESTS)
checkers: $(TARGET_CHECKERS)

$(LIBSBOX_SHARED): $(OBJS_LIBSBOX)
	@ echo "   CC   $@"
	@ $(CC) $(CFLAGS) -shared -o $@ $< $(SBOX_LDFLAGS)

$(LIBSBOX_STATIC): $(OBJS_LIBSBOX)
	@ echo "   AR   $@"
	@ $(AR) rcs $@ $<

$(LIBCHECKER_SHARED): $(OBJS_LIBCHECKER)
	@ echo "   CC   $@"
	@ $(CC) $(CFLAGS) -shared -o $@ $<

$(LIBCHECKER_STATIC): $(OBJS_LIBCHECKER)
	@ echo "   AR   $@"
	@ $(AR) rcs $@ $<

$(TARGET_RUNSBOX): $(LIBSBOX_SHARED) $(OBJS_RUNSBOX)
	@ echo "   LD   $@"
	@ $(CC) $(CFLAGS) -L. $(OBJS_RUNSBOX) $(RUNSBOX_LDFLAGS) $(LDFLAGS) -o $(TARGET_RUNSBOX)

%.c.o: %.c
	@ echo "   CC   $@"
	@ $(CC) $(CFLAGS) -c $< -o $@

%.cpp.o: %.cpp
	@ echo "   CXX  $@"
	@ $(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET_TESTS_C): %$(EXECUTABLE_SUFFIX) : %.c.o
	@ echo "   LD   $@"
	@ $(CC) $(CFLAGS) $(basename $@).c.o $(LDFLAGS) -o $@

$(TARGET_TESTS_CXX): %$(EXECUTABLE_SUFFIX) : %.cpp.o
	@ echo "   LD   $@"
	@ $(CXX) $(CXXFLAGS) $(basename $@).cpp.o $(LDFLAGS) -o $@

$(TARGET_CHECKERS_C): %$(EXECUTABLE_SUFFIX) : %.c.o $(LIBCHECKER_SHARED) 
	@ echo "   LD   $@"
	@ $(CC) $(CFLAGS) -L. $(basename $@).c.o $(CHECKER_LDFLAGS) $(LDFLAGS) -o $@

$(TARGET_CHECKERS_CXX): %$(EXECUTABLE_SUFFIX) : %.cpp.o $(LIBCHECKER_SHARED) 
	@ echo "   LD   $@"
	@ $(CXX) $(CXXFLAGS) -L. $(basename $@).cpp.o $(CHECKER_LDFLAGS) $(LDFLAGS) -o $@

install: 
	-@ for bin in $(INSTALL_BIN); do test -e $$bin && echo "  BIN   $$bin" && install -m755 -D $$bin $(DESTDIR)$(BINDIR)/$$bin; done
	-@ for lib in $(INSTALL_LIB); do test -e $$lib && echo "  LIB   $$lib" && install -m755 -D $$lib $(DESTDIR)$(LIBDIR)/$$lib; done
	-@ for header in $(INSTALL_HEADERS); do test -e $$header && echo "  HDR   $$header" && install -m644 -D $$header $(DESTDIR)$(INCLUDEDIR)/$$header; done
	-@ for checker in $(TARGET_CHECKERS); do test -e $$checker && echo "  BIN   $$checker" && install -m755 -D $$checker $(DESTDIR)$(LIBDIR)/sistem/$$checker; done
	-@ for man in $(INSTALL_MAN1); do test -e man/$$man && echo "  MAN   $$man" && install -m755 -D man/$$man $(DESTDIR)$(MANDIR)/man1/$$man; done

uninstall:
	@ for bin in $(INSTALL_BIN); do echo " RM BIN $$bin" && rm -f $(DESTDIR)$(BINDIR)/$$bin; done
	@ for lib in $(INSTALL_LIB); do echo " RM LIB $$lib" && rm -f $(DESTDIR)$(LIBDIR)/$$lib; done
	@ for header in $(INSTALL_HEADERS); do echo " RM HDR $$header" && rm -f $(DESTDIR)$(INCLUDEDIR)/$$header; done
	@ for checker in $(TARGET_CHECKERS); do echo " RM BIN $$checker" && rm -f $(DESTDIR)$(LIBDIR)/sistem/$$checker; done

clean-libsbox:
	@ echo " CLEAN  $(TARGET_LIBSBOX)"
	@ rm -f $(OBJS_LIBSBOX)
	@ rm -f $(TARGET_LIBSBOX_ALL)
clean-runsbox:
	@ echo " CLEAN  $(TARGET_RUNSBOX)"
	@ rm -f $(OBJS_RUNSBOX)
	@ rm -f $(TARGET_RUNSBOX)
clean-libchecker:
	@ echo " CLEAN  $(TARGET_LIBCHECKER)"
	@ rm -f $(TARGET_LIBCHECKER_ALL)
	@ rm -f $(OBJS_LIBCHECKER)
clean-tests:
	@ echo " CLEAN  tests"
	@ rm -f $(TARGET_TESTS)
	@ rm -f $(OBJS_TESTS)
	@ cd tests && for test in $(TESTS); do rm -f $$test/{test,given_output,given_exitcode}; done
clean-checkers:
	@ echo " CLEAN  checkers"
	@ rm -f $(TARGET_CHECKERS)
	@ rm -f $(OBJS_CHECKERS)

clean: clean-libsbox clean-runsbox clean-libchecker clean-tests clean-checkers

check: $(TARGET_RUNSBOX) $(TARGET_TESTS)
	@ cd tests && for test in $(TESTS); do bash test-runner.sh $$test || exit 1; done

check-all: $(TARGET_RUNSBOX) $(TARGET_TESTS)
	@ cd tests && for test in $(TESTS); do bash test-runner.sh $$test; done

help:
	@ echo "List of targets:"; LC_ALL=C $(MAKE) -pRrq -f $(lastword $(MAKEFILE_LIST)) : 2>/dev/null | awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | sort | egrep -v -e '^[^[:alnum:]]' -e '^$@$$' | xargs | sed 's/ /\n/g'

.PHONY: all clean install uninstall check help clean-libsbox clean-runsbox clean-libchecker clean-tests clean-checkers tests checkers
