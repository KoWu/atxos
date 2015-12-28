/*
 * mutex.h
 *
 * Created: 16.11.2015 13:57:21
 *  Author: Admin
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_

#include "thread.h"

typedef struct MUTEX_s
{
	volatile TCB* head;
} MUTEX;

void MutexInit(MUTEX* m);
void MutexLock(MUTEX* m);
void MutexUnlock(MUTEX* m);

#endif /* MUTEX_H_ */