#ifndef MIRIX_DOS_TIME_H
#define MIRIX_DOS_TIME_H

#include "sys/types.h"

#define CLOCK_REALTIME   0
#define CLOCK_MONOTONIC  1

extern time_t time(time_t *timer);

static inline int clock_gettime(int clk_id, struct timespec *ts) {
    if (!ts) return -1;
    (void)clk_id;
    ts->tv_sec = time(NULL);
    ts->tv_nsec = 0;
    return 0;
}

#endif /* MIRIX_DOS_TIME_H */
