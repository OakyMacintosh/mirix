#ifdef TARGET_DEFS_ONLY

#define CONFIG_TCC_ASM
#define NB_ASM_REGS 16

ST_FUNC void g(int c);
ST_FUNC void gen_le16(int c);
ST_FUNC void gen_le32(int c);

#else

#define USING_GLOBALS
#include "tcc.h"

enum {
    REG_R0, REG_R1, REG_R2, REG_R3,
    REG_R4, REG_R5, REG_R6, REG_R7,
    REG_R8, REG_R9, REG_R10, REG_R11,
    REG_R12, REG_SP, REG_LR, REG_PC,
    OPT_REG32, OPT_REG64, OPT_REGS, OPT_FREGS,
    OPT_IM8, OPT_IM16, OPT_IM32, OPT_IM64,
};
#define OP_REG32    (1 << OPT_REG32)
#define OP_REG64    (1 << OPT_REG64)
#define OP_REGS     (OP S)
#define OP_FREGS    (1 << OPT_FREGS)
#define OP_IM8      (1 << OPT_IM8)
#define OP_IM16     (1 << OPT_IM16)
#define OP_IM32     (1 << OPT_IM32)
#define OP_IM64     (1 << OPT_IM64)

#endif