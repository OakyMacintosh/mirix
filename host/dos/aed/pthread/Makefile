# Makefile for dosthread (AED - Aqua/Mirix Kernel Extensions for DOS)
# Targets DOS/32A + DJGPP toolchain
#
# Usage:
#   make          - build libdosthread.a
#   make test     - build the test binary (test.exe)
#   make clean    - remove build artifacts

# ---------------------------------------------------------------------------
# Toolchain (DJGPP cross-compiler prefixes; adjust if building natively)
# ---------------------------------------------------------------------------

CC      = i586-pc-msdosdjgpp-gcc
AR      = i586-pc-msdosdjgpp-ar
RANLIB  = i586-pc-msdosdjgpp-ranlib

# If you're building inside a native DJGPP environment (DOS or a VM),
# comment out the lines above and uncomment these:
# CC     = gcc
# AR     = ar
# RANLIB = ranlib

# ---------------------------------------------------------------------------
# Flags
# ---------------------------------------------------------------------------

CFLAGS  = -O2 -Wall -Wextra \
           -march=i386 \
           -fno-omit-frame-pointer \
           -fno-stack-protector \
           -std=gnu99

# -march=i386           target the baseline DOS/32A CPU
# -fno-omit-frame-pointer  keeps EBP valid; required for correct setjmp
# -fno-stack-protector  no SSP canary — we manage stacks manually
# -std=gnu99            allows inline asm and //comments

ARFLAGS = rcs

# ---------------------------------------------------------------------------
# Sources / objects
# ---------------------------------------------------------------------------

LIB_SRC = dosthread.c
LIB_OBJ = dosthread.o
LIB     = libdosthread.a

TEST_SRC = test.c
TEST_OBJ = test.o
TEST_BIN = test.exe

# ---------------------------------------------------------------------------
# Default target
# ---------------------------------------------------------------------------

.PHONY: all
all: $(LIB)

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------

$(LIB): $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(LIB_OBJ): $(LIB_SRC) dosthread.h
	$(CC) $(CFLAGS) -c $< -o $@

# ---------------------------------------------------------------------------
# Test binary (optional — only built with "make test")
# ---------------------------------------------------------------------------

.PHONY: test
test: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ) $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_OBJ): $(TEST_SRC) dosthread.h
	$(CC) $(CFLAGS) -c $< -o $@

# ---------------------------------------------------------------------------
# Clean
# ---------------------------------------------------------------------------

.PHONY: clean
clean:
	rm -f $(LIB_OBJ) $(LIB) $(TEST_OBJ) $(TEST_BIN)