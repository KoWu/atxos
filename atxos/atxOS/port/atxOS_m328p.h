/*
 * atxOS_m328p.h
 *
 * Created: 10.12.2015 20:19:12
 *  Author: Admin
 */ 


#ifndef ATXOS_M328P_H_
#define ATXOS_M328P_H_

#include <avr/io.h>

#define ATXOS_PORT_FREQ 16000000

#define ATXOS_PORT_RETADDRSIZE 2

#define ATXOS_PORT_RAMSTART RAMSTART
#define ATXOS_PORT_RAMSIZE (RAMEND - RAMSTART)


#define ATXOS_PORT_SCHEDULE_VECT TIMER0_COMPA_vect

#define ATXOS_PORT_TIMER_VECT TIMER1_COMPA_vect
#define ATXOS_PORT_TIMER_CUR() (TCNT1)
#define ATXOS_PORT_TIMER_US_TIMER_TICKS 16

#define ATXOS_PORT_WAKEUP_VECT TIMER1_COMPB_vect
#define ATXOS_PORT_WAKEUP_EN() (TIMSK1 |= (1 << OCIE1B))
#define ATXOS_PORT_WAKEUP_DIS() (TIMSK1 &= ~(1 << OCIE1B))
#define ATXOS_PORT_WAKEUP_AT(n) (OCR1B = n)


/************************************************************************/
/* timer 0 -> scheduler                                                 */
/* timer 1 -> timer                                                     */
/************************************************************************/
#define portSetupTimer()\
{\
  TCCR0A = (1 << WGM01);\
  TCCR0B = (1 << CS01);\
  OCR0A = 125 - 1;\
  TIMSK0 = (1 << OCIE0A);\
  \
  TCCR1B = (1 << CS12) | (1 << WGM12);\
  OCR1A = 64000 - 1;\
  TIMSK1 = (1 << OCIE1A);\
}

#endif /* ATXOS_M328P_H_ */