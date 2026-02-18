/*
 * MIRIX Cross-Architecture Runtime System
 * Provides unified runtime abstraction for all supported architectures
 */

#ifndef ARCH_RUNTIME_H
#define ARCH_RUNTIME_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

// Architecture types
typedef enum {
    ARCH_UNKNOWN = 0,
    ARCH_I386 = 1,
    ARCH_X86_64 = 2,
    ARCH_ARM = 3,
    ARCH_ARM64 = 4,
    ARCH_PPC = 5,
    ARCH_M68K = 6,
    ARCH_MIPS = 7
} arch_type_t;

// Platform types
typedef enum {
    PLATFORM_UNKNOWN = 0,
    PLATFORM_WINDOWS = 1,
    PLATFORM_MACOS = 2,
    PLATFORM_LINUX = 3,
    PLATFORM_BSD = 4,
    PLATFORM_DOS = 5,
    PLATFORM_UNIX = 6,
    PLATFORM_EMBEDDED = 7
} platform_type_t;

// Endianness types
typedef enum {
    ENDIAN_UNKNOWN = 0,
    ENDIAN_LITTLE = 1,
    ENDIAN_BIG = 2
} endian_type_t;

// CPU feature flags
typedef struct {
    bool has_fpu;           // Floating point unit
    bool has_mmx;           // MMX instructions
    bool has_sse;           // SSE instructions
    bool has_sse2;          // SSE2 instructions
    bool has_sse3;          // SSE3 instructions
    bool has_sse4_1;        // SSE4.1 instructions
    bool has_sse4_2;        // SSE4.2 instructions
    bool has_avx;           // AVX instructions
    bool has_avx2;          // AVX2 instructions
    bool has_fma;           // FMA instructions
    bool has_neon;          // ARM NEON instructions
    bool has_thumb;         // ARM Thumb mode
    bool has_altivec;       // PowerPC AltiVec
    bool has_virtualization; // Virtualization support
    bool has_64bit;         // 64-bit support
} arch_cpu_features_t;

// Memory management types
typedef enum {
    MEM_TYPE_UNKNOWN = 0,
    MEM_TYPE_RAM = 1,
    MEM_TYPE_ROM = 2,
    MEM_TYPE_FLASH = 3,
    MEM_TYPE_IO = 4,
    MEM_TYPE_DEVICE = 5
} mem_type_t;

// Cache information
typedef struct {
    uint32_t l1d_size;     // L1 data cache size
    uint32_t l1i_size;     // L1 instruction cache size
    uint32_t l2_size;       // L2 cache size
    uint32_t l3_size;       // L3 cache size
    uint32_t line_size;     // Cache line size
    uint32_t associativity;  // Cache associativity
} arch_cache_info_t;

// Architecture information
typedef struct {
    arch_type_t arch_type;
    platform_type_t platform_type;
    endian_type_t endianness;
    const char *arch_name;
    const char *platform_name;
    uint32_t word_size;      // Word size in bytes (4 or 8)
    uint32_t page_size;      // Page size in bytes
    uint32_t address_bits;   // Address space bits (32 or 64)
    arch_cpu_features_t cpu_features;
    arch_cache_info_t cache_info;
    uint32_t cpu_frequency;  // CPU frequency in MHz
    uint32_t num_cores;      // Number of CPU cores
    bool supports_multithreading;
    bool supports_virtual_memory;
    bool supports_mmu;
} arch_info_t;

// Context structure for cross-architecture support
typedef struct {
    // General purpose registers (architecture-specific)
    uint64_t regs[32];
    
    // Special registers
    uint64_t sp;            // Stack pointer
    uint64_t pc;            // Program counter
    uint64_t fp;            // Frame pointer
    
    // Status registers
    uint64_t flags;          // Processor flags
    uint64_t cpsr;          // Current program status register (ARM)
    
    // Floating point registers
    uint64_t fp_regs[32];    // Floating point registers
    uint64_t fp_status;      // Floating point status
    uint64_t fp_control;     // Floating point control
    
    // Debug registers
    uint64_t debug_regs[8];  // Debug registers
    uint64_t breakpoint;     // Breakpoint register
    
    // Architecture-specific data
    void *arch_specific;     // Pointer to arch-specific context
} arch_runtime_context_t;

// Memory management structures
typedef struct {
    uint64_t physical_addr;
    uint64_t virtual_addr;
    uint32_t size;
    mem_type_t type;
    uint32_t flags;
    bool present;
    bool writable;
    bool executable;
    bool user_accessible;
} arch_memory_mapping_t;

// Interrupt handling
typedef enum {
    INT_TYPE_UNKNOWN = 0,
    INT_TYPE_HARDWARE = 1,
    INT_TYPE_SOFTWARE = 2,
    INT_TYPE_TIMER = 3,
    INT_TYPE_NETWORK = 4,
    INT_TYPE_KEYBOARD = 5,
    INT_TYPE_DISK = 6,
    INT_TYPE_SYSCALL = 7
} interrupt_type_t;

typedef struct {
    interrupt_type_t type;
    uint32_t vector;
    uint32_t priority;
    void (*handler)(void *context);
    void *context;
    bool enabled;
} arch_interrupt_handler_t;

// System call interface
typedef struct {
    uint32_t number;
    uint64_t args[6];
    uint64_t result;
    uint64_t error_code;
} arch_syscall_t;

// Runtime configuration
typedef struct {
    bool debug_enabled;
    bool profiling_enabled;
    bool tracing_enabled;
    uint32_t stack_size;
    uint32_t heap_size;
    void *stack_base;
    void *heap_base;
} arch_runtime_config_t;

// Cross-architecture runtime functions
int arch_runtime_init(arch_runtime_config_t *config);
void arch_runtime_cleanup(void);
int arch_runtime_detect_architecture(void);
int arch_runtime_get_info(arch_info_t *info);
int arch_runtime_get_cpu_features(arch_cpu_features_t *features);
int arch_runtime_get_cache_info(arch_cache_info_t *cache);

// Context management
int arch_context_create(arch_runtime_context_t *context);
int arch_context_save(arch_runtime_context_t *context);
int arch_context_restore(arch_runtime_context_t *context);
int arch_context_switch(arch_runtime_context_t *from, arch_runtime_context_t *to);
void arch_context_destroy(arch_runtime_context_t *context);

// Memory management
void *arch_malloc(size_t size);
void *arch_calloc(size_t nmemb, size_t size);
void *arch_realloc(void *ptr, size_t size);
void arch_free(void *ptr);
int arch_memory_map(arch_memory_mapping_t *mapping);
int arch_memory_unmap(uint64_t virtual_addr, size_t size);
int arch_memory_protect(uint64_t addr, size_t size, uint32_t flags);

// Interrupt handling
int arch_interrupt_register(arch_interrupt_handler_t *handler);
int arch_interrupt_unregister(uint32_t vector);
int arch_interrupt_enable(uint32_t vector);
int arch_interrupt_disable(uint32_t vector);
void arch_interrupt_handler(uint32_t vector, void *context);

// System call interface
uint64_t arch_syscall_dispatch(uint32_t number, uint64_t *args);
int arch_syscall_register(uint32_t number, uint64_t (*handler)(arch_syscall_t *syscall));

// Utility functions
uint32_t arch_get_word_size(void);
uint32_t arch_get_page_size(void);
endian_type_t arch_get_endianness(void);
bool arch_is_64bit(void);
bool arch_supports_virtualization(void);
void arch_halt(void);
void arch_reboot(void);
void arch_debug_break(void);

// Atomic operations
uint32_t arch_atomic_add(volatile uint32_t *ptr, uint32_t value);
uint32_t arch_atomic_sub(volatile uint32_t *ptr, uint32_t value);
uint32_t arch_atomic_exchange(volatile uint32_t *ptr, uint32_t value);
bool arch_atomic_compare_exchange(volatile uint32_t *ptr, uint32_t expected, uint32_t desired);
void arch_memory_barrier(void);
void arch_memory_barrier_acquire(void);
void arch_memory_barrier_release(void);

// Timing and performance
uint64_t arch_get_timestamp(void);
uint64_t arch_get_cycle_count(void);
void arch_delay_cycles(uint64_t cycles);
void arch_delay_microseconds(uint32_t microseconds);

// Debug and profiling
void arch_debug_print(const char *format, ...);
void arch_trace_function_enter(const char *function);
void arch_trace_function_exit(const char *function);
void arch_profile_start(const char *name);
void arch_profile_end(const char *name);

// Architecture-specific includes (selected at compile time)
#if defined(ARCH_X86_64)
    // Don't include x86_64 arch.h to avoid conflicts
    // #include "../x86_64/arch.h"
#elif defined(ARCH_I386)
    #include "../i386/nonunix.h"
#elif defined(ARCH_ARM)
    #include "../arm/arch.h"
#elif defined(ARCH_ARM64)
    #include "../arm64/arch.h"
#elif defined(ARCH_PPC)
    #include "../ppc/arch.h"
#elif defined(ARCH_M68K)
    #include "../m68k/arch.h"
#elif defined(ARCH_MIPS)
    #include "../mips/arch.h"
#else
    // Default to x86_64 if no architecture is defined
    // Don't include x86_64 arch.h to avoid conflicts
    // #include "../x86_64/arch.h"
    #warning "No architecture specified, defaulting to x86_64"
#endif

#endif // ARCH_RUNTIME_H
