#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "aout_loader.h"

#define MIRIX_AOUT_MAGIC_OMAGIC 0x0107
#define MIRIX_AOUT_MAGIC_NMAGIC 0x0108
#define MIRIX_AOUT_MAGIC_ZMAGIC 0x010B
#define MIRIX_AOUT_HEADER_WORDS 7

static bool is_supported_magic(uint32_t magic) {
    return magic == MIRIX_AOUT_MAGIC_OMAGIC ||
           magic == MIRIX_AOUT_MAGIC_NMAGIC ||
           magic == MIRIX_AOUT_MAGIC_ZMAGIC;
}

bool mirix_aout_probe(const char *path, mirix_aout_info_t *info) {
    if (!path) {
        return false;
    }

    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        return false;
    }

    uint32_t header[MIRIX_AOUT_HEADER_WORDS];
    ssize_t bytes = read(fd, header, sizeof(header));
    close(fd);

    if (bytes != sizeof(header)) {
        return false;
    }

    uint32_t magic = header[0];
    if (!is_supported_magic(magic)) {
        return false;
    }

    if (info) {
        info->magic = magic;
        info->text_size = header[1];
        info->data_size = header[2];
        info->bss_size = header[3];
        info->entry_point = header[4];
        info->text_reloc_size = header[5];
        info->data_reloc_size = header[6];
    }

    return true;
}

int mirix_aout_launch(const char *path, char *const argv[]) {
    if (!path) {
        errno = EINVAL;
        return -1;
    }

    extern char **environ;
    return execve(path, argv, environ);
}
