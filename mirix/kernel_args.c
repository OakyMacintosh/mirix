#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "kernel_args.h"

// Kernel command line argument parser for MIRIX

// Default kernel arguments
static mirix_kernel_args_t default_args = {
    .timeshare_file = NULL,
    .alloc_size = 512 * 1024 * 1024, // 512MB
    .init_program = "./sysvinit.out",
    .root_filesystem = "build/X86_64-DEBUG/filesystem/rootfs",
    .lazyfs_backing_file = "./lazyfs.img", // New default
    .verbose = false,
    .cpu_count = 1,
    .help = false
};

// Print usage information
static void print_usage(const char *program_name) {
    printf("MIRIX Kernel\n");
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  -t, --timeshare FILE     Timeshare configuration file\n");
    printf("  -a, --alloc SIZE        Memory allocation size (default: 512MB)\n");
    printf("  -i, --init PROGRAM      Init program (default: ./sysvinit.out)\n");
    printf("  -r, --root FS          Root filesystem mount point (default: %s)\n", default_args.root_filesystem);
    printf("  -f, --lazyfs-file FILE LazyFS backing file (default: %s)\n", default_args.lazyfs_backing_file);
    printf("  -v, --verbose           Enable verbose output\n");
    printf("  -m, --mcpu COUNT       Number of CPUs (default: %d)\n", default_args.cpu_count);
    printf("  -h, --help             Show this help message\n\n");
}

// Parse memory size string
static size_t parse_memory_size(const char *size_str) {
    size_t size = strtoull(size_str, NULL, 10);
    
    // Check for suffixes
    size_t len = strlen(size_str);
    if (len > 0) {
        char suffix = size_str[len - 1];
        switch (suffix) {
            case 'K':
            case 'k':
                size *= 1024;
                break;
            case 'M':
            case 'm':
                size *= 1024 * 1024;
                break;
            case 'G':
            case 'g':
                size *= 1024 * 1024 * 1024;
                break;
            case 'B':
            case 'b':
                // Already in bytes
                break;
            default:
                printf("Warning: Unknown memory size suffix '%c', assuming bytes\n", suffix);
                break;
        }
    }
    
    return size;
}

// Parse CPU count
static int parse_cpu_count(const char *cpu_str) {
    char *endptr;
    long count = strtol(cpu_str, &endptr, 10);
    
    if (*endptr != '\0' || count < 1 || count > 256) {
        printf("Error: Invalid CPU count '%s', must be 1-256\n", cpu_str);
        return -1;
    }
    
    return (int)count;
}

// Parse kernel command line arguments
mirix_kernel_args_t* parse_kernel_args(int argc, char *argv[]) {
    mirix_kernel_args_t *args = malloc(sizeof(mirix_kernel_args_t));
    if (!args) {
        return NULL;
    }
    
    // Copy defaults
    *args = default_args;
    
    // Define long options
    static struct option long_options[] = {
        {"timeshare", required_argument, 0, 't'},
        {"alloc",      required_argument, 0, 'a'},
        {"init",       required_argument, 0, 'i'},
        {"root",       required_argument, 0, 'r'},
        {"lazyfs-file", required_argument, 0, 'f'}, // New long option
        {"verbose",    no_argument,       0, 'v'},
        {"mcpu",      required_argument, 0, 'm'},
        {"help",       no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    
    // Parse options
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "t:a:i:r:f:vm:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 't':
                args->timeshare_file = strdup(optarg);
                break;
                
            case 'a':
                args->alloc_size = parse_memory_size(optarg);
                break;
                
            case 'i':
                args->init_program = strdup(optarg);
                break;
                
            case 'r':
                args->root_filesystem = strdup(optarg);
                break;
                
            case 'f': // New case
                args->lazyfs_backing_file = strdup(optarg);
                break;
                
            case 'v':
                args->verbose = true;
                break;
                
            case 'm':
                args->cpu_count = parse_cpu_count(optarg);
                if (args->cpu_count == -1) {
                    free(args);
                    return NULL;
                }
                break;
                
            case 'h':
                args->help = true;
                print_usage(argv[0]);
                free(args);
                return NULL;
                
            case '?':
                free(args);
                return NULL;
                
            default:
                free(args);
                return NULL;
        }
    }
    
    // Validate arguments
    if (args->alloc_size == 0 || args->cpu_count < 1) {
        free(args);
        return NULL;
    }
    
    return args;
}

// Print parsed arguments
void print_kernel_args(const mirix_kernel_args_t *args) {
    if (!args) return;
    
    printf("\nMIRIX Kernel Configuration:\n");
    printf("==========================\n");
    
    if (args->timeshare_file) {
        printf("Timeshare file:    %s\n", args->timeshare_file);
    }
    
    printf("Memory allocation:  %zu bytes (%.1f MB)\n", 
           args->alloc_size, (double)args->alloc_size / (1024 * 1024));
    
    if (args->init_program) {
        printf("Init program:      %s\n", args->init_program);
    }
    
    if (args->root_filesystem) {
        printf("Root filesystem:   %s\n", args->root_filesystem);
    }
    
    if (args->lazyfs_backing_file) { // New print
        printf("LazyFS backing file: %s\n", args->lazyfs_backing_file);
    }
    
    printf("Verbose mode:      %s\n", args->verbose ? "YES" : "NO");
    printf("CPU count:         %d\n", args->cpu_count);
    printf("\n");
}

// Free kernel arguments
void free_kernel_args(mirix_kernel_args_t *args) {
    if (!args) return;
    
    // Only free if it's not the default static string
    if (args->timeshare_file && args->timeshare_file != default_args.timeshare_file) {
        free(args->timeshare_file);
    }
    if (args->init_program && args->init_program != default_args.init_program) {
        free(args->init_program);
    }
    if (args->root_filesystem && args->root_filesystem != default_args.root_filesystem) {
        free(args->root_filesystem);
    }
    if (args->lazyfs_backing_file && args->lazyfs_backing_file != default_args.lazyfs_backing_file) { // New free
        free(args->lazyfs_backing_file);
    }
    
    free(args);
}
