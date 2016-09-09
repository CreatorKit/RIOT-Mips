#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>

#include <periph/timer.h>
#include "cpu_conf.h"
#include <stdio.h>

#define CHANNEL_NUMOF       (1U)
timer_isr_ctx_t timer_isr_ctx[TIMER_NUMOF];

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    if (dev >= TIMER_NUMOF) {
        return -1;
    }

    timer_isr_ctx[dev].cb = cb;
    timer_isr_ctx[dev].arg = arg;

    /* Clear any pending Timer Interrupt */
    unsigned int now = mips32_get_c0(C0_COUNT);
    mips32_set_c0(C0_COMPARE, now - 1);

    /* Start the timer if stopped */
    mips32_bc_c0(C0_CAUSE, CR_DC);

    /* Enable Timer Interrupts */
    int ipti = (mips32_get_c0(C0_INTCTL) >> INTCTL_IPTI_SHIFT) & 7;
    mips32_bs_c0(C0_STATUS, (1 << (ipti + 8)));

    return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    if (dev >= TIMER_NUMOF || channel >= CHANNEL_NUMOF) {
        return -1;
    }
    
    unsigned int now = mips32_get_c0(C0_COUNT);
    mips32_set_c0(C0_COMPARE, now + timeout);

    return 1;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev >= TIMER_NUMOF) {
        return -1;
    }
    mips32_set_c0(C0_COMPARE, value);
    
    return 1;
}

int timer_clear(tim_t dev, int channel)
{
    mips32_bs_c0(C0_CAUSE, CR_DC);
    mips32_set_c0(C0_COMPARE, 0);

    return mips32_get_c0(C0_CAUSE);
}

unsigned int timer_read(tim_t dev)
{
    if (dev >= TIMER_NUMOF) {
        return -1;
    }
    unsigned int now = mips32_get_c0(C0_COUNT);

    return now;
}

void timer_start(tim_t dev)
{
    mips32_bc_c0(C0_CAUSE, CR_DC);
}

void timer_stop(tim_t dev)
{
    mips32_bs_c0(C0_CAUSE, CR_DC);
}

void timer_irq_enable(tim_t dev)
{
    int ipti = (mips32_get_c0(C0_INTCTL) >> INTCTL_IPTI_SHIFT) & 7;
    mips32_bs_c0(C0_STATUS, 1 << (ipti + 8));
}

void timer_irq_disable(tim_t dev)
{
    int ipti = (mips32_get_c0(C0_INTCTL) >> INTCTL_IPTI_SHIFT) & 7;
    mips32_bc_c0(C0_STATUS, 1 << (ipti + 8));
}
