/*
 * semaphore.c
 *
 * Created: 16.11.2015 13:56:54
 *  Author: Admin
 */ 
#include "semaphore.h"
#include "dispatcher.h"
#include "internals.h"
#include <util/atomic.h>


void SemaphoreInit(SEMAPHORE* s, BYTE size)
{
	s->head = 0;
	s->size = size;	
}

void SemaphoreDown(SEMAPHORE* s)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		if (s->size) {
			s->size--;
			return;
		}
		if (!s->head) {
			s->head = currentThread;
		} else {
			TCB* tcb;
			for (tcb = (TCB*) s->head; tcb->nextWaiting; tcb = tcb->nextWaiting);
			tcb->nextWaiting = (TCB*) currentThread;
		}
		currentThread->status = T_STATUS_WAITING;
	}
	ContextSwitch();
}

void SemaphoreUp(SEMAPHORE* s)
{
	TCB* tcb;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		s->size++;
		tcb = (TCB*) s->head;
		if (tcb) {
			s->head = tcb->nextWaiting;
			tcb->nextWaiting = 0;
			tcb->status = T_STATUS_ACTIVE;
		}
	}
}
