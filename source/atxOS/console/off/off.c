/*
 * off.c
 *
 * Created: 20.02.2016 15:45:58
 *  Author: Admin
 */ 
#include <avr/io.h>

void off() {
	PORTB &= ~(1<<PORTB5);
}