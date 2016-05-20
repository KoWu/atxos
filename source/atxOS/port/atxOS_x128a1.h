/*
 * atxOS_x128a1.h
 *
 * Created: 10.12.2015 20:19:34
 *  Author: Admin
 */ 


#ifndef ATXOS_X128A1_H_
#define ATXOS_X128A1_H_

#include <avr/io.h>

/* General settings. */
#define ATXOS_PORT_FREQ 16000000
#define ATXOS_PORT_RETADDRSIZE 3
#define ATXOS_PORT_RAMSTART RAMSTART
#define ATXOS_PORT_RAMSIZE (RAMEND - RAMSTART)

/* Scheduler settings. */
#define ATXOS_PORT_SCHEDULE_VECT TCC1_OVF_vect
#define ATXOS_PORT_SCHEDULE_TOP (250 - 1)

/* Timer settings. */
#define ATXOS_PORT_TIMER_VECT TCC0_OVF_vect
#define ATXOS_PORT_TIMER_CUR() (TCC0_CNT)
#define ATXOS_PORT_TIMER_US_TIMER_TICKS 16
#define ATXOS_PORT_TIMER_TOP (64000 - 1)

#define ATXOS_PORT_WAKEUP_VECT TCC0_CCA_vect
#define ATXOS_PORT_WAKEUP_EN() (TCC0.INTCTRLA |= (0x01))
#define ATXOS_PORT_WAKEUP_DIS() (TCC0.INTCTRLA &= ~(0x01))
#define ATXOS_PORT_WAKEUP_AT(n) (TCC0.CCA = n)
#define ATXOS_PORT_WAKEUP_ISR_TRIGGERED() (TCC0.INTFLAGS & (1 << 4))
#define ATXOS_PORT_WAKEUP_ISR_CLRFLAG() (TCC0.INTFLAGS |= (1 << 4))

/************************************************************************/
/* timer 1 -> scheduler                                                 */
/* timer 0 -> timer                                                     */
/************************************************************************/

#define portSetupTimer()\
{\
	TCC1.CTRLA = 0x05;\
	TCC1.CTRLB = 0;\
	TCC1.INTCTRLA = 0x03;\
	TCC1.INTCTRLB = 0;\
	TCC1.PER = ATXOS_PORT_SCHEDULE_TOP;\
	\
	TCC0.CTRLA = 0;\
	TCC0.CTRLB = 0;\
	TCC0.INTCTRLA = 0x03;\
	TCC0.INTCTRLB = 0x01;\
	TCC0.PER = ATXOS_PORT_TIMER_TOP;\
	\
	PMIC.CTRL = 0x07;\
}

#endif /* ATXOS_X128A1_H_ */