#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

 #include <getopt.h>

static int dos_optind = 1;
static int dos_optpos = 1;

static int dos_handle_long_option(int argc, char *const argv[], const struct option *longopts,
                                   int *longindex, char **optarg_ptr, const char *name, char *value) {
    size_t idx = 0;
    while (longopts[idx].name) {
        if (strcmp(longopts[idx].name, name) == 0) {
            if (longindex) {
                *longindex = (int)idx;
            }
            if (longopts[idx].has_arg == required_argument || (longopts[idx].has_arg == optional_argument && value)) {
                *optarg_ptr = value ? value : argv[dos_optind + 1];
                if (!value) {
                    dos_optind++;
                }
            } else {
                *optarg_ptr = NULL;
            }
            return longopts[idx].val;
        }
        idx++;
    }
    return '?';
}

static int dos_getopt_internal(int argc, char *const argv[], const char *optstring,
                               const struct option *longopts, int *longindex, int long_only) {
    optarg = NULL;
    optopt = 0;

    if (dos_optind >= argc) {
        return -1;
    }

    char *arg = argv[dos_optind];
    if (arg[0] != '-') {
        return -1;
    }

    if (arg[1] == '-') {
        char *eq = strchr(arg, '=');
        char *name = arg + 2;
        char *value = NULL;
        if (eq) {
            *eq = '\0';
            value = eq + 1;
        }

        int result = dos_handle_long_option(argc, argv, longopts, longindex, &optarg, name, value);
        dos_optind++;
        return result;
    }

    if (!optstring) {
        return -1;
    }

    char opt = arg[dos_optpos];
    dos_optpos++;

    if (arg[dos_optpos] == '\0') {
        dos_optind++;
        dos_optpos = 1;
    }

    const char *found = strchr(optstring, opt);
    if (!found) {
        optopt = opt;
        return '?';
    }

    if (found[1] == ':') {
        if (arg[dos_optpos] != '\0') {
            optarg = &arg[dos_optpos];
            dos_optind++;
            dos_optpos = 1;
        } else if (dos_optind + 1 < argc) {
            optarg = argv[dos_optind + 1];
            dos_optind += 2;
            dos_optpos = 1;
        } else {
            optopt = opt;
            return '?';
        }
    }

    return opt;
}

int getopt_long(int argc, char *const argv[], const char *optstring,
                const struct option *longopts, int *longindex) {
    if (!longopts) {
        return dos_getopt_internal(argc, argv, optstring, NULL, NULL, 0);
    }
    return dos_getopt_internal(argc, argv, optstring, longopts, longindex, 0);
}

int getopt_long_only(int argc, char *const argv[], const char *optstring,
                     const struct option *longopts, int *longindex) {
    return dos_getopt_internal(argc, argv, optstring, longopts, longindex, 1);
}

int kqueue(void) {
    errno = ENOSYS;
    return -1;
}

int kevent(int kq, const struct kevent *changelist, int nchanges,
           struct kevent *eventlist, int nevents, const struct timespec *timeout) {
    (void)kq; (void)changelist; (void)nchanges; (void)eventlist; (void)nevents; (void)timeout;
    errno = ENOSYS;
    return -1;
}

int shm_unlink(const char *name) {
    (void)name;
    return 0;
}

int madvise(void *addr, size_t length, int advice) {
    (void)addr; (void)length; (void)advice;
    return 0;
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
    if (!memptr || alignment == 0 || (alignment & (alignment - 1)) != 0) {
        return EINVAL;
    }

    void *ptr = malloc(size);
    if (!ptr) {
        return ENOMEM;
    }

    *memptr = ptr;
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
    (void)req; (void)rem;
    return 0;
}

int socket(int domain, int type, int protocol) {
    (void)domain; (void)type; (void)protocol;
    errno = ENOSYS;
    return -1;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    (void)sockfd; (void)addr; (void)addrlen;
    errno = ENOSYS;
    return -1;
}

int listen(int sockfd, int backlog) {
    (void)sockfd; (void)backlog;
    errno = ENOSYS;
    return -1;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    (void)sockfd; (void)addr; (void)addrlen;
    errno = ENOSYS;
    return -1;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    (void)sockfd; (void)addr; (void)addrlen;
    errno = ENOSYS;
    return -1;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    (void)sockfd; (void)buf; (void)len; (void)flags;
    errno = ENOSYS;
    return -1;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    (void)sockfd; (void)buf; (void)len; (void)flags;
    errno = ENOSYS;
    return -1;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    (void)addr; (void)length; (void)prot; (void)flags; (void)fd; (void)offset;
    errno = ENOSYS;
    return MAP_FAILED;
}

int munmap(void *addr, size_t length) {
    (void)addr; (void)length;
    return 0;
}

pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage) {
    (void)pid; (void)status; (void)options; (void)rusage;
    errno = ENOSYS;
    return -1;
}
