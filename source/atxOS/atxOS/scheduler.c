/*
 * scheduler.c
 *
 * Created: 05.11.2015 14:52:31
 *  Author: Admin
 */ 
#include <util/atomic.h>
#include "port/port.h"
#include "scheduler.h"
#include "dispatcher.h"
#include "thread.h"
#include "internals.h"


void schedYield()
{
	TCB* t;
	for (t = currentThread->nextThread;; t = t->nextThread) {
		if (t->status == T_STATUS_ACTIVE) {
			if (t->prioCurrent >= t->prioLevel) {
				t->prioCurrent = 0;
				break;
			}
			t->prioCurrent++;
		}
	}
	currentThread = t;
}

