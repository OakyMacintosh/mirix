# MIRIX Mach Integration Makefile
# This Makefile builds and integrates Mach components with the lazykernel

# Include MIRIX build definitions
include ../Makedefs/Config.mk
include ../Makedefs/Rules.mk
include ../Makedefs/Mach.mk

# Mach-specific configuration
MACH_ROOT = $(CURDIR)
MIRIX_ROOT = $(CURDIR)/..

# Export variables for sub-makes
export MACH_ROOT MIRIX_ROOT
export CC CXX LD AR RANLIB STRIP
export CFLAGS CXXFLAGS LDFLAGS ARFLAGS
export MACH_CFLAGS MACH_LDFLAGS MACH_LIBS

# Default target
all: mach

# Build all Mach components
mach: mach-interfaces mach-lib mach-threads mach-bootstrap

# Build Mach interfaces (requires MIG)
mach-interfaces: $(MACH_DEFS:%.defs=%.h)

# Build Mach object files
$(MACH_LIBBUILDDIR)/%.o: $(MACH_LIBDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(MACH_CFLAGS) -c $< -o $@

$(MACH_THREADSBUILDDIR)/%.o: $(MACH_THREADSDIR)/%.c
	@$(MKDIR) $(dir $@)
	$(CC) $(MACH_CFLAGS) -c $< -o $@

# Build Mach library
$(MACH_LIB): $(MACH_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@
	@echo "Built Mach library: $@"

# Build Mach threads library
$(MACH_THREAD_LIB): $(MACH_THREAD_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@
	@echo "Built Mach threads library: $@"

# Build Mach bootstrap
mach-bootstrap: $(MACH_BOOT_LIB)

# Integration with lazykernel
kernel-integration: $(MACH_KERNEL_LIBS)
	@echo "Mach kernel integration complete"
	@echo "Libraries: $(MACH_KERNEL_LIBS)"
	@echo "Objects: $(MACH_KERNEL_OBJS)"

# Userspace integration
userspace-integration: $(MACH_USERSPACE_LIBS)
	@echo "Mach userspace integration complete"
	@echo "Libraries: $(MACH_USERSPACE_LIBS)"
	@echo "Objects: $(MACH_USERSPACE_OBJS)"

# Generate all interfaces
interfaces: $(MACH_DEFS:%.defs=%.h) $(MACH_DEFS:%.defs=%.c) $(MACH_DEFS:%.defs=%_server.c)

# Build everything
everything: mach kernel-integration userspace-integration

# Test Mach components
test-mach: $(MACH_LIB) $(MACH_THREAD_LIB)
	@echo "Testing Mach library..."
	@if [ -f test/mach_test.c ]; then \
		$(MAKE) -C test; \
	else \
		echo "No Mach tests found"; \
	fi

# Benchmark Mach components
benchmark-mach: $(MACH_LIB) $(MACH_THREAD_LIB)
	@echo "Benchmarking Mach components..."
	@if [ -f benchmark/mach_bench.c ]; then \
		$(MAKE) -C benchmark; \
	else \
		echo "No Mach benchmarks found"; \
	fi

# Install Mach components
install: install-mach

# Clean build artifacts
clean: mach-clean
	$(RM) -r $(MACH_BUILDDIR)
	$(RM) $(MACH_DEFS:%.defs=%.h)
	$(RM) $(MACH_DEFS:%.defs=%.c)
	$(RM) $(MACH_DEFS:%.defs=%_server.c)

# Distribution clean
distclean: clean
	$(RM) -r autom4te.cache
	$(RM) config.log config.status
	$(RM) configure

# Create distribution
dist: $(DIST_FILES)
	@$(MKDIR) $(DISTDIR)/mach
	$(CP) $^ $(DISTDIR)/mach/
	$(TAR) czf $(PACKAGE_TARNAME)-mach.tar.gz $(DISTDIR)
	$(RM) -r $(DISTDIR)

# Help target
help: help-mach
	@echo ""
	@echo "Mach Integration Makefile"
	@echo "========================="
	@echo ""
	@echo "Additional targets:"
	@echo "  kernel-integration     - Build Mach for kernel integration"
	@echo "  userspace-integration  - Build Mach for userspace integration"
	@echo "  interfaces            - Generate all Mach interfaces"
	@echo "  everything             - Build all components and integrations"
	@echo "  test-mach              - Test Mach components"
	@echo "  benchmark-mach         - Benchmark Mach components"

# Include dependency files if they exist
-include $(MACH_OBJECTS:.o=.d)
-include $(MACH_THREAD_OBJECTS:.o=.d)
-include $(MACH_BOOT_OBJECTS:.o=.d)

# Force interface generation
.PHONY: interfaces force-interfaces

# Generate interfaces on demand
force-interfaces:
	$(RM) $(MACH_DEFS:%.defs=%.h)
	$(RM) $(MACH_DEFS:%.defs=%.c)
	$(RM) $(MACH_DEFS:%.defs=%_server.c)
	$(MAKE) interfaces

# Check for required tools
check-tools:
	@echo "Checking for required tools..."
	@command -v mig >/dev/null 2>&1 || echo "Warning: MIG not found, interfaces will be stubs"
	@command -v $(CC) >/dev/null 2>&1 || { echo "Error: Compiler not found"; exit 1; }
	@command -v $(AR) >/dev/null 2>&1 || { echo "Error: Archiver not found"; exit 1; }
	@echo "Tool check complete"

# Show configuration
show-config:
	@echo "Mach Configuration"
	@echo "=================="
	@echo "MACH_ROOT: $(MACH_ROOT)"
	@echo "MIRIX_ROOT: $(MIRIX_ROOT)"
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "MACH_CFLAGS: $(MACH_CFLAGS)"
	@echo "MACH_LIB: $(MACH_LIB)"
	@echo "MACH_THREAD_LIB: $(MACH_THREAD_LIB)"
	@echo "MACH_BOOT_LIB: $(MACH_BOOT_LIB)"
	@echo "MACH_SOURCES: $(words $(MACH_SOURCES)) files"
	@echo "MACH_THREAD_SOURCES: $(words $(MACH_THREAD_SOURCES)) files"
	@echo "MACH_DEFS: $(words $(MACH_DEFS)) files"

# Debug target
debug:
	@echo "Debug information:"
	@echo "Current directory: $(CURDIR)"
	@echo "Make version: $(MAKE_VERSION)"
	@echo "Shell: $(SHELL)"
	@echo "PATH: $(PATH)"
	@echo "Available .defs files:"
	@find $(MACH_INCDIR) -name "*.defs" -exec echo "  {}" \;

# Parallel build support
.NOTPARALLEL: interfaces

# Keep intermediate files
.PRECIOUS: $(MACH_DEFS:%.defs=%.h) $(MACH_DEFS:%.defs=%.c) $(MACH_DEFS:%.defs=%_server.c)
