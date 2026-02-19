#ifndef MIRIX_DOS_DPMI_H
#define MIRIX_DOS_DPMI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned long offset32;
    unsigned short selector;
} __dpmi_paddr;

static inline int __dpmi_get_protected_mode_interrupt_vector(int intno, __dpmi_paddr *vec) {
    if (vec) {
        vec->offset32 = 0;
        vec->selector = 0;
    }
    (void)intno;
    return 0;
}

static inline int __dpmi_set_protected_mode_interrupt_vector(int intno, __dpmi_paddr *vec) {
    (void)intno;
    (void)vec;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* MIRIX_DOS_DPMI_H */
