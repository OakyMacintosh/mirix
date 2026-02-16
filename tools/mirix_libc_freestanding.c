// MIRIX libc - Freestanding version for cross-compilation

#include <stddef.h>
#include <stdarg.h>

// Basic types for freestanding environment
typedef int pid_t;
typedef long ssize_t;

// Minimal printf implementation for freestanding environment
int mirix_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    // For cross-compilation, we'll use a minimal implementation
    // In the actual MIRIX runtime, this would use the kernel's write syscall
    const char *p = format;
    int count = 0;
    
    while (*p) {
        if (*p == '%' && *(p+1)) {
            p++;
            switch (*p) {
                case 's': {
                    const char *s = va_arg(args, const char*);
                    (void)s; // Suppress unused warning
                    count += 1; // Placeholder
                    break;
                }
                case 'd': {
                    int d = va_arg(args, int);
                    (void)d; // Suppress unused warning
                    count += 1; // Placeholder
                    break;
                }
                case 'u': {
                    unsigned int u = va_arg(args, unsigned int);
                    (void)u; // Suppress unused warning
                    count += 1; // Placeholder
                    break;
                }
                case 'z': {
                    if (*(p+1) == 'u') {
                        size_t zu = va_arg(args, size_t);
                        (void)zu; // Suppress unused warning
                        count += 1; // Placeholder
                        p++; // Skip the 'u'
                    }
                    break;
                }
                default:
                    count++;
                    break;
            }
        } else {
            // Write character (would be syscall in real implementation)
            count++;
        }
        p++;
    }
    
    va_end(args);
    return count;
}

// Minimal exit implementation
void mirix_exit(int status) {
    // In real implementation, this would call the kernel exit syscall
    // For cross-compilation, we just return
    (void)status;
}

// Minimal string operations
size_t mirix_strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return p - s;
}

char *mirix_strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

int mirix_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void *mirix_memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

void *mirix_memset(void *s, int c, size_t n) {
    char *p = s;
    while (n--) *p++ = c;
    return s;
}

int mirix_atoi(const char *nptr) {
    int result = 0;
    int sign = 1;
    
    // Skip whitespace
    while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n') nptr++;
    
    // Handle sign
    if (*nptr == '-') {
        sign = -1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }
    
    // Convert digits
    while (*nptr >= '0' && *nptr <= '9') {
        result = result * 10 + (*nptr - '0');
        nptr++;
    }
    
    return sign * result;
}

// Stub implementations for cross-compilation
void *mirix_malloc(size_t size) {
    (void)size;
    return (void*)0; // Not available in freestanding
}

void mirix_free(void *ptr) {
    (void)ptr; // Not available in freestanding
}

void *mirix_realloc(void *ptr, size_t size) {
    (void)ptr; (void)size;
    return (void*)0; // Not available in freestanding
}

// File operations (stubs for cross-compilation)
ssize_t mirix_write(int fd, const void *buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return count; // Stub
}

ssize_t mirix_read(int fd, void *buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return 0; // Stub
}

int mirix_open(const char *pathname, int flags, ...) {
    (void)pathname; (void)flags;
    return -1; // Stub
}

int mirix_close(int fd) {
    (void)fd;
    return 0; // Stub
}

// Process operations (stubs for cross-compilation)
pid_t mirix_fork(void) {
    return -1; // Stub
}

int mirix_execve(const char *pathname, char *const argv[], char *const envp[]) {
    (void)pathname; (void)argv; (void)envp;
    return -1; // Stub
}

pid_t mirix_waitpid(pid_t pid, int *status, int options) {
    (void)pid; (void)status; (void)options;
    return -1; // Stub
}

int mirix_kill(pid_t pid, int sig) {
    (void)pid; (void)sig;
    return -1; // Stub
}
