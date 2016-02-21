/*
 * semaphore.h
 * Semaphore definitions.
 * Created: 16.11.2015 13:56:42
 *  Author: Admin
 */ 


#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "thread.h"

/************************************************************************/
/* Like MUTEX but with the possibility of multiple accessors.           */
/************************************************************************/
typedef struct SEMAPHORE_s
{
	volatile TCB* head;
	volatile BYTE size;
} SEMAPHORE;

/************************************************************************/
/* Initializes a Semaphore statically.                                  */
/************************************************************************/
#define DEFINE_SEMAPHORE(semname, semasize) SEMAPHORE semname = {.head = 0, .size = semasize}

/************************************************************************/
/* Initializes a Semaphore dynamically.                                 */
/************************************************************************/
void SemaphoreInit(SEMAPHORE* s, BYTE size);

/************************************************************************/
/* Performs an up on the semaphore. If full, the thread will wait.      */
/************************************************************************/
void SemaphoreUp(SEMAPHORE* s);

/************************************************************************/
/* Performs a down on the semaphore. Wakes up the next waiting thread.  */
/************************************************************************/
void SemaphoreDown(SEMAPHORE* s);

#endif /* SEMAPHORE_H_ */