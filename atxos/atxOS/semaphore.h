/*
 * semaphore.h
 *
 * Created: 16.11.2015 13:56:42
 *  Author: Admin
 */ 


#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "thread.h"

typedef struct SEMAPHORE_s
{
	volatile TCB* head;
	volatile BYTE size;
} SEMAPHORE;

void SemaphoreInit(SEMAPHORE* s, BYTE size);
void SemaphoreUp(SEMAPHORE* s);
void SemaphoreDown(SEMAPHORE* s);

#endif /* SEMAPHORE_H_ */