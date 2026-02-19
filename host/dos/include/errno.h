#ifndef MIRIX_DOS_ERRNO_H
#define MIRIX_DOS_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

extern int errno;

#define ENOSYS 38
#define EMFILE 24
#define EBADF  9
#define EINVAL 22
#define EAGAIN 11
#define ENOMEM 12
#define ESRCH  3
#define EBUSY  16
#define EPERM  1

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_ERRNO_H */
