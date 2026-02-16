# MIRIX Mach Integration Rules
# This file contains rules for building and integrating Mach components

# Mach directories
MACH_SRCDIR = $(MACHDIR)
MACH_INCDIR = $(MACHDIR)/include
MACH_LIBDIR = $(MACHDIR)/libmach
MACH_THREADSDIR = $(MACHDIR)/libthreads
MACH_MIGDIR = $(MACHDIR)/mig
MACH_BOOTDIR = $(MACHDIR)/bootstrap

# Mach build directories
MACH_BUILDDIR = $(BUILDDIR)/mach
MACH_LIBBUILDDIR = $(MACH_BUILDDIR)/libmach
MACH_THREADSBUILDDIR = $(MACH_BUILDDIR)/libthreads
MIG_BUILDDIR = $(MACH_BUILDDIR)/mig

# Mach compiler flags
MACH_CFLAGS = $(CFLAGS) -I$(MACH_INCDIR) -I$(MACH_INCDIR)/mach
MACH_CFLAGS += -DMACH_KERNEL=1 -DMACH_USERSPACE=1
MACH_CFLAGS += -D__MACH__=1

# Mach library flags
MACH_LIBS = -lmach -lthreads
MACH_LDFLAGS = $(LDFLAGS) -L$(MACH_LIBBUILDDIR) -L$(MACH_THREADSBUILDDIR)

# Mach source files
MACH_SOURCES = \
	$(MACH_LIBDIR)/c/mach_error.c \
	$(MACH_LIBDIR)/c/mach_init.c \
	$(MACH_LIBDIR)/c/mach_msg.c \
	$(MACH_LIBDIR)/c/mach_msg_destroy.c \
	$(MACH_LIBDIR)/c/mach_msg_receive.c \
	$(MACH_LIBDIR)/c/mach_msg_send.c \
	$(MACH_LIBDIR)/c/mach_msg_server.c \
	$(MACH_LIBDIR)/c/mig_allocate.c \
	$(MACH_LIBDIR)/c/mig_deallocate.c \
	$(MACH_LIBDIR)/c/mig_reply_setup.c \
	$(MACH_LIBDIR)/c/mig_strncpy.c \
	$(MACH_LIBDIR)/c/mig_support.c \
	$(MACH_LIBDIR)/c/slot_name.c

MACH_THREAD_SOURCES = \
	$(MACH_THREADSDIR)/c_thread.c \
	$(MACH_THREADSDIR)/cthreads.c \
	$(MACH_THREADSDIR)/swt_thread.c

# Mach interface definition files
MACH_DEFS = \
	$(MACH_INCDIR)/mach/mach_port.defs \
	$(MACH_INCDIR)/mach/mach_host.defs \
	$(MACH_INCDIR)/mach/mach_interface.defs \
	$(MACH_INCDIR)/mach/mach4_interface.defs \
	$(MACH_INCDIR)/mach/mach_debug.defs \
	$(MACH_INCDIR)/mach/exc.defs \
	$(MACH_INCDIR)/mach/notify_user.defs \
	$(MACH_INCDIR)/mach/memory_object_default.defs \
	$(MACH_INCDIR)/mach/memory_object_user.defs \
	$(MACH_INCDIR)/mach/device.defs \
	$(MACH_INCDIR)/mach/device_reply.defs \
	$(MACH_INCDIR)/mach/device_request.defs \
	$(MACH_INCDIR)/mach/default_pager.defs \
	$(MACH_INCDIR)/mach/default_pager_helper.defs

# Mach object files
MACH_OBJECTS = $(MACH_SOURCES:%.c=$(MACH_LIBBUILDDIR)/%.o)
MACH_THREAD_OBJECTS = $(MACH_THREAD_SOURCES:%.c=$(MACH_THREADSBUILDDIR)/%.o)

# Mach libraries
MACH_LIB = $(MACH_LIBBUILDDIR)/libmach.a
MACH_THREAD_LIB = $(MACH_THREADSBUILDDIR)/libthreads.a

# Mach headers
MACH_HEADERS = \
	$(MACH_INCDIR)/mach.h \
	$(MACH_INCDIR)/mach/mach.h \
	$(MACH_INCDIR)/mach/mach_error.h \
	$(MACH_INCDIR)/mach/mach_init.h \
	$(MACH_INCDIR)/mach/mach_msg.h \
	$(MACH_INCDIR)/mach/mach_port.h \
	$(MACH_INCDIR)/mach/mach_host.h \
	$(MACH_INCDIR)/mach/mach_interface.h \
	$(MACH_INCDIR)/mach/mach_types.h \
	$(MACH_INCDIR)/mach/mach_traps.h \
	$(MACH_INCDIR)/mach/mach_vm.h \
	$(MACH_INCDIR)/mach/thread_status.h \
	$(MACH_INCDIR)/mach/thread_switch.h

# Build Mach library
$(MACH_LIB): $(MACH_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

# Build Mach threads library
$(MACH_THREAD_LIB): $(MACH_THREAD_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

# Build Mach object files
$(MACH_LIBBUILDDIR)/%.o: $(MACH_LIBDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(MACH_CFLAGS) -c $< -o $@

$(MACH_THREADSBUILDDIR)/%.o: $(MACH_THREADSDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(MACH_CFLAGS) -c $< -o $@

# Generate Mach interfaces with MIG
$(MACH_INCDIR)/mach/%.h: $(MACH_INCDIR)/mach/%.defs
	@if command -v mig >/dev/null 2>&1; then \
		echo "Generating Mach interface: $<"; \
		mig -user $(MACH_LIBBUILDDIR)/$*.c -server $(MACH_LIBBUILDDIR)/$*_server.c -header $@ $<; \
	else \
		echo "MIG not found, creating stub header: $@"; \
		echo "// Stub header generated because MIG not available" > $@; \
		echo "#ifndef _$(shell echo $* | tr '[:lower:]' '[:upper:]')_H_" >> $@; \
		echo "#define _$(shell echo $* | tr '[:lower:]' '[:upper:]')_H_" >> $@; \
		echo "#endif /* _$(shell echo $* | tr '[:lower:]' '[:upper:]')_H_ */" >> $@; \
	fi

$(MACH_LIBBUILDDIR)/%.c: $(MACH_INCDIR)/mach/%.defs
	@if command -v mig >/dev/null 2>&1; then \
		echo "Generating Mach user stub: $<"; \
		mig -user $@ -server $(MACH_LIBBUILDDIR)/$*_server.c -header $(MACH_INCDIR)/mach/$*.h $<; \
	else \
		echo "MIG not found, creating stub implementation: $@"; \
		echo "// Stub implementation generated because MIG not available" > $@; \
		echo "#include \"$(MACH_INCDIR)/mach/$*.h\"" >> $@; \
	fi

$(MACH_LIBBUILDDIR)/%_server.c: $(MACH_INCDIR)/mach/%.defs
	@if command -v mig >/dev/null 2>&1; then \
		echo "Generating Mach server stub: $<"; \
		mig -server $@ -user $(MACH_LIBBUILDDIR)/$*.c -header $(MACH_INCDIR)/mach/$*.h $<; \
	else \
		echo "MIG not found, creating stub server: $@"; \
		echo "// Stub server generated because MIG not available" > $@; \
		echo "#include \"$(MACH_INCDIR)/mach/$*.h\"" >> $@; \
	fi

# Mach integration with kernel
MACH_KERNEL_OBJS = $(MACH_OBJECTS) $(MACH_THREAD_OBJECTS)
MACH_KERNEL_LIBS = $(MACH_LIB) $(MACH_THREAD_LIB)

# Add Mach to kernel build
KERNEL_OBJECTS += $(MACH_KERNEL_OBJS)
KERNEL_LIBS += $(MACH_KERNEL_LIBS)
KERNEL_CFLAGS += $(MACH_CFLAGS)
KERNEL_LDFLAGS += $(MACH_LDFLAGS)

# Mach userspace integration
MACH_USERSPACE_OBJS = $(MACH_OBJECTS)
MACH_USERSPACE_LIBS = $(MACH_LIB)

# Add Mach to userspace build
USERSPACE_OBJECTS += $(MACH_USERSPACE_OBJS)
USERSPACE_LIBS += $(MACH_USERSPACE_LIBS)
USERSPACE_CFLAGS += $(MACH_CFLAGS)
USERSPACE_LDFLAGS += $(MACH_LDFLAGS)

# Mach bootstrap integration
MACH_BOOT_SOURCES = \
	$(MACH_BOOTDIR)/bootstrap.c \
	$(MACH_BOOTDIR)/bootstrap_server.c \
	$(MACH_BOOTDIR)/bootstrap_user.c

MACH_BOOT_OBJECTS = $(MACH_BOOT_SOURCES:%.c=$(MACH_BUILDDIR)/bootstrap/%.o)
MACH_BOOT_LIB = $(MACH_BUILDDIR)/bootstrap/libbootstrap.a

$(MACH_BOOT_LIB): $(MACH_BOOT_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(MACH_BUILDDIR)/bootstrap/%.o: $(MACH_BOOTDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(MACH_CFLAGS) -c $< -o $@

# Mach targets
.PHONY: mach mach-lib mach-threads mach-bootstrap mach-interfaces

mach: $(MACH_LIB) $(MACH_THREAD_LIB) $(MACH_BOOT_LIB)

mach-lib: $(MACH_LIB)

mach-threads: $(MACH_THREAD_LIB)

mach-bootstrap: $(MACH_BOOT_LIB)

mach-interfaces: $(MACH_DEFS:%.defs=%.h)

# Clean Mach build artifacts
mach-clean:
	$(RM) -r $(MACH_BUILDDIR)
	$(RM) $(MACH_DEFS:%.defs=%.h)
	$(RM) $(MACH_DEFS:%.defs=%.c)
	$(RM) $(MACH_DEFS:%.defs=%_server.c)

# Install Mach components
install-mach: $(MACH_LIB) $(MACH_THREAD_LIB) $(MACH_HEADERS)
	@$(MKDIR) $(DESTDIR)$(LIBDIR)
	$(CP) $(MACH_LIB) $(MACH_THREAD_LIB) $(DESTDIR)$(LIBDIR)/
	@$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/mach
	$(CP) $(MACH_HEADERS) $(DESTDIR)$(INCLUDEDIR)/mach/

# Uninstall Mach components
uninstall-mach:
	$(RM) $(addprefix $(DESTDIR)$(LIBDIR)/,$(notdir $(MACH_LIB) $(MACH_THREAD_LIB)))
	$(RM) -r $(DESTDIR)$(INCLUDEDIR)/mach

# Mach configuration for kernel
ifdef MACH_KERNEL
    KERNEL_CFLAGS += -DMACH_KERNEL_INTEGRATION=1
    KERNEL_SOURCES += $(MACH_SOURCES) $(MACH_BOOT_SOURCES)
    KERNEL_LIBS += $(MACH_LIBS)
endif

# Mach configuration for userspace
ifdef MACH_USERSPACE
    USERSPACE_CFLAGS += -DMACH_USERSPACE_INTEGRATION=1
    USERSPACE_SOURCES += $(MACH_SOURCES)
    USERSPACE_LIBS += $(MACH_USERSPACE_LIBS)
endif

# Help for Mach integration
help-mach:
	@echo "Mach Integration"
	@echo "================"
	@echo ""
	@echo "Available targets:"
	@echo "  mach           - Build all Mach components"
	@echo "  mach-lib       - Build Mach library"
	@echo "  mach-threads   - Build Mach threads library"
	@echo "  mach-bootstrap - Build Mach bootstrap"
	@echo "  mach-interfaces - Generate Mach interfaces"
	@echo "  mach-clean     - Clean Mach build artifacts"
	@echo "  install-mach   - Install Mach components"
	@echo "  uninstall-mach - Uninstall Mach components"
	@echo ""
	@echo "Configuration:"
	@echo "  MACH_KERNEL    - Enable Mach kernel integration"
	@echo "  MACH_USERSPACE - Enable Mach userspace integration"

.PHONY: help-mach
