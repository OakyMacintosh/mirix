# Aqua Kernel Makefile

# Include aqua build definitions FIRST
MIRIX_ROOT = $(CURDIR)
include $(MIRIX_ROOT)/Makedefs/Config.mk
include $(MIRIX_ROOT)/Makedefs/Rules.mk

# Mach integration
ifdef MACH_KERNEL
    include $(MIRIX_ROOT)/Makedefs/Mach.mk
    MACH_KERNEL_INTEGRATION = 1
endif

ifdef MACH_USERSPACE
    include $(MIRIX_ROOT)/Makedefs/Mach.mk
    MACH_USERSPACE_INTEGRATION = 1
endif

# Compiler and tools
CC = clang
LD = ld
CFLAGS = -Wall -Wextra -std=c99 -g -O2 -D_GNU_SOURCE -Wno-implicit-function-declaration -I./ -DMIRIX_PLATFORM_DOS
LDFLAGS =
HOST_DOS_INCLUDES = -Ihost/dos/include
HOST_DOS_PTHREAD_INCLUDES = -Ihost/dos/aed/pthread

# Source directories
SRCDIR = mirix
HOSTDIR = mirix/host
IPCDIR = mirix/ipc
SYSCALLDIR = mirix/syscall
POSIXDIR = mirix/posix
DRIVERDIR = mirix/drivers
LIBSYSDIR = mirix/libsystem
LIBSYSCALLDIR = mirix/libsyscall
LIBCDIR = mirix/libc
MNCDIR = mirix/mnc
BSDIR = bsd
ARCHDIR = arch
LOADERDIR = mirix/loader
BUILDDIR = build

BUILDTYPE ?= release
BUILDSUFFIX = $(BUILDTYPE)

# Architecture detection
ARCH ?= $(shell uname -m)
PLATFORM ?= $(shell uname -s)

# Build configuration
DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -g -O0 -DDEBUG
    BUILD_SUFFIX = -DEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

# Architecture-specific sources
ifeq ($(ARCH), x86_64)
    ARCH_SOURCES = $(ARCHDIR)/x86_64/arch.c
    ARCH_BUILD_DIR = $(BUILDDIR)/arch/x86_64
else ifeq ($(ARCH), i386)
    ARCH_SOURCES = $(ARCHDIR)/i386/nonunix.c
    ARCH_BUILD_DIR = $(BUILDDIR)/arch/i386
else
    ARCH_SOURCES = $(ARCHDIR)/generic/arch.c
    ARCH_BUILD_DIR = $(BUILDDIR)/arch/generic
endif

# Source files
KERNEL_SOURCES = \
	$(SRCDIR)/kernel.c \
	$(SRCDIR)/main.c \
	$(SRCDIR)/scheduler.c \
	$(SRCDIR)/kernel_args.c

HOST_SOURCES = \
	$(HOSTDIR)/host_interface.c

DOS_PTHREAD_DIR = host/dos/aed/pthread
DOS_PTHREAD_SOURCES = \
	$(DOS_PTHREAD_DIR)/dosthread.c

DOS_COMPAT_SOURCES = \
	host/dos/dos_compat.c

ENABLE_DOS_SUPPORT ?= 0

ifeq ($(ENABLE_DOS_SUPPORT),0)
DOS_PTHREAD_SOURCES =
DOS_COMPAT_SOURCES =
endif

IPC_SOURCES = \
	$(IPCDIR)/ipc.c

SYSCALL_SOURCES = \
	$(SYSCALLDIR)/syscall.c \
	$(SYSCALLDIR)/syscall_wrappers.c

POSIX_SOURCES = \
	$(POSIXDIR)/posix.c \
	$(POSIXDIR)/sus_simple.c

# DRIVER SOURCES
DRIVER_SOURCES = \
	$(DRIVERDIR)/lazyfs.c

MODULE_DIR = modules
MODULE_SOURCES = \
	$(MODULE_DIR)/registry.c \
	$(MODULE_DIR)/dos_personality.c

LIBDIST_DIR = libdist
LIBDIST_SOURCES = $(LIBDIST_DIR)/libdist.c

LIBSYS_SOURCES = \
	$(LIBSYSDIR)/libsystem.c

LIBSYSCALL_SOURCES = \
	$(LIBSYSCALLDIR)/libsyscall.c

LIBC_SOURCES = \
	$(SRCDIR)/libc/mirix_libc.c

BSD_SOURCES = \
	$(BSDIR)/bsd_syscalls.c \
	$(BSDIR)/bsd_proc.c

LOADER_SOURCES = \
	$(LOADERDIR)/aout_loader.c

MNC_SOURCES = \
	$(MNCDIR)/mnc_parser.c \
	$(MNCDIR)/mnc_compiler.c

# All sources
ALL_SOURCES = $(KERNEL_SOURCES) $(HOST_SOURCES) $(IPC_SOURCES) $(SYSCALL_SOURCES) $(POSIX_SOURCES) $(DRIVER_SOURCES) $(LIBSYS_SOURCES) $(LIBSYSCALL_SOURCES) $(LIBC_SOURCES) $(BSD_SOURCES) $(ARCH_SOURCES) $(MNC_SOURCES) $(DOS_PTHREAD_SOURCES) $(DOS_COMPAT_SOURCES) $(MODULE_SOURCES) $(LOADER_SOURCES)

# Object files
KERNEL_OBJECTS = $(KERNEL_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/$(SRCDIR)/%.o)
HOST_OBJECTS = $(HOST_SOURCES:$(HOSTDIR)/%.c=$(BUILDDIR)/$(HOSTDIR)/%.o)
IPC_OBJECTS = $(IPC_SOURCES:$(IPCDIR)/%.c=$(BUILDDIR)/$(IPCDIR)/%.o)
SYSCALL_OBJECTS = $(SYSCALL_SOURCES:$(SYSCALLDIR)/%.c=$(BUILDDIR)/$(SYSCALLDIR)/%.o)
POSIX_OBJECTS = $(POSIX_SOURCES:$(POSIXDIR)/%.c=$(BUILDDIR)/$(POSIXDIR)/%.o)
DRIVER_OBJECTS = $(DRIVER_SOURCES:$(DRIVERDIR)/%.c=$(BUILDDIR)/$(DRIVERDIR)/%.o)
LIBSYS_OBJECTS = $(LIBSYS_SOURCES:$(LIBSYSDIR)/%.c=$(BUILDDIR)/$(LIBSYSDIR)/%.o)
LIBSYSCALL_OBJECTS = $(LIBSYSCALL_SOURCES:$(LIBSYSCALLDIR)/%.c=$(BUILDDIR)/$(LIBSYSCALLDIR)/%.o)
LIBC_OBJECTS = $(LIBC_SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/$(SRCDIR)/%.o)
BSD_OBJECTS = $(BSD_SOURCES:$(BSDIR)/%.c=$(BUILDDIR)/$(BSDIR)/%.o)
ARCH_OBJECTS = $(ARCH_SOURCES:$(ARCHDIR)/%.c=$(BUILDDIR)/$(ARCHDIR)/%.o)
MNC_OBJECTS = $(MNC_SOURCES:$(MNCDIR)/%.c=$(BUILDDIR)/$(MNCDIR)/%.o)
DOS_PTHREAD_OBJECTS = $(DOS_PTHREAD_SOURCES:%.c=$(BUILDDIR)/%.o)
DOS_COMPAT_OBJECTS = $(DOS_COMPAT_SOURCES:%.c=$(BUILDDIR)/%.o)

MODULE_OBJECTS = $(MODULE_SOURCES:$(MODULE_DIR)/%.c=$(BUILDDIR)/$(MODULE_DIR)/%.o)
LIBDIST_OBJECTS = $(LIBDIST_SOURCES:$(LIBDIST_DIR)/%.c=$(BUILDDIR)/$(LIBDIST_DIR)/%.o)
LOADER_OBJECTS = $(LOADER_SOURCES:$(LOADERDIR)/%.c=$(BUILDDIR)/$(LOADERDIR)/%.o)

OBJECTS = $(KERNEL_OBJECTS) $(HOST_OBJECTS) $(IPC_OBJECTS) $(SYSCALL_OBJECTS) $(POSIX_OBJECTS) $(DRIVER_OBJECTS) $(LIBSYS_OBJECTS) $(LIBSYSCALL_OBJECTS) $(LIBC_OBJECTS) $(BSD_OBJECTS) $(ARCH_OBJECTS) $(MNC_OBJECTS) $(DOS_PTHREAD_OBJECTS) $(DOS_COMPAT_OBJECTS) $(MODULE_OBJECTS) $(LOADER_OBJECTS)

# Target executable
TARGET = $(BUILDDIR)/aqua_kernel$(BUILD_SUFFIX)

# M&C test executable
MNC_TEST = $(BUILDDIR)/test-mnc

# Default target and all targets
all: $(TARGET) $(MNC_TEST)

# Mach targets
mach:
	@echo "Building Mach components..."
	$(MAKE) -C mach

mach-kernel:
	@echo "Building Mach kernel integration..."
	$(MAKE) -C mach kernel-integration

mach-userspace:
	@echo "Building Mach userspace integration..."
	$(MAKE) -C mach userspace-integration

# Build with Mach kernel integration
all-mach-kernel:
	$(MAKE) MACH_KERNEL=1 all
	$(MAKE) mach-kernel

# Build with Mach userspace integration
all-mach-userspace:
	$(MAKE) MACH_USERSPACE=1 all
	$(MAKE) mach-userspace

# Build with full Mach integration
all-mach:
	$(MAKE) MACH_KERNEL=1 MACH_USERSPACE=1 all
	$(MAKE) mach

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)
	mkdir -p $(BUILDDIR)/$(SRCDIR)
	mkdir -p $(BUILDDIR)/$(HOSTDIR)
	mkdir -p $(BUILDDIR)/$(IPCDIR)
	mkdir -p $(BUILDDIR)/$(SYSCALLDIR)
	mkdir -p $(BUILDDIR)/$(POSIXDIR)
	mkdir -p $(BUILDDIR)/$(DRIVERDIR)
	mkdir -p $(BUILDDIR)/$(LIBSYSDIR)
	mkdir -p $(BUILDDIR)/$(LIBSYSCALLDIR)
	mkdir -p $(BUILDDIR)/$(SRCDIR)/libc
	mkdir -p $(BUILDDIR)/$(MNCDIR)
	mkdir -p $(BUILDDIR)/$(BSDIR)
	mkdir -p $(BUILDDIR)/$(ARCHDIR)
	mkdir -p $(ARCH_BUILD_DIR)
	mkdir -p $(BUILDDIR)/$(ARCHDIR)/x86_64
	mkdir -p $(BUILDDIR)/$(ARCHDIR)/i386
	mkdir -p $(BUILDDIR)/$(ARCHDIR)/generic
	mkdir -p $(BUILDDIR)/$(LOADERDIR)
	mkdir -p $(BUILDDIR)/host/dos/aed/pthread
	mkdir -p $(BUILDDIR)/host/dos
	mkdir -p $(BUILDDIR)/modules
	mkdir -p $(BUILDDIR)/libdist

# Build the main executable
$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Built MIRIX kernel: $@"

# Build M&C test executable
$(MNC_TEST): $(BUILDDIR)/$(MNCDIR)/test_mnc.o $(BUILDDIR)/$(MNCDIR)/mnc_parser.o $(BUILDDIR)/$(MNCDIR)/mnc_compiler.o | $(BUILDDIR)
	$(CC) $(BUILDDIR)/$(MNCDIR)/test_mnc.o $(BUILDDIR)/$(MNCDIR)/mnc_parser.o $(BUILDDIR)/$(MNCDIR)/mnc_compiler.o -o $@
	@echo "Built M&C test: $@"

libdist: $(LIBDIST_OBJECTS)
	@echo "Built libdist helper: $(LIBDIST_OBJECTS)"

$(BUILDDIR)/host/dos/dos_compat.o: host/dos/dos_compat.c | $(BUILDDIR)
	@mkdir -p $(dir $@)
	$(CC) $(HOST_DOS_INCLUDES) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/host/dos/aed/pthread/dosthread.o: host/dos/aed/pthread/dosthread.c | $(BUILDDIR)
	@mkdir -p $(dir $@)
	$(CC) $(HOST_DOS_INCLUDES) $(HOST_DOS_PTHREAD_INCLUDES) $(CFLAGS) -c $< -o $@

# Compile source files
$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile module sources
$(BUILDDIR)/$(MODULE_DIR)/%.o: $(MODULE_DIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile libdist helper
$(BUILDDIR)/$(LIBDIST_DIR)/%.o: $(LIBDIST_DIR)/%.c | $(BUILDDIR)/$(LIBDIST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies
$(BUILDDIR)/$(SRCDIR)/main.o: $(SRCDIR)/kernel.h $(SRCDIR)/kernel_args.h
$(BUILDDIR)/$(SRCDIR)/scheduler.o: $(SRCDIR)/kernel.h
$(BUILDDIR)/$(SRCDIR)/kernel_args.o: $(SRCDIR)/kernel_args.h
$(BUILDDIR)/$(HOSTDIR)/host_interface.o: $(HOSTDIR)/host_interface.h
$(BUILDDIR)/$(IPCDIR)/ipc.o: $(IPCDIR)/ipc.h
$(BUILDDIR)/$(SYSCALLDIR)/syscall.o: $(SYSCALLDIR)/syscall.h
$(BUILDDIR)/$(POSIXDIR)/posix.o: $(POSIXDIR)/posix.h $(POSIXDIR)/sus_simple.h $(SYSCALLDIR)/syscall.h
$(BUILDDIR)/$(POSIXDIR)/sus_simple.o: $(POSIXDIR)/sus_simple.h $(SYSCALLDIR)/syscall.h
$(BUILDDIR)/$(DRIVERDIR)/lazyfs.o: $(DRIVERDIR)/lazyfs.h
$(BUILDDIR)/$(LIBSYSDIR)/libsystem.o: $(LIBSYSDIR)/libsystem.h
$(BUILDDIR)/$(LIBSYSCALLDIR)/libsyscall.o: $(LIBSYSCALLDIR)/libsyscall.h
$(BUILDDIR)/$(SRCDIR)/libc/mirix_libc.o: $(SRCDIR)/libc/mirix_libc.h
$(BUILDDIR)/$(BSDIR)/bsd_syscalls.o: $(BSDIR)/bsd_syscalls.h
$(BUILDDIR)/$(ARCHDIR)/x86_64/arch.o: $(ARCHDIR)/x86_64/arch.h
$(BUILDDIR)/$(ARCHDIR)/i386/nonunix.o: $(ARCHDIR)/i386/nonunix.h
$(BUILDDIR)/$(MNCDIR)/mnc_parser.o: $(MNCDIR)/mnc_parser.h
$(BUILDDIR)/$(MNCDIR)/mnc_compiler.o: $(MNCDIR)/mnc_compiler.h $(MNCDIR)/mnc_parser.h
$(BUILDDIR)/$(MNCDIR)/test_mnc.o: $(MNCDIR)/mnc_parser.h $(MNCDIR)/mnc_compiler.h
$(BUILDDIR)/host/dos/aed/pthread/dosthread.o: host/dos/aed/pthread/dosthread.c host/dos/aed/pthread/dosthread.h
$(BUILDDIR)/host/dos/dos_compat.o: host/dos/dos_compat.c
$(BUILDDIR)/modules/registry.o: modules/registry.c modules/module_registry.h modules/dos_personality.h
$(BUILDDIR)/modules/dos_personality.o: modules/dos_personality.c modules/dos_personality.h
$(BUILDDIR)/mirix/loader/aout_loader.o: mirix/loader/aout_loader.c mirix/loader/aout_loader.h

# Clean build artifacts
clean:
	$(RM) -r $(BUILDDIR)

# Install target
install: all

# Uninstall target
uninstall:

# Distribution target
dist:

# Distribution clean target
distclean: clean

# Help target
help:
	@echo "MIRIX Build System"
	@echo "=================="
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build all targets"
	@echo "  clean            - Remove build artifacts"
	@echo "  install          - Install built files"
	@echo "  uninstall        - Remove installed files"
	@echo "  dist             - Create source distribution"
	@echo "  help             - Show this help"
	@echo ""
	@echo "Mach targets:"
	@echo "  mach             - Build Mach components"
	@echo "  mach-kernel      - Build Mach kernel integration"
	@echo "  mach-userspace   - Build Mach userspace integration"
	@echo "  all-mach-kernel  - Build with Mach kernel support"
	@echo "  all-mach-userspace - Build with Mach userspace support"
	@echo "  all-mach         - Build with full Mach support"
	@echo ""
	@echo "Configuration:"
	@echo "  ARCH             - Target architecture ($(ARCH))"
	@echo "  PLATFORM         - Host platform ($(PLATFORM))"
	@echo "  DEBUG            - Enable debug (0/1, current: $(DEBUG))"
	@echo "  MACH_KERNEL      - Enable Mach kernel integration"
	@echo "  MACH_USERSPACE   - Enable Mach userspace integration"

# Phony targets
.PHONY: all clean install uninstall dist distclean help
.PHONY: mach mach-kernel mach-userspace all-mach-kernel all-mach-userspace all-mach
