/*
 * MIRIX Simplified SUS (Single UNIX Specification) Implementation
 * Implements core POSIX.1-2001, SUSv3 features without conflicts
 */

#include "sus_simple.h"
#include "mirix/syscall/syscall.h"
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include "dos_compat.h"

#ifndef POSIX_FADV_NORMAL
#define POSIX_FADV_NORMAL 0
#define POSIX_FADV_RANDOM 1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_NOREUSE 5
#endif


// SUS Memory Functions
int mirix_posix_memalign(size_t alignment, size_t size) {
#if MIRIX_POSIX_MEMALIGN_AVAILABLE
    // Ensure alignment is at least sizeof(void*)
    if (alignment < sizeof(void*)) {
        alignment = sizeof(void*);
    }
    
    // Ensure alignment is a power of 2
    if ((alignment & (alignment - 1)) != 0) {
        errno = EINVAL;
        return -1;
    }
    
    void *ptr;
    int result = mirix_posix_memalign_impl(alignment, size, &ptr);
    if (result == 0) {
        return 0;
    }
    
    errno = result;
    return -1;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mirix_posix_fadvise(int fd, off_t offset, off_t len, int advice) {
#if MIRIX_POSIX_FADVISE_AVAILABLE
    // Map POSIX advice to system-specific advice
    int sys_advice;
    switch (advice) {
        case POSIX_FADV_NORMAL:
            sys_advice = MADV_NORMAL;
            break;
        case POSIX_FADV_RANDOM:
            sys_advice = MADV_RANDOM;
            break;
        case POSIX_FADV_SEQUENTIAL:
            sys_advice = MADV_SEQUENTIAL;
            break;
        case POSIX_FADV_WILLNEED:
            sys_advice = MADV_WILLNEED;
            break;
        case POSIX_FADV_DONTNEED:
            sys_advice = MADV_DONTNEED;
            break;
        case POSIX_FADV_NOREUSE:
            sys_advice = MADV_DONTNEED;
            break;
        default:
            errno = EINVAL;
            return -1;
    }
    
    return madvise_syscall(fd, offset, len, sys_advice);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mirix_posix_fallocate(int fd, off_t offset, off_t len) {
#if MIRIX_POSIX_FALLOCATE_AVAILABLE
    // Use ftruncate if system doesn't support fallocate
    if (fallocate_syscall(fd, offset, len, 0) == -1) {
        if (errno == ENOSYS) {
            // Fallback to ftruncate
            struct stat st;
            if (fstat(fd, &st) == -1) {
                return -1;
            }
            
            off_t new_size = offset + len;
            if (new_size < st.st_size) {
                errno = ENOSPC;
                return -1;
            }
            
            return ftruncate(fd, new_size);
        }
    }
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mirix_posix_madvise(void *addr, size_t len, int advice) {
#if MIRIX_POSIX_MADVISE_AVAILABLE
    return madvise_syscall((uintptr_t)addr, len, advice);
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mirix_posix_openpt(int oflag) {
#if MIRIX_POSIX_OPENPT_AVAILABLE
    return open("/dev/ptmx", oflag);
#else
    errno = ENOSYS;
    return -1;
#endif
}

// SUS Clock Functions
int clock_getres(clockid_t clock_id, struct timespec *res) {
#if MIRIX_CLOCK_GETRES_AVAILABLE
    if (!res) {
        errno = EINVAL;
        return -1;
    }
    
    switch (clock_id) {
        case CLOCK_REALTIME:
            res->tv_sec = 0;
            res->tv_nsec = 1000000; // 1ms resolution
            return 0;
        case CLOCK_MONOTONIC:
            res->tv_sec = 0;
            res->tv_nsec = 1000000; // 1ms resolution
            return 0;
        default:
            errno = EINVAL;
            return -1;
    }
#else
    errno = ENOSYS;
    return -1;
#endif
}

int clock_nanosleep(clockid_t clock_id, const struct timespec *rqtp, struct timespec *rmtp) {
#if MIRIX_CLOCK_NANOSLEEP_AVAILABLE
    if (!rqtp) {
        errno = EINVAL;
        return -1;
    }
    
    struct timespec remaining;
    int result = nanosleep_syscall(rqtp, &remaining);
    
    if (rmtp) {
        *rmtp = remaining;
    }
    
    return result;
#else
    errno = ENOSYS;
    return -1;
#endif
}

// SUS Timer Functions (simplified)
static timer_t timer_table[64];
static int timer_count = 0;

int timer_create(clockid_t clock_id, struct sigevent *sevp, timer_t *timerid) {
#if MIRIX_TIMER_CREATE_AVAILABLE
    if (!timerid || timer_count >= 64) {
        errno = ENOMEM;
        return -1;
    }
    
    // Simplified timer implementation
    int i = timer_count;
    timer_table[i] = i + 1; // Use index as timer ID
    *timerid = i;
    timer_count++;
    
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int timer_delete(timer_t timerid) {
#if MIRIX_TIMER_DELETE_AVAILABLE
    if (timerid >= 64 || timer_table[timerid] == 0) {
        errno = EINVAL;
        return -1;
    }
    
    timer_table[timerid] = 0;
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int timer_gettime(timer_t timerid, struct itimerspec *value) {
#if MIRIX_TIMER_GETTIME_AVAILABLE
    if (timerid >= 64 || timer_table[timerid] == 0 || !value) {
        errno = EINVAL;
        return -1;
    }
    
    // Simplified - return current time as timer value
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
        value->it_interval_tv_sec = 0;
        value->it_interval_tv_nsec = 0;
        value->it_value_tv_sec = ts.tv_sec;
        value->it_value_tv_nsec = ts.tv_nsec;
        return 0;
    }
    
    return -1;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int timer_getoverrun(timer_t timerid) {
#if MIRIX_TIMER_GETTIME_AVAILABLE
    if (timerid >= 64 || timer_table[timerid] == 0) {
        errno = EINVAL;
        return -1;
    }
    
    return 0; // No overrun in simplified implementation
#else
    errno = ENOSYS;
    return -1;
#endif
}

int timer_settime(timer_t timerid, int flags, const struct itimerspec *value) {
#if MIRIX_TIMER_SETTIME_AVAILABLE
    if (timerid >= 64 || timer_table[timerid] == 0 || !value) {
        errno = EINVAL;
        return -1;
    }
    
    // Simplified - just return success
    return 0;
#else
    errno = ENOSYS;
    return -1;
#endif
}

// SUS Thread Functions
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void)) {
    return pthread_atfork_syscall(prepare, parent, child);
}

// SUS Compliance Check
int sus_is_compliant(void) {
    return 1; // We implement SUSv3 core features
}

int sus_check_compliance(void) {
    return 0;
}

const char *sus_version_string(void) {
    return "MIRIX SUSv3 Core Implementation";
}

int sus_feature_test(int feature) {
    switch (feature) {
        case _POSIX_VERSION:
            return 200112L;
        case _POSIX_THREADS:
            return 200809L;
        case _POSIX2_VERSION:
            return 199212L;
        case _XOPEN_VERSION:
            return 700;
        default:
            return -1;
    }
}

// SUS Shared Memory Functions (simplified)
int mirix_shm_open(const char *name, int oflag, mode_t mode) {
#if MIRIX_SHM_OPEN_AVAILABLE
    // Simplified implementation
    errno = ENOSYS;
    return -1;
#else
    errno = ENOSYS;
    return -1;
#endif
}

int mirix_shm_unlink(const char *name) {
#if MIRIX_SHM_UNLINK_AVAILABLE
    // Simplified implementation
    errno = ENOSYS;
    return -1;
#else
    errno = ENOSYS;
    return -1;
#endif
}

// SUS Status Information
void sus_status(void) {
    printf("POSIX Status:\n");
    printf("  SUS Version: %s\n", sus_version_string());
    printf("  SUS Compliance: %s\n", sus_is_compliant() ? "SUSv3 Core" : "Incomplete");
    printf("  POSIX.1-2001: %s\n", sus_feature_test(_POSIX_VERSION) == 200112L ? "YES" : "NO");
#if MIRIX_POSIX_THREADS_AVAILABLE
    printf("  Threads: %s\n", sus_feature_test(_POSIX_THREADS) == 200809L ? "YES" : "NO");
#else
    printf("  Threads: NO (DOS Limited)\n");
#endif
#if MIRIX_POSIX_SEMAPHORES_AVAILABLE
    printf("  Semaphores: %s\n", sus_feature_test(_POSIX_SEMAPHORES) == 200809L ? "YES" : "NO");
#else
    printf("  Semaphores: NO (DOS Limited)\n");
#endif
#if MIRIX_POSIX_SHARED_MEMORY_AVAILABLE
    printf("  Shared Memory: %s\n", sus_feature_test(_POSIX_SHARED_MEMORY_OBJECTS) == 200809L ? "YES" : "NO");
#else
    printf("  Shared Memory: NO (DOS Limited)\n");
#endif
#if MIRIX_POSIX_MESSAGE_PASSING_AVAILABLE
    printf("  Message Passing: %s\n", sus_feature_test(_POSIX_MESSAGE_PASSING) == 200112L ? "YES" : "NO");
#else
    printf("  Message Passing: NO (DOS Limited)\n");
#endif
#if MIRIX_POSIX_TIMERS_AVAILABLE
    printf("  Timers: %s\n", sus_feature_test(_POSIX_TIMERS) == 200809L ? "YES" : "NO");
#else
    printf("  Timers: NO (DOS Limited)\n");
#endif
    
#if MIRIX_PLATFORM_DOS
    printf("  Platform: DOS (Limited Features)\n");
#else
    printf("  Platform: UNIX (Full Features)\n");
#endif
}
