#include <stdlib.h>

struct mirix_boot_config {
    const char *timeshare_path;
    size_t alloc_bytes;
    const char *init_path;
    const char *root_path;
    int verbose;
    int vcpus;
};

struct mirix_task {
    int pid;
    int state;
    void *stack;
};

