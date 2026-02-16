#ifndef MIRIX_LIBSYSCALL_H
#define MIRIX_LIBSYSCALL_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

// System call wrapper functions for MIRIX

// File operations
ssize_t mirix_sys_read(int fd, void *buf, size_t count);
ssize_t mirix_sys_write(int fd, const void *buf, size_t count);
int mirix_sys_open(const char *pathname, int flags, mode_t mode);
int mirix_sys_close(int fd);
int mirix_sys_ioctl(int fd, unsigned long request, void *arg);

// Process operations
pid_t mirix_sys_fork(void);
int mirix_sys_execve(const char *pathname, char *const argv[], char *const envp[]);
pid_t mirix_sys_waitpid(pid_t pid, int *status, int options);
void mirix_sys_exit(int status);
int mirix_sys_kill(pid_t pid, int sig);

// Memory operations
void* mirix_sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int mirix_sys_munmap(void *addr, size_t length);

// IPC operations
int mirix_sys_ipc_send(uint32_t target_pid, const void *msg, size_t msg_size, int flags);
int mirix_sys_ipc_recv(uint32_t sender_pid, void *msg, size_t *msg_size, int flags);

// Timer operations
int mirix_sys_timer_create(uint64_t interval_ms, int flags);
int mirix_sys_timer_delete(int timer_id);

#endif // MIRIX_LIBSYSCALL_H
