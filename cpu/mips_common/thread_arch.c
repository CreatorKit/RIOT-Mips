#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>
#include <stdio.h>

#include "thread.h"
#include "cpu.h"
#include "cpu_conf.h"

#define STACK_END_PAINT 0xdeadc0de

void __thread_switch(void);
void __thread_start(void);

char *thread_arch_stack_init(thread_task_func_t task_func, void *arg,
                             void *stack_start, int stack_size)
{
    /* make sure it is aligned to 8 bytes because currently
       we generate SDC1 instructions in _vprintf... */
    uintptr_t *p = (uintptr_t *)((long)(stack_start + stack_size) & ~7);

    /* paint */
    *p = STACK_END_PAINT;

    /* prepare stack for __context_restore() */
    p -= CONTEXT_SIZE_ON_STACK / sizeof(uintptr_t);

    p[4]  = (uintptr_t)arg;
    p[26] = mips32_get_c0(C0_STATUS);
    p[27] = mips32_get_c0(C0_CAUSE);
    asm volatile ("sw    $gp, 112(%0)" :: "r"(p));
    p[29] = (uintptr_t)task_func;
    p[31] = (uintptr_t)sched_task_exit;
    
    return (void *)p;
}

void thread_arch_stack_print(void)
{
    long *sp = (void *)sched_active_thread->sp;

    printf("Stack trace:\n");
    while (*sp != STACK_END_PAINT) {
        printf(" 0x%p: 0x%08lx\n", sp, *sp);
        sp++;
    }
}

void thread_arch_start_threading(void)
{
    irq_disable();
    sched_run();
    __thread_start();

    UNREACHABLE();
}

void thread_arch_yield(void)
{
    __thread_switch();
}
