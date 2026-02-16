#include "mirix_libc_freestanding.h"

int main(void) {
    mirix_printf("Hello from MIRIX cross-compiler!\n");
    mirix_printf("Target: %s\n", "mirix1");
    mirix_printf("Architecture: %s\n", "x86_64");
    mirix_printf("System: %s\n", "SYSV");
    return 0;
}
