/*
 * semaphore.c
 * Semaphore implementation.
 * Created: 16.11.2015 13:56:54
 *  Author: Admin
 */ 
#include "semaphore.h"
#include "dispatcher.h"
#include "internals.h"
#include <util/atomic.h>

/************************************************************************/
/* Initializes a Semaphore dynamically.                                 */
/************************************************************************/
void SemaphoreInit(SEMAPHORE* s, BYTE size)
{
	s->head = 0;
	s->size = size;	
}

/************************************************************************/
/* Performs an up on the semaphore. Wakes up the next waiting thread.   */
/************************************************************************/
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

/************************************************************************/
/* Performs a down on the semaphore. If full, the thread will wait.     */
/************************************************************************/
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
