# posix4dos: A POSIX-like layer for MS-DOS (DJGPP)

This project aims to provide a minimal POSIX-like layer for MS-DOS environments, specifically targeting the DJGPP (DJ's GNU Programming Platform) toolchain. DJGPP already provides a significant level of POSIX compatibility, so this layer primarily serves as a consistent interface or a place for future DOS-specific extensions.

## Project Structure

- `src/`: Contains the source code for the `posix4dos` library.
    - `posix4dos.h`: Header file for the `posix4dos` library.
    - `posix4dos.c`: Implementation of the wrapped POSIX functions.
- `example/`: Contains a simple example program demonstrating the use of `posix4dos`.
    - `example.c`: Source code for the example application.
- `Makefile`: Automates the build process for both the library and the example.

## Building the Project

To build `posix4dos`, you need to have the DJGPP development environment set up.

1.  **Navigate to the project root:**
    ```bash
    cd posix4dos
    ```

2.  **Build the library and example:**
    ```bash
    make all
    ```
    This command will:
    - Create `build/` and `lib/` directories.
    - Compile `posix4dos.c` into `posix4dos.o`.
    - Archive `posix4dos.o` into `lib/libposix4dos.a`.
    - Compile `example.c`, linking against `libposix4dos.a`, to create `example/example.exe`.

## Running the Example

After a successful build, you can run the example application from the `posix4dos` root directory:

```bash
example/example.exe
```

This will execute the `example.exe` program, which demonstrates file creation, writing, seeking, and reading using the `p4d_` wrapper functions. It will also create a `test_file.txt` in the `example/` directory.

## Cleaning Up

To remove all compiled files and directories:

```bash
make clean
```
