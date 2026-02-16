#ifndef MIRIX_LIBC_FREESTANDING_H
#define MIRIX_LIBC_FREESTANDING_H

#include <stddef.h>
#include <stdarg.h>

// Basic types for freestanding environment
typedef int pid_t;
typedef long ssize_t;

// Standard library functions
int mirix_printf(const char *format, ...);
void mirix_exit(int status);

// File operations (stubs for cross-compilation)
ssize_t mirix_write(int fd, const void *buf, size_t count);
ssize_t mirix_read(int fd, void *buf, size_t count);
int mirix_open(const char *pathname, int flags, ...);
int mirix_close(int fd);

// Process operations (stubs for cross-compilation)
pid_t mirix_fork(void);
int mirix_execve(const char *pathname, char *const argv[], char *const envp[]);
pid_t mirix_waitpid(pid_t pid, int *status, int options);
int mirix_kill(pid_t pid, int sig);

// Memory operations (stubs for cross-compilation)
void *mirix_malloc(size_t size);
void mirix_free(void *ptr);
void *mirix_realloc(void *ptr, size_t size);

// String operations
size_t mirix_strlen(const char *s);
char *mirix_strcpy(char *dest, const char *src);
int mirix_strcmp(const char *s1, const char *s2);
void *mirix_memcpy(void *dest, const void *src, size_t n);
void *mirix_memset(void *s, int c, size_t n);
int mirix_atoi(const char *nptr);

// Standard constants
#define EOF (-1)

#endif // MIRIX_LIBC_FREESTANDING_H
