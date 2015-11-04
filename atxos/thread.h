/*
 * thread.h
 *
 * Created: 29.10.2015 19:28:18
 *  Author: Admin
 */ 


#ifndef THREAD_H_
#define THREAD_H_

#include "hardware.h"

#define STACKSIZE 512

typedef enum threadstatus_e {
	THREAD_ACTIVE,
	THREAD_WAITING
} threadstatus;

typedef struct THREAD_s {
	volatile void* pStack;
	threadstatus status;
	struct THREAD_s* parent;
} THREAD;


char CreateThread(void* entrypoint);

#endif /* THREAD_H_ */