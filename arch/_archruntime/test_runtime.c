/*
 * Test program for MIRIX Cross-Architecture Runtime
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arch_runtime.h"

// Test functions
static void test_architecture_detection(void);
static void test_memory_management(void);
static void test_context_management(void);
static void test_atomic_operations(void);
static void test_timing_functions(void);
static void test_cpu_features(void);

int main(int argc, char *argv[]) {
    printf("=== MIRIX Cross-Architecture Runtime Test ===\n\n");
    
    // Initialize runtime with default configuration
    arch_runtime_config_t config = {
        .debug_enabled = true,
        .profiling_enabled = false,
        .tracing_enabled = false,
        .stack_size = 64 * 1024,  // 64KB stack
        .heap_size = 1024 * 1024   // 1MB heap
    };
    
    if (arch_runtime_init(&config) != 0) {
        printf("ERROR: Failed to initialize runtime\n");
        return 1;
    }
    
    // Run tests
    test_architecture_detection();
    test_memory_management();
    test_context_management();
    test_atomic_operations();
    test_timing_functions();
    test_cpu_features();
    
    printf("\n=== Runtime Test Complete ===\n");
    
    // Cleanup
    arch_runtime_cleanup();
    
    return 0;
}

static void test_architecture_detection(void) {
    printf("\n--- Architecture Detection Test ---\n");
    
    arch_info_t info;
    if (arch_runtime_get_info(&info) != 0) {
        printf("ERROR: Failed to get architecture information\n");
        return;
    }
    
    printf("Architecture: %s\n", info.arch_name);
    printf("Platform: %s\n", info.platform_name == PLATFORM_WINDOWS ? "Windows" :
                               info.platform_name == PLATFORM_MACOS ? "macOS" :
                               info.platform_name == PLATFORM_LINUX ? "Linux" :
                               info.platform_name == PLATFORM_BSD ? "BSD" :
                               info.platform_name == PLATFORM_DOS ? "DOS" :
                               info.platform_name == PLATFORM_UNIX ? "UNIX" : "Unknown");
    printf("Endianness: %s\n", info.endianness == ENDIAN_LITTLE ? "Little" :
                              info.endianness == ENDIAN_BIG ? "Big" : "Unknown");
    printf("Word Size: %u bytes\n", info.word_size);
    printf("Address Bits: %u\n", info.address_bits);
    printf("Page Size: %u bytes\n", info.page_size);
    printf("CPU Frequency: %u MHz\n", info.cpu_frequency);
    printf("Number of Cores: %u\n", info.num_cores);
    printf("Supports Multithreading: %s\n", info.supports_multithreading ? "YES" : "NO");
    printf("Supports Virtual Memory: %s\n", info.supports_virtual_memory ? "YES" : "NO");
    printf("Supports MMU: %s\n", info.supports_mmu ? "YES" : "NO");
    printf("Is 64-bit: %s\n", arch_is_64bit() ? "YES" : "NO");
    printf("Supports Virtualization: %s\n", arch_supports_virtualization() ? "YES" : "NO");
}

static void test_memory_management(void) {
    printf("\n--- Memory Management Test ---\n");
    
    // Test malloc
    void *ptr1 = arch_malloc(1024);
    void *ptr2 = arch_malloc(2048);
    void *ptr3 = arch_malloc(4096);
    
    printf("arch_malloc test:\n");
    printf("  1024 bytes: %p\n", ptr1);
    printf("  2048 bytes: %p\n", ptr2);
    printf("  4096 bytes: %p\n", ptr3);
    
    // Test calloc
    void *ptr4 = arch_calloc(10, 100);
    printf("arch_calloc test:\n");
    printf("  10 x 100 bytes: %p\n", ptr4);
    
    // Test realloc
    void *ptr5 = arch_realloc(ptr1, 2048);
    printf("arch_realloc test:\n");
    printf("  Reallocated to 2048 bytes: %p\n", ptr5);
    
    // Test free
    arch_free(ptr2);
    arch_free(ptr3);
    arch_free(ptr4);
    arch_free(ptr5);
    printf("Memory freed\n");
    
    // Test word size and page size
    printf("Word Size: %u bytes\n", arch_get_word_size());
    printf("Page Size: %u bytes\n", arch_get_page_size());
}

static void test_context_management(void) {
    printf("\n--- Context Management Test ---\n");
    
    arch_context_t context1, context2;
    
    // Test context creation
    if (arch_context_create(&context1) != 0) {
        printf("ERROR: Failed to create context 1\n");
        return;
    }
    
    if (arch_context_create(&context2) != 0) {
        printf("ERROR: Failed to create context 2\n");
        arch_context_destroy(&context1);
        return;
    }
    
    printf("Contexts created successfully\n");
    printf("Context 1 SP: 0x%016llX\n", context1.sp);
    printf("Context 2 SP: 0x%016llX\n", context2.sp);
    
    // Test context switching (if implemented)
    if (arch_context_switch(&context1, &context2) == 0) {
        printf("Context switch successful\n");
    } else {
        printf("Context switch not implemented or failed\n");
    }
    
    // Cleanup
    arch_context_destroy(&context1);
    arch_context_destroy(&context2);
    printf("Contexts destroyed\n");
}

static void test_atomic_operations(void) {
    printf("\n--- Atomic Operations Test ---\n");
    
    volatile uint32_t atomic_var = 0;
    
    // Test atomic add
    uint32_t old_val = arch_atomic_add(&atomic_var, 10);
    printf("Atomic add: %u -> %u (old: %u)\n", 0, atomic_var, old_val);
    
    // Test atomic sub
    old_val = arch_atomic_sub(&atomic_var, 5);
    printf("Atomic sub: %u -> %u (old: %u)\n", 10, atomic_var, old_val);
    
    // Test atomic exchange
    old_val = arch_atomic_exchange(&atomic_var, 42);
    printf("Atomic exchange: %u -> %u (old: %u)\n", 5, atomic_var, old_val);
    
    // Test atomic compare exchange
    bool success = arch_atomic_compare_exchange(&atomic_var, 42, 100);
    printf("Atomic compare exchange: %u -> %u (expected: 42, success: %s)\n", 
           42, atomic_var, success ? "YES" : "NO");
    
    // Test memory barriers
    arch_memory_barrier();
    arch_memory_barrier_acquire();
    arch_memory_barrier_release();
    printf("Memory barriers executed\n");
}

static void test_timing_functions(void) {
    printf("\n--- Timing Functions Test ---\n");
    
    // Test timestamp
    uint64_t timestamp1 = arch_get_timestamp();
    printf("Timestamp 1: %llu\n", timestamp1);
    
    // Test cycle count
    uint64_t cycles1 = arch_get_cycle_count();
    printf("Cycle count 1: %llu\n", cycles1);
    
    // Test delay
    printf("Testing 100ms delay...\n");
    arch_delay_microseconds(100000); // 100ms
    printf("Delay completed\n");
    
    uint64_t timestamp2 = arch_get_timestamp();
    uint64_t cycles2 = arch_get_cycle_count();
    
    printf("Timestamp 2: %llu\n", timestamp2);
    printf("Cycle count 2: %llu\n", cycles2);
    printf("Time difference: %llu ms\n", timestamp2 - timestamp1);
    printf("Cycle difference: %llu\n", cycles2 - cycles1);
}

static void test_cpu_features(void) {
    printf("\n--- CPU Features Test ---\n");
    
    arch_cpu_features_t features;
    if (arch_runtime_get_cpu_features(&features) != 0) {
        printf("ERROR: Failed to get CPU features\n");
        return;
    }
    
    printf("CPU Features:\n");
    printf("  FPU: %s\n", features.has_fpu ? "YES" : "NO");
    printf("  MMX: %s\n", features.has_mmx ? "YES" : "NO");
    printf("  SSE: %s\n", features.has_sse ? "YES" : "NO");
    printf("  SSE2: %s\n", features.has_sse2 ? "YES" : "NO");
    printf("  SSE3: %s\n", features.has_sse3 ? "YES" : "NO");
    printf("  SSE4.1: %s\n", features.has_sse4_1 ? "YES" : "NO");
    printf("  SSE4.2: %s\n", features.has_sse4_2 ? "YES" : "NO");
    printf("  AVX: %s\n", features.has_avx ? "YES" : "NO");
    printf("  AVX2: %s\n", features.has_avx2 ? "YES" : "NO");
    printf("  FMA: %s\n", features.has_fma ? "YES" : "NO");
    printf("  NEON: %s\n", features.has_neon ? "YES" : "NO");
    printf("  Thumb: %s\n", features.has_thumb ? "YES" : "NO");
    printf("  AltiVec: %s\n", features.has_altivec ? "YES" : "NO");
    printf("  Virtualization: %s\n", features.has_virtualization ? "YES" : "NO");
    printf("  64-bit: %s\n", features.has_64bit ? "YES" : "NO");
    
    // Test cache information
    arch_cache_info_t cache;
    if (arch_runtime_get_cache_info(&cache) == 0) {
        printf("\nCache Information:\n");
        printf("  L1 Data Cache: %u KB\n", cache.l1d_size / 1024);
        printf("  L1 Instruction Cache: %u KB\n", cache.l1i_size / 1024);
        printf("  L2 Cache: %u KB\n", cache.l2_size / 1024);
        if (cache.l3_size > 0) {
            printf("  L3 Cache: %u KB\n", cache.l3_size / 1024);
        }
        printf("  Cache Line Size: %u bytes\n", cache.line_size);
        printf("  Cache Associativity: %u-way\n", cache.associativity);
    }
}
