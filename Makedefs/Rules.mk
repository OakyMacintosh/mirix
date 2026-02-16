# MIRIX Build Rules
# This file contains common build rules for all Makefiles
# Note: Config.mk should be included before this file

# Host detection and configuration
HOST_CC = $(CC)
HOST_CXX = $(CXX)
HOST_LD = $(LD)
HOST_AR = $(AR)

# Function to detect host system
define detect_host
$(shell $(HOSTCONFIGDIR)/detect_host.sh)
endef

# Function to find compiler and linker
define find_compiler_linker
$(shell $(HOSTCONFIGDIR)/find_compiler.sh)
endef

# Build object file from C source
define build_c_object
$(CC) $(CFLAGS) -c $(1) -o $(2)
endef

# Build object file from C++ source
define build_cpp_object
$(CXX) $(CXXFLAGS) -c $(1) -o $(2)
endef

# Link executable
define link_executable
$(LD) $(LDFLAGS) $(1) -o $(2) $(3)
endef

# Create static library
define create_static_library
$(AR) $(ARFLAGS) $(2) $(1)
$(RANLIB) $(2)
endef

# Create shared library
define create_shared_library
$(CC) -shared $(LDFLAGS) $(1) -o $(2) $(3)
endef

# Pattern rules for building object files
$(BUILDDIR)/%.o: %.c
	@$(MKDIR) $(dir $@)
	$(call build_c_object,$<,$@)

$(BUILDDIR)/%.o: %.cpp
	@$(MKDIR) $(dir $@)
	$(call build_cpp_object,$<,$@)

$(BUILDDIR)/%.o: %.cc
	@$(MKDIR) $(dir $@)
	$(call build_cpp_object,$<,$@)

# Pattern rules for building static libraries
$(BUILDDIR)/%.a:
	@$(MKDIR) $(dir $@)
	$(call create_static_library,$^,$@)

# Pattern rules for building shared libraries
$(BUILDDIR)/%.so:
	@$(MKDIR) $(dir $@)
	$(call create_shared_library,$^,$@,$(3))

# Pattern rules for building executables
$(BUILDDIR)/%$(EXEEXT):
	@$(MKDIR) $(dir $@)
	$(call link_executable,$^,$@,$(3))

# Mach-specific rules
$(BUILDDIR)/mach/%.o: mach/%.c
	@$(MKDIR) $(dir $@)
	$(call build_c_object,$<,$@)

$(BUILDDIR)/mach/%.o: mach/libmach/c/%.c
	@$(MKDIR) $(dir $@)
	$(call build_c_object,$<,$@)

$(BUILDDIR)/mach/%.o: mach/libthreads/%.c
	@$(MKDIR) $(dir $@)
	$(call build_c_object,$<,$@)

# MIG (Mach Interface Generator) rules
%.h: %.defs
	@if command -v mig >/dev/null 2>&1; then \
		mig -user $*.c -server $*_server.c -header $@ $<; \
	else \
		echo "MIG not found, skipping interface generation for $<"; \
		touch $@; \
	fi

%.c: %.defs
	@if command -v mig >/dev/null 2>&1; then \
		mig -user $@ -server $*_server.c -header $*.h $<; \
	else \
		echo "MIG not found, skipping interface generation for $<"; \
		touch $@; \
	fi

# Dependency generation
$(BUILDDIR)/%.d: %.c
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< > $@

# Include dependency files if they exist
-include $(OBJECTS:.o=.d)

# Cross-compilation rules
ifdef CROSS_COMPILE
$(BUILDDIR)/cross/%.o: %.c
	@$(MKDIR) $(dir $@)
	$(CROSS_COMPILE)-gcc $(CFLAGS) -c $< -o $@

$(BUILDDIR)/cross/%$(EXEEXT): $(CROSS_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(CROSS_COMPILE)-gcc $(LDFLAGS) $^ -o $@
endif

# Documentation generation rules
%.html: %.md
	@if command -v pandoc >/dev/null 2>&1; then \
		pandoc -f markdown -t html $< -o $@; \
	else \
		echo "pandoc not found, skipping HTML generation for $<"; \
	fi

%.pdf: %.md
	@if command -v pandoc >/dev/null 2>&1; then \
		pandoc -f markdown -t pdf $< -o $@; \
	else \
		echo "pandoc not found, skipping PDF generation for $<"; \
	fi

# Test rules
test/%$(EXEEXT): test/%.c
	@$(MKDIR) $(dir $@)
	$(call build_c_object,$<,$(@:.exe=.o))
	$(call link_executable,$(@:.exe=.o),$@)

# Run tests
test: $(TESTS)
	@for test in $(TESTS); do \
		echo "Running $$test..."; \
		./$$test; \
	done

# Benchmark rules
benchmark/%$(EXEEXT): benchmark/%.c
	@$(MKDIR) $(dir $@)
	$(call build_c_object,$<,$(@:.exe=.o))
	$(call link_executable,$(@:.exe=.o),$@)

# Run benchmarks
benchmark: $(BENCHMARKS)
	@for bench in $(BENCHMARKS); do \
		echo "Running $$bench..."; \
		./$$bench; \
	done

# Installation rules
install-bin: $(BINS)
	@$(MKDIR) $(DESTDIR)$(BINDIR)
	$(CP) $^ $(DESTDIR)$(BINDIR)/

install-lib: $(LIBS)
	@$(MKDIR) $(DESTDIR)$(LIBDIR)
	$(CP) $^ $(DESTDIR)$(LIBDIR)/

install-include: $(HEADERS)
	@$(MKDIR) $(DESTDIR)$(INCLUDEDIR)
	$(CP) $^ $(DESTDIR)$(INCLUDEDIR)/

install-man: $(MANPAGES)
	@$(MKDIR) $(DESTDIR)$(MANDIR)
	$(CP) $^ $(DESTDIR)$(MANDIR)/

# Uninstallation rules
uninstall-bin:
	$(RM) $(addprefix $(DESTDIR)$(BINDIR)/,$(notdir $(BINS)))

uninstall-lib:
	$(RM) $(addprefix $(DESTDIR)$(LIBDIR)/,$(notdir $(LIBS)))

uninstall-include:
	$(RM) $(addprefix $(DESTDIR)$(INCLUDEDIR)/,$(notdir $(HEADERS)))

uninstall-man:
	$(RM) $(addprefix $(DESTDIR)$(MANDIR)/,$(notdir $(MANPAGES)))

# Distribution rules
dist: $(DIST_FILES)
	@$(MKDIR) $(DISTDIR)
	$(CP) $^ $(DISTDIR)/
	$(TAR) czf $(PACKAGE_TARNAME).tar.gz $(DISTDIR)
	$(RM) -r $(DISTDIR)

dist-bzip2: $(DIST_FILES)
	@$(MKDIR) $(DISTDIR)
	$(CP) $^ $(DISTDIR)/
	$(TAR) cjf $(PACKAGE_TARNAME).tar.bz2 $(DISTDIR)
	$(RM) -r $(DISTDIR)

dist-xz: $(DIST_FILES)
	@$(MKDIR) $(DISTDIR)
	$(CP) $^ $(DISTDIR)/
	$(TAR) cJf $(PACKAGE_TARNAME).tar.xz $(DISTDIR)
	$(RM) -r $(DISTDIR)

# Help target
help:
	@echo "MIRIX Build System"
	@echo "=================="
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build all targets"
	@echo "  clean        - Remove build artifacts"
	@echo "  install      - Install built files"
	@echo "  uninstall    - Remove installed files"
	@echo "  dist         - Create source distribution"
	@echo "  test         - Run tests"
	@echo "  benchmark    - Run benchmarks"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Configuration variables:"
	@echo "  ARCH         - Target architecture ($(ARCH))"
	@echo "  PLATFORM     - Host platform ($(PLATFORM))"
	@echo "  DEBUG        - Enable debug (0/1, current: $(DEBUG))"
	@echo "  OPTIMIZE     - Optimization level ($(OPTIMIZE))"
	@echo "  PREFIX       - Installation prefix ($(PREFIX))"
	@echo ""
	@echo "Cross-compilation:"
	@echo "  CROSS_COMPILE - Cross-compiler prefix"
	@echo "  TARGET_TRIPLE - Target triple ($(TARGET_TRIPLE))"

# Phony targets
.PHONY: all clean install uninstall dist test benchmark help
.PHONY: install-bin install-lib install-include install-man
.PHONY: uninstall-bin uninstall-lib uninstall-include uninstall-man
.PHONY: dist-bzip2 dist-xz
