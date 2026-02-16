#ifndef ARCH_X86_64_H
#define ARCH_X86_64_H

#include <stdint.h>
#include <stdbool.h>

// x86_64 architecture-specific definitions

// Context structure for x86_64
typedef struct {
    uint64_t rsp;    // Stack pointer
    uint64_t rbp;    // Base pointer
    uint64_t rbx;    // General purpose registers
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;    // Instruction pointer (saved separately)
} arch_context_t;

// CPU information structure
typedef struct {
    char vendor[13];     // CPU vendor string
    int family;         // CPU family
    int model;          // CPU model
    int stepping;       // CPU stepping
} arch_cpu_info_t;

// Page table entries
typedef struct {
    uint64_t present : 1;
    uint64_t writable : 1;
    uint64_t user : 1;
    uint64_t writethrough : 1;
    uint64_t nocache : 1;
    uint64_t accessed : 1;
    uint64_t dirty : 1;
    uint64_t pat : 1;
    uint64_t global : 1;
    uint64_t available1 : 3;
    uint64_t frame : 40;
    uint64_t available2 : 11;
    uint64_t nx : 1;
} __attribute__((packed)) pte_t;

// Page directory/table entries
typedef struct {
    uint64_t present : 1;
    uint64_t writable : 1;
    uint64_t user : 1;
    uint64_t writethrough : 1;
    uint64_t nocache : 1;
    uint64_t accessed : 1;
    uint64_t available1 : 1;
    uint64_t size : 1;
    uint64_t available2 : 4;
    uint64_t frame : 40;
    uint64_t available3 : 11;
    uint64_t nx : 1;
} __attribute__((packed)) pde_t;

// Control registers
#define CR0_PE          0x00000001  // Protected mode enable
#define CR0_MP          0x00000002  // Monitor coprocessor
#define CR0_EM          0x00000004  // Emulation
#define CR0_TS          0x00000008  // Task switched
#define CR0_ET          0x00000010  // Extension type
#define CR0_NE          0x00000020  // Numeric error
#define CR0_WP          0x00010000  // Write protect
#define CR0_AM          0x00040000  // Alignment mask
#define CR0_NW          0x20000000  // Not write-through
#define CR0_CD          0x40000000  // Cache disable
#define CR0_PG          0x80000000  // Paging

#define CR4_VME         0x00000001  // Virtual-8086 mode extensions
#define CR4_PVI         0x00000002  // Protected-mode virtual interrupts
#define CR4_TSD         0x00000004  // Time-stamp disable
#define CR4_DE          0x00000008  // Debugging extensions
#define CR4_PSE         0x00000010  // Page size extension
#define CR4_PAE         0x00000020  // Physical address extension
#define CR4_MCE         0x00000040  // Machine-check enable
#define CR4_PGE         0x00000080  // Page global enable
#define CR4_PCE         0x00000100  // Performance-monitor counter enable
#define CR4_OSFXSR      0x00000200  // OS support for FXSAVE/FXRSTOR
#define CR4_OSXMMEXCPT  0x00000400  // OS support for unmasked SIMD FP exceptions
#define CR4_UMIP        0x00000800  // User-mode instruction prevention
#define CR4_VMXE        0x00002000  // VMX enable
#define CR4_SMXE        0x00004000  // SMX enable
#define CR4_FSGSBASE    0x00010000  // FSGSBASE enable
#define CR4_PCIDE       0x00020000  // PCID enable
#define CR4_OSXSAVE     0x00040000  // XSAVE and XRESTORE enabled
#define CR4_SMEP        0x10000000  // Supervisor-mode execution prevention
#define CR4_SMAP        0x20000000  // Supervisor-mode access prevention

// Memory management
#define PAGE_SIZE       4096
#define PAGE_SHIFT      12
#define PAGE_MASK       (PAGE_SIZE - 1)

// Segment selectors
#define KERNEL_CS       0x08
#define KERNEL_DS       0x10
#define USER_CS         0x18
#define USER_DS         0x20

// Interrupt vectors
#define IRQ_BASE        0x20
#define SYSCALL_VECTOR  0x80

// Architecture-specific functions
int arch_init(void);
int arch_context_switch(arch_context_t *old_ctx, arch_context_t *new_ctx);
int arch_save_context(arch_context_t *ctx);
int arch_restore_context(arch_context_t *ctx);
int arch_get_cpu_info(arch_cpu_info_t *info);

// Memory and synchronization
void arch_memory_barrier(void);
void arch_compiler_barrier(void);

// CPU control
void arch_halt(void);
void arch_enable_interrupts(void);
void arch_disable_interrupts(void);
uint64_t arch_get_tsc(void);

// Internal functions
static int arch_setup_memory_management(void);
static int arch_setup_interrupts(void);
static void cpuid(uint32_t eax, uint32_t ecx, uint32_t *eax_out, uint32_t *ebx_out, 
                  uint32_t *ecx_out, uint32_t *edx_out);

#endif // ARCH_X86_64_H
