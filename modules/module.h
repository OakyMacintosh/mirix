#ifndef MIRIX_KERNEL_MODULE_H
#define MIRIX_KERNEL_MODULE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*kernel_module_init_fn)(void);
typedef void (*kernel_module_shutdown_fn)(void);

typedef struct kernel_module {
    const char *name;
    kernel_module_init_fn init;
    kernel_module_shutdown_fn shutdown;
} kernel_module_t;

int initialize_kernel_modules(void);
void shutdown_kernel_modules(void);
size_t kernel_module_count(void);
const kernel_module_t *get_kernel_module(size_t index);

#ifdef __cplusplus
}
#endif

#endif // MIRIX_KERNEL_MODULE_H
