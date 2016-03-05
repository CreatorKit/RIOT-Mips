#include <stdio.h>

#include "periph/uart.h"
#include "cpu.h"

#define MIPS_MALTA_ADDR 0xbf000500
#define MIPS_MALTA_VAL_RST 0x42

static void malta_reset(void)
{
    *(volatile long *)MIPS_MALTA_ADDR = MIPS_MALTA_VAL_RST;
    asm volatile ("wait");
}

void board_init(void)
{
    /* TODO */
}

void reboot(void)
{
    malta_reset();
}
