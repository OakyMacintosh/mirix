#!/usr/bin/awk -f
# Minimal DOS rootfs generator for aqua_kernel.
BEGIN {
    root = (ENVIRON["MIRIX_ROOTFS"] ? ENVIRON["MIRIX_ROOTFS"] : "build/X86_64-DEBUG/filesystem/rootfs")
    dirs[1] = "bin"
    dirs[2] = "etc"
    dirs[3] = "lib"
    dirs[4] = "tmp"

    for (i = 1; i <= 4; i++) {
        system(sprintf("mkdir -p '%s/%s'", root, dirs[i]))
    }

    write_file(root "/bin/shell.cmd", "@echo off\necho MIRIX DOS shell placeholder\n")
    write_file(root "/etc/version.txt", "MIRIX DOS ROOTFS for aqua_kernel\n")
    write_file(root "/lib/doslib.txt", "placeholder DOS library metadata\n")
    write_file(root "/tmp/README.txt", "Use this folder for temporary files\n")

    printf("DOS rootfs ready at %s\n", root) > "/dev/stderr"
}

function write_file(filename, contents) {
    printf "%s", contents > filename
    close(filename)c
}
