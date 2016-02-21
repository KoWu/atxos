/*
 * on.c
 *
 * Created: 20.02.2016 15:45:00
 *  Author: Admin
 */ 
#include <avr/io.h>

void on() {
	PORTB |= (1<<PORTB5);
}