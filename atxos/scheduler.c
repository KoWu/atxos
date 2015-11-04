/*
 * scheduler.c
 *
 * Created: 29.10.2015 20:10:38
 *  Author: Admin
 */
#include "scheduler.h"
#include "hardware.h"
#include "thread.h"

volatile void* pContext = 0;
THREAD threads[MAX_THREADS];

void schedNext() {
	static BYTE i = 0;
	do {
		threads[i++].pStack = pContext;
		if (i >= MAX_THREADS || !threads[i].pStack) {
			i = 0;
		}
	} while (threads[i].status != THREAD_ACTIVE);
	pContext = threads[i].pStack;
}