/*
 * Architecture Detection Functions for MIRIX Cross-Architecture Runtime
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "arch_runtime.h"

// CPUID instruction wrapper for x86 family
#ifdef __x86_64__ || __i386__
static void cpuid(uint32_t eax, uint32_t ecx, uint32_t *eax_out, uint32_t *ebx_out, 
                uint32_t *ecx_out, uint32_t *edx_out) {
    __asm__ volatile ("cpuid"
                    : "=a" (*eax_out), "=b" (*ebx_out), "=c" (*ecx_out), "=d" (*edx_out)
                    : "a" (eax), "c" (ecx));
}
#endif

// Detect x86/x86_64 family
static int detect_x86_family(arch_info_t *info) {
#if defined(__x86_64__) || defined(_M_X64)
    info->arch_type = ARCH_X86_64;
    info->arch_name = "x86_64";
    info->word_size = 8;
    info->address_bits = 64;
    info->cpu_features.has_64bit = true;
#elif defined(__i386__) || defined(_M_IX86)
    info->arch_type = ARCH_I386;
    info->arch_name = "i386";
    info->word_size = 4;
    info->address_bits = 32;
    info->cpu_features.has_64bit = false;
#else
    return -1;
#endif

    info->supports_multithreading = true;
    info->supports_virtual_memory = true;
    info->supports_mmu = true;
    
    // Detect CPU features using CPUID
#ifdef __x86_64__ || __i386__
    uint32_t eax, ebx, ecx, edx;
    
    // Get basic feature flags
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    
    info->cpu_features.has_fpu = (edx & (1 << 0)) != 0;
    info->cpu_features.has_mmx = (edx & (1 << 23)) != 0;
    info->cpu_features.has_sse = (edx & (1 << 25)) != 0;
    info->cpu_features.has_sse2 = (edx & (1 << 26)) != 0;
    info->cpu_features.has_sse3 = (ecx & (1 << 0)) != 0;
    
    // Get extended feature flags
    cpuid(7, 0, &eax, &ebx, &ecx, &edx);
    info->cpu_features.has_avx2 = (ebx & (1 << 5)) != 0;
    info->cpu_features.has_fma = (ecx & (1 << 12)) != 0;
    
    // Get extended CPU info
    cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx);
    info->cpu_features.has_avx = (ecx & (1 << 28)) != 0;
    
    // Get virtualization support
    cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    info->cpu_features.has_virtualization = (ecx & (1 << 5)) != 0; // VMX
    
    printf("x86 CPU features: FPU=%s MMX=%s SSE=%s SSE2=%s SSE3=%s AVX=%s AVX2=%s FMA=%s\n",
           info->cpu_features.has_fpu ? "YES" : "NO",
           info->cpu_features.has_mmx ? "YES" : "NO",
           info->cpu_features.has_sse ? "YES" : "NO",
           info->cpu_features.has_sse2 ? "YES" : "NO",
           info->cpu_features.has_sse3 ? "YES" : "NO",
           info->cpu_features.has_avx ? "YES" : "NO",
           info->cpu_features.has_avx2 ? "YES" : "NO",
           info->cpu_features.has_fma ? "YES" : "NO");
#endif

    // Get cache information
    info->cache_info.l1d_size = 32 * 1024;  // Default L1 data cache
    info->cache_info.l1i_size = 32 * 1024;  // Default L1 instruction cache
    info->cache_info.l2_size = 256 * 1024;  // Default L2 cache
    info->cache_info.l3_size = 2 * 1024 * 1024; // Default L3 cache
    info->cache_info.line_size = 64;        // Default cache line size
    info->cache_info.associativity = 8;     // Default associativity
    
    // Get CPU frequency (approximate)
    info->cpu_frequency = 2000; // Default 2GHz
    
    // Get number of cores
    info->num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (info->num_cores <= 0) {
        info->num_cores = 1;
    }
    
    return 0;
}

// Detect ARM/ARM64 family
static int detect_arm_family(arch_info_t *info) {
#if defined(__aarch64__) || defined(_M_ARM64)
    info->arch_type = ARCH_ARM64;
    info->arch_name = "ARM64";
    info->word_size = 8;
    info->address_bits = 64;
    info->cpu_features.has_64bit = true;
#elif defined(__arm__) || defined(_M_ARM)
    info->arch_type = ARCH_ARM;
    info->arch_name = "ARM";
    info->word_size = 4;
    info->address_bits = 32;
    info->cpu_features.has_64bit = false;
#else
    return -1;
#endif

    info->supports_multithreading = true;
    info->supports_virtual_memory = true;
    info->supports_mmu = true;
    
    // ARM-specific feature detection
#ifdef __ARM_NEON
    info->cpu_features.has_neon = true;
#endif

#ifdef __thumb__
    info->cpu_features.has_thumb = true;
#endif

    info->cpu_features.has_fpu = true; // Most ARM CPUs have FPU
    
    printf("ARM CPU features: FPU=%s NEON=%s THUMB=%s\n",
           info->cpu_features.has_fpu ? "YES" : "NO",
           info->cpu_features.has_neon ? "YES" : "NO",
           info->cpu_features.has_thumb ? "YES" : "NO");

    // ARM cache information (typical values)
    info->cache_info.l1d_size = 32 * 1024;
    info->cache_info.l1i_size = 32 * 1024;
    info->cache_info.l2_size = 256 * 1024;
    info->cache_info.l3_size = 0; // ARM typically doesn't have L3
    info->cache_info.line_size = 64;
    info->cache_info.associativity = 4;
    
    info->cpu_frequency = 1500; // Default 1.5GHz
    info->num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (info->num_cores <= 0) {
        info->num_cores = 1;
    }
    
    return 0;
}

// Detect PowerPC family
static int detect_ppc_family(arch_info_t *info) {
#if defined(__powerpc__) || defined(_M_PPC)
#if defined(__powerpc64__) || defined(_M_PPC64)
    info->arch_type = ARCH_PPC;
    info->arch_name = "PowerPC64";
    info->word_size = 8;
    info->address_bits = 64;
    info->cpu_features.has_64bit = true;
#else
    info->arch_type = ARCH_PPC;
    info->arch_name = "PowerPC";
    info->word_size = 4;
    info->address_bits = 32;
    info->cpu_features.has_64bit = false;
#endif
#else
    return -1;
#endif
}

// Detect Motorola 68K family
static int detect_m68k_family(arch_info_t *info) {
#if defined(__m68k__) || defined(_M_M68K)
    info->arch_type = ARCH_M68K;
    info->arch_name = "Motorola 68K";
    info->word_size = 4;
    info->address_bits = 32;
    info->cpu_features.has_64bit = false;
    
    info->supports_multithreading = false; // 68K typically doesn't support multithreading
    info->supports_virtual_memory = false;
    info->supports_mmu = false;
    
    info->cpu_features.has_fpu = false; // Some 68K variants have FPU, but not all
    info->cpu_features.has_virtualization = false;
    
    printf("Motorola 68K CPU detected\n");

    // 68K cache information (minimal)
    info->cache_info.l1d_size = 8 * 1024;
    info->cache_info.l1i_size = 8 * 1024;
    info->cache_info.l2_size = 0;
    info->cache_info.l3_size = 0;
    info->cache_info.line_size = 16;
    info->cache_info.associativity = 2;
    
    info->cpu_frequency = 50; // Typical 68K frequency
    info->num_cores = 1; // 68K is single-core
    
    return 0;
#else
    return -1;
#endif
}

// Detect MIPS family
static int detect_mips_family(arch_info_t *info) {
#if defined(__mips__) || defined(_M_MIPS)
#if defined(__mips64) || defined(_M_MIPS64)
    info->arch_type = ARCH_MIPS;
    info->arch_name = "MIPS64";
    info->word_size = 8;
    info->address_bits = 64;
    info->cpu_features.has_64bit = true;
#else
    info->arch_type = ARCH_MIPS;
    info->arch_name = "MIPS";
    info->word_size = 4;
    info->address_bits = 32;
    info->cpu_features.has_64bit = false;
#endif

    info->supports_multithreading = true;
    info->supports_virtual_memory = true;
    info->supports_mmu = true;
    
    info->cpu_features.has_fpu = true; // Most MIPS CPUs have FPU
    
    printf("MIPS CPU features: FPU=%s\n",
           info->cpu_features.has_fpu ? "YES" : "NO");

    // MIPS cache information
    info->cache_info.l1d_size = 32 * 1024;
    info->cache_info.l1i_size = 32 * 1024;
    info->cache_info.l2_size = 256 * 1024;
    info->cache_info.l3_size = 0;
    info->cache_info.line_size = 32;
    info->cache_info.associativity = 4;
    
    info->cpu_frequency = 1000; // Default 1GHz
    info->num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (info->num_cores <= 0) {
        info->num_cores = 1;
    }
    
    return 0;
#else
    return -1;
#endif
}

// Get platform name
static const char* get_platform_name(platform_type_t platform) {
    switch (platform) {
        case PLATFORM_WINDOWS: return "Windows";
        case PLATFORM_MACOS: return "macOS";
        case PLATFORM_LINUX: return "Linux";
        case PLATFORM_BSD: return "BSD";
        case PLATFORM_DOS: return "DOS";
        case PLATFORM_UNIX: return "UNIX";
        case PLATFORM_EMBEDDED: return "Embedded";
        default: return "Unknown";
    }
}

// Get endianness name
static const char* get_endianness_name(endian_type_t endianness) {
    switch (endianness) {
        case ENDIAN_LITTLE: return "Little Endian";
        case ENDIAN_BIG: return "Big Endian";
        default: return "Unknown";
    }
}

// Print architecture information
void arch_print_info(void) {
    arch_info_t info;
    
    if (arch_runtime_get_info(&info) != 0) {
        printf("Failed to get architecture information\n");
        return;
    }
    
    printf("\n=== Architecture Information ===\n");
    printf("Architecture: %s\n", info.arch_name);
    printf("Platform: %s\n", get_platform_name(info.platform_type));
    printf("Endianness: %s\n", get_endianness_name(info.endianness));
    printf("Word Size: %u bytes\n", info.word_size);
    printf("Address Bits: %u\n", info.address_bits);
    printf("Page Size: %u bytes\n", info.page_size);
    printf("CPU Frequency: %u MHz\n", info.cpu_frequency);
    printf("Number of Cores: %u\n", info.num_cores);
    printf("Supports Multithreading: %s\n", info.supports_multithreading ? "YES" : "NO");
    printf("Supports Virtual Memory: %s\n", info.supports_virtual_memory ? "YES" : "NO");
    printf("Supports MMU: %s\n", info.supports_mmu ? "YES" : "NO");
    
    printf("\n=== CPU Features ===\n");
    printf("FPU: %s\n", info.cpu_features.has_fpu ? "YES" : "NO");
    printf("MMX: %s\n", info.cpu_features.has_mmx ? "YES" : "NO");
    printf("SSE: %s\n", info.cpu_features.has_sse ? "YES" : "NO");
    printf("SSE2: %s\n", info.cpu_features.has_sse2 ? "YES" : "NO");
    printf("SSE3: %s\n", info.cpu_features.has_sse3 ? "YES" : "NO");
    printf("SSE4.1: %s\n", info.cpu_features.has_sse4_1 ? "YES" : "NO");
    printf("SSE4.2: %s\n", info.cpu_features.has_sse4_2 ? "YES" : "NO");
    printf("AVX: %s\n", info.cpu_features.has_avx ? "YES" : "NO");
    printf("AVX2: %s\n", info.cpu_features.has_avx2 ? "YES" : "NO");
    printf("FMA: %s\n", info.cpu_features.has_fma ? "YES" : "NO");
    printf("NEON: %s\n", info.cpu_features.has_neon ? "YES" : "NO");
    printf("Thumb: %s\n", info.cpu_features.has_thumb ? "YES" : "NO");
    printf("AltiVec: %s\n", info.cpu_features.has_altivec ? "YES" : "NO");
    printf("Virtualization: %s\n", info.cpu_features.has_virtualization ? "YES" : "NO");
    printf("64-bit: %s\n", info.cpu_features.has_64bit ? "YES" : "NO");
    
    printf("\n=== Cache Information ===\n");
    printf("L1 Data Cache: %u KB\n", info.cache_info.l1d_size / 1024);
    printf("L1 Instruction Cache: %u KB\n", info.cache_info.l1i_size / 1024);
    printf("L2 Cache: %u KB\n", info.cache_info.l2_size / 1024);
    if (info.cache_info.l3_size > 0) {
        printf("L3 Cache: %u KB\n", info.cache_info.l3_size / 1024);
    }
    printf("Cache Line Size: %u bytes\n", info.cache_info.line_size);
    printf("Cache Associativity: %u-way\n", info.cache_info.associativity);
    
    printf("================================\n\n");
}
