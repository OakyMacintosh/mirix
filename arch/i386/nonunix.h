#ifndef NONUNIX_H
#define NONUNIX_H

#include <stddef.h>
#include <stdbool.h>

// Platform types for MIRIX
typedef enum {
    MIRIX_PLATFORM_UNKNOWN = 0,
    MIRIX_PLATFORM_WINDOWS = 1,
    MIRIX_PLATFORM_MACOS = 2,
    MIRIX_PLATFORM_LINUX = 3,
    MIRIX_PLATFORM_GENERIC = 4
} mirix_platform_t;

// Platform information structure
typedef struct {
    mirix_platform_t platform_type;
    const char *platform_name;
    bool supports_threads;
    bool supports_fork;
    bool supports_signals;
    bool supports_unix_domain;
} mirix_platform_info_t;

// Non-UNIX platform functions
int nonunix_init(void);
void nonunix_cleanup(void);
int nonunix_get_platform_info(mirix_platform_info_t *info);

// Platform-specific functions
void* nonunix_malloc(size_t size);
void nonunix_free(void *ptr);
int nonunix_create_thread(void *(*start_routine)(void *), void *arg);
int nonunix_open_file(const char *path, int mode);
int nonunix_create_ipc_channel(const char *name);

#endif // NONUNIX_H
