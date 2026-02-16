#ifndef MIRIX_SYSCALL_H
#define MIRIX_SYSCALL_H

#include <stdint.h>
#include <stdbool.h>
#include "../kernel.h"
#include "../ipc/ipc.h"
#include "../host/host_interface.h"

// Syscall API
int syscall_init(void);
void syscall_cleanup(void);

// Syscall implementations
int syscall_exit(int exit_code);
int syscall_write(mirix_write_args_t *args);
int syscall_read(mirix_read_args_t *args);
int syscall_ipc_send(mirix_ipc_args_t *args);
int syscall_ipc_recv(mirix_ipc_args_t *args);
int syscall_fork(void);
int syscall_exec(const char *path, char *const argv[]);
int syscall_wait(int *status);
int syscall_timer_create(uint64_t interval_ms, bool periodic);
int syscall_timer_delete(int timer_fd);

// Internal syscall handlers
static void syscall_exit_impl(void *args);
static void syscall_write_impl(void *args);
static void syscall_read_impl(void *args);
static void syscall_ipc_send_impl(void *args);
static void syscall_ipc_recv_impl(void *args);
static void syscall_fork_impl(void *args);
static void syscall_exec_impl(void *args);
static void syscall_wait_impl(void *args);
static void syscall_timer_create_impl(void *args);
static void syscall_timer_delete_impl(void *args);

#endif // MIRIX_SYSCALL_H
