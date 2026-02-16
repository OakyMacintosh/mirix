#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "kernel.h"

// Simple round-robin scheduler
static struct {
    mirix_process_t *processes;
    size_t max_processes;
    size_t current_process;
    uint64_t quantum_ticks;
    uint64_t tick_count;
} scheduler_state;

// Initialize scheduler
int scheduler_init(void) {
    printf("Initializing scheduler...\n");
    
    scheduler_state.max_processes = 256;
    scheduler_state.processes = calloc(scheduler_state.max_processes, sizeof(mirix_process_t));
    if (!scheduler_state.processes) {
        return -1;
    }
    
    scheduler_state.current_process = 0;
    scheduler_state.quantum_ticks = 1000; // 1ms quantum
    scheduler_state.tick_count = 0;
    
    printf("Scheduler initialized\n");
    return 0;
}

// Scheduler tick - called from kernel main loop
void scheduler_tick(void) {
    scheduler_state.tick_count++;
    
    // Simple round-robin scheduling
    if (scheduler_state.tick_count % scheduler_state.quantum_ticks == 0) {
        // Time to switch to next process
        scheduler_state.current_process = (scheduler_state.current_process + 1) % scheduler_state.max_processes;
        
        // Find next runnable process
        while (scheduler_state.processes[scheduler_state.current_process].status != MIRIX_KERNEL_RUNNING) {
            scheduler_state.current_process = (scheduler_state.current_process + 1) % scheduler_state.max_processes;
            
            // If we've looped back to the start, break to avoid infinite loop
            if (scheduler_state.current_process == 0) {
                break;
            }
        }
    }
}

// Add new process to scheduler
int scheduler_add_process(const mirix_process_t *process) {
    if (!process) {
        return -1;
    }
    
    // Find empty slot
    for (size_t i = 0; i < scheduler_state.max_processes; i++) {
        if (scheduler_state.processes[i].pid == 0) {
            memcpy(&scheduler_state.processes[i], process, sizeof(mirix_process_t));
            printf("Process added to scheduler: PID %u\n", process->pid);
            return 0;
        }
    }
    
    return -1; // No free slots
}

// Remove process from scheduler
int scheduler_remove_process(uint32_t pid) {
    for (size_t i = 0; i < scheduler_state.max_processes; i++) {
        if (scheduler_state.processes[i].pid == pid) {
            memset(&scheduler_state.processes[i], 0, sizeof(mirix_process_t));
            printf("Process removed from scheduler: PID %u\n", pid);
            return 0;
        }
    }
    
    return -1; // Process not found
}

// Get current process
mirix_process_t* scheduler_get_current_process(void) {
    return &scheduler_state.processes[scheduler_state.current_process];
}

// Cleanup scheduler
void scheduler_cleanup(void) {
    if (scheduler_state.processes) {
        free(scheduler_state.processes);
        scheduler_state.processes = NULL;
    }
}
