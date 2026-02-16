#ifndef MIRIX_LIBSYSTEM_H
#define MIRIX_LIBSYSTEM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>

// Log levels
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3
} log_level_t;

// System information structure
typedef struct {
    const char *kernel_version;
    const char *architecture;
    int num_cpus;
    uint64_t total_memory;
    uint64_t free_memory;
} mirix_system_info_t;

// Time structure
typedef struct {
    uint64_t seconds;
    uint64_t nanoseconds;
} mirix_time_t;

// Device operations structure
typedef struct mirix_device_ops {
    int (*open)(const char *path, int flags);
    int (*close)(int fd);
    ssize_t (*read)(int fd, void *buf, size_t count);
    ssize_t (*write)(int fd, const void *buf, size_t count);
    int (*ioctl)(int fd, unsigned long request, void *arg);
} mirix_device_ops_t;

// Core library functions
int libsystem_init(void);
void libsystem_cleanup(void);

// Memory management
void* libsystem_malloc(size_t size);
void libsystem_free(void *ptr);
void* libsystem_realloc(void *ptr, size_t new_size);

// Process management
pid_t libsystem_getpid(void);
int libsystem_fork(void);

// System information
int libsystem_get_system_info(mirix_system_info_t *info);

// Time functions
int libsystem_get_time(mirix_time_t *time);

// Device management
int libsystem_register_device(const char *name, mirix_device_ops_t *ops);
int libsystem_unregister_device(const char *name);

// Logging
void libsystem_log(int level, const char *format, ...);

#endif // MIRIX_LIBSYSTEM_H
