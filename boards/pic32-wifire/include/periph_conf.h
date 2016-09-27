#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

/* Note this file must exist for xtimer code to build */
#define TIMER_NUMOF         (1U)
#define TIMER_0_CHANNELS    3

/* This value must exist even if no uart in use */
#define UART_NUMOF          (6U)

/* Route debug output to UART 4 (the USB one via FTDI chip) */
#define DEBUG_VIA_UART 4

#endif