#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kernel.h"
#include "kernel_args.h"

// Main entry point for the MIRIX lazykernel
int main(int argc, char **argv) {
    // XNU-style silent entry
    mirix_kernel_args_t *args = parse_kernel_args(argc, argv);
    if (!args) {
        return 1;
    }
    
    // Initialize and start the kernel
    int result = mirix_kernel_main_with_args(args);
    
    // Kernel handles its own cleanup
    return result;
}
