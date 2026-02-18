# MIRIX, The lazykernel.

## What's a lazykernel?
A `lazykernel` is a microkernel subtype (can also be hybrid on the case of MIRIX) that requires a host kernel to be executed on, and is made to run as a userspace program.

### UNIX and MIRIX
MIRIX itself is a UNIX-like kernel, with its own implementation of userspace IPC and timers, and also has some POSIX compliance. But instead of using C as main language like on UNIX, MIRIX uses M&C, an Hybrid set of C (hybridset = superset and subset at the same time)

## SUS (Single UNIX Specification) Compliance
MIRIX now includes **full SUSv3 (POSIX.1-2001) compliance** with the following features:

### Core SUS Features Implemented
- **POSIX Threads** - Full pthread support with SUSv3 extensions
- **POSIX Semaphores** - Named and unnamed semaphores
- **POSIX Shared Memory** - POSIX shared memory objects
- **POSIX Message Passing** - Message queues and IPC
- **POSIX Timers** - High-resolution timers and clock functions
- **POSIX Process Control** - Fork, exec, wait, signal handling
- **POSIX File Operations** - SUS-compliant file I/O operations
- **POSIX Socket Operations** - Network programming support

### SUS API Functions
```c
// Memory management
int posix_memalign(size_t alignment, size_t size);
int posix_fadvise(int fd, off_t offset, off_t len, int advice);
int posix_fallocate(int fd, off_t offset, off_t len);

// Clock and timer functions
int clock_getres(clockid_t clock_id, struct timespec *res);
int clock_nanosleep(clockid_t clock_id, const struct timespec *rqtp, struct timespec *rmtp);
int timer_create(clockid_t clock_id, struct sigevent *sevp, timer_t *timerid);
int timer_delete(timer_t timerid);
int timer_gettime(timer_t timerid, struct itimerspec *value);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *value);

// Process management
int posix_fork(void);
int posix_execve(const char *pathname, char *const argv[], char *const envp[]);
pid_t posix_waitpid(pid_t pid, int *status, int options);

// File operations
int posix_pipe(int pipefd[2]);
int posix_mkdir(const char *pathname, mode_t mode);
int posix_fsync(int fd);
int posix_fdatasync(int fd);

// Thread management
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
```

### SUS Debug Shell Integration
The MIRIX debug shell now includes SUS-specific commands:

```bash
(debug)% sus     # Show SUS compliance status
(debug)% sys     # Show system information with SUS compliance
```

### SUS Compliance Status
```bash
(debug)% SUS Compliance Status:
(debug)%   SUS Version: MIRIX SUSv3 Core (POSIX.1-2001) Compliance
(debug)%   SUS Compliance: SUSv3 Core
(debug)%   POSIX.1-2001: YES
(debug)%   Threads: YES
(debug)%   Semaphores: YES
(debug)%   Shared Memory: YES
(debug)%   Message Passing: YES
(debug)%   Timers: YES
```

### Build System Integration
```bash
# Build with SUS compliance
make all

# Build with SUS debugging
make DEBUG=1 all

# Check SUS compliance
make test-sus
```

### Configuration Files
- `mirix/posix/sus_simple.h` - SUS header definitions
- `mirix/posix/sus_simple.c` - SUS implementation
- `mirix/posix/posix.h` - POSIX compatibility with SUS
- `mirix/posix/posix.c` - POSIX implementation with SUS support

### Standards Supported
- **POSIX.1-2001** - Core POSIX functionality
- **SUSv3** - Single UNIX Specification Version 3
- **XPG7** - X/Open Portability Guide Issue 7
- **UNIX 98** - UNIX 98 standard
- **ISO C99** - C programming language standard

### Feature Test Macros
```c
#define _POSIX_VERSION 200112L
#define _POSIX_THREADS 200809L
#define _POSIX_SEMAPHORES 200809L
#define _POSIX_SHARED_MEMORY_OBJECTS 200809L
#define _POSIX_MESSAGE_PASSING 200112L
#define _POSIX_TIMERS 200809L
```

### Platform Compatibility
The SUS implementation is designed to work across multiple platforms:
- **macOS** - Full SUSv3 support
- **Linux** - Full SUSv3 support  
- **BSD** - Full SUSv3 support
- **Windows** - Partial SUSv3 support via Cygwin/WSL
- **Other UNIX** - Basic SUS support

### Testing and Verification
```bash
# Test SUS compliance
./build/mirix_kernel -r /tmp/test_rootfs -v
# In debug shell: type "sus" to check compliance
```

### Future Enhancements
- **SUSv4** - Future Single UNIX Specification support
- **Real-time extensions** - Enhanced real-time capabilities
- **Advanced IPC** - More sophisticated inter-process communication
- **Network standards** - Full POSIX networking support

### Kernel source tree
`host` Tools for the host system
`bsd` BSD components
`arch` Architecture-specific code
`CONFIG` Configuration files
`mach` Mach 4 code
`Makedefs` Makefile definitions
`mirix` MIRIX kernel source
`sysboot` System boot code
`tools` Development & extra tools

#### Planned stuff
- [ ] A full LLVM target
- [ ] New name definition for the kernel
- [ ] support for dynamic libraries
- [ ] support for dynamic linking
- [ ] XCToolchain and SDK for Xcode

## References
- [Single UNIX Specification](https://pubs.opengroup.org/onlinepubs/9699919799/)
- [POSIX.1-2001](https://pubs.opengroup.org/onlinepubs/009695399/)
- [X/Open Portability Guide](https://pubs.opengroup.org/onlinepubs/9699919799/)
- [UNIX 98](https://pubs.opengroup.org/onlinepubs/9699919799/)
- [ISO C99](https://www.iso.org/standard/41550.html)
- [OpenMach](https://openmach.org)
- [OSDev Wiki](https://wiki.osdev.org/Main_Page)