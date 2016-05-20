/*
 * time.c
 * All timing related stuff comes here.
 * Created: 12.12.2015 17:57:00
 *  Author: Admin
 */ 
#include <util/atomic.h>
#include "internals.h"
#include "hardware.h"
#include "scheduler.h"
#include "dispatcher.h"
#include "thread.h"
#include "port/port.h"

/* 1 sec = 1024ms, reicht 18,6h */
static WORD sec = 0;

/* Sleeping threads array. */
volatile TCB* sleepingThreads = 0;


/************************************************************************/
/* Wakes up all sleeping threads which are behind the current time.     */
/************************************************************************/
void wakeupThreads()
{
	TCB* tmp;
	
	overrun:
	tmp = (TCB*)sleepingThreads;
	if (tmp->wakeuptime.parts > ATXOS_PORT_TIMER_CUR()) {
		PORTB |= (1<<PORTB5);
	}
	tmp->status = T_STATUS_ACTIVE;
	tmp->prioCurrent = tmp->prioLevel;
	sleepingThreads = tmp->nextWaiting;
	tmp->nextWaiting = 0;

	if (!sleepingThreads || sleepingThreads->wakeuptime.secs)
		ATXOS_PORT_WAKEUP_DIS();
	else if (!sleepingThreads->wakeuptime.secs)
	{
		if (sleepingThreads->wakeuptime.parts <= ATXOS_PORT_TIMER_CUR())
			goto overrun;
		
		ATXOS_PORT_WAKEUP_ISR_CLRFLAG();
		ATXOS_PORT_WAKEUP_AT(sleepingThreads->wakeuptime.parts);
		ATXOS_PORT_WAKEUP_EN();
		
		if (sleepingThreads->wakeuptime.parts <= ATXOS_PORT_TIMER_CUR())
			goto overrun;
	}
}

/************************************************************************/
/* ISR Wakeup routine called if sleeping threads need to be woken up.   */
/************************************************************************/
ISR(ATXOS_PORT_WAKEUP_VECT)
{
	wakeupThreads();
}

/************************************************************************/
/* ISR Timer routine called every 1024ms.                               */
/************************************************************************/
ISR(ATXOS_PORT_TIMER_VECT) {
	TCB *tcb, *tmp;
	
	sec++;
	tmp = 0;

	for (tcb = (TCB*)sleepingThreads; tcb; tcb = tcb->nextWaiting) {
		if (!tcb->wakeuptime.secs) {
			tcb->wakeuptime.parts = 0;
			continue;
		}
		tmp = tmp ? tmp : tcb;
		tcb->wakeuptime.secs--;
	}
	
	//enable first entry if necessary.
	if (tmp && !tmp->wakeuptime.secs && !ATXOS_PORT_WAKEUP_ISR_TRIGGERED()) {
		if (tmp->wakeuptime.parts <= ATXOS_PORT_TIMER_CUR())
			wakeupThreads();
		else
		{
			ATXOS_PORT_WAKEUP_ISR_CLRFLAG();
			ATXOS_PORT_WAKEUP_AT(tmp->wakeuptime.parts);
			ATXOS_PORT_WAKEUP_EN();
			
			if (tmp->wakeuptime.parts <= ATXOS_PORT_TIMER_CUR())
				wakeupThreads();
		}
	}
}

/************************************************************************/
/* Suspend and put a thread into into the sleeping threads queue.       */
/************************************************************************/
void putSleep(TCB* tcb)
{
	TCB *c, *p;
	
	tcb->status = T_STATUS_WAITING;
	
	c = (TCB*)sleepingThreads;
	p = 0;
	
	//find position in sorted linked list.
	while(c && c->wakeuptime.secs < tcb->wakeuptime.secs) {
		p = c;
		c = c->nextWaiting;
	}
	while(c && c->wakeuptime.secs <= tcb->wakeuptime.secs && c->wakeuptime.parts < tcb->wakeuptime.parts) {
		p = c;
		c = c->nextWaiting;
	}
	
	//insert
	tcb->nextWaiting = c;
	if (!p) {
		sleepingThreads = tcb;
		} else {
		p->nextWaiting = tcb;
	}
	
	//enable first entry if necessary.
	if (!sleepingThreads->wakeuptime.secs && !ATXOS_PORT_WAKEUP_ISR_TRIGGERED()) {
		ATXOS_PORT_WAKEUP_AT(sleepingThreads->wakeuptime.parts);
		ATXOS_PORT_WAKEUP_EN();
	}
}

/************************************************************************/
/* Returns the time since system start in ms.                           */
/************************************************************************/
DWORD CurrentTime()
{
	DWORD result;
	WORD current;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		result = ATXOS_PORT_TIMER_CUR();
		current = sec;
	}
	
	result *= ATXOS_PORT_TIMER_US_TIMER_TICKS; // ticks in µs umrechnen.
	result /= 1000; // nach ms
	result += (DWORD)current * 1024; // sec zeit dazurechnen
	return result;
}

/************************************************************************/
/* Sleeps s seconds.                                                    */
/************************************************************************/
void Sleep(WORD s)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		DWORD tmp, tmp2;
		
		tmp = ATXOS_PORT_TIMER_CUR();
		
		tmp2 = s * 24;
		currentThread->wakeuptime.secs = s - 1;
		currentThread->wakeuptime.secs -= tmp2 / 1024;
		currentThread->wakeuptime.parts = tmp2 % 1024;
		currentThread->wakeuptime.parts *= 1000.0f / ATXOS_PORT_TIMER_US_TIMER_TICKS;
		currentThread->wakeuptime.parts = ATXOS_PORT_TIMER_TOP - currentThread->wakeuptime.parts;
		
		tmp += currentThread->wakeuptime.parts;
		currentThread->wakeuptime.secs += tmp / ATXOS_PORT_TIMER_TOP;
		currentThread->wakeuptime.parts = tmp % ATXOS_PORT_TIMER_TOP;
		
		putSleep((TCB*)currentThread);
		
		ContextSwitch();
	}
}

/************************************************************************/
/* Sleeps ms milliseconds.                                              */
/************************************************************************/
void Msleep(WORD ms)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		DWORD tmp = ATXOS_PORT_TIMER_CUR();
		
		currentThread->wakeuptime.secs = ms / 1024;
		currentThread->wakeuptime.parts = ms % 1024;
		currentThread->wakeuptime.parts *= 1000.0f / ATXOS_PORT_TIMER_US_TIMER_TICKS;
		
		tmp += currentThread->wakeuptime.parts;
		currentThread->wakeuptime.secs += tmp / ATXOS_PORT_TIMER_TOP;
		currentThread->wakeuptime.parts = tmp % ATXOS_PORT_TIMER_TOP;
		
		putSleep((TCB*)currentThread);
		
		ContextSwitch();
	}
}

/************************************************************************/
/* Sleep us microseconds ACTIVE.                                        */
/* 4 ticks for the while(us--) loop, 1 tick for every additional Mhz.   */
/************************************************************************/
void Usleep(WORD us)
{
	//TODO: macro with ATXOS_PORT_FREQ to support frequencies other than 16Mhz
	while(us--) {
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
		asm volatile ("nop");
	}
}