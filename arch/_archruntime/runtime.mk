#
# MIRIX Cross-Architecture Runtime Makefile
#

# Runtime source files
ARCHRUNTIME_SOURCES = \
	arch/_archruntime/arch_runtime.c \
	arch/_archruntime/arch_detect.c

# Runtime object files
ARCHRUNTIME_OBJECTS = $(ARCHRUNTIME_SOURCES:.c=.o)

# Runtime headers
ARCHRUNTIME_HEADERS = \
	arch/_archruntime/arch_runtime.h

# Include paths
ARCHRUNTIME_INCLUDES = -Iarch/_archruntime

# Compiler flags for runtime
ARCHRUNTIME_CFLAGS = \
	-std=c99 \
	-Wall -Wextra \
	-O2 \
	-g \
	-fno-omit-frame-pointer \
	-fno-stack-protector \
	-mno-red-zone \
	-fPIC

# Architecture-specific flags
ifeq ($(ARCH),x86_64)
	ARCHRUNTIME_CFLAGS += -DARCH_X86_64 -march=x86-64
else ifeq ($(ARCH),i386)
	ARCHRUNTIME_CFLAGS += -DARCH_I386 -march=i386
else ifeq ($(ARCH),arm)
	ARCHRUNTIME_CFLAGS += -DARCH_ARM -march=armv7-a
else ifeq ($(ARCH),arm64)
	ARCHRUNTIME_CFLAGS += -DARCH_ARM64 -march=armv8-a
else ifeq ($(ARCH),ppc)
	ARCHRUNTIME_CFLAGS += -DARCH_PPC -march=powerpc
else ifeq ($(ARCH),m68k)
	ARCHRUNTIME_CFLAGS += -DARCH_M68K -march=68040
else ifeq ($(ARCH),mips)
	ARCHRUNTIME_CFLAGS += -DARCH_MIPS -march=mips32r2
else
	# Default to x86_64 if not specified
	ARCHRUNTIME_CFLAGS += -DARCH_X86_64 -march=x86-64
	$(info Defaulting to x86_64 architecture)
	ARCH = x86_64
endif

# Platform-specific flags
ifeq ($(PLATFORM),windows)
	ARCHRUNTIME_CFLAGS += -DPLATFORM_WINDOWS
else ifeq ($(PLATFORM),macos)
	ARCHRUNTIME_CFLAGS += -DPLATFORM_MACOS
else ifeq ($(PLATFORM),linux)
	ARCHRUNTIME_CFLAGS += -DPLATFORM_LINUX
else ifeq ($(PLATFORM),bsd)
	ARCHRUNTIME_CFLAGS += -DPLATFORM_BSD
else ifeq ($(PLATFORM),dos)
	ARCHRUNTIME_CFLAGS += -DPLATFORM_DOS
else ifeq ($(PLATFORM),unix)
	ARCHRUNTIME_CFLAGS += -DPLATFORM_UNIX
else
	# Auto-detect platform
	ARCHRUNTIME_CFLAGS += -DPLATFORM_UNIX
endif

# Debug flags
ifdef DEBUG
	ARCHRUNTIME_CFLAGS += -DDEBUG -g3 -O0
else
	ARCHRUNTIME_CFLAGS += -DNDEBUG -O2
endif

# Profiling flags
ifdef PROFILING
	ARCHRUNTIME_CFLAGS += -DPROFILING -pg
endif

# Tracing flags
ifdef TRACING
	ARCHRUNTIME_CFLAGS += -DTRACING
endif

# Build rules
$(BUILD_DIR)/arch/_archruntime/%.o: arch/_archruntime/%.c
	@mkdir -p $(dir $@)
	$(CC) $(ARCHRUNTIME_CFLAGS) $(ARCHRUNTIME_INCLUDES) -c $< -o $@

# Runtime library
$(BUILD_DIR)/libarchruntime.a: $(ARCHRUNTIME_OBJECTS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^
	ranlib $@

# Clean runtime
clean-runtime:
	rm -f $(BUILD_DIR)/arch/_archruntime/*.o
	rm -f $(BUILD_DIR)/libarchruntime.a

# Install runtime headers
install-headers-runtime:
	@mkdir -p $(BUILD_DIR)/include/arch/_archruntime
	cp $(ARCHRUNTIME_HEADERS) $(BUILD_DIR)/include/arch/_archruntime/

# Help target
help-runtime:
	@echo "Cross-Architecture Runtime Build Targets:"
	@echo "  clean-runtime    - Clean runtime build files"
	@echo "  install-headers-runtime - Install runtime headers"
	@echo "  help-runtime    - Show this help"
	@echo ""
	@echo "Variables:"
	@echo "  ARCH           - Target architecture (x86_64, i386, arm, arm64, ppc, m68k, mips)"
	@echo "  PLATFORM       - Target platform (windows, macos, linux, bsd, dos, unix)"
	@echo "  DEBUG          - Enable debug build"
	@echo "  PROFILING      - Enable profiling"
	@echo "  TRACING        - Enable tracing"

.PHONY: clean-runtime install-headers-runtime help-runtime