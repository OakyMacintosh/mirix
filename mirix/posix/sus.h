#ifndef MIRIX_SUS_H
#define MIRIX_SUSIX_SUS_H

/*
 * MIRIX SUS (Single UNIX Specification) Compliance
 * Implements POSIX.1-2001, SUSv3, and related standards
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
// #include <semaphore.h>  // Commented out for now
// #include <mqueue.h>     // Commented out for now
#include <poll.h>
// #include <termios.h>    // Commented out for now
// #include <pty.h>        // Commented out for now
#include <syslog.h>
#include <pwd.h>
#include <grp.h>
#include <utime.h>
#include <glob.h>
#include <fnmatch.h>
#include <regex.h>
#include <tar.h>
#include <cpio.h>
#include <locale.h>
#include <langinfo.h>
#include <wchar.h>
#include <wctype.h>
#include <iconv.h>
#include <monetary.h>
#include <inttypes.h>
#include <stdalign.h>
// #include <threads.h>    // Commented out for now
#include <sched.h>
// #include <spawn.h>        // Commented out for now

#ifdef __cplusplus
extern "C" {
#endif

// SUS Feature Test Macros
#define _POSIX_VERSION 200112L
#define _POSIX2_VERSION 199212L
#define _XOPEN_VERSION 700
#define _XOPEN_XCU_VERSION 4

// SUS Feature Test Macros
#define _POSIX_ADVISORY_INFO 200809L
#define _POSIX_CHOWN_RESTRICTED 200809L
#define _POSIX_IPV6 200112L
#define _POSIX_JOB_CONTROL 200809L
#define _POSIX_MAPPED_FILES 200112L
#define _POSIX_MEMLOCK 200112L
#define _POSIX_MEMLOCK_RANGE 200112L
#define _POSIX_MEMORY_PROTECTION 200112L
#define _POSIX_MESSAGE_PASSING 200112L
#define _POSIX_MONOTONIC_CLOCK 200809L
#define _POSIX_PRIORITIZED_IO 200809L
#define _POSIX_RAW_SOCKETS 200809L
#define _POSIX_READER_WRITER_LOCKS 200809L
#define _POSIX_REALTIME_SIGNALS 200809L
#define _POSIX_REGEXP 200809L
#define _POSIX_SAVED_IDS 200809L
#define _POSIX_SEMAPHORES 200809L
#define _POSIX_SHARED_MEMORY_OBJECTS 200809L
#define _POSIX_SHELL 200809L
#define _POSIX_SPAWN 200809L
#define _POSIX_SPIN_LOCKS 200809L
#define _POSIX_SPORADIC_SERVER 200809L
#define _POSIX_SYNCHRONIZED_IO 200809L
#define _POSIX_THREAD_ATTR_STACKADDR 200809L
#define _POSIX_THREAD_CPUTIME 200809L
#define _POSIX_THREAD_PRIORITY_SCHEDULING 200809L
#define _POSIX_THREAD_SAFE_FUNCTIONS 200809L
#define _POSIX_THREAD_PROCESS_SHARED 200809L
#define _POSIX_THREADS 200809L
#define _POSIX_TIMEOUTS 200809L
#define _POSIX_TIMERS 200809L
#define _POSIX_TYPED_MEMORY_OBJECTS 200809L
#define _POSIX_VDISABLE 200809L

// XSI (X/Open System Interface) Extensions
#define _XOPEN_SHM 1
#define _XOPEN_STREAMS 1
#define _XOPEN_UNIX 1
#define _XOPEN_VERSION 700

// C Extension Macros
#define __STDC_VERSION__ 201112L
#define __STDC_NO_ATOMICS__ 1
#define __STDC_NO_VLA__ 1
#define __STDC_IEC_559_COMPLEX__ 1
#define __STDC_IEC_559_TRIPLET_COMPLEX__ 1
#define __STDC_IEC_60559_COMPLEX__ 1
#define __STDC_IEC_60559_TRIPLET_COMPLEX__ 1
#define __STDC_LIB_EXT1__ 1
#define __STDC_LIB_EXT2__ 1
#define __STDC_LIB_EXT3__ 1
#define __STDC_LIB_EXT4__ 1
#define __STDC_LIB_EXT5__ 1
#define __STDC_LIB_EXT6__ 1
#define __STDC_MB_MIGHT_NEQ_WC__ 1
#define __STDC_MB_MIGHT_NEQ_WC__ 1
#define __STDC_ISO_10646__ 201510L

// SUS Constants
#define FOPEN_MAX _POSIX_OPEN_MAX
#define FILENAME_MAX _POSIX_NAME_MAX
#define PATH_MAX _POSIX_PATH_MAX
#define LINK_MAX _POSIX_LINK_MAX
#define MAX_CANON _POSIX_NAME_MAX
#define NGROUPS_MAX _POSIX_NGROUPS_MAX
#define OPEN_MAX _POSIX_OPEN_MAX
#define PTHREAD_KEYS_MAX _POSIX_THREAD_KEYS_MAX
#define PTHREAD_DESTRUCTOR_ITERATIONS _POSIX_THREAD_DESTRUCTOR_ITERATIONS
#define PTHREAD_THREADS_MAX _POSIX_THREAD_THREADS_MAX
#define SSIZE_MAX _POSIX_SSIZE_MAX
#define STREAM_MAX _POSIX_STREAM_MAX
#define TZNAME_MAX _POSIX_TZNAME_MAX
#define _POSIX_HOST_NAME_MAX _POSIX_NAME_MAX
#define _POSIX_LOGIN_NAME_MAX _POSIX_NAME_MAX
#define _POSIX_MAX_INPUT _POSIX_INPUT_MAX
#define _POSIX_MAX_CANON _POSIX_NAME_MAX
#define _POSIX_PATH_MAX _POSIX_PATH_MAX
#define _POSIX_PIPE_BUF _POSIX_PIPE_BUF
#define _POSIX_SSIZE_MAX _POSIX_SSIZE_MAX
#define _POSIX_SYMLINK_MAX _POSIX_NAME_MAX
#define _POSIX_TTY_NAME_MAX _POSIX_NAME_MAX
#define _POSIX_TZNAME_MAX _POSIX_NAME_MAX

// SUS File Types
#define S_IFMT _POSIX_V6_ILP32_OFF
#define S_IFDIR _POSIX_V6_ILP32_OFF
#define S_IFCHR _POSIX_V6_ILP32_OFF
#define S_IFBLK _POSIX_V6_ILP32_OFF
#define S_IFREG _POSIX_V6_ILP32_OFF
#define S_IFIFO _POSIX_V6_ILP32_OFF
#define S_IFLNK _POSIX_V6_ILP32_OFF
#define S_IFSOCK _POSIX_V6_ILP32_OFF

// SUS Signal Extensions
#define SA_NOCLDSTOP _POSIX_SOURCE
#define SA_NOCLDWAIT _POSIX_SOURCE
#define SA_NODEFER _POSIX_SOURCE
#define SA_ONSTACK _POSIX_SOURCE
#define SA_RESETHAND _POSIX_SOURCE
#define SA_RESTART _POSIX_SOURCE
#define SA_SIGINFO _POSIX_SOURCE

// SUS Function Declarations
int posix_memalign(size_t alignment, size_t size);
int posix_fadvise(int fd, off_t offset, off_t len, int advice);
int posix_fallocate(int fd, off_t offset, off_t len);
int posix_madvise(void *addr, size_t len, int advice);
int posix_openpt(int oflag);
int posix_spawn_file_actions_addclosefrom_np(posix_spawn_file_actions_t *file_actions, int fromfd);
int posix_spawn_file_actions_adddup2_np(posix_spawn_file_actions_t *file_actions, int fd);
int posix_spawn_file_actions_addfchdir_np(posix_spawn_file_actions_t *file_actions, int fd);
int posix_spawn_file_actions_addfchdir_np(posix_spawn_file_actions_t *file_actions, int fd);
int posix_spawnattr_getflags_np(const posix_spawnattr_t *attr);
int posix_spawnattr_getpgroup_np(const posix_spawnattr_t *attr);
int posix_spawnattr_getsigdefault_np(const posix_spawnattr_t *attr);
int posix_spawnattr_getsigmask_np(const posix_spawnattr_t *attr, sigset_t *sigdefault);
int posix_spawnattr_getsigmask_np(const posix_spawnattr_t *attr, sigset_t *sigmask);
int posix_spawnattr_setsigdefault_np(posix_spawnattr_t *attr, const sigset_t *sigdefault);
int posix_spawnattr_setsigmask_np(posix_spawnattr_t *attr, const sigset_t *sigmask);
int posix_trace_enter(pid_t pid, void *addr, size_t len);
int posix_trace_event(pid_t pid, void *addr, size_t len);
int posix_trace_eventid_get(int event_id, void *addr, size_t len);
int posix_trace_eventid_open(int event_id);
int posix_trace_try_getnext_data(int *data_len);
int posix_trace_try_getnext_event(int *event_data_len);

// SUS Clock Functions
int clock_getres(clockid_t clock_id, struct timespec *res);
int clock_nanosleep(clockid_t clock_id, const struct timespec *rqtp, struct timespec *rmtp);

// SUS Timer Functions
int timer_create(clockid_t clock_id, struct sigevent *sevp, timer_t *timerid);
int timer_delete(timer_t timerid);
int timer_gettime(timer_t timerid, struct itimerspec *value);
int timer_getoverrun(timer_t timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *value);

// SUS Message Queue Functions
int mq_getsetattr(mqd_t mqdes, const struct mq_attr *mqstat);
int mq_notify(mqd_t mqdes, const struct sigevent *notification);
int mq_open(const char *name, int oflag, mode_t mode);
int mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);
int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat);
int mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio, const struct timespec *abs_timeout);
int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio, const struct timespec *abs_timeout);
int mq_unlink(const char *name);

// SUS Semaphore Functions
int sem_close(sem_t *sem);
int sem_destroy(sem_t *sem);
int sem_getvalue(sem_t *sem, int *sval);
int sem_init(sem_t *sem, unsigned int value, int pshared);
sem_t *sem_open(const char *name, int oflag);
int sem_post(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_trywait(sem_t *sem);
int sem_unlink(const char *name);
int sem_wait(sem_t *sem);

// SUS Shared Memory Functions
int shm_open(const char *name, int oflag, mode_t mode);
int shm_unlink(const char *name);
void *shmat(int shmid, const void *shmaddr, int shmflg);
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
int shmdt(const void *shmaddr);

// SUS Thread Functions
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
int pthread_attr_getaffinity_np(pthread_attr_t *attr, size_t cpusetsize, cpu_set_t *cpuset);
int pthread_attr_setaffinity_np(pthread_attr_t *attr, size_t cpusetsize, const cpu_set_t *cpuset);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_init(pthread_barrier_t *barrier, unsigned int count, const pthread_barrierattr_t *attr);
int pthread_barrier_wait(pthread_barrier_t *barrier);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *attr);
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);
int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock_id);
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);
int pthread_mutex_consistent(pthread_mutex_t *mutex);
int pthread_mutex_getprioceiling(pthread_mutex_t *mutex, int *prioceiling);
int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abs_timeout);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abs_timeout);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abs_timeout);
int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);

// SUS Spawn Functions
int posix_spawn(pid_t *pid, const char *path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]);
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *file_actions, int fd);
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *file_actions, int fd);
int posix_spawn_file_actions_addfchdir(posix_spawn_file_actions_t *file_actions, int fd);
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *file_actions, int fd, const char *path);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *file_actions);
int posix_spawn_file_actions_init(posix_spawn_file_actions_t *file_actions);
int posix_spawnattr_destroy(posix_spawnattr_t *attr);
int posix_spawnattr_getflags(const posix_spawnattr_t *attr, short *flags);
int posix_spawnattr_getpgroup(const posix_spawnattr_t *attr, pid_t *pgroup);
int posix_spawnattr_getschedparam(const posix_spawnattr_t *attr, struct sched_param *sched_param);
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *attr, int *sched_policy);
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *attr, sigset_t *sigdefault);
int posix_spawnattr_getsigmask(const posix_spawnattr_t *attr, sigset_t *sigmask);
int posix_spawnattr_init(posix_spawnattr_t *attr);
int posix_spawnattr_setflags(posix_spawnattr_t *attr, short flags);
int posix_spawnattr_setpgroup(posix_spawnattr_t *attr, pid_t pgroup);
int posix_spawnattr_setschedparam(posix_spawnattr_t *attr, const struct sched_param *sched_param);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *attr, int sched_policy);
int posix_spawnattr_setsigdefault(posix_spawnattr_t *attr, const sigset_t *sigdefault);
int posix_spawnattr_setsigmask(posix_spawnattr_t *attr, const sigset_t *sigmask);
int posix_spawnp(pid_t *pid, const char *path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]);
int posix_spawnp(pid_t *pid, const char *path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[]);

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_SUS_H */
