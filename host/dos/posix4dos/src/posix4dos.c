#include "posix4dos.h"
#include <errno.h> // For errno
#include <stdarg.h> // For va_list, va_start, va_end

// DJGPP's standard library functions are generally POSIX-compliant.
// For this minimal layer, we will mostly wrap them to provide a consistent
// prefix (p4d_) and a place for custom DOS-specific handling if needed later.

int p4d_open(const char *pathname, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }
    // DJGPP's open already handles DOS file access and modes.
    // It maps POSIX flags to DOS access modes.
    return open(pathname, flags, mode);
}

int p4d_close(int fd) {
    return close(fd);
}

ssize_t p4d_read(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t p4d_write(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}

off_t p4d_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}
