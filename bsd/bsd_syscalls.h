#ifndef BSD_SYSCALLS_H
#define BSD_SYSCALLS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <stddef.h>

// BSD-style system call wrappers for MIRIX

// File operations
int bsd_syscall_read(int fd, void *buf, size_t count);
int bsd_syscall_write(int fd, const void *buf, size_t count);
int bsd_syscall_open(const char *pathname, int flags, mode_t mode);
int bsd_syscall_close(int fd);
int bsd_syscall_ioctl(int fd, unsigned long request, void *arg);

// Process operations
int bsd_syscall_fork(void);
int bsd_syscall_execve(const char *pathname, char *const argv[], char *const envp[]);
int bsd_syscall_wait4(pid_t pid, int *status, int options, struct rusage *rusage);
void bsd_syscall_exit(int status);
int bsd_syscall_kill(pid_t pid, int sig);

// Memory operations
void* bsd_syscall_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int bsd_syscall_munmap(void *addr, size_t length);

// Network operations
int bsd_syscall_socket(int domain, int type, int protocol);
int bsd_syscall_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int bsd_syscall_listen(int sockfd, int backlog);
int bsd_syscall_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int bsd_syscall_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int bsd_syscall_send(int sockfd, const void *buf, size_t len, int flags);
int bsd_syscall_recv(int sockfd, void *buf, size_t len, int flags);

#endif // BSD_SYSCALLS_H
