#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <inttypes.h>

#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This file must exist else RIOT won't compile */

static inline void cpu_init_early(void)
{
}

static inline void cpu_print_last_instruction(void)
{
}

void board_init(void);

#ifdef __cplusplus
}
#endif

#endif