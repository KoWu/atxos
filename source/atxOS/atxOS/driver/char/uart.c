/*
 * uart.c
 * Buffered uart implementation.
 * Created: 20.02.2016 11:25:42
 *  Author: Admin
 */ 
#include "uart.h"

volatile TCB *rxwaiting, *txwaiting;
char rxbuf[UART_BUFSIZE], txbuf[UART_BUFSIZE];
char *rxr, *rxw, *txr, *txw;


/************************************************************************/
/* Called whenever a byte got received.                                 */
/************************************************************************/
ISR(USART_RX_vect) {
	*rxw++ = UDR0;
	
	if (rxw == rxbuf + UART_BUFSIZE) {
		rxw = rxbuf;
	}
	
	if (rxw == rxr) {
		UCSR0B &= ~(1 << RXCIE0);
	}
	
	if (rxwaiting) {
		rxwaiting->status = T_STATUS_ACTIVE;
		rxwaiting = 0;
	}
}

/************************************************************************/
/* Called whenever it is possible to send a byte.                       */
/************************************************************************/
ISR(USART_UDRE_vect) {
	UDR0 = *txr++;
	
	if (txr == txbuf + UART_BUFSIZE) {
		txr = txbuf;
	}
	
	if (txw == txr) {
		UCSR0B &= ~(1 << UDRIE0);
	}
	
	if (txwaiting) {
		txwaiting->status = T_STATUS_ACTIVE;
		txwaiting = 0;
	}
}

/************************************************************************/
/* Initializes the buffered uart.                                       */
/************************************************************************/
void uart_initialize() {
	rxr = rxw = rxbuf;
	txr = txw = txbuf;
	rxwaiting = txwaiting = 0;
	UBRR0 = UBRR_VAL;
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ00);
}

/************************************************************************/
/* Get a char from the rx uart buffer. Blocking if empty.               */
/************************************************************************/
char uart_getc() {
	char r;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		if (rxw == rxr && UCSR0B & (1 << RXCIE0)) {
			rxwaiting = currentThread;
			currentThread->status = T_STATUS_WAITING;
			ContextSwitch(); cli(); //TODO: soft content switch?
		}
		

		r = *rxr++;
		if (rxr == rxbuf + UART_BUFSIZE) {
			rxr = rxbuf;
		}
		
		UCSR0B |= (1 << RXCIE0);
	}
	
	return r;
}

/************************************************************************/
/* Put a char into the tx uart buffer. Blocking if full.                */
/************************************************************************/
void uart_putc(char c) {
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		*txw++ = c;
		
		if (txw == txbuf + UART_BUFSIZE) {
			txw = txbuf;
		}
		
		if (txw == txr) {
			txwaiting = currentThread;
			currentThread->status = T_STATUS_WAITING;
			ContextSwitch();
		}
		
		UCSR0B |= (1 << UDRIE0);
	}
}
