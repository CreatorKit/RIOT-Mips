#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>


#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif


/* We run from flash on PIC32 */
#define FLASH_XIP 1

extern void dummy(void);
static inline void cpu_init_early(void)
{
	/* Stop the linker from throwing away the PIC32 config register settings */
	dummy();
}


/* This file must exist else RIOT won't compile */
static inline void cpu_print_last_instruction(void)
{

}

void board_init(void);

#ifdef __cplusplus
}
#endif

#endif
