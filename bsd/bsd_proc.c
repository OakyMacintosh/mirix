#include "bsd_proc.h"
#include <string.h>
#include <stdio.h> // For debugging printfs

// Global array of processes (simple implementation for now)
struct proc all_procs[MAX_PROC];

// Initialize the process table
void bsd_proc_init(void) {
    memset(all_procs, 0, sizeof(all_procs));
    // Initialize proc 0 (kernel process)
    all_procs[0].p_pid = 0;
    all_procs[0].p_ppid = 0;
    all_procs[0].p_state = P_STATE_RUNNING;
    all_procs[0].p_comm = "kernel";
    // For MACH_KERNEL_INTEGRATION, assign a dummy task or the real kernel task
    // all_procs[0].p_task = ...;
    printf("BSD proc table initialized. Kernel proc (PID 0) created.\n");
}

// Find a proc by pid
struct proc* pfind(pid_t pid) {
    if (pid < 0 || pid >= MAX_PROC) {
        return NULL;
    }
    // In a real system, this would be a hash table or a more efficient lookup
    for (int i = 0; i < MAX_PROC; ++i) {
        if (all_procs[i].p_pid == pid && all_procs[i].p_state != P_STATE_UNUSED) {
            return &all_procs[i];
        }
    }
    return NULL;
}

// Allocate a new proc structure
struct proc* proc_alloc(void) {
    for (int i = 0; i < MAX_PROC; ++i) {
        if (all_procs[i].p_state == P_STATE_UNUSED) {
            memset(&all_procs[i], 0, sizeof(struct proc));
            all_procs[i].p_pid = i; // Simple PID assignment
            all_procs[i].p_state = P_STATE_EMBRYO;
            return &all_procs[i];
        }
    }
    fprintf(stderr, "Error: No free proc slots available!\n");
    return NULL; // No free slots
}

// Free a proc structure
void proc_free(struct proc* p) {
    if (p && p->p_pid >= 0 && p->p_pid < MAX_PROC) {
        p->p_state = P_STATE_UNUSED;
        p->p_pid = 0; // Clear PID
        // In a real system, further cleanup would be needed (e.g., resources, Mach task)
        printf("Proc (PID %d) freed.\n", p->p_pid);
    }
}