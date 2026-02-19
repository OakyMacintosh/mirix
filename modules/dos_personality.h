#ifndef MIRIX_DOS_PERSONALITY_H
#define MIRIX_DOS_PERSONALITY_H

#include "modules/module.h"
#include "mirix/kernel_args.h"

extern const kernel_module_t kernel_module_dos_personality;

void dos_personality_apply_defaults(mirix_kernel_args_t *args);

#endif // MIRIX_DOS_PERSONALITY_H
