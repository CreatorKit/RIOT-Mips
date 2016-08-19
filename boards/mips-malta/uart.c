#include <mips/m32c0.h>
#include "periph/uart.h"

#define MIPS_MALTA_UART 0xb80003f8

void uart_out_num(const uint8_t *data, size_t num)
{
    int i;
    for (i = 0; i < num; i++) {
        *(volatile char *)MIPS_MALTA_UART = *data++;
    }
}

void uart_out(const char *str)
{
    while (*str) {
        *(volatile char *)MIPS_MALTA_UART = *str++;
    }
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* FIXME: based on QEMU Malta pseudo-bootloader. Clean it up */
    asm volatile (
    "lui $t1, 0xb400 \n"
    "ori $t0, $0, 0x00df \n"
    "sw $t0, 0x0068($t1) \n"
    "lui $t1, 0xbbe0 \n"
    "ori $t0, $0, 0x00c0 \n"
    "sw $t0, 0x0048($t1) \n"
    "ori $t0, $0, 0x0040 \n"
    "sw $t0, 0x0050($t1) \n"
    "ori $t0, $0, 0x0080 \n"
    "sw $t0, 0x0058($t1) \n"
    "ori $t0, $0, 0x003f \n"
    "sw $t0, 0x0060($t1) \n"
    "ori $t0, $0, 0x00c1 \n"
    "sw $t0, 0x0080($t1) \n"
    "ori $t0, $0, 0x005e \n"
    "sw $t0, 0x0088($t1) \n");

    /* i8259 on MALTA board on a CPU without GIC is wired to HWINT0 */
    /* mips32_bs_c0(C0_STATUS, SR_HINT0); */

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    uart_out_num(data, len);
}
