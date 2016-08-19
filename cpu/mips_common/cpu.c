#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>

#include "periph/uart.h"
#include "cpu.h"
#include "kernel_init.h"
#include "arch/panic_arch.h"
#include <periph/timer.h>

void cpu_init_early(void)
{
    /* TODO: copy code from ROM to RAM */

    /* Enable CP1, leave the bootstrap mode and enable interrupts */
    mips32_set_c0(C0_STATUS, SR_CU1 | SR_CU0 | SR_IE);
}

void mips_start(void)
{
    cpu_init_early();
    board_init();

#if MODULE_NEWLIB
    /* initialize std-c library (this must be done after board_init) */
    extern void __libc_init_array(void);
    __libc_init_array();
#endif

    /* kernel_init */
    kernel_init();
}

void panic_arch(void)
{
    printf("\nPANIC!\n");
    while(1);
}

extern timer_isr_ctx_t timer_isr_ctx[];

int generic_interrupt_handler(int cause)
{
    int enabled = mips32_get_c0(C0_STATUS) & SR_IMASK;
    int pending = cause & CR_IMASK & enabled;
    int ipti = (mips32_get_c0(C0_INTCTL) >> INTCTL_IPTI_SHIFT) & 7;

    if (pending & (1 << (ipti + 8))) {
        mips32_bs_c0(C0_CAUSE, CR_DC);
        mips32_set_c0(C0_COMPARE, 0);
        timer_isr_ctx[0].cb(timer_isr_ctx[0].arg, 0);      
    } else {
        printf("UNHANDLED INTERRUPT 0x%x!\n", pending & enabled);
        panic_arch();
    }

    return 0;
}

int generic_exception_handler(void)
{
    int cause = mips32_get_c0(C0_CAUSE);
    int excp = (cause & CR_XMASK) >> CR_X_SHIFT;
    switch (excp) {
    case EXC_INTR:
        generic_interrupt_handler(cause);
        break;
    default:
        printf("UNHANDLED EXCEPTION!\n");
        panic_arch();
        break;
    }

    return 0;
}
