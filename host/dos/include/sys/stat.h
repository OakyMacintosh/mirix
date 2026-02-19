#ifndef MIRIX_DOS_SYS_STAT_H
#define MIRIX_DOS_SYS_STAT_H

#include "sys/types.h"

#define S_IFMT   0170000
#define S_IFDIR  0040000
#define S_IFREG  0100000

#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)

struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
};

int stat(const char *pathname, struct stat *st);
int fstat(int fd, struct stat *st);
int lstat(const char *pathname, struct stat *st);

#endif /* MIRIX_DOS_SYS_STAT_H */
