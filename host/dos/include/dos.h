#ifndef MIRIX_DOS_DOS_H
#define MIRIX_DOS_DOS_H

#ifdef __cplusplus
extern "C" {
#endif

static inline void outportb(unsigned short port, unsigned char value) {
    (void)port;
    (void)value;
}

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_DOS_H */
