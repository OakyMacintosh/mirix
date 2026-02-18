# MIRIX Cross-Architecture Runtime System

The MIRIX Cross-Architecture Runtime System provides a unified abstraction layer for running MIRIX across multiple CPU architectures and platforms. This system enables MIRIX to be compiled and run on x86, x86_64, ARM, ARM64, PowerPC, Motorola 68K, and MIPS architectures.

## Features

### Architecture Support
- **x86_64** - 64-bit Intel/AMD processors
- **i386** - 32-bit Intel/AMD processors  
- **ARM** - 32-bit ARM processors (ARMv7 and later)
- **ARM64** - 64-bit ARM processors (ARMv8 and later)
- **PowerPC** - 32-bit and 64-bit PowerPC processors
- **Motorola 68K** - Classic 68K processors
- **MIPS** - 32-bit and 64-bit MIPS processors

### Platform Support
- **Windows** - Microsoft Windows
- **macOS** - Apple macOS
- **Linux** - Linux distributions
- **BSD** - FreeBSD, NetBSD, OpenBSD
- **DOS** - MS-DOS and compatible systems
- **UNIX** - Generic UNIX systems
- **Embedded** - Embedded systems

### Core Features

#### Architecture Detection
- Automatic detection of CPU architecture
- CPU feature detection (SSE, AVX, NEON, AltiVec, etc.)
- Cache information detection
- Endianness detection
- Word size and address space detection

#### Memory Management
- Cross-architecture memory allocation
- Unified malloc/free interface
- Memory mapping and protection
- Page size abstraction
- Heap management

#### Context Management
- Architecture-agnostic context switching
- Thread context save/restore
- Stack management
- Register preservation

#### Atomic Operations
- Atomic add/subtract
- Atomic exchange
- Compare-and-swap
- Memory barriers (acquire/release/full)

#### Timing and Performance
- High-resolution timestamps
- Cycle counter access
- Microsecond delays
- Performance monitoring

#### Interrupt Handling
- Unified interrupt interface
- Architecture-specific interrupt dispatch
- Interrupt registration/unregistration
- Priority-based interrupt handling

#### System Call Interface
- Cross-architecture syscall dispatch
- Syscall registration system
- Argument marshaling
- Error handling

## Architecture

### Directory Structure
```
arch/_archruntime/
├── arch_runtime.h          # Main runtime header
├── arch_runtime.c          # Core runtime implementation
├── arch_detect.c           # Architecture detection
├── runtime.mk              # Build system
├── test_runtime.c          # Test program
└── README.md               # This file
```

### Key Components

#### Runtime State
The runtime maintains a global state structure containing:
- Current architecture information
- CPU features and cache info
- Runtime configuration
- Interrupt handlers
- System call handlers
- Current execution context

#### Architecture Information
```c
typedef struct {
    arch_type_t arch_type;
    platform_type_t platform_type;
    endian_type_t endianness;
    const char *arch_name;
    const char *platform_name;
    uint32_t word_size;
    uint32_t page_size;
    uint32_t address_bits;
    arch_cpu_features_t cpu_features;
    arch_cache_info_t cache_info;
    uint32_t cpu_frequency;
    uint32_t num_cores;
    bool supports_multithreading;
    bool supports_virtual_memory;
    bool supports_mmu;
} arch_info_t;
```

#### Context Structure
```c
typedef struct {
    uint64_t regs[32];        // General purpose registers
    uint64_t sp;              // Stack pointer
    uint64_t pc;              // Program counter
    uint64_t fp;              // Frame pointer
    uint64_t flags;           // Processor flags
    uint64_t cpsr;            // Current program status register (ARM)
    uint64_t fp_regs[32];     // Floating point registers
    uint64_t fp_status;       // Floating point status
    uint64_t fp_control;      // Floating point control
    uint64_t debug_regs[8];   // Debug registers
    uint64_t breakpoint;      // Breakpoint register
    void *arch_specific;      // Architecture-specific data
} arch_context_t;
```

## Usage

### Initialization
```c
#include "arch/_archruntime/arch_runtime.h"

int main() {
    // Configure runtime
    arch_runtime_config_t config = {
        .debug_enabled = true,
        .profiling_enabled = false,
        .tracing_enabled = false,
        .stack_size = 1024 * 1024,  // 1MB stack
        .heap_size = 16 * 1024 * 1024 // 16MB heap
    };
    
    // Initialize runtime
    if (arch_runtime_init(&config) != 0) {
        printf("Failed to initialize runtime\n");
        return 1;
    }
    
    // Use runtime features...
    
    // Cleanup
    arch_runtime_cleanup();
    return 0;
}
```

### Architecture Detection
```c
arch_info_t info;
if (arch_runtime_get_info(&info) != 0) {
    printf("Failed to get architecture info\n");
    return -1;
}

printf("Architecture: %s\n", info.arch_name);
printf("Platform: %s\n", info.platform_name);
printf("CPU Frequency: %u MHz\n", info.cpu_frequency);
printf("Number of Cores: %u\n", info.num_cores);
```

### Memory Management
```c
// Allocate memory
void *ptr = arch_malloc(1024);
if (!ptr) {
    printf("Allocation failed\n");
    return -1;
}

// Reallocate memory
ptr = arch_realloc(ptr, 2048);

// Free memory
arch_free(ptr);
```

### Context Management
```c
arch_context_t context1, context2;

// Create contexts
arch_context_create(&context1);
arch_context_create(&context2);

// Switch contexts
arch_context_switch(&context1, &context2);

// Cleanup
arch_context_destroy(&context1);
arch_context_destroy(&context2);
```

### Atomic Operations
```c
volatile uint32_t counter = 0;

// Atomic add
uint32_t old = arch_atomic_add(&counter, 1);

// Atomic exchange
old = arch_atomic_exchange(&counter, 100);

// Compare and swap
bool success = arch_atomic_compare_exchange(&counter, 100, 200);
```

## Building

### Prerequisites
- GCC or Clang compiler
- Make utility
- Standard C library

### Build Commands
```bash
# Build for current architecture
make

# Build for specific architecture
make ARCH=x86_64
make ARCH=arm64
make ARCH=ppc

# Build with debug
make DEBUG=1

# Build with profiling
make PROFILING=1

# Build with tracing
make TRACING=1

# Build runtime library
make libarchruntime.a

# Clean build
make clean-runtime
```

### Architecture-Specific Builds
```bash
# x86_64
make ARCH=x86_64 PLATFORM=linux

# ARM64
make ARCH=arm64 PLATFORM=linux

# PowerPC
make ARCH=ppc PLATFORM=linux

# MIPS
make ARCH=mips PLATFORM=linux

# DOS (i386)
make ARCH=i386 PLATFORM=dos
```

## Testing

### Run Test Suite
```bash
# Build and run test program
make test_runtime
./test_runtime
```

### Test Output
The test program demonstrates:
- Architecture detection
- Memory management
- Context management
- Atomic operations
- Timing functions
- CPU feature detection

## Integration

### Integrating with MIRIX Kernel
1. Include the runtime header in kernel files
2. Initialize runtime early in kernel startup
3. Use runtime abstractions instead of architecture-specific code
4. Register architecture-specific handlers as needed

### Example Integration
```c
// In kernel.c
#include "arch/_archruntime/arch_runtime.h"

int kernel_main() {
    // Initialize runtime
    arch_runtime_config_t config = {
        .debug_enabled = true,
        .stack_size = 1024 * 1024,
        .heap_size = 16 * 1024 * 1024
    };
    
    arch_runtime_init(&config);
    
    // Get architecture info
    arch_info_t info;
    arch_runtime_get_info(&info);
    
    printf("MIRIX running on %s\n", info.arch_name);
    
    // Use runtime features...
    
    return 0;
}
```

## Performance Considerations

### Optimization Tips
1. Use architecture-specific optimizations when available
2. Enable CPU feature detection for SIMD instructions
3. Use atomic operations for thread-safe code
4. Profile with `PROFILING=1` to identify bottlenecks
5. Use tracing with `TRACING=1` for debugging

### Memory Usage
- Runtime overhead: ~1KB for state structures
- Context overhead: ~2KB per context
- Stack allocation: Configurable (default 1MB)
- Heap allocation: Configurable (default 16MB)

## Porting to New Architectures

### Adding Support for New Architecture
1. Create architecture-specific header in `arch/<arch>/arch.h`
2. Implement architecture detection in `arch_detect.c`
3. Add architecture-specific assembly code for context switching
4. Update Makefile with new architecture flags
5. Test with the test suite

### Required Functions
- `arch_context_save()` - Save CPU context
- `arch_context_restore()` - Restore CPU context
- `arch_get_cycle_count()` - Get CPU cycle counter
- CPU feature detection code
- Cache information detection

## Troubleshooting

### Common Issues
1. **Build Failures**: Check architecture flags and compiler support
2. **Runtime Failures**: Verify architecture detection and initialization
3. **Context Switching**: Ensure assembly code is correct for target architecture
4. **Memory Issues**: Check stack and heap sizes in configuration

### Debug Mode
Enable debug mode for detailed logging:
```bash
make DEBUG=1
```

### Tracing
Enable tracing for function call tracking:
```bash
make TRACING=1
```

## Contributing

### Adding Features
1. Implement feature in `arch_runtime.c`
2. Add corresponding tests in `test_runtime.c`
3. Update documentation in `README.md`
4. Test on all supported architectures

### Reporting Issues
Please report issues with:
- Architecture type
- Platform type
- Compiler version
- Error messages
- Steps to reproduce

## License

This runtime system is part of the MIRIX project and follows the same license terms.

## Credits

The MIRIX Cross-Architecture Runtime System was developed to enable MIRIX to run across multiple CPU architectures and platforms, providing a unified abstraction layer for kernel and userspace code.
