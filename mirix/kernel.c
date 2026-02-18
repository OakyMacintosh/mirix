#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/stat.h>

#include "kernel.h"
#include "kernel_args.h"
#include "host/host_interface.h"
#include "ipc/ipc.h"
#include "syscall/syscall.h"
#include "posix/posix.h"
#include "drivers/lazyfs.h"
#include "bsd/bsd_proc.h"

#ifdef MACH_KERNEL_INTEGRATION
#include "mach/mach.h"
#include "mach/mach_error.h"
#include "mach/mach_init.h"
extern kern_return_t mach_kernel_init(void);
extern void mach_kernel_stats(void);
extern void mach_kernel_cleanup(void);
#endif

#ifdef MACH_USERSPACE_INTEGRATION
#include "mach/mach.h"
#include "mach/mach_error.h"
#include "mach/mach_init.h"
extern kern_return_t mach_userspace_init(void);
extern void mach_userspace_stats(void);
extern void mach_userspace_cleanup(void);
#endif

// Forward declarations
static void kernel_main_loop(void);
void scheduler_tick(void);
void kernel_panic(const char *message);

// Global kernel state
static mirix_kernel_state_t kernel_state;

// Store kernel arguments globally for userland access
static mirix_kernel_args_t *kernel_args = NULL;

// Kernel panic function
void kernel_panic(const char *message) {
    printf("\n");
    printf("**** KERNEL PANIC ****\n");
    printf("%s\n", message);
    printf("System halted!\n");
    
    // Set kernel state to panicked
    kernel_state.status = MIRIX_KERNEL_STOPPED;
    
    // Stay running until the break keystroke is invoqued.
    exit(0);
}

// Debug shell implementation
static void mirix_debug_shell(void) {
    printf("MIRIX Debug Shell\n");
    printf("(debug)%% Type 'exit' to return to kernel\n");
    printf("(debug)%% Commands: help, mem, ps, fs, sys, kern, net, exit\n");
    
    char line[256];
    while (1) {
        printf("(debug)%% ");
        fflush(stdout);
        
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        if (strcmp(line, "exit") == 0) {
            printf("(debug)%% Exiting debug shell\n");
            break;
        } else if (strcmp(line, "help") == 0) {
            printf("(debug)%% Available commands:\n");
            printf("(debug)%%   kern    - Show kernel state\n");
            printf("(debug)%%   net     - Show network status\n");
            printf("(debug)%%   sus     - Show SUS compliance status\n");
            printf("(debug)%%   exit    - Exit debug shell\n");
            printf("(debug)%%   fs      - Show filesystem info\n");
            printf("(debug)%%   sys     - Show system information\n");
            printf("(debug)%%   kern    - Show kernel state\n");
            printf("(debug)%%   net     - Show network status\n");
#ifdef MACH_KERNEL_INTEGRATION
            printf("(debug)%%   mach    - Show Mach kernel statistics\n");
#endif
#ifdef MACH_USERSPACE_INTEGRATION
            printf("(debug)%%   umach   - Show Mach userspace statistics\n");
#endif
            printf("(debug)%%   exit    - Exit debug shell\n");
        } else if (strcmp(line, "mem") == 0) {
            printf("(debug)%% Memory usage:\n");
            printf("(debug)%%   Kernel: %zu MB\n", kernel_args ? kernel_args->alloc_size / (1024*1024) : 512);
            printf("(debug)%%   Available: %zu MB\n", 1024); // Placeholder
            printf("(debug)%%   Used: %zu MB\n", kernel_args ? kernel_args->alloc_size / (1024*1024) : 512);
        } else if (strcmp(line, "ps") == 0) {
            printf("(debug)%% Process list:\n");
            printf("(debug)%%   PID %d: kernel (parent)\n", kernel_state.pid);
            printf("(debug)%%   No userland processes running\n");
        } else if (strcmp(line, "fs") == 0) {
            printf("(debug)%% Filesystem information:\n");
            if (kernel_args && kernel_args->root_filesystem) {
                printf("(debug)%%   Root: %s\n", kernel_args->root_filesystem);
                printf("(debug)%%   Type: LazyFS (case-sensitive)\n");
                printf("(debug)%%   Status: Mounted\n");
            } else {
                printf("(debug)%%   No filesystem mounted\n");
            }
        } else if (strcmp(line, "sys") == 0) {
            printf("(debug)%% System information:\n");
            printf("(debug)%%   Host interface: kqueue active\n");
            printf("(debug)%%   IPC system: Shared memory active\n");
            printf("(debug)%%   POSIX compliance: %s\n", sus_check_compliance() == 0 ? "SUSv3 Core" : "Incomplete");
            printf("(debug)%%   SUS version: %s\n", sus_version_string());
            printf("(debug)%%   Architecture: %s\n", "x86_64");
            printf("(debug)%%   Kernel: MIRIX v0.1\n");
            printf("(debug)%%   Build: DEBUG\n");
        } else if (strcmp(line, "kern") == 0) {
            printf("(debug)%% Kernel state:\n");
            printf("(debug)%%   Status: %s\n", 
                   kernel_state.status == MIRIX_KERNEL_INITIALIZING ? "Initializing" :
                   kernel_state.status == MIRIX_KERNEL_RUNNING ? "Running" :
                   kernel_state.status == MIRIX_KERNEL_SHUTTING_DOWN ? "Shutting down" :
                   kernel_state.status == MIRIX_KERNEL_STOPPED ? "Stopped" : "Unknown");
            printf("(debug)%%   PID: %d\n", kernel_state.pid);
            printf("(debug)%%   Services: Host, IPC, Syscall, POSIX, LazyFS");
#ifdef MACH_KERNEL_INTEGRATION
            printf(", Mach Kernel");
#endif
#ifdef MACH_USERSPACE_INTEGRATION
            printf(", Mach Userspace");
#endif
            printf("\n");
            
#ifdef MACH_KERNEL_INTEGRATION
            printf("(debug)%%   Mach kernel statistics:\n");
            mach_kernel_stats();
#endif

#ifdef MACH_USERSPACE_INTEGRATION
            printf("(debug)%%   Mach userspace statistics:\n");
            mach_userspace_stats();
#endif
        } else if (strcmp(line, "mach") == 0) {
#ifdef MACH_KERNEL_INTEGRATION
            printf("(debug)%% Mach kernel statistics:\n");
            mach_kernel_stats();
#endif
        } else if (strcmp(line, "umach") == 0) {
#ifdef MACH_USERSPACE_INTEGRATION
            printf("(debug)%%   Mach userspace statistics:\n");
            mach_userspace_stats();
#endif
        } else if (strcmp(line, "sus") == 0) {
            printf("(debug)%% SUS Compliance Status:\n");
            printf("(debug)%%   SUS Version: %s\n", sus_version_string());
            printf("(debug)%%   SUS Compliance: %s\n", sus_is_compliant() ? "SUSv3" : "Incomplete");
            printf("(debug)%%   POSIX.1-2001: %s\n", sus_feature_test(_POSIX_VERSION) == 200112L ? "YES" : "NO");
            printf("(debug)%%   Threads: %s\n", sus_feature_test(_POSIX_THREADS) == 200809L ? "YES" : "NO");
            printf("(debug)%%   Semaphores: %s\n", sus_feature_test(_POSIX_SEMAPHORES) == 200809L ? "YES" : "NO");
            printf("(debug)%%   Shared Memory: %s\n", sus_feature_test(_POSIX_SHARED_MEMORY_OBJECTS) == 200809L ? "YES" : "NO");
            printf("(debug)%%   Message Passing: %s\n", sus_feature_test(_POSIX_MESSAGE_PASSING) == 200112L ? "YES" : "NO");
            printf("(debug)%%   Timers: %s\n", sus_feature_test(_POSIX_TIMERS) == 200809L ? "YES" : "NO");
        } else if (strcmp(line, "net") == 0) {
            printf("(debug)%% Network status:\n");
            printf("(debug)%%   Host interface: kqueue active\n");
            printf("(debug)%%   IPC system: Shared memory active\n");
            printf("(debug)%%   No network interfaces configured\n");
        } else if (strlen(line) > 0) {
            printf("(debug)%% Unknown command: %s\n", line);
            printf("(debug)%% Type 'help' for available commands\n");
        }
    }
}

// Execute init program
static void execute_init_program(void) {
    if (!kernel_args || !kernel_args->init_program) {
        kernel_panic("No init program available");
        return;
    }
    
    printf("Executing: %s\n", kernel_args->init_program);
    
    // Fork and execute init program
    pid_t pid = fork();
    if (pid == 0) {
        // Child process - execute init
        char *args[] = {kernel_args->init_program, NULL};
        execv(kernel_args->init_program, args);
        
        // If we get here, exec failed
        kernel_panic("Failed to execute init program");
        exit(1);
    } else if (pid > 0) {
        // Parent process - wait for init
        int status;
        waitpid(pid, &status, 0);
        printf("Init program exited with status: %d\n", status);
        
        // If init exits, shutdown the kernel
        printf("Init program terminated, shutting down kernel...\n");
        kernel_state.status = MIRIX_KERNEL_SHUTTING_DOWN;
    } else {
        kernel_panic("Failed to fork for init program");
    }
}

// Main kernel entry point (legacy)
int mirix_kernel_main(int argc, char **argv) {
    // For backward compatibility, parse args and call new function
    mirix_kernel_args_t *args = parse_kernel_args(argc, argv);
    if (!args) {
        return 1;
    }
    
    int result = mirix_kernel_main_with_args(args);
    free_kernel_args(args);
    return result;
}

// Main kernel entry point with arguments
int mirix_kernel_main_with_args(mirix_kernel_args_t *args) {
    // Store kernel arguments globally
    kernel_args = args;
    
    // XNU-style boot messages
    printf("now loading...\n");
    
    // Initialize kernel state
    memset(&kernel_state, 0, sizeof(mirix_kernel_state_t));
    kernel_state.status = MIRIX_KERNEL_INITIALIZING;
    kernel_state.pid = getpid();
    
    // XNU-style initialization sequence with minimal output
    printf("Starting services...\n");
    
#ifdef MACH_KERNEL_INTEGRATION
    printf("Initializing Mach kernel services...\n");
    kern_return_t kr = mach_kernel_init();
    if (kr != KERN_SUCCESS) {
        kernel_panic("Failed to initialize Mach kernel services");
        free_kernel_args(args);
        return -1;
    }
    printf("Mach kernel services ready\n");
#endif

#ifdef MACH_USERSPACE_INTEGRATION
    printf("[i] Initializing Mach userspace services...\n");
    kern_return_t kr = mach_userspace_init();
    if (kr != KERN_SUCCESS) {
        kernel_panic("[err] Failed to initialize Mach userspace services");
        free_kernel_args(args);
        return -1;
    }
    printf("[i] Mach userspace services ready\n");
#endif
    
    if (host_interface_init() != 0) {
        kernel_panic("[err] Failed to initialize host interface");
        free_kernel_args(args);
        return -1;
    }
    
    if (ipc_system_init() != 0) {
        kernel_panic("[err] Failed to initialize IPC system");
        free_kernel_args(args);
        return -1;
    }
    
    if (syscall_init() != 0) {
        kernel_panic("[err] Failed to initialize syscall system");
        free_kernel_args(args);
        return -1;
    }
    
    if (posix_init() != 0) {
        kernel_panic("[err] Failed to initialize POSIX compatibility layer");
        free_kernel_args(args);
        return -1;
    }

    // Initialize BSD process management
    bsd_proc_init();
    
    // Check for root filesystem
    if (args && args->root_filesystem) {
        printf("Mounting root filesystem: %s\n", args->root_filesystem);
        
        // Check if root filesystem directory exists
        struct stat st;
        if (stat(args->root_filesystem, &st) != 0) {
            kernel_panic("Root filesystem directory does not exist");
            free_kernel_args(args);
            return -1;
        }
        
        if (!S_ISDIR(st.st_mode)) {
            kernel_panic("Root filesystem path is not a directory");
            free_kernel_args(args);
            return -1;
        }
        
        // Check for basic Unix filesystem structure
        char bin_path[512];
        char etc_path[512];
        char lib_path[512];
        char tmp_path[512];
        
        snprintf(bin_path, sizeof(bin_path), "%s/bin", args->root_filesystem);
        snprintf(etc_path, sizeof(etc_path), "%s/etc", args->root_filesystem);
        snprintf(lib_path, sizeof(lib_path), "%s/lib", args->root_filesystem);
        snprintf(tmp_path, sizeof(tmp_path), "%s/tmp", args->root_filesystem);
        
        if (stat(bin_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
            kernel_panic("Root filesystem missing /bin directory");
            free_kernel_args(args);
            return -1;
        }
        
        if (stat(etc_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
            kernel_panic("Root filesystem missing /etc directory");
            free_kernel_args(args);
            return -1;
        }
        
        if (stat(lib_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
            kernel_panic("Root filesystem missing /lib directory");
            free_kernel_args(args);
            return -1;
        }
        
        if (stat(tmp_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
            kernel_panic("Root filesystem missing /tmp directory");
            free_kernel_args(args);
            return -1;
        }
        
        if (lazyfs_init(args->lazyfs_backing_file, true) != 0) {
            kernel_panic("Failed to initialize root filesystem");
            free_kernel_args(args);
            return -1;
        }
        
        printf("Root filesystem mounted successfully\n");
    } else {
        kernel_panic("No root filesystem specified");
        free_kernel_args(args);
        return -1;
    }
    
    kernel_state.status = MIRIX_KERNEL_RUNNING;
    printf("MIRIX kernel ready\n");
    
    if (args && args->verbose) {
        printf("Configuration: %d CPUs, %zu MB memory\n", 
               args->cpu_count, args->alloc_size / (1024 * 1024));
    }
    
    // Main kernel loop
    kernel_main_loop();
    
    // Cleanup
    mirix_kernel_shutdown();
    free_kernel_args(args);
    
    return 0;
}

// Kernel main loop - handles events and scheduling
static void kernel_main_loop(void) {
    printf("Entering kernel main loop...\n");
    
    // Check if we have a root filesystem and init program
    if (kernel_args && kernel_args->root_filesystem) {
        printf("Root filesystem available, checking for init program...\n");
        
        if (kernel_args->init_program) {
            // Check if init program exists
            struct stat st;
            if (stat(kernel_args->init_program, &st) == 0) {
                printf("Starting init program: %s\n", kernel_args->init_program);
                execute_init_program();
                // If init exits, shutdown the kernel
                return;
            } else {
                printf("Init program not found: %s\n", kernel_args->init_program);
                printf("Falling back to debug shell...\n");
                mirix_debug_shell();
                // After debug shell, shutdown the kernel
                return;
            }
        } else {
            printf("No init program specified, entering debug shell...\n");
            mirix_debug_shell();
            // After debug shell, shutdown the kernel
            return;
        }
    } else {
        printf("No root filesystem, entering debug shell...\n");
        mirix_debug_shell();
        // After debug shell, shutdown the kernel
        return;
    }
    
    // Main kernel event loop (only reached if init program is running)
    while (kernel_state.status == MIRIX_KERNEL_RUNNING) {
        // Handle events
        host_interface_poll_events();
        
        // Schedule processes
        scheduler_tick();
        
        // Small delay to prevent CPU spinning
        usleep(10000); // 10ms
    }
}

// System call handler - dispatches to appropriate handlers
int mirix_syscall_handler(int syscall_num, void *args) {
    switch (syscall_num) {
        case MIRIX_SYSCALL_EXIT:
            return syscall_exit((int)(long)args);
        case MIRIX_SYSCALL_WRITE:
            return syscall_write((mirix_write_args_t*)args);
        case MIRIX_SYSCALL_READ:
            return syscall_read((mirix_read_args_t*)args);
        case MIRIX_SYSCALL_IPC_SEND:
            return syscall_ipc_send((mirix_ipc_args_t*)args);
        case MIRIX_SYSCALL_IPC_RECV:
            return syscall_ipc_recv((mirix_ipc_args_t*)args);
        default:
            fprintf(stderr, "Unknown syscall: %d\n", syscall_num);
            return -1;
    }
}

// Get kernel state
mirix_kernel_state_t* mirix_get_kernel_state(void) {
    return &kernel_state;
}

// Kernel shutdown
void mirix_kernel_shutdown(void) {
    kernel_state.status = MIRIX_KERNEL_SHUTTING_DOWN;
    
    // XNU-style cleanup sequence
#ifdef MACH_KERNEL_INTEGRATION
    printf("Shutting down Mach kernel services...\n");
    mach_kernel_cleanup();
#endif

#ifdef MACH_USERSPACE_INTEGRATION
    printf("Shutting down Mach userspace services...\n");
    mach_userspace_cleanup();
#endif
    
    lazyfs_cleanup();
    posix_cleanup();
    syscall_cleanup();
    ipc_system_cleanup();
    host_interface_cleanup();
    
    kernel_state.status = MIRIX_KERNEL_STOPPED;
}
