#ifndef MIRIX_DOS_SEMAPHORE_H
#define MIRIX_DOS_SEMAPHORE_H

#include <errno.h>
#include "fcntl.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct { int value; } sem_t;

#define SEM_FAILED ((sem_t *)0)

static inline sem_t *sem_open(const char *name, int oflag, ...) {
    (void)name;
    (void)oflag;
    static sem_t sem = { 1 };
    return &sem;
}

static inline int sem_close(sem_t *sem) {
    (void)sem;
    return 0;
}

static inline int sem_unlink(const char *name) {
    (void)name;
    return 0;
}

static inline int sem_wait(sem_t *sem) {
    (void)sem;
    return 0;
}

static inline int sem_post(sem_t *sem) {
    (void)sem;
    return 0;
}

static inline sem_t *sem_init(sem_t *sem, int pshared, unsigned int value) {
    (void)pshared;
    if (sem) sem->value = value;
    return sem;
}

static inline int sem_destroy(sem_t *sem) {
    (void)sem;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_SEMAPHORE_H */
