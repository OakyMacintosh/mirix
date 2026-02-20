# Tiny C Compiler (TCC)

Tiny C Compiler is a small, fast and portable ANSI C compiler originally
written by Fabrice Bellard.  This repository snapshot keeps the upstream
source intact inside `extras/software/src/tinycc` so it can be rebuilt or
cross-compiled as part of the Mirix/Aqua toolchain.

## Features

- **Minimal footprint.** TCC compiles itself and can execute C scripts
directly with `tcc -run`.
- **Faster turnaround.** Codegen for i386, x86_64, arm, aarch64 and
  riscv64 completes roughly 10× faster than `gcc -O0`.
- **Runtime flexibility.** You can mix standard C libraries with the
  optional `tcclib` helpers, run scripts with shebang lines, and even
  embed memory/bounds checking.
- **Universal portability.** The compiler is self-contained and has been
  used on Linux, BSD, macOS, Windows, and now within the Mirix/Aqua
  lazykernel toolchain.

## Building inside Mirix

Mirix consumes TCC both as a native compiler and to produce cross-
compiled binaries for the `any-none-aqua-lzyabi` ABI.  To build just the
compiler from the Mirix tree:

```bash
cd extras/software/src/tinycc
./configure
make
```

Normal options such as `make test`, `make doc`, `make clean`, etc.
behave like upstream TCC.  Mirix-specific customizations live in the
parent tree (e.g., `targets.conf`, `tools/mirix-gcc`, and the `build`
descriptions) rather than inside the vanilla TCC sources.

### Mirix/Aqua cross-target

We add an `x86_64-mirix` target to the TCC `Makefile` so the compiler
itself can be cross-built for Mirix/Aqua.  Its makefile definitions point
at the sysroot that `tools/build-mirix-toolchain` creates, so `x86_64-mirix-tcc`
pulls `crt1.o`, `libc.a` (which is just `libmirixc.a`), the Mirix header
tree, and the Aqua macros automatically.  The cross-compiler itself
defines `__AQUA__`, `__LZYABI__`, `__MIRIX__`, `__SYSV__`,
`LZYABI_TARGET "any-none-aqua-lzyabi"`, and `TARGETOS_MIRIX` to stay
consistent with the rest of the toolchain.

Run `tools/build-mirix-toolchain` before you build the Mirix cross-target
or run `./x86_64-mirix-tcc`; the script populates
`tools/build/mirix-cross/sysroot/x86_64-unknown-sysv-mirix1` with `crt1.o`,
`libc.a`, and `mirix_libc.h`.  The `MIRIX_TOOLCHAIN`/`MIRIX_SYSROOT`
variables inside the tinycc Makefile point to that directory so the compiler
“just works” without extra flags.  Once the sysroot exists, you can use the
compiler directly (or via `tools/mirix-gcc`/`multicc`) to emit `.aout`
binaries that `tools/run-lazy-kernel` can run.

## Documentation & examples

- Examples ship with the upstream sources (`ex1.c` through `ex5.c`,
  `tcctest.c`, etc.).  See `tcc-doc.html` or `tcc-doc.texi` for full
  reference material.
- Windows-specific instructions live in `tcc-win32.txt`.
- Use `man tcc` if the manual page has been generated via `make tcc.1`.

## License

TCC is distributed under the GNU Lesser General Public License.  Tiny
C Compiler and the upstream sources here remain the copyright of
**Fabrice Bellard**.
