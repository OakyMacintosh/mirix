#include <unistd.h>
#include "syscall.h"
#include "../libsyscall/libsyscall.h"

int clock_gettime_syscall(clockid_t clk_id, struct timespec *tp) {
    return clock_gettime(clk_id, tp);
}

int execve_syscall(const char *pathname, char *const argv[], char *const envp[]) {
    return mirix_sys_execve(pathname, argv, envp);
}

int fallocate_syscall(int fd, int mode, off_t offset, off_t len) {
    (void)mode;
    return ftruncate(fd, offset + len);
}

int fdatasync_syscall(int fd) {
    return fsync(fd);
}

pid_t fork_syscall(void) {
    return mirix_sys_fork();
}

int fsync_syscall(int fd) {
    return fsync(fd);
}

int madvise_syscall(void *addr, size_t length, int advice) {
    return madvise(addr, length, advice);
}

int mirix_posix_memalign_impl(size_t alignment, size_t size, void **memptr) {
    return posix_memalign(memptr, alignment, size);
}

int mkdir_syscall(const char *pathname, mode_t mode) {
    return mkdir(pathname, mode);
}

int nanosleep_syscall(const struct timespec *rqtp, struct timespec *rmtp) {
    return nanosleep(rqtp, rmtp);
}

int pipe_syscall(int pipefd[2]) {
    return pipe(pipefd);
}

int pthread_atfork_syscall(void (*prepare)(void), void (*parent)(void), void (*child)(void)) {
    return pthread_atfork(prepare, parent, child);
}

int socket_syscall(int domain, int type, int protocol) {
    return socket(domain, type, protocol);
}

pid_t waitpid_syscall(pid_t pid, int *status, int options) {
    return mirix_sys_waitpid(pid, status, options);
}