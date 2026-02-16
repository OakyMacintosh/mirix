/*
 * MIRIX Simplified SUS (Single UNIX Specification) Implementation
 * Implements core POSIX.1-2001, SUSv3 features without conflicts
 */

#include "sus_simple.h"
#include <mirix/syscall/syscall.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

// POSIX compatibility layer state
static struct {
    bool initialized;
    int signal_handlers[32];
    pthread_mutex_t file_table_mutex;
    mirix_file_entry_t file_table[256];
} posix_state;

// File table entry
typedef struct {
    int fd;
    int flags;
    mode_t mode;
    bool in_use;
} mirix_file_entry_t;

// Initialize POSIX compatibility layer
int posix_init(void) {
    if (posix_state.initialized) {
        return 0;
    }
    
    // Clear signal handlers
    memset(posix_state.signal_handlers, 0, sizeof(posix_state.signal_handlers));
    
    // Initialize mutex
    pthread_mutex_init(&posix_state.file_table_mutex, NULL);
    
    posix_state.initialized = true;
    return 0;
}

// Cleanup POSIX compatibility layer
void posix_cleanup(void) {
    if (!posix_state.initialized) return;
    
    pthread_mutex_destroy(&posix_state.file_table_mutex);
    posix_state.initialized = false;
}

// Find free file descriptor slot
static int find_free_fd_slot(void) {
    pthread_mutex_lock(&posix_state.file_table_mutex);
    
    for (int i = 3; i < 256; i++) {  // Start from 3 (after stdin/stdout/stderr)
        if (!posix_state.file_table[i].in_use) {
            posix_state.file_table[i].in_use = true;
            pthread_mutex_unlock(&posix_state.file_table_mutex);
            return i;
        }
    }
    
    pthread_mutex_unlock(&posix_state.file_table_mutex);
    return -1;  // No free slots
}

// POSIX wrapper functions

// open() wrapper
int posix_open(const char *pathname, int flags, ...) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    mode_t mode = 0;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = (mode_t)va_arg(args, int);  // Promote to int to avoid va_arg issues
        va_end(args);
    }
    
    // Call host system open
    int fd = open(pathname, flags, mode);
    if (fd == -1) {
        errno = errno;  // Preserve errno
        return -1;
    }
    
    // Find free slot in our file table
    int slot = find_free_fd_slot();
    if (slot == -1) {
        close(fd);
        errno = EMFILE;
        return -1;
    }
    
    posix_state.file_table[slot].fd = fd;
    posix_state.file_table[slot].flags = flags;
    posix_state.file_table[slot].mode = mode;
    
    printf("posix_open: %s -> fd %d (slot %d)\n", pathname, fd, slot);
    return slot;
}

// close() wrapper
int posix_close(int fd) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    if (fd < 0 || fd >= 256 || !posix_state.file_table[fd].in_use) {
        errno = EBADF;
        return -1;
    }
    
    int real_fd = posix_state.file_table[fd].fd;
    int result = close(real_fd);
    
    if (result == 0) {
        posix_state.file_table[fd].in_use = false;
        posix_state.file_table[fd].fd = -1;
    }
    
    printf("posix_close: fd %d (real %d) -> %d\n", fd, real_fd, result);
    return result;
}

// read() wrapper
ssize_t posix_read(int fd, void *buf, size_t count) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    if (fd < 0 || fd >= 256 || !posix_state.file_table[fd].in_use) {
        errno = EBADF;
        return -1;
    }
    
    int real_fd = posix_state.file_table[fd].fd;
    
    // Use MIRIX syscall for read
    mirix_read_args_t args = {
        .fd = real_fd,
        .buf = buf,
        .count = count
    };
    
    ssize_t result = syscall_read(&args);
    if (result == -1) {
        errno = errno;
    }
    
    printf("posix_read: fd %d (real %d), count %zu -> %zd\n", fd, real_fd, count, result);
    return result;
}

// write() wrapper
ssize_t posix_write(int fd, const void *buf, size_t count) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    if (fd < 0 || fd >= 256 || !posix_state.file_table[fd].in_use) {
        errno = EBADF;
        return -1;
    }
    
    int real_fd = posix_state.file_table[fd].fd;
    
    // Use MIRIX syscall for write
    mirix_write_args_t args = {
        .fd = real_fd,
        .buf = buf,
        .count = count
    };
    
    ssize_t result = syscall_write(&args);
    if (result == -1) {
        errno = errno;
    }
    
    printf("posix_write: fd %d (real %d), count %zu -> %zd\n", fd, real_fd, count, result);
    return result;
}

// fork() wrapper
pid_t posix_fork(void) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    pid_t result = syscall_fork();
    if (result == -1) {
        errno = errno;
    }
    
    printf("posix_fork -> %d\n", result);
    return result;
}

// exec() wrapper family
int posix_execve(const char *pathname, char *const argv[], char *const envp[]) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    int result = syscall_exec(pathname, (char**)argv);
    if (result == -1) {
        errno = errno;
    }
    
    printf("posix_execve: %s -> %d\n", pathname, result);
    return result;
}

// wait() wrapper
pid_t posix_waitpid(pid_t pid, int *status, int options) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    int result = syscall_wait(status);
    if (result == -1) {
        errno = errno;
    }
    
    printf("posix_waitpid: pid %d -> %d\n", pid, result);
    return result;
}

// kill() wrapper
int posix_kill(pid_t pid, int sig) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    // Store signal handler info
    if (sig >= 0 && sig < 32) {
        posix_state.signal_handlers[sig] = 1;
    }
    
    int result = kill(pid, sig);
    if (result == -1) {
        errno = errno;
    }
    
    printf("posix_kill: pid %d, sig %d -> %d\n", pid, sig, result);
    return result;
}

// gettimeofday() wrapper
int posix_gettimeofday(struct timeval *tv, struct timezone *tz) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    int result = gettimeofday(tv, tz);
    if (result == -1) {
        errno = errno;
    }
    
    return result;
}

// nanosleep() wrapper
int posix_nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!posix_state.initialized) {
        errno = ENOSYS;
        return -1;
    }
    
    int result = nanosleep(req, rem);
    if (result == -1) {
        errno = errno;
    }
    
    printf("posix_nanosleep: %ld.%09ld seconds -> %d\n", 
           req->tv_sec, req->tv_nsec, result);
    return result;
}

// pthread_create() wrapper
int posix_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                         void *(*start_routine)(void *), void *arg) {
    if (!posix_state.initialized) {
        return ENOSYS;
    }
    
    int result = pthread_create(thread, attr, start_routine, arg);
    printf("posix_pthread_create -> %d\n", result);
    return result;
}

// pthread_join() wrapper
int posix_pthread_join(pthread_t thread, void **retval) {
    if (!posix_state.initialized) {
        return ENOSYS;
    }
    
    int result = pthread_join(thread, retval);
    printf("posix_pthread_join -> %d\n", result);
    return result;
}
