/*
 *  dosthread.h
 *
 *  This file is part of AED - Aqua/Mirix Kernel Extensions for DOS.
 *
 *  Public API: a pthreads-compatible threading layer for MS-DOS / DJGPP,
 *  targeting DOS/32A as the DOS extender.
 */

#ifndef DOSTHREAD_H
#define DOSTHREAD_H

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Types
 * ---------------------------------------------------------------------- */

typedef unsigned int  pthread_t;
typedef void         *pthread_attr_t;       /* unused; present for API compat */
typedef void         *pthread_mutex_t;      /* opaque handle to struct dos_mutex */
typedef void         *pthread_mutexattr_t;  /* unused; present for API compat */

/* -------------------------------------------------------------------------
 * Thread API
 * ---------------------------------------------------------------------- */

/**
 * Create a new thread.
 *
 * @param thread         Receives the new thread's ID on success.
 * @param attr           Ignored — pass NULL.
 * @param start_routine  Thread entry point.  Receives arg, returns void *.
 * @param arg            Passed verbatim to start_routine.
 * @return 0 on success, EAGAIN if MAX_THREADS reached, ENOMEM on OOM.
 *
 * Threading is initialised automatically on the first call; there is no
 * explicit init function in the public API.
 */
int       pthread_create(pthread_t *thread,
                         const pthread_attr_t *attr,
                         void *(*start_routine)(void *),
                         void *arg);

/**
 * Terminate the calling thread and make retval available to pthread_join().
 * Does not return.
 */
void      pthread_exit(void *retval);

/**
 * Wait for the specified thread to finish and collect its return value.
 *
 * @param retval  If non-NULL, receives the value passed to pthread_exit().
 * @return 0 on success, ESRCH if the thread ID is invalid.
 */
int       pthread_join(pthread_t thread, void **retval);

/**
 * Return the calling thread's ID.
 */
pthread_t pthread_self(void);

/**
 * Voluntarily yield the CPU so another ready thread can run.
 *
 * Not part of the POSIX standard but very useful for cooperative-style
 * code and for keeping busy-wait loops from starving other threads.
 */
void      pthread_yield(void);

/* -------------------------------------------------------------------------
 * Mutex API
 * ---------------------------------------------------------------------- */

/**
 * Initialise a mutex.
 * @return 0 on success, ENOMEM if memory allocation fails.
 */
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr);

/**
 * Destroy a mutex and free its resources.
 * Behaviour is undefined if the mutex is still locked at the time of the call.
 */
int pthread_mutex_destroy(pthread_mutex_t *mutex);

/**
 * Lock a mutex, blocking (and yielding) until it becomes available.
 */
int pthread_mutex_lock(pthread_mutex_t *mutex);

/**
 * Unlock a mutex.
 * @return EPERM if the calling thread is not the owner.
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/**
 * Try to lock a mutex without blocking.
 * @return 0 if the lock was acquired, EBUSY if it is already held.
 */
int pthread_mutex_trylock(pthread_mutex_t *mutex);

/* -------------------------------------------------------------------------
 * Library lifecycle
 * ---------------------------------------------------------------------- */

/**
 * Restore the original INT 08h timer handler.
 *
 * MUST be called before your program exits (before exit() or returning from
 * main()) to avoid a dangling interrupt vector pointing at freed code/stack.
 */
void dosthread_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* DOSTHREAD_H */