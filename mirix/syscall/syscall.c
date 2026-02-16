#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "syscall.h"

// Syscall system state
static struct {
    bool initialized;
    void (*syscall_table[256])(void *args);
} syscall_state;

// Initialize syscall interface
int syscall_init(void) {
    if (syscall_state.initialized) {
        return 0;
    }
    
    // Clear syscall table
    memset(syscall_state.syscall_table, 0, sizeof(syscall_state.syscall_table));
    
    // Register syscall handlers
    syscall_state.syscall_table[MIRIX_SYSCALL_EXIT] = syscall_exit_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_WRITE] = syscall_write_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_READ] = syscall_read_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_IPC_SEND] = syscall_ipc_send_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_IPC_RECV] = syscall_ipc_recv_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_FORK] = syscall_fork_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_EXEC] = syscall_exec_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_WAIT] = syscall_wait_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_TIMER_CREATE] = syscall_timer_create_impl;
    syscall_state.syscall_table[MIRIX_SYSCALL_TIMER_DELETE] = syscall_timer_delete_impl;
    
    syscall_state.initialized = true;
    return 0;
}

// Cleanup syscall system
void syscall_cleanup(void) {
    syscall_state.initialized = false;
}

// Syscall implementations
int syscall_exit(int exit_code) {
    printf("Process exit with code: %d\n", exit_code);
    exit(exit_code);
    return exit_code;
}

int syscall_write(mirix_write_args_t *args) {
    if (!args || !args->buf) {
        return -1;
    }
    
    // Write to file descriptor
    ssize_t result = write(args->fd, args->buf, args->count);
    
    printf("syscall_write: fd=%d, count=%zu, result=%zd\n", 
           args->fd, args->count, result);
    
    return (int)result;
}

int syscall_read(mirix_read_args_t *args) {
    if (!args || !args->buf) {
        return -1;
    }
    
    // Read from file descriptor
    ssize_t result = read(args->fd, args->buf, args->count);
    
    printf("syscall_read: fd=%d, count=%zu, result=%zd\n", 
           args->fd, args->count, result);
    
    return (int)result;
}

int syscall_ipc_send(mirix_ipc_args_t *args) {
    if (!args) {
        return -1;
    }
    
    return ipc_send_message(getpid(), args->target_pid, 
                           args->msg, args->msg_size, args->flags);
}

int syscall_ipc_recv(mirix_ipc_args_t *args) {
    if (!args) {
        return -1;
    }
    
    mirix_message_t msg;
    int result = ipc_receive_message(getpid(), &msg, true);
    
    if (result == 0) {
        // Copy received message to user buffer
        size_t copy_size = (msg.data_size < args->msg_size) ? msg.data_size : args->msg_size;
        memcpy((void*)args->msg, msg.data, copy_size);
        result = copy_size;
    }
    
    return result;
}

int syscall_fork(void) {
    printf("syscall_fork called\n");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        printf("Child process created\n");
        return 0;
    } else if (pid > 0) {
        printf("Parent process, child PID: %d\n", pid);
        return pid;
    } else {
        perror("fork");
        return -1;
    }
}

int syscall_exec(const char *path, char *const argv[]) {
    if (!path) {
        return -1;
    }
    
    printf("syscall_exec: path=%s\n", path);
    
    int result = execvp(path, argv);
    perror("execvp");
    return result;
}

int syscall_wait(int *status) {
    printf("syscall_wait called\n");
    
    int stat;
    pid_t pid = wait(&stat);
    
    if (pid > 0 && status) {
        *status = stat;
    }
    
    return pid;
}

int syscall_timer_create(uint64_t interval_ms, bool periodic) {
    printf("syscall_timer_create: interval=%llu, periodic=%d\n", 
           (unsigned long long)interval_ms, periodic);
    
    // Create timer using host interface
    return host_interface_create_timer(interval_ms, periodic, NULL, NULL);
}

int syscall_timer_delete(int timer_fd) {
    printf("syscall_timer_delete: timer_fd=%d\n", timer_fd);
    
    close(timer_fd);
    return 0;
}

// Internal syscall handlers
static void syscall_exit_impl(void *args) {
    int exit_code = (int)(long)args;
    syscall_exit(exit_code);
}

static void syscall_write_impl(void *args) {
    syscall_write((mirix_write_args_t*)args);
}

static void syscall_read_impl(void *args) {
    syscall_read((mirix_read_args_t*)args);
}

static void syscall_ipc_send_impl(void *args) {
    syscall_ipc_send((mirix_ipc_args_t*)args);
}

static void syscall_ipc_recv_impl(void *args) {
    syscall_ipc_recv((mirix_ipc_args_t*)args);
}

static void syscall_fork_impl(void *args) {
    syscall_fork();
}

static void syscall_exec_impl(void *args) {
    // Args would be a structure containing path and argv
    syscall_exec(NULL, NULL);
}

static void syscall_wait_impl(void *args) {
    syscall_wait((int*)args);
}

static void syscall_timer_create_impl(void *args) {
    // Args would be a structure containing timer parameters
    syscall_timer_create(0, false);
}

static void syscall_timer_delete_impl(void *args) {
    int timer_fd = (int)(long)args;
    syscall_timer_delete(timer_fd);
}
