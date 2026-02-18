#ifndef __POSIX4DOS_H__
#define __POSIX4DOS_H__

#include <stdio.h> // For FILE, SEEK_SET etc.
#include <fcntl.h> // For O_RDONLY etc.
#include <unistd.h> // For off_t

#ifdef __cplusplus
extern "C" {
#endif

// Function declarations for our POSIX-like wrappers
int p4d_open(const char *pathname, int flags, ...);
int p4d_close(int fd);
ssize_t p4d_read(int fd, void *buf, size_t count);
ssize_t p4d_write(int fd, const void *buf, size_t count);
off_t p4d_lseek(int fd, off_t offset, int whence);

#ifdef __cplusplus
}
#endif

#endif // __POSIX4DOS_H__
