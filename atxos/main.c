/*
 * atxOS.c
 *
 * Created: 05.11.2015 13:51:43
 *  Author: rugonzal
 */ 


#include <avr/io.h>
#include "atxOS/atxOS.h"
#include "atxOS/hardware.h"
#include "atxOS/time.h"

BYTE teststack1[128];
BYTE teststack2[128];

void testthread1();
void testthread2();


int main(void)
{	
	atxOSInit();
	CreateThread(testthread1, teststack1, sizeof(teststack1));
	CreateThread(testthread2, teststack2, sizeof(teststack2));
	atxOSStart();
}

void testthread1() {
	DDRC = 0xff;
	for (;;) {
		PORTC |= (1<<PORTC1);
		Msleep(200);
		PORTC &= ~(1<<PORTC1);
		Msleep(200);
	}
}

void testthread2() {
	DDRC = 0xff;
	for (;;) {
		PORTC |= (1<<PORTC2);
		Msleep(1000);
		PORTC &= ~(1<<PORTC2);
		Msleep(1000);
	}
}
