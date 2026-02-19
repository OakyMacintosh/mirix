#ifndef MIRIX_AOUT_LOADER_H
#define MIRIX_AOUT_LOADER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t magic;
    uint32_t text_size;
    uint32_t data_size;
    uint32_t bss_size;
    uint32_t entry_point;
    uint32_t text_reloc_size;
    uint32_t data_reloc_size;
} mirix_aout_info_t;

bool mirix_aout_probe(const char *path, mirix_aout_info_t *info);
int mirix_aout_launch(const char *path, char *const argv[]);

#endif // MIRIX_AOUT_LOADER_H
