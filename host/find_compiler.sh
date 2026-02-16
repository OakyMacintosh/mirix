#!/bin/bash

# Compiler and linker detection script for MIRIX build system
# Outputs: CC_PATH:LD_PATH format

set -e

# Source host-specific configuration
HOST_CONFIG=$(./detect_host.sh)
HOST_FILE="${HOST_CONFIG}.c"

# Check if host-specific file exists
if [ ! -f "$HOST_FILE" ]; then
    echo "No host-specific configuration found for $HOST_CONFIG"
    echo "Using generic detection..."
    HOST_FILE="bsds-any.c"
fi

# Compile and run host detection program
CC_PATH=""
LD_PATH=""

# Create a temporary program to find compiler and linker
cat > temp_host_detect.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 4096

extern int find_compiler_and_linker(char *cc_path, size_t cc_size,
                                     char *ld_path, size_t ld_size);

int main(void) {
    char cc_path[MAX_PATH_LEN];
    char ld_path[MAX_PATH_LEN];
    
    if (find_compiler_and_linker(cc_path, sizeof(cc_path), ld_path, sizeof(ld_path)) == 0) {
        printf("%s:%s", cc_path, ld_path);
        return 0;
    } else {
        printf("NOT_FOUND:NOT_FOUND");
        return 1;
    }
}
EOF

# Compile with the host-specific file
if gcc -o temp_host_detect temp_host_detect.c "$HOST_FILE" 2>/dev/null; then
    RESULT=$(./temp_host_detect)
    if [ "$RESULT" != "NOT_FOUND:NOT_FOUND" ]; then
        echo "$RESULT"
        rm -f temp_host_detect temp_host_detect.c
        exit 0
    fi
fi

# Fallback detection
echo "Host-specific detection failed, using fallback..."

# Try common compilers
for cc in gcc clang cc; do
    if command -v "$cc" >/dev/null 2>&1; then
        CC_PATH=$(command -v "$cc")
        break
    fi
done

# Try common linkers
for ld in ld link; do
    if command -v "$ld" >/dev/null 2>&1; then
        LD_PATH=$(command -v "$ld")
        break
    fi
done

# If still not found, use defaults
if [ -z "$CC_PATH" ]; then
    CC_PATH="cc"
fi

if [ -z "$LD_PATH" ]; then
    LD_PATH="ld"
fi

echo "${CC_PATH}:${LD_PATH}"

# Cleanup
rm -f temp_host_detect temp_host_detect.c
