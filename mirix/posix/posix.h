#ifndef MIRIX_POSIX_H
#define MIRIX_POSIX_H

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
#include <stdarg.h>
#include <stdbool.h>

// Include SUS compliance
#include "sus_simple.h"

// File table entry
typedef struct {
    int fd;
    int flags;
    mode_t mode;
    bool in_use;
} mirix_file_entry_t;

// POSIX compatibility API
int posix_init(void);
void posix_cleanup(void);

// File operations
int posix_open(const char *pathname, int flags, ...);
int posix_close(int fd);
ssize_t posix_read(int fd, void *buf, size_t count);
ssize_t posix_write(int fd, const void *buf, size_t count);

// Process operations
pid_t posix_fork(void);
int posix_execve(const char *pathname, char *const argv[], char *const envp[]);
pid_t posix_waitpid(pid_t pid, int *status, int options);
int posix_kill(pid_t pid, int sig);

// Time operations
int posix_gettimeofday(struct timeval *tv, struct timezone *tz);
int posix_nanosleep(const struct timespec *req, struct timespec *rem);

// Thread operations
int posix_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                         void *(*start_routine)(void *), void *arg);
int posix_pthread_join(pthread_t thread, void **retval);

#endif // MIRIX_POSIX_H
