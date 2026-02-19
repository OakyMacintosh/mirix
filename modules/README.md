# Kernel Modules

The `modules/` directory defines build-time kernel modules. Unlike loadable modules, these modules are compiled directly into the kernel binary and are initialized by `initialize_kernel_modules()` during boot and torn down via `shutdown_kernel_modules()` during shutdown.

## Adding a Module

1. Create `modules/<name>.c` and implement a `kernel_module_t` with a unique name string and the desired init/shutdown callbacks.
2. Declare the module in `modules/<name>.h` using `extern const kernel_module_t kernel_module_<name>;`.
3. Include the new header from `modules/module_registry.h` so the registry can link the module into the build.
4. Register the module by adding it to the `kernel_modules` array inside `modules/registry.c` (modules are statically enumerated). Make sure to keep the entries terminated with `NULL`.

## Module Lifecycle

- `initialize_kernel_modules()` is invoked after the core kernel subsystems (`host_interface`, `ipc`, `syscall`, `posix`) finish, so modules can rely on those services.
- `shutdown_kernel_modules()` runs during kernel shutdown ahead of cleanup routines.

Modules are intentionally limited to compile-time to keep the DOS kernel simple; no runtime loader exists yet.

### DOS Personality Module

The `dos-personality` module is automatically enabled when you build the DOS kernel. During initialization it:

- Logs that the DOS personality is active.
- Ensures the kernel arguments have sensible defaults (`build/X86_64-DEBUG/filesystem/rootfs` for the root filesystem and `./dos_lazyfs.img` for the LazyFS backing file) if the command line omitted them.

These defaults keep DOS builds running with minimal configuration; add more modules for other personalities by following the standard module workflow described above.
