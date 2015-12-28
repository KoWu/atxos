/*
 * atxOS_x128a1.h
 *
 * Created: 10.12.2015 20:19:34
 *  Author: Admin
 */ 


#ifndef ATXOS_X128A1_H_
#define ATXOS_X128A1_H_

#include <avr/io.h>

#define ATXOS_PORT_FREQ 16000000

#define ATXOS_PORT_RETADDRSIZE 3

#define ATXOS_PORT_RAMSTART RAMSTART
#define ATXOS_PORT_RAMSIZE RAMSIZE


#define ATXOS_PORT_SCHEDULE_VECT TCC0_OVF_vect

#define portSetupTimer()\
{\
	TCC0.CTRLA = TC_CLKSEL_DIV64_gc;\
	TCC0.CTRLB = 0;\
	TCC0.PER = 200;\
	TCC0.CNT = 0;\
	TCC0.INTCTRLA = 0b00000011;\
	PMIC.CTRL = 0b00000111;\
}

#endif /* ATXOS_X128A1_H_ */