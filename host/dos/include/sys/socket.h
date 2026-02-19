#ifndef MIRIX_DOS_SYS_SOCKET_H
#define MIRIX_DOS_SYS_SOCKET_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int sa_family_t;
typedef unsigned int socklen_t;

struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
};

struct sockaddr_storage {
    sa_family_t ss_family;
    char __ss_padding[128 - sizeof(sa_family_t)];
};

#define AF_UNSPEC  0
#define AF_INET    2
#define AF_INET6   10
#define AF_UNIX    1
#define PF_INET    AF_INET
#define PF_INET6   AF_INET6
#define PF_UNIX    AF_UNIX

#define SOCK_STREAM 1
#define SOCK_DGRAM  2
#define SOCK_RAW    3

#define MSG_DONTWAIT 0
#define MSG_NOSIGNAL 0

#define SOL_SOCKET 1
#define SO_REUSEADDR 2

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_SYS_SOCKET_H */
