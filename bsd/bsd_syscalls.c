#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "bsd_syscalls.h"

// BSD-style system call compatibility layer for MIRIX
// Provides BSD system call interface

// BSD system call numbers
#define BSD_SYS_read         3
#define BSD_SYS_write        4
#define BSD_SYS_open         5
#define BSD_SYS_close        6
#define BSD_SYS_fork        7
#define BSD_SYS_execve       8
#define BSD_SYS_wait4        9
#define BSD_SYS_exit         10
#define BSD_SYS_kill         11
#define BSD_SYS_mmap         12
#define BSD_SYS_munmap       13
#define BSD_SYS_ioctl        14
#define BSD_SYS_socket       15
#define BSD_SYS_bind        16
#define BSD_SYS_listen      17
#define BSD_SYS_accept      18
#define BSD_SYS_connect     19
#define BSD_SYS_send        20
#define BSD_SYS_recv        21

// BSD-style system call wrappers
int bsd_syscall_read(int fd, void *buf, size_t count) {
    printf("bsd_syscall_read: fd=%d, buf=%p, count=%zu\n", fd, buf, count);
    
    ssize_t result = read(fd, buf, count);
    if (result == -1) {
        printf("bsd_syscall_read: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_read: success, bytes_read=%zd\n", result);
    }
    
    return (int)result;
}

int bsd_syscall_write(int fd, const void *buf, size_t count) {
    printf("bsd_syscall_write: fd=%d, buf=%p, count=%zu\n", fd, buf, count);
    
    ssize_t result = write(fd, buf, count);
    if (result == -1) {
        printf("bsd_syscall_write: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_write: success, bytes_written=%zd\n", result);
    }
    
    return (int)result;
}

int bsd_syscall_open(const char *pathname, int flags, mode_t mode) {
    printf("bsd_syscall_open: pathname=%s, flags=0x%x, mode=0%o\n", pathname, flags, mode);
    
    int result = open(pathname, flags, mode);
    if (result == -1) {
        printf("bsd_syscall_open: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_open: success, fd=%d\n", result);
    }
    
    return result;
}

int bsd_syscall_close(int fd) {
    printf("bsd_syscall_close: fd=%d\n", fd);
    
    int result = close(fd);
    if (result == -1) {
        printf("bsd_syscall_close: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_close: success\n");
    }
    
    return result;
}

int bsd_syscall_fork(void) {
    printf("bsd_syscall_fork: creating new process\n");
    
    int result = fork();
    if (result == -1) {
        printf("bsd_syscall_fork: failed, errno=%d\n", errno);
    } else if (result == 0) {
        printf("bsd_syscall_fork: child process created\n");
    } else {
        printf("bsd_syscall_fork: parent process, child_pid=%d\n", result);
    }
    
    return result;
}

int bsd_syscall_execve(const char *pathname, char *const argv[], char *const envp[]) {
    printf("bsd_syscall_execve: pathname=%s\n", pathname);
    
    int result = execve(pathname, argv, envp);
    if (result == -1) {
        printf("bsd_syscall_execve: failed, errno=%d\n", errno);
    }
    
    return result;
}

int bsd_syscall_wait4(pid_t pid, int *status, int options, struct rusage *rusage) {
    printf("bsd_syscall_wait4: pid=%d, options=0x%x\n", pid, options);
    
    int result = wait4(pid, status, options, rusage);
    if (result == -1) {
        printf("bsd_syscall_wait4: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_wait4: success, pid=%d, status=0x%x\n", result, status ? *status : 0);
    }
    
    return result;
}

void bsd_syscall_exit(int status) {
    printf("bsd_syscall_exit: status=%d\n", status);
    exit(status);
}

int bsd_syscall_kill(pid_t pid, int sig) {
    printf("bsd_syscall_kill: pid=%d, sig=%d\n", pid, sig);
    
    int result = kill(pid, sig);
    if (result == -1) {
        printf("bsd_syscall_kill: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_kill: success\n");
    }
    
    return result;
}

void* bsd_syscall_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    printf("bsd_syscall_mmap: addr=%p, length=%zu, prot=0x%x, flags=0x%x, fd=%d, offset=%lld\n",
           addr, length, prot, flags, fd, (long long)offset);
    
    void *result = mmap(addr, length, prot, flags, fd, offset);
    if (result == MAP_FAILED) {
        printf("bsd_syscall_mmap: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_mmap: success, addr=%p\n", result);
    }
    
    return result;
}

int bsd_syscall_munmap(void *addr, size_t length) {
    printf("bsd_syscall_munmap: addr=%p, length=%zu\n", addr, length);
    
    int result = munmap(addr, length);
    if (result == -1) {
        printf("bsd_syscall_munmap: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_munmap: success\n");
    }
    
    return result;
}

int bsd_syscall_ioctl(int fd, unsigned long request, void *arg) {
    printf("bsd_syscall_ioctl: fd=%d, request=0x%lx, arg=%p\n", fd, request, arg);
    
    int result = ioctl(fd, request, arg);
    if (result == -1) {
        printf("bsd_syscall_ioctl: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_ioctl: success, result=%d\n", result);
    }
    
    return result;
}

// Network system calls
int bsd_syscall_socket(int domain, int type, int protocol) {
    printf("bsd_syscall_socket: domain=%d, type=%d, protocol=%d\n", domain, type, protocol);
    
    int result = socket(domain, type, protocol);
    if (result == -1) {
        printf("bsd_syscall_socket: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_socket: success, fd=%d\n", result);
    }
    
    return result;
}

int bsd_syscall_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    printf("bsd_syscall_bind: sockfd=%d, addr=%p, addrlen=%d\n", sockfd, addr, addrlen);
    
    int result = bind(sockfd, addr, addrlen);
    if (result == -1) {
        printf("bsd_syscall_bind: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_bind: success\n");
    }
    
    return result;
}

int bsd_syscall_listen(int sockfd, int backlog) {
    printf("bsd_syscall_listen: sockfd=%d, backlog=%d\n", sockfd, backlog);
    
    int result = listen(sockfd, backlog);
    if (result == -1) {
        printf("bsd_syscall_listen: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_listen: success\n");
    }
    
    return result;
}

int bsd_syscall_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    printf("bsd_syscall_accept: sockfd=%d, addr=%p, addrlen=%p\n", sockfd, addr, addrlen);
    
    int result = accept(sockfd, addr, addrlen);
    if (result == -1) {
        printf("bsd_syscall_accept: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_accept: success, fd=%d\n", result);
    }
    
    return result;
}

int bsd_syscall_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    printf("bsd_syscall_connect: sockfd=%d, addr=%p, addrlen=%d\n", sockfd, addr, addrlen);
    
    int result = connect(sockfd, addr, addrlen);
    if (result == -1) {
        printf("bsd_syscall_connect: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_connect: success\n");
    }
    
    return result;
}

int bsd_syscall_send(int sockfd, const void *buf, size_t len, int flags) {
    printf("bsd_syscall_send: sockfd=%d, buf=%p, len=%zu, flags=0x%x\n", sockfd, buf, len, flags);
    
    ssize_t result = send(sockfd, buf, len, flags);
    if (result == -1) {
        printf("bsd_syscall_send: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_send: success, bytes_sent=%zd\n", result);
    }
    
    return (int)result;
}

int bsd_syscall_recv(int sockfd, void *buf, size_t len, int flags) {
    printf("bsd_syscall_recv: sockfd=%d, buf=%p, len=%zu, flags=0x%x\n", sockfd, buf, len, flags);
    
    ssize_t result = recv(sockfd, buf, len, flags);
    if (result == -1) {
        printf("bsd_syscall_recv: failed, errno=%d\n", errno);
    } else {
        printf("bsd_syscall_recv: success, bytes_recv=%zd\n", result);
    }
    
    return (int)result;
}
