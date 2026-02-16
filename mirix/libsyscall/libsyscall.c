#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>

#include "libsyscall.h"

// System call library for MIRIX
// Provides low-level system call interface

// System call numbers for MIRIX
typedef enum {
    MIRIX_SYSCALL_READ = 0,
    MIRIX_SYSCALL_WRITE = 1,
    MIRIX_SYSCALL_OPEN = 2,
    MIRIX_SYSCALL_CLOSE = 3,
    MIRIX_SYSCALL_FORK = 4,
    MIRIX_SYSCALL_EXEC = 5,
    MIRIX_SYSCALL_WAIT = 6,
    MIRIX_SYSCALL_EXIT = 7,
    MIRIX_SYSCALL_KILL = 8,
    MIRIX_SYSCALL_MMAP = 9,
    MIRIX_SYSCALL_MUNMAP = 10,
    MIRIX_SYSCALL_BRK = 11,
    MIRIX_SYSCALL_IOCTL = 12,
    MIRIX_SYSCALL_SOCKET = 13,
    MIRIX_SYSCALL_BIND = 14,
    MIRIX_SYSCALL_LISTEN = 15,
    MIRIX_SYSCALL_ACCEPT = 16,
    MIRIX_SYSCALL_CONNECT = 17,
    MIRIX_SYSCALL_SEND = 18,
    MIRIX_SYSCALL_RECV = 19,
    MIRIX_SYSCALL_IPC_SEND = 20,
    MIRIX_SYSCALL_IPC_RECV = 21,
    MIRIX_SYSCALL_TIMER_CREATE = 22,
    MIRIX_SYSCALL_TIMER_DELETE = 23
} mirix_syscall_num_t;

// System call wrapper functions
ssize_t mirix_sys_read(int fd, void *buf, size_t count) {
    printf("mirix_sys_read: fd=%d, buf=%p, count=%zu\n", fd, buf, count);
    
    // Use host system call for now
    ssize_t result = read(fd, buf, count);
    if (result == -1) {
        printf("mirix_sys_read: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_read: success, bytes_read=%zd\n", result);
    }
    
    return result;
}

ssize_t mirix_sys_write(int fd, const void *buf, size_t count) {
    printf("mirix_sys_write: fd=%d, buf=%p, count=%zu\n", fd, buf, count);
    
    // Use host system call for now
    ssize_t result = write(fd, buf, count);
    if (result == -1) {
        printf("mirix_sys_write: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_write: success, bytes_written=%zd\n", result);
    }
    
    return result;
}

int mirix_sys_open(const char *pathname, int flags, mode_t mode) {
    printf("mirix_sys_open: pathname=%s, flags=0x%x, mode=0%o\n", pathname, flags, mode);
    
    // Use host system call for now
    int result = open(pathname, flags, mode);
    if (result == -1) {
        printf("mirix_sys_open: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_open: success, fd=%d\n", result);
    }
    
    return result;
}

int mirix_sys_close(int fd) {
    printf("mirix_sys_close: fd=%d\n", fd);
    
    // Use host system call for now
    int result = close(fd);
    if (result == -1) {
        printf("mirix_sys_close: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_close: success\n");
    }
    
    return result;
}

pid_t mirix_sys_fork(void) {
    printf("mirix_sys_fork: creating new process\n");
    
    // Use host system call for now
    pid_t result = fork();
    if (result == -1) {
        printf("mirix_sys_fork: failed, errno=%d\n", errno);
    } else if (result == 0) {
        printf("mirix_sys_fork: child process created\n");
    } else {
        printf("mirix_sys_fork: parent process, child_pid=%d\n", result);
    }
    
    return result;
}

int mirix_sys_execve(const char *pathname, char *const argv[], char *const envp[]) {
    printf("mirix_sys_execve: pathname=%s\n", pathname);
    
    // Use host system call for now
    int result = execve(pathname, argv, envp);
    if (result == -1) {
        printf("mirix_sys_execve: failed, errno=%d\n", errno);
    }
    
    return result;
}

pid_t mirix_sys_waitpid(pid_t pid, int *status, int options) {
    printf("mirix_sys_waitpid: pid=%d, options=0x%x\n", pid, options);
    
    // Use host system call for now
    pid_t result = waitpid(pid, status, options);
    if (result == -1) {
        printf("mirix_sys_waitpid: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_waitpid: success, pid=%d, status=0x%x\n", result, status ? *status : 0);
    }
    
    return result;
}

void mirix_sys_exit(int status) {
    printf("mirix_sys_exit: status=%d\n", status);
    exit(status);
}

int mirix_sys_kill(pid_t pid, int sig) {
    printf("mirix_sys_kill: pid=%d, sig=%d\n", pid, sig);
    
    // Use host system call for now
    int result = kill(pid, sig);
    if (result == -1) {
        printf("mirix_sys_kill: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_kill: success\n");
    }
    
    return result;
}

void* mirix_sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    printf("mirix_sys_mmap: addr=%p, length=%zu, prot=0x%x, flags=0x%x, fd=%d, offset=%lld\n",
           addr, length, prot, flags, fd, (long long)offset);
    
    // Use host system call for now
    void *result = mmap(addr, length, prot, flags, fd, offset);
    if (result == MAP_FAILED) {
        printf("mirix_sys_mmap: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_mmap: success, addr=%p\n", result);
    }
    
    return result;
}

int mirix_sys_munmap(void *addr, size_t length) {
    printf("mirix_sys_munmap: addr=%p, length=%zu\n", addr, length);
    
    // Use host system call for now
    int result = munmap(addr, length);
    if (result == -1) {
        printf("mirix_sys_munmap: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_munmap: success\n");
    }
    
    return result;
}

int mirix_sys_ioctl(int fd, unsigned long request, void *arg) {
    printf("mirix_sys_ioctl: fd=%d, request=0x%lx, arg=%p\n", fd, request, arg);
    
    // Use host system call for now
    int result = ioctl(fd, request, arg);
    if (result == -1) {
        printf("mirix_sys_ioctl: failed, errno=%d\n", errno);
    } else {
        printf("mirix_sys_ioctl: success, result=%d\n", result);
    }
    
    return result;
}

// IPC system calls
int mirix_sys_ipc_send(uint32_t target_pid, const void *msg, size_t msg_size, int flags) {
    printf("mirix_sys_ipc_send: target_pid=%u, msg=%p, msg_size=%zu, flags=0x%x\n",
           target_pid, msg, msg_size, flags);
    
    // This would interface with the IPC system
    // For now, return success
    printf("mirix_sys_ipc_send: success\n");
    return 0;
}

int mirix_sys_ipc_recv(uint32_t sender_pid, void *msg, size_t *msg_size, int flags) {
    printf("mirix_sys_ipc_recv: sender_pid=%u, msg=%p, msg_size=%p, flags=0x%x\n",
           sender_pid, msg, msg_size, flags);
    
    // This would interface with the IPC system
    // For now, return no message
    printf("mirix_sys_ipc_recv: no message available\n");
    return -1;
}

// Timer system calls
int mirix_sys_timer_create(uint64_t interval_ms, int flags) {
    printf("mirix_sys_timer_create: interval_ms=%llu, flags=0x%x\n",
           (unsigned long long)interval_ms, flags);
    
    // This would interface with the timer system
    // For now, return a fake timer ID
    printf("mirix_sys_timer_create: success, timer_id=1\n");
    return 1;
}

int mirix_sys_timer_delete(int timer_id) {
    printf("mirix_sys_timer_delete: timer_id=%d\n", timer_id);
    
    // This would interface with the timer system
    // For now, return success
    printf("mirix_sys_timer_delete: success\n");
    return 0;
}
