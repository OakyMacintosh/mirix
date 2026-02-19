#include "modules/dos_personality.h"
#include "mirix/kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *const kDosDefaultRoot = "ROOT";
static const char *const kDosDefaultLazyfs = "./dos_lazyfs.img";

void dos_personality_apply_defaults(mirix_kernel_args_t *args) {
    if (!args) {
        return;
    }

    if (!args->root_filesystem || args->root_filesystem[0] == '\\0') {
        args->root_filesystem = strdup(kDosDefaultRoot);
        printf("[dos-personality] root set to '%s'\n", args->root_filesystem);
    }

    if (!args->lazyfs_backing_file || args->lazyfs_backing_file[0] == '\\0') {
        args->lazyfs_backing_file = strdup(kDosDefaultLazyfs);
        printf("[dos-personality] lazyfs image set to '%s'\n", args->lazyfs_backing_file);
    }
}

static int dos_personality_init(void) {
    printf("kernel module [dos-personality]: enabling DOS personality\n");
    mirix_kernel_args_t *args = mirix_get_kernel_args();
    dos_personality_apply_defaults(args);
    return 0;
}

static void dos_personality_shutdown(void) {
    printf("kernel module [dos-personality]: tearing down DOS behavior\n");
}

const kernel_module_t kernel_module_dos_personality = {
    .name = "dos-personality",
    .init = dos_personality_init,
    .shutdown = dos_personality_shutdown
};
