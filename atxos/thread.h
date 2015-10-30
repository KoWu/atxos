/*
 * thread.h
 *
 * Created: 29.10.2015 19:28:18
 *  Author: Admin
 */ 


#ifndef THREAD_H_
#define THREAD_H_

#define STACKSIZE 512

char CreateThread(void* entrypoint);

typedef struct THREAD_struct {
	volatile void* pStack;
	struct THREAD_struct* parent;
} THREAD;

#endif /* THREAD_H_ */