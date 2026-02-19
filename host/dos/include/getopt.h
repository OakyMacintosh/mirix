#ifndef MIRIX_DOS_GETOPT_H
#define MIRIX_DOS_GETOPT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* option->has_arg flags */
#define no_argument        0
#define required_argument  1
#define optional_argument  2

struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

int getopt_long(int argc, char * const argv[], const char *optstring,
                const struct option *longopts, int *longindex);
int getopt_long_only(int argc, char * const argv[], const char *optstring,
                     const struct option *longopts, int *longindex);
int getopt(int argc, char * const argv[], const char *optstring);

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_GETOPT_H */
