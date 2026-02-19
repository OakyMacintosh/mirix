#ifndef MIRIX_DOS_LIBGEN_H
#define MIRIX_DOS_LIBGEN_H

#ifdef __cplusplus
extern "C" {
#endif

static inline char *basename(char *path) {
    char *last = path;
    for (char *p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') last = p + 1;
    }
    return last;
}

static inline char *dirname(char *path) {
    char *last = path;
    char *p;
    for (p = path; *p; ++p) {
        if (*p == '/' || *p == '\\') last = p;
    }
    if (last == path) return ".";
    *last = '\0';
    return path;
}

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_LIBGEN_H */
