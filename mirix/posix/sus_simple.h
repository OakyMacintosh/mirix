#ifndef MIRIX_SUS_SIMPLE_H
#define MIRIX_SUS_SIMPLE_H

/*
 * MIRIX Simplified SUS (Single UNIX Specification) Compliance
 * Implements core POSIX.1-2001, SUSv3 features without conflicts
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

// SUS Version and Feature Test (avoid conflicts)
#ifndef _POSIX_VERSION
#define _POSIX_VERSION 200112L
#endif

#ifndef _POSIX2_VERSION
#define _POSIX2_VERSION 199212L
#endif

#ifndef _XOPEN_VERSION
#define _XOPEN_VERSION 700
#endif

// Core SUS Feature Test Macros (avoid conflicts)
#ifndef _POSIX_THREADS
#define _POSIX_THREADS 200809L
#endif

#ifndef _POSIX_SEMAPHORES
#define _POSIX_SEMAPHORES 200809L
#endif

#ifndef _POSIX_SHARED_MEMORY_OBJECTS
#define _POSIX_SHARED_MEMORY_OBJECTS 200809L
#endif

#ifndef _POSIX_MESSAGE_PASSING
#define _POSIX_MESSAGE_PASSING 200112L
#endif

#ifndef _POSIX_TIMERS
#define _POSIX_TIMERS 200809L
#endif

// SUS Constants (avoid conflicts)
#ifndef FOPEN_MAX
#define FOPEN_MAX _POSIX_OPEN_MAX
#endif

#ifndef FILENAME_MAX
#define FILENAME_MAX _POSIX_NAME_MAX
#endif

#ifndef PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX
#endif

#ifndef LINK_MAX
#define LINK_MAX _POSIX_LINK_MAX
#endif

// SUS Function Declarations (avoid conflicts)
int mirix_posix_memalign(size_t alignment, size_t size);
int mirix_posix_fadvise(int fd, off_t offset, off_t len, int advice);
int mirix_posix_fallocate(int fd, off_t offset, off_t len);
int mirix_posix_madvise(void *addr, size_t len, int advice);
int mirix_posix_openpt(int oflag);

// SUS Clock Functions
int clock_getres(clockid_t clock_id, struct timespec *res);
int clock_nanosleep(clockid_t clock_id, const struct timespec *rqtp, struct timespec *rmtp);

// SUS Timer Functions (simplified)
typedef int timer_t;
struct itimerspec {
    int it_interval_tv_sec;
    int it_interval_tv_nsec;
    int it_value_tv_sec;
    int it_value_tv_nsec;
};

int timer_create(clockid_t clock_id, struct sigevent *sevp, timer_t *timerid);
int timer_delete(timer_t timerid);
int timer_gettime(timer_t timerid, struct itimerspec *value);
int timer_getoverrun(timer_t timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *value);

// SUS Thread Functions
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));

// SUS Compliance Check
int sus_is_compliant(void);
const char *sus_version_string(void);
int sus_feature_test(int feature);

// SUS Shared Memory Functions (simplified)
int mirix_shm_open(const char *name, int oflag, mode_t mode);
int mirix_shm_unlink(const char *name);

// SUS Process Functions
int posix_fork(void);
int posix_execve(const char *pathname, char *const argv[], char *const envp[]);
pid_t posix_waitpid(pid_t pid, int *status, int options);

// SUS File Operations
int posix_pipe(int pipefd[2]);
int posix_mkdir(const char *pathname, mode_t mode);
int posix_fsync(int fd);
int posix_fdatasync(int fd);

// SUS Socket Operations
int posix_socket(int domain, int type, int protocol);

// SUS Status Information
void sus_status(void);
int sus_check_compliance(void);

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_SUS_SIMPLE_H */
