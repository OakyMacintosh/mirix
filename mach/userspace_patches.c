/*
 * MIRIX Mach Userspace Patches
 * 
 * This file contains patches to make Mach 4 code run in userspace
 * as part of the MIRIX lazykernel system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "mach/mach.h"
#include "mach/mach_error.h"
#include "mach/mach_init.h"

/* Userspace Mach port management */
typedef struct {
    mach_port_t port;
    char name[256];
    int ref_count;
    int is_kernel;
} mach_port_entry_t;

static mach_port_entry_t port_table[1024];
static int port_count = 0;

/* Userspace Mach task management */
typedef struct {
    mach_task_t task;
    pid_t pid;
    char name[256];
    int active;
} mach_task_entry_t;

static mach_task_entry_t task_table[64];
static int task_count = 0;

/* Userspace Mach message buffer management */
typedef struct {
    void *buffer;
    size_t size;
    int in_use;
} mach_msg_buffer_t;

static mach_msg_buffer_t msg_buffers[256];
static int msg_buffer_count = 0;

/* Initialize userspace Mach subsystem */
kern_return_t mach_userspace_init(void) {
    printf("MIRIX: Initializing userspace Mach subsystem\n");
    
    /* Clear tables */
    memset(port_table, 0, sizeof(port_table));
    memset(task_table, 0, sizeof(task_table));
    memset(msg_buffers, 0, sizeof(msg_buffers));
    
    /* Create special ports */
    port_table[0].port = MACH_PORT_NULL;
    strcpy(port_table[0].name, "MACH_PORT_NULL");
    port_table[0].ref_count = 1;
    port_count = 1;
    
    port_table[1].port = MACH_PORT_DEAD;
    strcpy(port_table[1].name, "MACH_PORT_DEAD");
    port_table[1].ref_count = 1;
    port_count = 2;
    
    /* Create kernel task */
    task_table[0].task = mach_task_self();
    task_table[0].pid = getpid();
    strcpy(task_table[0].name, "kernel_task");
    task_table[0].active = 1;
    task_count = 1;
    
    printf("MIRIX: Userspace Mach subsystem initialized\n");
    printf("MIRIX: %d ports, %d tasks available\n", port_count, task_count);
    
    return KERN_SUCCESS;
}

/* Userspace port allocation */
kern_return_t mach_port_allocate_userspace(mach_port_t *port, const char *name) {
    if (port_count >= 1024) {
        return KERN_RESOURCE_SHORTAGE;
    }
    
    /* Generate a unique port number */
    mach_port_t new_port = port_count + 1000;
    
    port_table[port_count].port = new_port;
    strncpy(port_table[port_count].name, name ? name : "unnamed", 255);
    port_table[port_count].ref_count = 1;
    port_table[port_count].is_kernel = 0;
    
    *port = new_port;
    port_count++;
    
    printf("MIRIX: Allocated port %d (%s)\n", new_port, name ? name : "unnamed");
    return KERN_SUCCESS;
}

/* Userspace port deallocation */
kern_return_t mach_port_deallocate_userspace(mach_port_t port) {
    for (int i = 0; i < port_count; i++) {
        if (port_table[i].port == port) {
            port_table[i].ref_count--;
            if (port_table[i].ref_count <= 0) {
                printf("MIRIX: Deallocated port %d (%s)\n", port, port_table[i].name);
                /* Remove from table */
                for (int j = i; j < port_count - 1; j++) {
                    port_table[j] = port_table[j + 1];
                }
                port_count--;
            }
            return KERN_SUCCESS;
        }
    }
    return KERN_INVALID_NAME;
}

/* Userspace message buffer allocation */
void *mach_msg_buffer_allocate(size_t size) {
    if (msg_buffer_count >= 256) {
        return NULL;
    }
    
    void *buffer = malloc(size);
    if (!buffer) {
        return NULL;
    }
    
    msg_buffers[msg_buffer_count].buffer = buffer;
    msg_buffers[msg_buffer_count].size = size;
    msg_buffers[msg_buffer_count].in_use = 1;
    msg_buffer_count++;
    
    return buffer;
}

/* Userspace message buffer deallocation */
void mach_msg_buffer_deallocate(void *buffer) {
    for (int i = 0; i < msg_buffer_count; i++) {
        if (msg_buffers[i].buffer == buffer) {
            free(buffer);
            /* Remove from table */
            for (int j = i; j < msg_buffer_count - 1; j++) {
                msg_buffers[j] = msg_buffers[j + 1];
            }
            msg_buffer_count--;
            return;
        }
    }
}

/* Userspace Mach message send */
kern_return_t mach_msg_send_userspace(mach_msg_header_t *msg, mach_msg_timeout_t timeout) {
    if (!msg) {
        return KERN_INVALID_ARGUMENT;
    }
    
    printf("MIRIX: Sending Mach message (msg_id=%d, size=%d)\n", 
           msg->msgh_id, msg->msgh_size);
    
    /* In userspace, we simulate message sending */
    /* For now, just return success */
    (void)timeout; /* Suppress unused warning */
    
    return KERN_SUCCESS;
}

/* Userspace Mach message receive */
kern_return_t mach_msg_receive_userspace(mach_msg_header_t *msg, mach_msg_timeout_t timeout) {
    if (!msg) {
        return KERN_INVALID_ARGUMENT;
    }
    
    printf("MIRIX: Receiving Mach message (timeout=%d)\n", timeout);
    
    /* In userspace, we simulate message receiving */
    /* For now, just return a dummy message */
    msg->msgh_bits = MACH_MSGH_BITS_LOCAL;
    msg->msgh_size = sizeof(mach_msg_header_t);
    msg->msgh_remote_port = MACH_PORT_NULL;
    msg->msgh_local_port = MACH_PORT_NULL;
    msg->msgh_voucher_port = MACH_PORT_NULL;
    msg->msgh_id = 0;
    
    return KERN_SUCCESS;
}

/* Userspace task creation */
kern_return_t task_create_userspace(mach_task_t *child_task, pid_t pid, const char *name) {
    if (task_count >= 64) {
        return KERN_RESOURCE_SHORTAGE;
    }
    
    /* Generate a unique task ID */
    mach_task_t new_task = task_count + 2000;
    
    task_table[task_count].task = new_task;
    task_table[task_count].pid = pid;
    strncpy(task_table[task_count].name, name ? name : "unnamed_task", 255);
    task_table[task_count].active = 1;
    
    *child_task = new_task;
    task_count++;
    
    printf("MIRIX: Created task %d (pid=%d, name=%s)\n", new_task, pid, name ? name : "unnamed_task");
    return KERN_SUCCESS;
}

/* Userspace task termination */
kern_return_t task_terminate_userspace(mach_task_t task) {
    for (int i = 0; i < task_count; i++) {
        if (task_table[i].task == task) {
            printf("MIRIX: Terminated task %d (pid=%d, name=%s)\n", 
                   task, task_table[i].pid, task_table[i].name);
            
            /* Remove from table */
            for (int j = i; j < task_count - 1; j++) {
                task_table[j] = task_table[j + 1];
            }
            task_count--;
            return KERN_SUCCESS;
        }
    }
    return KERN_INVALID_TASK;
}

/* Get Mach statistics */
void mach_userspace_stats(void) {
    printf("MIRIX Mach Userspace Statistics:\n");
    printf("  Ports: %d/1024\n", port_count);
    printf("  Tasks: %d/64\n", task_count);
    printf("  Message buffers: %d/256\n", msg_buffer_count);
    
    printf("Active ports:\n");
    for (int i = 0; i < port_count; i++) {
        printf("  %d: %s (ref_count=%d, kernel=%s)\n",
               port_table[i].port, port_table[i].name,
               port_table[i].ref_count,
               port_table[i].is_kernel ? "yes" : "no");
    }
    
    printf("Active tasks:\n");
    for (int i = 0; i < task_count; i++) {
        printf("  %d: pid=%d, name=%s, active=%s\n",
               task_table[i].task, task_table[i].pid,
               task_table[i].name,
               task_table[i].active ? "yes" : "no");
    }
}

/* Cleanup userspace Mach subsystem */
void mach_userspace_cleanup(void) {
    printf("MIRIX: Cleaning up userspace Mach subsystem\n");
    
    /* Free all message buffers */
    for (int i = 0; i < msg_buffer_count; i++) {
        if (msg_buffers[i].buffer) {
            free(msg_buffers[i].buffer);
        }
    }
    
    /* Clear all tables */
    memset(port_table, 0, sizeof(port_table));
    memset(task_table, 0, sizeof(task_table));
    memset(msg_buffers, 0, sizeof(msg_buffers));
    
    port_count = 0;
    task_count = 0;
    msg_buffer_count = 0;
    
    printf("MIRIX: Userspace Mach subsystem cleaned up\n");
}
