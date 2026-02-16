#ifndef MIRIX_IPC_H
#define MIRIX_IPC_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Maximum IPC message data size
#define MIRIX_IPC_MAX_DATA_SIZE 4096

// IPC message structure
typedef struct {
    uint32_t sender_pid;
    uint32_t receiver_pid;
    uint64_t timestamp;
    uint32_t flags;
    size_t data_size;
    uint8_t data[MIRIX_IPC_MAX_DATA_SIZE];
} mirix_message_t;

// Message queue structure
typedef struct {
    mirix_message_t *messages;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
} mirix_message_queue_t;

// IPC API
int ipc_system_init(void);
void ipc_system_cleanup(void);

int ipc_send_message(uint32_t sender_pid, uint32_t receiver_pid, 
                    const void *data, size_t data_size, uint32_t flags);
int ipc_receive_message(uint32_t receiver_pid, mirix_message_t *msg, bool block);
void ipc_process_messages(void);

// Internal helper
static uint64_t get_current_timestamp(void);

#endif // MIRIX_IPC_H
