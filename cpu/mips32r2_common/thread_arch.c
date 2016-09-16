#include <mips/cpu.h>
#include <mips/hal.h>
#include <stdio.h>

#include "thread.h"
#include "cpu.h"
#include "cpu_conf.h"

#define STACK_END_PAINT 0xdeadc0de
#define C0_STATUS_EXL 2

/*
 *    Stack Layout, note struct gpctx is defined in
 *    $MIPS_ELF_ROOT/mips-mti-elf/include/mips/hal.h
 *
 *    Top Of Stack
 *     ---------------
 *    |               |
 *    |  User Code    |
 *    |               |
 *     ---------------  <--- gpctx->sp
 *    |               |
 *    |    gpctx      |
 *    |               |
 *     ---------------
 *    |  16 byte pad  |
 *     ---------------   <--- sched_active_thread->sp
 */

char *thread_arch_stack_init(thread_task_func_t task_func, void *arg,
		void *stack_start, int stack_size)
{
	/* make sure it is aligned to 8 bytes this is a requirement of the O32 ABI */
	uintptr_t *p = (uintptr_t *)((long)(stack_start + stack_size) & ~7);
	uintptr_t *fp;

	/* paint */
	*p = STACK_END_PAINT;

	/* prepare stack for __exception_restore() */
	fp = p;
	p -= sizeof(struct gpctx) / sizeof(unsigned int);

	struct gpctx* initial_ctx = (struct gpctx*)p;
	initial_ctx->a[0] = (reg_t)arg;
	initial_ctx->status = mips32_get_c0(C0_STATUS) | SR_IE; /* Enable interrupts */
	asm volatile ("sw    $gp, 0(%0)" :: "r"(&initial_ctx->gp));
	initial_ctx->epc = (reg_t)task_func;
	initial_ctx->ra = (reg_t)sched_task_exit;
	initial_ctx->sp = (reg_t)fp;

	/*
	 * note the -4 (-16 bytes) as the toolchain exception handling code adjusts
	 * the sp for alignment
	 */
	p -= 4;

	return (void *)p;
}

void thread_arch_stack_print(void)
{
	uintptr_t *sp = (void *)sched_active_thread->sp;

	printf("Stack trace:\n");
	while(*sp != STACK_END_PAINT) {
		printf(" 0x%p: 0x%08lx\n", sp, *sp);
		sp++;
	}
}

extern void __exception_restore(void);
void thread_arch_start_threading(void)
{
	unsigned int status = mips32_get_c0(C0_STATUS);

	/*
	 * Set Exception level if we are not already running at it
	 * the ERL mode depends on the bootloader.
	 */

	if((status & C0_STATUS_EXL) == 0)
		mips32_set_c0(C0_STATUS, status | C0_STATUS_EXL);

	sched_run();

	asm volatile ("lw    $sp, 0(%0)" :: "r"(&sched_active_thread->sp));

	__exception_restore();

	UNREACHABLE();
}

void thread_arch_yield(void)
{
	/*
	 * throw a syscall exception to get into exception level
	 * we context switch at exception level.
	 *
	 * Note syscall 1 is reserved for UHI see:
	 * http://wiki.prplfoundation.org/w/images/4/42/UHI_Reference_Manual.pdf
	 */
	asm volatile("syscall 2");
}

/*
 * This attribute should not really be needed, it works around a toolchain
 * issue in 2016.05-03.
 */
void __attribute__((nomips16))
_mips_handle_exception(struct gpctx *ctx, int exception)
{
	unsigned int fault_instruction = 0, return_instruction = 0;
	struct gpctx *new_ctx;

	switch(exception) {

	case EXC_SYS:
		fault_instruction = *((unsigned int *)(ctx->epc));
		if(((fault_instruction >> 6) & 0xFFFFF) == 2) {
			/* Syscall 1 is reserved for UHI.
			 *
			 * save the stack pointer in the thread info
			 * note we want the saved value to include the
			 * saved off context and the 16 bytes padding.
			 * Note we cannot use the current sp value as
			 * the prologue of this function has adjusted it
			 */
			sched_active_thread->sp = (char*)(ctx->sp
					- sizeof(struct gpctx) - 16);

			sched_run();

			new_ctx = (struct gpctx*)((int)sched_active_thread->sp + 16);

			return_instruction = *((unsigned int *)(new_ctx->epc));

			if((return_instruction & 0xfc00003f) == 0xC) /* syscall */
				new_ctx->epc += 4; /*move PC past the syscall */

			/*
			 * The toolchain Exception restore code just wholesale copies the
			 * status register from the context back to the register loosing
			 * any changes that may have occured, 'status' is really global state
			 * You dont enable interrupts on one thread and not another...
			 * So we just copy the current status value into the saved value
			 * so nothing changes on the restore
			 */

			new_ctx->status = mips32_get_c0(C0_STATUS);

			asm volatile ("lw    $sp, 0(%0)" :: "r"(&sched_active_thread->sp));

			/*
			 * Jump straight to the exception restore code
			 * if we return this functions prologue messes up
			 * the stack  pointer
			 */
			__exception_restore();

			UNREACHABLE();
		}
		break;

		/*default:*/
	}
	/* Pass all other exceptions through to the toolchain handler */
	__exception_handle(ctx, exception);
}

