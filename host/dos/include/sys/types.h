#ifndef MIRIX_DOS_SYS_TYPES_H
#define MIRIX_DOS_SYS_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef int pid_t;
typedef int uid_t;
typedef int gid_t;
typedef unsigned long dev_t;
typedef unsigned long ino_t;
typedef unsigned long nlink_t;
typedef long off_t;
typedef unsigned long mode_t;
typedef long ssize_t;
typedef unsigned long timer_t;
#define MIRIX_HAS_TIMER_T
typedef unsigned long clockid_t;
typedef unsigned long time_t;
typedef long clock_t;
typedef long suseconds_t;

typedef union sigval {
    int sival_int;
    void *sival_ptr;
} sigval_t;

struct sigevent {
    int sigev_notify;
    int sigev_signo;
    sigval_t sigev_value;
};

#ifndef MIRIX_HAS_TIMESPEC
#define MIRIX_HAS_TIMESPEC
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
#endif

struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
};

struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

#endif /* MIRIX_DOS_SYS_TYPES_H */
