#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

#include "arch.h"

// x86_64 architecture-specific code for MIRIX

// CPU feature detection
typedef struct {
    bool sse2;
    bool sse3;
    bool sse4_1;
    bool sse4_2;
    bool avx;
    bool avx2;
    bool fma;
} x86_64_features_t;

static x86_64_features_t cpu_features;

// CPUID instruction wrapper
static void cpuid(uint32_t eax, uint32_t ecx, uint32_t *eax_out, uint32_t *ebx_out, 
                uint32_t *ecx_out, uint32_t *edx_out) {
    __asm__ volatile ("cpuid"
                    : "=a" (*eax_out), "=b" (*ebx_out), "=c" (*ecx_out), "=d" (*edx_out)
                    : "a" (eax), "c" (ecx));
}

// Initialize architecture-specific features
int arch_init(void) {
    printf("Initializing x86_64 architecture support...\n");
    
    // Detect CPU features
    uint32_t eax, ebx, ecx, edx;
    
    // Get basic feature flags
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    cpu_features.sse2 = (edx & (1 << 26)) != 0;
    cpu_features.sse3 = (ecx & (1 << 0)) != 0;
    
    // Get extended feature flags
    cpuid(7, 0, &eax, &ebx, &ecx, &edx);
    cpu_features.avx2 = (ebx & (1 << 5)) != 0;
    cpu_features.fma = (ecx & (1 << 12)) != 0;
    
    printf("x86_64 CPU features detected:\n");
    printf("  SSE2: %s\n", cpu_features.sse2 ? "YES" : "NO");
    printf("  SSE3: %s\n", cpu_features.sse3 ? "YES" : "NO");
    printf("  AVX2: %s\n", cpu_features.avx2 ? "YES" : "NO");
    printf("  FMA:  %s\n", cpu_features.fma ? "YES" : "NO");
    
    // Set up architecture-specific memory management
    arch_setup_memory_management();
    
    // Set up interrupt handling
    arch_setup_interrupts();
    
    printf("x86_64 architecture support initialized\n");
    return 0;
}

// Memory management setup
static int arch_setup_memory_management(void) {
    printf("Setting up x86_64 memory management...\n");
    
    // In a real kernel, this would set up page tables
    // For lazykernel, we rely on host memory management
    
    // Get page size
    long page_size = sysconf(_SC_PAGESIZE);
    printf("  Page size: %ld bytes\n", page_size);
    
    return 0;
}

// Interrupt handling setup
static int arch_setup_interrupts(void) {
    printf("Setting up x86_64 interrupt handling...\n");
    
    // In a real kernel, this would set up IDT and interrupt handlers
    // For lazykernel, we rely on host interrupt handling
    
    return 0;
}

// Architecture-specific system call entry
void arch_syscall_entry(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, 
                     uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    printf("arch_syscall_entry: syscall=%llu, args=%llu,%llu,%llu,%llu,%llu\n",
           (unsigned long long)syscall_num, (unsigned long long)arg1, 
           (unsigned long long)arg2, (unsigned long long)arg3, 
           (unsigned long long)arg4, (unsigned long long)arg5);
    
    // This would dispatch to the appropriate system call handler
    // For now, just log the call
}

// Context switching
int arch_context_switch(arch_context_t *old_ctx, arch_context_t *new_ctx) {
    printf("arch_context_switch: old=%p, new=%p\n", old_ctx, new_ctx);
    
    // In a real kernel, this would perform actual context switch
    // For lazykernel, we rely on host threading
    
    return 0;
}

// Save current context
int arch_save_context(arch_context_t *ctx) {
    if (!ctx) return -1;
    
    // Restore registers
    __asm__ volatile (
        "movq %0, %%rsp\n"
        "movq %1, %%rbp\n"
        "movq %2, %%rbx\n"
        "movq %3, %%r12\n"
        "movq %4, %%r13\n"
        "movq %5, %%r14\n"
        "movq %6, %%r15\n"
        : "=m" (ctx->rsp), "=m" (ctx->rbp), "=m" (ctx->rbx),
          "=m" (ctx->r12), "=m" (ctx->r13), "=m" (ctx->r14), "=m" (ctx->r15)
        : : "memory"
    );
    
    return 0;
}

// Restore context
int arch_restore_context(arch_context_t *ctx) {
    if (!ctx) return -1;
    
    // Restore registers
    __asm__ volatile (
        "movq %%rsp, %0\n"
        "movq %%rbp, %1\n"
        "movq %%rbx, %2\n"
        "movq %%r12, %3\n"
        "movq %%r13, %4\n"
        "movq %%r14, %5\n"
        "movq %%r15, %6\n"
        : "=m" (ctx->rsp), "=m" (ctx->rbp), "=m" (ctx->rbx),
          "=m" (ctx->r12), "=m" (ctx->r13), "=m" (ctx->r14), "=m" (ctx->r15)
        : : "memory"
    );
    
    return 0;
}

// Get CPU information
int arch_get_cpu_info(arch_cpu_info_t *info) {
    if (!info) return -1;
    
    // Get vendor string
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    
    // Reconstruct vendor string
    memcpy(info->vendor, &ebx, 4);
    memcpy(info->vendor + 4, &edx, 4);
    memcpy(info->vendor + 8, &ecx, 4);
    info->vendor[12] = '\0';
    
    // Get model/family info
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    info->stepping = eax & 0xF;
    info->model = (eax >> 4) & 0xF;
    info->family = (eax >> 8) & 0xF;
    
    printf("arch_get_cpu_info: vendor=%s, family=%d, model=%d, stepping=%d\n",
           info->vendor, info->family, info->model, info->stepping);
    
    return 0;
}

// Memory barriers
void arch_memory_barrier(void) {
    __asm__ volatile ("mfence" ::: "memory");
}

void arch_compiler_barrier(void) {
    __asm__ volatile ("" ::: "memory");
}

// Halt CPU
void arch_halt(void) {
    printf("arch_halt: halting CPU\n");
    __asm__ volatile ("hlt");
}

// Get current timestamp counter
uint64_t arch_get_tsc(void) {
    uint32_t low, high;
    __asm__ volatile ("rdtsc" : "=a" (low), "=d" (high));
    return ((uint64_t)high << 32) | low;
}

// Enable/disable interrupts
void arch_enable_interrupts(void) {
    __asm__ volatile ("sti");
}

void arch_disable_interrupts(void) {
    __asm__ volatile ("cli");
}
