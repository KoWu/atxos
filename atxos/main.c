/*
 * atxos.c
 *
 * Created: 08.10.2015 19:17:31
 * Author : Admin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "dispatch.h"
#include "thread.h"

void func();
void func1();

void init()
{
	//setup timer 0 to trigger every pi s
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;
	TCC0.CTRLB = 0;
	TCC0.PER = 497;
	TCC0.CNT = 0;
	TCC0.INTCTRLA = 0b00000011;
	
	//enable high,medium and low interrupts
	PMIC.CTRL = 0b00000111;	
}
 
int main(void)
{
	cli();
	init();
	sei();
	
	asm volatile (
		"push %A0\n\t"
		"push %B0\n\t"
		"push %A1\n\t"
		:: "e" (func), "r" ((char)0)
	);
}

ISR (TCC0_OVF_vect, ISR_NAKED)
{
	asm volatile ("rjmp ContextSwitch");
}

static volatile int funcc;

//init function
void func()
{
	funcc = 1000;
	CreateThread(func1);
	for(;;) {
		funcc--;
	}
}

//test function 1
void func1()
{
	for (;;) {
		funcc++;
	}
}