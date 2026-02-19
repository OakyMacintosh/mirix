#include "modules/module_registry.h"
#include <stdio.h>

static const kernel_module_t *const kernel_modules[] = {
    &kernel_module_dos_personality,
    NULL
};

int initialize_kernel_modules(void) {
    size_t idx = 0;
    int result = 0;

    printf("Loading %zu kernel module(s)...\n", kernel_module_count());
    while (kernel_modules[idx]) {
        const kernel_module_t *module = kernel_modules[idx];
        if (module->init) {
            printf("  initializing module '%s'...\n", module->name);
            result = module->init();
            if (result != 0) {
                printf("  module '%s' failed to initialize (err=%d)\n", module->name, result);
                return result;
            }
        }
        idx++;
    }

    return 0;
}

void shutdown_kernel_modules(void) {
    size_t idx = 0;
    while (kernel_modules[idx]) {
        const kernel_module_t *module = kernel_modules[idx];
        if (module->shutdown) {
            printf("  shutting down module '%s'...\n", module->name);
            module->shutdown();
        }
        idx++;
    }
}

size_t kernel_module_count(void) {
    size_t idx = 0;
    while (kernel_modules[idx]) {
        idx++;
    }
    return idx;
}

const kernel_module_t *get_kernel_module(size_t index) {
    if (index >= kernel_module_count()) {
        return NULL;
    }
    return kernel_modules[index];
}
