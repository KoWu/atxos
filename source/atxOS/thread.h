/*
 * Thread.h
 *
 * Created: 05.11.2015 14:51:41
 *  Author: Admin
 */ 

#include "hardware.h"
#include "time.h"

#ifndef THREAD_H_
#define THREAD_H_

/************************************************************************/
/* Active:  Thread is running.                                          */
/* Waiting: Thread is waiting for an event and is skipped by scheduler. */
/* Dead:    Thread was killed or exited otherwise. Unused.              */
/************************************************************************/
typedef enum ThreadStatus_e{
	T_STATUS_ACTIVE,
	T_STATUS_WAITING,
	T_STATUS_DEAD
} ThreadStatus;

/************************************************************************/
/* Thread priorities. Low values have the higher priority.              */
/************************************************************************/
typedef enum ThreadPriority_e {
	T_PRIO_HIGHEST = 0,
	T_PRIO_1,
	T_PRIO_2,
	T_PRIO_3,
	T_PRIO_4,
	T_PRIO_5,
	T_PRIO_6,
	T_PRIO_7,
	T_PRIO_8,
	T_PRIO_9,
	T_PRIO_10,
	T_PRIO_11,
	T_PRIO_12,
	T_PRIO_13,
	T_PRIO_14,
	T_PRIO_LOWEST
} ThreadPriority;

/************************************************************************/
/* Thread control block. Starts at the bottom of each threads stack.    */
/************************************************************************/
typedef struct TCB_s
{
	void* TopOfStack;
	struct TCB_s* nextThread;
	struct TCB_s* nextWaiting;
	ThreadStatus status;
	union {
		TIMEVAL wakeuptime;
	};
	BYTE prioLevel : 4;
	BYTE prioCurrent : 4;
} TCB;

/************************************************************************/
/* Creates a new thread.                                                */
/************************************************************************/
ATXVALUE CreateThread(void *start, BYTE* stack, int sizeOfStack, ThreadPriority priority);

#endif /* THREAD_H_ */