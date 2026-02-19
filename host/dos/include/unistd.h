#ifndef MIRIX_DOS_UNISTD_H
#define MIRIX_DOS_UNISTD_H

#include <stddef.h>

#define _SC_PAGESIZE 1

static inline long sysconf(int name) {
    if (name == _SC_PAGESIZE) {
        return 4096;
    }
    return -1;
}

static inline int getpagesize(void) {
    return (int)sysconf(_SC_PAGESIZE);
}

#endif /* MIRIX_DOS_UNISTD_H */
