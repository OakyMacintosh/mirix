#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include "ipc.h"

// IPC system state
static struct {
    bool initialized;
    void *shared_memory;
    size_t shared_memory_size;
    int shm_fd;
    sem_t *message_sem;
    mirix_message_queue_t message_queue;
} ipc_state;

// Initialize IPC system
int ipc_system_init(void) {
    if (ipc_state.initialized) {
        return 0;
    }
    
    // Use malloc instead of shared memory for simplicity
    ipc_state.shared_memory_size = 1024 * 1024; // 1MB
    ipc_state.shared_memory = malloc(ipc_state.shared_memory_size);
    if (!ipc_state.shared_memory) {
        return -1;
    }
    
    ipc_state.shm_fd = -1; // Not using shared memory
    
    // Initialize message queue
    ipc_state.message_queue.messages = (mirix_message_t*)ipc_state.shared_memory;
    ipc_state.message_queue.capacity = ipc_state.shared_memory_size / sizeof(mirix_message_t);
    ipc_state.message_queue.head = 0;
    ipc_state.message_queue.tail = 0;
    ipc_state.message_queue.count = 0;
    
    // Create semaphore for message synchronization
    ipc_state.message_sem = sem_open("/mirix_ipc_sem", O_CREAT, 0666, 1);
    if (ipc_state.message_sem == SEM_FAILED) {
        free(ipc_state.shared_memory);
        return -1;
    }
    
    ipc_state.initialized = true;
    return 0;
}

// Cleanup IPC system
void ipc_system_cleanup(void) {
    if (ipc_state.message_sem != SEM_FAILED) {
        sem_close(ipc_state.message_sem);
        sem_unlink("/mirix_ipc_sem");
    }
    
    if (ipc_state.shared_memory) {
        free(ipc_state.shared_memory);
    }
    
    if (ipc_state.shm_fd != -1) {
        close(ipc_state.shm_fd);
        shm_unlink("/mirix_ipc");
    }
    
    ipc_state.initialized = false;
}

// Send IPC message
int ipc_send_message(uint32_t sender_pid, uint32_t receiver_pid, 
                    const void *data, size_t data_size, uint32_t flags) {
    if (!ipc_state.initialized) {
        return -1;
    }
    
    sem_wait(ipc_state.message_sem);
    
    // Check if queue is full
    if (ipc_state.message_queue.count >= ipc_state.message_queue.capacity) {
        sem_post(ipc_state.message_sem);
        return -1; // Queue full
    }
    
    // Create message
    mirix_message_t *msg = &ipc_state.message_queue.messages[ipc_state.message_queue.tail];
    msg->sender_pid = sender_pid;
    msg->receiver_pid = receiver_pid;
    msg->timestamp = get_current_timestamp();
    msg->flags = flags;
    msg->data_size = data_size;
    
    // Copy data if it fits
    if (data_size <= MIRIX_IPC_MAX_DATA_SIZE) {
        memcpy(msg->data, data, data_size);
    } else {
        // For large messages, we'd need a different approach
        sem_post(ipc_state.message_sem);
        return -1;
    }
    
    // Update queue
    ipc_state.message_queue.tail = (ipc_state.message_queue.tail + 1) % ipc_state.message_queue.capacity;
    ipc_state.message_queue.count++;
    
    sem_post(ipc_state.message_sem);
    
    printf("IPC message sent from %u to %u (size: %zu)\n", 
           sender_pid, receiver_pid, data_size);
    
    return 0;
}

// Receive IPC message
int ipc_receive_message(uint32_t receiver_pid, mirix_message_t *msg, bool block) {
    if (!ipc_state.initialized || !msg) {
        return -1;
    }
    
    while (true) {
        sem_wait(ipc_state.message_sem);
        
        // Check if there's a message for this receiver
        int found_index = -1;
        for (int i = 0; i < ipc_state.message_queue.count; i++) {
            int index = (ipc_state.message_queue.head + i) % ipc_state.message_queue.capacity;
            mirix_message_t *candidate = &ipc_state.message_queue.messages[index];
            
            if (candidate->receiver_pid == receiver_pid) {
                found_index = index;
                break;
            }
        }
        
        if (found_index != -1) {
            // Copy message
            mirix_message_t *src_msg = &ipc_state.message_queue.messages[found_index];
            memcpy(msg, src_msg, sizeof(mirix_message_t));
            
            // Remove message from queue
            // Shift remaining messages
            for (int i = found_index; i != ipc_state.message_queue.head; 
                 i = (i - 1 + ipc_state.message_queue.capacity) % ipc_state.message_queue.capacity) {
                int prev = (i - 1 + ipc_state.message_queue.capacity) % ipc_state.message_queue.capacity;
                memcpy(&ipc_state.message_queue.messages[i], 
                       &ipc_state.message_queue.messages[prev], 
                       sizeof(mirix_message_t));
            }
            
            ipc_state.message_queue.head = (ipc_state.message_queue.head + 1) % ipc_state.message_queue.capacity;
            ipc_state.message_queue.count--;
            
            sem_post(ipc_state.message_sem);
            
            printf("IPC message received by %u (from %u, size: %zu)\n", 
                   receiver_pid, msg->sender_pid, msg->data_size);
            
            return 0;
        }
        
        sem_post(ipc_state.message_sem);
        
        if (!block) {
            return -1; // No message available and non-blocking
        }
        
        // Block and wait for message
        usleep(1000); // 1ms
    }
}

// Process pending IPC messages
void ipc_process_messages(void) {
    if (!ipc_state.initialized) {
        return;
    }
    
    // This would be called by the kernel main loop to handle
    // any pending message processing or routing
    // For now, it's a placeholder
}

// Get current timestamp (simplified)
static uint64_t get_current_timestamp(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}
