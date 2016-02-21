/*
 * atxOS.h
 *
 * Created: 05.11.2015 14:07:20
 *  Author: Admin
 */ 

#include "thread.h"

#ifndef INTERNALS_H_
#define INTERNALS_H_

//always holds a reference to the current thread running.
extern volatile TCB* currentThread;

//list of sleeping threads. unused
//extern volatile TCB* sleepingThreads;

#endif /* INTERNALS_H_ */