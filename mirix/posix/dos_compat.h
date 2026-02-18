/*
 * MIRIX DOS Compatibility Layer
 * Provides macros and stubs for DOS systems without multithreading support
 */

#ifndef MIRIX_DOS_COMPAT_H
#define MIRIX_DOS_COMPAT_H

/* DOS/Windows Detection Macros */
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
    #define MIRIX_PLATFORM_WINDOWS 1
#elif defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS)
    #define MIRIX_PLATFORM_DOS 1
#elif defined(__TOS__) || defined(__ATARI__) || defined(__MINT__)
    #define MIRIX_PLATFORM_TOS 1
#else
    #define MIRIX_PLATFORM_UNIX 1
#endif

/* Threading Support Macros */
#ifdef MIRIX_PLATFORM_DOS
    #define MIRIX_NO_THREADS 1
    #define MIRIX_NO_PTHREADS 1
    #define MIRIX_NO_SIGNALS 1
    #define MIRIX_NO_FORK 1
    #define MIRIX_NO_EXEC 1
    #define MIRIX_NO_SOCKETS 1
#else
    #define MIRIX_NO_THREADS 0
    #define MIRIX_NO_PTHREADS 0
    #define MIRIX_NO_SIGNALS 0
    #define MIRIX_NO_FORK 0
    #define MIRIX_NO_EXEC 0
    #define MIRIX_NO_SOCKETS 0
#endif

/* SUS Feature Availability Macros */
#ifdef MIRIX_PLATFORM_DOS
    #define MIRIX_SUS_AVAILABLE 0
    #define MIRIX_POSIX_THREADS_AVAILABLE 0
    #define MIRIX_POSIX_SEMAPHORES_AVAILABLE 0
    #define MIRIX_POSIX_SHARED_MEMORY_AVAILABLE 0
    #define MIRIX_POSIX_MESSAGE_PASSING_AVAILABLE 0
    #define MIRIX_POSIX_TIMERS_AVAILABLE 0
#else
    #define MIRIX_SUS_AVAILABLE 1
    #define MIRIX_POSIX_THREADS_AVAILABLE 1
    #define MIRIX_POSIX_SEMAPHORES_AVAILABLE 1
    #define MIRIX_POSIX_SHARED_MEMORY_AVAILABLE 1
    #define MIRIX_POSIX_MESSAGE_PASSING_AVAILABLE 1
    #define MIRIX_POSIX_TIMERS_AVAILABLE 1
#endif

/* Process Management Macros */
#if MIRIX_NO_FORK
    #define MIRIX_FORK() (-1)
    #define MIRIX_FORK_AVAILABLE 0
    #define MIRIX_GETPID() (getpid())
#else
    #define MIRIX_FORK() fork()
    #define MIRIX_FORK_AVAILABLE 1
    #define MIRIX_GETPID() (getpid())
#endif

#if MIRIX_NO_EXEC
    #define MIRIX_EXECVE(path, argv, envp) (-1)
    #define MIRIX_EXECVE_AVAILABLE 0
#else
    #define MIRIX_EXECVE(path, argv, envp) execve(path, argv, envp)
    #define MIRIX_EXECVE_AVAILABLE 1
#endif

/* Threading Macros */
#if MIRIX_NO_PTHREADS
    #define MIRIX_PTHREAD_T int
    #define MIRIX_PTHREAD_MUTEX_T int
    #define MIRIX_PTHREAD_CREATE(thread, attr, start, arg) (-1)
    #define MIRIX_PTHREAD_JOIN(thread, retval) (-1)
    #define MIRIX_PTHREAD_MUTEX_INIT(mutex, attr) (-1)
    #define MIRIX_PTHREAD_MUTEX_LOCK(mutex) (-1)
    #define MIRIX_PTHREAD_MUTEX_UNLOCK(mutex) (-1)
    #define MIRIX_PTHREAD_MUTEX_DESTROY(mutex) (-1)
    #define MIRIX_PTHREAD_ATFORK(prepare, parent, child) (-1)
    #define MIRIX_PTHREADS_AVAILABLE 0
#else
    #include <pthread.h>
    #define MIRIX_PTHREAD_T pthread_t
    #define MIRIX_PTHREAD_MUTEX_T pthread_mutex_t
    #define MIRIX_PTHREAD_CREATE(thread, attr, start, arg) pthread_create(thread, attr, start, arg)
    #define MIRIX_PTHREAD_JOIN(thread, retval) pthread_join(thread, retval)
    #define MIRIX_PTHREAD_MUTEX_INIT(mutex, attr) pthread_mutex_init(mutex, attr)
    #define MIRIX_PTHREAD_MUTEX_LOCK(mutex) pthread_mutex_lock(mutex)
    #define MIRIX_PTHREAD_MUTEX_UNLOCK(mutex) pthread_mutex_unlock(mutex)
    #define MIRIX_PTHREAD_MUTEX_DESTROY(mutex) pthread_mutex_destroy(mutex)
    #define MIRIX_PTHREAD_ATFORK(prepare, parent, child) pthread_atfork(prepare, parent, child)
    #define MIRIX_PTHREADS_AVAILABLE 1
#endif

/* Signal Handling Macros */
#if MIRIX_NO_SIGNALS
    #define MIRIX_SIGNAL(sig, handler) ((void)0)
    #define MIRIX_SIGNAL_AVAILABLE 0
#else
    #include <signal.h>
    #define MIRIX_SIGNAL(sig, handler) signal(sig, handler)
    #define MIRIX_SIGNAL_AVAILABLE 1
#endif

/* SUS Feature Test Macros for DOS */
#if MIRIX_SUS_AVAILABLE
    #define MIRIX_POSIX_VERSION 200112L
    #define MIRIX_POSIX2_VERSION 199212L
    #define MIRIX_XOPEN_VERSION 700
    #define MIRIX_POSIX_THREADS 200809L
    #define MIRIX_POSIX_SEMAPHORES (-1)
    #define MIRIX_POSIX_SHARED_MEMORY_OBJECTS (-1)
    #define MIRIX_POSIX_MESSAGE_PASSING (-1)
    #define MIRIX_POSIX_TIMERS (-1)
#else
    #define MIRIX_POSIX_VERSION 200112L
    #define MIRIX_POSIX2_VERSION 199212L
    #define MIRIX_XOPEN_VERSION 700
    #define MIRIX_POSIX_THREADS 200809L
    #define MIRIX_POSIX_SEMAPHORES 200809L
    #define MIRIX_POSIX_SHARED_MEMORY_OBJECTS 200809L
    #define MIRIX_POSIX_MESSAGE_PASSING 200112L
    #define MIRIX_POSIX_TIMERS 200809L
#endif

/* SUS Constants for DOS */
#ifdef MIRIX_PLATFORM_DOS
    #define MIRIX_FOPEN_MAX 16
    #define MIRIX_FILENAME_MAX 12
    #define MIRIX_PATH_MAX 64
    #define MIRIX_LINK_MAX 1
#else
    #include <limits.h>
    #define MIRIX_FOPEN_MAX FOPEN_MAX
    #define MIRIX_FILENAME_MAX FILENAME_MAX
    #define MIRIX_PATH_MAX PATH_MAX
    #define MIRIX_LINK_MAX LINK_MAX
#endif

/* SUS Constants for DOS */
#ifdef MIRIX_PLATFORM_DOS
    #define O_CREAT 0x0100
    #define O_RDONLY 0x0000
    #define O_WRONLY 0x0001
    #define O_RDWR 0x0002
    #define O_APPEND 0x0200
    #define O_TRUNC 0x0400
    #define O_EXCL 0x0800
    #define O_SYNC 0x1000
    #define O_ASYNC 0x2000
    #define O_DSYNC 0x1000
    #define O_RSYNC 0x2000
    #define O_NONBLOCK 0x4000
    #define O_NDELAY 0x8000
    #define O_NOCTTY 0x1000
    #define O_DIRECTORY 0x2000
    #define O_NOFOLLOW 0x4000
    #define O_CLOEXEC 0x4000
    #define O_BINARY 0x8000
    #define O_TEXT 0x4000
    #define O_ACCMODE 0x0003
    #define O_CREAT 0x0100
    #define O_EXCL 0x0800
    #define O_NOCTTY 0x1000
    #define O_TRUNC 0x0400
    #define O_APPEND 0x0200
    #define O_SYNC 0x1000
    #define O_ASYNC 0x2000
    #define O_DSYNC 0x1000
    #define O_RSYNC 0x2000
    #define O_NONBLOCK 0x4000
    #define O_NDELAY 0x8000
    #define O_DIRECTORY 0x2000
    #define O_NOFOLLOW 0x4000
    #define O_CLOEXEC 0x4000
    #define O_BINARY 0x8000
    #define O_TEXT 0x4000
    #define O_ACCMODE 0x0003
#else
    #include <fcntl.h>
    #ifndef O_CREAT
        #define O_CREAT O_CREAT
    #endif
    #ifndef O_RDONLY
        #define O_RDONLY O_RDONLY
    #endif
    #ifndef O_WRONLY
        #define O_WRONLY O_WRONLY
    #endif
    #ifndef O_RDWR
        #define O_RDWR O_RDWR
    #endif
    #ifndef O_APPEND
        #define O_APPEND O_APPEND
    #endif
    #ifndef O_TRUNC
        #define O_TRUNC O_TRUNC
    #endif
    #ifndef O_EXCL
        #define O_EXCL O_EXCL
    #endif
    #ifndef O_SYNC
        #define O_SYNC O_SYNC
    #endif
    #ifndef O_ASYNC
        #define O_ASYNC O_ASYNC
    #endif
    #ifndef O_DSYNC
        #define O_DSYNC O_DSYNC
    #endif
    #ifndef O_RSYNC
        #define O_RSYNC O_RSYNC
    #endif
    #ifndef O_NONBLOCK
        #define O_NONBLOCK O_NONBLOCK
    #endif
    #ifndef O_NDELAY
        #define O_NDELAY O_NDELAY
    #endif
    #ifndef O_NOCTTY
        #define O_NOCTTY O_NOCTTY
    #endif
    #ifndef O_DIRECTORY
        #define O_DIRECTORY O_DIRECTORY
    #endif
    #ifndef O_NOFOLLOW
        #define O_NOFOLLOW O_NOFOLLOW
    #endif
    #ifndef O_CLOEXEC
        #define O_CLOEXEC O_CLOEXEC
    #endif
    #ifndef O_BINARY
        #define O_BINARY O_BINARY
    #endif
    #ifndef O_TEXT
        #define O_TEXT O_TEXT
    #endif
    #ifndef O_ACCMODE
        #define O_ACCMODE O_ACCMODE
    #endif
#endif

/* SUS Function Availability for DOS */
#if MIRIX_SUS_AVAILABLE
    #define MIRIX_POSIX_MEMALIGN_AVAILABLE 1
    #define MIRIX_POSIX_FADVISE_AVAILABLE 1
    #define MIRIX_POSIX_FALLOCATE_AVAILABLE 1
    #define MIRIX_POSIX_MADVISE_AVAILABLE 1
    #define MIRIX_POSIX_OPENPT_AVAILABLE 1
    #define MIRIX_CLOCK_GETRES_AVAILABLE 1
    #define MIRIX_CLOCK_NANOSLEEP_AVAILABLE 1
    #define MIRIX_TIMER_CREATE_AVAILABLE 1
    #define MIRIX_TIMER_DELETE_AVAILABLE 1
    #define MIRIX_TIMER_GETTIME_AVAILABLE 1
    #define MIRIX_TIMER_SETTIME_AVAILABLE 1
    #define MIRIX_SHM_OPEN_AVAILABLE 1
    #define MIRIX_SHM_UNLINK_AVAILABLE 1
    #define MIRIX_POSIX_PIPE_AVAILABLE 1
    #define MIRIX_POSIX_MKDIR_AVAILABLE 1
    #define MIRIX_POSIX_FSYNC_AVAILABLE 1
    #define MIRIX_POSIX_FDATASYNC_AVAILABLE 1
    #define MIRIX_POSIX_SOCKET_AVAILABLE 1
#else
    #define MIRIX_POSIX_MEMALIGN_AVAILABLE 0
    #define MIRIX_POSIX_FADVISE_AVAILABLE 0
    #define MIRIX_POSIX_FALLOCATE_AVAILABLE 0
    #define MIRIX_POSIX_MADVISE_AVAILABLE 0
    #define MIRIX_POSIX_OPENPT_AVAILABLE 0
    #define MIRIX_CLOCK_GETRES_AVAILABLE 0
    #define MIRIX_CLOCK_NANOSLEEP_AVAILABLE 0
    #define MIRIX_TIMER_CREATE_AVAILABLE 0
    #define MIRIX_TIMER_DELETE_AVAILABLE 0
    #define MIRIX_TIMER_GETTIME_AVAILABLE 0
    #define MIRIX_TIMER_SETTIME_AVAILABLE 0
    #define MIRIX_SHM_OPEN_AVAILABLE 0
    #define MIRIX_SHM_UNLINK_AVAILABLE 0
    #define MIRIX_POSIX_PIPE_AVAILABLE 0
    #define MIRIX_POSIX_MKDIR_AVAILABLE 0
    #define MIRIX_POSIX_FSYNC_AVAILABLE 0
    #define MIRIX_POSIX_FDATASYNC_AVAILABLE 0
    #define MIRIX_POSIX_SOCKET_AVAILABLE 0
#endif

/* SUS Function Macros for DOS */
#if MIRIX_POSIX_MEMALIGN_AVAILABLE
    #define MIRIX_POSIX_MEMALIGN(alignment, size) mirix_posix_memalign(alignment, size)
#else
    #define MIRIX_POSIX_MEMALIGN(alignment, size) (-1)
#endif

#if MIRIX_POSIX_FADVISE_AVAILABLE
    #define MIRIX_POSIX_FADVISE(fd, offset, len, advice) mirix_posix_fadvise(fd, offset, len, advice)
#else
    #define MIRIX_POSIX_FADVISE(fd, offset, len, advice) (-1)
#endif

#if MIRIX_POSIX_FALLOCATE_AVAILABLE
    #define MIRIX_POSIX_FALLOCATE(fd, offset, len) mirix_posix_fallocate(fd, offset, len)
#else
    #define MIRIX_POSIX_FALLOCATE(fd, offset, len) (-1)
#endif

#if MIRIX_POSIX_MADVISE_AVAILABLE
    #define MIRIX_POSIX_MADVISE(addr, len, advice) mirix_posix_madvise(addr, len, advice)
#else
    #define MIRIX_POSIX_MADVISE(addr, len, advice) (-1)
#endif

#if MIRIX_POSIX_OPENPT_AVAILABLE
    #define MIRIX_POSIX_OPENPT(oflag) mirix_posix_openpt(oflag)
#else
    #define MIRIX_POSIX_OPENPT(oflag) (-1)
#endif

#if MIRIX_CLOCK_GETRES_AVAILABLE
    #define MIRIX_CLOCK_GETRES(clock_id, res) clock_getres(clock_id, res)
#else
    #define MIRIX_CLOCK_GETRES(clock_id, res) (-1)
#endif

#if MIRIX_CLOCK_NANOSLEEP_AVAILABLE
    #define MIRIX_CLOCK_NANOSLEEP(clock_id, rqtp, rmtp) clock_nanosleep(clock_id, rqtp, rmtp)
#else
    #define MIRIX_CLOCK_NANOSLEEP(clock_id, rqtp, rmtp) (-1)
#endif

#if MIRIX_TIMER_CREATE_AVAILABLE
    #define MIRIX_TIMER_CREATE(clock_id, sevp, timerid) timer_create(clock_id, sevp, timerid)
#else
    #define MIRIX_TIMER_CREATE(clock_id, sevp, timerid) (-1)
#endif

#if MIRIX_TIMER_DELETE_AVAILABLE
    #define MIRIX_TIMER_DELETE(timerid) timer_delete(timerid)
#else
    #define MIRIX_TIMER_DELETE(timerid) (-1)
#endif

#if MIRIX_TIMER_GETTIME_AVAILABLE
    #define MIRIX_TIMER_GETTIME(timerid, value) timer_gettime(timerid, value)
#else
    #define MIRIX_TIMER_GETTIME(timerid, value) (-1)
#endif

#if MIRIX_TIMER_SETTIME_AVAILABLE
    #define MIRIX_TIMER_SETTIME(timerid, flags, value) timer_settime(timerid, flags, value)
#else
    #define MIRIX_TIMER_SETTIME(timerid, flags, value) (-1)
#endif

#if MIRIX_SHM_OPEN_AVAILABLE
    #define MIRIX_SHM_OPEN(name, oflag, mode) mirix_shm_open(name, oflag, mode)
#else
    #define MIRIX_SHM_OPEN(name, oflag, mode) (-1)
#endif

#if MIRIX_SHM_UNLINK_AVAILABLE
    #define MIRIX_SHM_UNLINK(name) mirix_shm_unlink(name)
#else
    #define MIRIX_SHM_UNLINK(name) (-1)
#endif

#if MIRIX_POSIX_PIPE_AVAILABLE
    #define MIRIX_POSIX_PIPE(pipefd) posix_pipe(pipefd)
#else
    #define MIRIX_POSIX_PIPE(pipefd) (-1)
#endif

#if MIRIX_POSIX_MKDIR_AVAILABLE
    #define MIRIX_POSIX_MKDIR(pathname, mode) posix_mkdir(pathname, mode)
#else
    #define MIRIX_POSIX_MKDIR(pathname, mode) (-1)
#endif

#if MIRIX_POSIX_FSYNC_AVAILABLE
    #define MIRIX_POSIX_FSYNC(fd) posix_fsync(fd)
#else
    #define MIRIX_POSIX_FSYNC(fd) (-1)
#endif

#if MIRIX_POSIX_FDATASYNC_AVAILABLE
    #define MIRIX_POSIX_FDATASYNC(fd) posix_fdatasync(fd)
#else
    #define MIRIX_POSIX_FDATASYNC(fd) (-1)
#endif

#if MIRIX_POSIX_SOCKET_AVAILABLE
    #define MIRIX_POSIX_SOCKET(domain, type, protocol) posix_socket(domain, type, protocol)
#else
    #define MIRIX_POSIX_SOCKET(domain, type, protocol) (-1)
#endif

/* SUS Status Macros for DOS */
#if MIRIX_SUS_AVAILABLE
    #define MIRIX_SUS_IS_COMPLIANT() (0)  /* No SUS on DOS */
    #define MIRIX_SUS_VERSION_STRING() "MIRIX SUSv3 Core (DOS Limited)"
    #define MIRIX_SUS_FEATURE_TEST(feature) (-1)
#else
    #define MIRIX_SUS_IS_COMPLIANT() sus_is_compliant()
    #define MIRIX_SUS_VERSION_STRING() sus_version_string()
    #define MIRIX_SUS_FEATURE_TEST(feature) sus_feature_test(feature)
#endif

/* Debug Macros for DOS */
#ifdef MIRIX_PLATFORM_DOS
    #define MIRIX_DEBUG_PRINT(msg) printf("DOS: " msg)
#else
    #define MIRIX_DEBUG_PRINT(msg) printf(msg)
#endif

/* Memory Allocation Macros for DOS */
#ifdef MIRIX_PLATFORM_DOS
    #define MIRIX_MALLOC(size) malloc(size)
    #define MIRIX_FREE(ptr) free(ptr)
    #define MIRIX_CALLOC(nmemb, size) calloc(nmemb, size)
#else
    #define MIRIX_MALLOC(size) posix_malloc(size)
    #define MIRIX_FREE(ptr) free(ptr)
    #define MIRIX_CALLOC(nmemb, size) calloc(nmemb, size)
#endif

/* File I/O Macros for DOS */
#ifdef MIRIX_PLATFORM_DOS
    #define MIRIX_FILE_LIMITS_DOS "Max files: 8, Max handles: 16"
#else
    #define MIRIX_FILE_LIMITS_DOS "Max files: 1024, Max handles: 256"
#endif

#endif /* MIRIX_DOS_COMPAT_H */
