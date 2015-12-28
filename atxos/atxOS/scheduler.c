/*
 * scheduler.c
 *
 * Created: 05.11.2015 14:52:31
 *  Author: Admin
 */ 
#include <util/atomic.h>
#include "port/port.h"
#include "scheduler.h"
#include "dispatcher.h"
#include "thread.h"
#include "internals.h"


void schedYield()
{
	TCB* t;
	for (t = currentThread->nextThread;; t = t->nextThread) {
		if (t->status == T_STATUS_ACTIVE) {
			if (t->prioCurrent >= t->prioLevel) {
				t->prioCurrent = 0;
				break;
			}
			t->prioCurrent++;
		}
	}
	currentThread = t;
}

void schedPutSleep(TCB* tcb)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		TCB* t = (TCB*)sleepingThreads;
		if (t) {
			for(; t->nextWaiting; t = t->nextWaiting) {
				if (t->nextWaiting->info1 <= tcb->info1) {
					continue;
				}
				if (t->nextWaiting->info2 <= tcb->info2) {
					continue;
				}
				break;
			}
			tcb->nextWaiting = t->nextWaiting;
			t->nextWaiting = tcb;
		} else {
			sleepingThreads = tcb;
			if (!tcb->info1) {
				ATXOS_PORT_WAKEUP_EN();
			}
		}
		tcb->status = T_STATUS_WAITING;
		
		if (!sleepingThreads->info1) {
			ATXOS_PORT_WAKEUP_AT(sleepingThreads->info2);
		}
	}
	ContextSwitch();
}

ISR(ATXOS_PORT_WAKEUP_VECT)
{
	TCB* tmp = (TCB*)sleepingThreads;
	tmp->status = T_STATUS_ACTIVE;
	tmp->prioCurrent = tmp->prioLevel;
	sleepingThreads = tmp->nextWaiting;
	tmp->nextWaiting = 0;

	if (!sleepingThreads) {
		ATXOS_PORT_WAKEUP_DIS();
	} else if (!sleepingThreads->info1) {
		ATXOS_PORT_WAKEUP_AT(sleepingThreads->info2);
	}
}
