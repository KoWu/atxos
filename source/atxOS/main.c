/*
 * atxOS.c
 *
 * Created: 05.11.2015 13:51:43
 *  Author: rugonzal
 */ 


#include <avr/io.h>
#include "atxOS/atxOS.h"
#include "atxOS/hardware.h"
#include "console/console.h"

static BYTE teststack1[256];

void testthread1();

//DDRB = 0xff;
//PORTB |= (1<<PORTB5);
//PORTB &= ~(1<<PORTB5);
int main(void)
{
	DDRB = 0xff;
	AtxOSInit();
	register_console();
	CreateThread(testthread1, teststack1, sizeof(teststack1), T_PRIO_HIGHEST);
	AtxOSStart();
}


void testthread1() {
	for(;;) {
		Sleep(1);
	}
}
