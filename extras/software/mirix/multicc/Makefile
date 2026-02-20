# Multicc makefile
# SPDX-License-Identifier: MIT

CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O2
PREFIX ?= /usr
TARGET := multicc

all: multicc

multicc: multicc.cpp
	$(CXX) $(CXXFLAGS) $< -o $(TARGET)

clean:
	rm -f $(TARGET)

install:
	install -v $(TARGET) $(PREFIX)/bin/$(TARGET)
