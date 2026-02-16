#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/syscall.h>

#include "mirix_libc.h"

// MIRIX libc - Minimal C library for MIRIX userland programs

// Minimal printf implementation
int mirix_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    // Simple printf implementation - just pass through to host printf for now
    int result = vprintf(format, args);
    
    va_end(args);
    return result;
}

// Minimal exit implementation
void mirix_exit(int status) {
    // Call the kernel exit syscall
    syscall(SYS_exit, status);
}

// Minimal write implementation
ssize_t mirix_write(int fd, const void *buf, size_t count) {
    // Call the kernel write syscall
    return syscall(SYS_write, fd, buf, count);
}

// Minimal read implementation
ssize_t mirix_read(int fd, void *buf, size_t count) {
    // Call the kernel read syscall
    return syscall(SYS_read, fd, buf, count);
}

// Minimal open implementation
int mirix_open(const char *pathname, int flags, ...) {
    mode_t mode = 0;
    
    // Handle variable arguments for mode
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }
    
    // Call the kernel open syscall
    return syscall(SYS_open, pathname, flags, mode);
}

// Minimal close implementation
int mirix_close(int fd) {
    // Call the kernel close syscall
    return syscall(SYS_close, fd);
}

// Minimal fork implementation
pid_t mirix_fork(void) {
    // Call the kernel fork syscall
    return syscall(SYS_fork);
}

// Minimal execve implementation
int mirix_execve(const char *pathname, char *const argv[], char *const envp[]) {
    // Call the kernel exec syscall
    return syscall(SYS_execve, pathname, argv, envp);
}

// Minimal waitpid implementation
pid_t mirix_waitpid(pid_t pid, int *status, int options) {
    // Call the kernel wait syscall
    return syscall(SYS_wait4, pid, status, options, NULL);
}

// Minimal kill implementation
int mirix_kill(pid_t pid, int sig) {
    // Call the kernel kill syscall
    return syscall(SYS_kill, pid, sig);
}

// Minimal malloc implementation
void *mirix_malloc(size_t size) {
    // For now, use host malloc
    return malloc(size);
}

// Minimal free implementation
void mirix_free(void *ptr) {
    // For now, use host free
    free(ptr);
}

// Minimal realloc implementation
void *mirix_realloc(void *ptr, size_t size) {
    // For now, use host realloc
    return realloc(ptr, size);
}

// Minimal strlen implementation
size_t mirix_strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return p - s;
}

// Minimal strcpy implementation
char *mirix_strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

// Minimal strcmp implementation
int mirix_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Minimal memcpy implementation
void *mirix_memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}

// Minimal memset implementation
void *mirix_memset(void *s, int c, size_t n) {
    char *p = s;
    while (n--) *p++ = c;
    return s;
}

// Minimal atoi implementation
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

// Debug shell implementation
void mirix_debug_shell(void) {
    printf("MIRIX Debug Shell\n");
    printf("(debug)%% Type 'exit' to return to kernel\n");
    printf("(debug)%% Commands: help, mem, ps, exit\n");
    
    char line[256];
    while (1) {
        printf("(debug)%% ");
        fflush(stdout);
        
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        line[strcspn(line, "\n")] = '\0';
        
        if (strcmp(line, "exit") == 0) {
            printf("(debug)%% Exiting debug shell\n");
            break;
        } else if (strcmp(line, "help") == 0) {
            printf("(debug)%% Available commands:\n");
            printf("(debug)%%   help    - Show this help\n");
            printf("(debug)%%   mem     - Show memory usage\n");
            printf("(debug)%%   ps      - Show processes\n");
            printf("(debug)%%   exit    - Exit debug shell\n");
        } else if (strcmp(line, "mem") == 0) {
            printf("(debug)%% Memory usage:\n");
            printf("(debug)%%   Kernel: %d MB\n", 512); // Placeholder
            printf("(debug)%%   Userland: %d MB\n", 256); // Placeholder
        } else if (strcmp(line, "ps") == 0) {
            printf("(debug)%% Process list:\n");
            printf("(debug)%%   PID 1: kernel (parent)\n");
            printf("(debug)%%   PID 2: init program\n");
        } else if (strlen(line) > 0) {
            printf("(debug)%% Unknown command: %s\n", line);
            printf("(debug)%% Type 'help' for available commands\n");
        }
    }
}
