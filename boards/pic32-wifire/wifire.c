#include <stdio.h>

#include "periph/uart.h"

void board_init(void)
{
	/* Pin ctrl regs */
	volatile uint32_t * const _U4RXR = (uint32_t *) 0xBF801480;
	volatile uint32_t * const _RPF8R = (uint32_t *) 0xBF801660;
	volatile uint32_t * const _PORTFCLR = (uint32_t *) 0xBF860524;
	volatile uint32_t * const _TRISFCLR = (uint32_t *) 0xBF860514;
	volatile uint32_t * const _TRISFSET = (uint32_t *) 0xBF860518;
	volatile uint32_t * const _ODCFCLR = (uint32_t *) 0xBF860544;

	/*
	 * Setup pin mux for UART4 this is the one connected
	 * to the ftdi chip (usb<->uart)
	 */
	*_U4RXR = 0xb;
	*_RPF8R = 2;
	*_PORTFCLR = 0xa;
	*_TRISFCLR = 2;
	*_TRISFSET = 8;
	*_ODCFCLR = 0xa;

	/* intialise UART used for debug (printf) */
	uart_init(DEBUG_VIA_UART,9600,NULL,0);

	/* Route and Enable Timer interrupt */
	/*volatile uint32_t * const _IFS0 = (uint32_t *) 0xbf810040;*/
	volatile uint32_t * const _IEC0 = (uint32_t *) 0xbf8100c0;
	volatile uint32_t * const _IPC0 = (uint32_t *) 0xbf810140;

	/* Enable IRQ 0 CPU timer interrupt */
	*_IEC0 = 0x1;

	/* Set IRQ 0 to priority 1.0 */
	*_IPC0 = 0x4;
}

