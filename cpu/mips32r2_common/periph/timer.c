#include <mips/cpu.h>
#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>

#include <periph/timer.h>
#include "cpu_conf.h"
#include <stdio.h>
#include "sched.h"
#include "thread.h"
#include "board.h"

#define TIMER_ACCURACY_SHIFT 10 // setting this lower will improve accuracy at the cost of more regular interrupts (hence less power efficient).
#define TIMER_ACCURACY (1 << TIMER_ACCURACY_SHIFT)

#define CHANNELS 3

/*
 * The base MIPS count / compare timer is fixed frequency at core clock / 2 and is pretty basic
 * This timer is currently only supported in Vectored Interrupt Mode (VI), EIC mode is not supported yet.
 *
 * RIOT's xtimer expects the timer to operate at 1MHZ or any 2^n multiple or factor of this,
 * thus we maintain a software timer which counts at 1MHz. This is not particularly power efficient and may add latency too.
 *
 * If other SoC specific timers are available which are more flexible then it is advised to use them, this timer is a fallback version
 * that should work on all MIPS implementations.
 *
 */

static timer_isr_ctx_t timer_isr_ctx;
volatile unsigned int counter;
volatile unsigned int compares[CHANNELS];
static volatile int spurious_int;

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
	int i;

	if (dev != 0) {
		return -1;
	}

	(void)freq; /*Cannot adjust Frequency */

	timer_isr_ctx.cb = cb;
	timer_isr_ctx.arg = arg;

	/* Clear down soft counters */
	for(i=0;i<CHANNELS;i++)
		compares[i]=0;

	counter = 1 << TIMER_ACCURACY_SHIFT;

	/* Set compare up */
	mips_setcompare(mips_getcount() + TICKS_PER_US * TIMER_ACCURACY);

	/* Start the timer if stopped */
	mips32_bc_c0(C0_CAUSE, CR_DC);

	/* Enable Timer Interrupts */
	mips32_bs_c0(C0_STATUS, SR_HINT5);

	return 0;
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
	if (dev != 0) {
		return -1;
	}

	if (channel >= CHANNELS)
		return -1;

	timeout >>= TIMER_ACCURACY_SHIFT;
	timeout <<= TIMER_ACCURACY_SHIFT;

	asm volatile("di");
	compares[channel] = counter + timeout;
	asm volatile("ei");

	return channel;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
	if (dev != 0 ) {
		return -1;
	}

	if (channel >= CHANNELS)
		return -1;

	value >>= TIMER_ACCURACY_SHIFT;
	value <<= TIMER_ACCURACY_SHIFT;

	asm volatile("di");
	compares[channel] = value;
	asm volatile("ei");

	return channel;
}

int timer_clear(tim_t dev, int channel)
{
	if (dev != 0 ) {
		return -1;
	}

	if (channel >= CHANNELS)
		return -1;

	asm volatile("di");
	compares[channel] = 0;
	asm volatile("ei");

	return channel;
}

unsigned int timer_read(tim_t dev)
{
	if (dev != 0) {
		return -1;
	}
	return counter;
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
	mips32_bs_c0(C0_STATUS, SR_HINT5);
}

void timer_irq_disable(tim_t dev)
{
	mips32_bc_c0(C0_STATUS, SR_HINT5);
}


//note Compiler inserts GP context save + restore code (to current stack).
void __attribute__ ((interrupt("vector=hw5"))) _mips_isr_hw5(void)
{
	register int cr = mips_getcr();
	if (cr & CR_TI) {

		asm volatile("di");
		counter+=TIMER_ACCURACY;
		asm volatile("ei");

		if (counter == compares[0]) {
			/*
			 * The Xtimer code expects the ISR to take some time but our counter is a fake software one, so bump it a bit
			 * to give the impression some time elapsed in the ISR.
			 * Without this the callback ( _shoot(timer) on xtimer_core.c ) never fires.
			 */
			counter+= TIMER_ACCURACY;
			timer_isr_ctx.cb(timer_isr_ctx.arg,0);
			//asm volatile("di");
			//counter=0;
			//asm volatile("ei");
			if (sched_context_switch_request)
				thread_yield();
		}
		if (counter == compares[1]) {
			timer_isr_ctx.cb(timer_isr_ctx.arg,1);
			asm volatile("di");
			counter=0;
			asm volatile("ei");
			if (sched_context_switch_request)
				thread_yield();
		}
		if (counter == compares[2]) {
			timer_isr_ctx.cb(timer_isr_ctx.arg,2);
			asm volatile("di");
			counter=0;
			asm volatile("ei");
			if (sched_context_switch_request)
				thread_yield();
		}

		mips_setcompare(mips_getcount() + TICKS_PER_US * TIMER_ACCURACY);

	} else {
		spurious_int++;
	}
}

