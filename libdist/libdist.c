#include "libdist/libdist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>

#if LIBDIST_TARGET_DOS
#include <go32.h>
#endif

void libdist_print_banner(const char *program_name) {
    if (!program_name || program_name[0] == '\0') {
        program_name = "Mirix program";
    }
    printf("=== %s ===\n", program_name);
    printf("Built for Mirix/Aqua runtime -- %s\n", __DATE__);

#if LIBDIST_TARGET_DOS
    go32_dpmi_yield();
#endif
}

uint32_t libdist_random_seed(void) {
    return (uint32_t)time(NULL) ^ (uint32_t)getpid();
}

int libdist_open(const char *pathname, int flags, mode_t mode) {
    return open(pathname, flags, mode);
}

ssize_t libdist_read(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t libdist_write(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}

int libdist_close(int fd) {
    return close(fd);
}

pid_t libdist_fork(void) {
    return fork();
}

int libdist_execve(const char *pathname, char *const argv[], char *const envp[]) {
    return execve(pathname, argv, envp);
}

pid_t libdist_waitpid(pid_t pid, int *status, int options) {
    return waitpid(pid, status, options);
}

int libdist_mkdir(const char *pathname, mode_t mode) {
    return mkdir(pathname, mode);
}

int libdist_nanosleep(const struct timespec *req, struct timespec *rem) {
    return nanosleep(req, rem);
}

int libdist_clock_gettime(clockid_t clk_id, struct timespec *tp) {
    return clock_gettime(clk_id, tp);
}
