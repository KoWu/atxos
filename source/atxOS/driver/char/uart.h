/*
 * uart.h
 * Buffered uart.
 * Created: 20.02.2016 11:25:52
 *  Author: Admin
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <util/atomic.h>
#include "../../hardware.h"
#include "../../port/port.h"
#include "../../thread.h"
#include "../../internals.h"
#include "../../dispatcher.h"


#define UART_BUFSIZE	16

#define UART_BAUD		9600UL


#define UBRR_VAL ((ATXOS_PORT_FREQ + UART_BAUD * 8) / (UART_BAUD * 16) - 1)
#define BAUD_REAL (ATXOS_PORT_FREQ / (16 * (UBRR_VAL + 1)))
#define BAUD_ERROR ((BAUD_REAL * 1000) / UART_BAUD)

#if ((BAUD_ERROR < 990) || (BAUD_ERROR > 1010))
#error "Baud error rate > 1%"
#endif


/************************************************************************/
/* Initializes the buffered uart.                                       */
/************************************************************************/
void uart_initialize();

/************************************************************************/
/* Get a char from the rx uart buffer. Blocking if empty.               */
/************************************************************************/
char uart_getc();

/************************************************************************/
/* Put a char into the tx uart buffer. Blocking if full.                */
/************************************************************************/
void uart_putc(char c);

#endif /* UART_H_ */