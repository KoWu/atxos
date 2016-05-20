/*
 * mutex.c
 * Mutex implementation.
 * Created: 16.11.2015 13:57:10
 *  Author: Admin
 */ 
#include "mutex.h"
#include "dispatcher.h"
#include "internals.h"
#include <util/atomic.h>

/************************************************************************/
/* Initializes a mutex dynamically.                                     */
/************************************************************************/
void MutexInit(MUTEX* m)
{
	m->head = 0;
}

/************************************************************************/
/* Locks the mutex. Sleeps until the mutex is released.                 */
/************************************************************************/
void MutexLock(MUTEX* m)
{
	TCB* tcb;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		//multiple locks have no effect.
		if (currentThread == m->head) {
			return;
		}
		
		if (!m->head) {
			m->head = currentThread;
			return;
		}
	
		for (tcb = (TCB*) m->head; tcb->nextWaiting; tcb = tcb->nextWaiting);
		tcb->nextWaiting = (TCB*) currentThread;
		currentThread->status = T_STATUS_WAITING;
	}
	ContextSwitch();
}

/************************************************************************/
/* Unlocks the mutex. Next waiting thread is waked up.                  */
/************************************************************************/
void MutexUnlock(MUTEX* m)
{
	TCB* tcb;
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		tcb = (TCB*) m->head;
		if (tcb && tcb->nextWaiting) {
			m->head = tcb->nextWaiting;
			tcb->nextWaiting->status = T_STATUS_ACTIVE;
			tcb->nextWaiting = 0;
		} else {
			m->head = 0;
		}
	}
}
