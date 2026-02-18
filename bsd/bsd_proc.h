#ifndef _BSD_PROC_H_
#define _BSD_PROC_H_

#include <sys/types.h> // For pid_t
#include <stdbool.h>   // For bool

// Forward declaration of Mach task structure (if Mach integration is enabled)
#ifdef MACH_KERNEL_INTEGRATION
struct task;
#endif

// Process states (simplified)
typedef enum {
    P_STATE_UNUSED = 0, // Entry is unused
    P_STATE_EMBRYO,     // Process is being created
    P_STATE_RUNNING,    // Running or runnable
    P_STATE_SLEEPING,   // Sleeping, awaiting an event
    P_STATE_STOPPED,    // Stopped, e.g., by a signal
    P_STATE_ZOMBIE      // Terminated but not reaped by parent
} proc_state_t;

// The core BSD process structure
struct proc {
    pid_t           p_pid;      // Process ID
    pid_t           p_ppid;     // Parent process ID
    proc_state_t    p_state;    // Process state
    const char     *p_comm;     // Command name (for ps, etc.)

    // Placeholder for Mach task (if Mach integration is enabled)
#ifdef MACH_KERNEL_INTEGRATION
    struct task    *p_task;     // Associated Mach task
#endif

    // Add other essential fields as needed
    // void           *p_vmspace;  // Process virtual memory space
    // void           *p_uthread;  // Userland thread info
    // ...
};

// Global array or list of processes (simplified for initial implementation)
#define MAX_PROC 1024
extern struct proc all_procs[MAX_PROC];

// Function to find a proc by pid
struct proc* pfind(pid_t pid);

#endif // _BSD_PROC_H_
