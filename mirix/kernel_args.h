#ifndef KERNEL_ARGS_H
#define KERNEL_ARGS_H

#include <stdbool.h>
#include <stddef.h>

// Kernel command line arguments structure for MIRIX

typedef struct mirix_kernel_args {
    char *timeshare_file;      // Timeshare configuration file
    size_t alloc_size;         // Memory allocation size
    char *init_program;        // Init program to run
    char *command_program;     // Binary launched via -C
    char *root_filesystem;     // Root filesystem
    char *lazyfs_backing_file; // Path to the lazyfs backing file
    bool verbose;             // Verbose output
    int cpu_count;            // Number of CPUs
    bool help;                // Show help
} mirix_kernel_args_t;

// Kernel argument parsing functions
mirix_kernel_args_t* parse_kernel_args(int argc, char *argv[]);
void print_kernel_args(const mirix_kernel_args_t *args);
void free_kernel_args(mirix_kernel_args_t *args);

#endif // KERNEL_ARGS_H
