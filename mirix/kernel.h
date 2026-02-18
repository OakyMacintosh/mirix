#ifndef MIRIX_KERNEL_H
#define MIRIX_KERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
#include "kernel_args.h"

#ifdef MACH_KERNEL_INTEGRATION
#include "mach/mach.h"
#endif

#ifdef MACH_USERSPACE_INTEGRATION
#include "mach/mach.h"
#endif

// Kernel status states
typedef enum {
    MIRIX_KERNEL_UNINITIALIZED = 0,
    MIRIX_KERNEL_INITIALIZING,
    MIRIX_KERNEL_RUNNING,
    MIRIX_KERNEL_SHUTTING_DOWN,
    MIRIX_KERNEL_STOPPED
} mirix_kernel_status_t;

// Kernel state structure
typedef struct {
    mirix_kernel_status_t status;
    pid_t pid;
    uint64_t uptime_ticks;
    uint32_t num_processes;
    void *memory_map;
    size_t memory_size;
} mirix_kernel_state_t;

// System call numbers
typedef enum {
    MIRIX_SYSCALL_EXIT = 1,
    MIRIX_SYSCALL_WRITE = 2,
    MIRIX_SYSCALL_READ = 3,
    MIRIX_SYSCALL_IPC_SEND = 4,
    MIRIX_SYSCALL_IPC_RECV = 5,
    MIRIX_SYSCALL_FORK = 6,
    MIRIX_SYSCALL_EXEC = 7,
    MIRIX_SYSCALL_WAIT = 8,
    MIRIX_SYSCALL_TIMER_CREATE = 9,
    MIRIX_SYSCALL_TIMER_DELETE = 10
} mirix_syscall_t;

// System call argument structures
typedef struct {
    const void *buf;
    size_t count;
    int fd;
} mirix_write_args_t;

typedef struct {
    void *buf;
    size_t count;
    int fd;
} mirix_read_args_t;

typedef struct {
    int target_pid;
    const void *msg;
    size_t msg_size;
    int flags;
} mirix_ipc_args_t;

// Timer structure
typedef struct {
    uint32_t timer_id;
    uint64_t expire_time;
    bool periodic;
    uint64_t interval;
    void (*callback)(void *data);
    void *callback_data;
} mirix_timer_t;

// Process structure
typedef struct {
    uint32_t pid;
    uint32_t ppid;
    char name[256];
    mirix_kernel_status_t status;
    void *stack_base;
    size_t stack_size;
    uint64_t runtime_ticks;
} mirix_process_t;

// Kernel API functions
int mirix_kernel_main(int argc, char **argv);
int mirix_kernel_main_with_args(mirix_kernel_args_t *args);
int mirix_syscall_handler(int syscall_num, void *args);
mirix_kernel_state_t* mirix_get_kernel_state(void);
void mirix_kernel_shutdown(void);
void kernel_panic(const char *message);

// Internal kernel functions
static void kernel_main_loop(void);
void scheduler_tick(void);
static void mirix_debug_shell(void);
static void execute_init_program(void);

#endif // MIRIX_KERNEL_H
