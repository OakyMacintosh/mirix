# MIRIX Configuration Definitions
# This file contains common configuration variables for all Makefiles

# Architecture detection
ARCH ?= $(shell uname -m)
PLATFORM ?= $(shell uname -s | tr '[:upper:]' '[:lower:]')

# Build configuration
DEBUG ?= 0
OPTIMIZE ?= 2
BUILD_TYPE ?= release

# Directories
SRCDIR = mirix
HOSTDIR = mirix/host
IPCDIR = mirix/ipc
SYSCALLDIR = mirix/syscall
POSIXDIR = mirix/posix
DRIVERDIR = mirix/drivers
LIBSYSDIR = mirix/libsystem
LIBSYSCALLDIR = mirix/libsyscall
LIBCDIR = mirix/libc
BSDIR = bsd
ARCHDIR = arch
MNCDIR = mirix/mnc
MACHDIR = mach
TOOLSDIR = tools
HOSTCONFIGDIR = host

# Build directories
BUILDDIR = build
BUILD_SUFFIX = 
ifeq ($(DEBUG), 1)
    BUILD_SUFFIX = -DEBUG
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O$(OPTIMIZE) -DNDEBUG
endif

# Compiler and linker configuration
CC = gcc
CXX = g++
LD = ld
AR = ar
RANLIB = ranlib
STRIP = strip

# Common compiler flags
CFLAGS += -std=c99 -Wall -Wextra -fno-strict-aliasing
CXXFLAGS += -std=c++11 -Wall -Wextra -fno-strict-aliasing
LDFLAGS = 
ARFLAGS = rcs

# Platform-specific flags
ifeq ($(PLATFORM), darwin)
    CFLAGS += -D__APPLE__ -D_DARWIN_C_SOURCE
    LDFLAGS += -framework CoreFoundation -framework IOKit
else ifeq ($(PLATFORM), linux)
    CFLAGS += -D__linux__ -D_GNU_SOURCE
    LDFLAGS += -lpthread -ldl
else ifeq ($(PLATFORM), freebsd)
    CFLAGS += -D__FreeBSD__ -D_BSD_SOURCE
    LDFLAGS += -lpthread -lkvm
else ifeq ($(PLATFORM), netbsd)
    CFLAGS += -D__NetBSD__ -D_BSD_SOURCE
    LDFLAGS += -lpthread
else ifeq ($(PLATFORM), openbsd)
    CFLAGS += -D__OpenBSD__ -D_BSD_SOURCE
    LDFLAGS += -lpthread
endif

# Architecture-specific flags
ifeq ($(ARCH), x86_64)
    CFLAGS += -m64
    LDFLAGS += -m64
else ifeq ($(ARCH), i386)
    CFLAGS += -m32
    LDFLAGS += -m32
else ifeq ($(ARCH), arm64)
    CFLAGS += -march=armv8-a
else ifeq ($(ARCH), aarch64)
    CFLAGS += -march=armv8-a
endif

# MIRIX-specific flags
CFLAGS += -DMIRIX_KERNEL=1 -DMIRIX_USERSPACE=1
CFLAGS += -I$(SRCDIR) -I$(MACHDIR)/include

# Cross-compilation support
ifdef CROSS_COMPILE
    CC = $(CROSS_COMPILE)-gcc
    CXX = $(CROSS_COMPILE)-g++
    LD = $(CROSS_COMPILE)-ld
    AR = $(CROSS_COMPILE)-ar
    RANLIB = $(CROSS_COMPILE)-ranlib
    STRIP = $(CROSS_COMPILE)-strip
endif

# Target triple for cross-compilation
TARGET_TRIPLE ?= $(ARCH)-unknown-sysv-mirix1

# Version information
MIRIX_VERSION = 0.1
MIRIX_MAJOR_VERSION = 0
MIRIX_MINOR_VERSION = 1
MIRIX_PATCH_VERSION = 0

# Package configuration
PACKAGE_NAME = mirix
PACKAGE_VERSION = $(MIRIX_VERSION)
PACKAGE_TARNAME = $(PACKAGE_NAME)-$(PACKAGE_VERSION)

# Installation directories
PREFIX ?= /usr/local
EXEC_PREFIX = $(PREFIX)
BINDIR = $(EXEC_PREFIX)/bin
SBINDIR = $(EXEC_PREFIX)/sbin
LIBEXECDIR = $(EXEC_PREFIX)/libexec
DATAROOTDIR = $(PREFIX)/share
DATADIR = $(DATAROOTDIR)
SYSCONFDIR = $(PREFIX)/etc
SHAREDSTATEDIR = $(PREFIX)/com
LOCALSTATEDIR = $(PREFIX)/var
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(EXEC_PREFIX)/lib
LIBEXECDIR = $(EXEC_PREFIX)/libexec

# Documentation directories
DOCDIR = $(DATAROOTDIR)/doc/$(PACKAGE_TARNAME)
INFODIR = $(DATAROOTDIR)/info
HTMLDIR = $(DATAROOTDIR)/doc/$(PACKAGE_TARNAME)
DVIDIR = $(DATAROOTDIR)/doc/$(PACKAGE_TARNAME)
PDFDIR = $(DATAROOTDIR)/doc/$(PACKAGE_TARNAME)
PSDIR = $(DATAROOTDIR)/doc/$(PACKAGE_TARNAME)

# Build tools
MAKE = make
MKDIR = mkdir -p
RM = rm -f
RMDIR = rmdir
CP = cp -r
MV = mv
LN = ln -s
TAR = tar
GZIP = gzip
BZIP2 = bzip2
XZ = xz

# File extensions
OBJEXT = o
LIBEXT = a
EXEEXT = 

# Platform-specific file extensions
ifeq ($(PLATFORM), winnt)
    EXEEXT = .exe
    LIBEXT = .lib
    OBJEXT = obj
endif

# Default target
all:

# Clean target
clean-build:
	$(RM) -r $(BUILDDIR)

# Install target
install: all

# Uninstall target
uninstall:

# Distribution target
dist:

# Distribution clean target
distclean: clean-build

# Check target
check:

# Phony targets
.PHONY: all clean-build install uninstall dist distclean check
