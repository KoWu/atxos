/*
 * mutex.h
 * Mutex definitions.
 * Created: 16.11.2015 13:57:21
 *  Author: Admin
 */ 


#ifndef MUTEX_H_
#define MUTEX_H_

#include "thread.h"

/************************************************************************/
/* Mutex structure. Head is a list of threads waiting for the mutex.    */
/************************************************************************/
typedef struct MUTEX_s
{
	volatile TCB* head;
} MUTEX;

/************************************************************************/
/* Initializes a Mutex statically.                                      */
/************************************************************************/
#define DEFINE_MUTEX(mutexname) MUTEX mutexname = {.head = 0}

/************************************************************************/
/* Initializes a mutex dynamically.                                     */
/************************************************************************/
void MutexInit(MUTEX* m);

/************************************************************************/
/* Locks the mutex. Sleeps until the mutex is released.                 */
/************************************************************************/
void MutexLock(MUTEX* m);

/************************************************************************/
/* Unlocks the mutex. Next waiting thread is waked up.                  */
/************************************************************************/
void MutexUnlock(MUTEX* m);

#endif /* MUTEX_H_ */