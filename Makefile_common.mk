# Copyright (c) 1994-2005 by Wayne C. Gramlich
# All rights reserved

ROOT_DIRECTORY := $(shell cd $(ROOT); pwd)

ARCH := $(shell uname -m | sed "s/.*86/x86/g")
ARCH_COPTS_sparc := -DSPARC
ARCH_COPTS_x86 := -DX86
ARCH_COPTS := ${ARCH_COPTS_$(ARCH)}
OS := $(shell uname -s | sed -e "s/Linux/linux/g" -e "s/SunOS/solaris/g")
OS_COPTS_linux := -DLINUX
OS_COPTS_solaris := -DSOLARIS
OS_COPTS := ${OS_COPTS_$(OS)}
OS_LDOPTS_X11_linux := -L/usr/X11R6/lib
OS_LDOPTS_X11_solaris := -L/usr/openwin/lib
OS_LDOPTS_X11 := ${OS_LDOPTS_X11_$(OS)}
PLATFORM := $(OS)-$(ARCH)
CC := gcc
CXX := g++
COPTS := -O2
$(CFLAGS_SUPRESS)CFLAGS := ${COPTS} -DLINUX
$(WINDOWS_CFLAGS_SUPRESS)WINDOWS_CFLAGS := \
  ${COPTS} -DWINDOWS
STC_OPTIONS :=

# Cross compiler stuff:
MINGW_DIRECTORY := \
  $(ROOT_DIRECTORY)/mingw/install/i386-mingw32msvc
MINGW_BIN_DIRECTORY := $(MINGW_DIRECTORY)/bin
WINDOWS_PLATFORM := win-x86
WINDOWS_CC := $(MINGW_BIN_DIRECTORY)/gcc
WINDOWS_CXX := $(MINGW_BIN_DIRECTORY)/g++
WINDOWS_AR := $(MINGW_BIN_DIRECTORY)/ar
WINDOWS_RANLIB := $(MINGW_BIN_DIRECTORY)/ranlib
PLATFORM := linux-x86
LINUX_PLATFORM := linux-x86
PLATFORMS := \
  $(LINUX_PLATFORM) \
  $(WINDOWS_PLATFORM)

CLEAN=-rm -f core *~

$(LINUX_PLATFORM)/%.o: %.c
	$(CC) -o $@ -c ${CFLAGS} $<

$(LINUX_PLATFORM)/%.o: %.cpp
	$(CXX) -o $@ -c ${CFLAGS} $<

$(PLATFORM)/%.sti: %.sts
	$(STC) -i -E $(PLATFORM) $<

$(PLATFORM)/%.o: %.sts
	$(STC) -c -E $(PLATFORM) ${STC_FLAGS} $<

# Cross platform development (for Windows):

$(WINDOWS_PLATFORM)/%.o: %.c
	$(WINDOWS_CC) -o $@ -c $(WINDOWS_CFLAGS) $<

$(WINDOWS_PLATFORM)/%.o: %.cpp
	$(WINDOWS_CXX) -o $@ -c $(WINDOWS_CFLAGS) $<

$(WINDOWS_PLATFORM)/%.sti: %.sts
	$(STC) -i -E $(WINDOWS_PLATFORM) $<

$(WINDOWS_PLATFORM)/%.o: %.sts
	$(STC) -v -c -W -E $(WINDOWS_PLATFORM) \
  -w $(WINDOWS_CC) ${STC_FLAGS} $<

%.html: %.sts
	$(NSTC) -m html -o $@ $<

all:

$(LINUX_PLATFORM):
	rm -rf $@; mkdir $@

$(WINDOWS_PLATFORM):
	rm -rf $@; mkdir $@
