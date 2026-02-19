#ifndef MIRIX_DOS_SYS_SYSCALL_H
#define MIRIX_DOS_SYS_SYSCALL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Stub definitions so DOS builds can include <sys/syscall.h>. */

#define SYS_read        0
#define SYS_write       1
#define SYS_open        2
#define SYS_close       3
#define SYS_exit        4
#define SYS_fork        5
#define SYS_execve      6
#define SYS_wait4       7
#define SYS_kill        8
#define SYS_mmap        9
#define SYS_munmap      10
#define SYS_brk         11
#define SYS_ioctl       12
#define SYS_socket      13
#define SYS_bind        14
#define SYS_listen      15
#define SYS_accept      16
#define SYS_connect     17
#define SYS_send        18
#define SYS_recv        19
#define SYS_ipc_send    20
#define SYS_ipc_recv    21
#define SYS_timer_create 22
#define SYS_timer_delete 23

static inline long syscall(long number, ...) {
    (void)number;
    return -1;
}

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_SYS_SYSCALL_H */
