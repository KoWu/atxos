/*
 * time.c
 *
 * Created: 12.12.2015 17:57:00
 *  Author: Admin
 */ 
#include <util/atomic.h>
#include "internals.h"
#include "hardware.h"
#include "scheduler.h"
#include "thread.h"
#include "port/port.h"

/* 1 sec = 1024ms, reicht 18,6h */
static WORD sec = 0;
volatile TCB* sleepingThreads = 0;

ISR(ATXOS_PORT_TIMER_VECT) {
	TCB* tcb;
	sec++;

	for (tcb = (TCB*)sleepingThreads; tcb; tcb = tcb->nextWaiting) {
		tcb->info1--;
	}
	
	if (sleepingThreads && !sleepingThreads->info1) {
		ATXOS_PORT_WAKEUP_AT(sleepingThreads->info2);
		ATXOS_PORT_WAKEUP_EN();
	}
}

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
	result += (DWORD)current << 10; // sec zeit dazurechnen
	return result;
}

void Sleep(WORD s)
{
	
}

void Msleep(WORD ms)
{
	DWORD tmp = ATXOS_PORT_TIMER_CUR();
	currentThread->info1 = ms >> 10;
	currentThread->info2 = ms & 0x03ff;
	currentThread->info2 *= 62;
	currentThread->info2 += ((ms & 0x03ff) >> 1);
	
	tmp += currentThread->info2;
	currentThread->info1 += tmp / (64000 - 1);
	currentThread->info2 = tmp % (64000 - 1);
	
	schedPutSleep((TCB*)currentThread);
}
