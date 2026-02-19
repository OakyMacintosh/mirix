# MultiCC

`multicc` is a tiny C++ utility that automates the "host compile + MIRIX link" workflow for the `any-none-aqua-lzyabi` ABI.
It compiles each source file with a host compiler (default `clang`) and then uses `clang -target <triple>` to link everything against `build/mirix/libc/libmirixc.a`.

## Building it
```bash
cd extras/software/mirix/multicc
clang++ -std=c++17 -O2 multicc.cpp -o multicc
```

## Using it
```bash
./multicc -o build/bin/bash src/bash/main.c
```

By default the `mirix` target emits the macros `__AQUA__` and `__LZYABI__` and links with `-lmirixc` in `build/mirix/libc`.

### Options
- `-t <name>` / `--target=<name>` choose a target from `targets.conf`
- `--targets-file <path>` load additional target definitions
- `--list-targets` show the available targets
- `-c` compile-only
- `-o <file>` set the final executable name
- `--` everything after this flag is forwarded to both the compile and link commands (e.g. `-Wl,--no-whole-archive`).

### Extending
Add new `[target ...]` sections to `targets.conf` (or supply your own file with `--targets-file`). Each section can override:
- `triple` (the `-target` argument)
- `compiler` / `linker`
- `compile_flags` (pipe-separated)
- `link_flags`
- `libs`
- `sysroot`
- `description`

Other developers can drop new `target` sections into the same file or point multicc at their own configuration file to support additional cross toolchains.
