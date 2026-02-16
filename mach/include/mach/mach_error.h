#ifndef MACH_ERROR_H
#define MACH_ERROR_H

#include <stdio.h>

// Mach error string function
#ifdef __cplusplus
extern "C" {
#endif

const char *mach_error_string(kern_return_t error_code);

#ifdef __cplusplus
}
#endif

#endif /* MACH_ERROR_H */
