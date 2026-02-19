#ifndef LIBDIST_H
#define LIBDIST_H

#include <stdint.h>

#if defined(__djgpp__) || defined(__MSDOS__) || defined(__DOS__)
#define LIBDIST_TARGET_DOS 1
#else
#define LIBDIST_TARGET_DOS 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Minimal dist helpers for Mirix/Aqua userland programs. */
void libdist_print_banner(const char *program_name);
uint32_t libdist_random_seed(void);

/* Mirix-compatible syscalls exposed via libdist. */
int libdist_open(const char *pathname, int flags, mode_t mode);
ssize_t libdist_read(int fd, void *buf, size_t count);
ssize_t libdist_write(int fd, const void *buf, size_t count);
int libdist_close(int fd);
pid_t libdist_fork(void);
int libdist_execve(const char *pathname, char *const argv[], char *const envp[]);
pid_t libdist_waitpid(pid_t pid, int *status, int options);
int libdist_mkdir(const char *pathname, mode_t mode);
int libdist_nanosleep(const struct timespec *req, struct timespec *rem);
int libdist_clock_gettime(clockid_t clk_id, struct timespec *tp);

#ifdef __cplusplus
}
#endif

#endif // LIBDIST_H
