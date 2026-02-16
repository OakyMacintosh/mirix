# MIRIX Mach Integration

This document describes the integration of OpenMach (Mach 4) code with the MIRIX lazykernel system.

## Overview

The Mach integration provides both kernel-level and userspace Mach services to the MIRIX system, enabling advanced IPC, memory management, and task management capabilities.

## Architecture

### Components

1. **Mach Library** (`mach/libmach/`) - Core Mach userspace library
2. **Mach Threads** (`mach/libthreads/`) - C threads implementation
3. **Mach Interfaces** (`mach/include/mach/`) - Mach API headers and definitions
4. **MIG** (`mach/mig/`) - Mach Interface Generator
5. **Bootstrap** (`mach/bootstrap/`) - Mach bootstrap services
6. **Patches** - Integration patches for userspace and kernel

### Integration Points

#### Kernel Integration
- Port management and message passing
- Task creation and management
- Memory allocation and protection
- Inter-process communication

#### Userspace Integration
- Mach API compatibility layer
- Thread management
- Port rights and security
- Message queuing

## Build System

### Configuration

The build system uses the `Makedefs/` directory for configuration:

- `Config.mk` - Global configuration variables
- `Rules.mk` - Common build rules
- `Mach.mk` - Mach-specific rules and integration

### Targets

```bash
# Build all Mach components
make mach

# Build Mach kernel integration
make mach-kernel

# Build Mach userspace integration
make mach-userspace

# Build with full Mach integration
make all-mach

# Build with only kernel Mach support
make all-mach-kernel

# Build with only userspace Mach support
make all-mach-userspace
```

### Host Support

The build system supports multiple host platforms:

- **Darwin** (macOS) - Full support with Xcode tools
- **Linux** - GCC/Clang support
- **BSD** - FreeBSD, NetBSD, OpenBSD support
- **Windows** - MinGW/MSVC support
- **IRIX** - SGI IRIX support
- **MS-DOS** - DJGPP support

## Usage

### Kernel Integration

To enable Mach kernel integration:

```bash
make MACH_KERNEL=1
```

This will:
- Build Mach kernel patches
- Integrate Mach services with the kernel
- Provide kernel-level IPC and memory management
- Enable Mach task management

### Userspace Integration

To enable Mach userspace integration:

```bash
make MACH_USERSPACE=1
```

This will:
- Build Mach userspace library
- Provide Mach API compatibility
- Enable userspace threads and ports
- Support Mach message passing

### Full Integration

For complete Mach support:

```bash
make MACH_KERNEL=1 MACH_USERSPACE=1
# or simply:
make all-mach
```

## API Reference

### Kernel Mach API

```c
// Initialize kernel Mach subsystem
kern_return_t mach_kernel_init(void);

// Port management
kern_return_t mach_port_allocate_kernel(mach_port_t *port, const char *name, void *data);
kern_return_t mach_port_deallocate_kernel(mach_port_t port);

// Message passing
kern_return_t mach_msg_enqueue(mach_port_t port, mach_msg_header_t *msg, int priority);
kern_return_t mach_msg_dequeue(mach_port_t port, mach_msg_header_t *msg, size_t max_size);

// Task management
kern_return_t task_create_kernel(mach_task_t *child_task, pid_t pid, const char *name, int priority);
kern_return_t task_terminate_kernel(mach_task_t task);

// Memory management
kern_return_t vm_allocate_kernel(void **addr, size_t size, int protection, const char *owner);
kern_return_t vm_deallocate_kernel(void *addr);
```

### Userspace Mach API

```c
// Initialize userspace Mach subsystem
kern_return_t mach_userspace_init(void);

// Port management
kern_return_t mach_port_allocate_userspace(mach_port_t *port, const char *name);
kern_return_t mach_port_deallocate_userspace(mach_port_t port);

// Message passing
kern_return_t mach_msg_send_userspace(mach_msg_header_t *msg, mach_msg_timeout_t timeout);
kern_return_t mach_msg_receive_userspace(mach_msg_header_t *msg, mach_msg_timeout_t timeout);

// Task management
kern_return_t task_create_userspace(mach_task_t *child_task, pid_t pid, const char *name);
kern_return_t task_terminate_userspace(mach_task_t task);
```

## Configuration Options

### Kernel Configuration

- `MACH_KERNEL` - Enable kernel Mach integration
- `MACH_KERNEL_INTEGRATION` - Kernel integration flag
- `MACH_MEMORY_SIZE` - Kernel Mach memory pool size
- `MACH_MAX_PORTS` - Maximum kernel ports
- `MACH_MAX_TASKS` - Maximum kernel tasks

### Userspace Configuration

- `MACH_USERSPACE` - Enable userspace Mach integration
- `MACH_USERSPACE_INTEGRATION` - Userspace integration flag
- `MACH_THREAD_STACK_SIZE` - Thread stack size
- `MACH_MSG_QUEUE_SIZE` - Message queue size
- `MACH_PORT_RIGHTS` - Port rights configuration

## Examples

### Basic Mach Port Usage

```c
#include <mach/mach.h>
#include <mach/mach_error.h>

int main(void) {
    mach_port_t port;
    kern_return_t kr;
    
    // Initialize Mach subsystem
    kr = mach_userspace_init();
    if (kr != KERN_SUCCESS) {
        mach_error("mach_userspace_init", kr);
        return 1;
    }
    
    // Allocate a port
    kr = mach_port_allocate_userspace(&port, "example_port");
    if (kr != KERN_SUCCESS) {
        mach_error("mach_port_allocate_userspace", kr);
        return 1;
    }
    
    printf("Allocated port: %d\n", port);
    
    // Deallocate port
    kr = mach_port_deallocate_userspace(port);
    if (kr != KERN_SUCCESS) {
        mach_error("mach_port_deallocate_userspace", kr);
        return 1;
    }
    
    // Cleanup
    mach_userspace_cleanup();
    return 0;
}
```

### Message Passing Example

```c
#include <mach/mach.h>
#include <mach/mach_error.h>

int main(void) {
    mach_port_t port;
    mach_msg_header_t msg;
    kern_return_t kr;
    
    // Initialize Mach
    kr = mach_userspace_init();
    if (kr != KERN_SUCCESS) {
        mach_error("mach_userspace_init", kr);
        return 1;
    }
    
    // Allocate port
    kr = mach_port_allocate_userspace(&port, "msg_port");
    if (kr != KERN_SUCCESS) {
        mach_error("mach_port_allocate_userspace", kr);
        return 1;
    }
    
    // Create message
    msg.msgh_bits = MACH_MSGH_BITS_LOCAL;
    msg.msgh_size = sizeof(mach_msg_header_t);
    msg.msgh_remote_port = MACH_PORT_NULL;
    msg.msgh_local_port = port;
    msg.msgh_voucher_port = MACH_PORT_NULL;
    msg.msgh_id = 12345;
    
    // Send message
    kr = mach_msg_send_userspace(&msg, MACH_MSG_TIMEOUT_NONE);
    if (kr != KERN_SUCCESS) {
        mach_error("mach_msg_send_userspace", kr);
        return 1;
    }
    
    // Receive message
    mach_msg_header_t recv_msg;
    kr = mach_msg_receive_userspace(&recv_msg, MACH_MSG_TIMEOUT_NONE);
    if (kr != KERN_SUCCESS) {
        mach_error("mach_msg_receive_userspace", kr);
        return 1;
    }
    
    printf("Received message: id=%d\n", recv_msg.msgh_id);
    
    // Cleanup
    mach_userspace_cleanup();
    return 0;
}
```

## Debugging

### Statistics

Both kernel and userspace Mach subsystems provide statistics:

```c
// Kernel statistics
mach_kernel_stats();

// Userspace statistics
mach_userspace_stats();
```

### Debug Output

Enable debug output by defining `MACH_DEBUG`:

```bash
make CFLAGS="-DMACH_DEBUG=1" all-mach
```

### Common Issues

1. **MIG not found** - Install Xcode Command Line Tools on macOS
2. **Port allocation failure** - Check port table limits
3. **Message queue full** - Increase queue size
4. **Memory allocation failure** - Check memory pool size

## Testing

### Unit Tests

```bash
# Run Mach tests
make -C mach test-mach

# Run Mach benchmarks
make -C mach benchmark-mach
```

### Integration Tests

```bash
# Test kernel integration
make MACH_KERNEL=1 test

# Test userspace integration
make MACH_USERSPACE=1 test

# Test full integration
make all-mach test
```

## Performance

### Benchmarks

The Mach integration includes performance benchmarks:

- Port allocation/deallocation speed
- Message passing latency
- Task creation overhead
- Memory allocation performance

### Optimization

- Use port caching for frequently used ports
- Batch message operations when possible
- Pre-allocate message buffers
- Use kernel Mach for high-performance operations

## Security

### Port Rights

Mach ports support fine-grained access control:

- **Receive rights** - Allow receiving messages
- **Send rights** - Allow sending messages
- **Send-once rights** - Allow one-time sending
- **Port set rights** - Allow port set membership

### Memory Protection

Kernel Mach provides memory protection:

- **Read-only** - Read-only access
- **Read-write** - Read and write access
- **Execute** - Execute permission
- **No access** - No access allowed

## Future Enhancements

### Planned Features

1. **Distributed Mach** - Network-transparent IPC
2. **Mach Debugger** - Kernel debugging support
3. **Mach Tracing** - Performance tracing
4. **Mach Security** - Enhanced security model
5. **Mach Virtualization** - VM support

### Compatibility

- **Mach 3.0** - Full backward compatibility
- **Mach 4.0** - Enhanced features
- **XNU Mach** - macOS compatibility layer

## Contributing

### Adding New Mach Features

1. Update `Makedefs/Mach.mk` with new sources
2. Add API to appropriate header files
3. Implement kernel and userspace versions
4. Add tests and benchmarks
5. Update documentation

### Submitting Patches

1. Fork the repository
2. Create feature branch
3. Implement changes
4. Add tests
5. Submit pull request

## References

- [Mach Kernel Interface Manual](https://www.gnu.org/software/hurd/gnumach-doc/)
- [Mach 4.0 Reference](https://github.com/apple-oss-distributions/mach)
- [XNU Kernel Programming](https://developer.apple.com/library/archive/documentation/Darwin/Conceptual/KernelProgramming/)
- [MIG Interface Generator](https://www.gnu.org/software/hurd/mig/)

## License

The Mach integration code is licensed under the same terms as the MIRIX project.
