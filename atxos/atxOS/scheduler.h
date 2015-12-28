/*
 * scheduler.h
 *
 * Created: 05.11.2015 14:52:41
 *  Author: Admin
 */ 


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "thread.h"

void schedYield();

void schedPutSleep(TCB* tcb);

//prio level from 0 to 16
#define SCHED_MIN_PRIO 0x0f
#define SCHED_MAX_PRIO 0x00

#endif /* SCHEDULER_H_ */