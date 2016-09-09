#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <inttypes.h>

#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void cpu_print_last_instruction(void)
{
}

void board_init(void);

#ifdef __cplusplus
}
#endif

#endif
