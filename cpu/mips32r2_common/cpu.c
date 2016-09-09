#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>

#include "periph/uart.h"
#include "cpu.h"
#include "kernel_init.h"
#include "arch/panic_arch.h"
#include <periph/timer.h>
#include <assert.h>

void mips_start(void);

extern void _fini(void);
extern void atexit(void (*)(void));
extern void _init(void);
extern void exit(int);

void software_init_hook(void)
{
	/*
	 * Note the mips toolchain crt expects to jump to main by RIOT wants the user code to start at main
	 * for some perverse reason, but the crt does provide this hook function which get called
	 * fairly close to the jump to main, thus if we finish off the job of the crt here and never
	 * return we can support this madness.
	 */

	atexit(_fini);
	_init();

	mips_start();

	exit(-1);
}

void cpu_init_early(void)
{
	/* Enable CP0 + CP1 */
	mips32_set_c0(C0_STATUS, SR_CU1 | SR_CU0);
}

void mips_start(void)
{
	cpu_init_early();
	board_init();

#if MODULE_NEWLIB
#error "This Port is designed to work with the (newlib) C library provided with the mips sdk toolchain"
#endif

	/* kernel_init */
	kernel_init();
}

void panic_arch(void)
{
	printf("\nPANIC!\n");
	assert(0);
	while (1);
}

