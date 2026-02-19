# Multicc makefile
# SPDX-License-Identifier: MIT

CXX ?= clang++
CXXFLAGS ?= -std=c++17 -O2

all: multicc

multicc: multicc.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f multicc
