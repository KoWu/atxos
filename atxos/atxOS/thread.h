/*
 * Thread.h
 *
 * Created: 05.11.2015 14:51:41
 *  Author: Admin
 */ 

#include "hardware.h"

#ifndef THREAD_H_
#define THREAD_H_

typedef enum ThreadStatus_e{
	T_STATUS_ACTIVE,
	T_STATUS_WAITING,
	T_STATUS_DEAD
} ThreadStatus;

typedef struct TCB_s
{
	void* TopOfStack;
	struct TCB_s* nextThread;
	struct TCB_s* nextWaiting;
	WORD info1, info2;
	ThreadStatus status;
	BYTE prioLevel : 4;
	BYTE prioCurrent : 4;
} TCB;

int CreateThread(void *start, BYTE* stack, int sizeOfStack);

#endif /* THREAD_H_ */