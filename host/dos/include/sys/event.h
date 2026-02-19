#ifndef MIRIX_DOS_SYS_EVENT_H
#define MIRIX_DOS_SYS_EVENT_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kevent {
    uintptr_t ident;
    short filter;
    unsigned short flags;
    unsigned int fflags;
    intptr_t data;
    void *udata;
};

#define EVFILT_READ   (-1)
#define EVFILT_WRITE  (-2)
#define EVFILT_TIMER  (-3)

#define EV_ADD        0x0001
#define EV_DELETE     0x0002
#define EV_ENABLE     0x0004
#define EV_DISABLE    0x0008

#define EV_SET(kev, a, b, c, d, e, f) do { \
    (kev)->ident = (uintptr_t)(a);             \
    (kev)->filter = (short)(b);                \
    (kev)->flags = (unsigned short)(c);        \
    (kev)->fflags = (unsigned int)(d);         \
    (kev)->data = (intptr_t)(e);               \
    (kev)->udata = (void *)(f);                \
} while (0)

int kqueue(void);
int kevent(int kq, const struct kevent *changelist, int nchanges,
           struct kevent *eventlist, int nevents, const struct timespec *timeout);

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_SYS_EVENT_H */
