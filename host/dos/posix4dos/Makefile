# Makefile for posix4dos library and example using DJGPP

# Compiler and linker
CC = gcc
AR = ar
CFLAGS = -Wall -Wextra -g
LDFLAGS =

# Directories
SRCDIR = src
EXDIR = example
BUILDDIR = build
LIBDIR = lib

# Files
C_SOURCES = $(SRCDIR)/posix4dos.c
OBJECTS = $(C_SOURCES:.c=.o)
HEADERS = $(SRCDIR)/posix4dos.h

EXAMPLE_SRC = $(EXDIR)/example.c
EXAMPLE_OBJ = $(EXDIR)/example.o
EXAMPLE_BIN = $(EXDIR)/example.exe

LIBRARY = $(LIBDIR)/libposix4dos.a

# Default target
all: dirs $(LIBRARY) $(EXAMPLE_BIN)

dirs:
	@mkdir -p $(BUILDDIR) $(LIBDIR) $(EXDIR)

$(LIBRARY): $(OBJECTS)
	@echo "Building library $@"
	$(AR) rcs $@ $(OBJECTS)

$(OBJECTS): $(C_SOURCES) $(HEADERS)
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIBRARY)
	@echo "Compiling and linking example $@"
	$(CC) $(CFLAGS) -I$(SRCDIR) $(EXAMPLE_SRC) $(LDFLAGS) -L$(LIBDIR) -lposix4dos -o $@

# Clean up
clean:
	@echo "Cleaning up..."
	@rm -f $(OBJECTS) $(EXAMPLE_OBJ) $(EXAMPLE_BIN) $(LIBRARY)
	@rm -rf $(BUILDDIR) $(LIBDIR)

.PHONY: all clean dirs
