#ifndef MACH_H
#define MACH_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Mach return codes
typedef int kern_return_t;

#define KERN_SUCCESS                0
#define KERN_INVALID_ADDRESS        1
#define KERN_PROTECTION_FAILURE      2
#define KERN_NO_SPACE              3
#define KERN_INVALID_ARGUMENT       4
#define KERN_FAILURE               5
#define KERN_RESOURCE_SHORTAGE     6
#define KERN_NOT_RECEIVER          7
#define KERN_NO_ACCESS            8
#define KERN_MEMORY_FAILURE       9
#define KERN_MEMORY_ERROR         10
#define KERN_ALREADY_IN_SET       11
#define KERN_NOT_IN_SET           12
#define KERN_NAME_EXISTS          13
#define KERN_ABORTED             14
#define KERN_INVALID_NAME         15
#define KERN_INVALID_TASK         16
#define KERN_INVALID_RIGHT        17
#define KERN_INVALID_VALUE        18
#define KERN_UREFS              19
#define KERN_INVALID_CAPABILITY   20
#define KERN_RIGHT_EXISTS        21
#define KERN_INVALID_HOST        22
#define KERN_MEMORY_PRESENT       23
#define KERN_MEMORY_DATA_MOVED   24
#define KERN_MEMORY_RESTART_COPY 25
#define KERN_INVALID_PROCESSOR_SET 26
#define KERN_POLICY_LIMIT         27
#define KERN_INVALID_POLICY       28
#define KERN_INVALID_OBJECT       29
#define KERN_ALREADY_WAITING     30
#define KERN_ALREADY_SET         31
#define KERN_NOT_SET             32
#define KERN_NOT_LOCKED          34
#define KERN_NOT_UNLOCKED        35
#define KERN_EXISTS             36
#define KERN_TERMINATED          37
#define KERN_SET_ATTRIBUTES      40
#define KERN_NOT_SUPPORTED       41
#define KERN_NO_DATA            42
#define KERN_INVALID_RIGHT       43
#define KERN_INVALID_CAPABILITY  44
#define KERN_POLICY_STATIC       45

// Mach port types
typedef unsigned int mach_port_t;

#define MACH_PORT_NULL            0
#define MACH_PORT_DEAD            ((mach_port_t)~0)

// Mach task types
typedef unsigned int mach_task_t;

#define MACH_TASK_NULL            0

// Mach message types
typedef struct {
    mach_msg_bits_t       msgh_bits;
    mach_msg_size_t       msgh_size;
    mach_port_t           msgh_remote_port;
    mach_port_t           msgh_local_port;
    mach_port_t           msgh_voucher_port;
    mach_msg_id_t         msgh_id;
} mach_msg_header_t;

typedef unsigned int mach_msg_bits_t;
typedef unsigned int mach_msg_size_t;
typedef unsigned int mach_msg_id_t;

// Mach message bits
#define MACH_MSGH_BITS_REMOTE        0x0000001
#define MACH_MSGH_BITS_LOCAL         0x0000002
#define MACH_MSGH_BITS_VOUCHER      0x0000004
#define MACH_MSGH_BITS_COMPLEX       0x0000008

#define MACH_MSGH_BITS_REMOTE_MASK   0x0000001
#define MACH_MSGH_BITS_LOCAL_MASK    0x0000002
#define MACH_MSGH_BITS_VOUCHER_MASK  0x0000004

// Mach message timeout
typedef unsigned int mach_msg_timeout_t;

#define MACH_MSG_TIMEOUT_NONE       0

// Mach port rights
#define MACH_PORT_RIGHT_SEND        0x00000001
#define MACH_PORT_RIGHT_RECEIVE     0x00000002
#define MACH_PORT_RIGHT_SEND_ONCE   0x00000004
#define MACH_PORT_RIGHT_PORT_SET    0x00000008
#define MACH_PORT_RIGHT_DEAD_NAME   0x00000010
#define MACH_PORT_RIGHT_LABELH      0x00000020
#define MACH_PORT_RIGHT_NUMBER      0x0000003f

// Memory protection
#define VM_PROT_NONE            0x00
#define VM_PROT_READ            0x01
#define VM_PROT_WRITE           0x02
#define VM_PROT_EXECUTE         0x04

// Memory allocation flags
#define VM_FLAGS_ANYWHERE        0x01
#define VM_FLAGS_FIXED           0x02
#define VM_FLAGS_PURGABLE       0x04
#define VM_FLAGS_NO_CACHE       0x08

// Mach API functions
#ifdef __cplusplus
extern "C" {
#endif

// Kernel Mach functions
kern_return_t mach_kernel_init(void);
void mach_kernel_stats(void);
void mach_kernel_cleanup(void);

kern_return_t mach_port_allocate_kernel(mach_port_t *port, const char *name, void *data);
kern_return_t mach_port_deallocate_kernel(mach_port_t port);

kern_return_t mach_msg_enqueue(mach_port_t port, mach_msg_header_t *msg, int priority);
kern_return_t mach_msg_dequeue(mach_port_t port, mach_msg_header_t *msg, size_t max_size);

kern_return_t task_create_kernel(mach_task_t *child_task, pid_t pid, const char *name, int priority);
kern_return_t task_terminate_kernel(mach_task_t task);

kern_return_t vm_allocate_kernel(void **addr, size_t size, int protection, const char *owner);
kern_return_t vm_deallocate_kernel(void *addr);

// Userspace Mach functions
kern_return_t mach_userspace_init(void);
void mach_userspace_stats(void);
void mach_userspace_cleanup(void);

kern_return_t mach_port_allocate_userspace(mach_port_t *port, const char *name);
kern_return_t mach_port_deallocate_userspace(mach_port_t port);

kern_return_t mach_msg_send_userspace(mach_msg_header_t *msg, mach_msg_timeout_t timeout);
kern_return_t mach_msg_receive_userspace(mach_msg_header_t *msg, mach_msg_timeout_t timeout);

kern_return_t task_create_userspace(mach_task_t *child_task, pid_t pid, const char *name);
kern_return_t task_terminate_userspace(mach_task_t task);

// Standard Mach functions
mach_task_t mach_task_self(void);
kern_return_t mach_thread_self(void);

#ifdef __cplusplus
}
#endif

#endif /* MACH_H */
