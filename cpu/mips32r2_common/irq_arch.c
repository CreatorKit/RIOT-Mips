#include <mips/m32c0.h>
#include "arch/irq_arch.h"


/*

NO IRQ stack Yet.

#define ISR_STACK_SIZE 4096
__attribute__ ((aligned (8))) int __isr_stack[ISR_STACK_SIZE/sizeof(int)];
*/

unsigned int irq_arch_enable(void)
{
	unsigned int status;
	asm volatile ("ei %0" : "=r"(status));
	return status;
}

unsigned int irq_arch_disable(void)
{
	unsigned int status;
	asm volatile ("di %0" : "=r"(status));
	return status;
}

void irq_arch_restore(unsigned int state)
{
	if (state & SR_IE) {
		mips32_bs_c0(C0_STATUS, SR_IE);
	} else {
		mips32_bc_c0(C0_STATUS, SR_IE);
	}
}

int irq_arch_in(void)
{
	return (mips32_get_c0(C0_STATUS) & SR_EXL) != 0;
}
